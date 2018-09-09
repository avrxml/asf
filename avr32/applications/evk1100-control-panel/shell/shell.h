/*****************************************************************************
 *
 * \file
 *
 * \brief AVR32 UC3 Control Panel Command Exec module.
 *
 * This module provides a Command execution service.
 *
 *
 *                       The example is written for UC3 and EVK1100.
 *
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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


#ifndef __SHELL_H__
#define __SHELL_H__

#include "compiler.h"
#include "FreeRTOS.h"

#include "supervisor.h"

//_____ M A C R O S ________________________________________________________

// Special char values
#define CR                    '\r'
#define LF                    '\n'
#define CTRL_C                0x03
#define BKSPACE_CHAR          '\b'
#define ABORT_CHAR            CTRL_C
#define CRLF                  "\r\n"
#define CTRL_Q                0x11
#define QUIT_APPEND           CTRL_Q

#define OK_CRLF               "OK"CRLF
#define ERROR_CRLF            "Error"CRLF

#define SHELL_ERRMSG_NOTIMPLEMENTED   "NOT IMPLEMENTED"CRLF
#define SHELL_ERRMSG_CONFIGERROR      ERROR_CRLF"COULD NOT SET CONFIG"CRLF
#define SHELL_ERRMSG_MEMALLOC         ERROR_CRLF"Memory allocation failed"CRLF
#define SHELL_ERRMSG_MAINTENANCEMODE  ERROR_CRLF"Command unavailable in maintenance mode"CRLF
#define SHELL_MSG_REBOOT              "Reboot to take effect"CRLF
#define SHELL_MSG_CONFIG_SET          "config updated"CRLF

/*! The max length of a response line. */
#define SHELL_MAX_MSGOUT_LEN   128

/*!
 * The exec command return status.
 */
typedef enum {
   SHELL_EXECSTATUS_OK,               // The command was successfully executed.
   SHELL_EXECSTATUS_OK_NO_FREE,       // The command was successfully executed, but the caller has not to free buffer.
   SHELL_EXECSTATUS_KO,               // The command execution failed.
} eExecStatus;


/*!
 *  \brief Function pointer typedef that each command should abide to.
 *
 *  \param xModId         Input. The module that is calling this function.
 *  \param FsNavId        Input. The file system navigator id to use if the cmd
 *                        is a file system command.
 *  \param ac             Input. The argument counter.
 *  \param av             Input. The argument vector.
 *  \param ppcStringReply Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        Else a malloc for the response string is performed if
 *                        the returned status is OK; the caller must then free
 *                        this string.
 *
 *  \return the status of the command execution.
 */
typedef eExecStatus (*pfShellCmd) ( eModId xModId, signed short FsNavId,
                                    int ac, signed portCHAR *av[],
                                    signed portCHAR **ppcStringReply);


/*!
 *  \brief a command line executor.
 *
 *  \param pcStringCmd    Input. The cmd line to execute. NULL-terminated string.
 *                        Format: cmd [arg[=val]], 2 (arg,val) maximum.
 *                        WARNING: this string will be modified.
 *  \param xModId         Input. The module that is calling this exe function.
 *  \param FsNavId        Input. The file system navigator id to use if the cmd
 *                        is a file system command.
 *  \param ppcStringReply Output. The caller must free this string (by calling vportFree())
 *                        only if it is non-NULL and the returned status is OK.
 *
 *  \return the status of the command execution.
 */
eExecStatus Shell_exec( signed portCHAR *pcStringCmd,
                        eModId xModId,
                        signed short FsNavId,
                        signed portCHAR **ppcStringReply);

/*!
 *  \brief Print a string directly to the output stream of a module.
 *
 *  \param xModId            Input. The module associated with the target output stream.
 *  \param pcStringToDisplay Input. The string to print.
 *
 */
void v_shell_Print_String_To_Requester_Stream( eModId xModId,
                                               const signed portCHAR *pcStringToDisplay );

#endif // __SHELL_H__
