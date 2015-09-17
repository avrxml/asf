#ifndef IP64_SPECIAL_PORTS_H
#define IP64_SPECIAL_PORTS_H

/* The IP64 special ports module allows specific ports on the IP64
   translator to be mapped to specific address in the IPv6
   network. The module provides three function prototypes that must be
   implemented by the user.

   The IP64 special ports module must be enabled by

#define IP64_SPECIAL_PORTS_CONF_ENABLE 1

   Otherwise, the functions are replaced by empty default definitions
   in the ip64-special-ports.c module.

   Port numbers are always in network byte order. */


/* Translate the special port to an IPv6 address for inbound
   packets. */
int ip64_special_ports_translate_incoming(uint16_t incoming_port,
					  uip_ip6addr_t *newaddr,
					  uint16_t *newport);
int ip64_special_ports_translate_outgoing(uint16_t incoming_port,
					  const uip_ip6addr_t *ip6addr,
					  uint16_t *newport);
/* Check if an incoming (destination) port is special. */
int ip64_special_ports_incoming_is_special(uint16_t port);

/* Check if an outgoing (source) port is special. */
int ip64_special_ports_outgoing_is_special(uint16_t port);


#endif /* IP64_SPECIAL_PORTS_H */
