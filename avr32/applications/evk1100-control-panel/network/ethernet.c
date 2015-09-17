/*****************************************************************************
 *
 * \file
 *
 * \brief lwIP on Ethernet entry point.
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



#include <stdlib.h>
#include <string.h>

#include "conf_eth.h"
#include "conf_explorer.h"

#include "gpio.h" // Have to include gpio.h before FreeRTOS.h as long as FreeRTOS
                  // redefines the inline keyword to empty.

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"

/* Demo program include files. */
#include "serial.h"
#include "fsaccess.h"
#include "config_file.h"
#include "conf_lwip_threads.h"

/* Ethernet includes */
#include "ethernet.h"
#include "macb.h"

#if (HTTP_USED == 1)
  #include "BasicWEB.h"
#endif

#if (TFTP_USED == 1)
  #include "BasicTFTP.h"
#endif

#if (SMTP_USED == 1)
  #include "BasicSMTP.h"
#endif

/* lwIP includes */
#include "lwip/sys.h"
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "lwip/memp.h"
#include "lwip/stats.h"
#include "lwip/init.h"
#if ( (LWIP_VERSION) == ((1U << 24) | (3U << 16) | (2U << 8) | (LWIP_VERSION_RC)) )
#include "netif/loopif.h"
#endif

/* shell includes */
#include "supervisor.h"
#include "shell.h"

#include "mmi.h"

//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

/* global variable containing MAC Config (hw addr, IP, GW, ...) */
struct netif MACB_if;

/*! The CFG system mutex. */
extern xSemaphoreHandle   xCFGMutex;

//_____ D E C L A R A T I O N S ____________________________________________

/* Initialisation required by lwIP. */
static void prvlwIPInit( void );

/* Initialisation of Ethernet interfaces by reading config file */
static void prvEthernetConfigureInterface(void * param);

static void prv_v_set_default_macaddr( unsigned portCHAR aMacAddress[] ); // FORWARD

static void prv_v_set_default_netconfig( unsigned portCHAR aMacAddress[],
                                         struct ip_addr *pxIpAddr,
                                         struct ip_addr *pxNetMask,
                                         struct ip_addr *pxGateway ); // FORWARD


/*-----------------------------------------------------------*/

void vStartEthernetTaskLauncher( unsigned portBASE_TYPE uxPriority )
{
  /* Spawn the Sentinel task. */
  xTaskCreate( vStartEthernetTask, ( const signed portCHAR * )"ETHLAUNCH",
		       configMINIMAL_STACK_SIZE + 192, NULL, uxPriority, ( xTaskHandle * )NULL );
}

/*
 * Configure lwIP and MACB, start lwIP layer, start servers tasks through lwIP services.
 */
portTASK_FUNCTION( vStartEthernetTask, pvParameters )
{
static const gpio_map_t MACB_GPIO_MAP =
{
  {EXTPHY_MACB_MDC_PIN,     EXTPHY_MACB_MDC_FUNCTION   },
  {EXTPHY_MACB_MDIO_PIN,    EXTPHY_MACB_MDIO_FUNCTION  },
  {EXTPHY_MACB_RXD_0_PIN,   EXTPHY_MACB_RXD_0_FUNCTION },
  {EXTPHY_MACB_TXD_0_PIN,   EXTPHY_MACB_TXD_0_FUNCTION },
  {EXTPHY_MACB_RXD_1_PIN,   EXTPHY_MACB_RXD_1_FUNCTION },
  {EXTPHY_MACB_TXD_1_PIN,   EXTPHY_MACB_TXD_1_FUNCTION },
  {EXTPHY_MACB_TX_EN_PIN,   EXTPHY_MACB_TX_EN_FUNCTION },
  {EXTPHY_MACB_RX_ER_PIN,   EXTPHY_MACB_RX_ER_FUNCTION },
  {EXTPHY_MACB_RX_DV_PIN,   EXTPHY_MACB_RX_DV_FUNCTION },
  {EXTPHY_MACB_TX_CLK_PIN,  EXTPHY_MACB_TX_CLK_FUNCTION}
};

  // Assign GPIO to MACB
  gpio_enable_module(MACB_GPIO_MAP, sizeof(MACB_GPIO_MAP) / sizeof(MACB_GPIO_MAP[0]));


  /* Setup lwIP. */
  prvlwIPInit();

#if (HTTP_USED == 1)
  /* Create the WEB server task.  This uses the lwIP RTOS abstraction layer.*/
  sys_thread_new( "WEB", vBasicWEBServer, ( void * ) NULL,
                   CTRLPANEL_WEB_SERVER_STACK_SIZE,
                   CTRLPANEL_WEB_SERVER_PRIORITY );
#endif

#if (TFTP_USED == 1)
  /* Create the TFTP server task.  This uses the lwIP RTOS abstraction layer.*/
  sys_thread_new( "TFTP", vBasicTFTPServer, ( void * ) NULL,
                   CTRLPANEL_TFTP_SERVER_STACK_SIZE,
                   CTRLPANEL_TFTP_SERVER_PRIORITY );
#endif

#if (SMTP_USED == 1)
  /* Create the SMTP Client task.  This uses the lwIP RTOS abstraction layer.*/
  sys_thread_new( "SMTP", vBasicSMTPClient, ( void * ) NULL,
                   CTRLPANEL_SMTP_CLIENT_STACK_SIZE,
                   CTRLPANEL_SMTP_CLIENT_PRIORITY );
#endif
  // Kill this task.
  vTaskDelete(NULL);
}


//! Callback executed when the TCP/IP init is done.
static void tcpip_init_done(void *arg)
{
  sys_sem_t *sem;
  sem = (sys_sem_t *)arg;
  prvEthernetConfigureInterface(NULL);
#if ( (LWIP_VERSION) == ((1U << 24) | (3U << 16) | (2U << 8) | (LWIP_VERSION_RC)) )
  sys_sem_signal(*sem); // Signal the waiting thread that the TCP/IP init is done.
#else
  sys_sem_signal(sem); // Signal the waiting thread that the TCP/IP init is done.
#endif
}

/*!
 * \brief Stop the Ethernet module resources.
 */
void v_ethernet_stopResources( void )
{
  // Disable the MACB.
  vDisableMACBOperations( &AVR32_MACB );
}

/*! \brief get the current MAC address :
 *         formatted as follow :
 *              XX:XX:XX:XX:XX:XX
 *
 *  \param pcConfig   Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        The caller has to allocate this buffer
 *
 */
void vEthernetGetMACAddr(portCHAR * pcConfig)
{
  if (pcConfig != NULL)
  {
    sprintf(pcConfig, "%02x:%02x:%02x:%02x:%02x:%02x", MACB_if.hwaddr[0], MACB_if.hwaddr[1],
                                                       MACB_if.hwaddr[2], MACB_if.hwaddr[3],
                                                       MACB_if.hwaddr[4], MACB_if.hwaddr[5]);
  }
}

/*! \brief get the current IP address :
 *         formatted as follow :
 *              XXX.XXX.XXX.XXX
 *
 *  \param pcConfig   Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        The caller has to allocate this buffer
 *
 */
void vEthernetGetIPAddr(portCHAR * pcConfig)
{
  if (pcConfig != NULL)
  {
    sprintf(pcConfig,"%d.%d.%d.%d", (u16_t)(ntohl((MACB_if.ip_addr.addr) >> 24) & 0xff),
                                (u16_t)(ntohl((MACB_if.ip_addr.addr) >> 16) & 0xff),
                                (u16_t)(ntohl((MACB_if.ip_addr.addr) >> 8) & 0xff),
                                (u16_t) ntohl((MACB_if.ip_addr.addr) & 0xff ));
  }
}

/*! \brief get the current Subnet mask :
 *         formatted as follow :
 *              XXX.XXX.XXX.XXX
 *
 *  \param pcConfig   Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        The caller has to allocate this buffer
 *
 */
void vEthernetGetSubnetMask(portCHAR * pcConfig)
{
  if (pcConfig != NULL)
  {
    sprintf(pcConfig,"%d.%d.%d.%d", (u16_t)(ntohl((MACB_if.netmask.addr) >> 24) & 0xff),
                                (u16_t)(ntohl((MACB_if.netmask.addr) >> 16) & 0xff),
                                (u16_t)(ntohl((MACB_if.netmask.addr) >> 8) & 0xff),
                                (u16_t) ntohl((MACB_if.netmask.addr) & 0xff ));
  }
}

/*! \brief get the current GW address :
 *         formatted as follow :
 *              XXX.XXX.XXX.XXX
 *
 *  \param pcConfig   Input/Output. The response string.
 *                        If Input is NULL, no response string will be output.
 *                        The caller has to allocate this buffer
 *
 */
void vEthernetGetGWAddr(portCHAR * pcConfig)
{
  if (pcConfig != NULL)
  {
    sprintf(pcConfig,"%d.%d.%d.%d", (u16_t)(ntohl((MACB_if.gw.addr) >> 24) & 0xff),
                                (u16_t)(ntohl((MACB_if.gw.addr) >> 16) & 0xff),
                                (u16_t)(ntohl((MACB_if.gw.addr) >> 8) & 0xff),
                                (u16_t) ntohl((MACB_if.gw.addr) & 0xff ));
  }
}


/*! \brief The get sensor config command: get the config fields value of a sensor.
 *         Takes one parameter, that is the sensor's name.
 *         Format: get_sensor_config sensorname
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
eExecStatus e_ethernet_cmd_set_config( eModId xModId, signed short FsNavId,
                                     int ac, signed portCHAR *av[],
                                     signed portCHAR **ppcStringReply )
{

    if (config_file_set_value(ETHERNET_CONFIG_FILE, ac, av) != 0)
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


/*! \brief The set sensor config command: set the value of a config field of a sensor.
 *         Takes three parameters.
 *         The first parameter is the sensor's name, the second parameter is
 *         the config field name, the third parameter is the value.
 *         Format: set_sensor_config sensorname field=value
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
eExecStatus e_ethernet_cmd_get_config( eModId xModId, signed short FsNavId,
                                     int ac, signed portCHAR *av[],
                                     signed portCHAR **ppcStringReply )
{
portCHAR buf[18];

  if(ppcStringReply != NULL)
  {
    /* allocate a buffer for answer */
    *ppcStringReply = (signed portCHAR *)pvPortMalloc(130);
    if( NULL == *ppcStringReply )
    {
       *ppcStringReply = (signed portCHAR *)SHELL_ERRMSG_MEMALLOC;
       return( SHELL_EXECSTATUS_KO );
    }
    /* add some static data */
    strcpy((char *)*ppcStringReply, "macaddr=");
    /* get MAC addr and add it to the buffer */
    vEthernetGetMACAddr(buf);
    strcat((char *)*ppcStringReply,buf);
    /* add some static data */
    strcat((char *)*ppcStringReply,"\r\nipaddr=");
    /* get IP addr and add it to the buffer */
    vEthernetGetIPAddr(buf);
    strcat((char *)*ppcStringReply,buf);
    /* add some static data */
    strcat((char *)*ppcStringReply,"\r\nsubmask=");
    /* get Subnet Mask and add it to the buffer */
    vEthernetGetSubnetMask(buf);
    strcat((char *)*ppcStringReply,buf);
    /* add some static data */
    strcat((char *)*ppcStringReply,"\r\ngwaddr=");
    /* get GW addr and add it to the buffer */
    vEthernetGetGWAddr(buf);
    strcat((char *)*ppcStringReply,buf);
    /* add some static data */
    strcat((char *)*ppcStringReply,"\r\n");
    /* no error, return */
    return( SHELL_EXECSTATUS_OK );
  }
  return( SHELL_EXECSTATUS_KO );
}


/*!
 *  \brief start lwIP layer.
 */
static void prvlwIPInit( void )
{
  sys_sem_t sem;
#if ( (LWIP_VERSION) != ((1U << 24) | (3U << 16) | (2U << 8) | (LWIP_VERSION_RC)) )
  err_t     err_sem;
#endif

#if ( (LWIP_VERSION) == ((1U << 24) | (3U << 16) | (2U << 8) | (LWIP_VERSION_RC)) )
  sem = sys_sem_new(0); // Create a new semaphore.
  tcpip_init(tcpip_init_done, &sem);
  sys_sem_wait(sem);    // Block until the lwIP stack is initialized.
  sys_sem_free(sem);    // Free the semaphore.
#else
  err_sem = sys_sem_new(&sem, 0);
  if (err_sem == ERR_OK)
  {
    tcpip_init(tcpip_init_done, &sem);
    sys_sem_wait(&sem);    // Block until the lwIP stack is initialized.
    sys_sem_free(&sem);    // Free the semaphore.
  }
#endif
}

/*!
 *  \brief set Ethernet config, by parsing ETHERNET_CONFIG_FILE file.
 */
static void prvEthernetConfigureInterface(void * param)
{
struct ip_addr xIpAddr, xNetMask, xGateway;
extern err_t ethernetif_init( struct netif *netif );
portCHAR token[18];
portCHAR IPAddress[16];
portCHAR * current;
unsigned portCHAR MacAddress[6];
portSHORT NbDigits = 0;
unsigned int AddrByte;


  // We're going to access the file system to do configuration
  // => get the CFG mutex.
  if( pdFALSE == x_supervisor_SemaphoreTake( xCFGMutex, 0 ) )
  { // Failed to get the CFG mutex.
    /* Use default parameters. */
    prv_v_set_default_netconfig( MacAddress, &xIpAddr, &xNetMask, &xGateway );
  }
  /* open the config file */
  else
  {
    if (config_file_get_value(ETHERNET_CONFIG_FILE, "macaddr" , token) >= 0)
    {
      current = token;
      while (NbDigits < 6)
      {
        /* get two next bytes and convert as integer */
        sscanf(current, "%02x", &AddrByte);
        MacAddress[NbDigits++] = (portCHAR)AddrByte;
        current = strpbrk(current, ":");

        if ((current == NULL) && (NbDigits == 6))
        {
          prv_v_set_default_macaddr( MacAddress );
          break;
        }
        current++;
      }
    }
    else
    {
      /* can't find field in config file, use default parameters */
      prv_v_set_default_macaddr( MacAddress );
    }
    /* pass the MACB address to AVR32_EMAC module */
    vMACBSetMACAddress(MacAddress);
    /* set MAC hardware address length to be used by lwIP */
    // MACB_if.hwaddr_len = 6;
    /* set MAC hardware address to be used by lwIP */
    // memcpy((char *)MACB_if.hwaddr, MacAddress, MACB_if.hwaddr_len);

    if (config_file_get_value(ETHERNET_CONFIG_FILE, "ipaddr" , token) >= 0)
    {
#if ( (LWIP_VERSION) == ((1U << 24) | (3U << 16) | (2U << 8) | (LWIP_VERSION_RC)) )
      /* get IP address */
      xIpAddr.addr = inet_addr(token);
#else
      xIpAddr.addr = ipaddr_addr(token);
#endif
    }
    else
    {
      /* can't find field in config file, use default parameters */
      IP4_ADDR(&xIpAddr,ETHERNET_CONF_IPADDR0,ETHERNET_CONF_IPADDR1,ETHERNET_CONF_IPADDR2,ETHERNET_CONF_IPADDR3);
    }

    if (config_file_get_value(ETHERNET_CONFIG_FILE, "submask", token) >= 0)
    {
#if ( (LWIP_VERSION) == ((1U << 24) | (3U << 16) | (2U << 8) | (LWIP_VERSION_RC)) )
      /* get subnet mask */
      xNetMask.addr = inet_addr(token);
#else
      xNetMask.addr = ipaddr_addr(token);
#endif
    }
    else
    {
      /* can't find field in config file, use default parameters */
      IP4_ADDR(&xNetMask,ETHERNET_CONF_NET_MASK0,ETHERNET_CONF_NET_MASK1,ETHERNET_CONF_NET_MASK2,ETHERNET_CONF_NET_MASK3);
    }

    if (config_file_get_value(ETHERNET_CONFIG_FILE, "gwaddr" , token) >= 0)
    {
#if ( (LWIP_VERSION) == ((1U << 24) | (3U << 16) | (2U << 8) | (LWIP_VERSION_RC)) )
      /* get GW address */
      xGateway.addr = inet_addr(token);
#else
      xGateway.addr = ipaddr_addr(token);
#endif
    }
    else
    {
      /* can't find field in config file, use default parameters */
      IP4_ADDR(&xGateway,ETHERNET_CONF_GATEWAY_ADDR0,ETHERNET_CONF_GATEWAY_ADDR1,ETHERNET_CONF_GATEWAY_ADDR2,ETHERNET_CONF_GATEWAY_ADDR3);
    }
    // Release the CFG mutex.
    x_supervisor_SemaphoreGive( xCFGMutex );
  }

  /* add data to netif */
  netif_add(&MACB_if, &xIpAddr, &xNetMask, &xGateway, NULL, ethernetif_init, tcpip_input);
  /* make it the default interface */
  netif_set_default(&MACB_if);
  /* bring it up */
  netif_set_up(&MACB_if);
  /* get new IP Address */
  vEthernetGetIPAddr(IPAddress);
  /* set IP Address to Display */
#ifdef MMILCD_ENABLE
  vMMI_DisplayIP(IPAddress);
#endif
}


/*!
 *  \brief The development only ip statck stats: display the TCP/IP stack stats on COM2.
 *         No parameters.
 *         Format: ipstat
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
eExecStatus e_ip_stat( eModId xModId, signed short FsNavId,
                       int ac, signed portCHAR *av[],
                       signed portCHAR **ppcStringReply )
{
   if( NULL != ppcStringReply )
      *ppcStringReply = NULL;
   stats_display();
   return( SHELL_EXECSTATUS_OK );
}


/*############# PRIVATE  ######################*/

/*!
 *  \brief Set the default mac addr.
 *
 *  \param aMacAddress[] Mac address array of len 6.
 */
static void prv_v_set_default_macaddr( unsigned portCHAR aMacAddress[] )
{
   aMacAddress[0] = ETHERNET_CONF_ETHADDR0;
   aMacAddress[1] = ETHERNET_CONF_ETHADDR1;
   aMacAddress[2] = ETHERNET_CONF_ETHADDR2;
   aMacAddress[3] = ETHERNET_CONF_ETHADDR3;
   aMacAddress[4] = ETHERNET_CONF_ETHADDR4;
   aMacAddress[5] = ETHERNET_CONF_ETHADDR5;
}

/*!
 *  \brief Set the default network configuration.
 *
 *  \param aMacAddress[]  Mac address array of len 6
 *  \param pxIpAddr       pointer on Ip address struct
 *  \param pxNetMask      pointer on network mask address struct
 *  \param pxGateway      pointer on gateway Ip address struct
 */
static void prv_v_set_default_netconfig( unsigned portCHAR aMacAddress[],
                                         struct ip_addr *pxIpAddr,
                                         struct ip_addr *pxNetMask,
                                         struct ip_addr *pxGateway )
{
   // Default MAC addr.
   prv_v_set_default_macaddr( aMacAddress );
   // pass the MACB address to AVR32_EMAC module
   vMACBSetMACAddress( aMacAddress );
   // set MAC hardware address length to be used by lwIP
   // MACB_if.hwaddr_len = 6;
   // set MAC hardware address to be used by lwIP
   // memcpy(MACB_if.hwaddr, aMacAddress, MACB_if.hwaddr_len);

   // Default ip addr.
   IP4_ADDR(pxIpAddr,ETHERNET_CONF_IPADDR0,ETHERNET_CONF_IPADDR1,ETHERNET_CONF_IPADDR2,ETHERNET_CONF_IPADDR3);

   // Default Subnet mask.
   IP4_ADDR(pxNetMask,ETHERNET_CONF_NET_MASK0,ETHERNET_CONF_NET_MASK1,ETHERNET_CONF_NET_MASK2,ETHERNET_CONF_NET_MASK3);

   // Default Gw addr.
   IP4_ADDR(pxGateway,ETHERNET_CONF_GATEWAY_ADDR0,ETHERNET_CONF_GATEWAY_ADDR1,ETHERNET_CONF_GATEWAY_ADDR2,ETHERNET_CONF_GATEWAY_ADDR3);
}
