/**
 * @file vendor_data.h
 *
 * @brief Declarations of API functions to access vendor specific data
 * functionality
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
