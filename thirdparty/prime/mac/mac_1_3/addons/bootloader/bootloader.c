/**
 * \file
 *
 * \brief PLC Bootloader application for ATMEL PRIME Service Node
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

/**
 *  \mainpage ATMEL PLC Bootloader Application for Service Node
 *
 *  \section Purpose
 *
 *  This application shows how to use the bootloader in a Service Node on PLC
 *	boards.
 *
 *  \section Requirements
 *
 *  This package should be used with PLC boards, on which there is dedicated PLC
 *	hardware.
 *
 *  \section Description
 *
 *  This application will check the firmware that must be used and will
 *	initialize the board with it.
 *
 */

#include "string.h"

/* Atmel boards includes. */
#include "board.h"

/* Atmel library includes. */
#include "asf.h"

/* Bootloader include. */
#include "conf_version.h"

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

/* watchdog reset period is 3 seconds, (base unit is the slow clock divided by
 * 128, ~ 4 ms) */
#define WATCHDOG_INITIAL_VALUE 0xFFFF

/* Array to translate the FW identifier to its associated flash memory zone */
uint32_t ul_fw_version_to_address[3] = {BOOT_FLASH_EXEC_FIRMWARE_START_ADDRESS,
					BOOT_FLASH_FIRMWARE_V1_START_ADDRESS,
					BOOT_FLASH_FIRMWARE_V2_START_ADDRESS};

/* Prime Bootloader version (in fixed position in flash) */
#ifdef __GNUC__
const unsigned char PRBO_version[32] __attribute__((section(".frwvrs"))) = PRBO_VERSION;
#endif
#ifdef __ICCARM__
#pragma location = ".frwvrs"
__root const char PRBO_version[32] = PRBO_VERSION;
#endif

/* Temporal buffer to store the flash pages content */
uint8_t uc_page[BOOT_FLASH_PAGE_SIZE];

#define BOOT_USER_SIGNATURE_SIZE_8               BOOT_FLASH_PAGE_SIZE
#define BOOT_USER_SIGNATURE_SIZE_32              (BOOT_FLASH_PAGE_SIZE / sizeof(uint32_t))

/**
 * \brief Verify one page of written flash against its data
 *        source in ram.
 *
 * \param puc_page_in_ram     Pointer to the data source in ram
 * \param puc_page_in_flash   Pointer to the page in flash
 *
 * \return 1  Comparison result is OK
 * \return 0  Comparison result is NOK
 */
static uint8_t verify_page(uint8_t *puc_page_in_ram, uint8_t *puc_page_in_flash)
{
	uint16_t i = 0;

	while (i < BOOT_FLASH_PAGE_SIZE) {
		if (puc_page_in_ram[i] != puc_page_in_flash[i]) {
			return 0;
		}

		i++;
	}
	return 1;
}

/**
 * \brief This function erases the indicated FW version.
 *
 * \param  fw_to_erase   Identifier of the FW version to erase.
 *
 * \return 1  Erase OK
 * \return 0  Erase NOK
 */
static uint8_t erase_fw_version(boot_flash_fw_version_t fw_to_erase)
{
	uint8_t i = 0;
	uint8_t ul_result;
	uint32_t *pul_flash = NULL;
	uint32_t ul_base_address = 0;

	if (fw_to_erase > BOOT_REGIONS) {
		return 0;
	}

	ul_base_address = ul_fw_version_to_address[fw_to_erase];

	for (i = 0; i < BOOT_FLASH_FIRMWARE_MAX_SIZE_IN_SECTORS; i++) {
		pul_flash = (uint32_t *)(ul_base_address + i * BOOT_FLASH_SECTOR_SIZE);
		ul_result = flash_erase_sector((uint32_t)pul_flash);
		if (ul_result != FLASH_RC_OK) {
			return 0;
		}
	}
	return 1;
}

/**
 * \brief This function copies the FW stored in orig_version into dest_version.
 *
 * \param   orig_version   Identifier of the FW version to copy.
 * \param   dest_version   Destination of the copy process.
 *
 * \return 1  Copy is OK
 * \return 0  Copy is NOK
 */
static uint8_t copy_fw_version(boot_flash_fw_version_t orig_version,
		boot_flash_fw_version_t dest_version)
{
	uint8_t i = 0, j = 0;
	uint8_t ul_result;
	uint32_t ul_sector_offset = 0;
	uint32_t ul_page_offset = 0;
	uint32_t ul_orig_base_address = 0;
	uint32_t ul_dest_base_address = 0;

	if ((orig_version > BOOT_REGIONS) || (dest_version > BOOT_REGIONS)) {
		return 0;
	}

	ul_orig_base_address = ul_fw_version_to_address[orig_version];
	ul_dest_base_address = ul_fw_version_to_address[dest_version];

	for (i = 0; i < BOOT_FLASH_FIRMWARE_MAX_SIZE_IN_SECTORS; i++) {
		ul_sector_offset = i * BOOT_FLASH_SECTOR_SIZE;
		/* Copy sector, page by page */
		for (j = 0; j < BOOT_FLASH_PAGES_PER_SECTOR; j++) {
			ul_page_offset = ul_sector_offset + j * BOOT_FLASH_PAGE_SIZE;
			memcpy(uc_page, (uint8_t *)(ul_orig_base_address + ul_page_offset), sizeof(uc_page));
			ul_result = flash_write(ul_dest_base_address + ul_page_offset, uc_page, sizeof(uc_page), 0);
			if (ul_result != FLASH_RC_OK) {
				return 0;
			}

			/* Verify the written data */

			/* If not successfully verified, repeat the page writing process */
			if (!verify_page(uc_page, (uint8_t *)(ul_dest_base_address + ul_page_offset))) {
				j--;
			}
		}
	}
	return 1;
}

/**
 * \brief Main code entry point.
 */
int main(void)
{
	uint8_t uc_user_sign_buf[BOOT_FLASH_PAGE_SIZE];
	uint8_t *puc_boot_config;
	uint8_t uc_boot_status;
	uint8_t uc_value = 0;
	boot_flash_fw_version_t uc_fw_to_use = BOOT_FLASH_FW_VERSION_EXEC;
	uint32_t i;

	/* Ensure all priority bits are assigned as preemption priority bits. */
	NVIC_SetPriorityGrouping(__NVIC_PRIO_BITS);
	__set_BASEPRI(0);

	/* Initialize flash: 6 wait states for flash writing. */
	flash_init(FLASH_ACCESS_MODE_128, 6);

	/* Read the Boot status from user signature */
	flash_read_user_signature((void *)uc_user_sign_buf,
			BOOT_USER_SIGNATURE_SIZE_32);

	/* provide boot config */
	puc_boot_config = uc_user_sign_buf + BOOT_STATUS_OFFSET_USER_SIGN;
	uc_boot_status = (uint8_t)(*puc_boot_config);

	/* check validity boot status. In case of error, FW V1 is loaded by default */
	if (!boot_is_error(uc_boot_status)) {
		/* Check if it is needed to change firmware */
		if (boot_is_swap_cmd(uc_boot_status)) {
			/* Update "FW in use" information and clear the "FU done" flag */
			if (boot_is_fw_v1_in_use(uc_boot_status)) {
				uc_value = BOOT_FW_V2_IN_USE_MSK;
				uc_fw_to_use = BOOT_FLASH_FW_VERSION_2;
			} else {
				uc_value = BOOT_FW_V1_IN_USE_MSK;
				uc_fw_to_use = BOOT_FLASH_FW_VERSION_1;
			}
		}
	} else {
		uc_value = BOOT_FW_V1_IN_USE_MSK;
	}

	/* Swap Fw if it is needed */
	if (uc_value) {
		/* change osc to external to optimize proc. time */
		sysclk_init();
		/* store boot config */
		*puc_boot_config = uc_value;
		/* Erase the user signature */
		flash_erase_user_signature();
		/* Update user signature */
		flash_write_user_signature((void *)uc_user_sign_buf, BOOT_USER_SIGNATURE_SIZE_32);
		if (uc_fw_to_use) {
			/* Erase the execution zone */
			(void)erase_fw_version(BOOT_FLASH_FW_VERSION_EXEC);
			/* Update new FW */
			(void)copy_fw_version(uc_fw_to_use, BOOT_FLASH_FW_VERSION_EXEC);
		}

		/* Return to slow RC */
		osc_enable(OSC_SLCK_32K_RC);
		osc_wait_ready(OSC_SLCK_32K_RC);
		pmc_switch_mck_to_sclk(SYSCLK_PRES_1);
		/* Switch clock to fast RC */
		osc_enable(OSC_MAINCK_12M_RC);
		osc_wait_ready(OSC_MAINCK_12M_RC);
		pmc_switch_mck_to_mainck(SYSCLK_PRES_1);
	}

	__disable_irq();
	/* Disable SysTick */
	SysTick->CTRL = 0;
	/* Disable IRQs & clear pending IRQs */
	for (i = 0; i < 8; i++) {
		NVIC->ICER[i] = 0xFFFFFFFF;
		NVIC->ICPR[i] = 0xFFFFFFFF;
	}

	/* Modify vector table location */
	__DSB();
	__ISB();
	/* set the stack pointer also to the start of the firmware */
	__set_MSP(*(int *)(BOOT_FLASH_EXEC_FIRMWARE_START_ADDRESS));
	/* offset the start of the vector table (first 6 bits must be zero) */
	/* The register containing the offset, from 0x00000000, is at 0xE000ED08
	 **/
	SCB->VTOR = ((uint32_t)BOOT_FLASH_EXEC_FIRMWARE_START_ADDRESS & SCB_VTOR_TBLOFF_Msk);
	__DSB();
	__ISB();
	__enable_irq();
	/* * (int *) 0xE000ED08 = FIRMWARE_START_ADDRESS; */

	/* jump to the start of the firmware, casting the address as function
	 * pointer to the start of the firmware. We want to jump to the address 
	 * of the reset */

	/* handler function, that is the value that is being pointed at position
	 * FIRMWARE_RESET_ADDRESS */
	void (*runFirmware)(void) = NULL;
	runFirmware = (void (*)(void))(*(uint32_t *)BOOT_FLASH_EXEC_FIRMWARE_RESET_ADDRESS);
	runFirmware();

	while (1) {
	}
}
