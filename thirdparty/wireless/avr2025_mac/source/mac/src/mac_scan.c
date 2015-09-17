/**
 * @brief Implements the MLME_SCAN primitives.
 *
 * This file implements functions to handle all MLME-SCAN primitives and the
 * corresponding scan algorithm. This includes ED, Active, Passive and Orphan
 * scan. All required timers and frames (beacon request and orphan
 * notification frames) are assembled and their transmission is initiated.
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
#include <stddef.h>
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
#include "mac_config.h"
#include "mac_build_config.h"

#if (MAC_SCAN_SUPPORT == 1)

/* === Macros =============================================================== */

/**
 * Scan duration formula: \f$aBaseSuperframeDuration (2^SD + 1)\f$
 * where \f$0 <= SD <= 14\f$
 */
#define MAC_CALCULATE_SYMBOL_TIME_SCANDURATION(SD) \
	(aBaseSuperframeDuration * ((1UL << (SD)) + 1))

/*
 * Max beacon Order in beacon-enabled network
 */
#define BEACON_NETWORK_MAX_BO           (14)

/*
 * Beacon Request and Orphan Notification command frame payload length
 */
#define BEAC_REQ_ORPH_NOT_PAYLOAD_LEN       (1)

/* === Globals ============================================================= */

static uint8_t scan_type;
static uint32_t scan_channels;
static uint8_t scan_curr_channel;
static uint8_t scan_curr_page;
static uint8_t scan_duration;

#if ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) || \
	(MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1))
/* Original PAN-ID before starting of active or passive scan. */
#endif /* ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) ||
        *(MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1)) */

/* === Prototypes ========================================================== */

#if ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) || \
	(MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1) || \
	(MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1))
static void mac_t_scan_duration_cb(void *callback_parameter);

#endif /* ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) || \
        *  (MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1) || \
        *  (MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1))
        */

static void mac_awake_scan(buffer_t *buff_ptr);
static void scan_set_complete(retval_t set_status);
static void scan_clean_up(buffer_t *buf);
static void scan_proceed(uint8_t scanning_type, buffer_t *buf);
static bool send_scan_cmd(bool beacon_req);

/* === Implementation ====================================================== */

/*
 * @brief Proceed with a scan request
 *
 * This function proceeds with the scanning.
 * The current channel is incremented. It is checked if it belongs to the
 * list of channels to scan. If so, start scanning. If all channels done,
 * send out the MLME_SCAN.confirm message.
 *
 * @param scanning_type The type of the scan operation to proceed with.
 * @param buf Buffer to send mlme scan confirm to NHLE.
 */
static void scan_proceed(uint8_t scanning_type, buffer_t *buf)
{
	retval_t set_status;
	mlme_scan_conf_t *msc = (mlme_scan_conf_t *)BMM_BUFFER_POINTER(buf);

	/* Set the channel page to perform scan */
	set_status = set_tal_pib_internal(phyCurrentPage,
			(void *)&scan_curr_page);

	/* Loop over all channels the MAC has been requested to scan */
	for (; scan_curr_channel <= MAX_CHANNEL; scan_curr_channel++) {
#if ((MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1) ||	\
		(MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1))
		if (
			((MAC_SCAN_ACTIVE == mac_scan_state) ||
			(MAC_SCAN_PASSIVE == mac_scan_state)) &&
			mac_pib.mac_AutoRequest
			) {
			/*
			 * For active or passive scans, bail out if we
			 * reached our maximum number of PANDescriptors that
			 * could
			 * be stored. That way, the upper layer will get the
			 * correct set of unscanned channels returned, so it can
			 * continue scanning if desired.
			 *
			 * According to 802.15.4-2006 PAN descriptor are only
			 * present
			 * in the scan confirm message in case the PIB attribute
			 * macAutoRequest is true.
			 */
			if (msc->ResultListSize >= MAX_PANDESCRIPTORS) {
				break;
			}
		}
#endif /* ((MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1) ||
		 *(MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1)) */
#if (MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1)
		if (MLME_SCAN_TYPE_ORPHAN == scanning_type) {
			/*
			 * In an orphan scan, terminate if any coordinator
			 * realignment packet has been received.
			 */
			if (msc->ResultListSize) {
				break;
			}
		}
#endif /* (MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1) */

		if ((msc->UnscannedChannels & (1UL << scan_curr_channel)) !=
				0) {
#if (MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1)
			if (MLME_SCAN_TYPE_ACTIVE == scanning_type) {
				mac_scan_state = MAC_SCAN_ACTIVE;
			}
#endif /* (MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) */
#if (MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1)
			if (MLME_SCAN_TYPE_PASSIVE == scanning_type) {
				mac_scan_state = MAC_SCAN_PASSIVE;
			}
#endif /* (MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1) */
			if (MLME_SCAN_TYPE_ORPHAN == scanning_type) {
				mac_scan_state = MAC_SCAN_ORPHAN;
			}

			/* Set the channel to perform scan */
			set_status = set_tal_pib_internal(phyCurrentChannel,
					(void *)&scan_curr_channel);

			if (MAC_SUCCESS != set_status) {
				/*
				 * Free the buffer used for sending orphan
				 * notification command
				 */
				bmm_buffer_free((buffer_t *)mac_scan_cmd_buf_ptr);

				mac_scan_cmd_buf_ptr = NULL;

				/* Set radio to sleep if allowed */
				mac_sleep_trans();

				msc->status = MAC_NO_BEACON;

				/* Orphan scan does not return any list. */
				msc->ResultListSize = 0;

				/* Append the scan confirm message to the
				 * MAC-NHLE queue */
				qmm_queue_append(&mac_nhle_q, buf);

				mac_scan_state = MAC_SCAN_IDLE;
			}

#if (_DEBUG_ > 0)
			Assert(MAC_SUCCESS == set_status);
#endif
			/* Continue scanning, after setting channel */
			scan_set_complete(set_status);
			return;
		}
	}

	/* All channels were scanned. The confirm needs to be prepared */
	switch (scanning_type) {
#if (MAC_SCAN_ED_REQUEST_CONFIRM == 1)
	case MLME_SCAN_TYPE_ED:
		msc->status = MAC_SUCCESS;
		scan_clean_up(buf);
		break;
#endif /* (MAC_SCAN_ED_REQUEST_CONFIRM == 1) */

#if (MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1)
	case MLME_SCAN_TYPE_ACTIVE:
	{
		/*
		 * Free the buffer which was received from scan request and
		 * reused
		 * for beacon request frame transmission.
		 */
		bmm_buffer_free((buffer_t *)mac_scan_cmd_buf_ptr);

		mac_scan_cmd_buf_ptr = NULL;

		if (!mac_pib.mac_AutoRequest) {
			msc->status = MAC_SUCCESS;
		} else if (msc->ResultListSize >= MAX_PANDESCRIPTORS) {
			msc->status = MAC_LIMIT_REACHED;
		} else if (msc->ResultListSize) {
			msc->status = MAC_SUCCESS;
		} else {
			msc->status = MAC_NO_BEACON;
		}

		/* Restore macPANId after active scan completed. */
#if (_DEBUG_ > 0)
		set_status =
#endif
		set_tal_pib_internal(macPANId, (void *)&mac_scan_orig_panid);

#if (_DEBUG_ > 0)
		Assert(MAC_SUCCESS == set_status);
#endif
		/* Done with scanning */
		scan_clean_up((buffer_t *)mac_conf_buf_ptr);
	}
	break;
#endif /* (MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) */

#if (MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1)
	case MLME_SCAN_TYPE_PASSIVE:
		if (!mac_pib.mac_AutoRequest) {
			msc->status = MAC_SUCCESS;
		} else if (msc->ResultListSize >= MAX_PANDESCRIPTORS) {
			msc->status = MAC_LIMIT_REACHED;
		} else if (msc->ResultListSize) {
			msc->status = MAC_SUCCESS;
		} else {
			msc->status = MAC_NO_BEACON;
		}

		/* Restore macPANId after passive scan completed. */
#if (_DEBUG_ > 0)
		set_status =
#endif
		set_tal_pib_internal(macPANId, (void *)&mac_scan_orig_panid);

#if (_DEBUG_ > 0)
		Assert(MAC_SUCCESS == set_status);
#endif
		scan_clean_up(buf);
		break;
#endif /* (MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1) */

#if (MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1)
	case MLME_SCAN_TYPE_ORPHAN:

		/* Free the buffer used for sending orphan notification command
		**/
		bmm_buffer_free((buffer_t *)mac_scan_cmd_buf_ptr);

		mac_scan_cmd_buf_ptr = NULL;

		if (msc->ResultListSize > 0) {
			msc->status = MAC_SUCCESS;
		} else {
			msc->status = MAC_NO_BEACON;
		}

		/* Orphan scan does not return any list. */
		msc->ResultListSize = 0;

		scan_clean_up(buf);
		break;
#endif /* (MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1) */

	default:
		break;
	}
} /* scan_proceed() */

/*
 * @brief Continues handling of MLME_SCAN.request once the radio is awake
 *
 * @param scan_buf Pointer to Scan request buffer.
 */
static void mac_awake_scan(buffer_t *scan_buf)
{
	mlme_scan_conf_t *msc;
	msc = (mlme_scan_conf_t *)BMM_BUFFER_POINTER(scan_buf);

	/* Set the first channel at which the scan is started */
	scan_curr_channel = MIN_CHANNEL;

	switch (scan_type) {
#if (MAC_SCAN_ED_REQUEST_CONFIRM == 1)
	case MLME_SCAN_TYPE_ED:
		msc->scan_result_list[0].ed_value[1] = 0; /* First channel's
		                                           * accumulated energy
		                                           * level */
		mac_scan_state = MAC_SCAN_ED;
		scan_proceed(MLME_SCAN_TYPE_ED, (buffer_t *)scan_buf);
		break;
#endif /* (MAC_SCAN_ED_REQUEST_CONFIRM == 1) */

#if ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) || \
		(MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1))
	case MLME_SCAN_TYPE_ACTIVE:
	case MLME_SCAN_TYPE_PASSIVE:
	{
		/*
		 * Before commencing an active or passive scan, the MAC sublayer
		 * shall store the value of macPANId and then set it to 0cFFFF
		 * for
		 * the duration of the scan. This enables the receive filter to
		 * accept all beacons rather than just the beacons from its
		 * current PAN (see 7.5.6.2). On completion of the scan, the
		 * MAC sublayer shall restore the value of macPANId to the
		 * value stored before the scan began.
		 */
		uint16_t broadcast_panid = BROADCAST;

		mac_scan_orig_panid = tal_pib.PANId;

#if (_DEBUG_ > 0)
		retval_t set_status =
#endif
		set_tal_pib_internal(macPANId, (void *)&broadcast_panid);

#if (_DEBUG_ > 0)
		Assert(MAC_SUCCESS == set_status);
		set_status = set_status;
#endif
		if (MLME_SCAN_TYPE_ACTIVE == scan_type) {
			/*
			 * In active scan reuse the scan request buffer for
			 * sending beacon request.
			 */
			mac_scan_cmd_buf_ptr = (uint8_t *)scan_buf;
		}

		/* Allocate a large size buffer for scan confirm. */
		mac_conf_buf_ptr
			= (uint8_t *)bmm_buffer_alloc(LARGE_BUFFER_SIZE);

		if (NULL == mac_conf_buf_ptr) {
			/*
			 * Large buffer is not available for sending scan
			 * confirmation,
			 * hence the scan request buffer (small buffer) is used
			 * to send
			 * the scan confirmation.
			 */
			msc->status = MAC_INVALID_PARAMETER;

			/* Append scan confirm message to the MAC-NHLE queue */
			qmm_queue_append(&mac_nhle_q, scan_buf);

			/* Set radio to sleep if allowed */
			mac_sleep_trans();
			return;
		}

		if (MLME_SCAN_TYPE_PASSIVE == scan_type) {
			/* Free the scan request buffer when in passive scan. */
			bmm_buffer_free(scan_buf);
		}

		msc = (mlme_scan_conf_t *)BMM_BUFFER_POINTER(
				(buffer_t *)mac_conf_buf_ptr);

		msc->cmdcode = MLME_SCAN_CONFIRM;
		msc->ScanType = scan_type;
		msc->ChannelPage = scan_curr_page;
		msc->UnscannedChannels = scan_channels;
		msc->ResultListSize = 0;
		msc->scan_result_list[0].ed_value[0] = 0;

		scan_proceed(scan_type, (buffer_t *)mac_conf_buf_ptr);
		break;
	}
#endif /* ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) ||
		 *(MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1)) */

#if (MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1)
	case MLME_SCAN_TYPE_ORPHAN:
		/* Buffer allocated for orphan notification command */
		mac_scan_cmd_buf_ptr
			= (uint8_t *)bmm_buffer_alloc(LARGE_BUFFER_SIZE);

		if (NULL == mac_scan_cmd_buf_ptr) {
			msc->status = MAC_INVALID_PARAMETER;

			/* Append scan confirm message to the MAC-NHLE queue */
			qmm_queue_append(&mac_nhle_q, scan_buf);

			/* Set radio to sleep if allowed */
			mac_sleep_trans();
			return;
		}

		scan_proceed(MLME_SCAN_TYPE_ORPHAN,
				(buffer_t *)mac_conf_buf_ptr);
		break;
#endif /* (MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1) */

	default:
		msc->status = MAC_INVALID_PARAMETER;
		/* Append scan confirm message to the MAC-NHLE queue */
		qmm_queue_append(&mac_nhle_q, scan_buf);

		/* Set radio to sleep if allowed */
		mac_sleep_trans();
		break;
	}
} /* mac_awake_scan() */

#if ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) || \
	(MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1))

/**
 * @brief Send a beacon request or orphan notification command frame
 *
 * This function sends a beacon request or orphan notification command frame.
 * An MPDU containing either a beacon request or an orphan notification command
 * frame is constructed and sent.
 *
 * @param beacon_req True if a beacon request command frame shall be sent,
 *                   otherwise (false) an orphan notification command frame
 *                   will be sent.
 *
 * @return True if the frame transmission has been initiated, false otherwise.
 */
static bool send_scan_cmd(bool beacon_req)
{
	retval_t tal_tx_status;
	uint8_t frame_len;
	uint8_t *frame_ptr;
	uint16_t fcf;
	uint16_t bc_addr = BROADCAST;

	/*
	 * mac_scan_cmd_buf_ptr holds the buffer allocated for sending beacon
	 * request or orphan notification command. In active scan the scan
	 * request
	 * buffer is used to send a beacon request and in orphan scan new buffer
	 * is
	 * allocated to send an orphan notification.
	 */
	frame_info_t *transmit_frame
		= (frame_info_t *)BMM_BUFFER_POINTER(
			(buffer_t *)mac_scan_cmd_buf_ptr);

	/* Get the payload pointer. */
	frame_ptr = (uint8_t *)transmit_frame +
			LARGE_BUFFER_SIZE -
			BEAC_REQ_ORPH_NOT_PAYLOAD_LEN - 2; /* Add 2 octets for
	                                                    * FCS. */

	transmit_frame->buffer_header = (buffer_t *)mac_scan_cmd_buf_ptr;

	if (beacon_req) {
		fcf = FCF_SET_FRAMETYPE(FCF_FRAMETYPE_MAC_CMD) |
				FCF_SET_DEST_ADDR_MODE(FCF_SHORT_ADDR) |
				FCF_SET_SOURCE_ADDR_MODE(FCF_NO_ADDR);

		/* Update the length. */
		frame_len = BEAC_REQ_ORPH_NOT_PAYLOAD_LEN +
				2 + /* Add 2 octets for FCS */
				2 + /* 2 octets for short Destination Address */
				2 + /* 2 octets for Destination PAN-Id */
				3; /* 3 octets DSN and FCF */

		/* Build the command frame id. */
		*frame_ptr = transmit_frame->msg_type = BEACONREQUEST;
	} else { /* Orphan Notification command */
		fcf = FCF_SET_FRAMETYPE(FCF_FRAMETYPE_MAC_CMD) |
				FCF_SET_DEST_ADDR_MODE(FCF_SHORT_ADDR) |
				FCF_SET_SOURCE_ADDR_MODE(FCF_LONG_ADDR) |
				FCF_PAN_ID_COMPRESSION;

		/* Update the length. */
		frame_len = BEAC_REQ_ORPH_NOT_PAYLOAD_LEN +
				2 + /* Add 2 octets for FCS */
				2 + /* 2 octets for short Destination Address */
				2 + /* 2 octets for Destination PAN-Id */
				8 + /* 8 octets for long Source Address */
				3; /* 3 octets DSN and FCF */

		/* Build the command frame id. */
		*frame_ptr = transmit_frame->msg_type = ORPHANNOTIFICATION;

		/* Orphan notification contains long source address. */
		frame_ptr -= 8;
		convert_64_bit_to_byte_array(tal_pib.IeeeAddress, frame_ptr);
	}

	/* Destination address */
	frame_ptr -= 2;
	convert_16_bit_to_byte_array(bc_addr, frame_ptr);

	/* Destination PANid */
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
	transmit_frame->mpdu = frame_ptr;

#if (TAL_TYPE == AT86RF230B)
	/* Transmit data without CSMA-CA and no frame retry. */
	tal_tx_status = tal_tx_frame(transmit_frame, NO_CSMA_NO_IFS, false);
#else
	/* Transmit data with unslotted CSMA-CA and no frame retry. */
	tal_tx_status = tal_tx_frame(transmit_frame, CSMA_UNSLOTTED, false);
#endif

	if (MAC_SUCCESS == tal_tx_status) {
		MAKE_MAC_BUSY();
		return true;
	} else {
		return false;
	}
} /* send_scan_cmd() */

#endif /* ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) ||
        *(MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1)) */

/**
 * @brief Continue scanning after setting of PIB attributes
 *
 * This functions continues scanning once the corresponding PIB
 * attribute change has been completed depending on the status.
 *
 * @param set_status Status of the Request to change the PIB attribute
 */
void scan_set_complete(retval_t set_status)
{
	switch (mac_scan_state) {
#if (MAC_SCAN_ED_REQUEST_CONFIRM == 1)
	case MAC_SCAN_ED:
	{
		if (MAC_SUCCESS == set_status) {
			MAKE_MAC_BUSY();
			tal_ed_start(scan_duration);
		} else {
			/* Channel not supported, continue. */
			scan_curr_channel++;
		}
	}
	break;
#endif /* (MAC_SCAN_ED_REQUEST_CONFIRM == 1) */

#if (MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1)
	case MAC_SCAN_ACTIVE:
		if (MAC_SUCCESS == set_status) {
			/*
			 * The TAL switches ON the transmitter while sending a
			 * beacon request, hence the MAC can call
			 * send_scan_cmd() to send
			 * the beacon directly
			 */

			/* Send an beacon request command */
			if (!send_scan_cmd(true)) {
				uint8_t timer_id = T_Scan_Duration;

				/*
				 * Beacon request could not be transmitted
				 * since there is no buffer available stop
				 * scanning
				 * we call the function usually called by the
				 * scan
				 * duration timer to pretend scanning has
				 * finished
				 */
				mac_t_scan_duration_cb((uint8_t *)&timer_id);
			}
		} else {
			/* Channel not supported, continue. */
			scan_curr_channel++;
		}
		break;
#endif /* (MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) */

#if (MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1)
	case MAC_SCAN_PASSIVE:
	{
		if (MAC_SUCCESS == set_status) {
			uint8_t status = tal_rx_enable(PHY_RX_ON);

			if (PHY_RX_ON == status) {
				retval_t timer_status;
				uint32_t tmr
					= MAC_CALCULATE_SYMBOL_TIME_SCANDURATION(
						scan_duration);

				timer_status = pal_timer_start(T_Scan_Duration,
						TAL_CONVERT_SYMBOLS_TO_US(
						tmr),
						TIMEOUT_RELATIVE,
						(FUNC_PTR)mac_t_scan_duration_cb,
						NULL);
#if (_DEBUG_ > 0)
				Assert(MAC_SUCCESS == timer_status);
#endif
				if (MAC_SUCCESS != timer_status) {
					uint8_t timer_id = T_Scan_Duration;

					/*
					 * Scan duration timer could not be
					 * started so we
					 * call the timer callback function
					 * directly this
					 * will basically shorten scanning
					 * without having
					 * really scanned.
					 */
					mac_t_scan_duration_cb((void *)&timer_id);
				}
			} else {
				/* Did not work, continue. */
				scan_curr_channel++;
				scan_proceed(MLME_SCAN_TYPE_PASSIVE,
						(buffer_t *)mac_conf_buf_ptr);
			}
		} else {
			/* Channel not supported, continue. */
			scan_curr_channel++;
		}
	}
	break;
#endif /* (MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1) */

#if (MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1)
	case MAC_SCAN_ORPHAN:
		if (MAC_SUCCESS == set_status) {
			/* Send an orphan notification command */
			if (!send_scan_cmd(false)) {
				uint8_t timer_id = T_Scan_Duration;

				/*
				 * Orphan notification could not be transmitted
				 * since there
				 * is no buffer available stop scanning
				 * we call the function usually called by the
				 * scan duration
				 * timer to pretend scanning has finished
				 */
				mac_t_scan_duration_cb((uint8_t *)&timer_id);
			}
		} else {
			/* Channel not supported, continue. */
			scan_curr_channel++;
		}
		break;
#endif /* (MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1) */

	default:
		break;
	}
} /* scan_set_complete() */

#if ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) || \
	(MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1))

/**
 * @brief Continue scanning after the completion of frame transmission.
 *
 * This functions continues the corresponding scaning depending on status
 * from the transmission of a beacon request or orphan notification frame.
 *
 * @param status Status of transmission
 */
void mac_scan_send_complete(retval_t status)
{
	retval_t timer_status;

	mac_pib.mac_DSN++;

	if (MAC_SUCCESS == status) {
		uint32_t tmr = 0;

		if (MAC_SCAN_ACTIVE == mac_scan_state) {
			tmr = MAC_CALCULATE_SYMBOL_TIME_SCANDURATION(
					scan_duration);
		} else {
			/*
			 * Since this function is only called in active or
			 * orphan scan state,
			 * this case is handling the orphan scan state.
			 */
			tmr = mac_pib.mac_ResponseWaitTime;
		}

		timer_status = pal_timer_start(T_Scan_Duration,
				TAL_CONVERT_SYMBOLS_TO_US(tmr),
				TIMEOUT_RELATIVE,
				(FUNC_PTR)mac_t_scan_duration_cb,
				NULL);
#if (_DEBUG_ > 0)
		Assert(MAC_SUCCESS == timer_status);
#endif
		if (MAC_SUCCESS != timer_status) {
			uint8_t timer_id = T_Scan_Duration;

			/*
			 * Scan duration timer could not be started, so we call
			 * the timer callback function directly. This will
			 * basically
			 * shorten scanning without having really scanned.
			 */
			mac_t_scan_duration_cb((void *)&timer_id);
		}
	} else {
		/* Did not work, continue. */
		scan_curr_channel++;
		scan_proceed(scan_type, (buffer_t *)mac_conf_buf_ptr);
	}
}

#endif /* ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) ||
        *(MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1)) */

#if ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) || \
	(MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1) || \
	(MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1))

/*
 * @brief MAC scan timer callback
 *
 * @param callback_parameter Callback parameter.
 */
static void mac_t_scan_duration_cb(void *callback_parameter)
{
	/* mac_conf_buf_ptr holds the buffer for scan confirm */
	mlme_scan_conf_t *msc
		= (mlme_scan_conf_t *)BMM_BUFFER_POINTER(
			(buffer_t *)mac_conf_buf_ptr);

	switch (mac_scan_state) {
	case MAC_SCAN_ACTIVE:
	case MAC_SCAN_PASSIVE:
	case MAC_SCAN_ORPHAN:
		msc->UnscannedChannels &= ~(1UL << scan_curr_channel);
		scan_proceed(scan_type, (buffer_t *)mac_conf_buf_ptr);
		break;

	default:
		break;
	}

	callback_parameter = callback_parameter; /* Keep compiler happy. */
}

#endif /* ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) || \
        *  (MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1) || \
        *  (MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1))
        */

/**
 * @brief The MLME-SCAN.request primitive makes a request for a node to
 * start a scan procedure.
 *
 * 802.15.4. Section 7.1.11.1.
 *
 * The MLME-SCAN.request primitive is used to initiate a channel scan over a
 * given list of channels. A device can use a channel scan to measure the
 * energy on the channel, search for the coordinator with which it associated,
 * or search for all coordinators transmitting beacon frames within the
 * POS of the scanning device.
 *
 * The MLME-SCAN.request primitive is generated by the next higher layer and
 * issued to its MLME to initiate a channel scan to search for activity within
 * the POS of the device. This primitive can be used to perform an ED scan to
 * determine channel usage, an active or passive scan to locate beacon frames
 * containing any PAN identifier, or an orphan scan to locate a PAN to which
 * the device is currently associated.
 *
 * ED or active scans can be performed before an FFD starts operation as a
 * PAN coordinator. Active or passive scans can be performed prior to selecting
 * a PAN for association. Orphan scans can be performed to attempt to locate a
 * specific coordinator with which communication has been lost.
 *
 * All devices shall be capable of performing passive scans and orphan scans;
 * ED scans and active scans are optional for an RFD.
 *
 * When the MLME receives the MLME-SCAN.request primitive, it initiates a scan
 * in all channels specified in the ScanChannels parameter. The MLME suspends
 * all beacon transmissions for the duration of the scan. During a scan, the
 * MAC sublayer only accepts frames received over the PHY data service that are
 * relevant to the scan being performed (see 7.5.2.1).
 *
 * An ED scan allows a device to obtain a measure of the peak energy in each
 * requested channel. The ED scan is performed on each channel by the MLMEs
 * repeatedly issuing the PLME-ED.request primitive to the PHY until
 * [aBaseSuperframeDuration * (2n + 1)] symbols, where n is the value of the
 * ScanDuration parameter, have elapsed. The MLME notes the maximum energy
 * measurement and moves on to the next channel in the channel list. A device
 * will be able to store between one and an implementation-specified maximum
 * number of channel ED measurements. The ED scan terminates when the number
 * of channel ED measurements stored equals this implementation-specified
 * maximum or when every channel specified in the channel list has been scanned.
 *
 * An active scan is used by an FFD to locate all coordinators transmitting
 * beacon frames within its POS. The active scan is performed on each channel
 * by the MLMEs first sending a beacon request command (see 7.3.2.4). The MLME
 * then enables the receiver and records the information contained in each
 * received beacon in a PAN descriptor structure (see Table 41 in 7.1.5.1.1).
 * A device will be able to store between one and an implementation-specified
 * maximum number of PAN descriptors. The active scan on a particular channel
 * terminates when the number of PAN descriptors stored equals this
 * implementation-specified maximum or when [aBaseSuperframeDuration*(2n + 1)]
 * symbols, where n is the value of the ScanDuration parameter, have elapsed.
 * If the latter condition has been satisfied, the channel is considered to
 * have been scanned. Where possible, the scan is repeated on each channel and
 * terminates when the number of PAN descriptors stored equals the
 * implementation-specified maximum or when every channel in the set of
 *  available channels has been scanned.
 *
 * A passive scan, like an active scan, is used to locate all coordinators
 * transmitting beacon frames within the POS of the scanning device; the
 * difference is that the passive scan is a receive-only operation and does not
 * transmit the beacon request command. The passive scan is performed on each
 * channel by the MLMEs enabling its receiver and recording the information
 * contained in each received beacon in a PAN descriptor structure
 * (see Table 41 in 7.1.5.1.1). A device will be able to store between one and
 * an implementation-specified maximum number of PAN descriptors. The passive
 * scan on a particular channel terminates when the number of PAN descriptors
 * stored equals this implementation-specified maximum or when
 * [aBaseSuperframeDuration * (2n + 1)] symbols, where n is the value of the
 * ScanDuration parameter, have elapsed. If the latter condition has been
 * satisfied, the channel is considered to have been scanned. Where possible,
 * the scan is repeated on each channel and terminates when the number of PAN
 * descriptors stored equals the implementation-specified maximum or when
 * every channel in the set of available channels has been scanned.
 *
 * An orphan scan is used to locate the coordinator with which the scanning
 * device had previously associated. The orphan scan is performed on each
 * channel by the MLME first sending an orphan notification command
 * (see 7.3.2.3). The MLME then enables its receiver for at most
 * aResponseWaitTime symbols. If the device receives a coordinator realignment
 * command within this time, the MLME will disable its receiver. Otherwise, the
 * scan is repeated on the next channel in the channel list. The scan
 * terminates when the device receives a coordinator realignment command
 * (see 7.3.2.5) or when every channel in the set of available channels has
 * been scanned.
 *
 * The results of an ED scan are recorded in a list of ED values and reported
 * to the next higher layer through the MLME-SCAN.confirm primitive with a
 * status of MAC_SUCCESS. The results of an active or passive scan are recorded
 * in a set of PAN descriptor values and reported to the next higher layer
 * through the MLME-SCAN.confirm primitive. If no beacons were found, the
 * MLME-SCAN.confirm primitive will contain a null set of PAN descriptor
 * values and a status of NO_BEACON. Otherwise, the MLME-SCAN.confirm primitive
 * will contain the set of PANDescriptor values found, a list of unscanned
 * channels, and a status of MAC_SUCCESS.
 *
 * The results of an orphan scan are reported to the next higher layer through
 * the MLME-SCAN.confirm primitive. If successful, the MLME-SCAN.confirm
 * primitive will contain a status of MAC_SUCCESS. If the device did not receive
 * a
 * coordinator realignment command, MLME-SCAN.confirm primitive will contain
 * a status of NO_BEACON. In either case, the PANDescriptorList and
 * EnergyDetectList parameters of the MLMESCAN.confirm primitive are null.
 *
 * If any parameter in the MLME-SCAN.request primitive is not supported or is
 * out of range, the MAC sublayer will issue the MLME-SCAN.confirm primitive
 * with a status of MAC_INVALID_PARAMETER.
 *
 * @param m The MLME_SCAN.request message
 */
void mlme_scan_request(uint8_t *m)
{
	mlme_scan_req_t *msr = (mlme_scan_req_t *)BMM_BUFFER_POINTER(
			(buffer_t *)m);
	mlme_scan_conf_t *msc;
	/* Save the original channel. */
	mac_scan_orig_channel = tal_pib.CurrentChannel;

	/* Save the original channel page. */
	mac_scan_orig_page = tal_pib.CurrentPage;

	/* Save the scan request parameters */
	scan_duration = msr->ScanDuration;
	scan_type = msr->ScanType;
	scan_channels = msr->ScanChannels;
	scan_curr_page = msr->ChannelPage;

	msc = (mlme_scan_conf_t *)msr;

	/*
	 * Store the scan request buffer reused to create the corresponding
	 * scan confirmation
	 */
	mac_conf_buf_ptr = m;

	msc->cmdcode = MLME_SCAN_CONFIRM;
	msc->ScanType = scan_type;
	msc->UnscannedChannels = scan_channels;
	msc->ChannelPage = scan_curr_page;
	msc->ResultListSize = 0;
	msc->scan_result_list[0].ed_value[0] = 0;

	if ((MAC_POLL_IDLE != mac_poll_state) ||
			(MAC_SCAN_IDLE != mac_scan_state)
			) {
		/* Ignore scan request while being in a polling state or
		 * scanning. */
		msc->status = MAC_INVALID_PARAMETER;

		/* Append the scan confirmation message to the MAC-NHLE queue */
		qmm_queue_append(&mac_nhle_q, (buffer_t *)m);

		return;
		/* no break here due to return */
	}

#ifndef REDUCED_PARAM_CHECK

	/*
	 * Check for invalid channels to scan.
	 * This can be either an emtpy scan mask, or a scan mask that contains
	 * invalid channels for this band.
	 */

	/*
	 * Checck also for a scan duration that is lower than
	 * the max. beacon order.
	 */
	if ((0 == scan_channels) ||
			((scan_channels & INVERSE_CHANNEL_MASK) != 0) ||
			(scan_duration > BEACON_NETWORK_MAX_BO)
			) {
		msc->status = MAC_INVALID_PARAMETER;

		/* Append the scan confirm message to the MAC-NHLE queue */
		qmm_queue_append(&mac_nhle_q, (buffer_t *)m);

		return;
	}
#endif  /* REDUCED_PARAM_CHECK */

#if (MAC_SCAN_ED_REQUEST_CONFIRM == 0)

	/*
	 * If ED scan is not supported, the ED scan request
	 * will be rejected before node will be woken up.
	 */
	if (MLME_SCAN_TYPE_ED == scan_type) {
		msc->status = MAC_INVALID_PARAMETER;

		/* Append the scan confirm message to the MAC-NHLE queue */
		qmm_queue_append(&mac_nhle_q, (buffer_t *)m);

		return;
	}
#endif /*  (MAC_SCAN_ED_REQUEST_CONFIRM == 0) */

	/* wake up radio first */
	mac_trx_wakeup();

	mac_awake_scan((buffer_t *)m);
} /* mlme_scan_request() */

#if (MAC_SCAN_ED_REQUEST_CONFIRM == 1)

/**
 * @brief ED scan callback function.
 *
 * This function is a callback function from the TAL after ED scan
 * is performed on a specified channel.
 *
 * @param energy_level Maximum energy on the channel
 */
void tal_ed_end_cb(uint8_t energy_level)
{
	MAKE_MAC_NOT_BUSY();

	mlme_scan_conf_t *msc;

	/*
	 * Scan request buffer is used to generate a scan confirm for the ED
	 * scan
	 * which is stored in mac_conf_buf_ptr.
	 */
	msc = (mlme_scan_conf_t *)BMM_BUFFER_POINTER(
			(buffer_t *)mac_conf_buf_ptr);

	uint8_t n_eds;

	n_eds = msc->ResultListSize;
	msc->scan_result_list[0].ed_value[n_eds] = energy_level;
	msc->ResultListSize++;
	msc->scan_result_list[0].ed_value[n_eds + 1] = 0;

	msc->UnscannedChannels &= ~(1UL << scan_curr_channel);

	/* Continue with next channel */
	scan_proceed(MLME_SCAN_TYPE_ED, (buffer_t *)mac_conf_buf_ptr);
}

#endif /* (MAC_SCAN_ED_REQUEST_CONFIRM == 1) */

/*
 * @brief Clean-up for scanning
 *
 * This is a helper function for clean-up functionality during the end of
 * scanning.
 *
 * @param buffer Pointer to mlme_scan_conf_t structure
 */
static void scan_clean_up(buffer_t *buffer)
{
#if (_DEBUG_ > 0)
	retval_t set_status;
#endif

	/* Append the scan confirm message into the internal event queue */
	qmm_queue_append(&mac_nhle_q, buffer);

	/* Set original channel page and channel. */
	scan_curr_page = mac_scan_orig_page;

#if (_DEBUG_ > 0)
	set_status =
#endif
	set_tal_pib_internal(phyCurrentPage, (void *)&scan_curr_page);
#if (_DEBUG_ > 0)
	Assert(MAC_SUCCESS == set_status);
	set_status = set_status;
#endif

	scan_curr_channel = mac_scan_orig_channel;

#if (_DEBUG_ > 0)
	set_status =
#endif
	set_tal_pib_internal(phyCurrentChannel, (void *)&scan_curr_channel);
#if (_DEBUG_ > 0)
	Assert(MAC_SUCCESS == set_status);
#endif

	mac_scan_state = MAC_SCAN_IDLE;

	/* Set radio to sleep if allowed */
	mac_sleep_trans();
}

#if (MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1)

/**
 * @brief Processing a coordinator realignment command frame during Orphan scan
 *
 * This function processes a coordinator realignment command frame received
 * as a response to the reception of an orphan notification
 * command frame (i.e. while being in the middle of an orphan scan procedure).
 * The PAN ID, coord. short address, logical channel, and the device's new
 * short address will be written to the PIB.
 *
 * @param ind Frame reception buffer
 */
void mac_process_orphan_realign(buffer_t *buf_ptr)
{
	retval_t set_status;

	/* Device received a coordinator realignment during an orphan scan */

	/* Free the buffer used for sending orphan notification command */
	bmm_buffer_free((buffer_t *)mac_scan_cmd_buf_ptr);
	mac_scan_cmd_buf_ptr = NULL;

	/*
	 * Scan confirm with scan type orphan is given to the NHLE using the
	 * scan request buffer, which was stored in mac_conf_buf_ptr.
	 */
	mlme_scan_conf_t *msc
		= (mlme_scan_conf_t *)BMM_BUFFER_POINTER(
			(buffer_t *)mac_conf_buf_ptr);

	msc->cmdcode = MLME_SCAN_CONFIRM;
	msc->status = MAC_SUCCESS;
	msc->ScanType = MLME_SCAN_TYPE_ORPHAN;
	msc->UnscannedChannels = 0;
	msc->ResultListSize = 0;

	/* Append the scan confirmation message to the MAC-NHLE queue */
	qmm_queue_append(&mac_nhle_q, (buffer_t *)mac_conf_buf_ptr);

	mac_scan_state = MAC_SCAN_IDLE;

	/* Set radio to sleep if allowed */
	mac_sleep_trans();

	/*
	 * The buffer in which the coordinator realignment is received is
	 * freed up
	 */
	bmm_buffer_free(buf_ptr);

	/* Set the appropriate PIB entries */
	set_status = set_tal_pib_internal(macPANId,
			(void *)&mac_parse_data.mac_payload_data.coord_realign_data.pan_id);

#if (_DEBUG_ > 0)
	Assert(MAC_SUCCESS == set_status);
#endif

	if (BROADCAST !=
			mac_parse_data.mac_payload_data.coord_realign_data.
			short_addr) {
		/* Short address only to be set if not broadcast address */
		set_status = set_tal_pib_internal(macShortAddress,
				(void *)&mac_parse_data.mac_payload_data.coord_realign_data.short_addr);

#if (_DEBUG_ > 0)
		Assert(MAC_SUCCESS == set_status);
#endif
	}

	mac_pib.mac_CoordShortAddress
		= mac_parse_data.mac_payload_data.coord_realign_data.
			coord_short_addr;

	/*
	 * If frame version subfield indicates a 802.15.4-2006 compatible frame,
	 * the channel page is appended as additional information element.
	 */
	if (mac_parse_data.fcf & FCF_FRAME_VERSION_2006) {
		set_status
			= set_tal_pib_internal(phyCurrentPage,
				(void *)&mac_parse_data.mac_payload_data.coord_realign_data.channel_page);
#if (_DEBUG_ > 0)
		Assert(MAC_SUCCESS == set_status);
#endif
	}

	set_status = set_tal_pib_internal(phyCurrentChannel,
			(void *)&mac_parse_data.mac_payload_data.coord_realign_data.logical_channel);

#if (_DEBUG_ > 0)
	Assert(MAC_SUCCESS == set_status);
#endif

	scan_set_complete(set_status);
} /* mac_process_orphan_realign() */

#endif /* (MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1) */

#endif /* (MAC_SCAN_SUPPORT == 1) */

/* EOF */
