/**
 * \file serial_drv.c
 *
 * \brief Handles Serial driver functionalities
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 */

/* === INCLUDES ============================================================ */

#include <asf.h>
#include <string.h>
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

/* Flag to update the pdc addr and len in SAM4S*/
uint8_t first_byte_received = true;
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
uint8_t *pdc_rxbuf_readaddr = NULL;
//uint8_t pdc_rx_next_buffer[BLE_MAX_RX_PAYLOAD_SIZE]; //Improved with Next Buffer chain
volatile bool pdc_uart_enabled = false;

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
	
	/* Enable Buffer Empty Interrupt */
	usart_enable_interrupt(BLE_UART, US_IER_TXBUFE);
}

static inline uint8_t configure_primary_uart(void)
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
  	
	/* Configure the UART Tx and Rx Pin Modes */
  	ioport_set_pin_peripheral_mode(EXT1_PIN_13, (PIO_PERIPH_A | PIO_DEFAULT));
  	ioport_set_pin_peripheral_mode(EXT1_PIN_14, (PIO_PERIPH_A | PIO_DEFAULT));

#if  (UART_FLOWCONTROL_4WIRE_MODE == true) || (UART_FLOWCONTROL_6WIRE_MODE == true)
	/* Configure the UART RTS and CTS Pin Modes */
	ioport_set_pin_peripheral_mode(EXT1_PIN_6, (PIO_PERIPH_A | PIO_DEFAULT));
	ioport_set_pin_peripheral_mode(EXT1_PIN_5, (PIO_PERIPH_A | PIO_DEFAULT));
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

#if SAMG55	
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
#if SAM4S
	usart_enable_interrupt(BLE_UART, US_IER_RXBUFF | US_IER_OVRE | US_IER_TIMEOUT);
#else //SAMG55
  	usart_enable_interrupt(BLE_UART, US_IER_RXBUFF | US_IER_OVRE | US_IER_TIMEOUT | US_IER_CMP);
#endif  	

  	/* Enable UART interrupt */
  	NVIC_EnableIRQ(BLE_UART_IRQn);
	  
	pdc_uart_enabled = true;
	  
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

uint8_t configure_serial_drv(void)
{
#if UART_FLOWCONTROL_6WIRE_MODE == true
	configure_patch_usart();
#else
	configure_primary_uart();
#endif
	return STATUS_OK;
}

void configure_usart_after_patch(void)
{

#if UART_FLOWCONTROL_6WIRE_MODE == true
	configure_primary_uart();
#endif

}

static inline void pdc_update_rx_transfer(void)
{
#if SAMG55
	/* Initialize the Rx buffers for data receive */
  	ble_usart_rx_pkt.ul_addr = (uint32_t)pdc_rx_buffer;
	pdc_enable_transfer(ble_usart_pdc, PERIPH_PTCR_RXTEN);
	
  	ble_usart_rx_pkt.ul_size = BLE_MAX_RX_PAYLOAD_SIZE;
	  
	/* Configure the PDC for data receive */
	pdc_rx_init(ble_usart_pdc, &ble_usart_rx_pkt, NULL);
	pdc_rxbuf_readaddr = &pdc_rx_buffer[0];
#endif

#if SAM4S
	if(true == first_byte_received)
	{
		static uint8_t slave_state_connected = false;
		first_byte_received = false;
	
		/* Initialize the Rx buffers for data receive */
		ble_usart_rx_pkt.ul_addr = (uint32_t)&pdc_rx_buffer[0];

		pdc_enable_transfer(ble_usart_pdc, PERIPH_PTCR_RXTEN);

		ble_usart_rx_pkt.ul_size =  1;
		pdc_rxbuf_readaddr = &pdc_rx_buffer[1];
		if (!slave_state_connected)
		{
			slave_state_connected = true;
			pdc_rxbuf_readaddr = &pdc_rx_buffer[0];
		}
	}
	else
	{
		first_byte_received = true;
	
		/* Initialize the Rx buffers for data receive */
		ble_usart_rx_pkt.ul_addr = (uint32_t)&pdc_rx_buffer[1];

		pdc_enable_transfer(ble_usart_pdc, PERIPH_PTCR_RXTEN);

		ble_usart_rx_pkt.ul_size = BLE_MAX_RX_PAYLOAD_SIZE - 1;
		pdc_rxbuf_readaddr = &pdc_rx_buffer[0];
	}
	/* Configure the PDC for data receive */
	pdc_rx_init(ble_usart_pdc, &ble_usart_rx_pkt, NULL);
#endif
}

static inline uint32_t usart_is_rx_timeout(Usart *p_usart)
{
	return (p_usart->US_CSR & US_CSR_TIMEOUT) > 0;
}

static inline uint32_t usart_is_rx_buffer_overrun(Usart *p_usart)
{
	return (p_usart->US_CSR & US_CSR_OVRE) > 0;
}

#if SAMG55
static inline uint32_t usart_is_rx_compare(Usart *p_usart)
{
	return (p_usart->US_CSR & US_CSR_CMP) > 0;
}
#endif

static inline uint32_t usart_clear_tx_empty(Usart *p_usart)
{
	return (p_usart->US_TNCR  = 0);
}



static inline void ble_pdc_uart_handler(void)
{
	if (usart_is_tx_buf_empty(BLE_UART) && (ble_usart_tx_cmpl == false))
	{
		usart_disable_interrupt(BLE_UART, US_IER_TXBUFE);
		usart_clear_tx_empty(BLE_UART);
		ble_usart_tx_cmpl  = true;
	}
		
	if(!usart_is_rx_buffer_overrun(BLE_UART))
	{
		uint32_t timeout = usart_is_rx_timeout(BLE_UART);
		if (usart_is_rx_buf_full(BLE_UART) || timeout)
		{
			uint16_t rx_count;
			/* Disable the Receive Transfer before read */
			pdc_disable_transfer(ble_usart_pdc, PERIPH_PTCR_RXTDIS);
			rx_count = (ble_usart_rx_pkt.ul_size - pdc_read_rx_counter(ble_usart_pdc));
			if(rx_count)
			{
				pdc_update_rx_transfer();								
				platform_dma_process_rxdata(pdc_rxbuf_readaddr, rx_count);		
				#if SERIAL_DRV_RX_CB_ENABLE
					SERIAL_DRV_RX_CB();
				#endif
			}						
				
			
			if (timeout)
			{
				/* Clear the Rx Frame Timeout Interrupt */
				usart_start_rx_timeout(BLE_UART);
			}						
		}

#if SAMG55			
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
	ble_usart_tx_cmpl = false;
	ble_pdc_send_data(data, len);
	
	/* Wait for Tx Data write complete */
	while(ble_usart_tx_cmpl == false);
	
	if(ble_usart_tx_cmpl)
	{
		#if SERIAL_DRV_TX_CB_ENABLE
		SERIAL_DRV_TX_CB();
		#endif
	}
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
	  
	  if(ble_usart_tx_cmpl)
	  {
		  #if SERIAL_DRV_TX_CB_ENABLE
		  SERIAL_DRV_TX_CB();
		  #endif
	  }
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

void platfrom_start_rx(void)
{
	
}

/* EOF */

