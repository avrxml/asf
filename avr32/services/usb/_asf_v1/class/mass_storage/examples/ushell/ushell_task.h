/**************************************************************************
 *
 * \file
 *
 * \brief uShell command line interpreter.
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
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#ifndef _USHELL_TASK_H_
#define _USHELL_TASK_H_


//_____ I N C L U D E S ____________________________________________________

#include "fs_com.h"

//_____ M A C R O S ________________________________________________________


/*! \name Specific ASCII Values
 */
//! @{
#define  ASCII_CR             '\r'
#define  ASCII_LF             '\n'
#define  ASCII_BKSPACE        '\b'
#define  ASCII_ESCAPE         0x1B
#define  ASCII_CTRL_Q         0x11
#define  ASCII_CTRL_C         0x03
//! @}


/*! \name Shell Commands
 */
//! @{
#define  CMD_NONE             0x00
#define  CMD_NB_DRIVE         0x01
#define  CMD_DF               0x02
#define  CMD_FORMAT           0x03
#define  CMD_MOUNT            0x04
#define  CMD_SPACE            0x05
#define  CMD_LS               0x06
#define  CMD_LS_MORE          0x07
#define  CMD_CD               0x08
#define  CMD_UP               0x09
#define  CMD_CAT              0x0A
#define  CMD_CAT_MORE         0x0B
#define  CMD_HELP             0x0C
#define  CMD_MKDIR            0x0D
#define  CMD_TOUCH            0x0E
#define  CMD_RM               0x0F
#define  CMD_APPEND           0x10
#define  CMD_SET_ID           0x11
#define  CMD_GOTO_ID          0x12
#define  CMD_CP               0x13
#define  CMD_MV               0x14
#define  CMD_SYNC             0x15
#define  CMD_PERFORM          0x16

#define  CMD_LS_USB           0x18
#define  CMD_USB_SUSPEND      0x19
#define  CMD_USB_RESUME       0x1A
//! @}



/*! \name String Values for Commands
 */
//! @{
#define  STR_DISK             "disk"
#define  STR_DF               "df"
#define  STR_FORMAT           "format"
#define  STR_MOUNT            "mount"
#define  STR_SPACE            "space"
#define  STR_LS               "ls"
#define  STR_LS_MORE          "ls|more"
#define  STR_CD               "cd"
#define  STR_UP               "cd.."
#define  STR_CAT              "cat"
#define  STR_CAT_MORE         "cat|more"
#define  STR_HELP             "help"
#define  STR_MKDIR            "mkdir"
#define  STR_TOUCH            "touch"
#define  STR_RM               "rm"
#define  STR_APPEND           "append"
#define  STR_MARK             "mark"
#define  STR_GOTO             "goto"
#define  STR_CP               "cp"
#define  STR_MV               "mv"
#define  STR_SYNC             "sync"
#define  STR_PERFORM          "perf"

#define  STR_LS_USB           "lsusb"
#define  STR_USB_SUSPEND      "suspend"
#define  STR_USB_RESUME       "resume"
//! @}


/*! \name String Messages
 */
//! @{
#define  MSG_PROMPT           "$>"
#define  MSG_WELCOME          "\x0C" \
                              "--------------------------\r\n" \
                              "    ATMEL AVR32 uShell\r\n" \
                              "--------------------------\r\n"
#define  MSG_EXIT             "\x0C" \
                              "------------------------\r\n" \
                              "uShell not available\r\n" \
                              "------------------------\r\n"
#define  MSG_ER_CMD_NOT_FOUND "Command not found\r\n"
#define  MSG_ER_MOUNT         "Unable to mount drive\r\n"
#define  MSG_ER_DRIVE         "Drive does not exist\r\n"
#define  MSG_ER_RM            "Can not erase, if the name is a directory, check it is empty\r\n"
#define  MSG_ER_UNKNOWN_FILE  "Unknown file\r\n"
#define  MSG_ER_FORMAT        "Format fails\r\n"
#define  MSG_APPEND_WELCOME   "\r\nSimple text editor, enter char to append, ^q to exit and save\r\n"
#define  MSG_HELP             "\r\n---- Ushell Commands:\r\n" \
                              " HELP                  Provides Help information for Ushell commands.\r\n" \
                              " !                     Previous command (history).\r\n" \
                              " $                     Next command (history).\r\n" \
                              "\r\n---- File Systems Commands:\r\n" \
                              " DISK                  Displays the number of drives.\r\n" \
                              " DF                    Displays free space of all connected drives.\r\n" \
                              " FORMAT [/A:]          Formats drive, /A drive letter (a, b, etc.).\r\n" \
                              " MOUNT [/A:]           Mounts drive, /A drive letter (a, b, etc.).\r\n" \
                              " [/A:]                 Mounts drive, /A drive letter (a, b, etc.).\r\n" \
                              " SPACE                 Displays drive capacity.\r\n" \
                              " LS [|MORE]            Displays a list of files and subdirectories in a directory.\r\n" \
                              " CD [..][path]         Changes the current directory.\r\n" \
                              " CAT [file name]       Displays file contents.\r\n" \
                              " MKDIR [dir name]      Creates a directory.\r\n" \
                              " TOUCH [file name]     Creates a file.\r\n" \
                              " RM [*][file name]     Deletes one or more files or directories.\r\n" \
                              " APPEND [file name]    Appends file from terminal input\r\n" \
                              " MARK                  Record the current directory in bookmark\r\n" \
                              " GOTO                  Go to bookmarked directory\r\n" \
                              " CP [file name][path\\] Copys file to bookmarked directory or to path argument.\r\n" \
                              " MV [path] [new name]  Renames file from path with new name.\r\n" \
                              " SYNC [src_path] [dest_path]  Synchronizes a folder content with other folder.\r\n" \
                              " PERF [/A:] [/A:]      Evaluates the transfer rate between two devices.\r\n" \
                              "\r\n---- USB Host Commands:\r\n" \
                              " LSUSB                 Displays USB information.\r\n" \
                              " SUSPEND               Suspends USB bus activity.\r\n" \
                              " RESUME                Resumes USB bus activity.\r\n" \

#define  MSG_NO_DEVICE        "Not currently applicable to supported connected device(s) if any\r\n"
#define  MSG_REMOTE_WAKEUP_OK "Device supports remote wake-up\r\n"
#define  MSG_REMOTE_WAKEUP_KO "Device does not support remote wake-up\r\n"
#define  MSG_SELF_POWERED     "Device is self-powered\r\n"
#define  MSG_BUS_POWERED      "Device is bus-powered\r\n"
#define  MSG_USB_SUSPENDED    "USB is suspended!\r\n"
#define  MSG_OK               "ok\r\n"
#define  MSG_KO               "FAIL\r\n"
#define  MSG_DEVICE_FULL_SPEED "Device is full-speed\r\n"
#define  MSG_DEVICE_LOW_SPEED  "Device is low-speed\r\n"
#define  MSG_DEVICE_HIGH_SPEED "Device is high-speed\r\n"
#define  MSG_ER_PASTE         "Paste Fail\r\n"
//! @}


//_____ D E C L A R A T I O N S ____________________________________________

extern void ushell_task_init(uint32_t pba_hz);
#ifdef FREERTOS_USED
extern void ushell_task(void *pvParameters);
#else
extern void ushell_task(void);
#endif

#endif  // _USHELL_TASK_H_
