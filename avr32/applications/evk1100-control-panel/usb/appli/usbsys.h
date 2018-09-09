/*****************************************************************************
 *
 * \file
 *
 * \brief AVR32 UC3 Control Panel USB system interface.
 *
 * This module is in charge of defining the USB objects and starting the USB tasks.
 *
 * - OS:                 FreeRTOS port on AVR32 UC3.
 * - Boards:             EVK1100
 *
 *
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
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


#ifndef __USBSYS_H__
#define __USBSYS_H__

#include <time.h>

#include "FreeRTOS.h"

#include "shell.h"

#include "supervisor.h"


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________


//_____ D E C L A R A T I O N S ____________________________________________


/*!
 * Start the USB module.
 *
 * \return true upon success, else false.
 */
bool b_usbsys_start( void );


/*!
 *  \brief In host mode, display basic low-level information about the connected device.
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
 *  \return the status of the command execution. If the status is SHELL_EXECSTATUS_KO,
 *  the caller MUST NOT FREE the response string. If the status is SHELL_EXECSTATUS_OK
 *  and the response string is not NULL, the caller MUST free the response string.
 */
eExecStatus e_usbsys_lsusb( eModId xModId, signed short FsNavId,
                       int ac, signed portCHAR *av[],
                       signed portCHAR **ppcStringReply );


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
                                      signed portCHAR **ppcStringReply );

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
                                     signed portCHAR **ppcStringReply );


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
                                      signed portCHAR **ppcStringReply );


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
 *  \return the status of the command execution. If the status is SHELL_EXECSTATUS_KO,
 *  the caller MUST NOT FREE the response string. If the status is SHELL_EXECSTATUS_OK
 *  and the response string is not NULL, the caller MUST free the response string.
 */
eExecStatus e_usbsys_cp_web_to_local( eModId xModId, signed short FsNavId,
                                      int ac, signed portCHAR *av[],
                                      signed portCHAR **ppcStringReply );

#endif // __USBSYS_H__
