



#ifndef OTA_NVM_H
#define OTA_NVM_H

#include "ota.h"

void ota_nvm_init(void);
void ota_nvm_write(address_type_t addr_type, uint32_t addr, uint16_t size, uint8_t *content);
void ota_nvm_read(address_type_t addr_type, uint32_t addr, uint16_t size, uint8_t *content);
void ota_nvm_swap_image(address_type_t addr_type, uint32_t start_addr, uint32_t size);

#endif /* OTA_NVM_H */