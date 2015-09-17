
#include "compiler.h"
#include "string.h"

#include "ota_mgr_client.h"
#include "uip.h"
#include "uip-ds6-route.h"

node_address_t common_server_address;

node_address_t upgrade_server_address;

void configure_common_server_details(addr_mode_t addr_mode, uint8_t *addr)
{
	if (NATIVE_ADDR_MODE == addr_mode)
	{
		memcpy(&common_server_address.native_addr, addr, NATIVE_ADDR_SIZE);
	}
	else if (EXTENDED_ADDR_MODE == addr_mode)
	{
		memcpy(&common_server_address.extended_addr, addr, EXTENDED_ADDR_SIZE);
	}
}


void configure_upgrade_server_details(addr_mode_t addr_mode, const uint8_t *addr)
{
	if (NATIVE_ADDR_MODE == addr_mode)
	{
		memcpy(&upgrade_server_address.native_addr, addr, NATIVE_ADDR_SIZE);
	}
	else if (EXTENDED_ADDR_MODE == addr_mode)
	{
		memcpy(&upgrade_server_address.extended_addr, addr, EXTENDED_ADDR_SIZE);
	}
}

void get_common_server_details(addr_mode_t addr_mode, uint8_t *addr)
{
	if (NATIVE_ADDR_MODE == addr_mode)
	{
		addr = (uint8_t *)&common_server_address.native_addr;
	}
	else if (EXTENDED_ADDR_MODE == addr_mode)
	{
		addr = (uint8_t *)&common_server_address.extended_addr;
	}
}

void get_upgrade_server_details(addr_mode_t addr_mode, uint8_t *addr)
{
	if (NATIVE_ADDR_MODE == addr_mode)
	{
		addr = (uint8_t *)&upgrade_server_address.native_addr;
	}
	else if (EXTENDED_ADDR_MODE == addr_mode)
	{
		addr = (uint8_t *)&upgrade_server_address.extended_addr;
	}
}


uint8_t *get_parent_native_addr(void)
{
	uip_ipaddr_t *ipaddr;
	ipaddr = uip_ds6_defrt_choose();
	return (uint8_t*) &ipaddr->u16[4];
}
