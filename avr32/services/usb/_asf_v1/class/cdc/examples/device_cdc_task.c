/**************************************************************************
 *
 * \file
 *
 * \brief Management of the USB device CDC task.
 *
 * This file manages the USB device CDC task.
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


//_____  I N C L U D E S ___________________________________________________

#include <stdio.h>
#include "usart.h"     // Shall be included before FreeRTOS header files, since 'inline' is defined to ''; leading to
// link errors
#include "conf_usb.h"
#include "power_clocks_lib.h"
#include "print_funcs.h"
#include "cdc_example.h"


#if USB_DEVICE_FEATURE == true

#include "board.h"
#ifdef FREERTOS_USED
#include "FreeRTOS.h"
#include "task.h"
#endif
#include "usb_drv.h"
#include "gpio.h"
#include "intc.h"
#if BOARD == EVK1100 || BOARD == EVK1101
# include "joystick.h"
#endif
#include "usb_descriptors.h"
#include "usb_standard_request.h"
#include "device_cdc_task.h"
#include "uart_usb_lib.h"
#include "fifo.h"


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

//_____ D E C L A R A T I O N S ____________________________________________

static volatile uint16_t  sof_cnt;

#define USART_IRQ           DBG_USART_IRQ
#define USART_RX_BUFFER     64 // Unit is in characters.

static fifo_desc_t  fifo_desc_usart;
static uint8_t      fifo_rx[USART_RX_BUFFER];


#if (defined __GNUC__) && (defined __AVR32__)
	__attribute__((__interrupt__))
#elif (defined __ICCAVR32__)
	__interrupt
#endif
static void int_handler_usart(void)
{
	if (DBG_USART->csr & AVR32_USART_RXRDY_MASK) {
		uint8_t i, status;
		i = (DBG_USART->rhr & AVR32_USART_RHR_RXCHR_MASK) >> AVR32_USART_RHR_RXCHR_OFFSET;
		status = fifo_push_uint8(&fifo_desc_usart, i);
		if (status == FIFO_ERROR_OVERFLOW) {
			// error
			gpio_clr_gpio_pin(LED0_GPIO);
			gpio_clr_gpio_pin(LED1_GPIO);
			gpio_clr_gpio_pin(LED2_GPIO);
			gpio_clr_gpio_pin(LED3_GPIO);
		}
	}
}



//!
//! @brief This function initializes the hardware/software resources
//! required for device CDC task.
//!
void device_cdc_task_init(void)
{
	sof_cnt   =0 ;
	uart_usb_init();

	fifo_init(&fifo_desc_usart, &fifo_rx, USART_RX_BUFFER);

	// Register the USART interrupt handler to the interrupt controller.
	// Highest priority is required for the USART, since we do not want to loose
	// any characters.
	Disable_global_interrupt();
	INTC_register_interrupt(&int_handler_usart, USART_IRQ, AVR32_INTC_INT3);
	Enable_global_interrupt();

#ifndef FREERTOS_USED
#if USB_HOST_FEATURE == true
	// If both device and host features are enabled, check if device mode is engaged
	// (accessing the USB registers of a non-engaged mode, even with load operations,
	// may corrupt USB FIFO data).
	if (Is_usb_device())
#endif  // USB_HOST_FEATURE == true
		Usb_enable_sof_interrupt();
#endif  // FREERTOS_USED

#ifdef FREERTOS_USED
	xTaskCreate(device_cdc_task,
			configTSK_USB_DCDC_NAME,
			configTSK_USB_DCDC_STACK_SIZE,
			NULL,
			configTSK_USB_DCDC_PRIORITY,
			NULL);

#endif  // FREERTOS_USED
}


//!
//! @brief Entry point of the device CDC task management
//!
#ifdef FREERTOS_USED
void device_cdc_task(void *pvParameters)
#else
void device_cdc_task(void)
#endif
{
	uint8_t c;
	int status;
	static bool b_startup=true;

#ifdef FREERTOS_USED
	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();

	while (true) {
		vTaskDelayUntil(&xLastWakeTime, configTSK_USB_DCDC_PERIOD);

		// First, check the device enumeration state
		if (!Is_device_enumerated()) continue;
#else
		// First, check the device enumeration state
		if (!Is_device_enumerated()) return;
#endif  // FREERTOS_USED

		if (b_startup) {
			printf("\r\nUSB DEVICE Communications Device Class demo.\r\n");
			b_startup=false;
		}

		if (sof_cnt >= NB_MS_BEFORE_FLUSH) { //Flush buffer in Timeout
			sof_cnt=0;
			uart_usb_flush();
		}

#if BOARD == EVK1100 || BOARD == EVK1101
		if (is_joystick_right())
		       printf("Joystick Right key pressed.\r\n");

		if (is_joystick_left())
		       printf("Joystick Left key pressed.\r\n");

		if (is_joystick_down())
		       printf("Joystick Down key pressed.\r\n");

		if (is_joystick_up())
		       printf("Joystick Up key pressed.\r\n");

		if (is_joystick_pressed())
			printf("Joystick Select key pressed.\r\n");
#endif
#if BOARD == EVK1100 || BOARD == EVK1101 || BOARD == UC3C_EK
		if (!gpio_get_pin_value(GPIO_PUSH_BUTTON_0))
			printf("Button 0 key pressed.\r\n");

		if (!gpio_get_pin_value(GPIO_PUSH_BUTTON_1))
			printf("Button 1 key pressed.\r\n");
#endif

		if (fifo_get_used_size(&fifo_desc_usart) ) { // Something on USART ?
			if (uart_usb_tx_ready() ) { // "USART"-USB free ?
				status = fifo_pull_uint8(&fifo_desc_usart, &c);
				if (status == FIFO_OK) {
					uart_usb_putchar(c);           // Loop back, USART to USB
				}
			}
		}

		if (uart_usb_test_hit()) { // Something received from the USB ?
			if (usart_tx_ready(DBG_USART)) { // USART free ?
				c = uart_usb_getchar();
				usart_write_char(DBG_USART, c);   // loop back USB to USART
			}
		}
#ifdef FREERTOS_USED
	}
#endif
}


//!
//! @brief usb_sof_action
//!
//! This function increments the sof_cnt counter each time
//! the USB Start-of-Frame interrupt subroutine is executed (1 ms).
//! Useful to manage time delays
//!
void usb_sof_action(void)
{
	sof_cnt++;
}

#endif  // USB_DEVICE_FEATURE == true
