/**************************************************************************//**
  \file platform_drv.c
 
  \brief Includes implementation for SAMB11 Platform Driver
 
  Copyright (c) 2016, Atmel Corporation. All rights reserved.
  Released under NDA
  Licensed under Atmel's Limited License Agreement.
 
 
  THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
  EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
 
  Atmel Corporation: http://www.atmel.com
 
******************************************************************************/


#include <stdint.h>
#include <string.h>

#include "platform.h"
#include "gpio.h"

#include "ke_msg.h"

#include "common.h"
#include "event_handler.h"

#include "samb11.h"
#include "gpio_from_sdk.h"

#include "reg_common.h"

void init_clock(void);

uint8_t (*platform_register_isr)(uint8_t isr_index,void *fp);
uint8_t (*platform_unregister_isr)(uint8_t isr_index);
uint32_t  *apps_resume_cb;
uint32_t 	*actualfreq;
uint32_t 	*wakeup_event_pending;
uint32_t	*wakeup_source_active_cb;
void (*volatile updateuartbr_fp)(void);
void (*rwip_prevent_sleep_set)(uint16_t prv_slp_bit);
void (*rwip_prevent_sleep_clear)(uint16_t prv_slp_bit);
//#ifdef CHIPVERSION_B0
void (*handle_ext_wakeup_isr)(void);
/** callback function type for handling wakeup source active callback */
typedef void (*wakeup_source_active_callback) (uint32_t);
extern void wakeup_active_event_callback(uint32_t wakeup_source);
extern uint8_t wakeup_int_unregister_callback(uint8_t wakeup_source);

//#endif	//CHIPVERSION_B0
resume_callback samb11_app_resume_cb;
#ifdef CHIPVERSION_A4
uint8_t register_isr(uint8_t isr_index,void *fp);
uint8_t unregister_isr(uint8_t isr_index);
#endif	//CHIPVERSION_A4

//#define TASK_INTERNAL_APP  62
#define MAX_BLE_EVT_LEN						512
#define MAX_PLF_EVT_LEN						128

#define BLE_EVENT_BUFFER_START_INDEX		0
#define PLF_EVENT_BUFFER_START_INDEX		(BLE_EVENT_BUFFER_START_INDEX + MAX_BLE_EVT_LEN)

#define MAX_EVT_BUFF_LEN 	(MAX_BLE_EVT_LEN + MAX_PLF_EVT_LEN) 
#define REG_PL_WR(addr, value)       (*(volatile uint32_t *)(addr)) = (value)
#define REG_PL_RD(addr)              (*(volatile uint32_t *)(addr))

/* pwr APIs */
static void (*pwr_enable_arm_wakeup)(uint32_t wakeup_domain);
static void (*pwr_disable_arm_wakeup)(uint32_t wakeup_domain);
static int (*pwr_arm_wakeup_req)(void);
static int (*pwr_wait_BLE_out_of_reset)(uint32_t threshold);

static void* gstrFwSem;
static void* InternalAppMsgQHandle;
static void (*ke_msg_send)(void const *param_ptr);
static void* (*ke_msg_alloc)(ke_msg_id_t const id, ke_task_id_t const dest_id,
	ke_task_id_t const src_id, uint16_t const param_len);
static int (*os_sem_up)(void* pstrSem);
static int (*os_sem_down)(void* pstrSem);
static int (*NMI_MsgQueueRecv)(void* pHandle,void ** pvRecvBuffer);
static void (*ke_free)(void* mem_ptr);
static ke_task_id_t (* gapm_get_task_from_id)(ke_msg_id_t id);
static ke_task_id_t (* gapm_get_id_from_task)(ke_msg_id_t id);
volatile uint8_t platform_initialized = 0;

static platform_interface_callback ble_stack_message_handler;
uint8_t rx_buffer[MAX_EVT_BUFF_LEN];
uint16_t plf_event_buff_index;
#define ISR_RAM_MAP_START_ADDRESS	(0x10000000)
#define VECTOR_TABLE_LAST_INDEX		47

port port_list[LPGPIO_MAX];
uint32_t default_samb11_clock_init[2];
void samb11_plf_resume_callback(void);

void init_port_list(void)
{
	uint8_t i;
	memset(port_list,0,sizeof(port_list));
	for(i=0;i<(sizeof(port_list)/sizeof(port_list[0]));i++) {
		port_list[i].bit.gpio_num = i;
		port_list[i].bit.available = 1;
		port_list[i].bit.configured = 0;
	}
	//Set the GPIO for SWD is not available
	port_list[0].bit.available = 0;
	port_list[1].bit.available = 0;
	//GPIO 14 is used for Coex and controlled by Firmware
	//port_list[14].bit.available = 0;
}

void init_clock(void)
{
	uint32_t regval;
	regval = REG_PL_RD(LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0);
	//disable SPI-FLASH core clock
	//regval &= ~(LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_SPIFLASH_CLK_EN);
	//disable SPI core clocks
	regval &= ~(LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_CORTUS_SPI0_CORE_CLK_EN);
	regval &= ~(LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_CORTUS_SPI1_CORE_CLK_EN);
	//disable I2C0 Core clocks
	//regval &= ~(LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_CORTUS_I2C0_CORE_CLK_EN);		//do not disable I2C because I2C driver in ASF will not enable.
	//disable ARM dual timer core clocks.
	regval &= ~(LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_DUALTIMER_CLK_EN);
	//disable counter0 clock 
	//regval &= ~(LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_COUNTER_0_CLK_EN);
	//disable Watchdog core clocks.
	//regval &= ~(LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_WATCHDOG_0_CLK_EN);
	regval &= ~(LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_WATCHDOG_1_CLK_EN);
	//disable UART core & interface clocks
	regval &= ~(LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_UART_CORE_CLK_EN);
	regval &= ~(LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_UART_IF_CLK_EN);
	regval &= ~(LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_UART_1_CORE_CLK_EN);
	regval &= ~(LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_UART_1_IF_CLK_EN);
	//disable I2C core 1 clocks
	//regval &= ~(LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0_CORTUS_I2C1_CORE_CLK_EN);		//do not disable I2C because I2C driver in ASF will not enable.
	REG_PL_WR(LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0,regval);
	default_samb11_clock_init[0] = regval;
	
	regval = REG_PL_RD(LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1);
	//disable efuse clocks
	regval &= ~(LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1_EFUSE_1_CLK_EN);
	regval &= ~(LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1_EFUSE_2_CLK_EN);
	regval &= ~(LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1_EFUSE_3_CLK_EN);
	regval &= ~(LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1_EFUSE_4_CLK_EN);
	regval &= ~(LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1_EFUSE_5_CLK_EN);
	regval &= ~(LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1_EFUSE_6_CLK_EN);
	//disable SPI SCK Phase int clock 
	regval &= ~(LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1_SPI0_SCK_PHASE_INT_CLK_EN);
	regval &= ~(LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1_SPI1_SCK_PHASE_INT_CLK_EN);
	REG_PL_WR(LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1,regval);
	default_samb11_clock_init[1] = regval;
	//ARM debugger
	
	//store default values
}

plf_drv_status platform_driver_init()
{
	plf_drv_status status = STATUS_NOT_INITIALIZED;
	if((platform_initialized == 0) || (platform_initialized != 1)) {
		init_port_list();
		init_clock();
		// Initialize the ble stack message handler to NULL
		ble_stack_message_handler = NULL;

#ifdef CHIPVERSION_B0
		NVIC_DisableIRQ(GPIO0_IRQn);
		NVIC_DisableIRQ(GPIO1_IRQn);
		/* NVIC_DisableIRQ(PORT0_COMB_IRQn); */
		/* NVIC_DisableIRQ(PORT1_COMB_IRQn); */
		platform_register_isr = (uint8_t (*)(uint8_t ,void *))0x000007d7;
		platform_unregister_isr = (uint8_t (*)(uint8_t ))0x000007bd;
		handle_ext_wakeup_isr = (void (*)(void))0x1bc51;
		gapm_get_task_from_id = (ke_task_id_t (*)(ke_msg_id_t))(*((unsigned int *)0x100400bc));
		gapm_get_id_from_task = (ke_task_id_t (*)(ke_msg_id_t))(*((unsigned int *)0x100400b8));
		rwip_prevent_sleep_set = (void (*)(uint16_t))0x0001b99f;
		rwip_prevent_sleep_clear = (void (*)(uint16_t))0x0001b9db;
		apps_resume_cb = (uint32_t *)0x1004003c;
		actualfreq = (uint32_t *)0x10041FC0; /* set to NULL for now as clock calibration is disabled for the time being */  /*(uint32_t *)0x10006bd8;*/
		updateuartbr_fp = (void (*)())0x10041FC4;
		wakeup_source_active_cb = (uint32_t *)0x10041FD4;
		wakeup_event_pending = (uint32_t *)0x10041FD8;
		/* power APIs */
		pwr_enable_arm_wakeup = (void (*)(uint32_t wakeup_domain))0x0001cbe9;
		pwr_disable_arm_wakeup = (void (*)(uint32_t wakeup_domain))0x0001cd8f;
		pwr_arm_wakeup_req = (int (*)(void))0x0001cea3;
		pwr_wait_BLE_out_of_reset = (int (*)(uint32_t threshold))0x0001cbcf;
#else
		NVIC_DisableIRQ(PORT0_ALL_IRQn);
		NVIC_DisableIRQ(PORT1_ALL_IRQn);
		platform_register_isr = register_isr;
		platform_unregister_isr = unregister_isr;
		handle_ext_wakeup_isr = (void (*)(void))0x14085;
#endif
		platform_unregister_isr(GPIO1_COMBINED_VECTOR_TABLE_INDEX);
		platform_register_isr(GPIO1_COMBINED_VECTOR_TABLE_INDEX,(void*)PORT1_COMB_Handler);
		platform_register_isr(GPIO0_COMBINED_VECTOR_TABLE_INDEX,(void*)gpio0_combined_isr_handler);
		
		// Initializing the FW messaging functions.
#ifdef CHIPVERSION_A3
		ke_msg_send 	= (void (*)(void const *))0x00015cd9;
		ke_msg_alloc 	= (void* (*)(ke_msg_id_t const id, ke_task_id_t const dest_id,
										ke_task_id_t const src_id, uint16_t const param_len) ) 0x00015ca9;
		os_sem_up 		= (int (*)(void*)) 0x00017aed;
		gstrFwSem 		= (void*) 0x10000dec;
		NMI_MsgQueueRecv = (int(*)(void*, void ** )) 0x00017c7b;
		InternalAppMsgQHandle = (void*) 0x10002bd8;
		ke_free = (void(*)(void*)) 0x00015bc9;
//#elif CHIPVERSION_A4
		//ke_msg_send     = (void (*)(void const *)) 0x00015f4d;
		//ke_msg_alloc    = (void * (*)(ke_msg_id_t const id, ke_task_id_t const dest_id,
				//ke_task_id_t const src_id, uint16_t const param_len)) 0x00015f1d;
		//os_sem_up               = (int (*)(void *)) 0x00017dd9;
		//gstrFwSem               = (void *)0x100004e4;
		//NMI_MsgQueueRecv = (int (*)(void *, void ** )) 0x00017f67;
		//InternalAppMsgQHandle = (void *)0x10001158;
		//ke_free = (void (*)(void *)) 0x00015e3d;
#elif CHIPVERSION_B0
		ke_msg_send 	= (void (*)(void const *))(*((unsigned int *)0x100400e4));
		ke_msg_alloc 	= (void* (*)(ke_msg_id_t const id, ke_task_id_t const dest_id,
										ke_task_id_t const src_id, uint16_t const param_len) )0x00019fe9;
		os_sem_up 		= (int (*)(void*))0x0001dbdd;
		os_sem_down		= (int (*)(void*))0x0001dc5b;
		gstrFwSem 		= (void*)0x100405ec;
		NMI_MsgQueueRecv = (int(*)(void*, void ** ))0x0001d5e3;
		InternalAppMsgQHandle = (void*)0x10040c20;
		ke_free = (void(*)(void*))0x00019f09;
#endif
		memset(rx_buffer,0,sizeof(rx_buffer));
		plf_event_buff_index = PLF_EVENT_BUFFER_START_INDEX;
		platform_event_init();
		
#ifdef CHIPVERSION_B0
		/* NVIC_EnableIRQ(PORT0_COMB_IRQn); */
		/* NVIC_EnableIRQ(PORT1_COMB_IRQn); */
		NVIC_EnableIRQ(GPIO0_IRQn);
		NVIC_EnableIRQ(GPIO1_IRQn);
#else
		/* chris.choi : check keil driver's CMSDK_CM0.h and asf's samb11g18a.h (it's different so i don't know what should be used. */
		/* it's already asked to sanghai china team. */
		/* NVIC_EnableIRQ(PORT0_ALL_IRQn); */
		/* NVIC_EnableIRQ(PORT1_ALL_IRQn); */
		NVIC_EnableIRQ(7);
		NVIC_EnableIRQ(8);
#endif	//CHIPVERSION_B0		
		
#ifndef CHIPVERSION_B0		
		// spi_flash clock fix.
		spi_flash_clock_init();
#endif
		samb11_app_resume_cb = NULL;
		*apps_resume_cb = (uint32_t)((resume_callback)samb11_plf_resume_callback);
		*wakeup_source_active_cb = (uint32_t)((wakeup_source_active_callback)wakeup_active_event_callback);
		wakeup_int_unregister_callback(0);
		wakeup_int_unregister_callback(1);
		wakeup_int_unregister_callback(2);
		platform_initialized = 1;
		status = STATUS_SUCCESS;
	}
	else {
		platform_initialized = 1;
		status = STATUS_ALREADY_INITIALIZED;
	}
	return status;
}

#ifdef CHIPVERSION_A4
uint8_t unregister_isr(uint8_t isr_index)
{
	uint8_t result = INTC_OK;
	if((isr_index < UART1_RX_VECTOR_TABLE_INDEX) || (VECTOR_TABLE_LAST_INDEX > 47))
		return INTC_INVALID_IRQ;
	else 
	{
		REG_PL_WR(isr_index*4 + ISR_RAM_MAP_START_ADDRESS, 0);
	}
	return result;
}

uint8_t register_isr(uint8_t isr_index,void *fp)
{
	uint8_t result = INTC_OK;
	if((isr_index < UART1_RX_VECTOR_TABLE_INDEX) || (VECTOR_TABLE_LAST_INDEX > 47))
		result = INTC_INVALID_IRQ;
	else if(fp == NULL)
		result = INTC_INVALID_ISR;
	else if(REG_PL_RD(ISR_RAM_MAP_START_ADDRESS + (isr_index*4)) != NULL)
		result = INTC_ISR_ALREADY_REGISTERED;
	else 
	{
		REG_PL_WR(isr_index*4 + ISR_RAM_MAP_START_ADDRESS, (uint32_t) fp);
	}
	return result;
}
#endif	//CHIPVERSION_A4


plf_drv_status platform_register_ble_msg_handler(platform_interface_callback fp)
{
	plf_drv_status status;
	if(platform_initialized == 1) {
		ble_stack_message_handler = fp;
		status = STATUS_SUCCESS;
	}
	else {
		status = STATUS_NOT_INITIALIZED;
	}
	return status;
}

static void at_ke_msg_send(void const * param_ptr)
{
	struct ke_msg *kemsg;
	uint8_t osc_en = REG_PL_RD(0x4000B1EC)&0x01;
	ke_msg_send(param_ptr);
	kemsg = ke_param2msg(param_ptr);

	if((kemsg != NULL) && (kemsg->dest_id != TASK_INTERNAL_APP)) {
		if(osc_en == 0)
		{
			/* BLE Core is off, issue a wakeup request*/
			/* First, make sure PD4 is powered up and out of reset */
			pwr_enable_arm_wakeup(1<<1);
			pwr_arm_wakeup_req();
			while (pwr_wait_BLE_out_of_reset(3));
			pwr_disable_arm_wakeup(1<<1);
			REG_PL_WR(0x4000B020, 1);
#ifndef CHIPVERSION_B0
			while(REG_PL_RD(0x4000B020));
#endif	//CHIPVERSION_B0
		}
		else
		{
			/*  */
			os_sem_up(gstrFwSem);
		}
	}
}


//Sends a message through RW kernel messaging API
//struct ke_msg * p_msg;
void platform_interface_send(uint8_t* data, uint32_t len)
{
	struct ke_msghdr *p_msg_hdr = (struct ke_msghdr *)((void *)data);
	void* params;
	
	#if (CHIPVERSION_B0)
	ke_task_id_t dest_id;
	dest_id = p_msg_hdr->dest_id;
	if(gapm_get_task_from_id != NULL)
		p_msg_hdr->dest_id = gapm_get_task_from_id(dest_id);
	#endif	//CHIPVERSION_B0
	// Allocate the kernel message
	params = ke_msg_alloc(p_msg_hdr->id, p_msg_hdr->dest_id, p_msg_hdr->src_id, p_msg_hdr->param_len);
											
	//no params
	if (p_msg_hdr->param_len == 0)
	{
		// Send message directly
		at_ke_msg_send(params);
	}
	else
	{
		//copy params
		memcpy(params,&(data[KE_MSG_HDR_LENGTH]),p_msg_hdr->param_len);
		// Send the kernel message
		at_ke_msg_send(params);
	}
}

void send_plf_int_msg_ind(uint8_t intr_index, uint8_t callback_id, void *data, uint16_t data_len)
{
	void* params;
//#if (CHIPVERSION_A4)	
	//os_sem_up(gstrFwSem);
//#endif
	// Allocate the kernel message
	params = ke_msg_alloc(PERIPHERAL_INTERRUPT_EVENT, TASK_INTERNAL_APP, BUILD_INTR_SRCID(callback_id,intr_index), data_len);
											
	//no params
	if(params != NULL) {
		if((data_len != 0) && (NULL != data))
		{
			//copy params
			memcpy(params, data, data_len);
		}
		// Send the kernel message
		at_ke_msg_send(params);
	}
}

///Debug KE Timer Command parameters
struct dbg_ke_timer
{
    ///Timer period, resolution of 10 ms     
    uint32_t	period;
    ///Status of Timer, 1-> Start, 0-> Stop
    uint8_t		status;
};
#if 0
static int NMI_MsgQueueNoMsgs(NMI_MsgQueueHandle* pHandle)
{	
	int MsgQueueNoMsgs = 0;
	os_sem_down(&pHandle->strCriticalSection);
	while(pHandle->pstrMessageList != NULL)
	{
		Message * pstrMessge = pHandle->pstrMessageList->pstrNext;
		pHandle->pstrMessageList = pstrMessge;	
		MsgQueueNoMsgs++;
	}
	os_sem_up(&pHandle->strCriticalSection);
	return MsgQueueNoMsgs;
}
#endif 
static int NMI_MsgQueueDestroyOnKeID(NMI_MsgQueueHandle* pHandle, ke_msg_id_t u16KeMsgId)
{
	int num_of_freed=0;
	Message * current , *prev = NULL;
	if((NULL == pHandle) && (NULL == pHandle->pstrMessageList))
	{
		return num_of_freed;
	}

	current = pHandle->pstrMessageList;
	os_sem_down(&pHandle->strCriticalSection);
	while(NULL != current)
	{
		if(u16KeMsgId == ((struct ke_msg*)current->pvBuffer)->id)
		{
			num_of_freed++;
			if(NULL != prev)
			{
				prev->pstrNext = current->pstrNext;
				ke_free(current);
				current = prev->pstrNext;
			}
			else
			{
				pHandle->pstrMessageList = current->pstrNext;
				ke_free(current);
				current = pHandle->pstrMessageList;
			}
		}
		else
		{
			prev = current;
			current = current ->pstrNext;
		}
	}
	os_sem_up(&pHandle->strCriticalSection);
	return num_of_freed;
}

static void platform_start_event_timeout(uint32_t timeout)
{
	uint8_t msg[8+5] = {0};
	uint8_t u16TxLen = 0;

	msg[u16TxLen++] = ((DBG_KE_TIMER_REQ) & 0x00FF );
	msg[u16TxLen++] = (((DBG_KE_TIMER_REQ)>>8) & 0x00FF );
	msg[u16TxLen++] = ((TASK_DBG) & 0x00FF );
	msg[u16TxLen++] = (((TASK_DBG)>>8) & 0x00FF );	
	msg[u16TxLen++] = ((TASK_INTERNAL_APP) & 0x00FF );
	msg[u16TxLen++] = (((TASK_INTERNAL_APP)>>8) & 0x00FF );
	msg[u16TxLen++] = ((5) & 0x00FF );
	msg[u16TxLen++] = (((5)>>8) & 0x00FF );
	
	msg[u16TxLen++] = (uint8_t)((timeout) & 0x00FF );
	msg[u16TxLen++] = (uint8_t)(( (timeout) >> 8) & 0x00FF) ;
	msg[u16TxLen++] = (uint8_t)(( (timeout) >> 16) & 0x00FF);
	msg[u16TxLen++] = (uint8_t)(( (timeout) >> 24) & 0x00FF);
	
	msg[u16TxLen++] = ((1) & 0x00FF );
	
	platform_interface_send(msg, u16TxLen);
}

static void platform_stop_event_timeout(void)
{
	uint8_t msg[8+5] = {0};
	uint8_t u16TxLen = 0;

	msg[u16TxLen++] = ((DBG_KE_TIMER_REQ) & 0x00FF );
	msg[u16TxLen++] = (((DBG_KE_TIMER_REQ)>>8) & 0x00FF );
	msg[u16TxLen++] = ((TASK_DBG) & 0x00FF );
	msg[u16TxLen++] = (((TASK_DBG)>>8) & 0x00FF );	
	msg[u16TxLen++] = ((TASK_INTERNAL_APP) & 0x00FF );
	msg[u16TxLen++] = (((TASK_INTERNAL_APP)>>8) & 0x00FF );
	msg[u16TxLen++] = ((5) & 0x00FF );
	msg[u16TxLen++] = (((5)>>8) & 0x00FF );
	
	msg[u16TxLen++] = (uint8_t)((0) & 0x00FF );
	msg[u16TxLen++] = (uint8_t)(( (0) >> 8) & 0x00FF) ;
	msg[u16TxLen++] = (uint8_t)(( (0) >> 16) & 0x00FF);
	msg[u16TxLen++] = (uint8_t)(( (0) >> 24) & 0x00FF);
	
	msg[u16TxLen++] = ((0) & 0x00FF );
	
	platform_interface_send(msg, u16TxLen);
}

//Waits on InternalAppMsgQHandle Queue
// This function won't busy wait if nothing on queue but will down the semaphore and go to a blocking state
// The OS then will move control to other higher priority tasks and will only return if these tasks finish processing AND the queue has received 
//a msg
plf_drv_status platform_event_wait(uint32_t timeout)
{
	static struct ke_msg* rcv_msg;
	static struct ke_msghdr	*ke_msg_hdr;
	plf_drv_status status = STATUS_SUCCESS;
	uint8_t bEventTimeoutFlag = 0;
	uint8_t bLoopAgain = 0;
	if(((uint32_t)-1 != timeout) && ((uint32_t)0 < timeout))
	{			
		platform_start_event_timeout(timeout-1);
		bEventTimeoutFlag = 1;
	}

	do {
		if(NMI_MsgQueueRecv(InternalAppMsgQHandle, (void**)&rcv_msg) == STATUS_SUCCESS)
		{
			uint16_t msg_id = rcv_msg->id;
			uint16_t src_id = rcv_msg->src_id;
			uint8_t* data = (uint8_t*)rcv_msg->param;
			uint16_t len = rcv_msg->param_len;
			bLoopAgain = 0;
			
			if((rcv_msg->id != DBG_KE_TIMER_RESP) && (bEventTimeoutFlag))
			{
				//Stop timer if it is still running
				platform_stop_event_timeout();
			}
				
			if(msg_id == PERIPHERAL_INTERRUPT_EVENT)
			{
				if(plf_event_buff_index+len > MAX_EVT_BUFF_LEN)
					plf_event_buff_index = PLF_EVENT_BUFFER_START_INDEX;
				memcpy(&rx_buffer[plf_event_buff_index],data,len);
				platform_event_post(src_id,&rx_buffer[plf_event_buff_index],len);
				plf_event_buff_index += len;
				status = STATUS_RECEIVED_PLF_EVENT_MSG;
			}
			else
			{	
				// BLE stack messages
				if(ble_stack_message_handler) {
					if(rcv_msg->id == DBG_KE_TIMER_RESP)
					{
						if(bEventTimeoutFlag)
						{
							status = STATUS_TIMEOUT;	
						}
						else
						{
							bLoopAgain = 1;
						}
					}
					else
					{
						ke_msg_hdr = (struct ke_msghdr *)((void *)(rx_buffer+BLE_EVENT_BUFFER_START_INDEX));
						ke_msg_hdr->id = rcv_msg->id;
//#if (CHIPVERSION_A3 || CHIPVERSION_A4)
					//ke_msg_hdr->src_id = rcv_msg->src_id;
//#else
					ke_msg_hdr->src_id = gapm_get_id_from_task(rcv_msg->src_id);
//#endif  /* (CHIPVERSION_A3 || CHIPVERSION_A4) */
						ke_msg_hdr->dest_id = rcv_msg->dest_id;
						ke_msg_hdr->param_len = rcv_msg->param_len;
						ke_msg_hdr++;
						if(rcv_msg->param_len > 0) {
							memcpy((void *)ke_msg_hdr,rcv_msg->param,rcv_msg->param_len);
						}
						ble_stack_message_handler(rx_buffer,(rcv_msg->param_len + sizeof(struct ke_msghdr)));
						status = STATUS_RECEIVED_BLE_MSG;
					}
				}
			}
			ke_free(rcv_msg);
		}
		else
		{
			status = STATUS_FAILURE;
		}
	}while(bLoopAgain);
	
	if(bEventTimeoutFlag)
	{
		//CleanUp if there is any remaining DBG_KE_TIMER_RESP message in queue
		NMI_MsgQueueDestroyOnKeID(InternalAppMsgQHandle, DBG_KE_TIMER_RESP);
	}
	return status;
}

plf_drv_status acquire_sleep_lock()
{
	plf_drv_status status = STATUS_RESOURCE_BUSY;
	//uint8_t osc_en = REG_PL_RD(0x4000B1EC)&0x01;
	
	rwip_prevent_sleep_set(APP_PREVENT_SLEEP);
#if 0
	if(osc_en == 0)
	{
		/* BLE Core is off, issue a wakeup request*/
		/* First, make sure PD4 is powered up and out of reset */
		pwr_enable_arm_wakeup(1<<1);
		pwr_arm_wakeup_req();
		while (pwr_wait_BLE_out_of_reset(3));
		pwr_disable_arm_wakeup(1<<1);
		REG_PL_WR(0x4000B020, 1);
#ifndef CHIPVERSION_B0
		while(REG_PL_RD(0x4000B020));
#endif	//CHIPVERSION_B0
	}
	else
	{
		/*  */
		os_sem_up(gstrFwSem);
	}
#endif	//0
	return status;
}

plf_drv_status release_message_lock()
{
	plf_drv_status status = STATUS_SUCCESS;
	rwip_prevent_sleep_clear(MSG_PREVENT_SLEEP);
	return status;
}

plf_drv_status release_sleep_lock()
{
	plf_drv_status status = STATUS_SUCCESS;
	rwip_prevent_sleep_clear(MSG_PREVENT_SLEEP);
	rwip_prevent_sleep_clear(APP_PREVENT_SLEEP);
	return status;
}

void samb11_plf_resume_callback(void)
{
	//spi_flash_turn_off();
	REG_PL_WR(LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_0,default_samb11_clock_init[0]);
	REG_PL_WR(LPMCU_CORTEX_MISC_REGS_LPMCU_CLOCK_ENABLES_1,default_samb11_clock_init[1]);
	if(samb11_app_resume_cb != NULL)
	{
		samb11_app_resume_cb();
	}
}

plf_drv_status register_resume_callback(resume_callback cb)
{
	plf_drv_status status = STATUS_SUCCESS;
	if(cb == NULL)
	{
		status = STATUS_INVALID_ARGUMENT;
	}
	else 
	{
		samb11_app_resume_cb = cb;
		//*apps_resume_cb = (uint32_t)cb;
	}
	return status;
}

void platform_chip_reset(void)
{
	volatile uint32_t loop=0;
#ifdef SAMB11
	//Coldboot registers
	*((uint32_t *)0x4000F040) = 0x00;
	*((uint32_t *)0x4000F044) = 0x78;
	//Make sure to reset all peripherals
	//lpmcu_global_reset_1
	*((uint32_t *)0x4000B008) = 0x00;
	//lpmcu_wakeup_ctrl; open PD7
	*((uint32_t *)0x4000E010) = 0x0200;
	//arm_sleep_wakeup_ctrl[16] //by Mohamed AbdelMoneem, includes 543210 delay below them
	*((uint32_t *)0x4000B190) = (*((uint32_t *)0x4000B190) & ~(0x00010000));
	*((uint32_t *)0x4000B190) = (*((uint32_t *)0x4000B190) |  (0x00010000));
	*((uint32_t *)0x4000B190) = (*((uint32_t *)0x4000B190) & ~(0x00010000));
	//dummy loop
	for(loop=0; loop<543210;loop++);
	//efuse_global_reset
	*((uint32_t *)0x4000A000) = 0x0;
	//dummy loop
	for(loop=0; loop<43210;loop++);
	//AOn_global_reset
	*((uint32_t *)0x4000F010) = 0x00;
#else
#endif
	return;
}
