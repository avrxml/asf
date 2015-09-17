/**
 * \file
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

#ifndef REGIONS_H_INCLUDED
#define REGIONS_H_INCLUDED

#define REGIONS_INFO_SIZE          (INFO_SIZE)
#define REGIONS_INFO_RESERVED_SIZE (INFO_SIZE/4 - 6 - 12/4)
/** Boot informations (for regions) */
typedef struct regions_info {
	/** Firmware length in regions */
	uint32_t length[2];
	/** Firmware signature in regions */
	uint32_t signature[2];
	/** Firmware to boot in region # (0 or 1) */
	uint32_t boot_region;
	/** Software trigger to force action */
	uint32_t trigger;
	/** Boot file name, if update from extended memory (8+.+3) */
	char  boot_file_name[12];
	/** padding words */
	uint32_t padding[REGIONS_INFO_RESERVED_SIZE];
} regions_info_t;

void region_info_read(void *addr, struct regions_info *info);
void region_info_write(void *addr, struct regions_info *info);
uint32_t region_signature(void *addr, uint32_t size);
bool region_check_valid(void *addr, uint32_t size, uint32_t signature);

#endif /* REGIONS_H_INCLUDED */
