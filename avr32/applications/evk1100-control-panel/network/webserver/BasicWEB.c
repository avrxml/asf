/*****************************************************************************
 *
 * \file
 *
 * \brief Basic WEB Server for AVR32 UC3.
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
  Implements a simplistic WEB server.  Every time a connection is made and
  data is received a dynamic page that shows the current TCP/IP statistics
  is generated and returned.  The connection is then closed.

  This file was adapted from a FreeRTOS lwIP slip demo supplied by a third
  party.
*/

/*
  Changes from V3.2.2

  + Changed the page returned by the lwIP WEB server demo to display the
    task status table rather than the TCP/IP statistics.
*/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#if (HTTP_USED == 1)


/* Standard includes. */
#include <stdio.h>
#include <string.h>

#include "conf_eth.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "serial.h"

/* Demo includes. */
/* Demo app includes. */
#include "portmacro.h"

/* lwIP includes. */
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "lwip/memp.h"
#include "lwip/stats.h"
#include "lwip/init.h"
#if ( (LWIP_VERSION) == ((1U << 24) | (3U << 16) | (2U << 8) | (LWIP_VERSION_RC)) )
#include "netif/loopif.h"
#endif

/* FAT includes */
#include "ctrl_access.h"
#include "fsaccess.h"
#include "config_file.h"

/* Ethernet includes */
#include "ethernet.h"

/* shell includes */
#include "supervisor.h"
#include "shell.h"
#include "datalog.h"

/* Default html page */
#include "defaultpage.htm.h"

/* trace include */
#include "tracedump.h"

#include "BasicWEB.h"

//_____ M A C R O S ________________________________________________________

/*! The port on which we listen. */
#define webHTTP_PORT    ( 80 )

/*! The server protocol. */
#define webSERVER_PROTOCOL "HTTP/1.1"

/*! Delay on close error. */
#define webSHORT_DELAY    ( 10 )

#ifndef ERR_DIR
#define ERR_DIR "errors"
#endif /* ERR_DIR */

/*! The server software name. */
#define webSERVER_SOFTWARE    "AVR32 UC3 WEB SERVER"

/*! The max number of simultaneous http connection. */
#define webHTTP_NB_CONN       1 // 3

/*! The priority of a http connection. */
#define webHTTP_CONNECTION_PRIORITY  (CTRLPANEL_WEB_SERVER_PRIORITY+1)

/*! The stack size of a http connection task */
#define webHTTP_CONNECTION_STACK_SIZE   384 // 256 // 512?

/*! The folder were server files are */
#define webSERVER_FOLDER "A:/WEB/"

/*! The config file location */
#define HTTP_CONFIG_FILE "A:/CFG/http.txt"

/*! The error buffer size for error page */
#define ERR_BUFFER_SIZE   512

/*! The max number of sector to try to send */
#define webNB_SECTOR_TO_SEND   4

/*! The max amount of fat data to try to send */
#define webMAX_DATA_TO_SEND    webNB_SECTOR_TO_SEND * FS_SIZE_OF_SECTOR

/*! The max timeout before killing all pending tasks (ms) */
#define webCONN_TIMEOUT    5000

//_____ D E F I N I T I O N S ______________________________________________

/* Ptr on read file function type. */
typedef ssize_t (*pf_read_src)( int fd, void *buf, size_t count );

/* Function to process the current connection */
static void prvweb_ParseHTMLRequest( struct netconn *pxNetCon );

/* Function to send a file to the current connection */
static void prvweb_SendFile( struct netconn *pxNetCon, char* filename );

/* Function to parse a request and send it to the shell */
static void prvweb_ParseCGIRequest( struct netconn *pxNetCon, char* path );

/* Function to send an error page to the current connection */
static void prvweb_SendErrorPage( struct netconn *pxNetCon, int s, char* extra_header, const char* text );

/* Function to prepare the body of the error page */
static unsigned portLONG prulweb_BuildErrorBody(portCHAR * response, int s, char* title, const char* text );

/* Function to prepare the tail of the error page */
static unsigned portLONG prulweb_BuildErrorTail(portCHAR * response);

/* Function to add header to the response */
static unsigned portLONG prulweb_BuildHeaders(portCHAR * response, int s, char* title, char* extra_header, char* me, char* mt);

/*! Current nb of HTTP connections. */
static portSHORT sCurrentNbHTTPConn = 0;

/*! HTTP port for server. */
static unsigned int webHttpPort = 0;

/*! Mutex to access the current_nb_http_conn variable. */
static xSemaphoreHandle xMutexNbHTTPConn;

/*! The handlers for connection tasks. */
xTaskHandle tTaskHandle[webHTTP_NB_CONN] = { NULL }; // { NULL, NULL, NULL };

/*! The size of the default contant html page. */
static size_t x_default_page_len;

/*! The Web server system mutex. */
extern xSemaphoreHandle   xWEBMutex;

/*! The CFG system mutex. */
extern xSemaphoreHandle   xCFGMutex;

//_____ D E C L A R A T I O N S ____________________________________________

/* The function that implements the single connection. */
portTASK_FUNCTION_PROTO( prvweb_ProcessSingleConnection, pvParameters );


#if configCTRLPANEL_TRACE == 1
/*!
 * \brief Print dev info on the trace port.
 */
void v_basicweb_trace( void )
{
   NAKED_TRACE_COM2( "BASICWEB:<%d>", sCurrentNbHTTPConn );
}
#endif


/*! \brief WEB server main task
 *         check for incoming connection and process it
 *
 */
portTASK_FUNCTION( vBasicWEBServer, pvParameters )
{
  struct netconn  *pxHTTPListener, *pxNewConnection;
  portCHAR        token[6];
  portSHORT       TaskIdx;
  portLONG        err_count;


  /* initialize current nb connection */
  sCurrentNbHTTPConn = 0;
  vSemaphoreCreateBinary( xMutexNbHTTPConn );

  x_default_page_len = strlen( pcDefaultPage );

  /* HTTP configuration. */
  // Get the xCFGMutex.
  if( pdFALSE == x_supervisor_SemaphoreTake( xCFGMutex, 0 ) )
  {
    // Failed to get the CFG mutex, use the default HTTP config.
    webHttpPort = webHTTP_PORT;
  }
  // Mutex has been taken
  else
  {
    // get the field value for port number
    if (config_file_get_value(HTTP_CONFIG_FILE, "port" , token) >= 0)
    {
      sscanf(token, "%u", &webHttpPort);
    }
    // if it does not exist, use the default value
    else
    {
      webHttpPort = webHTTP_PORT;
    }
    // Release the xCFGMutex.
    x_supervisor_SemaphoreGive( xCFGMutex );
  }

  // Create a new tcp connection handle
  pxHTTPListener = netconn_new( NETCONN_TCP );
  netconn_bind(pxHTTPListener, NULL, webHttpPort );
  netconn_listen( pxHTTPListener );
  // Loop forever
  for( ;; )
  {
#if ( (LWIP_VERSION) == ((1U << 24) | (3U << 16) | (2U << 8) | (LWIP_VERSION_RC)) )
    /* Wait for a first connection. */
    pxNewConnection = netconn_accept(pxHTTPListener);
#else
    while(netconn_accept(pxHTTPListener, &pxNewConnection) != ERR_OK)
    {
		vTaskDelay( webSHORT_DELAY );
	}
#endif
    if(pxNewConnection != NULL)
    {
      /* read the nb of connection, no need to use Mutex */
      while( webHTTP_NB_CONN == sCurrentNbHTTPConn )
      {
        vTaskDelay( webSHORT_DELAY );
      }

      // Take the xWEBMutex if there are no active connections.
      if( 0 == sCurrentNbHTTPConn )
      {
         if( pdFALSE == x_supervisor_SemaphoreTake( xWEBMutex, 0 ) )
         {
            prvweb_SendErrorPage( pxNewConnection, 503, "", "AVR32 UC3 Web server under maintenance." );
            // Close the connection.
            vTaskDelay( 50 );
            err_count = 4;
            while( netconn_close( pxNewConnection ) != ERR_OK )
            {
               if (--err_count == 0) break;
               vTaskDelay( webSHORT_DELAY );
            }
            err_count = 4;
            while( netconn_delete( pxNewConnection ) != ERR_OK )
            {
               if (--err_count == 0) break;
               vTaskDelay( webSHORT_DELAY );
            }
            continue;
         }
      }

      // Find an available spot in the tTaskHandle[] array.
      // We're sure to find one because sCurrentNbHTTPConn < webHTTP_NB_CONN.
      TaskIdx = 0;
      while( NULL != tTaskHandle[TaskIdx] ) TaskIdx++;

      while( xSemaphoreTake(xMutexNbHTTPConn , portMAX_DELAY ) != pdTRUE );
      sCurrentNbHTTPConn++;
      // Release the mutex.
      xSemaphoreGive( xMutexNbHTTPConn );

      // TRACE_COM2( "nb http conn:%d",sCurrentNbHTTPConn );

      if( xTaskCreate( prvweb_ProcessSingleConnection,
                                ( signed portCHAR * )"WCONN",
                                webHTTP_CONNECTION_STACK_SIZE, pxNewConnection,
                                webHTTP_CONNECTION_PRIORITY,
                                &tTaskHandle[TaskIdx] ) != pdPASS)
      {
          TRACE_COM2( "xTaskCreate() alloc error" );

          /* delete connection */
          err_count = 4;
          while( netconn_close( pxNewConnection ) != ERR_OK )
          {
            if (--err_count == 0) break;
            vTaskDelay( webSHORT_DELAY );
          }
          err_count = 4;
          while( netconn_delete( pxNewConnection ) != ERR_OK )
          {
            if (--err_count == 0) break;
            vTaskDelay( webSHORT_DELAY );
          }

          while( xSemaphoreTake(xMutexNbHTTPConn , portMAX_DELAY ) != pdTRUE );
          sCurrentNbHTTPConn--;
          // Release the xWEBMutex if there are no active connections.
          if( 0 == sCurrentNbHTTPConn )
          {
             x_supervisor_SemaphoreGive( xWEBMutex );
          }
          // Release the mutex.
          xSemaphoreGive( xMutexNbHTTPConn );
      }// end if task not created
    }// end if new connection

  }// end infinite loop
}

/*! \brief The set webserver config command: set the http port.
 *         Takes one parameter : port=ppppp
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
eExecStatus e_webserver_cmd_set_config( eModId xModId, signed short FsNavId,
                                     int ac, signed portCHAR *av[],
                                     signed portCHAR **ppcStringReply )
{
  if (config_file_set_value(HTTP_CONFIG_FILE, ac, av) != 0)
  {
    if(ppcStringReply != NULL)
    {
      *ppcStringReply = (signed portCHAR *)SHELL_ERRMSG_CONFIGERROR;
    }
    return( SHELL_EXECSTATUS_KO );
  }
  if(ppcStringReply != NULL)
  {
    /* allocate a buffer for answer */
    *ppcStringReply = (signed portCHAR *)pvPortMalloc( strlen( SHELL_MSG_REBOOT ) +1 ); // Alloc
    if( NULL != *ppcStringReply )
    {
       strcpy( (char *)*ppcStringReply, SHELL_MSG_REBOOT );
    }
  }
  return( SHELL_EXECSTATUS_OK );
}

/*! \brief The get webserver config command: get the config fields value of http port
 *         Takes no parameter
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
eExecStatus e_webserver_cmd_get_config( eModId xModId, signed short FsNavId,
                                     int ac, signed portCHAR *av[],
                                     signed portCHAR **ppcStringReply )
{
  if(ppcStringReply != NULL)
  {
    /* allocate a buffer for answer */
    *ppcStringReply = (signed portCHAR *)pvPortMalloc(15);
    if( NULL == *ppcStringReply )
    {
       *ppcStringReply = (signed portCHAR *)SHELL_ERRMSG_MEMALLOC;
       return( SHELL_EXECSTATUS_KO );
    }
    /* get HTTP port and add it to the buffer */
    sprintf((char *)*ppcStringReply,"port=%u\r\n", webHttpPort);
    /* no error, return */
    return( SHELL_EXECSTATUS_OK );
  }
  return( SHELL_EXECSTATUS_KO );
}

/*! \brief process an incoming connection
 *         parse the request and close/delete the connection
 *
 *  \note Input. The netconn to use to send and receive data.
 *
 */
portTASK_FUNCTION( prvweb_ProcessSingleConnection, pvParameters )
{
struct netconn * pxNewConnection;
portSHORT i;
portLONG err_count;
/*
  while( xSemaphoreTake(xMutexNbHTTPConn , portMAX_DELAY ) != pdTRUE );
  sCurrentNbHTTPConn++;
  // Release the mutex.
  xSemaphoreGive( xMutexNbHTTPConn );
*/

  pxNewConnection = ( struct netconn *)pvParameters;
  prvweb_ParseHTMLRequest( pxNewConnection );

  // Since we cannot know when the client has closed the connection(lwIP bug),
  // we set a 1000ms delay (the time for the file transfer to end), then we close
  // the connection ourselves.
  // NOTE FOR IMPROVMENTS: we could set this delay depending on the amount of
  // data that was sent to the client(the more the longer).
  vTaskDelay( 50 );
  err_count = 4;
  while( netconn_close( pxNewConnection ) != ERR_OK )
  {
    if (--err_count == 0) break;
    vTaskDelay( webSHORT_DELAY );
  }

  //vTaskDelay( 100 );
  err_count = 4;
  while( netconn_delete( pxNewConnection ) != ERR_OK )
  {
    if (--err_count == 0) break;
    vTaskDelay( webSHORT_DELAY );
  }
  // netconn_close( pxNewConnection );
  // netconn_delete( pxNewConnection );
  while( xSemaphoreTake(xMutexNbHTTPConn , portMAX_DELAY ) != pdTRUE );
  for (i = 0 ; i < webHTTP_NB_CONN ; i++)
  {
    if (tTaskHandle[i] == xTaskGetCurrentTaskHandle())
    {
      tTaskHandle[i] = NULL;
      break;
    }
  }
  sCurrentNbHTTPConn--;

  // Release the xWEBMutex if there are no other active connections.
  if( 0 == sCurrentNbHTTPConn )
  {
     x_supervisor_SemaphoreGive( xWEBMutex );
  }

  // Release the mutex.
  xSemaphoreGive( xMutexNbHTTPConn );

  vTaskDelete(NULL);
  /* nothing after delete task : this will not be reached */
}

/*! \brief parse the incoming request
 *         parse the HTML request and send file or execute CGI request
 *
 *  \param pxNetCon   Input. The netconn to use to send and receive data.
 *
 */
static void prvweb_ParseHTMLRequest( struct netconn *pxNetCon )
{
struct netbuf *pxRxBuffer;
portCHAR *pcRxString;
portCHAR *file;
portCHAR *path;
portCHAR *protocol;
unsigned portSHORT Length;
#if ( (LWIP_VERSION) != ((1U << 24) | (3U << 16) | (2U << 8) | (LWIP_VERSION_RC)) )
err_t    err_recv;
#endif

#if ( (LWIP_VERSION) == ((1U << 24) | (3U << 16) | (2U << 8) | (LWIP_VERSION_RC)) )
  /* We expect to immediately get data. */
  pxRxBuffer = netconn_recv( pxNetCon );
#else
  err_recv = netconn_recv( pxNetCon, &pxRxBuffer);
  if (err_recv != ERR_OK)
  {
    if (pxRxBuffer != NULL)
        goto delete_buffer;
    return;
  }
#endif

  if( pxRxBuffer != NULL )
  {
    /* Where is the data? */
    netbuf_data( pxRxBuffer, (void *) &pcRxString, &Length );
    /* Parse the first line of the request. */
    if ( pcRxString == (char*) 0 )
    {
      NAKED_TRACE_COM2( "ParseHTMLRequest(): empty request" );
      prvweb_SendErrorPage( pxNetCon, 400, "", "Wrong request : no action submitted." );
      goto delete_buffer;
    }
    /* get first occurrence of space char or \t or \n or \r */
    path = strpbrk( pcRxString, " \t\n\r" );
    if ( path == (char*) 0 )
    {
      NAKED_TRACE_COM2( "ParseHTMLRequest(): parse error" );
      prvweb_SendErrorPage( pxNetCon, 400, "", "Can't parse request." );
      goto delete_buffer;
    }
    *path++ = '\0';
    protocol = strpbrk( path, " \t\r\n" );
    *protocol++ = '\0';
    /* Is this a GET? */
    if( !strncmp( pcRxString, "GET", 3 ) )
    {
      if ( path[0] != '/' )
      {
         NAKED_TRACE_COM2( "ParseHTMLRequest(): wrong request" );
         prvweb_SendErrorPage( pxNetCon, 400, "", "Wrong request." );
         goto delete_buffer;
      }
      /* if there is a ?, this is a CGI request */
      protocol = strpbrk( path, "?" );
      if (protocol != NULL)
      {
        /* parse the CGI request */
        prvweb_ParseCGIRequest(pxNetCon, &path[1]);
        goto delete_buffer;
      }
      else
      {
        /* a file has been requested */
        file = (portCHAR *)pvPortMalloc((MAX_FILE_PATH_LENGTH * 2) + 4);
        if( NULL == file )
        {
           NAKED_TRACE_COM2( "ParseHTMLRequest(): mem alloc failed" );
           prvweb_SendErrorPage( pxNetCon, 400, "", "Memory allocation failed." );
           goto delete_buffer;
        }
        /* add web directory structure */
        sprintf(file,webSERVER_FOLDER);
        strcat(file,&(path[1]));
        /* check filename, if none, send index.htm */
        if ( file[sizeof(webSERVER_FOLDER)-1] == '\0' )
        {
          strcat(file,"index.htm");
        }
        /* go to the file transfer */
        prvweb_SendFile( pxNetCon, file );
        vPortFree(file);
      }
    }
    /* Is this a POST? */
    else if( !strncmp( pcRxString, "POST", 4 ) )
    {
      NAKED_TRACE_COM2( "ParseHTMLRequest(): POST request not supported" );
      prvweb_SendErrorPage( pxNetCon, 400, "", "POST request not supported for now." );
    }
    /* Is this a HEAD? */
    else if( !strncmp( pcRxString, "HEAD", 4 ) )
    {
      NAKED_TRACE_COM2( "ParseHTMLRequest(): HEAD request not supported" );
      prvweb_SendErrorPage( pxNetCon, 400, "", "HEAD request not supported for now." );
    }
    /* We don't handle anything else. */
    else
    {
      NAKED_TRACE_COM2( "ParseHTMLRequest(): unexpected request" );
      prvweb_SendErrorPage( pxNetCon, 400, "", "Can't parse request." );
    }
  }
  else
  {
    NAKED_TRACE_COM2( "ParseHTMLRequest(): nothing rxed" );
  }


delete_buffer: // netbuf_delete() handles the case when pxRxBuffer == NULL
  netbuf_delete( pxRxBuffer );
}

/*! \brief parse the CGI request and send it to the shell
 *
 *  \param pxNetCon   Input. The netconn to use to send and receive data.
 *  \param request    Input. The incoming CGI request.
 *
 */
static void prvweb_ParseCGIRequest( struct netconn *pxNetCon, char* request )
{
portCHAR cgi_present = false, loop;
signed portCHAR * pcStringCmd;
signed portCHAR * CurrentChar;
signed portCHAR * pcStringReply;
eExecStatus       xExeStatus;
portCHAR *pcResp_data;
portLONG header_size;

  /* when entering this function, path contains :
               command.cgi?arg1=val1&arg2=val2
  it should be command arg1=val1 arg2=val2 after parsing operation */

  /* allocate command space */
  pcStringCmd = pvPortMalloc( 100 );
  if (pcStringCmd != NULL)
  {

    loop = true;
    CurrentChar = pcStringCmd;
    while (loop)
    {
      switch (*request)
      {
        case '.':
        {
          if (cgi_present == false)
          {
            cgi_present = true;
            request += strlen(".cgi");
          }
          else
          {
            *CurrentChar++ = *request++;
          }
        break;
        }
        /* move ? and & to space */
        case '?':
        case '&':
          *CurrentChar++ = ' ';
          request++;
        break;
        case '%':
          /* move %20 to space */
          if ((*(request+1) == '2') && (*(request+2) == '0'))
          {
            *CurrentChar++ = ' ';
            request += 3;
          }
          /* keep %22 */
          else if ((*(request+1) == '2') && (*(request+2) == '2'))
          {
            *CurrentChar++ = '"';
            request += 3;
          }
          /* else keep the % symbol */
          else
          {
            *CurrentChar++ = *request++;
          }
        break;
        case '\r':
        case '\n':
        case '\0':
        case ' ':
          *CurrentChar = '\0';
          loop = false;
        break;
        default:
          *CurrentChar++ = *request++;
        break;
      }
    }

    /* send command to the shell, no error control for this module */
    xExeStatus = Shell_exec(pcStringCmd, SYS_MODID_HTTP, -1, &pcStringReply);
    /* free buffer */
    vPortFree(pcStringCmd);

    if (pcStringReply != NULL)
    {
        pcResp_data = pvPortMalloc( 130 + strlen( (char *)pcStringReply ) ); // 130(header size) + sizeof( reply )
        if( NULL == pcResp_data )
        {
           if( SHELL_EXECSTATUS_OK == xExeStatus )
           {
              vPortFree(pcStringReply);
           }
           NAKED_TRACE_COM2( "ParseCGIRequest(): reply mem alloc failed" );
           // Avoid returning an error page so that the remote web browser keeps
           // on trying.
           return;
        }
        /* add the header */
        header_size = prulweb_BuildHeaders( pcResp_data, 200, "OK", "", "", "text/html");
        // Add the data.
        strcpy( pcResp_data + header_size, (char *)pcStringReply );
        if( SHELL_EXECSTATUS_OK == xExeStatus )
        {
           vPortFree(pcStringReply);
        }

        /* send the response to network */
        netconn_write(pxNetCon, pcResp_data, (u16_t)strlen( pcResp_data ), NETCONN_COPY );
        vPortFree( pcResp_data );
    }
  }
  else
  {
    NAKED_TRACE_COM2( "ParseCGIRequest(): request mem alloc failed" );
    // Avoid returning an error page so that the remote web browser keeps on trying.
  }
}


/*!
 * \brief Copy part of the Default html page to another buffer.
 *
 * \param fd    Input. file descriptor. Ignored.
 * \param buf   Input/ouput. Destination memory ptr.
 * \param count Input. Amount of bytes to read.
 *
 * \return amount of data read (or -1 if error)
 */
static ssize_t prv_read_default_page( int fd, void *buf, size_t count )
{
  const char *pcOffset;


  // WARNING NOTE: this implementation assumes that the default page len is < 2*webMAX_DATA_TO_SEND.
  if( x_default_page_len >= ( 2*webMAX_DATA_TO_SEND ) )
    return( -1 );
  else
  {
    if( x_default_page_len >= webMAX_DATA_TO_SEND )
      pcOffset = pcDefaultPage + ( ( webMAX_DATA_TO_SEND == count )
                                    ? 0 : ( ( FS_SIZE_OF_SECTOR <= count )
                                            ? webMAX_DATA_TO_SEND : ( x_default_page_len - count ) ) );
    else
      pcOffset = pcDefaultPage + ( ( FS_SIZE_OF_SECTOR <= count )
                                    ? 0 : ( x_default_page_len - count ) );
    memcpy( buf, pcOffset, count );
    return( count );
  }
}


/*! \brief parse the file request and send the requested file to the host
 *
 *  \param pxNetCon   Input. The netconn to use to send and receive data.
 *  \param filename   Input. The incoming filename.
 *
 */
static void prvweb_SendFile( struct netconn *pxNetCon, char* filename )
{
portLONG            size, header_size;
portLONG            fd = -1;
portCHAR            *resp_data;
unsigned portSHORT  i;
err_t               error;
pf_read_src         pfread = read;  // Set the default read to a file read.

  // NAKED_TRACE_COM2( "SendFile(): %s", filename );

  /* allocate response */
  resp_data = (portCHAR *)pvPortMalloc(webMAX_DATA_TO_SEND);

  if (resp_data != NULL)
  {
    /* add the header */
    header_size = prulweb_BuildHeaders(resp_data, 200, "OK", "", "", "text/html");

    if ((fd = open((const char *)filename, O_RDONLY)) < 0)
    {
      // Set the read function to read from the pcDefaultPage
      pfread = prv_read_default_page;
      size = x_default_page_len;
      NAKED_TRACE_COM2( "SendFile(): file not found. Default page len:%d", x_default_page_len );
    }
    else    /* get the file size */
    {
      size = fsaccess_file_get_size(fd);
    }

    /* check if file should be sent in a single frame */
    if ( size <= (webMAX_DATA_TO_SEND - header_size))
    {
      /* get the data from filesystem */
      if( pfread(fd,&resp_data[header_size],size) != size)
      {
        NAKED_TRACE_COM2( "SendFile(): short file read access error" );
        prvweb_SendErrorPage( pxNetCon, 404, filename, "Error during read access." );
        goto quit_send_page;
      }
      /* send the response to network */
      error = netconn_write(pxNetCon, resp_data, (u16_t)(header_size + size), NETCONN_COPY );
      if (error != 0)
      {
        NAKED_TRACE_COM2( "SendFile(): short file nw write error" );
        goto quit_send_page;
      }
    }
    else
    {
      /* send the header */
      error = netconn_write(pxNetCon, resp_data, header_size, NETCONN_COPY );
      if (error != 0)
      {
        NAKED_TRACE_COM2( "SendFile(): long file hdr nw write error" );
        goto quit_send_page;
      }

      /* try to send the biggest frame contained in the file */
      for (i = webNB_SECTOR_TO_SEND ; i > 0 ; i--)
      {
        /* get sectors of maximum size */
        while(size > i * FS_SIZE_OF_SECTOR)
        {
          /* get the data from filesystem */
          if( pfread(fd,resp_data, i * FS_SIZE_OF_SECTOR) !=  i * FS_SIZE_OF_SECTOR)
          {
            NAKED_TRACE_COM2( "SendFile(): long file read access error" );
            prvweb_SendErrorPage( pxNetCon, 404, filename, "Error during read access." );
            goto quit_send_page;
          }

          /* send the response to network */
          error = netconn_write(pxNetCon, resp_data, (u16_t)(i * FS_SIZE_OF_SECTOR), NETCONN_COPY );
          if (error != 0)
          {
            NAKED_TRACE_COM2( "SendFile(): long file nw write error" );
            goto quit_send_page;
          }

          size -= (i * FS_SIZE_OF_SECTOR);
        }
      }
      /* finish with the few data remaining (less than 1 sector)*/
      if ( size > 0 )
      {
        /* get the data from filesystem */
        if( pfread(fd,resp_data,size) != size)
        {
          NAKED_TRACE_COM2( "SendFile(): long file end read access error" );
          prvweb_SendErrorPage( pxNetCon, 404, filename, "Error during read access." );
          goto quit_send_page;
        }
        /* send the response to network */
        error = netconn_write(pxNetCon, resp_data, (u16_t)size, NETCONN_COPY );
        if (error != 0)
        {
          NAKED_TRACE_COM2( "SendFile(): long file end nw write error" );
          goto quit_send_page;
        }
      }
    }
  }// end if response != NULL
  else
  {
    NAKED_TRACE_COM2( "SendFile(): mem alloc failed" );
    __asm__ __volatile__ ("nop");
  }
quit_send_page:
  // vPortFree() handles the case where resp_data==NULL
  vPortFree(resp_data);
  /* close the file */
  // close() handles the case where fd is an invalid file descriptor.
  close(fd);
}

/*! \brief send the static error page
 *
 *  \param pxNetCon     Input. The netconn to use to send and receive data.
 *  \param s            Input. The error code.
 *  \param extra_header Input. data to push in the error answer.
 *  \param text         Input. text to display in the error page.
 *
 */
static void prvweb_SendErrorPage(struct netconn *pxNetCon, int s, char* extra_header, const char* text )
{
portCHAR* title;
unsigned portLONG size;
portCHAR * err_data;

  err_data = (portCHAR *)pvPortMalloc(ERR_BUFFER_SIZE);
  if( NULL == err_data )
  {
    NAKED_TRACE_COM2( "prvweb_SendErrorPage(): mem alloc failed" );
    return;
  }
  /* Write the status line. */
  switch ( s )
  {
    case 400: title = "Bad Request"; break;
    case 401: title = "Unauthorized"; break;
    case 403: title = "Forbidden"; break;
    case 404: title = "Not Found"; break;
    case 408: title = "Request Timeout"; break;
    case 500: title = "Internal Error"; break;
    case 501: title = "Not Implemented"; break;
    case 503: title = "Service Unavailable"; break;
    default: title = "Something"; break;
  }
  /* add the header */
  size = prulweb_BuildHeaders(err_data, s, title, extra_header, "", "text/html");
  /* add the body */
  size += prulweb_BuildErrorBody(&err_data[size], s, title, text );
   /* add the tail */
  size += prulweb_BuildErrorTail(&err_data[size]);
  /* send the response to network */
  netconn_write(pxNetCon, err_data, size, NETCONN_COPY );
  /* free the buffer */
  vPortFree(err_data);
}

/*! \brief build the static error page body
 *
 *  \param response     Input/Output. the buffer to store data.
 *  \param s            Input. The error code.
 *  \param title        Input. title of the error page.
 *  \param text         Input. text to display in the error page.
 *
 */
static unsigned portLONG prulweb_BuildErrorBody(portCHAR * response, int s, char* title, const char* text )
{
unsigned portLONG retsize = 0;

  /* Send built-in error page. */
  retsize = sprintf(response, "<HTML><HEAD>");
  retsize += sprintf(&response[retsize], "<TITLE>%d %s</TITLE></HEAD>", s, title);
  retsize += sprintf(&response[retsize], "<BODY BGCOLOR=\"#FFFFFF\" TEXT=\"#000000\" LINK=\"#FF8C00\" VLINK=\"#FF8C00\">");
  retsize += sprintf(&response[retsize], "<H1>%d %s</H1>", s, title);
  retsize += sprintf(&response[retsize], "<H3>%s</H3>", text );
  return (retsize);
}

/*! \brief build the static error page tail
 *
 *  \param response     Input/Output. the buffer to store data.
 *
 */
static unsigned portLONG prulweb_BuildErrorTail( portCHAR * response )
{
unsigned portLONG retsize = 0;
portCHAR buff[15];
  vEthernetGetIPAddr(buff);
  retsize = sprintf(response, "<HR><ADDRESS><A HREF=\"http://%s\">", buff);
  retsize += sprintf(&response[retsize], "%s</A></ADDRESS></BODY></HTML>", webSERVER_SOFTWARE );
  return (retsize);
}

/*! \brief build the HTTP header
 *
 *  \param response     Input/Output. the buffer to store data.
 *  \param s            Input. The page code (200, 404, etc).
 *  \param title        Input. title of the page.
 *  \param extra_header Input. text to display in the error page.
 *  \param me           Input. MIME encoding, if used.
 *  \param mt           Input. MIME type if used.
 *
 */
static unsigned portLONG prulweb_BuildHeaders(portCHAR * response, int s, char * title, char* extra_header, char* me, char* mt )
{
unsigned portLONG retsize = 0;
portLONG s100;

  retsize = sprintf( response, "%s %d %s\r\n", webSERVER_PROTOCOL, s, title );
  retsize += sprintf( &response[retsize], "Server: %s\r\n", webSERVER_SOFTWARE );
  s100 = s / 100;
  // if ( s100 != 2 && s100 != 3 )
  if ( s100 != 3 )
  {
    retsize += sprintf( &response[retsize], "Cache-Control: no-cache,no-store\r\n" );
  }
  if ( extra_header != (char*) 0 && extra_header[0] != '\0' )
  {
    retsize += sprintf( &response[retsize], "%s\r\n", extra_header );
  }
  if ( me != (char*) 0 && me[0] != '\0' )
  {
    retsize += sprintf(&response[retsize], "Content-Encoding: %s\r\n", me );
  }
  if ( mt != (char*) 0 && mt[0] != '\0' )
  {
    retsize += sprintf( &response[retsize], "Content-Type: %s\r\n", mt );
  }
  retsize += sprintf( &response[retsize], "Connection: close\r\n\r\n" );

  return (retsize);
}

#endif
