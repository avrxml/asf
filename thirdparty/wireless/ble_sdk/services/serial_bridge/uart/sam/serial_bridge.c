/**
 * \file serial_bridge.c
 *
 * \brief Handles Serial Bridge driver functionalities
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

#include "asf.h"
#include "serial_bridge.h"
#include "conf_serialbridge.h"
#include "serial_fifo.h"
#include "conf_serialdrv.h"

/* === GLOBALS ========================================================== */
extern volatile bool ble_usart_tx_cmpl;
volatile bool ble_eusart_tx_cmpl = true;

static ser_fifo_desc_t ble_eusart_tx_fifo;
static  uint8_t ble_eusart_tx_buf[BLE_MAX_TX_PAYLOAD_SIZE];
static  uint8_t ble_usart_tx_buf[BLE_MAX_TX_PAYLOAD_SIZE];


static ser_fifo_desc_t ble_eusart_rx_fifo;
static uint8_t ble_eusart_rx_buf[BLE_MAX_TX_PAYLOAD_SIZE];

ser_fifo_desc_t ble_usart_tx_fifo;
ser_fifo_desc_t ble_usart_rx_fifo;
uint8_t ble_usart_rx_buf[BLE_MAX_RX_PAYLOAD_SIZE];

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




uint8_t serial_bridge_init(void)
{
#if SAMG55	
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
#endif
#if SAM4S
	const sam_uart_opt_t uart_settings = {
		.ul_mck			= sysclk_get_peripheral_hz(),
		.ul_baudrate   		= CONF_UART_BAUDRATE,
		.ul_mode		= US_MR_CHMODE_NORMAL		
	};
#endif
#if SAMG55
	/* Enable the peripheral and set USART mode. */
	flexcom_enable(SB_USART_FLEXCOM);
	flexcom_set_opmode(SB_USART_FLEXCOM, FLEXCOM_USART);

	/* Configure USART */
	usart_init_rs232(SB_UART, &usart_settings,
	sysclk_get_peripheral_hz());
	
	/* Enable the receiver and transmitter. */
	usart_enable_tx(SB_UART);
	usart_enable_rx(SB_UART);

	/* Enable UART IRQ */
	usart_enable_interrupt(SB_UART, US_IER_RXRDY);
#endif	
#if SAM4S
	/* Enabling the peripheral clock */
	sysclk_enable_peripheral_clock(SB_UART_ID);
	
	/* Enabling Pull-Up for UART RXD pin */
	pio_pull_up(PIOB,PIO_PB2A_URXD1,PIO_PULLUP);
	
	/* Initialize UART */
	uart_init(SB_UART,&uart_settings);
	
	/* Enable UART IRQ */
	uart_enable_interrupt(SB_UART, US_IER_RXRDY);
#endif

	ser_fifo_init(&ble_eusart_rx_fifo, ble_eusart_rx_buf, BLE_MAX_RX_PAYLOAD_SIZE);
	ser_fifo_init(&ble_eusart_tx_fifo, ble_eusart_tx_buf, BLE_MAX_TX_PAYLOAD_SIZE);
	ser_fifo_init(&ble_usart_tx_fifo, ble_usart_tx_buf, BLE_MAX_TX_PAYLOAD_SIZE);
	ser_fifo_init(&ble_usart_rx_fifo, ble_usart_rx_buf, BLE_MAX_RX_PAYLOAD_SIZE);

	/* Enable UART interrupt */
	NVIC_EnableIRQ(SB_UART_IRQn);

	return STATUS_OK;
}

void SB_UART_Handler(void)
{
#if SAMG55	
	if (usart_is_rx_ready(SB_UART))
	{
		uint32_t rx_data = 0;
		if (usart_read(SB_UART, &rx_data) == 0)
		{
			ser_fifo_push_uint8(&ble_eusart_rx_fifo, rx_data);
		}
	}
	
	if ((usart_is_tx_empty(SB_UART))  && (!ble_eusart_tx_cmpl))
	{
		uint8_t txdata;
		if(ser_fifo_pull_uint8(&ble_eusart_tx_fifo, &txdata) == SER_FIFO_OK)
		{
			usart_putchar(SB_UART, txdata);
		}
		else
		{
			usart_disable_interrupt(SB_UART, US_IER_TXEMPTY);
			ble_eusart_tx_cmpl  = true;
		}
	}
#endif
#if SAM4S
	if (uart_is_rx_ready(SB_UART))
	{
		uint32_t rx_data = 0;
		if (uart_read(SB_UART, (uint8_t *)&rx_data) == 0)
		{
			ser_fifo_push_uint8(&ble_eusart_rx_fifo, rx_data);
		}
	}
	
	if ((uart_is_tx_empty(SB_UART))  && (!ble_eusart_tx_cmpl))
	{
		uint8_t txdata;
		if(ser_fifo_pull_uint8(&ble_eusart_tx_fifo, &txdata) == SER_FIFO_OK)
		{
			uart_write(SB_UART, txdata);
		}
		else
		{
			uart_disable_interrupt(SB_UART, US_IER_TXEMPTY);
			ble_eusart_tx_cmpl  = true;
		}
	}
#endif
}

extern void ble_pdc_send_data(uint8_t *buf, uint16_t len);

void serial_bridge_task(void)
{
	uint8_t t_rx_data;

	/* Check the UART Rx data from BLE UART */
	if(ser_fifo_pull_uint8(&ble_usart_rx_fifo, (uint8_t *)&t_rx_data) == SER_FIFO_OK)
	{		
		LED_Toggle(LED0);
		/* Write to the EDBG UART Buffer */
		ser_fifo_push_uint8(&ble_eusart_tx_fifo, (uint8_t)t_rx_data);
		if(ble_eusart_tx_cmpl)
		{			
		  if(ser_fifo_pull_uint8(&ble_eusart_tx_fifo, &t_rx_data) == SER_FIFO_OK)
		  {	
			  ble_eusart_tx_cmpl = false;
			  #if SAMG55
			  usart_putchar(SB_UART, t_rx_data);
			  //Enable the USART Empty Interrupt
			  usart_enable_interrupt(SB_UART, US_IER_TXEMPTY);				
			  #endif
			  #if SAM4S
			  uart_write(SB_UART, t_rx_data);
			  //Enable the USART Empty Interrupt
			  uart_enable_interrupt(SB_UART, US_IER_TXEMPTY);
			  #endif	
			  
		  }
		}
	}
	
	/* Check the UART Rx data from EDBG UART */
	if(ser_fifo_pull_uint8(&ble_eusart_rx_fifo, (uint8_t *)&t_rx_data) == SER_FIFO_OK)
	{
		LED_Toggle(LED0);
		/* Write to BLE UART Buffer */
		ser_fifo_push_uint8(&ble_usart_tx_fifo, (uint8_t)t_rx_data);
		if (ble_usart_tx_cmpl)
		{
			static uint8_t ble_usart_tx_byte[2] = {0, 0};
			if(ser_fifo_pull_uint8(&ble_usart_tx_fifo, &t_rx_data) == SER_FIFO_OK)
			{
				ble_usart_tx_byte[0] = t_rx_data;
				ble_usart_tx_cmpl = false;
				ble_pdc_send_data(ble_usart_tx_byte, 1);
			}
		}		
	}
}

extern void platfrom_start_rx(void);
void platform_dtm_interface_receive(uint8_t rx_data)
{
	platfrom_start_rx();
	ser_fifo_push_uint8(&ble_usart_rx_fifo, (uint8_t)rx_data);
}

/* EOF */
