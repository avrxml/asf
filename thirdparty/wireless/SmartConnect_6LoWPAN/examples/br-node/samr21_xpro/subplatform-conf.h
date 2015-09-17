#ifndef SUBPLATFORM_CONF_H
#define SUBPLATFORM_CONF_H

#include "platform-conf.h"
/*---------------------------------------------------------------------------*/
/* 
 * Define the pin extension header on the SAM R21 board that the expansion
 * boards are connected to. On the SAM R21 X-plained board there are
 * three possible headers where to connect the boards. This define
 * sets which are used.
 */
/* Ethernet module */
#ifdef ETHERNET_HEADER_CONF
#define ETHERNET_HEADER     ETHERNET_HEADER_CONF
#else   /* ETHERNET_HEADER */
#define ETHERNET_HEADER     EXT_HEADER_1
#endif  /* ETHERNET_HEADER */

#if ETHERNET_HEADER == REB233XPRO_HEADER
#error "The extension header for the radio and the ethernet module are set to the same, but must not be. See __FILE__."
#endif /* ETHERNET_HEADER == REB233XPRO_HEADER */
/*---------------------------------------------------------------------------*/
/* Allow the device to join two multicast groups. */
#undef UIP_CONF_DS6_MADDR_NBU
#define UIP_CONF_DS6_MADDR_NBU                        2

#undef NBR_TABLE_CONF_MAX_NEIGHBORS
#define NBR_TABLE_CONF_MAX_NEIGHBORS                  20

#undef UIP_CONF_BUFFER_SIZE
#define UIP_CONF_BUFFER_SIZE                          800

#undef UIP_CONF_TCP_MSS
#define UIP_CONF_TCP_MSS                              700

#undef UIP_CONF_RECEIVE_WINDOW
#define UIP_CONF_RECEIVE_WINDOW                       700

// #undef NBR_TABLE_CONF_MAX_NEIGHBORS
// #define NBR_TABLE_CONF_MAX_NEIGHBORS         64

#undef UIP_CONF_MAX_ROUTES
#define UIP_CONF_MAX_ROUTES                           32 // 32

/* Make HTTP/TCP connection way faster. */
#undef UIP_CONF_TCP_SPLIT
#define UIP_CONF_TCP_SPLIT                            1
#undef UIP_SPLIT_CONF_SIZE
#define UIP_SPLIT_CONF_SIZE                           8

// #define NETSTACK_RADIO_MAX_PAYLOAD_LEN 125

#define NETSTACK_AES_KEY "thingsquare mist"

#undef UIP_FALLBACK_INTERFACE
#define UIP_FALLBACK_INTERFACE ip64_uip_fallback_interface

#define IP64_ADDRMAP_CONF_ENTRIES                     20 //32

/*#undef IEEE_ADDR_CONF_HARDCODED
#define IEEE_ADDR_CONF_HARDCODED             1

#undef IEEE_ADDR_CONF_IN_FLASH
#define IEEE_ADDR_CONF_IN_FLASH              0*/

#endif /* SUBPLATFORM_CONF_H */
