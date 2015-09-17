/**
 * \file
 *
 * \brief PRIME Hardware Abstraction Layer
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef HAL_H_INCLUDED
#define HAL_H_INCLUDED

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/* @endcond */

/**
 * \ingroup prime_group
 * \defgroup hal_prime_group PRIME Hardware Abstraction Layer
 *
 * This module provides configuration and utils for PLC HAL layer.
 *
 * @{
 */

#include "compiler.h"

#if SAM4CP
#include "boot_regions_sam4cp16b_0.h"
#elif SAM4CMP16_0
#include "boot_regions_sam4cmp16c_0.h"
#elif SAM4CMS16_0
#include "boot_regions_sam4cms16c_0.h"
#elif SAM4C
#include "boot_regions_sam4c16c_0.h"
#else
#  error No known Atmel PLC boot defined
#endif

/* \name Length of MAC Address */
/* @{ */
#define MAC_LENGTH      6
/* @} */

/* \name Configuration parameters */
/* @{ */
#define HAL_USER_SIGNATURE_SIZE                (BOOT_FLASH_PAGE_SIZE / (sizeof(uint32_t)))
#define HAL_MACCFG_OFFSET_USER_SIGN            0
#define HAL_PHYCFG_OFFSET_USER_SIGN            16
#define HAL_BOOT_STATUS_OFFSET_USER_SIGN       BOOT_STATUS_OFFSET_USER_SIGN
/* @} */

/* \name Firmware Upgrade: configuration parameters */
/* @{ */
#define HAL_FU_MAXIMUM_FIRMW_SIZE      BOOT_FLASH_FIRMWARE_MAX_SIZE
#define HAL_FU_MAXIMUM_FIRMW_SECTORS   BOOT_FLASH_FIRMWARE_MAX_SIZE_IN_SECTORS
#define HAL_ADDR_START_SERVICE         BOOT_FLASH_EXEC_FIRMWARE_START_ADDRESS

#define HAL_BOOT_CMD_SWAP              BOOT_CMD_SWAP_ENABLE
#define HAL_BOOT_FW_V1_IN_USE_MSK      BOOT_FW_V1_IN_USE_MSK
#define HAL_BOOT_FW_V2_IN_USE_MSK      BOOT_FW_V2_IN_USE_MSK

#define HAL_BOOT_FW1                   BOOT_FLASH_FIRMWARE_V1_START_ADDRESS
#define HAL_BOOT_FW2                   BOOT_FLASH_FIRMWARE_V2_START_ADDRESS

#define HAL_FLASH_PAGE_SIZE            BOOT_FLASH_PAGE_SIZE
/* @} */

/* \name Firmware Information List Configuration */
/* @{ */
#define HAL_FW_INFO_ID_LIST_MAX_SIZE      32
#define HAL_FW_INFO_NUMBER_ID             3
/* @} */

/* \name Base node table space in flash */
/* @{ */
#define HAL_BASE_NODE_TABLE_INIT          0xF000
/* @} */

/* \name Bootloader watchdog mode */
/* @{ */
#define HAL_BOOTLOADER_WD_MODE            0xA1   /* Internal watchdog enabled */
/* @} */

/* \name Flash memory states */
/* @{ */
#define HAL_FW_FLASH_MEM_ENABLED          0xA5000001
#define HAL_FW_FLASH_MEM_DISABLED         0xA5000000
/* @} */

/*! \name Firmware Information List  */
/* @{ */
typedef struct {
	uint8_t uc_info_id;
	uint8_t uc_len_id;
	char c_data_id[HAL_FW_INFO_ID_LIST_MAX_SIZE];
} x_fw_info_list_t;
/* @} */

/*! \name Information of PRIME version */
/* @{ */
#define HAL_PRIME_VERSION_SIZE           32
typedef struct _hal_sw_version_info {
	uint16_t us_vendor_id;
	uint16_t us_product_id;
	uint8_t uc_version_len;
	char c_fw_version[HAL_PRIME_VERSION_SIZE];
} hal_sw_version_info;
/* @} */

/* \name Hardware Abstraction Layer Interface */
/* @{ */
void hal_init(void);
void hal_start(void);
void hal_mtp_signalling(void);
void hal_reset_plc_modem(void);
void hal_restart_system(uint8_t err_type);
void hal_read_mac_data_cfg(void *pv_dst, uint16_t us_size);
uint8_t hal_write_mac_data_cfg(void *pv_src, uint16_t us_size);
void hal_read_phy_data_cfg(void *pv_dst, uint16_t us_size);
uint8_t hal_write_phy_data_cfg(void *pv_src, uint16_t us_size);
void hal_read_boot_cfg(void *pv_dst, uint16_t us_size);
uint8_t hal_write_boot_cfg(void *pv_src, uint16_t us_size);
void hal_read_fu_data_cfg(void *pv_dst, uint16_t us_size);
uint8_t hal_write_fu_data_cfg(void *pv_src, uint16_t us_size);
void hal_swap_startup_address(void);
void hal_get_startup_address(uint32_t *startAddr);
void hal_get_optional_startup_address(uint32_t *startAddr);
void hal_set_fw_page_address(uint32_t addr);
uint32_t hal_get_fw_page_address(void);
void hal_get_fw_version(hal_sw_version_info *vPtr);
void hal_read_fw_data(uint32_t addr, uint8_t *buf, uint16_t us_size);
uint8_t hal_write_fw_data(uint32_t addr, uint8_t *buf, uint16_t us_size);
uint32_t hal_get_max_fw_size(void);
void hal_get_fu_info_list(uint8_t *buf);
void hal_set_fu_info_list(uint8_t *buf);
uint8_t hal_unlock_mem(void);
uint8_t hal_lock_mem(void);

/* @} */

/* \name Bootloader Firmware upgrade feature */
/* @{ */
void hal_configure_bootloader_watchdog_mode(void);
uint8_t hal_update_bootloader(void);
uint8_t hal_copy_bootloader(uint32_t addr_new_bootloader,
		uint32_t addr_old_bootloader);
uint8_t hal_erase_Bootloader(uint32_t bootloader_addr);

/* @} */

/* @} */

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/* @endcond */

#endif /* HAL_H_INCLUDED */
