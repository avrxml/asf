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


#ifndef _MCI_H_
#define _MCI_H_

/**
 * \defgroup group_avr32_drivers_mci MCI - MultiMedia Card Interface
 *
 * This driver interfaces the MCI (MultiMedia Card Interface) module.
 * It will add functions for SD/MMC card reading, writing and management.
 *
 * \{
 */

#include <avr32/io.h>
#include "compiler.h"

#include "sd_mmc_cmd.h"

#if SD_MMC_MCI_ENABLE
#include "conf_sd_mmc_mci.h"
#else
#warning default value can be change
#define MCI_DEFAULT_SPEED_HZ      200000    //!> Default card speed in Hz
#define MCI_DEFAULT_PWSDIV          0x07    //!< Power Saving Divider.
#endif


//_____ D E F I N I T I O N S ______________________________________________

/*! \name Return Values
 */
//! @{
#define MCI_SUCCESS                 0 //!< Successful completion.
#define MCI_FAILURE                -1 //!< Failure because of some unspecified reason.
#define MCI_INVALID_INPUT           1 //!< Input value out of range.
//! @}

/*! \name Controller bus width
 */
//! @{
#define MCI_BUS_SIZE_1_BIT          0x00  //!< Bus Width Selected 1 bit.
#define MCI_BUS_SIZE_4_BIT          0x02  //!< Bus Width Selected 4 bits.
#define MCI_BUS_SIZE_8_BIT          0x03  //!< Bus Width Selected 8 bits.
//! @}

/*! \name Controller slot available
 */
//! @{
#define MCI_SLOT_A                  0x00        //!< Slot A Selected.
#define MCI_SLOT_B                  0x01        //!< Slot B Selected.
#define MCI_LAST_SLOTS              MCI_SLOT_B  //!< Last Slot Available by the MCI.
#define MCI_DEFAULT_SLOT            MCI_SLOT_A  //!< Default Slot Selected.
#define MCI_NR_SLOTS                2           //!< Number of MCI slots
//! @}

/*! \name Driver Default Value Configuration (should not be modified)
 */
//! @{
#define MCI_DEFAULT_DTOLMUL         0x07    //!< Data Timeout Multiplier.

#define MCI_DEFAULT_DTOLCYC         0x02    //!< Data Timeout Cycle Number.

#define MCI_DEFAULT_DTOREG        (MCI_DEFAULT_DTOLMUL<<AVR32_MCI_DTOR_DTOMUL_OFFSET) | \
                                  (MCI_DEFAULT_DTOLCYC<<AVR32_MCI_DTOR_DTOCYC_OFFSET)
//! @}

/*! \name CMDR Register bits definition
 */
//! @{

// Special Command
#define MCI_SPCMD_NONE                 (AVR32_MCI_CMDR_SPCMD_NO_SPEC_CMD<<AVR32_MCI_CMDR_SPCMD_OFFSET)  // (MCI) Not a special CMD
#define MCI_SPCMD_INIT                 (AVR32_MCI_CMDR_SPCMD_INIT_CMD   <<AVR32_MCI_CMDR_SPCMD_OFFSET)  // (MCI) Initialization CMD
#define MCI_SPCMD_SYNC                 (AVR32_MCI_CMDR_SPCMD_SYNC_CMD   <<AVR32_MCI_CMDR_SPCMD_OFFSET)  // (MCI) Synchronized CMD
#define MCI_SPCMD_IT_CMD               (AVR32_MCI_CMDR_SPCMD_INT_CMD    <<AVR32_MCI_CMDR_SPCMD_OFFSET)  // (MCI) Interrupt command
#define MCI_SPCMD_IT_REP               (AVR32_MCI_CMDR_SPCMD_INT_RESP   <<AVR32_MCI_CMDR_SPCMD_OFFSET)  // (MCI) Interrupt response
// Transfer Command
#define MCI_TRCMD_NO                   (AVR32_MCI_TRCMD_NO_TRANS   <<AVR32_MCI_TRCMD_OFFSET)            // (MCI) No transfer
#define MCI_TRCMD_START                (AVR32_MCI_TRCMD_START_TRANS<<AVR32_MCI_TRCMD_OFFSET)            // (MCI) Start transfer
#define MCI_TRCMD_STOP                 (AVR32_MCI_TRCMD_STOP_TRANS <<AVR32_MCI_TRCMD_OFFSET)            // (MCI) Stop transfer
// Response Type
#define MCI_RSPTYP_NO                  (AVR32_MCI_RSPTYP_NO_RESP              <<AVR32_MCI_RSPTYP_OFFSET)// (MCI) No response
#define MCI_RSPTYP_48                  (AVR32_MCI_RSPTYP_48_BIT_RESP          <<AVR32_MCI_RSPTYP_OFFSET)// (MCI) 48-bit response
#define MCI_RSPTYP_48B                 (AVR32_MCI_RSPTYP_48_BIT_RESP_WITH_BUSY<<AVR32_MCI_RSPTYP_OFFSET)// (MCI) 48-bit response with busy
#define MCI_RSPTYP_136                 (AVR32_MCI_RSPTYP_136_BIT_RESP         <<AVR32_MCI_RSPTYP_OFFSET)// (MCI) 136-bit response
// Transfer Type
#define MCI_TRTYP_BLOCK                (AVR32_MCI_TRTYP_BLOCK      <<AVR32_MCI_TRTYP_OFFSET)            // (MCI) Block Transfer type
#define MCI_TRTYP_MULTIPLE             (AVR32_MCI_TRTYP_MULTI_BLOCK<<AVR32_MCI_TRTYP_OFFSET)            // (MCI) Multiple Block transfer type
#define MCI_TRTYP_STREAM               (AVR32_MCI_TRTYP_STREAM     <<AVR32_MCI_TRTYP_OFFSET)            // (MCI) Stream transfer type
#define MCI_TRTYP_SDIO_BYTE            (AVR32_MCI_TRTYP_SDIO_BYTE  <<AVR32_MCI_TRTYP_OFFSET)            // (MCI) SDIO byte transfer type
#define MCI_TRTYP_SDIO_BLOCK           (AVR32_MCI_TRTYP_SDIO_BLOCK <<AVR32_MCI_TRTYP_OFFSET)            // (MCI) SDIO block transfer type

#define MCI_OPDCMD                     (AVR32_MCI_OPDCMD_MASK)                                          // (MCI) Open Drain Command
#define MCI_MAXLAT                     (AVR32_MCI_MAXLAT_MASK)                                          // (MCI) Maximum Latency for Command to respond
#define MCI_TRDIR                      (AVR32_MCI_TRDIR_MASK)                                           // (MCI) Transfer Direction
//! @}

extern int shadow_sr;

//_____ P R O T O T Y P E S ________________________________________________

/*! \brief Resets the MCI controller
 *
 * \param mci   Base address of the MCI instance.
 *
 * \retval
 */
extern void mci_reset(volatile avr32_mci_t *mci);

/*! \brief Disables the MCI controller
 *
 * \param mci   Base address of the MCI instance.
 *
 * \retval
 */
void mci_disable(volatile avr32_mci_t *mci);

/*! \brief Enables the MCI controller
 *
 * \param mci   Base address of the MCI instance.
 *
 * \retval
 */
void mci_enable(volatile avr32_mci_t *mci);

/*! \brief Return the status register and cache in a shadow variable bits
 *         that are cleared when reading the status register.
 *
 * \param mci        Base address of the MCI instance.
 */
__always_inline static int mci_get_sr(volatile avr32_mci_t *mci)
{
  shadow_sr = (shadow_sr & (AVR32_MCI_SR_DTOE_MASK | AVR32_MCI_SR_DCRCE_MASK | AVR32_MCI_SR_CSTOE_MASK | AVR32_MCI_SR_BLKOVRE_MASK)) | mci->sr;
  return shadow_sr;
}

/*! \brief Stop the MCI controller
 *
 * \param mci   Base address of the MCI instance.
 *
 * \retval
 */
extern void mci_stop(volatile avr32_mci_t *mci);

/*! \brief Initialize the MCI controller
 *
 * \param mci         Base address of the MCI instance.
 * \param card_slot   Card slot number
 * \param pbb_hz      PBB Frequency
 *
 * \retval MCI_SUCCESS  Initialization successfully done.
 * \retval MCI_INVALID_INPUT  One or more of the arguments is out of valid range.
 */
extern int mci_init( volatile avr32_mci_t *mci,
                     uint8_t card_slot,
                     uint32_t pbb_hz);

/*! \brief Set Speed Card
 *
 * \param mci         Base address of the MCI instance.
 * \param pbb_hz      PBB Frequency
 * \param card_speed  Card Frequency
 *
 * \retval MCI_SUCCESS  Initialization successfully done.
 * \retval MCI_INVALID_INPUT  One or more of the arguments is out of valid range.
 */
extern void mci_set_speed(volatile avr32_mci_t *mci,
                          uint32_t pbb_hz,
                          uint32_t card_speed);

/*! \brief Generic function to send a command to the MMC or SDCard.
 *
 * \param mci   Base address of the MCI instance.
 * \param cmd   Command Value.
 * \param arg   Argument Value.
 *
 * \retval MCI_SUCCESS  Command Successfully sent
 * \retval error  Error value
 */
extern int  mci_send_cmd (volatile avr32_mci_t *mci,
                          unsigned int cmd,
                          unsigned int arg);

/*! \brief Read command argument response.
 *
 * \param mci   Base address of the MCI instance.
 *
 * \retval argument  Return value of the previously command.
 */
extern unsigned int mci_read_response(volatile avr32_mci_t *mci);

/*! \brief Set SDIO Bus Width.
 *
 * \param mci   Base address of the MCI instance.
 * \param busWidth      Bus Width Value
 *
 * \retval MCI_SUCCESS  Command successfully done.
 * \retval MCI_INVALID_INPUT  One or more of the arguments is out of valid range.
 */
extern int mci_set_bus_size(volatile avr32_mci_t *mci, uint8_t busWidth);

/*! \brief Set Block Size.
 *
 * \param mci      Base address of the MCI instance.
 * \param length   Block Length Value
 *
 * \retval
 */
extern void mci_set_block_size(volatile avr32_mci_t *mci, uint16_t length);

/*! \brief Set Block Count.
 *
 * \param mci      Base address of the MCI instance.
 * \param length   Block Count Value
 *
 * \retval
 */
extern void mci_set_block_count(volatile avr32_mci_t *mci, uint16_t cnt);

/*! \brief Mci Busy Status.
 *
 * \param mci   Base address of the MCI instance.
 *
 * \retval
 */
extern void mci_wait_busy_signal(volatile avr32_mci_t *mci);

/*! \brief Mci Write data.
 *
 * \param mci   Base address of the MCI instance.
 * \param data  Data to write
 * \retval
 */
__always_inline static void mci_wr_data(volatile avr32_mci_t *mci,uint32_t data)
{
  mci->tdr = data;
}

/*! \brief Mci Read data.
 *
 * \param mci   Base address of the MCI instance.
 *
 * \retval data Data read
 */
__always_inline static uint32_t mci_rd_data(volatile avr32_mci_t *mci)
{
  return (mci->rdr);
}

/*! \brief Mci Select slot Card.
 *
 * \param mci        Base address of the MCI instance.
 * \param card_slot  Slot number.
 * \param bus_width  Bus Width .
 *
 * \retval data Data read
 */
extern int mci_select_card(volatile avr32_mci_t *mci, uint8_t card_slot, uint8_t bus_width);



/*! \brief Tests if the MCI is able to take a new command.
 *
 * \param mci        Base address of the MCI instance.
 *
 * \return \c 1 if the MCI can take a new command, otherwise \c 0.
 */
__always_inline static int mci_cmd_ready(volatile avr32_mci_t *mci)
{
  return ((mci_get_sr(mci)&AVR32_MCI_SR_CMDRDY_MASK) != 0);
}

/*! \brief Tests if the MCI is ready to transmit data.
 *
 * \param mci        Base address of the MCI instance.
 *
 * \return \c 1 if the MCI Transmit Holding Register is free, otherwise \c 0.
 */
__always_inline static int mci_tx_ready(volatile avr32_mci_t *mci)
{
  return ((mci_get_sr(mci)&AVR32_MCI_SR_TXRDY_MASK) != 0);
}

/*! \brief Tests if the MCI is ready to receive data.
 *
 * \param mci        Base address of the MCI instance.
 *
 * \return \c 1 if the MCI Receive Holding Register is free, otherwise \c 0.
 */
__always_inline static int mci_rx_ready(volatile avr32_mci_t *mci)
{
  return (mci_get_sr(mci)&AVR32_MCI_SR_RXRDY_MASK) != 0;
}

/*! \brief Tests if the MCI has ended the block Transmission/ Reception.
 *
 * \param mci        Base address of the MCI instance.
 *
 * \return \c 1 if the MCI Block is ended, otherwise \c 0.
 */
__always_inline static int mci_data_block_ended(volatile avr32_mci_t *mci)
{
  return (mci_get_sr(mci)&AVR32_MCI_SR_XFRDONE_MASK) != 0;
}

/*! \brief Tests if the MCI Status detects a CRC error.
 *
 * \param mci        Base address of the MCI instance.
 *
 * \return \c 1 if the MCI CRC Error Flag is set, otherwise \c 0.
 */
__always_inline static int mci_crc_error(volatile avr32_mci_t *mci)
{
  if( mci_get_sr(mci)&AVR32_MCI_SR_DCRCE_MASK )
  {
    shadow_sr&=~AVR32_MCI_SR_DCRCE_MASK;
    return true;
  }
  else
    return false;
}

/*! \brief Tests if an overrun occur.
 *
 * \param mci        Base address of the MCI instance.
 *
 * \return \c 1 if the MCI overrun flag is set, otherwise \c 0.
 */
__always_inline static int mci_overrun_error(volatile avr32_mci_t *mci)
{
  if( mci_get_sr(mci)&AVR32_MCI_SR_OVRE_MASK )
  {
    return true;
  }
  else
    return false;
}

/*! \brief Tests if an underrun occur.
 *
 * \param mci        Base address of the MCI instance.
 *
 * \return \c 1 if the MCI underrun flag is set, otherwise \c 0.
 */
__always_inline static int mci_underrun_error(volatile avr32_mci_t *mci)
{
  if( mci_get_sr(mci)&AVR32_MCI_SR_UNRE_MASK )
  {
    return true;
  }
  else
    return false;
}

/**
 * \}
 */

#endif // _MCI_H_
