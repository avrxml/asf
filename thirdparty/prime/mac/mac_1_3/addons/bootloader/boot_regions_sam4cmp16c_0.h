/**
 * \file
 *
 * \brief Bootloader configuration for SAM4CMP16C_0.
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
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

#ifndef BOOT_REGIONS_SAM4CMP_H_INCLUDED
#define BOOT_REGIONS_SAM4CMP_H_INCLUDED

#include "compiler.h"

/* Bootloader regions */
#define BOOT_REGIONS                        2

/* Bootloader address */
#define BOOT_START_ADDRESS                  IFLASH_CNC_ADDR
/* Bootloader configuration */
#define BOOT_FIRMWARE_MAX_SIZE              0x00004000
#define BOOT_STATUS_OFFSET_USER_SIGN        128
#define BOOT_WD_MODE_OFFSET_USER_SIGN               0x108

/* Firmware configuration */
#define BOOT_FLASH_FIRMWARE_MAX_SIZE             0x00050000
#define BOOT_FLASH_PAGE_SIZE                     IFLASH_PAGE_SIZE
#define BOOT_FLASH_PAGES_PER_SECTOR              128
#define BOOT_FLASH_SECTOR_SIZE                   (BOOT_FLASH_PAGES_PER_SECTOR *	\
	BOOT_FLASH_PAGE_SIZE)
#define BOOT_FLASH_FIRMWARE_MAX_SIZE_IN_SECTORS  (BOOT_FLASH_FIRMWARE_MAX_SIZE / \
	BOOT_FLASH_SECTOR_SIZE)

/* Region configuration */
#define BOOT_FIRST_SECTOR_START_ADDRESS          (IFLASH_CNC_ADDR + 0x00010000)
#define BOOT_FLASH_EXEC_FIRMWARE_START_ADDRESS   (IFLASH_CNC_ADDR + 0x00010000)
#define BOOT_FLASH_EXEC_FIRMWARE_RESET_ADDRESS   (BOOT_FLASH_EXEC_FIRMWARE_START_ADDRESS + 4)
#define BOOT_FLASH_FIRMWARE_V1_START_ADDRESS     (BOOT_FLASH_EXEC_FIRMWARE_START_ADDRESS + \
	BOOT_FLASH_FIRMWARE_MAX_SIZE)
#define BOOT_FLASH_FIRMWARE_V1_RESET_ADDRESS     (BOOT_FLASH_FIRMWARE_V1_START_ADDRESS + 4)
#define BOOT_FLASH_FIRMWARE_V2_START_ADDRESS     (BOOT_FLASH_FIRMWARE_V1_START_ADDRESS + \
	BOOT_FLASH_FIRMWARE_MAX_SIZE)
#define BOOT_FLASH_FIRMWARE_V2_RESET_ADDRESS     (BOOT_FLASH_FIRMWARE_V2_START_ADDRESS + 4)

/* Bootloader register flags */
#define BOOT_CMD_SWAP_ENABLE         0x01
#define BOOT_FW_V1_IN_USE_MSK        0x02
#define BOOT_FW_V2_IN_USE_MSK        0x04

/* Bootloader values */
typedef enum {
	BOOT_FLASH_FW_VERSION_EXEC,
	BOOT_FLASH_FW_VERSION_1,
	BOOT_FLASH_FW_VERSION_2,
	BOOT_FLASH_FW_VERSION_INVALID
} boot_flash_fw_version_t;

/**
 * \brief Get FW V1 in use
 *
 */
static inline uint8_t boot_is_fw_v1_in_use(uint8_t uc_boot_status)
{
	return(uc_boot_status & BOOT_FW_V1_IN_USE_MSK);
}

/**
 * \brief Get FW V2 in use
 *
 */
static inline uint8_t boot_is_fw_v2_in_use(uint8_t uc_boot_status)
{
	return(uc_boot_status & BOOT_FW_V2_IN_USE_MSK);
}

/**
 * \brief Get status of swap command
 *
 */
static inline uint8_t boot_is_swap_cmd(uint8_t uc_boot_status)
{
	return(uc_boot_status & BOOT_CMD_SWAP_ENABLE);
}

/**
 * \brief Get status error
 *
 */
static inline uint8_t boot_is_error(uint8_t uc_boot_status)
{
	if ((uc_boot_status & BOOT_FW_V1_IN_USE_MSK) &&
			(uc_boot_status & BOOT_FW_V2_IN_USE_MSK)) {
		return true;
	} else {
		return false;
	}
}

#endif /* BOOT_REGIONS_SAM4CMP_H_INCLUDED */
