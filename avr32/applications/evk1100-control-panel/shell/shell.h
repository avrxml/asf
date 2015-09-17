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
