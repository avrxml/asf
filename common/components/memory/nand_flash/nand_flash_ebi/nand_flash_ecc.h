/**
 * \file
 *
 * \brief ECC flash operation.
 *
 * This file contains definitions and functions for ECC NAND Flash operation.
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

#ifndef NAND_FLASH_ECC_H_INCLUDED
#define NAND_FLASH_ECC_H_INCLUDED

/**
 * NAND Flash ECC layer is called by upper layer, it will call the raw layer
 * to do write/read operations, and do ECC check to the write/read result,
 * it then will feedback the ECC check result to the upper layer.
 *
 * -# nand_flash_ecc_initialize is used to initializes an nand_flash_ecc
 * instance.
 * -# nand_flash_ecc_write_page is used to write a NAND Flash page with ECC
 * result, the function will calculate ECC for the data that is going to be
 * written, and write data and spare(with calculated ECC) to NAND Flash device.
 * -# nand_flash_ecc_read_page is used to read a NAND Flash page with ECC
 * check, the function will read out data and spare first, then it calculates
 * ecc with data and then compare with the readout ECC, and feedback the ECC
 * check result to upper layer.
 */

#include "nand_flash_raw.h"

struct nand_flash_ecc {
	struct nand_flash_raw raw;
};

uint32_t nand_flash_ecc_initialize(struct nand_flash_ecc *ecc,
		const struct nand_flash_model *model,
		uint32_t command_address,
		uint32_t address_address, uint32_t data_address);

uint32_t nand_flash_ecc_read_page(const struct nand_flash_ecc *ecc,
		uint16_t block, uint16_t page, uint8_t *data, uint8_t *spare);

uint32_t nand_flash_ecc_write_page(const struct nand_flash_ecc *ecc,
		uint16_t block, uint16_t page, uint8_t *data, uint8_t *spare);

#endif /* NAND_FLASH_ECC_H_INCLUDED */
