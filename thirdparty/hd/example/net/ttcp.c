/*! \page License
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
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
 *
 * This file was derived from a part of the lwIP TCP/IP stack.
 */

#include "lwip/opt.h"

#include "lwip/mem.h"
#include "lwip/raw.h"
#include "lwip/icmp.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "lwip/inet_chksum.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"

#include <net/ttcp.h>
#include <ports/avr32/timer.h>
#include <util/util.h>
#include <net/lwip_setup.h>
#include "getopt.h"
#include <wl/cmd_wl.h>

#define TTCP_MODE_TRANSMIT          0
#define TTCP_MODE_RECEIVE           1

typedef void (ttcp_done_cb_t)(void *opaque, int result);


struct ttcp {

        /* options */
        struct ip_addr addr;    /* host */
        uint16_t port;          /* -p */
        uint16_t nbuf;          /* -n */
        int mode;               /* -t */
        int verbose;            /* -v */
        int udp;                /* -u */

        /* common */
        uint16_t print_cnt;
        uint32_t start_time;
        uint32_t left;
        uint32_t recved;
        ttcp_done_cb_t* done_cb;
        void* opaque;
        uint32_t buflen;	/* -l */
        uint32_t tid;

        /* TCP specific */
        struct tcp_pcb* tpcb;
        struct tcp_pcb* lpcb;
        char* payload;

        /* UDP specific */
        int udp_started;
	uint16_t udp_end_marker_left;
        struct udp_pcb* upcb;
};


/**
 * Calculate bitrate based on number of bytes transmitted and elapsed time
 */
static void
ttcp_print_stats(struct ttcp *ttcp)
{
        uint32_t ms = timer_get_ms() - ttcp->start_time;
        uint32_t bytes = ttcp->mode == TTCP_MODE_TRANSMIT ?
                ttcp->nbuf * ttcp->buflen : ttcp->recved;

        if (ttcp->verbose)
                printk("\n");

        printk("TTCP [%p]: %d bytes processed, %d.%d KB/s (%s/%s)\n",
               ttcp, bytes, bytes / ms, bytes % ms, ttcp->udp ? "udp" : "tcp",
               ttcp->mode == TTCP_MODE_TRANSMIT ? "tx" : "rx");
}


/**
 * Clean up and free the ttcp structure
 */
static void
ttcp_destroy(struct ttcp* ttcp)
{
        if (ttcp->tpcb) {
                tcp_arg(ttcp->tpcb, NULL);
                tcp_sent(ttcp->tpcb, NULL);
                tcp_recv(ttcp->tpcb, NULL);
                tcp_err(ttcp->tpcb, NULL);
                tcp_close(ttcp->tpcb);
        }

        if (ttcp->lpcb) {
                tcp_arg(ttcp->lpcb, NULL);
                tcp_accept(ttcp->lpcb, NULL);
                tcp_close(ttcp->lpcb);
        }

        if (ttcp->upcb) {
                udp_disconnect(ttcp->upcb);
                udp_remove(ttcp->upcb);
        }

        if (ttcp->payload)
                free(ttcp->payload);
        timer_cancel_timeout(ttcp->tid);
        free(ttcp);
}


/**
 * Invoked when transfer is done or aborted (non-zero result).
 */
static void
ttcp_done(struct ttcp* ttcp, int result)
{
        if (result == 0)
                ttcp_print_stats(ttcp);

        if (ttcp->done_cb)
                ttcp->done_cb(ttcp->opaque, result);

        ttcp_destroy(ttcp);
}

static void
tcp_timeout_cb(void *ctx);

/**
 * Only used in TCP mode.
 * Will transmit a maximum of pbuf->tot_len bytes.
 * Called upon connect and when there's space available in the TCP send window
 *
 */
static void
tcp_send_data(struct ttcp *ttcp)
{
        err_t err;
        uint32_t len;

	len = ttcp->left;

        /* don't send more than we have in the payload */
        if (len > ttcp->buflen)
                len = ttcp->buflen;

        /* We cannot send more data than space available in the send
           buffer. */
        if (len > tcp_sndbuf(ttcp->tpcb))
                len = tcp_sndbuf(ttcp->tpcb);

        err = tcp_write(ttcp->tpcb, ttcp->payload, len, 0);

        /* ERR_MEM now means out of buffers, try again later */

        if (err == ERR_OK)
                ttcp->left -= len;
        else if (err != ERR_MEM)
                printk("TTCP [%p]: tcp_write failed %d\n", ttcp, tcp_sndbuf(ttcp->tpcb));

        ttcp->tid = timer_sched_timeout_cb(0, TIMEOUT_ONESHOT,
                                           tcp_timeout_cb, ttcp);
}


/**
 * Only used in TCP mode.
 * Scheduled by tcp_send_data(). tcp_sent() is not used for performance reasons.
 */
static void
tcp_timeout_cb(void *ctx)
{
        struct ttcp *ttcp = ctx;

        if (ttcp->left > 0) {
                tcp_send_data(ttcp);
                if (ttcp->verbose) {
                        printk(".");
                        if (ttcp->print_cnt % 80 == 0)
                                printk("\n");
                        ttcp->print_cnt++;
                }
                return;
        }

        /* all sent - empty queue */
        if (ttcp->tpcb->snd_queuelen)
                ttcp->tid = timer_sched_timeout_cb(0, TIMEOUT_ONESHOT,
                                                   tcp_timeout_cb, ttcp);
        else
                ttcp_done(ttcp, 0);
}


#if 0
/**
 * Only used in TCP mode.
 * Called by lwip when there is new space available in the TCP send buffer
 */
static err_t
tcp_sent_cb(void *arg, struct tcp_pcb *pcb, u16_t len)
{
        struct ttcp *ttcp = arg;

        if (ttcp->left > 0) {
                tcp_send_data(ttcp);
                if (ttcp->verbose) {
                        printk(".");
                        if (ttcp->print_cnt % 80 == 0)
                                printk("\n");
                        ttcp->print_cnt++;
                }

        } else if (pcb->snd_queuelen == 0) {
                ttcp_done(ttcp, 0);
        }

        return ERR_OK;
}
#endif

/**
 * Only used in TCP mode.
 */
static err_t
tcp_connect_cb(void *arg, struct tcp_pcb *tpcb, err_t err)
{
        struct ttcp* ttcp = arg;

        printk("TTCP [%p]: connect\n", ttcp);

        ttcp->start_time = timer_get_ms();
#if 0
        tcp_sent(tpcb, tcp_sent_cb);
#endif
        tcp_send_data(ttcp);
        return ERR_OK;
}


/**
 * Only used in TCP mode.
 */
static void
tcp_conn_err_cb(void *arg, err_t err)
{
        struct ttcp* ttcp = arg;

        printk("TTCP [%p]: connection error\n", ttcp);

        ttcp->tpcb = NULL; /* free'd by lwip upon return */
        ttcp_done(ttcp, err);
}


/**
 * Only used in TCP mode.
 */
static err_t
tcp_recv_cb(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
        struct ttcp* ttcp = arg;

        /* p will be NULL when remote end is done */
        if (p == NULL) {
                ttcp_done(ttcp, 0);
                return ERR_OK;
        }

        /* for print_stats() */
        ttcp->recved += p->tot_len;
        if (ttcp->verbose) {
                printk(".");
                if (ttcp->print_cnt % 80 == 0)
                        printk("\n");
                ttcp->print_cnt++;
        }

        tcp_recved(pcb, p->tot_len);
        pbuf_free(p);
        return ERR_OK;
}


/**
 * Only used in TCP mode.
 */
static err_t
tcp_accept_cb(void *arg, struct tcp_pcb *newpcb, err_t err)
{
        struct ttcp* ttcp = arg;

        ttcp->tpcb = newpcb;
        tcp_recv(ttcp->tpcb, tcp_recv_cb);
        tcp_err(ttcp->tpcb, tcp_conn_err_cb);

        printk("TTCP [%p]: accept\n", ttcp);
        ttcp->start_time = timer_get_ms();
        return ERR_OK;
}




/**
 * Start TCP transfer.
 */
static int
tcp_start(struct ttcp* ttcp)
{
        ttcp->tpcb = tcp_new();
        if (ttcp->tpcb == NULL) {
                printk("TTCP [%p]: could not allocate pcb\n", ttcp);
                return -1;
        }

        ttcp->payload = malloc(ttcp->buflen);
        if (ttcp->payload == NULL) {
                printk("TTCP [%p]: could not allocate payload\n", ttcp);
                return -1;
        }

        tcp_arg(ttcp->tpcb,  ttcp);

        if (ttcp->mode == TTCP_MODE_TRANSMIT) {
                tcp_err(ttcp->tpcb,  tcp_conn_err_cb);
                tcp_recv(ttcp->tpcb, tcp_recv_cb);
                if (tcp_connect(ttcp->tpcb, &ttcp->addr, ttcp->port,
                                tcp_connect_cb) != ERR_OK) {
                        printk("TTCP [%p]: tcp connect failed\n", ttcp);
                        return -1;
                }

        } else {
                tcp_bind(ttcp->tpcb, IP_ADDR_ANY, ttcp->port);
                ttcp->lpcb = tcp_listen(ttcp->tpcb);
                if (ttcp->lpcb == NULL) {
                        printk("TTCP [%p]: listen failed\n", ttcp);
                        return -1;
                }
                tcp_accept(ttcp->lpcb, tcp_accept_cb);
        }

        return 0;
}


static void
udp_send_data(struct ttcp* ttcp);

/**
 * Only used in UDP mode. Scheduled after data has been sent in udp_send_data()
 * if we have more data to send.
 */
static void udp_timeout_cb(void *ctx)
{
        struct ttcp* ttcp = ctx;
        udp_send_data(ttcp);
}


static int
udp_send_bytes(struct ttcp* ttcp, uint32_t len)
{
        struct pbuf* p = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);
        if (p == NULL) {
                printk("TTCP [%p]: could not allocate pbuf\n", ttcp);
                return -1;
        }

        if (udp_send(ttcp->upcb, p) != ERR_OK) {
                printk("TTCP [%p]: udp_send() failed\n", ttcp);
                pbuf_free(p);
                return -1;
        }

        pbuf_free(p);
        return 0;
}

/**
 * Only used in UDP mode. First call will send the start marker. When all
 * ttcp data has been sent, a number of end markers will be sent. After
 * end marker transmission, this function will complete the ttcp process.
 */
static void
udp_send_data(struct ttcp* ttcp)
{
        /* send start marker first time */
        if (!ttcp->udp_started) {
                if (udp_send_bytes(ttcp, 4) == 0) {
                        ttcp->udp_started = 1;
                        ttcp->start_time = timer_get_ms();
                }
        }

        /* normal case */
        else if (ttcp->left) {
                /* send data */
                if (udp_send_bytes(ttcp, ttcp->buflen) == 0)
                        ttcp->left -= ttcp->buflen;
        }

        /* end marker? */
        else if (ttcp->left == 0 && ttcp->udp_end_marker_left) {
                if (udp_send_bytes(ttcp, 4) == 0)
                        ttcp->udp_end_marker_left--;
        }

        /* all end markers sent */
        else if (ttcp->left == 0) {
                ttcp_done(ttcp, 0);
                return;
        }

        ttcp->tid = timer_sched_timeout_cb(0, TIMEOUT_ONESHOT,
                                           udp_timeout_cb, ttcp);
}


/**
 * Only used in UDP mode. Will finalize the ttcp process when an end marker
 * is seen.
 */
static void
udp_recv_cb(void *arg, struct udp_pcb *upcb, struct pbuf *p,
            struct ip_addr *addr, u16_t port)
{
        struct ttcp* ttcp = arg;

        /* got start marker? we might lose this so if we get it just reset
	 * the timer
	 */
        if (!ttcp->udp_started && p->tot_len <= 4) {
                ttcp->start_time = timer_get_ms();
		ttcp->udp_started = 1;
		goto out;
	}

        /* after receiving at least 1 byte, check end marker
	 * don't check udp_started since we might have lost the start marker
	 */
        if (ttcp->recved && p->tot_len <= 4) {
                ttcp_done(ttcp, 0);
                goto out;
        }

        /* for print_stats() */
        ttcp->recved += p->tot_len;
        if (ttcp->verbose) {
                printk(".");
                if (ttcp->print_cnt % 80 == 0)
                        printk("\n");
                ttcp->print_cnt++;
        }

out:
        pbuf_free(p);
}


/**
 * Start UDP transfer.
 */
static int
udp_start(struct ttcp* ttcp)
{
	ttcp->udp_end_marker_left = 5;
        ttcp->upcb = udp_new();
        if (ttcp->upcb == NULL) {
                printk("TTCP [%p]: could not allocate pcb\n", ttcp);
                return -1;
        }

        if (ttcp->mode == TTCP_MODE_TRANSMIT) {
                if (udp_connect(ttcp->upcb, &ttcp->addr, ttcp->port) !=
                    ERR_OK) {
                        printk("TTCP [%p]: udp connect failed\n", ttcp);
                        return -1;
                }
                udp_send_data(ttcp);
        } else {
                udp_recv(ttcp->upcb, udp_recv_cb, ttcp);
        }

        return 0;
}


/**
 * Start a new ttcp transfer. It should be possible to call this function
 * multiple times in order to get multiple ttcp streams. done_cb() will be
 * invoked upon completion.
 *
 */
int
ttcp_start(struct ip_addr addr, uint16_t port, void *opaque,
           ttcp_done_cb_t *done_cb,
           int mode, uint16_t nbuf, uint16_t buflen, int udp, int verbose)
{
        struct ttcp* ttcp;
        int status;

        if (mode != TTCP_MODE_TRANSMIT && mode != TTCP_MODE_RECEIVE) {
                printk("TTCP [-]: invalid mode\n");
                return -1;
        }

        if (nbuf == 0) {
                printk("TTCP [-]: invalid nbuf\n");
                return -1;
        }

        if (buflen == 0) {
                printk("TTCP [-]: invalid buflen\n");
                return -1;
        }

        ttcp = calloc(1, sizeof(struct ttcp));
        if (ttcp == NULL) {
                printk("TTCP [-]: could not allocate memory for ttcp\n");
                return -1;
        }

        ttcp->addr = addr;
        ttcp->port = port;
        ttcp->nbuf = nbuf;
        ttcp->mode = mode;
        ttcp->left = nbuf * buflen;
        ttcp->done_cb = done_cb;
        ttcp->opaque = opaque;
        ttcp->udp = udp;
        ttcp->verbose = verbose;
        ttcp->buflen = buflen;

        printk("TTCP [%p]: nbuf=%d, buflen=%d, port=%d (%s/%s)\n",
               ttcp, ttcp->nbuf, ttcp->buflen, ttcp->port,
               ttcp->udp ? "udp" : "tcp",
               ttcp->mode == TTCP_MODE_TRANSMIT ? "tx" : "rx");

        if (ttcp->udp)
                status = udp_start(ttcp);
        else
                status = tcp_start(ttcp);

        if (status)
                goto fail;

        return 0;

fail:
        ttcp_destroy(ttcp);
        return -1;
}

char usage[] = "Usage: ttcp -t/-r [-options] host\n\
        -l      length of bufs written to network (default 1024)\n\
        -n      number of bufs written to network (default 1024)\n\
        -p      port number to send to (default 2000)\n\
        -u      udp\n\
        -v      verbose\n";

/**
 *
 */
cmd_state_t
cmd_ttcp(int argc, char* argv[], void* ctx)
{

        int c;
        int mode = TTCP_MODE_TRANSMIT;
        int verbose = 0;
        uint16_t buflen = 1024;
        uint16_t nbuf = 1024;
        uint16_t port = 2000;
        int udp = 0;
        struct ip_addr addr = { 0 };
        struct net_cfg *net_cfg = ctx;

        if (! netif_is_up(net_cfg->netif)) {
                printk("Cannot start TTCP - Network is unconnected\n");
                return CMD_DONE;
        }

        optind = 1;
        while ((c = getopt(argc, argv, "utrl:n:p:v")) != -1) {
                switch (c) {
                case 't':
                        mode = TTCP_MODE_TRANSMIT;
                        break;
                case 'r':
                        mode = TTCP_MODE_RECEIVE;
                        break;
                case 'l':
                        buflen = atoi(optarg);
                        break;
                case 'v':
                        verbose = 1;
                        break;
                case 'n':
                        nbuf = atoi(optarg);
                        break;
                case 'u':
                        udp = 1;
                        break;
                case 'p':
                        port = atoi(optarg);
                        break;
                }
        }
        if (mode == TTCP_MODE_TRANSMIT) {
                if (optind >= argc) {
                        printk("%s", usage);
                        return CMD_DONE;
                }
                addr = str2ip(argv[optind]);
                if (!addr.addr) {
                        printk("%s", usage);
                        return CMD_DONE;
                }
        }

        if (ttcp_start(addr, port, NULL, NULL, mode, nbuf, buflen, udp,
                       verbose))
                return CMD_DONE;

        return CMD_DONE;
}
