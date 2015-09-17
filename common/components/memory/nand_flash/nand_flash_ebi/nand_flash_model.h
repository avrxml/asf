/**
 * \file
 *
 * \brief Flash model definition.
 *
 * This file contains flash model definitions and functions for NAND Flash
 * module.
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

#ifndef NAND_FLASH_MODEL_H_INCLUDED
#define NAND_FLASH_MODEL_H_INCLUDED

#include "nand_flash_spare_scheme.h"

/**
 *
 * \{
 */

/**
 * Nand_Flash_Model_opts NAND Flash Model options
 * - NAND_FLASH_MODEL_DATA_BUS_8
 * - NAND_FLASH_MODEL_DATA_BUS_16
 * - NAND_FLASH_MODEL_COPY_BACK
 */

/* Indicate the NAND Flash uses an 8-bit databus. */
#define NAND_FLASH_MODEL_DATA_BUS_8     (0 << 0)

/* Indicate the NAND Flash uses a 16-bit databus. */
#define NAND_FLASH_MODEL_DATA_BUS_16    (1 << 0)

/* The NAND Flash supports the copy-back (internal page-to-page copy). */
#define NAND_FLASH_MODEL_COPY_BACK     (1 << 1)

/* Model of NAND Flash device. */
struct nand_flash_model {
	/* Identifier for the device. */
	uint8_t device_id;
	/* Special options for the NAND Flash. */
	uint8_t options;
	/* Size of the data area of a page, in bytes. */
	uint16_t page_size_in_bytes;
	/* Size of the device in MB. */
	uint16_t device_size_in_megabytes;
	/* Size of one block in kilobytes. */
	uint16_t block_size_in_kilobytes;
	/* Spare area placement scheme. */
	const struct nand_flash_spare_scheme *scheme;
};

/* Number of NAND Flash model inside the list. */
#define NAND_FLASH_MODEL_LIST_SIZE         58

/* List of NAND Flash model which can be recognized by the software.*/
extern const struct nand_flash_model
		nand_flash_model_list[NAND_FLASH_MODEL_LIST_SIZE];

uint32_t nand_flash_model_find(const struct nand_flash_model *model_list,
		uint32_t size, uint32_t chip_id, struct nand_flash_model *model);
uint32_t nand_flash_model_translate_access(const struct nand_flash_model *model,
		uint32_t address, uint32_t size, uint16_t *block,
		uint16_t *page, uint16_t *offset);
const struct nand_flash_spare_scheme *nand_flash_model_get_scheme(const struct
		nand_flash_model *model);
uint8_t nand_flash_model_get_device_id(const struct nand_flash_model *model);
uint16_t nand_flash_model_get_device_size_in_blocks(const struct
		nand_flash_model *model);
uint32_t nand_flash_model_get_device_size_in_pages(const struct nand_flash_model
		*model);
uint64_t nand_flash_model_get_device_size_in_bytes(const struct nand_flash_model
		*model);
uint16_t nand_flash_model_get_device_size_in_megabytes(const struct
		nand_flash_model *model);
uint32_t nand_flash_model_get_block_size_in_pages(const struct nand_flash_model
		*model);
uint32_t nand_flash_model_get_block_size_in_bytes(const struct nand_flash_model
		*model);
uint16_t nand_flash_model_get_page_data_size(const struct nand_flash_model
		*model);
uint16_t nand_flash_model_get_page_spare_size(const struct nand_flash_model
		*model);
uint32_t nand_flash_model_get_data_bus_width(const struct nand_flash_model
		*model);
uint32_t nand_flash_model_small_block(const struct nand_flash_model *model);
uint32_t nand_flash_model_copy_back(const struct nand_flash_model *model);

/**
 * \}
 */

#endif /* NAND_FLASH_MODEL_H_INCLUDED */
