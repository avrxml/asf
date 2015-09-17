/*
 * Copyright (c) 2014, Thingsquare, http://www.thingsquare.com/.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdio.h>
#include <stdint.h>
#include "contiki.h"
#include "dev/watchdog.h"
#include "flash.h"
#include "asf.h"
#include "nvm.h"
/*---------------------------------------------------------------------------*/
/*
 * Datasheet errata to take into consideration when implementing:
 * When NVMCTRL issues either erase or write commands and the NVMCTRL cache is not 
 * in LOW_POWER mode, CPU hardfault exception may occur. Errata reference: 10804
 * Fix/Workaround
 * Either:
 * - turn off cache before issuing flash commands by setting the NVMCTRL CTRLB.CACHEDIS bit to 
 * one. 
 * - Configure the cache in LOW_POWER mode by writing 0x1 into the NVMCTRL 
 * CTRLB.READMODE bits.
 */

/* Constants, refer to the datasheet, section 9 */
#define NVM_PAGES             4096
#define NVM_PAGES_PER_ROW     4     /* erase only possible w row-granularity */
#define NVM_PAGESIZE          64    /* size of a page in flash, in bytes */
#define NVM_FLASH_END_D20J18  (0x0ul + 262144) /* 256kB flash */
/*---------------------------------------------------------------------------*/
#define _DEBUG_                 0
#if _DEBUG_
#define PRINTF(...)       printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif
/*---------------------------------------------------------------------------*/
void
flash_init(void)
{
#if 1
  struct nvm_config nvm_conf;
  nvm_get_config_defaults(&nvm_conf);
  nvm_set_config(&nvm_conf);
#endif
}
/*---------------------------------------------------------------------------*/
void
flash_clear(unsigned long addr)
{
#if 1
  enum status_code error_code = STATUS_OK;

  /* Note that this clear the entire row the page is located in (= 4 pages) */

  /* Check if the destination address is valid */
  if(addr > NVM_FLASH_END_D20J18) {
    return;
  }

  /* clear out the lower bits to ensure we are addressing a page start */
  addr &= ~(0x000000ff);
  error_code = nvm_erase_row(addr);
  if(error_code != 0) {
    PRINTF("flash clear error %02x\n", error_code);
  }
#endif
}
/*---------------------------------------------------------------------------*/
void
flash_write(unsigned long addr16, uint16_t data16)
{
#if 1
  uint8_t page_buffer[NVM_PAGESIZE];
  enum status_code error_code = STATUS_OK;
  uint32_t page_start;
  int page_offset;
  int i;

  if(addr16 >= NVM_FLASH_END_D20J18) {
    /* error, must not write outside flash */
    PRINTF("flash: error invalid address\n");
    return;
  }

  page_start = (uint32_t) addr16 & ~(NVM_PAGESIZE - 1);
  page_offset = addr16 - page_start;

  PRINTF("flash write 0x%04x: start %p offset %u\n", data16, page_start, page_offset);

  /* 
   * We must write a full page (64B) at once due to buffering in the NVM.
   * Write all ones' except the new data, this preserves any existing data
   */
  memset(page_buffer, 0xff, NVM_PAGESIZE);
  page_buffer[page_offset] = (uint8_t) (data16 & 0x00ff);
  page_buffer[page_offset + 1] = (uint8_t) (data16 >> 8);

  /* print out the buffer we are about to write */
  PRINTF("0x");
  for(i = 0; i < NVM_PAGESIZE; i++) {
    PRINTF("%02x", page_buffer[i]);
  }
  PRINTF("\n");

  /* Write the data to flash */
  error_code = nvm_write_buffer((const uint32_t) page_start, page_buffer, NVM_PAGESIZE);
  if(error_code != 0) {
    PRINTF("flash write error %02x\n", error_code);
  }
#endif
}
/*---------------------------------------------------------------------------*/
uint16_t
flash_read(unsigned long addr16)
{
  return *(uint16_t *) addr16;
}
/*---------------------------------------------------------------------------*/
void
flash_setup(void)
{
  system_interrupt_disable_global();
}
/*---------------------------------------------------------------------------*/
void
flash_done(void)
{
  system_interrupt_enable_global();
}
/*---------------------------------------------------------------------------*/

