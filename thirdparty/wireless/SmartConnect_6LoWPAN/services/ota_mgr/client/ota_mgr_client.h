
#ifndef OTA_MGR_CLIENT_H
#define OTA_MGR_CLIENT_H

#include "ota.h"
#include "ota_mgr.h"

extern node_address_t common_server_address;
extern node_address_t upgrade_server_address;

void configure_common_server_details(addr_mode_t addr_mode, uint8_t *addr);
void get_common_server_details(addr_mode_t addr_mode, uint8_t *addr);
void configure_upgrade_server_details(addr_mode_t addr_mode, const uint8_t *addr);
void get_upgrade_server_details(addr_mode_t addr_mode, uint8_t *addr);
uint8_t *get_parent_native_addr(void);
#endif /* OTA_MGR_CLIENT_H */