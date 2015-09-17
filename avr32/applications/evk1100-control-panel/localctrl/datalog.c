/*****************************************************************************
 *
 * \file
 *
 * \brief AVR32 UC3 Control Panel data logger module.
 *
 * This module is in charge of : \n
 *    * gathering the logs of data coming from the sensors and the actuators
 * of the Control Panel,
 *    * filling the log files.
 *
 *                       This file has been tested on UC3 and EVK1100.
 *
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


/*!
 * Detailed description of the module:
 * TODO
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

//_____  I N C L U D E S ___________________________________________________

#include <string.h>

#include "gpio.h" // Have to include gpio.h before FreeRTOS.h as long as FreeRTOS
                  // redefines the inline keyword to empty.

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "datalog.h"
#include "tracedump.h"
#include "fsaccess.h"
#include "sensor.h"
#include "temperature.h"
#include "light.h"
#include "potentiometer.h"

#include "cptime.h"

#include "BasicSMTP.h"

//_____ M A C R O S ________________________________________________________

/*! The maximum number of data logs that the xLogsQueue can contain. */
#define DATALOG_LOGSQUEUE_SIZE          30
// If we assume that a data log max size is 64Bytes, this gives us a total
// size of 1920 Bytes.
// One of the worst case example: js|Tue Oct 31 11:32:53 2006|js_press|alarm

/*! The nb of items in the xLogsQueue hi-threshold upon which we should dump the
    logs to file = 70% of the max nb of items the queue can hold. */
#define DATALOG_LOGSQUEUE_HITHRESHOLD   (DATALOG_LOGSQUEUE_SIZE*70/100)

/*! The nb of items in the xLogsQueue lo-threshold upon which we should stop
    dumping the logs to file = 30% of the max nb of items the queue can hold. */
#define DATALOG_LOGSQUEUE_LOTHRESHOLD   (DATALOG_LOGSQUEUE_SIZE*30/100)

/*! The max log file size. */
#define DATALOG_LOGFILE_MAXSIZE         20480

/*! Data log task stack size. */
#define DATALOG_STACK_SIZE              configMINIMAL_STACK_SIZE

/*! LogToFile task stack size. */
#define LOGTOFILE_STACK_SIZE            configMINIMAL_STACK_SIZE

/*! Invalid log id. */
#define DATALOG_INVALID_LOGSRC_ID       (0xFFFFFFFF)


//_____ D E F I N I T I O N S ______________________________________________


/*! The datalog system mutex. */
extern xSemaphoreHandle     xLOGMutex;

/*! "\<drive letter>:/LOG" strings. */
const signed portCHAR *const pcStringLogDirectoryNameA = (signed portCHAR *)"A:/LOG";
const signed portCHAR *const pcStringLogDirectoryNameB = (signed portCHAR *)"B:/LOG";

/*! The \<drive letter>:/LOG directory where the logs file are currently stored. */
const signed portCHAR *pcStringCurrentLogDirectoryName;// = (const signed portCHAR *)pcStringLogDirectoryNameA;

/*! Data logger task handle. */
static xTaskHandle   xDataLogHndl = NULL;

/*! LogToFile task handle. */
static xTaskHandle   xLogToFileHndl = NULL;

/*! The data logger module status. */
static eStatus xDataLogStatus = SYS_STATUS_STARTUP;

/*! The handle of the queue of data logs. */
static xQueueHandle xLogsQueue = 0;

/*! The number of logs in xLogsQueue. */
static unsigned portBASE_TYPE   uxNbMsgsInLogsQueue = 0;

/*! The name of the current logfile, always "LOG/mmddyy_hhmm.log".\n
 *  Thus, allocating 20Bytes for this string is enough.
 */
static char acLogFileName[25];
static char acPreviousLogFileName[25];

/*! Array of nicknames */
char *acLogSourceName[12] = { "temp", "pot", "light", "pb1", "pb2", "pb3", "js",
   "ledb1", "ledb2", "ledm1", "ledm2", "lcd" };

/*! Pre-allocated memory space to build a log. */
static char acLogString[DATALOG_LOG_MAXSIZE];

/*! Pool of xLogDef structures(FIFO) used for log allocation from ISR. */
static xLogDef   axLogsPool[DATALOG_LOGSQUEUE_HITHRESHOLD];

/*! Pointer on the next free log in the circular pool of xLogDef structures. */
static U8        u8FreeLogIdx = 0; // Is incremented upon alloc.

/*! Temporary array to set the current date of a log file. */
static char      pcTempoDate[17];

/*! The local time raw value. */
extern time_t xcptime_LocalTime;

extern int sprintf(char *out, const char *format, ...);  // From printf-stdarg.c

static portTASK_FUNCTION( vDataLogTask, pvParameters );   // FORWARD
static portTASK_FUNCTION( vLogToFileTask, pvParameters ); // FORWARD
static int prv_xopen_current_logfile( void );             // FORWARD
static void prv_vsave_logs( int fd_current_logfile );     // FORWARD
void v_datalog_AddSensorLog( eLogSourceId SensorId );     // FORWARD
void prv_vdatalog_pool_log_free( void *pxLog );           // FORWARD


//_____ D E C L A R A T I O N S ____________________________________________

#if configCTRLPANEL_TRACE == 1
/*!
 * \brief Print dev info on the trace port.
 */
void v_datalog_trace( void )
{
   NAKED_TRACE_COM2( "DATALOG:<%d><%d>", uxNbMsgsInLogsQueue, u8FreeLogIdx );
}
#endif

/*!
 * Start the data logger module.
 *
 * \param uxPriority The base priority of the data logger tasks.
 *
 * \return true upon success, else false.
 */
bool bdatalog_start( unsigned portBASE_TYPE uxPriority )
{
   int i;

   /* By default, set the current log directory to be in the DataFlash. */
   pcStringCurrentLogDirectoryName = pcStringLogDirectoryNameA;

   /* Init the xLogDef pool: set each log as free. */
   for( i=0; i<DATALOG_LOGSQUEUE_HITHRESHOLD; i++)
      axLogsPool[i].id = DATALOG_INVALID_LOGSRC_ID;

   /* Spawn the data log task. */
   if( pdPASS != xTaskCreate( vDataLogTask,
                              ( const signed portCHAR * )"DLOG",
                              DATALOG_STACK_SIZE, NULL, uxPriority,
                              &xDataLogHndl ) )
   {
      xDataLogStatus = SYS_STATUS_DOWN;
      return false;
   }

   /* Spawn the LogToFile task. */
   if( pdPASS != xTaskCreate( vLogToFileTask,
                              ( const signed portCHAR * )"LOGF",
                              LOGTOFILE_STACK_SIZE, NULL, uxPriority +1,
                              &xLogToFileHndl ) )
   {
      vTaskDelete( xDataLogHndl );
      xDataLogStatus = SYS_STATUS_DOWN;
      return false;
   }

   // Create the xLogsQueue capable of containing DATALOG_LOGSQUEUE_SIZE ptrs
   // to xLogDef structures.
   xLogsQueue = xQueueCreate( DATALOG_LOGSQUEUE_SIZE, sizeof( xLogDef * ) );
   if( 0 == xLogsQueue )
   { // Queue was not created and must not be used.
      vTaskDelete( xDataLogHndl );
      vTaskDelete( xLogToFileHndl );
      return false;
   }

   return( true );
}


/*!
 * Return the status of the datalog module.
 *
 * \return one of the eStatus enum value.
 */
eStatus xDataLog_GetStatus( void )
{
   return( xDataLogStatus );
}

/************************************ PRIVATE ********************************/

/*!
 * \brief The datalog gathering task.
 *
 */
static portTASK_FUNCTION( vDataLogTask, pvParameters )
{
   portTickType   xLastFocusTime;
   U8             u8TempWalkThrough = 0;
   U8             u8PotWalkThrough = 0;
   U8             u8LightWalkThrough = 0;


   /* Just to stop compiler warnings. */
   ( void ) pvParameters;

   xDataLogStatus = SYS_STATUS_RUNNING;

   /* We need to initialize xLastFocusTime prior to the first call to vTaskDelayUntil(). */
   xLastFocusTime = xTaskGetTickCount();

   for(;;)
   {
      // TODO:
      // 1. check all sensors sampling rate and order them in an array.
      // The array holds a delay for each sensor(so, it's an array of struct).
      // This step is out of the loop; it's an init.
      // 2. DelayUntil() of the shortest sampling rate(in array[0]).
      // 3. Perform the appropriate sensor get value.
      // 4. Update the delay left for all sensors (update the array according to
      // the previous delay and the sensors sampling rate (they may have changed
      // in the meantime)).
      // 5. Go to step 2.

      /*############# TEMPORARY ##########################*/
      vTaskDelayUntil( &xLastFocusTime, ( portTickType ) 1000 / portTICK_RATE_MS );

      // Log the temperature every ul_temp_lograte.
      u8TempWalkThrough++;
      if( ul_temp_lograte <= u8TempWalkThrough )
      {
         u8TempWalkThrough = 0;
         /* Get and log in the xLogsQueue the current temperature sensor value. */
         v_datalog_AddSensorLog( DATALOG_ID_TEMP );
      }

      // Log the potentiometer every ul_pot_lograte.
      u8PotWalkThrough++;
      if( ul_pot_lograte <= u8PotWalkThrough )
      {
         u8PotWalkThrough = 0;
         /* Get and log in the xLogsQueue the current potentiometer sensor value. */
         v_datalog_AddSensorLog( DATALOG_ID_POT );
      }

      // Log the potentiometer every ul_light_lograte.
      u8LightWalkThrough++;
      if( ul_light_lograte <= u8LightWalkThrough )
      {
         u8LightWalkThrough = 0;
         /* Get and log in the xLogsQueue the current light sensor value. */
         v_datalog_AddSensorLog( DATALOG_ID_LIGHT );
      }
      /*##################################################*/
   }
}


/*!
 * \brief Get the current value of a sensor and log it in the xLogsQueue.
 *
 * \param SensorId The sensor id of the sensor to get the value from.
 */
void v_datalog_AddSensorLog( eLogSourceId SensorId )
{
   xLogDef                  *pxLog;


   // Check if it is time to empty the xLogsQueue.
   uxNbMsgsInLogsQueue = uxQueueMessagesWaiting( xLogsQueue );
   if( DATALOG_LOGSQUEUE_HITHRESHOLD <= uxNbMsgsInLogsQueue )
   {
      // Resume the log-to-file task.
      vTaskSuspend( xLogToFileHndl );
      vTaskResume( xLogToFileHndl );
      // WARNING: What would happen if we try to resume a task that is not suspended?
      // Is the FreeRTOS Kernel implementation safe on this one? To be sure, we issue
      // a suspend before a resume.
   }

   // Alloc a log structure.
   pxLog = pxdatalog_log_alloc_init( DATALOG_ALLOC_DYNAMIC );

   if( NULL != pxLog )
   {
      // Init the id field of the log.
      pxLog->id = SensorId;

      // Get the value from the sensor.
      if( false == b_sensor_get_value( pxLog ) )
      {   // The get value operation failed.
         vdatalog_log_free( pxLog ); // Free the log.
         return;
      }

      // Add the log to the xLogsQueue.
      if( errQUEUE_FULL == xQueueSend( xLogsQueue, (void *)&pxLog, 0 ) )
      {   // The xLogsQueue is full. This may happen if the logtofile task failed
          // or if it is too slow.
         vdatalog_log_free( pxLog );
         // Resume the log-to-file task.
         vTaskSuspend( xLogToFileHndl );
         vTaskResume( xLogToFileHndl );
         // WARNING: What would happen if we try to resume a task that is not suspended?
         // Is the FreeRTOS Kernel implementation safe on this one? To be sure, we issue
         // a suspend before a resume.
      }
   }
}


/*!
 * \brief Add a log in the xLogsQueue, from within an ISR.
 *
 * \param pxLog The log to add to the xLogsQueue.
 *
 * \note This function must be called last by an ISR (Interrupt Service Routine)
 * associated with an event-based sensor.
 */
portBASE_TYPE x_datalog_AddLogFromISR( xLogDef *pxLog )
{
   portBASE_TYPE xYieldRequired = pdFALSE;


   if( ( NULL != pxLog ) && ( DATALOG_LOGSQUEUE_SIZE != uxNbMsgsInLogsQueue ) )
   {
      // NOTE: xQueueSendFromISR() doesn't say if it could add the log or not.
      // For this reason, we will first test if the queue is full. Note that
      // we cannot call uxQueueMessagesWaiting() from an ISR, so we'll play it
      // safe by looking at the latest call to uxQueueMessagesWaiting().
      // There is some hand-tuning here involved: the vLogToFileTask() must
      // work often enough so that the xLogsQueue never gets full.
      if( DATALOG_LOGSQUEUE_SIZE == uxNbMsgsInLogsQueue )
      {   // Safety
         vdatalog_log_free( pxLog );
      }
      else
      {   // Add the log to the xLogsQueue.
         portENTER_CRITICAL();
         xQueueSendFromISR( xLogsQueue, (void *)&pxLog, &xYieldRequired );
         portEXIT_CRITICAL();
      }
   }
   return( xYieldRequired );
}

/*!
 * \brief The log-to-file task.
 *
 */
static portTASK_FUNCTION( vLogToFileTask, pvParameters )
{
   int fd_current_logfile;


   /* Just to stop compiler warnings. */
   ( void ) pvParameters;

   xDataLogStatus = SYS_STATUS_RUNNING;

   /* Init the current log file name. */
   *acLogFileName = '\0';
   *acPreviousLogFileName = '\0';

   for(;;)
   {
      /* 1) Suspend ourselves. */
      vTaskSuspend( NULL ); // TO KEEP
      // NOTE: the task should be resumed when it is necessary to log the data
      // to the current log file, i.e.:
      //    (*) if the xLogQueue is 75% full,
      // OR
      //    (*) if the vDataLog_FlushLogs() has been called.

      // Take the data logger system mutex.
      if( false == x_supervisor_SemaphoreTake( xLOGMutex, 0 ) ) // 0 == Do not wait for the mutex.
      {
         // Pop the oldest logs from the logs queue so that the Control Panel
         // can still do logs acquisition.
         prv_vsave_logs( -1 );
         continue;
      }

      /* 2) Log the data into the current log file. */
      // 2a) Open the current log file.
      fd_current_logfile = prv_xopen_current_logfile();
      /* if( -1 == fd_current_logfile )
      {   // Should never happen (cf prv_xopen_current_logfile() final implem.)
         continue;
      } */
      // Although the open should not fail, if it happens(!!!), prv_vsave_logs()
      // will correctly handle the (-1) value by just poping the oldest logs from
      // the logs queue.

      // 2b) Move each item of the queue to the log file then close the file.
      prv_vsave_logs(fd_current_logfile);

      // Release the data logger system mutex.
      x_supervisor_SemaphoreGive( xLOGMutex );
   }
}


/*!
 * \brief Allocate and init a xLogDef structure.
 *
 * \param bAllocType Allocation type (dynamic or static(FromISR))
 *
 * \return Pointer on an xLogDef structure or NULL if the alloc failed.
 */
xLogDef *pxdatalog_log_alloc_init( bool bAllocType )
{
   xLogDef   *pxLog;
   void      (*pfFreeLogStruct)(void *);


   // Alloc a log structure.
   if( DATALOG_ALLOC_DYNAMIC == bAllocType )
   {   // Dynamic alloc.
      pxLog = ( xLogDef * )pvPortMalloc( sizeof( xLogDef ) );
      pfFreeLogStruct = vPortFree; // Because pvPortMalloc() was used to alloc
                                   // the log structure.
   }
   else
   {   // Static alloc.
      // TODO : add mutex if events are not on the same int level.
      if( DATALOG_INVALID_LOGSRC_ID != axLogsPool[u8FreeLogIdx].id )
      {   // Unavailable log.
         return( NULL );
      }
      else
      {
         pxLog = &(axLogsPool[u8FreeLogIdx++]);
         u8FreeLogIdx = (u8FreeLogIdx == DATALOG_LOGSQUEUE_HITHRESHOLD) ? 0 : u8FreeLogIdx;
         pfFreeLogStruct = prv_vdatalog_pool_log_free;
      }
   }

   // Init the fields that can possibly be filled now.
   if( NULL != pxLog )
   {
      pxLog->rawdate = xcptime_LocalTime;
      pxLog->pfFreeLogStruct = pfFreeLogStruct;
      pxLog->pcStringLog = NULL;
      pxLog->pfFreeStringLog = NULL;
   }

   return( pxLog );
}


/*!
 * \brief Free a data log structure and its content.
 *
 * \param pxLog pointer on the data log structure to free.
 *
 */
void vdatalog_log_free( xLogDef *pxLog )
{
   // Free the log string.
   if( NULL != pxLog->pfFreeStringLog)
   {
      pxLog->pfFreeStringLog( pxLog->pcStringLog );
   }
   // Free the structure.
   pxLog->pfFreeLogStruct( pxLog );
}


/*!
 * \brief Mark a data log structure from the pool of logs as free.
 *
 * \param pxLog pointer on the data log structure to free.
 *
 */
void prv_vdatalog_pool_log_free( void *pxLog )
{
   ((xLogDef *)pxLog)->id = DATALOG_INVALID_LOGSRC_ID;
}


/*!
 * \brief Build a log string from a xLogDef log structure.
 *
 * \param pxLog Pointer on a data log structure
 * \param pcLogString Allocated DATALOG_LOG_MAXSIZE string to fill.
 *
 */
void vdatalog_make_logstring( xLogDef *pxLog, signed portCHAR *pcLogString )
{
   struct tm   *pxLogDate;


   // Get the broken-down representation of the log date.
   pxLogDate = gmtime( &(pxLog->rawdate) );

   // Build the log string.
   // WARNING: pxLogDate->tm_year == number of years since 1900.
   // For years >= 2000, we'll display the last 2 digits only.
   if( pxLogDate->tm_year >= 100 )
      pxLogDate->tm_year -= 100;
   sprintf( (char *)pcLogString, "%s | %.2d/%.2d/%.2d %.2d:%.2d:%.2d | %s\r\n",
            acLogSourceName[pxLog->id], pxLogDate->tm_mon +1,
            pxLogDate->tm_mday, pxLogDate->tm_year, pxLogDate->tm_hour,
            pxLogDate->tm_min, pxLogDate->tm_sec, pxLog->pcStringLog );
}



/*!
 * \brief Open the current log file.
 *
 * \return the file descriptor or -1 if open failed.
 */
static int prv_xopen_current_logfile( void )
{
   int       fd_current_logfile;
   struct tm *pxDate;


   // TRACE_COM2( "open logfile begin==%d", xcptime_LocalTime );

   while( 1 )
   {
      fd_current_logfile = -1; // Init to default value.

      /* Open the log file. */
      if( '\0' != *acLogFileName )
      {   // If we have an active current log file, simply open it in append mode.
         fd_current_logfile = open( acLogFileName, O_APPEND );

         // Check if the max file size has been reached.
         if( -1 != fd_current_logfile )
         {
            if( DATALOG_LOGFILE_MAXSIZE <= fsaccess_file_get_size( fd_current_logfile ) )
            {   // The current log file has reached the max size.
               // Get the current time in the "YYYYMMDDHHMMSSMS" string format.
               v_cptime_GetDateInFatStringFormat( pcTempoDate );
               // Set the file date.
               nav_file_dateset( (FS_STRING)pcTempoDate, FS_DATE_LAST_WRITE );
               close( fd_current_logfile ); // Close the file.
               *acLogFileName = '\0';       // Reset the current log file name.
               continue; // Do another loop to create/open a new file.
            }
         }
         else
         {  // The file has been removed.
            *acLogFileName = '\0'; // Reset the current log file name.
            continue;              // Do another loop to create/open a new file.
         }
      }
      else
      {   // Create a new log file.
         // Get the broken-down representation of the current date.
         pxDate = gmtime( &xcptime_LocalTime );

         // Build the filename: mmddyy_hhmm.log
         // WARNING: pxDate->tm_year == number of years since 1900.
         // For years >= 2000, we'll display the last 2 digits only.
         if( pxDate->tm_year >= 100 )
            pxDate->tm_year -= 100;

         sprintf( acLogFileName, "%s/%.2d%.2d%.2d_%.2d%.2d.log",
                  pcStringCurrentLogDirectoryName, pxDate->tm_mon +1, pxDate->tm_mday,
                  pxDate->tm_year, pxDate->tm_hour, pxDate->tm_min );

         NAKED_TRACE_COM2( "Creating log file %s", acLogFileName );

         // Create the log file only if the /LOG directory exists.
         if( true == fsaccess_IsDirPresent( (const char *)pcStringCurrentLogDirectoryName ) )
         {     // The LOG/ directory exists.
            // Create and open the file.
            // if the file already exists, then the file size is reset.
            fd_current_logfile = open( acLogFileName, (O_CREAT|O_WRONLY) );
            // previous file is closed, send a mail
            if ( *acPreviousLogFileName != '\0' )
            {
               // post alarm to SMTP task
               v_SMTP_Post(acPreviousLogFileName, acPreviousLogFileName);
            }
            strncpy(acPreviousLogFileName, acLogFileName, strlen(acLogFileName));
         }
      }
      if( -1 == fd_current_logfile )
      {   // The open failed. We're not in maintenance mode.
         // Just remove the oldest log file: TODO
         NAKED_TRACE_COM2( "Failed opening the current log file %s", acLogFileName );
         /*########### TEMPORARY #############*/
         break;
         /*###################################*/
      }
      else break;
   }

   return( fd_current_logfile );
}


/*!
 * \brief If the log file descriptor is valid, pop the oldest logs from the logs
 * queue into the current log file until the low threshold of the logs queue is
 * reached; then close the current log file. If the log file descriptor is
 * invalid, simply pop the oldest logs from the logs queue until the low
 * threshold of the logs queue is reached.
 *
 * \param fd_current_logfile The file descriptor of the current log file.
 *
 */
static void prv_vsave_logs( int fd_current_logfile )
{
   xLogDef        *pxLog;
   int            iLogStringLen;
   portTickType   xLastFocusTime;


   // We need to initialise xLastFocusTime prior to the first call to vTaskDelayUntil().
   xLastFocusTime = xTaskGetTickCount();

   // NOTE: do not fully flush the xLogsQueue at once because it will take too
   //       much time?
   // Issue vTaskDelayUntil({10,100}) upon each log pop?
   while( DATALOG_LOGSQUEUE_LOTHRESHOLD < ( uxNbMsgsInLogsQueue = uxQueueMessagesWaiting( xLogsQueue ) ) )
   {
      // Get the oldest log from the queue.
      // NOTE: we are sure there is an item to get => no block time.
      if( pdTRUE == xQueueReceive( xLogsQueue, &pxLog, ( portTickType ) 0 ) )
      {
         if( -1 == fd_current_logfile )
         {   // Special case: simply pop the logs.
            vdatalog_log_free( pxLog ); // Free the log.
         }
         else
         {
            /* 1) Build the log string to write in the file. */
            vdatalog_make_logstring( pxLog, (signed portCHAR *)acLogString );

            /* 2) Free the log structure. */
            vdatalog_log_free( pxLog ); // Free the log.

            /* 3) Write the log to the file. */
            iLogStringLen = strlen( acLogString );
            if( iLogStringLen != write( fd_current_logfile, acLogString, iLogStringLen ) )
               break;
            // NOTE: if the write failed, we'd rather stop the current write session
            // and start on solid ground later.
         }
      }
      // Give processor time to the other tasks.
      vTaskDelayUntil( &xLastFocusTime, ( portTickType )TASK_DELAY_MS(20) );
   }
   if( -1 != fd_current_logfile )
   {
      // Get the current time in the "YYYYMMDDHHMMSSMS" string format.
      v_cptime_GetDateInFatStringFormat( pcTempoDate );
      // Set the file date.
      nav_file_dateset( (FS_STRING)pcTempoDate, FS_DATE_LAST_WRITE );
      close( fd_current_logfile ); // Close the log file.
   }
}


/*!
 * \brief Set the current logs to be stored to the DataFlash
 *
 * \return true
 */
bool b_datalog_set_current_log_drive_toDF( void )
{
   // If the current logs were being stored on the SDMMC memory, force a new
   // file creation (by resetting the file name).
   if(*acLogFileName == 'B')
   {
      *acLogFileName = '\0';
   }
   pcStringCurrentLogDirectoryName = pcStringLogDirectoryNameA;
   return(pdTRUE);
}


/*!
 * \brief Set the current logs to be stored to the SDMMC card
 *
 * \return true
 */
bool b_datalog_set_current_log_drive_toSDMMC( void )
{
   // If the current logs were being stored on the DF memory, force a new
   // file creation (by resetting the file name).
   if(*acLogFileName == 'A')
   {
      *acLogFileName = '\0';
   }
   pcStringCurrentLogDirectoryName = pcStringLogDirectoryNameB;
   return(pdTRUE);
}
