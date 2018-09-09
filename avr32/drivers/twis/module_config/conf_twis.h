/*****************************************************************************
 *
 * \file
 *
 * \brief TWIS Configuration File for AVR32 UC3.
 *
 * This file defines a useful set of functions for TWIM on AVR32 devices.
 *
 *****************************************************************************/

/**
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
#ifndef _CONF_TWIS_H
#define _CONF_TWIS_H
#include "board.h"

#if UC3C
// These defines are missing from or wrong in the toolchain header file ip_xxx.h or part.h
#	if !defined(AVR32_TWIS0_GROUP)
#		define AVR32_TWIS0_GROUP         27
#	else
#		warning "Duplicate define(s) to remove from the ASF"
#	endif
#	if !defined(AVR32_TWIS1_GROUP)
#		define AVR32_TWIS1_GROUP         28
#	else
#		warning "Duplicate define(s) to remove from the ASF"
#	endif
#	if !defined(AVR32_TWIS2_GROUP)
#		define AVR32_TWIS2_GROUP         46
#	else
#		warning "Duplicate define(s) to remove from the ASF"
#	endif
#	define CONF_TWIS_IRQ_LINE          AVR32_TWIS0_IRQ
#	define CONF_TWIS_IRQ_GROUP         AVR32_TWIS0_GROUP
#	define CONF_TWIS_IRQ_LEVEL         1
#elif UC3A3
// These defines are missing from or wrong in the toolchain header file ip_xxx.h or part.h
#	if !defined(AVR32_TWIS0_GROUP)
#		define AVR32_TWIS0_GROUP         27
#	else
#		warning "Duplicate define(s) to remove from the ASF"
#	endif
#	if !defined(AVR32_TWIS1_GROUP)
#		define AVR32_TWIS1_GROUP         28
#	else
#		warning "Duplicate define(s) to remove from the ASF"
#	endif
#	define CONF_TWIS_IRQ_LINE          AVR32_TWIS0_IRQ
#	define CONF_TWIS_IRQ_GROUP         AVR32_TWIS0_GROUP
#	define CONF_TWIS_IRQ_LEVEL         1
#elif UC3L
// These defines are missing from or wrong in the toolchain header file ip_xxx.h or part.h
#	if !defined(AVR32_TWIS0_GROUP)
#		define AVR32_TWIS0_GROUP         22
#	else
#		warning "Duplicate define(s) to remove from the ASF"
#	endif
#	if !defined(AVR32_TWIS1_GROUP)
#		define AVR32_TWIS1_GROUP         23
#	else
#		warning "Duplicate define(s) to remove from the ASF"
#	endif
#	define CONF_TWIS_IRQ_LINE          AVR32_TWIS0_IRQ
#	define CONF_TWIS_IRQ_GROUP         AVR32_TWIS0_GROUP
#	define CONF_TWIS_IRQ_LEVEL         1
#elif UC3D
// These defines are missing from or wrong in the toolchain header file ip_xxx.h or part.h
#	if !defined(AVR32_TWIS_GROUP)
#		define AVR32_TWIS_GROUP         11
#	else
#		warning "Duplicate define(s) to remove from the ASF"
#	endif
#	define CONF_TWIS_IRQ_LINE          AVR32_TWIS_IRQ
#	define CONF_TWIS_IRQ_GROUP         AVR32_TWIS_GROUP
#	define CONF_TWIS_IRQ_LEVEL         1
#else
#	error MCU Not Supported
#endif

#endif // _CONF_TWIS_H
