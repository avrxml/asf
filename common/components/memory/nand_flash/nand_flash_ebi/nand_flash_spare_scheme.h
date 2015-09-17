/**
 * \file
 *
 * \brief Flash spare area scheme.
 *
 * This file contains definitions and functions to do NAND Flash device's
 * spare area operations.
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

#ifndef NAND_FLASH_SPARE_SCHEME_H_INCLUDED
#define NAND_FLASH_SPARE_SCHEME_H_INCLUDED

#include "nand_flash_common.h"

struct nand_flash_spare_scheme {
	uint8_t bad_block_marker_position;
	uint8_t ecc_byte_number;
	uint8_t ecc_bytes_positions[NAND_COMMON_MAX_SPARE_ECC_BYTES];
	uint8_t extra_byte_number;
	uint8_t extra_bytes_positions[NAND_COMMON_MAX_SPARE_EXTRA_BYTES];
};

extern const struct nand_flash_spare_scheme nand_flash_spare_scheme_256;
extern const struct nand_flash_spare_scheme nand_flash_spare_scheme_512;
extern const struct nand_flash_spare_scheme nand_flash_spare_scheme_2048;
extern const struct nand_flash_spare_scheme nand_flash_spare_scheme_4096;

void nand_flash_spare_scheme_read_bad_block_marker(const struct
		nand_flash_spare_scheme *scheme, const uint8_t *spare,
		uint8_t *marker);
void nand_flash_spare_scheme_write_bad_block_marker(const struct
		nand_flash_spare_scheme *scheme, uint8_t *spare,
		uint8_t marker);
void nand_flash_spare_scheme_read_ecc(const struct nand_flash_spare_scheme
		*scheme, const uint8_t *spare, uint8_t *ecc);
void nand_flash_spare_scheme_write_ecc(const struct nand_flash_spare_scheme
		*scheme, uint8_t *spare, const uint8_t *ecc);
void nand_flash_spare_scheme_read_extra(const struct nand_flash_spare_scheme
		*scheme, const uint8_t *spare, void *extra, uint8_t size,
		uint8_t offset);
void nand_flash_spare_scheme_write_extra(const struct nand_flash_spare_scheme
		*scheme, uint8_t *spare, const void *extra, uint8_t size,
		uint8_t offset);

#endif /* NAND_FLASH_SPARE_SCHEME_H_INCLUDED */

