/*****************************************************************************
 *
 * \file
 *
 * \brief AVR32 UC3 Control Panel systems commands interface.
 *
 * This module provides commands interface to the subsystems of the Ctrl Panel.
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
 * This module provides commands interface to the subsystems of the Ctrl Panel.
 *
 * Detailed description of the module:
 * TODO
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <string.h>
#include "gpio.h" // Have to include gpio.h before FreeRTOS.h as long as FreeRTOS
                  // redefines the inline keyword to empty.
#include "FreeRTOS.h"
#include "task.h"
#include "shell.h"

#include "ethernet.h"
#include "BasicWEB.h"
#include "BasicSMTP.h"
#include "cptime.h"

#ifdef USB_ENABLE
#include "conf_usb.h"
#include "usb_drv.h"
#include "usb_task.h"
#endif

#include "com1shell.h"
#include "sensor.h"
#include "supervisor.h"
#include "syscmds.h"

//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

const signed portCHAR *const SYSCMDS_MSG_HELP = (signed portCHAR *)"\
"CRLF"get_sys_config sys={net,http,time} : display the config of a system module"CRLF"\
set_sys_config sys=net {macaddr,ipaddr,submask,gwaddr}=value : set one of the config fields of the network module"CRLF"\
set_sys_config sys=http port=value : set the HTTP port of the Web server module"CRLF"\
set_sys_config sys=time curtime=\"mm/dd/yy hh:mm:ss\" : set the current time of the Control Panel"CRLF"\
set_sys_config sys=smtp {port,mailto,mailfrom,server}=value : set one of the config fields of the SMTP client"CRLF"\
maintain : switch to maintenance mode"CRLF"\
cp_logs_to_ukey : cp the /LOG dir to a USB Mass Storage device"CRLF
"reboot : sw reset"CRLF"\
lsusb : display info on the USB conn(DEVELOPMENT ONLY)"CRLF
#if configCTRLPANEL_TRACE == 1
"\
ipstat : display info about the TCP/IP stack on the COM2 port(DEVELOPMENT ONLY)"CRLF"\
dbgtrace : display debug info on the USART1 trace port(DEVELOPMENT ONLY)"CRLF
#endif
"\
version: display the Ctrl Panel sw version."CRLF;

/*! The CFG system mutex. */
extern xSemaphoreHandle   xCFGMutex;

/*! The Ctrl Panel sw version. */
extern const char *const pcCtrlPanelVersion;

eExecStatus e_supervisor_switch_to_maintenance_mode( eModId xModId,
                              signed short FsNavId,
                              int ac, signed portCHAR *av[],
                              signed portCHAR **ppcStringReply );

#if configCTRLPANEL_TRACE == 1
#if configSTACK_CONSUMPTION_CHECK
signed portCHAR acTaskListInfo[1024];
#endif
#endif

//_____ D E C L A R A T I O N S ____________________________________________

/*!
 *  \brief The get sys config command: get the config fields value of a subsystem
 *         Takes one parameter, that is the subsystem's name.
 *         Format: get_sys_config sys=sysname
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
eExecStatus e_syscmds_cmd_get_config( eModId xModId, signed short FsNavId,
                                      int ac, signed portCHAR *av[],
                                      signed portCHAR **ppcStringReply )
{
   if ( !strcmp( (char *)av[1], "net" ) )
   {
      return (e_ethernet_cmd_get_config( xModId, FsNavId, ac, av, ppcStringReply));
   }
   else if ( !strcmp( (char *)av[1], "http" ) )
   {
      return (e_webserver_cmd_get_config( xModId, FsNavId, ac, av, ppcStringReply));
   }
   else if ( !strcmp( (char *)av[1], "time" ) )
   {
      return (e_cptime_cmd_get_config( xModId, FsNavId, ac, av, ppcStringReply));
   }
   else if ( !strcmp( (char *)av[1], "smtp" ) )
   {
      return (e_smtpclient_cmd_get_config( xModId, FsNavId, ac, av, ppcStringReply));
   }
   else return( SHELL_EXECSTATUS_KO );
}


/*!
 *  \brief The set sys config command: set the value of a config field of a subsystem.
 *         Takes three parameters.
 *         The first parameter is the system's name, the second parameter is
 *         the config field name, the third parameter is its value.
 *         Format: set_sys_config sys=sysname field=value
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
eExecStatus e_syscmds_cmd_set_config( eModId xModId, signed short FsNavId,
                                      int ac, signed portCHAR *av[],
                                      signed portCHAR **ppcStringReply )
{
   eExecStatus   xRet = SHELL_EXECSTATUS_KO;


   // Take the CFG mutex.
   if( pdFALSE == x_supervisor_SemaphoreTake( xCFGMutex, 0 ) )
   {
      if( NULL != ppcStringReply )
      {
         *ppcStringReply = (signed portCHAR *)SHELL_ERRMSG_MAINTENANCEMODE;
      }
      return( SHELL_EXECSTATUS_KO );
   }
   else if ( !strcmp( (char *)av[1], "net" ) )
   {
      xRet = e_ethernet_cmd_set_config( xModId, FsNavId, ac-2, &(av[2]), ppcStringReply );
   }
   else if ( !strcmp( (char *)av[1], "http" ) )
   {
      xRet = e_webserver_cmd_set_config( xModId, FsNavId, ac-2, &(av[2]), ppcStringReply );
   }
   else if ( !strcmp( (char *)av[1], "time" ) )
   {
      xRet = e_cptime_cmd_set_config( xModId, FsNavId, ac-2, &(av[2]), ppcStringReply );
   }
   else if ( !strcmp( (char *)av[1], "smtp" ) )
   {
      xRet = e_smtpclient_cmd_set_config( xModId, FsNavId, ac-2, &(av[2]), ppcStringReply);
   }

   x_supervisor_SemaphoreGive( xCFGMutex ); // Release the CFG mutex.
   return( xRet );
}

/*!
 *  \brief The sys help command: display the system & subsystems available shell commands.
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
eExecStatus e_syscmds_help( eModId xModId, signed short FsNavId,
                            int ac, signed portCHAR *av[],
                            signed portCHAR **ppcStringReply )
{
   // 1) If the way to reply is unavailable, it's no use to continue.
   if( ppcStringReply == NULL )
      return( SHELL_EXECSTATUS_KO );

   // 2) Perform the command.
   *ppcStringReply = (signed portCHAR *)SYSCMDS_MSG_HELP;

   return( SHELL_EXECSTATUS_OK_NO_FREE );
}


/*!
 *  \brief The sys reboot command: Sw reset of the Ctrl Panel.
 *         Format: reboot
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
 *  \return the status of the command execution(makes no sense).
 */
eExecStatus e_syscmds_reboot( eModId xModId, signed short FsNavId,
                              int ac, signed portCHAR *av[],
                              signed portCHAR **ppcStringReply )
{
  int LimitWaitMaintenanceCount = 1024;


  // Switch to maintenance mode: safety in case a task is currently accessing the
  // file system in write mode.
  e_supervisor_switch_to_maintenance_mode( xModId, FsNavId, 0, NULL, NULL );

  // Wait until the Ctrl Panel is in maintenance mode before performing the sw
  // reset.
  // PROS: safer because the switch to maintenance mode is not necessarily immediate.
  // CONS: if we want to perform a sw reset, this may be because the Control Panel
  // is in an instable state (and/or one or several tasks have crashed); in this
  // case, the wait below may end-up being an infinite loop.
  // Limit the waiting time to avoid falling into an infinite loop.
  while( ( false == b_supervisor_IsInMaintenanceMode() ) && LimitWaitMaintenanceCount-- );

  //**
  //** Sw reset.
  //**
  // NOTE: we have to stop all possible ITs else there is a risk to get ITs
  // just after the sw reset while the modules init has not yet been performed
  // (especially while the ITs registering have not all been performed).
  Disable_global_interrupt();
  // Disable USB IT.
  Usb_disable();
  // Disable COM1 IT.
  v_com1shell_stopResources();
  // Disable COM2 IT.
  v_tracedump_stopResources();
  // Disable SENSORS IT.
  v_sensor_stop();
  // Disable MACB IT.
  v_ethernet_stopResources();
  Enable_global_interrupt();
  Usb_disable_otg_pad();
//  Reset_CPU();

  return( SHELL_EXECSTATUS_OK );
}


/*!
 *  \brief The sys version command: Display the Ctrl Panel sw version.
 *         Format: version
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
 *  \return the status of the command execution(makes no sense).
 */
eExecStatus e_syscmds_version( eModId xModId, signed short FsNavId,
                              int ac, signed portCHAR *av[],
                              signed portCHAR **ppcStringReply )
{
   // 1) If the way to reply is unavailable, it's no use to continue.
   if( ppcStringReply == NULL )
      return( SHELL_EXECSTATUS_KO );

   // 2) Perform the command.
   *ppcStringReply = (signed portCHAR *)pcCtrlPanelVersion;

   return( SHELL_EXECSTATUS_OK_NO_FREE );
}


/*!
 *  \brief The dbgtrace command: displays info on the trace port(USART1).
 *         Format: dbgtrace
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
eExecStatus e_syscmds_trace( eModId xModId, signed short FsNavId,
                             int ac, signed portCHAR *av[],
                             signed portCHAR **ppcStringReply )
{
   if( NULL != ppcStringReply )
      *ppcStringReply = NULL;
   v_syscmds_display_traces(); // Display the debug traces on the trace port.
   return( SHELL_EXECSTATUS_OK );
}


/*!
 * \brief Display debug info on the trace port(USART1).
 *
 */
void v_syscmds_display_traces( void )
{
#if configCTRLPANEL_TRACE == 1
#if configSTACK_CONSUMPTION_CHECK
   // Print the OS currently active tasks and info.
   /* WARNING FROM FREERTOS: This is a VERY costly function that should be used
   for debug only. It leaves interrupts disabled for a LONG time. */
   vTaskList( acTaskListInfo );
   NAKED_TRACE_COM2( "%s", acTaskListInfo );
#endif
   // Print dev info trace of several ctrl Panel modules.
   v_cptime_trace();    // cptime.c module
   v_datalog_trace();   // datalog.c module
#ifdef NW_INTEGRATED_IN_CONTROL_PANEL
   v_basicweb_trace();  // BasicWEB.c module
#endif
   v_supervisor_trace(); // supervisor.c module
#endif
}
