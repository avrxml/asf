/*****************************************************************************
 *
 * \file
 *
 * \brief Control Panel supervisor module.
 *
 * The supervisor module:
 *     * initializes the dump port COM2,
 *     * increments the raw time integer value every second,
 *     * TODO : add its other jobs.
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

#include <stdlib.h>
#include <stdio.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"

/* Demo program include files. */
#include "conf_ctrlpanel.h"
#include "tracedump.h"

#include "navigation.h"
#include "com1shell.h"
#include "datalog.h"
#include "cptime.h"
#include "sensor.h"

#include "mmi.h"

#include "shell.h"
#include "syscmds.h"

#include "fsaccess.h"

// #include "wdt.h"

#ifdef NW_INTEGRATED_IN_CONTROL_PANEL
#include "ethernet.h"
#include "netif/etharp.h"
#include "BasicWEB.h"
#endif

#ifdef USB_ENABLE
#include "conf_usb.h"
#include "usbsys.h"
#include "usb_task.h"
#include "usb_standard_request.h"
#include "usb_drv.h"
#endif

//_____ M A C R O S ________________________________________________________

//! @defgroup sup_local_def Supervisor specific definitions.
//!
//! @{
//! The Sentinel task stack size.
#define SUPERVISOR_STACK_SIZE	        ( configMINIMAL_STACK_SIZE + 192 )

//! The supervisor rate in ticks. Since the supervisor task is in charge of updating
//! the local time every second, this rate must be less or equal to 1000.
#define SUPERVISOR_DEFAULT_PERIOD       ( ( portTickType ) 100 / portTICK_RATE_MS  ) // Every 100ms

//! Return status true
#define SUPERVISOR_CHECK_OK	        0
//! Return status false
#define SUPERVISOR_CHECK_KO	        (-1)

//! The delay between each heap usage check.
#define SUPERVISOR_DELAY_HEAPCHECK      600  // once per mn.

//! The delay between each time update.
#define SUPERVISOR_DELAY_TIMEUPDATE     10  // once per sec.

#if DISPLAY_MMI_SECOND == 1
//! The delay between each time update on the MMI.
#define SUPERVISOR_DELAY_MMI_TIMEUPDATE 10 // once per sec.
#else
//! The delay between each time update on the MMI.
#define SUPERVISOR_DELAY_MMI_TIMEUPDATE 600 // once per min.
#endif

//! The delay between each tasks list display.
#define SUPERVISOR_DELAY_PRINTTASKLIST 600 // every mn.

//! Priority definitions for most of the tasks in the demo application. Some
//! tasks just use the idle priority.
#define mainCOMSH_TASK_PRIORITY        ( tskIDLE_PRIORITY + 2 ) // TODO: +1?
#define mainETH_TASK_PRIORITY          ( tskIDLE_PRIORITY + 1 )
#define mainDATALOG_TASK_PRIORITY      ( tskIDLE_PRIORITY + 2 )

//! The supervisor User Action stack size.
#define SUPERVISOR_USER_ACTION_STACK_SIZE      (configMINIMAL_STACK_SIZE+64)

//! The supervisor User Action task priority.
#define SUPERVISOR_USER_ACTION_TASK_PRIORITY   ( tskIDLE_PRIORITY + 2 )

//! The supervisor queue length.
#define SUPERVISOR_QUEUE_SIZE           1

//! The watchdog timeout delay.
// #define SUPERVISOR_WDT_TIMEOUT  ((unsigned long long)((30*SUPERVISOR_DEFAULT_PERIOD)*1000)) // 3000ms
//! @}

#ifdef USB_ENABLE

//! @defgroup sup_usb_roles USB operating modes
//!
//! @{
//! No USB connection
#define SUPERVISOR_USB_ROLE_NONE                0
//! The Control Panel is acting as a USB device
#define SUPERVISOR_USB_ROLE_DEVICE              1
//! The Control Panel is acting as a USB host
#define SUPERVISOR_USB_ROLE_HOST                2
//! @}

#if USB_DEVICE_FEATURE == true
//! The number of system mutexes to take before switching to maintenance mode.
#if NW_INTEGRATED_IN_CONTROL_PANEL
#define SUPERVISOR_MAINTENANCE_NBMUTEX_TOTAKE            4
#else
#define SUPERVISOR_MAINTENANCE_NBMUTEX_TOTAKE            3
#endif
#endif
#endif

//_____ D E F I N I T I O N S ______________________________________________

/*! The local time raw value. */
extern time_t xcptime_LocalTime;

eExecStatus e_supervisor_switch_to_maintenance_mode( eModId xModId,
signed short FsNavId,
int ac, signed portCHAR *av[],
signed portCHAR **ppcStringReply );

#if defined(__GNUC__)
extern void __heap_start__;
extern void __heap_end__;

portBASE_TYPE *pxHighestHeapAddr = (portBASE_TYPE *)&__heap_start__;

void prvCheckMallocHeap( void );
#endif

#ifdef MMILCD_ENABLE
/*! MMI User Action task handle. */
static xTaskHandle   xSupervisorUserActionHndl = NULL;
#endif

#ifdef USB_ENABLE
#if USB_DEVICE_FEATURE == true
/*! The USB system mutex. */
xSemaphoreHandle     xUSBMutex;
#endif
#endif

#if NW_INTEGRATED_IN_CONTROL_PANEL
/*! The Web server system mutex. */
xSemaphoreHandle     xWEBMutex;
#endif

/*! The LOG system mutex. */
xSemaphoreHandle     xLOGMutex;

/*! The SHELL system mutex. */
xSemaphoreHandle     xSHELLFSMutex;

/*! The CFG system mutex. */
xSemaphoreHandle     xCFGMutex;

#ifdef USB_ENABLE
#if USB_DEVICE_FEATURE == true
/*! Maintenance mode flag. */
static bool          bIsInMaintenance = false;

/*! Out-of-Maintenance mode flag. */
static bool          bOutOfMaintenance = false;

/*! The Control Panel current USB role. */
static U8            u8CurrentUsbRole = SUPERVISOR_USB_ROLE_NONE;

#if configCTRLPANEL_TRACE == 1
static int TempoVbusOn = 0;
static int TempoVbusOff = 0;
#endif

/*! Maintenance mode requested variable. */
U8                   u8IsMaintenanceRequired = 0;
#endif
#endif

#ifdef MMILCD_ENABLE
/*! The handle of the queue of SUPERVISOR, holding user actions to perform. */
xQueueHandle xSUPERVISORQueue = 0;

/*! MMI navigator ID. */
signed short      sMmiNavId;
#endif



//_____ D E C L A R A T I O N S ____________________________________________

/* Forward declaration. */
static portTASK_FUNCTION_PROTO( vSupervisorTask, pvParameters );
static void prv_v_leave_maintenance_mode( void );
#ifdef MMILCD_ENABLE
static portTASK_FUNCTION( vSupervisorUserActionTask, pvParameters );
static void prv_v_manage_user_action( void );
#endif
#ifdef USB_ENABLE
#if USB_DEVICE_FEATURE == true
static void prv_v_common_leave_UsbDevice_mode( void );
#endif
#endif

/*-----------------------------------------------------------*/

void vSupervisor_Start( unsigned portBASE_TYPE uxPriority )
{
   /* Spawn the Sentinel task. */
   xTaskCreate( vSupervisorTask, ( const signed portCHAR * )"SUPER",
                SUPERVISOR_STACK_SIZE, NULL, uxPriority, ( xTaskHandle * )NULL );
}
/*-----------------------------------------------------------*/


/*!
 * \brief Supervisor task.
 *
 * \return never
 */
static portTASK_FUNCTION( vSupervisorTask, pvParameters )
{
   portTickType xDelayLength = SUPERVISOR_DEFAULT_PERIOD;
   portTickType xLastFocusTime;
#if configHEAP_INIT == 1
#if defined(__GNUC__)
   portLONG     lCheckHeapDelay = 1;
#endif
#endif
#if configCTRLPANEL_TRACE == 1
   portLONG     lPrintTrace = 3;
#endif
   portLONG     lUpdateTimeDelay = 1;
#ifdef MMILCD_ENABLE
   portLONG     lUpdateMMITimeDelay = 1;
   portCHAR DateTime[21];
   struct tm *pxDate;
   bool ms_connected_displayed = pdFALSE;
   bool enum_connected_displayed = pdFALSE;
#endif

   /* The parameters are not used. */
   ( void )pvParameters;

#if configCTRLPANEL_TRACE == 1
   /* Initialize the dump port COM2. */
   itracedump_Init();
#endif

#ifdef MMILCD_ENABLE
   // The MMI module.
   if( pdFALSE == bMMI_start() )
   {
      // vParTestSetLED( ERROR_LED_ID, pdTRUE );
      while( 1 );
   }
#endif

   // Create the SHELL mutex.
   vSemaphoreCreateBinary( xSHELLFSMutex );
   if( NULL == xSHELLFSMutex )
   { // The mutex creation failed.
      // TODO: Add msg on LCD.
      // vParTestSetLED( ERROR_LED_ID, pdTRUE );
      while( 1 );
   }
   // Start the COM1 Shell module.
   vStartCom1Shell( mainCOMSH_TASK_PRIORITY );

   // Create the CFG mutex.
   vSemaphoreCreateBinary( xCFGMutex );
   if( NULL == xCFGMutex )
   { // The mutex creation failed.
      // TODO: Add msg on LCD.
      // vParTestSetLED( ERROR_LED_ID, pdTRUE );
      while( 1 );
   }

   // Start the sensor module.
   if( false == bsensor_start() )
   {
      // TODO: Add msg on LCD.
      // vParTestSetLED( ERROR_LED_ID, pdTRUE );
      while( 1 );
   }

#if NW_INTEGRATED_IN_CONTROL_PANEL
   // Create the Web server mutex.
   vSemaphoreCreateBinary( xWEBMutex );
   if( NULL == xWEBMutex )
   { // The mutex creation failed.
      // TODO: Add msg on LCD.
      // vParTestSetLED( ERROR_LED_ID, pdTRUE );
      while( 1 );
   }
   // Start network tasks.
   vStartEthernetTaskLauncher( tskIDLE_PRIORITY + 1 );
#endif

   // Create the LOG mutex.
   vSemaphoreCreateBinary( xLOGMutex );
   if( NULL == xLOGMutex )
   { // The mutex creation failed.
      // TODO: Add msg on LCD.
      // vParTestSetLED( ERROR_LED_ID, pdTRUE );
      while( 1 );
   }
   // Start the data logger module.
   if( false == bdatalog_start( mainDATALOG_TASK_PRIORITY ) )
   {
      // TODO: Add msg on LCD.
      // vParTestSetLED( ERROR_LED_ID, pdTRUE );
      while( 1 );
   }

#ifdef USB_ENABLE
#if USB_DEVICE_FEATURE == true
   // Create the USB mutex.
   vSemaphoreCreateBinary( xUSBMutex );
   if( NULL == xUSBMutex )
   { // The mutex creation failed.
      // TODO: Add msg on LCD.
      // vParTestSetLED( ERROR_LED_ID, pdTRUE );
      while( 1 );
   }
   // Immediately take the USB mutex. i.e. when we're a Mass Storage device,
   // we'll refuse to give r/w access to the host until a user action. This user
   // action will make the Ctrl Panel device switch to maintenance mode, in which
   // the Mass Storage USB host has r/w access to the Ctrl Panel file system.
   while( pdFALSE == x_supervisor_SemaphoreTake( xUSBMutex, 0 ) );
#endif
   // Start the USB module tasks.
   if( false == b_usbsys_start() )
   {
      // TODO: Add msg on LCD.
      // vParTestSetLED( ERROR_LED_ID, pdTRUE );
      while( 1 );
   }
#endif

#ifdef MMILCD_ENABLE
   // Create the supervisor queue to deal with MMI actions
   xSUPERVISORQueue = xQueueCreate( SUPERVISOR_QUEUE_SIZE, sizeof(bool *) );
   if( 0 == xSUPERVISORQueue )
   {
      // TODO: Add msg on LCD.
      // vParTestSetLED( ERROR_LED_ID, pdTRUE );
      while( 1 );
   }

   /* Get a File System navigator for MMI actions. */
   fsaccess_take_mutex();
   sMmiNavId = fsaccess_alloc_nav_id();
   nav_select( sMmiNavId );   // Select the navigator.
   fsaccess_give_mutex();

   /* Spawn the User Action task. */
   if( pdPASS != xTaskCreate( vSupervisorUserActionTask,
                              ( const signed portCHAR * )"MMIACT",
                              SUPERVISOR_USER_ACTION_STACK_SIZE, NULL, SUPERVISOR_USER_ACTION_TASK_PRIORITY,
                              &xSupervisorUserActionHndl ) )
   {
      vTaskDelete( xSupervisorUserActionHndl );
      // TODO: Add msg on LCD.
      // vParTestSetLED( ERROR_LED_ID, pdTRUE );
      while( 1 );
   }
#endif // #ifdef MMILCD_ENABLE

   /* We need to initialise xLastFlashTime prior to the first call to vTaskDelayUntil(). */
   xLastFocusTime = xTaskGetTickCount();

#if defined(__GNUC__)
   NAKED_TRACE_COM2( "heap start @=%d, heap end @=%d", \
                     (portBASE_TYPE *)&__heap_start__, \
                     (portBASE_TYPE *)&__heap_end__ );
#endif

   /* Enable the watchdog timer. */
   // wdt_enable( SUPERVISOR_WDT_TIMEOUT );

   for(;;)
   {
      /* Delay for the flash period then check. */
      vTaskDelayUntil( &xLastFocusTime, xDelayLength );

      // wdt_clear(); // Kick the watchdog!

      /* MMI USB management. */
#ifdef MMILCD_ENABLE
#ifdef USB_ENABLE
/*** Start of Host behaviour ***/
      // first occurrence of MS connection, Host mode
      if (ms_connected == true && ms_connected_displayed == pdFALSE)
      {
        // display connected logo
        ms_connected_displayed = pdTRUE;
        vMMI_DisplayUSBState(ms_connected_displayed);
        // Display User Menu
        vMMI_SetUserMenuMode(eUserMenuUSBHost, pdTRUE);

      }
      // first occurrence of MS disconnection, end of Host mode
      if (ms_connected == false && ms_connected_displayed == pdTRUE)
      {
        // remove connected logo
        ms_connected_displayed = pdFALSE;
        vMMI_DisplayUSBState(ms_connected_displayed);
        // clear User Menu
        vMMI_SetUserMenuMode(eUserMenuIdle, pdTRUE);
      }
/*** End of Host behaviour ***/
/*** Start of Device behaviour ***/
#if USB_DEVICE_FEATURE == true
      // first occurrence of Device connection, Device mode
      if (Is_device_enumerated() && ( enum_connected_displayed == pdFALSE ) )
      {
        if( true == bIsInMaintenance )
        {
          // display connected logo
          enum_connected_displayed = pdTRUE;
          vMMI_DisplayUSBState(enum_connected_displayed);
          // Display User Menu
          vMMI_SetUserMenuMode(eUserMenuUSBDevice, pdTRUE);
        }
      }
      // first occurrence of Device disconnection, end of Device mode
      else if (!Is_device_enumerated() && enum_connected_displayed == pdTRUE)
      {
        // remove connected logo
        enum_connected_displayed = pdFALSE;
        vMMI_DisplayUSBState(enum_connected_displayed);
        // clear User Menu
        vMMI_SetUserMenuMode(eUserMenuIdle, pdTRUE);
      }
      else
      {
        // remove connected logo => this makes the USB logo blink when the Control
        // Panel is behaving as a USB key.
        enum_connected_displayed = pdFALSE;
        vMMI_DisplayUSBState(enum_connected_displayed);
      }
/*** End of Device behaviour ***/
#endif // #if USB_DEVICE_FEATURE == true
#endif // #ifdef USB_ENABLE
#endif // #ifdef MMILCD_ENABLE

      /* update time every SUPERVISOR_DELAY_TIMEUPDATE seconds. */
      if( 0 == --lUpdateTimeDelay )
      {
         /* Update the local time. */
         lUpdateTimeDelay = SUPERVISOR_DELAY_TIMEUPDATE;
         xcptime_LocalTime++;
         // v_cptime_UpdateLocalTime();
      }

#ifdef MMILCD_ENABLE
      /* Update time displayed on the LCD. */
      if( 0 == --lUpdateMMITimeDelay)
      {
         // Get the broken-down representation of the current date.
         pxDate = gmtime( &xcptime_LocalTime );

         // WARNING: pxDate->tm_year == number of years since 1900.
         // For years >= 2000, we'll display the last 2 digits only.
         if( pxDate->tm_year >= 100 )  pxDate->tm_year -= 100;
#if DISPLAY_MMI_SECOND == 1
         sprintf( DateTime, "%02d/%02d/20%02d  %02d:%02d:%02d", pxDate->tm_mon +1, pxDate->tm_mday,
                            pxDate->tm_year, pxDate->tm_hour, pxDate->tm_min, pxDate->tm_sec );
#else
         sprintf( DateTime, "%02d/%02d/20%02d     %02d:%02d", pxDate->tm_mon +1, pxDate->tm_mday,
                            pxDate->tm_year, pxDate->tm_hour, pxDate->tm_min );
#endif
         vMMI_DisplayDateAndTime(DateTime);
         lUpdateMMITimeDelay = SUPERVISOR_DELAY_MMI_TIMEUPDATE;
      }

      // Manage MMI
      vMMI_Manage();

      // Manage MMI user action
      prv_v_manage_user_action();
#endif // #ifdef MMILCD_ENABLE

#ifdef USB_ENABLE
      if( true == bOutOfMaintenance )
      {
         prv_v_leave_maintenance_mode();
      }
#endif

      /* Execute a scheduled command if expiration date is up. */
      v_cptime_ExecuteScheduledCmd();

#if configHEAP_INIT == 1
#if defined(__GNUC__)
      /* Record the malloc() heap highest consumption every SUPERVISOR_DELAY_HEAPCHECK seconds. */
      if( 0 == --lCheckHeapDelay )
      {
         lCheckHeapDelay = SUPERVISOR_DELAY_HEAPCHECK;
         prvCheckMallocHeap();
      }
#endif
#endif

#if configCTRLPANEL_TRACE == 1
      // Display traces on USART1 every SUPERVISOR_DELAY_PRINTTASKLIST seconds.
      if( 0 == --lPrintTrace )
      {
         lPrintTrace = SUPERVISOR_DELAY_PRINTTASKLIST; // Restart the delay.
         v_syscmds_display_traces();
      }
#endif

   } // for(;;)
} /*lint !e715 !e818 !e830 Function definition must be standard for task creation. */



#ifdef MMILCD_ENABLE
/*!
 * \brief The MMI user-action task: in charge of launching an action requested
 * through the MMI.
 *
 */
static portTASK_FUNCTION( vSupervisorUserActionTask, pvParameters )
{
bool (*pfUserAction) (void);
bool result;

   /* Just to stop compiler warnings. */
   ( void ) pvParameters;

   for(;;)
   {
      /* 1) Suspend ourselves. */
      vTaskSuspend( NULL ); // TO KEEP
      // NOTE: the task should be resumed when it is necessary to perform a user action

      // get queue information
      if ( pdTRUE == xQueueReceive( xSUPERVISORQueue, &pfUserAction, ( portTickType ) 0 ) )
      {
        /* 2) Perform the action. */
        if (pfUserAction != NULL)
        {
          result = pfUserAction();
          if (result)
          {
            NAKED_TRACE_COM2( "User function succeed");
          }
          else
          {
            NAKED_TRACE_COM2( "User function failed");
          }
        }
      }
   }
}
#endif // #ifdef MMILCD_ENABLE

#ifdef MMILCD_ENABLE
#ifdef USB_ENABLE

/*!
 * \brief Launch the "Copy Logs" MMI task that copies the /LOG directory from the \n
 * Control Panel to the USB Mass Storage device.
 *
 * \return pdTRUE
 *
 */
bool b_USBHostCopyLogs( void )
{
#if configCTRLPANEL_TRACE == 1
   eExecStatus       xRet;
#endif


   // Display User Menu Acting
   vMMI_SetUserMenuMode(eUserMenuUSBHostActing, pdFALSE);
   vTaskDelay( 3 ); // So that the supervisor task gets the focus to update the
                    // LCD screen.
   NAKED_TRACE_COM2( "Copy Logs" );

   // Perform the task
#if configCTRLPANEL_TRACE == 1
   xRet =
#endif
   e_usbsys_cp_logs_to_key( SYS_MODID_MMI, sMmiNavId, -1, NULL, NULL );
#if configCTRLPANEL_TRACE == 1
   if( SHELL_EXECSTATUS_KO == xRet )
      NAKED_TRACE_COM2( "Copying logs from Ctrl Panel to USB MS device failed.")
#endif

   // Display User Menu
   vMMI_SetUserMenuMode(eUserMenuUSBHost, pdFALSE);
   return (pdTRUE);
}

/*!
 * \brief Launch the "Move Logs" MMI task that moves the /LOG directory content \n
 * from the Control Panel to the USB Mass Storage device.
 *
 * \return pdTRUE
 *
 */
bool b_USBHostMoveLogs( void )
{
#if configCTRLPANEL_TRACE == 1
   eExecStatus       xRet;
#endif


   // Display User Menu Acting
   vMMI_SetUserMenuMode(eUserMenuUSBHostActing, pdFALSE);
   vTaskDelay( 3 ); // So that the supervisor task gets the focus to update the
                    // LCD screen.
   NAKED_TRACE_COM2( "Move Logs" );

   // Perform the task
#if configCTRLPANEL_TRACE == 1
   xRet =
#endif
   e_usbsys_mv_logs_to_key( SYS_MODID_MMI, sMmiNavId, -1, NULL, NULL );
#if configCTRLPANEL_TRACE == 1
   if( SHELL_EXECSTATUS_KO == xRet )
      NAKED_TRACE_COM2( "Copying logs from Ctrl Panel to USB MS device failed.")
#endif

   // Display User Menu
   vMMI_SetUserMenuMode(eUserMenuUSBHost, pdFALSE);
   return (pdTRUE);
}

/*!
 * \brief Launch the "Upload WEB Files" MMI task that copies the /WEB directory
 * from the USB Mass Storage device to the Control Panel.
 *
 * \return pdTRUE
 *
 */
bool b_USBHostCopyWeb( void )
{
#if configCTRLPANEL_TRACE == 1
   eExecStatus       xRet;
#endif


   // Display User Menu Acting
   vMMI_SetUserMenuMode(eUserMenuUSBHostActing, pdFALSE);
   vTaskDelay( 3 ); // So that the supervisor task gets the focus to update the
                    // LCD screen.
   NAKED_TRACE_COM2( "Copy Web Files" );

   // Perform the task
#if configCTRLPANEL_TRACE == 1
   xRet =
#endif
   e_usbsys_cp_web_to_local( SYS_MODID_MMI, sMmiNavId, -1, NULL, NULL );
#if configCTRLPANEL_TRACE == 1
   if( SHELL_EXECSTATUS_KO == xRet )
      NAKED_TRACE_COM2( "Copying /WEB from USB MS device to Ctrl Panel failed.")
#endif

  // Display User Menu
  vMMI_SetUserMenuMode(eUserMenuUSBHost, pdFALSE);
  return (pdTRUE);
}

/*!
 * \brief Launch the "Upload CFG Files" MMI task that copies the /CFG directory
 * from the USB Mass Storage device to the Control Panel.
 *
 * \return pdTRUE
 *
 */
bool b_USBHostCopyCfg( void )
{
#if configCTRLPANEL_TRACE == 1
   eExecStatus       xRet;
#endif


   // Display User Menu Acting
   vMMI_SetUserMenuMode(eUserMenuUSBHostActing, pdFALSE);
   vTaskDelay( 3 ); // So that the supervisor task gets the focus to update the
                    // LCD screen.
   NAKED_TRACE_COM2( "Copy Config Files" );

   // Perform the task
#if configCTRLPANEL_TRACE == 1
   xRet =
#endif
   e_usbsys_cp_cfg_to_local( SYS_MODID_MMI, sMmiNavId, -1, NULL, NULL );
#if configCTRLPANEL_TRACE == 1
   if( SHELL_EXECSTATUS_KO == xRet )
      NAKED_TRACE_COM2( "Copying /CFG from USB MS device to Ctrl Panel failed.")
#endif

  // Display User Menu
  vMMI_SetUserMenuMode(eUserMenuUSBHost, pdFALSE);
  return (pdTRUE);
}
#endif

// TODO: the function header.
static void prv_v_manage_user_action( void )
{
unsigned portBASE_TYPE   uxNbMsgsInQueue = 0;

   // Check if it is time to empty the xSUPERVISORQueue.
   uxNbMsgsInQueue = uxQueueMessagesWaiting( xSUPERVISORQueue );
   if( 0 != uxNbMsgsInQueue )
   {
      // Resume the User Action task.
      vTaskSuspend( xSupervisorUserActionHndl );
      vTaskResume( xSupervisorUserActionHndl );
      // WARNING: What would happen if we try to resume a task that is not suspended?
      // Is the FreeRTOS Kernel implementation safe on this one? To be sure, we issue
      // a suspend before a resume.
   }
}
#endif // #ifdef MMILCD_ENABLE


/*! \brief Take a system mutex.
 *
 *  \param xSemaphore   A handle to the semaphore being released.
 *  \param xBlockTime   The time in ticks to wait for the semaphore to become available.
 *
 *  \return pdTRUE if the semaphore was obtained. pdFALSE if xBlockTime expired without the semaphore becoming available.
 */
portBASE_TYPE x_supervisor_SemaphoreTake( xSemaphoreHandle xSemaphore,portTickType xBlockTime )
{
#ifdef USB_ENABLE
#if USB_DEVICE_FEATURE == true
   // Deal with the case where (the remote USB host disconnects then plugs back
   // almost immediately) AND (we haven't fully left the maintenance mode yet).
   if( ( xSemaphore == xUSBMutex ) && ( true == bOutOfMaintenance ) )
      return( pdFALSE );
   else
#endif
#endif
      return( xSemaphoreTake( xSemaphore, xBlockTime ) );
}


/*! \brief Release a system mutex.
 *
 *  \param xSemaphore   A handle to the semaphore being released.
 *
 *  \return pdTRUE if the semaphore was released. pdFALSE if an error occurred.
 */
portBASE_TYPE x_supervisor_SemaphoreGive( xSemaphoreHandle xSemaphore )
{
#ifdef USB_ENABLE
#if USB_DEVICE_FEATURE == true
   if( ( 0 != u8IsMaintenanceRequired ) && ( false == bOutOfMaintenance ) )
   {
      // If we have to enter in the maintenance mode, do not release the mutex.
      // => this mutex is now the property of the supervisor.
      u8IsMaintenanceRequired++;

      // If all mutexes have been acquired, switch to maintenance mode.
      if( ( SUPERVISOR_MAINTENANCE_NBMUTEX_TOTAKE +1 ) == u8IsMaintenanceRequired )
      {
         fat_cache_flush(); // Flush the FAT cache.
         nav_reset();       // Reset all file system navigators. We will mount
                            // the com1shell default drive when we'll leave the
                            // maintenance mode.
         // Switch to maintenance mode.
         xSemaphoreGive( xUSBMutex );

         // If the USB clock is frozen, unfreeze it so that we can write in the
         // USB registers. The USB clock is frozen if a "Device Suspend" event
         // occurred (no more USB activity detected) (cf. usb_general_interrupt()).
         if(true == Is_usb_clock_frozen())
         {
           Usb_unfreeze_clock();
         }
         // If it is not already detached, physically detach the USB device.
         if(false == Is_usb_detached())
         {
           Usb_detach();
         }
         vTaskDelay(500); // Wait 500ms
         Usb_attach();     // Reconnect the device.

         bIsInMaintenance = true;
         u8IsMaintenanceRequired = 0;
         TRACE_COM2( "Entering maintenance mode");
#ifdef MMILCD_ENABLE
         vMMI_SetUserMenuMode( eUserMenuWaitHost, pdTRUE );
#endif
      }
      return( pdTRUE );
   }
   else
#endif
#endif
      return( xSemaphoreGive( xSemaphore ) );
}


/*!
 *  \brief Called upon remote host connect (upon VBUS ON event when we are device)
 */
void v_supervisor_Host_Connects( void )
{
#ifdef USB_ENABLE
#if configCTRLPANEL_TRACE == 1
   TempoVbusOn++;
#endif
   if(( 0 == u8IsMaintenanceRequired )&&( false == bIsInMaintenance ))
   {
      // If we connect to the host and the Control Panel is not "set as USB key",
      // properly detach from the USB host (instead of behaving out of the USB spec
      // by not completely enumerating as a Mass Storage device).
      Usb_unfreeze_clock();
      Usb_detach();
   }
   if( SUPERVISOR_USB_ROLE_NONE == u8CurrentUsbRole )
   {   // Consider only one of these events in case we get several.
      u8CurrentUsbRole = SUPERVISOR_USB_ROLE_DEVICE;
   }
#endif
}


/*!
 *  \brief Called upon VBUS OFF event.
 *  \warning: the name of the function may imply that this is called in case we
 *  were a device, but this may not always be the case: this function may be called
 *  when we are a device. Explanation: the VBUS OFF event occurs in both cases.
 */
void v_supervisor_Host_Disconnects( void )
{
#ifdef USB_ENABLE
#if configCTRLPANEL_TRACE == 1
   TempoVbusOff++;
#endif
   if( SUPERVISOR_USB_ROLE_DEVICE == u8CurrentUsbRole )
   {   // Consider only one of these events in case we get several.
      u8CurrentUsbRole = SUPERVISOR_USB_ROLE_NONE;
      if( true == bIsInMaintenance )
      {
         bIsInMaintenance = false;  // We're not in maintenance mode anymore.
         bOutOfMaintenance = true;  // We have actions to take upon maintenance mode leave.
      }
   }
#endif
}


/*!
 *  \brief Called upon remote device connection (upon PIN ID CHANGE event when we are host)
 */
void v_supervisor_Device_Connects( void )
{
#ifdef USB_ENABLE
   if( SUPERVISOR_USB_ROLE_NONE == u8CurrentUsbRole )
   {   // Consider only one of these events in case we get several.
      u8CurrentUsbRole = SUPERVISOR_USB_ROLE_HOST;
   }
#endif
}

/*!
 *  \brief Called upon remote device disconnection (upon PIN ID CHANGE event when we are host)
 */
void v_supervisor_Device_Disconnects( void )
{
#ifdef USB_ENABLE
   if( SUPERVISOR_USB_ROLE_HOST == u8CurrentUsbRole )
   {   // Consider only one of these events in case we get several.
      u8CurrentUsbRole = SUPERVISOR_USB_ROLE_NONE;
      ms_new_device_connected = false, ms_connected = false;
   }
#endif
}


/*############################# TEMPORARY ###############################*/


/*!
 *  \brief The switch-to-maintenance-mode command: initiate the process to \n
 *         switch to maintenance mode.
 *         Format: maintain
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
eExecStatus e_supervisor_switch_to_maintenance_mode( eModId xModId,
                              signed short FsNavId,
                              int ac, signed portCHAR *av[],
                              signed portCHAR **ppcStringReply )
{
   if( NULL != ppcStringReply )
      *ppcStringReply = NULL;

#ifdef USB_ENABLE
#if USB_DEVICE_FEATURE == true
   if( ( false == bIsInMaintenance )
       && ( 0 == u8IsMaintenanceRequired ) )
   {   // We're not in maintenance mode.
      // Initiate the process of switching to maintenance mode.
      if( 0 == u8IsMaintenanceRequired )   u8IsMaintenanceRequired++;

      // Take all maintenance mutex except the USB mutex.
      if( true == x_supervisor_SemaphoreTake( xLOGMutex, 0 ) )       u8IsMaintenanceRequired++;
#if NW_INTEGRATED_IN_CONTROL_PANEL
      if( true == x_supervisor_SemaphoreTake( xWEBMutex, 0 ) )       u8IsMaintenanceRequired++;
#endif
      if( true == x_supervisor_SemaphoreTake( xSHELLFSMutex, 0 ) )   u8IsMaintenanceRequired++;
      if( true == x_supervisor_SemaphoreTake( xCFGMutex, 0 ) )       u8IsMaintenanceRequired++;

      // If all mutexes have been acquired, switch to maintenance mode.
      if( ( SUPERVISOR_MAINTENANCE_NBMUTEX_TOTAKE +1 ) == u8IsMaintenanceRequired )
      {
         fat_cache_flush(); // flush the FAT cache.
         nav_reset();       // Reset all file system navigators. We will mount
                            // the com1shell default drive when we'll leave the
                            // maintenance mode.
         // Switch to maintenance mode.
         xSemaphoreGive( xUSBMutex );

         // If the USB clock is frozen, unfreeze it so that we can write in the
         // USB registers.
         if(true == Is_usb_clock_frozen())
         {
           Usb_unfreeze_clock();
         }
         // If it is not akready detached, physically detach the USB device.
         if(false == Is_usb_detached())
         {
           Usb_detach();
         }
         vTaskDelay(500); // Wait 500ms
         Usb_attach();     // Reconnect the device.

         bIsInMaintenance = true;
         u8IsMaintenanceRequired = 0;
         TRACE_COM2( "Entering maintenance mode");
#ifdef MMILCD_ENABLE
         vMMI_SetUserMenuMode( eUserMenuWaitHost, pdTRUE );
#endif
      }
      // ELSE: we'll switch to maintenance mode in x_supervisor_SemaphoreGive()
      // (when the mutex(es) that we couldn't get will be released).
   }
   else
   {
      NAKED_TRACE_COM2( "Won't go to maintenance mode:"CRLF"bIsInMaintenance=%d u8CurrentUsbRole=%d u8IsMaintenanceRequired=%d", bIsInMaintenance, u8CurrentUsbRole, u8IsMaintenanceRequired );
   }
#endif
#endif

   return( SHELL_EXECSTATUS_OK );
}

/*!
 *  \brief The switch-to-maintenance-mode command: initiate the process to \n
 *         switch to maintenance mode.
 *
 */
bool b_supervisor_switch_to_maintenance_mode( void )
{
  e_supervisor_switch_to_maintenance_mode( SYS_MODID_NONE, 0, 0, NULL, NULL );
  return (pdTRUE);
}


#ifdef USB_ENABLE
#if USB_DEVICE_FEATURE == true
/*!
 *  \brief Leave the waiting-for-host mode while the USB host has not been plugged
 *  in yet.
 *
 */
bool b_supervisor_leave_waitingforhost_mode( void )
{
  bIsInMaintenance = false;  // We're not in maintenance mode anymore.
  prv_v_common_leave_UsbDevice_mode();
  return (pdTRUE);
}


/*!
 *  \brief Leave the USB Mass Storage mode while the USB host is plugged in.
 *
 */
bool b_supervisor_leave_UsbMassStorage_mode( void )
{
  Usb_detach();
  bIsInMaintenance = false;  // We're not in maintenance mode anymore.
  prv_v_leave_maintenance_mode();
  return (pdTRUE);
}
#endif
#endif


#if configCTRLPANEL_TRACE == 1
/*!
 * \brief Print dev info on the trace port.
 */
void v_supervisor_trace( void )
{
#if configHEAP_INIT
#if __GNUC__
  prvCheckMallocHeap(); // Get an update of the malloc() heap highest consumption.
  NAKED_TRACE_COM2( "SUP:<%d><%d><%d>", TempoVbusOn, TempoVbusOff, pxHighestHeapAddr );
#endif
#else
  NAKED_TRACE_COM2( "SUP:<%d><%d>", TempoVbusOn, TempoVbusOff );
#endif
}
#endif


#ifdef USB_ENABLE
#if USB_DEVICE_FEATURE == true
/*!
 * \brief Maintenance mode status.
 *
 * \return true if the maintenance mode is active else false.
 */
bool b_supervisor_IsInMaintenanceMode( void )
{
  return( bIsInMaintenance );
}
#endif
#endif

/*#######################################################################*/

/*################################## PRIVATE ##################################*/


#ifdef USB_ENABLE
#if USB_DEVICE_FEATURE == true
/*!
 * \brief Release the semaphores and change the MMI user menu mode.
 *
 */
static void prv_v_common_leave_UsbDevice_mode( void )
{
   // Take the USB mutex.
   xSemaphoreGive( xUSBMutex ); // Release it in case the USB module didn't
   // Then take it back.
   // NOTE: we have to use xSemaphoreTake() rather than x_supervisor_xSemaphoreTake()
   // or we'll get a rejection(cf x_supervisor_xSemaphoreTake() implementation).
   if( pdFALSE == xSemaphoreTake( xUSBMutex, 0 ) )
   {   // SHOULD NEVER HAPPEN!!!
      TRACE_COM2( "Unexpected SemaphoreTake( xUSBMutex ) failure!!!" );
   }

   // Release all system mutexes except the xUSBMutex.
   xSemaphoreGive( xLOGMutex );
   xSemaphoreGive( xSHELLFSMutex );
   xSemaphoreGive( xCFGMutex );
#if NW_INTEGRATED_IN_CONTROL_PANEL
   xSemaphoreGive( xWEBMutex );
#endif

#ifdef MMILCD_ENABLE
   vMMI_SetUserMenuMode(eUserMenuIdle, pdTRUE);
#endif
}
#endif
#endif

/*!
 * \brief Do the required actions to leave the maintenance mode.
 *
 */
static void prv_v_leave_maintenance_mode( void )
{
#ifdef USB_ENABLE
#if USB_DEVICE_FEATURE == true
   // Release the semaphores and change the MMI user menu mode.
   prv_v_common_leave_UsbDevice_mode();

   // The actions to take to leave the maintenance mode are now done.
   bOutOfMaintenance = false;
   // We just left the USB Mass Storage device mode.
   // Special case: for com1shell, remount the local drive.
   v_com1shell_mount_local_drive();
#endif
#endif
   NAKED_TRACE_COM2( "Leaving maintenance mode");
}


#ifdef __GNUC__
/*######## TEMPORARY #########################################*/
void prvCheckMallocHeap( void )
{
   portBASE_TYPE *pxMem;


   pxMem = ((portBASE_TYPE *)&__heap_end__) -1;
   while( (portBASE_TYPE *)&__heap_start__ != pxMem )
   {
      if( (portBASE_TYPE)0xA5A5A5A5 != *pxMem )
      {
         if( pxHighestHeapAddr < pxMem )
            pxHighestHeapAddr = pxMem;
         NAKED_TRACE_COM2( "worst@=%d", pxHighestHeapAddr );
         break;
      }
      else pxMem--;
   }
}
#endif
