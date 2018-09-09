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


#ifndef _SD_MMC_SPI_MEM_H_
#define _SD_MMC_SPI_MEM_H_

/**
 * \defgroup group_avr32_components_memory_sd_mmc_sd_mmc_spi_mem SD/MMC SPI Memory
 *
 * \ingroup group_avr32_components_memory_sd_mmc_sd_mmc_spi
 *
 * \{
 */

#include "conf_access.h"

#if SD_MMC_SPI_MEM == DISABLE
  #error sd_mmc_spi_mem.h is #included although SD_MMC_SPI_MEM is disabled
#endif


#include "ctrl_access.h"


//_____ D E F I N I T I O N S ______________________________________________

#define   SD_MMC_REMOVED       0
#define   SD_MMC_INSERTED      1
#define   SD_MMC_REMOVING      2


//---- CONTROL FONCTIONS ----
//!
//! @brief This function initializes the hw/sw resources required to drive the SD_MMC_SPI.
//!/
extern void           sd_mmc_spi_mem_init(void);

//!
//! @brief This function tests the state of the SD_MMC memory and sends it to the Host.
//!        For a PC, this device is seen as a removable media
//!        Before indicating any modification of the status of the media (GOOD->NO_PRESENT or vice-versa),
//!         the function must return the BUSY data to make the PC accepting the change
//!
//! @return                Ctrl_status
//!   Media is ready       ->    CTRL_GOOD
//!   Media not present    ->    CTRL_NO_PRESENT
//!   Media has changed    ->    CTRL_BUSY
//!/
extern Ctrl_status    sd_mmc_spi_test_unit_ready(void);

//!
//! @brief This function gives the address of the last valid sector.
//!
//! @param *nb_sector  number of sector (sector = 512B). OUT
//!
//! @return                Ctrl_status
//!   Media ready          ->  CTRL_GOOD
//!   Media not present    ->  CTRL_NO_PRESENT
//!/
extern Ctrl_status    sd_mmc_spi_read_capacity(uint32_t *nb_sector);

//!
//! @brief This function returns the write protected status of the memory.
//!
//! Only used by memory removal with a HARDWARE SPECIFIC write protected detection
//! ! The user must unplug the memory to change this write protected status,
//! which cannot be for a SD_MMC.
//!
//! @return false  -> the memory is not write-protected (always)
//!/
extern bool           sd_mmc_spi_wr_protect(void);

//!
//! @brief This function tells if the memory has been removed or not.
//!
//! @return false  -> The memory isn't removed
//!
extern bool           sd_mmc_spi_removal(void);


//---- ACCESS DATA FONCTIONS ----

#if ACCESS_USB == true
// Standard functions for open in read/write mode the device

//!
//! @brief This function performs a read operation of n sectors from a given address on.
//! (sector = 512B)
//!
//!         DATA FLOW is: SD_MMC => USB
//!
//! @param addr         Sector address to start the read from
//! @param nb_sector    Number of sectors to transfer
//!
//! @return                Ctrl_status
//!   It is ready    ->    CTRL_GOOD
//!   A error occur  ->    CTRL_FAIL
//!
extern Ctrl_status    sd_mmc_spi_usb_read_10(uint32_t addr, uint16_t nb_sector);

//! This function initializes the SD/MMC memory for a write operation
//!
//!         DATA FLOW is: USB => SD_MMC
//!
//! (sector = 512B)
//! @param addr         Sector address to start write
//! @param nb_sector    Number of sectors to transfer
//!
//! @return                Ctrl_status
//!   It is ready    ->    CTRL_GOOD
//!   An error occurs  ->    CTRL_FAIL
//!
extern Ctrl_status    sd_mmc_spi_usb_write_10(uint32_t addr, uint16_t nb_sector);

#endif // #if ACCESS_USB == true

#if ACCESS_MEM_TO_RAM == true
// Standard functions for read/write 1 sector to 1 sector ram buffer


//! This function reads 1 sector from SD/MMC to internal ram buffer
//!
//!         DATA FLOW is: SD/MMC => RAM
//!
//! (sector = 512B)
//! @param addr         Sector address to read
//! @param ram          Ram buffer pointer
//!
//! @return                Ctrl_status
//!   It is ready      ->    CTRL_GOOD
//!   An error occurs  ->    CTRL_FAIL
//!
extern Ctrl_status    sd_mmc_spi_mem_2_ram(uint32_t addr, void *ram);

//! This function initializes the memory for a write operation
//! from ram buffer to SD/MMC (1 sector)
//!
//!         DATA FLOW is: RAM => SD/MMC
//!
//! (sector = 512B)
//! @param addr         Sector address to write
//! @param ram          Ram buffer pointer
//!
//! @return                Ctrl_status
//!   It is ready      ->    CTRL_GOOD
//!   An error occurs  ->    CTRL_FAIL
//!
extern Ctrl_status    sd_mmc_spi_ram_2_mem(uint32_t addr, const void *ram);

#endif // end #if ACCESS_MEM_TO_RAM == true

/**
 * \}
 */

#endif  // _SD_MMC_SPI_MEM_H_
