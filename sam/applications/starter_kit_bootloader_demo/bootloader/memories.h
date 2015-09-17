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

#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

#ifdef MEM_USE_FLASH

/* if dual bank flash */
#ifndef IFLASH_ADDR
#  define IFLASH_ADDR      IFLASH0_ADDR
#endif
/* if dual bank flash */
#ifndef IFLASH_PAGE_SIZE
#  define IFLASH_PAGE_SIZE IFLASH0_PAGE_SIZE
#endif

#define memory_init        mem_flash_init
#define memory_cleanup     mem_flash_cleanup
#define memory_erase       mem_flash_erase
#define memory_write       mem_flash_page_write
#define memory_next        mem_flash_page_next
#define memory_lock        mem_flash_lock
#define memory_unlock      mem_flash_unlock
#define memory_flush()

void mem_flash_init(void);
void mem_flash_cleanup(void);
uint32_t mem_flash_erase(void * addr,uint32_t size);
bool mem_flash_page_write(void * addr,void * data);
void *mem_flash_page_next(void * addr);
bool mem_flash_lock(void * start,void * end);
bool mem_flash_unlock(void * start,void * end);

#endif /* #ifdef MEM_USE_FLASH */

#endif
