/**
 * @file mac_tx_coord_realignment_command.c
 *
 * @brief Implements the coordinator realignment command. *
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
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "return_val.h"
#include "pal.h"
#include "bmm.h"
#include "qmm.h"
#include "tal.h"
#include "ieee_const.h"
#include "mac_msg_const.h"
#include "mac_api.h"
#include "mac_msg_types.h"
#include "mac_data_structures.h"
#include "stack_config.h"
#include "mac_internal.h"
#include "mac.h"

/* === Macros =============================================================== */

/*
 * Coordinator realignment payload length
 */

/*
 * In 802.15.4-2006 the channel page may be added, if the new channel page is
 * different than the original value. In order to simplify the code, it
 * is always added.
 */
#define COORD_REALIGN_PAYLOAD_LEN       (9)

/* === Globals ============================================================== */

/* === Prototypes =========================================================== */

/* === Implementation ======================================================= */

#if ((MAC_ORPHAN_INDICATION_RESPONSE == 1) || (MAC_START_REQUEST_CONFIRM == 1))

/**
 * @brief Sends a coordinator realignment command frame
 *
 * This function is called either in response to the reception of an orphan
 * notification command from a device (cmd_type = ORPHANREALIGNMENT),
 * or gratuitously whenever the PAN parameters are about to be changed
 * (cmd_type = COORDINATORREALIGNMENT). In the first case, the
 * paramater mor contains a pointer to the respective
 * MLME_ORPHAN.response message, while in the latter case this
 * parameter is unused, and can be passed as NULL.
 *
 * @param cmd_type Determines directed or broadcast mode
 * @param buf_ptr Pointer to the buffer, using which coord_realignment_command
 *                to be sent
 * @param new_panid Contains the new PAN-ID in case there is a network
 *                  realignment
 * @param new_channel Contains the new channel in case there is a network
 *                    realignment
 * @param new_page Contains the new channel page in case there is a network
 *                 realignment
 *
 * @return True if coord_realignment_command is sent successfully,
 *         false otherwise
 */
bool mac_tx_coord_realignment_command(frame_msgtype_t cmd_type,
		buffer_t *buf_ptr,
		uint16_t new_panid,
		uint8_t new_channel,
		uint8_t new_page)
{
	retval_t tal_tx_status;
	uint8_t frame_len;
	uint8_t *frame_ptr;
	uint8_t *temp_frame_ptr;
	uint16_t fcf;
	uint16_t bc_addr = BROADCAST;

	/*
	 * Orphan request is reused to send coordinator realignment
	 * command frame and finally to send comm-status-indication
	 */
	mlme_orphan_resp_t orphan_resp;
	memcpy(&orphan_resp,
			(mlme_orphan_resp_t *)BMM_BUFFER_POINTER((buffer_t *)
			buf_ptr),
			sizeof(mlme_orphan_resp_t));

	frame_info_t *coord_realignment_frame
		= (frame_info_t *)BMM_BUFFER_POINTER((buffer_t *)buf_ptr);

	coord_realignment_frame->msg_type = cmd_type;
	coord_realignment_frame->buffer_header = buf_ptr;

	/* Get the payload pointer. */
	frame_ptr = temp_frame_ptr
				= (uint8_t *)coord_realignment_frame +
					LARGE_BUFFER_SIZE -
					COORD_REALIGN_PAYLOAD_LEN - 2; /* Add 2
	                                                                * octets
	                                                                * for
	                                                                * FCS.
	                                                                **/

	/* Update the payload field. */
	*frame_ptr++ = COORDINATORREALIGNMENT;

	/*
	 * The payload of the frame has the parameters of the new PAN
	 * configuration
	 */
#ifdef TEST_HARNESS_BIG_ENDIAN
	*frame_ptr++ = (new_panid >> 8);
	*frame_ptr++ =  new_panid;

	*frame_ptr++ = (tal_pib.ShortAddress >> 8);
	*frame_ptr++ =  tal_pib.ShortAddress;
#else
	*frame_ptr++ = new_panid;
	*frame_ptr++ = (new_panid >> 8);

	*frame_ptr++ = tal_pib.ShortAddress;
	*frame_ptr++ = (tal_pib.ShortAddress >> 8);
#endif
	*frame_ptr++ = new_channel;

	/*
	 * Insert the device's short address, or 0xFFFF if this is a
	 * gratuitous realigment.
	 */
	if (ORPHANREALIGNMENT == cmd_type) {
#ifdef TEST_HARNESS_BIG_ENDIAN
		*frame_ptr++ = (orphan_resp.ShortAddress >> 8);
		*frame_ptr++ = orphan_resp.ShortAddress;
#else
		*frame_ptr++ = orphan_resp.ShortAddress;
		*frame_ptr++ = (orphan_resp.ShortAddress >> 8);
#endif
	} else {
		*frame_ptr++ = (uint8_t)BROADCAST;
		*frame_ptr++ = (BROADCAST >> 8);
	}

	/* Add channel page no matter if it changes or not. */
	*frame_ptr++ = new_page;

	/* Get the payload pointer again to add the MHR. */
	frame_ptr = temp_frame_ptr;

	/* Update the length. */
	frame_len = COORD_REALIGN_PAYLOAD_LEN +
			2 + /* Add 2 octets for FCS */
			2 + /* 2 octets for Destination PAN-Id */
			2 + /* 2 octets for short Destination Address */
			2 + /* 2 octets for Source PAN-Id */
			8 + /* 8 octets for long Source Address */
			3; /* 3 octets DSN and FCF */

	/* Source address */
	frame_ptr -= 8;
	convert_64_bit_to_byte_array(tal_pib.IeeeAddress, frame_ptr);

	/* Source PAN-Id */
	frame_ptr -= 2;
	convert_16_bit_to_byte_array(tal_pib.PANId, frame_ptr);

	/* Destination Address and FCF */
	if (ORPHANREALIGNMENT == cmd_type) {
		/*
		 * Coordinator realignment in response to an orphan
		 * notification command received from a device. This is always
		 * sent to a 64-bit device address, and the device is
		 * requested to acknowledge the reception.
		 */
		fcf
			= FCF_SET_SOURCE_ADDR_MODE(FCF_LONG_ADDR) |
				FCF_SET_DEST_ADDR_MODE(FCF_LONG_ADDR) |
				FCF_SET_FRAMETYPE(FCF_FRAMETYPE_MAC_CMD) |
				FCF_ACK_REQUEST | FCF_FRAME_VERSION_2006;

		frame_ptr -= 8;
		frame_len += 6; /* Add further 6 octets for long Destination
		                 * Address */
		convert_64_bit_to_byte_array(orphan_resp.OrphanAddress,
				frame_ptr);
	} else {
		/*
		 * Coordinator realignment gratuitously sent when the PAN
		 * configuration changes. This is sent to the (16-bit)
		 * broadcast address.
		 */
		fcf
			= FCF_SET_SOURCE_ADDR_MODE(FCF_LONG_ADDR) |
				FCF_SET_DEST_ADDR_MODE(FCF_SHORT_ADDR) |
				FCF_SET_FRAMETYPE(FCF_FRAMETYPE_MAC_CMD) |

		                /*
		                 * Since the channel page is always added at the
		                 * end of the
		                 * coordinator realignment command frame, the
		                 * frame version subfield
		                 * needs to indicate a 802.15.4-2006 compatible
		                 * frame.
		                 */
				FCF_FRAME_VERSION_2006;

		frame_ptr -= 2;
		convert_16_bit_to_byte_array(bc_addr, frame_ptr);
	}

	/* Destination PAN-Id */
	frame_ptr -= 2;
	convert_16_bit_to_byte_array(bc_addr, frame_ptr);

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
	coord_realignment_frame->mpdu = frame_ptr;

	/* The frame is given to the TAL for transmission */
#ifdef BEACON_SUPPORT
	if (NON_BEACON_NWK == tal_pib.BeaconOrder) {
		/* In Nonbeacon network the frame is sent with unslotted
		 * CSMA-CA. */
		tal_tx_status = tal_tx_frame(coord_realignment_frame,
				CSMA_UNSLOTTED, true);
	} else {
		/* Beacon-enabled network */
		if (ORPHANREALIGNMENT == cmd_type) {
			/* In Beacon network the Orphan Realignment frame is
			 * sent with slotted CSMA-CA. */
			tal_tx_status = tal_tx_frame(coord_realignment_frame,
					CSMA_SLOTTED, true);
		} else {
#ifdef ENABLE_QUEUE_CAPACITY

			/*
			 * Coordinator Realignment frame is sent to broadcast
			 * address,
			 * so it needs to be appended at the end of the
			 * broadcast queue.
			 */
			if (QUEUE_FULL ==
					qmm_queue_append(&broadcast_q,
					buf_ptr)) {
				return false;
			}

#else
			qmm_queue_append(&broadcast_q, buf_ptr);
#endif  /* ENABLE_QUEUE_CAPACITY */

			return true;
		}
	}

#else

	/* In build without beacon support the frame is sent with unslotted
	 * CSMA-CA. */
	tal_tx_status = tal_tx_frame(coord_realignment_frame, CSMA_UNSLOTTED,
			true);
#endif  /* BEACON_SUPPORT */

	if (MAC_SUCCESS == tal_tx_status) {
		/*
		 * A positive confirmation is given since the TAL has
		 * successfuly accepted
		 * the frame for transmission.
		 */
		MAKE_MAC_BUSY();
		return true;
	} else {
		/*
		 * TAL was unable to transmit the frame, hence a negative
		 * confirmation
		 * is returned.
		 */
		return false;
	}
} /* mac_tx_coord_realignment_command() */

#endif /* ((MAC_ORPHAN_INDICATION_RESPONSE == 1) || (MAC_START_REQUEST_CONFIRM
        *== 1)) */

/* EOF */
