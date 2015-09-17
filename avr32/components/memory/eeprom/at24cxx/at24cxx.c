/*****************************************************************************
 *
 * \file
 *
 * \brief AT24cxx driver for AVR32 UC3.
 *
 * This file is the AT24cxx driver.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "board.h"
#include "compiler.h"
#include "gpio.h"
#include "conf_at24cxx.h"
#include "at24cxx.h"
#include "cycle_counter.h"

#if UC3C || UC3A3
#include "twim.h"
#else
#include "twi.h"
#endif

static uint32_t cpu_hz;

void at24cxx_init(int32_t fcpu) {
        /* Store cpu frequency locally*/
        cpu_hz = fcpu;
}


void at24cxx_write_byte(uint16_t byte_address, uint8_t byte_value) {
  uint8_t pack[3];
  twi_package_t twi_package;

  pack[0] = (byte_address&0xFF00)>>8;
  pack[1] = byte_address&0xFF;
  pack[2] = byte_value;

  twi_package.chip = AT24CXX_TWI_ADDRESS;
  twi_package.addr_length = 1;
  twi_package.buffer = &pack;
  twi_package.length = sizeof(pack);

  while(twi_master_write(AT24CXX_TWI, &twi_package)!=TWI_SUCCESS);

  return;
}


void at24cxx_write_continuous(uint16_t start_address, uint16_t length, uint8_t const *wr_buffer) {
  uint8_t *pack = {0};
  twi_package_t twi_package;

  pack[0] = (start_address&0xFF00)>>8;
  pack[1] = start_address&0xFF;
  uint16_t idx;
  for (idx=0;idx<length;idx++)
    pack[2+idx] = wr_buffer[idx];

  twi_package.chip = AT24CXX_TWI_ADDRESS;
  twi_package.addr_length = 0;
  twi_package.buffer = &pack;
  twi_package.length = sizeof(pack);

  while(twi_master_write(AT24CXX_TWI, &twi_package)!=TWI_SUCCESS);

  return;
}


uint8_t at24cxx_read_byte(uint16_t byte_address) {
  uint8_t data;
  twi_package_t twi_package;

  twi_package.chip = AT24CXX_TWI_ADDRESS;
  twi_package.addr_length = 0;
  twi_package.buffer = &byte_address;
  twi_package.length = 2;
  while(twi_master_write(AT24CXX_TWI, &twi_package)!=TWI_SUCCESS);

  twi_package.chip = AT24CXX_TWI_ADDRESS;
  twi_package.addr_length = 0;
  twi_package.buffer = &data;
  twi_package.length = 1;
  while(twi_master_read(AT24CXX_TWI, &twi_package)!=TWI_SUCCESS);


  return data;

}


void at24cxx_read_continuous(uint16_t start_address, uint16_t length, uint8_t *rd_buffer) {
  twi_package_t twi_package;

  twi_package.chip = AT24CXX_TWI_ADDRESS;
  twi_package.addr_length = 0;
  twi_package.buffer = &start_address;
  twi_package.length = 2;
  while(twi_master_write(AT24CXX_TWI, &twi_package)!=TWI_SUCCESS);

  twi_package.chip = AT24CXX_TWI_ADDRESS;
  twi_package.addr_length = 0;
  twi_package.buffer = rd_buffer;
  twi_package.length = length;
  while(twi_master_read(AT24CXX_TWI, &twi_package)!=TWI_SUCCESS);

}
