/**************************************************************************
 *
 * \file
 *
 * \brief Host management of the USB device memories.
 *
 * This file manages the accesses to the remote USB device memories.
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


#ifndef _HOST_MEM_H_
#define _HOST_MEM_H_


#include "conf_access.h"

#if MEM_USB == DISABLE
  #error host_mem.h is #included although MEM_USB is disabled
#endif


#include "ctrl_access.h"


//_____ D E F I N I T I O N S ______________________________________________

#define MS_GET_MAX_LUN      0xFE

#define HOST_SECTOR_SIZE    512   // Sector = 512 bytes


extern uint8_t      host_selected_lun;

extern uint8_t      g_pipe_ms_in;
extern uint8_t      g_pipe_ms_out;


//---- CONTROL FUNCTIONS ----

extern Ctrl_status  host_ms_inquiry(void);
extern uint8_t      host_ms_request_sense(void);

extern uint8_t      host_get_lun(void);
extern Ctrl_status  host_test_unit_ready(uint8_t lun);
extern Ctrl_status  host_read_capacity(uint8_t lun, uint32_t *u32_nb_sector);
extern uint8_t      host_read_sector_size(uint8_t lun);
extern bool         host_wr_protect(uint8_t lun);
extern bool         host_removal(void);


//---- ACCESS DATA FUNCTIONS ----

// RAM interface
#if ACCESS_MEM_TO_RAM == true
extern Ctrl_status  host_read_10_ram      (uint32_t addr,       void *ram);
extern Ctrl_status  host_read_10_extram   (uint32_t addr,       void *ram, uint8_t nb_sector);
extern Ctrl_status  host_write_10_ram     (uint32_t addr, const void *ram);
extern Ctrl_status  host_write_10_extram  (uint32_t addr, const void *ram, uint8_t nb_sector);
#endif


#endif  // _HOST_MEM_H_
