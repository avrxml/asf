/*****************************************************************************
 *
 * \file
 *
 * \brief Control Panel COM1 shell module.
 *
 * This module manages a command shell on COM1.
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


/*!
 * Brief description of the module.
 * This module is in charge of a command line shell on COM1.
 *
 * Detailed description of the module.
 * Creation of one task that is in charge of getting a command line from COM1,
 * launching the execution of the command and giving feedback of this execution
 * to COM1.
 *
 */

/* Scheduler include files. */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"

/* Demo program include files. */
#include "serial.h"

#include "supervisor.h"
#include "serial.h"
#include "fs_com.h"
#include "fsaccess.h"

#include "shell.h"

#include "com1shell.h"

#include "tracedump.h"


//_____ M A C R O S ________________________________________________________

// String messages
#define COM1SHELL_MSG_LINETOOLONG       "\r\nLine too long!\r\n"


/*! The max length of a cmd name. */
#define MAX_CMD_LEN                 32
/*! The max length of a cmd line. */
#define COM1SHELL_MAX_CMDLINE_LEN   (MAX_CMD_LEN + 2 * MAX_FILE_PATH_LENGTH)


#define COM1SHELL_STACK_SIZE		    384

/* We should find that each character can be queued for Tx immediately and we
don't have to block to send. */
#define COM1SHELL_NO_BLOCK		    ( ( portTickType ) 0 )

/* The Rx task will block on the Rx queue for a long period. */
#define COM1SHELL_RX_BLOCK_TIME		    ( ( portTickType ) 0xffff )
#define COM1SHELL_RX_SHORT_BLOCK_TIME	    ( ( portTickType ) 1 )


//! Baud rate used by the serial port 1.
#define COM1SHELL_BAUDRATE      ( ( unsigned portLONG ) 57600 )
//! COM Port1 buffers length.
#define COM1SHELL_RXBUFFER_LEN    ( ( unsigned portLONG ) 64 )
#define COM1SHELL_TXBUFFER_LEN    ( ( unsigned portLONG ) 32 )

//! Messages printed on the COM1 port.
#define COM1SHELL_MSG_BANNER   "\x0C\r\n---------- ATMEL AVR32 Shell ----------\r\n"
#define COM1SHELL_MSG_PROMPT   "$>"

/*! The COM1 shell module status. */
static eStatus xCom1ShellStatus = SYS_STATUS_STARTUP;

/* Handle to the com port. */
static xComPortHandle xComPort1Hndl = NULL;

/*! COM1 shell task handle. */
xTaskHandle xCom1ShellHndl = NULL;

/*! COM1 shell navigator ID. */
signed short      sCom1ShellNavId;


/*! The cmd line string. */
static signed portCHAR acStringCmd[COM1SHELL_MAX_CMDLINE_LEN+1];

static unsigned portCHAR prvGetCmdLine( void );                    // Forward.
static signed portBASE_TYPE prvGetChar( signed portCHAR *pcByte,
                                        portTickType xBlockTime ); // FORWARD
static portTASK_FUNCTION( vCom1ShellTask, pvParameters );          // Forward
/*-----------------------------------------------------------*/

/*!
 * Start the COM1 shell module.
 * \param uxPriority The priority base of the COM1 shell tasks.
 */
void vStartCom1Shell( unsigned portBASE_TYPE uxPriority )
{
   xCom1ShellStatus = SYS_STATUS_STARTUP;

   /* Get a File System navigator for COM1SHELL. */
   fsaccess_take_mutex();
   sCom1ShellNavId = fsaccess_alloc_nav_id();
   fsaccess_give_mutex();

   /* Try to mount local drive */
   v_com1shell_mount_local_drive();

   /* Spawn the shell task. */
   if( xTaskCreate( vCom1ShellTask, ( const signed portCHAR * ) "SH",
                    COM1SHELL_STACK_SIZE, NULL, uxPriority, &xCom1ShellHndl ) != pdPASS )
      xCom1ShellStatus = SYS_STATUS_DOWN;
}

/*!
 * \brief Stop the com1shell module resources.
 */
void v_com1shell_stopResources( void )
{
  vSerialClose( xComPort1Hndl );
}

/*!
 * \brief Return the status of the COM1 shell module.
 * \return one of the eStatus enum value.
 */
eStatus xCom1Shell_GetStatus( void )
{
   return xCom1ShellStatus;
}
/*-----------------------------------------------------------*/

static portTASK_FUNCTION( vCom1ShellTask, pvParameters )
{
   signed portCHAR   *pcStringReply = NULL;
   eExecStatus       xExeStatus;


   /* Just to stop compiler warnings. */
   ( void ) pvParameters;

   /* Initialize the COM1 port. */
   xComPort1Hndl = xUsartInit( serCOM1, COM1SHELL_BAUDRATE,
                              COM1SHELL_RXBUFFER_LEN, COM1SHELL_TXBUFFER_LEN );
   if(xComPort1Hndl == 0)
   {
     xCom1ShellStatus = SYS_STATUS_DOWN;
     vTaskDelete(NULL); // Kill this task.
   }

   xCom1ShellStatus = SYS_STATUS_RUNNING;

   vcom1shell_PrintMsg((signed portCHAR *)COM1SHELL_MSG_BANNER);

   for(;;)
   {
      // 1) Get the command.
      if(prvGetCmdLine() == 0)
         continue;

      // 2) Execute the command.
      xExeStatus = Shell_exec(acStringCmd, SYS_MODID_COM1SHELL, sCom1ShellNavId, &pcStringReply);

      // 3) Print the response.
      if( NULL != pcStringReply )
         vcom1shell_PrintMsg(pcStringReply);
      if( ( NULL != pcStringReply ) && ( SHELL_EXECSTATUS_OK == xExeStatus ) )
         vPortFree(pcStringReply);
   }
}
/*-----------------------------------------------------------*/

/*!
 * \brief Get a command line.
 *
 * \return Command line length.
 */
static unsigned portCHAR prvGetCmdLine( void )
{
   signed portCHAR     c;
   unsigned portCHAR   idx = 0;


   vcom1shell_PrintMsg( (signed portCHAR *)COM1SHELL_MSG_PROMPT );
   while( idx < COM1SHELL_MAX_CMDLINE_LEN )
   {
      if ( prvGetChar( &c, COM1SHELL_RX_BLOCK_TIME ) == pdTRUE )
      {
         switch (c)
         {
            case LF:
               vcom1shell_PutChar(CR);  // Add CR
               vcom1shell_PutChar(c);   // LF
               acStringCmd[idx] = '\0';  // Add NUL char
               return(idx);
             case CR:
               vcom1shell_PutChar(c);   // CR
               vcom1shell_PutChar(LF);  // Add CR
               acStringCmd[idx] = '\0';  // Add NUL char
               return(idx);
             case ABORT_CHAR:             // ^c abort cmd
               idx = 0;
               vcom1shell_PutChar(LF);           // Add LF
               vcom1shell_PrintMsg( (signed portCHAR *)COM1SHELL_MSG_PROMPT );
               break;
            case BKSPACE_CHAR:           // Backspace
               if (idx > 0)
               {
                  vcom1shell_PrintMsg( (signed portCHAR *)"\b \b" );
                  idx--;
               }
               break;
            default:
               vcom1shell_PutChar(c);            // Echo
               acStringCmd[idx++] = c;   // Append to cmd line
               break;
         }
      }
   }
   vcom1shell_PrintMsg( (signed portCHAR *)COM1SHELL_MSG_LINETOOLONG );
   return(0);
}


/*-----------------------------------------------------------*/

/*!
 * \brief Print a string to the COM1 port.
 */
void vcom1shell_PrintMsg(const signed portCHAR *pcString)
{
   unsigned portSHORT usRemainChar = 0;
   unsigned portSHORT usMsgLen = strlen((const portCHAR * )pcString);

   if(usMsgLen==0)
      return;
   usRemainChar = usMsgLen;
   do {
      usRemainChar = usUsartPutString( xComPort1Hndl,
                                       (const signed portCHAR *)(pcString + usMsgLen - usRemainChar),
                                       usRemainChar );
   }while( usRemainChar );
}
/*-----------------------------------------------------------*/

/*!
 * \brief Put a char to the COM1 port.
 */
void vcom1shell_PutChar(signed portCHAR cByte)
{
   xUsartPutChar( xComPort1Hndl, cByte, COM1SHELL_NO_BLOCK);
}
/*-----------------------------------------------------------*/

/*!
 * \brief Get a char from the COM1 port.
 */
signed portBASE_TYPE com1shell_GetChar(signed portCHAR *pcByte)
{
   return( xUsartGetChar( xComPort1Hndl, pcByte, COM1SHELL_RX_BLOCK_TIME ) );
}
/*-----------------------------------------------------------*/

/*!
 * \brief Get a char from the COM1 port.
 */
static signed portBASE_TYPE prvGetChar(signed portCHAR *pcByte, portTickType xBlockTime)
{
   return( xUsartGetChar( xComPort1Hndl, pcByte, xBlockTime ) );
}
/*-----------------------------------------------------------*/

/*!
 * \brief Mount default local drive 'a:'
 */
void v_com1shell_mount_local_drive( void )
{
   fsaccess_take_mutex();
   nav_select( sCom1ShellNavId );   // Select the COM1SHELL navigator.
   nav_drive_set(LUN_ID_AT45DBX_MEM);
   nav_partition_mount();
   fsaccess_give_mutex();
}

