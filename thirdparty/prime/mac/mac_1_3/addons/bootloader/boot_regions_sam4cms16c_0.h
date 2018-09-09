/**
 * \file
 *
 * \brief Bootloader configuration for SAM4CMS16C_0.
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */

#ifndef BOOT_REGIONS_SAM4CMS_H_INCLUDED
#define BOOT_REGIONS_SAM4CMS_H_INCLUDED

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

#endif /* BOOT_REGIONS_SAM4CMS_H_INCLUDED */
