/**
 * \addtogroup uip
 * @{
 */

/**
 * \defgroup uipdns uIP hostname resolver functions
 * @{
 *
 * The uIP DNS resolver functions are used to lookup a hostname and
 * map it to a numerical IP address. It maintains a list of resolved
 * hostnames that can be queried with the resolv_lookup()
 * function. New hostnames can be resolved using the resolv_query()
 * function.
 *
 * The event resolv_event_found is posted when a hostname has been
 * resolved. It is up to the receiving process to determine if the
 * correct hostname has been found by calling the resolv_lookup()
 * function with the hostname.
 */

/**
 * \file
 * DNS host name to IP address resolver.
 * \author Adam Dunkels <adam@dunkels.com>
 *
 * This file implements a DNS host name to IP address resolver.
 */

/*
 * Copyright (c) 2002-2003, Adam Dunkels.
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
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: resolv.c,v 1.10 2010/10/19 18:29:04 adamdunkels Exp $
 *
 */

#include "net/ip/tcpip.h"
#include "net/ip/resolv.h"
#include "net/ip/uip-udp-packet.h"

#include "ip64-addr.h"
#include "compiler.h"
#include "mdns.h"
#define VERBOSE_DEBUG 0

#define WITH_EDNS_LIMIT_UDP_RES_PAYLOAD 1

#if UIP_UDP

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define MDNS_CONF_SUPPORTS_MDNS  0
#define MDNS_CONF_MDNS_RESPONDER 0

#ifndef NULL
#define NULL (void *)0
#endif /* NULL */

// If MDNS_CONF_SUPPORTS_MDNS is set, then queries
// for domain names in the local TLD will use mDNS as
// described by draft-cheshire-dnsext-multicastdns.
#ifndef MDNS_CONF_SUPPORTS_MDNS
#define MDNS_CONF_SUPPORTS_MDNS (1)
#endif

#ifndef MDNS_CONF_MDNS_RESPONDER
#define MDNS_CONF_MDNS_RESPONDER MDNS_CONF_SUPPORTS_MDNS
#endif

#ifndef MDNS_CONF_MDNS_INCLUDE_GLOBAL_V6_ADDRS
#define MDNS_CONF_MDNS_INCLUDE_GLOBAL_V6_ADDRS (0)
#endif

/** \internal The maximum number of retries when asking for a name. */
#ifndef MDNS_CONF_MAX_RETRIES
#define MDNS_CONF_MAX_RETRIES (8)
#endif

#ifndef MDNS_CONF_MAX_MDNS_RETRIES
#define MDNS_CONF_MAX_MDNS_RETRIES (3)
#endif

#ifndef MDNS_CONF_MAX_DOMAIN_NAME_SIZE
#define MDNS_CONF_MAX_DOMAIN_NAME_SIZE (32)
#endif

#if UIP_CONF_IPV6 && MDNS_CONF_MDNS_RESPONDER
#include "net/ipv6/uip-ds6.h"
#endif

/** \internal The DNS message header. */
struct dns_hdr {
  uint16_t id;
  uint8_t flags1, flags2;
#define DNS_FLAG1_RESPONSE        0x80
#define DNS_FLAG1_OPCODE_STATUS   0x10
#define DNS_FLAG1_OPCODE_INVERSE  0x08
#define DNS_FLAG1_OPCODE_STANDARD 0x00
#define DNS_FLAG1_AUTHORATIVE     0x04
#define DNS_FLAG1_TRUNC           0x02
#define DNS_FLAG1_RD              0x01
#define DNS_FLAG2_RA              0x80
#define DNS_FLAG2_ERR_MASK        0x0f
#define DNS_FLAG2_ERR_NONE        0x00
#define DNS_FLAG2_ERR_NAME        0x03
  uint16_t numquestions;
  uint16_t numanswers;
  uint16_t numauthrr;
  uint16_t numextrarr;
};

#if 1
#define MDNS_ENCODE_INDEX(i)		(uip_htons(i + 61616))
#define MDNS_DECODE_INDEX(i)		(unsigned char)(uip_ntohs(i) - 61616)
#else // The following versions are useful for debugging.
#define MDNS_ENCODE_INDEX(i)		(uip_htons(i))
#define MDNS_DECODE_INDEX(i)		(unsigned char)(uip_ntohs(i))
#endif

/** \internal The DNS answer message structure. */
COMPILER_PACK_SET(1)
struct dns_answer {
  /* DNS answer record starts with either a domain name or a pointer
     to a name already present somewhere in the packet. */
  uint16_t type;
  uint16_t class;
  uint16_t ttl[2];
  uint16_t len;
#if UIP_CONF_IPV6
  uint8_t ipaddr[16];
#else
  uint8_t ipaddr[4];
#endif
};
COMPILER_PACK_RESET()
#if MDNS_CONF_MDNS_RESPONDER
/** \internal The DNS question message structure. */
COMPILER_PACK_SET(1)
struct  dns_question {
  uint16_t type;
  uint16_t class;
};

#endif
COMPILER_PACK_RESET()
static uip_ipaddr_t dns_server;
static uint8_t dns_server_configured = 0;

#define DNS_TYPE_A		(1)
#define DNS_TYPE_CNAME	(5)
#define DNS_TYPE_PTR	(12)
#define DNS_TYPE_MX		(15)
#define DNS_TYPE_TXT	(16)
//#define DNS_TYPE_AAAA	(28)
#define DNS_TYPE_AAAA	(1) /* XXX temporarily fake A records */
#define DNS_TYPE_SRV	(33)
#define DNS_TYPE_ANY	(255)

#define DNS_CLASS_IN	(1)
#define DNS_CLASS_ANY	(255)

#ifndef DNS_PORT
#define DNS_PORT	(53)
#endif

#ifndef MDNS_PORT
#define MDNS_PORT	(5353)
#endif

#ifndef MDNS_RESPONDER_PORT
#define MDNS_RESPONDER_PORT	(5354)
#endif

#ifndef CONTIKI_CONF_DEFAULT_HOSTNAME
#define CONTIKI_CONF_DEFAULT_HOSTNAME	"contiki"
#endif

#if MDNS_CONF_MDNS_RESPONDER
static char mdns_hostname[MDNS_CONF_MAX_DOMAIN_NAME_SIZE + 1] =
  CONTIKI_CONF_DEFAULT_HOSTNAME;
#endif

#if 1 // MDNS_CONF_SUPPORTS_MDNS
#if UIP_CONF_IPV6
static const uip_ipaddr_t mdns_mdns_addr = {
  .u8 = {
         0xff, 0x02, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0xfb,
         }
};
#else
static const uip_ipaddr_t mdns_mdns_addr = {
  .u8 = {224, 0, 0, 251}
};
#endif
#endif

struct namemap {
#define STATE_UNUSED 0
#define STATE_NEW    1
#define STATE_ASKING 2
#define STATE_DONE   3
#define STATE_ERROR  4
  uint8_t state;
  uint8_t tmr;
  uint8_t retries;
  uint8_t seqno;
  uint8_t err;
  uint8_t is_mdns;
  char name[MDNS_CONF_MAX_DOMAIN_NAME_SIZE + 1];
  uip_ipaddr_t ipaddr;
};

#ifndef MDNS_CONF_MDNS_ENTRIES
#define MDNS_ENTRIES 4
#else /* MDNS_CONF_MDNS_ENTRIES */
#define MDNS_ENTRIES UIP_CONF_MDNS_ENTRIES
#endif /* MDNS_CONF_MDNS_ENTRIES */


static struct namemap names[MDNS_ENTRIES];

static uint8_t seqno;

static struct uip_udp_conn *mdns_conn = NULL;

static struct etimer retry;

process_event_t mdns_event_found;

PROCESS(mdns_process, "DNS mdnser");

static void mdns_found(char *name, uip_ipaddr_t *ipaddr);

/*-----------------------------------------------------------------------------------*/
/** \internal
 * Walk through a compact encoded DNS name and return the end of it.
 *
 * \return The end of the name.
 */
/*-----------------------------------------------------------------------------------*/
static unsigned char *
parse_name(unsigned char *query)
{
  unsigned char n;

#if VERBOSE_DEBUG
  printf("mdnser: Parsing name: ");
#endif

  do {
    n = *query;
    if(n & 0xc0) {
      *query++ = 0;
      break;
    }

    *query++ = '.';

    while(n > 0) {
#if VERBOSE_DEBUG
      printf("%c", *query);
#endif
      ++query;
      --n;
    };
#if VERBOSE_DEBUG
    printf(".");
#endif
  } while(*query != 0);
#if VERBOSE_DEBUG
  printf("\n");
#endif
  return query + 1;
}

static unsigned char *
encode_name(unsigned char *query, char *nameptr)
{
  char *nptr;

  --nameptr;
  /* Convert hostname into suitable query format. */
  do {
    uint8_t n = 0;

    ++nameptr;
    nptr = (char *)query;
    ++query;
    for(n = 0; *nameptr != '.' && *nameptr != 0; ++nameptr) {
      *query = *nameptr;
      ++query;
      ++n;
    }
    *nptr = n;
  } while(*nameptr != 0);

  *query++ = 0;                 // End the the name.

  return query;
}


/*-----------------------------------------------------------------------------------*/
/** \internal
 * Runs through the list of names to see if there are any that have
 * not yet been queried and, if so, sends out a query.
 */
/*-----------------------------------------------------------------------------------*/
static void
check_entries(void)
{
  volatile uint8_t i;
  char *query;
  register struct dns_hdr *hdr;
  register struct namemap *namemapptr;

  for(i = 0; i < MDNS_ENTRIES; ++i) {
    namemapptr = &names[i];
    if(namemapptr->state == STATE_NEW || namemapptr->state == STATE_ASKING) {
      etimer_set(&retry, CLOCK_SECOND);
      if(namemapptr->state == STATE_ASKING) {
        if(--namemapptr->tmr == 0) {
#if MDNS_CONF_SUPPORTS_MDNS
          if(++namemapptr->retries ==
             (namemapptr->is_mdns ? MDNS_CONF_MAX_MDNS_RETRIES :
              MDNS_CONF_MAX_RETRIES))
#else
          if(++namemapptr->retries == MDNS_CONF_MAX_RETRIES)
#endif
          {
            namemapptr->state = STATE_ERROR;
            mdns_found(namemapptr->name, NULL);
            continue;
          }
          namemapptr->tmr = namemapptr->retries;
        } else {
          /*      printf("Timer %d\n", namemapptr->tmr); */
          /* Its timer has not run out, so we move on to next
             entry. */
          continue;
        }
      } else {
        namemapptr->state = STATE_ASKING;
        namemapptr->tmr = 1;
        namemapptr->retries = 0;
      }
      hdr = (struct dns_hdr *)uip_appdata;
      memset(hdr, 0, sizeof(struct dns_hdr));
      hdr->id = MDNS_ENCODE_INDEX(i);

      if(!namemapptr->is_mdns) {
        hdr->flags1 = DNS_FLAG1_RD;
      }
      hdr->numquestions = UIP_HTONS(1);
#if WITH_EDNS_LIMIT_UDP_RES_PAYLOAD
      hdr->numextrarr = UIP_HTONS(1); /* EDNS request */
#endif /* WITH_EDNS_LIMIT_UDP_RES_PAYLOAD */

      query = (char *)uip_appdata + sizeof(*hdr);
      query = (char *)encode_name((unsigned char *)query, namemapptr->name);
      {
#if UIP_CONF_IPV6
        *query++ = (int8_t)((DNS_TYPE_AAAA) >> 8);
        *query++ = (int8_t)((DNS_TYPE_AAAA));
#else
        *query++ = (int8_t)((DNS_TYPE_A) >> 8);
        *query++ = (int8_t)((DNS_TYPE_A));
#endif
        *query++ = (int8_t)((DNS_CLASS_IN) >> 8);
        *query++ = (int8_t)((DNS_CLASS_IN));
      }

#if WITH_EDNS_LIMIT_UDP_RES_PAYLOAD
      /* EDNS additional record */
      *query++ = (int8_t) 0x00; /* root */
      *query++ = (int8_t) 0x00; /* EDNS0 */
      *query++ = (int8_t) 0x29; /* EDNS0 */
      *query++ = (int8_t) 0x00; /* 150 bytes UDP payload */
      *query++ = (int8_t) 0x96; /* 150 bytes UDP payload */
      *query++ = (int8_t) 0x00; /* Higher bits */
      *query++ = (int8_t) 0x00; /* EDNS version 0 */
      *query++ = (int8_t) 0x00; /* Z */
      *query++ = (int8_t) 0x00; /* Z */
      *query++ = (int8_t) 0x00; /* Data len */
      *query++ = (int8_t) 0x00; /* Data len */
#endif /* WITH_EDNS_LIMIT_UDP_RES_PAYLOAD */

      if(MDNS_CONF_SUPPORTS_MDNS && namemapptr->is_mdns) {
        uip_udp_packet_sendto(mdns_conn,
                              uip_appdata,
                              (query - (char *)uip_appdata),
                              &mdns_mdns_addr, UIP_HTONS(MDNS_PORT));

#if VERBOSE_DEBUG
        printf("mdnser: (i=%d) Sent MDNS request for \"%s\".\n", i,
               namemapptr->name);
#endif /* VERBOSE_DEBUG */
      } else {
	if(dns_server_configured) {
	  uip_udp_packet_sendto(mdns_conn,
				uip_appdata,
				(query - (char *)uip_appdata),
				&dns_server, UIP_HTONS(DNS_PORT));
#if VERBOSE_DEBUG
	  printf("mdnser: (i=%d) Sent DNS request for \"%s\".\n", i,
		 namemapptr->name);
#endif /* VERBOSE_DEBUG */
	} else {
#if VERBOSE_DEBUG
	  printf("mdnser: no DNS server configured, (i=%d) no DNS request sent for \"%s\".\n", i,
		 namemapptr->name);
#endif /* VERBOSE_DEBUG */
	}
      }
      break;
    }
  }
}
/*-----------------------------------------------------------------------------------*/
/** \internal
 * Called when new UDP data arrives.
 */
/*-----------------------------------------------------------------------------------*/
static void
newdata(void)
{
  unsigned char *nameptr;
  struct dns_answer *ans;
  struct dns_hdr *hdr;
  static uint8_t nquestions, nanswers;
  static uint8_t i;
  register struct namemap *namemapptr;

  hdr = (struct dns_hdr *)uip_appdata;

  /* We only care about the question(s) and the answers. The authrr
     and the extrarr are simply discarded. */
  nquestions = (uint8_t) uip_htons(hdr->numquestions);
  nanswers = (uint8_t) uip_htons(hdr->numanswers);

#if VERBOSE_DEBUG
  printf("mdnser: nquestions=%d, nanswers=%d\n", nquestions, nanswers);
#endif

  if((hdr->flags1 == 0) && (hdr->flags2 == 0)) {
    // This is an DNS request!
#if MDNS_CONF_MDNS_RESPONDER
    {
      struct dns_question *question;

      if(!nquestions) {
        // Query with no questions...?
        return;
      }
      if(nanswers) {
        // Skip queries with answers for now, even though they are valid.
        return;
      }
      nameptr = (char *)hdr + sizeof(*hdr);

      i = 0;

      char *name = 0;

      while(nquestions > 0) {
        if(*nameptr & 0xc0) {
          /* Compressed name...? Does this even make sense? */
          nameptr += 2;
        } else {
          /* Not compressed name. */
          name = (char *)nameptr + 1;
          nameptr = parse_name((uint8_t *) nameptr);
        }
        question = (struct dns_question *)nameptr;

#if VERBOSE_DEBUG
        printf("mdnser: Question %d: \"%s\" type=%d class=%d\n", ++i, name,
               uip_htons(question->type), uip_htons(question->class));
#endif
        nameptr += sizeof(struct dns_question);
        nquestions--;

        if(((uip_ntohs(question->class) & 0x7FFF) != DNS_CLASS_IN)
           || ((question->type != UIP_HTONS(DNS_TYPE_ANY))
#if UIP_CONF_IPV6
               && (question->type != UIP_HTONS(DNS_TYPE_AAAA))
#else
               && (question->type != UIP_HTONS(DNS_TYPE_A))
#endif
           )) {
          continue;             // Skipit.
        }

        if(0 != strncasecmp(mdns_hostname, name, strlen(mdns_hostname))) {
	  printf("skip 1\n");
          continue;             // Skipit.
	}

        if(0 != strcasecmp(name + strlen(mdns_hostname), ".local")) {
	  printf("skip 2\n");
          continue;             // Skipit.
	}

#if VERBOSE_DEBUG
        printf("mdnser: THIS IS A REQUEST FOR US!!!\n");
#endif
        hdr->flags1 |= DNS_FLAG1_RESPONSE | DNS_FLAG1_AUTHORATIVE;
        hdr->numquestions = UIP_HTONS(0);
        hdr->numauthrr = 0;
        hdr->numextrarr = 0;

        nameptr = (char *)hdr + sizeof(*hdr);

#if UIP_CONF_IPV6
        uint8_t acount = 0;

        for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
          if(uip_ds6_if.addr_list[i].isused
#if !MDNS_CONF_MDNS_INCLUDE_GLOBAL_V6_ADDRS
             && uip_is_addr_link_local(&uip_ds6_if.addr_list[i].ipaddr)
#endif
            ) {
            if(!acount) {
              nameptr = encode_name(nameptr, name);
            } else {
              *nameptr++ = 0xc0;
              *nameptr++ = sizeof(*hdr);
            }
            ans = (struct dns_answer *)nameptr;
            ans->type = UIP_HTONS(DNS_TYPE_AAAA);
            ans->class = UIP_HTONS(DNS_CLASS_IN | 0x8000);
            ans->ttl[0] = 0;
            ans->ttl[1] = UIP_HTONS(120);
            ans->len = UIP_HTONS(sizeof(uip_ipaddr_t));

            uip_ipaddr_copy((uip_ipaddr_t *)ans->ipaddr,
                            &uip_ds6_if.addr_list[i].ipaddr);
            nameptr = (char *)ans + sizeof(*ans);
            acount++;
          }
        }
        hdr->numanswers = uip_htons(acount);
#else
        hdr->numanswers = UIP_HTONS(1);
        nameptr = encode_name(nameptr, name);
        ans = (struct dns_answer *)nameptr;
        ans->type = UIP_HTONS(DNS_TYPE_A);
        ans->class = UIP_HTONS(DNS_CLASS_IN | 0x8000);
        ans->ttl[0] = 0;
        ans->ttl[1] = UIP_HTONS(120);
        ans->len = UIP_HTONS(sizeof(uip_ipaddr_t));
        uip_gethostaddr((uip_ipaddr_t *) ans->ipaddr);
        nameptr = (char *)ans + sizeof(*ans);
#endif

#define UIP_UDP_BUF                        ((struct uip_udpip_hdr *)&uip_buf[UIP_LLH_LEN])

#if MDNS_CONF_SUPPORTS_MDNS
        uip_udp_packet_sendto(mdns_conn,
                              uip_appdata,
                              (nameptr - (unsigned char *)uip_appdata),
                              UIP_UDP_BUF->srcport ==
                              UIP_HTONS(MDNS_PORT) ? &mdns_mdns_addr :
                              &UIP_UDP_BUF->srcipaddr, UIP_UDP_BUF->srcport);
#else /* MDNS_CONF_SUPPORTS_MDNS */
	uip_udp_packet_sendto(mdns_conn,
                              uip_appdata,
                              (nameptr - (unsigned char *)uip_appdata),
			      &UIP_UDP_BUF->srcipaddr, UIP_UDP_BUF->srcport);
#endif /* MDNS_CONF_SUPPORTS_MDNS */
        return;
      }
      return;
    }
#endif
    // Ignore other requests.
    return;
  }

  /* The ID in the DNS header should be our entry into the name
     table. */
  i = MDNS_DECODE_INDEX(hdr->id);
  if(i >= MDNS_ENTRIES) {
#if VERBOSE_DEBUG
    printf("mdnser: Bad ID (%04X) on incoming DNS response\n",
           uip_htons(hdr->id));
#endif
    return;
  }
  namemapptr = &names[i];

  if(i < MDNS_ENTRIES && namemapptr->state == STATE_ASKING) {

    if(!nanswers) {
      // Skipit.
      return;
    }

    /* This entry is now finished. */
    namemapptr->state = STATE_ERROR;    // We'll change this to DONE when we find the record.
    namemapptr->err = hdr->flags2 & DNS_FLAG2_ERR_MASK;

#if VERBOSE_DEBUG
    printf("mdnser: Incoming response for \"%s\" query.\n",
           namemapptr->name);
#endif

    /* Check for error. If so, call callback to inform. */
    if(namemapptr->err != 0) {
      namemapptr->state = STATE_ERROR;
      mdns_found(namemapptr->name, NULL);
      return;
    }

    nameptr = (unsigned char *)hdr + sizeof(*hdr);

    while(nquestions > 0) {
      if(*nameptr & 0xc0) {
        /* Compressed name. */
        nameptr += 2;
#if VERBOSE_DEBUG
        printf("mdnser: Compressed anwser\n");
#endif
      } else {
        /* Not compressed name. */
        /* Skip the name in the question. XXX: This should really be
           checked agains the name in the question, to be sure that they
           match. */
        nameptr = parse_name((uint8_t *) nameptr);
      }
      nameptr += 4;             // Skip past question data
      nquestions--;
    }

    while(nanswers > 0) {
      struct dns_answer anscopy;

      /* The first byte in the answer resource record determines if it
         is a compressed record or a normal one. */
      if(*nameptr & 0xc0) {
        /* Compressed name. */
        nameptr += 2;
#if VERBOSE_DEBUG
        printf("mdnser: Compressed anwser\n");
#endif
      } else {
        /* Not compressed name. */
        /* Skip the name in the question. XXX: This should really be
           checked agains the name in the question, to be sure that they
           match. */
        nameptr = parse_name((uint8_t *)nameptr);
      }

      //      ans = (struct dns_answer *)nameptr;

      memcpy(&anscopy, nameptr, sizeof(struct dns_answer));
      ans = &anscopy;

#if VERBOSE_DEBUG
      printf("mdnser: Answer: type %d, class %d, ttl %d, length %d\n",
             uip_ntohs(ans->type), uip_ntohs(ans->class) & 0x7FFF,
             (int)((uint32_t) uip_ntohs(ans->ttl[0])
                   << 16) | (uint32_t) uip_ntohs(ans->ttl[1]),
             uip_ntohs(ans->len));
#endif


      /* Check for IP address type and Internet class. Others are
         discarded. */

#if 0 // XXX pretend A entries UIP_CONF_IPV6
      if((ans->type == UIP_HTONS(DNS_TYPE_AAAA))
         && ((uip_ntohs(ans->class) & 0x7FFF) == DNS_CLASS_IN)
         && (ans->len == UIP_HTONS(16)))
#else // UIP_CONF_IPV6
      if(ans->type == UIP_HTONS(DNS_TYPE_A)
         && ((uip_ntohs(ans->class) & 0x7FFF) == DNS_CLASS_IN)
         && (ans->len == UIP_HTONS(4)))
#endif
      {
#if VERBOSE_DEBUG
        printf("mdnser: Answer is usable.\n");
#endif
        namemapptr->state = STATE_DONE;
	/* XXX translate IPv4 address to IPv6 address with ip64_addr_4to6 */
	uip_ip6addr_t ip6addr;
	ip64_addr_4to6((uip_ip4addr_t *)ans->ipaddr, &ip6addr);
	uip_ipaddr_copy(&namemapptr->ipaddr, &ip6addr);
	/*        uip_ipaddr_copy(&namemapptr->ipaddr, (uip_ipaddr_t *) ans->ipaddr);*/
        mdns_found(namemapptr->name, &namemapptr->ipaddr);
        return;
      } else {
#if VERBOSE_DEBUG
        printf("mdnser: Answer not usable.\n");
#endif
        // Keep looking.
        nameptr = nameptr + 10 + uip_htons(ans->len);
      }
      --nanswers;
    }
  } else {
#if VERBOSE_DEBUG
    printf("mdnser: Bad ID (%04X) on incoming DNS response\n",
           uip_htons(hdr->id));
#endif
  }
}

#if MDNS_CONF_MDNS_RESPONDER
#define MDNS_EVENT_START_COLLISION_CHECK		(0xF0)
void
start_name_collision_check()
{
  process_post(&mdns_process, MDNS_EVENT_START_COLLISION_CHECK, 0);
}

void
mdns_set_hostname(const char *hostname)
{
  strncpy(mdns_hostname, hostname, MDNS_CONF_MAX_DOMAIN_NAME_SIZE);

  start_name_collision_check();
}

const char *
mdns_get_hostname(void)
{
  return mdns_hostname;
}
#endif


/*-----------------------------------------------------------------------------------*/
void
mdns_init(void)
{
  process_start(&mdns_process, NULL);
}
/*-----------------------------------------------------------------------------------*/
PROCESS_THREAD(mdns_process, ev, data)
{
  PROCESS_BEGIN();

  memset(names, 0, sizeof(names));

  mdns_event_found = process_alloc_event();

#if VERBOSE_DEBUG
  printf("mdnser: Process started.\n");
#if MDNS_CONF_SUPPORTS_MDNS
  printf("mdnser: Supports MDNS name resolution.\n");
#endif
#endif

#if MDNS_CONF_MDNS_RESPONDER
  mdns_conn = udp_new(NULL, 0, NULL);
  uip_udp_bind(mdns_conn, UIP_HTONS(MDNS_PORT));
  mdns_conn->rport = 0;
  start_name_collision_check();

#if UIP_CONF_IPV6
  uip_ds6_maddr_add(&mdns_mdns_addr);
#endif /* UIP_CONF_IPV6 */
#else
  mdns_conn = udp_new(NULL, 0, NULL);
  mdns_conn->rport = 0;
#endif


  while(1) {
    PROCESS_WAIT_EVENT();

    if(ev == PROCESS_EVENT_TIMER) {
      if(mdns_conn != NULL) {
        tcpip_poll_udp(mdns_conn);
      }

    } else if(ev == tcpip_event) {
      if((uip_udp_conn == mdns_conn)) {
        if(uip_newdata()) {
          newdata();
        }
        if(uip_poll()) {
          check_entries();
        }
      }
#if MDNS_CONF_MDNS_RESPONDER
    } else if(ev == MDNS_EVENT_START_COLLISION_CHECK) {
      // TODO: implement this better!
      char full_name[strlen(mdns_hostname) + sizeof(".local.")];

      strcpy(full_name, mdns_hostname);
      strcat(full_name, ".local.");
      mdns_query(full_name);
#endif
    }
  }

  PROCESS_END();
}

static char dns_name_without_dots[MDNS_CONF_MAX_DOMAIN_NAME_SIZE + 1];        // For removing trailing dots.

/*-----------------------------------------------------------------------------------*/
/**
 * Queues a name so that a question for the name will be sent out.
 *
 * \param name The hostname that is to be queried.
 */
/*-----------------------------------------------------------------------------------*/
void
mdns_query(const char *name)
{
  static uint8_t i;
  static uint8_t lseq, lseqi;
  register struct namemap *nameptr;

  lseq = lseqi = 0;
  nameptr = 0;                  //compiler warning if not initialized

  {                             // Remove trailing dots, if present.
    size_t len = strlen(name);

    if(name[len - 1] == '.') {
      strncpy(dns_name_without_dots, name, sizeof(dns_name_without_dots));
      while(len && (dns_name_without_dots[len - 1] == '.')) {
        dns_name_without_dots[--len] = 0;
      }
      name = dns_name_without_dots;
    }
  }

  for(i = 0; i < MDNS_ENTRIES; ++i) {
    nameptr = &names[i];
    if((nameptr->state == STATE_UNUSED) || (0 == strcmp(nameptr->name, name))) {
      break;
    }
    if(seqno - nameptr->seqno > lseq) {
      lseq = seqno - nameptr->seqno;
      lseqi = i;
    }
  }

  if(i == MDNS_ENTRIES) {
    i = lseqi;
    nameptr = &names[i];
  }
#if VERBOSE_DEBUG
  printf("mdnser: Starting query for \"%s\".\n", name);
#endif


#if MDNS_CONF_SUPPORTS_MDNS
  {
    size_t name_len = strlen(name);
    static const char local_suffix[] = "local";

    if((name_len > (sizeof(local_suffix) - 1)) &&
       (0 == strcmp(name + name_len - (sizeof(local_suffix) - 1),
		    local_suffix))) {
#if VERBOSE_DEBUG
      printf("mdnser: Using MDNS to look up \"%s\".\n", name);
#endif
      nameptr->is_mdns = 1;
    } else {
      nameptr->is_mdns = 0;
    }
  }
#endif

  strncpy(nameptr->name, name, sizeof(nameptr->name));
  nameptr->state = STATE_NEW;
  nameptr->seqno = seqno;
  ++seqno;

  // Force check_entires() to run on our process.
  process_post(&mdns_process, PROCESS_EVENT_TIMER, 0);
}
/*-----------------------------------------------------------------------------------*/
/**
 * Look up a hostname in the array of known hostnames.
 *
 * \note This function only looks in the internal array of known
 * hostnames, it does not send out a query for the hostname if none
 * was found. The function mdns_query() can be used to send a query
 * for a hostname.
 *
 * \return A pointer to a 4-byte representation of the hostname's IP
 * address, or NULL if the hostname was not found in the array of
 * hostnames.
 */
/*-----------------------------------------------------------------------------------*/
uip_ipaddr_t *
mdns_lookup(const char *name)
{
  static uint8_t i;
  struct namemap *nameptr;

  {                             // Remove trailing dots, if present.
    size_t len = strlen(name);

    if(name[len - 1] == '.') {
      strncpy(dns_name_without_dots, name, sizeof(dns_name_without_dots) - 1);
      name = dns_name_without_dots;
      while(len && (dns_name_without_dots[len - 1] == '.')) {
        dns_name_without_dots[--len] = 0;
      }
    }
  }

#if UIP_CONF_LOOPBACK_INTERFACE
  if(strcmp(name, "localhost")) {
#if UIP_CONF_IPV6
    static uip_ipaddr_t loopback = {
      .u8 = {
             0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x01,
             }
    };
#else
    static uip_ipaddr_t loopback = {
      .u8 = {127, 0, 0, 1}
    };
#endif
    return &loopback;
  }
#endif

  /* Walk through the list to see if the name is in there. If it is
     not, we return NULL. */
  for(i = 0; i < MDNS_ENTRIES; ++i) {
    nameptr = &names[i];
    if(nameptr->state == STATE_DONE && (strcmp(name, nameptr->name) == 0)) {
#if VERBOSE_DEBUG
      printf("mdnser: Found \"%s\" in cache.\n", name);
      uip_ipaddr_t *addr = &nameptr->ipaddr;

      printf
        ("mdnser: %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x \n",
         ((uint8_t *) addr)[0], ((uint8_t *) addr)[1], ((uint8_t *) addr)[2],
         ((uint8_t *) addr)[3], ((uint8_t *) addr)[4], ((uint8_t *) addr)[5],
         ((uint8_t *) addr)[6], ((uint8_t *) addr)[7], ((uint8_t *) addr)[8],
         ((uint8_t *) addr)[9], ((uint8_t *) addr)[10],
         ((uint8_t *) addr)[11], ((uint8_t *) addr)[12],
         ((uint8_t *) addr)[13], ((uint8_t *) addr)[14],
         ((uint8_t *) addr)[15]);
#endif
      return &nameptr->ipaddr;
    }
  }
#if VERBOSE_DEBUG
  printf("mdnser: \"%s\" is NOT cached.\n", name);
#endif
  return NULL;
}
/*-----------------------------------------------------------------------------------*/
/**
 * Obtain the currently configured DNS server.
 *
 * \return A pointer to a 4-byte representation of the IP address of
 * the currently configured DNS server or NULL if no DNS server has
 * been configured.
 */
/*-----------------------------------------------------------------------------------*/
uip_ipaddr_t *
mdns_getserver(void)
{
  if(mdns_conn == NULL) {
    return NULL;
  }
  return &dns_server;
//
//  return &mdns_conn->ripaddr;
}
/*-----------------------------------------------------------------------------------*/
/**
 * Configure a DNS server.
 *
 * \param dnsserver A pointer to a 4-byte representation of the IP
 * address of the DNS server to be configured.
 */
/*-----------------------------------------------------------------------------------*/
void
mdns_conf(const uip_ipaddr_t *dnsserver)
{
  if(dnsserver != NULL) {
    uip_ipaddr_copy(&dns_server, dnsserver);
    dns_server_configured = 1;
  } else {
    dns_server_configured = 0;
  }
}
/*-----------------------------------------------------------------------------------*/
/** \internal
 * Callback function which is called when a hostname is found.
 *
 */
/*-----------------------------------------------------------------------------------*/
static void
mdns_found(char *name, uip_ipaddr_t *ipaddr)
{
#if VERBOSE_DEBUG
  if(ipaddr) {
    printf("mdnser: Found address for \"%s\".\n", name);
    printf
      ("mdnser: %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x \n",
       ((uint8_t *)ipaddr)[0], ((uint8_t *)ipaddr)[1],
       ((uint8_t *)ipaddr)[2], ((uint8_t *)ipaddr)[3],
       ((uint8_t *)ipaddr)[4], ((uint8_t *)ipaddr)[5],
       ((uint8_t *)ipaddr)[6], ((uint8_t *)ipaddr)[7],
       ((uint8_t *)ipaddr)[8], ((uint8_t *)ipaddr)[9],
       ((uint8_t *)ipaddr)[10], ((uint8_t *)ipaddr)[11],
       ((uint8_t *)ipaddr)[12], ((uint8_t *)ipaddr)[13],
       ((uint8_t *)ipaddr)[14], ((uint8_t *)ipaddr)[15]);
  } else {
    printf("mdnser: Unable to retrieve address for \"%s\".\n", name);
  }
#endif

  process_post(PROCESS_BROADCAST, mdns_event_found, name);
}
/*-----------------------------------------------------------------------------------*/
#endif /* UIP_UDP */

/** @} */
/** @} */
