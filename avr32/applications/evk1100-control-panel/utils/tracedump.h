/*****************************************************************************
 *
 * \file
 *
 * \brief Control Panel trace dump module.
 *
 * This file defines a set of functions to dump trace.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef _AVR32_TRACE_DUMP_H
#define _AVR32_TRACE_DUMP_H

#include <avr32/io.h>
#include "FreeRTOS.h"


/*! Value returned by function when it completed successfully */
#define DUMP_SUCCESS 0

/*! Value returned by function when it was unable to complete successfully
    for some unspecified reason */
#define DUMP_FAILURE -1

/*!
 * This function initializes a dump port.
 * \return DUMP_SUCCESS or DUMP_FAILURE
 * \warning FreeRTOS must already be up & running when calling this function.
 */
int itracedump_Init(void);

/*!
 * \brief Stop the tracedump module resources.
 */
void v_tracedump_stopResources( void );

/*!
 * Send a string of characters to the dump port.
 * \param pcString The string to dump
 * \return The number of characters that could not be sent.
 * \warning FreeRTOS must already be up & running when calling this function.
 */
unsigned portSHORT ustracedump_Print(const signed portCHAR * const pcString);

/*!
 * Send a string of characters to the dump port.
 * \param pcString The string to dump
 * \warning FreeRTOS must already be up & running when calling this function.
 */
void vtracedump_PrintBlocking(const signed portCHAR * const pcString);

/*!
 * \brief Put a char to the dump port.
 *
 * \param cByte The character to put
 */
void vtracedump_Putchar(signed portCHAR cByte );

/*!
 * \brief Put a char to the dump port.
 *
 * \param cByte The character to put
 */
void vtracedump_Putchar_Block(signed portCHAR cByte );


extern int printk(const char *format, ...);

#if configCTRLPANEL_TRACE == 1
#define NAKED_TRACE_COM2(...) \
{\
  printk( __VA_ARGS__ );\
  printk( (char *)"\r\n" );\
}

#define TRACE_COM2(...) \
{\
  printk( __FILE__ ":" ASTRINGZ(__LINE__) ": " );\
  NAKED_TRACE_COM2( __VA_ARGS__ );\
}

#else // No trace.
#define NAKED_TRACE_COM2(...)
#define TRACE_COM2(...)
#endif
/*  printk( __VA_ARGS__ );\
  printk( (char *)"\r\n" );\
*/
#endif /* #ifndef _AVR32_TRACE_DUMP_H */
