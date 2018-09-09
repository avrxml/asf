/*****************************************************************************
 *
 * \file
 *
 * \brief AT24cxx driver for AVR32 UC3.
 *
 * This file is the AT24cxx driver.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
