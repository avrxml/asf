/**
 * \file
 *
 * \brief SAM L21 serial driver configuration.
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
#define CONF_UART_BAUDRATE			115200
#define CONF_BLE_UART_CLOCK			GCLK_GENERATOR_0

#define CONF_FLCR_BLE_USART_MODULE  SERCOM0
#define CONF_FLCR_BLE_MUX_SETTING   USART_RX_1_TX_0_RTS_2_CTS_3
#define CONF_FLCR_BLE_PINMUX_PAD0   PINMUX_PA04D_SERCOM0_PAD0
#define CONF_FLCR_BLE_PINMUX_PAD1   PINMUX_PA05D_SERCOM0_PAD1
#define CONF_FLCR_BLE_PINMUX_PAD2   PINMUX_PA06D_SERCOM0_PAD2
#define CONF_FLCR_BLE_PINMUX_PAD3   PINMUX_PA07D_SERCOM0_PAD3
#define CONF_FLCR_BLE_BAUDRATE      115200
#define CONF_FLCR_BLE_UART_CLOCK	GCLK_GENERATOR_0

#if (BLE_MODULE == BTLC1000_ZR)
	/* BTLC1000 Wakeup Pin */
	#define BTLC1000_WAKEUP_PIN			(EXT1_PIN_3)
	/* BTLC1000 Chip Enable Pin */
	#define BTLC1000_CHIP_ENABLE_PIN	(EXT1_PIN_7)
 
#elif (BLE_MODULE == BTLC1000_MR)
	/* BTLC1000 Wakeup Pin */
	#define BTLC1000_WAKEUP_PIN			(EXT1_PIN_6)
	/* BTLC1000 Chip Enable Pin */
	#define BTLC1000_CHIP_ENABLE_PIN	(EXT1_PIN_4)
#endif

#define BLE_UART_RTS_PIN			(EXT1_PIN_16)
#define BLE_UART_CTS_PIN			(EXT1_PIN_18)

/* set port pin high */
#define IOPORT_PIN_LEVEL_HIGH		(true)
/* Set port pin low */
#define IOPORT_PIN_LEVEL_LOW		(false)

#define BLE_MAX_RX_PAYLOAD_SIZE 1024
#define BLE_MAX_TX_PAYLOAD_SIZE 1024

#define BTLC1000_HOST_WAKEUP_PIN					EXT1_PIN_9
#define BTLC1000_HOST_WAKEUP_EIC_PIN				PIN_PB04A_EIC_EXTINT4
#define BTLC1000_HOST_WAKEUP_EIC_MUX				MUX_PB04A_EIC_EXTINT4
#define BTLC1000_HOST_WAKEUP_EIC_LINE				4

#define BTLC1000_UART_CTS_PIN						(EXT1_PIN_16)

#define HOST_SYSTEM_SLEEP_MODE						SYSTEM_SLEEPMODE_IDLE

void platform_host_wake_interrupt_handler(void);
static inline void btlc1000_host_wakeup_config(void);
static inline void btlc1000_host_wakeup_handler(void);

/* BTLC1000 Host Wakeup Initialization */
static inline void btlc1000_host_wakeup_config(void)
{
	struct extint_chan_conf eint_chan_conf;
	extint_chan_get_config_defaults(&eint_chan_conf);

	eint_chan_conf.gpio_pin           = BTLC1000_HOST_WAKEUP_EIC_PIN;
	eint_chan_conf.gpio_pin_pull      = EXTINT_PULL_UP;
	eint_chan_conf.gpio_pin_mux       = BTLC1000_HOST_WAKEUP_EIC_MUX;
	eint_chan_conf.detection_criteria = EXTINT_DETECT_FALLING;
	eint_chan_conf.filter_input_signal = true;	
	eint_chan_conf.enable_async_edge_detection = true;
	
	extint_chan_set_config(BTLC1000_HOST_WAKEUP_EIC_LINE, &eint_chan_conf);
	
	extint_register_callback(btlc1000_host_wakeup_handler,
	BTLC1000_HOST_WAKEUP_EIC_LINE,
	EXTINT_CALLBACK_TYPE_DETECT);
	
	extint_chan_enable_callback(BTLC1000_HOST_WAKEUP_EIC_LINE,
	EXTINT_CALLBACK_TYPE_DETECT);
	
}
		
static inline void btlc1000_host_wakeup_handler(void)
{
	platform_host_wake_interrupt_handler();
}

static inline bool host_event_data_ready_pin_level(void)
{
	return (port_pin_get_input_level(BTLC1000_HOST_WAKEUP_PIN));
}

static inline bool btlc1000_cts_pin_level(void)
{
	return (port_pin_get_output_level(BTLC1000_UART_CTS_PIN));
}

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
	port_pin_set_config(BTLC1000_CHIP_ENABLE_PIN, &pin_conf);	
}

#endif /* CONF_SERIALDRV_H_INCLUDED */
