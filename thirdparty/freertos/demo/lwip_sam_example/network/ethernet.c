/**
 * \file
 *
 * \brief Ethernet management for the FreeRTOS with lwIP example.
 *
 * Copyright (c) 2012-2018 Microchip Technology Inc. and its subsidiaries.
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
 */

#include <string.h>

/* Have to include gpio.h before FreeRTOS.h as long as FreeRTO redefines
 * the inline keyword to empty.
 */

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"

/* Demo program include files. */
#include "partest.h"
#include "conf_lwip_threads.h"

/* ethernet includes */
#include "ethernet.h"
#include "netif/ethernetif.h"
#include "conf_eth.h"

#if SAM3XA
#include "emac.h"
#else
#include "gmac.h"
#endif

#include "ethernet_phy.h"

#if (HTTP_USED == 1)
#include "BasicWEB.h"
#endif

#if (TFTP_USED == 1)
#include "BasicTFTP.h"
#endif

#if (SMTP_USED == 1)
#include "BasicSMTP.h"
#endif
#include "ethernet.h"
/* lwIP includes */
#include "lwip/sys.h"
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "lwip/memp.h"
#include <lwip/dhcp.h>
#include <lwip/dns.h>
#include <lwip/netif.h>
#include "lwip/stats.h"
#include "lwip/init.h"
#if ( (LWIP_VERSION) == ((1U << 24) | (3U << 16) | (2U << 8) | (LWIP_VERSION_RC)) )
#include "netif/loopif.h"
#else
#include "lwip/inet.h"
#endif
/* Global variable containing MAC Config (hw addr, IP, GW, ...) */
struct netif gs_net_if;

/* Timer for calling lwIP tmr functions without system */
typedef struct timers_info {
	uint32_t timer;
	uint32_t timer_interval;
	void (*timer_func)(void);
} timers_info_t;

/**
 *  \brief Set ethernet config.
 */
static void ethernet_configure_interface(void)
{
	struct ip_addr x_ip_addr, x_net_mask, x_gateway;
	extern err_t ethernetif_init(struct netif *netif);

#if defined(DHCP_USED)
	x_ip_addr.addr = 0;
	x_net_mask.addr = 0;
#else
	/* Default ip addr */
	IP4_ADDR(&x_ip_addr, ETHERNET_CONF_IPADDR0, ETHERNET_CONF_IPADDR1,
			ETHERNET_CONF_IPADDR2, ETHERNET_CONF_IPADDR3);

	/* Default subnet mask */
	IP4_ADDR(&x_net_mask, ETHERNET_CONF_NET_MASK0, ETHERNET_CONF_NET_MASK1,
			ETHERNET_CONF_NET_MASK2, ETHERNET_CONF_NET_MASK3);

	/* Default gateway addr */
	IP4_ADDR(&x_gateway, ETHERNET_CONF_GATEWAY_ADDR0,
			ETHERNET_CONF_GATEWAY_ADDR1,
			ETHERNET_CONF_GATEWAY_ADDR2,
			ETHERNET_CONF_GATEWAY_ADDR3);
#endif

	/* Add data to netif */
	netif_add(&gs_net_if, &x_ip_addr, &x_net_mask, &x_gateway, NULL,
			ethernetif_init, ethernet_input);

	/* Make it the default interface */
	netif_set_default(&gs_net_if);

	/* Setup callback function for netif status change */
	netif_set_status_callback(&gs_net_if, status_callback);

	/* Bring it up */
#if defined(DHCP_USED)
	printf("LwIP: DHCP Started");
	dhcp_start(&gs_net_if);
#else
	printf("LwIP: Static IP Address Assigned");
	netif_set_up(&gs_net_if);
#endif
}

/**
 *  \brief Callback executed when the TCP/IP init is done.
 */
static void tcpip_init_done(void *arg)
{
	sys_sem_t *sem;
	sem = (sys_sem_t *)arg;

	/* Set hw and IP parameters, initialize MAC too */
	ethernet_configure_interface();
#if ( (LWIP_VERSION) == ((1U << 24) | (3U << 16) | (2U << 8) | (LWIP_VERSION_RC)) )
  sys_sem_signal (*sem);    /* Signal the waiting thread that the TCP/IP init is done. */
#else
  sys_sem_signal (sem);    /* Signal the waiting thread that the TCP/IP init is done. */
#endif
}

/**
 *  \brief start lwIP layer.
 */
static void prvlwIPInit( void )
{
  sys_sem_t sem;

#if ( (LWIP_VERSION) == ((1U << 24) | (3U << 16) | (2U << 8) | (LWIP_VERSION_RC)) )
	sem = sys_sem_new(0);   /* Create a new semaphore. */
	tcpip_init(tcpip_init_done, &sem);
	sys_sem_wait(sem);      /* Block until the lwIP stack is initialized. */
	sys_sem_free(sem);      /* Free the semaphore. */
#else
	err_t err_sem;
	err_sem = sys_sem_new(&sem, 0); /* Create a new semaphore. */
	tcpip_init(tcpip_init_done, &sem);
	sys_sem_wait(&sem);     /* Block until the lwIP stack is initialized. */
	sys_sem_free(&sem);     /* Free the semaphore. */
#endif
}

void vStartEthernetTaskLauncher( unsigned portBASE_TYPE uxPriority )
{
	/* Spawn the Sentinel task. */
	xTaskCreate( vStartEthernetTask, (const signed portCHAR *)"ETHLAUNCH",
			configMINIMAL_STACK_SIZE, NULL, uxPriority,
			(xTaskHandle *)NULL );
}

/*! \brief create ethernet task, for ethernet management.
 *
 *  \param uxPriority   Input. priority for the task, it should be low
 *
 */
portTASK_FUNCTION(vStartEthernetTask, pvParameters)
{
	/* Setup lwIP. */
	prvlwIPInit();

#if (HTTP_USED == 1)
	/* Create the WEB server task.  This uses the lwIP RTOS abstraction layer. */
	sys_thread_new("WEB", vBasicWEBServer, (void *)NULL,
			lwipBASIC_WEB_SERVER_STACK_SIZE,
			lwipBASIC_WEB_SERVER_PRIORITY);
#endif

#if (TFTP_USED == 1)
  /* Create the TFTP server task.  This uses the lwIP RTOS abstraction layer. */
	sys_thread_new("TFTP", vBasicTFTPServer, (void *)NULL,
			lwipBASIC_TFTP_SERVER_STACK_SIZE,
			lwipBASIC_TFTP_SERVER_PRIORITY);
#endif

#if (SMTP_USED == 1)
  /* Create the SMTP Client task.  This uses the lwIP RTOS abstraction layer. */
	sys_thread_new("SMTP", vBasicSMTPClient, (void *)NULL,
			lwipBASIC_SMTP_CLIENT_STACK_SIZE,
			lwipBASIC_SMTP_CLIENT_PRIORITY);
#endif
	/* Kill this task. */
	vTaskDelete(NULL);
}

/**
 *  \brief Status callback used to print address given by DHCP.
 *
 * \param netif Instance to network interface.
 */
void status_callback(struct netif *netif)
{
	int8_t c_mess[25];
	if (netif_is_up(netif)) {
		printf("Network up");
		strcpy((char *)c_mess, "IP=");
		strcat((char *)c_mess,
				inet_ntoa(*(struct in_addr *)&(netif->ip_addr)));
		printf((char const *)c_mess);
	} else {
		printf("Network down");
	}
}
