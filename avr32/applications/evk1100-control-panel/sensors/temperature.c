/*****************************************************************************
 *
 * \file
 *
 * \brief AVR32 UC3 Control Panel temperature sensor module.
 *
 * - Supported boards:   EVK1100
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

//_____  I N C L U D E S ___________________________________________________
#include <string.h>

#include "compiler.h"
#include "FreeRTOS.h"

#include "temperature.h"

#include "datalog.h"
#include "shell.h"
#include "board.h"
#include "gpio.h"
#include "adc.h"
#include "sensor.h"
#include "config_file.h"
#include "fsaccess.h"

#include "BasicSMTP.h"

//_____ M A C R O S ________________________________________________________

/*! Max string length of a get config. */
#define TEMPERATURE_GETCONF_MAXLEN   64

/*! Path for the config file */
#define SENSOR_TEMP_CONFIG_FILE   "A:/CFG/TEMP.TXT"


//_____ D E F I N I T I O N S ______________________________________________

//! The following table give the correspondance between the ADC code and the temperature
//! Each elements of the table corresponds to an ADC code value.
//! The index in the table give the corresponding temperature (inC) with
//! the following formula : Tempeature=index-20.
//! table[O] corresponds to -20C temperature code
//! The following table gives the ADC code for VCC=3.3V and Aref=AVcc
const unsigned short temperature_code[]={
   0x3B4,0x3B0,0x3AB,0x3A6,0x3A0,0x39A,0x394,0x38E,0x388,0x381,0x37A,0x373,
   0x36B,0x363,0x35B,0x353,0x34A,0x341,0x338,0x32F,0x325,0x31B,0x311,0x307,
   0x2FC,0x2F1,0x2E6,0x2DB,0x2D0,0x2C4,0x2B8,0x2AC,0x2A0,0x294,0x288,0x27C,
   0x26F,0x263,0x256,0x24A,0x23D,0x231,0x225,0x218,0x20C,0x200,0x1F3,0x1E7,
   0x1DB,0x1CF,0x1C4,0x1B8,0x1AC,0x1A1,0x196,0x18B,0x180,0x176,0x16B,0x161,
   0x157,0x14D,0x144,0x13A,0x131,0x128,0x11F,0x117,0x10F,0x106,0xFE,0xF7,
   0xEF,0xE8,0xE1,0xDA,0xD3,0xCD,0xC7,0xC0,0xBA,0xB5,0xAF,0xAA,0xA4,0x9F,
   0x9A,0x96,0x91,0x8C,0x88,0x84,0x80,0x7C,0x78,0x74,0x71,0x6D,0x6A,0x67,
   0x64,0x61,0x5E,0x5B,0x58,0x55,0x53,0x50,0x4E,0x4C,0x49,0x47,0x45,0x43,
   0x41,0x3F,0x3D,0x3C,0x3A,0x38};

/*! Value for the log rate. */
unsigned int ul_temp_lograte = 5;

/*! Min value for alarm. */
static int l_temp_min = -50;

/*! Max Value for alarm. */
static int l_temp_max = 100;

/*! Alarm upon event. */
static bool b_temp_alarm = pdFALSE;
/*! Alarm for max is pending ? */
static bool b_temp_alarm_max = pdFALSE;
/*! Alarm for min is pending ? */
static bool b_temp_alarm_min = pdFALSE;

/*! ADC cell. */
static volatile avr32_adc_t * adc= (volatile avr32_adc_t *) &AVR32_ADC;

/*! The CFG system mutex. */
extern xSemaphoreHandle   xCFGMutex;

//_____ D E C L A R A T I O N S ____________________________________________
extern int sprintf(char *out, const char *format, ...);  // From printf-stdarg.c


/*!
 *  \brief Init the temperature channel.
 *
 *  \return true upon success, false if error.
 */
bool b_temperature_init ( void )
{
portCHAR token[6];
portCHAR * unit;

   // Get the xCFGMutex.
   if( pdTRUE == x_supervisor_SemaphoreTake( xCFGMutex, 0 ) )
   {
       // get the field
       if (config_file_get_value(SENSOR_TEMP_CONFIG_FILE, "alarm" , token) >= 0)
       {
         // update value
         if (!strcmp(token, "on"))
         {
           b_temp_alarm = pdTRUE;
         }
       }
       if (config_file_get_value(SENSOR_TEMP_CONFIG_FILE, "min" , token) >= 0)
       {
         unit = strpbrk(token , "C");
         if (unit != NULL)
         {
           *unit = '\0';
         }
         l_temp_min = atol(token);
       }
       if (config_file_get_value(SENSOR_TEMP_CONFIG_FILE, "max" , token) >= 0)
       {
         unit = strpbrk(token , "C");
         if (unit != NULL)
         {
           *unit = '\0';
         }
         l_temp_max = atol(token);
       }
       if (config_file_get_value(SENSOR_TEMP_CONFIG_FILE, "lograte" , token) >= 0)
       {
         ul_temp_lograte = atoi(token);
       }
     // Release the xCFGMutex.
     x_supervisor_SemaphoreGive( xCFGMutex );
   }
   /* enable pin for sensor */
   gpio_enable_module_pin( ADC_TEMPERATURE_PIN , ADC_TEMPERATURE_FUNCTION );

   return (true);
}

/*!
 *  \brief Get the temperature sensor config.
 *
 *  \param ppcStringReply Input/Output. The response string. NEVER NULL AS INPUT.
 *                        A malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_temperature_get_config( signed portCHAR **ppcStringReply )
{
   // Alloc space for the reply.
   *ppcStringReply = (signed portCHAR *)pvPortMalloc( TEMPERATURE_GETCONF_MAXLEN );
   if( NULL == *ppcStringReply )
   {
      *ppcStringReply = (signed portCHAR *)SHELL_ERRMSG_MEMALLOC;
      return( SHELL_EXECSTATUS_KO );
   }
   // Build the string.
   sprintf( (char *)*ppcStringReply, "lograte=%d\r\n""min=%dC\r\n""max=%dC\r\n""alarm=%s\r\n",
            ul_temp_lograte, l_temp_min, l_temp_max, ((b_temp_alarm == pdTRUE) ? "on" : "off") );
   return( SHELL_EXECSTATUS_OK );
}

/*!
 *  \brief Set the sensor config.
 *
 *  \param ppcStringReply Input/Output. The response string. NEVER NULL AS INPUT.
 *  \param ac             Input. Number of args
 *  \param av             Input. pointer to args
 *
 *  \return the status of the command execution.
 */
eExecStatus e_temperature_set_config( signed portCHAR **ppcStringReply, int ac, signed portCHAR *av[] )
{
portCHAR * token;


    if (config_file_set_value(SENSOR_TEMP_CONFIG_FILE, ac, av) != 0)
    {
      *ppcStringReply = (signed portCHAR *)SHELL_ERRMSG_CONFIGERROR;
      // return error
      return( SHELL_EXECSTATUS_KO );
    }

  // alarm field
  if (!strcmp((char *)av[0] , "alarm"))
  {
    if (!strcmp((char *)av[1] , "on"))
    {
      b_temp_alarm = pdTRUE;
      *ppcStringReply = (signed portCHAR *)SENSOR_MSG_ALARM_ON;
      return( SHELL_EXECSTATUS_OK_NO_FREE );
    }
    else if (!strcmp( (char *)av[1], "off"))
    {
      b_temp_alarm = pdFALSE;
      *ppcStringReply = (signed portCHAR *)SENSOR_MSG_ALARM_OFF;
      return( SHELL_EXECSTATUS_OK_NO_FREE );
    }
    else
    {
      *ppcStringReply = (signed portCHAR *)SHELL_ERRMSG_CONFIGERROR;
      return( SHELL_EXECSTATUS_KO );
    }
  }
  // lograte field
  else if (!strcmp((char *)av[0] , "lograte"))
  {
    ul_temp_lograte = atoi((char *)av[1]);
    *ppcStringReply = (signed portCHAR *)SENSOR_MSG_CONFIG_SET;
    return( SHELL_EXECSTATUS_OK_NO_FREE );
  }
  // min field
  else if (!strcmp((char *)av[0] , "min"))
  {
    token = strpbrk((char *)av[1] , "C");
    if (token != NULL)
    {
      *token = '\0';
    }
    l_temp_min = atoi((char *)av[1]);
    *ppcStringReply = (signed portCHAR *)SENSOR_MSG_CONFIG_SET;
    return( SHELL_EXECSTATUS_OK_NO_FREE );
  }
  // max field
  else if (!strcmp((char *)av[0] , "max"))
  {
    token = strpbrk((char *)av[1] , "C");
    if (token != NULL)
    {
      *token = '\0';
    }
    l_temp_max = atoi((char *)av[1]);
    *ppcStringReply = (signed portCHAR *)SENSOR_MSG_CONFIG_SET;
    return( SHELL_EXECSTATUS_OK_NO_FREE );
  }
  // unknown field : error
  else

  {
    *ppcStringReply = (signed portCHAR *)SHELL_ERRMSG_CONFIGERROR;
    return( SHELL_EXECSTATUS_KO );
  }
}

/*!
 *  \brief Get the current temperature value.
 *
 *  \param pxLog a Log structure.
 *
 *  \return true upon success, false if error.
 */
bool b_temperature_get_value( xLogDef *pxLog )
{
   int i_current_val, value, index = 0;


   /* enable channel for sensor */
   adc_enable( adc, ADC_TEMPERATURE_CHANNEL );
   // start conversion
   adc_start( adc );
   // get value for sensor
   value = adc_get_value( adc, ADC_TEMPERATURE_CHANNEL );
   /* Disable channel for sensor */
   adc_disable( adc, ADC_TEMPERATURE_CHANNEL );

   if(value > temperature_code[0])
   {
        i_current_val = -20;
   }
   else
   {
      while(temperature_code[index++] > value);
      i_current_val = (index - 1 - 20);
   }

   // Alloc memory for the log string.
   pxLog->pcStringLog = pvPortMalloc( 12*sizeof( char ) );
   if( NULL == pxLog->pcStringLog )
   {
      return( false );
   }
   pxLog->pfFreeStringLog = vPortFree; // Because pvPortMalloc() was used to
                                       // alloc the log string.

   // Build the log string.
   if( i_current_val <= l_temp_min )
   {
      sprintf( pxLog->pcStringLog, "%3dC | min", i_current_val );
      // if alarms have to be checked and no alarm for min was pending
      if (( b_temp_alarm == pdTRUE ) && ( b_temp_alarm_min == pdFALSE ))
      {
        // alarm has been taken into account,
        // don't reenter this test before leaving min area
        b_temp_alarm_min = pdTRUE;
        // allow alarm if max is reached
        b_temp_alarm_max = pdFALSE;
        // post alarm to SMTP task
        v_SMTP_Post("Min Temp Alarm", NULL);
      }
   }
   else if( i_current_val >= l_temp_max )
   {
      sprintf( pxLog->pcStringLog, "%3dC | max", i_current_val );
      // if alarms have to be checked and no alarm for max was pending
      if (( b_temp_alarm == pdTRUE ) && ( b_temp_alarm_max == pdFALSE ))
      {
        // alarm has been taken into account,
        // don't reenter this test before leaving max area
        b_temp_alarm_max = pdTRUE;
        // allow alarm if min is reached
        b_temp_alarm_min = pdFALSE;
        // post alarm to SMTP task
        v_SMTP_Post("Max Temp Alarm", NULL);
      }
   }
   else
   {
      sprintf( pxLog->pcStringLog, "%3dC", i_current_val );
      // if alarms have to be checked
      if ( b_temp_alarm == pdTRUE )
      {
        // no alarm is pending
        b_temp_alarm_max = pdFALSE;
        b_temp_alarm_min = pdFALSE;
      }
   }

   return( true );
}
