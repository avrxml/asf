/**
 * \file serial_drv.c
 *
 * \brief Handles Serial driver functionalities
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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
#include "serial_drv.h"
#include "conf_serialdrv.h"

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

uint8_t configure_serial_drv(void)
{
	sam_usart_opt_t usart_settings;

	/* Configure console UART. */
	sysclk_enable_peripheral_clock(BLE_UART_ID);

	usart_settings.baudrate = CONF_UART_BAUDRATE;
	usart_settings.char_length = CONF_UART_CHAR_LENGTH;
	usart_settings.parity_type = CONF_UART_PARITY;
	usart_settings.stop_bits= CONF_UART_STOP_BITS;
	usart_settings.channel_mode= US_MR_CHMODE_NORMAL;

	ioport_set_pin_peripheral_mode(EXT1_PIN_UART_RX,
	IOPORT_MODE_MUX_A);
	ioport_set_pin_peripheral_mode(EXT1_PIN_UART_TX,
	IOPORT_MODE_MUX_A);

	flexcom_enable(BLE_USART_FLEXCOM);
	flexcom_set_opmode(BLE_USART_FLEXCOM, FLEXCOM_USART);

	/* Configure USART */
	usart_init_rs232(BLE_UART, &usart_settings,
				 sysclk_get_peripheral_hz());
				 
	/* Enable the receiver and transmitter. */
	usart_enable_tx(BLE_UART);
	usart_enable_rx(BLE_UART);	

	/* Enable UART IRQ */
	usart_enable_interrupt(BLE_UART, US_IER_RXRDY);
	//usart_enable_interrupt(BLE_UART, US_IER_ENDRX);
	//usart_enable_interrupt(BLE_UART, US_IER_RXBUFF);

	/* Enable UART interrupt */
	NVIC_EnableIRQ(BLE_UART_IRQn);

	return STATUS_OK;
}

void BLE_UART_Handler(void)
{
	uint32_t status_isr;
	status_isr = usart_get_status(BLE_UART);
	
	if ((status_isr & US_CSR_RXRDY))
	{
		#if SERIAL_DRV_RX_CB_ENABLE == true
			SERIAL_DRV_RX_CB();
		#endif
	}
}

uint16_t serial_drv_send(uint8_t* data, uint16_t len)
{
 uint32_t temp, i;
 for (i =0; i < len; i++)
 {
	temp = *data++;
	usart_putchar(BLE_UART, temp); 
 }
 while(usart_is_tx_ready(BLE_UART) == 0);
 return STATUS_OK;
}

uint8_t serial_read_data(uint8_t* data, uint16_t max_len)
{
	uint32_t i;
	uint16_t uart_rx_data;
	for (i = 0; i < max_len; ++i)
	{
		if(serial_read_byte(&uart_rx_data) == STATUS_OK)
		{
			data[i] = (uint8_t)uart_rx_data;
		}
		else
		{
			return i;
		}
	}
 return max_len;
}

uint8_t serial_read_byte(uint16_t* data)
{
   uint32_t temp;
   if (usart_read(BLE_UART, &temp) == 0)
   {
	   *data = (uint8_t)temp;
	   return STATUS_OK;
   }
   else
   {	  
	   return STATUS_ERR_BUSY;
   }
}


/* EOF */
