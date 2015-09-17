/*****************************************************************************
 *
 * \file
 *
 * \brief AVR32 UC3 Control Panel USB system module.
 *
 * This module is in charge of : \n
 *    * defining the USB objects
 *    * starting the USB tasks.
 *
 * - OS:                 FreeRTOS port on AVR32 UC3.
 * - Boards:             EVK1100
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


/*!
 * Detailed description of the module:
 * TODO
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

//_____  I N C L U D E S ___________________________________________________

#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "usbsys.h"

#include "conf_usb.h"
#include "usb_drv.h"
#include "usb_task.h"
#if USB_DEVICE_FEATURE == true
#include "device_mass_storage_task.h"
#endif
#if USB_HOST_FEATURE == true
#include "host_mass_storage_task.h"
#endif

#include "shell.h"

//_____ M A C R O S ________________________________________________________

#define MSG_NO_DEVICE         "No supported device connected"CRLF
#define MSG_REMOTE_WAKEUP_OK  "Device supports remote wake-up"CRLF
#define MSG_REMOTE_WAKEUP_KO  "Device does not support remote wake-up"CRLF
#define MSG_SELF_POWERED      "Device is self-powered"CRLF
#define MSG_BUS_POWERED       "Device is bus-powered"CRLF
#define MSG_USB_SUSPENDED     "USB is suspended!"CRLF
#define MSG_OK                "OK"CRLF
#define MSG_KO                "KO"CRLF
#define MSG_DEVICE_FULL_SPEED "Device is full-speed"CRLF
#define MSG_DEVICE_LOW_SPEED  "Device is low-speed"CRLF

#define MSG_COPY_LOGS_TO_KEY  "Copying the log files to the USB key"CRLF
#define MSG_MOVE_LOGS_TO_KEY  "Moving the log files to the USB key"CRLF
#define MSG_COPY_CFG_TO_LOCAL "Copying the /CFG directory from the USB key"CRLF
#define MSG_COPY_WEB_TO_LOCAL "Copying the /WEB directory from the USB key"CRLF

//_____ D E F I N I T I O N S ______________________________________________


//_____ D E C L A R A T I O N S ____________________________________________

extern int sprintf(char *out, const char *format, ...);  // From printf-stdarg.c

// FORWARD
static eExecStatus prv_e_usbsys_sync_cp_ukey( eModId xModId, signed short FsNavId,
                                              signed portCHAR **ppcStringReply,
                                              signed portCHAR *pcStringMsgAction,
                                              U8 sync_direction, const char *pcdir_name,
                                              bool bDeleteSrc );

/*!
 * Start the USB modules.
 *
 * \return true upon success, else false.
 */
bool b_usbsys_start( void )
{
   // Create the usb_tsk_semphr and launch the usb_task() task.
   usb_task_init();
#if USB_DEVICE_FEATURE == true
  // Init the device Mass Storage module and
  // create the device_mass_storage_task() task
  device_mass_storage_task_init();
#endif
#if USB_HOST_FEATURE == true
  // Init the host Mass Storage module and
  // create the host_mass_storage_task() task
  host_mass_storage_task_init();
#endif

   return( true );
}


/*!
 *  \brief In host mode, display basic low-level information about the connected device.
 *  The device should be supported by the host (configured).
 *         No parameters.
 *         Format: lsusb
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Ignored.
 *  \param ac             Input. The argument counter. Ignored.
 *  \param av             Input. The argument vector. Ignored
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed here;
 *                        the caller must free this string.
 *
 *  \return the status of the command execution.
 */
eExecStatus e_usbsys_lsusb( eModId xModId, signed short FsNavId,
                       int ac, signed portCHAR *av[],
                       signed portCHAR **ppcStringReply )
{
#if USB_HOST_FEATURE == true
   signed portCHAR *pcStringToPrint;
   U8 i, j;


   if( NULL != ppcStringReply )
      *ppcStringReply = NULL;

   if (!Is_host_ready() && !Is_host_suspended())
   {
      v_shell_Print_String_To_Requester_Stream( xModId, (signed portCHAR *)MSG_NO_DEVICE );
      return SHELL_EXECSTATUS_KO;
   }

   pcStringToPrint = (signed portCHAR *)pvPortMalloc( SHELL_MAX_MSGOUT_LEN ); // Alloc
   if( NULL == pcStringToPrint )
   {
      return( SHELL_EXECSTATUS_KO );
   }

   if (Is_host_suspended())
   {
      v_shell_Print_String_To_Requester_Stream( xModId, (signed portCHAR *)MSG_USB_SUSPENDED CRLF );
   }

   sprintf( (char *)pcStringToPrint, "VID: 0x%.4X, PID: 0x%.4X\r\n"
            "Device MaxPower is %d mA\r\n"
            "%s"
            "%s",
            Get_VID(), Get_PID(),
            2 * Get_maxpower(),
            Is_device_self_powered() ? MSG_SELF_POWERED : MSG_BUS_POWERED,
            Is_usb_full_speed_mode() ? MSG_DEVICE_FULL_SPEED : MSG_DEVICE_LOW_SPEED );
   v_shell_Print_String_To_Requester_Stream( xModId, pcStringToPrint );
   sprintf( (char *)pcStringToPrint, "%s"
            "Supported interface(s): %u",
            Is_device_supports_remote_wakeup() ? MSG_REMOTE_WAKEUP_OK : MSG_REMOTE_WAKEUP_KO,
            Get_nb_supported_interface() );
   v_shell_Print_String_To_Requester_Stream( xModId, pcStringToPrint );

   for (i = 0; i < Get_nb_supported_interface(); i++)
   {
      sprintf( (char *)pcStringToPrint, "\r\nInterface nb: %u, AltS nb: %u, Class: 0x%.2X,"
               " SubClass: 0x%.2X, Protocol: 0x%.2X\r\n" "\tAssociated Ep Nbrs:",
               Get_interface_number(i), Get_altset_nb(i), Get_class(i),
               Get_subclass(i), Get_protocol(i) );
      v_shell_Print_String_To_Requester_Stream( xModId, pcStringToPrint );

      if (Get_nb_ep(i))
      {
         for (j = 0; j < Get_nb_ep(i); j++)
         {
            sprintf( (char *)pcStringToPrint, " %u", (U16)Get_ep_nbr(i, j) );
            v_shell_Print_String_To_Requester_Stream( xModId, pcStringToPrint );
         }
      }
      else
      {
         v_shell_Print_String_To_Requester_Stream( xModId, (signed portCHAR *)"None" );
      }
   }

   vPortFree( pcStringToPrint );

   v_shell_Print_String_To_Requester_Stream( xModId, (signed portCHAR *)CRLF );
#else
   v_shell_Print_String_To_Requester_Stream( xModId, (signed portCHAR *)MSG_NO_DEVICE );
#endif
   return( SHELL_EXECSTATUS_OK );
}


/*!
 *  \brief In host mode, copy the /LOG directory to the USB MS key.
 *         No parameters.
 *         Format: cp_logs_to_ukey
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Input. The file system navigator id to use.
 *  \param ac             Input. The argument counter. Ignored.
 *  \param av             Input. The argument vector. Ignored
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string may be performed here;
 *                        the caller must thus free this string.
 *
 *  \return the status of the command execution. If the status is SHELL_EXECSTATUS_KO,
 *  the caller MUST NOT FREE the response string. If the status is SHELL_EXECSTATUS_OK
 *  and the response string is not NULL, the caller MUST free the response string.
 */
eExecStatus e_usbsys_cp_logs_to_key( eModId xModId, signed short FsNavId,
                                      int ac, signed portCHAR *av[],
                                      signed portCHAR **ppcStringReply )
{
   return( prv_e_usbsys_sync_cp_ukey( xModId, FsNavId, ppcStringReply,
                                      (signed portCHAR *)MSG_COPY_LOGS_TO_KEY,
                                      HOST_TO_DEVICE, "LOG/", false ) );
}


/*!
 *  \brief In host mode, move the /LOG directory content to the USB MS key.
 *         No parameters.
 *         Format: mv_logs_to_ukey
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Input. The file system navigator id to use.
 *  \param ac             Input. The argument counter. Ignored.
 *  \param av             Input. The argument vector. Ignored
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string may be performed here;
 *                        the caller must thus free this string.
 *
 *  \return the status of the command execution. If the status is SHELL_EXECSTATUS_KO,
 *  the caller MUST NOT FREE the response string. If the status is SHELL_EXECSTATUS_OK
 *  and the response string is not NULL, the caller MUST free the response string.
 */
eExecStatus e_usbsys_mv_logs_to_key( eModId xModId, signed short FsNavId,
                                     int ac, signed portCHAR *av[],
                                     signed portCHAR **ppcStringReply )
{
   return( prv_e_usbsys_sync_cp_ukey( xModId, FsNavId, ppcStringReply,
                                      (signed portCHAR *)MSG_MOVE_LOGS_TO_KEY,
                                      HOST_TO_DEVICE, "LOG/", true ) );
}


/*!
 *  \brief In host mode, copy the /CFG directory content of the USB MS key to the
 *  Control Panel /CFG directory.
 *  No parameters.
 *  Format: cp_cfg_to_local
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Input. The file system navigator id to use.
 *  \param ac             Input. The argument counter. Ignored.
 *  \param av             Input. The argument vector. Ignored
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string may be performed here;
 *                        the caller must thus free this string.
 *
 *  \return the status of the command execution. If the status is SHELL_EXECSTATUS_KO,
 *  the caller MUST NOT FREE the response string. If the status is SHELL_EXECSTATUS_OK
 *  and the response string is not NULL, the caller MUST free the response string.
 */
eExecStatus e_usbsys_cp_cfg_to_local( eModId xModId, signed short FsNavId,
                                      int ac, signed portCHAR *av[],
                                      signed portCHAR **ppcStringReply )
{
   return( prv_e_usbsys_sync_cp_ukey( xModId, FsNavId, ppcStringReply,
                                      (signed portCHAR *)MSG_COPY_CFG_TO_LOCAL,
                                      DEVICE_TO_HOST, "CFG/", false ) );
}


/*!
 *  \brief In host mode, copy the /WEB directory content of the USB MS key to the
 *  Control Panel /WEB directory.
 *  No parameters.
 *  Format: cp_web_to_local
 *
 *  \note  This function must be of the type pfShellCmd defined by the shell module.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Input. The file system navigator id to use.
 *  \param ac             Input. The argument counter. Ignored.
 *  \param av             Input. The argument vector. Ignored
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string may be performed here;
 *                        the caller must thus free this string.
 *
 *
 *  \return the status of the command execution. If the status is SHELL_EXECSTATUS_KO,
 *  the caller MUST NOT FREE the response string. If the status is SHELL_EXECSTATUS_OK
 *  and the response string is not NULL, the caller MUST free the response string.
 */
eExecStatus e_usbsys_cp_web_to_local( eModId xModId, signed short FsNavId,
                                      int ac, signed portCHAR *av[],
                                      signed portCHAR **ppcStringReply )
{
   return( prv_e_usbsys_sync_cp_ukey( xModId, FsNavId, ppcStringReply,
                                      (signed portCHAR *)MSG_COPY_WEB_TO_LOCAL,
                                      DEVICE_TO_HOST, "WEB/", false ) );
}


/************************************ PRIVATE ********************************/


/*!
 *  \brief Common synchronization function between the Control Panel and a USB key.
 *
 *  \param xModId             Input. The module that is calling this function.
 *  \param FsNavId            Input. The file system navigator id to use.
 *  \param ppcStringReply     Input/Output. The response string.
 *                            If Input is NULL, no response string will be output.
 *                            Else a malloc for the response string may be performed
 *                            here; the caller must thus free this string.
 *  \param pcStringMsgAction  Input. The message to display when performing the
 *                            action.
 *  \param sync_direction     Input. The direction of the synchonization(DEVICE_TO_HOST
 *                            or HOST_TO_DEVICE)
 *  \param pcdir_name         Input. The name of the directory to synchronize.
 *  \param bDeleteSrc         Input. Flag to perform a copy(false) or a move(true).
 *
 *  \return the status of the command execution. If the status is SHELL_EXECSTATUS_KO,
 *  the caller MUST NOT FREE the response string. If the status is SHELL_EXECSTATUS_OK
 *  and the response string is not NULL, the caller MUST free the response string.
 */
static eExecStatus prv_e_usbsys_sync_cp_ukey( eModId xModId, signed short FsNavId,
                                              signed portCHAR **ppcStringReply,
                                              signed portCHAR *pcStringMsgAction,
                                              U8 sync_direction, const char *pcdir_name,
                                              bool bDeleteSrc )
{
   if( NULL != ppcStringReply )
      *ppcStringReply = NULL;

#if USB_HOST_FEATURE == true
   if (!Is_host_ready() && !Is_host_suspended())
   {
      v_shell_Print_String_To_Requester_Stream( xModId, (signed portCHAR *)MSG_NO_DEVICE );
      return SHELL_EXECSTATUS_KO;
   }

   v_shell_Print_String_To_Requester_Stream( xModId, pcStringMsgAction );

   if( false == host_mass_storage_task_sync_drives( FsNavId, sync_direction, pcdir_name, bDeleteSrc ) )
   {
      v_shell_Print_String_To_Requester_Stream( xModId, (signed portCHAR *)MSG_KO );
   }
   else
   {
      v_shell_Print_String_To_Requester_Stream( xModId, (signed portCHAR *)MSG_OK );
   }

#endif
   return( SHELL_EXECSTATUS_OK );
}
