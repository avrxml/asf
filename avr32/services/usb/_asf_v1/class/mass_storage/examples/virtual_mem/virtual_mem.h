/**************************************************************************
 *
 * \file
 *
 * \brief Management of the virtual memory.
 *
 * This file manages the virtual memory.
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
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


#ifndef _VIRTUAL_MEM_H_
#define _VIRTUAL_MEM_H_


#include "conf_access.h"

#if VIRTUAL_MEM == DISABLE
  #error virtual_mem.h is #included although VIRTUAL_MEM is disabled
#endif


#include "ctrl_access.h"


//_____ D E F I N I T I O N S ______________________________________________

//! FAT specification
#define ENTRY_SIZE          32    // Size of entry in bytes

//! Value of virtual PBR
#define VMEM_SECTOR_SIZE    512   // Sector = 512 bytes
#define VMEM_CLUSTER_SIZE   1     // Number of sectors per cluster
#define VMEM_RESERVED_SIZE  1     // Number of reserved sectors in the Reserved region of the volume starting at the first sector (FAT sector) of the volume
                                  // It must be 1 for FAT 12 and 16 and 32 for FAT 32
#define VMEM_NB_FATS        2     // Number of FATs, it must be 2 to support all systems
#define VMEM_NB_ROOT_ENTRY  16    // Count of 32-byte directory entries in the root directory (32 * this value = must be a multiple of sector size)
                                  // It must be 512 for FAT 12 and 16 and 0 for FAT 32
#define VMEM_MEDIA_TYPE     0xF8  // Media type (not removable=0xF8 or removable=0xF0), the standard value is 0xF8
#define VMEM_SIZE_FAT       1     // Number of sectors per FAT
// Physical structure of disk
#define VMEM_SECT_PER_TRACK 1     // Number of sectors per track
#define VMEM_NB_HEAD        1     // Number of heads

#define VMEM_NB_HIDDEN_SECT 0     // This field should always be zero on non-partitioned media
#define VMEN_DRIVE_NUMBER   0x80  // 0x00 for floppy disk; 0x80 for hard disk

// First sector of the virtual disk (this disk isn't partitioned, only 1 partion)
#define PBR_SECTOR          0
#define FAT_SECTOR          (PBR_SECTOR  + VMEM_RESERVED_SIZE)
#define ROOT_SECTOR         (FAT_SECTOR  + VMEM_NB_FATS * VMEM_SIZE_FAT)
#define FILE_SECTOR         (ROOT_SECTOR + VMEM_NB_ROOT_ENTRY * ENTRY_SIZE / VMEM_SECTOR_SIZE)  // 1 sector = size root dir

#define FILE_SIZE           16    // Number of available clusters

// Size of virtual mem
#define VMEM_NB_SECTOR      (FILE_SIZE * VMEM_CLUSTER_SIZE + FILE_SECTOR)


#if VMEM_NB_SECTOR > 0xFF
  #error Virtual memory volume size > 512 bytes * 0xFF
#endif


//---- CONTROL FUNCTIONS ----

extern Ctrl_status  virtual_test_unit_ready(void);
extern Ctrl_status  virtual_read_capacity(uint32_t *u32_nb_sector);
extern bool         virtual_wr_protect(void);
extern bool         virtual_removal(void);


//---- ACCESS DATA FUNCTIONS ----

// USB interface
#if ACCESS_USB == true
extern Ctrl_status  virtual_usb_read_10 (uint32_t addr, uint16_t nb_sector);
extern Ctrl_status  virtual_usb_write_10(uint32_t addr, uint16_t nb_sector);
#endif

// RAM interface
#if ACCESS_MEM_TO_RAM == true
extern Ctrl_status  virtual_mem_2_ram(uint32_t addr,       void *ram);
extern Ctrl_status  virtual_ram_2_mem(uint32_t addr, const void *ram);
#endif


#endif  // _VIRTUAL_MEM_H_
