/**
 * \file
 *
 * \brief RC032Xx on STK600 board configuration template
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
