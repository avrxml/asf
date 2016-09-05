/**
 * @file mac_gts.c
 *
 * @brief This file implements GTS feature for MAC.
 *
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* === Includes ============================================================ */
#include <compiler.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "return_val.h"
#include "pal.h"
#include "bmm.h"
#include "qmm.h"
#include "tal.h"
#include "tal_slotted_csma.h"
#include "ieee_const.h"
#include "mac_msg_const.h"
#include "mac_api.h"
#include "mac_msg_types.h"
#include "mac_data_structures.h"
#include "stack_config.h"
#include "mac_internal.h"
#include "mac.h"
#include "mac_config.h"
#include "mac_build_config.h"
#ifdef MAC_SECURITY_ZIP
#include "mac_security.h"
#endif  /* MAC_SECURITY_ZIP */

#ifdef GTS_SUPPORT

#ifdef FFD
static bool mac_gts_allocate(gts_char_t GtsCharacteristics,
		uint16_t DevAddress);
static bool mac_gts_deallocate(gts_char_t GtsCharacteristics,
		uint16_t DevAddress, bool persist);

#endif /* FFD */

static void mac_send_gts_ind(gts_char_t GtsChar, uint16_t dev_addr);
static void process_deallocate_data_q(queue_t *q_ptr);

#ifdef GTS_DEBUG
void gts_debug(uint8_t gts_index, bool data_pin, bool set, bool val);

#endif

#ifdef FFD
/** PANC table for GTS maintenance */
mac_pan_gts_mgmt_t mac_pan_gts_table[MAX_GTS_ON_PANC];

/** Length of active GTS in PANC table */
uint8_t mac_pan_gts_table_len = 0;

/** Reference for maintaining minCAPLength */
const uint8_t minCAPLength[4] = { 9, 4, 2, 1};
#endif /* FFD */

#ifdef GTS_DEBUG
uint8_t debug_pins[7][2] = {{DEBUG_PIN4, DEBUG_PIN11},
			    {DEBUG_PIN5, DEBUG_PIN12},
			    {DEBUG_PIN6, DEBUG_PIN13},
			    {DEBUG_PIN7, DEBUG_PIN14},
			    {DEBUG_PIN8, DEBUG_PIN15},
			    {DEBUG_PIN9, DEBUG_PIN16},
			    {DEBUG_PIN10, DEBUG_PIN17}};
#endif
mac_dev_gts_mgmt_t mac_dev_gts_table[MAX_GTS_ON_DEV];
uint8_t mac_dev_gts_table_len = 0;

#ifdef FFD
static queue_t gts_q[MAX_GTS_ON_PANC];
#endif /* FFD */
static queue_t dev_tx_gts_q;

/*
 * @brief The MLME-GTS.request primitive makes a request for device to
 * request for GTS or on PANC to allocate or deallocate a GTS for itself
 * or other devices
 *
 * 802.15.4. Section 7.1.7.1.
 *
 * @param m The MLME-GTS.request message.
 */
void mlme_gts_request(uint8_t *m)
{
	mlme_gts_req_t mgr;
	memcpy(&mgr, BMM_BUFFER_POINTER((buffer_t *)m),
			sizeof(mlme_gts_req_t));

	if (MAC_NO_SHORT_ADDR_VALUE <= tal_pib.ShortAddress ||
			(MAC_NO_SHORT_ADDR_VALUE <=
			mac_pib.mac_CoordShortAddress &&
			MAC_PAN_COORD_STARTED != mac_state)) {
		mac_gen_mlme_gts_conf((buffer_t *)m, MAC_NO_SHORT_ADDRESS,
				mgr.GtsChar);
		return;
	} else if (true != mac_pib.mac_GTSPermit    ||
			(0 == mgr.GtsChar.GtsLength) ||
			(MAC_ASSOCIATED == mac_state &&
			(mgr.DeviceShortAddr != tal_pib.ShortAddress ||
			MAC_SYNC_TRACKING_BEACON != mac_sync_state ||
			(mgr.GtsChar.GtsCharType == GTS_DEALLOCATE &&
			(!mac_dev_gts_table[mgr.GtsChar.GtsDirection].
			GtsStartingSlot ||
			mgr.GtsChar.GtsLength !=
			mac_dev_gts_table[mgr.GtsChar.GtsDirection].GtsLength))
			)
			)
			) {
		mac_gen_mlme_gts_conf((buffer_t *)m, MAC_INVALID_PARAMETER,
				mgr.GtsChar);
		return;
	}

#ifdef FFD
	else if (MAC_PAN_COORD_STARTED == mac_state) {
		if (GTS_ALLOCATE & mgr.GtsChar.GtsCharType) {
			if (mac_gts_allocate(mgr.GtsChar,
					mgr.DeviceShortAddr)) {
				mac_gen_mlme_gts_conf((buffer_t *)m,
						MAC_SUCCESS,
						mgr.GtsChar);
				return;
			} else {
				mac_gen_mlme_gts_conf((buffer_t *)m,
						MAC_NO_DATA,
						mgr.GtsChar);
				return;
			}
		} else {
			if (mac_gts_deallocate(mgr.GtsChar, mgr.DeviceShortAddr,
					true)) {
				mac_gen_mlme_gts_conf((buffer_t *)m,
						MAC_SUCCESS,
						mgr.GtsChar);
				return;
			} else {
				mac_gen_mlme_gts_conf((buffer_t *)m,
						MAC_NO_DATA,
						mgr.GtsChar);
				return;
			}
		}
	}
#endif /* FFD */
	else if (MAC_ASSOCIATED == mac_state) {
		frame_info_t *transmit_frame
			= (frame_info_t *)BMM_BUFFER_POINTER((buffer_t *)m);

		mac_trx_wakeup();

		/* Build the GTS Request command frame. */
		uint8_t tal_tx_status;
		uint8_t frame_len;
		uint8_t *frame_ptr;
		uint8_t *temp_frame_ptr;
		uint16_t fcf;

		/*
		 * Use the mlme gts request buffer for transmitting
		 * gts request frame.
		 */
		frame_info_t *gts_req_frame
			= (frame_info_t *)(BMM_BUFFER_POINTER((buffer_t *)m));

		gts_req_frame->msg_type = GTSREQUEST;
		gts_req_frame->buffer_header = (buffer_t *)m;

		/* Get the payload pointer. */
		frame_ptr = temp_frame_ptr
					= (uint8_t *)gts_req_frame +
						LARGE_BUFFER_SIZE -
						GTS_REQ_PAYLOAD_LEN - 2; /* Add
		                                                          * 2
		                                                          *
		                                                          *
		                                                          *octets
		                                                          * for
		                                                          * FCS.
		                                                          **/

		/* Update the payload field. */
		*frame_ptr++ = GTSREQUEST;
		/* Build the GTS characteristics info. */
		*frame_ptr = *((uint8_t *)&mgr.GtsChar);

		/* Get the payload pointer again to add the MHR. */
		frame_ptr = temp_frame_ptr;

		/* Update the length. */
		frame_len = GTS_REQ_PAYLOAD_LEN +
				2 + /* Add 2 octets for FCS */
				2 + /* 2 octets for Destination PAN-Id */
				2 + /* 2 octets for short Destination Address */
				2 + /* 2 octets for short Source Address */
				3; /* 3 octets DSN and FCF */

		/* Source address */
		frame_ptr -= 2;

		convert_16_bit_to_byte_array(tal_pib.ShortAddress, frame_ptr);

		frame_ptr -= 2;
		convert_16_bit_to_byte_array(mac_pib.mac_CoordShortAddress,
				frame_ptr);

		fcf = FCF_SET_FRAMETYPE(FCF_FRAMETYPE_MAC_CMD) |
				FCF_SET_DEST_ADDR_MODE(FCF_SHORT_ADDR) |
				FCF_SET_SOURCE_ADDR_MODE(FCF_SHORT_ADDR) |
				FCF_ACK_REQUEST | FCF_PAN_ID_COMPRESSION;

		/* Destination PAN-Id */
		frame_ptr -= 2;
		convert_16_bit_to_byte_array(tal_pib.PANId, frame_ptr);

		/* Set DSN. */
		frame_ptr--;
		*frame_ptr = mac_pib.mac_DSN++;

		/* Set the FCF. */
		frame_ptr -= 2;
		convert_spec_16_bit_to_byte_array(fcf, frame_ptr);

		/* First element shall be length of PHY frame. */
		frame_ptr--;
		*frame_ptr = frame_len;

		/* Finished building of frame. */
		gts_req_frame->mpdu = frame_ptr;

		tal_tx_status
			= tal_tx_frame(transmit_frame, CSMA_SLOTTED, true);

		if (MAC_SUCCESS == tal_tx_status) {
			uint8_t update_index = mgr.GtsChar.GtsDirection;

			if (mgr.DeviceShortAddr ==
					mac_pib.mac_CoordShortAddress) {
				update_index |= 0x02;
			}

			if (GTS_DEALLOCATE == mgr.GtsChar.GtsCharType) {
				mac_dev_gts_table[update_index].GtsLength
					= 0;
				mac_dev_gts_table[update_index].GtsStartingSlot
					= 0;
				mac_dev_gts_table[update_index].GtsState
					= GTS_STATE_IDLE;
				mac_gen_mlme_gts_conf((buffer_t *)m,
						MAC_SUCCESS, mgr.GtsChar);
				return;
			} else {
				mac_dev_gts_table[update_index].GtsReq_ptr = m;
				mac_dev_gts_table[update_index].GtsState
					= GTS_STATE_REQ_SENT;
				mac_dev_gts_table[update_index].GtsPersistCount
					= aGTSDescPersistenceTime;
				mac_dev_gts_table[update_index].GtsLength
					= mgr.GtsChar.GtsLength;
				MAKE_MAC_BUSY();
			}
		} else {
			mac_gen_mlme_gts_conf((buffer_t *)m, tal_tx_status,
					mgr.GtsChar);
		}
	} else {
		mac_gen_mlme_gts_conf((buffer_t *)m, MAC_INVALID_PARAMETER,
				mgr.GtsChar);
	}
	return;
}

void mac_gen_mlme_gts_conf(buffer_t *buf_ptr, uint8_t status,
		gts_char_t gts_char)
{
	mlme_gts_conf_t *gts_conf
		= (mlme_gts_conf_t *)BMM_BUFFER_POINTER(buf_ptr);

	gts_conf->cmdcode = MLME_GTS_CONFIRM;
	gts_conf->status = status;
	gts_conf->GtsChar = gts_char;

	/* Append the associate confirm message to MAC-NHLE queue. */
	qmm_queue_append(&mac_nhle_q, buf_ptr);
}

#ifdef FFD
void mac_process_gts_request(buffer_t *gts_req)
{
	/* Use the frame reception buffer for association indication. */
	mlme_gts_ind_t *mgi = (mlme_gts_ind_t *)BMM_BUFFER_POINTER(
			gts_req);

	mgi->DeviceAddr = mac_parse_data.src_addr.short_address;

	mgi->GtsChar = mac_parse_data.mac_payload_data.gts_req_data;
	mgi->cmdcode = MLME_GTS_INDICATION;

	if (GTS_ALLOCATE == (mgi->GtsChar).GtsCharType) {
		if (mac_gts_allocate(mgi->GtsChar, mgi->DeviceAddr)) {
			/* Append the MLME GTS indication to the MAC-NHLE queue.
			**/
			qmm_queue_append(&mac_nhle_q, gts_req);
		} else {
			bmm_buffer_free(gts_req);
		}
	} else {
		if (mac_gts_deallocate(mgi->GtsChar, mgi->DeviceAddr, false)) {
			/* Append the MLME GTS indication to the MAC-NHLE queue.
			**/
			qmm_queue_append(&mac_nhle_q, gts_req);
		} else {
			bmm_buffer_free(gts_req);
		}
	}
}

#endif /* FFD */

#ifdef FFD
void mac_gts_table_update(void)
{
	uint8_t table_index;

	for (table_index = 0; table_index < mac_pan_gts_table_len;
			table_index++) {
		if (mac_pan_gts_table[table_index].ExpiryCount > 0 &&
				--mac_pan_gts_table[table_index].ExpiryCount ==
				0) {
			gts_char_t gts_char;
			uint16_t dev_addr
				= mac_pan_gts_table[table_index].DevShortAddr;
			gts_char.GtsDirection
				= mac_pan_gts_table[table_index].GtsDesc
					.GtsDirection;
			gts_char.GtsLength
				= mac_pan_gts_table[table_index].GtsDesc
					.GtsLength;
			gts_char.GtsCharType = GTS_DEALLOCATE;
			gts_char.Reserved = 0;
			if (mac_gts_deallocate(gts_char,
					mac_pan_gts_table[table_index].
					DevShortAddr, true)) {
				buffer_t *buffer_header;
				mlme_gts_ind_t *mgi;

				buffer_header = bmm_buffer_alloc(
						LARGE_BUFFER_SIZE);

				if (NULL == buffer_header) {
					/* Buffer is not available */
					return;
				}

				mgi = (mlme_gts_ind_t *)BMM_BUFFER_POINTER(
						buffer_header);

				mgi->DeviceAddr = dev_addr;

				mgi->GtsChar = gts_char;
				mgi->cmdcode = MLME_GTS_INDICATION;

				/* Append the MLME GTS indication to the
				 * MAC-NHLE queue. */
				qmm_queue_append(&mac_nhle_q, buffer_header);
				--table_index;
			}
		}
	}
}

#endif /* FFD */

#ifdef FFD
uint8_t mac_add_gts_info(uint8_t *frame_ptr)
{
	uint8_t table_index;
	uint8_t update_octets_count = 0;
	uint8_t direction_mask = 0;
	mac_gts_spec_t mac_gts_spec;

	mac_gts_spec.GtsDescCount = 0;
	mac_gts_spec.Reserved = 0;

	ENTER_CRITICAL_REGION();

	for (table_index = 0; table_index < MAX_GTS_ON_PANC; table_index++) {
		if (0 < mac_pan_gts_table[table_index].PersistenceCount) {
			frame_ptr--;
			*frame_ptr
				= (mac_pan_gts_table[table_index].GtsDesc.
					GtsLength
					<< 4)
					| mac_pan_gts_table[table_index].GtsDesc
					.GtsStartingSlot;
			frame_ptr--;
			*frame_ptr
				= mac_pan_gts_table[table_index].DevShortAddr >>
					8;                                             /*
			                                                                *
			                                                                *GTS
			                                                                *
			                                                                *
			                                                                *List
			                                                                **/
			frame_ptr--;
			*frame_ptr
				= mac_pan_gts_table[table_index].DevShortAddr;    /*
			                                                           *
			                                                           *GTS
			                                                           *
			                                                           *
			                                                           *List
			                                                           **/

			update_octets_count += 3;

			direction_mask <<= 1;

			if (GTS_RX_SLOT &
					mac_pan_gts_table[table_index].GtsDesc.
					GtsDirection) {
				direction_mask |= GTS_RX_SLOT;
			}

			mac_gts_spec.GtsDescCount++;

			--mac_pan_gts_table[table_index].PersistenceCount;
		}
	}

	if (mac_gts_spec.GtsDescCount > 0) {
		frame_ptr--;
		*frame_ptr = direction_mask; /* GTS Direction Mask */
		++update_octets_count;
	}

	frame_ptr--;
	mac_gts_spec.GtsPermit = mac_pib.mac_GTSPermit;
	*frame_ptr = *((uint8_t *)&mac_gts_spec);

	LEAVE_CRITICAL_REGION();
	return update_octets_count;
}

#endif /* FFD */

#ifdef FFD
bool mac_gts_allocate(gts_char_t GtsCharacteristics, uint16_t DevAddress)
{
	uint8_t Index = 0;

	if (MAX_GTS_ON_PANC <= mac_pan_gts_table_len ||
			(tal_pib.BeaconOrder < 4 &&
			((mac_final_cap_slot - GtsCharacteristics.GtsLength) <=
			minCAPLength[tal_pib.BeaconOrder])) ||
			(tal_pib.BeaconOrder >= 4 &&
			((mac_final_cap_slot +
			1) - GtsCharacteristics.GtsLength) < 1)
			) {
		return false;
	}

	for (Index = 0; Index < mac_pan_gts_table_len; Index++) {
		if (mac_pan_gts_table[Index].DevShortAddr == DevAddress &&
				mac_pan_gts_table[Index].GtsDesc.GtsDirection ==
				GtsCharacteristics.GtsDirection) {
			return false;
		}
	}

	ENTER_CRITICAL_REGION();

	mac_pan_gts_table[mac_pan_gts_table_len].DevShortAddr = DevAddress;

	mac_pan_gts_table[mac_pan_gts_table_len].GtsDesc.GtsLength
		= GtsCharacteristics.GtsLength;

	mac_pan_gts_table[mac_pan_gts_table_len].GtsDesc.GtsStartingSlot
		= (mac_final_cap_slot -
			GtsCharacteristics.GtsLength) + 1;

	mac_pan_gts_table[mac_pan_gts_table_len].GtsDesc.GtsDirection
		= GtsCharacteristics.GtsDirection;

	mac_pan_gts_table[mac_pan_gts_table_len].PersistenceCount
		= aGTSDescPersistenceTime;

	if (tal_pib.BeaconOrder >= 9) {
		mac_pan_gts_table[mac_pan_gts_table_len].ExpiryCount
			= GTS_EXPIRY_BO_9_TO_14 +
				aGTSDescPersistenceTime;
	} else {
		mac_pan_gts_table[mac_pan_gts_table_len].ExpiryCount
			= GTS_EXPIRY_BO_0_TO_8 +
				aGTSDescPersistenceTime;
	}

	mac_final_cap_slot -= GtsCharacteristics.GtsLength;

	++mac_pan_gts_table_len;

	for (Index = mac_pan_gts_table_len; Index < MAX_GTS_ON_PANC; Index++) {
		if (mac_pan_gts_table[Index].DevShortAddr == DevAddress &&
				mac_pan_gts_table[Index].GtsDesc.GtsDirection ==
				GtsCharacteristics.GtsDirection) {
			mac_pan_gts_table[Index].PersistenceCount = 0;
			mac_pan_gts_table[Index].ExpiryCount = 0;
		}
	}

	LEAVE_CRITICAL_REGION();
	return true;
}

#endif /* FFD */

#ifdef FFD
bool mac_gts_deallocate(gts_char_t GtsCharacteristics, uint16_t DevAddress,
		bool persist)
{
	uint8_t table_index;
	uint8_t table_index1;
	uint8_t temp_slot = FINAL_CAP_SLOT_DEFAULT + 1;
	queue_t *temp_ptr_q;

	for (table_index = 0; table_index < mac_pan_gts_table_len;
			table_index++) {
		/* Check to identify the GTS to be deallocated from the
		 * table...*/
		if (mac_pan_gts_table[table_index].DevShortAddr == DevAddress &&
				mac_pan_gts_table[table_index].GtsDesc.
				GtsDirection ==
				GtsCharacteristics.GtsDirection &&
				mac_pan_gts_table[table_index].GtsDesc.GtsLength
				== GtsCharacteristics.GtsLength) {
			ENTER_CRITICAL_REGION();
			temp_ptr_q = mac_pan_gts_table[table_index].gts_data_q;

			for (table_index1 = table_index;
					table_index1 <
					(mac_pan_gts_table_len - 1);
					table_index1++) {
				mac_pan_gts_table[table_index1].DevShortAddr
					= mac_pan_gts_table[table_index1
						+ 1].DevShortAddr;

				mac_pan_gts_table[table_index1].GtsDesc.
				GtsDirection
					= mac_pan_gts_table[table_index1
						+ 1].GtsDesc.GtsDirection;

				mac_pan_gts_table[table_index1].GtsDesc.
				GtsLength
					= mac_pan_gts_table[table_index1
						+ 1].GtsDesc.GtsLength;

				mac_pan_gts_table[table_index1].GtsDesc.
				GtsStartingSlot
					= temp_slot -
						mac_pan_gts_table[table_index1 +
						1].GtsDesc.GtsLength;

				mac_pan_gts_table[table_index1].ExpiryCount
					= mac_pan_gts_table[table_index1
						+ 1].ExpiryCount;

				mac_pan_gts_table[table_index1].PersistenceCount
					= aGTSDescPersistenceTime;

				mac_pan_gts_table[table_index1].gts_data_q
					= mac_pan_gts_table[table_index1
						+ 1].gts_data_q;

				temp_slot
					-= mac_pan_gts_table[table_index1].
						GtsDesc
						.GtsLength;
			}

			memset(&mac_pan_gts_table[table_index1], 0x00,
					sizeof(mac_pan_gts_mgmt_t));

			mac_pan_gts_table[table_index1].gts_data_q = temp_ptr_q;

			--mac_pan_gts_table_len;

			mac_final_cap_slot = temp_slot - 1;

			for (; table_index1 < MAX_GTS_ON_PANC; table_index1++) {
				/* mac_pan_gts_table[table_index1].DevShortAddr
				 * = mac_pan_gts_table[table_index1 +
				 * 1].DevShortAddr; */
				/*  */

				/*
				 *
				 *mac_pan_gts_table[table_index1].GtsDesc.GtsDirection
				 * = mac_pan_gts_table[table_index1 +
				 * 1].GtsDesc.GtsDirection; */
				/*  */

				/*
				 *
				 *mac_pan_gts_table[table_index1].GtsDesc.GtsLength
				 * = mac_pan_gts_table[table_index1 +
				 * 1].GtsDesc.GtsLength; */
				/*  */

				/*
				 *
				 *mac_pan_gts_table[table_index1].GtsDesc.GtsStartingSlot
				 * = mac_pan_gts_table[table_index1 +
				 * 1].GtsDesc.GtsStartingSlot; */
				/*  */

				/* mac_pan_gts_table[table_index1].ExpiryCount =
				 * mac_pan_gts_table[table_index1 +
				 * 1].ExpiryCount; */
				/*  */

				/*
				 *
				 *mac_pan_gts_table[table_index1].PersistenceCount
				 * = mac_pan_gts_table[table_index1 +
				 * 1].PersistenceCount; */
				if (0 <
						mac_pan_gts_table[table_index1 +
						1].PersistenceCount) {
					memcpy(&mac_pan_gts_table[table_index1],
							&mac_pan_gts_table[
								table_index1 +
								1],
							sizeof(
								mac_pan_gts_mgmt_t));
				}
			}

			if (persist) {
				mac_pan_gts_table[MAX_GTS_ON_PANC -
				1].DevShortAddr = DevAddress;
				mac_pan_gts_table[MAX_GTS_ON_PANC -
				1].GtsDesc.GtsDirection
					= GtsCharacteristics.
						GtsDirection;
				mac_pan_gts_table[MAX_GTS_ON_PANC -
				1].GtsDesc.GtsLength
					= GtsCharacteristics.GtsLength;
				mac_pan_gts_table[MAX_GTS_ON_PANC -
				1].GtsDesc.GtsStartingSlot = 0;
				mac_pan_gts_table[MAX_GTS_ON_PANC -
				1].PersistenceCount
					= aGTSDescPersistenceTime;
			} else {
				mac_pan_gts_table[MAX_GTS_ON_PANC -
				1].PersistenceCount = 0;
			}

			LEAVE_CRITICAL_REGION();
			process_deallocate_data_q(temp_ptr_q);
			return true;
		} else {
			temp_slot
				-= mac_pan_gts_table[table_index].GtsDesc.
					GtsLength;
		}
	}
	return false;
}

#endif /* FFD */

void mac_parse_bcn_gts_info(uint8_t gts_count, uint8_t gts_dir,
		mac_gts_list_t *gts_list_ptr)
{
	uint8_t loop_index, table_index;
	gts_char_t gts_char;
	uint8_t curr_gts_dir;
	uint16_t device_addr;

	gts_char.Reserved = 0;

	for (loop_index = 0; loop_index < gts_count; loop_index++) {
		curr_gts_dir = ((gts_dir >> loop_index) & 0x01);

		table_index = curr_gts_dir;
		device_addr
			= (gts_list_ptr->dev_addr[1] <<
				8) | gts_list_ptr->dev_addr[0];

		gts_char.GtsCharType = GTS_ALLOCATE;
		gts_char.GtsDirection = curr_gts_dir;
		gts_char.GtsLength = gts_list_ptr->length;

		if (device_addr == tal_pib.ShortAddress || device_addr ==
				mac_pib.mac_CoordShortAddress) {
			if (device_addr == mac_pib.mac_CoordShortAddress) {
				table_index |= 0x02;
			}

			if (GTS_STATE_REQ_SENT ==
					mac_dev_gts_table[table_index].GtsState)
			{
				if (0 == gts_list_ptr->starting_slot) {
					mac_dev_gts_table[table_index].GtsState
						= GTS_STATE_IDLE;
					mac_gen_mlme_gts_conf(
							(buffer_t *)mac_dev_gts_table[
								table_index].GtsReq_ptr,
							MAC_DENIED,
							gts_char);
				} else if (gts_list_ptr->length ==
						mac_dev_gts_table[table_index].
						GtsLength) {
					mac_dev_gts_table[table_index].
					GtsStartingSlot
						= gts_list_ptr->
							starting_slot;
					mac_dev_gts_table[table_index].GtsState
						= GTS_STATE_ALLOCATED;
					mac_gen_mlme_gts_conf(
							(buffer_t *)mac_dev_gts_table[
								table_index].GtsReq_ptr,
							MAC_SUCCESS,
							gts_char);
				}
			} else if (GTS_STATE_IDLE ==
					mac_dev_gts_table[table_index].GtsState
					&&
					0 != gts_list_ptr->starting_slot) {
				mac_dev_gts_table[table_index].GtsLength
					= gts_list_ptr->length;
				mac_dev_gts_table[table_index].GtsStartingSlot
					= gts_list_ptr->starting_slot;
				mac_send_gts_ind(gts_char, device_addr);
				mac_dev_gts_table[table_index].GtsState
					= GTS_STATE_ALLOCATED;
			} else if (GTS_STATE_ALLOCATED ==
					mac_dev_gts_table[table_index].GtsState
					&&
					0 == gts_list_ptr->starting_slot) {
				mac_dev_gts_table[table_index].GtsLength
					= gts_list_ptr->length;
				mac_dev_gts_table[table_index].GtsStartingSlot
					= gts_list_ptr->starting_slot;
				gts_char.GtsCharType = GTS_DEALLOCATE;
				mac_send_gts_ind(gts_char, device_addr);
				mac_dev_gts_table[table_index].GtsState
					= GTS_STATE_IDLE;
				if (DEV_TX_SLOT_INDEX == table_index) {
					process_deallocate_data_q(&dev_tx_gts_q);
				}
			} else {
				mac_dev_gts_table[table_index].GtsStartingSlot
					= gts_list_ptr->starting_slot;
			}
		}

		gts_list_ptr++;
	}
}

void mac_send_gts_ind(gts_char_t GtsChar, uint16_t dev_addr)
{
	buffer_t *buffer_header;
	mlme_gts_ind_t *mgi;

	buffer_header = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	if (NULL == buffer_header) {
		/* Buffer is not available */
		Assert("Buffer not allocated..." == 0);
	}

	GtsChar.Reserved = 0;
	mgi = (mlme_gts_ind_t *)BMM_BUFFER_POINTER(buffer_header);

	mgi->DeviceAddr = dev_addr;
	mgi->GtsChar = GtsChar;
	mgi->cmdcode = MLME_GTS_INDICATION;

	/* Append the MLME GTS indication to the MAC-NHLE queue. */
	qmm_queue_append(&mac_nhle_q, buffer_header);
}

void handle_gts_data_req(mcps_data_req_t *data_req, uint8_t *msg)
{
	uint32_t slot_duration1 = 0;
	uint32_t frame_tx_time = 0;
#ifdef FFD
	uint8_t loop_index = 0;
#endif /* FFD */
	uint16_t dst_addr;

	frame_info_t *transmit_frame;
	buffer_t *buf_ptr;

	queue_t *q_ptr = NULL;
	ADDR_COPY_DST_SRC_16(dst_addr, data_req->DstAddr);
#ifdef FFD
	if (MAC_PAN_COORD_STARTED == mac_state) {
		for (loop_index = 0; loop_index < mac_pan_gts_table_len;
				loop_index++) {
			if (dst_addr ==
					mac_pan_gts_table[loop_index].
					DevShortAddr
					&&
					GTS_RX_SLOT ==
					mac_pan_gts_table[loop_index].GtsDesc.
					GtsDirection) {
				transmit_frame
					= (frame_info_t *)BMM_BUFFER_POINTER(
						(buffer_t *)msg);
				frame_tx_time = calc_frame_transmit_duration(
						transmit_frame->mpdu);
				slot_duration1
					=  (TAL_CONVERT_SYMBOLS_TO_US(
						TAL_GET_SUPERFRAME_DURATION_TIME(
						tal_pib.SuperFrameOrder)) >>
						4) *
						mac_pan_gts_table[loop_index].
						GtsDesc.GtsLength;
				if (slot_duration1 < frame_tx_time) {
					mac_gen_mcps_data_conf((buffer_t *)msg,
							(uint8_t)MAC_FRAME_TOO_LONG,
					#ifdef ENABLE_TSTAMP
							data_req->msduHandle,
							0);
					#else
							data_req->msduHandle);
					#endif  /* ENABLE_TSTAMP */
					return;
				} else {
					/* Append the MCPS data request into the
					 * GTS queue */
					#ifdef ENABLE_QUEUE_CAPACITY
					if (QUEUE_FULL ==
							qmm_queue_append(
							mac_pan_gts_table
							[loop_index].gts_data_q,
							(buffer_t *)msg)) {
						mac_gen_mcps_data_conf((buffer_t
								*)msg,
								(uint8_t)MAC_CHANNEL_ACCESS_FAILURE,
						#ifdef ENABLE_TSTAMP
								data_req->msduHandle,
								0);
						#else
								data_req->
								msduHandle);
						#endif  /* ENABLE_TSTAMP */
					}

					#else
					qmm_queue_append(mac_pan_gts_table[
								loop_index].gts_data_q,
							(buffer_t *)msg);
					#endif   /* ENABLE_QUEUE_CAPACITY */
					q_ptr
						= mac_pan_gts_table[loop_index].
							gts_data_q;
					break;
				}
			}
		}
	} else
#endif /* FFD */
	if (MAC_ASSOCIATED == mac_state && mac_pib.mac_CoordShortAddress ==
			dst_addr &&
			0 !=
			mac_dev_gts_table[DEV_TX_SLOT_INDEX].GtsStartingSlot) {
		transmit_frame = (frame_info_t *)BMM_BUFFER_POINTER(
				(buffer_t *)msg);
		frame_tx_time = calc_frame_transmit_duration(
				transmit_frame->mpdu);
		slot_duration1
			=  (TAL_CONVERT_SYMBOLS_TO_US(
				TAL_GET_SUPERFRAME_DURATION_TIME(
				tal_pib.SuperFrameOrder)) >>
				4) *
				mac_dev_gts_table[DEV_TX_SLOT_INDEX].GtsLength;
		if (slot_duration1 < frame_tx_time) {
			mac_gen_mcps_data_conf((buffer_t *)msg,
					(uint8_t)MAC_FRAME_TOO_LONG,
			#ifdef ENABLE_TSTAMP
					data_req->msduHandle,
					0);
			#else
					data_req->msduHandle);
			#endif  /* ENABLE_TSTAMP */
			return;
		} else {
			/* Append the MCPS data request into the GTS queue */
			#ifdef ENABLE_QUEUE_CAPACITY
			if (QUEUE_FULL ==
					qmm_queue_append(&dev_tx_gts_q,
					(buffer_t *)msg)) {
				mac_gen_mcps_data_conf((buffer_t *)msg,
						(uint8_t)MAC_CHANNEL_ACCESS_FAILURE,
				#ifdef ENABLE_TSTAMP
						data_req->msduHandle,
						0);
				#else
						data_req->msduHandle);
				#endif  /* ENABLE_TSTAMP */
			}

			#else
			qmm_queue_append(&dev_tx_gts_q, (buffer_t *)msg);
			#endif   /* ENABLE_QUEUE_CAPACITY */
			q_ptr = &dev_tx_gts_q;
		}
	}

	if (NULL == q_ptr) {
		mac_gen_mcps_data_conf((buffer_t *)msg,
				(uint8_t)MAC_INVALID_GTS,
			#ifdef ENABLE_TSTAMP
				data_req->msduHandle, 0);
			#else
				data_req->msduHandle);
			#endif  /* ENABLE_TSTAMP */
	} else {
#ifdef FFD
		if (MAC_PAN_COORD_STARTED == mac_state && 1 == q_ptr->size) {
			loop_index = loop_index + MAC_ACTIVE_CFP_GTS1;
			if (loop_index == mac_superframe_state) {
				buf_ptr = qmm_queue_read(q_ptr, NULL);

				if (NULL == buf_ptr) {
					/* Nothing to be done. */
					return;
				}

				transmit_frame
					= (frame_info_t *)BMM_BUFFER_POINTER(
						buf_ptr);

				transmit_frame->buffer_header = buf_ptr;

				frame_tx_time = calc_frame_transmit_duration(
						transmit_frame->mpdu);
				slot_duration1 = sw_timer_get_residual_time(
						T_CAP);
				if (slot_duration1 > frame_tx_time) {
					mac_tx_gts_data(q_ptr);
					qmm_queue_remove(q_ptr, NULL);
				}
			}
		} else
#endif /* FFD */
		if (MAC_ASSOCIATED == mac_state && 1 == q_ptr->size) {
			if (MAC_DEV_GTS_TX == mac_superframe_state) {
				buf_ptr = qmm_queue_read(q_ptr, NULL);

				if (NULL == buf_ptr) {
					/* Nothing to be done. */
					return;
				}

				transmit_frame
					= (frame_info_t *)BMM_BUFFER_POINTER(
						buf_ptr);

				transmit_frame->buffer_header = buf_ptr;

				frame_tx_time = calc_frame_transmit_duration(
						transmit_frame->mpdu);
				slot_duration1 = sw_timer_get_residual_time(
						T_CAP);
				if (slot_duration1 > frame_tx_time) {
					mac_tx_gts_data(q_ptr);
					qmm_queue_remove(q_ptr, NULL);
				}
			}
		}
	}

	return;
}

void reset_gts_globals(void)
{
#ifdef FFD
	mac_pan_gts_table_len = 0;
	memset(&mac_pan_gts_table, 0,
			sizeof(mac_pan_gts_mgmt_t) * MAX_GTS_ON_PANC);
#endif /* FFD */
	mac_dev_gts_table_len = 0;
	memset(&mac_dev_gts_table, 0,
			sizeof(mac_dev_gts_mgmt_t) * MAX_GTS_ON_DEV);
	init_gts_queues();
}

void mac_t_gts_cb(void *callback_parameter)
{
	uint32_t next_timer_dur = 0;
	uint32_t slot_duration;
	mac_trx_wakeup();
	#ifdef GTS_DEBUG
	port_pin_set_output_level(DEBUG_PIN3, 0);
	#endif
	ENTER_CRITICAL_REGION();
	slot_duration
		= (TAL_CONVERT_SYMBOLS_TO_US(TAL_GET_SUPERFRAME_DURATION_TIME(
			tal_pib.SuperFrameOrder)) >> 4);
#ifdef FFD
	if (MAC_PAN_COORD_STARTED == mac_state && mac_pan_gts_table_len > 0) {
		queue_t *temp_ptr = NULL;
		uint8_t temp_index = 0;
		if (MAC_ACTIVE_CAP == mac_superframe_state) {
			temp_index = mac_pan_gts_table_len - 1;
			next_timer_dur = slot_duration *
					(mac_pan_gts_table[temp_index].GtsDesc.
					GtsLength);
			mac_superframe_state = MAC_ACTIVE_CFP_GTS1;
			temp_ptr = mac_pan_gts_table[temp_index].gts_data_q;
			#ifdef GTS_DEBUG
			gts_debug(MAC_ACTIVE_CAP, GTS_DEBUG_SLOT_PIN,
					GTS_DEBUG_SET, GTS_DEBUG_VALUE_HIGH);
			#endif
		} else if (MAC_ACTIVE_CFP_GTS1 == mac_superframe_state &&
				(mac_pan_gts_table_len >= 2)) {
			temp_index = mac_pan_gts_table_len - 2;
			next_timer_dur = slot_duration *
					(mac_pan_gts_table[mac_pan_gts_table_len
					- 2].GtsDesc.GtsLength);
			mac_superframe_state = MAC_ACTIVE_CFP_GTS2;
			temp_ptr = mac_pan_gts_table[temp_index].gts_data_q;
			#ifdef GTS_DEBUG
			gts_debug(MAC_ACTIVE_CAP, GTS_DEBUG_SLOT_PIN,
					GTS_DEBUG_SET, GTS_DEBUG_VALUE_LOW);
			gts_debug(MAC_ACTIVE_CFP_GTS1, GTS_DEBUG_SLOT_PIN,
					GTS_DEBUG_SET, GTS_DEBUG_VALUE_HIGH);
			#endif
		} else if (MAC_ACTIVE_CFP_GTS2 == mac_superframe_state &&
				(mac_pan_gts_table_len >= 3)) {
			temp_index = mac_pan_gts_table_len - 3;
			next_timer_dur = slot_duration *
					(mac_pan_gts_table[mac_pan_gts_table_len
					- 3].GtsDesc.GtsLength);
			mac_superframe_state = MAC_ACTIVE_CFP_GTS3;
			temp_ptr = mac_pan_gts_table[temp_index].gts_data_q;
			#ifdef GTS_DEBUG
			gts_debug(MAC_ACTIVE_CFP_GTS1, GTS_DEBUG_SLOT_PIN,
					GTS_DEBUG_SET, GTS_DEBUG_VALUE_LOW);
			gts_debug(MAC_ACTIVE_CFP_GTS2, GTS_DEBUG_SLOT_PIN,
					GTS_DEBUG_SET, GTS_DEBUG_VALUE_HIGH);
			#endif
		} else if (MAC_ACTIVE_CFP_GTS3 == mac_superframe_state &&
				(mac_pan_gts_table_len >= 4)) {
			temp_index = mac_pan_gts_table_len - 4;
			next_timer_dur = slot_duration *
					(mac_pan_gts_table[mac_pan_gts_table_len
					- 4].GtsDesc.GtsLength);
			mac_superframe_state = MAC_ACTIVE_CFP_GTS4;
			temp_ptr = mac_pan_gts_table[temp_index].gts_data_q;
			#ifdef GTS_DEBUG
			gts_debug(MAC_ACTIVE_CFP_GTS2, GTS_DEBUG_SLOT_PIN,
					GTS_DEBUG_SET, GTS_DEBUG_VALUE_LOW);
			gts_debug(MAC_ACTIVE_CFP_GTS3, GTS_DEBUG_SLOT_PIN,
					GTS_DEBUG_SET, GTS_DEBUG_VALUE_HIGH);
			#endif
		} else if (MAC_ACTIVE_CFP_GTS4 == mac_superframe_state &&
				(mac_pan_gts_table_len >= 5)) {
			temp_index = mac_pan_gts_table_len - 5;
			next_timer_dur = slot_duration *
					(mac_pan_gts_table[mac_pan_gts_table_len
					- 5].GtsDesc.GtsLength);
			mac_superframe_state = MAC_ACTIVE_CFP_GTS5;
			temp_ptr = mac_pan_gts_table[temp_index].gts_data_q;
			#ifdef GTS_DEBUG
			gts_debug(MAC_ACTIVE_CFP_GTS3, GTS_DEBUG_SLOT_PIN,
					GTS_DEBUG_SET, GTS_DEBUG_VALUE_LOW);
			gts_debug(MAC_ACTIVE_CFP_GTS4, GTS_DEBUG_SLOT_PIN,
					GTS_DEBUG_SET, GTS_DEBUG_VALUE_HIGH);
			#endif
		} else if (MAC_ACTIVE_CFP_GTS5 == mac_superframe_state &&
				(mac_pan_gts_table_len >= 6)) {
			temp_index = mac_pan_gts_table_len - 6;
			next_timer_dur = slot_duration *
					(mac_pan_gts_table[mac_pan_gts_table_len
					- 6].GtsDesc.GtsLength);
			mac_superframe_state = MAC_ACTIVE_CFP_GTS6;
			temp_ptr = mac_pan_gts_table[temp_index].gts_data_q;
			#ifdef GTS_DEBUG
			gts_debug(MAC_ACTIVE_CFP_GTS4, GTS_DEBUG_SLOT_PIN,
					GTS_DEBUG_SET, GTS_DEBUG_VALUE_LOW);
			gts_debug(MAC_ACTIVE_CFP_GTS5, GTS_DEBUG_SLOT_PIN,
					GTS_DEBUG_SET, GTS_DEBUG_VALUE_HIGH);
			#endif
		} else if (MAC_ACTIVE_CFP_GTS6 == mac_superframe_state &&
				(mac_pan_gts_table_len == 7)) {
			temp_index = mac_pan_gts_table_len - 7;
			next_timer_dur = slot_duration *
					(mac_pan_gts_table[mac_pan_gts_table_len
					- 7].GtsDesc.GtsLength);
			mac_superframe_state = MAC_ACTIVE_CFP_GTS7;
			temp_ptr = mac_pan_gts_table[temp_index].gts_data_q;
			#ifdef GTS_DEBUG
			gts_debug(MAC_ACTIVE_CFP_GTS5, GTS_DEBUG_SLOT_PIN,
					GTS_DEBUG_SET, GTS_DEBUG_VALUE_LOW);
			gts_debug(MAC_ACTIVE_CFP_GTS6, GTS_DEBUG_SLOT_PIN,
					GTS_DEBUG_SET, GTS_DEBUG_VALUE_HIGH);
			#endif
		} else { /* Do nothing as mac_superframe_state will be modified
			  * at end of Active reagion*/
		}

		if (NULL != temp_ptr && (*temp_ptr).size > 0) {
			reset_gts_expiry(&mac_pan_gts_table[temp_index]);
			#ifdef GTS_DEBUG
			gts_debug((mac_pan_gts_table_len -
					((mac_superframe_state -
					MAC_ACTIVE_CFP_GTS1) + 1)),
					GTS_DEBUG_DATA_PIN, GTS_DEBUG_TOGGLE,
					0);
			#endif
			mac_tx_gts_data(temp_ptr);
		}
	} else
#endif /* FFD */
	if (MAC_ASSOCIATED == mac_state) {
		if (MAC_ACTIVE_CAP == mac_superframe_state) {
			if (0 ==
					mac_dev_gts_table[DEV_TX_SLOT_INDEX].
					GtsStartingSlot) {
				next_timer_dur = slot_duration *
						(FINAL_CAP_SLOT_DEFAULT -
						mac_final_cap_slot);
				mac_superframe_state = MAC_DEV_GTS_IDLE;
			} else if ((mac_final_cap_slot + 1) ==
					mac_dev_gts_table[DEV_TX_SLOT_INDEX].
					GtsStartingSlot) {
				next_timer_dur = slot_duration *
						(mac_dev_gts_table[
							DEV_TX_SLOT_INDEX].
						GtsLength);
				mac_superframe_state = MAC_DEV_GTS_TX;
				mac_tx_gts_data(&dev_tx_gts_q);
			} else {
				next_timer_dur = slot_duration *
						(mac_dev_gts_table[
							DEV_TX_SLOT_INDEX].
						GtsStartingSlot -
						(mac_final_cap_slot + 1));
				mac_superframe_state = MAC_DEV_GTS_IDLE1;
			}
		} else if (MAC_DEV_GTS_IDLE1 == mac_superframe_state) {
			next_timer_dur = slot_duration *
					(mac_dev_gts_table[DEV_TX_SLOT_INDEX].
					GtsLength);
			mac_superframe_state = MAC_DEV_GTS_TX;
			mac_tx_gts_data(&dev_tx_gts_q);
		} else if (MAC_DEV_GTS_TX == mac_superframe_state) {
			uint8_t temp
				= (mac_dev_gts_table[DEV_TX_SLOT_INDEX].
					GtsStartingSlot
					- 1) +
					mac_dev_gts_table[DEV_TX_SLOT_INDEX].
					GtsLength;
			if (FINAL_CAP_SLOT_DEFAULT != temp) {
				next_timer_dur = slot_duration *
						(FINAL_CAP_SLOT_DEFAULT - temp);
				mac_superframe_state = MAC_DEV_GTS_IDLE2;
			}
		}
	}

	if (0 != next_timer_dur) {
		pal_timer_start(T_CAP, next_timer_dur,
				TIMEOUT_RELATIVE,
				(FUNC_PTR)mac_t_gts_cb,
				NULL);
	}

	callback_parameter = callback_parameter;
	LEAVE_CRITICAL_REGION();
}

void init_gts_queues(void)
{
#ifdef FFD
	uint8_t loop_index;
	for (loop_index = 0; loop_index < MAX_GTS_ON_PANC; loop_index++) {
#ifdef ENABLE_QUEUE_CAPACITY
		qmm_queue_init(&gts_q[loop_index], GTS_QUEUE_CAPACITY);
#else
		qmm_queue_init(&gts_q[loop_index]);
#endif  /* ENABLE_QUEUE_CAPACITY */
		mac_pan_gts_table[loop_index].gts_data_q = &gts_q[loop_index];
	}
#else
#ifdef ENABLE_QUEUE_CAPACITY
	qmm_queue_init(&dev_tx_gts_q, GTS_QUEUE_CAPACITY);
#else
	qmm_queue_init(&dev_tx_gts_q);
#endif  /* ENABLE_QUEUE_CAPACITY */
#endif
}

void mac_tx_gts_data(queue_t *gts_data)
{
	buffer_t *buf_ptr;
	frame_info_t *transmit_frame;
	retval_t tal_tx_status;
	buf_ptr = qmm_queue_remove(gts_data, NULL);

	Assert(buf_ptr != NULL);

	if (NULL == buf_ptr) {
		/* Nothing to be done. */
		return;
	}

	/* Broadcast data present and to be sent. */
	transmit_frame = (frame_info_t *)BMM_BUFFER_POINTER(buf_ptr);

	transmit_frame->buffer_header = buf_ptr;

	transmit_frame->gts_queue = gts_data;

#ifdef MAC_SECURITY_ZIP
	if (transmit_frame->mpdu[1] & FCF_SECURITY_ENABLED) {
		mcps_data_req_t pmdr;

		build_sec_mcps_data_frame(&pmdr, transmit_frame);

		if (pmdr.SecurityLevel > 0) {
			/* Secure the Frame */
			retval_t build_sec = mac_secure(transmit_frame,	\
					transmit_frame->mac_payload, &pmdr);

			if (MAC_SUCCESS != build_sec) {
				/* The MAC Data Payload is encrypted based on
				 * the security level. */
				mac_gen_mcps_data_conf(
						(buffer_t *)transmit_frame->buffer_header,
						(uint8_t)build_sec,
			#ifdef ENABLE_TSTAMP
						pmdr.msduHandle,
						0);
			#else
						pmdr.msduHandle);
			#endif  /* ENABLE_TSTAMP */

				return;
			}
		}
	}

#endif

	tal_tx_status = tal_tx_frame(transmit_frame, NO_CSMA_WITH_IFS, true);

	if (MAC_SUCCESS == tal_tx_status) {
		MAKE_MAC_BUSY();
		#ifdef GTS_DEBUG
		gts_debug((mac_pan_gts_table_len -
				((mac_superframe_state -
				MAC_ACTIVE_CFP_GTS1) + 1)), GTS_DEBUG_DATA_PIN,
				GTS_DEBUG_TOGGLE, 0);
		#endif
	} else {
		mac_gen_mcps_data_conf(
				(buffer_t *)transmit_frame->buffer_header,
				(uint8_t)MAC_CHANNEL_ACCESS_FAILURE,
		#ifdef ENABLE_TSTAMP
				transmit_frame->msduHandle,
				0);
		#else
				transmit_frame->msduHandle);
		#endif  /* ENABLE_TSTAMP */
	}
}

void handle_gts_data_tx_end(void)
{
#ifdef FFD
	if (MAC_PAN_COORD_STARTED == mac_state) {
		if (MAC_ACTIVE_CFP_GTS1 <= mac_superframe_state &&
				MAC_ACTIVE_CFP_GTS7 >= mac_superframe_state) {
			mac_tx_gts_data(mac_pan_gts_table[mac_pan_gts_table_len
					- ((mac_superframe_state -
					MAC_ACTIVE_CFP_GTS1) + 1)].gts_data_q);
		}
	} else
#endif /* FFD */
	if (MAC_ASSOCIATED == mac_state && MAC_DEV_GTS_TX ==
			mac_superframe_state) {
		#ifdef GTS_DEBUG
		port_pin_toggle_output_level(DEBUG_PIN12);
		#endif
		mac_tx_gts_data(&dev_tx_gts_q);
	}
}

#ifdef GTS_DEBUG
void gts_debug(uint8_t gts_index, bool data_pin, bool set, bool val)
{
	if (set) {
		if (data_pin) {
			port_pin_set_output_level(debug_pins[gts_index][1],
					val);
		} else {
			port_pin_set_output_level(debug_pins[gts_index][0],
					val);
		}
	} else {
		if (data_pin) {
			port_pin_toggle_output_level(debug_pins[gts_index][1]);
		} else {
			port_pin_toggle_output_level(debug_pins[gts_index][0]);
		}
	}
}

#endif
static void process_deallocate_data_q(queue_t *q_ptr)
{
	buffer_t *buf_ptr;
	frame_info_t *transmit_frame;
	while (q_ptr->size > 0) {
		buf_ptr = qmm_queue_remove(q_ptr, NULL);

		transmit_frame = (frame_info_t *)BMM_BUFFER_POINTER(buf_ptr);

		mac_gen_mcps_data_conf(
				(buffer_t *)buf_ptr,
				(uint8_t)MAC_INVALID_GTS,
			#ifdef ENABLE_TSTAMP
				transmit_frame->msduHandle,
				0);
			#else
				transmit_frame->msduHandle);
			#endif  /* ENABLE_TSTAMP */
	}
}

void flush_gts_queues(void)
{
#ifdef FFD
	uint8_t Index;
	for (Index = 0; Index < MAX_GTS_ON_PANC; Index++) {
		qmm_queue_flush(&gts_q[Index]);
	}
#endif /* FFD */
	qmm_queue_flush(&dev_tx_gts_q);
}

#ifdef FFD
void reset_gts_expiry(mac_pan_gts_mgmt_t *mac_pan_gts_entry)
{
	if (tal_pib.BeaconOrder >= 9 && mac_pan_gts_entry->ExpiryCount <
			GTS_EXPIRY_BO_9_TO_14) {
		mac_pan_gts_entry->ExpiryCount = GTS_EXPIRY_BO_9_TO_14;
	} else if (tal_pib.BeaconOrder < 9 && mac_pan_gts_entry->ExpiryCount <
			GTS_EXPIRY_BO_0_TO_8) {
		mac_pan_gts_entry->ExpiryCount = GTS_EXPIRY_BO_0_TO_8;
	}
}

#endif /* FFD */

void handle_gts_sync_loss(void)
{
	uint8_t table_index;
	gts_char_t gts_char;
	for (table_index = 0; table_index < MAX_GTS_ON_DEV; table_index++) {
		if (GTS_STATE_ALLOCATED ==
				mac_dev_gts_table[table_index].GtsState) {
			gts_char.GtsCharType = GTS_DEALLOCATE;
			gts_char.GtsDirection = table_index & 0x01;
			gts_char.GtsLength
				= mac_dev_gts_table[table_index].GtsLength;

			if (table_index & 0x02) {
				mac_send_gts_ind(gts_char,
						mac_pib.mac_CoordShortAddress);
			} else {
				mac_send_gts_ind(gts_char,
						tal_pib.ShortAddress);
			}
		} else if (GTS_STATE_REQ_SENT ==
				mac_dev_gts_table[table_index].GtsState) {
			mac_gen_mlme_gts_conf((buffer_t *)mac_dev_gts_table[
						table_index].GtsReq_ptr,
					MAC_NO_DATA,
					gts_char);
		}

		memset(&mac_dev_gts_table[table_index], 0,
				sizeof(mac_pan_gts_mgmt_t));
	}
}

#endif /* GTS_SUPPORT */
