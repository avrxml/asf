/*****************************************************************************
 *
 * \file
 *
 * \brief CTRL_ACCESS interface for SD/MMC card.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


//_____  I N C L U D E S ___________________________________________________

#include "conf_access.h"


#if (SD_MMC_MCI_0_MEM == ENABLE) || (SD_MMC_MCI_1_MEM == ENABLE)

#include "conf_sd_mmc_mci.h"
#include "sd_mmc_mci.h"
#include "sd_mmc_mci_mem.h"


//_____ D E F I N I T I O N S ______________________________________________

#define Sd_mmc_mci_access_signal_on()
#define Sd_mmc_mci_access_signal_off()

//! Initialization sequence status per Slot.
extern  bool sd_mmc_mci_init_done[MCI_NR_SLOTS];
//! SD/MMC Card Size per Slot.
extern  uint32_t  g_card_size[MCI_NR_SLOTS];
//! SD/MMC Card Presence status per Slot.
static  uint8_t   sd_mmc_mci_presence_status[MCI_NR_SLOTS] = {SD_MMC_INSERTED, SD_MMC_INSERTED};

/*! \name Control Interface
 */
//! @{


Ctrl_status sd_mmc_mci_test_unit_ready(uint8_t slot)
{
  if (slot > MCI_LAST_SLOTS) return CTRL_FAIL;

  Sd_mmc_mci_access_signal_on();
  switch (sd_mmc_mci_presence_status[slot])
  {
    case SD_MMC_REMOVED:
      sd_mmc_mci_init_done[slot] = false;
      if (sd_mmc_mci_mem_check(slot))
      {
        sd_mmc_mci_presence_status[slot] = SD_MMC_INSERTED;
        Sd_mmc_mci_access_signal_off();
        return CTRL_BUSY;
      }
      Sd_mmc_mci_access_signal_off();
      return CTRL_NO_PRESENT;

    case SD_MMC_INSERTED:
      if (!sd_mmc_mci_mem_check(slot))
      {
        sd_mmc_mci_presence_status[slot] = SD_MMC_REMOVING;
        sd_mmc_mci_init_done[slot] = false;
        Sd_mmc_mci_access_signal_off();
        return CTRL_BUSY;
      }
      Sd_mmc_mci_access_signal_off();
      return CTRL_GOOD;

    case SD_MMC_REMOVING:
      sd_mmc_mci_presence_status[slot] = SD_MMC_REMOVED;
      Sd_mmc_mci_access_signal_off();
      return CTRL_NO_PRESENT;

    default:
      sd_mmc_mci_presence_status[slot] = SD_MMC_REMOVED;
      Sd_mmc_mci_access_signal_off();
      return CTRL_BUSY;
  }
}


Ctrl_status sd_mmc_mci_test_unit_ready_0(void)
{
   return sd_mmc_mci_test_unit_ready(0);
}


Ctrl_status sd_mmc_mci_test_unit_ready_1(void)
{
   return sd_mmc_mci_test_unit_ready(1);
}


Ctrl_status sd_mmc_mci_read_capacity(uint8_t slot, uint32_t *nb_sector)
{
   Sd_mmc_mci_access_signal_on();

   if( !sd_mmc_mci_mem_check(slot) )
   {
     Sd_mmc_mci_access_signal_off();
     return CTRL_NO_PRESENT;
   }
   *nb_sector = g_card_size[slot]-1;
   Sd_mmc_mci_access_signal_off();
   return CTRL_GOOD;
}


Ctrl_status sd_mmc_mci_read_capacity_0(uint32_t *nb_sector)
{
  return sd_mmc_mci_read_capacity(0, nb_sector);
}


Ctrl_status sd_mmc_mci_read_capacity_1(uint32_t *nb_sector)
{
  return sd_mmc_mci_read_capacity(1, nb_sector);
}


bool sd_mmc_mci_wr_protect(uint8_t slot)
{
  return is_sd_mmc_mci_card_protected(slot);
}


bool sd_mmc_mci_wr_protect_0(void)
{
  return sd_mmc_mci_wr_protect(0);
}


bool sd_mmc_mci_wr_protect_1(void)
{
  return sd_mmc_mci_wr_protect(1);
}


bool sd_mmc_mci_removal(uint8_t slot)
{
  return false;
}


bool sd_mmc_mci_removal_0(void)
{
  return sd_mmc_mci_removal(0);
}


bool sd_mmc_mci_removal_1(void)
{
  return sd_mmc_mci_removal(1);
}

//! @}

#if ACCESS_USB == true

#include "conf_usb.h"
#ifdef USB_DEVICE_VENDOR_ID
  // USB Device Stack V2
#include "udi_msc.h"
#else
  // USB Device Stack V1
#include "usb_drv.h"
#include "scsi_decoder.h"
#endif

/*! \name MEM <-> USB Interface
 */
//! @{


Ctrl_status sd_mmc_mci_usb_read_10(uint8_t slot, uint32_t addr, uint16_t nb_sector)
{
   Sd_mmc_mci_access_signal_on();

   if( !sd_mmc_mci_mem_check(slot) )
   {
     Sd_mmc_mci_access_signal_off();
     return CTRL_NO_PRESENT;
   }

   if( !sd_mmc_mci_dma_read_open(slot, addr, NULL, nb_sector) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_read_multiple_sector(slot, nb_sector) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_read_close(slot) )
     return CTRL_FAIL;

   Sd_mmc_mci_access_signal_off();
   return CTRL_GOOD;
}


Ctrl_status sd_mmc_mci_usb_read_10_0(uint32_t addr, uint16_t nb_sector)
{
  return sd_mmc_mci_usb_read_10(0, addr, nb_sector);
}


Ctrl_status sd_mmc_mci_usb_read_10_1(uint32_t addr, uint16_t nb_sector)
{
  return sd_mmc_mci_usb_read_10(1, addr, nb_sector);
}


void sd_mmc_mci_read_multiple_sector_callback(const void *psector)
{
#ifdef USB_DEVICE_VENDOR_ID
   // USB Device Stack V2
   udi_msc_trans_block( true, (uint8_t*)psector, SD_MMC_SECTOR_SIZE, NULL);
#else
  // USB Device Stack V1
  uint16_t data_to_transfer = SD_MMC_SECTOR_SIZE;

  // Transfer read sector to the USB interface.
  while (data_to_transfer)
  {
    while (!Is_usb_in_ready(g_scsi_ep_ms_in))
    {
      if(!Is_usb_endpoint_enabled(g_scsi_ep_ms_in))
         return; // USB Reset
    }

    Usb_reset_endpoint_fifo_access(g_scsi_ep_ms_in);
    data_to_transfer = usb_write_ep_txpacket(g_scsi_ep_ms_in, psector,
                                             data_to_transfer, &psector);
    Usb_ack_in_ready_send(g_scsi_ep_ms_in);
  }
#endif
}


Ctrl_status sd_mmc_mci_usb_write_10(uint8_t slot,uint32_t addr, uint16_t nb_sector)
{
   Sd_mmc_mci_access_signal_on();

   if( !sd_mmc_mci_mem_check(slot) )
   {
     Sd_mmc_mci_access_signal_off();
     return CTRL_NO_PRESENT;
   }

   if( !sd_mmc_mci_dma_write_open(slot, addr, NULL, nb_sector) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_write_multiple_sector(slot, nb_sector) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_write_close(slot) )
     return CTRL_FAIL;

   Sd_mmc_mci_access_signal_off();
   return CTRL_GOOD;

}


Ctrl_status sd_mmc_mci_usb_write_10_0(uint32_t addr, uint16_t nb_sector)
{
  return sd_mmc_mci_usb_write_10(0, addr, nb_sector);
}


Ctrl_status sd_mmc_mci_usb_write_10_1(uint32_t addr, uint16_t nb_sector)
{
  return sd_mmc_mci_usb_write_10(1, addr, nb_sector);
}


void sd_mmc_mci_write_multiple_sector_callback(void *psector)
{
#ifdef USB_DEVICE_VENDOR_ID
   // USB Device Stack V2
   udi_msc_trans_block( false, (uint8_t*)psector, SD_MMC_SECTOR_SIZE, NULL);
#else
  // USB Device Stack V1
  uint16_t data_to_transfer = SD_MMC_SECTOR_SIZE;

  // Transfer sector to write from the USB interface.
  while (data_to_transfer)
  {
    while (!Is_usb_out_received(g_scsi_ep_ms_out))
    {
      if(!Is_usb_endpoint_enabled(g_scsi_ep_ms_out))
         return; // USB Reset
    }

    Usb_reset_endpoint_fifo_access(g_scsi_ep_ms_out);
    data_to_transfer = usb_read_ep_rxpacket(g_scsi_ep_ms_out, psector,
                                            data_to_transfer, &psector);
    Usb_ack_out_received_free(g_scsi_ep_ms_out);
  }
#endif
}


//! @}

#endif  // ACCESS_USB == true


#if ACCESS_MEM_TO_RAM == true

/*! \name MEM <-> RAM Interface
 */
//! @{


Ctrl_status sd_mmc_mci_mem_2_ram(uint8_t slot, uint32_t addr, void *ram)
{
   if( !sd_mmc_mci_mem_check(slot) )
   {
     Sd_mmc_mci_access_signal_off();
     return CTRL_NO_PRESENT;
   }

   if( !sd_mmc_mci_read_open(slot, addr, 1) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_read_sector_2_ram(slot, ram) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_read_close(slot) )
     return CTRL_FAIL;

   Sd_mmc_mci_access_signal_off();
   return CTRL_GOOD;

}


Ctrl_status sd_mmc_mci_dma_mem_2_ram(uint8_t slot, uint32_t addr, void *ram)
{
   if( !sd_mmc_mci_mem_check(slot) )
   {
     Sd_mmc_mci_access_signal_off();
     return CTRL_NO_PRESENT;
   }

   if( !sd_mmc_mci_dma_read_open(slot, addr, ram, 1) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_dma_read_sector_2_ram(slot,ram) )
     return CTRL_FAIL;

   if(! sd_mmc_mci_read_close(slot) )
     return CTRL_FAIL;

   Sd_mmc_mci_access_signal_off();
   return CTRL_GOOD;

}


Ctrl_status sd_mmc_mci_mem_2_ram_0(uint32_t addr, void *ram)
{
  return sd_mmc_mci_mem_2_ram(0, addr, ram);
}


Ctrl_status sd_mmc_mci_mem_2_ram_1(uint32_t addr, void *ram)
{
  return sd_mmc_mci_mem_2_ram(1, addr, ram);
}


Ctrl_status sd_mmc_mci_multiple_mem_2_ram(uint8_t slot, uint32_t addr, void *ram, uint32_t nb_sectors)
{
   if( !sd_mmc_mci_mem_check(slot) )
   {
     Sd_mmc_mci_access_signal_off();
     return CTRL_NO_PRESENT;
   }

   if( !sd_mmc_mci_read_open(slot, addr, nb_sectors) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_read_multiple_sector_2_ram(slot, ram, nb_sectors) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_read_close(slot) )
     return CTRL_FAIL;

   Sd_mmc_mci_access_signal_off();
   return CTRL_GOOD;
}


Ctrl_status sd_mmc_mci_dma_multiple_mem_2_ram(uint8_t slot, uint32_t addr, void *ram, uint32_t nb_sectors)
{
   if( !sd_mmc_mci_mem_check(slot) )
   {
     Sd_mmc_mci_access_signal_off();
     return CTRL_NO_PRESENT;
   }

   if( !sd_mmc_mci_dma_read_open(slot, addr, ram, nb_sectors ) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_dma_read_multiple_sector_2_ram(slot, ram, nb_sectors) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_read_close(slot) )
     return CTRL_FAIL;

   Sd_mmc_mci_access_signal_off();
   return CTRL_GOOD;
}


Ctrl_status sd_mmc_mci_multiple_mem_2_ram_0(uint32_t addr, void *ram, uint32_t nb_sectors)
{
  return sd_mmc_mci_multiple_mem_2_ram(0, addr, ram, nb_sectors);
}


Ctrl_status sd_mmc_mci_multiple_mem_2_ram_1(uint32_t addr, void *ram, uint32_t nb_sectors)
{
  return sd_mmc_mci_multiple_mem_2_ram(1, addr, ram, nb_sectors);
}


Ctrl_status sd_mmc_mci_ram_2_mem(uint8_t slot, uint32_t addr, const void *ram)
{
   if( !sd_mmc_mci_mem_check(slot) )
   {
     Sd_mmc_mci_access_signal_off();
     return CTRL_NO_PRESENT;
   }

   if( !sd_mmc_mci_write_open(slot, addr, 1) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_write_sector_from_ram(slot,ram) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_write_close(slot) )
     return CTRL_FAIL;

   Sd_mmc_mci_access_signal_off();
   return CTRL_GOOD;
}


Ctrl_status sd_mmc_mci_dma_ram_2_mem(uint8_t slot, uint32_t addr, const void *ram)
{
   if( !sd_mmc_mci_mem_check(slot) )
   {
     Sd_mmc_mci_access_signal_off();
     return CTRL_NO_PRESENT;
   }

   if( !sd_mmc_mci_dma_write_open(slot, addr, ram, 1) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_dma_write_sector_from_ram(slot, ram) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_write_close(slot) )
     return CTRL_FAIL;

   Sd_mmc_mci_access_signal_off();
   return CTRL_GOOD;
}


Ctrl_status sd_mmc_mci_ram_2_mem_0(uint32_t addr, const void *ram)
{
  return sd_mmc_mci_ram_2_mem(0, addr, ram);
}


Ctrl_status sd_mmc_mci_ram_2_mem_1(uint32_t addr, const void *ram)
{
  return sd_mmc_mci_ram_2_mem(1, addr, ram);
}


Ctrl_status sd_mmc_mci_multiple_ram_2_mem(uint8_t slot, uint32_t addr, const void *ram, uint32_t nb_sectors)
{
   if( !sd_mmc_mci_mem_check(slot) )
   {
     Sd_mmc_mci_access_signal_off();
     return CTRL_NO_PRESENT;
   }

   if( !sd_mmc_mci_write_open(slot, addr, nb_sectors) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_write_multiple_sector_from_ram(slot, ram, nb_sectors) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_write_close(slot) )
     return CTRL_FAIL;

   Sd_mmc_mci_access_signal_off();
   return CTRL_GOOD;
}


Ctrl_status sd_mmc_mci_dma_multiple_ram_2_mem(uint8_t slot, uint32_t addr, const void *ram, uint32_t nb_sectors)
{
   if( !sd_mmc_mci_mem_check(slot) )
   {
     Sd_mmc_mci_access_signal_off();
     return CTRL_NO_PRESENT;
   }

   if( !sd_mmc_mci_dma_write_open(slot, addr, ram, nb_sectors) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_dma_write_multiple_sector_from_ram(slot, ram, nb_sectors) )
     return CTRL_FAIL;

   if( !sd_mmc_mci_write_close(slot) )
     return CTRL_FAIL;

   Sd_mmc_mci_access_signal_off();
   return CTRL_GOOD;
}


Ctrl_status sd_mmc_mci_multiple_ram_2_mem_0(uint32_t addr, const void *ram, uint32_t nb_sectors)
{
  return sd_mmc_mci_multiple_ram_2_mem(0, addr, ram, nb_sectors);
}


Ctrl_status sd_mmc_mci_multiple_ram_2_mem_1(uint32_t addr, const void *ram, uint32_t nb_sectors)
{
  return sd_mmc_mci_multiple_ram_2_mem(1, addr, ram, nb_sectors);
}


//! @}

#endif  // ACCESS_MEM_TO_RAM == true


#endif  // SD_MMC_MCI_0_MEM == ENABLE || SD_MMC_MCI_1_MEM == ENABLE
