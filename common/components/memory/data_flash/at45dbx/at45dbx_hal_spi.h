/**
 * \file
 *
 * \brief Connection of the AT45DBX DataFlash to SPI interface driver.
 *
 * This file manages the connection of the AT45dbx DataFlash driver to an
 * SPI service (could be spi master service or usart in spi mode). The SPI
 * service selection depends on  AT45DBX_USES_SPI_MASTER_SERVICE or
 * AT45DBX_USES_USART_SPI_SERVICE in conf_at45dbx.h
 *
 * Copyright (c) 2010-2015 Atmel Corporation. All rights reserved.
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

#ifndef _AT45DBX_HAL_SPI_H_
#define _AT45DBX_HAL_SPI_H_

#include "conf_at45dbx.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \name Functions to connect the DataFlash driver with the SPI Multiple-Sector Access Functions
 */
//! @{

// Sanity check to enable connection with only one SPI service
#if defined(AT45DBX_USES_SPI_MASTER_SERVICE) &&  defined(AT45DBX_USES_USART_SPI_SERVICE)
	#error only one SPI service should be selected
#endif
// At least one SPI service should be connected...
#if !defined(AT45DBX_USES_SPI_MASTER_SERVICE) &&  !defined(AT45DBX_USES_USART_SPI_SERVICE)
	#error No SPI service selected
#endif

#if defined(AT45DBX_USES_SPI_MASTER_SERVICE)
#  include "spi_master.h"
#  define driver  spi
#  define spi_setup_device  spi_master_setup_device
#elif defined(AT45DBX_USES_USART_SPI_SERVICE)
#  include "usart_spi.h"
#  define driver  usart_spi
#endif

// Link common functions to the driver used (spi or usart_spi)
#define at45dbx_drv_device  ATPASTE2(driver, _device)
#define at45dbx_drv_setup_device  ATPASTE2(driver, _setup_device)
#define at45dbx_drv_select_device  ATPASTE2(driver, _select_device)
#define at45dbx_drv_deselect_device  ATPASTE2(driver, _deselect_device)
#define at45dbx_drv_write_packet  ATPASTE2(driver, _write_packet)
#define at45dbx_drv_read_packet  ATPASTE2(driver, _read_packet)

// Create as many usart_spi_device as required...
#define AT45DBX_CS0    AT45DBX_CS  // To keep compliance
static struct at45dbx_drv_device at45dbx_devices[] = {
# define AT45DBX_CS_ID(slot, unused) \
		{ .id = AT45DBX_CS##slot},
		MREPEAT(AT45DBX_MEM_CNT, AT45DBX_CS_ID, ~)
# undef AT45DBX_CS_ID
};

/*! \brief Initialize SPI external resource for AT45dbx DataFlash driver.
 *
 *  This function initializes the DataFlash component as well as the SPI service
 *  the memory is connected to.
 *  \pre The conf_at45dbx.h file should provide the following information
 *	 - Selection of the SPI service the DataFlash is connected
 *  (AT45DBX_USES_SPI_MASTER_SERVICE or  AT45DBX_USES_USART_SPI_SERVICE)
 *  - AT45DBX_MEM_SIZE: The DataFlash type the driver is connected to.
 *  - AT45DBX_MEM_CNT: The number of DataFlash components (number of chipselect)
 *  - AT45DBX_SPI_MODULE:  The name of the SPI module used by the DataFlash driver
 *  - AT45DBX_SPI_MASTER_SPEED: The SPI bus speed.
 */
inline void at45dbx_spi_init(void)
{
#if defined(AT45DBX_USES_SPI_MASTER_SERVICE)
	if (spi_is_enabled(AT45DBX_SPI_MODULE)) {
		return;
	}
	spi_master_init(AT45DBX_SPI_MODULE);
	spi_enable(AT45DBX_SPI_MODULE);
#elif defined(AT45DBX_USES_USART_SPI_SERVICE)
	usart_spi_init(AT45DBX_SPI_MODULE);
#endif
}

/*! \brief Select one external DataFlash component
 *
 * \param mem_id  The DataFlash index number.
 */
inline void at45dbx_spi_select_device(uint8_t mem_id)
{
	switch(mem_id) {
#define AT45DBX_SELECT(slot, unused) \
	case slot:\
		at45dbx_drv_setup_device(AT45DBX_SPI_MODULE, &at45dbx_devices[slot],\
		SPI_MODE_0, AT45DBX_SPI_MASTER_SPEED, 0);\
		at45dbx_drv_select_device(AT45DBX_SPI_MODULE, &at45dbx_devices[slot]); \
		break;
		MREPEAT(AT45DBX_MEM_CNT, AT45DBX_SELECT, ~)
#undef AT45DBX_SELECT
	default:
		/* unhandled_case(id); */
		return;
	}
}

/*! \brief Unselect one external DataFlash component
 *
 * \param mem_id  The DataFlash index number.
 */
inline void at45dbx_spi_deselect_device(uint8_t mem_id)
{
	switch(mem_id) {
#define AT45DBX_DESELECT(slot, unused) \
	case slot:\
		at45dbx_drv_deselect_device(AT45DBX_SPI_MODULE, &at45dbx_devices[slot]); \
		break;
		MREPEAT(AT45DBX_MEM_CNT, AT45DBX_DESELECT, ~)
#undef AT45DBX_DESELECT
	default:
		/* unhandled_case(id); */
		return;
	}
}

/*! \brief Send one byte to the DataFlash.
 *
 * \param data The data byte to send.
 * \pre The DataFlash should be selected first using at45dbx_spi_select_device
 */
inline void at45dbx_spi_write_byte(uint8_t data)
{
	at45dbx_drv_write_packet(AT45DBX_SPI_MODULE, &data, 1);
}

/*! \brief Get one byte (read) from the DataFlash.
 *
 * \return The received byte.
 * \pre The DataFlash should be selected first using at45dbx_spi_select_device
 */
inline void at45dbx_spi_read_byte(uint8_t *data)
{
	at45dbx_drv_read_packet(AT45DBX_SPI_MODULE, data, 1);
}


/**
 * \brief Receive a sequence of bytes from a DataFlash
 *
 *
 * \param data   data buffer to read
 * \param len    Length of data
 * \pre The DataFlash should be selected first using at45dbx_spi_select_device
 */
inline void at45dbx_spi_read_packet(void const *data, size_t len)
{
	at45dbx_drv_read_packet(AT45DBX_SPI_MODULE, (uint8_t*)data, len);
}

/**
 * \brief Send a sequence of bytes to a DataFlash from
 *
 *
 * \param data   data buffer to write
 * \param len    Length of data
 * \pre The DataFlash should be selected first using at45dbx_spi_select_device
 *
 */
inline void at45dbx_spi_write_packet(void const *data, size_t len)
{
	at45dbx_drv_write_packet(AT45DBX_SPI_MODULE, (uint8_t*)data, len);
}

//! @}

#ifdef __cplusplus
}
#endif

#endif  // _AT45DBX_HAL_SPI_H_
