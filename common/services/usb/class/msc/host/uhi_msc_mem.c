/**
 * \file
 *
 * \brief USB host Mass Storage interface for control access module..
 *
 * Copyright (C) 2011-2012 Atmel Corporation. All rights reserved.
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

#include "uhi_msc.h"
#include "uhi_msc_mem.h"

/**
 * \ingroup uhi_msc_mem_group
 * \defgroup uhi_msc_mem_group_internal Implementation of USB host Mass Storage
 * interface for control access module
 *
 * Internal implementation 
 * @{
 */

/**
 * \name Internal variables
 */
//@{
//! Current LUN selected
static uint8_t uhi_msc_mem_lun;
//! \name Volatile flag tested by functions in pooling mode and updated by a callback
//@{
static volatile bool uhi_msc_mem_command_ongoing;
static volatile bool uhi_msc_mem_command_status;
//@}
//@}

/**
 * \name Internal functions
 */
//@{
static void uhi_msc_mem_stop_pooling(bool b_success);
static Ctrl_status uhi_msc_mem_translate_status(lun_status_t status);
//@}


/**
 * \name External functions
 */
//@{

uint8_t uhi_msc_mem_get_lun(void)
{
	while (!uhi_msc_is_available());
	return uhi_msc_get_lun();
}

Ctrl_status uhi_msc_mem_test_unit_ready(uint8_t lun)
{
	uhi_msc_lun_t *lun_desc;

	while (!uhi_msc_is_available());

	uhi_msc_mem_command_ongoing = true;
	uhi_msc_mem_lun = lun;
	if (!uhi_msc_scsi_test_unit_ready(uhi_msc_mem_lun, uhi_msc_mem_stop_pooling)) {
		return CTRL_FAIL;
	}
	while (uhi_msc_mem_command_ongoing);
	if (!uhi_msc_mem_command_status) {
		return CTRL_FAIL;
	}
	lun_desc = uhi_msc_get_lun_desc(uhi_msc_mem_lun);
	return uhi_msc_mem_translate_status(lun_desc->status);
}

Ctrl_status uhi_msc_mem_read_capacity(uint8_t lun, uint32_t * u32_nb_sector)
{
	uhi_msc_lun_t *lun_desc;

	while (!uhi_msc_is_available());
	uhi_msc_mem_lun = lun;
	lun_desc = uhi_msc_get_lun_desc(uhi_msc_mem_lun);
	if (lun_desc == NULL) {
		return CTRL_FAIL;
	}
	if (lun_desc->capacity.block_len != 512) {
		// Note: The ctrl_access module uses only the data transfer size of 512 bytes.
		// The uhi_msc_mem module does not implement a RAM cache 
		// to support the transfer size more than 512 bytes.
		return CTRL_FAIL; // Not supported
	}
	*u32_nb_sector = lun_desc->capacity.max_lba;
	return uhi_msc_mem_translate_status(lun_desc->status);
}

uint8_t uhi_msc_mem_read_sector_size(uint8_t lun)
{
	uhi_msc_lun_t *lun_desc;

	while (!uhi_msc_is_available());
	uhi_msc_mem_lun = lun;
	lun_desc = uhi_msc_get_lun_desc(uhi_msc_mem_lun);
	if (lun_desc == NULL) {
		return 0;
	}
	return lun_desc->capacity.block_len / 512;
}

bool uhi_msc_mem_wr_protect(uint8_t lun)
{
	uhi_msc_lun_t *lun_desc;

	while (!uhi_msc_is_available());
	uhi_msc_mem_lun = lun;
	lun_desc = uhi_msc_get_lun_desc(uhi_msc_mem_lun);
	if (lun_desc == NULL) {
		return true;
	}
	if (lun_desc->capacity.block_len != 512) {
		return true;
	}
	return lun_desc->b_write_protected;
}

bool uhi_msc_mem_removal(void)
{
	return true;
}

Ctrl_status uhi_msc_mem_read_10_ram(uint32_t addr, void *ram)
{
	uhi_msc_lun_t *lun_desc;

	while (!uhi_msc_is_available());
	// uhi_msc_mem_lun already selected by a previous command
	lun_desc = uhi_msc_get_lun_desc(uhi_msc_mem_lun);
	if (lun_desc == NULL) {
		return CTRL_FAIL;
	}
	if (uhi_msc_mem_translate_status(lun_desc->status) != CTRL_GOOD) {
		return uhi_msc_mem_translate_status(lun_desc->status);
	}
	if (lun_desc->capacity.block_len != 512) {
		return CTRL_FAIL; // Not supported
	}
	uhi_msc_mem_command_ongoing = true;
	uhi_msc_scsi_read_10(uhi_msc_mem_lun, addr, ram, 1,
			uhi_msc_mem_stop_pooling);
	while (uhi_msc_mem_command_ongoing);
	if (!uhi_msc_mem_command_status) {
		return CTRL_FAIL;
	}
	return uhi_msc_mem_translate_status(lun_desc->status);

}

Ctrl_status uhi_msc_mem_write_10_ram(uint32_t addr, const void *ram)
{
	uhi_msc_lun_t *lun_desc;

	while (!uhi_msc_is_available());
	// uhi_msc_mem_lun already selected by a previous command
	lun_desc = uhi_msc_get_lun_desc(uhi_msc_mem_lun);
	if (lun_desc == NULL) {
		return CTRL_FAIL;
	}
	if (uhi_msc_mem_translate_status(lun_desc->status) != CTRL_GOOD) {
		return uhi_msc_mem_translate_status(lun_desc->status);
	}
	if (lun_desc->capacity.block_len != 512) {
		return CTRL_FAIL; // Not supported
	}
	uhi_msc_mem_command_ongoing = true;
	uhi_msc_scsi_write_10(uhi_msc_mem_lun, addr, ram, 1,
			uhi_msc_mem_stop_pooling);
	while (uhi_msc_mem_command_ongoing);
	if (!uhi_msc_mem_command_status) {
		return CTRL_FAIL;
	}
	return uhi_msc_mem_translate_status(lun_desc->status);
}

//@}

/**
 * \name Internal functions
 */
//@{

/**
 * \brief Callback to update volatile variable used by pooling
 *
 * \param b_success true, if the scsi command is successful
 */
static void uhi_msc_mem_stop_pooling(bool b_success)
{
	uhi_msc_mem_command_ongoing = false;
	uhi_msc_mem_command_status = b_success;
}

/**
 * \brief Translates the LUN status from UHI MSC to CTRL ACCESS module
 *
 * \param status   UHI MSC LUN status
 *
 * \return CTRL ACCESS LUN status
 */
static Ctrl_status uhi_msc_mem_translate_status(lun_status_t status)
{
	switch (status) {
	case LUN_GOOD:
		return CTRL_GOOD;
	case LUN_NOT_PRESENT:
		return CTRL_NO_PRESENT;
	case LUN_BUSY:
		return CTRL_BUSY;
	case LUN_FAIL:
	default:
		return CTRL_FAIL;
	}
}

//@}

//@}
