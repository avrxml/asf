/**************************************************************************//**
  \file common.h
 
  \brief Includes datatypes and signatures for common
 
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

#ifndef __COMMON_H
#define __COMMON_H
#include <stdint.h>

#define REG_WR(addr, value)      (*(volatile unsigned int *)(addr)) = (value)
#define REG_RD(addr)             (*(volatile unsigned int *)(addr))

enum INTC_STATUS
{
    /// INTC status OK
    INTC_OK = 0x00,
		///	INTC Invalid IRQ number
		INTC_INVALID_IRQ,
		/// INTC Invalid IRQ handler
		INTC_INVALID_ISR,
    /// INTC Error Register ISR
    INTC_ISR_ALREADY_REGISTERED,
};

#ifndef CHIPVERSION_B0
#define TASK_INTERNAL_APP	0x3E
#define APP_FIRST_MSG(event_index)	((event_index << 10))
#else
#define TASK_INTERNAL_APP	0x05
#define APP_FIRST_MSG(event_index)	((event_index << 8))
#endif

#define TASK_DBG			0x03

/// Message API of the DBG task
enum dbg_msg_id
{
    // Debug commands description.
    DBG_MSG_ID_CMD_FIRST = ((uint16_t)(TASK_DBG << 8)),
    DBG_RD_MEM_REQ = DBG_MSG_ID_CMD_FIRST,
    DBG_WR_MEM_REQ,
    DBG_DEL_PARAM_REQ,
    DBG_RD_PARAM_REQ,
    DBG_WR_PARAM_REQ,
    DBG_PLF_RESET_REQ,
    DBG_FLASH_IDENT_REQ,
    DBG_FLASH_ERASE_REQ,
    DBG_FLASH_WRITE_REQ,
    DBG_FLASH_READ_REQ,

    DBG_RD_KE_STATS_REQ,
    DBG_RF_REG_RD_REQ,
    DBG_RF_REG_WR_REQ,
    DBG_HW_REG_RD_REQ,
    DBG_HW_REG_WR_REQ,

    DBG_LE_SET_BD_ADDR_REQ,
    DBG_LE_SET_TYPE_PUB_REQ,
    DBG_LE_SET_TYPE_RAND_REQ,
    DBG_LE_SET_CRC_REQ,
    DBG_LE_LLCP_DISCARD_REQ,
    DBG_LE_RESET_RX_CNT_REQ,
    DBG_LE_RESET_TX_CNT_REQ,
    DBG_SET_TX_PW_REQ,

    DBG_WLAN_COEX_REQ,

    DBG_RD_MEM_INFO_REQ,
    
    DBG_MSG_ID_EVT_FIRST,

    /// Debug events description.
    DBG_RD_MEM_CMP_EVT = DBG_MSG_ID_EVT_FIRST,
    DBG_WR_MEM_CMP_EVT,
    DBG_DEL_PARAM_CMP_EVT,
    DBG_RD_PARAM_CMP_EVT,
    DBG_WR_PARAM_CMP_EVT,
    DBG_FLASH_IDENT_CMP_EVT,
    DBG_FLASH_ERASE_CMP_EVT,
    DBG_FLASH_WRITE_CMP_EVT,
    DBG_FLASH_READ_CMP_EVT,

    DBG_RD_KE_STATS_CMP_EVT,
    DBG_TRACE_WARNING_EVT,
    DBG_RF_REG_RD_CMP_EVT,
    DBG_RF_REG_WR_CMP_EVT,
    DBG_PLF_RESET_CMP_EVT,
    DBG_HW_REG_RD_CMP_EVT,
    DBG_HW_REG_WR_CMP_EVT,

    DBG_LE_SET_BD_ADDR_CMP_EVT,
    DBG_LE_SET_TYPE_PUB_CMP_EVT,
    DBG_LE_SET_TYPE_RAND_CMP_EVT,
    DBG_LE_SET_CRC_CMP_EVT,
    DBG_LE_LLCP_DISCARD_CMP_EVT,
    DBG_LE_RESET_RX_CNT_CMP_EVT,
    DBG_LE_RESET_TX_CNT_CMP_EVT,
    DBG_SET_TX_PW_CMP_EVT,


    DBG_WLAN_COEX_CMP_EVT,

    DBG_RD_MEM_INFO_CMP_EVT,
	
    DBG_MSG_ID_EVT_LAST_FIRST
};

enum 
{
	//CMDs
	DBG_MSG_ID_CMD_SECOND = DBG_MSG_ID_EVT_LAST_FIRST,
	DBG_KE_TIMER_REQ = DBG_MSG_ID_CMD_SECOND,
	
	//EVTs
	DBG_MSG_ID_EVT_SECOND,
	DBG_KE_TIMER_RESP = DBG_MSG_ID_EVT_SECOND,
	
	//EndOfDbgEnum
	DBG_MSG_ID_EVT_LAST
};

typedef struct _tstrOsTask{
	unsigned long 	u32Sp;						/*!< Task stack pointer */
	void			(*pvTaskEntryFn)(void*);	/*!< Task entry function */
	unsigned char	u8State;					/*!< Task state */
	unsigned short	u8Priority;					/*!< Task Priority. 254 is the lowest priority and 0 is the highest */
	unsigned short	u16StackSize;				/*!< Task stack size */
	unsigned char	*pu8Stack;					/*!< Pointer to task stack memory start */
	unsigned char	u8MarkForRemoval;			/*!< When set, this task will not be added to any queue */
	unsigned long	u64SleepTime;				/*!< Sleep time of the task in case of it was sleeping */
	char*			cName;						/*!< Task Name */
	struct _tstrOsTask *pstrNext;				/*!< Pointer to the next object at the list */
} tstrOsTask;

typedef struct _tstrOsSemaphore{
	unsigned long 	u32Count;					/*!< Semaphore Count */
	char*			cName;						/*!< Semaphore Name */
	tstrOsTask 		*pstrWaitingLst;			/*!< Waiting tasks list */
} tstrOsSemaphore;

typedef struct __Message_struct
{
	void* pvBuffer;
	unsigned int u32Length;
	struct __Message_struct *pstrNext;
} Message;

typedef struct __MessageQueue_struct
{	
	tstrOsSemaphore hSem;
	char bExiting;
	unsigned int u32ReceiversCount;
	tstrOsSemaphore strCriticalSection;
	Message * pstrMessageList;
} NMI_MsgQueueHandle;

#define BUILD_INTR_SRCID(callback_index,intr_index)	((callback_index << 8)|(intr_index))

enum internal_app_msg_id
{
	PERIPHERAL_INTERRUPT_EVENT = APP_FIRST_MSG(TASK_INTERNAL_APP),
};

#define BUILD_INTR_EVENT_MSG(msg,src_id,param,param_len)	\
			msg->id = PERIPHERAL_INTERRUPT_EVENT;								\
			msg->dest_id = TASK_INTERNAL_APP;										\
			msg->src_id = src_id;																\
			msg->param_len = param_len;													\
			if(param_len > 0)																		\
				memcpy(msg->param,param,param_len);
/**
 * \brief Registers an ISR for an IRQ.
 *
 * Registers an ISR into the vector table for the given IRQ index that are usable by
 * SAMB11 applications.
 *
 * \param[in]	isr_index  index into the vector table
 * \param[in]	fp  			 Pointer to the ISR routine.
 *
 */
extern uint8_t (*platform_register_isr)(uint8_t isr_index,void *fp);

/**
 * \brief Unregisters an ISR for an IRQ.
 *
 * Unregisters an ISR from the vector table for the given IRQ index that are usable by
 * SAMB11 applications.
 *
 * \param[in]	isr_index  index into the vector table
 *
 */
extern uint8_t (*platform_unregister_isr)(uint8_t isr_index);
extern uint32_t *actualfreq;
extern uint32_t *wakeup_event_pending;
//#ifdef CHIPVERSION_B0
extern void (*handle_ext_wakeup_isr)(void);
void PORT1_COMB_Handler(void);
//#endif	//CHIPVERSION_B0

void gpio1_combined_isr_handler(void);
void gpio0_combined_isr_handler(void);

#endif	//__COMMON_H
