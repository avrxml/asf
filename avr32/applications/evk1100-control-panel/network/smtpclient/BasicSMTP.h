/*****************************************************************************
 *
 * \file
 *
 * \brief Basic SMTP client for AVR32 UC3.
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

#ifndef BASIC_SMTP_CLIENT_H
#define BASIC_SMTP_CLIENT_H



#if (SMTP_USED == 1)

#include "portmacro.h"

/* The function that implements the SMTP client task. */
portTASK_FUNCTION_PROTO( vBasicSMTPClient, pvParameters );

/*  */
void v_SMTP_Post( portCHAR * MailSubject, portCHAR * Filename );
void v_SMTP_PostFromISR( portCHAR * MailSubject, portCHAR * Filename );



#else  // SMTP_USED != 1 : don't send message, simply trace on debug COM port

#include "tracedump.h"
#include "gpio.h"
#include "board.h"

#define v_SMTP_Post(x, y)           NAKED_TRACE_COM2("Send Mail with subject : %s\r\n",x)
#define v_SMTP_PostFromISR(x, y)
#endif


#include "shell.h"

/*! \brief The set smtp client config command: set the SMTP variables.
 * @verbatim
 *         Takes 4 parameters :
 *                        port=ppppp
 *                        server=smtp.domain.com
 *                        mailto=recipient@domain.com
 *                        mailfrom=sender@domain.com
 * @endverbatim
 *
 *  \note  This function must be of the type eExecStatus defined by the shell module.
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
eExecStatus e_smtpclient_cmd_set_config( eModId xModId, signed short FsNavId,
                                     int ac, signed portCHAR *av[],
                                     signed portCHAR **ppcStringReply );


/*! \brief The get smtp client config command: get the SMTP variables.
 *         Takes no parameter
 *
 *  \note  This function must be of the type eExecStatus defined by the shell module.
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
eExecStatus e_smtpclient_cmd_get_config( eModId xModId, signed short FsNavId,
                                     int ac, signed portCHAR *av[],
                                     signed portCHAR **ppcStringReply );
#endif

