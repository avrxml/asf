/**
 * \file
 *
 * \brief MEGA FLASH DRIVER
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
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
