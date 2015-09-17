/*****************************************************************************
 *
 * \file
 *
 * \brief Touch button example for QT1081.
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
 *****************************************************************************/

/*! \mainpage
 * \section intro Introduction
 * This is the documentation for the QT1081 driver.
 *
 * This example demonstrates how to use the QT1081 interface.
 *
 * \section files Main Files
 * - qt1081_example.c: the QT1081 example
 * - qt1081.h: the driver interface for the QT1081
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC for AVR32 and for IAR Embedded Workbench
 * for Atmel AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Information
 * All AVR32 devices with a GPIO can be used.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - EVK1105
 * - CPU clock: 12 MHz;
 * - USART abstracted with a USB CDC connection to a PC;port via a standard RS232 DB9 cable;
 * - PC terminal settings:
 *   - 57600 bps,
 *   - 8 data bits,
 *   - no parity bit,
 *   - 1 stop bit,
 *   - no flow control.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


// Include Files
#include <avr32/io.h>
#include "compiler.h"
#include "board.h"
#include "pm.h"
#include "gpio.h"
#include "eic.h"
#include "qt1081.h"

#define INT_MODE_EIC	0
#define INT_MODE_GPIO	1
/* Select either interrupt mode with a detect signal that is connected to the EIC controller or
 * GPIO interrupt mode.*/
#define INT_MODE	INT_MODE_EIC

#if __GNUC__
#include "intc.h"
#endif

#define is_touch_up() 		is_touch_sensor_0()
#define is_touch_down()		is_touch_sensor_1()
#define is_touch_right()	is_touch_sensor_2()
#define is_touch_left()		is_touch_sensor_3()
#define is_touch_center()	is_touch_sensor_4()

//! Structure holding the configuration parameters of the EIC module.
eic_options_t eic_options[2];

#if __GNUC__
__attribute__ ((__interrupt__))
#elif __ICCAVR32__
#pragma handler = AVR32_EIC_IRQ_GROUP, 1
__interrupt
#endif
#if(INT_MODE == INT_MODE_GPIO)
void touch_button_isr(void){
	// UP
	if(gpio_get_pin_interrupt_flag(QT1081_TOUCH_SENSOR_0))
	{
		gpio_clear_pin_interrupt_flag(QT1081_TOUCH_SENSOR_0);
		gpio_tgl_gpio_pin(LED0_GPIO);
	}
	// DOWN
	if(gpio_get_pin_interrupt_flag(QT1081_TOUCH_SENSOR_1))
	{
		gpio_clear_pin_interrupt_flag(QT1081_TOUCH_SENSOR_1);
		gpio_tgl_gpio_pin(LED1_GPIO);
	}
	// Right
	if(gpio_get_pin_interrupt_flag(QT1081_TOUCH_SENSOR_2))
	{
		gpio_clear_pin_interrupt_flag(QT1081_TOUCH_SENSOR_2);
		gpio_tgl_gpio_pin(LED2_GPIO);
	}
	// Left
	if(gpio_get_pin_interrupt_flag(QT1081_TOUCH_SENSOR_3))
	{
		gpio_clear_pin_interrupt_flag(QT1081_TOUCH_SENSOR_3);
		gpio_tgl_gpio_pin(LED3_GPIO);
	}
	// Push
	if(gpio_get_pin_interrupt_flag(QT1081_TOUCH_SENSOR_4))
	{
		gpio_clear_pin_interrupt_flag(QT1081_TOUCH_SENSOR_4);
		gpio_tgl_gpio_pin(LED0_GPIO);
	}

}
#endif
#if( INT_MODE == INT_MODE_EIC)
void touch_button_isr(void){

	if(is_touch_up()){
		gpio_tgl_gpio_pin(LED0_GPIO);
	}
	if(is_touch_down()){
		gpio_tgl_gpio_pin(LED1_GPIO);
	}
	if(is_touch_right()){
		gpio_tgl_gpio_pin(LED1_GPIO);
	}
	if(is_touch_left()){
		gpio_tgl_gpio_pin(LED2_GPIO);
	}
	if(is_touch_center()){
		gpio_tgl_gpio_pin(LED2_GPIO);
	}

	eic_clear_interrupt_line(&AVR32_EIC, QT1081_EIC_EXTINT_INT);
}
#endif

// main function
int main(void) {

	pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);

	// Enable edge-triggered interrupt.
	eic_options[0].eic_mode  = EIC_MODE_EDGE_TRIGGERED;
	// Interrupt will trigger on falling edge (this is a must-do for the keypad scan
	// feature if the chosen mode is edge-triggered).
	eic_options[0].eic_edge  = EIC_EDGE_RISING_EDGE;
	// Initialize in synchronous mode : interrupt is synchronized to the clock
	eic_options[0].eic_async = EIC_SYNCH_MODE;
	// Set the interrupt line number.
	eic_options[0].eic_line  = QT1081_EIC_EXTINT_INT;

	// Activate LED0 & LED1 & LED2 & LED3 pins in GPIO output mode and switch them off.
	gpio_set_gpio_pin(LED0_GPIO);
	gpio_set_gpio_pin(LED1_GPIO);
	gpio_set_gpio_pin(LED2_GPIO);
	gpio_set_gpio_pin(LED3_GPIO);

	gpio_enable_module_pin( QT1081_EIC_EXTINT_PIN, QT1081_EIC_EXTINT_FUNCTION);

#if( INT_MODE == INT_MODE_GPIO)
	Disable_global_interrupt();

#if __GNUC__
	INTC_init_interrupts();
    /* Register interrupt handler to the interrupt controller
     * up, down buttons on PB22, PB23 -> GPIO_IRQ_6
     */
	INTC_register_interrupt(&touch_button_isr, AVR32_GPIO_IRQ_6, 0);//AVR32_INTC_INT0);
	/* Other buttons on PB[24..26] -> GPIO_IRQ_7 (PB23 - PB31) */
	INTC_register_interrupt(&touch_button_isr, AVR32_GPIO_IRQ_7, 0);
#endif
	gpio_enable_pin_interrupt(QT1081_TOUCH_SENSOR_0, GPIO_RISING_EDGE);
	gpio_enable_pin_interrupt(QT1081_TOUCH_SENSOR_1, GPIO_RISING_EDGE);
	gpio_enable_pin_interrupt(QT1081_TOUCH_SENSOR_2, GPIO_RISING_EDGE);
	gpio_enable_pin_interrupt(QT1081_TOUCH_SENSOR_3, GPIO_RISING_EDGE);
	gpio_enable_pin_interrupt(QT1081_TOUCH_SENSOR_4, GPIO_RISING_EDGE);

	Enable_global_interrupt();
#endif

#if(INT_MODE == INT_MODE_EIC)
	Disable_global_interrupt();

#if __GNUC__
	INTC_init_interrupts();
    /* Register the EXTINT1 interrupt handler to the interrupt controller
     */
	INTC_register_interrupt(&touch_button_isr, QT1081_EIC_EXTINT_IRQ, AVR32_INTC_INT0);
#endif

	  // Init the EIC controller with the options
	  eic_init(&AVR32_EIC, eic_options, 1);
	  // Enable the EIC lines.
	  eic_enable_lines(&AVR32_EIC, (1<<eic_options[0].eic_line));
	  // Enable the interrupt for each EIC line.
	  eic_enable_interrupt_lines(&AVR32_EIC, (1<<eic_options[0].eic_line));

	Enable_global_interrupt();
#endif

	while(true);

	return 0;
}
