/**************************************************************************
 *
 * \file
 *
 * \brief Management of the virtual memory.
 *
 * This file manages the virtual memory.
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
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


//_____  I N C L U D E S ___________________________________________________

#include "conf_access.h"


#if VIRTUAL_MEM == ENABLE

#include "virtual_mem.h"


//_____ M A C R O S ________________________________________________________


//_____ P R I V A T E   D E C L A R A T I O N S ____________________________


//_____ D E F I N I T I O N S ______________________________________________

static uint8_t vmem_data[VMEM_NB_SECTOR * VMEM_SECTOR_SIZE];

static bool cram_init = false;


//_____ D E C L A R A T I O N S ____________________________________________

//! This function initializes the memory
static void virtual_check_init(void)
{
  uint8_t i;

  if (cram_init) return;

  // PBR sector init
  // Offset 0
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +   0] = 0xEB;  // JMP inst to PBR boot code
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +   1] = 0x3C;
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +   2] = 0x90;

  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +   3] = 'A'; // OEM name
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +   4] = 'T';
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +   5] = 'M';
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +   6] = 'E';
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +   7] = 'L';
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +   8] = ' ';
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +   9] = ' ';
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  10] = ' ';

  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  11] = (uint8_t)VMEM_SECTOR_SIZE;            // Bytes per sector
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  12] = VMEM_SECTOR_SIZE >> 8;
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  13] = VMEM_CLUSTER_SIZE;               // Sectors per cluster
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  14] = (uint8_t)VMEM_RESERVED_SIZE;          // Number of reserved sectors
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  15] = VMEM_RESERVED_SIZE >> 8;
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  16] = VMEM_NB_FATS;                    // Number of FATs
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  17] = (uint8_t)VMEM_NB_ROOT_ENTRY;          // Number of root directory entries
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  18] = VMEM_NB_ROOT_ENTRY >> 8;
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  19] = (uint8_t)VMEM_NB_SECTOR;
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  20] = VMEM_NB_SECTOR >> 8;
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  21] = VMEM_MEDIA_TYPE;                 // Media type
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  22] = (uint8_t)VMEM_SIZE_FAT;               // FAT size
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  23] = VMEM_SIZE_FAT >> 8;
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  24] = (uint8_t)VMEM_SECT_PER_TRACK;         // Sectors per track
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  25] = VMEM_SECT_PER_TRACK >> 8;
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  26] = (uint8_t)VMEM_NB_HEAD;                // Number of heads
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  27] = VMEM_NB_HEAD >> 8;
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  28] = (uint8_t)VMEM_NB_HIDDEN_SECT;         // Number of hidden sectors
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  29] = (uint8_t)(VMEM_NB_HIDDEN_SECT >> 8);
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  30] = (uint8_t)(VMEM_NB_HIDDEN_SECT >> 16);
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  31] = VMEM_NB_HIDDEN_SECT >> 24;
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  32] = 0x00;                            // Number of sectors for FAT 32 only
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  33] = 0x00;
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  34] = 0x00;
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  35] = 0x00;
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  36] = VMEN_DRIVE_NUMBER;               // Driver number
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  37] = 0x00;                            // Reserved (it must be 0x00, set to 0x01 by Windows after error)
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  38] = 0x29;                            // Extended boot signature
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  39] = 0x00;                            // Volume ID
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  40] = 0x00;
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  41] = 0x00;
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  42] = 0x00;

  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  43] = 'V'; // Volume Label
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  44] = 'I';
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  45] = 'R';
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  46] = 'T';
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  47] = 'U';
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  48] = 'A';
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  49] = 'L';
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  50] = ' ';
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  51] = 'M';
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  52] = 'E';
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  53] = 'M';

  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  54] = 'F'; // File System Type in ASCII
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  55] = 'A';
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  56] = 'T';
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  57] = '1';
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  58] = '2';
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  59] = ' ';
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  60] = ' ';
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE +  61] = ' ';

  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE + 510] = 0x55;
  vmem_data[PBR_SECTOR * VMEM_SECTOR_SIZE + 511] = 0xAA;
  // End of PBR

  for (i = 0; i < VMEM_NB_FATS; i++)
  {
    // FAT sector init
    vmem_data[(FAT_SECTOR + i * VMEM_SIZE_FAT) * VMEM_SECTOR_SIZE + 0] = 0xF8;
    vmem_data[(FAT_SECTOR + i * VMEM_SIZE_FAT) * VMEM_SECTOR_SIZE + 1] = 0xFF;
    vmem_data[(FAT_SECTOR + i * VMEM_SIZE_FAT) * VMEM_SECTOR_SIZE + 2] = 0xFF;  // Other sectors set to 0x00
  }

  cram_init = true;
}


//! This function tests memory state, and starts memory initialization
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status virtual_test_unit_ready(void)
{
  virtual_check_init();

  return CTRL_GOOD;
}


//! This function returns the address of the last valid sector
//! @param u32_nb_sector  Pointer to number of sectors (sector = 512 bytes)
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status virtual_read_capacity(uint32_t *u32_nb_sector)
{
  virtual_check_init();
  *u32_nb_sector = Max(VMEM_NB_SECTOR, 8) - 1;

  return CTRL_GOOD;
}


//! This function returns the write-protected mode
//! Only used by memory removal with a HARDWARE-SPECIFIC write-protected detection
//! @warning The customer must unplug the memory to change this write-protected mode.
//! @return true if the memory is protected
bool virtual_wr_protect(void)
{
  return false;
}


//! This function informs about the memory type
//! @return true if the memory is removable
bool virtual_removal(void)
{
  return false;
}


//------------ SPECIFIC FUNCTIONS FOR TRANSFER BY USB --------------------------

#if ACCESS_USB == true

#include "usb_drv.h"
#include "scsi_decoder.h"

//! This function transfers the memory data (programmed in sbc_read_10) directly to the USB interface
//! sector = 512 bytes
//! @param addr         Sector address to start read
//! @param nb_sector    Number of sectors to transfer
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status virtual_usb_read_10(uint32_t addr, uint16_t nb_sector)
{
  const void *ptr_cram;
  uint16_t data_to_transfer;

  virtual_check_init();
  if (addr + nb_sector > Max(VMEM_NB_SECTOR, 8)) return CTRL_FAIL;

  while (nb_sector--)
  {
    // If overflow (possible with size virtual mem < 8 sectors) then read the last sector
    addr = min(addr, VMEM_NB_SECTOR - 1);

    ptr_cram = &vmem_data[addr++ * VMEM_SECTOR_SIZE];
    data_to_transfer = VMEM_SECTOR_SIZE;
    while (data_to_transfer)
    {
      while (!Is_usb_in_ready(g_scsi_ep_ms_in))
      {
         if(!Is_usb_endpoint_enabled(g_scsi_ep_ms_in))
            return CTRL_FAIL; // USB Reset
      }

      Usb_reset_endpoint_fifo_access(g_scsi_ep_ms_in);
      data_to_transfer = usb_write_ep_txpacket(g_scsi_ep_ms_in, ptr_cram,
                                               data_to_transfer, &ptr_cram);
      Usb_ack_in_ready_send(g_scsi_ep_ms_in);
    }
  }

  return CTRL_GOOD;
}


//! This function transfers the USB data (programmed in sbc_write_10) directly to the memory interface
//! sector = 512 bytes
//! @param addr         Sector address to start write
//! @param nb_sector    Number of sectors to transfer
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status virtual_usb_write_10(uint32_t addr, uint16_t nb_sector)
{
  void *ptr_cram;
  uint16_t data_to_transfer;

  virtual_check_init();
  if (addr + nb_sector > VMEM_NB_SECTOR) return CTRL_FAIL;

  ptr_cram = &vmem_data[addr * VMEM_SECTOR_SIZE];
  while (nb_sector--)
  {
    data_to_transfer = VMEM_SECTOR_SIZE;
    while (data_to_transfer)
    {
      while (!Is_usb_out_received(g_scsi_ep_ms_out))
      {
         if(!Is_usb_endpoint_enabled(g_scsi_ep_ms_out))
            return CTRL_FAIL; // USB Reset
      }

      Usb_reset_endpoint_fifo_access(g_scsi_ep_ms_out);
      data_to_transfer = usb_read_ep_rxpacket(g_scsi_ep_ms_out, ptr_cram,
                                              data_to_transfer, &ptr_cram);
      Usb_ack_out_received_free(g_scsi_ep_ms_out);
    }
  }

  return CTRL_GOOD;
}

#endif  // ACCESS_USB == true


//------------ SPECIFIC FUNCTIONS FOR TRANSFER BY RAM --------------------------

#if ACCESS_MEM_TO_RAM == true

#include <string.h>

//! This function transfers 1 data sector from memory to RAM
//! sector = 512 bytes
//! @param addr         Sector address to start read
//! @param ram          Address of RAM buffer
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status virtual_mem_2_ram(uint32_t addr, void *ram)
{
  virtual_check_init();
  if (addr + 1 > Max(VMEM_NB_SECTOR, 8)) return CTRL_FAIL;

  // If overflow (possible with size virtual mem < 8 sectors) then read the last sector
  addr = min(addr, VMEM_NB_SECTOR - 1);

  memcpy(ram, &vmem_data[addr * VMEM_SECTOR_SIZE], VMEM_SECTOR_SIZE);

  return CTRL_GOOD;
}


//! This function transfers 1 data sector from memory to RAM
//! sector = 512 bytes
//! @param addr         Sector address to start write
//! @param ram          Address of RAM buffer
//! @return                            Ctrl_status
//!   It is ready                ->    CTRL_GOOD
//!   Memory unplug              ->    CTRL_NO_PRESENT
//!   Not initialized or changed ->    CTRL_BUSY
//!   An error occurred          ->    CTRL_FAIL
Ctrl_status virtual_ram_2_mem(uint32_t addr, const void *ram)
{
  virtual_check_init();
  if (addr + 1 > VMEM_NB_SECTOR) return CTRL_FAIL;

  memcpy(&vmem_data[addr * VMEM_SECTOR_SIZE], ram, VMEM_SECTOR_SIZE);

  return CTRL_GOOD;
}

#endif  // ACCESS_MEM_TO_RAM == true


#endif  // VIRTUAL_MEM == ENABLE
