/**
 * @file mac_process_beacon_frame.c
 *
 * @brief Implements the processing of a received beacon frame.
 * This happens either after scan, where the PAN Descriptor list is filled, or
 * while the device is tracking beacon frames. In the latter case the device
 * checks whether the coordinator does have pending data and initiates the
 * transmission of a data request frame if required.
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

#if ((MAC_SCAN_SUPPORT == 1) || (MAC_SYNC_REQUEST == 1))

/* === Macros =============================================================== */

/*
 * Extract the beacon order from the Superframe Spec.
 */
#define GET_BEACON_ORDER(spec)          ((spec) & 0x000F)

/*
 * Extract the superframe order from the Superframe Spec.
 */
#define GET_SUPERFRAME_ORDER(spec)      (((spec) & 0x00F0) >> 4)

/*
 * Extract the final CAP slot from Superframe Spec.
 */
#define GET_FINAL_CAP(spec)             (((spec) & 0x0F00) >> 8)

/*
 * Extract the GTS permit from GTS Spec.
 */
#define GET_GTS_PERMIT(spec)            (((spec) & 0x80) >> 7)
/* === Globals ============================================================= */

/* === Prototypes ========================================================== */

#ifdef BEACON_SUPPORT
static void mac_t_wait_for_bc_time_cb(void *callback_parameter);

#endif /* BEACON_SUPPORT */

/* === Implementation ====================================================== */

/**
 * @brief Processes received beacon frame
 *
 * This function processes a received beacon frame.
 * When the system is scanning it records PAN descriptor information
 * contained in the beacon. These PAN descriptors will be reported to the
 * next higher layer via MLME_SCAN.confirm.
 * Also this routine constructs the MLME_BEACON_NOTIFY.indication.
 * Additionally when a device is synced with the coordinator, it tracks beacon
 * frames, checks whether the coordinator does have pending data and will
 * initiate the transmission of a data request frame.
 * The routine uses global "parse_data" structure.
 * The PAN descriptors are stored in the mlme_scan_conf_t structure.
 *
 * @param beacon Pointer to the buffer in which the beacon was received
 *
 */
void mac_process_beacon_frame(buffer_t *beacon)
{
	bool matchflag;
	wpan_pandescriptor_t *pand_long_start_p = NULL;
	wpan_pandescriptor_t pand_long;
	mlme_scan_conf_t *msc = NULL;

#if ((MAC_BEACON_NOTIFY_INDICATION == 1) || \
	((MAC_INDIRECT_DATA_BASIC == 1) && (MAC_SYNC_REQUEST == 1)) \
	)
	uint8_t numaddrshort;
	uint8_t numaddrlong;
#endif

#ifdef BEACON_SUPPORT
	uint16_t beacon_length;

	/*
	 * Extract the superframe parameters of the beacon frame only if
	 * scanning is NOT ongoing.
	 */
	if (MAC_SCAN_IDLE == mac_scan_state) {
#if (MAC_START_REQUEST_CONFIRM == 1)
		/* Beacon frames are not of interest for a PAN coordinator. */
		if (MAC_PAN_COORD_STARTED != mac_state)
#endif /* MAC_START_REQUEST_CONFIRM */
		{
			uint8_t superframe_order;
			uint8_t beacon_order;

			/*
			 * For a device, the parameters obtained from the
			 * beacons are used to
			 * update the PIBs at TAL
			 */
			beacon_order
				= GET_BEACON_ORDER(
					mac_parse_data.mac_payload_data.beacon_data.superframe_spec);

#if (_DEBUG_ > 0)
			retval_t set_status =
#endif
			set_tal_pib_internal(macBeaconOrder,
					(void *)&beacon_order);

#if (_DEBUG_ > 0)
			Assert(MAC_SUCCESS == set_status);
#endif
			superframe_order
				= GET_SUPERFRAME_ORDER(
					mac_parse_data.mac_payload_data.beacon_data.superframe_spec);
#if (_DEBUG_ > 0)
			set_status =
#endif
			set_tal_pib_internal(macSuperframeOrder,
					(void *)&superframe_order);

#if (_DEBUG_ > 0)
			Assert(MAC_SUCCESS == set_status);
#endif

			mac_final_cap_slot
				= GET_FINAL_CAP(
					mac_parse_data.mac_payload_data.beacon_data.superframe_spec);

			/*
			 * In a beacon-enabled network with the battery life
			 * extension
			 * enabled, the first backoff slot boundary is computed
			 * after the
			 * end of the beacon's IFS
			 */
			if ((tal_pib.BeaconOrder < NON_BEACON_NWK) &&
					tal_pib.BattLifeExt) {
				/* Length given in octets */
				beacon_length = mac_parse_data.mpdu_length +
						PHY_OVERHEAD;

				/* Convert to symbols */
				beacon_length *= SYMBOLS_PER_OCTET;

				/* Space needed for IFS is added to it */
				if (mac_parse_data.mpdu_length <=
						aMaxSIFSFrameSize) {
					beacon_length += macMinSIFSPeriod_def;
				} else {
					beacon_length += macMinLIFSPeriod_def;
				}

				/* Round up to backoff slot boundary */
				beacon_length
					= (beacon_length +
						aUnitBackoffPeriod -
						1) / aUnitBackoffPeriod;
				beacon_length *= aUnitBackoffPeriod;

				/*
				 * Slotted CSMA-CA with macBattLifeExt must
				 * start within
				 * the first macBattLifeExtPeriods backoff slots
				 * of the CAP
				 */
				beacon_length
					+= mac_pib.mac_BattLifeExtPeriods *
						aUnitBackoffPeriod;
			}

#ifdef GTS_SUPPORT
			mac_pib.mac_GTSPermit = GET_GTS_PERMIT(
					mac_parse_data.mac_payload_data.beacon_data.gts_spec);

			if (mac_parse_data.mac_payload_data.beacon_data.gts_spec
					& 0x07) {
				mac_parse_bcn_gts_info(
						mac_parse_data.mac_payload_data.beacon_data.gts_spec & 0x07,
						mac_parse_data.mac_payload_data.beacon_data.gts_direction,
						mac_parse_data.mac_payload_data.beacon_data.gts_list);
			}

			{
				uint8_t table_index;
				gts_char_t gts_char;
				for (table_index = 0;
						table_index < MAX_GTS_ON_DEV;
						table_index++) {
					if (GTS_STATE_REQ_SENT ==
							mac_dev_gts_table[
								table_index].
							GtsState &&
							0 <
							mac_dev_gts_table[
								table_index
							].GtsPersistCount &&
							0 ==
							--mac_dev_gts_table[
								table_index].
							GtsPersistCount) {
						gts_char.GtsCharType
							= GTS_ALLOCATE;
						gts_char.GtsDirection
							= table_index &
								0x01;
						gts_char.GtsLength
							= mac_dev_gts_table
								[table_index].
								GtsLength;
						gts_char.Reserved = 0;
						mac_dev_gts_table[table_index].
						GtsState
							= GTS_STATE_IDLE;
						mac_dev_gts_table[table_index].
						GtsLength = 0;
						mac_gen_mlme_gts_conf((buffer_t
								*)mac_dev_gts_table[
									table_index].
								GtsReq_ptr,
								MAC_NO_DATA,
								gts_char);
					}
				}
			}
#endif /* GTS_SUPPORT */
		} /* (MAC_PAN_COORD_STARTED != mac_state) */
	} /* (MAC_SCAN_IDLE == mac_scan_state) */
#endif  /* BEACON_SUPPORT */

	/*
	 * The following section needs to be done when we are
	 * either scanning (and look for a new PAN descriptor to be returned
	 * as part of the scan confirm message),
	 * or we need to create a beacon notification (in which case we are
	 * interested in any beacon, but omit the generation of scan confirm).
	 */
	{
		uint8_t index;

		/*
		 * If we are scanning a scan confirm needs to be created.
		 *
		 * According to 802.15.4-2006 this is only done in case the PIB
		 * attribute macAutoRequest is true. Otherwise the PAN
		 * descriptor will
		 * NOT be put into the PAN descriptor list of the Scan confirm
		 * message.
		 */
		if (
			((MAC_SCAN_ACTIVE == mac_scan_state) ||
			(MAC_SCAN_PASSIVE == mac_scan_state)) &&
			mac_pib.mac_AutoRequest
			) {
			/*
			 * mac_conf_buf_ptr points to the buffer allocated for
			 * scan
			 * confirmation.
			 */
			msc =  (mlme_scan_conf_t *)BMM_BUFFER_POINTER(
					((buffer_t *)mac_conf_buf_ptr));

			/*
			 * The PAN descriptor list is updated with the
			 * PANDescriptor of the
			 * received beacon
			 */
			pand_long_start_p
				= (wpan_pandescriptor_t *)&msc->scan_result_list;
		}

		/*
		 * The beacon data received from the parse variable is arranged
		 * into a PAN descriptor structure style
		 */
		pand_long.CoordAddrSpec.AddrMode = mac_parse_data.src_addr_mode;
		pand_long.CoordAddrSpec.PANId = mac_parse_data.src_panid;

		if (FCF_SHORT_ADDR == pand_long.CoordAddrSpec.AddrMode) {
			/* Initially clear the complete address. */
			pand_long.CoordAddrSpec.Addr.long_address = 0;

			ADDR_COPY_DST_SRC_16(
					pand_long.CoordAddrSpec.Addr.short_address,
					mac_parse_data.src_addr.short_address);
		} else {
			ADDR_COPY_DST_SRC_64(
					pand_long.CoordAddrSpec.Addr.long_address,
					mac_parse_data.src_addr.long_address);
		}

		pand_long.LogicalChannel = tal_pib.CurrentChannel;
		pand_long.ChannelPage    = tal_pib.CurrentPage;
		pand_long.SuperframeSpec
			= mac_parse_data.mac_payload_data.beacon_data.
				superframe_spec;
		pand_long.GTSPermit
			= mac_parse_data.mac_payload_data.beacon_data.
				gts_spec >> 7;
		pand_long.LinkQuality    = mac_parse_data.ppdu_link_quality;
#ifdef ENABLE_TSTAMP
		pand_long.TimeStamp      = mac_parse_data.time_stamp;
#endif  /* ENABLE_TSTAMP */

		/*
		 * If we are scanning we need to check whether this is a new
		 * PAN descriptor.
		 *
		 * According to 802.15.4-2006 this is only done in case the PIB
		 * attribute macAutoRequest is true. Otherwise the PAN
		 * descriptor will
		 * NOT be put into the PAN descriptor list of the Scan confirm
		 * message.
		 */
		if (
			((MAC_SCAN_ACTIVE == mac_scan_state) ||
			(MAC_SCAN_PASSIVE == mac_scan_state)) &&
			mac_pib.mac_AutoRequest
			) {
			/*
			 * This flag is used to indicate a match of the current
			 *(received) PAN
			 * descriptor with one of those present already in the
			 * list.
			 */
			matchflag = false;

			/*
			 * The beacon frame PAN descriptor is compared with the
			 * PAN descriptors
			 * present in the list and determine if the current PAN
			 * descriptor is to be taken as a valid one. A PAN is
			 * considered to be
			 * the same as an existing one, if all, the PAN Id, the
			 * coordinator address
			 * mode, the coordinator address, and the Logical
			 * Channel are same.
			 */
			for (index = 0; index < msc->ResultListSize;
					index++, pand_long_start_p++) {
				if ((pand_long.CoordAddrSpec.PANId ==
						pand_long_start_p->CoordAddrSpec
						.PANId) &&
						(pand_long.CoordAddrSpec.
						AddrMode ==
						pand_long_start_p->CoordAddrSpec
						.AddrMode) &&
						(pand_long.LogicalChannel ==
						pand_long_start_p->
						LogicalChannel) &&
						(pand_long.ChannelPage ==
						pand_long_start_p->ChannelPage)
						) {
					if (pand_long.CoordAddrSpec.AddrMode ==
							WPAN_ADDRMODE_SHORT) {
						if (pand_long.CoordAddrSpec.Addr
								.short_address
								==
								pand_long_start_p
								->CoordAddrSpec.
								Addr.
								short_address) {
							/* Beacon with same
							 * parameters already
							 * received */
							matchflag = true;
							break;
						}
					} else {
						if (pand_long.CoordAddrSpec.Addr
								.long_address ==
								pand_long_start_p
								->CoordAddrSpec.
								Addr.
								long_address) {
							/* Beacon with same
							 * parameters already
							 * received */
							matchflag = true;
							break;
						}
					}
				}
			}

			/*
			 * If the PAN descriptor is not in the current list, and
			 * there is space
			 * left, it is put into the list
			 */
			if ((!matchflag) &&
					(msc->ResultListSize <
					MAX_PANDESCRIPTORS)) {
				memcpy(pand_long_start_p, &pand_long,
						sizeof(pand_long));
				msc->ResultListSize++;
			}
		}
	}

#if ((MAC_BEACON_NOTIFY_INDICATION == 1) || \
	((MAC_INDIRECT_DATA_BASIC == 1) && (MAC_SYNC_REQUEST == 1)) \
	)

	/* The short and extended pending addresses are extracted from the
	 * beacon */
	numaddrshort
		= NUM_SHORT_PEND_ADDR(
			mac_parse_data.mac_payload_data.beacon_data.pending_addr_spec);

	numaddrlong
		= NUM_LONG_PEND_ADDR(
			mac_parse_data.mac_payload_data.beacon_data.pending_addr_spec);
#endif

#if (MAC_BEACON_NOTIFY_INDICATION == 1)

	/*
	 * In all cases (PAN or device) if the payload is not equal to zero
	 * or macAutoRequest is false, MLME_BEACON_NOTIFY.indication is
	 * generated
	 */
	if ((mac_parse_data.mac_payload_data.beacon_data.beacon_payload_len >
			0) ||
			(!mac_pib.mac_AutoRequest)
			) {
		mlme_beacon_notify_ind_t *mbni
			= (mlme_beacon_notify_ind_t *)BMM_BUFFER_POINTER(((
					buffer_t *)beacon));

		/* The beacon notify indication structure is built */
		mbni->cmdcode       = MLME_BEACON_NOTIFY_INDICATION;
		mbni->BSN           = mac_parse_data.sequence_number;
		mbni->PANDescriptor = pand_long;
		mbni->PendAddrSpec
			= mac_parse_data.mac_payload_data.beacon_data.
				pending_addr_spec;

		if ((numaddrshort > 0) || (numaddrlong > 0)) {
			mbni->AddrList
				= mac_parse_data.mac_payload_data.beacon_data
					.pending_addr_list;
		}

		mbni->sduLength
			= mac_parse_data.mac_payload_data.beacon_data.
				beacon_payload_len;
		mbni->sdu
			= mac_parse_data.mac_payload_data.beacon_data.
				beacon_payload;

		/*
		 * The beacon notify indication is given to the NHLE and then
		 * the buffer
		 * is freed up.
		 */
		qmm_queue_append(&mac_nhle_q, (buffer_t *)beacon);
	} else
#endif /* (MAC_BEACON_NOTIFY_INDICATION == 1) */
	{
		/* Payload is not present, hence the buffer is freed here */
		bmm_buffer_free(beacon);
	}

	/* Handling of ancounced broadcast traffic by the parent. */
#ifdef BEACON_SUPPORT
	if (MAC_SCAN_IDLE == mac_scan_state) {
		/*
		 * In case this is a beaconing network, and this node is not
		 * scanning,
		 * and the FCF indicates pending data thus indicating broadcast
		 * data at
		 * parent, the node needs to be awake until the received
		 * broadcast
		 * data has been received.
		 */
		if (mac_parse_data.fcf & FCF_FRAME_PENDING) {
			mac_bc_data_indicated = true;

			/*
			 * Start timer since the broadcast frame is expected
			 * within
			 * macMaxFrameTotalWaitTime symbols.
			 */
			if (MAC_POLL_IDLE == mac_poll_state) {
				/*
				 * If the poll state is not idle, there is
				 * already an
				 * indirect transaction ongoing.
				 * Since the T_Poll_Wait_Time is going to be
				 * re-used,
				 * this timer can only be started, if we are not
				 * in
				 * a polling state other than idle.
				 */
				uint32_t response_timer
					= mac_pib.mac_MaxFrameTotalWaitTime;
				response_timer = TAL_CONVERT_SYMBOLS_TO_US(
						response_timer);

				if (MAC_SUCCESS !=
						pal_timer_start(T_Poll_Wait_Time,
						response_timer,
						TIMEOUT_RELATIVE,
						(FUNC_PTR)
						mac_t_wait_for_bc_time_cb,
						NULL)) {
					mac_t_wait_for_bc_time_cb(NULL);
				}
			} else {
				/*
				 * Any indirect poll operation is ongoing, so
				 * the timer will
				 * not be started, i.e. nothing to be done here.
				 * Once this ongoing indirect transaction has
				 * finished, this
				 * node will go back to sleep anyway.
				 */
			}
		} else {
			mac_bc_data_indicated = false;
		}
	} /* (MAC_SCAN_IDLE == mac_scan_state) */
#endif /* BEACON_SUPPORT */

	/* Handling of presented indirect traffic by the parent for this node.
	**/
#if ((MAC_INDIRECT_DATA_BASIC == 1) && (MAC_SYNC_REQUEST == 1))
	if (MAC_SCAN_IDLE == mac_scan_state) {
		/*
		 * If this node is NOT scanning, and is doing a
		 * mlme_sync_request,
		 * then the pending address list of the beacon is examined to
		 * see
		 * if the node's parent has data for this node.
		 */
		if (mac_pib.mac_AutoRequest) {
			if (MAC_SYNC_NEVER != mac_sync_state) {
				uint8_t index;
#if (_DEBUG_ > 0)
				bool status;
#endif

				/*
				 * Short address of the device is compared with
				 * the
				 * pending short address in the beacon frame
				 */

				/*
				 * PAN-ID and CoordAddress does not have to be
				 * checked here,
				 * since the device is already synced with the
				 * coordinator, and
				 * only beacon frames passed from data_ind.c
				 *(where the first level
				 * filtering is already done) are received. The
				 * pending addresses
				 * in the beacon frame are compared with the
				 * device address. If a
				 * match is found, it indicates that a data
				 * belonging to this
				 * deivce is present with the coordinator and
				 * hence a data request
				 * is sent to the coordinator.
				 */
				uint16_t cur_short_addr;

				for (index = 0; index < numaddrshort; index++) {
					cur_short_addr
						= convert_byte_array_to_16_bit((
								mac_parse_data.
								mac_payload_data
								.
								beacon_data.
								pending_addr_list
								+
								index *
								sizeof(uint16_t)));
					if (cur_short_addr ==
							tal_pib.ShortAddress) {
						/*
						 * Device short address matches
						 * with one of the address
						 * in the beacon address list.
						 * Implicit poll (using the
						 * device short address) is done
						 * to get the pending data
						 */
#if (_DEBUG_ > 0)
						status =
#endif
						mac_build_and_tx_data_req(false,
								false,
								0,
								NULL,
								0);

#if (_DEBUG_ > 0)
						Assert(status == true);
#endif
						return;
					}
				}

				/*
				 * Extended address of the device is compared
				 * with
				 * the pending extended address in the beacon
				 * frame
				 */
				uint64_t cur_long_addr;

				for (index = 0; index < numaddrlong; index++) {
					cur_long_addr
						= convert_byte_array_to_64_bit((
								mac_parse_data.
								mac_payload_data
								.
								beacon_data.
								pending_addr_list
								+
								numaddrshort *
								sizeof(uint16_t)
								+
								index *
								sizeof(uint64_t)));

					if (cur_long_addr ==
							tal_pib.IeeeAddress) {
						/*
						 * Device extended address
						 * matches with one of the
						 * address in the beacon address
						 * list. Implicit poll
						 * (using the device extended
						 * address) is done to get
						 * the pending data
						 */
#if (_DEBUG_ > 0)
						status =
#endif
						mac_build_and_tx_data_req(false,
								true,
								0,
								NULL,
								0);

#if (_DEBUG_ > 0)
						Assert(status == true);
#endif
						return;
					}
				}
			}
		} /* (mac_pib.mac_AutoRequest) */
	} /* (MAC_SCAN_IDLE == mac_scan_state) */
#endif /* (MAC_INDIRECT_DATA_BASIC == 1) && (MAC_SYNC_REQUEST == 1)) */
} /* mac_process_beacon_frame() */

#endif /* ((MAC_SCAN_SUPPORT == 1) || (MAC_SYNC_REQUEST == 1)) */

#ifdef BEACON_SUPPORT

/**
 * @brief Handle T_Poll_Wait_Time timer in case we expected a broadcast
 *        frame from our coordinator, but did not receive one.
 *
 * @param callback_parameter Callback parameter.
 */
static void mac_t_wait_for_bc_time_cb(void *callback_parameter)
{
	if ((mac_bc_data_indicated) &&
			(MAC_SCAN_IDLE == mac_scan_state) &&
			(MAC_POLL_IDLE == mac_poll_state)
			) {
		mac_bc_data_indicated = false;

		/*
		 * No expected broadcast frame has been received, so the node is
		 * sent
		 * back to sleep.
		 */
		mac_sleep_trans();
	}

	callback_parameter = callback_parameter; /* Keep compiler happy. */
}

#endif  /* BEACON_SUPPORT */

/* EOF */
