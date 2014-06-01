/**
 * \file
 *
 * \brief SAM D20/D21/R21 LED Toggle Example
 *
 * Copyright (C) 2012-2014 Atmel Corporation. All rights reserved.
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

/**
 * \mainpage SAM D20/D21/R21 LED Toggle Example
 * See \ref appdoc_main "here" for project documentation.
 * \copydetails appdoc_preface
 *
 *
 * \page appdoc_preface Overview
 * This application demonstrates a simple example to turn on the board LED when
 * a button is pressed, using a variety of methods and modules within the device.
 */

/**
 * \page appdoc_main SAM D20/D21/R21 LED Toggle Example
 *
 * Overview:
 * - \ref appdoc_sam0_led_toggle_app_intro
 * - \ref appdoc_sam0_led_toggle_app_usage
 * - \ref appdoc_sam0_led_toggle_app_config
 * - \ref appdoc_sam0_led_toggle_app_compinfo
 * - \ref appdoc_sam0_led_toggle_app_contactinfo
 *
 * \section appdoc_sam0_led_toggle_app_intro Introduction
 * This application demonstrates a simple example to turn on the board LED when
 * a button is pressed, using a variety of methods and modules within the device.
 *
 * \section appdoc_sam0_led_toggle_app_usage Usage
 * When run, press the board button to turn on the board LED, release to turn
 * the LED off. If the application settings are altered, the application must be
 * recompiled and re-run on the device.
 *
 * \section appdoc_sam0_led_toggle_app_config Configuration
 * The table \ref appdoc_sam0_led_toggle_app_conftable "below" shows the
 * possible configurations of this example.
 *
 * \anchor appdoc_sam0_led_toggle_app_conftable
 * <table>
 *  <caption>Example Configurations</caption>
 * 	<tr>
 * 		<th>USE_INTERRUPTS</th>
 * 		<th>USE_EIC</th>
 * 		<th>Result</th>
 * 	</tr>
 * 	<tr>
 * 		<td>false</td>
 * 		<td>false</td>
 * 		<td>Polled via PORT driver</td>
 * 	</tr>
 * 	<tr>
 * 		<td>false</td>
 * 		<td>true</td>
 * 		<td>Polled via EIC driver</td>
 * 	</tr>
 * 	<tr>
 * 		<td>true</td>
 * 		<td>false</td>
 * 		<td>Asynchronous using SysTick handler</td>
 * 	</tr>
 * 	<tr>
 * 		<td>true</td>
 * 		<td>true</td>
 * 		<td>Asynchronous using EIC handler</td>
 * 	</tr>
 * </table>
 *
 * \section appdoc_sam0_led_toggle_app_compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for ARM.
 * Other compilers may or may not work.
 *
 * \section appdoc_sam0_led_toggle_app_contactinfo Contact Information
 * For further information, visit
 * <a href="http://www.atmel.com">http://www.atmel.com</a>.
 */

#include <asf.h>

/** If \true, interrupts are used to alter the board state, when \false polling
 *  is used.
 */
#define USE_INTERRUPTS   true

/** If \true, the External Interrupt Controller module is used to check when the
 *   button state changes, when \false the PORT module is used.
 */
#define USE_EIC          true


/** Updates the board LED to the current button state. */
static void update_led_state(void)
{
	bool pin_state = port_pin_get_input_level(BUTTON_0_PIN);
	port_pin_set_output_level(LED_0_PIN, pin_state);
}

#if USE_INTERRUPTS == true
#  if USE_EIC == true
/** Callback function for the EXTINT driver, called when an external interrupt
 *  detection occurs.
 */
static void extint_callback(void)
{
	update_led_state();
}

/** Configures and registers the External Interrupt callback function with the
 *  driver.
 */
static void configure_eic_callback(void)
{
	extint_register_callback(extint_callback,
			BUTTON_0_EIC_LINE,
			EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_enable_callback(BUTTON_0_EIC_LINE,
			EXTINT_CALLBACK_TYPE_DETECT);
}
#  else
/** Handler for the device SysTick module, called when the SysTick counter
 *  reaches the set period.
 *
 *  \note As this is a raw device interrupt, the function name is significant
 *        and must not be altered to ensure it is hooked into the device's
 *        vector table.
 */
void SysTick_Handler(void)
{
	update_led_state();
}

/** Configures the SysTick module to fire a SysTick interrupt every 999 system
 *  clock source cycles.
 */
static void configure_systick_handler(void)
{
	SysTick->CTRL = 0;
	SysTick->LOAD = 999;
	SysTick->VAL  = 0;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}
#  endif
#endif

#if USE_EIC == true
/** Configures the External Interrupt Controller to detect changes in the board
 *  button state.
 */
static void configure_extint(void)
{
	struct extint_chan_conf eint_chan_conf;
	extint_chan_get_config_defaults(&eint_chan_conf);

	eint_chan_conf.gpio_pin           = BUTTON_0_EIC_PIN;
	eint_chan_conf.gpio_pin_mux       = BUTTON_0_EIC_MUX;
	eint_chan_conf.detection_criteria = EXTINT_DETECT_BOTH;
	eint_chan_conf.filter_input_signal = true;
	extint_chan_set_config(BUTTON_0_EIC_LINE, &eint_chan_conf);
}
#endif

int main(void)
{
	system_init();

#if USE_EIC == true
	configure_extint();
#endif

#if USE_INTERRUPTS == true
#  if USE_EIC == false
	configure_systick_handler();
#  else
	configure_eic_callback();
#  endif

	system_interrupt_enable_global();

	while (true) {
		/* Do nothing - use interrupts */
	}
#else
#  if USE_EIC == false
	while (true) {
		update_led_state();
	}
#  else
	while (true) {
		if (extint_chan_is_detected(BUTTON_0_EIC_LINE)) {
			extint_chan_clear_detected(BUTTON_0_EIC_LINE);

			update_led_state();
		}
	}
#  endif
#endif
}
