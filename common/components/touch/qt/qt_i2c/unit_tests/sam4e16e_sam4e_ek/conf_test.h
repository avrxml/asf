/**
 * \file
 *
 * \brief Unit test configuration.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef CONF_TEST_H_INCLUDED
#define CONF_TEST_H_INCLUDED

/** USART Interface */
#define CONF_TEST_USART      CONSOLE_UART
/** Baudrate setting */
#define CONF_TEST_BAUDRATE   115200
/** Parity setting */
#define CONF_TEST_PARITY     UART_MR_PAR_NO

/**
 * \brief Set QTouch parameter for the unit test.
 *
 * \param setup_block Pointer to setup block buffer.
 */
static void ut_set_qt_param(struct qt_setup_block *setup_block)
{
	/*
	 * Set parameter for QT slider.
	 */
	setup_block->slider_num_keys = 7; /* X0~X6 are for slider, total 7  */
	setup_block->slider_resolution = QT_SLIDER_RESOLUTION_7_BIT;

	/*
	 * Set all GPIO as output with low level.
	 */
	setup_block->gpio_direction = 0x1C; /* 0x1C is for GPIO1~GPIO3 */
	setup_block->gpio_gpo_drive2 = 0x0;
}

/**
 * \brief Check if any key pressed.
 *
 * \param qt_status Pointer to QT status buffer.
 *
 * \retval true Some keys pressed.
 * \retval false No key pressed.
 */
static bool ut_is_any_key_pressed(struct qt_status *qt_status)
{
#define QT_LEFT_KEY_MASK     0x01
#define QT_RIGHT_KEY_MASK    0x02

	bool key_pressed = false;

	if (qt_status->key_status_2 & QT_LEFT_KEY_MASK) {
		key_pressed = true;
	}

	if (qt_status->key_status_2 & QT_RIGHT_KEY_MASK) {
		key_pressed = true;
	}

	if (qt_status->general_status & QT_GENERAL_STATUS_SDET) {
		key_pressed = true;
	}

	return key_pressed;
}

#endif /* CONF_TEST_H_INCLUDED */
