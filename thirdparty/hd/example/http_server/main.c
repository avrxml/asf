/*! \page License
 * Copyright (C) 2009, H&D Wireless AB All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of H&D Wireless AB may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY H&D WIRELESS AB ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*!
 * \file *********************************************************************
 *
 * \brief HTTP Server WiFi Demo Application
 *
 * This file provides a an example application for the H&D Wireless AB
 * WiFi driver.
 *
 *   \li SPB104 + EVK1104 (SDIO)
 *   \li SPB104 + EVK1105 (SPI)
 *   \li SPB104 + EVK1104 (SPI + irq)
 *   \li SPB105 + EVK1105 (SPI)
 *   \li SPB104 + EVK1100
 *   \li SPB104 + EVK1101
 *
 *
 *****************************************************************************/

/*! \mainpage http_server_demo HTTP Server WiFi Demo Application
 * \section http_server_intro Introduction
 * This is the documentation for the WiFi HTTP Server demo application.
 * This application demonstrates the use of the (o)WL API &copy;
 *
 * Information on the SPB104 and SPB105 devices can be found here:
 * http://www.hd-wireless.se
 *
 * \section files Main Files
 *
 * HTTP server :
 *
 * - http_server/main.c : Application entry point
 * - http_server/fsdata.c : Filesystem data image
 * - http_server/http_server_gui.c : HTTP Server-specific GUI handling
 *
 * Support code :
 *
 * - net/fs.c : Filesystem stub for HTTP server file access
 * - net/httpd.c : HTTP Server
 * - net/lwip_setup.c : IP stack configuration and setup
 * - net/ping.c : ICMP echo implementation
 * - net/ttcp.c : TTCP traffic source/sink implementation for
 *                throughput measurements
 *
 * - ports/avr32/avr32_sdio.c : SDIO HAL
 * - ports/avr32/avr32_spi.c : SPI HAL
 * - ports/avr32/board_init : EVK board setup
 * - ports/avr32/fw_download_extflash.c : WiFi firmware download
 *                                        implementation for boards
 *                                        with the firmware image
 *                                        in external flash.
 * - ports/avr32/gui.c : Generic GUI implementation
 * - ports/avr32/gui_getstring.c : GUI input for encryption keys
 * - ports/avr32/nvram.c : NOR flash access functions (used for
 *                         WiFi firmware access when the firmware
 *                         is stored in external flash)
 * - ports/avr32/timer.c : Timer implementation
 * - ports/avr32/conf/conf_*.h : Board component configurations
 * - ports/avr32/conf/lwipopts.h : lwIP configuration tailored for
 *                                 this application
 *
 * - utils/console.c : Serial-USB console
 * - utils/util.c : Utility functions
 * - utils/printf-stdarg.c : String formatting functions
 * - utils/getopt.c : Replacement getopt. Getopt from newlib
 *                    uses printf/scanf with floating point support.
 *                    The HTTP server code example does not use the
 *                    floating point support in printf/scanf and
 *                    removing that support saves up to 20kB of code
 *                    memory. Getopt is only used by ttcp.c and ping.c
 *
 * - wl/cmd_wl.c : Console command handlers
 * - wl/fw_download.c : WiFi firmware download implementation for
 *                   boards with the firmware image embedded in the
 *                   application.
 * - wl/wl_cm.c : Simple WiFi Connection Manager implementation.
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC for AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Information
 *   \li SPB104 + EVK1104 (SDIO)
 *   \li SPB104 + EVK1105 (SPI)
 *   \li SPB104 + EVK1104 (SPI + irq)
 *   \li SPB105 + EVK1105 (SPI)
 *   \li SPB104 + EVK1100
 *   \li SPB104 + EVK1101
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - EVK1100 evaluation kits with SPB104 card.
 * - EVK1101 evaluation kits with SPB104 card.
 * - EVK1104 evaluation kits with SPB104 card.
 * - EVK1105 evaluation kits with SPB104 card.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.hd-wireless.se/">H&D Wireless</A>.\n
 */

#include <compiler.h>
#include "board.h"
#include "gpio.h"
#include "adc.h"

#include <stdint.h>
#include <wl_api.h>
#include <wl/wl_cm.h>

#include "lwip/init.h"
#include "lwip/dhcp.h"
#include "lwip/tcp.h"
#include "netif/etharp.h"
#include "netif/wlif.h"

#include <ports/avr32/board_init.h>
#include <util/trace.h>
#include <wl/fw_download.h>
#include <ports/avr32/timer.h>
#include <util/util.h>
#include <wl/cmd_wl.h>
#include <net/httpd.h>
#include <net/ping.h>
#include <net/ttcp.h>
#include <ports/avr32/gui.h>
#include <net/lwip_setup.h>
#include "http_server_gui.h"
#include "fsdata_app.h"

struct http_server {
        struct net_cfg net_cfg;
        uint8_t wl_init_complete;
};


/**
 *
 */
static void
wl_cm_conn_cb(struct wl_network_t* net, void* ctx)
{
        struct http_server* hs = ctx;

        gui_link_up_cb();
        printk("link up, connected to \"%s\"\n", ssid2str(&net->ssid));
        if ( hs->net_cfg.dhcp_enabled ) {
                printk("requesting dhcp ... ");

                dhcp_start(hs->net_cfg.netif);
                hs->net_cfg.dhcp_running = 1;
        }
        else {
                netif_set_up(hs->net_cfg.netif);
        }
}


/**
 *
 */
static void
wl_cm_disconn_cb(void* ctx)
{
        struct http_server* hs = ctx;

        gui_link_down_cb();

        if (netif_is_up(hs->net_cfg.netif)) {
                if (hs->net_cfg.dhcp_running) {
                        printk("link down, release dhcp\n");
                        dhcp_release(hs->net_cfg.netif);
                        dhcp_stop(hs->net_cfg.netif);
                        hs->net_cfg.dhcp_running = 0;
                } else {
                        printk("link down\n");
                        netif_set_down(hs->net_cfg.netif);
                }
        }
}


/**
 *
 */
static void
ip_status_cb(struct netif* netif)
{
        if (netif_is_up(netif)) {
                gui_status_up_cb();
                printk("bound to %s\n", ip2str(netif->ip_addr));
                printk("starting httpd ... ");
                if (httpd_start() == ERR_OK)
                        printk("ok\n");
                else
                        printk("fail\n");

        }
        else {
                gui_status_down_cb();
                printk("stopping httpd\n");
                httpd_stop();
        }
}


/**
 *
 */
void
adc_init(void)
{
        static const gpio_map_t ADC_GPIO_MAP = {
                { AVR32_ADC_AD_0_PIN, AVR32_ADC_AD_0_FUNCTION },
        };
        volatile avr32_adc_t *adc = &AVR32_ADC;

        gpio_enable_module(ADC_GPIO_MAP,
                           sizeof(ADC_GPIO_MAP) / sizeof(ADC_GPIO_MAP[0]));
        AVR32_ADC.mr |= 0x1 << AVR32_ADC_MR_PRESCAL_OFFSET;
        adc_configure(adc);
        adc_enable(adc, 0);
}


/**
 *
 */
void
led_init(void)
{
        LED_Off(LED0);
        LED_Off(LED1);
        LED_Off(LED2);
        LED_Off(LED3);
}


/**
 *
 */
void
poll(struct http_server* hs)
{
        /* this will trigger any scheduled timer callbacks */
        timer_poll();

        /* handle console input */
        console_poll();

        /* wl api 'tick' */
        wl_tick(timer_get_ms());

        /* lwip driver poll */
        wlif_poll(hs->net_cfg.netif);

#ifdef WITH_GUI
        if (hs->wl_init_complete)
                gui_exec(timer_get_ms());
#endif
}


/**
 *
 */
void
wl_init_complete_cb(void* ctx)
{
        struct http_server *hs = ctx;
        struct ip_addr ipaddr, netmask, gw;
        wl_err_t wl_status;

        IP4_ADDR(&gw, 0,0,0,0);
        IP4_ADDR(&ipaddr, 0,0,0,0);
        IP4_ADDR(&netmask, 0,0,0,0);

        /* default is dhcp enabled */
        hs->net_cfg.dhcp_enabled = 1;
        start_ip_stack(&hs->net_cfg,
                       ipaddr,
                       netmask,
                       gw);
        netif_set_status_callback(hs->net_cfg.netif, ip_status_cb);

        /* initialize shell */
        console_init();
        console_add_cmd("scan", cmd_scan, &hs->net_cfg);
        console_add_cmd("connect", cmd_connect, &hs->net_cfg);
        console_add_cmd("ip", cmd_set_ip, &hs->net_cfg);
        console_add_cmd("setkey", cmd_setkey, &hs->net_cfg);
        console_add_cmd("status", cmd_status, &hs->net_cfg);
        console_add_cmd("powersave", cmd_power, &hs->net_cfg);
        console_add_cmd("psconf", cmd_psconf, &hs->net_cfg);
        console_add_cmd("ping", cmd_ping, &hs->net_cfg);
        console_add_cmd("ttcp", cmd_ttcp, &hs->net_cfg);
#ifdef WITH_WPA
        console_add_cmd("wpass", cmd_setpass, &hs->net_cfg);
        console_add_cmd("dpass", cmd_delpass, &hs->net_cfg);
#endif

        /* start connection manager */
        wl_status = wl_cm_start(NULL, wl_cm_conn_cb, wl_cm_disconn_cb, hs);
        ASSERT(wl_status == WL_SUCCESS, "failed to init wl conn mgr");

        /* init devices used by httpd */
        adc_init();
        led_init();

        gui_start();
        fsdata_app_enable();
        wl_scan();

        hs->wl_init_complete = 1;
}

/**
 *
 */
int
main(void)
{
        wl_err_t wl_status;
        int status;
        struct http_server *hs;
        enum wl_host_attention_mode mode;

        board_init();

        hs = calloc(1, sizeof(struct http_server));
        ASSERT(hs, "out of memory");

        hs->net_cfg.netif = calloc(1, sizeof(struct netif));
        ASSERT(hs->net_cfg.netif, "out of memory");


        timer_init(NULL, NULL);
        lwip_init();

        status = fw_download_init();
        ASSERT(status == 0, "failed to prepare for firmware download\n");


        wl_status = wl_fw_download(hs, fw_download_cb, &mode);
        if (wl_status != WL_SUCCESS)
                goto err;

        wl_status = wl_init(hs, wl_init_complete_cb, mode);
        if (wl_status != WL_SUCCESS)
                goto err;

        /* start main loop */
        for (;;)
                poll(hs);


 err:
        /* show error message on console and display if wlan initialization fails */

#define WL_CARD_FAILURE_STR     "Could not detect wl device, aborting\n"
#define WL_FIRMWARE_INVALID_STR "Invalid firmware data, aborting\n"
#define WL_OTHER_FAILURE_STR    "Failed to start wl initialization\n"

        gui_start();
        switch (wl_status) {
        case WL_CARD_FAILURE:
                printk(WL_CARD_FAILURE_STR);
                gui_fatal(WL_CARD_FAILURE_STR);
                break;

        case WL_FIRMWARE_INVALID:
                printk(WL_FIRMWARE_INVALID_STR);
                gui_fatal(WL_FIRMWARE_INVALID_STR);
                break;

        default:
                printk(WL_OTHER_FAILURE_STR);
                gui_fatal(WL_OTHER_FAILURE_STR);
                break;
        }
        for (;;) {
                timer_poll();

#ifdef WITH_GUI
                gui_exec(timer_get_ms());
#endif
        }

}
