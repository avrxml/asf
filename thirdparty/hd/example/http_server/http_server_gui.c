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
#include <ctype.h>
#include <board.h>
#include <stdlib.h>
#include <stdint.h>
#include <wl_api.h>
#include <wl/wl_cm.h>

#include "lwip/netif.h"

#include "http_server_gui.h"
#include <ports/avr32/gui.h>
#if BOARD == EVK1104 || BOARD == EVK1105
#include "et024006dhu.h"
#else
#define WHITE 0
#define RED 1
#endif
#include <ports/avr32/board_init.h>
#include <util/util.h>

#include <string.h>


#define MAX_KEY_LEN 64
char ascii_key[MAX_KEY_LEN];


char gui_print_buf[80];
static struct wl_network_list_t gui_network_list = { 0 };
static int gui_network_cnt = 0;
#define gui_printk(line, args...) do {                                  \
                extern char gui_print_buf[80];                          \
                sniprintf(gui_print_buf, sizeof(gui_print_buf), args);  \
                gui_set_title(gui_print_buf, line);                     \
        } while(0)


static void copy_network_list(struct wl_network_list_t *dst,
                              struct wl_network_list_t *src)
{
        int i;
        for (i = 0; i < dst->cnt; i++)
                free(dst->net[i]);
        free(dst->net);

        dst->cnt = 0;

        if (src->cnt == 0)
                return;
        dst->net = calloc(1, src->cnt * sizeof(struct wl_network_t *));
        if (dst->net == NULL) {
                printk("could not allocate all gui net array\n");
                return;
        }

        for (i = 0; i < src->cnt; i++) {
                struct wl_network_t *net = src->net[i];
                dst->net[i] = malloc(sizeof(*net));
                if (dst->net[i] == NULL) {
                        printk("could not allocate all gui nets\n");
                        return;
                }

                memcpy(dst->net[i], net, sizeof(*net));
                dst->cnt++;
        }
}

/**
 *
 */
static void
gui_scan_cb(void)
{
        struct wl_network_list_t* wl_network_list;
        uint8_t i;

        wl_scan();
        gui_clear_scroll_box();
        gui_set_bg_color(WHITE);
        gui_add_scroll_box_item("Scan Contents:", sizeof "Scan Contents:");

        wl_get_network_list(&wl_network_list);
        if (wl_network_list->cnt == 0)
                gui_printk(1, "No networks found");
        else
                gui_printk(1, " ");

        copy_network_list(&gui_network_list, wl_network_list);
        gui_network_cnt = gui_network_list.cnt;

        for (i = 0; i < gui_network_cnt; i++) {
                struct wl_network_t* net = gui_network_list.net[i];
                char print_buf[80];
                siprintf(print_buf, "%s (%ld) %s", ssid2str(&net->ssid),
                         net->rssi, enc_type2str(net->enc_type));
                printk("%s\n", print_buf);
                gui_add_scroll_box_item(print_buf, strlen(print_buf));
        }

        gui_inc_scroll_cursor();        /* inc cursor idx to make it >0 */
}


static void set_wpa_key_cb(char *ascii_key, size_t len, void *ctx) {
        struct wl_network_t *net = ctx;
        char cmd[80];

        if (NULL == ascii_key || 0 == len || NULL == ctx) {
                printk("%s : Failed to add pass phrase\n", __func__);
                gui_printk(1, "Failed to add pass phrase");
                return;
        }

        sniprintf(cmd, sizeof cmd, "wpass %s %s", ssid2str(&net->ssid), ascii_key);
        printk("%s : \"%s\"\n", __func__, cmd);
        console_schedule_cmd(cmd, 1);

        gui_printk(0, "Connecting ... ");
        gui_printk(1, " ");
        if ( WL_SUCCESS != wl_cm_set_network(&net->ssid, NULL) ) {
                printk("FAIL\n");
        }
}

static void set_wep_key_cb(char *ascii_key, size_t len, void *ctx) {
        struct wl_network_t *net = ctx;
        char cmd[80];

        if (NULL == ascii_key || 0 == len || NULL == ctx) {
                printk("%s : Failed to add wep key\n", __func__);
                gui_printk(1, "Failed to add key");
                return;
        }

        sniprintf(cmd, sizeof cmd, "setkey 0 %s", ascii_key);
        printk("%s : \"%s\"\n", __func__, cmd);
        console_schedule_cmd(cmd, 1);

        gui_printk(0, "Connecting ... ");
        gui_printk(1, " ");
        if ( WL_SUCCESS != wl_cm_set_network(&net->ssid, NULL) ) {
                printk("FAIL\n");
        }
}

/**
 *
 */
static void
gui_connect_cb(void)
{
        int index;
        struct wl_network_t* selected_net;
        struct wl_network_t* current_net = wl_get_current_network();

        index = gui_scroll_idx() - 1;
        if (index < 0 || index >= gui_network_cnt) {
                return;
        }
        selected_net = gui_network_list.net[index];
        if (equal_bssid(&selected_net->bssid, &current_net->bssid)) {
                return;
        }

        if (selected_net->enc_type != ENC_TYPE_NONE) /* this net needs key. */
        {
                if (selected_net->enc_type == ENC_TYPE_WEP) {
                        /* if key available in flash, get it, and start with that. */
                        gui_getstring(set_wep_key_cb, selected_net);
                }
                else {
                        gui_getstring(set_wpa_key_cb, selected_net);

                }
        }
        else {
                console_schedule_cmd("setkey none", 1);
                gui_printk(0, "Connecting ... ");
                gui_printk(1, " ");
                wl_cm_set_network(&selected_net->ssid, NULL);
        }
}

#if BOARD == EVK1100
/* On evk1100 w. DIP204 display. Return to AP list when back button is pressed */
static void
go_back_cb(void)
{
        gui_restore_buttons();
        gui_infobox_onoff(false);
}
#endif

/**
 *
 */
void
gui_status_up_cb(void)
{
        struct wl_network_t *net = wl_get_current_network();

        gui_printk(0, "Connected to \"%s\"", ssid2str(&net->ssid));
        gui_printk(1, "My ip address is %s", ip2str(netif_default->ip_addr));
#if BOARD == EVK1100
        gui_save_buttons();
        gui_clear_infobox(0);

        gui_add_infobox_item(ssid2str(&net->ssid), 0);

        gui_add_infobox_item(ip2str(netif_default->ip_addr), 1);
        gui_infobox_onoff(true);
        gui_set_button(0, "Back", sizeof "Back", go_back_cb);
#endif
}


/**
 *
 */
void gui_status_down_cb(void)
{

}


/**
 *
 */
void gui_link_up_cb(void)
{
        char str[WL_SSID_MAX_LENGTH + 1];
        struct wl_network_t *net = wl_get_current_network();

        memset(str, 0, sizeof(str));
        strncpy(str, net->ssid.ssid, net->ssid.len);
        gui_printk(0, "Connected to \"%s\"", str);
        gui_printk(1, "Requesting dhcp ... ");
}


/**
 *
 */
void gui_link_down_cb(void)
{
        gui_printk(0, "Connecting ... ");
        gui_printk(1, " ");
}


/**
 *
 */
void
gui_start(void)
{
        pm_freq_param_t pm_freq_param = {
                .cpu_f = FCPU_HZ,
                .pba_f = FPBA_HZ,
                .osc0_f = FOSC0,
                .osc0_startup = OSC0_STARTUP
        };
        gui_init(&pm_freq_param);
        gui_draw(1);
        gui_printk(0, "Disconnected");
        gui_set_button(0, "Scan", sizeof "Scan", gui_scan_cb);
        gui_set_button(1, "Connect", sizeof "Connect", gui_connect_cb);
        gui_set_button(2, "Up", sizeof "Up", gui_dec_scroll_cursor);
        gui_set_button(3, "Down", sizeof "Down", gui_inc_scroll_cursor);
        gui_clear_scroll_box();
        gui_set_bg_color(WHITE);
        gui_add_scroll_box_item("Scan Contents:", sizeof "Scan Contents:");
}



/**
 *
 */
void gui_fatal(const char *str)
{
        gui_clear_scroll_box();
        gui_clear_infobox(0);
        gui_set_bg_color(RED);
        gui_printk(0, "%s", str);
}
