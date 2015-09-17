

#ifndef IP64_CONF_SUB_H
#define IP64_CONF_SUB_H

#include "ip64-eth-interface.h"
#define DISABLE_DHCP 0
#undef UIP_FALLBACK_INTERFACE
#define UIP_FALLBACK_INTERFACE            ip64_eth_interface
#define IP64_CONF_UIP_FALLBACK_INTERFACE  ip64_eth_interface
#define IP64_CONF_INPUT                   ip64_eth_interface_input
#define IP64_CONF_DHCP                    1
//#if (DISABLE_DHCP == 0)
//#define IP64_CONF_DHCP                    1
//#define IP64_CONF_STATIC                  0
//#else
//#define IP64_CONF_DHCP                    0
//#define IP64_CONF_STATIC                  1
//#define STATIC_IPADDR                     "10.10.1.2"
//#define STATIC_NETMASK                    "255.0.0.0"
//#define STATIC_DEFAULT_ROUTER             "10.10.1.2" // currently not being used
//#endif

#include "ksz8851snl-ip64-driver.h"

#define IP64_CONF_ETH_DRIVER              ksz8851snl_ip64_driver

#endif /* IP64_CONF_SUB_H */


