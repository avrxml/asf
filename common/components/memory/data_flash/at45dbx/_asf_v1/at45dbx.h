/*****************************************************************************
 *
 * \file
 *
 * \brief Management of the AT45DBX data flash controller through SPI.
 *
 * This file manages the accesses to the AT45DBX data flash components.
 *
 * Copyright (c) 2014-2016 Atmel Corporation. All rights reserved.
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


#ifndef _AT45DBX_H_
#define _AT45DBX_H_

/**
 * \defgroup group_common_components_memory_data_flash_at45dbx_asf_v1 AT45DBX DataFlash (from ASF ver. 1)
 *
 * Driver for the AT45dbX series SPI DataFlash memories (from ASF V1).
 * It provides functions for initialization and read/write operations at byte or sector level.
 *
 * \{
 */

#include "conf_access.h"

#if AT45DBX_MEM == DISABLE
  #error at45dbx.h is #included although AT45DBX_MEM is disabled
#endif


#include "spi.h"


//_____ D E F I N I T I O N S ______________________________________________

/*! \name Available AT45DBX Sizes
 *
 * Number of address bits of available AT45DBX data flash memories.
 *
 * \note Only memories with page sizes of at least 512 bytes (sector size) are
 *       supported.
 */
//! @{
#define AT45DBX_2MB             21
#define AT45DBX_4MB             22
#define AT45DBX_8MB             23
//! @}

//! Number of bits for addresses within sectors.
#ifdef AT45DB641E
#define AT45DBX_SECTOR_BITS     8
#else
#define AT45DBX_SECTOR_BITS     9
#endif

//! Sector size in bytes.
#define AT45DBX_SECTOR_SIZE     (1 << AT45DBX_SECTOR_BITS)


//_____ D E C L A R A T I O N S ____________________________________________

/*! \name Control Functions
 */
//! @{

/*! \brief Initializes the data flash controller and the SPI channel by which
 *         the DF is controlled.
 *
 * \param spiOptions  Initialization options of the DF SPI channel.
 * \param pba_hz      SPI module input clock frequency (PBA clock, Hz).
 *
 * \retval true Success.
 * \retval false Failure.
 */
extern bool at45dbx_init(spi_options_t spiOptions, unsigned int pba_hz);

/*! \brief Performs a memory check on all DFs.
 *
 * \retval true Success.
 * \retval false Failure.
 */
extern bool at45dbx_mem_check(void);

/*! \brief Opens a DF memory in read mode at a given sector.
 *
 * \param sector  Start sector.
 *
 * \retval true Success.
 * \retval false Failure.
 *
 * \note Sector may be page-unaligned (depending on the DF page size).
 */
extern bool at45dbx_read_open(U32 sector);

/*! \brief Unselects the current DF memory.
 */
extern void at45dbx_read_close(void);

/*! \brief This function opens a DF memory in write mode at a given sector.
 *
 * \param sector  Start sector.
 *
 * \retval true Success.
 * \retval false Failure.
 *
 * \note Sector may be page-unaligned (depending on the DF page size).
 *
 * \note If \ref AT45DBX_PAGE_SIZE > \ref AT45DBX_SECTOR_SIZE, page content is
 *       first loaded in buffer to then be partially updated by write byte or
 *       write sector functions.
 */
extern bool at45dbx_write_open(U32 sector);

/*! \brief Fills the end of the current logical sector and launches page programming.
 */
extern void at45dbx_write_close(void);

//! @}


/*! \name Single-Byte Access Functions
 */
//! @{

/*! \brief Performs a single byte read from DF memory.
 *
 * \return The read byte.
 *
 * \note First call must be preceded by a call to the \ref at45dbx_read_open
 *       function.
 */
extern U8 at45dbx_read_byte(void);

/*! \brief Performs a single byte write to DF memory.
 *
 * \param b The byte to write.
 *
 * \retval true Success.
 * \retval false Failure.
 *
 * \note First call must be preceded by a call to the \ref at45dbx_write_open
 *       function.
 */
extern bool at45dbx_write_byte(U8 b);

//! @}


/*! \name Multiple-Sector Access Functions
 */
//! @{

/*! \brief Reads \a nb_sector sectors from DF memory.
 *
 * Data flow is: DF -> callback.
 *
 * \param nb_sector Number of contiguous sectors to read.
 *
 * \retval true Success.
 * \retval false Failure.
 *
 * \note First call must be preceded by a call to the \ref at45dbx_read_open
 *       function.
 *
 * \note As \ref AT45DBX_PAGE_SIZE is always a multiple of
 *       \ref AT45DBX_SECTOR_SIZE, there is no need to check page end for each
 *       byte.
 */
extern bool at45dbx_read_multiple_sector(U16 nb_sector);

/*! \brief Callback function invoked after each sector read during
 *         \ref at45dbx_read_multiple_sector.
 *
 * \param psector Pointer to read sector.
 */
extern void at45dbx_read_multiple_sector_callback(const void *psector);

/*! \brief Writes \a nb_sector sectors to DF memory.
 *
 * Data flow is: callback -> DF.
 *
 * \param nb_sector Number of contiguous sectors to write.
 *
 * \retval true Success.
 * \retval false Failure.
 *
 * \note First call must be preceded by a call to the \ref at45dbx_write_open
 *       function.
 *
 * \note As \ref AT45DBX_PAGE_SIZE is always a multiple of
 *       \ref AT45DBX_SECTOR_SIZE, there is no need to check page end for each
 *       byte.
 */
extern bool at45dbx_write_multiple_sector(U16 nb_sector);

/*! \brief Callback function invoked before each sector write during
 *         \ref at45dbx_write_multiple_sector.
 *
 * \param psector Pointer to sector to write.
 */
extern void at45dbx_write_multiple_sector_callback(void *psector);

//! @}


/*! \name Single-Sector Access Functions
 */
//! @{

/*! \brief Reads 1 DF sector to a RAM buffer.
 *
 * Data flow is: DF -> RAM.
 *
 * \param ram Pointer to RAM buffer.
 *
 * \retval true Success.
 * \retval false Failure.
 *
 * \note First call must be preceded by a call to the \ref at45dbx_read_open
 *       function.
 */
extern bool at45dbx_read_sector_2_ram(void *ram);

/*! \brief Writes 1 DF sector from a RAM buffer.
 *
 * Data flow is: RAM -> DF.
 *
 * \param ram Pointer to RAM buffer.
 *
 * \retval true Success.
 * \retval false Failure.
 *
 * \note First call must be preceded by a call to the \ref at45dbx_write_open
 *       function.
 */
extern bool at45dbx_write_sector_from_ram(const void *ram);

//! @}

/**
 * \}
 */

#endif  // _AT45DBX_H_
