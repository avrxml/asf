/**
 * @file mac_data_req.c
 *
 * @brief Implements data request related functions
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
#include "mac_build_config.h"

#ifdef MAC_SECURITY_ZIP
#include "mac_security.h"
#endif  /* MAC_SECURITY_ZIP */

#if (MAC_INDIRECT_DATA_BASIC == 1)

/* === Macros =============================================================== */

/*
 * Data request payload length
 */
#define DATA_REQ_PAYLOAD_LEN            (1)

/* === Globals ============================================================= */

/* === Prototypes ========================================================== */

#if (MAC_INDIRECT_DATA_FFD == 1)
static buffer_t *build_null_data_frame(void);
static uint8_t find_long_buffer(void *buf, void *long_addr);
static uint8_t find_short_buffer(void *buf, void *short_addr);

#endif  /*  (MAC_INDIRECT_DATA_FFD == 1)*/

/* === Implementation ====================================================== */

/**
 * @brief Build and transmits data request command frame
 *
 * This function builds and transmits a data request command frame.
 *
 *
 * @param expl_poll Data request due to explicit MLME poll request
 * @param force_own_long_addr Forces the usage of the Extended Address as
 *                            Source Address. This a allows for implicitly
 *                            poll for pending data at the coordinator if
 *                            the Extended Address was used in the Beacon frame.
 * @param expl_dest_addr_mode Mode of subsequent destination address to be used
 *                            explicitly (0/2/3).
 *                            0: No explicit destination address attached,
 *                               use either macCoordShortAddress or
 *                               macCoordExtendedAddress
 *                            2: Use explicitly attached address in parameter
 *                               expl_dest_addr as destination address as
 *                               short address
 *                            3: Use explicitly attached address in parameter
 *                               expl_dest_addr as destination address as
 *                               extended address
 * @param expl_dest_addr Explicitly attached destination address for data
 *                       request frame. This is to be treated as either not
 *                       present, short or extended address, depending on
 *                       parameter expl_dest_addr_mode.
 * @param expl_dest_pan_id Explicitly attached destination PAN-Id (Coordinator
 *                         PAN-Id) for data request frame.
 *                         This is to be treated only as present, depending on
 *                         parameter expl_dest_addr_mode.
 *
 * @return True if data request command frame was created and sent to
 *         the TAL successfully, false otherwise.
 */
bool mac_build_and_tx_data_req(bool expl_poll,
		bool force_own_long_addr,
		uint8_t expl_dest_addr_mode,
		address_field_t *expl_dest_addr,
		uint16_t expl_dest_pan_id)
{
	retval_t tal_tx_status;
	bool intrabit = false;
	uint8_t frame_len;
	uint8_t *frame_ptr;
	uint16_t fcf;
	buffer_t *buf_ptr = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	if (NULL == buf_ptr) {
		return false;
	}

	frame_info_t *transmit_frame = (frame_info_t *)BMM_BUFFER_POINTER(
			buf_ptr);

	/*
	 * If this data request cmd frame was initiated by a device due to
	 * implicit
	 * poll, set msgtype to DATAREQUEST_IMPL_POLL.
	 * If this data request cmd frame was initiated by a MLME poll request,
	 * set msgtype to DATAREQUEST.
	 */
	if (expl_poll) {
		transmit_frame->msg_type = DATAREQUEST;
	} else {
		transmit_frame->msg_type = DATAREQUEST_IMPL_POLL;
	}

	/*
	 * The buffer header is stored as a part of frame_info_t structure
	 * before the
	 * frame is given to the TAL. After the transmission of the frame, reuse
	 * the buffer using this pointer.
	 */
	transmit_frame->buffer_header = buf_ptr;

	/* Update the payload length. */
	frame_len = DATA_REQ_PAYLOAD_LEN +
			2 + /* Add 2 octets for FCS */
			2 + /* 2 octets for short Source Address */
			2 + /* 2 octets for short Destination Address */
			2 + /* 2 octets for Destination PAN-Id */
			3; /* 3 octets DSN and FCF */

	/* Get the payload pointer. */
	frame_ptr = (uint8_t *)transmit_frame +
			LARGE_BUFFER_SIZE -
			DATA_REQ_PAYLOAD_LEN - 2; /* Add 2 octets for FCS. */

	/*
	 * Build the command frame id.
	 * This is actually being written into "transmit_frame->layload[0]".
	 */
	*frame_ptr = DATAREQUEST;

	/* Source Address */

	/*
	 * Long address needs to be used if a short address is not present
	 * or if we are forced to use the long address.
	 *
	 * This is used for example in cases where the coordinator indicates
	 * pending data for us using our extended address.
	 *
	 * This is also used for transmitting a data request frame
	 * during association, since here we always need to use our
	 * extended address.
	 */
	if ((BROADCAST == tal_pib.ShortAddress) ||
			(CCPU_ENDIAN_TO_LE16(MAC_NO_SHORT_ADDR_VALUE) ==
			tal_pib.ShortAddress) ||
			force_own_long_addr) {
		frame_ptr -= 8;
		frame_len += 6; /* Add further 6 octets for long Source Address
		                **/

		/* Build the Source address. */
		convert_64_bit_to_byte_array(tal_pib.IeeeAddress, frame_ptr);
		fcf = FCF_SET_FRAMETYPE(FCF_FRAMETYPE_MAC_CMD) |
				FCF_SET_SOURCE_ADDR_MODE(FCF_LONG_ADDR) |
				FCF_ACK_REQUEST;
	} else {
		frame_ptr -= 2;

		/* Build the Source address. */
		convert_16_bit_to_byte_array(tal_pib.ShortAddress, frame_ptr);

		fcf = FCF_SET_FRAMETYPE(FCF_FRAMETYPE_MAC_CMD) |
				FCF_SET_SOURCE_ADDR_MODE(FCF_SHORT_ADDR) |
				FCF_ACK_REQUEST;
	}

	/* Source PAN-Id */

	/*
	 * In IEEE 802.15.4 the PAN ID Compression bit may always be set.
	 * See page 154:
	 * If the data request command is being sent in response to the receipt
	 * of a beacon frame indicating that data are pending for that device,
	 * the Destination Addressing Mode subfield of the Frame Control field
	 * may be set to zero ..."
	 * In order to keep the implementation simple the address info is also
	 * in
	 * this case 2 or 3, i.e. the destination address info is present.
	 * This in return means that the PAN ID Compression bit is always set
	 * for
	 * data request frames, except the expl_dest_pan_id parameter is
	 * different from
	 * our own PAN-Id PIB attribute.
	 */
	if ((expl_dest_addr_mode != FCF_NO_ADDR) &&
			(expl_dest_pan_id != tal_pib.PANId)
			) {
		frame_ptr -= 2;
		frame_len += 2; /* Add further 6 octets for long Source Pan-Id
		                **/

		convert_16_bit_to_byte_array(tal_pib.PANId, frame_ptr);
	} else {
		/*
		 * The source PAN Id is not present since the PAN ID
		 * Compression bit is set.
		 */
		/* Set intra-PAN bit. */
		intrabit = true;
		fcf |= FCF_PAN_ID_COMPRESSION;
	}

	/* Destination Address */
	if (FCF_SHORT_ADDR == expl_dest_addr_mode) {
		/* An explicit short destination address is requested. */
		fcf |= FCF_SET_DEST_ADDR_MODE(FCF_SHORT_ADDR);

		frame_ptr -= 2;
		convert_16_bit_to_byte_array(expl_dest_addr->short_address,
				frame_ptr);
	} else if (FCF_LONG_ADDR == expl_dest_addr_mode) {
		/* An explicit long destination address is requested. */
		fcf |= FCF_SET_DEST_ADDR_MODE(FCF_LONG_ADDR);

		frame_ptr -= 8;
		frame_len += 6; /* Add further 6 octets for long Destination
		                 * Address */
		convert_64_bit_to_byte_array(expl_dest_addr->long_address,
				frame_ptr);
	} else {
		/* No explicit destination address is requested. */
		if (CCPU_ENDIAN_TO_LE16(MAC_NO_SHORT_ADDR_VALUE) !=
				mac_pib.mac_CoordShortAddress) {
			/*
			 * If current value of short address for coordinator PIB
			 * is
			 * NOT 0xFFFE, the current value of the short address
			 * for
			 * coordinator shall be used as desination address.
			 */
			fcf |= FCF_SET_DEST_ADDR_MODE(FCF_SHORT_ADDR);

			frame_ptr -= 2;
			convert_16_bit_to_byte_array(
					mac_pib.mac_CoordShortAddress,
					frame_ptr);
		} else {
			/*
			 * If current value of short address for coordinator PIB
			 * is 0xFFFE,
			 * the current value of the extended address for
			 * coordinator
			 * shall be used as desination address.
			 */
			fcf |= FCF_SET_DEST_ADDR_MODE(FCF_LONG_ADDR);

			frame_ptr -= 8;
			frame_len += 6; /* Add further 6 octets for long
			                 * Destination Address */
			convert_64_bit_to_byte_array(
					mac_pib.mac_CoordExtendedAddress,
					frame_ptr);
		}
	}

	/* Destination PAN-Id */
	frame_ptr -= 2;

	if (intrabit) {
		/* Add our PAN-Id. */
		convert_16_bit_to_byte_array(tal_pib.PANId, frame_ptr);
	} else {
		/*
		 * There is an expclicit destination address present AND
		 * the destination PAN-Id is different from our own PAN-ID,
		 * so include the source PAN-id into the frame.
		 */
		convert_16_bit_to_byte_array(expl_dest_pan_id, frame_ptr);
	}

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
	transmit_frame->mpdu = frame_ptr;

	/* Transmission should be done with CSMA-CA and frame retries. */
#ifdef BEACON_SUPPORT

	/*
	 * Now it gets tricky:
	 * In Beacon network the frame is sent with slotted CSMA-CA only if:
	 * 1) the node is associated, or
	 * 2) the node is idle, but synced before association,
	 * 3) the node is a Coordinator (we assume, that coordinators are always
	 *    in sync with their parents).
	 *
	 * In all other cases, the frame has to be sent using unslotted CSMA-CA.
	 */
	csma_mode_t cur_csma_mode;

	if (NON_BEACON_NWK != tal_pib.BeaconOrder) {
		if (
			((MAC_IDLE == mac_state) &&
			(MAC_SYNC_BEFORE_ASSOC == mac_sync_state)) ||
#if (MAC_START_REQUEST_CONFIRM == 1)
			(MAC_ASSOCIATED == mac_state) ||
			(MAC_COORDINATOR == mac_state)
#else
			(MAC_ASSOCIATED == mac_state)
#endif /* MAC_START_REQUEST_CONFIRM */
			) {
			cur_csma_mode = CSMA_SLOTTED;
		} else {
			cur_csma_mode = CSMA_UNSLOTTED;
		}
	} else {
		/* In Nonbeacon network the frame is sent with unslotted
		 * CSMA-CA. */
		cur_csma_mode = CSMA_UNSLOTTED;
	}

	tal_tx_status = tal_tx_frame(transmit_frame, cur_csma_mode, true);
#else   /* No BEACON_SUPPORT */
	/* In Nonbeacon build the frame is sent with unslotted CSMA-CA. */
	tal_tx_status = tal_tx_frame(transmit_frame, CSMA_UNSLOTTED, true);
#endif  /* BEACON_SUPPORT */

	if (MAC_SUCCESS == tal_tx_status) {
		MAKE_MAC_BUSY();
		return true;
	} else {
		/* TAL is busy, hence the data request could not be transmitted
		**/
		bmm_buffer_free(buf_ptr);

		return false;
	}
} /* mac_build_and_tx_data_req() */

#if (MAC_INDIRECT_DATA_FFD == 1)

/*
 * @brief Constructs a null data frame
 *
 * @return Pointer to the created null data frame, NULL otherwise.
 */
static buffer_t *build_null_data_frame(void)
{
	bool use_long_addr_dest;
	frame_info_t *transmit_frame;
	uint8_t frame_len;
	uint8_t *frame_ptr;
	uint16_t fcf = 0;
	buffer_t *buf_ptr = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	if (NULL == buf_ptr) {
		return NULL;
	}

	transmit_frame = (frame_info_t *)BMM_BUFFER_POINTER(buf_ptr);

	/* No data payload, this is a null packet.*/
	transmit_frame->msg_type = NULL_FRAME;
	transmit_frame->buffer_header = buf_ptr;

	/* No indirect transmission. */
	transmit_frame->indirect_in_transit = false;

	/* Update the payload length. */
	frame_len = 2 + /* Add 2 octets for FCS */
			2 + /* 2 octets for short Destination Address */
			2 + /* 2 octets for Destination PAN-Id */
			2 + /* 2 octets for short Source Address */
			3; /* 3 octets DSN and FCF */

	/* Get the payload pointer. */
	frame_ptr = (uint8_t *)transmit_frame + LARGE_BUFFER_SIZE - 2; /* Add 2
	                                                                * octets
	                                                                * for
	                                                                * FCS.
	                                                                **/

	/*
	 * Set Source Address.
	 */
	if ((BROADCAST == tal_pib.ShortAddress) ||
			(MAC_NO_SHORT_ADDR_VALUE == tal_pib.ShortAddress)
			) {
		/* Use long address as source address. */
		frame_ptr -= 8;
		frame_len += 6;
		convert_64_bit_to_byte_array(tal_pib.IeeeAddress, frame_ptr);
		fcf |= FCF_SET_SOURCE_ADDR_MODE(FCF_LONG_ADDR);
	} else {
		/* Use short address as source address. */
		frame_ptr -= 2;
		convert_16_bit_to_byte_array(tal_pib.ShortAddress, frame_ptr);
		fcf |= FCF_SET_SOURCE_ADDR_MODE(FCF_SHORT_ADDR);
	}

	/* Shall the Intra-PAN bit set? */
	if (tal_pib.PANId == mac_parse_data.src_panid) {
		/*
		 * Both PAN-Ids are identical.
		 * Set intra-PAN bit.
		 */
		fcf |= FCF_PAN_ID_COMPRESSION;
	} else {
		/* Set Source PAN-Id. */
		frame_ptr -= 2;
		frame_len += 2;
		convert_16_bit_to_byte_array(tal_pib.PANId, frame_ptr);
	}

	/* Set Destination Address. */
	use_long_addr_dest = (FCF_LONG_ADDR == mac_parse_data.src_addr_mode);

	/* Destination address is set from source address of received frame. */
	if (use_long_addr_dest) {
		frame_ptr -= 8;
		frame_len += 6;
		convert_64_bit_to_byte_array(
				mac_parse_data.src_addr.long_address,
				frame_ptr);

		fcf |= FCF_SET_DEST_ADDR_MODE(FCF_LONG_ADDR) |
				FCF_SET_FRAMETYPE(FCF_FRAMETYPE_DATA);
	} else {
		frame_ptr -= 2;
		convert_16_bit_to_byte_array(
				mac_parse_data.src_addr.short_address,
				frame_ptr);
		fcf |= FCF_SET_DEST_ADDR_MODE(FCF_SHORT_ADDR) |
				FCF_SET_FRAMETYPE(FCF_FRAMETYPE_DATA);
	}

	/* Destination PANId is set from source PANId of received frame. */
	frame_ptr -= 2;
	convert_16_bit_to_byte_array(mac_parse_data.src_panid, frame_ptr);

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
	transmit_frame->mpdu = frame_ptr;

	return buf_ptr;
} /* build_null_data_frame() */

/**
 * @brief Processes a received data request command frame
 *
 * This function processes a received data request command frame
 * at the coordinator, searches for pending indirect data frames
 * for the originator and initiates the frame transmission of the
 * data frame with CSMA-CA.
 *
 * @param msg Frame reception buffer pointer
 */
void mac_process_data_request(buffer_t *msg)
{
	/* Buffer pointer to next indirect data frame to be transmitted. */
	buffer_t *buf_ptr_next_data;
	search_t find_buf;
	frame_info_t *transmit_frame;
	retval_t tal_tx_status;

	/* Free the buffer of the received frame. */
	bmm_buffer_free(msg);

	/* Ignore data request if we are not PAN coordinator or coordinator. */
	if ((MAC_IDLE == mac_state) ||
			(MAC_ASSOCIATED == mac_state)
			) {
#if (_DEBUG_ > 0)
		Assert("Neither PAN coordinator nor coordinator" == 0);
#endif
		return;
	}

	/* Check the addressing mode */
	if (mac_parse_data.src_addr_mode == FCF_SHORT_ADDR) {
		/*
		 * Look for pending data in the indirect queue for
		 * this short address.
		 */

		/*
		 * Assign the function pointer for searching the
		 * data having address of the requested device.
		 */
		find_buf.criteria_func = find_short_buffer;

		/* Update the short address to be searched. */
		find_buf.handle = &mac_parse_data.src_addr.short_address;
	} else if (mac_parse_data.src_addr_mode == FCF_LONG_ADDR) {
		/*
		 * Look for pending data in the indirect queue for
		 * this long address.
		 */

		/*
		 * Assign the function pointer for searching the
		 * data having address of the requested device.
		 */
		find_buf.criteria_func = find_long_buffer;

		/* Update the long address to be searched. */
		find_buf.handle = &mac_parse_data.src_addr.long_address;
	} else {
#if (_DEBUG_ > 0)
		Assert("Unexpected addressing mode" == 0);
#endif
		return;
	}

	/*
	 * Read from the indirect queue. The removal of items from this queue
	 * will be done after successful transmission of the frame.
	 */
	buf_ptr_next_data = qmm_queue_read(&indirect_data_q, &find_buf);

	/* Note: The find_buf structure is reused below, so do not change this.
	**/

	if (NULL == buf_ptr_next_data) {
		mac_handle_tx_null_data_frame();
		return;
	} else {
		/* Indirect data found and to be sent. */
		transmit_frame = (frame_info_t *)BMM_BUFFER_POINTER(
				buf_ptr_next_data);

		/*
		 * We need to check whether the source PAN-Id of the previously
		 * received data request frame is identical to the destination
		 * PAN-Id
		 * of the pending frame. If not the frame shall not be
		 * transmitted,
		 * but a Null Data frame instead.
		 */
		if (mac_parse_data.src_panid !=
				convert_byte_array_to_16_bit(&transmit_frame->
				mpdu[PL_POS_DST_PAN_ID_START])
				) {
			mac_handle_tx_null_data_frame();
			return;
		} else {
			/*
			 * The frame to be transmitted next is marked.
			 * This is necessary since the queue needs to be
			 * traversed again
			 * to find other pending indirect data frames for this
			 * particular
			 * recipient.
			 */
			transmit_frame->indirect_in_transit = true;
			transmit_frame->buffer_header = buf_ptr_next_data;

			/*
			 * Go through the indirect data queue to find out the
			 * frame pending for
			 * the device which has requested for the data.
			 */

			/*
			 * Since the buffer header has already been stored in
			 * transmit_frame->buffer_header, it can be reused here
			 * for
			 * other purpose.
			 */

			/*
			 * It is assumed that the find_buf struct does still
			 * have
			 * the original values from above.
			 */
			buf_ptr_next_data = qmm_queue_read(&indirect_data_q,
					&find_buf);

			/*
			 * Check whether there is another indirect data
			 * available
			 * for the same recipient.
			 */
			if (NULL != buf_ptr_next_data) {
				transmit_frame->mpdu[PL_POS_FCF_1]
					|= FCF_FRAME_PENDING;
			}
		}

#ifdef MAC_SECURITY_ZIP
		if (transmit_frame->mpdu[1] & FCF_SECURITY_ENABLED) {
			mcps_data_req_t pmdr;

			build_sec_mcps_data_frame(&pmdr, transmit_frame);

			if (pmdr.SecurityLevel > 0) {
				/* Secure the Frame */
				retval_t build_sec = mac_secure(transmit_frame,	\
						transmit_frame->mac_payload,
						&pmdr);

				if (MAC_SUCCESS != build_sec) {
					/* The MAC Data Payload is encrypted
					 * based on the security level. */
					transmit_frame->indirect_in_transit
						= false;
					return;
				}
			}
		}
#endif

		/*
		 * Transmission should be done with CSMA-CA or
		 * quickly after the ACK of the data request command.
		 * Here it's done quickly after the ACK w/o CSMA.
		 */
		tal_tx_status = tal_tx_frame(transmit_frame, NO_CSMA_WITH_IFS,
				false);

		if (MAC_SUCCESS == tal_tx_status) {
			MAKE_MAC_BUSY();
		} else {
			/*
			 * TAL rejects frame transmission, since it's too close
			 * to the next
			 * beacon transmission. The frame is kept in the
			 * indirect queue.
			 */
			transmit_frame->indirect_in_transit = false;
		}
	}
} /* mac_process_data_request() */

/*
 * @brief Creates and transmits a Null Data frame
 *
 * This function creates and transmits a Null Data frame in case the
 * coordinator does not have pending data to be transmitted.
 */
void mac_handle_tx_null_data_frame(void)
{
	frame_info_t *tx_frame;
	retval_t tal_tx_status;
	buffer_t *buf_ptr;

	/*
	 * No matching pending item in the queue,
	 * so a Null Data frame is created.
	 */
	buf_ptr = build_null_data_frame();

	if (NULL != buf_ptr) {
		tx_frame = (frame_info_t *)BMM_BUFFER_POINTER(buf_ptr);

		/*
		 * Transmission should be done with CSMA-CA or
		 * quickly after the ACK of the data request command.
		 * Here it's done quickly after the ACK w/o CSMA.
		 */
		tal_tx_status = tal_tx_frame(tx_frame, NO_CSMA_WITH_IFS, false);

		if (MAC_SUCCESS == tal_tx_status) {
			MAKE_MAC_BUSY();
		} else {
			/*
			 * Transmission to TAL failed, free up the buffer used
			 * to create
			 * Null Data frame.
			 */
			bmm_buffer_free(buf_ptr);
		}
	}
}

/*
 * @brief Checks for matching short address
 *
 * This callback function checks whether the passed short address
 * matches with the frame in the queue.
 *
 * @param buf Pointer to indirect data buffer
 * @param short_addr Short address to be searched
 *
 * @return 1 if short address passed matches with the destination
 * address of the indirect frame , 0 otherwise
 */
static uint8_t find_short_buffer(void *buf, void *short_addr)
{
	frame_info_t *data = (frame_info_t *)buf;
	uint16_t short_addr_temp = 0;
	memcpy((uint8_t *)&short_addr_temp, (uint8_t *)short_addr, \
			sizeof(short_addr_temp));

	if (!data->indirect_in_transit) {
		/*
		 * Read octet 2 of Frame Control Field containing the type
		 * of destination address.
		 */
		uint8_t dst_addr_mode
			= (data->mpdu[PL_POS_FCF_2] >>
				FCF_2_DEST_ADDR_OFFSET) & FCF_ADDR_MASK;

		/*
		 * Compare indirect data frame's dest_address(short)
		 * with short address passed.
		 */
		if ((dst_addr_mode == FCF_SHORT_ADDR) &&
				(short_addr_temp ==
				convert_byte_array_to_16_bit(&data->mpdu[
					PL_POS_DST_ADDR_START]))
				) {
			return 1;
		}
	}

	return 0;
}

/*
 * @brief Checks for matching extended address
 *
 * This callback function checks whether the passed short address
 * matches with the frame in the queue.
 *
 * @param buf Pointer to indirect data buffer
 * @param long_addr Extended address to be searched
 *
 * @return 1 if extended address passed matches with the destination
 * address of the indirect frame , 0 otherwise
 */
static uint8_t find_long_buffer(void *buf, void *long_addr)
{
	frame_info_t *data = (frame_info_t *)buf;
	uint64_t long_addr_temp = 0;
	memcpy((uint8_t *)&long_addr_temp, (uint8_t *)long_addr, \
			sizeof(long_addr_temp));

	if (!data->indirect_in_transit) {
		/*
		 * Read octet 2 of Frame Control Field containing the type
		 * of destination address.
		 */
		uint8_t dst_addr_mode
			= (data->mpdu[PL_POS_FCF_2] >>
				FCF_2_DEST_ADDR_OFFSET) & FCF_ADDR_MASK;

		/*
		 * Compare indirect data frame's dest_address(extended)
		 * with the exended address passed.
		 */
		if ((dst_addr_mode == FCF_LONG_ADDR) &&
				(long_addr_temp ==
				convert_byte_array_to_64_bit(&data->mpdu[
					PL_POS_DST_ADDR_START]))
				) {
			return 1;
		}
	}

	return 0;
}

#endif  /* (MAC_INDIRECT_DATA_FFD == 1) */
#endif  /* (MAC_INDIRECT_DATA_BASIC == 1) */

/* EOF */
