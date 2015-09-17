/*****************************************************************************
 *
 * \file
 *
 * \brief Control Panel trace dump module.
 *
 * This file defines a set of functions to dump trace.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
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
