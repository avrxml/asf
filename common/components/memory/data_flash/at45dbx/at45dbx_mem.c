/**
 * \file
 *
 * \brief CTRL_ACCESS interface for the AT45DBX data flash driver.
 *
 * Copyright (c) 2011-2016 Atmel Corporation. All rights reserved.
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


//_____  I N C L U D E S ___________________________________________________

#include "conf_access.h"


#if AT45DBX_MEM == ENABLE

#include "conf_at45dbx.h"
#include "at45dbx.h"
#include "at45dbx_mem.h"

#ifdef AT45DB641E
//!< Address bits for byte position within buffer.
#define AT45DBX_BYTE_ADDR_BITS            9

//! Number of bits for addresses within pages.
#define AT45DBX_PAGE_BITS                 (AT45DBX_BYTE_ADDR_BITS - 1)

//! Page size in bytes.
#define AT45DBX_PAGE_SIZE                 (1 << AT45DBX_PAGE_BITS)
#endif


//_____ D E F I N I T I O N S ______________________________________________

/*! \name Control Interface
 */
//! @{

static bool b_at45dbx_unloaded = false;

Ctrl_status at45dbx_test_unit_ready(void)
{
	if (b_at45dbx_unloaded) {
		return CTRL_NO_PRESENT;
	}
	return (at45dbx_mem_check() == true) ? CTRL_GOOD : CTRL_NO_PRESENT;
}


Ctrl_status at45dbx_read_capacity(U32 *u32_nb_sector)
{
	/* FATFS sector size 512Byte. */	
	*u32_nb_sector = (AT45DBX_MEM_CNT << (AT45DBX_MEM_SIZE - 9)) - 1;
	return CTRL_GOOD;
}


bool at45dbx_wr_protect(void)
{
	return false;
}


bool at45dbx_removal(void)
{
	return true;
}

bool at45dbx_unload(bool unload)
{
	b_at45dbx_unloaded = unload;
}


//! @}


#if ACCESS_USB == true

#include "udi_msc.h"

//! Sector buffer.
static uint8_t sector_buf[AT45DBX_SECTOR_SIZE];

/*! \name MEM <-> USB Interface
 */
//! @{


Ctrl_status at45dbx_usb_read_10(U32 addr, U16 nb_sector)
{
	if (addr + nb_sector > AT45DBX_MEM_CNT << (AT45DBX_MEM_SIZE - AT45DBX_SECTOR_BITS)){
		return CTRL_FAIL;
	}
	at45dbx_read_sector_open(addr);
	while (nb_sector--) {
		// Read the next sector.
		at45dbx_read_sector_to_ram(sector_buf);
		udi_msc_trans_block( true, sector_buf, AT45DBX_SECTOR_SIZE, NULL);
	}
	at45dbx_read_close();
	return CTRL_GOOD;
}

Ctrl_status at45dbx_usb_write_10(U32 addr, U16 nb_sector)
{
	if (addr + nb_sector > AT45DBX_MEM_CNT << (AT45DBX_MEM_SIZE - AT45DBX_SECTOR_BITS)){
		return CTRL_FAIL;
	}

	at45dbx_write_sector_open(addr);
	while (nb_sector--) {
		// Write the next sector.
		udi_msc_trans_block( false, sector_buf, AT45DBX_SECTOR_SIZE, NULL);
		at45dbx_write_sector_from_ram(sector_buf);
	}
	at45dbx_write_close();
	return CTRL_GOOD;
}

//! @}

#endif  // ACCESS_USB == true


#if ACCESS_MEM_TO_RAM == true

/*! \name MEM <-> RAM Interface
 */
//! @{


Ctrl_status at45dbx_df_2_ram(U32 addr, void *ram)
{
	/* FATFS sector size 512Byte. */
	if (addr + 1 > AT45DBX_MEM_CNT << (AT45DBX_MEM_SIZE - 9)){
		return CTRL_FAIL;
	}

#ifdef AT45DB641E
	at45dbx_read_sector_open(addr*2);
	at45dbx_read_sector_to_ram(ram);
	at45dbx_read_close();

	at45dbx_read_sector_open(addr*2+1);
	at45dbx_read_sector_to_ram(ram + AT45DBX_PAGE_SIZE);
	at45dbx_read_close();
#else
	at45dbx_read_sector_open(addr);
	at45dbx_read_sector_to_ram(ram);
	at45dbx_read_close();
#endif

	return CTRL_GOOD;
}


Ctrl_status at45dbx_ram_2_df(U32 addr, const void *ram)
{
	/* FATFS sector size 512Byte. */
	if (addr + 1 > AT45DBX_MEM_CNT << (AT45DBX_MEM_SIZE - 9)) {
		return CTRL_FAIL;
	}

#ifdef AT45DB641E
	at45dbx_write_sector_open(addr*2);
	at45dbx_write_sector_from_ram(ram);
	at45dbx_write_close();

	at45dbx_write_sector_open(addr*2+1);
	at45dbx_write_sector_from_ram(ram + AT45DBX_PAGE_SIZE);
	at45dbx_write_close();
#else
	at45dbx_write_sector_open(addr);
	at45dbx_write_sector_from_ram(ram);
	at45dbx_write_close();
#endif

	return CTRL_GOOD;
}


//! @}

#endif  // ACCESS_MEM_TO_RAM == true


#endif  // AT45DBX_MEM == ENABLE
