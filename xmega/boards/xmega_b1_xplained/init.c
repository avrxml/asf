/**
 *
 * \file
 *
 * \brief XMEGA B1 Xplained board init.
 *
 * This file contains board initialization function.
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include "board.h"
#include "ioport.h"

#ifdef CONF_BOARD_LCD_BACKLIGHT_PWM
#include "tc.h"
#endif


void board_init(void)
{
	ioport_configure_pin(LED0_GPIO,
			IOPORT_DIR_OUTPUT|IOPORT_INIT_LOW|IOPORT_INV_ENABLED);
	ioport_configure_pin(LED1_GPIO,
			IOPORT_DIR_OUTPUT|IOPORT_INIT_LOW|IOPORT_INV_ENABLED);
	ioport_configure_pin(LED2_GPIO,
			IOPORT_DIR_OUTPUT|IOPORT_INIT_LOW|IOPORT_INV_ENABLED);
	ioport_configure_pin(LED3_GPIO,
			IOPORT_DIR_OUTPUT|IOPORT_INIT_LOW|IOPORT_INV_ENABLED);

	ioport_configure_pin(BACKLIGHT_GPIO, IOPORT_DIR_OUTPUT|IOPORT_INIT_LOW);

	ioport_configure_pin(LED_POWER, IOPORT_DIR_OUTPUT|IOPORT_INIT_HIGH);


	ioport_configure_pin(GPIO_PUSH_BUTTON_0, IOPORT_DIR_INPUT |
			IOPORT_PULL_UP);
	ioport_configure_pin(GPIO_PUSH_BUTTON_1, IOPORT_DIR_INPUT |
			IOPORT_PULL_UP);
	ioport_configure_pin(GPIO_PUSH_BUTTON_2, IOPORT_DIR_INPUT |
			IOPORT_PULL_UP);
	ioport_configure_pin(GPIO_PUSH_BUTTON_3, IOPORT_DIR_INPUT |
			IOPORT_PULL_UP);

#ifdef CONF_BOARD_AT45DBX
	ioport_configure_pin(AT45DBX_MASTER_SCK, IOPORT_DIR_OUTPUT |
			IOPORT_INIT_HIGH);
	ioport_configure_pin(AT45DBX_MASTER_MOSI, IOPORT_DIR_OUTPUT |
			IOPORT_INIT_HIGH);
	ioport_configure_pin(AT45DBX_MASTER_MISO, IOPORT_DIR_INPUT);
	ioport_configure_pin(AT45DBX_CS, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	// Setting PORT_USART0_bm to one will move the pin location
	// of USARTC0 (in mode SPI) from Px[3:0] to Px[7:4].
	PORTC.REMAP |= PORT_USART0_bm;
#endif

#ifdef CONF_BOARD_LIGHT_SENSOR
	ioport_configure_pin(LIGHT_SENSOR_SIGNAL_PIN, IOPORT_DIR_INPUT
			| IOPORT_INPUT_DISABLE);
#endif

#ifdef CONF_BOARD_TEMPERATURE_SENSOR
	ioport_configure_pin(TEMPERATURE_SENSOR_SIGNAL_PIN, IOPORT_DIR_INPUT
			| IOPORT_INPUT_DISABLE);
#endif

#ifdef CONF_BOARD_POTENTIOMETER_SENSOR
	ioport_configure_pin(POTENTIOMETER_SIGNAL_PIN, IOPORT_DIR_INPUT
			| IOPORT_INPUT_DISABLE);
#endif

#ifdef CONF_BOARD_ENABLE_MXT143E_XPLAINED
	ioport_configure_pin(MXT143E_XPLAINED_SCK, IOPORT_DIR_OUTPUT
			| IOPORT_INIT_HIGH);
	ioport_configure_pin(MXT143E_XPLAINED_MOSI, IOPORT_DIR_OUTPUT
			| IOPORT_INIT_HIGH);
	ioport_configure_pin(MXT143E_XPLAINED_MISO, IOPORT_DIR_INPUT);
	ioport_configure_pin(MXT143E_XPLAINED_CS, IOPORT_DIR_OUTPUT
			| IOPORT_INIT_HIGH);
	ioport_configure_pin(MXT143E_XPLAINED_CHG, IOPORT_DIR_INPUT);
	ioport_configure_pin(MXT143E_XPLAINED_DC, IOPORT_DIR_OUTPUT
			| IOPORT_INIT_LOW);
	#ifndef MXT143E_XPLAINED_BACKLIGHT_DISABLE
	ioport_configure_pin(MXT143E_XPLAINED_BACKLIGHT, IOPORT_DIR_OUTPUT
			| IOPORT_INIT_LOW);
	#endif
	ioport_configure_pin(MXT143E_XPLAINED_LCD_RESET, IOPORT_DIR_OUTPUT
			| IOPORT_INIT_LOW);
	// Setting PORT_USART0_bm to one will move the pin location
	// of USARTC0 (in mode SPI) from Px[3:0] to Px[7:4].
	PORTC.REMAP |= PORT_USART0_bm;
#endif

#ifdef CONF_BOARD_LCD_BACKLIGHT_PWM
	backlight_start_pwm();
#endif
#ifdef CONF_BOARD_ENABLE_USARTC0
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 3), IOPORT_DIR_OUTPUT
			| IOPORT_INIT_HIGH);
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 2), IOPORT_DIR_INPUT);
#endif

#ifdef CONF_BOARD_ENABLE_USARTE0
	/* Note: this will disable touch buttons 2 and 3 as they share the same
	 * physical pins at the USART E module. */
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTE, 3), IOPORT_DIR_OUTPUT
			| IOPORT_INIT_HIGH);
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTE, 2), IOPORT_DIR_INPUT);
#endif

#if defined (SENSORS_XPLAINED_BOARD)
	/* Configure the Xplained Sensor extension board, if any, after
	 * the platform Xplained board has configured basic clock settings,
	 * GPIO pin mapping, interrupt controller options, etc.
	 */
	sensor_board_init ();
#endif
}


#ifdef CONF_BOARD_LCD_BACKLIGHT_PWM
void backlight_set_pwm(uint16_t pwm)
{
	tc_write_cc(&BACKLIGHT_TIMER, TC_CCB, pwm);
}

uint16_t backlight_get_pwm()
{
	return tc_read_cc(&BACKLIGHT_TIMER, TC_CCB);
}

void backlight_stop_pwm()
{
	tc_disable_cc_channels(&BACKLIGHT_TIMER,TC_CCBEN);
	backlight_off();
	tc_disable(&BACKLIGHT_TIMER);
}

void backlight_start_pwm()
{
	// Backlight PWM init
	// Unmask clock for TIMER_EXAMPLE
	tc_enable(&BACKLIGHT_TIMER);
	// Configure TC in PWM Single Slope PWM
	BACKLIGHTPORT.REMAP |= PORT_TC0B_bm;
	tc_set_wgm(&BACKLIGHT_TIMER, TC_WG_SS);
	backlight_set_pwm(0);
	tc_enable_cc_channels(&BACKLIGHT_TIMER,TC_CCBEN);
	// Run TC at 2MHz clock resolution
	tc_set_resolution(&BACKLIGHT_TIMER, BACKLIGHT_TIMER_FREQUENCY);
	backlight_set_pwm(BACKLIGHT_TIMER_COUNT);
}

#endif
