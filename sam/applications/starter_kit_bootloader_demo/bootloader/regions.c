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

#include "bootloader.h"

#define none_blank(v32) (((v32)==0xFFFFFFFF)?0:(v32))

/**
 * Read region information
 * \param addr Address to read
 * \param info Pointer to region information to store
 */
void region_info_read(void *addr, struct regions_info *info)
{
	uint32_t i;
	struct regions_info *p = (struct regions_info*)addr;
	info->length[0]    = none_blank(p->length[0]);
	info->length[1]    = none_blank(p->length[1]);
	info->signature[0] = none_blank(p->signature[0]);
	info->signature[1] = none_blank(p->signature[1]);
	info->boot_region  = (p->boot_region == 1) ? 1 : 0; /* only accept 0 & 1 */
	switch(p->trigger) { /* Accept valid modes only */
	case TRIGGER_LOAD:
	case TRIGGER_LOAD_FILE:
	case TRIGGER_UPDATE:
	case TRIGGER_UPDATE_FILE:
		info->trigger = p->trigger;
		break;
	default:
		info->trigger = TRIGGER_BOOT;
	}
	for (i = 0; i < 12; i ++) {
		info->boot_file_name[i] = p->boot_file_name[i];
	}
	if (info->boot_file_name[0] <= ' ' ||
		info->boot_file_name[0] == 0xFF) {
		info->boot_file_name[0] = 0;
	}
	/* Keep all paddings */
	for (i = 0; i < REGIONS_INFO_RESERVED_SIZE; i ++) {
		info->padding[i] = p->padding[i];
	}
}

/**
 * Write region information
 * \param addr Address to read
 * \param info Pointer to region information to store
 */
void region_info_write(void *addr, struct regions_info *info)
{
	if (NULL == info) {
		return;
	}
	memory_write(addr, info);
}

/**
 * Calculate signature of a region
 * \param addr region start address
 * \param size region data size
 * \return signature
 */
uint32_t region_signature(void *addr, uint32_t size)
{
	uint8_t *p_byte;
	uint32_t l_byte, u_byte;
	uint32_t count;
	p_byte = (uint8_t*)addr;
	l_byte = 1;
	u_byte = 0;
	/* Calculate CRC */
	while(size) {
		if (size > 5550) {
			count = 5550;
		} else {
			count = size;
		}
		size -= count;
		while (count != 0) {
			l_byte += *p_byte;
			u_byte += l_byte;
			p_byte ++;
			count --;
		}
		l_byte = (l_byte & 0xFFFF) + (l_byte >> 16) * 15;
		u_byte = (u_byte & 0xFFFF) + (u_byte >> 16) * 15;
	}
	if (l_byte >= 65521) {
		l_byte -= 65521;
	}
	u_byte = (u_byte & 0xFFFF) + (u_byte >> 16) * 15;
	if (u_byte >= 65521) {
		u_byte -= 65521;
	}
	return (u_byte << 16) | l_byte;
}

/**
 * Checks that a region is valid
 * \param addr region address
 * \param size region data size
 * \param signature expected signature
 * \return true if region is valid
 */
bool region_check_valid(void *addr, uint32_t size, uint32_t signature)
{
	uint32_t signature1;
	/* Check length */
	if (size > APP_SIZE || size == 0) {
		return false;
	}
	/* Check signature */
	signature1 = region_signature(addr, size);
	if (signature != signature1) {
		dbg_print("region: %x sign diff, app %x <> info %x\r\n",
				(unsigned)addr, (unsigned)signature1, (unsigned)signature);
		return false;
	}
	return true;
}

