/**
 * \file
 *
 * \brief NAND Flash ctrl access interface definition.
 *
 * This file contains ctrl access interface functions of NAND Flash module.
 *
 * Copyright (c) 2012-2015 Atmel Corporation. All rights reserved.
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

#ifndef NAND_FLASH_MEM_H_INCLUDED
#define NAND_FLASH_MEM_H_INCLUDED

#include "ctrl_access.h"

#define NAND_FLASH_VERSION     "1.0"

#define NAND_FLASH_NOT_INIT    0
#define NAND_FLASH_READY       1
#define NAND_FLASH_BUSY        2
#define NAND_FLASH_UNLOADED    3

/* ---- CONTROL FUNCTIONS ---- */
/**
 * \brief This function tests memory state, and starts memory initialization
 *
 * \return Ctrl_status
 */
Ctrl_status  nand_flash_test_unit_ready(void);
/**
 * \brief This function returns the address of the last valid sector
 *
 * \param uint32_t_nb_sector  Pointer to number of sectors (sector=512 bytes)
 *
 * \return Ctrl_status
 */
Ctrl_status  nand_flash_read_capacity(uint32_t *u32_nb_sector);
/**
 * \brief This function returns the write-protected mode
 *
 * \return true if the memory is protected
 */
bool         nand_flash_wr_protect(void);
/**
 * \brief This function informs about the memory type
 *
 * \return true if the memory is removable
 */
bool         nand_flash_removal(void);
/**
 * \brief This function unloads/loads the memory
 *
 * \return true if memory unload/load success
 */
bool         nand_flash_unload(bool unload);

/* ---- ACCESS DATA FUNCTIONS ---- */

/* USB interface */
#if ACCESS_USB == true
/**
 * \brief This function transfers the memory data to the USB MSC interface
 *
 * \param addr Sector address to start read
 * \param nb_sector Number of sectors to transfer (sector=512 bytes)
 *
 * \return Ctrl_status
 */
Ctrl_status  nand_flash_usb_read_10(uint32_t addr, uint16_t nb_sector);
/**
 * \brief This function transfers the USB MSC data to the memory
 *
 * \param addr Sector address to start write
 * \param nb_sector Number of sectors to transfer (sector=512 bytes)
 *
 * \return Ctrl_status
 */
Ctrl_status  nand_flash_usb_write_10(uint32_t addr, uint16_t nb_sector);

#endif

/* RAM interface */
#if ACCESS_MEM_TO_RAM == true
/**
 * \brief This function transfers 1 data sector from memory to RAM
 *
 * \param addr Sector address to start read
 * \param ram Address of RAM buffer
 *
 * \return Ctrl_status
 */
Ctrl_status  nand_flash_mem_2_ram(uint32_t addr, void *ram);
/**
 * \brief This function transfers 1 data sector from RAM to memory
 *
 * \param addr Sector address to start write
 * \param ram Address of RAM buffer
 *
 * \return Ctrl_status
 */
Ctrl_status  nand_flash_ram_2_mem(uint32_t addr, const void *ram);

#endif

#endif /* NAND_FLASH_MEM_H_INCLUDED */
