/*****************************************************************************
 *
 * \file
 *
 * \brief MCI driver for AVR32 UC3.
 *
 * This file contains basic functions for the AVR32 MCI, with support for all
 * modes, settings and clock speeds.
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

#include "mci.h"

//! Global Error Mask
#define MCI_SR_ERROR		        ( AVR32_MCI_SR_UNRE_MASK  |\
                                  AVR32_MCI_SR_OVRE_MASK  |\
                                  AVR32_MCI_SR_DTOE_MASK  |\
                                  AVR32_MCI_SR_DCRCE_MASK |\
                                  AVR32_MCI_SR_RTOE_MASK  |\
                                  AVR32_MCI_SR_RENDE_MASK |\
                                  AVR32_MCI_SR_RCRCE_MASK |\
                                  AVR32_MCI_SR_RDIRE_MASK |\
                                  AVR32_MCI_SR_RINDE_MASK)


int shadow_sr=0;

void mci_reset(volatile avr32_mci_t *mci)
{
  mci->cr = (1<<AVR32_MCI_CR_SWRST);
}

void mci_disable(volatile avr32_mci_t *mci)
{
  // Disable the MCI
  mci->cr = (1<<AVR32_MCI_CR_MCIDIS) | (1<<AVR32_MCI_CR_PWSDIS);
}

void mci_enable(volatile avr32_mci_t *mci)
{
  // Enable the MCI
  mci->cr = (1<<AVR32_MCI_CR_MCIEN) | (1<<AVR32_MCI_CR_PWSEN);
}

void mci_stop(volatile avr32_mci_t *mci)
{
  // Disable all the interrupts
  mci->idr = 0xffffffff;

  // Reset the MCI
  mci_reset(mci);

  // Disable the MCI
  mci_disable(mci);
}

void mci_set_speed(volatile avr32_mci_t *mci,
                   uint32_t pbb_hz,
                   uint32_t card_speed)
{
  uint32_t mci_mode_register;
  uint32_t clkdiv;
  uint32_t rest;
  union u_cfg{
    uint32_t          cfg;
    avr32_mci_cfg_t   CFG;
  };
  union u_cfg val;

  // Get the Mode Register
  mci_mode_register = mci->mr;

  if (card_speed > AVR32_MCI_HSDIS_MAX_FREQ)
  { // Use of the High Speed mode of the MCI macro.
    val.cfg = mci->cfg;
    val.CFG.hsmode = 1;
    mci->cfg = val.cfg;
  }

  // Multimedia Card Interface clock (MCCK or MCI_CK) is Master Clock (MCK)
  // divided by (2*(CLKDIV+1))
  if (card_speed > 0)
  {
    clkdiv = pbb_hz / (card_speed * 2);
    rest   = pbb_hz % (card_speed * 2);
    if (rest)
    { // Ensure that the card_speed can not be higher than expected.
      clkdiv += 1;
    }

    if (clkdiv > 0)
    {
      clkdiv -= 1;
    }
  }
  else
  {
    clkdiv = 0;
  }

  // Write new configuration
  mci_mode_register &= ~AVR32_MCI_MR_CLKDIV_MASK; // Clear previous value
  mci_mode_register |= clkdiv; // Set the new one
  mci->mr = mci_mode_register;
}

int mci_init(volatile avr32_mci_t *mci,
              uint8_t card_slot,
              uint32_t pbb_hz)
{
  if (card_slot>MCI_LAST_SLOTS)
    return MCI_INVALID_INPUT;

  // Initialize all bits of the shadow status register.
  shadow_sr=0;

  // Reset the MCI
  mci_reset(mci);

  // Disable the MCI
  mci_disable(mci);

  // Disable all the interrupts
  mci->idr = 0xffffffff;

  // Setup configuration register
  mci->cfg = 0;

  // Clear Mode register
  mci->mr = 0;

  // Set the Data Timeout Register to 1 Mega Cycles
  mci->dtor = (MCI_DEFAULT_DTOREG);

  // Set the Mode Register
  mci_set_speed(mci, pbb_hz, MCI_DEFAULT_SPEED_HZ);
  mci->mr |= ((MCI_DEFAULT_PWSDIV<<AVR32_MCI_MR_PWSDIV) | AVR32_MCI_MR_RDPROOF_MASK | AVR32_MCI_MR_WRPROOF_MASK);

  // Set the SD/MMC Card Register
  mci->sdcr = (MCI_BUS_SIZE_1_BIT>>AVR32_MCI_SDCR_SDCBUS_OFFSET)|(card_slot>>AVR32_MCI_SDCR_SDCSEL_OFFSET);

  // Enable the MCI and the Power Saving
  mci_enable(mci);

  return MCI_SUCCESS;
}

void mci_set_block_size(volatile avr32_mci_t *mci,
                        uint16_t length)
{
  uint32_t mci_mode_register;

  mci_mode_register = mci->mr;
  mci_mode_register &= ~AVR32_MCI_MR_BLKLEN_MASK; // Clear previous BLKLEN
  mci_mode_register |= (length<<AVR32_MCI_MR_BLKLEN_OFFSET); // Set the new value

  mci->mr = mci_mode_register;
}

void mci_set_block_count(volatile avr32_mci_t *mci,
                         uint16_t cnt)
{
  union u_blkr{
    uint32_t          blkr;
    avr32_mci_blkr_t  BLKR;
  };
  union u_blkr val;

  val.blkr = mci->blkr;
  val.BLKR.bcnt = cnt;
  mci->blkr = val.blkr;
}

int mci_send_cmd(volatile avr32_mci_t *mci,
                 unsigned int cmd,
                 unsigned int arg)
{
  unsigned int	error;

  // Send the command
  mci->argr = arg;
  mci->cmdr = cmd;

  // wait for CMDRDY Status flag to read the response
  while( !(mci_cmd_ready(mci)) );

  // Test error  ==> if crc error and response R3 ==> don't check error
  error = mci_get_sr(mci) & MCI_SR_ERROR;
  if(error != 0 )
  {
    // if the command is SEND_OP_COND the CRC error flag is always present (cf : R3 response)
    if ( (cmd != SD_MMC_SDCARD_APP_OP_COND_CMD) && (cmd != SD_MMC_MMC_SEND_OP_COND_CMD))
    {
      if( error != AVR32_MCI_SR_RTOE_MASK )
        // filter RTOE error which happens when using the HS mode
        return error;
    }
    else
    {
      if (error != AVR32_MCI_SR_RCRCE_MASK)
        return error;
    }
  }
  return MCI_SUCCESS;

}

int mci_set_bus_size(volatile avr32_mci_t *mci,
                     uint8_t busWidth)
{
  uint32_t mci_sdcr_register;

  if (busWidth > MCI_BUS_SIZE_8_BIT)
    return MCI_INVALID_INPUT;

  mci_sdcr_register = mci->sdcr;
  mci_sdcr_register &= ~AVR32_MCI_SDCR_SDCBUS_MASK; // Clear previous buswidth
  mci_sdcr_register |= (busWidth<<AVR32_MCI_SDCR_SDCBUS_OFFSET);
  mci->sdcr = mci_sdcr_register;

  return MCI_SUCCESS;
}


unsigned int mci_read_response(volatile avr32_mci_t *mci)
{
  return mci->rspr0;
}

void mci_wait_busy_signal(volatile avr32_mci_t *mci)
{
  while(!(mci_get_sr(mci)&AVR32_MCI_SR_NOTBUSY_MASK));
}

int mci_select_card(volatile avr32_mci_t *mci,
                    uint8_t card_slot,
                    uint8_t bus_width)
{
  if (card_slot > MCI_LAST_SLOTS)
    return MCI_INVALID_INPUT;

  // Select the card slot and bus width
  mci->sdcr = (bus_width<<AVR32_MCI_SDCR_SDCBUS_OFFSET) | (card_slot<<AVR32_MCI_SDCR_SDCSEL_OFFSET);

  return MCI_SUCCESS;
}
