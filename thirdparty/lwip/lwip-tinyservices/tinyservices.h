/**
 *
 * \file
 *
 * \brief Tiny DNS Server.
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
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
 */

#ifndef TINYSERVICES_H_INCLUDED
#define TINYSERVICES_H_INCLUDED

#include "lwip/ip_addr.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"

/* DHCP options */
#define DHCP_OPTION_DOMAIN_NAME 15
#define DHCP_OPTION_VENDOR_SPECIFIC_INFO 43

/** Handle slightly smaller response than standard implementations */
#define DHCP_RESPONSE_DEFAULT_SIZE (1024)

/** DNS client port is set to 53000 */
#define DNS_CLIENT_PORT (53000)

#ifndef DNS_SERVER_PORT
#define DNS_SERVER_PORT (53)
#endif

#ifndef MDNS_PORT
#define MDNS_PORT (5353)
#endif

/** Default to slightly smaller DNS request than normal */
#define DNS_DEFAULT_REQUEST_LEN  (300)

/* DNS protocol flags */
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

#ifndef DNS_RRTYPE_A
#define DNS_RRTYPE_A              1     /* a host address */
#endif

#ifndef DNS_RRTYPE_NS
#define DNS_RRTYPE_NS             2     /* an authoritative name server */
#endif

#ifndef DNS_RRTYPE_CNAME
#define DNS_RRTYPE_CNAME          5     /* the canonical name for an alias */
#endif

#ifndef DNS_RRTYPE_PTR
#define DNS_RRTYPE_PTR            12    /* a domain name pointer */
#endif

#ifndef DNS_RRTYPE_SRV
#define DNS_RRTYPE_SRV            33    /* a service locator */
#endif

#ifndef DNS_RRTYPE_TXT
#define DNS_RRTYPE_TXT            16    /* arbitrary text - used by DNS-SD and others */
#endif

#ifndef DNS_RRCLASS_IN
#define DNS_RRCLASS_IN            1     /* internet class */
#endif

#define DNS_RTYPE_A_FLAG         (1           << 0)
#define DNS_RTYPE_NS_FLAG        (1           << 1)
#define DNS_RTYPE_PTR_FLAG       (1           << 2)
#define DNS_RTYPE_SRV_FLAG       (1           << 3)
#define DNS_RTYPE_TXT_FLAG       (1           << 4)


#define SIZEOF_DNS_HDR (12)
#define SIZEOF_MIN_DNS_QUERY (5)
#define SIZEOF_MIN_DNS_STR   (2)

#define MSG_COMPRESSION_FLAGS    (0xc0)
#define MSG_COMPRESSION_FLAGS16  (0xc000)
#define MSG_OFFSET_MASK          (0x3FF)
#define COMPRESSED_STR_SIZE      (2)
#define SIZEOF_TYPEFIELD    (2)
#define SIZEOF_CLASSFIELD   (2)
#define SIZEOF_LENFIELD     (2)
#define SIZEOF_TTLFIELD     (4)
#define SIZEOF_QDCOUNTFIELD (2)
#define SIZEOF_ANCOUNTFIELD (2)
#define SIZEOF_NSCOUNTFIELD (2)
#define SIZEOF_ARCOUNTFIELD (2)
#define SIZEOF_IPADDR       (4)
#define SIZEOF_PRIOFIELD    (2)
#define SIZEOF_WEIGHTFIELD  (2)
#define SIZEOF_PORTFIELD    (2)

#define SIZEOF_SRV_PRIORITYFIELD (2)
#define SIZEOF_SRV_WEIGHTFIELD   (2)
#define SIZEOF_SRV_PORTFIELD     (2)
#define SIZEOF_SRV_RDATA_HDR     (SIZEOF_SRV_PRIORITYFIELD  + SIZEOF_SRV_WEIGHTFIELD + SIZEOF_SRV_PORTFIELD)
#define DNS_TID_OFFSET      (0)
#define DNS_FLAGS_OFFSET    (2)
#define DNS_QDCOUNT_OFFSET  (4)
#define DNS_ANCOUNT_OFFSET  (6)
#define DNS_NSCOUNT_OFFSET  (8)
#define DNS_ARCOUNT_OFFSET (10)

/** Handle default DNS Response size */
#define DNS_RESPONSE_DEFAULT_SIZE (512)

#define TINY_DNS_CLIENT_TIMEOUT        (5000) /* default to 5 seconds */

#define MDNS_ADDRESS             224, 0, 0, 251
#define MDNS_ADDR_IP             _tiny_ip4_addr(MDNS_ADDRESS)

#define DNS_SRV_PRIORITY         0
#define DNS_SRV_WEIGHT           0

#define HTTP_SRV                 "_http"
#define SERVICES                 "_services"
#define DNS_SD                   "_dns-sd"
#define TCP_PROTO                "_tcp"
#define UDP_PROTO                "_udp"
#define LOCAL_DOMAIN             "local"

#define HTTP_SRV_RR              HTTP_SRV "." TCP_PROTO "." LOCAL_DOMAIN

#define HTTP_SRV_LOCAL_NAME_RR   HTTP_SRV_LOCAL_NAME "." HTTP_SRV "." TCP_PROTO "." LOCAL_DOMAIN
#define HTTP_SRV_SHORT_NAME_RR   HTTP_SRV_LOCAL_NAME "." LOCAL_DOMAIN

#define QUERY_STRING_LEN                   (30)

#define DNS_SD_RR                SERVICES "." DNS_SD "." UDP_PROTO "." LOCAL_DOMAIN
//_mfi-config._tcp.local
#define MFI_RR                   "_mfi-config." TCP_PROTO "." LOCAL_DOMAIN         

#define DNS_RRTYPE_AAAA          (28)

//smartconnect._mfi._tcp.local
#define MFI_SRV_LOCAL_NAME_RR    HTTP_SRV_LOCAL_NAME "." MFI_RR                       

typedef enum dns_transmission_type {
    DNS_MULTICAST,
    DNS_UNICAST
} dns_transmission_type_t;

typedef enum subqry {
    DNS_QRY_REVERSE_NAME_LOOKUP_FOR_LOCAL_IP,
    DNS_QRY_HTTP_LOCAL,
    DNS_QRY_DNS_SD_LOCAL,
	DNS_QRY_MFI_LOCAL,	
    DNS_QRY_UNKNOWN,
    DNS_QRY_NONE
} subqry_t;

struct label_t {
    char *   label;
    uint16_t location;
};

#if LWIP_UDP && LWIP_DHCP

/** Start Tiny DHCP server */
void lwip_tiny_dhcpserver_start(void);
/** Stop Tiny DHCP server */
void lwip_tiny_dhcpserver_stop(void);

void lwip_dhcp_register_mac(uint8_t *mac);
void lwip_dhcp_unregister_mac(uint8_t *mac);

/** Start Tiny DNS-SD server */
void lwip_tiny_dnssdserver_start(void);
void lwip_tiny_dnssdserver_stop(void);

#endif

/** Put host name into DNS RDATA format */
uint32_t lwip_dns_hton_string(struct pbuf * pbuf, uint32_t offset, char const * name, bool flag, uint32_t * plen);
uint32_t pbuf_read8(struct pbuf * pbuf, uint32_t offset, uint8_t * pval);
uint32_t pbuf_write8(struct pbuf * pbuf, uint32_t offset, uint8_t val);
uint32_t pbuf_write16be(struct pbuf * pbuf, uint32_t offset, uint16_t val);
uint32_t pbuf_read16be(struct pbuf * pbuf, uint32_t offset, uint16_t * pval);
uint32_t pbuf_write32le(struct pbuf * pbuf, uint32_t offset, uint32_t val);
uint32_t pbuf_write32be(struct pbuf * pbuf, uint32_t offset, uint32_t val);
uint32_t pbuf_read32le(struct pbuf * pbuf, uint32_t offset, uint32_t * pval);
err_t pbuf_take_partial(struct pbuf *buf, uint32_t offset, const void *dataptr, u16_t len);

static inline ip_addr_t _tiny_ip4_addr(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
	ip_addr_t r;
	IP4_ADDR(&r, a, b, c, d);
	return r;
}

static inline uint32_t pbuf_write_ip(struct pbuf *pbuf, uint32_t offset, ip_addr_t addr)
{
	return pbuf_write32le(pbuf, offset, addr.addr);
}

static inline uint32_t pbuf_read_ip(struct pbuf * pbuf, uint32_t offset, ip_addr_t * p_addr)
{
    return pbuf_read32le(pbuf, offset, (uint32_t *)(&(p_addr->addr)));
}

#endif /* TINYSERVICES_H_INCLUDED */
