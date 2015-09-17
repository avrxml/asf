/**
 * \file
 *
 * \brief USB host driver for Mass Storage Class interface.
 *
 * Copyright (C) 2011-2015 Atmel Corporation. All rights reserved.
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

#ifndef _UHI_MSC_H_
#define _UHI_MSC_H_

#include "conf_usb_host.h"
#include "usb_protocol_msc.h"
#include "spc_protocol.h"
#include "sbc_protocol.h"
#include "uhc.h"
#include "uhi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup uhi_msc_group
 * \defgroup uhi_msc_group_uhc Interface with USB Host Core (UHC)
 *
 * Define and functions required by UHC.
 * 
 * @{
 */

//! Global define which contains standard UHI API for UHC.
//! It must be added in USB_HOST_UHI define from conf_usb_host.h file.
#define UHI_MSC { \
	.install = uhi_msc_install, \
	.enable = uhi_msc_enable, \
	.uninstall = uhi_msc_uninstall, \
	.sof_notify = NULL, \
}

/**
 * \name Functions required by UHC
 * @{
 */
uhc_enum_status_t uhi_msc_install(uhc_device_t* dev);
void uhi_msc_enable(uhc_device_t* dev);
void uhi_msc_uninstall(uhc_device_t* dev);
//@}

//@}

/**
 * \ingroup uhi_group
 * \defgroup uhi_msc_group UHI for Mass Storage Class
 *
 * Common APIs used by high level application to use this USB host class.
 *
 * @{
 */

/**
 * \name Struct to access at Logical Unit Numbers (LUNs)
 * @{
 */

//! Status of LUN
typedef enum
{
	LUN_GOOD       = 0,  //!< Success, memory ready.
	LUN_FAIL       = 1,  //!< An error occurred.
	LUN_NOT_PRESENT = 2, //!< Memory unplugged.
	LUN_BUSY       = 3,  //!< Memory not initialized or changed.
} lun_status_t;

//! Callback type used by uhi_msc_scsi() functions
typedef void (*uhi_msc_scsi_callback_t) (bool);

//! LUN structure information
typedef struct {
	struct sbc_read_capacity10_data capacity;
	bool b_write_protected;
	lun_status_t status;
} uhi_msc_lun_t;

//@}

/**
 * \brief Tests if the interface UHI Mass Storage is available
 * The UHI Mass Storage can be busy during the enumeration of a USB Device MSC.
 *
 * \return true, if UHI Mass Storage is available
 */
bool uhi_msc_is_available(void);

/**
 * \brief Gives the number of LUN available
 * Note: A LUN can be available, but with a status LUN_NOT_PRESENT.
 * It is the case for a card reader without card.
 *
 * \return Number of LUN available
 */
uint8_t uhi_msc_get_lun(void);

/**
 * \name Functions to access at LUNs
 * @{
 */

/**
 * \brief Gives information about a LUN
 *
 * \param lun  LUN number
 *
 * \return Pointer on the LUN information structure.
 */
uhi_msc_lun_t* uhi_msc_get_lun_desc(uint8_t lun);

/**
 * \brief Checks and update the status of the LUN
 *
 * \param lun       LUN number
 * \param callback  Callback to call at the end of scsi command
 *
 * \return true, if the scsi command has been accepted
 */
bool uhi_msc_scsi_test_unit_ready(uint8_t lun, uhi_msc_scsi_callback_t callback);

/**
 * \brief Reads a LUN data section to RAM buffer
 * Note: The sector size used to define the data section
 * is the sector size returned by LUN in \capacity field.
 *
 * \param lun       LUN number
 * \param addr      Sector address to read
 * \param ram       RAM address used to store the data
 * \param nb_sector Number of sector to read
 * \param callback  Callback to call at the end of scsi command
 *
 * \return true, if the scsi command has been accepted
 */
bool uhi_msc_scsi_read_10(uint8_t lun, uint32_t addr, uint8_t *ram,
		uint8_t nb_sector, uhi_msc_scsi_callback_t callback);

/**
 * \brief Writes a RAM buffer in a LUN data section
 * Note: The sector size used to define the data section
 * is the sector size returned by LUN in \capacity field.
 *
 * \param lun       LUN number
 * \param addr      Sector address to write
 * \param ram       RAM address of data to write
 * \param nb_sector Number of sector to write
 * \param callback  Callback to call at the end of scsi command
 *
 * \return true, if the scsi command has been accepted
 */
bool uhi_msc_scsi_write_10(uint8_t lun, uint32_t addr, const uint8_t *ram,
		uint8_t nb_sector, uhi_msc_scsi_callback_t callback);
//@}

//@}

#ifdef __cplusplus
}
#endif
#endif // _UHI_MSC_H_
