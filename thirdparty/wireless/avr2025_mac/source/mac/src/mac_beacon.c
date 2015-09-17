/**
 *
 * @file mac_beacon.c
 *
 * @brief
 * Implements the building of beacon frames and initiates transmission via
 * CSMA-CA after reception of a beacon request frame in a nonbeacon-enabled PAN.
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

/* === Includes ============================================================= */
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

/* === Macros =============================================================== */

/*
 * Time in (advance) symbols before beacon interval when beacon is prepared
 */
#define ADVNC_BCN_PREP_TIME                 (50)

/* Minimum Wakeup time for beacon synchronization when handling Wakeup from
 * application sleep*/
#define MAC_MIN_WAKEUP_US                   (200)

/*
 * (Minimal) Beacon payload length
 * 2 octets Superframe Spec
 * At least 1 octet GTS fields
 * At least 1 octet Pending Address fields
 */
#define BEACON_PAYLOAD_LEN                  (4)

/*
 * Maximum number of pending extended and/short addresses to be added to
 * Beacon frame indicating pending data.
 */
#define BEACON_MAX_PEND_ADDR_CNT            (7)

/* === Globals ============================================================== */

#ifdef MAC_SECURITY_BEACON
extern mlme_start_req_t msr_params;    /* Intermediate start parameters */
#endif

#if (MAC_START_REQUEST_CONFIRM == 1)

#ifdef BEACON_SUPPORT

/*
 * Static buffer used for beacon transmission in a BEACON build.
 * In a build without beacon support, in order to save the static buffer,
 * a new buffer will be allocated to transmit the beacon frame.
 */
static uint8_t beacon_buffer[LARGE_BUFFER_SIZE];
#endif  /* BEACON_SUPPORT */

#if (MAC_INDIRECT_DATA_FFD == 1)
/* Pointer used for adding pending addresses to the beacon frame. */
static uint8_t *beacon_ptr;

/* Variable to hold number the pending addresses. */
static uint8_t pending_address_count;

#endif  /* (MAC_INDIRECT_DATA_FFD == 1) */

#ifdef TEST_HARNESS
static uint8_t vpan_no;
#endif  /* TEST_HARNESS */

/* === Prototypes =========================================================== */

#if (MAC_INDIRECT_DATA_BASIC == 1)
static uint8_t add_pending_extended_address_cb(void *buf_ptr, void *handle);
static uint8_t add_pending_short_address_cb(void *buf_ptr, void *handle);
static uint8_t mac_buffer_add_pending(uint8_t *buf_ptr);

#endif

#ifdef BEACON_SUPPORT
static void mac_t_beacon_cb(void *callback_parameter);
static void mac_t_prepare_beacon_cb(void *callback_parameter);

/* TODO */

static void mac_t_superframe_cb(void *callback_parameter);

#endif  /* BEACON_SUPPORT */

/* === Implementation ======================================================= */

#if (MAC_INDIRECT_DATA_FFD == 1)

/*
 * @brief Populates the beacon frame with pending addresses
 *
 * This function populates the beacon frame with pending addresses by
 * traversing through the indirect queue.
 *
 * @param buf_ptr Pointer to the location in the beacon frame buffer where the
 * pending addresses are to be updated
 *
 * @return Number of bytes added in the beacon frame as a result of pending
 * address
 */
static uint8_t mac_buffer_add_pending(uint8_t *buf_ptr)
{
	search_t find_buf;
	uint8_t number_of_ext_address = 0;

	/*
	 * Set pointer to beacon to proper place. Currently it is still
	 * pointing at the first octet of the beacon payload (if there
	 * is any included.
	 * The beacon_ptr pointer will be updated (i.e. decreased) in
	 * the functions add_pending_short_address_cb() &
	 * add_pending_extended_address_cb() according to the included
	 * octets containing the pending addresses.
	 *
	 * Note: Since the pending addresses is filled from the back,
	 * the extended are filled in first.
	 */
	beacon_ptr = buf_ptr;

	/* Initialize extended address count. */
	pending_address_count = 0;

	/*
	 * This callback function traverses through the indirect queue and
	 * updates the beacon buffer with the pending extended addresses.
	 */
	find_buf.criteria_func = add_pending_extended_address_cb;

	/*
	 * At the end of this function call (qmm_queue_read), the beacon buffer
	 * will be updated with the short address (if any) of the indirect
	 * data (if any) present in the indirect queue.
	 */
	qmm_queue_read(&indirect_data_q, &find_buf);

	/*
	 * The count of extended addresses added in the beacon frame is backed
	 * up
	 * (as the same variable will be used to count the number of added
	 * short addresses).
	 */
	number_of_ext_address = pending_address_count;

	/* Initialize extended address count. */
	pending_address_count = 0;

	/*
	 * This callback function traverses through the indirect queue and
	 * updates the beacon buffer with the pending short addresses.
	 */
	find_buf.criteria_func = add_pending_short_address_cb;

	/*
	 * At the end of this function call (qmm_queue_read), the beacon buffer
	 * will be updated with the extended address (if any) of the indirect
	 * data (if any) present in the indirect queue.
	 */
	qmm_queue_read(&indirect_data_q, &find_buf);

	/*
	 * Update buf_ptr to current position of beginning of
	 * pending address specifications filled above.
	 * Fill in Pending Address Specification (see IEEE 802.15.4-2006 Table
	 * 46).
	 * In order to this buf_ptr needs to be decremented.
	 */
	buf_ptr = beacon_ptr - 1;
	*buf_ptr = (pending_address_count) | (number_of_ext_address  << 4);

	/*
	 * Total number of bytes used for pending address in beacon frame.
	 * Note: The length of the one octet for the Pending Address
	 * Specification
	 * is already included in the default beacon frame length
	 * (see BEACON_PAYLOAD_LEN).
	 */
	pending_address_count = (pending_address_count * sizeof(uint16_t)) +
			(number_of_ext_address * sizeof(uint64_t));

	return pending_address_count;
} /* mac_buffer_add_pending() */

#endif /* (MAC_INDIRECT_DATA_FFD == 1) */

/**
 * @brief Builds and transmits the beacon frame
 *
 * This function is called to build a beacon frame. For beaconless network
 * this function also transmits the generated beacon frame.
 *
 * @param beacon_enabled Flag indicating the mode of beacon transmission
 * @param beacon_buffer_header For build without beacon support only:
 *                             Pointer to buffer of beacon frame to be
 *                             transmitted in nonbeacon network.
 **/
#ifdef BEACON_SUPPORT
void mac_build_and_tx_beacon(bool beacon_enabled)
#else   /* No BEACON_SUPPORT */
void mac_build_and_tx_beacon(bool beacon_enabled,
		buffer_t *beacon_buffer_header)
#endif  /* BEACON_SUPPORT / No BEACON_SUPPORT */
{
	frame_info_t *transmit_frame;
	uint16_t superframe_spec = 0;
	uint16_t fcf;
	uint8_t frame_len;
	uint8_t *frame_ptr;
#if ((defined MAC_SECURITY_BEACON)  || (defined MAC_SECURITY_2006_BEACON))
	uint8_t *frame_ptr_mhr_gts = NULL;
	uint8_t *mac_payload_ptr = NULL;
	mcps_data_req_t beacon_sec_buf;
#endif

#ifdef BEACON_SUPPORT

	/*
	 * The frame is given to the TAL in the 'frame_info_t' format,
	 * hence an instance of the frame_info_t is created.
	 */
	transmit_frame = (frame_info_t *)beacon_buffer;

	/* Buffer header not required in BEACON build. */
	transmit_frame->buffer_header = NULL;
#else   /* No BEACON_SUPPORT */
	uint8_t *beacon_buffer = (uint8_t *)BMM_BUFFER_POINTER(
			beacon_buffer_header);

	/*
	 * The frame is given to the TAL in the 'frame_info_t' format,
	 * hence an instance of the frame_info_t is created.
	 */
	transmit_frame = (frame_info_t *)beacon_buffer;

	/* Store buffer header to be able to release it later properly. */
	transmit_frame->buffer_header = beacon_buffer_header;
#endif  /* BEACON_SUPPORT / No BEACON_SUPPORT */

	/* MAC transmit frame type */
	transmit_frame->msg_type = BEACON_MESSAGE;

	/* Update the payload length. */
	frame_len = BEACON_PAYLOAD_LEN +
			2 + /* Add 2 octets for FCS */
			2 + /* 2 octets for short Source Address */
			2 + /* 2 octets for short Source PAN-Id */
			3; /* 3 octets DSN and FCF */

	/* Get the payload pointer. */
	frame_ptr = (uint8_t *)transmit_frame +
			LARGE_BUFFER_SIZE - 2; /* Add 2 octets for FCS. */

	/* Build the beacon payload if it exists. */
	if (mac_pib.mac_BeaconPayloadLength > 0) {
		frame_ptr -= mac_pib.mac_BeaconPayloadLength;
		frame_len += mac_pib.mac_BeaconPayloadLength;

		memcpy(frame_ptr, mac_beacon_payload,
				mac_pib.mac_BeaconPayloadLength);
#if ((defined MAC_SECURITY_BEACON)  || (defined MAC_SECURITY_2006_BEACON))
		mac_payload_ptr = frame_ptr;
#endif
	}

	/* Build the Pending address field. */
#if (MAC_INDIRECT_DATA_FFD == 1)
	{
		/*
		 * Check if the indirect queue has entries, otherwise there is
		 * nothing
		 * to add as far as pending addresses is concerned.
		 */
		if (indirect_data_q.size > 0) {
			uint8_t pending_addr_octets = mac_buffer_add_pending(
					frame_ptr);
			frame_len += pending_addr_octets;
			frame_ptr -= pending_addr_octets + 1;
		} else {
			/* No pending data available. */
			frame_ptr--;
			*frame_ptr = 0;
		}
	}
#else

	/*
	 * If indirect transmission is not enabled, the Pending Address Spec
	 * field is always 0.
	 */
	frame_ptr--;
	*frame_ptr = 0;
#endif

	/* frame_ptr now points to the GTS Specification .
	**/
#ifdef GTS_SUPPORT
	mac_gts_table_update();
	uint8_t gts_octets = mac_add_gts_info(frame_ptr);
	if (gts_octets > 0) {
		frame_len += gts_octets;
		frame_ptr -= gts_octets + 1;
	} else {
		frame_ptr--;
	}

#else
	frame_ptr--;
	*frame_ptr = 0;
#endif /* GTS_SUPPORT */

	/* The superframe specification field is updated. */
#ifdef BEACON_SUPPORT
	superframe_spec = tal_pib.BeaconOrder;
	superframe_spec |= (tal_pib.SuperFrameOrder << 4);
	superframe_spec |= (mac_final_cap_slot << 8);

	if (tal_pib.BattLifeExt) {
		superframe_spec |= (1U << BATT_LIFE_EXT_BIT_POS);
	}

#else
	superframe_spec = NON_BEACON_NWK;
	superframe_spec |= (NON_BEACON_NWK << 4);
	superframe_spec |= (FINAL_CAP_SLOT_DEFAULT << 8);
#endif

	if (MAC_PAN_COORD_STARTED == mac_state) {
		superframe_spec |= (1U << PAN_COORD_BIT_POS);
	}

#if (MAC_ASSOCIATION_INDICATION_RESPONSE == 1)
	if (mac_pib.mac_AssociationPermit) {
		superframe_spec |= (1U << ASSOC_PERMIT_BIT_POS);
	}
#endif

	/* Set the Superframe Specification Field. */
	frame_ptr -= 2;
	convert_spec_16_bit_to_byte_array(superframe_spec, frame_ptr);

#if ((defined MAC_SECURITY_BEACON)  || (defined MAC_SECURITY_2006_BEACON))
	frame_ptr_mhr_gts = frame_ptr;

	/* Copy the security info into the beacon security structure buffer */
	beacon_sec_buf.SecurityLevel = msr_params.BeaconSecurityLevel;
	beacon_sec_buf.KeyIdMode = msr_params.BeaconKeyIdMode;
	beacon_sec_buf.KeySource = msr_params.BeaconKeySource;
	beacon_sec_buf.KeyIndex = msr_params.BeaconKeyIndex;
	beacon_sec_buf.msduLength = mac_pib.mac_BeaconPayloadLength;

	/*
	 * Note: The value of the payload_length parameter will be updated
	 *       if security needs to be applied.
	 */
	if (beacon_sec_buf.SecurityLevel > 0) {
		if (MAC_SUCCESS !=
				mac_build_aux_sec_header(&frame_ptr,
				&beacon_sec_buf, &frame_len)) {
			/* Todo MAC Security Issue */
			return;
		}

		/* place the GTS  and Super frame specification fields into the
		 * before the MIC - Data */
		if ((beacon_sec_buf.SecurityLevel == 1) ||
				(beacon_sec_buf.SecurityLevel == 5)) {
			memmove((frame_ptr_mhr_gts - 0x04), frame_ptr_mhr_gts, \
					(mac_payload_ptr - frame_ptr_mhr_gts));
		} else if ((beacon_sec_buf.SecurityLevel == 2) ||
				(beacon_sec_buf.SecurityLevel == 6)) {
			memmove((frame_ptr_mhr_gts - 0x08), frame_ptr_mhr_gts, \
					(mac_payload_ptr - frame_ptr_mhr_gts));
		} else if ((beacon_sec_buf.SecurityLevel == 3) ||
				(beacon_sec_buf.SecurityLevel == 7)) {
			memmove((frame_ptr_mhr_gts - 0x10), frame_ptr_mhr_gts, \
					(mac_payload_ptr - frame_ptr_mhr_gts));
		}
	}
#endif  /* (MAC_SECURITY_BEACON || MAC_SECURITY_2006_BEACON) */

	/*
	 * Source address.
	 */
	if (CCPU_ENDIAN_TO_LE16(MAC_NO_SHORT_ADDR_VALUE) ==
			tal_pib.ShortAddress) {
		frame_ptr -= 8;
		frame_len += 6; /* Add further 6 octets for long Source Address
		                **/
		convert_64_bit_to_byte_array(tal_pib.IeeeAddress, frame_ptr);
#if ((defined MAC_SECURITY_BEACON)  || (defined MAC_SECURITY_2006_BEACON))
		beacon_sec_buf.SrcAddrMode = FCF_LONG_ADDR;
#endif

		fcf = FCF_SET_SOURCE_ADDR_MODE((uint16_t)FCF_LONG_ADDR);
	} else {
		frame_ptr -= 2;
		convert_16_bit_to_byte_array(tal_pib.ShortAddress, frame_ptr);
#if ((defined MAC_SECURITY_BEACON)  || (defined MAC_SECURITY_2006_BEACON))
		beacon_sec_buf.SrcAddrMode = FCF_SHORT_ADDR;
#endif
		fcf = FCF_SET_SOURCE_ADDR_MODE((uint16_t)FCF_SHORT_ADDR);
	}

	/* Source PAN-Id */
	frame_ptr -= 2;
	convert_16_bit_to_byte_array(tal_pib.PANId, frame_ptr);

#if ((defined MAC_SECURITY_BEACON)  || (defined MAC_SECURITY_2006_BEACON))
	beacon_sec_buf.DstPANId = tal_pib.PANId;
#endif

#ifdef TEST_HARNESS
	if (mac_pib.privateVirtualPANs > 0) {
		uint16_t temp = CCPU_ENDIAN_TO_LE16(tal_pib.PANId);

		/*
		 * This changes the PAN-ID of subsequent beacon frames to
		 * simulate
		 * virtual PANs for testing purposes.
		 */
		temp += vpan_no;
		convert_16_bit_to_byte_array(CCPU_ENDIAN_TO_LE16(
				temp), frame_ptr);
		vpan_no++;
		vpan_no = vpan_no % mac_pib.privateVirtualPANs;
	}
#endif /* TEST_HARNESS */

	/* Set BSN. */
	frame_ptr--;
	*frame_ptr = mac_pib.mac_BSN++;

	fcf = fcf | FCF_SET_FRAMETYPE(FCF_FRAMETYPE_BEACON);

#if ((defined MAC_SECURITY_BEACON)  || (defined MAC_SECURITY_2006_BEACON))
	if (beacon_sec_buf.SecurityLevel > 0) {
		fcf |= FCF_SECURITY_ENABLED | FCF_FRAME_VERSION_2006;
	}
#endif

#if (MAC_START_REQUEST_CONFIRM == 1)
#ifdef BEACON_SUPPORT

	/*
	 * In case
	 * 1) the node is a PAN Coordinator or Coordinator, and
	 * 2) this is beacon-enabled network, and
	 * 3) there is a broadcast frame to be transmitted,
	 * the frame pending bit in the frame control field of the beacon frame
	 * to be transmitted needs to be set in order to indicate this to all
	 * listening children nodes.
	 */
	if (((MAC_PAN_COORD_STARTED == mac_state) ||
			(MAC_COORDINATOR == mac_state)) &&
			(tal_pib.BeaconOrder < NON_BEACON_NWK) &&
			(broadcast_q.size > 0)
			) {
		fcf |= FCF_FRAME_PENDING;
	}
#endif  /* BEACON_SUPPORT */
#endif /* (MAC_START_REQUEST_CONFIRM == 1) */

	/* Set the FCF. */
	frame_ptr -= 2;
	convert_spec_16_bit_to_byte_array(fcf, frame_ptr);

	/* First element shall be length of PHY frame. */
	frame_ptr--;
	*frame_ptr = frame_len;

	/* Finished building of frame. */
	transmit_frame->mpdu = frame_ptr;

#ifdef BEACON_SUPPORT
#if ((defined MAC_SECURITY_BEACON)  || (defined MAC_SECURITY_2006_BEACON))
	if (beacon_sec_buf.SecurityLevel > 0) {
		retval_t build_sec = mac_secure(transmit_frame, mac_payload_ptr,
				&beacon_sec_buf);
		if (MAC_SUCCESS != build_sec) {
			return;
		}
	}
#endif
	if (!beacon_enabled) {
		/* Buffer header not required in BEACON build. */
		transmit_frame->buffer_header = NULL;

		/*
		 * In a beaconless network the beacon is transmitted with
		 * unslotted CSMA-CA.
		 */
		tal_tx_frame(transmit_frame, CSMA_UNSLOTTED, false);

		MAKE_MAC_BUSY();
	}

	/*
	 * In case beaconing mode is enabled, the beacon will be transmitted
	 * using function tal_tx_beacon once the beacon timer has expired.
	 * Therefore there is nothing to be done here.
	 */
#else   /* No Beacon support */

	/*
	 * In a beaconless network the beacon is transmitted with
	 * unslotted CSMA-CA.
	 */
	tal_tx_frame(transmit_frame, CSMA_UNSLOTTED, false);

	MAKE_MAC_BUSY();

	beacon_enabled = beacon_enabled; /* Keep compiler happy. */
#endif  /* BEACON_SUPPORT */
} /* mac_build_and_tx_beacon() */

#if (MAC_INDIRECT_DATA_BASIC == 1)

/*
 * @brief Appends the pending short addresses into the beacon frame
 *
 * This function appends the pending short addresses to the beacon based
 * on frames currently in the indirect queue.
 *
 * @param buf_ptr Pointer to the indirect data in the indirect queue
 * @param handle Callback parameter
 *
 * @return 0 to traverse through the full indirect queue
 *
 */
static uint8_t add_pending_short_address_cb(void *buf_ptr, void *handle)
{
	frame_info_t *frame = (frame_info_t *)buf_ptr;

	/* Only 7 short addresses are allowed in one Beacon frame. */
	if (pending_address_count < BEACON_MAX_PEND_ADDR_CNT) {
		/*
		 * Only if the destination addressing mode is short address mode
		 * then the
		 * indirect data is used to populate the beacon buffer with
		 * short
		 * destination address.
		 */
		if (FCF_SHORT_ADDR ==
				((frame->mpdu[PL_POS_FCF_2] >>
				FCF_2_DEST_ADDR_OFFSET) & FCF_ADDR_MASK)) {
			beacon_ptr -= sizeof(uint16_t);
			memcpy(beacon_ptr, &frame->mpdu[PL_POS_DST_ADDR_START],
					sizeof(uint16_t));
			pending_address_count++;
		}
	}

	handle = handle; /* Keep compiler happy. */

	return 0;
}

#endif /* (MAC_INDIRECT_DATA_BASIC == 1) */

#if (MAC_INDIRECT_DATA_BASIC == 1)

/*
 * @brief Appends the pending extended address into beacon
 *
 * This function appends pending extended addresses in the indirect queue
 * to the beacon.
 *
 * @param buf_ptr Pointer to the indirect data in the indirect queue
 * @param handle Callback parameter
 *
 * @return 0 to traverse through the full indirect queue
 *
 */
static uint8_t add_pending_extended_address_cb(void *buf_ptr, void *handle)
{
	frame_info_t *frame = (frame_info_t *)buf_ptr;

	/* Only 7 extended addresses are allowed in one Beacon frame. */
	if (pending_address_count < BEACON_MAX_PEND_ADDR_CNT) {
		/*
		 * Only if the destination addressing mode is extended address
		 * mode then the
		 * indirect data is used to populate the beacon buffer with
		 * extended
		 * destination address.
		 */
		if (FCF_LONG_ADDR ==
				((frame->mpdu[PL_POS_FCF_2] >>
				FCF_2_DEST_ADDR_OFFSET) & FCF_ADDR_MASK)) {
			beacon_ptr -= sizeof(uint64_t);
			memcpy(beacon_ptr, &frame->mpdu[PL_POS_DST_ADDR_START],
					sizeof(uint64_t));
			pending_address_count++;
		}
	}

	handle = handle; /* Keep compiler happy. */

	return 0;
}

#endif /* (MAC_INDIRECT_DATA_BASIC == 1) */

/**
 * @brief Processes a beacon request
 *
 *  This function is called in case a beacon request frame has been received by
 *  a coordinator. In a nonbeacon-enabled PAN the generation of a beacon frame
 *  using CSMA-CA is initiated. In a beacon-enabled PAN no extra beacon frame
 *  will be transmitted apart from the standard beacon frames.
 *
 * @param msg Pointer to the buffer in which the beaocn request was received
 */
void mac_process_beacon_request(buffer_t *msg)
{
#ifdef BEACON_SUPPORT

	/*
	 * The buffer in which the beacon request was received is freed up.
	 * This is only done in a BEACON build, since a static buffer is used
	 * to transmit the beacon frame.
	 */
	bmm_buffer_free((buffer_t *)msg);

	/*
	 * If the network is a beacon enabled network then the beacons will not
	 * be
	 * transmitted.
	 */
	if (tal_pib.BeaconOrder == NON_BEACON_NWK) {
		/* The beacon is transmitted using CSMA-CA. */
		mac_build_and_tx_beacon(false);
	}

#else   /* No BEACON_SUPPORT */
	mac_build_and_tx_beacon(false, (buffer_t *)msg);
#endif  /* BEACON_SUPPORT / No BEACON_SUPPORT */
}

#ifdef BEACON_SUPPORT

/**
 * @brief Starts the beacon timer
 *
 * This function is called during MLME_START.request operation to start the
 * timers for beacon transmission in case the network is of type beacon enabled,
 * or to start the timer to count the transcation presistence time of indirect
 * data in case the network is nonbeacon enabled.
 *
 * The timers started for a beacon enabled and nonbeacon enabled networks are
 * different with different timeout values.
 *
 * In case of a beacon enabled network two timers are started. The first timer
 * prepares the beacon and second timer sends the beacon. In case of the
 * nonbeacon enabled network, only a single timer is started with timeout as
 * the beacon interval.
 *
 * For a beacon enabled network the first beacon of the network is prepared and
 * sent in this function and the subsequent beacons are sent in the callback
 * function of the timer.
 */
void mac_start_beacon_timer(void)
{
	uint32_t beacon_tx_time;
	uint32_t beacon_int_symbols;

	/*
	 * This is a beacon enabled network.
	 * The First beacon in a beacon enabled network is transmitted
	 * directly without CSMA-CA.
	 * This function call, to transmit the beacon frame without
	 * CSMA-CA (direct), will cause it to store the beacon at the TAL.
	 */
	mac_build_and_tx_beacon(true);

	/*
	 * For the first beacon the current time is used as beacon transmission
	 * time. For consecutive beacon transmissions, this beacon transmission
	 * time is added to the beacon interval and then used.
	 */
	pal_get_current_time(&beacon_tx_time);

	/*
	 * Timer gives the time in microseconds, but the PIB value at the TAL is
	 * in
	 * symbols. Hence a coversion is done before setting the PIB at the TAL.
	 */
	beacon_tx_time = TAL_CONVERT_US_TO_SYMBOLS(beacon_tx_time);

	/* Now it gets tricky:
	 * Since teh PIB attribute macBeaconTxTime is supposed to hold the time
	 * of the last transmitted beacon frame, but we are going to transmit
	 * our first beacon frame just now (in teh subsequentially called
	 * function
	 * mac_t_beacon_cb(), we need to actually substract the time of one
	 * entire
	 * beacon period from this time.
	 * Otherwise we would leave out the second beacon in function
	 * mac_t_beacon_cb().
	 */
	beacon_int_symbols = TAL_GET_BEACON_INTERVAL_TIME(tal_pib.BeaconOrder);

	beacon_tx_time
		= tal_sub_time_symbols(beacon_tx_time,
			beacon_int_symbols);

	set_tal_pib_internal(macBeaconTxTime, (void *)&beacon_tx_time);

	/*
	 * Now start the handling of all relevant beacon timers:
	 * 1) T_Beacon: The regular beacon timer
	 * 2) T_Beacon_Preparation: The Pre-beacon timer is used to give the
	 * node
	 *    sufficient time prior to the actual beacon timer in order to
	 * prepare
	 *    the next beacon frame or wake-up before the next beacon reception.
	 * 3) T_Superframe: The timer when the inactive portion of the
	 * superframe
	 *    starts; if there is not inactive portion, this timer is NOT
	 * running.
	 *
	 * This is simply done by calling the same function that will later be
	 * called upon expiration of the main beacon timer and will handle alle
	 * other timers as well to have a common time base for beacon related
	 * timers.
	 */
	/* This will also actually transmit the beacon. */
	mac_t_beacon_cb(NULL);
} /* mac_start_beacon_timer() */

#endif /* BEACON_SUPPORT */

#ifdef BEACON_SUPPORT

/*
 * @brief Callback function of the beacon preparation time timer
 *
 * This is the calback function of the timer started with timeout
 * value equal to ADVNC_BCN_PREP_TIME symbols less than beacon interval.
 *
 * In case of the beacon enabled network the beacon frame is created and
 * stored at the TAL. After preparing the beacon, the timer is restarted for
 * the next beacon interval less the beacon preparation time.
 *
 * @param callback_parameter Callback parameter
 */
static void mac_t_prepare_beacon_cb(void *callback_parameter)
{
	mac_trx_wakeup();
	/* For a beacon enabled network, the beacon is stored at the TAL. */

	mac_build_and_tx_beacon(true);

	callback_parameter = callback_parameter; /* Keep compiler happy. */
} /* mac_t_prepare_beacon_cb() */

#endif  /* BEACON_SUPPORT */

#ifdef BEACON_SUPPORT

/*
 * @brief Transmits beacon frame after beacon interval
 *
 * This function is a callback function of the timer started with the timeout
 * value as the beacon interval time. The TAL function for transmitting
 * the beacon is called and the timer for sending the next beacon is started.
 *
 * @param callback_parameter Callback parameter
 */
static void mac_t_beacon_cb(void *callback_parameter)
{
	/*
	 * This check here is done in order to stop beacon timing in case
	 * the network has transitioned from a beacon-enabled network to
	 * nonbeacon-enabled network.
	 */
	/* Wake up radio first */

	if (tal_pib.BeaconOrder < NON_BEACON_NWK) {
		/*
		 * In case the node is currently scanning, no beacon will be
		 * transmitted.
		 */
		if (MAC_SCAN_IDLE == mac_scan_state) {
			/*
			 * The frame is given to the TAL in the 'frame_info_t'
			 * format,
			 * hence an instance of the frame_info_t is created.
			 */
			frame_info_t *transmit_frame
				= (frame_info_t *)beacon_buffer;

			/* This TAL function transmits the beacon fame. */
			tal_tx_beacon(transmit_frame);
		}

		uint32_t beacon_int_us;
		uint32_t beacon_tx_time_symbols;
		uint32_t beacon_tx_time_us;
		uint32_t next_beacon_tx_time;
		retval_t status = FAILURE;

		/* 1) Start with main beacon timer. */
		beacon_int_us
			= TAL_CONVERT_SYMBOLS_TO_US(
				TAL_GET_BEACON_INTERVAL_TIME(
				tal_pib.BeaconOrder));

		/* This was the time when when transmitted the previous beacon
		 * frame. */
		beacon_tx_time_us = TAL_CONVERT_SYMBOLS_TO_US(
				tal_pib.BeaconTxTime);

		/*
		 * This is supposed to be the time when we just had transmitted
		 * this
		 * beacon frame (see calling of tal_tx_beacon() above).
		 */
		next_beacon_tx_time = pal_add_time_us(beacon_tx_time_us,
				beacon_int_us);

		/*
		 * In order to get the proper timeout value for the next beacon
		 * timer,
		 * we have to add one more beacon interval time.
		 * If the timer cannot be started, then we add more beacon
		 * intervals,
		 * until we finally succeed.
		 */
		while (MAC_SUCCESS != status) {
			next_beacon_tx_time = pal_add_time_us(
					next_beacon_tx_time, beacon_int_us);

			status = pal_timer_start(T_Beacon,
					next_beacon_tx_time,
					TIMEOUT_ABSOLUTE,
					(FUNC_PTR)mac_t_beacon_cb,
					NULL);
		}

		mac_superframe_state = MAC_ACTIVE_CAP;

		#ifdef GTS_SUPPORT
		pal_timer_stop(T_CAP);
		#ifdef GTS_DEBUG
		port_pin_toggle_output_level(DEBUG_PIN1);
		port_pin_set_output_level(DEBUG_PIN4, 0);
		port_pin_set_output_level(DEBUG_PIN5, 0);
		port_pin_set_output_level(DEBUG_PIN6, 0);
		port_pin_set_output_level(DEBUG_PIN7, 0);
		#endif
		#endif /* GTS_SUPPORT */

		/*
		 * Even if this may look odd, since we already had added a
		 * beacon
		 * interval time to this variable before the while loop above,
		 * we need to manually substract one beacon interval time,
		 * in order to keep pace with the new value of macBeaconTxTime
		 * just in case several calls of pal_add_time_us() had to be
		 * done
		 * in the while loop before the timer could be started.
		 */
		beacon_tx_time_us = pal_sub_time_us(next_beacon_tx_time,
				beacon_int_us);

		beacon_tx_time_symbols
			= TAL_CONVERT_US_TO_SYMBOLS(beacon_tx_time_us);

		/* The beacon transmission time is updated at TAL. */
		set_tal_pib_internal(macBeaconTxTime,
				(void *)&beacon_tx_time_symbols);

		{
			/* 2) Beacon preparation timer for building the new
			 * beacon frame. */
			uint32_t next_beacon_prep_time;

			/*
			 * A regular but absolute timer is started which will
			 * expire
			 * ADVNC_BCN_PREP_TIME symbols prior to beacon interval.
			 * On its expiry
			 * the beacon frame is created and given to TAL.
			 * The absolute time at which the beacon is to be
			 * prepared is calculated by
			 * subtracting the beacon preparation time from the
			 * absolute time value of
			 * the beacon transmission.
			 */
			next_beacon_prep_time = pal_sub_time_us(
					next_beacon_tx_time,
					TAL_CONVERT_SYMBOLS_TO_US(
					ADVNC_BCN_PREP_TIME));

			/* This is the timer started for preparing the beacon.
			**/
			status = FAILURE;

			while (MAC_SUCCESS != status) {
				status = pal_timer_start(T_Beacon_Preparation,
						next_beacon_prep_time,
						TIMEOUT_ABSOLUTE,
						(FUNC_PTR)mac_t_prepare_beacon_cb,
						NULL);

				next_beacon_prep_time = pal_add_time_us(
						next_beacon_prep_time,
						beacon_int_us);
			}
		}

		/* 3) Superframe timer for determining end of active portion. */
		if (tal_pib.SuperFrameOrder < tal_pib.BeaconOrder) {
			pal_timer_start(T_Superframe,
					TAL_CONVERT_SYMBOLS_TO_US(
					TAL_GET_SUPERFRAME_DURATION_TIME(
					tal_pib.SuperFrameOrder)),
					TIMEOUT_RELATIVE,
					(FUNC_PTR)mac_t_superframe_cb,
					NULL);
		    #ifdef GTS_DEBUG
			port_pin_set_output_level(DEBUG_PIN2, 1);
			#endif
		}

#ifdef GTS_SUPPORT
		if (mac_final_cap_slot < FINAL_CAP_SLOT_DEFAULT) {
			uint32_t cap_end_duration = (TAL_CONVERT_SYMBOLS_TO_US(
					TAL_GET_SUPERFRAME_DURATION_TIME(tal_pib
					.SuperFrameOrder)) >>
					4) * (mac_final_cap_slot + 1);

			pal_timer_start(T_CAP, cap_end_duration,
					TIMEOUT_RELATIVE,
					(FUNC_PTR)mac_t_gts_cb,
					NULL);
			#ifdef GTS_DEBUG
			port_pin_set_output_level(DEBUG_PIN3, 1);
			#endif
		}
#endif /* GTS_SUPPORT */

		/*
		 * Once the timing calculation for the next beacon has been
		 * finished,
		 * a pending broadcast frame will be transmitted.
		 * Of course this is only done if the node is not scanning.
		 */
		if (MAC_SCAN_IDLE == mac_scan_state) {
			/*
			 * Check for pending broadcast data frames in the
			 * broadcast queue
			 * and transmit exactly one broadcast data frame in case
			 * there
			 * are pending broadcast frames.
			 */
			if (broadcast_q.size > 0) {
				mac_tx_pending_bc_data();
			}
		}
	} /* (tal_pib.BeaconOrder < NON_BEACON_NWK) */

	callback_parameter = callback_parameter; /* Keep compiler happy. */
} /* mac_t_beacon_cb() */

#endif  /* BEACON_SUPPORT */

#ifdef BEACON_SUPPORT

/*
 * @brief Handles the superframe timer expiry
 *
 * This function is the callback function of the superframe timer.
 * This function is called once the inactive portion of a superframe
 * is entered.
 *
 * @param callback_parameter Callback parameter
 */
/* TODO */
static void mac_t_superframe_cb(void *callback_parameter)
{
	/*     * Go to sleep (independent of the value of macRxOnWhenIdle) */
	/*     * because we enter the inactive portion now. */

	/*     * Note: Do not use mac_sleep_trans() here, because this would
	 * check */
	/*     * macRxOnWhenIdle first. */
	/*     * / */
	/*    */
	/*  */
	mac_sleep_trans();

	/*    callback_parameter = callback_parameter;  / * Keep compiler happy.
	** / */
    #ifdef GTS_DEBUG
	port_pin_set_output_level(DEBUG_PIN2, 0);
	port_pin_set_output_level(DEBUG_PIN4, 0);
	port_pin_set_output_level(DEBUG_PIN5, 0);
	port_pin_set_output_level(DEBUG_PIN6, 0);
	port_pin_set_output_level(DEBUG_PIN7, 0);
	port_pin_set_output_level(DEBUG_PIN8, 0);
	port_pin_set_output_level(DEBUG_PIN9, 0);
	port_pin_set_output_level(DEBUG_PIN10, 0);
	#endif

	mac_superframe_state = MAC_INACTIVE;
}

#endif /* BEACON_SUPPORT */

#ifdef BEACON_SUPPORT

/*
 * @brief Handles transmission of pending broadcast data
 *
 * This function handles the transmission of pending broadcast data
 * frames in a beacon-enabled network, which need to be transmitted
 * immediately after the beacon transmission.
 * As defined by 802.15.4-2006 exactly one broadcast data frame is
 * transmitted at one point of time. Further pending broadcast frames
 * are transmitted after the next beacon frame.
 */
void mac_tx_pending_bc_data(void)
{
	buffer_t *buf_ptr;
	frame_info_t *transmit_frame;
	retval_t tal_tx_status;

	buf_ptr = qmm_queue_remove(&broadcast_q, NULL);

	Assert(buf_ptr != NULL);

	if (NULL == buf_ptr) {
		/* Nothing ot be done. */
		return;
	}

	/* Broadcast data present and to be sent. */
	transmit_frame = (frame_info_t *)BMM_BUFFER_POINTER(buf_ptr);

	transmit_frame->buffer_header = buf_ptr;

	tal_tx_status = tal_tx_frame(transmit_frame, NO_CSMA_WITH_IFS, false);

	if (MAC_SUCCESS == tal_tx_status) {
		MAKE_MAC_BUSY();
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

#endif /* BEACON_SUPPORT */

#endif /* MAC_START_REQUEST_CONFIRM */
#if  (defined ENABLE_SLEEP || defined RTC_SLEEP)

/*
 * @brief MAC Wakeup Callback Function from application for Synchronizing beacon
 * timing after Wakeup
 *
 * This function Handles the residual time for Beacon Synchronization after
 * Wakeup
 * @param res_time remaining time to be synchronized with next beacon timing.
 */
void mac_wakeup(uint32_t res_time)
{
  #ifdef FFD
	if ((MAC_PAN_COORD_STARTED == mac_state) ||
			(MAC_COORDINATOR == mac_state)) {
		sw_timer_stop(T_Beacon_Preparation);
		if (res_time >= MAC_MIN_WAKEUP_US) {
			pal_timer_start(
					T_Beacon_Preparation,
					res_time,
					TIMEOUT_RELATIVE,
					(
						FUNC_PTR)mac_t_prepare_beacon_cb,
					NULL);
		} else {
			mac_t_prepare_beacon_cb(NULL);
		}
	}
  #endif
	if (MAC_ASSOCIATED == mac_state) {
		sw_timer_stop(T_Beacon_Tracking_Period);
		if (pal_is_timer_running(T_Missed_Beacon)) {
			sw_timer_stop(T_Missed_Beacon);
		}

		if (res_time >= MAC_MIN_WAKEUP_US) {
			pal_timer_start(T_Beacon_Tracking_Period,
					res_time,
					TIMEOUT_RELATIVE,
					(FUNC_PTR)mac_t_tracking_beacons_cb,
					NULL);
		} else {
			mac_t_tracking_beacons_cb(NULL);
		}
	}
}

#endif
/* EOF */
