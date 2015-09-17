/**
 * \file
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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

#ifdef MEM_USE_FLASH

/** The last byte address of internal flash */
#define IFLASH_END (IFLASH_ADDR + IFLASH_SIZE - 1)

/* critical disable irq */
#define mem_flash_op_enter() do {\
		__DSB(); __ISB();        \
		cpu_irq_disable();       \
		__DMB();                 \
	} while (0)
/* critical enable irq */
#define mem_flash_op_exit() do {  \
		__DMB(); __DSB(); __ISB();\
		cpu_irq_enable();         \
	} while (0)

/**
 * Initialize the flash
 */
void mem_flash_init(void)
{
	dbg_print("flash: init ...\r\n");
	flash_init(FLASH_ACCESS_MODE_128, 6);
	dbg_print("flash: init done\r\n");
}

/**
 * Cleans up the flash
 */
void mem_flash_cleanup(void)
{
	dbg_print("flash: cleanup ...\r\n");
	dbg_print("flash: cleanup done\r\n");
}

/**
 * Erase area in flash
 *
 * \param addr Flash address in bytes
 * \param size number of bytes to erase
 *
 * \return erased bytes (could be larger than size)
 */
uint32_t mem_flash_erase(void *addr, uint32_t size)
{
#if SAM4S || SAM4E || SAM4N
	uint32_t page_addr = (uint32_t)addr;
	uint32_t page_off  = page_addr % (IFLASH_PAGE_SIZE*8);
	uint32_t rc, erased = 0;
	if (page_off) {
		dbg_print("flash: erase address must be 8 page aligned\r\n");
		page_addr = page_addr - page_off;
		dbg_print("flash: erase from %x\r\n", (unsigned)page_addr);
	}
	for (erased = 0; erased < size;) {
		rc = flash_erase_page((uint32_t)page_addr, IFLASH_ERASE_PAGES_8);
		erased    += IFLASH_PAGE_SIZE*8;
		page_addr += IFLASH_PAGE_SIZE*8;
		if (rc != FLASH_RC_OK) {
			dbg_print("flash: %x erase error\r\n", (unsigned)page_addr);
			break;
		}
	}
	return erased;
#elif SAMG55
	uint32_t page_addr = (uint32_t)addr;
	uint32_t page_off  = page_addr % (IFLASH_PAGE_SIZE*16);
	uint32_t rc, erased = 0;
	if (page_off) {
		dbg_print("flash: erase address must be 16 page aligned\r\n");
		page_addr = page_addr - page_off;
		dbg_print("flash: erase from %x\r\n", (unsigned)page_addr);
	}
	for (erased = 0; erased < size;) {
		rc = flash_erase_page((uint32_t)page_addr, IFLASH_ERASE_PAGES_16);
		erased    += IFLASH_PAGE_SIZE*16;
		page_addr += IFLASH_PAGE_SIZE*16;
		if (rc != FLASH_RC_OK) {
			dbg_print("flash: %x erase error\r\n", (unsigned)page_addr);
			break;
		}
	}
	return erased;
#else
	/* Everything assume to be erased since we use erase and write command */
	UNUSED(addr);
	return size;
#endif
}

/**
 * Write one page to flash
 *
 * \param addr Flash address in bytes
 * \param data Source data buffer
 *
 * \return true if success.
 */
bool mem_flash_page_write(void *addr, void *data)
{
	uint32_t rc = FLASH_RC_OK;
	mem_flash_op_enter();
#if SAM4S || SAM4E || SAM4N || SAMG55
	/* The EWP command can only be used in 8 KBytes sector for SAM4S, SAM4E
	 * and SAM4N, so an erase command is requried before write operation.
	 */
	rc = flash_write((uint32_t)addr, data, IFLASH_PAGE_SIZE, false);
#else
	rc = flash_write((uint32_t)addr, data, IFLASH_PAGE_SIZE, true);
#endif
	mem_flash_op_exit();
	if (FLASH_RC_OK != rc) {
		return false;
	}
	return true;
}

/**
 * Return next page address after/start at input address
 *
 * \param addr Flash address in bytes
 * \return Next page address in bytes
 */
void *mem_flash_page_next(void *addr)
{
	uint32_t address = (uint32_t)addr;
	uint32_t offset = address % IFLASH_PAGE_SIZE;
	if (offset) {
		address += (IFLASH_PAGE_SIZE - offset);
	}
	/* Mapped address to boot area */
	if (address < IFLASH_ADDR) {
		address += IFLASH_ADDR;
	}
	return (void*)address;
}

/**
 * Lock flash region
 *
 * \param start Start physical address
 * \param end   End physical address
 *
 * \return true if success
 */
bool mem_flash_lock(void* start, void* end)
{
	uint32_t rc;
	uint32_t start_addr = (uint32_t)start;
	uint32_t end_addr = (uint32_t)end;
	if (start_addr < IFLASH_ADDR) {
		start_addr = IFLASH_ADDR;
	}
	if (end_addr > IFLASH_END || end_addr == 0) {
		end_addr = IFLASH_END;
	}
	if (start_addr >= end_addr) {
		return false;
	}
	mem_flash_op_enter();
	rc = flash_lock(start_addr, end_addr, NULL, NULL);
	mem_flash_op_exit();
	if (rc) {
		return false;
	}
	return true;
}

/**
 * Unlock flash region
 *
 * \param start Start physical address
 * \param end   End physical address
 *
 * \return true if success 
 */
bool mem_flash_unlock(void *start, void* end)
{
	uint32_t rc;
	uint32_t start_addr = (uint32_t)start;
	uint32_t end_addr = (uint32_t)end;
	if (start_addr < IFLASH_ADDR) {
		start_addr = IFLASH_ADDR;
	}
	if (end_addr > IFLASH_END || end_addr == 0) {
		end_addr = IFLASH_END;
	}
	if (start_addr >= end_addr) {
		return false;
	}
	mem_flash_op_enter();
	rc = flash_unlock(start_addr, end_addr, NULL, NULL);
	mem_flash_op_exit();
	if (rc) {
		return false;
	}
	return true;
}

#endif /* #ifdef MEM_USE_FLASH */

