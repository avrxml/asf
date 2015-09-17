/**************************************************************************
 *
 * \file
 *
 * \brief Host management of the USB device memories.
 *
 * This file manages the accesses to the remote USB device memories.
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
