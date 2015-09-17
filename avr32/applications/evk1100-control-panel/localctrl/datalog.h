/*****************************************************************************
 *
 * \file
 *
 * \brief AVR32 UC3 Control Panel data logger module interface.
 *
 * This module is in charge of the logs of data coming from the sensors and the
 * actuators of the Control Panel.
 *
 *
 *                       This file has been tested on UC3 and EVK1100.
 *
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#ifndef __DATALOG_H__
#define __DATALOG_H__

#include <time.h>

#include "FreeRTOS.h"

#include "supervisor.h"


//_____ M A C R O S ________________________________________________________

/*! A data log max size. */
#define DATALOG_LOG_MAXSIZE             64

/*! xLogDef structure allocation types. */
#define DATALOG_ALLOC_DYNAMIC           true
#define DATALOG_ALLOC_STATIC            false

/*!
 * Logs Sources(sensors and actuators) id.
 */
typedef enum
{
   DATALOG_ID_TEMP,    // The temperature sensor.
   DATALOG_ID_POT,     // The potentiometer sensor.
   DATALOG_ID_LIGHT,   // The light sensor.
   DATALOG_ID_PB1,     // The pushbutton1 sensor.
   DATALOG_ID_PB2,     // The pushbutton2 sensor.
   DATALOG_ID_PB3,     // The pushbutton3 sensor.
   DATALOG_ID_JS,      // The joystick sensor.
   DATALOG_ID_LEDB1,   // The bicolor led 1 actuator.
   DATALOG_ID_LEDB2,   // The bicolor led 2 actuator.
   DATALOG_ID_LEDM1,   // The monocolor led 1 actuator.
   DATALOG_ID_LEDM2,   // The monocolor led 2 actuator.
   DATALOG_ID_LCD,     // The LCD actuator.
} eLogSourceId;

//_____ D E F I N I T I O N S ______________________________________________

/*! A data log structure. */
typedef struct LogDef {
   int    id; // Id of the sensor or actuator that produced the log.
   time_t rawdate; // Raw date value the log was produced.
   char   *pcStringLog; // Const or allocated. Content is "{value,event}[|alarm]\r\n"
   void   (*pfFreeStringLog)( void *pcStr ); // Fct ptr to call to free the memory
                                             // used by the pcStringLog field.
   void   (*pfFreeLogStruct)( void *pxLog ); // Fct ptr to call to free the memory
                                             // used by the current LogDef structure.
}xLogDef;

/*! Array of nicknames */
extern char *acLogSourceName[];


//_____ D E C L A R A T I O N S ____________________________________________

#if configCTRLPANEL_TRACE == 1
/*!
 * \brief Print dev info on the trace port.
 */
void v_datalog_trace( void );
#endif

/*!
 * Start the data logger module.
 *
 * \param uxPriority The base priority of the data logger tasks.
 *
 * \return true upon success, else false.
 */
bool bdatalog_start( unsigned portBASE_TYPE uxPriority );


/*!
 * \brief Allocate and init a xLogDef structure.
 *
 * \param bAllocType Allocation type (dynamic or static)
 *
 * \return Pointer on an xLogDef structure or NULL if the alloc failed.
 */
xLogDef *pxdatalog_log_alloc_init( bool bAllocType );

/*!
 * \brief Allocate and init a xLogDef structure from an ISR.
 *
 * \return Pointer on an xLogDef structure or NULL if the alloc failed.
 */
#define pxdatalog_log_alloc_init_FromISR()   pxdatalog_log_alloc_init( DATALOG_ALLOC_STATIC )


/*!
 * \brief Free a data log structure and its content.
 *
 * \param pxLog pointer on the data log structure to free.
 *
 */
void vdatalog_log_free( xLogDef *pxLog );


/*!
 * \brief Build a log string from a xLogDef log structure.
 *
 * \param pxLog Pointer on a data log structure
 * \param pcLogString Allocated DATALOG_LOG_MAXSIZE string to fill.
 *
 */
void vdatalog_make_logstring( xLogDef *pxLog, signed portCHAR *pcLogString );

/*!
 * \brief Add a log in the xLogsQueue, from within an ISR.
 *
 * \param pxLog The log to add to the xLogsQueue.
 *
 * \note This function must be called last by an ISR (Interrupt Service Routine)
 * associated with an event-based sensor.
 */
portBASE_TYPE x_datalog_AddLogFromISR( xLogDef *pxLog );


/*!
 * \brief Set the current logs to be stored to the DataFlash
 *
 * \return true
 */
bool b_datalog_set_current_log_drive_toDF( void );


/*!
 * \brief Set the current logs to be stored to the SDMMC card
 *
 * \return true
 */
bool b_datalog_set_current_log_drive_toSDMMC( void );

/*!
 * Return the status of the datalog module.
 *
 * \return one of the eStatus enum value.
 */
eStatus xDataLog_GetStatus( void );

#endif
