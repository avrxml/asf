#include "ip64.h"
#include "ip64-special-ports.h"

#ifndef IP64_SPECIAL_PORTS_CONF_ENABLE
#define EMPTY_DEFINITIONS 1
#else
#if IP64_SPECIAL_PORTS_CONF_ENABLE == 0
#define EMPTY_DEFINITIONS 1
#endif /* IP64_SPECIAL_PORTS_CONF_ENABLE */
#endif /* IP64_SPECIAL_PORTS_CONF_ENABLE */



#if EMPTY_DEFINITIONS
/*---------------------------------------------------------------------------*/
int
ip64_special_ports_translate_incoming(uint16_t incoming_port,
				      uip_ip6addr_t *newaddr,
				      uint16_t *newport)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
int
ip64_special_ports_translate_outgoing(uint16_t incoming_port,
				      const uip_ip6addr_t *ip6addr,
				      uint16_t *newport)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
int
ip64_special_ports_incoming_is_special(uint16_t port)
{
  /* Default is to have no special ports. */
  return 0;
}
/*---------------------------------------------------------------------------*/
int
ip64_special_ports_outgoing_is_special(uint16_t port)
{
  /* Default is to have no special ports. */
  return 0;
}
/*---------------------------------------------------------------------------*/
#endif /* EMPTY_DEFINITIONS */
