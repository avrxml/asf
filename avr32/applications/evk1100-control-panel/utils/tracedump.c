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

/* Scheduler include files. */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"

#include "serial.h"
#include "tracedump.h"


//! Baud rate used by the serial port 2.
#define dumpCOM2_BAUDRATE      ( ( unsigned portLONG ) 57600 )
//! COM Port2 buffer length.
#define dumpCOM2_BUFFER_LEN    ( ( unsigned portLONG ) 64 )

//! Banner printed on the dump port.
// WARNING: its length must be smaller or equal to dumpCOM2_BUFFER_LEN.
#define TRACE_MSG_BANNER       "\x0C\r\n---------- Traces Dump \r\n"

#define DUMPCOM2_NO_BLOCK      ( ( portTickType ) 0 )

//! COM Port2 handle.
static xComPortHandle  xComPort2Hndl = (xComPortHandle)NULL;

/*!
 * This function initializes the dump port.
 * \return DUMP_SUCCESS or DUMP_FAILURE
 * \warning FreeRTOS must already be up & running when calling this function.
 */
int itracedump_Init( void )
{
  int iStatus = DUMP_SUCCESS;

  //**
  //** Init the port.
  //**
  // Init the COM Port2.
  xComPort2Hndl = xUsartInit(serCOM2, dumpCOM2_BAUDRATE, 0, dumpCOM2_BUFFER_LEN);
  if(xComPort2Hndl == 0)
    return(DUMP_FAILURE);

  //**
  //** Dump a banner to the port.
  //**
  vtracedump_PrintBlocking((const signed portCHAR * const)TRACE_MSG_BANNER);

  return(iStatus);
}

/*!
 * \brief Stop the tracedump module resources.
 */
void v_tracedump_stopResources( void )
{
  vSerialClose( xComPort2Hndl );
}

/*!
 * Send a string of characters to the dump port.
 * \param pcString The string to dump
 * \return The number of characters that could not be sent.
 * \warning FreeRTOS must already be up & running when calling this function.
 */
unsigned portSHORT ustracedump_Print(const signed portCHAR * const pcString)
{
  return( usUsartPutString( xComPort2Hndl, pcString, strlen( (const portCHAR * )pcString ) ) );
}


/*!
 * Send a string of characters to the dump port.
 * \param pcString The string to dump
 * \warning FreeRTOS must already be up & running when calling this function.
 */
void vtracedump_PrintBlocking(const signed portCHAR * const pcString)
{
   unsigned portSHORT usRemainChar = 0;
   unsigned portSHORT usMsgLen = strlen((const portCHAR * )pcString);

   usRemainChar = usMsgLen;
   do{
      usRemainChar = usUsartPutString( xComPort2Hndl,
                                       (const signed portCHAR * const)(pcString + usMsgLen - usRemainChar),
                                       usRemainChar );
   }while( usRemainChar );
}


/*!
 * \brief Put a char to the dump port.
 *
 * \param cByte The character to put
 */
void vtracedump_Putchar(signed portCHAR cByte )
{
   xUsartPutChar( xComPort2Hndl, cByte, DUMPCOM2_NO_BLOCK );
}

/*!
 * \brief Put a char to the dump port.
 *
 * \param cByte The character to put
 */
void vtracedump_Putchar_Block(signed portCHAR cByte )
{
   xUsartPutChar( xComPort2Hndl, cByte, -1 );
}
