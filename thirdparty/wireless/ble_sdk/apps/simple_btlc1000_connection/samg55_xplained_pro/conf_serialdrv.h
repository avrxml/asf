/**
 * \file
 *
 * \brief SAM G55 serial driver configuration.
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
 *
 */

#ifndef CONF_SERIALDRV_H_INCLUDED
#define CONF_SERIALDRV_H_INCLUDED

/** UART Interface */
#define BLE_UART            EXT1_UART_MODULE
#define BLE_UART_ID		    ID_FLEXCOM0
#define BLE_USART_FLEXCOM   FLEXCOM0
#define BLE_UART_IRQn		FLEXCOM0_IRQn
/* Configuration for console uart IRQ handler */
#define BLE_UART_Handler    FLEXCOM0_Handler

/** Baudrate setting */
#define CONF_UART_BAUDRATE   (115200UL)
/** Character length setting */
#define CONF_UART_CHAR_LENGTH  US_MR_CHRL_8_BIT
/** Parity setting */
#define CONF_UART_PARITY     US_MR_PAR_NO
/** Stop bits setting */
#define CONF_UART_STOP_BITS    US_MR_NBSTOP_1_BIT

void serial_rx_callback(void);
void serial_tx_callback(void);

#define SERIAL_DRV_RX_CB serial_rx_callback
#define SERIAL_DRV_TX_CB serial_tx_callback
#define SERIAL_DRV_TX_CB_ENABLE  true
#define SERIAL_DRV_RX_CB_ENABLE  true

static inline void ble_enable_pin_init(void)
{
	ioport_init();

	ioport_set_pin_dir(EXT1_PIN_10, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(EXT1_PIN_10, IOPORT_PIN_LEVEL_HIGH);
	ioport_set_pin_dir(EXT1_PIN_6, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(EXT1_PIN_6, IOPORT_PIN_LEVEL_HIGH);
}

static inline void ble_enable_pin_set_low(void)
{
	//ioport_set_pin_level(EXT1_PIN_5, IOPORT_PIN_LEVEL_LOW);
}

static inline void ble_enable_pin_set_high(void)
{
	//ioport_set_pin_level(EXT1_PIN_5, IOPORT_PIN_LEVEL_HIGH);
}


#endif /* CONF_SERIALDRV_H_INCLUDED */
