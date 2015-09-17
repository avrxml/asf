/*****************************************************************************
 *
 * \file
 *
 * \brief AVR32 UC3 Control Panel joystick sensor module.
 *
 * - Supported boards:   EVK1100
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
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

//_____  I N C L U D E S ___________________________________________________
#include <string.h>

#include "compiler.h"
#include "gpio.h" // Have to include gpio.h before FreeRTOS.h as long as FreeRTOS
                  // redefines the inline keyword to empty.
#include "FreeRTOS.h"
#include "task.h"

#include "datalog.h"
#include "shell.h"
#include "board.h"
#include "sensor.h"
#include "mmi.h"
#include "pushb.h"

#include "config_file.h"
#include "fsaccess.h"

#include "BasicSMTP.h"

//_____ M A C R O S ________________________________________________________

/*! Max string length of a get config. */
#define PUSHB_GETCONF_MAXLEN   12

/*! Pushbutton events. */
#define PUSHB_EVENT_PRESS      true
#define PUSHB_EVENT_RELEASE    false

#define SENSOR_PB1_CONFIG_FILE    "A:/CFG/PB1.TXT"
#define SENSOR_PB2_CONFIG_FILE    "A:/CFG/PB2.TXT"
#define SENSOR_PB3_CONFIG_FILE    "A:/CFG/PB3.TXT"

#define PB1_POSITION              GPIO_PUSH_BUTTON_0
#define PB2_POSITION              GPIO_PUSH_BUTTON_1
#define PB3_POSITION              GPIO_PUSH_BUTTON_2


//_____ D E F I N I T I O N S ______________________________________________

/*! Pushbutton events string. */
const signed portCHAR *acpc_pushb_events[2] = {
   (signed portCHAR *)"RELEASE",
   (signed portCHAR *)"PRESS"
};

/*! Pushbutton events emulation. */
static bool b_pushb1 = PUSHB_EVENT_RELEASE;
static bool b_pushb2 = PUSHB_EVENT_RELEASE;
static bool b_pushb3 = PUSHB_EVENT_RELEASE;

/*! Alarm upon event. */
static bool bAlarm1 = pdFALSE;
static bool bAlarm2 = pdFALSE;
static bool bAlarm3 = pdFALSE;

/*! The CFG system mutex. */
extern xSemaphoreHandle   xCFGMutex;


//_____ D E C L A R A T I O N S ____________________________________________
/* interrupt handler. */
#if defined(__GNUC__)
__attribute__((__naked__))
#elif defined(__ICCAVR32__)
#pragma shadow_registers = full   // Naked.
#endif
void vpushb_ISR( void );

/* soft interrupt handler. where treatment should be done */
#ifdef __GNUC__
__attribute__((__noinline__))
#endif
static portBASE_TYPE prvpushb_ISR_NonNakedBehaviour( void );

static eExecStatus prv_e_pushb_get_config( signed portCHAR **ppcStringReply,
                                           bool bAlarm );

static eExecStatus prv_e_pushb_set_config(bool * bAlarm, portCHAR * filename, signed portCHAR **ppcStringReply, int ac, signed portCHAR *av[] );



/*!
 *  \brief Init the push button 1 sensor.
 *
 *  \return true upon success, false if error.
 */
bool b_pushb1_init ( void )
{
portCHAR token[6];

   // Get the xCFGMutex.
   if( pdTRUE == x_supervisor_SemaphoreTake( xCFGMutex, 20 ) )
   {
       // get the field
       if (config_file_get_value(SENSOR_PB1_CONFIG_FILE, "alarm" , token) >= 0)
       {
         // update value
         if (!strcmp(token, "on"))
         {
           bAlarm1 = pdTRUE;
         }
       }
     // Release the xCFGMutex.
     x_supervisor_SemaphoreGive( xCFGMutex );
   }
   /* configure push button to produce IT on input change */
   gpio_enable_pin_interrupt(PB1_POSITION , GPIO_PIN_CHANGE);
   /* Disable all interrupts */
   Disable_global_interrupt();
   /* register push button 1 handler on level 3 */
   INTC_register_interrupt( (__int_handler)&vpushb_ISR, AVR32_GPIO_IRQ_0 + (PB1_POSITION/8), AVR32_INTC_INT3);
   /* Enable all interrupts */
   Enable_global_interrupt();
   return (true);
}

/*!
 * \brief Stop the push button 1 sensor.
 */
void v_pushb1_stop( void )
{
  /* Disable the IT. */
  gpio_disable_pin_interrupt( PB1_POSITION );
}

/*!
 *  \brief Init the push button 2 sensor.
 *
 *  \return true upon success, false if error.
 */
bool b_pushb2_init ( void )
{
portCHAR token[6];

   // Get the xCFGMutex.
   if( pdTRUE == x_supervisor_SemaphoreTake( xCFGMutex, 20 ) )
   {
       // get the field
       if (config_file_get_value(SENSOR_PB2_CONFIG_FILE, "alarm" , token) >= 0)
       {
         // update value
         if (!strcmp(token, "on"))
         {
           bAlarm2 = pdTRUE;
         }
       }
     // Release the xCFGMutex.
     x_supervisor_SemaphoreGive( xCFGMutex );
   }
   /* configure push button to produce IT on input change */
   gpio_enable_pin_interrupt(PB2_POSITION , GPIO_PIN_CHANGE);
   /* Disable all interrupts */
   Disable_global_interrupt();
   /* register push button 2 handler on level 3 */
   INTC_register_interrupt( (__int_handler)&vpushb_ISR, AVR32_GPIO_IRQ_0 + (PB2_POSITION/8), AVR32_INTC_INT3);
   /* Enable all interrupts */
   Enable_global_interrupt();
   return (true);
}

/*!
 * \brief Stop the push button 2 sensor.
 */
void v_pushb2_stop( void )
{
  /* Disable the IT. */
  gpio_disable_pin_interrupt( PB2_POSITION );
}

/*!
 *  \brief Init the push button 3 sensor.
 *
 *  \return true upon success, false if error.
 */
bool b_pushb3_init ( void )
{
portCHAR token[6];

   // Get the xCFGMutex.
   if( pdTRUE == x_supervisor_SemaphoreTake( xCFGMutex, 20 ) )
   {
       // get the field
       if (config_file_get_value(SENSOR_PB3_CONFIG_FILE, "alarm" , token) >= 0)
       {
         // update value
         if (!strcmp(token, "on"))
         {
           bAlarm3 = pdTRUE;
         }
       }
     // Release the xCFGMutex.
     x_supervisor_SemaphoreGive( xCFGMutex );
   }
   /* configure push button to produce IT on input change */
   gpio_enable_pin_interrupt(PB3_POSITION , GPIO_PIN_CHANGE);
   /* Disable all interrupts */
   Disable_global_interrupt();
   /* register push button 3 handler on level 3 */
   INTC_register_interrupt( (__int_handler)&vpushb_ISR, AVR32_GPIO_IRQ_0 + (PB3_POSITION/8), AVR32_INTC_INT3);
   /* Enable all interrupts */
   Enable_global_interrupt();
   return (true);
}

/*!
 * \brief Stop the push button 3 sensor.
 */
void v_pushb3_stop( void )
{
  /* Disable the IT. */
  gpio_disable_pin_interrupt( PB3_POSITION );
}

/*!
 *  \brief Get the pushbutton1 sensor config.
 *
 *  \param ppcStringReply Input/Output. The response string. NEVER NULL AS INPUT.
 *                        A malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_pushb1_get_config( signed portCHAR **ppcStringReply )
{
   return( prv_e_pushb_get_config( ppcStringReply, bAlarm1 ) );
}


/*!
 *  \brief Get the pushbutton2 sensor config.
 *
 *  \param ppcStringReply Input/Output. The response string. NEVER NULL AS INPUT.
 *                        A malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_pushb2_get_config( signed portCHAR **ppcStringReply )
{
   return( prv_e_pushb_get_config( ppcStringReply, bAlarm2 ) );
}


/*!
 *  \brief Get the pushbutton3 sensor config.
 *
 *  \param ppcStringReply Input/Output. The response string. NEVER NULL AS INPUT.
 *                        A malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_pushb3_get_config( signed portCHAR **ppcStringReply )
{
   return( prv_e_pushb_get_config( ppcStringReply, bAlarm3 ) );
}


/*! \brief set push button config command: set the config fields value of a sensor.
 *         Takes one parameter, that is the sensor's name.
 *         Format: set_sensor_config sensor=name param=value
 *
 *  \param ac             Input. The argument counter. For this command, should be 2.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_pushb1_set_config( signed portCHAR **ppcStringReply, int ac, signed portCHAR *av[] )
{
  return (prv_e_pushb_set_config(&bAlarm1, SENSOR_PB1_CONFIG_FILE, ppcStringReply, ac, av));
}

/*! \brief set push button config command: set the config fields value of a sensor.
 *         Takes one parameter, that is the sensor's name.
 *         Format: set_sensor_config sensor=name param=value
 *
 *  \param ac             Input. The argument counter. For this command, should be 2.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_pushb2_set_config( signed portCHAR **ppcStringReply, int ac, signed portCHAR *av[] )
{
  return (prv_e_pushb_set_config(&bAlarm2, SENSOR_PB2_CONFIG_FILE, ppcStringReply, ac, av));
}

/*! \brief set push button config command: set the config fields value of a sensor.
 *         Takes one parameter, that is the sensor's name.
 *         Format: set_sensor_config sensor=name param=value
 *
 *  \param ac             Input. The argument counter. For this command, should be 2.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_pushb3_set_config( signed portCHAR **ppcStringReply, int ac, signed portCHAR *av[] )
{
  return (prv_e_pushb_set_config(&bAlarm3, SENSOR_PB3_CONFIG_FILE, ppcStringReply, ac, av));
}

/*!
 *  \brief Get a pushbutton current state.
 *
 *  \param pxLog a Log structure. The id field identifies the pushbutton.
 *
 *  \return true upon success, false if error.
 */
bool b_pushb1_get_value( xLogDef *pxLog )
{
   // Build the log string.
   pxLog->pfFreeStringLog = NULL; // We use a constant.
   pxLog->pcStringLog = (portCHAR *)acpc_pushb_events[b_pushb1];

   return( true );
}

/*!
 *  \brief Get a pushbutton current state.
 *
 *  \param pxLog a Log structure. The id field identifies the pushbutton.
 *
 *  \return true upon success, false if error.
 */
bool b_pushb2_get_value( xLogDef *pxLog )
{
   // Build the log string.
   pxLog->pfFreeStringLog = NULL; // We use a constant.
   pxLog->pcStringLog = (portCHAR *)acpc_pushb_events[b_pushb2];

   return( true );
}

/*!
 *  \brief Get a pushbutton current state.
 *
 *  \param pxLog a Log structure. The id field identifies the pushbutton.
 *
 *  \return true upon success, false if error.
 */
bool b_pushb3_get_value( xLogDef *pxLog )
{
   // Build the log string.
   pxLog->pfFreeStringLog = NULL; // We use a constant.
   pxLog->pcStringLog = (portCHAR *)acpc_pushb_events[b_pushb3];

   return( true );
}

/*****************************************************************************/
/******************************* PRIVATE *************************************/
/*****************************************************************************/


/*! \brief The get config command: get the config fields value of a sensor.
 *
 *  \param bAlarm         Input. the alarm value to return.
 *  \param ppcStringReply Input/Output. The response string.
 *
 *  \return the status of the command execution.
 */
static eExecStatus prv_e_pushb_get_config( signed portCHAR **ppcStringReply,
                                           bool bAlarm )
{
   if ( bAlarm == true )
     *ppcStringReply = (signed portCHAR *)SENSOR_MSG_ALARM_ON;
   else
     *ppcStringReply = (signed portCHAR *)SENSOR_MSG_ALARM_OFF;
   return( SHELL_EXECSTATUS_OK_NO_FREE );
}

/*! \brief set push button config command: set the config fields value of a sensor.
 *
 *  \param bAlarm         Input. The flag to set or clear.
 *  \param filename       Input. The configuration filename to update.
 *  \param ppcStringReply Input/Output. The response string.
 *  \param ac             Input. The argument counter. For this command, should be 2.
 *  \param av             Input. The argument vector.
 *
 *  \return the status of the command execution.
 */
static eExecStatus prv_e_pushb_set_config(bool * bAlarm, portCHAR * filename, signed portCHAR **ppcStringReply, int ac, signed portCHAR *av[] )
{
  if (config_file_set_value(filename, ac, av) != 0)
  {
    *ppcStringReply = (signed portCHAR *)SHELL_ERRMSG_CONFIGERROR;
    // return error
    return( SHELL_EXECSTATUS_KO );
  }
  if (!strcmp((char *)av[1] , "on"))
  {
    *bAlarm = pdTRUE;
    *ppcStringReply = (signed portCHAR *)SENSOR_MSG_ALARM_ON;
    return( SHELL_EXECSTATUS_OK_NO_FREE );
  }
  else if (!strcmp( (char *)av[1], "off"))
  {
    *bAlarm = pdFALSE;
    *ppcStringReply = (signed portCHAR *)SENSOR_MSG_ALARM_OFF;
    return( SHELL_EXECSTATUS_OK_NO_FREE );
  }
  else
  {
    *ppcStringReply = (signed portCHAR *)SHELL_ERRMSG_CONFIGERROR;
    return( SHELL_EXECSTATUS_KO );
  }
}

/*! \brief push button naked interrupt handler.
 *
 */
#if defined(__GNUC__)
__attribute__((__naked__))
#elif defined(__ICCAVR32__)
#pragma shadow_registers = full   // Naked.
#endif
void vpushb_ISR( void )
{
 /* This ISR can cause a context switch, so the first statement must be a
     call to the portENTER_SWITCHING_ISR() macro.  This must be BEFORE any
     variable declarations. */
  portENTER_SWITCHING_ISR();

  prvpushb_ISR_NonNakedBehaviour();

  portEXIT_SWITCHING_ISR();
}

/*! \brief push button interrupt handler. Here, declarations should be done
 *
 */
#if defined(__GNUC__)
__attribute__((__noinline__))
#elif defined(__ICCAVR32__)
#pragma optimize = no_inline
#endif
static portBASE_TYPE prvpushb_ISR_NonNakedBehaviour( void )
{
xLogDef *pxLog;
portBASE_TYPE xSwitchRequired = pdFALSE;

  /* depending on the push buttons */
  if (gpio_get_pin_interrupt_flag(PB1_POSITION))
  {
      /* input is pulled up, if 1 : input is not active */
      if (gpio_get_pin_value(PB1_POSITION))
      {
         b_pushb1 = PUSHB_EVENT_RELEASE;
      }
      else
      {
         b_pushb1 = PUSHB_EVENT_PRESS;
         if ( bAlarm1 == pdTRUE )
         {
           // post alarm to SMTP task
           v_SMTP_PostFromISR("Push Button 1 Alarm", NULL);
         }
      }
      // Alloc and init a log.
      pxLog = pxdatalog_log_alloc_init_FromISR();
      if( NULL != pxLog )
      {
         // Init the id field of the log.
         pxLog->id = DATALOG_ID_PB1;
         /* set log event string */
         pxLog->pcStringLog = (portCHAR *)acpc_pushb_events[b_pushb1];
         /* set free function pointer */
         pxLog->pfFreeStringLog = NULL;
         /* add the log entry */
         xSwitchRequired = x_datalog_AddLogFromISR( pxLog );
      }
      /* allow new interrupt : clear the IFR flag */
      gpio_clear_pin_interrupt_flag(PB1_POSITION);
  }
  if (gpio_get_pin_interrupt_flag(PB2_POSITION))
  {
      /* input is pulled up, if 1 : input is not active */
      if (gpio_get_pin_value(PB2_POSITION))
      {
         b_pushb2 = PUSHB_EVENT_RELEASE;
      }
      else
      {
         b_pushb2 = PUSHB_EVENT_PRESS;
         if ( bAlarm2 == pdTRUE )
         {
           // post alarm to SMTP task
           v_SMTP_PostFromISR("Push Button 2 Alarm", NULL);
         }
      }
      // Alloc and init a log.
      pxLog = pxdatalog_log_alloc_init_FromISR();
      if( NULL != pxLog )
      {
         // Init the id field of the log.
         pxLog->id = DATALOG_ID_PB2;
         /* set log event string */
         pxLog->pcStringLog = (portCHAR *)acpc_pushb_events[b_pushb2];
         /* set free function pointer */
         pxLog->pfFreeStringLog = NULL;
         /* add the log entry */
         xSwitchRequired = x_datalog_AddLogFromISR( pxLog );
      }
      /* allow new interrupt : clear the IFR flag */
      gpio_clear_pin_interrupt_flag(PB2_POSITION);
  }
  if (gpio_get_pin_interrupt_flag(PB3_POSITION))
  {
      /* input is pulled up, if 1 : input is not active */
      if (gpio_get_pin_value(PB3_POSITION))
      {
         b_pushb3 = PUSHB_EVENT_RELEASE;
      }
      else
      {
         b_pushb3 = PUSHB_EVENT_PRESS;
         if ( bAlarm3 == pdTRUE )
         {
           // post alarm to SMTP task
           v_SMTP_PostFromISR("Push Button 3 Alarm", NULL);
         }
      }
      // Alloc and init a log.
      pxLog = pxdatalog_log_alloc_init_FromISR();
      if( NULL != pxLog )
      {
         // Init the id field of the log.
         pxLog->id = DATALOG_ID_PB3;
         /* set log event string */
         pxLog->pcStringLog = (portCHAR *)acpc_pushb_events[b_pushb3];
         /* set free function pointer */
         pxLog->pfFreeStringLog = NULL;
         /* add the log entry */
         xSwitchRequired = x_datalog_AddLogFromISR( pxLog );
      }
      /* allow new interrupt : clear the IFR flag */
      gpio_clear_pin_interrupt_flag(PB3_POSITION);
  }
  return( xSwitchRequired );
}
