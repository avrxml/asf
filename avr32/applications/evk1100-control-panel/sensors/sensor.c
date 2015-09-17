/*****************************************************************************
 *
 * \file
 *
 * \brief AVR32 UC3 Control Panel sensors interface module.
 *
 * This module acts as an interface to all sensors.
 *
 *
 *                       The example is written for UC3 and EVK1100.
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


/*
 * Brief description of the module:
 * This module acts as an interface to all sensors.
 *
 * Detailed description of the module:
 * TODO
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <string.h>

#include "compiler.h"
#include "FreeRTOS.h"

#include "tracedump.h"

#include "supervisor.h"

#include "shell.h"

#include "sensor.h"
#include "temperature.h"
#include "potentiometer.h"
#include "light.h"
#include "pushb.h"
#include "joystick.h"
#include "adc.h"

//_____ M A C R O S ________________________________________________________

/*! Max number of sensors. */
#define SENSOR_MAXNB_SENSORS   (DATALOG_ID_JS - DATALOG_ID_TEMP +1)

#define SENSOR_LIST            "{temp,pot,light,pb1,pb2,pb3,js,all}"

//_____ D E F I N I T I O N S ______________________________________________

/*! A sensor register cell structure. */
typedef struct SensorReg {
   // Fct ptr to the actual configure sensor function.
   bool (*pfConfigureSensor)( void );
   // Fct ptr to the actual sensor get value function.
   bool (*pfGetSensorValue)( xLogDef *pxLog );
   // Fct ptr to the actual sensor get config function.
   eExecStatus (*pfGetSensorConfig)( signed portCHAR **ppcStringReply );
   // Fct ptr to the actual sensor set config function.
   eExecStatus (*pfSetSensorConfig)( signed portCHAR **ppcStringReply, int ac, signed portCHAR *av[] );
   // Fct ptr to the actual stop sensor function.
   void (*pfStopSensor)( void );
}xSensorReg;

xSensorReg axSensorsRegistry[SENSOR_MAXNB_SENSORS] = {
   { b_temperature_init, b_temperature_get_value, e_temperature_get_config, e_temperature_set_config, NULL } ,
   { b_potentiometer_init, b_potentiometer_get_value, e_potentiometer_get_config, e_potentiometer_set_config, NULL } ,
   { b_light_init, b_light_get_value, e_light_get_config, e_light_set_config, NULL } ,
   { b_pushb1_init, b_pushb1_get_value, e_pushb1_get_config, e_pushb1_set_config, v_pushb1_stop } ,
   { b_pushb2_init, b_pushb2_get_value, e_pushb2_get_config, e_pushb2_set_config, v_pushb2_stop } ,
   { b_pushb3_init, b_pushb3_get_value, e_pushb3_get_config, e_pushb3_set_config, v_pushb3_stop } ,
   { b_joystick_init, b_joystick_get_value, e_joystick_get_config, e_joystick_set_config, v_joystick_stop }
};

/*! Error msg upon get_sensor_value syntax error. */
const signed portCHAR *const SENSOR_ERRMSG_GETVAL_SYNTAXERROR   = (signed portCHAR *)"Error"CRLF"Usage: get_sensor_value sensor=sensorname"CRLF;

/*! Error msg upon get_sensor_config syntax error. */
const signed portCHAR *const SENSOR_ERRMSG_GETCFG_SYNTAXERROR   = (signed portCHAR *)"Error"CRLF"Usage: get_sensor_config sensor=sensorname"CRLF;

/*! Error msg upon get_sensor_config syntax error. */
const signed portCHAR *const SENSOR_ERRMSG_SETCFG_SYNTAXERROR   = (signed portCHAR *)"Error"CRLF"Usage: set_sensor_config sensor=sensorname param=value"CRLF;

/*! Error msg upon get_sensor_value sensor error. */
const signed portCHAR *const SENSOR_ERRMSG_GETVAL_FAIL          = (signed portCHAR *)"Error"CRLF"Sensor failed to deliver a value."CRLF;

/*! Error msg upon unreferenced sensor error. */
const signed portCHAR *const SENSOR_ERRMSG_UNREFSENSOR   = (signed portCHAR *)"Error"CRLF"Unreferenced sensor name"CRLF;

const signed portCHAR *const SENSOR_MSG_HELP            = (signed portCHAR *)"\
"CRLF"get_sensor_config sensor="SENSOR_LIST" : display the config of a sensor"CRLF"\
set_sensor_config sensor=temp min=val : set the min temp that triggers an alarm"CRLF"\
set_sensor_config sensor=temp max=val : set the max temp that triggers an alarm"CRLF"\
set_sensor_config sensor=pot min=val : set the min value that triggers an alarm"CRLF"\
set_sensor_config sensor=pot max=val : set the max value that triggers an alarm"CRLF"\
set_sensor_config sensor=light min=val : set the min value that triggers an alarm"CRLF"\
set_sensor_config sensor=light max=val : set the max value that triggers an alarm"CRLF"\
set_sensor_config sensor=pb{1,2,3} alarm={on,off} : en/disable an alarm upon event"CRLF"\
set_sensor_config sensor=js alarm={on,off} : en/disable an alarm upon event"CRLF"\
get_sensor_value sensor="SENSOR_LIST" : display the current value of one sensor or of all sensors"CRLF;

/*! The CFG system mutex. */
extern xSemaphoreHandle   xCFGMutex;

//_____ D E C L A R A T I O N S ____________________________________________

/*!
 * Start the sensor module.
 *
 * \return true upon success, else false.
 */
bool bsensor_start( void )
{
   volatile avr32_adc_t * adc= (volatile avr32_adc_t *) &AVR32_ADC;
   int i;

   /* configure ADC */
   adc_configure( adc );

   for( i = 0; i < SENSOR_MAXNB_SENSORS ; i++)
   {
      if ( axSensorsRegistry[i].pfConfigureSensor() != true )
      {
         NAKED_TRACE_COM2( "Init sensor %d failed", i );
         return false;
      }
   }
   return true;
}

/*!
 * \brief Stop the sensor module resources.
 */
void v_sensor_stop( void )
{
  int i;
  for( i = 0; i < SENSOR_MAXNB_SENSORS ; i++)
  {
    if( NULL != axSensorsRegistry[i].pfStopSensor )
    {
      axSensorsRegistry[i].pfStopSensor();
    }
  }
}


/*!
 *  \brief Get a current sensor value.
 *
 *  \param pxLog a Log structure. The sensor id is in the id field of pxLog.
 *
 *  \return true upon success, false if error.
 */
bool b_sensor_get_value( xLogDef *pxLog )
{
   switch( pxLog->id )
   {
      case DATALOG_ID_TEMP:
         return( b_temperature_get_value( pxLog ) );
      case DATALOG_ID_POT:
         return( b_potentiometer_get_value( pxLog ) );
      case DATALOG_ID_LIGHT:
         return( b_light_get_value( pxLog ) );
      case DATALOG_ID_PB1:
         return( b_pushb1_get_value( pxLog ) );
      case DATALOG_ID_PB2:
         return( b_pushb2_get_value( pxLog ) );
      case DATALOG_ID_PB3:
         return( b_pushb3_get_value( pxLog ) );
      case DATALOG_ID_JS:
         return( b_joystick_get_value( pxLog ) );
      default:
         TRACE_COM2( "Unknown sensor id %d", pxLog->id );
         return( false );
   }
}

/*!
 *  \brief The get sensor value command: get current value from a sensor.
 *         Format: get_sensor_value sensor=sensorname
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Ignored.
 *  \param ac             Input. The argument counter. For this command, should be 2.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_sensor_cmd_get_value( eModId xModId, signed short FsNavId,
                                    int ac, signed portCHAR *av[],
                                    signed portCHAR **ppcStringReply )
{
   int       i;
   xLogDef   *pxLog;
   int       end=0;


   /* 0) If the way to reply is unavailable, it's no use to continue. */
   if( ppcStringReply == NULL )
      return( SHELL_EXECSTATUS_KO );

   /* 1) Check the input. */
   //  i) Two arguments exactly.
   if( 2 != ac )
   {   // Syntax error.
      *ppcStringReply = (signed portCHAR *)SENSOR_ERRMSG_GETVAL_SYNTAXERROR;
      return( SHELL_EXECSTATUS_KO );
   }
   // No need to check that av[0] == sensor. We actually use the "sensor=name"
   // format just to comply with the cgi calls rules.

   //  ii) Identify the sensor.
   for( i=DATALOG_ID_TEMP;
        i<=DATALOG_ID_JS && strcmp( (char *)av[1], acLogSourceName[i] );
        i++ );

   if( ( SENSOR_MAXNB_SENSORS == i ) && strcmp( (char *)av[1], "all" ) )
   {
      *ppcStringReply = (signed portCHAR *)SENSOR_ERRMSG_UNREFSENSOR;
      return( SHELL_EXECSTATUS_KO );
   }

   // Alloc and init a log.
   pxLog = pxdatalog_log_alloc_init( DATALOG_ALLOC_DYNAMIC );
   if( NULL == pxLog )
   {
      *ppcStringReply = (signed portCHAR *)SHELL_ERRMSG_MEMALLOC;
      return( SHELL_EXECSTATUS_KO );
   }

   /* 3) Perform the command. */
   if( SENSOR_MAXNB_SENSORS != i )
   {               // Case: one sensor.
      // Init the id field of the log.
      pxLog->id = i;

      // Get the value.
      if( false == axSensorsRegistry[i].pfGetSensorValue( pxLog ) )
      {
         vdatalog_log_free( pxLog );
         *ppcStringReply = (signed portCHAR *)SENSOR_ERRMSG_GETVAL_FAIL;
         return( SHELL_EXECSTATUS_KO );
      }

      /* 4) Build the reply. */
      // Alloc space for the reply.
      *ppcStringReply = (signed portCHAR *)pvPortMalloc( DATALOG_LOG_MAXSIZE );
      if( NULL == *ppcStringReply )
      {
         vdatalog_log_free( pxLog );
         *ppcStringReply = (signed portCHAR *)SHELL_ERRMSG_MEMALLOC;
         return( SHELL_EXECSTATUS_KO );
      }

      // Build the string.
      vdatalog_make_logstring( pxLog, *ppcStringReply );

      // Free the log.
      vdatalog_log_free( pxLog );
   }
   else
   {               // Case: all sensors
      // Alloc space for the reply.
      *ppcStringReply = (signed portCHAR *)pvPortMalloc( SENSOR_MAXNB_SENSORS*DATALOG_LOG_MAXSIZE );
      if( NULL == *ppcStringReply )
      {
         vdatalog_log_free( pxLog );
         *ppcStringReply = (signed portCHAR *)SHELL_ERRMSG_MEMALLOC;
         return( SHELL_EXECSTATUS_KO );
      }

      for( i=DATALOG_ID_TEMP; i<=DATALOG_ID_JS; i++ )
      {
         // Init the id field of the log.
         pxLog->id = i;

         // Get the value.
         if( false == axSensorsRegistry[i].pfGetSensorValue( pxLog ) )
         {
            vdatalog_log_free( pxLog );
            vPortFree( *ppcStringReply );
            *ppcStringReply = (signed portCHAR *)SENSOR_ERRMSG_GETVAL_FAIL;
            return( SHELL_EXECSTATUS_KO );
         }

         /* 4) Build the reply. */

         // Build the string.
         vdatalog_make_logstring( pxLog, *ppcStringReply + end );
         end = strlen( (char *)*ppcStringReply );

         // Free the log string.
         if( NULL != pxLog->pfFreeStringLog)
         {
            pxLog->pfFreeStringLog( pxLog->pcStringLog );
         }
      } // for( i=DATALOG_ID_TEMP; i<=DATALOG_ID_JS; i++ )
      // Free the log.
      pxLog->pfFreeStringLog = NULL; // Do that because we already freed the log string.
      vdatalog_log_free( pxLog );
   }

   return( SHELL_EXECSTATUS_OK );
}


/*  \brief The get sensor config command: get the config fields value of a sensor.
 *         Takes one parameter, that is the sensor's name.
 *         Format: get_sensor_config sensor=sensorname
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Ignored.
 *  \param ac             Input. The argument counter. For this command, should be 1.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_sensor_cmd_get_config( eModId xModId, signed short FsNavId,
                                     int ac, signed portCHAR *av[],
                                     signed portCHAR **ppcStringReply )
{
   int       i;


   /* 0) If the way to reply is unavailable, it's no use to continue. */
   if( ppcStringReply == NULL )
      return( SHELL_EXECSTATUS_KO );

   /* 1) Check the input. */
   //  i) Two arguments exactly.
   if( 2 != ac )
   {   // Syntax error.
      *ppcStringReply = (signed portCHAR *)SENSOR_ERRMSG_GETCFG_SYNTAXERROR;
      return( SHELL_EXECSTATUS_KO );
   }
   // No need to check that av[0] == sensor. We actually use the "sensor=name"
   // format just to comply with the cgi calls rules.

   //  ii) Identify the sensor.
   for( i=DATALOG_ID_TEMP;
        i<=DATALOG_ID_JS && strcmp( (char *)av[1], acLogSourceName[i] );
        i++ );

   if( SENSOR_MAXNB_SENSORS == i )
   {
      *ppcStringReply = (signed portCHAR *)SENSOR_ERRMSG_UNREFSENSOR;
      return( SHELL_EXECSTATUS_KO );
   }

   // Get the reply
   return( axSensorsRegistry[i].pfGetSensorConfig( ppcStringReply ) );
}


/*  \brief The set sensor config command: set the value of a config field of a sensor.
 *         Takes three parameters.
 *         The first parameter is the sensor's name, the second parameter is
 *         the config field name, the third parameter is the value.
 *         Format: set_sensor_config sensorname field=value
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Ignored.
 *  \param ac             Input. The argument counter. For this command, should be 3.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_sensor_cmd_set_config( eModId xModId, signed short FsNavId,
                                     int ac, signed portCHAR *av[],
                                     signed portCHAR **ppcStringReply )
{
   eExecStatus   xRet = SHELL_EXECSTATUS_KO;
   int       i;

   // Take the CFG mutex.
   if( pdFALSE == x_supervisor_SemaphoreTake( xCFGMutex, 0 ) )
   {
      if( NULL != ppcStringReply )
      {
         *ppcStringReply = (signed portCHAR *)SHELL_ERRMSG_MAINTENANCEMODE;
      }
      return( SHELL_EXECSTATUS_KO );
   }
   else if(ppcStringReply != NULL)
   {
      /* 1) Check the input. */
      //  i) 4 arguments exactly.
      if( 4 != ac )
      {   // Syntax error.
         *ppcStringReply = (signed portCHAR *)SENSOR_ERRMSG_SETCFG_SYNTAXERROR;
         x_supervisor_SemaphoreGive( xCFGMutex ); // Release the CFG mutex.
         return( SHELL_EXECSTATUS_KO );
      }
      // No need to check that av[0] == sensor. We actually use the "sensor=name"
      // format just to comply with the cgi calls rules.

      //  ii) Identify the sensor.
      for( i=DATALOG_ID_TEMP;
           i<=DATALOG_ID_JS && strcmp( (char *)av[1], acLogSourceName[i] );
           i++ );

      if( SENSOR_MAXNB_SENSORS == i )
      {
         *ppcStringReply = (signed portCHAR *)SENSOR_ERRMSG_UNREFSENSOR;
         x_supervisor_SemaphoreGive( xCFGMutex ); // Release the CFG mutex.
         return( SHELL_EXECSTATUS_KO );
      }

     // Get the reply
     xRet = axSensorsRegistry[i].pfSetSensorConfig( ppcStringReply, (ac - 2), &av[2] );
   }

   x_supervisor_SemaphoreGive( xCFGMutex ); // Release the CFG mutex.
   return( xRet );
}


/*  \brief The sensor help command: display the sensors available shell commands.
 *         Format: help
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Ignored.
 *  \param ac             Ignored.
 *  \param av             Ignored.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_sensor_help( eModId xModId, signed short FsNavId,
                           int ac, signed portCHAR *av[],
                           signed portCHAR **ppcStringReply )
{
   // 1) If the way to reply is unavailable, it's no use to continue.
   if( ppcStringReply == NULL )
      return( SHELL_EXECSTATUS_KO );

   // 2) Perform the command.
   *ppcStringReply = (signed portCHAR *)SENSOR_MSG_HELP;

   return( SHELL_EXECSTATUS_OK_NO_FREE );
}
