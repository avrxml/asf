/**
 * \file
 *
 * \brief Embedded Debugger (EDBG) Virtual COM Port driver for FreeRTOS demo.
 *
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
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

#ifndef CDC_H
#define CDC_H

#include <board.h>
#include <usart_interrupt.h>

/**
 * \defgroup edbg_cdc_rx_group Embedded Debugger (EDBG) Virtual COM Port driver
 *
 * Simple driver for receiving from the EDBG Virtual COM Port via SERCOM USART.
 *
 * This makes use of the \ref asfdoc_sam0_sercom_usart_group, but configures a
 * custom interrupt handler for the SERCOM instead of using the driver's own
 * handler.
 * The purpose of this driver is to avoid the overhead of the SERCOM USART
 * driver's callback functionality, allowing the received data to be handled
 * directly for increased throughput / reduced overhead.
 *
 * Both USART receiver and transmitter are enabled, but only the receive
 * interrupt is enabled since the intended use of the transmitter is to echo
 * back the received characters.
 *
 * The custom interrupt handler can, e.g., be a stripped down version of the
 * SERCOM USART callback driver's interrupt handler
 * (\ref _usart_interrupt_handler()).
 *
 * @{
 */

/**
 * \brief Initialize USART for reception from EDBG Virtual COM Port
 *
 * This function initializes the specified SERCOM USART driver instance for use
 * with the Embedded Debugger (EDBG) Virtual COM Port. Both transmitter and
 * receiver are enabled, but only the receive interrupt is enabled.
 *
 * A custom handler function for the receive interrupt must must be supplied.
 * See the SERCOM USART callback driver's handler for an implementation
 * template: \ref _usart_interrupt_handler().
 *
 * \param usart Address of USART instance to use.
 * \param cdc_rx_handler Address of receive interrupt handler.
 */
static inline void cdc_rx_init(struct usart_module *const usart,
		sercom_handler_t cdc_rx_handler)
{
	struct usart_config config_struct;
	uint8_t instance_index;

	usart_get_config_defaults(&config_struct);

	config_struct.mux_setting = EDBG_CDC_SERCOM_MUX_SETTING;
	config_struct.pinmux_pad0 = EDBG_CDC_SERCOM_PINMUX_PAD0;
	config_struct.pinmux_pad1 = EDBG_CDC_SERCOM_PINMUX_PAD1;
	config_struct.pinmux_pad2 = EDBG_CDC_SERCOM_PINMUX_PAD2;
	config_struct.pinmux_pad3 = EDBG_CDC_SERCOM_PINMUX_PAD3;

	while (usart_init(usart, EDBG_CDC_MODULE, &config_struct) != STATUS_OK) {
		// Intentionally left empty
	}

	// Inject our own interrupt handler
	instance_index = _sercom_get_sercom_inst_index(EDBG_CDC_MODULE);
	_sercom_set_handler(instance_index, cdc_rx_handler);

	// Enable the UART transceiver
	usart_enable(usart);
	usart_enable_transceiver(usart, USART_TRANSCEIVER_TX);
	usart_enable_transceiver(usart, USART_TRANSCEIVER_RX);

	// ..and the RX Complete interrupt
	((SercomUsart *)EDBG_CDC_MODULE)->INTENSET.reg = SERCOM_USART_INTFLAG_RXC;
}

/** @} */

#endif // CDC_H
