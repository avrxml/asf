 /**
 * \file
 *
 * \brief RC032Xx on STK600 board configuration template
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

#ifndef CONF_BOARD_H
#define CONF_BOARD_H

	/* Enable AC pins input configuration */
/* #define CONF_BOARD_ENABLE_AC_PINS */

	/* Configure pins to support on-board DataFlash */
/* #define CONF_BOARD_AT45DBX */

	/* Configure pins for USARTC0 */
/* #define CONF_BOARD_ENABLE_USARTC0 */

	/* Configure pins for USARTD0 */
/* #define CONF_BOARD_ENABLE_USARTD0 */

	/* Configure pins for USARTC0 in one wire mode */
/* #define CONF_BOARD_ENABLE_USARTC0_ONEWIRE */

	/* Configure pins for USARTD0 in one wire mode */
/* #define CONF_BOARD_ENABLE_USARTD0_ONEWIRE */

	/* Output clock on PORTC pin 7 */
/* #define CONF_BOARD_CLKOUT_PC7 */

	/* XCL OUT0 configurations */
/* #define CONF_BOARD_XCL_OUT0_PD0 */
/* #define CONF_BOARD_XCL_OUT0_PD4 */
/* #define CONF_BOARD_XCL_OUT0_PC0 */
/* #define CONF_BOARD_XCL_OUT0_PC4 */
/* #define CONF_BOARD_XCL_OUT0_PD0_INV */
/* #define CONF_BOARD_XCL_OUT0_PD4_INV */
/* #define CONF_BOARD_XCL_OUT0_PC0_INV */
/* #define CONF_BOARD_XCL_OUT0_PC4_INV */


	/* XCL CC0 and CC1 configurations */
/* #define CONF_BOARD_XCL_CC0_PC2 */
/* #define CONF_BOARD_XCL_CC0_PC3 */
/* #define CONF_BOARD_XCL_CC0_PD2 */
/* #define CONF_BOARD_XCL_CC0_PD3 */
/* #define CONF_BOARD_XCL_CC0_PC2_INV */
/* #define CONF_BOARD_XCL_CC0_PC3_INV */
/* #define CONF_BOARD_XCL_CC0_PD2_INV */
/* #define CONF_BOARD_XCL_CC0_PD3_INV */

	/* XCL IN0 configurations */
/* #define CONF_BOARD_XCL_IN0_PD2 */
/* #define CONF_BOARD_XCL_IN0_PD2_INV */
/* #define CONF_BOARD_XCL_IN0_PD6 */
/* #define CONF_BOARD_XCL_IN0_PD6_INV */
/* #define CONF_BOARD_XCL_IN0_PC2 */
/* #define CONF_BOARD_XCL_IN0_PC2_INV */
/* #define CONF_BOARD_XCL_IN0_PC6 */
/* #define CONF_BOARD_XCL_IN0_PC6_INV */

	/* XCL IN1 Configurations */
/* #define CONF_BOARD_XCL_IN1_PD0 */
/* #define CONF_BOARD_XCL_IN1_PD0_INV */
/* #define CONF_BOARD_XCL_IN1_PD4 */
/* #define CONF_BOARD_XCL_IN1_PD4_INV */
/* #define CONF_BOARD_XCL_IN1_PC0 */
/* #define CONF_BOARD_XCL_IN1_PC0_INV */
/* #define CONF_BOARD_XCL_IN1_PC4 */
/* #define CONF_BOARD_XCL_IN1_PC4_INV */

	/* XCL IN2 Configurations */
/* #define CONF_BOARD_XCL_IN2_PD1 */
/* #define CONF_BOARD_XCL_IN2_PD1_INV */
/* #define CONF_BOARD_XCL_IN2_PD5 */
/* #define CONF_BOARD_XCL_IN2_PD5_INV */
/* #define CONF_BOARD_XCL_IN2_PC1 */
/* #define CONF_BOARD_XCL_IN2_PC1_INV */
/* #define CONF_BOARD_XCL_IN2_PC5 */
/* #define CONF_BOARD_XCL_IN2_PC5_INV */

	/* XCL IN3 Configurations */
/* #define CONF_BOARD_XCL_IN3_PD3 */
/* #define CONF_BOARD_XCL_IN3_PD3_INV */
/* #define CONF_BOARD_XCL_IN3_PD7 */
/* #define CONF_BOARD_XCL_IN3_PD7_INV */
/* #define CONF_BOARD_XCL_IN3_PC3 */
/* #define CONF_BOARD_XCL_IN3_PC3_INV */
/* #define CONF_BOARD_XCL_IN3_PC7 */
/* #define CONF_BOARD_XCL_IN3_PC7_INV */

#endif // CONF_BOARD_H
