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

/* Scheduler include files. */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
