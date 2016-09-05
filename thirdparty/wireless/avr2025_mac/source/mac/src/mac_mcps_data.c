/**
 * @file mac_mcps_data.c
 *
 * @brief Handles MCPS related primitives and frames
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
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
#include "mac_security.h"
#endif

/* === Macros =============================================================== */
/* Length of the MAC Aux Header Frame Counter */
#define FRAME_COUNTER_LEN               (0x04)

#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
/* Security Control Field: Security Level mask */
#define SEC_CTRL_SEC_LVL_MASK           (0x07)

/* Security Control Field: Key Identifier mask */
#define SEC_CTRL_KEY_ID_MASK            (0x03)

/* Security Control Field: Key Identifier Field position */
#define SEC_CTRL_KEY_ID_FIELD_POS       (3)

#define KEY_ID_MODE_0                   (0x00)
#define KEY_ID_MODE_1                   (0x01)
#define KEY_ID_MODE_2                   (0x02)
#define KEY_ID_MODE_3                   (0x03)

#endif
/* === Globals ============================================================= */

/* === Prototypes ========================================================== */

static retval_t build_data_frame(mcps_data_req_t *pmdr,
		frame_info_t *frame);

#if (MAC_INDIRECT_DATA_FFD == 1)
static void handle_persistence_time_decrement(void);
static void handle_exp_persistence_timer(buffer_t *buf_ptr);
static void mac_t_persistence_cb(void *callback_parameter);
static uint8_t decrement_persistence_time(void *buf_ptr, void *handle);
static uint8_t check_persistence_time_zero(void *buf_ptr, void *handle);

#endif /* (MAC_INDIRECT_DATA_FFD == 1) */

/* MAC-internal Buffer functions */
#if (MAC_PURGE_REQUEST_CONFIRM == 1)
static uint8_t check_msdu_handle_cb(void *buf, void *handle);
static bool mac_buffer_purge(uint8_t msdu_handle);

#endif /* (MAC_PURGE_REQUEST_CONFIRM == 1) */

/* === Implementation ====================================================== */

/*
 * @brief Initiates mcps data confirm message
 *
 * This function creates the mcps data confirm structure,
 * and appends it into internal event queue.
 *
 * @param buf Buffer for mcps data confirmation.
 * @param status Data transmission status.
 * @param handle MSDU handle.
 * @param timestamp Time in symbols at which the data were transmitted
 *        (only if timestampting is enabled).
 */
#ifdef ENABLE_TSTAMP
void mac_gen_mcps_data_conf(buffer_t *buf, uint8_t status, uint8_t handle,
		uint32_t timestamp)
#else
void mac_gen_mcps_data_conf(buffer_t *buf, uint8_t status, uint8_t handle)
#endif  /* ENABLE_TSTAMP */
{
	mcps_data_conf_t *mdc = (mcps_data_conf_t *)BMM_BUFFER_POINTER(buf);

	mdc->cmdcode = MCPS_DATA_CONFIRM;
	mdc->msduHandle = handle;
	mdc->status = status;
#ifdef ENABLE_TSTAMP
	mdc->Timestamp = timestamp;
#endif  /* ENABLE_TSTAMP */

	qmm_queue_append(&mac_nhle_q, buf);
}

/**
 * @brief Builds the data frame for transmission
 *
 * This function builds the data frame for transmission.
 * The NWK layer has supplied the parameters.
 * The frame_info_t data type is constructed and filled in.
 * Also the FCF is constructed based on the parameters passed.
 *
 * @param msg Pointer to the MCPS-DATA.request parameter
 */
void mcps_data_request(uint8_t *msg)
{
	retval_t status = FAILURE;
	mcps_data_req_t mdr;

	memcpy(&mdr, BMM_BUFFER_POINTER(
			(buffer_t *)msg), sizeof(mcps_data_req_t));

	if ((mdr.TxOptions & WPAN_TXOPT_INDIRECT) == 0
#ifdef GTS_SUPPORT
			|| (mdr.TxOptions & WPAN_TXOPT_GTS) == 0
#endif /* GTS_SUPPORT */
			) {
		/*
		 * Data Requests for a coordinator using direct transmission are
		 * accepted in all non-transient states (no polling and no
		 * scanning
		 * is ongoing).
		 */
		if ((MAC_POLL_IDLE != mac_poll_state) ||
				(MAC_SCAN_IDLE != mac_scan_state)
				) {
			mac_gen_mcps_data_conf((buffer_t *)msg,
					(uint8_t)MAC_CHANNEL_ACCESS_FAILURE,
#ifdef ENABLE_TSTAMP
					mdr.msduHandle,
					0);
#else
					mdr.msduHandle);
#endif  /* ENABLE_TSTAMP */
			return;
		}
	}

#ifndef REDUCED_PARAM_CHECK
	/*To check the broadcst address*/
	uint16_t broadcast_check;
	ADDR_COPY_DST_SRC_16(broadcast_check, mdr.DstAddr);
	/* Check whether somebody requests an ACK of broadcast frames */
	if ((mdr.TxOptions & WPAN_TXOPT_ACK) &&
			(FCF_SHORT_ADDR == mdr.DstAddrMode) &&
			(BROADCAST == broadcast_check)) {
		mac_gen_mcps_data_conf((buffer_t *)msg,
				(uint8_t)MAC_INVALID_PARAMETER,
#ifdef ENABLE_TSTAMP
				mdr.msduHandle,
				0);
#else
				mdr.msduHandle);
#endif  /* ENABLE_TSTAMP */
		return;
	}

#ifdef GTS_SUPPORT
	/* Check whether somebody requests an ACK of broadcast frames */
	if ((mdr.TxOptions & WPAN_TXOPT_GTS) &&
			((FCF_SHORT_ADDR != mdr.DstAddrMode) ||
			(FCF_SHORT_ADDR != mdr.SrcAddrMode) ||
			(MAC_ASSOCIATED == mac_state && mdr.DstAddr !=
			mac_pib.mac_CoordShortAddress))) {
		mac_gen_mcps_data_conf((buffer_t *)msg,
				(uint8_t)MAC_INVALID_PARAMETER,
#ifdef ENABLE_TSTAMP
				mdr.msduHandle,
				0);
#else
				mdr.msduHandle);
#endif  /* ENABLE_TSTAMP */
		return;
	}
#endif /* GTS_SUPPORT */

	/* Check whether both Src and Dst Address are not present */
	if ((FCF_NO_ADDR == mdr.SrcAddrMode) &&
			(FCF_NO_ADDR == mdr.DstAddrMode)) {
		mac_gen_mcps_data_conf((buffer_t *)msg,
				(uint8_t)MAC_INVALID_ADDRESS,
#ifdef ENABLE_TSTAMP
				mdr.msduHandle,
				0);
#else
				mdr.msduHandle);
#endif  /* ENABLE_TSTAMP */
		return;
	}

	/* Check whether Src or Dst Address indicate reserved values */
	if ((FCF_RESERVED_ADDR == mdr.SrcAddrMode) ||
			(FCF_RESERVED_ADDR == mdr.DstAddrMode)) {
		mac_gen_mcps_data_conf((buffer_t *)msg,
				(uint8_t)MAC_INVALID_PARAMETER,
#ifdef ENABLE_TSTAMP
				mdr.msduHandle,
				0);
#else
				mdr.msduHandle);
#endif  /* ENABLE_TSTAMP */
		return;
	}
#endif  /* REDUCED_PARAM_CHECK */

	/* Now all is fine, continue... */
	frame_info_t *transmit_frame
		= (frame_info_t *)BMM_BUFFER_POINTER((buffer_t *)msg);

	/* Store the message type */
	transmit_frame->msg_type = MCPS_MESSAGE;
	transmit_frame->msduHandle = mdr.msduHandle;

#if (MAC_INDIRECT_DATA_FFD == 1)
	/* Indirect transmission not ongoing yet. */
	transmit_frame->indirect_in_transit = false;
#endif  /* (MAC_INDIRECT_DATA_FFD == 1) */

#ifdef GTS_SUPPORT
	transmit_frame->gts_queue = NULL;
#endif /* GTS_SUPPORT */

	status = build_data_frame(&mdr, transmit_frame);

	if (MAC_SUCCESS != status) {
		/* The frame is too long. */
		mac_gen_mcps_data_conf((buffer_t *)msg,
				(uint8_t)status,
#ifdef ENABLE_TSTAMP
				mdr.msduHandle,
				0);
#else
				mdr.msduHandle);
#endif  /* ENABLE_TSTAMP */
		return;
	}

	/*
	 * Broadcast transmission in a beacon-enabled network intiated by a
	 * PAN Coordinator or Coordinator is put into the broadcast queue..
	 */
#if (MAC_START_REQUEST_CONFIRM == 1)
#ifdef BEACON_SUPPORT
	/*To check the broadcast address*/
	uint16_t broadcast;
	ADDR_COPY_DST_SRC_16(broadcast, mdr.DstAddr);
	if (
		((MAC_PAN_COORD_STARTED == mac_state) ||
		(MAC_COORDINATOR == mac_state)) &&
		(tal_pib.BeaconOrder < NON_BEACON_NWK) &&
		(FCF_SHORT_ADDR == mdr.DstAddrMode) &&
		(BROADCAST == broadcast)
		) {
		/* Append the MCPS data request into the broadcast queue */
#ifdef ENABLE_QUEUE_CAPACITY
		if (QUEUE_FULL ==
				qmm_queue_append(&broadcast_q,
				(buffer_t *)msg)) {
			mac_gen_mcps_data_conf((buffer_t *)msg,
					(uint8_t)MAC_CHANNEL_ACCESS_FAILURE,
#ifdef ENABLE_TSTAMP
					mdr.msduHandle,
					0);
#else
					mdr.msduHandle);
#endif  /* ENABLE_TSTAMP */
			return;
		}
#endif   /* ENABLE_QUEUE_CAPACITY */
		qmm_queue_append(&broadcast_q, (buffer_t *)msg);
		return;
	}
#endif  /* BEACON_SUPPORT */
#endif /* (MAC_START_REQUEST_CONFIRM == 1) */

	/*
	 * Indirect transmission is only allowed if we are
	 * a PAN coordinator or coordinator.
	 */
#if (MAC_INDIRECT_DATA_FFD == 1)
	if (
		(mdr.TxOptions & WPAN_TXOPT_INDIRECT) &&
		((MAC_PAN_COORD_STARTED == mac_state) ||
		(MAC_COORDINATOR == mac_state))
		) {
		/* Append the MCPS data request into the indirect data queue */
#ifdef ENABLE_QUEUE_CAPACITY
		if (QUEUE_FULL ==
				qmm_queue_append(&indirect_data_q,
				(buffer_t *)msg)) {
			mac_gen_mcps_data_conf((buffer_t *)msg,
					(uint8_t)MAC_TRANSACTION_OVERFLOW,
#ifdef ENABLE_TSTAMP
					mdr.msduHandle,
					0);
#else
					mdr.msduHandle);
#endif  /* ENABLE_TSTAMP */
			return;
		}

#else
		qmm_queue_append(&indirect_data_q, (buffer_t *)msg);
#endif  /* ENABLE_QUEUE_CAPACITY */

		/*
		 * If an FFD does have pending data,
		 * the MAC persistence timer needs to be started.
		 */
		transmit_frame->persistence_time
			= mac_pib.mac_TransactionPersistenceTime;
		mac_check_persistence_timer();
	} else
#endif /* (MAC_INDIRECT_DATA_FFD == 1) */

#ifdef GTS_SUPPORT
	if (mdr.TxOptions & WPAN_TXOPT_GTS) {
		handle_gts_data_req(&mdr, msg);
	} else
#endif /* GTS_SUPPORT */

	/*
	 * We are NOT indirect, so we need to transmit using
	 * CSMA_CA in the CAP (for beacon enabled) or immediately (for
	 * a non-beacon enabled).
	 */
	{
		mac_trx_wakeup();

		transmit_frame->buffer_header = (buffer_t *)msg;

		/* Transmission should be done with CSMA-CA and with frame
		 * retries. */

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
					mac_gen_mcps_data_conf((buffer_t *)msg,
							(uint8_t)build_sec,
			#ifdef ENABLE_TSTAMP
							mdr.msduHandle,
							0);
			#else
							mdr.msduHandle);
			#endif  /* ENABLE_TSTAMP */

					return;
				}
			}
		}
#endif

#ifdef BEACON_SUPPORT
		csma_mode_t cur_csma_mode;

		if (NON_BEACON_NWK == tal_pib.BeaconOrder) {
			/* In Nonbeacon network the frame is sent with unslotted
			 * CSMA-CA. */
			cur_csma_mode = CSMA_UNSLOTTED;
		} else {
			/* In Beacon network the frame is sent with slotted
			 * CSMA-CA. */
			cur_csma_mode = CSMA_SLOTTED;
		}

		status = tal_tx_frame(transmit_frame, cur_csma_mode, true);

#else   /* No BEACON_SUPPORT */

		/* In Non beacon build the frame is sent with unslotted CSMA-CA.
		**/
		status = tal_tx_frame(transmit_frame, CSMA_UNSLOTTED, true);
#endif  /* BEACON_SUPPORT / No BEACON_SUPPORT */

		if (MAC_SUCCESS == status) {
			MAKE_MAC_BUSY();
		} else {
			/* Transmission to TAL failed, generate confirmation
			 * message. */
			mac_gen_mcps_data_conf((buffer_t *)msg,
					(uint8_t)MAC_CHANNEL_ACCESS_FAILURE,
#ifdef ENABLE_TSTAMP
					mdr.msduHandle,
					0);
#else
					mdr.msduHandle);
#endif  /* ENABLE_TSTAMP */

			/* Set radio to sleep if allowed */
			mac_sleep_trans();
		}
	}
} /* mcps_data_request() */

/**
 * @brief Processes data frames
 *
 * This function processes the data frames received and sends
 * mcps_data_indication to the NHLE.
 *
 * @param buf_ptr Pointer to receive buffer of the data frame
 */
void mac_process_data_frame(buffer_t *buf_ptr)
{
	mcps_data_ind_t *mdi
		= (mcps_data_ind_t *)BMM_BUFFER_POINTER(buf_ptr);

	if (mac_parse_data.mac_payload_length == 0) {
		/*
		 * A null frame is neither indicated to the higher layer
		 * nor checked for for frame pending bit set, since
		 * null data frames with frame pending bit set are nonsense.
		 */

		/* Since no indication is generated, the frame buffer is
		 * released. */
		bmm_buffer_free(buf_ptr);

		/* Set radio to sleep if allowed */
		mac_sleep_trans();
	} else {
		/* Build the MLME_Data_indication parameters. */
		mdi->DSN = mac_parse_data.sequence_number;
#ifdef ENABLE_TSTAMP
		mdi->Timestamp = mac_parse_data.time_stamp;
#endif /* ENABLE_TSTAMP */

		/* Source address info */
		mdi->SrcAddrMode = mac_parse_data.src_addr_mode;
		mdi->SrcPANId = mac_parse_data.src_panid;

		if (FCF_LONG_ADDR == mdi->SrcAddrMode ||
				FCF_SHORT_ADDR == mdi->SrcAddrMode) {
			mdi->SrcAddr = 0;
			if (FCF_SHORT_ADDR == mdi->SrcAddrMode) {
				ADDR_COPY_DST_SRC_16(mdi->SrcAddr,
						mac_parse_data.src_addr.short_address);
			} else if (FCF_LONG_ADDR == mdi->SrcAddrMode) {
				ADDR_COPY_DST_SRC_64(mdi->SrcAddr,
						mac_parse_data.src_addr.long_address);
			}
		} else {
			/*
			 * Even if the Source address mode is zero, and the
			 * source address
			 * informationis is not present, the values are cleared
			 * to prevent
			 * the providing of trash information.
			 */
			mdi->SrcPANId = 0;
			mdi->SrcAddr = 0;
		}

		/* Start of duplicate detection. */
		if ((mdi->DSN == mac_last_dsn) &&
				(mdi->SrcAddr == mac_last_src_addr)
				) {
			/*
			 * This is a duplicated frame.
			 * It will not be indicated to the next higher layer,
			 * but nevetheless the frame pending bit needs to be
			 * checked and acted upon.
			 */

			/* Since no indication is generated, the frame buffer is
			 * released. */
			bmm_buffer_free(buf_ptr);
		} else {
			/* Generate data indication to next higher layer. */

			/* Store required information for perform subsequent
			 * duplicate detections.
			 */
			mac_last_dsn = mdi->DSN;
			mac_last_src_addr = mdi->SrcAddr;

			/* Destination address info */
			mdi->DstAddrMode = mac_parse_data.dest_addr_mode;

			/*
			 * Setting the address to zero is required for a short
			 * address
			 * and in case no address is included. Therefore the
			 * address
			 * is first always set to zero to reduce code size.
			 */
			mdi->DstAddr = 0;

			/*
			 * Setting the PAN-Id to the Destiantion PAN-Id is
			 * required
			 * for a both short and long address, but not in case no
			 * address
			 * is included. Therefore the PAN-ID is first always set
			 * to
			 * the Destination PAN-IDto reduce code size.
			 */
			mdi->DstPANId = mac_parse_data.dest_panid;
			if (FCF_LONG_ADDR == mdi->DstAddrMode) {
				ADDR_COPY_DST_SRC_64(mdi->DstAddr,
						mac_parse_data.dest_addr.long_address);
			} else if (FCF_SHORT_ADDR == mdi->DstAddrMode) {
				ADDR_COPY_DST_SRC_16(mdi->DstAddr,
						mac_parse_data.dest_addr.short_address);
			} else {
				/*
				 * Even if the Destination address mode is zero,
				 * and the destination
				 * address information is not present, the
				 * values are cleared to
				 * prevent the providing of trash information.
				 * The Desintation address was already cleared
				 * above.
				 */
				mdi->DstPANId = 0;
			}

			mdi->mpduLinkQuality = mac_parse_data.ppdu_link_quality;

#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
			mdi->SecurityLevel = mac_parse_data.sec_ctrl.sec_level;
			mdi->KeyIdMode = mac_parse_data.sec_ctrl.key_id_mode;
			mdi->KeyIndex = mac_parse_data.key_id[0];
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006)  */

			mdi->msduLength = mac_parse_data.mac_payload_length;

			/* Set pointer to data frame payload. */
			mdi->msdu
				= mac_parse_data.mac_payload_data.data.payload;

			mdi->cmdcode = MCPS_DATA_INDICATION;

			/* Append MCPS data indication to MAC-NHLE queue */
			qmm_queue_append(&mac_nhle_q, buf_ptr);
#ifdef GTS_SUPPORT
#ifdef FFD
			{
				uint8_t loop_index;
				for (loop_index
							= 0;
						loop_index <
						mac_pan_gts_table_len;
						loop_index++) {
					if (FCF_SHORT_ADDR ==
							mdi->SrcAddrMode &&
							(uint16_t)(mdi->SrcAddr)
							== mac_pan_gts_table[
								loop_index].
							DevShortAddr
							&&
							GTS_TX_SLOT ==
							mac_pan_gts_table[
								loop_index].
							GtsDesc.
							GtsDirection) {
						#ifdef GTS_DEBUG
						port_pin_toggle_output_level(
								DEBUG_PIN11);             /*
						                                           *
						                                           *coord
						                                           *
						                                           *
						                                           *rx
						                                           **/
						#endif
						reset_gts_expiry(
								&mac_pan_gts_table[
									loop_index]);
						break;
					}
				}
			}
#endif /* FFD */
#endif /* GTS_SUPPORT */
		} /* End of duplicate detection. */

		/* Continue with checking the frame pending bit in the received
		 * data frame.
		 */
#if (MAC_INDIRECT_DATA_BASIC == 1)
		if (mac_parse_data.fcf & FCF_FRAME_PENDING) {
#if (MAC_START_REQUEST_CONFIRM == 1)

			/* An node that is not PAN coordinator may poll for
			 * pending data. */
			if (MAC_PAN_COORD_STARTED != mac_state)
#endif  /* (MAC_START_REQUEST_CONFIRM == 1) */
			{
				address_field_t src_addr;

				/* Build command frame due to implicit poll
				 * request */

				/*
				 * No explicit destination address attached, so
				 * use current
				 * values of PIB attributes macCoordShortAddress
				 * or
				 * macCoordExtendedAddress.
				 */

				/*
				 * This implicit poll (i.e. corresponding data
				 * request
				 * frame) is to be sent to the same node that we
				 * have received
				 * this data frame. Therefore the source address
				 * information
				 * from this data frame needs to be extracted,
				 * and used for the
				 * data request frame appropriately.
				 * Use this as destination address explicitly
				 * and
				 * feed this to the function
				 * mac_build_and_tx_data_req
				 */
				if (FCF_SHORT_ADDR ==
						mac_parse_data.src_addr_mode) {
					ADDR_COPY_DST_SRC_16(
							src_addr.short_address,
							mac_parse_data.src_addr.short_address);

					mac_build_and_tx_data_req(false,
							false,
							FCF_SHORT_ADDR,
							(address_field_t *)&(
								src_addr),
							mac_parse_data.src_panid);
				} else if (FCF_LONG_ADDR ==
						mac_parse_data.src_addr_mode) {
					ADDR_COPY_DST_SRC_64(
							src_addr.long_address,
							mac_parse_data.src_addr.long_address);

					mac_build_and_tx_data_req(false,
							false,
							FCF_LONG_ADDR,
							(address_field_t *)&(
								src_addr),
							mac_parse_data.src_panid);
				} else {
					mac_build_and_tx_data_req(false, false,
							0, NULL, 0);
				}
			}
		} else
#endif /* (MAC_INDIRECT_DATA_BASIC == 1) */
		{
			/* Frame pending but was not set, so no further action
			 * required. */
			/* Set radio to sleep if allowed */
			mac_sleep_trans();
		} /* if (mac_parse_data.fcf & FCF_FRAME_PENDING) */
	} /* (mac_parse_data.payload_length == 0) */
} /* mac_process_data_frame() */

/*
 * @brief Builds MCPS data frame
 *
 * This function builds the data frame.
 *
 * @param pmdr Request parameters
 * @param buffer Pointer to transmission frame
 * @param indirect Transmission is direct or indirect
 *
 * @return Status of the attempt to build the data frame
 */
static retval_t build_data_frame(mcps_data_req_t *pmdr,
		frame_info_t *frame)
{
	uint8_t frame_len;
	uint8_t *frame_ptr;
	uint16_t fcf = 0;

	frame_len = pmdr->msduLength +
			2 + /* Add 2 octets for FCS */
			3; /* 3 octets DSN and FCF */

	/*
	 * Payload pointer points to data, which was already been copied
	 * into buffer
	 */
	frame_ptr = (uint8_t *)frame +
			LARGE_BUFFER_SIZE -
			pmdr->msduLength - 2; /* Add 2 octets for FCS. */

#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
	uint8_t *mac_payload_ptr = frame_ptr;

	/*
	 * Note: The value of the payload_length parameter will be updated
	 *       if security needs to be applied.
	 */
	if (pmdr->SecurityLevel > 0) {
		retval_t build_sec = mac_build_aux_sec_header(&frame_ptr, pmdr,
				&frame_len);

		if (MAC_SUCCESS != build_sec) {
			return (build_sec);
		}
	}
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */

	/*
	 * Set Source Address.
	 */
	if (FCF_SHORT_ADDR == pmdr->SrcAddrMode) {
		frame_ptr -= 2;
		frame_len += 2;
		convert_16_bit_to_byte_array(tal_pib.ShortAddress, frame_ptr);
	} else if (FCF_LONG_ADDR == pmdr->SrcAddrMode) {
		frame_ptr -= 8;
		frame_len += 8;
		convert_64_bit_to_byte_array(tal_pib.IeeeAddress, frame_ptr);
	}

	/* Shall the Intra-PAN bit set? */
	if ((tal_pib.PANId == pmdr->DstPANId) &&
			(FCF_NO_ADDR != pmdr->SrcAddrMode) &&
			(FCF_NO_ADDR != pmdr->DstAddrMode)) {
		/*
		 * Both address are present and both PAN-Ids are identical.
		 * Set intra-PAN bit.
		 */
		fcf |= FCF_PAN_ID_COMPRESSION;
	} else if (FCF_NO_ADDR != pmdr->SrcAddrMode) {
		/* Set Source PAN-Id. */
		frame_ptr -= 2;
		frame_len += 2;
		convert_16_bit_to_byte_array(tal_pib.PANId, frame_ptr);
	}

	/* Set the Destination Addressing fields. */
	if (FCF_NO_ADDR != pmdr->DstAddrMode) {
		if (FCF_SHORT_ADDR == pmdr->DstAddrMode) {
			frame_ptr -= 2;
			frame_len += 2;
			convert_16_bit_to_byte_address(pmdr->DstAddr,
					frame_ptr);
		} else {
			frame_ptr -= 8;
			frame_len += 8;
			convert_64_bit_to_byte_array(pmdr->DstAddr, frame_ptr);
		}

		frame_ptr -= 2;
		frame_len += 2;
		convert_16_bit_to_byte_array(pmdr->DstPANId, frame_ptr);
	}

	/* Set DSN. */
	frame_ptr--;
	*frame_ptr = mac_pib.mac_DSN++;

	/* Set the FCF. */
#ifdef TEST_HARNESS

	/*
	 * When performing tests this PIB attribute defaults to 1
	 * (i.e. a standard data frame). If not set to 1, it is used as a
	 * (supposedly illegal) frame type to fill into the frame type
	 * field of the data frame's FCF. In effect, valid (illegal)
	 * values range from 4 through 7.
	 */
	if (mac_pib.privateIllegalFrameType != 1) {
		fcf |= FCF_SET_FRAMETYPE(mac_pib.privateIllegalFrameType);
	} else
#endif /* TEST_HARNESS */
	{
		fcf |= FCF_SET_FRAMETYPE(FCF_FRAMETYPE_DATA);
	}

#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
	if (pmdr->SecurityLevel > 0) {
		fcf |= FCF_SECURITY_ENABLED | FCF_FRAME_VERSION_2006;
	}
#endif

	if (pmdr->TxOptions & WPAN_TXOPT_ACK) {
		fcf |= FCF_ACK_REQUEST;
	}

	/*
	 * 802.15.4-2006 section 7.1.1.1.3:
	 *
	 * If the msduLength parameter is greater than aMaxMACSafePayloadSize,
	 * the MAC sublayer will set the Frame Version subfield of the
	 * Frame Control field to one.
	 */
	if (pmdr->msduLength > aMaxMACSafePayloadSize) {
		fcf |= FCF_FRAME_VERSION_2006;
	}

	/* Set FCFs address mode */
	fcf |= FCF_SET_SOURCE_ADDR_MODE(pmdr->SrcAddrMode);
	fcf |= FCF_SET_DEST_ADDR_MODE(pmdr->DstAddrMode);

	frame_ptr -= 2;
	convert_spec_16_bit_to_byte_array(fcf, frame_ptr);

	/*
	 * In case the frame gets too large, return error.
	 */
	if (frame_len > aMaxPHYPacketSize) {
		return MAC_FRAME_TOO_LONG;
	}

	/* First element shall be length of PHY frame. */
	frame_ptr--;
	*frame_ptr = frame_len;

	/* Finished building of frame. */
	frame->mpdu = frame_ptr;

#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
	frame->mac_payload = mac_payload_ptr;
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */

	return MAC_SUCCESS;
} /* build_data_frame() */

#if (MAC_INDIRECT_DATA_FFD == 1)

/*
 * @brief Start the Persistence timer for indirect data
 *
 * This function starts the persistence timer for handling of indirect
 * data.
 */
void mac_start_persistence_timer(void)
{
	retval_t status = FAILURE;
	/* Interval of indirect data persistence timer */
	uint32_t persistence_int_us;

#ifdef BEACON_SUPPORT

	/*
	 * This is a beacon build.
	 */
	uint8_t bo_for_persistence_tmr;

	if (tal_pib.BeaconOrder == NON_BEACON_NWK) {
		/*
		 * The timeout interval for the indirect data persistence timer
		 * is
		 * based on the define below and is the same as for a nonbeacon
		 * build.
		 */
		bo_for_persistence_tmr = BO_USED_FOR_MAC_PERS_TIME;
	} else {
		/*
		 * The timeout interval for the indirect data persistence timer
		 * is
		 * based on the current beacon order.
		 */
		bo_for_persistence_tmr = tal_pib.BeaconOrder;
	}

	persistence_int_us
		= TAL_CONVERT_SYMBOLS_TO_US(TAL_GET_BEACON_INTERVAL_TIME(
			bo_for_persistence_tmr));
#else   /* No BEACON_SUPPORT */

	/*
	 * This is a nonbeacon build. The timeout interval for the indirect data
	 * persistence timer is based on the define below.
	 */

	persistence_int_us
		= TAL_CONVERT_SYMBOLS_TO_US(TAL_GET_BEACON_INTERVAL_TIME(
			BO_USED_FOR_MAC_PERS_TIME));
#endif  /* BEACON_SUPPORT / No BEACON_SUPPORT */

	/* Start the indirect data persistence timer now. */
	status = pal_timer_start(T_Data_Persistence,
			persistence_int_us,
			TIMEOUT_RELATIVE,
			(FUNC_PTR)mac_t_persistence_cb,
			NULL);

	if (MAC_SUCCESS != status) {
		/* Got to the persistence timer callback function immediately.
		**/
		mac_t_persistence_cb(NULL);
#if (_DEBUG_ > 0)
		Assert("Indirect data persistence timer start failed" == 0);
#endif
	}
}

#endif /* (MAC_INDIRECT_DATA_FFD == 1) */

#if (MAC_INDIRECT_DATA_FFD == 1)

/*
 * @brief Handles timeout of indirect data persistence timer
 *
 * This function is a callback function of the timer started for checking
 * the mac persistence time of indirect data in the queue.
 *
 * @param callback_parameter Callback parameter
 */
static void mac_t_persistence_cb(void *callback_parameter)
{
	/* Decrement the persistence time for indirect data. */
	handle_persistence_time_decrement();

	if (indirect_data_q.size > 0) {
		/* Restart persistence timer. */
		mac_start_persistence_timer();
	}

	callback_parameter = callback_parameter; /* Keep compiler happy. */
}

#endif /* (MAC_INDIRECT_DATA_FFD == 1) */

#if (MAC_INDIRECT_DATA_FFD == 1)

/*
 * @brief Handles the decrement of the persistence time
 *
 * Handles the decrement of the persistance time of each indirect data frame
 * in the indirect queue.
 * If the persistance time of any indirect data reduces to zero, the
 * frame is removed from the indirect queue and
 * a confirmation for that indirect data is sent with the status
 * transaction expired.
 */
static void handle_persistence_time_decrement(void)
{
	search_t find_buf;

	/*
	 * This callback function traverses through the indirect queue and
	 * decrements the persistence time for each data frame.
	 */
	find_buf.criteria_func = decrement_persistence_time;

	/*
	 * At the end of this function call (qmm_queue_read), the indirect data
	 * will be updated with the decremented persistence time.
	 */
	qmm_queue_read(&indirect_data_q, &find_buf);

	/*
	 * Once we have updated the persistence timer, any frame with a
	 * persistence
	 * time of zero needs to be removed from the indirect queue.
	 */
	buffer_t *buffer_persistent_zero = NULL;

	/*
	 * This callback function traverses through the indirect queue and
	 * searches for a data frame with persistence time equal to zero.
	 */
	find_buf.criteria_func = check_persistence_time_zero;

	do {
		buffer_persistent_zero = qmm_queue_remove(&indirect_data_q,
				&find_buf);

		if (NULL != buffer_persistent_zero) {
			handle_exp_persistence_timer(buffer_persistent_zero);
		}
	} while (NULL != buffer_persistent_zero);
}

#endif /* (MAC_INDIRECT_DATA_FFD == 1) */

#if (MAC_INDIRECT_DATA_FFD == 1)

/*
 * @brief Decrements the persistence time of an indirect data frame
 *
 * @param buf_ptr Pointer to the indirect data in the indirect queue
 * @param handle Callback parameter
 *
 * @return 0 to traverse through the full indirect queue
 *
 */
static uint8_t decrement_persistence_time(void *buf_ptr, void *handle)
{
	frame_info_t *frame = (frame_info_t *)buf_ptr;

	/*
	 * In case the frame is currently in the process of being transmitted,
	 * the persistence time is not decremented, to avoid the expiration of
	 * the persistence timer during transmission.
	 * Once the transmission is done (and was not successful),
	 * the frame will still be in the indirect queue and the persistence
	 * timer will be decremented again.
	 */
	if (!frame->indirect_in_transit) {
		/* Decrement the persistence time for this indirect data frame.
		**/
		frame->persistence_time--;
	}

	handle = handle; /* Keep compiler happy. */

	return 0;
}

#endif /* (MAC_INDIRECT_DATA_FFD == 1) */

#if (MAC_INDIRECT_DATA_FFD == 1)

/*
 * @brief Checks for indirect data with persistence time zero
 *
 * This callback function checks whether the persistence time
 * of the indirect data is set to zero.
 *
 * @param buf Pointer to indirect data buffer
 *
 * @return 1 if extended address passed matches with the destination
 * address of the indirect frame , 0 otherwise
 */
static uint8_t check_persistence_time_zero(void *buf_ptr, void *handle)
{
	frame_info_t *frame = (frame_info_t *)buf_ptr;

	/* Frame shall not be in transmission. */
	if (!frame->indirect_in_transit) {
		if (frame->persistence_time == 0) {
			return 1;
		}
	}

	handle = handle; /* Keep compiler happy. */

	return 0;
}

#endif /* (MAC_INDIRECT_DATA_FFD == 1) */

#if (MAC_INDIRECT_DATA_FFD == 1)

/*
 * @brief Generates notification for expired transaction
 *
 * This function generates the confirmation for those indirect data buffers
 * whose persistence time has reduced to zero.
 *
 * @param buf_ptr Pointer to buffer of indirect data whose persistance time
 * has reduced to zero
 */
static void handle_exp_persistence_timer(buffer_t *buf_ptr)
{
	frame_info_t *trans_frame = (frame_info_t *)BMM_BUFFER_POINTER(buf_ptr);

	/*
	 * The frame should never be in transmission while this function
	 * is called.
	 */
	Assert(trans_frame->indirect_in_transit == false);

	switch (trans_frame->msg_type) {
#if (MAC_ASSOCIATION_INDICATION_RESPONSE == 1)
	case ASSOCIATIONRESPONSE:
	{
		mac_mlme_comm_status(MAC_TRANSACTION_EXPIRED,
				buf_ptr);
	}
	break;
#endif  /* (MAC_ASSOCIATION_INDICATION_RESPONSE == 1) */

#if (MAC_DISASSOCIATION_BASIC_SUPPORT == 1)
	case DISASSOCIATIONNOTIFICATION:

		/*
		 * Prepare disassociation confirm message after transmission of
		 * the disassociation notification frame.
		 */
		mac_prep_disassoc_conf((buffer_t *)buf_ptr,
				MAC_TRANSACTION_EXPIRED);
		break;
#endif  /* (MAC_DISASSOCIATION_BASIC_SUPPORT == 1) */

	case MCPS_MESSAGE:
	{
		mac_gen_mcps_data_conf((buffer_t *)buf_ptr,
				(uint8_t)MAC_TRANSACTION_EXPIRED,
#ifdef ENABLE_TSTAMP
				trans_frame->msduHandle,
				0);
#else
				trans_frame->msduHandle);
#endif  /* ENABLE_TSTAMP */
	}
	break;

	default:
		Assert("Unknown buf_ptr type" == 0);
		/* Nothing to be done here. */
		break;
	}
}

#endif /* (MAC_INDIRECT_DATA_FFD == 1) */

#if ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_FFD == 1))

/*
 * @brief Purges a buffer corresponding to a MSDU handle
 *
 * This function tries to purge a given msdu by finding its msdu handle.
 * If the handle is found, that buffer is freed up for further use.
 * This routine will typically be called from the mlme_purge_request routine.
 *
 * @param msdu_handle MSDU handle
 *
 * @return True if the MSDU handle is found in the indirect queue
 *         and removed successfully, false otherwise.
 */
static bool mac_buffer_purge(uint8_t msdu_handle)
{
	buffer_t *buf_ptr;
	search_t find_buf;
	uint8_t handle = msdu_handle;

	/*
	 * Callback function  for searching the data having MSDU handle
	 * given by purge request
	 */
	find_buf.criteria_func = check_msdu_handle_cb;

	/* Update the MSDU handle to be searched */
	find_buf.handle = &handle;

	/* Remove from indirect queue if the short address matches */
	buf_ptr = qmm_queue_remove(&indirect_data_q, &find_buf);

	if (NULL != buf_ptr) {
		/* Free the buffer allocated, after purging */
		bmm_buffer_free((buffer_t *)buf_ptr);

		return true;
	}

	/* No data available in the indirect queue with the MSDU handle. */
	return false;
}

/**
 * @brief Processes a MCPS-PURGE.request primitive
 *
 * This functions processes a MCPS-PURGE.request from the NHLE.
 * The MCPS-PURGE.request primitive allows the next higher layer
 * to purge an MSDU from the transaction queue.
 * On receipt of the MCPS-PURGE.request primitive, the MAC sublayer
 * attempts to find in its transaction queue the MSDU indicated by the
 * msduHandle parameter. If an MSDU matching the given handle is found,
 * the MSDU is discarded from the transaction queue, and the MAC
 * sublayer issues the MCPSPURGE. confirm primitive with a status of
 * MAC_SUCCESS. If an MSDU matching the given handle is not found, the MAC
 * sublayer issues the MCPS-PURGE.confirm primitive with a status of
 * INVALID_HANDLE.
 *
 * @param msg Pointer to the MCPS-PURGE.request parameter
 */
void mcps_purge_request(uint8_t *msg)
{
	mcps_purge_req_t *mpr
		= (mcps_purge_req_t *)BMM_BUFFER_POINTER(((buffer_t *)msg));

	mcps_purge_conf_t *mpc = (mcps_purge_conf_t *)mpr;

	uint8_t purge_handle = mpr->msduHandle;

	/* Update the purge confirm structure */
	mpc->cmdcode = MCPS_PURGE_CONFIRM;
	mpc->msduHandle = purge_handle;

	if (mac_buffer_purge(purge_handle)) {
		mpc->status = MAC_SUCCESS;
	} else {
		mpc->status = MAC_INVALID_HANDLE;
	}

	qmm_queue_append(&mac_nhle_q, (buffer_t *)msg);
}

/*
 * @brief Checks whether MSDU handle matches
 *
 * @param buf Pointer to indirect data buffer
 * @param handle MSDU handle to be searched
 *
 * @return 1 if MSDU handle matches with the indirect data, 0 otherwise
 */
static uint8_t check_msdu_handle_cb(void *buf, void *handle)
{
	frame_info_t *frame = (frame_info_t *)buf;
	uint8_t msdu;

	msdu = *((uint8_t *)handle);

	/* Compare the MSDU handle */
	if (frame->msduHandle == msdu) {
		return 1;
	}

	return 0;
}

#endif /* ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_FFD == 1)) */
/* EOF */
