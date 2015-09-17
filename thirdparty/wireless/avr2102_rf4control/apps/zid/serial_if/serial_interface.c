/**
 * @file serial_interface.c
 *
 * @brief RF4CE Serial Interface Implementation
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

/*
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

#ifdef SIO_HUB

/* === INCLUDES ============================================================ */

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "compiler.h"
#include "app_config.h"
#include "nwk_msg_code.h"
#include "serial_api.h"
#include "sio2host.h"
#include "rf4ce.h"
#ifdef ZRC_PROFILE
#include "pb_pairing.h"
#include "zrc.h"
#endif
#ifdef ZID_PROFILE
#include "pb_pairing.h"
#include "zid.h"
#endif
#ifdef ZID_DEVICE
#include "zid_device.h"
#endif
#ifdef ZID_ADAPTOR
#include "zid_adaptor.h"
#endif
#ifdef VENDOR_DATA
#include "vendor_data.h"
#endif

/* === MACROS ============================================================== */

/* UART communication framing */

/** The start of transmission delimiter. */
#define SOT                             (1)

/** The end of transmission delimiter. */
#define EOT                             (4)

/**
 * A UART state that expects a \ref SOT to be received as the next character.
 */
#define UART_RX_STATE_SOT               (1)

/**
 * A UART state that expects the length to be received as the next character.
 */
#define UART_RX_STATE_LENGTH            (2)

/**
 * A UART state that expects the protocol id  to be received as the next
 * character.
 */
#define UART_RX_PROTOCOL_ID             (3)

/**
 * A UART state that expects the next data character to be received.
 */
#define UART_RX_STATE_DATA              (4)

/**
 * A UART state that expects a \ref EOT to be received as the next character.
 */
#define UART_RX_STATE_EOT               (5)

#define SIO_BUF_COUNT                   (2)

#define SIO_RX_BUF_SIZE                 (250)
#define SIO_TX_BUF_SIZE                 SIO_RX_BUF_SIZE

/* Set the Interface to Transfer the Serial Data */
#ifdef UART0
#define SIO_CHANNEL                     (SIO_0)
#endif
#ifdef UART1
#define SIO_CHANNEL                     (SIO_1)
#endif
#ifdef USB0
#define SIO_CHANNEL                     (SIO_2)
#endif

/* === Globals ============================================================= */

/**
 * This is the receive buffer of the UART.
 */
static uint8_t sio_rx_buf[SIO_RX_BUF_SIZE];

/**
 * This is the transmit buffer of the UART.
 */
static uint8_t sio_tx_buf[SIO_BUF_COUNT][SIO_TX_BUF_SIZE];

/**
 * This pointer points to the next free element inside the
 * receive buffer of the UART.
 */
static uint8_t *sio_rx_ptr;

/**
 * This is the receiver state of the UART. (\ref UART_RX_STATE_SOT,
 *   \ref UART_RX_STATE_LENGTH, \ref UART_RX_STATE_DATA, or
 *   \ref UART_RX_STATE_EOT)
 */
static volatile uint8_t sio_rx_state;

/**
 * This is the length of the message should be received.
 */
static uint8_t sio_rx_length;

/**
 * This is the length of the message should be transmitted.
 */
static uint8_t data[SIO_RX_BUF_SIZE];
static uint8_t data_length = 0;
static uint8_t rx_index = 0;
static uint8_t head = 0, buf_count = 0;
static uint8_t protocol_id;

#if (defined  RF4CE_CALLBACK_PARAM)
static zid_indication_callback_t zid_ind;

#endif
static nwk_indication_callback_t nwk_ind;

/* === Prototypes ========================================================== */

static inline void process_incoming_sio_data(void);
static uint8_t *get_next_tx_buffer(void);
static inline void handle_incoming_msg(void);

#ifdef RF4CE_CALLBACK_PARAM
static void nlde_data_confirm(nwk_enum_t Status, uint8_t PairingRef, profile_id_t OrgProfile
#ifdef NLDE_HANDLE
		, uint8_t Handle
#endif
		);

static void nlme_reset_confirm(nwk_enum_t Status);
static void nlme_rx_enable_confirm(nwk_enum_t Status);

static void nlme_unpair_indication(uint8_t PairingRef);
static void nlme_start_confirm(nwk_enum_t Status);
static void nlme_set_confirm(nwk_enum_t Status, nib_attribute_t NIBAttribute,
		uint8_t NIBAttributeIndex);
static void nlme_get_confirm(nwk_enum_t Status, nib_attribute_t NIBAttribute,
		uint8_t NIBAttributeIndex, void *NIBAttributeValue);

#if (NWK_UNPAIR_REQ_CONF == 1) || (defined RF4CE_PLATFORM)
static void nlme_unpair_confirm(uint8_t Status, uint8_t PairingRef);

#endif
#if ((NWK_UPDATE_KEY == 1) && (defined RF4CE_SECURITY)) || \
	(defined RF4CE_PLATFORM)
static void nlme_update_key_confirm(nwk_enum_t Status, uint8_t PairingRef);

#endif
#if (defined CHANNEL_AGILITY) || (defined RF4CE_PLATFORM)
static void nwk_ch_agility_confirm(nwk_enum_t Status, bool ChannelChanged,
		uint8_t LogicalChannel);

#endif
#ifdef PBP_ORG
static void pbp_org_pair_confirm(nwk_enum_t Status, uint8_t PairingRef);

#endif
#ifdef PBP_REC
static void pbp_rec_pair_confirm(nwk_enum_t Status, uint8_t PairingRef);

#endif

static void zid_connect_confirm(nwk_enum_t Status, uint8_t PairingRef);

#ifdef ZID_DEVICE

static void zid_heartbeat_confirm(nwk_enum_t Status, uint8_t PairingRef);

#endif
static void zid_report_data_confirm(nwk_enum_t Status, uint8_t PairingRef);

static void zid_get_attribute_confirm(nwk_enum_t Status, uint8_t PairingRef,
		zid_attribute_t ZIDAttribute, uint8_t ZIDAttributeIndex,
		uint8_t AttributeSize, uint8_t *ZIDAttributeValue);
static void zid_set_attribute_confirm(nwk_enum_t Status, uint8_t PairingRef,
		zid_attribute_t ZIDAttribute, uint8_t ZIDAttributeIndex);

#if (defined ZID_ADAPTOR)
static void zid_standby_confirm(nwk_enum_t Status, bool StdbyEnabled);
static void zid_heartbeat_indication(uint8_t PairingRef);
static void zid_standby_leave_indication(void);
static void zid_set_report_confirm(nwk_enum_t Status, uint8_t PairingRef);
static void zid_get_report_confirm(nwk_enum_t Status, uint8_t PairingRef);

#endif
#ifdef VENDOR_DATA
static void vendor_data_confirm(nwk_enum_t Status, uint8_t PairingRef, profile_id_t ProfileId
#ifdef NLDE_HANDLE
		, uint8_t Handle
#endif
		);
static void vendor_data_ind(uint8_t PairingRef, uint16_t VendorId,
		uint8_t nsduLength, uint8_t *nsdu, uint8_t RxLinkQuality,
		uint8_t RxFlags);

#endif
static void zid_report_data_indication(uint8_t PairingRef,
		uint8_t num_report_records,
		zid_report_data_record_t *zid_report_data_record_ptr,
		uint8_t RxLinkQuality, uint8_t RxFlags);

void zid_indication_callback_init(void);

#ifdef ZID_DEVICE
static void zid_get_report_indication(uint8_t PairingRef,
		zid_report_types_t zid_report_type,
		zid_report_desc_t zid_report_desc, uint8_t RxLinkQuality,
		uint8_t RxFlags);

#endif
#endif /* RF4CE_CALLBACK_PARAM */
#ifdef ZID_DEVICE
static void zid_set_null_report_confirm(nwk_enum_t Status);

#endif

/* === Implementation ====================================================== */

/**
 * @brief This function does the initialization of the zid indication functions
 */
#ifdef RF4CE_CALLBACK_PARAM
void zid_indication_callback_init(void)
{
#if (defined ZID_ADAPTOR)
	zid_ind.zid_heartbeat_indication_cb = zid_heartbeat_indication;
	zid_ind.zid_standby_leave_indication_cb = zid_standby_leave_indication;
#endif
#ifdef ZID_DEVICE
	zid_ind.zid_get_report_indication_cb = zid_get_report_indication;
#endif
	zid_ind.zid_report_data_indication_cb = zid_report_data_indication;
#ifdef VENDOR_DATA
	zid_ind.vendor_data_ind_cb = vendor_data_ind;
#endif
	register_zid_indication_callback(&zid_ind);
	nwk_ind.nlme_unpair_indication_cb = nlme_unpair_indication;

	register_nwk_indication_callback(&nwk_ind);
}

#endif

/**
 * @brief This function does the initialization of the SIO or UART.
 */
void serial_interface_init(void)
{
	sio_rx_state = UART_RX_STATE_SOT;
	sio2host_init();
}

/**
 * @brief Function to handle the state machine serial data exchange.
 */
void serial_data_handler(void)
{
	/* Rx processing */
	if (data_length == 0) {
		/* No data to process, read the stream IO */
		rx_index = 0;
		data_length = sio2host_rx(data, SIO_RX_BUF_SIZE);
	} else { /* Data has been received, process the data */
		 /* Process each single byte */
		process_incoming_sio_data();
		data_length--;
		rx_index++;
	}

	/* Tx processing */
	if (buf_count != 0) {
		if (sio2host_tx(sio_tx_buf[head],
				(sio_tx_buf[head][1] + 3)) != 0) {
			head++;
			head %= SIO_BUF_COUNT;
			buf_count--;
		} else {
			/* @ToDo what happens if none or only a part of the
			 * bytes could be transmitted? */
		}
	}
}

/*
 * @brief Process data received from SIO
 */
static inline void process_incoming_sio_data(void)
{
	switch (sio_rx_state) {
	/* SOT Received */
	case UART_RX_STATE_SOT:
		sio_rx_ptr = sio_rx_buf;
		if (SOT == data[rx_index]) {
			sio_rx_state = UART_RX_STATE_LENGTH;
		}

		break;

	/* Get the Payload Length */
	case UART_RX_STATE_LENGTH:
		sio_rx_length = data[rx_index];
		if (sio_rx_length) {
			sio_rx_state = UART_RX_PROTOCOL_ID;
			*sio_rx_ptr = sio_rx_length;
			sio_rx_ptr++;
		} else {
			/* NULL message */
			sio_rx_ptr = sio_rx_buf;
			sio_rx_state = UART_RX_STATE_SOT;
		}

		break;

	case UART_RX_PROTOCOL_ID:

		protocol_id = data[rx_index];
		sio_rx_length--;
		sio_rx_state = UART_RX_STATE_DATA;

		break;

	case UART_RX_STATE_DATA:
		*sio_rx_ptr = data[rx_index];
		sio_rx_ptr++;
		sio_rx_length--;
		if (!sio_rx_length) {
			sio_rx_state = UART_RX_STATE_EOT;
		}

		break;

	/* End of transmission detected */
	case UART_RX_STATE_EOT:
		if (EOT == data[rx_index]) {
			if (RF4CONTROL_PID == protocol_id) {
				/* Message received successfully */
				handle_incoming_msg();
			}
		}

		/* Make rx buffer ready for next reception before handling
		 * received data. */
		sio_rx_ptr = sio_rx_buf;
		sio_rx_state = UART_RX_STATE_SOT;
		break;

	/* Unknown state and initialize the serial input handler state to the
	 * initial state */
	default:
		sio_rx_ptr = sio_rx_buf;
		sio_rx_state = UART_RX_STATE_SOT;
		break;
	}
}

static uint8_t *get_next_tx_buffer(void)
{
	if (buf_count != SIO_BUF_COUNT) {
		uint8_t *buf;
		uint8_t tail;

		tail = (head + buf_count) % SIO_BUF_COUNT;
		buf = (uint8_t *)(&sio_tx_buf[tail]);
		buf_count++;
		/* Add message start character */
		*buf++ = SOT;
		return buf;
	}

	return NULL;
}

static inline void handle_incoming_msg(void)
{
	bool ret_val;

	switch (sio_rx_buf[1]) { /* message type */
	case NLDE_DATA_REQUEST:

		/* Attention: The TxOption field is moved before the nsduLength
		 * field! */
		ret_val = nlde_data_request(sio_rx_buf[2],
				(profile_id_t)sio_rx_buf[3],
				((uint16_t)sio_rx_buf[4] |
				((uint16_t)sio_rx_buf[5] << 8)),                                       /*
		                                                                                        *
		                                                                                        *VendorId
		                                                                                        **/
				sio_rx_buf[7],             /* nsduLength */
				&sio_rx_buf[8],
				sio_rx_buf[6]            /* TxOptions */
#ifdef NLDE_HANDLE
				, 1
#endif
#ifdef RF4CE_CALLBACK_PARAM
				, (FUNC_PTR)nlde_data_confirm
#endif
				);
		break;

	case NLME_RESET_REQUEST:
		ret_val = nlme_reset_request(sio_rx_buf[2]
#ifdef RF4CE_CALLBACK_PARAM
				, (FUNC_PTR)nlme_reset_confirm
#endif
				);
		break;

#if (NWK_GET == 1)
	case NLME_GET_REQUEST:
		ret_val
			= nlme_get_request(
				(nib_attribute_t)sio_rx_buf[2], sio_rx_buf[3]
#ifdef RF4CE_CALLBACK_PARAM
				, (FUNC_PTR)nlme_get_confirm
#endif
				);
		break;
#endif

	case NLME_RX_ENABLE_REQUEST:
#if (PAL_GENERIC_TYPE == AVR32) || (PAL_GENERIC_TYPE == SAM3)
		{
			uint32_t rx_on_duration;
			MEMCPY_ENDIAN(&rx_on_duration, &sio_rx_buf[2],
					sizeof(uint32_t));
			ret_val = nlme_rx_enable_request(rx_on_duration
#ifdef RF4CE_CALLBACK_PARAM
					, (FUNC_PTR)nlme_rx_enable_confirm
#endif
					);
		}
#else
		ret_val = nlme_rx_enable_request(*(uint32_t *)&sio_rx_buf[2]
#ifdef RF4CE_CALLBACK_PARAM
				, (FUNC_PTR)nlme_rx_enable_confirm
#endif
				);
#endif

		break;

#if (NWK_SET == 1)
	case NLME_SET_REQUEST:
		ret_val = nlme_set_request((nib_attribute_t)sio_rx_buf[2],
				sio_rx_buf[3], &sio_rx_buf[5]
#ifdef RF4CE_CALLBACK_PARAM
				, (FUNC_PTR)nlme_set_confirm
#endif
				);
		break;

#endif
	case NLME_START_REQUEST:
		ret_val = nlme_start_request(
#ifdef RF4CE_CALLBACK_PARAM
				(FUNC_PTR)nlme_start_confirm
#endif
				);
		break;

#if (NWK_UNPAIR_REQ_CONF == 1)
	case NLME_UNPAIR_REQUEST:
		ret_val = nlme_unpair_request(sio_rx_buf[2]
#ifdef RF4CE_CALLBACK_PARAM
				, (FUNC_PTR)nlme_unpair_confirm
#endif
				);
		break;

#endif
#if (NWK_UNPAIR_IND_RESP == 1)
	case NLME_UNPAIR_RESPONSE:
		ret_val = nlme_unpair_response(sio_rx_buf[2]);
		break;

#endif
#if ((NWK_UPDATE_KEY == 1) && (defined RF4CE_SECURITY)) || \
		(defined RF4CE_PLATFORM)
	case NLME_UPDATE_KEY_REQUEST:
		ret_val = nlme_update_key_request(sio_rx_buf[2], &sio_rx_buf[3]
#ifdef RF4CE_CALLBACK_PARAM
				, (FUNC_PTR)nlme_update_key_confirm
#endif
				);
		break;

#endif
#if (defined CHANNEL_AGILITY) || (defined RF4CE_PLATFORM)
	case NWK_CH_AGILITY_REQUEST:
		ret_val
			= nwk_ch_agility_request((nwk_agility_mode_t)sio_rx_buf[
					2]
#ifdef RF4CE_CALLBACK_PARAM
				, (FUNC_PTR)nwk_ch_agility_confirm
#endif
				);
		break;

#endif
#ifdef PBP_ORG
	case PBP_ORG_PAIR_REQUEST:
#if (PAL_GENERIC_TYPE == AVR32)
		{
			dev_type_t OrgDevTypeList[DEVICE_TYPE_LIST_SIZE];
			profile_id_t OrgProfileIdList[PROFILE_ID_LIST_SIZE];
			profile_id_t DiscProfileIdList[PROFILE_ID_LIST_SIZE];

			for (uint8_t i = 0; i < DEVICE_TYPE_LIST_SIZE; i++) {
				OrgDevTypeList[i]
					= (dev_type_t)sio_rx_buf[3 + i];
			}
			for (uint8_t i = 0; i < PROFILE_ID_LIST_SIZE; i++) {
				OrgProfileIdList[i]
					= (profile_id_t)sio_rx_buf[6 +
						i];
			}
			for (uint8_t i = 0; i < PROFILE_ID_LIST_SIZE; i++) {
				DiscProfileIdList[i]
					= (profile_id_t)sio_rx_buf[13 +
						2 + i];
			}

			ret_val = pbp_org_pair_request(sio_rx_buf[2], /*
			                                               *
			                                               *OrgAppCapabilities
			                                               **/
					OrgDevTypeList,            /*
			                                            *OrgDevTypeList
			                                            **/
					OrgProfileIdList,          /*
			                                            *OrgProfileIdList
			                                            **/
					(dev_type_t)sio_rx_buf[13],            /*
			                                                        *
			                                                        *SearchDevType
			                                                        **/
					sio_rx_buf[14],            /*
			                                            *DiscProfileIdListSize
			                                            **/
					DiscProfileIdList       /*
			                                         *DiscProfileIdList,
			                                         **/
#ifdef RF4CE_CALLBACK_PARAM
					, (FUNC_PTR)pbp_org_pair_confirm
#endif
					);
		}
#else
		ret_val = pbp_org_pair_request(sio_rx_buf[2],
				(dev_type_t *)&sio_rx_buf[3],
				(profile_id_t *)&sio_rx_buf[3 +
				DEVICE_TYPE_LIST_SIZE],
				(dev_type_t)sio_rx_buf[3 +
				DEVICE_TYPE_LIST_SIZE + PROFILE_ID_LIST_SIZE],
				sio_rx_buf[4 + DEVICE_TYPE_LIST_SIZE +
				PROFILE_ID_LIST_SIZE],
				(profile_id_t *)&sio_rx_buf[5 +
				DEVICE_TYPE_LIST_SIZE + PROFILE_ID_LIST_SIZE]
#ifdef RF4CE_CALLBACK_PARAM
				, (FUNC_PTR)pbp_org_pair_confirm
#endif
				);
#endif
		break;

#endif
#ifdef PBP_REC
	case PBP_REC_PAIR_REQUEST:
#if (PAL_GENERIC_TYPE == AVR32) || (PAL_GENERIC_TYPE == SAM3)
		{
			dev_type_t RecDevTypeList[DEVICE_TYPE_LIST_SIZE];
			profile_id_t RecProfileIdList[PROFILE_ID_LIST_SIZE];

			for (uint8_t i = 0; i < DEVICE_TYPE_LIST_SIZE; i++) {
				RecDevTypeList[i]
					= (dev_type_t)sio_rx_buf[3 + i];
			}
			for (uint8_t i = 0; i < PROFILE_ID_LIST_SIZE; i++) {
				RecProfileIdList[i]
					= (profile_id_t)sio_rx_buf[6 +
						i];
			}
			ret_val = pbp_rec_pair_request(sio_rx_buf[2], /*
			                                               *
			                                               *RecAppCapabilities
			                                               **/
					RecDevTypeList,       /* (dev_type_t
			                                       *
			                                       **)&sio_rx_buf[3],
			                                       **/
					RecProfileIdList       /* (profile_id_t
			                                        *
			                                        **)&sio_rx_buf[3
			                                        * + 3], */
#ifdef RF4CE_CALLBACK_PARAM
					, (FUNC_PTR)pbp_rec_pair_confirm
#endif
					);
		}
#else
		ret_val = pbp_rec_pair_request(
				sio_rx_buf[2] /*uint8_t RecAppCapabilities*/,
				(dev_type_t *)&sio_rx_buf[3] /*dev_type_t
		                                              *
		                                              *RecDevTypeList[DEVICE_TYPE_LIST_SIZE]*/,
				(profile_id_t *)&sio_rx_buf[3 + 3]           /*profile_id_t
		                                                              *
		                                                              *RecProfileIdList[PROFILE_ID_LIST_SIZE]*/
#ifdef RF4CE_CALLBACK_PARAM
				, (FUNC_PTR)pbp_rec_pair_confirm
#endif
				);
#endif
		break;
#endif

#if (defined ZID_ADAPTOR)
	case ZID_REC_CONNECT_REQUEST:
		ret_val = zid_rec_connect_request(
				sio_rx_buf[2] /*uint8_t RecAppCapabilities*/,
				(dev_type_t *)&sio_rx_buf[3] /*dev_type_t
		                                              *
		                                              *RecDevTypeList[DEVICE_TYPE_LIST_SIZE]*/,
				(profile_id_t *)&sio_rx_buf[3 + 3]               /*profile_id_t
		                                                                  *
		                                                                  *RecProfileIdList[PROFILE_ID_LIST_SIZE]*/
#ifdef RF4CE_CALLBACK_PARAM
				, (FUNC_PTR)zid_connect_confirm
#endif
				);
		break;
#endif

#ifdef ZID_DEVICE
	case ZID_ORG_CONNECT_REQUEST:
		ret_val = zid_org_connect_request(sio_rx_buf[2], /*uint8_t
		                                                  *
		                                                  *OrgAppCapabilities,
		                                                  **/
				(dev_type_t *)&sio_rx_buf[3],               /*dev_type_t
		                                                             *
		                                                             *OrgDevTypeList[DEVICE_TYPE_LIST_SIZE],
		                                                             **/
				(profile_id_t *)&sio_rx_buf[3 + 3],               /*profile_id_t
		                                                                   *
		                                                                   *OrgProfileIdList[PROFILE_ID_LIST_SIZE],*/
				(dev_type_t)sio_rx_buf[13],               /*dev_type_t
		                                                           *
		                                                           *SearchDevType,*/
				sio_rx_buf[14],               /*uint8_t
		                                               *
		                                               *DiscProfileIdListSize,*/
				(profile_id_t *)&sio_rx_buf[15]               /*profile_id_t
		                                                               *
		                                                               *DiscProfileIdList[PROFILE_ID_LIST_SIZE]);*/
#ifdef RF4CE_CALLBACK_PARAM
				, (FUNC_PTR)zid_connect_confirm
#endif
				);
		break;

#endif
#ifdef ZID_ADAPTOR
	case ZID_SET_REPORT_REQUEST:
		ret_val = zid_set_report_request(sio_rx_buf[2], /*uint8_t
		                                                 * pairingref,
		                                                 **/
				sio_rx_buf[4],               /*uint8_t
		                                              * payloadlength */
				&sio_rx_buf[5],               /* uint8_t
		                                               **payload */
				sio_rx_buf[3]               /* uint8_t Txoptions
		                                            **/
#ifdef RF4CE_CALLBACK_PARAM
				, (FUNC_PTR)zid_set_report_confirm
#endif
				);
		break;
#endif

	case ZID_REPORT_DATA_REQUEST:
	{
		uint8_t num_records, *msg_ptr, *desc_ptr;
		uint8_t i, buffer[256]; /* need to decide the size */
		num_records = sio_rx_buf[5];
		/* sio_rx_buf[4] is ignored since zid_payload length is not used
		 **/
		/* zid_report_data_record_t *zid_report_data_record_ptr[8]; */
		zid_report_data_record_t zid_report_data_record[8];
		msg_ptr = (uint8_t *)&sio_rx_buf[6];
		desc_ptr = (uint8_t *)&buffer[0];
		for (i = 0; i < num_records; i++) {
			/* zid_report_data_record_ptr[i] =
			 *
			 *&zid_report_data_record[i];//(zid_report_data_record_t
			 * *)msg_ptr; */
			/* zid_report_data_record[i].report_size = *msg_ptr++;
			 **/
			zid_report_data_record[i].report_type
				= (zid_report_types_t)*msg_ptr++;
			zid_report_data_record[i].report_desc_identifier
				= (zid_report_desc_t)*msg_ptr++;

			zid_report_data_record[i].report_data
				= (void *)desc_ptr;

			switch (zid_report_data_record[i].report_desc_identifier)
			{
			case MOUSE:
			{
				/* Mouse report received and construct the ZID
				 * mouse report */
				mouse_desc_t *mouse_desc_ptr
					= (mouse_desc_t *)desc_ptr;
				mouse_desc_ptr->button0 = *msg_ptr++ & 0x01;
				mouse_desc_ptr->button1 = *msg_ptr++ & 0x01;
				mouse_desc_ptr->button2 = *msg_ptr++ & 0x01;
				mouse_desc_ptr->x_coordinate = *msg_ptr++;
				mouse_desc_ptr->y_coordinate = *msg_ptr++;
				desc_ptr += sizeof(mouse_desc_t);
				break;
			}

			case KEYBOARD:
			{
				/* Keyboard report received and construct the
				 * ZID Keyboard report */
				if (zid_report_data_record[i].report_type ==
						INPUT) {
					keyboard_input_desc_t *mouse_desc_ptr
						= (
						keyboard_input_desc_t
						*)
							desc_ptr;
					mouse_desc_ptr->modifier_keys
						= *msg_ptr++;
					memcpy(mouse_desc_ptr->key_code,
							msg_ptr, 6);
					msg_ptr += 6;
					desc_ptr
						+= sizeof(keyboard_input_desc_t);
				} else if (zid_report_data_record[i].report_type
						== OUTPUT) {
					keyboard_output_desc_t *mouse_desc_ptr
						= (
						keyboard_output_desc_t
						*)
							desc_ptr;
					mouse_desc_ptr->num_lock = *msg_ptr++ &
							0x01;
					mouse_desc_ptr->caps_lock = *msg_ptr++ &
							0x01;
					mouse_desc_ptr->scroll_lock
						= *msg_ptr++ &
							0x01;
					mouse_desc_ptr->compose =  *msg_ptr++ &
							0x01;
					mouse_desc_ptr->kana =  *msg_ptr++ &
							0x01;
					desc_ptr
						+= sizeof(keyboard_output_desc_t);
				} else {
					return;
				}

				break;
			}

			case CONTACT_DATA:
			{
				/* Construct & send the ZID contact data report
				**/
				contact_data_report_t *contact_data;
				contact_data
					= (contact_data_report_t *)desc_ptr;

				contact_data->contact_type = *msg_ptr++ & 0x0F;
				contact_data->contact_index = *msg_ptr++ & 0x0F;
				contact_data->contact_state = *msg_ptr++ & 0x03;
				contact_data->major_axis_orientation
					= *msg_ptr++;
				contact_data->pressure = *msg_ptr++;
				contact_data->location_x = *msg_ptr++;
				contact_data->location_x
					|= (((uint16_t)*msg_ptr++
						<< 8) & 0x0F00);
				contact_data->location_y = *msg_ptr++ & 0x0F;
				contact_data->location_y
					|= (((uint16_t)*msg_ptr++
						<< 4) & 0x0FF0);
				contact_data->major_axis_length = *msg_ptr++ &
						0xFF;
				contact_data->major_axis_length
					|= ((uint16_t)*msg_ptr
						++ << 8);
				contact_data->minor_axis_length = *msg_ptr++ &
						0xFF;
				contact_data->minor_axis_length
					|= ((uint16_t)*msg_ptr
						++ << 8);

				desc_ptr += sizeof(contact_data_report_t);
				break;
			}

			case TAP_GESTURE:
			{
				/* Send the ZID tap gesture report */
				tap_gesture_report_t *tap_gesture;
				tap_gesture = (tap_gesture_report_t *)desc_ptr;

				tap_gesture->finger_count = *msg_ptr++ & 0x07;
				tap_gesture->type = *msg_ptr++ & 0x1F;
				tap_gesture->location_x = *msg_ptr++;
				tap_gesture->location_x
					|= ((uint16_t)*msg_ptr++ <<
						8);
				tap_gesture->location_y = *msg_ptr++;
				tap_gesture->location_y
					|= ((uint16_t)*msg_ptr++ <<
						4);
				desc_ptr += sizeof(tap_gesture_report_t);
				break;
			}

			case SCROLL_GESTURE:
			{
				/* Send the scroll gesture report */
				scroll_gesture_report_t *scroll_gesture;
				scroll_gesture
					= (scroll_gesture_report_t *)desc_ptr;

				scroll_gesture->finger_count = *msg_ptr++ & 0x7;
				scroll_gesture->type = *msg_ptr++ & 0x1F;

				/*                  scroll_gesture->reserved_1 =
				 * sio_rx_buf[9]; */
				scroll_gesture->direction = *msg_ptr++ & 0x07;
				scroll_gesture->distance = *msg_ptr++ & 0x0F;
				scroll_gesture->distance
					|= ((uint16_t)*msg_ptr++
						<< 4);

				desc_ptr += sizeof(scroll_gesture_report_t);
				break;
			}

			case PINCH_GESTURE:
			{
				/* Send the pinch gesture report */
				pinch_gesture_report_t *pinch_gesture;
				pinch_gesture
					= (pinch_gesture_report_t *)desc_ptr;

				pinch_gesture->direction = *msg_ptr++ & 0x01;
				pinch_gesture->finger_present = *msg_ptr++ &
						0x01;
				pinch_gesture->distance = *msg_ptr++;
				pinch_gesture->distance
					|= ((uint16_t)*msg_ptr++ <<
						8);

				pinch_gesture->center_x = *msg_ptr++;
				pinch_gesture->center_x
					|= ((uint16_t)*msg_ptr++ <<
						8);
				pinch_gesture->center_y = *msg_ptr++ & 0x0F;
				pinch_gesture->center_y
					|= ((uint16_t)*msg_ptr++ <<
						4);

				desc_ptr += sizeof(pinch_gesture_report_t);
				break;
			}

			case ROTATE_GESTURE:
			{
				/* Send the rotate gesture report */
				rotation_gesture_report_t *rotation_gesture;
				rotation_gesture
					= (rotation_gesture_report_t *)desc_ptr;

				rotation_gesture->direction = *msg_ptr++ & 0x01;
				rotation_gesture->finger_present = *msg_ptr++ &
						0x01;
				rotation_gesture->magnitude = *msg_ptr++;

				desc_ptr += sizeof(rotation_gesture_report_t);
				break;
			}

			case SYNC:
			{
				/* Send the synchronization report */
				sync_report_t *sync;
				sync = (sync_report_t *)desc_ptr;

				sync->contact_count = *msg_ptr++ & 0x0F;
				sync->gesture = *msg_ptr++ & 0x01;

				desc_ptr += sizeof(sync_report_t);
				break;
			}

			case TOUCH_SENSOR_PROPERTIES:
			{
				/* Send the touch sensor properties report */
				touch_sensor_properties_t *
				touch_sensor_properties;
				touch_sensor_properties
					= (touch_sensor_properties_t
						*)desc_ptr;

				touch_sensor_properties->
				no_of_additional_contacts
					= *msg_ptr++ &
						0x0F;
				touch_sensor_properties->origin = *msg_ptr++ &
						0x03;
				touch_sensor_properties->reliable_index
					= *msg_ptr++ &
						0x01;
				touch_sensor_properties->gestures = *msg_ptr++ &
						0x01;
				touch_sensor_properties->resolution_x
					= *msg_ptr++;
				touch_sensor_properties->resolution_y
					= *msg_ptr++;
				touch_sensor_properties->max_coordinate_x
					= *msg_ptr++;
				touch_sensor_properties->max_coordinate_y
					= *msg_ptr++ &
						0x0F;
				touch_sensor_properties->max_coordinate_x
					= (touch_sensor_properties
						->
						max_coordinate_x
						<< 4) |
						(*msg_ptr++ &
						0x0F);

				touch_sensor_properties->max_coordinate_y
					= (touch_sensor_properties
						->
						max_coordinate_y
						<< 8) |
						*msg_ptr++;
				touch_sensor_properties->shape = *msg_ptr++ &
						0x07;

				desc_ptr += sizeof(touch_sensor_properties_t);
				break;
			}

			case TAP_SUPPORT_PROPERTIES:
			{
				/* Send the tap support properties report */
				tap_support_properties_t *tap_support_properties;
				tap_support_properties
					= (tap_support_properties_t
						*)desc_ptr;

				tap_support_properties->single_tap
					= *msg_ptr++ &
						0x01;
				tap_support_properties->tap_and_a_half
					= *msg_ptr++ &
						0x01;
				tap_support_properties->double_tap
					= *msg_ptr++ &
						0x01;
				tap_support_properties->long_tap = *msg_ptr++ &
						0x01;

				desc_ptr += sizeof(tap_support_properties_t);
				break;
			}
			}

			/* msg_ptr +=
			 * (zid_report_data_record_ptr[i]->report_size -3); */
		}

		ret_val = zid_report_data_request(sio_rx_buf[2], /*uint8_t
			                                          * PairingRef,
			                                          **/
				num_records,               /*uint8_t
			                                    * zidPayloadLength,
			                                    **/
				zid_report_data_record,               /*uint8_t
			                                               * **zidPayload,
			                                               **/
				sio_rx_buf[3]              /*, uint8_t
			                                    * TxOptions);*/
#ifdef RF4CE_CALLBACK_PARAM
				, (FUNC_PTR)zid_report_data_confirm
#endif
				);
	}
	break;

#if (defined ZID_PROFILE) && (ZID_SET == 1)
	case ZID_SET_ATTRIBUTE_REQUEST:
		ret_val = zid_set_attribute_request(sio_rx_buf[2], /*uint8_t
		                                                    *
		                                                    *PairingRef,*/
				(zid_attribute_t)sio_rx_buf[3],                 /*zid_attribute_t
		                                                                 *
		                                                                 *ZIDAttribute,
		                                                                 **/
				sio_rx_buf[4],                 /*uint8_t
		                                                *
		                                                *ZIDAttributeIndex,
		                                                **/
				&sio_rx_buf[6]                 /*,uint8_t
		                                                **ZIDAttributeValue)*/
#ifdef RF4CE_CALLBACK_PARAM
				, (FUNC_PTR)zid_set_attribute_confirm
#endif
				);
		break;
#endif

#if (defined ZID_PROFILE) && (ZID_GET == 1)
	case ZID_GET_ATTRIBUTE_REQUEST:
		ret_val = zid_get_attribute_request(sio_rx_buf[2], /* bool OTA
		                                                   **/
				sio_rx_buf[3],                 /*uint8_t
		                                                * PairingRef, */
				(zid_attribute_t)sio_rx_buf[4],                 /*zid_attribute_t
		                                                                 *
		                                                                 *ZIDAttribute,
		                                                                 **/
				sio_rx_buf[5]                /*uint8_t
		                                              *
		                                              *ZIDAttributeIndex);*/
#ifdef RF4CE_CALLBACK_PARAM
				, (FUNC_PTR)zid_get_attribute_confirm
#endif
				);
		break;
#endif

#ifdef ZID_ADAPTOR
	case ZID_STANDBY_REQUEST:
		ret_val = zid_standby_request((bool)sio_rx_buf[2]
#ifdef RF4CE_CALLBACK_PARAM
				, (FUNC_PTR)zid_standby_confirm
#endif
				);
		break;
#endif

#if 0
#ifdef ZID_ADAPTOR
	case ZID_GET_ATTRIBUTE_REQUEST:
		/* zid_get_attribute_request */
		break;
#endif
#endif

#if 0
#ifdef ZID_ADAPTOR
	case ZID_GET_IDLE_REQUEST:
		ret_val = zid_get_idle_request(sio_rx_buf[2], sio_rx_buf[3]);
		break;
#endif
#endif

#ifdef VENDOR_DATA
	case VENDOR_DATA_REQUEST:
		ret_val = nlde_data_request(sio_rx_buf[2], /*uint8_t
		                                            * PairingRef,*/
				PROFILE_ID_ZID,
				(uint16_t)sio_rx_buf[4] |
				((uint16_t)sio_rx_buf[5] << 8),                                   /*uint16_t
		                                                                                   *
		                                                                                   *VendorId,*/
				sio_rx_buf[7],         /*uint8_t nsduLength,*/
				&sio_rx_buf[8],         /*uint8_t *nsdu,*/
				sio_rx_buf[6]         /*uint8_t TxOptions*/
#ifdef NLDE_HANDLE
				, 1
#endif
#ifdef RF4CE_CALLBACK_PARAM
				, (FUNC_PTR)vendor_data_confirm
#endif
				);
		break;

#endif
#ifdef ZID_ADAPTOR
	case ZID_GET_REPORT_REQUEST:
		ret_val
			= zid_get_report_data_request(sio_rx_buf[2],
				(zid_report_types_t)sio_rx_buf[
					4], (zid_report_desc_t)sio_rx_buf[5],
				sio_rx_buf[3]
#ifdef RF4CE_CALLBACK_PARAM
				, (FUNC_PTR)zid_get_report_confirm
#endif
				);

		break;
#endif

#ifdef ZID_DEVICE
	case ZID_HEARTBEAT_REQUEST:
		ret_val = zid_heartbeat_request(sio_rx_buf[2]
#ifdef RF4CE_CALLBACK_PARAM
				, (FUNC_PTR)zid_heartbeat_confirm
#endif
				);
		break;

	case ZID_SET_NULL_REPORT_REQUEST:
		ret_val = zid_set_null_report(sio_rx_buf[3],
				(null_report_t *)&sio_rx_buf[4]);
		zid_set_null_report_confirm(NWK_SUCCESS);

		break;
#endif

	default:
		Assert("???" == 0);
		break;
	}

	if (ret_val == false) {
		Assert("Buffer is not available (Test harness)" == 0);
	}
}

/* Network primitives callbacks*/

#ifndef ZRC_PROFILE
void nlde_data_indication(uint8_t PairingRef, profile_id_t ProfileId,
		uint16_t VendorId, uint8_t nsduLength, uint8_t *nsdu,
		uint8_t RxLinkQuality, uint8_t RxFlags)
{
	uint8_t *msg_buf;
	uint8_t *nsdu_ptr;
	uint8_t i;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = NLDE_DATA_IND_LEN + nsduLength + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = NLDE_DATA_INDICATION;
	*msg_buf++ = PairingRef;
	*msg_buf++ = ProfileId;
	*msg_buf++ = (uint8_t)VendorId; /* LSB */
	*msg_buf++ = (uint8_t)(VendorId >> 8); /* MSB */
	*msg_buf++ = RxLinkQuality;
	*msg_buf++ = RxFlags;
	*msg_buf++ = nsduLength;
	nsdu_ptr = nsdu;
	/* @ToDo use memcpy */
	for (i = 0; i < nsduLength; i++) {
		*msg_buf++ = *nsdu_ptr++;
	}
	*msg_buf = EOT;
}

#endif

#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void nlde_data_confirm(nwk_enum_t Status, uint8_t PairingRef, profile_id_t OrgProfile
#ifdef NLDE_HANDLE
		, uint8_t Handle
#endif
		)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = NLDE_DATA_CONF_LEN + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = NLDE_DATA_CONFIRM;
	*msg_buf++ = Status;
	*msg_buf++ = PairingRef;
	*msg_buf++ = OrgProfile;
	*msg_buf = EOT;
	/* Keeps compiler happy */
#ifdef NLDE_HANDLE
	Handle = Handle;
#endif
}

#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void nlme_reset_confirm(nwk_enum_t Status)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = NLME_RESET_CONF_LEN + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = NLME_RESET_CONFIRM;
	*msg_buf++ = Status;
	*msg_buf = EOT;
}

#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void nlme_rx_enable_confirm(nwk_enum_t Status)
{
	uint8_t *msg_buf;
	msg_buf = get_next_tx_buffer();
	*msg_buf++ = NLME_RX_ENABLE_CONF_LEN + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = NLME_RX_ENABLE_CONFIRM;
	*msg_buf++ = Status;
	*msg_buf = EOT;
}

#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void nlme_set_confirm(nwk_enum_t Status, nib_attribute_t NIBAttribute,
		uint8_t NIBAttributeIndex)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = 5; /* *msg_buf++ = NLME_SET_CONF_LEN+RF4CONTROL_PID_LEN; */
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = NLME_SET_CONFIRM;
	*msg_buf++ = Status;
	*msg_buf++ = NIBAttribute;
	*msg_buf++ = NIBAttributeIndex;
	*msg_buf = EOT;
}

#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void nlme_get_confirm(nwk_enum_t Status, nib_attribute_t NIBAttribute,
		uint8_t NIBAttributeIndex, void *NIBAttributeValue)
{
	uint8_t *msg_buf;
	uint8_t i;
	uint8_t attr_len;

	uint8_t *ptr;
	ptr = NIBAttributeValue;

	msg_buf = get_next_tx_buffer();
	attr_len = nwk_get_nib_attribute_size(NIBAttribute);
	*msg_buf++ = NLME_GET_CONF_LEN + attr_len + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = NLME_GET_CONFIRM;
	*msg_buf++ = Status;
	*msg_buf++ = NIBAttribute;
	*msg_buf++ = NIBAttributeIndex;
	*msg_buf++ = attr_len;
	for (i = 0; i < attr_len; i++) {
		*msg_buf++ = *ptr++;
	}
	*msg_buf = EOT;
}

#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void nlme_start_confirm(nwk_enum_t Status)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = NLME_START_CONF_LEN + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = NLME_START_CONFIRM;
	*msg_buf++ = Status;
	*msg_buf = EOT;
}

#if (NWK_UNPAIR_IND_RESP == 1) || (defined RF4CE_PLATFORM)
#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void nlme_unpair_indication(uint8_t PairingRef)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = NLME_UNPAIR_IND_LEN + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = NLME_UNPAIR_INDICATION;
	*msg_buf++ = PairingRef;
	*msg_buf = EOT;
}

#endif

#if ((NWK_UNPAIR_REQ_CONF == 1) || (defined RF4CE_PLATFORM))
#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void nlme_unpair_confirm(uint8_t Status, uint8_t PairingRef)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = NLME_UNPAIR_CONF_LEN + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = NLME_UNPAIR_CONFIRM;
	*msg_buf++ = Status;
	*msg_buf++ = PairingRef;
	*msg_buf = EOT;
}

#endif

#if ((NWK_UPDATE_KEY == 1) && (defined RF4CE_SECURITY)) || \
	(defined RF4CE_PLATFORM)
#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void nlme_update_key_confirm(nwk_enum_t Status, uint8_t PairingRef)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = NLME_UPDATE_KEY_CONF_LEN + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = NLME_UPDATE_KEY_CONFIRM;
	*msg_buf++ = Status;
	*msg_buf++ = PairingRef;
	*msg_buf = EOT;
}

#endif

#if (defined CHANNEL_AGILITY) || (defined RF4CE_PLATFORM)
void nwk_ch_agility_indication(uint8_t LogicalChannel)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = NWK_CH_AGILITY_IND_LEN + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = NWK_CH_AGILITY_INDICATION;
	*msg_buf++ = LogicalChannel;
	*msg_buf = EOT;
}

#endif

#if (defined CHANNEL_AGILITY) || (defined RF4CE_PLATFORM)
#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void nwk_ch_agility_confirm(nwk_enum_t Status, bool ChannelChanged,
		uint8_t LogicalChannel)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = NWK_CH_AGILITY_CONF_LEN + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = NWK_CH_AGILITY_CONFIRM;
	*msg_buf++ = Status;
	*msg_buf++ = (uint8_t)ChannelChanged;
	*msg_buf++ = LogicalChannel;
	*msg_buf = EOT;
}

#endif

#ifdef VENDOR_DATA
#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void vendor_data_confirm(nwk_enum_t Status, uint8_t PairingRef, profile_id_t ProfileId
#ifdef NLDE_HANDLE
		, uint8_t Handle
#endif
		)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = 3 + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = VENDOR_DATA_CONFIRM;
	*msg_buf++ = Status;
	*msg_buf++ = PairingRef;
	*msg_buf = EOT;
	/* Keeps compiler Happy */
	ProfileId = ProfileId;
#ifdef NLDE_HANDLE
	Handle = Handle;
#endif
}

#endif

#ifdef VENDOR_DATA
#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void vendor_data_ind(uint8_t PairingRef, uint16_t VendorId,
		uint8_t nsduLength, uint8_t *nsdu, uint8_t RxLinkQuality,
		uint8_t RxFlags)
{
	uint8_t *msg_buf;
	uint8_t *nsdu_ptr;
	uint8_t i;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = 8 + nsduLength + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = VENDOR_DATA_INDICATION;
	*msg_buf++ = PairingRef;
	*msg_buf++ = PROFILE_ID_ZID;
	*msg_buf++ = (uint8_t)(VendorId & 0xFF);
	*msg_buf++ = (uint8_t)(VendorId >> 8);
	*msg_buf++ = RxLinkQuality;
	*msg_buf++ = RxFlags;
	*msg_buf++ = nsduLength;
	nsdu_ptr = nsdu;
	/* @ToDo use memcpy */
	for (i = 0; i < nsduLength; i++) {
		*msg_buf++ = *nsdu_ptr++;
	}
	*msg_buf = EOT;
}

#endif

#if (defined ZID_PROFILE)
#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void zid_connect_confirm(nwk_enum_t Status, uint8_t PairingRef)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = 3 + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = ZID_CONNECT_CONFIRM;
	*msg_buf++ = Status;
	*msg_buf++ = PairingRef;
	*msg_buf = EOT;
}

#endif

#if (defined ZID_PROFILE) && (ZID_SET == 1)
#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void zid_set_attribute_confirm(nwk_enum_t Status, uint8_t PairingRef,
		zid_attribute_t ZIDAttribute,
		uint8_t ZIDAttributeIndex)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = 5 + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = ZID_SET_ATTRIBUTE_CONFIRM;
	*msg_buf++ = Status;
	*msg_buf++ = PairingRef;
	*msg_buf++ = ZIDAttribute;
	*msg_buf++ = ZIDAttributeIndex;
	*msg_buf = EOT;
}

#endif

#if (defined ZID_PROFILE) && (ZID_GET == 1)
#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void zid_get_attribute_confirm(nwk_enum_t Status, uint8_t PairingRef,
		zid_attribute_t ZIDAttribute,
		uint8_t ZIDAttributeIndex, uint8_t AttributeSize,
		uint8_t *ZIDAttributeValue)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = 6 + AttributeSize + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = ZID_GET_ATTRIBUTE_CONFIRM;
	*msg_buf++ = Status;
	*msg_buf++ = PairingRef;
	*msg_buf++ = ZIDAttribute;
	*msg_buf++ = ZIDAttributeIndex;
	*msg_buf++ = AttributeSize;
	for (uint8_t i = 0; i < AttributeSize; i++) {
		*msg_buf++ = *ZIDAttributeValue++;
	}
	*msg_buf = EOT;
}

#endif

#if (defined ZID_DEVICE)

/*
 * void zid_heartbeat_indication(nwk_enum_t Status, uint8_t PairingRef)
 * {
 * }
 */
#endif

#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void zid_report_data_confirm(nwk_enum_t Status, uint8_t PairingRef)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = 3 + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = ZID_REPORT_DATA_CONFIRM;
	*msg_buf++ = Status;
	*msg_buf++ = PairingRef;
	*msg_buf = EOT;
}

#ifdef ZID_ADAPTOR
#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void zid_set_report_confirm(nwk_enum_t Status, uint8_t PairingRef)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = 3 + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = ZID_SET_REPORT_CONFIRM;
	*msg_buf++ = Status;
	*msg_buf++ = PairingRef;
	*msg_buf = EOT;
}

#endif

#ifdef ZID_ADAPTOR
#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void zid_get_report_confirm(nwk_enum_t Status, uint8_t PairingRef)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = 3 + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = ZID_GET_REPORT_CONFIRM;
	*msg_buf++ = Status;
	*msg_buf++ = PairingRef;
	*msg_buf = EOT;
}

#endif

#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void zid_report_data_indication(uint8_t PairingRef, uint8_t num_report_records,
		zid_report_data_record_t *zid_report_data_record_ptr,
		uint8_t RxLinkQuality, uint8_t RxFlags)
{
	uint8_t *msg_buf;
	uint8_t *length_ptr, temp;

	/* Handle the various incoming zid report data */
	msg_buf = get_next_tx_buffer();
	length_ptr = msg_buf;
	msg_buf++;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = ZID_REPORT_DATA_INDICATION;
	*msg_buf++ = PairingRef;
	*msg_buf++ = RxLinkQuality;
	*msg_buf++ = RxFlags;

	msg_buf++; /* Skipping the zid payloadlength. this will be updated at
	            * the end */
	*msg_buf++ = num_report_records;
	*length_ptr = 7;

	for (uint8_t i = 0; i < num_report_records; i++) {
		/* *msg_buf++ = zid_report_data_record_ptr->report_size; */
		*msg_buf++ = (uint8_t)zid_report_data_record_ptr->report_type;
		*msg_buf++
			= (uint8_t)zid_report_data_record_ptr->
				report_desc_identifier;
		(*length_ptr) += 2;
		switch (zid_report_data_record_ptr->report_desc_identifier) {
		case MOUSE:
		{
			mouse_desc_t *mouse_desc;
			mouse_desc
				= (mouse_desc_t *)zid_report_data_record_ptr->
					report_data;
			*msg_buf++ = mouse_desc->button0;
			*msg_buf++ = mouse_desc->button1;
			*msg_buf++ = mouse_desc->button2;
			*msg_buf++ = mouse_desc->x_coordinate;
			*msg_buf++ = mouse_desc->y_coordinate;
			(*length_ptr) += sizeof(mouse_desc_t);
			break;
		}

		case KEYBOARD:
		{
			if (zid_report_data_record_ptr->report_type == INPUT) {
				keyboard_input_desc_t *keyboard_input_desc;
				keyboard_input_desc
					= (keyboard_input_desc_t *)
						zid_report_data_record_ptr
						->report_data;
				*msg_buf++ = keyboard_input_desc->modifier_keys;
				memcpy(msg_buf, keyboard_input_desc->key_code,
						6);
				msg_buf += 6;
				(*length_ptr) += sizeof(keyboard_input_desc_t);
			} else {
				keyboard_output_desc_t *keyboard_output_desc;
				keyboard_output_desc
					= (keyboard_output_desc_t *)
						zid_report_data_record_ptr
						->report_data;
				*msg_buf++ = keyboard_output_desc->num_lock;
				*msg_buf++ = keyboard_output_desc->caps_lock;
				*msg_buf++ = keyboard_output_desc->scroll_lock;
				*msg_buf++ = keyboard_output_desc->compose;
				*msg_buf++ = keyboard_output_desc->kana;
				(*length_ptr) += sizeof(keyboard_output_desc_t);
			}

			break;
		}

		case CONTACT_DATA:
		{
			contact_data_report_t *contact_data_report;
			contact_data_report
				= (contact_data_report_t *)
					zid_report_data_record_ptr
					->report_data;

			*msg_buf++ = contact_data_report->contact_type;
			*msg_buf++ = contact_data_report->contact_index;
			*msg_buf++ = contact_data_report->contact_state;
			*msg_buf++
				= contact_data_report->major_axis_orientation;
			*msg_buf++ = contact_data_report->pressure;
			*msg_buf++ = contact_data_report->location_x;
			*msg_buf++
				= (contact_data_report->location_x >>
					8) & 0x0F;
			*msg_buf++ = (contact_data_report->location_y  & 0x0F);
			*msg_buf++
				= (contact_data_report->location_y >> 4 &
					0xFF);
			*msg_buf++ = contact_data_report->major_axis_length;
			*msg_buf++
				= (contact_data_report->major_axis_length >>
					8);
			*msg_buf++ = contact_data_report->minor_axis_length;
			*msg_buf++
				= (contact_data_report->minor_axis_length >>
					8);

			(*length_ptr) += sizeof(contact_data_report_t);

			break;
		}

		case TAP_GESTURE:
		{
			tap_gesture_report_t *tap_gesture_report;
			tap_gesture_report
				= (tap_gesture_report_t *)
					zid_report_data_record_ptr
					->report_data;

			*msg_buf++ = tap_gesture_report->finger_count;
			*msg_buf++ = tap_gesture_report->type;
			*msg_buf++ = tap_gesture_report->location_x;
			*msg_buf++
				= (tap_gesture_report->location_x >> 8 &
					0x0F);
			*msg_buf++ = (tap_gesture_report->location_y & 0x0F);
			*msg_buf++
				= (tap_gesture_report->location_y >>
					4) & 0xFF;

			(*length_ptr) += sizeof(tap_gesture_report_t);

			break;
		}

		case SCROLL_GESTURE:
		{
			scroll_gesture_report_t *scroll_gesture_report;
			scroll_gesture_report
				= (scroll_gesture_report_t *)
					zid_report_data_record_ptr
					->report_data;

			*msg_buf++ = scroll_gesture_report->finger_count;
			*msg_buf++ = scroll_gesture_report->type;
			*msg_buf++ = scroll_gesture_report->direction;
			*msg_buf++ = scroll_gesture_report->distance & 0x0F;
			*msg_buf++ = (scroll_gesture_report->distance >> 4);

			(*length_ptr) += sizeof(scroll_gesture_report_t);

			break;
		}

		case PINCH_GESTURE:
		{
			pinch_gesture_report_t *pinch_gesture_report;
			pinch_gesture_report
				= (pinch_gesture_report_t *)
					zid_report_data_record_ptr
					->report_data;

			*msg_buf++ = pinch_gesture_report->direction;
			*msg_buf++ = pinch_gesture_report->finger_present;
			*msg_buf++ = pinch_gesture_report->distance & 0xFF;
			*msg_buf++ = (pinch_gesture_report->distance >> 8);
			*msg_buf++ = pinch_gesture_report->center_x;
			*msg_buf++
				= ((pinch_gesture_report->center_x >>
					8) & 0x0F);
			*msg_buf++ = (pinch_gesture_report->center_y & 0x0F);
			*msg_buf++ = (pinch_gesture_report->center_y >> 4);
			(*length_ptr) += sizeof(pinch_gesture_report_t);
			break;
		}

		case ROTATE_GESTURE:
		{
			rotation_gesture_report_t *rotation_gesture_report;
			rotation_gesture_report
				= (rotation_gesture_report_t *)
					zid_report_data_record_ptr
					->report_data;

			*msg_buf++ = rotation_gesture_report->direction;
			*msg_buf++ = rotation_gesture_report->finger_present;
			*msg_buf++ = rotation_gesture_report->magnitude;
			(*length_ptr) += sizeof(rotation_gesture_report_t);
			break;
		}

		case SYNC:
		{
			sync_report_t *sync_report;
			sync_report
				= (sync_report_t *)zid_report_data_record_ptr
					->report_data;

			*msg_buf++ = sync_report->contact_count;
			*msg_buf++ = sync_report->gesture;
			(*length_ptr) += sizeof(sync_report_t);

			break;
		}

		case TOUCH_SENSOR_PROPERTIES:
		{
			touch_sensor_properties_t *touch_sensor_properties;
			touch_sensor_properties
				= (touch_sensor_properties_t *)
					zid_report_data_record_ptr
					->report_data;

			*msg_buf++
				= touch_sensor_properties->
					no_of_additional_contacts;
			*msg_buf++ = touch_sensor_properties->origin;
			*msg_buf++ = touch_sensor_properties->reliable_index;
			*msg_buf++ = touch_sensor_properties->gestures;
			*msg_buf++ = touch_sensor_properties->resolution_x;
			*msg_buf++ = touch_sensor_properties->resolution_y;
			*msg_buf++
				= (touch_sensor_properties->max_coordinate_x >>
					4);
			*msg_buf++
				= (touch_sensor_properties->max_coordinate_y >>
					8) & 0x0F;
			*msg_buf++
				= (touch_sensor_properties->max_coordinate_x  &
					0x0F);

			*msg_buf++
				= (touch_sensor_properties->max_coordinate_y  &
					0xFF);
			*msg_buf++ = touch_sensor_properties->shape;

			(*length_ptr) += sizeof(touch_sensor_properties_t);

			break;
		}

		case TAP_SUPPORT_PROPERTIES:
		{
			tap_support_properties_t *tap_support_properties;
			tap_support_properties
				= (tap_support_properties_t *)
					zid_report_data_record_ptr
					->report_data;

			*msg_buf++ = tap_support_properties->single_tap;
			*msg_buf++ = tap_support_properties->tap_and_a_half;
			*msg_buf++ = tap_support_properties->double_tap;
			*msg_buf++ = tap_support_properties->long_tap;

			(*length_ptr) += sizeof(tap_support_properties_t);

			break;
		}
		}
		zid_report_data_record_ptr++;
	}
	temp = (*length_ptr);
	length_ptr += 6;
	*length_ptr = temp - 6;

#if 0
	*msg_buf++ = zidPayloadLength;
	for (uint8_t i = 0; i < zidPayloadLength; i++) {
		*msg_buf++ = *zidPayload++;
	}
#endif
	*msg_buf = EOT;
}

#ifdef ZID_ADAPTOR
#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void zid_standby_confirm(nwk_enum_t Status, bool StdbyEnabled)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = 3 + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = ZID_STANDBY_CONFIRM;
	*msg_buf++ = Status;
	*msg_buf++ = StdbyEnabled;
	*msg_buf = EOT;
}

#endif

#ifdef ZID_ADAPTOR
#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void zid_standby_leave_indication(void)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = 1 + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = ZID_STANDBY_LEAVE_INDICATION;
	*msg_buf = EOT;
}

#endif

#if 0
#ifdef ZID_ADAPTOR
void zid_get_idle_confirm(nwk_enum_t Status, uint8_t PairingRef,
		uint8_t ReportId, uint8_t IdleRate)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = 5 + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = ZID_GET_IDLE_CONFIRM;
	*msg_buf++ = Status;
	*msg_buf++ = PairingRef;
	*msg_buf++ = ReportId;
	*msg_buf++ = IdleRate;
	*msg_buf = EOT;
}

#endif
#endif

#ifdef ZID_ADAPTOR
#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void zid_heartbeat_indication(uint8_t PairingRef)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = 2 + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = ZID_HEARTBEAT_INDICATION;
	*msg_buf++ = PairingRef;
	*msg_buf = EOT;
}

#endif

#ifdef ZID_DEVICE
#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void zid_heartbeat_confirm(nwk_enum_t Status, uint8_t PairingRef)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = 3 + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = ZID_HEARTBEAT_CONFIRM;
	*msg_buf++ = Status;
	*msg_buf++ = PairingRef;
	*msg_buf = EOT;
}

#endif

#ifdef ZID_DEVICE
static void zid_set_null_report_confirm(nwk_enum_t Status)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = 2 + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = ZID_SET_NULL_REPORT_CONFIRM;
	*msg_buf++ = Status;
	*msg_buf = EOT;
}

#endif

#ifdef PBP_REC
#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void pbp_rec_pair_confirm(nwk_enum_t Status, uint8_t PairingRef)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = 3 + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = PBP_REC_PAIR_CONFIRM;
	*msg_buf++ = Status;
	*msg_buf++ = PairingRef;
	*msg_buf = EOT;
}

#endif

#ifdef PBP_ORG
#ifdef RF4CE_CALLBACK_PARAM
static
#endif
void pbp_org_pair_confirm(nwk_enum_t Status, uint8_t PairingRef)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = 3 + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = PBP_ORG_PAIR_CONFIRM;
	*msg_buf++ = Status;
	*msg_buf++ = PairingRef;
	*msg_buf = EOT;
}

#endif

#ifdef ZID_DEVICE
static void zid_get_report_indication(uint8_t PairingRef,
		zid_report_types_t zid_report_type,
		zid_report_desc_t zid_report_desc, uint8_t RxLinkQuality,
		uint8_t RxFlags)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();
	*msg_buf++ = 4 + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = ZID_GET_REPORT_INDICATION;
	*msg_buf++ = PairingRef;
	*msg_buf++ = zid_report_type;
	*msg_buf++ = zid_report_desc;
	RxLinkQuality = RxLinkQuality;
	RxFlags =  RxFlags;
	*msg_buf = EOT;
}

#endif

#endif /* SIO_HUB */
/* EOF */
