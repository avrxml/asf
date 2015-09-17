/*****************************************************************************
 *
 * \file
 *
 * \brief SD/MMC card driver using MCI interface.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


//_____ I N C L U D E S ____________________________________________________

#include "conf_access.h"
#include <stdio.h>


#if SD_MMC_MCI_0_MEM == ENABLE || SD_MMC_MCI_1_MEM == ENABLE

#include "gpio.h"
#include "mci.h"
#include "conf_sd_mmc_mci.h"
#include "sd_mmc_mci.h"
#include "cycle_counter.h"

//_____ P R I V A T E   D E C L A R A T I O N S ____________________________

// Variable to manage the specification of the card
bool  sd_mmc_mci_init_done[MCI_NR_SLOTS] = {false, false};

uint8_t    g_card_type[      MCI_NR_SLOTS]; // Global Card Type
uint32_t   g_card_rca[       MCI_NR_SLOTS]; // Global RCA
uint32_t   g_card_size[      MCI_NR_SLOTS]; // Global Card Size
uint16_t   g_card_freq[      MCI_NR_SLOTS]; // Global Card Frequency
uint8_t    g_card_bus_width[ MCI_NR_SLOTS]; // Global Card Bus Width

  // These buffers should be dynamically allocated; may be in the sd_mmc driver initialization.
COMPILER_WORD_ALIGNED
uint8_t sector_buf_0[SD_MMC_SECTOR_SIZE];

COMPILER_WORD_ALIGNED
uint8_t sector_buf_1[SD_MMC_SECTOR_SIZE];

// Global Pbb Speed in case of reinitialization Launch
static uint32_t g_pbb_hz = 0;

// Global Cpu speed, needed by the cpu_delay() function
static uint32_t g_cpu_hz = 0;

// Variables to manage a standby/restart access
volatile uint32_t gl_ptr_mem[MCI_NR_SLOTS]={0, 0};

// MCI Instance
volatile avr32_mci_t *mci = &AVR32_MCI;

static bool  sd_mmc_mci_get_csd( uint8_t slot );
static bool  sd_mmc_get_ext_csd( uint8_t slot );
static bool  sd_mmc_set_block_len(uint8_t slot, uint16_t length );

#if ACCESS_USB == true

static bool is_dma_ram_2_mci_complete( void );
static void dma_ram_2_mci(const void *ram, size_t size);
static bool is_dma_mci_2_ram_complete( void );
static void dma_mci_2_ram(void *ram, size_t size);

#include "conf_usb.h"
#ifndef USB_DEVICE_VENDOR_ID
// USB Device Stack V1
static bool is_dma_usb_2_ram_complete( void );
static void dma_usb_2_ram(void *ram, size_t size);
static bool is_dma_ram_2_usb_complete( void );
static void dma_ram_2_usb(const void *ram, size_t size);
#endif

#endif


//_____ D E F I N I T I O N S ______________________________________________

bool is_sd_mmc_mci_card_present(uint8_t slot)
{
  if (slot > MCI_LAST_SLOTS)
    return false;

  switch(slot)
  {
    case MCI_SLOT_B:
        return (gpio_get_pin_value(SD_SLOT_4BITS_CARD_DETECT)==SD_SLOT_4BITS_CARD_DETECT_VALUE);
    case MCI_SLOT_A:
        return (gpio_get_pin_value(SD_SLOT_8BITS_CARD_DETECT)==SD_SLOT_8BITS_CARD_DETECT_VALUE);
  }

  return false;
}

bool is_sd_mmc_mci_card_protected(uint8_t slot)
{
  if (slot > MCI_LAST_SLOTS)
    return false;

  switch(slot)
  {
    case MCI_SLOT_B:
        return (gpio_get_pin_value(SD_SLOT_4BITS_WRITE_PROTECT)==SD_SLOT_4BITS_WRITE_PROTECT_VALUE);
    case MCI_SLOT_A:
        return (gpio_get_pin_value(SD_SLOT_8BITS_WRITE_PROTECT)==SD_SLOT_8BITS_WRITE_PROTECT_VALUE);
  }

  return false;
}

static bool  sd_mmc_mci_get_csd(uint8_t slot)
{
  csd_t csd;

  uint32_t max_Read_DataBlock_Length;
  uint32_t mult;
  uint32_t blocknr;
  uint8_t tmp;
  const uint16_t freq_unit[4] = {10, 100, 1000, 10000};
  const uint8_t mult_fact[16] = {0, 10, 12, 13, 15, 20, 26, 30, 35, 40, 45, 52, 55, 60, 70, 80}; // MMC tabs...

  if (slot > MCI_LAST_SLOTS)
    return false;
  // Select Slot card before any other command.
  mci_select_card(mci, slot, g_card_bus_width[slot]);

  //-- (CMD9)
  if (mci_send_cmd(mci, SD_MMC_SEND_CSD_CMD, g_card_rca[slot])!=MCI_SUCCESS)
    return false;


  csd.csd[0] = mci_read_response(mci);
  csd.csd[1] = mci_read_response(mci);
  csd.csd[2] = mci_read_response(mci);
  csd.csd[3] = mci_read_response(mci);

  //-- Read "System specification version", only available on MMC card
  // field: SPEC_VERS (only on MMC)
  if (MMC_CARD & g_card_type[slot]) // TO BE ADDED
  {
    if (CSD_SPEC_VER_4_0 == (MSB0(csd.csd[0]) & CSD_MSK_SPEC_VER))
    {
      g_card_type[slot] |= MMC_CARD_V4;
    }
  }

  //-- Compute MMC/SD speed
  // field: TRAN_SPEED (CSD V1 & V2 are the same)
  g_card_freq[slot]  = mult_fact[csd.csd_v1.tranSpeed >> 3]; // Get Multiplier factor
  if (SD_CARD & g_card_type[slot])
  {
    // SD card don't have same frequency that MMC card
    if( 26 == g_card_freq[slot] )
    {
      g_card_freq[slot] = 25;
    }
    else if( 52 == g_card_freq[slot] )
    {
      g_card_freq[slot] = 50;
    }
  }
  g_card_freq[slot] *= freq_unit[ csd.csd_v1.tranSpeed&0x07 ]; // Get transfer rate unit

  //-- Compute card size in number of block
  // field: WRITE_BL_LEN, READ_BL_LEN, C_SIZE (CSD V1 & V2 are not the same)
  if (SD_CARD_HC & g_card_type[slot])
  {
    g_card_size[slot] = (csd.csd_v2.deviceSizeH<<16)+(csd.csd_v2.deviceSizeL&0xFFff);

    // memory capacity = (C_SIZE+1) * 1K sector
    g_card_size[slot] = (g_card_size[slot] + 1) << 10;  // unit 512B
  }
  else
  {
    // Check block size
    tmp = csd.csd_v1.writeBlLen; // WRITE_BL_LEN
    if (tmp < CSD_BLEN_512)
      return false;  // block size < 512B not supported by firmware

    tmp = csd.csd_v1.readBlLen; // READ_BL_LEN
    if (tmp < CSD_BLEN_512)
      return false;  // block size < 512B not supported by firmware

    //// Compute Memory Capacity
    // compute MULT
    mult = 1 << (csd.csd_v1.cSizeMult + 2);
    max_Read_DataBlock_Length = 1<<csd.csd_v1.readBlLen;
    // compute MSB of C_SIZE
    blocknr = csd.csd_v1.deviceSizeH << 2;
    // compute MULT * (LSB of C-SIZE + MSB already computed + 1) = BLOCKNR
    blocknr = mult * ( blocknr + csd.csd_v1.deviceSizeL + 1 );
    g_card_size[slot] = ((max_Read_DataBlock_Length * blocknr)/512);
  }

  return true;
}

static bool  sd_mmc_get_ext_csd( uint8_t slot )
{
  uint8_t  i;
  uint32_t val;

  if (slot > MCI_LAST_SLOTS)
    return false;
  // Select Slot card before any other command.
  mci_select_card(mci, slot, g_card_bus_width[slot]);

  mci_set_block_size(mci, SD_MMC_SECTOR_SIZE); // Ext CSD = 512B size
  mci_set_block_count(mci, 1);

  //** (CMD8)
  // read the extended CSD
  if(mci_send_cmd(mci, SD_MMC_SEND_EXT_CSD_CMD, 0 )!=MCI_SUCCESS)
    return false;

  // READ_EXT_CSD   // discard bytes not used
  for (i = (512L/8); i!=0; i--)
  {
    while(!(mci_rx_ready(mci)));
    mci_rd_data(mci);
    while(!(mci_rx_ready(mci)));
    if( ((64-26) == i) && (g_card_type[slot]&MMC_CARD_HC) )
    {
      // If MMC HC then read Sector Count. Byte 212 is LSB, Byte 215 is MSB.
      val = mci_rd_data(mci);
      g_card_size[slot] = swap32(val);
    }
    else
    {
      val = mci_rd_data(mci);
      if( (64-24) == i )
      {  // Read byte at offset 196
        if( MSB0(val) & 0x02 )
          g_card_freq[slot] = 52*1000;
        else
          g_card_freq[slot] = 26*1000;
      }
    }
  }

   return true;
}

static bool  sd_mmc_set_block_len(uint8_t slot, uint16_t length )
{
  if (slot > MCI_LAST_SLOTS)
    return false;
  // Select Slot card before any other command.
  mci_select_card(mci, slot, g_card_bus_width[slot]);

  if(mci_send_cmd(mci, SD_MMC_SET_BLOCKLEN_CMD, length)!=MCI_SUCCESS)
    return false;

  // check response, card must be in TRAN state
  if ((mci_read_response(mci) & MMC_TRAN_STATE_MSK) != MMC_TRAN_STATE)
    return false;

  mci_set_block_size(mci, length);
  mci_set_block_count(mci, 1);
  return true;
}

bool sd_mmc_mci_init(unsigned char card_slot, long pbb_hz, long cpu_hz)
{
  union
  {
    unsigned long                 mcfg;
    avr32_hmatrix_mcfg_t          MCFG;
  } u_avr32_hmatrix_mcfg;

  union
  {
    unsigned long                 scfg;
    avr32_hmatrix_scfg_t          SCFG;
  } u_avr32_hmatrix_scfg;

  // For the USBB DMA HMATRIX master, use infinite length burst.
  u_avr32_hmatrix_mcfg.mcfg = AVR32_HMATRIX.mcfg[AVR32_HMATRIX_MASTER_USBB_DMA];
  u_avr32_hmatrix_mcfg.MCFG.ulbt = AVR32_HMATRIX_ULBT_INFINITE;
  AVR32_HMATRIX.mcfg[AVR32_HMATRIX_MASTER_USBB_DMA] = u_avr32_hmatrix_mcfg.mcfg;

  // For the USBB DPRAM HMATRIX slave, use the USBB DMA as fixed default master.
  u_avr32_hmatrix_scfg.scfg = AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_USBB_DPRAM];
  u_avr32_hmatrix_scfg.SCFG.fixed_defmstr = AVR32_HMATRIX_MASTER_USBB_DMA;
  u_avr32_hmatrix_scfg.SCFG.defmstr_type = AVR32_HMATRIX_DEFMSTR_TYPE_FIXED_DEFAULT;
  AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_USBB_DPRAM] = u_avr32_hmatrix_scfg.scfg;

  g_pbb_hz = pbb_hz;
  g_cpu_hz = cpu_hz;

  // Init MCI controller
  if (mci_init(mci, card_slot, pbb_hz)!=MCI_SUCCESS)
    return false;

  // Default card initialization. This can be removed since the card init will
  // automatically be done when needed.
  sd_mmc_mci_card_init(card_slot);
  return true;
}

bool sd_mmc_mci_card_init(unsigned char slot)
{
  uint32_t response;

  if (true == sd_mmc_mci_init_done[slot])
    return true;

  // Default card is not known.
  g_card_type[slot] = UNKNOWN_CARD;

  // Default bus size is 1 bit.
  g_card_bus_width[slot] = MCI_BUS_SIZE_1_BIT;

  // Default card speed and disable High Speed mode.
  mci_init(mci, slot, g_pbb_hz);

  // Wait for 1ms, then wait for 74 more clock cycles (see MMC norms)
  if (mci_send_cmd(mci, SD_MMC_INIT_STATE_CMD, 0xFFFFFFFF)!=MCI_SUCCESS)
    return false;

  //-- (CMD0)
  // Set card in idle state
  if (mci_send_cmd(mci, SD_MMC_GO_IDLE_STATE_CMD, 0xFFFFFFFF)!=MCI_SUCCESS)
    return false;

sd_mmc_init_step1:
  // (CMD1)
  // To send its Operating Conditions Register contents command only supported by MMC card
  if(mci_send_cmd(mci, SD_MMC_MMC_SEND_OP_COND_CMD, OCR_MSK_BUSY|OCR_MSK_VOLTAGE_ALL|OCR_MSK_HC)==MCI_SUCCESS)
  {
    // MMC cards always respond to MMC_SEND_OP_COND
    g_card_type[slot] = MMC_CARD;
    response = mci_read_response(mci);
    if( !(response & OCR_MSK_BUSY) )
    {
      // here card busy, it did not completed its initialization process
      // resend command MMC_SEND_OP_COND
      goto sd_mmc_init_step1;                   // loop until it is ready
    }
    if( 0!=(response & OCR_MSK_HC) )
    {
      g_card_type[slot] |= MMC_CARD_HC;
    }
  }
  else
  {
    // SD cards do not respond to MMC_SEND_OP_COND
    g_card_type[slot] = SD_CARD;

    //-- (CMD8)
    // enables to expand new functionality to some existing commands supported only by SD HC card
    if (mci_send_cmd(mci, SD_MMC_SD_SEND_IF_COND_CMD, 0x000001AA)==MCI_SUCCESS)
    {
      // It is a SD HC
      if( 0x000001AA == mci_read_response(mci))
      {
        g_card_type[slot] |= SD_CARD_V2;
      }
    }

sd_mmc_init_step2:
    //-- (CMD55)
    // Indicates to the card that the next command is an application specific command rather than a standard command
    // CMD55 shall always precede ACMD41
    if (mci_send_cmd(mci, SD_MMC_APP_CMD, 0)!=MCI_SUCCESS)
      return false;

    //-- (ACMD41)
    // Sends host OCR register
    if( SD_CARD_V2 & g_card_type[slot] )
    {
      // Sends host capacity support information (HCS)
      if (mci_send_cmd(mci, SD_MMC_SDCARD_APP_OP_COND_CMD, OCR_MSK_BUSY|OCR_MSK_VOLTAGE_3_2V_3_3V|OCR_MSK_HC)!=MCI_SUCCESS)
        return false;
    }
    else
    {
      if (mci_send_cmd(mci, SD_MMC_SDCARD_APP_OP_COND_CMD, OCR_MSK_BUSY|OCR_MSK_VOLTAGE_3_2V_3_3V)!=MCI_SUCCESS)
        return false;
    }
    response = mci_read_response(mci);

    if( !(response & OCR_MSK_BUSY) )
    {
      // Card Busy, resend ACMD41 precede of CMD55
      goto sd_mmc_init_step2;
    }
    // Card read then check HC type
    if (response & OCR_MSK_HC)
    {
      g_card_type[slot] |= SD_CARD_HC;  // Card SD High Capacity
    }
  }


  // Here card ready and type (MMC <V4, MMC V4, MMC HC, SD V1, SD V2 HC) detected

  //-- (CMD2)
  // Send CID
  if (mci_send_cmd(mci, SD_MMC_ALL_SEND_CID_CMD, 0)!=MCI_SUCCESS)
    return false;

  //-- (CMD3)
  // Set relative address
  if(MMC_CARD & g_card_type[slot])
  {
    // For MMC card, you send an address to card
    g_card_rca[slot] = RCA_DEFAULT_ADR;
    if(mci_send_cmd(mci, SD_MMC_SET_RELATIVE_ADDR_CMD, RCA_DEFAULT_ADR)!=MCI_SUCCESS)
      return false;
  }
  else
  {
    // For SD  card, you ask an address to card
    if(mci_send_cmd(mci, SD_MMC_SET_RELATIVE_ADDR_CMD, RCA_RESERVE_ADR)!=MCI_SUCCESS)
      return false;
  }
  if (SD_CARD & g_card_type[slot])
  {
    // For SD  card, you receive address of card
    g_card_rca[slot] = mci_read_response(mci) & RCA_MSK_ADR ;
  }


  //-- (CMD9)
  // Read & analyze CSD register
  if (sd_mmc_mci_get_csd(slot)!=true)
    return false;


  //-- (CMD7)-R1b
  // select card
  if (mci_send_cmd(mci, SD_MMC_SEL_DESEL_CARD_CMD, g_card_rca[slot])!=MCI_SUCCESS)
    return false;

  // Wait end of busy
  mci_wait_busy_signal(mci);// read busy state on DAT0

  // Get clock by checking the extended CSD register
  if (MMC_CARD_V4 & g_card_type[slot])
  {
    // Get clock (MMC V4) and size (MMC V4 HC) by checking the extended CSD register
      //-- (CMD8)
    if (sd_mmc_get_ext_csd(slot)!=true)
         return false;
  }


#if (SD_4_BIT == ENABLE)
  // set bus size
  if (SD_CARD & g_card_type[slot])
  {
    // set 4-bit bus for SD Card
    //-- (CMD55)
    if(mci_send_cmd(mci, SD_MMC_APP_CMD, g_card_rca[slot])!=MCI_SUCCESS)
      return false;

    //-- (CMD6)
    if(mci_send_cmd(mci, SD_MMC_SDCARD_SET_BUS_WIDTH_CMD, SD_BUS_4_BIT )!=MCI_SUCCESS)
      return false;

    g_card_bus_width[slot] = MCI_BUS_SIZE_4_BIT;
    if (mci_set_bus_size(mci, g_card_bus_width[slot])!=MCI_SUCCESS)
      return false;
  }
  else // MMC bus width management
  {
    // set 8-bit bus for MMC Card
    if (MMC_CARD_V4 & g_card_type[slot])
    {
      //-- (CMD6)-R1b
      // set 8-bit bus width (appeared from V4.0 specification)
      if (mci_send_cmd( mci,
                        MMC_SWITCH_CMD,
                        ( (uint32_t)MMC_SWITCH_WRITE     <<24)|
                          ((uint32_t)MMC_SWITCH_BUS_WIDTH<<16)|
                          ((uint32_t)MMC_SWITCH_VAL_8BIT << 8)|
                          ((uint32_t)MMC_SWITCH_CMD_SET))!=MCI_SUCCESS)
      {
        return false;
      }
      // Wait end of busy
      mci_wait_busy_signal(mci);// read busy state on DAT0
      g_card_bus_width[slot] = MCI_BUS_SIZE_8_BIT;
      if (mci_set_bus_size(mci, g_card_bus_width[slot])!=MCI_SUCCESS)
       return false;
    }
  }
#endif

   if (MMC_CARD_V4 & g_card_type[slot])
   {
      //-- (CMD6)-R1b
      // set high speed interface timing
      if (mci_send_cmd( mci,
                      MMC_SWITCH_CMD,
                        ((uint32_t)MMC_SWITCH_WRITE     <<24)|
                        ((uint32_t)MMC_SWITCH_HIGH_SPEED<<16)|
                        ((uint32_t)MMC_SWITCH_VAL_HS    << 8)|
                        ((uint32_t)MMC_SWITCH_CMD_SET))!=MCI_SUCCESS)
      {
        return false;
      }
      // Wait end of busy
      mci_wait_busy_signal(mci);
   }


   if( SD_CARD_V2 & g_card_type[slot] )
   {
/** \brief Switch func argument definitions */
#define SDMMC_SWITCH_FUNC_MODE_CHECK  (0 << 31)   /**< Check function */
#define SDMMC_SWITCH_FUNC_MODE_SWITCH (1 << 31)   /**< Switch function */
#define SDMMC_SWITCH_FUNC_HIGH_SPEED  (1 << 0)    /**< High Speed */
#define SDMMC_SWITCH_FUNC_G1_KEEP     (0xf << 0)  /**< Group 1 No influence */
#define SDMMC_SWITCH_FUNC_G2_KEEP     (0xf << 4)  /**< Group 2 No influence */
#define SDMMC_SWITCH_FUNC_G3_KEEP     (0xf << 8)  /**< Group 3 No influence */
#define SDMMC_SWITCH_FUNC_G4_KEEP     (0xf << 12) /**< Group 4 No influence */
#define SDMMC_SWITCH_FUNC_G5_KEEP     (0xf << 16) /**< Group 5 No influence */
#define SDMMC_SWITCH_FUNC_G6_KEEP     (0xf << 20) /**< Group 6 No influence */

      mci_set_block_size(mci, 512/8); // CMD6 512 bits status
      mci_set_block_count(mci, 1);

      //-- (CMD6)
      // Check if we can enter into the High Speed mode.
      if (mci_send_cmd( mci
                      , SD_SWITCH_FUNC
                      , SDMMC_SWITCH_FUNC_MODE_CHECK | SDMMC_SWITCH_FUNC_HIGH_SPEED
                      )!=MCI_SUCCESS)
      {
        return false;
      }
      // Wait end of busy
      mci_wait_busy_signal(mci);// read busy state on DAT0

      bool b_hs_supported=false;
      {
        uint8_t i;
        for ( i = 0; i<(512L/8); i+=4)
        {
          volatile uint32_t data;
          while(!(mci_rx_ready(mci)));
          data = mci_rd_data(mci);
          if(i==16)
          {
            if(((data>>24)&0xf)==1)
              b_hs_supported = true;
            break;
          }
        }
      }

      if (b_hs_supported==false )
        goto sd_mmc_init_step3;

      if (mci_send_cmd( mci
                      , SD_SWITCH_FUNC
                      , SDMMC_SWITCH_FUNC_MODE_SWITCH
                      | SDMMC_SWITCH_FUNC_G6_KEEP
                      | SDMMC_SWITCH_FUNC_G5_KEEP
                      | SDMMC_SWITCH_FUNC_G4_KEEP
                      | SDMMC_SWITCH_FUNC_G3_KEEP
                      | SDMMC_SWITCH_FUNC_G2_KEEP
                      | SDMMC_SWITCH_FUNC_HIGH_SPEED
                      )!=MCI_SUCCESS)
      {
        return false;
      }
      {
        uint8_t i;
        for ( i = 0; i<(512L/8); i+=4)
        {
          volatile uint32_t data;
          while(!(mci_rx_ready(mci)));
          data = mci_rd_data(mci);
        }
      }

      /* A 8 cycle delay is required after switch command
       * @ 200KHz clock this should be 40 uS, but we use
       * 80 to handle imprecise clock setting.
       */
      cpu_delay_us(80, g_cpu_hz);

      union u_cfg{
        unsigned long     cfg;
        avr32_mci_cfg_t   CFG;
      };
      union u_cfg val;
      val.cfg = mci->cfg;
      val.CFG.hsmode = 1;
      mci->cfg = val.cfg;

      // deselect card
      if (mci_send_cmd(mci, SD_MMC_SEL_DESEL_CARD_CMD, 0)!=MCI_SUCCESS)
        return false;

      // Wait end of busy
      mci_wait_busy_signal(mci);// read busy state on DAT0

      //-- (CMD9)
      // Read & analyze CSD register
      if (sd_mmc_mci_get_csd(slot)!=true)
        return false;

      // select card
      if (mci_send_cmd(mci, SD_MMC_SEL_DESEL_CARD_CMD, g_card_rca[slot])!=MCI_SUCCESS)
        return false;

      // Wait end of busy
      mci_wait_busy_signal(mci);// read busy state on DAT0
  }

sd_mmc_init_step3:
  // Set clock
  mci_set_speed(mci, g_pbb_hz, g_card_freq[slot]*1000);

  //-- (CMD13)
  // Check if card is ready, the card must be in TRAN state
  if(sd_mmc_mci_cmd_send_status(slot)!=true)
    return false;

  if ((mci_read_response(mci) & MMC_TRAN_STATE_MSK) != MMC_TRAN_STATE)
    return false;

  //-- (CMD16)
  // Set the card block length to 512B
  if (sd_mmc_set_block_len (slot, SD_MMC_SECTOR_SIZE)!=true)
    return false;

  // USB Test Unit Attention requires a state "busy" between "not present" and "ready" state
  // otherwise never report card change
  sd_mmc_mci_init_done[slot] = true;

  return true;
}

bool sd_mmc_mci_read_sector_2_ram(uint8_t slot, void *ram)
{
  uint32_t wordsToRead;
  int *pRam = ram;

  // Read data
  wordsToRead = (SD_MMC_SECTOR_SIZE/sizeof(*pRam));
  while(wordsToRead>0)
  {
    while(!(mci_rx_ready(mci)));
    *pRam++ = mci_rd_data(mci);
    while(!(mci_rx_ready(mci)));
    *pRam++ = mci_rd_data(mci);
    while(!(mci_rx_ready(mci)));
    *pRam++ = mci_rd_data(mci);
    while(!(mci_rx_ready(mci)));
    *pRam++ = mci_rd_data(mci);
    while(!(mci_rx_ready(mci)));
    *pRam++ = mci_rd_data(mci);
    while(!(mci_rx_ready(mci)));
    *pRam++ = mci_rd_data(mci);
    while(!(mci_rx_ready(mci)));
    *pRam++ = mci_rd_data(mci);
    while(!(mci_rx_ready(mci)));
    *pRam++ = mci_rd_data(mci);
    wordsToRead-=8;
  }

  return true;
}

bool sd_mmc_mci_dma_read_sector_2_ram(uint8_t slot, void *ram)
{
  int *pRam = ram;

  // Src Address: the MCI registers.
  AVR32_DMACA.sar1 = (uint32_t)&AVR32_MCI.fifo;

  // Dst Address: the OutputData[] array.
  AVR32_DMACA.dar1 = (unsigned long)pRam;

  // Channel 1 Ctrl register high
  AVR32_DMACA.ctl1h =
    ( (SD_MMC_SECTOR_SIZE/4) << AVR32_DMACA_CTL1H_BLOCK_TS_OFFSET) // Block transfer size
    ;

  // Enable Channel 1 : start the process.
  AVR32_DMACA.chenreg = ((2<<AVR32_DMACA_CHENREG_CH_EN_OFFSET) | (2<<AVR32_DMACA_CHENREG_CH_EN_WE_OFFSET));

  // Wait for the end of the AES->RAM transfer (channel 1).
  while(AVR32_DMACA.chenreg & (2<<AVR32_DMACA_CHENREG_CH_EN_OFFSET));

  return true;
}

bool sd_mmc_mci_read_multiple_sector_2_ram(uint8_t slot, void *ram, uint32_t nb_sector)
{
  uint32_t wordsToRead;
  int *pRam = ram;

  // Read data
  while( nb_sector>0 )
  {
    wordsToRead = (SD_MMC_SECTOR_SIZE/sizeof(*pRam));
    while(wordsToRead>0)
    {
        while(!(mci_rx_ready(mci)));
        *pRam++ = mci_rd_data(mci);
        while(!(mci_rx_ready(mci)));
        *pRam++ = mci_rd_data(mci);
        while(!(mci_rx_ready(mci)));
        *pRam++ = mci_rd_data(mci);
        while(!(mci_rx_ready(mci)));
        *pRam++ = mci_rd_data(mci);
        while(!(mci_rx_ready(mci)));
        *pRam++ = mci_rd_data(mci);
        while(!(mci_rx_ready(mci)));
        *pRam++ = mci_rd_data(mci);
        while(!(mci_rx_ready(mci)));
        *pRam++ = mci_rd_data(mci);
        while(!(mci_rx_ready(mci)));
        *pRam++ = mci_rd_data(mci);
        wordsToRead-=8;
    }
    nb_sector--;
  }

  return true;
}

#if ACCESS_USB == true

#ifndef USB_DEVICE_VENDOR_ID
// USB Device Stack V1
#include "usb_drv.h"
#include "scsi_decoder.h"
static void dma_ram_2_usb(const void *ram, size_t size)
{
  AVR32_USBB_UDDMAX_nextdesc(g_scsi_ep_ms_in) = (uint32_t)NULL;
  AVR32_USBB_UDDMAX_addr(g_scsi_ep_ms_in) = (uint32_t)ram;
  AVR32_USBB_UDDMAX_control(g_scsi_ep_ms_in) = ((size << AVR32_USBB_UXDMAX_CONTROL_CH_BYTE_LENGTH_OFFSET)
                                           & AVR32_USBB_UXDMAX_CONTROL_CH_BYTE_LENGTH_MASK)
                                         //| AVR32_USBB_UXDMAX_CONTROL_BURST_LOCK_EN_MASK
                                         //| AVR32_USBB_UXDMAX_CONTROL_DMAEND_EN_MASK
                                         //| AVR32_USBB_UXDMAX_CONTROL_BUFF_CLOSE_IN_EN_MASK
                                         | AVR32_USBB_UXDMAX_CONTROL_CH_EN_MASK;

  // Workaround for bug 3501.
  (void)AVR32_USBB_UDDMAX_control(g_scsi_ep_ms_in);
}

static bool is_dma_ram_2_usb_complete( void )
{
  if(AVR32_USBB_UDDMAX_status(g_scsi_ep_ms_in) & AVR32_USBB_UXDMAX_STATUS_CH_EN_MASK)
    return false;
  else
    return true;
}
#endif

static void dma_mci_2_ram(void *ram, size_t size)
{
  int *pRam = ram;

  // Src Address: the MCI registers.
  AVR32_DMACA.sar1 = (uint32_t)&AVR32_MCI.fifo;

  // Dst Address: the OutputData[] array.
  AVR32_DMACA.dar1 = (unsigned long)pRam;

  // Channel 1 Ctrl register high
  AVR32_DMACA.ctl1h =
    ( (size/4) << AVR32_DMACA_CTL1H_BLOCK_TS_OFFSET) // Block transfer size
    ;

  // Enable Channel 1 : start the process.
  AVR32_DMACA.chenreg = ((2<<AVR32_DMACA_CHENREG_CH_EN_OFFSET) | (2<<AVR32_DMACA_CHENREG_CH_EN_WE_OFFSET));
}

static bool is_dma_mci_2_ram_complete( void )
{
  // Wait for the end of the AES->RAM transfer (channel 1).
  if (AVR32_DMACA.chenreg & (2<<AVR32_DMACA_CHENREG_CH_EN_OFFSET))
    return false;

  return true;
}
#endif

bool sd_mmc_mci_dma_read_multiple_sector_2_ram(uint8_t slot, void *ram, uint32_t nb_sector)
{
  int *pRam = ram;

  // Src Address: the MCI registers.
  AVR32_DMACA.sar1 = (uint32_t)&AVR32_MCI.fifo;

  // Dst Address: the OutputData[] array.
  AVR32_DMACA.dar1 = (unsigned long)pRam;

  // Channel 1 Ctrl register high
  AVR32_DMACA.ctl1h =
    ( (nb_sector*(SD_MMC_SECTOR_SIZE/4)) << AVR32_DMACA_CTL1H_BLOCK_TS_OFFSET) // Block transfer size
    ;

  // Enable Channel 1 : start the process.
  AVR32_DMACA.chenreg = ((2<<AVR32_DMACA_CHENREG_CH_EN_OFFSET) | (2<<AVR32_DMACA_CHENREG_CH_EN_WE_OFFSET));

  // Wait for the end of the AES->RAM transfer (channel 1).
  while(AVR32_DMACA.chenreg & (2<<AVR32_DMACA_CHENREG_CH_EN_OFFSET));

  return true;
}

bool sd_mmc_mci_write_sector_from_ram(uint8_t slot, const void *ram)
{
  uint32_t wordsToWrite;
  const unsigned int *pRam = ram;

  // Write Data
  wordsToWrite = (SD_MMC_SECTOR_SIZE/sizeof(*pRam));
  while(wordsToWrite>0)
  {
    while(!mci_tx_ready(mci));
    mci_wr_data(mci,*pRam++);
    while(!mci_tx_ready(mci));
    mci_wr_data(mci,*pRam++);
    while(!mci_tx_ready(mci));
    mci_wr_data(mci,*pRam++);
    while(!mci_tx_ready(mci));
    mci_wr_data(mci,*pRam++);
    while(!mci_tx_ready(mci));
    mci_wr_data(mci,*pRam++);
    while(!mci_tx_ready(mci));
    mci_wr_data(mci,*pRam++);
    while(!mci_tx_ready(mci));
    mci_wr_data(mci,*pRam++);
    while(!mci_tx_ready(mci));
    mci_wr_data(mci,*pRam++);
    wordsToWrite-=8;
  }

  return true;
}

bool sd_mmc_mci_dma_write_sector_from_ram(uint8_t slot, const void *ram)
{
  const unsigned int *pRam = ram;

  // Dst Address: the OutputData[] array.
  AVR32_DMACA.sar1 = (unsigned long)pRam;

  // Src Address: the MCI registers.
  AVR32_DMACA.dar1 = (uint32_t)&AVR32_MCI.fifo;

  // Channel 1 Ctrl register high
  AVR32_DMACA.ctl1h =
    ( (SD_MMC_SECTOR_SIZE/4) << AVR32_DMACA_CTL1H_BLOCK_TS_OFFSET) // Block transfer size
    ;

  // Enable Channel 1 : start the process.
  AVR32_DMACA.chenreg = ((2<<AVR32_DMACA_CHENREG_CH_EN_OFFSET) | (2<<AVR32_DMACA_CHENREG_CH_EN_WE_OFFSET));

  // Wait for the end of the AES->RAM transfer (channel 1).
  while(AVR32_DMACA.chenreg & (2<<AVR32_DMACA_CHENREG_CH_EN_OFFSET));

  return true;
}

bool sd_mmc_mci_write_multiple_sector_from_ram(uint8_t slot, const void *ram, uint32_t nb_sector)
{
  uint32_t wordsToWrite;
  const unsigned int *pRam = ram;

  // Write Data
  while( nb_sector>0 )
  {
    wordsToWrite = (SD_MMC_SECTOR_SIZE/sizeof(*pRam));
    while(wordsToWrite>0)
    {
      while(!mci_tx_ready(mci));
      mci_wr_data(mci,*pRam++);
      while(!mci_tx_ready(mci));
      mci_wr_data(mci,*pRam++);
      while(!mci_tx_ready(mci));
      mci_wr_data(mci,*pRam++);
      while(!mci_tx_ready(mci));
      mci_wr_data(mci,*pRam++);
      while(!mci_tx_ready(mci));
      mci_wr_data(mci,*pRam++);
      while(!mci_tx_ready(mci));
      mci_wr_data(mci,*pRam++);
      while(!mci_tx_ready(mci));
      mci_wr_data(mci,*pRam++);
      while(!mci_tx_ready(mci));
      mci_wr_data(mci,*pRam++);
      wordsToWrite-=8;
    }
    nb_sector--;
  }

  return true;
}

#if ACCESS_USB == true

#ifndef USB_DEVICE_VENDOR_ID
// USB Device Stack V1
#include "usb_drv.h"
#include "scsi_decoder.h"
static void dma_usb_2_ram(void *ram, size_t size)
{
  AVR32_USBB_UDDMAX_nextdesc(g_scsi_ep_ms_out) = (uint32_t)NULL;
  AVR32_USBB_UDDMAX_addr(g_scsi_ep_ms_out) = (uint32_t)ram;
  AVR32_USBB_UDDMAX_control(g_scsi_ep_ms_out) = ((size << AVR32_USBB_UXDMAX_CONTROL_CH_BYTE_LENGTH_OFFSET)
                                           & AVR32_USBB_UXDMAX_CONTROL_CH_BYTE_LENGTH_MASK)
                                         //| AVR32_USBB_UXDMAX_CONTROL_BURST_LOCK_EN_MASK
                                         //| AVR32_USBB_UXDMAX_CONTROL_DMAEND_EN_MASK
                                         //| AVR32_USBB_UXDMAX_CONTROL_BUFF_CLOSE_IN_EN_MASK
                                         | AVR32_USBB_UXDMAX_CONTROL_CH_EN_MASK;

  // Workaround for bug 3501.
  (void)AVR32_USBB_UDDMAX_control(g_scsi_ep_ms_out);
}

static bool is_dma_usb_2_ram_complete( void )
{
  if(AVR32_USBB_UDDMAX_status(g_scsi_ep_ms_out) & AVR32_USBB_UXDMAX_STATUS_CH_EN_MASK)
    return false;
  else
    return true;
}
#endif

static void dma_ram_2_mci(const void *ram, size_t size)
{
  const unsigned int *pRam = ram;

  // Dst Address: the OutputData[] array.
  AVR32_DMACA.sar1 = (unsigned long)pRam;

  // Src Address: the MCI registers.
  AVR32_DMACA.dar1 = (uint32_t)&AVR32_MCI.fifo;

  // Channel 1 Ctrl register high
  AVR32_DMACA.ctl1h =
    ( (size/4) << AVR32_DMACA_CTL1H_BLOCK_TS_OFFSET) // Block transfer size
    ;

  // Enable Channel 1 : start the process.
  AVR32_DMACA.chenreg = ((2<<AVR32_DMACA_CHENREG_CH_EN_OFFSET) | (2<<AVR32_DMACA_CHENREG_CH_EN_WE_OFFSET));
}

static bool is_dma_ram_2_mci_complete( void )
{
  // Wait for the end of the AES->RAM transfer (channel 1).
  if (AVR32_DMACA.chenreg & (2<<AVR32_DMACA_CHENREG_CH_EN_OFFSET))
    return false;

  return true;
}
#endif

bool sd_mmc_mci_dma_write_multiple_sector_from_ram(uint8_t slot, const void *ram, uint32_t nb_sector)
{
  const unsigned int *pRam = ram;

  // Dst Address: the OutputData[] array.
  AVR32_DMACA.sar1 = (unsigned long)pRam;

  // Src Address: the MCI registers.
  AVR32_DMACA.dar1 = (uint32_t)&AVR32_MCI.fifo;

  // Channel 1 Ctrl register high
  AVR32_DMACA.ctl1h =
    ( (nb_sector*(SD_MMC_SECTOR_SIZE/4)) << AVR32_DMACA_CTL1H_BLOCK_TS_OFFSET) // Block transfer size
    ;

  // Enable Channel 1 : start the process.
  AVR32_DMACA.chenreg = ((2<<AVR32_DMACA_CHENREG_CH_EN_OFFSET) | (2<<AVR32_DMACA_CHENREG_CH_EN_WE_OFFSET));

  // Wait for the end of the AES->RAM transfer (channel 1).
  while(AVR32_DMACA.chenreg & (2<<AVR32_DMACA_CHENREG_CH_EN_OFFSET));

  return true;
}

bool sd_mmc_mci_mem_check(uint8_t slot)
{
  uint8_t timeout_init = 0;

  if (slot > MCI_LAST_SLOTS)
    return false;

  // Select Slot card before any other command.
  mci_select_card(mci, slot, g_card_bus_width[slot]);

  // Check card presence
  if (is_sd_mmc_mci_card_present(slot) == false)
  {
    sd_mmc_mci_init_done[slot] = false;
    return false;
  }

  if (sd_mmc_mci_init_done[slot] == false)
  {
    while (sd_mmc_mci_card_init(slot)!=true)
    {
      timeout_init++;
      if (timeout_init>10) return false;
    }
  }
  if (sd_mmc_mci_init_done[slot] == true)
    return true;
  else
    return false;
}

bool sd_mmc_mci_read_open (uint8_t slot, uint32_t pos, uint16_t nb_sector)
{
  uint32_t addr;

  if (slot > MCI_LAST_SLOTS)
    return false;

  // Select Slot card before any other command.
  mci_select_card(mci, slot, g_card_bus_width[slot]);

  // Set the global memory ptr at a Byte address.
  gl_ptr_mem[slot] = pos ;

  // wait for MMC not busy
  mci_wait_busy_signal(mci);

  addr =  gl_ptr_mem[slot];
  // send command
  if((!(SD_CARD_HC  & g_card_type[slot]))
  && (!(MMC_CARD_HC & g_card_type[slot])) )
  {
    addr <<= 9; // For NO HC card the address must be translate in byte address
  }

  //** (CMD13)
  // Necessary to clear flag error "ADDRESS_OUT_OF_RANGE" (ID LABO = MMC15)
  if(mci_send_cmd(mci, SD_MMC_SEND_STATUS_CMD, g_card_rca[slot])!=MCI_SUCCESS)
  {
    return false;
  }

  // Request Block Length
  mci_set_block_size(mci, SD_MMC_SECTOR_SIZE);

  // Set Block Count
  mci_set_block_count(mci, nb_sector);

  //** (CMD17)
  if(mci_send_cmd(mci, SD_MMC_READ_MULTIPLE_BLOCK_CMD, addr)!=MCI_SUCCESS)
  {
    return false;
  }

  // check response
  if ((mci_read_response(mci) & CS_FLAGERROR_RD_WR) != 0)
  {
    return false;
  }

  return true;
}

bool sd_mmc_mci_dma_read_open(uint8_t slot, uint32_t pos, void* ram, uint16_t nb_sector)
{
  uint32_t addr;

  if (slot > MCI_LAST_SLOTS)
    return false;

  // Select Slot card before any other command.
  mci_select_card(mci, slot, g_card_bus_width[slot]);

  // Set the global memory ptr at a Byte address.
  gl_ptr_mem[slot] = pos ;

  // wait for MMC not busy
  mci_wait_busy_signal(mci);

  addr =  gl_ptr_mem[slot];
  // send command
  if((!(SD_CARD_HC  & g_card_type[slot]))
  && (!(MMC_CARD_HC & g_card_type[slot])) )
  {
    addr <<= 9; // For NO HC card the address must be translate in byte address
  }

  //** (CMD13)
  // Necessary to clear flag error "ADDRESS_OUT_OF_RANGE" (ID LABO = MMC15)
  if(mci_send_cmd(mci, SD_MMC_SEND_STATUS_CMD, g_card_rca[slot])!=MCI_SUCCESS)
  {
    return false;
  }

  // Request Block Length
  mci_set_block_size(mci, SD_MMC_SECTOR_SIZE);

  // Set Block Count
  mci_set_block_count(mci, nb_sector);

  // Enable the DMACA
  AVR32_DMACA.dmacfgreg = 1 << AVR32_DMACA_DMACFGREG_DMA_EN_OFFSET;

  AVR32_MCI.dma = 0;

  // Linked list ptrs: not used.
  AVR32_DMACA.llp1 = 0x00000000;

  // Channel 1 Ctrl register low
  AVR32_DMACA.ctl1l =
    (0                                            << AVR32_DMACA_CTL1L_INT_EN_OFFSET)       | // Do not enable interrupts
    (2                                            << AVR32_DMACA_CTL1L_DST_TR_WIDTH_OFFSET) | // Dst transfer width: 32 bits
    (2                                            << AVR32_DMACA_CTL1L_SRC_TR_WIDTH_OFFSET) | // Src transfer width: 32 bits
    (0                                            << AVR32_DMACA_CTL1L_DINC_OFFSET)         | // Dst address increment: increment
    (0                                            << AVR32_DMACA_CTL1L_SINC_OFFSET)         | // Src address increment: increment
    (3                                            << AVR32_DMACA_CTL1L_DST_MSIZE_OFFSET)    | // Dst burst transaction len: 16 data items
    (3                                            << AVR32_DMACA_CTL1L_SRC_MSIZE_OFFSET)    | // Src burst transaction len: 16 data items
    (0                                            << AVR32_DMACA_CTL1L_S_GATH_EN_OFFSET)    | // Source gather: disabled
    (0                                            << AVR32_DMACA_CTL1L_D_SCAT_EN_OFFSET)    | // Destination scatter: disabled
    (2                                            << AVR32_DMACA_CTL1L_TT_FC_OFFSET)        | // transfer type:P2M, flow controller: DMACA
    (1                                            << AVR32_DMACA_CTL1L_DMS_OFFSET)          | // Dest master: HSB master 2
    (0                                            << AVR32_DMACA_CTL1L_SMS_OFFSET)          | // Source master: HSB master 1
    (0                                            << AVR32_DMACA_CTL1L_LLP_D_EN_OFFSET)     | // Not used
    (0                                            << AVR32_DMACA_CTL1L_LLP_S_EN_OFFSET)       // Not used
    ;

  // Channel 1 Config register low
  AVR32_DMACA.cfg1l =
    (0                                            << AVR32_DMACA_CFG1L_HS_SEL_DST_OFFSET) | // Destination handshaking: ignored because the dst is memory.
    (0                                            << AVR32_DMACA_CFG1L_HS_SEL_SRC_OFFSET)   // Source handshaking: hw handshaking
    ; // All other bits set to 0.

  // Channel 1 Config register high
  AVR32_DMACA.cfg1h =
    (0                      << AVR32_DMACA_CFG1H_DEST_PER_OFFSET) | // Dest hw handshaking itf: ignored because the dst is memory.
    (AVR32_DMACA_CH_MMCI_RX << AVR32_DMACA_CFG1H_SRC_PER_OFFSET)    // Source hw handshaking itf:
    ; // All other bits set to 0.

  // Setup MCI DMA register
  AVR32_MCI.dma = AVR32_MCI_DMA_DMAEN_MASK | (AVR32_MCI_DMA_CHKSIZE_16_BYTES << AVR32_MCI_DMA_CHKSIZE_OFFSET);

  //** (CMD17)
  if(mci_send_cmd(mci, SD_MMC_READ_MULTIPLE_BLOCK_CMD, addr)!=MCI_SUCCESS)
  {
    return false;
  }

  // check response
  if ((mci_read_response(mci) & CS_FLAGERROR_RD_WR) != 0)
  {
    return false;
  }

  return true;
}

bool sd_mmc_mci_read_close (uint8_t slot)
{
  if( (mci_crc_error(mci)) )
  {
    return false;  // An CRC error has been seen
  }

  mci_wait_busy_signal(mci);

  if( mci_send_cmd( mci, SD_MMC_STOP_READ_TRANSMISSION_CMD, 0xffffffff ) != MCI_SUCCESS)
    return false;

  /*
  if( (mci_overrun_error(mci)) )
  {
    return false;
  }

  if( (mci_underrun_error(mci)) )
  {
    return false;
  }*/

  return true;
}

bool sd_mmc_mci_write_open (uint8_t slot, uint32_t pos, uint16_t nb_sector)
{
  uint32_t addr;

  if (slot > MCI_LAST_SLOTS)
    return false;

  // Select Slot card before any other command.
  mci_select_card(mci,slot,g_card_bus_width[slot]);

  // Set the global memory ptr at a Byte address.
  gl_ptr_mem[slot] = pos ;

  // wait for MMC not busy
  mci_wait_busy_signal(mci);

   /*
   // (CMD13)
   // Necessary to clear flag error "ADDRESS_OUT_OF_RANGE" (ID LABO = MMC15)
   if( !mmc_drv_send_cmd( MMC_SEND_STATUS, g_card_rca, MMC_RESP_R1 ) )
   {
      return false;
   }
   mmc_drv_read_response();
   */

  addr = gl_ptr_mem[slot];
  // send command
  if((!(SD_CARD_HC  & g_card_type[slot]))
  && (!(MMC_CARD_HC & g_card_type[slot])) )
  {
    addr <<= 9; // For NO HC card the address must be translate in byte address
  }

  // Set Block Length
  mci_set_block_size(mci, SD_MMC_SECTOR_SIZE);

  // Set Block Count
  mci_set_block_count(mci, nb_sector);

  //** (CMD24)
  if(mci_send_cmd(mci, SD_MMC_WRITE_MULTIPLE_BLOCK_CMD, addr )!=MCI_SUCCESS)
  {
    return false;
  }

  // check response
  if ((mci_read_response(mci) & CS_FLAGERROR_RD_WR) != 0)
  {
    return false;
  }

  return true;
}

bool sd_mmc_mci_dma_write_open (uint8_t slot, uint32_t pos, const void* ram, uint16_t nb_sector)
{
  uint32_t addr;

  if (slot > MCI_LAST_SLOTS)
    return false;

  // Select Slot card before any other command.
  mci_select_card(mci, slot, g_card_bus_width[slot]);

  // Set the global memory ptr at a Byte address.
  gl_ptr_mem[slot] = pos ;

  // wait for MMC not busy
  mci_wait_busy_signal(mci);

   /*
   // (CMD13)
   // Necessary to clear flag error "ADDRESS_OUT_OF_RANGE" (ID LABO = MMC15)
   if( !mmc_drv_send_cmd( MMC_SEND_STATUS, g_card_rca, MMC_RESP_R1 ) )
   {
      return false;
   }
   mmc_drv_read_response();
   */

  addr = gl_ptr_mem[slot];
  // send command
  if((!(SD_CARD_HC  & g_card_type[slot]))
  && (!(MMC_CARD_HC & g_card_type[slot])) )
  {
    addr <<= 9; // For NO HC card the address must be translate in byte address
  }

  // Set Block Length
  mci_set_block_size(mci, SD_MMC_SECTOR_SIZE);

  // Set Block Count
  mci_set_block_count(mci, nb_sector);

  // Enable the DMACA
  AVR32_DMACA.dmacfgreg = 1 << AVR32_DMACA_DMACFGREG_DMA_EN_OFFSET;

  AVR32_MCI.dma = 0;

  // Linked list ptrs: not used.
  AVR32_DMACA.llp1 = 0x00000000;

  // Channel 1 Ctrl register low
  AVR32_DMACA.ctl1l =
    (0                                            << AVR32_DMACA_CTL1L_INT_EN_OFFSET)       | // Do not enable interrupts
    (2                                            << AVR32_DMACA_CTL1L_DST_TR_WIDTH_OFFSET) | // Dst transfer width: 32 bits
    (2                                            << AVR32_DMACA_CTL1L_SRC_TR_WIDTH_OFFSET) | // Src transfer width: 32 bits
    (0                                            << AVR32_DMACA_CTL1L_DINC_OFFSET)         | // Dst address increment: increment
    (0                                            << AVR32_DMACA_CTL1L_SINC_OFFSET)         | // Src address increment: increment
    (3                                            << AVR32_DMACA_CTL1L_DST_MSIZE_OFFSET)    | // Dst burst transaction len: 16 data items
    (3                                            << AVR32_DMACA_CTL1L_SRC_MSIZE_OFFSET)    | // Src burst transaction len: 16 data items
    (0                                            << AVR32_DMACA_CTL1L_S_GATH_EN_OFFSET)    | // Source gather: disabled
    (0                                            << AVR32_DMACA_CTL1L_D_SCAT_EN_OFFSET)    | // Destination scatter: disabled
    (1                                            << AVR32_DMACA_CTL1L_TT_FC_OFFSET)        | // transfer type:M2P, flow controller: DMACA
    (0                                            << AVR32_DMACA_CTL1L_DMS_OFFSET)          | // Dest master: HSB master 1
    (1                                            << AVR32_DMACA_CTL1L_SMS_OFFSET)          | // Source master: HSB master 2
    (0                                            << AVR32_DMACA_CTL1L_LLP_D_EN_OFFSET)     | // Not used
    (0                                            << AVR32_DMACA_CTL1L_LLP_S_EN_OFFSET)       // Not used
    ;

  // Channel 1 Config register low
  AVR32_DMACA.cfg1l =
    (0                            << AVR32_DMACA_CFG1L_HS_SEL_DST_OFFSET) | // Destination handshaking: hw handshaking
    (0                            << AVR32_DMACA_CFG1L_HS_SEL_SRC_OFFSET)   // Source handshaking: ignored because the dst is memory.
    ; // All other bits set to 0.

  // Channel 1 Config register high
  AVR32_DMACA.cfg1h =
    (AVR32_DMACA_CH_MMCI_TX << AVR32_DMACA_CFG1H_DEST_PER_OFFSET) | // Dest hw handshaking itf:
    (0                      << AVR32_DMACA_CFG1H_SRC_PER_OFFSET)    // Source hw handshaking itf: ignored because the dst is memory.
    ; // All other bits set to 0.

  // Setup MCI DMA register
  AVR32_MCI.dma = AVR32_MCI_DMA_DMAEN_MASK | (AVR32_MCI_DMA_CHKSIZE_16_BYTES << AVR32_MCI_DMA_CHKSIZE_OFFSET);

  //** (CMD24)
  if(mci_send_cmd(mci, SD_MMC_WRITE_MULTIPLE_BLOCK_CMD, addr )!=MCI_SUCCESS)
  {
    return false;
  }

  // check response
  if ((mci_read_response(mci) & CS_FLAGERROR_RD_WR) != 0)
  {
    return false;
  }

  return true;
}

bool sd_mmc_mci_write_close (uint8_t slot)
{
  if( (mci_crc_error(mci)) )
  {
    return false;  // An CRC error has been seen
  }

  while(!(mci_data_block_ended(mci)));

  if( mci_send_cmd( mci, SD_MMC_STOP_WRITE_TRANSMISSION_CMD, 0xffffffff ) != MCI_SUCCESS)
  {
    return false;
  }

  /*
  if( (mci_overrun_error(mci)) )
  {
    return false;
  }

  if( (mci_underrun_error(mci)) )
  {
    return false;
  }*/

  if( slot==SD_SLOT_4BITS )
  {
    while(!(gpio_get_pin_value(SD_SLOT_4BITS_DATA0_PIN)));                             // Wait until the card is ready.
  }
  else
  {
    while(!(gpio_get_pin_value(SD_SLOT_8BITS_DATA0_PIN)));                             // Wait until the card is ready.
  }
  return true;
}

#if ACCESS_USB == true

#ifdef USB_DEVICE_VENDOR_ID
// USB Device Stack V2
#include "udi_msc.h"

bool sd_mmc_mci_read_multiple_sector(uint8_t slot, uint16_t nb_sector)
{
  bool b_first_step=true;
  uint8_t   buffer_id=0;

  // Pipeline the 2 DMA transfer in order to speed-up the performances:
  // DMA MCI -> RAM
  // DMA RAM -> USB
  //
  while (nb_sector--) {
    // (re)load first stage.
    dma_mci_2_ram((0==(buffer_id++%2))?&sector_buf_0:&sector_buf_1, SD_MMC_SECTOR_SIZE);

    // (re)load second stage.
    if( !b_first_step ) {
      if (!udi_msc_trans_block(true, (0==(buffer_id%2))?sector_buf_0:sector_buf_1, SD_MMC_SECTOR_SIZE, NULL)) {
    	    return false;
      }

    }
    b_first_step = false;
    // Wait completion of DMACA stage.
    while( !is_dma_mci_2_ram_complete() );
  }

  // Complete execution of the last transfer (which is in the pipe).
  if (!udi_msc_trans_block(true, (0!=(buffer_id%2))?sector_buf_0:sector_buf_1, SD_MMC_SECTOR_SIZE, NULL))
    return false;

  return true;
}

bool sd_mmc_mci_write_multiple_sector(uint8_t slot, uint16_t nb_sector)
{
  bool b_first_step=true;
  uint8_t   buffer_id=0;

  // Pipeline the 2 DMA transfer in order to speed-up the performances:
  // DMA USB -> RAM
  // DMA RAM -> MCI
  //
  while (nb_sector--) {

    // (re)load second stage.
    if( !b_first_step ) {
      dma_ram_2_mci((0!=(buffer_id%2))?&sector_buf_0:&sector_buf_1, SD_MMC_SECTOR_SIZE);
    }
    udi_msc_trans_block(false, (0==(buffer_id++%2))?sector_buf_0:sector_buf_1, SD_MMC_SECTOR_SIZE, NULL);
    if( !b_first_step ) {
      // Wait completion of DMACA stage.
      while( !is_dma_ram_2_mci_complete() );
    }
    b_first_step=false;
  }

  // Complete execution of the last transfer (which is in the pipe).
  dma_ram_2_mci((0!=(buffer_id%2))?&sector_buf_0:&sector_buf_1, SD_MMC_SECTOR_SIZE);
  while( !is_dma_ram_2_mci_complete() );
  return true;
}

#else
// USB Device Stack V1

#include "usb_drv.h"
#include "scsi_decoder.h"

bool sd_mmc_mci_read_multiple_sector(uint8_t slot, uint16_t nb_sector)
{
  bool b_last_state_full=false;
  uint8_t   buffer_id=0;

  // Turn on features used by the DMA-USB.
  Usb_enable_endpoint_bank_autoswitch(g_scsi_ep_ms_in);

  // Pipeline the 2 DMA transfer in order to speed-up the performances:
  // DMA MCI -> RAM
  // DMA RAM -> USB
  //
  while (nb_sector--) {
    // (re)load first stage.
    if( buffer_id==0 ) {
      dma_mci_2_ram(&sector_buf_0, SD_MMC_SECTOR_SIZE);
      buffer_id=1;

    } else {
      dma_mci_2_ram(&sector_buf_1, SD_MMC_SECTOR_SIZE);
      buffer_id=0;
    }

    // (re)load second stage.
    if( b_last_state_full ) {
      if( buffer_id==0 ) {
        dma_ram_2_usb(&sector_buf_0, SD_MMC_SECTOR_SIZE);
      } else {
        dma_ram_2_usb(&sector_buf_1, SD_MMC_SECTOR_SIZE);
      }
      // Wait completion of both stages.
      while( !is_dma_mci_2_ram_complete() );
      while( !is_dma_ram_2_usb_complete() );

    } else {
      b_last_state_full=true;
      // Wait completion of first stage only.
      while( !is_dma_mci_2_ram_complete() );
    }
  }

  // Complete execution of the last transfer (which is in the pipe).
  if( buffer_id==0 ) {
    dma_ram_2_usb(&sector_buf_1, SD_MMC_SECTOR_SIZE);
  } else {
    dma_ram_2_usb(&sector_buf_0, SD_MMC_SECTOR_SIZE);
  }
  while( !is_dma_ram_2_usb_complete() );

  // Wait until the USB RAM is empty before disabling the autoswitch feature.
  while( Usb_nb_busy_bank(g_scsi_ep_ms_in)!=0 );

  // Turn off exotic USB features that may not work for other devices (at45dbx...)
  Usb_disable_endpoint_bank_autoswitch(g_scsi_ep_ms_in);
  return true;
}

bool sd_mmc_mci_write_multiple_sector(uint8_t slot, uint16_t nb_sector)
{
  bool b_last_state_full=false;
  uint8_t   buffer_id=0;

  // Turn on features used by the DMA-USB.
  Usb_enable_endpoint_bank_autoswitch(g_scsi_ep_ms_out);

  // Pipeline the 2 DMA transfer in order to speed-up the performances:
  // DMA USB -> RAM
  // DMA RAM -> MCI
  //
  while (nb_sector--) {
    // (re)load first stage.
    if( buffer_id==0 ) {
      dma_usb_2_ram(&sector_buf_0, SD_MMC_SECTOR_SIZE);
      buffer_id=1;

    } else {
      dma_usb_2_ram(&sector_buf_1, SD_MMC_SECTOR_SIZE);
      buffer_id=0;
    }

    // (re)load second stage.
    if( b_last_state_full ) {
      if( buffer_id==0 ) {
        dma_ram_2_mci(&sector_buf_0, SD_MMC_SECTOR_SIZE);
      } else {
        dma_ram_2_mci(&sector_buf_1, SD_MMC_SECTOR_SIZE);
      }
      // Wait completion of both stages.
      while( !is_dma_usb_2_ram_complete() );
      while( !is_dma_ram_2_mci_complete() );

    } else {
      b_last_state_full=true;
      // Wait completion of the first stage only.
      while( !is_dma_usb_2_ram_complete() );
    }
  }

  // Complete execution of the last transfer (which is in the pipe).
  if( buffer_id==0 ) {
    dma_ram_2_mci(&sector_buf_1, SD_MMC_SECTOR_SIZE);
  } else {
    dma_ram_2_mci(&sector_buf_0, SD_MMC_SECTOR_SIZE);
  }
  while( !is_dma_ram_2_mci_complete() );

  // Wait until the USB RAM is empty before disabling the autoswitch feature.
  while( Usb_nb_busy_bank(g_scsi_ep_ms_out)!=0 );

  // Turn off exotic USB features that may not work for other devices (at45dbx...)
  Usb_disable_endpoint_bank_autoswitch(g_scsi_ep_ms_out);
  return true;
}
#endif

#endif

#if (defined MMC_CARD_SECU_FUNC) && (MMC_CARD_SECU_FUNC == ENABLE)

bool sd_mmc_mci_lock_unlock (uint8_t slot, uint8_t cmd, uint8_t pwd_len, uint8_t * password) // password length in Bytes (possible values : 2,6,14)
{
   uint8_t block_count;

  if (slot > MCI_LAST_SLOTS)
    return false;

  // Select Slot card before any other command.
  mci_select_card(mci, slot, g_card_bus_width[slot]);

  // Wait Busy Signal
  mci_wait_busy_signal(mci);

  if (false == sd_mmc_set_block_len (slot, (uint8_t) pwd_len+2))
  {
    return false;
  }

   // Send Lock/Unlock Command
   mci_send_cmd(mci, SD_MMC_LOCK_UNLOCK, 0);

   // check response
   if ((mci_read_response(mci) & MMC_TRAN_STATE_MSK) != MMC_TRAN_STATE)
   {
      return false;
   }

   // Sends the command
   mci_wr_data(mci, cmd);
   // Sends the data
   if (cmd != CMD_FULL_ERASE)
   {
      mci_wr_data(mci, pwd_len); // PWD_LENGTH
   }
   for ( block_count = 0 ; block_count < pwd_len ; block_count++) //PWD
   {
      mci_wr_data(mci, *(password+block_count));
   }

   sd_mmc_set_block_len(slot,SD_MMC_SECTOR_SIZE);

   return true;
}

bool is_sd_mmc_mci_locked(uint8_t slot)
{
   uint32_t response;

  if (slot > MCI_LAST_SLOTS)
    return false;
  // Select Slot card before any other command.
  mci_select_card(mci, slot, g_card_bus_width[slot]);

   // ask status
   if (sd_mmc_mci_cmd_send_status(slot)==false)
      return true;
   response = mci_read_response(mci);
   if ((((uint8_t)(response>>24))&0x02)!=0x00)
   { // Then it is locked
      return true;
   }
   return false;
}

bool sd_mmc_mci_lock_unlock_failed(uint8_t slot)
{
   uint32_t response;

  if (slot > MCI_LAST_SLOTS)
    return false;
  // Select Slot card before any other command.
  mci_select_card(mci, slot, g_card_bus_width[slot]);

   // ask status
   if (sd_mmc_mci_cmd_send_status(slot)==false)
      return true;
   response = mci_read_response(mci);
   if ((((Byte)(response>>24))&0x01)!=0x00)
   {  // Then it failed
      return false;
   }
   return true;
}
#endif      // end FUNC_MMC_CARD_SECU

bool  sd_mmc_mci_cmd_send_status(uint8_t slot)
{
  if (slot > MCI_LAST_SLOTS)
    return false;
  // Select Slot card before any other command.
  mci_select_card(mci, slot, g_card_bus_width[slot]);

  if (mci_send_cmd(mci, SD_MMC_SEND_STATUS_CMD, g_card_rca[slot])!=MCI_SUCCESS)
    return false;

  return true;
}

#endif  // SD_MMC_MCI_0_MEM == ENABLE || SD_MMC_MCI_1_MEM == ENABLE
