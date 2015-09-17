/**
 * \file
 *
 * \brief MEGA FLASH DRIVER
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
#include <stdint.h>
#include "interrupt.h"
#include "compiler.h"
#include "parts.h"
#include "flash.h"
#include "sysclk.h"
#include "status_codes.h"

void flash_write(uint32_t flash_addr, uint32_t length, uint8_t *data)
{
	static uint8_t temp_buf[SPM_PAGESIZE];
	static uint32_t remaining_len;
	remaining_len = length;
	uint8_t *ptr = data;
	static uint32_t next_page_addr;
	static uint16_t current_len;
	do {
		next_page_addr
			= ((flash_addr +
				SPM_PAGESIZE) - (flash_addr % SPM_PAGESIZE));

		/* copy the data in the page to be written into a temporary
		 * buffer,before erasing */
		flash_read((flash_addr - (flash_addr % SPM_PAGESIZE)),
				SPM_PAGESIZE, temp_buf);

		/* fill the temporary page buffer starting from address 0 */
		flash_fill_page_buffer(0x0000, SPM_PAGESIZE, temp_buf);

		/* If the length of bytes to be written crosses the current
		 * page,
		 *      write till the end of the current page and calculate the
		 * remaining length to be written*/
		if ((flash_addr + remaining_len) > (next_page_addr)) {
			current_len = (next_page_addr - flash_addr);
		} else {
			current_len = remaining_len;
		}

		remaining_len -= current_len;

		/* Fill the page buffer with the data to be written at the given
		 * address */
		flash_fill_page_buffer(flash_addr, current_len, ptr);
		/* Erase and program flash page */
		flash_program_page(flash_addr);
		flash_addr = next_page_addr;
		ptr += current_len;
	} while (remaining_len != 0); /* Check if there is remaining  data  to
	                               * be written to the next page */
}

#ifdef __ICCAVR__
#pragma optimize = no_inline
#endif
void flash_erase_page(uint32_t page_number)
{
	uint32_t page_start_addr;
	/* Find the start address of the given page number */
	page_start_addr = (page_number * FLASH_PAGE_SIZE);
	irqflags_t flags;
	flags = cpu_irq_save();

	EEPROM_BUSY_WAIT();
	/* Perform Page Erase */
	FLASH_ERASE(page_start_addr);
	SPM_BUSY_WAIT(); /* Wait until the memory is erased. */

	/* Reenable RWW-section again. */
	ENABLE_RWW_SECTION();

	cpu_irq_restore(flags);
}

#ifdef __ICCAVR__
#pragma optimize = no_inline
#endif
void flash_fill_page_buffer(uint32_t flash_addr, uint16_t length,
		uint8_t *data )
{
	uint8_t temp;
	uint8_t write_length = length;
	uint16_t start_offset = (flash_addr % SPM_PAGESIZE);

	/* For even address we can directly write a word to the address.
	 * For odd address the previous byte has to be copied and written
	 * together as a word */
	if (start_offset % 2) { /* odd address */
		temp = PGM_READ_BYTE_FAR(flash_addr - 1);
		uint16_t w = temp;
		w += (*data++) << 8;
		FLASH_PAGE_FILL(start_offset++, w);
		length--;
	}

	for (uint16_t i = start_offset; i < length + start_offset; i += 2) {
		/* If the last address to be written is odd ,then copy the
		 * adjacent byte and push it as a word */
		if (((length + start_offset) - i) == 1) {
			temp = PGM_READ_BYTE_FAR(flash_addr + write_length);
			uint16_t w = *data++;
			w += (temp) << 8;
			FLASH_PAGE_FILL(i, w);
		} else {
			uint16_t w = *data++;
			w += (*data++) << 8;
			FLASH_PAGE_FILL(i, w);
		}
	}
}

#ifdef __ICCAVR__
#pragma optimize = no_inline
#endif
void flash_program_page(uint32_t page_start_addr)
{
	irqflags_t flags;

	flags = cpu_irq_save();

	EEPROM_BUSY_WAIT();

	FLASH_ERASE(page_start_addr);
	SPM_BUSY_WAIT(); /* Wait until the memory is erased. */

	FLASH_PAGE_WRITE(page_start_addr); /* Store buffer in flash page. */
	SPM_BUSY_WAIT(); /* Wait until the memory is written. */

	/* Reenable RWW-section again. */
	ENABLE_RWW_SECTION();

	cpu_irq_restore(flags);
}

void flash_read(uint32_t read_addr, uint32_t len, uint8_t *ret_buf)
{
	irqflags_t flags;

	flags = cpu_irq_save();

	for (uint16_t k = 0; k < len; k++) {
		/* copy a byte and push to the ret_buf */
		*(ret_buf + k) = PGM_READ_BYTE_FAR(read_addr++);
	}
	cpu_irq_restore(flags);
}

/* ! EOF */
