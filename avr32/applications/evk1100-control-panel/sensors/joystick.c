/*****************************************************************************
 *
 * \file
 *
 * \brief AVR32 UC3 Control Panel joystick sensor module.
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

#include "gpio.h" // Have to include gpio.h before FreeRTOS.h as long as FreeRTOS
                  // redefines the inline keyword to empty.

#include "FreeRTOS.h"
#include "task.h"

#include "joystick.h"

#include "datalog.h"
#include "shell.h"
#include "board.h"
#include "sensor.h"
#include "config_file.h"
#include "fsaccess.h"

#include "mmi.h"

#include "BasicSMTP.h"

//_____ M A C R O S ________________________________________________________

/*! Max string length of a get config. */
#define JS_GETCONF_MAXLEN   16

/*! Max number of events on joystick. */
#define JS_NB_EVENT         32

/*! Mask for events on joystick. */
#define JS_EVENT_PUSH          0x01
#define JS_EVENT_LEFT          0x02
#define JS_EVENT_RIGHT         0x04
#define JS_EVENT_UP            0x08
#define JS_EVENT_DOWN          0x10

/*! Path to the config file. */
#define SENSOR_JS_CONFIG_FILE    "A:/CFG/JS.TXT"

//_____ D E F I N I T I O N S ______________________________________________

/*! Joystick events string, regarding bit field value. */
const signed portCHAR *acpc_js_events[JS_NB_EVENT] = {
   (signed portCHAR *)"RELEASE",(signed portCHAR *)"PRESS",(signed portCHAR *)"LEFT",(signed portCHAR *)"LEFT/PRESS",
   (signed portCHAR *)"RIGHT",(signed portCHAR *)"RIGHT/PRESS",(signed portCHAR *)"?",(signed portCHAR *)"?",
   (signed portCHAR *)"UP",(signed portCHAR *)"UP/PRESS",(signed portCHAR *)"UP/LEFT",(signed portCHAR *)"UP/LEFT/PRESS",
   (signed portCHAR *)"UP/RIGHT",(signed portCHAR *)"UP/RIGHT/PRESS",(signed portCHAR *)"?",(signed portCHAR *)"?",
   (signed portCHAR *)"DOWN",(signed portCHAR *)"DOWN/PRESS",(signed portCHAR *)"DOWN/LEFT",(signed portCHAR *)"DOWN/LEFT/PRESS",
   (signed portCHAR *)"DOWN/RIGHT",(signed portCHAR *)"DOWN/RIGHT/PRESS",(signed portCHAR *)"?",(signed portCHAR *)"?",
   (signed portCHAR *)"?",(signed portCHAR *)"?",(signed portCHAR *)"?",(signed portCHAR *)"?",
   (signed portCHAR *)"?",(signed portCHAR *)"?",(signed portCHAR *)"?",(signed portCHAR *)"?",
};

/*! Joystick events. */
static unsigned short x_joystick = 0;

/*! Alarm upon event. */
static bool bAlarm = false;

/*! Debouncing variable. */
static U32 u32LastCountValue = 0;

/*! The CFG system mutex. */
extern xSemaphoreHandle   xCFGMutex;

//_____ D E C L A R A T I O N S ____________________________________________

/* interrupt handler. */
#if defined(__GNUC__)
__attribute__((__naked__))
#elif defined(__ICCAVR32__)
#pragma shadow_registers = full   // Naked.
#endif
void vjoystick_ISR( void );

/* soft interrupt handler. where treatment should be done */
#ifdef __GNUC__
__attribute__((__noinline__))
#endif
static portBASE_TYPE prvjoystick_ISR_NonNakedBehaviour( void );


/*!
 *  \brief Init the joystick sensor.
 *
 *  \return true upon success, false if error.
 */
bool b_joystick_init ( void )
{
portCHAR token[6];

   // Get the xCFGMutex.
   if( pdTRUE == x_supervisor_SemaphoreTake( xCFGMutex, 0 ) )
   {
       // get the field
       if (config_file_get_value(SENSOR_JS_CONFIG_FILE, "alarm" , token) >= 0)
       {
         // update value
         if (!strcmp(token, "on"))
         {
           bAlarm = pdTRUE;
         }
       }
     // Release the xCFGMutex.
     x_supervisor_SemaphoreGive( xCFGMutex );
   }
   /* configure joystick up to produce IT on all state change */
   gpio_enable_pin_interrupt(GPIO_JOYSTICK_UP , GPIO_PIN_CHANGE);
   /* configure joystick down to produce IT on all state change */
   gpio_enable_pin_interrupt(GPIO_JOYSTICK_DOWN , GPIO_PIN_CHANGE);
   /* configure joystick right to produce IT on all state change */
   gpio_enable_pin_interrupt(GPIO_JOYSTICK_RIGHT , GPIO_PIN_CHANGE);
   /* configure joystick left to produce IT on all state change */
   gpio_enable_pin_interrupt(GPIO_JOYSTICK_LEFT , GPIO_PIN_CHANGE);
   /* configure joystick press to produce IT on all state change */
   gpio_enable_pin_interrupt(GPIO_JOYSTICK_PUSH , GPIO_PIN_CHANGE);
   /* Disable all interrupts */
   Disable_global_interrupt();
   /* register joystick handler on level 3 */
   INTC_register_interrupt( (__int_handler)&vjoystick_ISR, AVR32_GPIO_IRQ_0 + (GPIO_JOYSTICK_UP/8), AVR32_INTC_INT3);
   INTC_register_interrupt( (__int_handler)&vjoystick_ISR, AVR32_GPIO_IRQ_0 + (GPIO_JOYSTICK_PUSH/8), AVR32_INTC_INT3);
   /* Enable all interrupts */
   Enable_global_interrupt();
   return (true);
}

/*!
 * \brief Stop the joystick sensor.
 */
void v_joystick_stop( void )
{
  /* Disable joystick input change ITs. */
  gpio_disable_pin_interrupt( GPIO_JOYSTICK_UP );
  gpio_disable_pin_interrupt( GPIO_JOYSTICK_DOWN );
  gpio_disable_pin_interrupt( GPIO_JOYSTICK_RIGHT );
  gpio_disable_pin_interrupt( GPIO_JOYSTICK_LEFT );
  gpio_disable_pin_interrupt( GPIO_JOYSTICK_PUSH );
}


/*!
 *  \brief Get the joystick sensor config.
 *
 *  \param ppcStringReply Input/Output. The response string. NEVER NULL AS INPUT.
 *                        A malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_joystick_get_config( signed portCHAR **ppcStringReply )
{
   // if alarm is set, set status to ON
   if ( bAlarm == true )
     *ppcStringReply = (signed portCHAR *)SENSOR_MSG_ALARM_ON;
   else
     // set status to OFF
     *ppcStringReply = (signed portCHAR *)SENSOR_MSG_ALARM_OFF;
   return( SHELL_EXECSTATUS_OK_NO_FREE );
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
eExecStatus e_joystick_set_config( signed portCHAR **ppcStringReply, int ac, signed portCHAR *av[] )
{

  // set the field value
  if (config_file_set_value(SENSOR_JS_CONFIG_FILE, ac, av) != 0)
  {
    *ppcStringReply = (signed portCHAR *)SHELL_ERRMSG_CONFIGERROR;
    // return error
    return( SHELL_EXECSTATUS_KO );
  }
  // alarm=on
  if (!strcmp((char *)av[1] , "on"))
  {
    bAlarm = pdTRUE;
    *ppcStringReply = (signed portCHAR *)SENSOR_MSG_ALARM_ON;
    return( SHELL_EXECSTATUS_OK_NO_FREE );
  }
  // alarm=off
  else if (!strcmp( (char *)av[1], "off"))
  {
    bAlarm = pdFALSE;
    *ppcStringReply = (signed portCHAR *)SENSOR_MSG_ALARM_OFF;
    return( SHELL_EXECSTATUS_OK_NO_FREE );
  }
  // error
  else
  {
    *ppcStringReply = (signed portCHAR *)SHELL_ERRMSG_CONFIGERROR;
    return( SHELL_EXECSTATUS_KO );
  }
}

/*!
 *  \brief Get the current joystick state.
 *
 *  \param pxLog a Log structure.
 *
 *  \return true upon success, false if error.
 */
bool b_joystick_get_value( xLogDef *pxLog )
{

   // Build the log string.
   pxLog->pfFreeStringLog = NULL; // We use a constant.
   pxLog->pcStringLog = (portCHAR *)acpc_js_events[x_joystick];

   return( true );
}


/*! \brief joystick naked interrupt handler.
 *
 */
#if defined(__GNUC__)
__attribute__((__naked__))
#elif defined(__ICCAVR32__)
#pragma shadow_registers = full   // Naked.
#endif
void vjoystick_ISR( void )
{
 /* This ISR can cause a context switch, so the first statement must be a
     call to the portENTER_SWITCHING_ISR() macro.  This must be BEFORE any
     variable declarations. */
  portENTER_SWITCHING_ISR();

  prvjoystick_ISR_NonNakedBehaviour();

  portEXIT_SWITCHING_ISR();
}

/*! \brief joystick interrupt handler. Here, declarations should be done
 *
 */
#if defined(__GNUC__)
__attribute__((__noinline__))
#elif defined(__ICCAVR32__)
#pragma optimize = no_inline
#endif
static portBASE_TYPE prvjoystick_ISR_NonNakedBehaviour( void )
{
  xLogDef *pxLog;
  U32     u32CurrentCountValue = xTaskGetTickCount();


  // Debouncing: ignore joystick events occuring in less than 250ms before the last
  // valid joystick event.
  if((( u32CurrentCountValue >= u32LastCountValue )
        && ( u32CurrentCountValue - u32LastCountValue <= 250))
      ||
       (( u32CurrentCountValue < u32LastCountValue )
        && ( u32CurrentCountValue + (0xFFFFFFFF - u32LastCountValue) <= 250)))
  {
    // Clear all interrupts flag (it's no use checking which event has bouncing).
    gpio_clear_pin_interrupt_flag(GPIO_JOYSTICK_PUSH);
    Clr_bits(x_joystick, JS_EVENT_PUSH);
    gpio_clear_pin_interrupt_flag(GPIO_JOYSTICK_LEFT);
    Clr_bits(x_joystick, JS_EVENT_LEFT);
    gpio_clear_pin_interrupt_flag(GPIO_JOYSTICK_RIGHT);
    Clr_bits(x_joystick, JS_EVENT_RIGHT);
    gpio_clear_pin_interrupt_flag(GPIO_JOYSTICK_UP);
    Clr_bits(x_joystick, JS_EVENT_UP);
    gpio_clear_pin_interrupt_flag(GPIO_JOYSTICK_DOWN);
    Clr_bits(x_joystick, JS_EVENT_DOWN);
    return(pdFALSE);
  }
  else
  {
    // Update last valid joystick event timestamp.
    u32LastCountValue = u32CurrentCountValue;

    // Check all events.
    if (gpio_get_pin_interrupt_flag(GPIO_JOYSTICK_UP))
    {
       /* input is pulled up, if 1 : input is not active */
       if (gpio_get_pin_value(GPIO_JOYSTICK_UP))
       {
         /* clear bit UP */
         Clr_bits(x_joystick, JS_EVENT_UP);
       }
       else
       {
         Set_bits(x_joystick, JS_EVENT_UP);
       }
       /* allow new interrupt : clear the IFR flag */
       gpio_clear_pin_interrupt_flag(GPIO_JOYSTICK_UP);
     }
     if (gpio_get_pin_interrupt_flag(GPIO_JOYSTICK_DOWN))
     {
       /* input is pulled up, if 1 : input is not active */
       if (gpio_get_pin_value(GPIO_JOYSTICK_DOWN))
       {
         Clr_bits(x_joystick, JS_EVENT_DOWN);
       }
       else
       {
         Set_bits(x_joystick, JS_EVENT_DOWN);
       }
       /* allow new interrupt : clear the IFR flag */
       gpio_clear_pin_interrupt_flag(GPIO_JOYSTICK_DOWN);
     }
     if (gpio_get_pin_interrupt_flag(GPIO_JOYSTICK_LEFT))
     {
       /* input is pulled up, if 1 : input is not active */
       if (gpio_get_pin_value(GPIO_JOYSTICK_LEFT))
       {
         Clr_bits(x_joystick, JS_EVENT_LEFT);
       }
       else
       {
         Set_bits(x_joystick, JS_EVENT_LEFT);
   #ifdef MMILCD_ENABLE
         vMMI_UserMenuDisplayPreviousItem(pdTRUE);
   #endif
       }
       /* allow new interrupt : clear the IFR flag */
       gpio_clear_pin_interrupt_flag(GPIO_JOYSTICK_LEFT);
     }
     if (gpio_get_pin_interrupt_flag(GPIO_JOYSTICK_RIGHT))
     {
       /* input is pulled up, if 1 : input is not active */
       if (gpio_get_pin_value(GPIO_JOYSTICK_RIGHT))
       {
         Clr_bits(x_joystick, JS_EVENT_RIGHT);
       }
       else
       {
         Set_bits(x_joystick, JS_EVENT_RIGHT);
   #ifdef MMILCD_ENABLE
         vMMI_UserMenuDisplayNextItem(pdTRUE);
   #endif
       }
       /* allow new interrupt : clear the IFR flag */
       gpio_clear_pin_interrupt_flag(GPIO_JOYSTICK_RIGHT);
     }
     if (gpio_get_pin_interrupt_flag(GPIO_JOYSTICK_PUSH))
     {
       /* input is pulled up, if 1 : input is not active */
       if (gpio_get_pin_value(GPIO_JOYSTICK_PUSH))
       {
         Clr_bits(x_joystick, JS_EVENT_PUSH);
       }
       else
       {
         Set_bits(x_joystick, JS_EVENT_PUSH);
         if ( bAlarm == pdTRUE )
         {
           // post alarm to SMTP task
           v_SMTP_PostFromISR("Joystick Alarm", NULL);
         }

   #ifdef MMILCD_ENABLE
         vMMI_UserMenuValidateItem(pdTRUE);
   #endif
       }
       /* allow new interrupt : clear the IFR flag */
       gpio_clear_pin_interrupt_flag(GPIO_JOYSTICK_PUSH);
     }
  }
  // Alloc and init a log.
  pxLog = pxdatalog_log_alloc_init_FromISR();
  if( NULL == pxLog )
    return( pdFALSE );
  // Init the id field of the log.
  pxLog->id = DATALOG_ID_JS;
  /* set log event string */
  pxLog->pcStringLog = (portCHAR *)acpc_js_events[x_joystick];
  /* set free function pointer */
  pxLog->pfFreeStringLog = NULL;
  /* add the log entry */
  return( x_datalog_AddLogFromISR( pxLog ) );
}
