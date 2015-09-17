/*****************************************************************************
 *
 * \file
 *
 * \brief Driver of the CS2200 clocking device chip.
 *
 * The CS2200-CP is an extremely versatile system clocking device that utilizes
 * a programmable phase lock loop. The CS2200-CP is based on an analog PLL
 * architecture comprised of a Delta-Sigma Fractional-N Frequency Synthesizer.
 * This architecture allows for frequency synthesis and clock generation from a
 * stable reference clock.
 *
 * The datasheet can be downloaded here:
 * http://www.cirrus.com/en/pubs/proDatasheet/CS2200-CP_F1.pdf
 *
 *                       used.
 *
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#include <avr32/io.h>
#include "compiler.h"

#include "cs2200.h"

#if CS2200_INTERFACE == CS2200_INTERFACE_SPI
#  include "spi.h"
#elif CS2200_INTERFACE == CS2200_INTERFACE_TWI
#    include "twi.h"
#endif
#include "gpio.h"
#include "board.h"

#if CS2200_INTERFACE == CS2200_INTERFACE_SPI
#  warning SPI interface not supported.
#endif

//! \name Expected Protocol Version
//! @{
#define CS2200_EXPECTED_DEVICE_ID     0x00
#define CS2200_DEVICE_CTRL_LOCKED     0x00
//! @}

/*! \name Device ID Register Bit-Masks
 */
//! @{
#define CS2200_DEVICE_ID_REG_MASK           0xF8
#define CS2200_DEVICE_ID_REG_OFFSET         3

#define CS2200_DEVICE_CTRL_UNLOCK_REG_MASK     0x80
#define CS2200_DEVICE_CTRL_UNLOCK_REG_OFFSET   8
//! @}


void cs2200_switch_on(void)
{
}

void cs2200_switch_off(void)
{
}

bool cs2200_setup(uint32_t out_freq, uint32_t fosc)
{
  int device_id;
  int nb_tries = CS2200_NB_TRIES;

  do
  {
    device_id = (CS2200_READ_DEVICE_ID() & CS2200_DEVICE_ID_REG_MASK) >> CS2200_DEVICE_ID_REG_OFFSET;
  // Make sure the chip is functional.
  } while ((device_id != CS2200_EXPECTED_DEVICE_ID)
         && --nb_tries);

  // If number of tries is over, return an error.
  if (!nb_tries)
    return false;

  // Freeze chip during the programming sequence
  CS2200_WRITE_GLOBAL_CFG(1<<3);

  CS2200_WRITE_DEVICE_CTRL(0x00); // AUX_OUT output driver enabled. CLK_OUT output driver enabled.
  CS2200_WRITE_DEVICE_CFG_1( 0 << 5 // Left-shift R-value by 0 (x 1).
                           | 0 << 1 // RefClk: is the source of the AUX_OUT signal
                           );
  CS2200_WRITE_32_BITS_RATIO(_32_BITS_RATIO(out_freq, fosc)); // Program a default clock.
  CS2200_WRITE_FUNCT_CFG_1( 0x00 << 6 // Push-Pull, Active High (output `high' for unlocked condition, `low' for locked condition).
                          | 0x02 << 3 // Reference Clock Input Divider: / 1 [8 MHz to 18.75 MHz]
                          );
  CS2200_WRITE_FUNCT_CFG_2( 0x00 << 4 // Clock outputs are driven `low' when PLL is unlocked.
                          );

  // Unleash chip
  CS2200_WRITE_GLOBAL_CFG( 0x00 << 3 // Freeze
                         | 0x01 << 0 // EnDevCfg2
                         );
  cs2200_enter_test_mode();
  return true;
}

void cs2200_enter_test_mode(void)
{
  CS2200_WRITE_TEST_MODE_1(0x99); // Unlock
  CS2200_WRITE_TEST_MODE_2(0x80); // force_normal state
}


void cs2200_leave_test_mode(void)
{
  CS2200_WRITE_TEST_MODE_2(0x00); // test mode default setting
  CS2200_WRITE_TEST_MODE_1(0x00); // Lock
}


void cs2200_freq_clk_out(uint32_t ratio)
{
  CS2200_WRITE_32_BITS_RATIO(ratio);
}


void cs2200_freq_clk_adjust(uint16_t lsh_ratio)
{
  static uint16_t s_ratio;
  s_ratio=lsh_ratio;
  cs2200_write_ex(CS2200_REG_LSW_RATIO_ADDR, &s_ratio, sizeof(lsh_ratio));
}


#if 0
void cs2200_set_new_freq_clk_out(uint32_t ratio)
{
  //int pll_lock_state;
  //int nb_tries = CS2200_NB_TRIES;

  cs2200_leave_test_mode();
  CS2200_WRITE_32_BITS_RATIO(ratio);
  /*
  do
  {
    pll_lock_state = (CS2200_READ_DEVICE_CTRL() & CS2200_DEVICE_CTRL_UNLOCK_REG_MASK) >> CS2200_DEVICE_CTRL_UNLOCK_REG_OFFSET;
  // Make sure the chip is functional.
  } while ((pll_lock_state != CS2200_DEVICE_CTRL_LOCKED)
         && --nb_tries);
*/
  cs2200_enter_test_mode();
}
#endif

#if CS2200_INTERFACE == CS2200_INTERFACE_TWI

void cs2200_read(uint8_t address, void *buffer, uint8_t len)
{
  twi_package_t twi_packet;
  int twi_status;

  do{
    twi_packet.chip        = CS2200_TWI_SLAVE_ADDRESS;         //! TWI chip address to communicate with.
    twi_packet.addr[0]     = address;                          //! Register address/commands inside the slave chip.
    twi_packet.addr_length = 1;                                //! Length of the TWI data address segment (1-3 bytes).
    twi_packet.buffer      = buffer;                           //! Where to find the data to be written.
    twi_packet.length      = len;                              //! How many bytes do we want to write.
    twi_status=twi_master_read(CS2200_TWI, &twi_packet);
  }
  while( twi_status != TWI_SUCCESS );
}

void cs2200_write(uint8_t address, const void *buffer, uint8_t len)
{
  twi_package_t twi_packet;
  int twi_status;

  do{
    twi_packet.chip        = CS2200_TWI_SLAVE_ADDRESS;         //! TWI chip address to communicate with.
    twi_packet.addr[0]     = address | 0x80;                   //! Register address/commands inside the slave chip + auto increment.
    twi_packet.addr_length = 1;                                //! Length of the TWI data address segment (1-3 bytes).
    twi_packet.buffer      = (void *)buffer;                   //! Where to find the data to be written.
    twi_packet.length      = len;                              //! How many bytes do we want to write.
    twi_status=twi_master_write(CS2200_TWI, &twi_packet);
  }
  while( twi_status != TWI_SUCCESS );
}


int cs2200_write_ex(uint8_t address, const void *buffer, uint8_t len)
{
  twi_package_t twi_packet;

  twi_packet.chip        = CS2200_TWI_SLAVE_ADDRESS;         //! TWI chip address to communicate with.
  twi_packet.addr[0]     = address | 0x80;                   //! Register address/commands inside the slave chip + auto increment.
  twi_packet.addr_length = 1;                                //! Length of the TWI data address segment (1-3 bytes).
  twi_packet.buffer      = (void *)buffer;                   //! Where to find the data to be written.
  twi_packet.length      = len;                              //! How many bytes do we want to write.
  return twi_master_write_ex(CS2200_TWI, &twi_packet);
}

#endif
