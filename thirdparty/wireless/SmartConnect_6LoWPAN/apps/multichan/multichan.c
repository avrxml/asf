/*
 * Copyright (c) 2012, Thingsquare, http://www.thingsquare.com/.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "contiki.h"
#include "multichan.h"
#include "net/packetbuf.h"
#include "net/queuebuf.h"
#include "net/netstack.h"
#include "net/linkaddr.h"
#include <string.h>
#include <stdio.h>
#include "lib/random.h"
#include "net/mac/mac-sequence.h"

#if UIP_CONF_IPV6_RPL
#define WITH_RPL_AUTHORITY 1
#endif

#ifndef WITH_RPL_AUTHORITY
#define WITH_RPL_AUTHORITY 0 /* Max authority iff RPL root */
#endif
#if WITH_RPL_AUTHORITY
#include "net/rpl/rpl.h"
#include "net/rpl/rpl-private.h"
#else /* WITH_RPL_AUTHORITY */
#include "node-id.h" /* Max authority iff node_id == 1 */
#endif /* WITH_RPL_AUTHORITY */

#define PUSH_BASE_PERIOD (16 * CLOCK_SECOND) /* The base timer for the
					       authority push. */
static uint8_t push_num_pushed;
#define PUSH_MAX_NUM    4                   /* The maximum number of
					       authority pushes to
					       perform. */

#define AUTH_AGE_PERIOD (20 * CLOCK_SECOND) /* Automatically weaken authority */
#define AUTH_BEST 0 /* Best authority (RPL root) */
#define AUTH_WORST 70 /* Worst authority possible before AUTH_NONE */
#define AUTH_NONE 0xff
#define AUTH_GRANULARITY 2 /* My authority is my neighbor's authority + AUTH_GRANULARITY */
#define AUTH_MAX_AGE 30 /* Number of AUTH_AGE_PERIOD before authority is removed */
static uint8_t my_auth; /* My timesynch authority */
static uint8_t my_auth_age; /* My timesynch authority age */
static struct timer timer_ageauth; /* Age authority timer */

static int forced_auth = 0; /* Flag used to force best authority (even if not RPL root). For debugging purposes only. */

/* The number of channels used. */
#ifdef MULTICHAN_CONF_NR_CHANNELS
#define NR_CHANNELS MULTICHAN_CONF_NR_CHANNELS
#else /* MULTICHAN_CONF_NR_CHANNELS */
#define NR_CHANNELS 50 /* Total number of channels */
#endif /* MULTICHAN_CONF_NR_CHANNELS */

/* The first channel. */
#ifdef MULTICHAN_CONF_CHANNEL_OFFSET
#define CHANNEL_OFFSET MULTICHAN_CONF_CHANNEL_OFFSET
#else /* MULTICHAN_CONF_CHANNEL_OFFSET */
#define CHANNEL_OFFSET 0
#endif /* MULTICHAN_CONF_CHANNEL_OFFSET */

#ifdef MULTICHAN_CONF_CHANNEL_PERIOD
#define CHANNEL_PERIOD MULTICHAN_CONF_CHANNEL_PERIOD
#else /* MULTICHAN_CONF_CHANNEL_PERIOD */
#define CHANNEL_PERIOD ((4 * CLOCK_SECOND) / 10) /* Duration on each channel */
#endif /* MULTICHAN_CONF_CHANNEL_PERIOD */
#define TIME_TABLE_GRANULARITY 16 /* Time increases per channel */
#define INCREMENT_TIME_PERIOD (CHANNEL_PERIOD / TIME_TABLE_GRANULARITY)
#define TIME_WRAP (NR_CHANNELS * TIME_TABLE_GRANULARITY) /* Restart time */
static uint16_t my_time; /* Current time, decides which radio channel to use */
static uint8_t my_channel = -1; /* Current channel */
static int my_channel_idx = -1; /* Current channel index */
static uint8_t time_channel_table[NR_CHANNELS];

#define BLINK_ON_EVEN_CHANNELS 0
#if BLINK_ON_EVEN_CHANNELS
#include "dev/leds.h"
#endif /* BLINK_ON_EVEN_CHANNELS */

#define LOG_CHANNEL_ACTIVITY 1
#if LOG_CHANNEL_ACTIVITY
static int log_channel_tx[NR_CHANNELS];
static int log_channel_noack[NR_CHANNELS];
static int log_channel_rx[NR_CHANNELS];
static signed char log_channel_rssi[NR_CHANNELS];
#endif /* LOG_CHANNEL_ACTIVITY */

#define SCAN_DELAY (20*CLOCK_SECOND) /* Delay from AUTH_NONE until scan begins */
#define SCAN_RESPONSE_DELAY (CLOCK_SECOND/32) /* Rapidly scan all channels */
static struct timer timer_scan; /* Scan channels timer */

static struct timer timer_push; /* Authority push timer */

PROCESS(channel_process, "Channel hopper");
PROCESS(send_reply_process, "Synch reply");
static int send_reply = 0;
static int already_received_reply = 0;

static int send_scan_reply(void);

/* Example: #define MULTICHAN_CONF_SET_CHANNEL(x) cc1120_channel_set(x) */
#ifdef MULTICHAN_CONF_SET_CHANNEL
#define SET_CHANNEL(x) MULTICHAN_CONF_SET_CHANNEL(x)
#else /* MULTICHAN_CONF_SET_CHANNEL */
#error Error: Platform lacks #define MULTICHAN_CONF_SET_CHANNEL. See multichan.c for example.
#endif

/* Example: #define MULTICHAN_CONF_READ_RSSI(x) cc1120_read_rssi() */
#ifdef MULTICHAN_CONF_READ_RSSI
#define READ_RSSI(x) MULTICHAN_CONF_READ_RSSI(x)
#else /* MULTICHAN_CONF_READ_RSSI */
#error Error: Platform lacks #define MULTICHAN_CONF_READ_RSSI. See multichan.c for example.
#endif


#define PKT_NORMAL 252
#define PKT_SCAN_PROBE 253
#define PKT_SCAN_REPLY 254

#define WITH_HEADER 1
#if WITH_HEADER
struct hdr {
  uint8_t type;
  uint8_t auth;
  uint8_t time[2];
};
#endif /* WITH_HEADER */

struct pkt_scan {
  uint8_t type;
  uint8_t percent;
};
struct pkt_scan_reply {
  uint8_t type;
  uint8_t auth;
  uint8_t time[2];
};

static int
send_packet_chan(int broadcast);

#define _DEBUG_ 0
#if _DEBUG_
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif
#define PRINTF_FULL(...)

#define MULTICHAN_802154_AUTOACK 1
#if MULTICHAN_802154_AUTOACK
#include "sys/rtimer.h"
#include "dev/watchdog.h"

#ifndef ACK_WAIT_TIME
#define ACK_WAIT_TIME                      RTIMER_SECOND / 500
#endif
#ifndef AFTER_ACK_DETECTED_WAIT_TIME
#define AFTER_ACK_DETECTED_WAIT_TIME       RTIMER_SECOND / 250
#endif

#define ACK_LEN 3
#endif /* MULTICHAN_802154_AUTOACK */

#define WITH_DEST_FILTERING 1
#define WITH_DUP_FILTERING 1

#define WITH_SEND_TWO_CHANNELS 1
#if WITH_SEND_TWO_CHANNELS
#define SEND_TWO_CHANNELS_THRESHOLD (3*INCREMENT_TIME_PERIOD)
static clock_time_t time_last_channel_change;
static clock_time_t time_next_channel_change;
static int last_channel = -1;
static int next_channel = -1;
#endif /* WITH_SEND_TWO_CHANNELS */

#define WITH_STATS 0
#define PRINT_STATS 0
#define PRINT_STATS_PERIOD (60*CLOCK_SECOND)
#if WITH_STATS
static struct timer timer_stats;
static int pkts_tx = 0;
static int pkts_tx_acked = 0;
static int pkts_rx = 0;
static int dup_last = 0;
static int dup_none = 0;
static int dup_next = 0;
static int scans_short = 0;
static int scans_full = 0;
static int scan_success = 0;
#endif /* WITH_STATS */

#define DEBUG_COOJA 0
#if DEBUG_COOJA
static int debug_cooja_last_auth = -1;
#endif /* DEBUG_COOJA */

/*---------------------------------------------------------------------------*/
static int
send_one_packet(mac_callback_t sent, void *ptr)
{
  int ret, is_broadcast;

#if WITH_HEADER
  struct hdr *chdr;
  int hdrlen;
#endif /* WITH_HEADER */

  if(my_auth == AUTH_NONE) {
    /* We are not synchronized */
    mac_call_sent_callback(sent, ptr, MAC_TX_ERR, 1);
    return 0;
  }

  packetbuf_set_addr(PACKETBUF_ADDR_SENDER, &linkaddr_node_addr);
#if MULTICHAN_802154_AUTOACK
  packetbuf_set_attr(PACKETBUF_ATTR_MAC_ACK, 1);
#endif /* MULTICHAN_802154_AUTOACK */

#if WITH_HEADER
  if(packetbuf_hdralloc(sizeof(struct hdr)) == 0) {
    /* Failed to allocate space for multichan header */
    PRINTF("multichan: send failed 1, too large header\n");
    mac_call_sent_callback(sent, ptr, MAC_TX_ERR_FATAL, 1);
    return 0;
  }
  chdr = packetbuf_hdrptr();
  chdr->type = PKT_NORMAL;
  chdr->auth = my_auth;
  chdr->time[0] = my_time >> 8;
  chdr->time[1] = my_time & 0xff;
  /*  w_memcpy((unsigned char*)&chdr->time, (unsigned char*)&my_time,
      sizeof(my_time));*/
#endif /* WITH_HEADER */

  /* Create the MAC header for the data packet. */
  hdrlen = NETSTACK_FRAMER.create();
  if(hdrlen < 0) {
    /* Failed to send */
    PRINTF("multichan: send failed 2, too large header\n");
    mac_call_sent_callback(sent, ptr, MAC_TX_ERR_FATAL, 1);
    return 0;
  }

  is_broadcast = linkaddr_cmp(packetbuf_addr(PACKETBUF_ADDR_RECEIVER),
                              &linkaddr_null);

#ifdef NETSTACK_ENCRYPT
    NETSTACK_ENCRYPT();
#endif /* NETSTACK_ENCRYPT */

#if WITH_SEND_TWO_CHANNELS
  /* Check if we recently changed channels, or soon will.
   * If so, we send the message on both channels! */
  if(clock_time() - time_last_channel_change < SEND_TWO_CHANNELS_THRESHOLD
      && last_channel >= 0) {
    SET_CHANNEL(last_channel);
    ret = send_packet_chan(is_broadcast);
#if MULTICHAN_802154_AUTOACK

    /* If we already received a unicast-ACK, no need to transmit again */
    if(!is_broadcast && ret == MAC_TX_OK) {
      SET_CHANNEL(my_channel);
      mac_call_sent_callback(sent, ptr, ret, 1);
    } else {
      SET_CHANNEL(my_channel);
      ret = send_packet_chan(is_broadcast);
      mac_call_sent_callback(sent, ptr, ret, 1);
    }
#else /* MULTICHAN_802154_AUTOACK */
    SET_CHANNEL(my_channel);
    ret = send_packet_chan(is_broadcast);
    mac_call_sent_callback(sent, ptr, ret, 1);
#endif /* MULTICHAN_802154_AUTOACK */

    PRINTF("Duplicated tx on previous channel\n");
#if WITH_STATS
    dup_last++;
#endif /* WITH_STATS */
  } else if((clock_time() > time_next_channel_change
      || time_next_channel_change - clock_time() < SEND_TWO_CHANNELS_THRESHOLD)
      && next_channel >= 0) {
    ret = send_packet_chan(is_broadcast);
#if MULTICHAN_802154_AUTOACK
    /* If we already received a unicast-ACK, no need to transmit again */
    if(!is_broadcast && ret == MAC_TX_OK) {
      mac_call_sent_callback(sent, ptr, ret, 1);
    } else {
      SET_CHANNEL(next_channel);
      ret = send_packet_chan(is_broadcast);
      SET_CHANNEL(my_channel);
      mac_call_sent_callback(sent, ptr, ret, 1);
    }
#else /* MULTICHAN_802154_AUTOACK */
    SET_CHANNEL(next_channel);
    ret = send_packet_chan(is_broadcast);
    SET_CHANNEL(my_channel);
    mac_call_sent_callback(sent, ptr, ret, 1);
#endif /* MULTICHAN_802154_AUTOACK */

    PRINTF("Duplicated tx on next channel\n");
#if WITH_STATS
    dup_next++;
#endif /* WITH_STATS */
  } else {
    ret = send_packet_chan(is_broadcast);
    mac_call_sent_callback(sent, ptr, ret, 1);
#if WITH_STATS
    dup_none++;
#endif /* WITH_STATS */
  }

#else /* WITH_SEND_TWO_CHANNELS */
  ret = send_packet_chan(is_broadcast);
  mac_call_sent_callback(sent, ptr, ret, 1);
#endif /* WITH_SEND_TWO_CHANNELS */
  return 1;
}
/*---------------------------------------------------------------------------*/
static void
send_packet(mac_callback_t sent, void *ptr)
{
  send_one_packet(sent, ptr);
}
/*---------------------------------------------------------------------------*/
static int
send_packet_chan(int is_broadcast)
{
  int ret;

#if LOG_CHANNEL_ACTIVITY
  log_channel_tx[my_channel_idx]++;
#endif /* LOG_CHANNEL_ACTIVITY */

#if MULTICHAN_802154_AUTOACK
  /*int is_broadcast;*/
  uint8_t dsn;
  dsn = ((uint8_t *)packetbuf_hdrptr())[2] & 0xff;

  NETSTACK_RADIO.prepare(packetbuf_hdrptr(), packetbuf_totlen());

  /*is_broadcast = linkaddr_cmp(packetbuf_addr(PACKETBUF_ADDR_RECEIVER),
                              &linkaddr_null);*/

  if(NETSTACK_RADIO.receiving_packet() ||
      (!is_broadcast && NETSTACK_RADIO.pending_packet())) {

    /* Currently receiving a packet over air or the radio has
         already received a packet that needs to be read before
         sending with auto ack. */
    ret = MAC_TX_COLLISION;

  } else {
    switch(NETSTACK_RADIO.transmit(packetbuf_totlen())) {
    case RADIO_TX_OK:
      if(is_broadcast) {
        ret = MAC_TX_OK;
      } else {
        rtimer_clock_t wt;

#if WITH_STATS
        pkts_tx++;
#endif /* WITH_STATS */

        /* Check for ack */
        wt = RTIMER_NOW();
        //watchdog_periodic();
		wdt_reset_count();
        while(RTIMER_CLOCK_LT(RTIMER_NOW(), wt + ACK_WAIT_TIME));

        ret = MAC_TX_NOACK;
        if(NETSTACK_RADIO.receiving_packet() ||
            NETSTACK_RADIO.pending_packet() ||
            NETSTACK_RADIO.channel_clear() == 0) {
          int len;
          uint8_t ackbuf[ACK_LEN];

          wt = RTIMER_NOW();
          //watchdog_periodic();
		  wdt_reset_count();
          while(RTIMER_CLOCK_LT(RTIMER_NOW(), wt + AFTER_ACK_DETECTED_WAIT_TIME));

          if(NETSTACK_RADIO.pending_packet()) {
            len = NETSTACK_RADIO.read(ackbuf, ACK_LEN);
            if(len == ACK_LEN && ackbuf[2] == dsn) {
              /* Ack received */
              ret = MAC_TX_OK;
#if WITH_STATS
              pkts_tx_acked++;
#endif /* WITH_STATS */
            } else {
              /* Not an ack or ack not for us: collision */
              ret = MAC_TX_COLLISION;
            }
          }
        }

#if LOG_CHANNEL_ACTIVITY
        if(ret != MAC_TX_OK) {
          log_channel_noack[my_channel_idx]++;
        }
#endif /* LOG_CHANNEL_ACTIVITY */
      }
      break;
    case RADIO_TX_COLLISION:
      ret = MAC_TX_COLLISION;
      break;
    default:
      ret = MAC_TX_ERR;
      break;
    }
  }

#else /* ! MULTICHAN_802154_AUTOACK */

  switch(NETSTACK_RADIO.send(packetbuf_hdrptr(), packetbuf_totlen())) {
  case RADIO_TX_OK:
    ret = MAC_TX_OK;
    break;
  case RADIO_TX_COLLISION:
    ret = MAC_TX_COLLISION;
    break;
  case RADIO_TX_NOACK:
    ret = MAC_TX_NOACK;
    break;
  default:
    ret = MAC_TX_ERR;
    break;
  }

#endif /* ! MULTICHAN_802154_AUTOACK */

  return ret;
}
/*---------------------------------------------------------------------------*/
static void
send_list(mac_callback_t sent, void *ptr, struct rdc_buf_list *buf_list)
{
  while(buf_list != NULL) {
    /* We backup the next pointer, as it may be nullified by
     * mac_call_sent_callback() */
    struct rdc_buf_list *next = buf_list->next;
    int last_sent_ok;

    queuebuf_to_packetbuf(buf_list->buf);
    last_sent_ok = send_one_packet(sent, ptr);

    /* If packet transmission was not successful, we should back off and let
     * upper layers retransmit, rather than potentially sending out-of-order
     * packet fragments. */
    if(!last_sent_ok) {
      return;
    }
    buf_list = next;
  }
}
/*---------------------------------------------------------------------------*/
static void
handle_new_time(uint8_t auth, unsigned char* time_ptr)
{
  if(auth != AUTH_NONE &&
     (my_auth == AUTH_NONE || auth + AUTH_GRANULARITY < my_auth)) {
    timer_set(&timer_ageauth, AUTH_AGE_PERIOD); /* reset age timeout */

    if(my_auth == AUTH_NONE) {
      printf("multichan: Auth improved from NONE to %d\n",
             auth + AUTH_GRANULARITY);
    }
    my_auth = auth + AUTH_GRANULARITY;

#if DEBUG_COOJA
    if(debug_cooja_last_auth != my_auth) {
      printf("#A a=%d 1\n", my_auth);
      debug_cooja_last_auth = my_auth;
    }
#endif /* DEBUG_COOJA */

    my_auth_age = 0;
    memcpy(&my_time, time_ptr, sizeof(my_time));
    PRINTF("Authority inherited %u, time %u\n", my_auth, my_time);

#if DEBUG_COOJA
    if(debug_cooja_last_auth != my_auth) {
      printf("#A a=%d 2\n", my_auth);
      debug_cooja_last_auth = my_auth;
    }
#endif /* DEBUG_COOJA */

    if(my_auth >= AUTH_WORST) {
      PRINTF("Authority is already bad\n");
      my_auth = AUTH_NONE;
#if DEBUG_COOJA
      if(debug_cooja_last_auth != my_auth) {
        printf("#A a=%d 3\n", my_auth);
        debug_cooja_last_auth = my_auth;
      }
#endif /* DEBUG_COOJA */
      timer_set(&timer_scan, SCAN_DELAY);
    }
  } else {
    PRINTF_FULL("Authority not inherited %u + %u !< %u, time %u\n", auth,
           AUTH_GRANULARITY, my_auth, my_time);
  }
}
/*---------------------------------------------------------------------------*/
static int
send_scan_probe(int percent)
{
  struct pkt_scan pkt;
  int hdrlen;

  /* Prepare scan probe */
  packetbuf_clear();
  pkt.type = PKT_SCAN_PROBE;
  pkt.percent = percent;
  packetbuf_copyfrom(&pkt, sizeof(struct pkt_scan));
  packetbuf_set_datalen(sizeof(struct pkt_scan));

  /* 802.15.4 header */
  packetbuf_set_addr(PACKETBUF_ADDR_SENDER, &linkaddr_node_addr);
#if MULTICHAN_802154_AUTOACK
  packetbuf_set_attr(PACKETBUF_ATTR_MAC_ACK, 0);
#endif /* MULTICHAN_802154_AUTOACK */
  hdrlen = NETSTACK_FRAMER.create();
  if(hdrlen < 0) {
    /* Failed to send */
    PRINTF("multichan: send probe failed, too large header\n");
    return MAC_TX_ERR_FATAL;
  }

#ifdef NETSTACK_ENCRYPT
    NETSTACK_ENCRYPT();
#endif /* NETSTACK_ENCRYPT */

  PRINTF_FULL("Send scan probe\n");
  return NETSTACK_RADIO.send(packetbuf_hdrptr(), packetbuf_totlen());
}
/*---------------------------------------------------------------------------*/
static int
send_scan_reply(void)
{
  struct pkt_scan_reply pkt;
  int hdrlen;

  if(my_auth == AUTH_NONE) {
    PRINTF_FULL("No auth., no scan reply\n");
    return MAC_TX_ERR_FATAL;
  }

  /* Prepare scan probe */
  packetbuf_clear();
  pkt.type = PKT_SCAN_REPLY;
  pkt.auth = my_auth;
  /*  w_memcpy(&pkt.time, &my_time, sizeof(my_time));*/
  pkt.time[0] = my_time >> 8;
  pkt.time[1] = my_time & 0xff;
  packetbuf_copyfrom(&pkt, sizeof(struct pkt_scan_reply));
  packetbuf_set_datalen(sizeof(struct pkt_scan_reply));

  /* 802.15.4 header */
  packetbuf_set_addr(PACKETBUF_ADDR_SENDER, &linkaddr_node_addr);
#if MULTICHAN_802154_AUTOACK
  packetbuf_set_attr(PACKETBUF_ATTR_MAC_ACK, 0);
#endif /* MULTICHAN_802154_AUTOACK */
  hdrlen = NETSTACK_FRAMER.create();
  if(hdrlen < 0) {
    /* Failed to send */
    PRINTF("multichan: send probe failed, too large header\n");
    return MAC_TX_ERR_FATAL;
  }

#ifdef NETSTACK_ENCRYPT
  NETSTACK_ENCRYPT();
#endif /* NETSTACK_ENCRYPT */

  PRINTF_FULL("Send scan reply\n");
  return NETSTACK_RADIO.send(packetbuf_hdrptr(), packetbuf_totlen());
}
/*---------------------------------------------------------------------------*/
static void
packet_input(void)
{
  if(packetbuf_datalen() <= 0 || packetbuf_totlen() <= 0) {
    return;
  }

#if MULTICHAN_802154_AUTOACK
  if(packetbuf_datalen() == ACK_LEN) {
    /* Ignore ack packets */
    /* PRINTF("multichan: ignored ack\n"); */
    return;
  }
#endif /* MULTICHAN_802154_AUTOACK */

#ifdef NETSTACK_DECRYPT
  NETSTACK_DECRYPT();
#endif /* NETSTACK_DECRYPT */

  if(NETSTACK_FRAMER.parse() < 0) {
    printf("multichan: failed to parse %u\n", packetbuf_datalen());
  } else {
    if(packetbuf_datalen() == 0) {
      /* Empty packet - ignore */
      return;
    }

#if WITH_HEADER
    struct hdr *chdr;
    struct pkt_scan *pkt_scan;
    struct pkt_scan_reply *pkt_scan_reply;
    pkt_scan = packetbuf_dataptr();
    pkt_scan_reply = packetbuf_dataptr();

    /* Parse scan packets */
    if(packetbuf_datalen() == sizeof(struct pkt_scan)
        && pkt_scan->type == PKT_SCAN_PROBE) {
      int r;
      if(my_auth == AUTH_NONE) {
        PRINTF_FULL("No auth., no scan reply\n");
        return;
      }
      r = random_rand()%100;
      if(r > pkt_scan->percent) {
        PRINTF_FULL("Avoiding congestion, no scan reply (%u>%u)\n", r, pkt_scan->percent);
        return;
      }
      /*send_scan_reply();*/
      send_reply = 1;
      process_poll(&send_reply_process);
      return;
    }
    if(packetbuf_datalen() == sizeof(struct pkt_scan_reply)
        && pkt_scan_reply->type == PKT_SCAN_REPLY) {
      uint16_t time;
      already_received_reply = 1;
      time = (pkt_scan_reply->time[0] << 8) +
        pkt_scan_reply->time[1];
      handle_new_time(pkt_scan_reply->auth,
                      (unsigned char *)&time);
      return;
    }

    /* Strip multichan protocol header */
    chdr = packetbuf_dataptr();
    if(chdr->type == PKT_NORMAL) {
      uint16_t time;
      time = (chdr->time[0] << 8) + chdr->time[1];
      handle_new_time(chdr->auth, (unsigned char *)&time);
      packetbuf_hdrreduce(sizeof(struct hdr));
    }
#endif /* WITH_HEADER */

#if LOG_CHANNEL_ACTIVITY
    log_channel_rx[my_channel_idx]++;
#endif /* LOG_CHANNEL_ACTIVITY */

#if WITH_DEST_FILTERING
    if(packetbuf_datalen() <= 0 || packetbuf_totlen() <= 0) {
      return;
    }
    if(!linkaddr_cmp(packetbuf_addr(PACKETBUF_ADDR_RECEIVER),
                     &linkaddr_node_addr)
        && !linkaddr_cmp(packetbuf_addr(PACKETBUF_ADDR_RECEIVER),
                         &linkaddr_null)) {
      return;
    }

#endif /* WITH_DEST_FILTERING */

#if WITH_DUP_FILTERING
    /* Check for duplicate packet by comparing the sequence number
       of the incoming packet with the last few ones we saw. */
    if(mac_sequence_is_duplicate()) {
      /* Drop the packet. */
      PRINTF("multichan: drop duplicate link layer packet %u\n",
             packetbuf_attr(PACKETBUF_ATTR_PACKET_ID));
      return;
    } else {
      mac_sequence_register_seqno();
    }
#endif /* WITH_DUP_FILTERING */

#if WITH_STATS
    pkts_rx++;
#endif /* WITH_STATS */

    NETSTACK_MAC.input();
  }
}
/*---------------------------------------------------------------------------*/
static int
on(void)
{
  return NETSTACK_RADIO.on();
}
/*---------------------------------------------------------------------------*/
static int
off(int keep_radio_on)
{
  if(keep_radio_on) {
    return NETSTACK_RADIO.on();
  } else {
    return NETSTACK_RADIO.off();
  }
}
/*---------------------------------------------------------------------------*/
static unsigned short
channel_check_interval(void)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
static void
multichan_force_auth(int become_auth)
{
  forced_auth = become_auth;
}
/*---------------------------------------------------------------------------*/
/* This function pushes a node's authority to its neighbors, but only
   if the node has AUTH_BEST authority. This is used as part of the
   startup procedure of a root node, and is also done at regular and
   increasing intervals. */
PROCESS(push_auth_to_network_process, "Multichan auth push process");
PROCESS_THREAD(push_auth_to_network_process, ev, data)
{
  PROCESS_BEGIN();
  /* Push our authority over all channels */
  if(my_auth == AUTH_BEST) {
    static int scan_channel;
    static int offset;

    scan_channel = 0;
    offset = random_rand() % NR_CHANNELS;

    while(scan_channel < NR_CHANNELS) {

      SET_CHANNEL(time_channel_table[(scan_channel + offset) % NR_CHANNELS]);
#if BLINK_ON_EVEN_CHANNELS
      leds_toggle(LEDS_ALL);
#endif /* BLINK_ON_EVEN_CHANNELS */

      PRINTF_FULL("multichan: Sending auth push at channel [%u] %u\n", scan_channel,
		  time_channel_table[scan_channel]);

      /* Send scan reply to push our authority to all that hears. */
      send_scan_reply();

      scan_channel++;

      PROCESS_PAUSE();
    }
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
static void
push_auth_to_network(void)
{
  process_start(&push_auth_to_network_process, NULL);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(channel_process, ev, data)
{
  static struct etimer et;
  static struct etimer et2;
  int ch;

  PROCESS_BEGIN();

  timer_set(&timer_ageauth, AUTH_AGE_PERIOD);
  if(my_auth == AUTH_NONE) {
    timer_set(&timer_scan, SCAN_DELAY);
  }

#if PRINT_STATS
  timer_set(&timer_stats, PRINT_STATS_PERIOD);
#endif /* PRINT_STATS */

  etimer_set(&et, INCREMENT_TIME_PERIOD);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

  while(1) {

#if PRINT_STATS
    if(timer_expired(&timer_stats)) {
      int i;
      timer_set(&timer_stats, PRINT_STATS_PERIOD);
      printf("STATS: pkts ack/tx %u/%u, rx %u\n", pkts_tx_acked, pkts_tx, pkts_rx);
      printf("STATS: dups early %u none %u late %u\n", dup_last, dup_none, dup_next);
      printf("STATS: scans short %u full %u success %u\n", scans_short, scans_full, scan_success);
#if LOG_CHANNEL_ACTIVITY
      printf("STATS: tx/ch: ");
      for(i = 0; i < NR_CHANNELS; i++) {
        printf("%d ", log_channel_tx[i]);
      }
      printf("\n");
      printf("STATS: noack/ch: ");
      for(i = 0; i < NR_CHANNELS; i++) {
        printf("%d ", log_channel_noack[i]);
      }
      printf("\n");
      printf("STATS: rx/ch: ");
      for(i = 0; i < NR_CHANNELS; i++) {
        printf("%d ", log_channel_rx[i]);
      }
      printf("\n");
      printf("STATS: last rssi/ch: ");
      for(i = 0; i < NR_CHANNELS; i++) {
        printf("%d ", log_channel_rssi[i]);
      }
      printf("\n");
#endif /* LOG_CHANNEL_ACTIVITY */
    }
#endif /* PRINT_STATS */


    if(timer_expired(&timer_push)) {
      int multiplication_factor;

      multiplication_factor = 1 << push_num_pushed;
      timer_set(&timer_push, PUSH_BASE_PERIOD * multiplication_factor);
      if(push_num_pushed < PUSH_MAX_NUM) {
	push_auth_to_network();
	push_num_pushed++;
      }
    }

    /* Age authority */
    if(timer_expired(&timer_ageauth)) {
      timer_set(&timer_ageauth, AUTH_AGE_PERIOD);
      if(my_auth != AUTH_NONE) {
        my_auth++;
        my_auth_age++;
#if DEBUG_COOJA
        if(debug_cooja_last_auth != my_auth) {
          printf("#A a=%d 4\n", my_auth);
          debug_cooja_last_auth = my_auth;
        }
#endif /* DEBUG_COOJA */
        PRINTF_FULL("Authority aged %u, %u\n", my_auth, my_auth_age);
        if(my_auth >= AUTH_WORST) {
          PRINTF("Authority timed out\n");
          my_auth = AUTH_NONE;
#if DEBUG_COOJA
          if(debug_cooja_last_auth != my_auth) {
            printf("#A a=%d 5\n", my_auth);
            debug_cooja_last_auth = my_auth;
          }
#endif /* DEBUG_COOJA */
          timer_set(&timer_scan, SCAN_DELAY);
        } else if(my_auth_age >= AUTH_MAX_AGE) {
          PRINTF("Authority died from old age\n");
          my_auth = AUTH_NONE;
#if DEBUG_COOJA
          if(debug_cooja_last_auth != my_auth) {
            printf("#A a=%d 6\n", my_auth);
            debug_cooja_last_auth = my_auth;
          }
#endif /* DEBUG_COOJA */
          timer_set(&timer_scan, SCAN_DELAY);
        }
      }
    }

    /* Start scanning all channels */
    if(my_auth == AUTH_NONE && timer_expired(&timer_scan)) {
      static int scan_channel;
      static int offset;

      /* First, probe our guessed channel, hoping it's still correct */
      PRINTF("Probing our guessed channel\n");
#if WITH_STATS
      scans_short++;
#endif /* WITH_STATS */

      /* Send scan probe */
      send_scan_probe(100);

      /* Await response, or try to reduce congestion */
      etimer_set(&et2, SCAN_RESPONSE_DELAY);
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et2));

      /* Send scan probe */
      send_scan_probe(50);

      /* Await response, or start scanning */
      etimer_reset(&et2);
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et2));
      printf("multichan: Started scanning %d channels\n", NR_CHANNELS);
      scan_channel = 0;
      offset = random_rand()%NR_CHANNELS;
#if WITH_STATS
      if(my_auth == AUTH_NONE) {
        scans_full++;
      }
#endif /* WITH_STATS */
      while(my_auth == AUTH_NONE && scan_channel < NR_CHANNELS) {

        SET_CHANNEL(time_channel_table[(scan_channel+offset)%NR_CHANNELS]);
#if BLINK_ON_EVEN_CHANNELS
        leds_toggle(LEDS_ALL);
#endif /* BLINK_ON_EVEN_CHANNELS */

        PRINTF_FULL("multichan: Sending scan probe at channel [%u] %u\n", scan_channel,
               time_channel_table[scan_channel]);

        /* Send scan probe */
        send_scan_probe(100);

        /* Await response, or try to reduce congestion */
        etimer_reset(&et2);
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et2));

        /* Send scan probe */
        send_scan_probe(50);

        /* Await response, or try next channel */
        etimer_reset(&et2);
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et2));

        scan_channel++;
      }
      if(my_auth != AUTH_NONE) {
        printf("multichan: Finished scanning, have auth %d\n",
               my_auth);
      } else {
        printf("multichan: Finished scanning, have no auth\n");
      }

      timer_set(&timer_scan, SCAN_DELAY); /* Repeat scan later if still not synchronized */

      /* We restart the progress-time-timer, to avoid that the timer now rapidly fires to catch up */
      etimer_set(&et, INCREMENT_TIME_PERIOD);

#if WITH_STATS
      if(my_auth != AUTH_NONE) {
        scan_success++;
      }
#endif /* WITH_STATS */
    }

    /* Give us authority if we are the RPL root */
#if WITH_RPL_AUTHORITY
    {
      rpl_dag_t *dag;
      dag = rpl_get_any_dag();
      if(dag != NULL) {
        if(dag->rank == RPL_MIN_HOPRANKINC) {
	  /*          printf("We are the RPL DAG root\n");*/
          my_auth = AUTH_BEST;
#if DEBUG_COOJA
          if(debug_cooja_last_auth != my_auth) {
            printf("#A a=%d 7\n", my_auth);
            debug_cooja_last_auth = my_auth;
          }
#endif /* DEBUG_COOJA */
        }
      }
    }
#else /* WITH_RPL_AUTHORITY */
    if(node_id == 1) {
      my_auth = AUTH_BEST;
#if DEBUG_COOJA
      if(debug_cooja_last_auth != my_auth) {
        printf("#A a=%d 8\n", my_auth);
        debug_cooja_last_auth = my_auth;
      }
#endif /* DEBUG_COOJA */
    }
#endif /* WITH_RPL_AUTHORITY */
    if(forced_auth) {
      my_auth = AUTH_BEST;
    }

    /* Increase time, change channel */
    my_time++;
    if(my_time >= TIME_WRAP) {
      my_time = 0;
    }
    ch = time_channel_table[my_time / TIME_TABLE_GRANULARITY];
    my_channel_idx = my_time / TIME_TABLE_GRANULARITY;

    if(ch != my_channel) {
      PRINTF_FULL("change channel (t=%d) [%d]: channel %u\n", my_time, my_time / TIME_TABLE_GRANULARITY, ch);
#if BLINK_ON_EVEN_CHANNELS
    if ((my_channel_idx % 8) == 0) {
      leds_toggle(LEDS_ALL);
    }
#endif /* BLINK_ON_EVEN_CHANNELS */
#if LOG_CHANNEL_ACTIVITY
    log_channel_rssi[my_channel_idx] = READ_RSSI();
#endif /* LOG_CHANNEL_ACTIVITY */

#if WITH_SEND_TWO_CHANNELS
      time_last_channel_change = clock_time();
      last_channel = my_channel;
      time_next_channel_change =
          clock_time() + TIME_TABLE_GRANULARITY * INCREMENT_TIME_PERIOD;
      next_channel = time_channel_table[((my_time / TIME_TABLE_GRANULARITY) + 1)
                                        % 50];
#endif /* WITH_SEND_TWO_CHANNELS */

      /* TODO XXX Check if we actually managed to change the channel */
      /* XXX Or do this in the radio driver */
      my_channel = ch;
      SET_CHANNEL(ch);
    }

    if(etimer_expired(&et)) {
      etimer_reset(&et);
    }
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(send_reply_process, ev, data)
{
  static struct etimer et;

  PROCESS_BEGIN();

  while(1) {
    PROCESS_WAIT_UNTIL(send_reply);
    send_reply = 0;
    already_received_reply = 0;

#define BACKOFF_MAX_TIME (SCAN_RESPONSE_DELAY)
    etimer_set(&et, 1+(random_rand()%BACKOFF_MAX_TIME));
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    /* Suppress reply if we recently heard one */
    if(!already_received_reply) {
      send_scan_reply();
      PRINTF("Sent reply\n");
    } else {
      PRINTF("Suppressing reply\n");
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
void
multichan_set_channel_list(uint8_t* chans, int len)
{
  int t;
  /* Populate channel table incrementally */
  for(t = 0; t < NR_CHANNELS; t++) {
    printf("Time %d: channel %d\n", t, chans[t%len]);
    time_channel_table[t] = chans[t % len] + CHANNEL_OFFSET;
  }

}
/*---------------------------------------------------------------------------*/
static void
init(void)
{
  int t;
  static int inited = 0;
  if(inited) {
    return;
  }
  inited = 1;

  my_time = 0;
  my_auth = AUTH_NONE;
#if DEBUG_COOJA
  if(debug_cooja_last_auth != my_auth) {
    printf("#A a=%d 9\n", my_auth);
    debug_cooja_last_auth = my_auth;
  }
#endif /* DEBUG_COOJA */

#if LOG_CHANNEL_ACTIVITY
  for(t = 0; t < NR_CHANNELS; t++) {
    log_channel_tx[t] = 0;
    log_channel_noack[t] = 0;
    log_channel_rx[t] = 0;
    log_channel_rssi[t] = 0;
  }
#endif /* LOG_CHANNEL_ACTIVITY */

  /* Populate channel table incrementally.
   * This list should be configured with multichan_set_channel_list() */
  for(t = 0; t < NR_CHANNELS; t++) {
    time_channel_table[t] = t + CHANNEL_OFFSET;
  }

  process_start(&channel_process, NULL);
  process_start(&send_reply_process, NULL);
  on();
}
/*---------------------------------------------------------------------------*/
const struct rdc_driver multichan_driver = {
    "multichan",
    init,
    send_packet,
    send_list,
    packet_input,
    on,
    off,
    channel_check_interval,
};
/*---------------------------------------------------------------------------*/
