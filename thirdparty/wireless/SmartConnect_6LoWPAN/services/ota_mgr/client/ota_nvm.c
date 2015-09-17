
#include "string.h"
#include "ota_nvm.h"
#include "ota.h"
#include "nvm.h"
#include "ota_mgr.h"
#include "common_nvm.h"
#include "delay.h"

#define OFFSET_ADDRESS    (0x1EFE0UL)

uint8_t buff[100];

void ota_nvm_init(void)
{
	nvm_init(INT_FLASH);
	//uint16_t index;
	//uint32_t addr = OFFSET_ADDRESS;
	//for (index = 0; index < 100; index++)
	//{
		//memset(buff, index, 100);
		//HAL_FlashWrite(addr, buff, 100);
		//addr += 100;
	//}
}

void ota_nvm_write(address_type_t addr_type, uint32_t addr, uint16_t size, uint8_t *content)
{
	if (MEMORY_OFFSET_ADDRESS == addr_type)
	{
		addr += OFFSET_ADDRESS;
	}
	else if (MEMORY_NODE_INFO_ADDRESS == addr_type)
	{
		addr += NVM_DEVICE_INFO_LOCATION;
	}
	nvm_write(INT_FLASH,addr, content, size);
	delay_ms(50);
}

void ota_nvm_read(address_type_t addr_type, uint32_t addr, uint16_t size, uint8_t *content)
{
	if (MEMORY_OFFSET_ADDRESS == addr_type)
	{
		addr += OFFSET_ADDRESS;
	}
	else if (MEMORY_NODE_INFO_ADDRESS == addr_type)
	{
		addr += NVM_DEVICE_INFO_LOCATION;
	}
	memcpy(content, (uint8_t *)addr, size);
}

void ota_nvm_swap_image(address_type_t addr_type, uint32_t start_addr, uint32_t size)
{
	//if (UPGRADE == mode_code)
	{
		//nvm_page_swap(INT_FLASH, start_addr, size);
	}
}
