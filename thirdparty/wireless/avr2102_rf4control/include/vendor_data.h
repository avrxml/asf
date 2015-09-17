/**
 * @file vendor_data.h
 *
 * @brief Declarations of API functions to access vendor specific data
 * functionality
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
 */

#if ((defined VENDOR_DATA) || (defined DOXYGEN))

/* Prevent double inclusion */
#ifndef VENDOR_DATA_H
#define VENDOR_DATA_H

/* === INCLUDES ============================================================ */

#include <stdio.h>

#include "rf4ce.h"
#ifdef ZRC_PROFILE
#include "zrc.h"
#endif

/* === MACROS ============================================================== */

/** The maximum size of a vendor-specific data frame; i.e. maximum MSDU - header
 * and footer overhead: aMxMACSafePayloadSize - 12 */
#define MAX_NWK_VENDOR_DATA_SIZE    90

#define APPLICATION_SEND_COORDINATES_TIME          200000

/* === TYPES =============================================================== */

/**
 * Enum Ids for vendor-specific commands
 */
typedef enum vendor_cmd_id_tag {
	BATTERY_STATUS_REQ,
	BATTERY_STATUS_RESP,
	ALIVE_REQ,
	ALIVE_RESP,
	FW_VERSION_REQ,
	FW_VERSION_RESP,
	RX_ON_REQ,
	RX_ON_RESP,
	FW_DATA_REQ,
	FW_DATA_RESP,
	FW_SWAP_REQ,
	FW_SWAP_RESP
#ifdef ADC_ACCELEROMETER
	,
	ACC_ENABLE_REQ,
	ACC_DISABLE_IND,
	ACC_POSITION
#endif  /* ADC_ACCELEROMETER */
} SHORTENUM vendor_cmd_id_t;

/**
 * Structure handling firmware data frames
 */
typedef struct fw_data_frame_tag {
	vendor_cmd_id_t cmd_id; /**< vendor command id */
	uint16_t frame_cnt; /**< firmware image frame counter */
	uint16_t total_num_frames; /**< total number of frames used for image */
	uint8_t fw_data[0]; /**< actual image data */
} fw_data_frame_t;

/**
 * Enum Ids for vendor status code
 */
typedef enum vendor_status_tag {
	VD_SUCCESS,
	VD_NOT_SUPPORTED_ATTRIBUTE,
	VD_UNSUPPORTED_SIZE,
	VD_INVALID_CHECKSUM
} SHORTENUM vendor_status_t;

/* === IMPLEMENTATION ====================================================== */

/**
 * @brief Vendor data request
 *
 * The vendor data request allows a device to send a vendor specific data frame.
 *
 * @param PairingRef    Pairing reference of the recipient of the transmission
 * @param ProfileId     Profile id used for the transmission
 * @param VendorId      Vendor id used for the transmission
 * @param nsduLength    Length of the payload data
 * @param nsdu          Pointer to the payload data
 * @param TxOptions     Transit options
 *
 * @return true if command could be handled; else false
 *
 * @ingroup group_RF4CONTROL_ZRC_API
 */
bool vendor_data_request(uint8_t PairingRef, profile_id_t ProfileId,
		uint16_t VendorId, uint8_t nsduLength, uint8_t *nsdu,
		uint8_t TxOptions);

/**
 * @brief Vendor data alive request
 *
 * Hook for the application indicating that an alive request command was
 * received.
 *
 * @ingroup group_RF4CONTROL_ZRC_API
 */
void vendor_app_alive_req(void);

#endif /* VENDOR_DATA_H */

#endif  /* #ifdef VENDOR_DATA */
