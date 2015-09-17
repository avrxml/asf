/**
 * \file
 *
 * \brief Raw flash operation.
 *
 * This file contains definitions and functions for raw NAND Flash operation.
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
 *
 * The raw NAND Flash layer drives hardware-specific Bus Read and Bus Write
 * operations to communicate with the NAND Flash device. It directly interacts
 * with hardware's register to operate NAND Flash interface.The raw NAND Flash
 * layer code implement procedures to program basic NAND Flash operations.
 * It takes care of issuing the correct sequences of write/read operations for
 * each command. All functions in the layer are blocked i.e. they wait for
 * the completion of an operation. Raw NAND Flash layer access NAND Flash
 * device by EBI.
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef NAND_FLASH_RAW_H_INCLUDED
#define NAND_FLASH_RAW_H_INCLUDED

#include "nand_flash_model.h"

/* Describes a physical Raw NAND Flash chip connected to the microcontroller. */
struct nand_flash_raw {
	/* Model describing this NAND Flash characteristics. */
	struct nand_flash_model model;
	/* Address for sending commands to the NAND Flash. */
	uint32_t command_address;
	/* Address for sending addresses to the NAND Flash. */
	uint32_t address_address;
	/* Address for sending data to the NAND Flash. */
	uint32_t data_address;
};

uint32_t nand_flash_raw_initialize(struct nand_flash_raw *raw,
		const struct nand_flash_model *model, uint32_t command_address,
		uint32_t address_address, uint32_t data_address);
void nand_flash_raw_reset(const struct nand_flash_raw *raw);
uint32_t nand_flash_raw_read_id(const struct nand_flash_raw *raw);
uint32_t nand_flash_raw_erase_block(const struct nand_flash_raw *raw,
		uint16_t block);
void nand_flash_raw_read_page(const struct nand_flash_raw *raw,
		uint16_t block, uint16_t page, uint8_t *data, uint8_t *spare);
uint32_t nand_flash_raw_write_page(const struct nand_flash_raw *raw,
		uint16_t block, uint16_t page, uint8_t *data, uint8_t *spare);
uint32_t nand_flash_raw_copy_page(const struct nand_flash_raw *raw,
		uint16_t source_block, uint16_t source_page,
		uint16_t dest_block, uint16_t dest_page);
uint32_t nand_flash_raw_copy_block(const struct nand_flash_raw *raw,
		uint16_t source_block, uint16_t dest_block);

#endif /* NAND_FLASH_RAW_H_INCLUDED */

