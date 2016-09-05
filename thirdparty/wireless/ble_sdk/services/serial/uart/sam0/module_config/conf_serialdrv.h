/**
 * \file
 *
 * \brief SAM D21/L21 serial driver configuration.
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
 *
 */

#ifndef CONF_SERIALDRV_H_INCLUDED
#define CONF_SERIALDRV_H_INCLUDED


/* BLE UART Configuration details */
#define CONF_BLE_USART_MODULE		EXT1_UART_MODULE
#define CONF_BLE_MUX_SETTING		EXT1_UART_SERCOM_MUX_SETTING
#define CONF_BLE_PINMUX_PAD0		EXT1_UART_SERCOM_PINMUX_PAD0
#define CONF_BLE_PINMUX_PAD1		EXT1_UART_SERCOM_PINMUX_PAD1
#define CONF_BLE_PINMUX_PAD2		EXT1_UART_SERCOM_PINMUX_PAD2
#define CONF_BLE_PINMUX_PAD3		EXT1_UART_SERCOM_PINMUX_PAD3
#define CONF_BLE_BAUDRATE			115200
#define CONF_BLE_UART_CLOCK			GCLK_GENERATOR_0

#define CONF_FLCR_BLE_USART_MODULE  SERCOM0
#define CONF_FLCR_BLE_MUX_SETTING   USART_RX_1_TX_0_RTS_2_CTS_3
#define CONF_FLCR_BLE_PINMUX_PAD0   PINMUX_PA04D_SERCOM0_PAD0
#define CONF_FLCR_BLE_PINMUX_PAD1   PINMUX_PA05D_SERCOM0_PAD1
#define CONF_FLCR_BLE_PINMUX_PAD2   PINMUX_PA06D_SERCOM0_PAD2
#define CONF_FLCR_BLE_PINMUX_PAD3   PINMUX_PA07D_SERCOM0_PAD3
#define CONF_FLCR_BLE_BAUDRATE      115200
#define CONF_FLCR_BLE_UART_CLOCK	GCLK_GENERATOR_0

/* BTLC1000 Wakeup Pin */
#define BTLC1000_WAKEUP_PIN			(EXT1_PIN_6)
/* BTLC1000 Chip Enable Pin */
#define BTLC1000_CHIP_ENABLE_PIN	(EXT1_PIN_4)

#define BLE_UART_RTS_PIN			(EXT1_PIN_16)
#define BLE_UART_CTS_PIN			(EXT1_PIN_18)

/* BTLC1000 50ms Reset Duration */
#define BTLC1000_RESET_MS			(50)

/* set port pin high */
#define IOPORT_PIN_LEVEL_HIGH		(true)
/* Set port pin low */
#define IOPORT_PIN_LEVEL_LOW		(false)

void serial_rx_callback(void);
void serial_tx_callback(void);

#define SERIAL_DRV_RX_CB serial_rx_callback
#define SERIAL_DRV_TX_CB serial_tx_callback
#define SERIAL_DRV_TX_CB_ENABLE  true
#define SERIAL_DRV_RX_CB_ENABLE  true

#define BLE_MAX_TX_PAYLOAD_SIZE 512
#define BLE_MAX_RX_PAYLOAD_SIZE 512


/* Set BLE Wakeup pin to be low */
static inline bool ble_wakeup_pin_level(void)
{
	return (port_pin_get_output_level(BTLC1000_WAKEUP_PIN));
}

/* Set BLE Wakeup pin to be low */
static inline void ble_wakeup_pin_set_low(void)
{
	port_pin_set_output_level(BTLC1000_WAKEUP_PIN, 
							IOPORT_PIN_LEVEL_LOW);
}

/* Set wakeup pin to high */
static inline void ble_wakeup_pin_set_high(void)
{
	port_pin_set_output_level(BTLC1000_WAKEUP_PIN, 
							IOPORT_PIN_LEVEL_HIGH);
}

/* Set enable pin to Low */
static inline void ble_enable_pin_set_low(void)
{
	port_pin_set_output_level(BTLC1000_CHIP_ENABLE_PIN, 
							IOPORT_PIN_LEVEL_LOW);
}

/* Set enable pin to high */
static inline void ble_enable_pin_set_high(void)
{
	port_pin_set_output_level(BTLC1000_CHIP_ENABLE_PIN, 
							IOPORT_PIN_LEVEL_HIGH);
}

/* Configure the BTLC1000 control(chip_enable, wakeup) pins */
static inline void ble_configure_control_pin(void)
{
	struct port_config pin_conf;
	
	/* initialize the delay before use */
	delay_init();
	
	/* get the default values for port pin configuration */
	port_get_config_defaults(&pin_conf);

	/* Configure control pins as output */
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	pin_conf.input_pull = PORT_PIN_PULL_DOWN;
	
	port_pin_set_config(BTLC1000_WAKEUP_PIN, &pin_conf);
	/* set wakeup pin to low */
	ble_wakeup_pin_set_high();
	
	port_pin_set_config(BTLC1000_CHIP_ENABLE_PIN, &pin_conf);
	/* set chip enable to low */
	ble_enable_pin_set_low();
	
	/* Delay for 50ms */
	delay_ms(BTLC1000_RESET_MS);
	
	/* set chip enable to high */
	ble_enable_pin_set_high();
	
}

static inline void ble_reset(void)
{
	/* BTLC1000 Reset Sequence @Todo */
	ble_enable_pin_set_high();
	ble_wakeup_pin_set_high();
	delay_ms(BTLC1000_RESET_MS);
	
	ble_enable_pin_set_low();
	ble_wakeup_pin_set_low();
	delay_ms(BTLC1000_RESET_MS);
	
	ble_enable_pin_set_high();
	ble_wakeup_pin_set_high();
	delay_ms(BTLC1000_RESET_MS);
}

#endif /* CONF_SERIALDRV_H_INCLUDED */
