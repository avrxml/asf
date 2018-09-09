/**
 * \file serial_drv.c
 *
 * \brief Handles Serial driver functionalities
 *
 * Copyright (c) 2017-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 */

/* === INCLUDES ============================================================ */

#include <asf.h>
#include <string.h>
#include "ble_utils.h"
#include "serial_drv.h"
#include "conf_serialdrv.h"
#include "serial_fifo.h"
#include "platform.h"

/* === GLOBALS ========================================================== */
volatile bool ble_usart_tx_cmpl = true;

static volatile uint16_t ble_txbyte_count = 0;
#if UART_FLOWCONTROL_6WIRE_MODE == true
static volatile uint8_t *ble_txbuf_ptr = NULL;
#endif
uint16_t g_txdata;

#if SAM4S
uint8_t buf_temp[3] = {'R','W', '!'};
#endif

extern void platform_pdc_process_rxdata(uint8_t *buf, uint16_t len);

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/**
 * \brief Set peripheral mode for one single IOPORT pin.
 * It will configure port mode and disable pin mode (but enable peripheral).
 * \param pin IOPORT pin to configure
 * \param mode Mode masks to configure for the specified pin (\ref ioport_modes)
 */
#define ioport_set_pin_peripheral_mode(pin, mode) \
	do {\
		ioport_set_pin_mode(pin, mode);\
		ioport_disable_pin(pin);\
	} while (0)
	
/* === PROTOTYPES ========================================================== */

/* === GLOBALS ========================================================== */

/* === IMPLEMENTATION ====================================================== */

Pdc *ble_usart_pdc;
pdc_packet_t ble_usart_tx_pkt;
pdc_packet_t ble_usart_rx_pkt;
uint8_t pdc_rx_buffer[BLE_MAX_RX_PAYLOAD_SIZE];
volatile uint16_t dma_buf_index;
volatile bool pdc_uart_enabled = false;
volatile enum sleepmgr_mode current_sleep_mode = SLEEPMGR_ACTIVE;

void ble_pdc_send_data(uint8_t *buf, uint16_t len);

void ble_pdc_send_data(uint8_t *buf, uint16_t len)
{

#if SAM4S	
	/* Initialize the Tx buffers for data transfer */
	if(len == 3)
	{
		ble_usart_tx_pkt.ul_addr = (uint32_t)buf_temp;
	}
	else
#endif	
	{
		ble_usart_tx_pkt.ul_addr = (uint32_t)buf;	
	}
	
	ble_usart_tx_pkt.ul_size = len;
				
	/* Configure the PDC for data transmit */
	pdc_tx_init(ble_usart_pdc, &ble_usart_tx_pkt, NULL);
	
	/* Enable Tx Empty Interrupt */
	usart_enable_interrupt(BLE_UART, US_IER_TXEMPTY);
}

static inline uint8_t configure_primary_uart(uint32_t bus_baudrate)
{ 	
  	/* Usart async mode 8 bits transfer test */
  	sam_usart_opt_t usart_settings = {
	  	.baudrate     = bus_baudrate,
	  	.char_length  = US_MR_CHRL_8_BIT,
	  	.parity_type  = US_MR_PAR_NO,
	  	.stop_bits    = US_MR_NBSTOP_1_BIT,
	  	.channel_mode = US_MR_CHMODE_NORMAL,
	  	/* This field is only used in IrDA mode. */
	  	.irda_filter  = 0
  	};
	  	 
	usart_reset(BLE_UART);
  	
	/* Configure the UART Tx and Rx Pin Modes */
#if SAM4S
	gpio_configure_pin(PIO_PA21_IDX, PIO_PERIPH_A | PIO_PULLUP);
	gpio_configure_pin(PIO_PA22_IDX, PIO_PERIPH_A | PIO_PULLUP);
#else	//SAMG55
  	ioport_set_pin_peripheral_mode(USART0_RXD_GPIO, USART0_RXD_FLAGS | IOPORT_MODE_PULLUP);
  	ioport_set_pin_peripheral_mode(USART0_TXD_GPIO, USART0_TXD_FLAGS);
#endif	  

#if  (UART_FLOWCONTROL_4WIRE_MODE == true) || (UART_FLOWCONTROL_6WIRE_MODE == true)
	/* Configure the UART RTS and CTS Pin Modes */
#if SAM4S
	gpio_configure_pin(PIO_PA24_IDX, PIO_PERIPH_A | PIO_DEFAULT);
	gpio_configure_pin(PIO_PA25_IDX, PIO_PERIPH_A | PIO_DEFAULT);
#else //SAMG55
	ioport_set_pin_peripheral_mode(USART0_RTS_GPIO, USART0_RTS_FLAGS);
	ioport_set_pin_peripheral_mode(USART0_CTS_GPIO, USART0_CTS_FLAGS);
#endif
#endif
  	
  	/* Clock Configuration for UART */
  	sysclk_enable_peripheral_clock(BLE_UART_ID);

#if SAMG55  	
  	/* Enable the peripheral and set USART mode. */
  	flexcom_enable(BLE_USART_FLEXCOM);
  	flexcom_set_opmode(BLE_USART_FLEXCOM, FLEXCOM_USART);
#endif

#if  (UART_FLOWCONTROL_4WIRE_MODE == true) || (UART_FLOWCONTROL_6WIRE_MODE == true)
	/* Configure USART with Flow Control */
	usart_init_hw_handshaking(BLE_UART, &usart_settings,
	sysclk_get_peripheral_hz());
#else
  	/* Configure USART */
  	usart_init_rs232(BLE_UART, &usart_settings,
  	sysclk_get_peripheral_hz());
#endif
  	
  	/* Enable the receiver and transmitter. */
  	usart_enable_tx(BLE_UART);
  	usart_enable_rx(BLE_UART);
	  
	usart_set_rx_timeout(BLE_UART, RX_TIMEOUT_VALUE);
	usart_start_rx_timeout(BLE_UART);

#if (SLEEP_WALKING_ENABLED && SAMG55)	
	usart_set_sleepwalking(BLE_UART, 0x04, true, false, 0x5A);
#endif
 
  	ble_usart_pdc = usart_get_pdc_base(BLE_UART);
  	  	
/* Initialize the Rx buffers for data receive */
  	ble_usart_rx_pkt.ul_addr = (uint32_t)pdc_rx_buffer;
	ble_usart_rx_pkt.ul_size = BLE_MAX_RX_PAYLOAD_SIZE;	
	
  	/* Configure the PDC for data receive */
  	pdc_rx_init(ble_usart_pdc, &ble_usart_rx_pkt, NULL);
  	
  	pdc_enable_transfer(ble_usart_pdc, PERIPH_PTCR_RXTEN | PERIPH_PTSR_TXTEN);
  	
  	/* Enable UART IRQ */
#if (SLEEP_WALKING_ENABLED && SAMG55)
  	usart_enable_interrupt(BLE_UART, US_IER_RXBUFF | US_IER_OVRE | US_IER_TIMEOUT | US_IER_CMP);
#else
	usart_enable_interrupt(BLE_UART, US_IER_RXBUFF | US_IER_OVRE | US_IER_TIMEOUT);
#endif

  	/* Enable UART interrupt */
  	NVIC_EnableIRQ(BLE_UART_IRQn);
	  
	pdc_uart_enabled = true;
	
	dma_buf_index = 0;
	  
	return STATUS_OK;
}

#if UART_FLOWCONTROL_6WIRE_MODE == true
static inline uint8_t configure_patch_usart(void)
{
	 /* Usart async mode 8 bits transfer test */
	 sam_usart_opt_t usart_settings = {
		 .baudrate     = CONF_UART_BAUDRATE,
		 .char_length  = US_MR_CHRL_8_BIT,
		 .parity_type  = US_MR_PAR_NO,
		 .stop_bits    = US_MR_NBSTOP_1_BIT,
		 .channel_mode = US_MR_CHMODE_NORMAL,
		 /* This field is only used in IrDA mode. */
		 .irda_filter  = 0
	 };
	 
	 ioport_set_pin_peripheral_mode(EXT1_PIN_16, IOPORT_MODE_MUX_A);
	 ioport_set_pin_peripheral_mode(EXT1_PIN_17, IOPORT_MODE_MUX_A);
	
	 /* Enable the peripheral and set USART mode. */
	 flexcom_enable(BLE_PATCH_USART_FLEXCOM);
	 flexcom_set_opmode(BLE_PATCH_USART_FLEXCOM, FLEXCOM_USART);

	 /* Configure USART */
	 usart_init_rs232(BLE_PATCH_UART, &usart_settings,
	 sysclk_get_peripheral_hz());
	 
	 /* Enable the receiver and transmitter. */
	 usart_enable_tx(BLE_PATCH_UART);
	 usart_enable_rx(BLE_PATCH_UART);

	 /* Enable UART IRQ */
	 usart_enable_interrupt(BLE_PATCH_UART, US_IER_RXRDY);

	 /* Enable UART interrupt */
	 NVIC_EnableIRQ(BLE_PATCH_UART_IRQn);

	 return STATUS_OK;
}
#endif

uint8_t configure_serial_drv(uint32_t bus_baudrate)
{
#if UART_FLOWCONTROL_6WIRE_MODE == true
	(void)bus_baudrate;
	configure_patch_usart();
#else
	configure_primary_uart(bus_baudrate);
#endif
	return STATUS_OK;
}

void configure_usart_after_patch(uint32_t bus_baudrate)
{

#if UART_FLOWCONTROL_6WIRE_MODE == true
	configure_primary_uart(bus_baudrate);
#else
	(void)bus_baudrate;	
#endif

}

static inline void pdc_update_rx_transfer(void)
{
	/* Initialize the Rx buffers for data receive */
  	ble_usart_rx_pkt.ul_addr = (uint32_t)pdc_rx_buffer;
	
  	ble_usart_rx_pkt.ul_size = BLE_MAX_RX_PAYLOAD_SIZE;
	  
	/* Configure the PDC for data receive */
	pdc_rx_init(ble_usart_pdc, &ble_usart_rx_pkt, NULL);

	dma_buf_index = 0;
}

static inline uint32_t usart_is_rx_timeout(Usart *p_usart)
{
	return (p_usart->US_CSR & US_CSR_TIMEOUT) > 0;
}

static inline uint32_t usart_is_rx_buffer_overrun(Usart *p_usart)
{
	return (p_usart->US_CSR & US_CSR_OVRE) > 0;
}

#if (SLEEP_WALKING_ENABLED && SAMG55)
static inline uint32_t usart_is_rx_compare(Usart *p_usart)
{
	return (p_usart->US_CSR & US_CSR_CMP) > 0;
}
#endif

static inline uint32_t usart_clear_tx_empty(Usart *p_usart)
{
	return (p_usart->US_TNCR  = 0);
}

void platform_set_ble_rts_high(void)
{
	/* Set the host RTS pin to high */
	/* In US_MR.USART_MODE = 2, the RTS pin is driven High when RTSEN is set */
	usart_drive_RTS_pin_low(BLE_UART);
}

void platform_set_ble_rts_low(void)
{
	/* Set the host RTS pin to low */
	/* In US_MR.USART_MODE = 2, the RTS pin is driven Low when RTSDIS is set */
	usart_drive_RTS_pin_high(BLE_UART);
}

static inline void ble_pdc_uart_handler(void)
{
	if (usart_is_tx_empty(BLE_UART) && (ble_usart_tx_cmpl == false))
	{
		usart_disable_interrupt(BLE_UART, US_IER_TXEMPTY);
		usart_clear_tx_empty(BLE_UART);
		ble_usart_tx_cmpl  = true;
	}
		
	if(!usart_is_rx_buffer_overrun(BLE_UART))
	{
		bool timeout = usart_is_rx_timeout(BLE_UART);
		bool rx_buf_full = usart_is_rx_buf_full(BLE_UART);
		
		if (timeout)
		{
			/* Clear the Rx Frame Timeout Interrupt */
			usart_start_rx_timeout(BLE_UART);
		}
		
		if (rx_buf_full || timeout)
		{			
			uint16_t rx_count;						
			rx_count = ((ble_usart_rx_pkt.ul_size - dma_buf_index) - pdc_read_rx_counter(ble_usart_pdc));	
						
			if(rx_count)
			{				
				#if SERIAL_DRV_RX_CB_ENABLE
					SERIAL_DRV_RX_CB();
				#endif				
				platform_dma_process_rxdata((uint8_t *)(ble_usart_rx_pkt.ul_addr+dma_buf_index), rx_count);					
				dma_buf_index += rx_count;				
			}
			
			if (rx_buf_full)
			{
				pdc_update_rx_transfer();
			}													
		}

#if (SLEEP_WALKING_ENABLED && SAMG55)		
		if(usart_is_rx_compare(BLE_UART))
		{
			usart_reset_status(BLE_UART);
			#if SERIAL_DRV_RX_CB_ENABLE
				SERIAL_DRV_RX_CB();
			#endif
		}	
#endif		
	}
	else
	{
	 /* Handle Buffer Over-Run Error */	
	 usart_reset_status(BLE_UART);		  
	}	
}

#if UART_FLOWCONTROL_6WIRE_MODE == true
void BLE_PATCH_UART_Handler(void)
{
	if (usart_is_rx_ready(BLE_PATCH_UART))
	{
		uint32_t rx_data = 0;
		if (usart_read(BLE_PATCH_UART, &rx_data) == 0)
		{
			platform_process_rxdata((uint8_t)rx_data);
		}
	}
	
	if ((usart_is_tx_empty(BLE_PATCH_UART))  && (!ble_usart_tx_cmpl))
	{
		/* USART Tx callback */
		if(ble_txbyte_count)
		{
			g_txdata = *ble_txbuf_ptr;
			usart_putchar(BLE_PATCH_UART, g_txdata);
			if(--ble_txbyte_count)
			{
				++ble_txbuf_ptr;
			}
		}
		else
		{
			usart_disable_interrupt(BLE_PATCH_UART, US_IER_TXEMPTY);
			ble_usart_tx_cmpl  = true;
		}
	}
}
#endif

void BLE_UART_Handler(void)
{
	ble_pdc_uart_handler();
}


static inline void ble_pdc_serial_drv_send(uint8_t *data, uint16_t len)
{				 
	ble_pdc_send_data(data, len);
	
	/* Set the tx complete flag to false and wait for UART transmission to be completed */
	ble_usart_tx_cmpl = false;
	
	/* Wait for transmitter holding register empty, i.e Wait until last bit is transmitted in USART */
	while(ble_usart_tx_cmpl == false);
}

#if UART_FLOWCONTROL_6WIRE_MODE == true
static inline void ble_patch_serial_drv_send(uint8_t *data, uint16_t len)
{
  ble_txbuf_ptr = data;
  ble_txbyte_count = len;
  ble_usart_tx_cmpl = false;
  if(ble_txbyte_count)
  {
	  g_txdata = *ble_txbuf_ptr;
	  usart_putchar(BLE_PATCH_UART, g_txdata);
	  //Enable the USART Empty Interrupt
	  usart_enable_interrupt(BLE_PATCH_UART, US_IER_TXEMPTY);
	  if(--ble_txbyte_count)
	  {
		  ++ble_txbuf_ptr;
	  }
	  /* Wait for ongoing transmission complete */
	  while(ble_usart_tx_cmpl == false);
  }
}
#endif

uint16_t serial_drv_send(uint8_t* data, uint16_t len)
{
	if (pdc_uart_enabled)
	{
		ble_pdc_serial_drv_send(data, len);
	} 
	else
	{
#if UART_FLOWCONTROL_6WIRE_MODE == true	
		ble_patch_serial_drv_send(data, len);
#endif
	}
	
	return STATUS_OK;
}

uint32_t platform_serial_drv_tx_status(void)
{
	return((ble_usart_tx_cmpl == true) ? 0 : 1);
}

void platform_leave_critical_section(void)
{
	Enable_global_interrupt();
}
void platform_enter_critical_section(void)
{
	Disable_global_interrupt();
}

/* Initialize the sleep manager */
void platform_configure_sleep_manager(void)
{
	sleepmgr_init();
	/* Initialize the sleep manager, lock initial mode. */
	sleepmgr_lock_mode(current_sleep_mode);
}

void platform_restore_from_sleep(void)
{
	/* Check the Clock is stable and ready to receive the data */
	
	/* Restore the System from sleep */
	sleepmgr_unlock_mode(current_sleep_mode);
	
	current_sleep_mode = SLEEPMGR_ACTIVE;
	sleepmgr_lock_mode(current_sleep_mode);	
}

/* Set the Host in sleep */
void platform_set_hostsleep(void)
{
	sleepmgr_unlock_mode(current_sleep_mode);
		
	/* Put the system to sleep */
	current_sleep_mode = SLEEPMGR_SLEEP_WFI;
	/*
	* Go to sleep in the deepest allowed sleep mode 
	*/
	sleepmgr_lock_mode(current_sleep_mode);	
	sleepmgr_enter_sleep();					
}

uint16_t serial_drive_rx_data_count(void)
{
	return ((ble_usart_rx_pkt.ul_size - dma_buf_index) - pdc_read_rx_counter(ble_usart_pdc));
}
/* EOF */

