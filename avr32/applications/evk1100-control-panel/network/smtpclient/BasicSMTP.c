/*****************************************************************************
 *
 * \file
 *
 * \brief Basic SMTP Client for AVR32 UC3.
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




#include <string.h>

#include "gpio.h" // Have to include gpio.h before FreeRTOS.h as long as FreeRTOS
                  // redefines the inline keyword to empty.

// Scheduler includes.
#include "FreeRTOS.h"
#include "task.h"
#include "BasicSMTP.h"
#include "semphr.h"

// Demo includes.
#include "portmacro.h"
#include "intc.h"
#include "fsaccess.h"
#include "conf_explorer.h"
#include "config_file.h"
#include "supervisor.h"
#include "ethernet.h"

// lwIP includes.
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "lwip/memp.h"
#include "lwip/stats.h"
#include "lwip/opt.h"
#include "lwip/api.h"
#include "lwip/arch.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "lwip/init.h"
#if ( (LWIP_VERSION) == ((1U << 24) | (3U << 16) | (2U << 8) | (LWIP_VERSION_RC)) )
#include "netif/loopif.h"
#endif


//! define the error string to use SMTP through configuration
#define SMTP_ERRMSG_CONFIGURESMTP         "SMTP has not been compiled, please set SMTP_USED to 1 at preprocessor level\r\n"


#if (SMTP_USED == 1)

// undefine read in order to use fsaccess functions and not lwIP ones
#ifdef read
#undef read
#endif

// undefine write in order to use fsaccess functions and not lwIP ones
#ifdef write
#undef write
#endif


//! SMTP default port
#define SMTP_PORT     25
//! SMTP HELO code answer
#define SMTP_HELO_STRING                  "220"
//! SMTP end of transmission code answer
#define SMTP_END_OF_TRANSMISSION_STRING   "221"
//! SMTP OK code answer
#define SMTP_OK_STRING                    "250"
//! SMTP start of transmission code answer
#define SMTP_START_OF_TRANSMISSION_STRING "354"
//! SMTP DATA string
#define SMTP_DATA_STRING                  "DATA\r\n"
//! SMTP EOM string
#define SMTP_MAIL_END_STRING              "\r\n.\r\n"
//! SMTP QUIT string
#define SMTP_QUIT_STRING                  "QUIT\r\n"

//! the size of a packet
#define SMTP_PACKET_SIZE                  512
//! the mailbox queue size
#define SMTP_MAILS_QUEUE_SIZE             2

//! The config file location
#define SMTP_CONFIG_FILE                  "A:/CFG/smtp.txt"

//! The CFG system mutex.
extern xSemaphoreHandle   xCFGMutex;

//! states for SMTP state machine
typedef enum{
  eSMTPIdle = 0,      // IDLE mode
  eSMTPHeloSent,      // HELO has been sent
  eSMTPMailFromSent,  // MAIL FROM has been set
  eSMTPMailToSent,    // RECPT TO has been sent
  eSMTPDataSent,      // DATA has been sent
  eSMTPContentSent,   // EOM has been sent
  eSMTPQuitSent,      // QUIT has been sent
  eSMTPMailSent,      // End of state machine, will return to IDLE
  eSMTPNbState        // should be used to know the states number
}eSMTPCurrentStateType;


//! SMTP sending state mchine current state
eSMTPCurrentStateType eSMTPCurrentState = eSMTPIdle;

//! temporary buffer for SMTP response
portCHAR cTempBuffer[200];

//! mail recipient
portCHAR cMailTo[40];

//! mail sender
portCHAR cMailFrom[40];

//! Server Name
portCHAR cServerName[16];

//! SMTP port
static unsigned int uiSMTPPort;

//! The handle of the queue of mails.
static xQueueHandle xMailsQueue = 0;

//! mail structure posted through mail box
typedef struct {
  portCHAR * File;    // filename if user wants to send its content
  portCHAR * Subject; // subject of the mail
  bool NeedFree;      // flag to free structure if not posted from ISR
}xMailDef;





//! var for mail params under IT
xMailDef xMailFromISR;


/*!
 *  \brief Post a message to send
 *
 *  \param MailSubject : the subject for the mail to send
 *  \param Filename : the filename for data to send as mail content
 */
void v_SMTP_Post(portCHAR * MailSubject, portCHAR * Filename)
{
xMailDef * pxMail;

   // try to allocate a descriptor for this mail
   pxMail = (xMailDef *) pvPortMalloc(sizeof(xMailDef));
   // if allocation succeeds
   if (pxMail != NULL)
   {
      // try to allocate a buffer for the subject
      pxMail->Subject = (portCHAR *) pvPortMalloc(strlen(MailSubject));
      // if allocation succeeds
      if (pxMail->Subject != NULL)
      {
         // copy the subject to mailbox item
         strcpy(pxMail->Subject, MailSubject );
         // if a filename as been provided
         if (Filename != NULL)
         {
            // try to allocate a buffer for the filename
            pxMail->File = (portCHAR *) pvPortMalloc(strlen(Filename));
            // if allocation succeeds
            if (pxMail->File != NULL)
            {
               // copy the filename to mailbox item
               strcpy(pxMail->File, Filename );
            }
         }
         else
         {
            // mark field as unused
            pxMail->File = NULL;
         }
         // item must be free, so mark it so.
         pxMail->NeedFree = pdTRUE;
         // post item to the mailbox
         xQueueSend( xMailsQueue, (void *)&pxMail, 0 );
      }
   }
}

/*!
 *  \brief Post a message to send from ISR
 *
 *  \param MailSubject : MUST be a const, no dynamic allocation in ISR
 *  \param Filename :    MUST be a const, no dynamic allocation in ISR
 */
void v_SMTP_PostFromISR(portCHAR * MailSubject, portCHAR * Filename)
{
xMailDef * pxMail = &xMailFromISR;
portBASE_TYPE xYieldRequired = pdFALSE;

   // reference mail subject
   pxMail->Subject = MailSubject;
   // reference file name
   pxMail->File = Filename;
   // reference IT field
   pxMail->NeedFree = pdFALSE;

   // post message to queue, disable IT prior to do it
   portENTER_CRITICAL();
   xQueueSendFromISR( xMailsQueue, (void *)&pxMail, &xYieldRequired );
   portEXIT_CRITICAL();
}


/*!
 *  \brief Task for SMTP management
 */
portTASK_FUNCTION( vBasicSMTPClient, pvParameters )
{

   struct sockaddr_in stServeurSockAddr;
   portLONG lRetval;
   portLONG lSocket = -1;
   portLONG ulResponseCode = 0;
   xMailDef * pxMail;
   int Size, SizeRead;
   portCHAR * pcRespData;
   int fd;
   portCHAR cToken[6];

   // Just to stop compiler warnings.
   ( void ) pvParameters;

   // Create the xMailsQueue capable of containing DATALOG_LOGSQUEUE_SIZE ptrs
   // to xLogDef structures.
   xMailsQueue = xQueueCreate( SMTP_MAILS_QUEUE_SIZE, sizeof( xMailDef * ) );

   // SMTP configuration.
   // Get the xCFGMutex.
   if( pdFALSE == x_supervisor_SemaphoreTake( xCFGMutex, 500 ) )
   {
      // Failed to get the CFG mutex, use the default HTTP config.
      uiSMTPPort = SMTP_PORT;
      cMailTo[0] = '\0';
      cMailFrom[0] = '\0';
      cServerName[0] = '\0';
   }
   // Mutex has been taken
   else
   {
      // get the field value for port number
      if (config_file_get_value(SMTP_CONFIG_FILE, "port" , cToken) >= 0)
      {
         sscanf(cToken, "%u", &uiSMTPPort);
      }
      // if it does not exist, use the default value
      else
      {
         uiSMTPPort = SMTP_PORT;
      }
      // try to get the mailto field
      if (config_file_get_value(SMTP_CONFIG_FILE, "mailto", cMailTo) < 0)
      {
         cMailTo[0] = '\0';
         // can't find field in config file, warn user
         NAKED_TRACE_COM2("Warning : No mailto configured !!Please fill mailto= field in %s\r\n", SMTP_CONFIG_FILE);
      }
      // try to get the mailfrom field
      if (config_file_get_value(SMTP_CONFIG_FILE, "mailfrom", cMailFrom) < 0)
      {
         cMailFrom[0] = '\0';
         // can't find field in config file, warn user
         NAKED_TRACE_COM2("Warning : No mailfrom configured !!Please fill mailfrom= field in %s\r\n", SMTP_CONFIG_FILE);
      }
      // try to get the server field
      if (config_file_get_value(SMTP_CONFIG_FILE, "server", cServerName) < 0)
      {
         cServerName[0] = '\0';
         // can't find field in config file, warn user
         NAKED_TRACE_COM2("Warning : No server name configured !! Please fill server= field in %s\r\n", SMTP_CONFIG_FILE);
      }
      // Release the xCFGMutex.
      x_supervisor_SemaphoreGive( xCFGMutex );
   }

   for(;;)
   {
      // NOTE: the task should be resumed when it is necessary to send a mail
      // Get the oldest mail from the queue.
      // NOTE: we are sure there is an item to get => no block time.
      if( pdTRUE == xQueueReceive( xMailsQueue, &pxMail, ( portTickType )1000 ) )
      {
         if (cServerName[0] == '\0')
         {
            // can't find field in config file, warn user
            NAKED_TRACE_COM2("Warning : No server name configured !! Please fill server= field in %s\r\n", SMTP_CONFIG_FILE);
         }
         else if (cMailTo[0] == '\0')
         {
            // can't find field in config file, warn user
            NAKED_TRACE_COM2("Warning : No mailto configured !!Please fill mailto= field in %s\r\n", SMTP_CONFIG_FILE);
         }
         else if (cMailFrom[0] == '\0')
         {
            // can't find field in config file, warn user
            NAKED_TRACE_COM2("Warning : No mailfrom configured !!Please fill mailfrom= field in %s\r\n", SMTP_CONFIG_FILE);
         }
         else
         {
            // Set up port
            memset(&stServeurSockAddr, 0, sizeof(stServeurSockAddr));
            stServeurSockAddr.sin_len = sizeof(stServeurSockAddr);
            stServeurSockAddr.sin_addr.s_addr = inet_addr(cServerName);
            stServeurSockAddr.sin_port = htons(uiSMTPPort);
            stServeurSockAddr.sin_family = AF_INET;

            // socket as a stream
            if ( (lSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            {
               // socket failed
               NAKED_TRACE_COM2("Socket Failed\r\n");
            }
            // connect to the server
            if(connect(lSocket,(struct sockaddr *)&stServeurSockAddr,sizeof(stServeurSockAddr)) < 0)
            {
               // connect failed
               NAKED_TRACE_COM2("Connect Failed\r\n");
            }
            else
            {
               eSMTPCurrentState = eSMTPIdle;
               while ( eSMTPCurrentState != eSMTPMailSent )
               {
                  // wait for SMTP Server answer
                  do
                  {
                     lRetval = recv(lSocket, cTempBuffer, sizeof(cTempBuffer), 0);
                  }while (lRetval <= 0);

                  cTempBuffer[3] = '\0';
                  // Get the response code from server
                  ulResponseCode = atoi(cTempBuffer);

                  switch (ulResponseCode)
                  {
                     case 220:
                     {
                        // send helo
                        send(lSocket, "HELO ", 5, 0);
                        send(lSocket, cServerName, strlen(cServerName), 0);
                        send(lSocket, "\r\n", 2, 0);
                        eSMTPCurrentState = eSMTPHeloSent;
                        break;
                     }
                     case 221:
                     {
                        // QUIT sequence has been acknowledged by server
                        if (eSMTPCurrentState == eSMTPQuitSent)
                        {
                           eSMTPCurrentState = eSMTPMailSent;
                        }
                        break;
                     }
                     case 250:
                     {
                        if (eSMTPCurrentState == eSMTPHeloSent)
                        {
                           // send MAIL FROM
                           send(lSocket, "MAIL FROM: <", 12, 0); ;
                           send(lSocket, cMailFrom, strlen(cMailFrom), 0);
                           send(lSocket, ">\r\n", 3, 0);
                           eSMTPCurrentState = eSMTPMailFromSent;
                        }
                        else if (eSMTPCurrentState == eSMTPMailFromSent)
                        {
                           // send MAIL TO
                           send(lSocket, "RCPT TO: <", 10, 0); ;
                           send(lSocket, cMailTo, strlen(cMailTo), 0);
                           send(lSocket, ">\r\n", 3, 0);
                           eSMTPCurrentState = eSMTPMailToSent;
                        }
                        else if (eSMTPCurrentState == eSMTPMailToSent)
                        {
                           // send DATA
                           send(lSocket, SMTP_DATA_STRING, 6, 0);
                           eSMTPCurrentState = eSMTPDataSent;
                        }
                        else if (eSMTPCurrentState == eSMTPContentSent)
                        {
                           // send QUIT
                           send(lSocket, SMTP_QUIT_STRING, 6, 0);
                           eSMTPCurrentState = eSMTPQuitSent;
                        }
                        break;
                     }
                     case 354:
                     {
                        if (eSMTPCurrentState == eSMTPDataSent)
                        {
                           // set Subject field
                           send(lSocket, "Subject:", 8, 0);
                           // add subject
                           send(lSocket, pxMail->Subject, strlen(pxMail->Subject), 0);
                           send(lSocket, "\r\nFROM:", 7, 0);
                           send(lSocket, cMailFrom, strlen(cMailFrom), 0);
                           send(lSocket, "\r\nTO:", 5, 0);
                           send(lSocket, cMailTo, strlen(cMailTo), 0);
                           send(lSocket, "\r\n\r\n", 4, 0);
                           // if a file has been specified, copy the content in the mail body
                           if (pxMail->File != NULL)
                           {
                              // allocate response
                              pcRespData = (portCHAR *)pvPortMalloc(SMTP_PACKET_SIZE);
                              if (pcRespData != NULL)
                              {
                                 if ((fd = open((const char *)pxMail->File, O_RDONLY)) >= 0)
                                 {
                                    Size = fsaccess_file_get_size(fd);
                                    // get sectors of maximum size
                                    while(Size > 0)
                                    {
                                       // get the data from filesystem
                                       SizeRead = read(fd, pcRespData, SMTP_PACKET_SIZE);
                                       // if error occurs during the read
                                       if (SizeRead <= 0)
                                       {
                                          // end the loop and send what has already been added
                                          break;
                                       }
                                       // sned data to the socket
                                       send(lSocket, pcRespData, SizeRead, 0);
                                       // decrease remaing size
                                       Size -= SizeRead;
                                    }
                                    // close the file
                                    close(fd);
                                    // free the buffer
                                    vPortFree(pcRespData);
                                 }
                                 else
                                 {
                                    // warn user : can't open the file
                                    NAKED_TRACE_COM2("Open file fails\r\n");
                                 }
                              }
                              else
                              {
                                 // warn user : can't malloc the file buffer
                                 NAKED_TRACE_COM2("SMTP : Malloc fails\r\n");
                              }
                           }
                           // add "<CRLF>.<CRLF>"
                           send(lSocket, SMTP_MAIL_END_STRING, 5, 0);
                           eSMTPCurrentState = eSMTPContentSent;
                        }
                        break;
                     }
                     default:
                     {
                        // unknown SMTP code
                        NAKED_TRACE_COM2("Unimplented %l SMTP response from server\r\n",ulResponseCode);
                        // break loop and reset state machine
                        eSMTPCurrentState = eSMTPMailSent;
                        break;
                     }
                  }
               }
               // close the socket
               close(lSocket);
            }
            // if the item was not posted from ISR
            if (pxMail->NeedFree == pdTRUE)
            {
               // if a file has been specified
               if ( pxMail->File != NULL )
               {
                  // free the item
                  vPortFree(pxMail->File);
               }
               // free the items
               vPortFree(pxMail->Subject);
               vPortFree(pxMail);
            }
         }
      }
   }
}
#endif


/*! \brief The set smtp client config command: set the SMTP variables.
 * \verbatim
 *         Takes 4 parameters :
 *                        port=ppppp
 *                        server=smtp.domain.com
 *                        mailto=recipient@domain.com
 *                        mailfrom=sender@domain.com
 * \endverbatim
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
                                     signed portCHAR **ppcStringReply )
{
#if (SMTP_USED == 1)
  if (config_file_set_value(SMTP_CONFIG_FILE, ac, av) != 0)
  {
    *ppcStringReply = (signed portCHAR *)SHELL_ERRMSG_CONFIGERROR;
    // return error
    return( SHELL_EXECSTATUS_KO );
  }

  if (!strcmp((char *)av[0] , "port"))
  {
    uiSMTPPort = atoi((char *)av[1]);
    *ppcStringReply = (signed portCHAR *)SHELL_MSG_CONFIG_SET;
    return( SHELL_EXECSTATUS_OK_NO_FREE );
  }
  else if (!strcmp((char *)av[0] , "mailto"))
  {
    cMailTo[0]='\0';
    strncat(cMailTo, (char *)av[1], Min(sizeof(cMailTo),strlen((char *)av[1])));
    *ppcStringReply = (signed portCHAR *)SHELL_MSG_CONFIG_SET;
    return( SHELL_EXECSTATUS_OK_NO_FREE );
  }
  else if (!strcmp((char *)av[0] , "mailfrom"))
  {
    cMailFrom[0]='\0';
    strncat(cMailFrom, (char *)av[1], Min(sizeof(cMailFrom),strlen((char *)av[1])));
    *ppcStringReply = (signed portCHAR *)SHELL_MSG_CONFIG_SET;
    return( SHELL_EXECSTATUS_OK_NO_FREE );
  }
  else if (!strcmp((char *)av[0] , "server"))
  {
    cServerName[0]='\0';
    strncat(cServerName, (char *)av[1], Min(sizeof(cServerName),strlen((char *)av[1])));
    *ppcStringReply = (signed portCHAR *)SHELL_MSG_CONFIG_SET;
    return( SHELL_EXECSTATUS_OK_NO_FREE );
  }
  else
  {
    *ppcStringReply = (signed portCHAR *)SHELL_ERRMSG_CONFIGERROR;
    return( SHELL_EXECSTATUS_KO );
  }
#else
    *ppcStringReply = (signed portCHAR *)SMTP_ERRMSG_CONFIGURESMTP;
    return( SHELL_EXECSTATUS_KO );
#endif
}


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
                                     signed portCHAR **ppcStringReply )
{

  if(ppcStringReply != NULL)
  {
    // allocate a buffer for answer
    *ppcStringReply = (signed portCHAR *)pvPortMalloc(MAX_CONFIG_FILE_SIZE);
    if( NULL == *ppcStringReply )
    {
       *ppcStringReply = (signed portCHAR *)SHELL_ERRMSG_MEMALLOC;
       return( SHELL_EXECSTATUS_KO );
    }
#if (SMTP_USED == 1)
    // get SMTP variables and add them to the buffer
    sprintf((char *)*ppcStringReply,"port=%u\r\nserver=%s\r\nmailto=%s\r\nmailfrom=%s\r\n",
                                     uiSMTPPort,
                                     cServerName,
                                     cMailTo,
                                     cMailFrom);
#else
    // get SMTP variables and add them to the buffer
    sprintf((char *)*ppcStringReply,"port=NotCompiled\r\nserver=NotCompiled\r\nmailto=NotCompiled\r\nmailfrom=NotCompiled\r\n");
#endif
    /* no error, return */
    return( SHELL_EXECSTATUS_OK );
  }
  return( SHELL_EXECSTATUS_KO );
}
