/**
 * @file api_parser.c
 *
 * @brief RF4CE API parser.
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

/**
 * \ingroup  group_api_parser_rf4ce
 * @{
 */
#ifdef SIO_HUB

/* === INCLUDES ============================================================ */

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "compiler.h"
#include "nwk_msg_code.h"
#include "app_config.h"
#include "conf_sio2ncp.h"
#include "sio2ncp.h"
#include "api_parser.h"
#include "rf4ce.h"
#ifdef ZRC_PROFILE
#include "pb_pairing.h"
#include "zrc.h"
#endif
#include "vendor_data.h"
#include "nwk_config.h"

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
 * A UART state that expects the length to be received as the next character.
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

#define SIO_RX2NCP_BUF_SIZE             (200)
#define SIO_TX_BUF_SIZE                 SIO_RX2NCP_BUF_SIZE

/* === Globals ============================================================= */

/**
 * This is the receive buffer of the UART.
 */
static uint8_t sio_rx_buf[SIO_RX2NCP_BUF_SIZE];

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
static uint8_t data[20];
static uint8_t data_length;
static uint8_t rx_index;
static uint8_t head;
static uint8_t buf_count;
static uint8_t protocol_id;
static zrc_indication_callback_t zrc_ind_cb;
static nwk_indication_callback_t nwk_ind_callback;
/* === Prototypes ========================================================== */

static inline void process_incoming_data(void);

/* nwk_enum_t nwk_init(void); */
static uint8_t *get_next_tx_buffer(void);
static inline void handle_incoming_msg(void);

/*
 * bool nlme_reset_request(bool SetDefaultNIB,
 *              FUNC_PTR confirm_cb
 *              );
 * bool nlde_data_request(uint8_t PairingRef, profile_id_t ProfileId,
 *              uint16_t VendorId, uint8_t nsduLength, uint8_t *nsdu,
 *              uint8_t TxOptions,
 *              uint8_t Handle,
 *              FUNC_PTR confirm_cb
 *              );
 * bool nlme_auto_discovery_request(uint8_t RecAppCapabilities,
 *              dev_type_t RecDevTypeList[DEVICE_TYPE_LIST_SIZE],
 *              profile_id_t RecProfileIdList[PROFILE_ID_LIST_SIZE],
 *              uint32_t AutoDiscDuration,
 *              FUNC_PTR confirm_cb
 *              );
 * bool nlme_discovery_request(uint16_t DstPANId, uint16_t DstNwkAddr,
 *              uint8_t OrgAppCapabilities,
 *              dev_type_t OrgDevTypeList[3],
 *              profile_id_t OrgProfileIdList[7],
 *              dev_type_t SearchDevType,
 *              uint8_t DiscProfileIdListSize,
 *              profile_id_t DiscProfileIdList[7],
 *              uint32_t DiscDuration,
 *              FUNC_PTR confirm_cb
 *              );
 *
 * bool nlme_discovery_response(nwk_enum_t Status, uint64_t DstIEEEAddr,
 *              uint8_t RecAppCapabilities, dev_type_t RecDevTypeList[3],
 *              profile_id_t RecProfileIdList[7], uint8_t DiscReqLQI);
 * bool nlme_pair_request(uint8_t LogicalChannel, uint16_t DstPANId,
 *              uint64_t DstIEEEAddr,
 *              uint8_t OrgAppCapabilities,
 *              dev_type_t OrgDevTypeList[DEVICE_TYPE_LIST_SIZE],
 *              profile_id_t OrgProfileIdList[PROFILE_ID_LIST_SIZE],
 *              uint8_t KeyExTransferCount,
 *              FUNC_PTR confirm_cb
 *              );
 * bool nlme_pair_response(nwk_enum_t Status, uint16_t DstPANId,
 *              uint64_t DstIEEEAddr,
 *              uint8_t RecAppCapabilities, dev_type_t RecDevTypeList[3],
 *              profile_id_t RecProfileIdList[7], uint8_t ProvPairingRef);
 * bool nlme_get_request(nib_attribute_t NIBAttribute, uint8_t
 * NIBAttributeIndex,
 *              FUNC_PTR confirm_cb
 *              );
 * bool nlme_rx_enable_request(uint32_t RxOnDuration,
 *              FUNC_PTR confirm_cb
 *              );
 * bool nlme_set_request(nib_attribute_t NIBAttribute, uint8_t
 * NIBAttributeIndex,
 *              uint8_t *NIBAttributeValue,
 *              FUNC_PTR confirm_cb
 *              );
 * bool nlme_start_request(
 *              FUNC_PTR confirm_cb);
 *
 * bool nlme_unpair_request(uint8_t PairingRef,
 *              FUNC_PTR confirm_cb
 *              );
 * bool nlme_unpair_response(uint8_t PairingRef);
 *
 * bool nlme_update_key_request(uint8_t PairingRef, uint8_t NewLinkKey[16],
 *              FUNC_PTR confirm_cb
 *              );
 * bool nwk_ch_agility_request(nwk_agility_mode_t AgilityMode,
 *              FUNC_PTR confirm_cb
 *              );
 * bool pbp_org_pair_request(uint8_t OrgAppCapabilities,
 *              dev_type_t OrgDevTypeList[DEVICE_TYPE_LIST_SIZE],
 *              profile_id_t OrgProfileIdList[PROFILE_ID_LIST_SIZE],
 *              dev_type_t SearchDevType, uint8_t DiscProfileIdListSize,
 *              profile_id_t DiscProfileIdList[PROFILE_ID_LIST_SIZE],
 *              FUNC_PTR confirm_cb
 *              );
 * bool pbp_rec_pair_request(uint8_t RecAppCapabilities,
 *              dev_type_t RecDevTypeList[3],
 *              profile_id_t RecProfileIdList[7],
 *              FUNC_PTR confirm_cb
 *              );
 * bool zrc_cmd_request(uint8_t PairingRef, uint16_t VendorId,
 *              zrc_cmd_code_t CmdCode,
 *              uint8_t CmdLength, uint8_t *Cmd, uint8_t TxOptions,
 *              FUNC_PTR confirm_cb
 *              );
 * bool zrc_cmd_disc_request(uint8_t PairingRef,
 *              FUNC_PTR confirm_cb
 *              );
 * bool zrc_cmd_disc_response(uint8_t PairingRef, uint8_t *SupportedCmd);
 * void register_nwk_indication_callback(nwk_indication_callback_t *nwk_ind_cb);
 * void register_zrc_indication_callback(
 *              zrc_indication_callback_t *zrc_ind_callback);
 */
/* === Externals ==========================================================  */
/* Network confirmation callback pointers */

nlme_start_confirm_cb_t nlme_start_confirm_cb;
nlme_reset_confirm_cb_t nlme_reset_confirm_cb;
nlme_rx_enable_confirm_cb_t nlme_rx_enable_confirm_cb;
nlde_data_confirm_cb_t nlde_data_confirm_cb;

#if (defined RF4CE_TARGET) || (defined RF4CE_PLATFORM)
nlme_auto_discovery_confirm_cb_t nlme_auto_discovery_confirm_cb;
#endif
#if (NWK_DISCOVERY_REQ_CONF == 1)
nlme_discovery_confirm_cb_t nlme_discovery_confirm_cb;
#endif
#if (NWK_GET == 1)
nlme_get_confirm_cb_t nlme_get_confirm_cb;
#endif
#if (NWK_PAIR_REQ_CONF == 1)
nlme_pair_confirm_cb_t nlme_pair_confirm_cb;
#endif
#if (NWK_SET == 1)
nlme_set_confirm_cb_t nlme_set_confirm_cb;
#endif
#if (NWK_UNPAIR_REQ_CONF == 1)
nlme_unpair_confirm_cb_t nlme_unpair_confirm_cb;
#endif
#if (NWK_UPDATE_KEY == 1) || (defined RF4CE_PLATFORM)
nlme_update_key_confirm_cb_t nlme_update_key_confirm_cb;
#endif
#if (defined CHANNEL_AGILITY) || (defined RF4CE_PLATFORM)
nwk_ch_agility_confirm_cb_t nwk_ch_agility_confirm_cb;
#endif
pbp_org_pair_confirm_cb_t pbp_org_pair_confirm_cb;
pbp_rec_pair_confirm_cb_t pbp_rec_pair_confirm_cb;
#ifndef RF4CE_TARGET
zrc_cmd_confirm_cb_t zrc_cmd_confirm_cb;
#endif
zrc_cmd_disc_confirm_cb_t zrc_cmd_disc_confirm_cb;

/** Gets attribute size by reading from a table stored in flash */
#define GET_NIB_ATTR_SIZE(x)    PGM_READ_BYTE( \
		&nib_attr_size[x - nwkActivePeriod])

/* === Globals ============================================================= */

#if (NWK_SET == 1) || (NWK_GET == 1)
/** Size constants for NIB attributes */
static FLASH_DECLARE(uint8_t nib_attr_size[]) = {
	sizeof(uint32_t),           /* 0x60: nwkActivePeriod */
	sizeof(uint8_t),            /* 0x61: nwkBaseChannel */
	sizeof(uint8_t),            /* 0x62: nwkDiscoveryLQIThreshold */
	sizeof(uint32_t),           /* 0x63: nwkDiscoveryRepetitionInterval */
	sizeof(uint32_t),           /* 0x64: nwkDutyCycle */
	sizeof(uint32_t),           /* 0x65: nwkFrameCounter */
	sizeof(bool),               /* 0x66: nwkIndicateDiscoveryRequests */
	sizeof(bool),               /* 0x67: nwkInPowerSave */
	sizeof(pairing_table_t),    /* 0x68: nwkPairingTable */
	sizeof(uint8_t),            /* 0x69: nwkMaxDiscoveryRepetitions */
	sizeof(uint8_t),            /* 0x6A: nwkMaxFirstAttemptCSMABackoffs */
	sizeof(uint8_t),            /* 0x6B: nwkMaxFirstAttemptFrameRetries */
	sizeof(uint8_t),            /* 0x6C: nwkMaxReportedNodeDescriptors */
	sizeof(uint32_t),           /* 0x6D: nwkResponseWaitTime */
	sizeof(uint8_t),            /* 0x6E: nwkScanDuration */
	(15 * sizeof(uint8_t)),     /* 0x6F: nwkUserString */
	sizeof(uint64_t),           /* 0x70: nwkPrivateIeeeAddr */
	sizeof(uint16_t),           /* 0x71: nwkPrivateVendorIdentifier */
	(7 * sizeof(uint8_t)),      /* 0x72: nwkPrivateVendorString */
	sizeof(uint8_t),            /* 0x73: nwkPrivateNodeCapabilities */
	sizeof(uint16_t),           /* 0x74: nwkPrivatePanIdentifier */
	sizeof(uint16_t),           /* 0x75: nwkPrivateShortAddress */
	sizeof(uint8_t),            /* 0x76: nwkPrivateMaxPairingTableEntries */
	sizeof(bool),               /* 0x77: nwkChAgEnabled */
	sizeof(uint32_t),           /* 0x78: nwkPrivateChAgScanInterval */
	sizeof(uint8_t)             /* 0x79: nwkPrivateChAgEdThreshold */
#ifdef ZRC_PROFILE
	,
	sizeof(uint8_t),            /* 0x80: aplKeyRepeatInterval */
	sizeof(uint8_t),            /* 0x81: aplKeyRepeatWaitTime */
	sizeof(uint8_t)             /* 0x82: aplKeyExchangeTransferCount */
#endif
};
#endif

/**
 * @brief This function does the initialization of the SIO or UART.
 */
void serial_api_init(void)
{
	sio_rx_state = UART_RX_STATE_SOT;
	sio2ncp_init();
}

nwk_enum_t nwk_init(void)
{
	serial_api_init();
	return NWK_SUCCESS;
}

/**
 * @brief Function to handle the state machine serial data exchange.
 */
bool nwk_task(void)
{
	/* Rx processing */
	if (data_length == 0) {
		/* No data to process, read the stream IO */
		rx_index = 0;
		data_length = sio2ncp_rx(data, SIO_RX2NCP_BUF_SIZE);
	} else { /* Data has been received, process the data */
		 /* Process each single byte */
		process_incoming_data();
		data_length--;
		rx_index++;
	}

	/* Tx processing */
	if (buf_count != 0) {
		if (sio2ncp_tx(sio_tx_buf[head],
				(sio_tx_buf[head][1] + 3)) != 0) {
			head++;
			head %= SIO_BUF_COUNT;
			buf_count--;
		} else {
			/* @ToDo what happens if none or only a part of the
			 * bytes could be transmitted? */
		}
	}

	return true;
}

/*
 * @brief Process data received from SIO
 */
static inline void process_incoming_data(void)
{
	switch (sio_rx_state) {
	case UART_RX_STATE_SOT:
		sio_rx_ptr = sio_rx_buf;
		if (SOT == data[rx_index]) {
			sio_rx_state = UART_RX_STATE_LENGTH;
		}

		break;

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
	switch (sio_rx_buf[1]) { /* message type */
	case NLME_RESET_CONFIRM:
	{
		nlme_reset_confirm_cb((nwk_enum_t)sio_rx_buf[2]);
	}
	break;

	case NLDE_DATA_CONFIRM:
	{
		nlde_data_confirm_cb((nwk_enum_t)sio_rx_buf[2], sio_rx_buf[3],
				(profile_id_t)sio_rx_buf[4], 1);
	}
	break;

	case NLME_AUTO_DISCOVERY_CONFIRM:
	{
		uint64_t SrcIEEEAddr;
		MEMCPY_ENDIAN(&SrcIEEEAddr, &sio_rx_buf[3], 8);

		nlme_auto_discovery_confirm_cb((nwk_enum_t)sio_rx_buf[2],
				SrcIEEEAddr);
	}
	break;

	case NLME_DISCOVERY_CONFIRM:
	{
		node_desc_t NodeDescList[3];
		uint8_t i;
		uint8_t k;
		uint8_t numnodes = sio_rx_buf[3];
		uint8_t index = 5;
		for (i = 0; i < numnodes; i++) {
			NodeDescList[i].Status
				= (nwk_enum_t)sio_rx_buf[index++];
			NodeDescList[i].LogicalChannel = sio_rx_buf[index++];
			NodeDescList[i].PANId = sio_rx_buf[index++];
			NodeDescList[i].PANId |=  (sio_rx_buf[index++] << 8);
			MEMCPY_ENDIAN(&NodeDescList[i].IEEEAddr,
					&sio_rx_buf[index], 8);
			index += 8;
			NodeDescList[i].NodeCapabilities = sio_rx_buf[index++]; /*
				                                                 *
				                                                 *17
				                                                 **/
			NodeDescList[i].VendorId = sio_rx_buf[index++];
			NodeDescList[i].VendorId |= (sio_rx_buf[index++] << 8);
			for (k = 0; k < 7; k++) {
				NodeDescList[i].VendorString[k]
					= sio_rx_buf[index++];                 /*
					                                        *
					                                        *20
					                                        **/
			}
			NodeDescList[i].AppCapabilities = sio_rx_buf[index++]; /*
				                                                *
				                                                *27
				                                                **/
			for (k = 0; k < 15; k++) {
				NodeDescList[i].UserString[k]
					= sio_rx_buf[index++];               /*
					                                      *
					                                      *28
					                                      **/
			}
			for (k = 0; k < DEVICE_TYPE_LIST_SIZE; k++) {
				NodeDescList[i].DevTypeList[k]
					= (dev_type_t)sio_rx_buf
						[index++];                                 /*
					                                                    *
					                                                    *43
					                                                    **/
			}
			for (k = 0; k < PROFILE_ID_LIST_SIZE; k++) {
				NodeDescList[i].ProfileIdList[k]
					= (profile_id_t)
						sio_rx_buf[index++];                          /*
					                                                       *
					                                                       *46
					                                                       **/
			}
			NodeDescList[i].DiscReqLQI = sio_rx_buf[index++]; /* 53
				                                          **/
		}

		nlme_discovery_confirm_cb((nwk_enum_t)sio_rx_buf[2],
				sio_rx_buf[3], NodeDescList);
	}
	break;

	case NLME_GET_CONFIRM:
	{
		nlme_get_confirm_cb((nwk_enum_t)sio_rx_buf[2],
				(nib_attribute_t)sio_rx_buf[3],
				sio_rx_buf[4], (void *)&sio_rx_buf[6]);
	}
	break;

	case NLME_PAIR_CONFIRM:
	{
		uint8_t RecVendorString[7];
		for (uint8_t i = 0; i < 7; i++) {
			RecVendorString[i] = (uint8_t)sio_rx_buf[6 + i];
		}
		uint8_t RecUserString[15];
		uint8_t index = 14;
		for (uint8_t i = 0; i < 15; i++) {
			RecUserString[i] = (uint8_t)sio_rx_buf[index++];
		}
		dev_type_t RecDevTypeList[DEVICE_TYPE_LIST_SIZE];
		for (uint8_t i = 0; i < DEVICE_TYPE_LIST_SIZE; i++) {
			RecDevTypeList[i] = (dev_type_t)sio_rx_buf[index++];
		}
		profile_id_t RecProfileIdList[PROFILE_ID_LIST_SIZE];
		for (uint8_t i = 0; i < PROFILE_ID_LIST_SIZE; i++) {
			RecProfileIdList[i] = (profile_id_t)sio_rx_buf[index++];
		}

		nlme_pair_confirm_cb((nwk_enum_t)sio_rx_buf[2], sio_rx_buf[3],
				((uint16_t)sio_rx_buf[4] |
				((uint16_t)sio_rx_buf[5] << 8)),   /* Rec Vendor
			                                            * Id// */
				RecVendorString, /* uint8_t RecVendorString[7],
			                         **/
				sio_rx_buf[6 + 7], /* RecAppCapabilities */
				RecUserString,
				RecDevTypeList,
				RecProfileIdList);
	}
	break;

	case NLME_RX_ENABLE_CONFIRM:
	{
		nlme_rx_enable_confirm_cb((nwk_enum_t)sio_rx_buf[2]);
	}
	break;

	case NLME_SET_CONFIRM:
	{
		nlme_set_confirm_cb((nwk_enum_t)sio_rx_buf[2],
				(nib_attribute_t)sio_rx_buf[3],
				sio_rx_buf[4]);
	}
	break;

	case NLME_START_CONFIRM:
	{
		nlme_start_confirm_cb((nwk_enum_t)sio_rx_buf[2]);
	}
	break;

	case NLME_UNPAIR_CONFIRM:
	{
		nlme_unpair_confirm_cb((nwk_enum_t)sio_rx_buf[2],
				sio_rx_buf[3]);
	}
	break;

	case NLME_UPDATE_KEY_CONFIRM:
	{
		nlme_update_key_confirm_cb((nwk_enum_t)sio_rx_buf[2],
				sio_rx_buf[3]);
	}
	break;

	case NWK_CH_AGILITY_CONFIRM:
	{
		nwk_ch_agility_confirm_cb((nwk_enum_t)sio_rx_buf[2],
				(bool)sio_rx_buf[3], sio_rx_buf[4]);
	}
	break;

	case PBP_ORG_PAIR_CONFIRM:
	{
		pbp_org_pair_confirm_cb((nwk_enum_t)sio_rx_buf[2],
				sio_rx_buf[3]);
	}
	break;

	case PBP_REC_PAIR_CONFIRM:
	{
		pbp_rec_pair_confirm_cb((nwk_enum_t)sio_rx_buf[2],
				sio_rx_buf[3]);
	}
	break;

	case ZRC_CMD_CONFIRM:
	{
		zrc_cmd_confirm_cb((nwk_enum_t)sio_rx_buf[2], sio_rx_buf[3],
				(cec_code_t)sio_rx_buf[4]);
	}
	break;

	case ZRC_CMD_DISCOVERY_CONFIRM:
	{
		zrc_cmd_disc_confirm_cb((nwk_enum_t)sio_rx_buf[2],
				sio_rx_buf[3], &sio_rx_buf[4]);
	}
	break;

	case NLDE_DATA_INDICATION:
	{
		if (nwk_ind_callback.nlde_data_indication_cb == NULL) {
			return;
		}

		nwk_ind_callback.nlde_data_indication_cb(
				sio_rx_buf[2], (profile_id_t)sio_rx_buf[3],
				((uint16_t)sio_rx_buf[4] |
				((uint16_t)sio_rx_buf[5] << 8)),   /* Vendor
			                                           * Id// */
				sio_rx_buf[8], /* nsdu length */
				&sio_rx_buf[9],
				sio_rx_buf[6], /* rx link quality */
				sio_rx_buf[7] /* rx flags */
				);
	}
	break;

	case NLME_COMM_STATUS_INDICATION:
	{
		if (nwk_ind_callback.nlme_comm_status_indication_cb == NULL) {
			return;
		}

		uint64_t SrcIEEEAddr;
		MEMCPY_ENDIAN(&SrcIEEEAddr, &sio_rx_buf[7], 8);
		nwk_ind_callback.nlme_comm_status_indication_cb(
				(nwk_enum_t)sio_rx_buf[2], sio_rx_buf[3],
				((uint16_t)sio_rx_buf[4] |
				((uint16_t)sio_rx_buf[5] << 8)),       /*
			                                                *
			                                                *DstPANId//
			                                                **/
				sio_rx_buf[6],
				SrcIEEEAddr);
	}
	break;

	case NLME_DISCOVERY_INDICATION:
	{
		if (nwk_ind_callback.nlme_discovery_indication_cb == NULL) {
			return;
		}

		uint64_t DstIEEEAddr;
		MEMCPY_ENDIAN(&DstIEEEAddr, &sio_rx_buf[3], 8);
		nwk_ind_callback.nlme_discovery_indication_cb(
				(nwk_enum_t)sio_rx_buf[2], DstIEEEAddr, /*
			                                                 *
			                                                 *DstIEEEaddr
			                                                 **/
				sio_rx_buf[11],
				((uint16_t)sio_rx_buf[12] |
				((uint16_t)sio_rx_buf[13] << 8)),
				(uint8_t *)&sio_rx_buf[14], sio_rx_buf[21],
				(uint8_t *)&sio_rx_buf[22],
				(dev_type_t *)&sio_rx_buf[37],
				(profile_id_t *)&sio_rx_buf[40],
				(dev_type_t)sio_rx_buf[47],
				sio_rx_buf[48]);
	}
	break;

	case NLME_PAIR_INDICATION:
	{
		uint8_t OrgVendorString[7];
		for (uint8_t i = 0; i < 7; i++) {
			OrgVendorString[i] = (uint8_t)sio_rx_buf[16 + i];
		}
		uint8_t OrgUserString[15];
		for (uint8_t i = 0; i < 7; i++) {
			OrgUserString[i] = (uint8_t)sio_rx_buf[24 + i];
		}
		dev_type_t OrgDevTypeList[3];
		for (uint8_t i = 0; i < 3; i++) {
			OrgDevTypeList[i] = (dev_type_t)sio_rx_buf[39 + i];
		}
		profile_id_t OrgProfileIdList[7];
		for (uint8_t i = 0; i < 3; i++) {
			OrgProfileIdList[i] = (profile_id_t)sio_rx_buf[42 + i];
		}
		uint64_t SrcIEEEAddr;
		MEMCPY_ENDIAN(&SrcIEEEAddr, &sio_rx_buf[5], 8);

		if (nwk_ind_callback.nlme_pair_indication_cb == NULL) {
			return;
		}

		nwk_ind_callback.nlme_pair_indication_cb(
				(nwk_enum_t)sio_rx_buf[2],
				((uint16_t)sio_rx_buf[3] |
				((uint16_t)sio_rx_buf[4] << 8)),
				SrcIEEEAddr,
				sio_rx_buf[13],
				((uint16_t)sio_rx_buf[14] |
				((uint16_t)sio_rx_buf[15] << 8)),
				OrgVendorString, sio_rx_buf[23], OrgUserString,
				OrgDevTypeList, OrgProfileIdList,
				sio_rx_buf[49],
				sio_rx_buf[50]);
	}
	break;

	case NLME_UNPAIR_INDICATION:
	{
		if (nwk_ind_callback.nlme_unpair_indication_cb == NULL) {
			return;
		}

		nwk_ind_callback.nlme_unpair_indication_cb(
				sio_rx_buf[2]); /* pairing ref */
	}
	break;

	case NWK_CH_AGILITY_INDICATION:
	{
		if (nwk_ind_callback.nwk_ch_agility_indication_cb == NULL) {
			return;
		}

		nwk_ind_callback.nwk_ch_agility_indication_cb(
				sio_rx_buf[2]); /* logical channel */
	}
	break;

	case ZRC_CMD_INDICATION:
	{
		if (zrc_ind_cb.zrc_cmd_indication_cb == NULL) {
			return;
		}

		zrc_ind_cb.zrc_cmd_indication_cb(
				sio_rx_buf[2], /* pairing ref */
				sio_rx_buf[5], /* nsdulength */
				&sio_rx_buf[6],
				sio_rx_buf[3], /* rxlink quality */
				sio_rx_buf[4] /* rxflags */
				);
	}
	break;

	case ZRC_CMD_DISCOVERY_INDICATION:
	{
		if (zrc_ind_cb.zrc_cmd_disc_indication_cb == NULL) {
			return;
		}

		zrc_ind_cb.zrc_cmd_disc_indication_cb(

				sio_rx_buf[2] /* pairing ref */

				);
	}
	break;

	case VENDOR_DATA_INDICATION:
	{
		zrc_ind_cb.vendor_data_ind_cb(

				sio_rx_buf[2], /* pairing ref */
				((uint16_t)sio_rx_buf[4] |
				((uint16_t)sio_rx_buf[5] << 8)),   /* vendor id
			                                           **/
				sio_rx_buf[8], /* nsdu length */
				&sio_rx_buf[9],
				sio_rx_buf[6], /* rx link quality */
				sio_rx_buf[7] /* rx flags */
				);
	}
	break;
	}
}

bool nlme_reset_request(bool SetDefaultNIB,
		FUNC_PTR confirm_cb
		)
{
	uint8_t *msg_buf;
	msg_buf = get_next_tx_buffer();

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return false;
	}

	if (confirm_cb == NULL) {
		return false;
	}

	nlme_reset_confirm_cb = (nlme_reset_confirm_cb_t)confirm_cb;

	*msg_buf++ = NLME_RESET_REQ_LEN + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = NLME_RESET_REQUEST;
	*msg_buf++ = SetDefaultNIB;
	*msg_buf = EOT;
	return true;
}

bool nlde_data_request(uint8_t PairingRef, profile_id_t ProfileId,
		uint16_t VendorId, uint8_t nsduLength, uint8_t *nsdu,
		uint8_t TxOptions,
		uint8_t Handle,
		FUNC_PTR confirm_cb
		)
{
	uint8_t *msg_buf;
	uint8_t *ptr;
	msg_buf = get_next_tx_buffer();
	if (NULL == msg_buf) {
		return false;
	}

	if (confirm_cb == NULL) {
		return false;
	}

	nlde_data_confirm_cb = (nlde_data_confirm_cb_t)confirm_cb;

	*msg_buf++ = NLDE_DATA_REQ_LEN + RF4CONTROL_PID_LEN + nsduLength;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = NLDE_DATA_REQUEST;
	*msg_buf++ = PairingRef;
	*msg_buf++ = ProfileId;
	*msg_buf++ = (uint8_t)VendorId; /* LSB */
	*msg_buf++ = (uint8_t)(VendorId >> 8); /* MSB */
	*msg_buf++ = TxOptions;
	*msg_buf++ = nsduLength;
	ptr = nsdu;
	/* @ToDo use memcpy */
	/* for (uint8_t i = 0; i < nsduLength; i++) */
	/* { */
	/* *msg_buf++ = *ptr++; */
	/* } */
	memcpy(msg_buf, ptr, ((sizeof(uint8_t)) * nsduLength));
	msg_buf += (sizeof(uint8_t)) * nsduLength;
	*msg_buf = EOT;
	Handle = Handle;
	return true;
}

/* The NLME-AUTO-DISCOVERY.request primitive allows the application to request
 * the NLME automatically handles the receipt of discovery request command
 * frames. */

bool nlme_auto_discovery_request(uint8_t RecAppCapabilities,
		dev_type_t RecDevTypeList[DEVICE_TYPE_LIST_SIZE],
		profile_id_t RecProfileIdList[PROFILE_ID_LIST_SIZE],
		uint32_t AutoDiscDuration,
		FUNC_PTR confirm_cb
		)
{
	uint8_t *msg_buf;
	uint8_t i;
	msg_buf = get_next_tx_buffer();

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return false;
	}

	if (confirm_cb == NULL) {
		return false;
	}

	nlme_auto_discovery_confirm_cb
		= (nlme_auto_discovery_confirm_cb_t)confirm_cb;

	*msg_buf++ = NLME_AUTO_DISCOVERY_REQ_LEN + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = NLME_AUTO_DISCOVERY_REQUEST;
	*msg_buf++ = RecAppCapabilities;

	for (i = 0; i < DEVICE_TYPE_LIST_SIZE; i++) {
		*msg_buf++ = RecDevTypeList[i];
	}
	for (i = 0; i < PROFILE_ID_LIST_SIZE; i++) {
		*msg_buf++ = RecProfileIdList[i];
	}
	MEMCPY_ENDIAN(msg_buf, &AutoDiscDuration, sizeof(uint32_t));
	msg_buf += sizeof(uint32_t);
	*msg_buf = EOT;
	return true;
}

bool nlme_discovery_request(uint16_t DstPANId, uint16_t DstNwkAddr,
		uint8_t OrgAppCapabilities,
		dev_type_t OrgDevTypeList[3],
		profile_id_t OrgProfileIdList[7],
		dev_type_t SearchDevType,
		uint8_t DiscProfileIdListSize,
		profile_id_t DiscProfileIdList[7],
		uint32_t DiscDuration,
		FUNC_PTR confirm_cb
		)
{
	uint8_t *msg_buf;
	uint8_t i;
	msg_buf = get_next_tx_buffer();

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return false;
	}

	if (confirm_cb == NULL) {
		return false;
	}

	nlme_discovery_confirm_cb = (nlme_discovery_confirm_cb_t)confirm_cb;

	*msg_buf++ = NLME_DISCOVERY_REQ_LEN + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = NLME_DISCOVERY_REQUEST;
	*msg_buf++ = (uint8_t)DstPANId; /* LSB */
	*msg_buf++ = (uint8_t)(DstPANId >> 8); /* MSB */
	*msg_buf++ = (uint8_t)DstNwkAddr; /* LSB */
	*msg_buf++ = (uint8_t)(DstNwkAddr >> 8); /* MSB */
	*msg_buf++ = OrgAppCapabilities;
	for (i = 0; i < DEVICE_TYPE_LIST_SIZE; i++) {
		*msg_buf++ = OrgDevTypeList[i];
	}
	for (i = 0; i < PROFILE_ID_LIST_SIZE; i++) {
		*msg_buf++ = OrgProfileIdList[i];
	}

	*msg_buf++ = SearchDevType;
	*msg_buf++ = DiscProfileIdListSize;
	for (i = 0; i < PROFILE_ID_LIST_SIZE; i++) {
		*msg_buf++ = DiscProfileIdList[i];
	}

	memcpy(msg_buf, &DiscDuration, sizeof(uint32_t));
	msg_buf += sizeof(uint32_t);
	*msg_buf = EOT;
	return true;
}

bool nlme_discovery_response(nwk_enum_t Status, uint64_t DstIEEEAddr,
		uint8_t RecAppCapabilities, dev_type_t RecDevTypeList[3],
		profile_id_t RecProfileIdList[7], uint8_t DiscReqLQI)
{
	uint8_t *msg_buf;
	uint8_t i;
	msg_buf = get_next_tx_buffer();

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return false;
	}

	*msg_buf++ = NLME_DISCOVERY_RESP_LEN + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = NLME_DISCOVERY_RESPONSE;
	*msg_buf++ = Status;
	MEMCPY_ENDIAN(msg_buf, &DstIEEEAddr, sizeof(DstIEEEAddr));
	msg_buf += sizeof(uint64_t);
	*msg_buf++ = RecAppCapabilities;

	for (i = 0; i < DEVICE_TYPE_LIST_SIZE; i++) {
		*msg_buf++ = RecDevTypeList[i];
	}
	for (i = 0; i < PROFILE_ID_LIST_SIZE; i++) {
		*msg_buf++ = RecProfileIdList[i];
	}
	*msg_buf++ = DiscReqLQI;
	*msg_buf = EOT;
	return true;
}

/* The NLME-PAIR.request primitive allows the application to request the NLME
 * pair
 * with another device. This primitive would normally be issued following a
 * discovery
 * operation via the NLME-DISCOVERY.request primitive. */

bool nlme_pair_request(uint8_t LogicalChannel, uint16_t DstPANId,
		uint64_t DstIEEEAddr,
		uint8_t OrgAppCapabilities,
		dev_type_t OrgDevTypeList[DEVICE_TYPE_LIST_SIZE],
		profile_id_t OrgProfileIdList[PROFILE_ID_LIST_SIZE],
		uint8_t KeyExTransferCount,
		FUNC_PTR confirm_cb
		)
{
	uint8_t *msg_buf;
	uint8_t i;
	msg_buf = get_next_tx_buffer();

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return false;
	}

	if (confirm_cb == NULL) {
		return false;
	}

	nlme_pair_confirm_cb = (nlme_pair_confirm_cb_t)confirm_cb;

	*msg_buf++ = NLME_PAIR_REQ_LEN + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = NLME_PAIR_REQUEST;
	*msg_buf++ = LogicalChannel;
	*msg_buf++ = (uint8_t)DstPANId; /* LSB */
	*msg_buf++ = (uint8_t)(DstPANId >> 8); /* MSB */
	memcpy(msg_buf, &DstIEEEAddr, sizeof(DstIEEEAddr));
	msg_buf += sizeof(uint64_t);
	*msg_buf++ = OrgAppCapabilities;

	for (i = 0; i < DEVICE_TYPE_LIST_SIZE; i++) {
		*msg_buf++ = OrgDevTypeList[i];
	}
	for (i = 0; i < PROFILE_ID_LIST_SIZE; i++) {
		*msg_buf++ = OrgProfileIdList[i];
	}
	*msg_buf++ = KeyExTransferCount;
	*msg_buf = EOT;
	return true;
}

bool nlme_pair_response(nwk_enum_t Status, uint16_t DstPANId,
		uint64_t DstIEEEAddr,
		uint8_t RecAppCapabilities, dev_type_t RecDevTypeList[3],
		profile_id_t RecProfileIdList[7], uint8_t ProvPairingRef)
{
	uint8_t *msg_buf;
	uint8_t i;
	msg_buf = get_next_tx_buffer();

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return false;
	}

	*msg_buf++ = NLME_PAIR_RESP_LEN + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = NLME_PAIR_RESPONSE;
	*msg_buf++ = Status;
	*msg_buf++ = (uint8_t)DstPANId; /* LSB */
	*msg_buf++ = (uint8_t)(DstPANId >> 8); /* MSB */
	MEMCPY_ENDIAN(msg_buf, &DstIEEEAddr, sizeof(DstIEEEAddr));
	msg_buf += sizeof(uint64_t);
	*msg_buf++ = RecAppCapabilities;

	for (i = 0; i < DEVICE_TYPE_LIST_SIZE; i++) {
		*msg_buf++ = RecDevTypeList[i];
	}
	for (i = 0; i < PROFILE_ID_LIST_SIZE; i++) {
		*msg_buf++ = RecProfileIdList[i];
	}
	*msg_buf++ = ProvPairingRef;
	*msg_buf = EOT;
	return true;
}

/* The NLME-GET.request primitive allows the application to request the value of
 * a
 * NIB attribute from the NLME. */

bool nlme_get_request(nib_attribute_t NIBAttribute, uint8_t NIBAttributeIndex,
		FUNC_PTR confirm_cb
		)
{
	uint8_t *msg_buf;
	msg_buf = get_next_tx_buffer();

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return false;
	}

	if (confirm_cb == NULL) {
		return false;
	}

	nlme_get_confirm_cb = (nlme_get_confirm_cb_t)confirm_cb;

	*msg_buf++ = NLME_GET_REQ_LEN + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = NLME_GET_REQUEST;
	*msg_buf++ = NIBAttribute;
	*msg_buf++ = NIBAttributeIndex;
	*msg_buf = EOT;
	return true;
}

/* The NLME-RX-ENABLE.request primitive allows the application to request that
 * the
 * receiver is either enabled (for a finite period or until further notice) or
 * disabled. */
bool nlme_rx_enable_request(uint32_t RxOnDuration,
		FUNC_PTR confirm_cb
		)
{
	uint8_t *msg_buf;
	msg_buf = get_next_tx_buffer();

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return false;
	}

	if (confirm_cb == NULL) {
		return false;
	}

	nlme_rx_enable_confirm_cb = (nlme_rx_enable_confirm_cb_t)confirm_cb;

	*msg_buf++ = NLME_RX_ENABLE_REQ_LEN + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = NLME_RX_ENABLE_REQUEST;
	MEMCPY_ENDIAN(msg_buf, &RxOnDuration, sizeof(RxOnDuration));
	msg_buf += sizeof(uint32_t);
	*msg_buf = EOT;
	return true;
}

bool nlme_set_request(nib_attribute_t NIBAttribute, uint8_t NIBAttributeIndex, uint8_t *NIBAttributeValue

		, FUNC_PTR confirm_cb

		)
{
	uint8_t *msg_buf;
	uint8_t i;
	uint8_t attr_len;
	uint8_t *ptr;
	ptr = NIBAttributeValue;
	msg_buf = get_next_tx_buffer();
	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return false;
	}

	if (confirm_cb == NULL) {
		return false;
	}

	nlme_set_confirm_cb = (nlme_set_confirm_cb_t)confirm_cb;

	attr_len = nwk_get_nib_attribute_size(NIBAttribute);
	*msg_buf++ = NLME_SET_REQ_LEN + RF4CONTROL_PID_LEN + attr_len;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = NLME_SET_REQUEST;
	*msg_buf++ = NIBAttribute;
	*msg_buf++ = NIBAttributeIndex;
	*msg_buf++ = attr_len;
	for (i = 0; i < attr_len; i++) {
		*msg_buf++ = *ptr++;
	}
	*msg_buf = EOT;
	return true;
}

/* The NLME-START.request primitive allows the application to request the NLME
 * start a network.*/
bool nlme_start_request(FUNC_PTR confirm_cb)
{
	uint8_t *msg_buf;
	msg_buf = get_next_tx_buffer();

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return false;
	}

	if (confirm_cb == NULL) {
		return false;
	}

	nlme_start_confirm_cb = (nlme_start_confirm_cb_t)confirm_cb;

	*msg_buf++ = NLME_START_REQ_LEN + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = NLME_START_REQUEST;
	*msg_buf = EOT;
	return true;
}

/* The NLME-UNPAIR.request primitive allows the application to request the NLME
 * removes a pairing link with another device both in the local and remote
 * pairing tables. */

bool nlme_unpair_request(uint8_t PairingRef,
		FUNC_PTR confirm_cb
		)
{
	uint8_t *msg_buf;
	msg_buf = get_next_tx_buffer();

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return false;
	}

	if (confirm_cb == NULL) {
		return false;
	}

	nlme_unpair_confirm_cb = (nlme_unpair_confirm_cb_t)confirm_cb;

	*msg_buf++ = NLME_UNPAIR_REQ_LEN + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = NLME_UNPAIR_REQUEST;
	*msg_buf++ = PairingRef;
	*msg_buf = EOT;
	return true;
}

/* The NLME-UNPAIR.response primitive allows the application to notify the NLME
 * that the pairing link indicated via the NLME-UNPAIR.indication primitive can
 * be
 * removed from the pairing table. */
bool nlme_unpair_response(uint8_t PairingRef)
{
	uint8_t *msg_buf;
	msg_buf = get_next_tx_buffer();

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return false;
	}

	*msg_buf++ = NLME_UNPAIR_RESP_LEN + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = NLME_UNPAIR_RESPONSE;
	*msg_buf++ = PairingRef;
	*msg_buf = EOT;
	return true;
}

/* The NLME-UPDATE-KEY.request primitive allows the application to request the
 * NLME change the security link key of an entry in the pairing table. */

bool nlme_update_key_request(uint8_t PairingRef, uint8_t NewLinkKey[16],
		FUNC_PTR confirm_cb
		)
{
	uint8_t *msg_buf;
	uint8_t *ptr;
	uint8_t i;
	msg_buf = get_next_tx_buffer();

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return false;
	}

	if (confirm_cb == NULL) {
		return false;
	}

	nlme_update_key_confirm_cb = (nlme_update_key_confirm_cb_t)confirm_cb;

	*msg_buf++ = NLME_UPDATE_KEY_REQ_LEN + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = NLME_UPDATE_KEY_REQUEST;
	*msg_buf++ = PairingRef;
	ptr = NewLinkKey;
	for (i = 0; i < 16; i++) {
		*msg_buf++ = *ptr++;
	}
	*msg_buf = EOT;
	return true;
}

/* The function configures the channel agility mode. */

bool nwk_ch_agility_request(nwk_agility_mode_t AgilityMode,
		FUNC_PTR confirm_cb
		)
{
	uint8_t *msg_buf;
	msg_buf = get_next_tx_buffer();

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return false;
	}

	if (confirm_cb == NULL) {
		return false;
	}

	nwk_ch_agility_confirm_cb = (nwk_ch_agility_confirm_cb_t)confirm_cb;

	*msg_buf++ = NWK_CH_AGILITY_REQ_LEN + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = NWK_CH_AGILITY_REQUEST;
	*msg_buf++ = AgilityMode;
	*msg_buf = EOT;
	return true;
}

bool pbp_org_pair_request(uint8_t OrgAppCapabilities,
		dev_type_t OrgDevTypeList[DEVICE_TYPE_LIST_SIZE],
		profile_id_t OrgProfileIdList[PROFILE_ID_LIST_SIZE],
		dev_type_t SearchDevType, uint8_t DiscProfileIdListSize,
		profile_id_t DiscProfileIdList[PROFILE_ID_LIST_SIZE],
		FUNC_PTR confirm_cb

		)
{
	uint8_t *msg_buf;
	uint8_t i;
	msg_buf = get_next_tx_buffer();

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return false;
	}

	if (confirm_cb == NULL) {
		return false;
	}

	pbp_org_pair_confirm_cb = (pbp_org_pair_confirm_cb_t)confirm_cb;

	*msg_buf++ = PBP_ORG_PAIR_REQ_LEN + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = PBP_ORG_PAIR_REQUEST;
	*msg_buf++ = OrgAppCapabilities;

	for (i = 0; i < DEVICE_TYPE_LIST_SIZE; i++) {
		*msg_buf++ = OrgDevTypeList[i];
	}
	for (i = 0; i < PROFILE_ID_LIST_SIZE; i++) {
		*msg_buf++ = OrgProfileIdList[i];
	}

	*msg_buf++ = SearchDevType;
	*msg_buf++ = DiscProfileIdListSize;

	for (i = 0; i < PROFILE_ID_LIST_SIZE; i++) {
		*msg_buf++ = DiscProfileIdList[i];
	}
	*msg_buf = EOT;
	return true;
}

bool pbp_rec_pair_request(uint8_t RecAppCapabilities,
		dev_type_t RecDevTypeList[3],
		profile_id_t RecProfileIdList[7],
		FUNC_PTR confirm_cb
		)
{
	uint8_t *msg_buf;
	uint8_t i;
	msg_buf = get_next_tx_buffer();

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return false;
	}

	if (confirm_cb == NULL) {
		return false;
	}

	pbp_rec_pair_confirm_cb = (pbp_rec_pair_confirm_cb_t)confirm_cb;

	*msg_buf++ = PBP_REC_PAIR_REQ_LEN + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = PBP_REC_PAIR_REQUEST;
	*msg_buf++ = RecAppCapabilities;

	for (i = 0; i < DEVICE_TYPE_LIST_SIZE; i++) {
		*msg_buf++ = RecDevTypeList[i];
	}
	for (i = 0; i < PROFILE_ID_LIST_SIZE; i++) {
		*msg_buf++ = RecProfileIdList[i];
	}
	*msg_buf = EOT;
	return true;
}

/**
 * @brief Handles the key press from the application.
 *
 * @param PairingRef        Pairing reference
 * @param VendorId         Vendor Id information
 * @param CmdCode          ZRC cmd code (Press/Release)
 * @param CmdLength        Length of the rc cmd (rc cmd code + Payload).
 * @param Cmd              Actual rc command code (rc cmd code + Payload)
 * @param TxOptions        Transmission options (ex- ack, security etc)
 *
 * @return true            returns the status of zrc_cmd_request.
 */
bool zrc_cmd_request(uint8_t PairingRef, uint16_t VendorId,
		zrc_cmd_code_t CmdCode,
		uint8_t CmdLength, uint8_t *Cmd, uint8_t TxOptions,
		FUNC_PTR confirm_cb
		)
{
	uint8_t *msg_buf;
	uint8_t *ptr;
	uint8_t i;
	msg_buf = get_next_tx_buffer();

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return false;
	}

	if (confirm_cb == NULL) {
		return false;
	}

	zrc_cmd_confirm_cb = (zrc_cmd_confirm_cb_t)confirm_cb;

	*msg_buf++ = ZRC_CMD_REQ_LEN + RF4CONTROL_PID_LEN + CmdLength;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = ZRC_CMD_REQUEST;
	*msg_buf++ = PairingRef;
	*msg_buf++ = (uint8_t)VendorId; /* LSB */
	*msg_buf++ = (uint8_t)(VendorId >> 8); /* MSB */
	*msg_buf++ = CmdCode;
	*msg_buf++ = TxOptions;
	*msg_buf++ = CmdLength;
	ptr = Cmd;
	/* @ToDo use memcpy */
	for (i = 0; i < CmdLength; i++) {
		*msg_buf++ = *ptr++;
	}

	*msg_buf = EOT;
	return true;
}

/**
 * @brief Handles the cmd discovery request.
 *
 * @param PairingRef    pairing ref of the originator device.
 *
 * @return true         if cmd discovery is sent using nlde_data_req.
 */
bool zrc_cmd_disc_request(uint8_t PairingRef,
		FUNC_PTR confirm_cb
		)
{
	uint8_t *msg_buf;
	msg_buf = get_next_tx_buffer();

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return false;
	}

	if (confirm_cb == NULL) {
		return false;
	}

	zrc_cmd_disc_confirm_cb = (zrc_cmd_disc_confirm_cb_t)confirm_cb;

	*msg_buf++ = ZRC_CMD_DISCOVERY_REQ_LEN + RF4CONTROL_PID_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = ZRC_CMD_DISCOVERY_REQUEST;
	*msg_buf++ = PairingRef;
	*msg_buf = EOT;
	return true;
}

/**
 * @brief Send out the cmd discovery response.
 *
 * @param PairingRef    pairing ref of the originator device.
 * @param SupportedCmd  Address of provided 32 byte array containing supported
 * command codes
 *
 * @return true         if cmd discovery response is sent using nlde_data_req.
 */
bool zrc_cmd_disc_response(uint8_t PairingRef, uint8_t *SupportedCmd)
{
	uint8_t *msg_buf;
	uint8_t i;
	msg_buf = get_next_tx_buffer();

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return false;
	}

	*msg_buf++ = ZRC_CMD_DISCOVERY_RESP_LEN;
	*msg_buf++ = RF4CONTROL_PID;
	*msg_buf++ = ZRC_CMD_DISCOVERY_RESPONSE;
	*msg_buf++ = PairingRef;
	for (i = 0; i < 32; i++) {
		*msg_buf++ = *SupportedCmd++;
	}
	*msg_buf = EOT;
	return true;
}

void register_nwk_indication_callback(nwk_indication_callback_t *nwk_ind_cb)
{
#if (defined CHANNEL_AGILITY) || (defined RF4CE_PLATFORM)
	if (nwk_ind_cb->nwk_ch_agility_indication_cb != NULL) {
		nwk_ind_callback.nwk_ch_agility_indication_cb
			= nwk_ind_cb->nwk_ch_agility_indication_cb;
	}
#endif
#if (defined RF4CE_TARGET) || (defined RF4CE_PLATFORM)
	if (nwk_ind_cb->nlme_pair_indication_cb != NULL) {
		nwk_ind_callback.nlme_pair_indication_cb
			= nwk_ind_cb->nlme_pair_indication_cb;
	}
#endif
#if (NWK_UNPAIR_IND_RESP == 1) || (defined RF4CE_PLATFORM)
	if (nwk_ind_cb->nlme_unpair_indication_cb != NULL) {
		nwk_ind_callback.nlme_unpair_indication_cb
			= nwk_ind_cb->nlme_unpair_indication_cb;
	}
#endif
#if (((defined RF4CE_TARGET) && (NWK_DISCOVERY_IND_RESP == 1)) || \
	(defined RF4CE_PLATFORM))
	if (nwk_ind_cb->nlme_discovery_indication_cb != NULL) {
		nwk_ind_callback.nlme_discovery_indication_cb
			= nwk_ind_cb->nlme_discovery_indication_cb;
	}
#endif
#if (defined RF4CE_TARGET) || (defined RF4CE_PLATFORM)
	if (nwk_ind_cb->nlme_comm_status_indication_cb != NULL) {
		nwk_ind_callback.nlme_comm_status_indication_cb
			= nwk_ind_cb->nlme_comm_status_indication_cb;
	}
#endif
#ifdef ZRC_PROFILE
	if (nwk_ind_cb->zrc_data_indication_cb != NULL) {
		nwk_ind_callback.zrc_data_indication_cb
			= nwk_ind_cb->zrc_data_indication_cb;
	}
#endif
	if (nwk_ind_cb->nlde_data_indication_cb != NULL) {
		nwk_ind_callback.nlde_data_indication_cb
			= nwk_ind_cb->nlde_data_indication_cb;
	}
}

void register_zrc_indication_callback(
		zrc_indication_callback_t *zrc_ind_callback)
{
#if (defined RF4CE_TARGET) || (defined ZRC_CMD_DISCOVERY)
	if (zrc_ind_callback->zrc_cmd_indication_cb != NULL) {
		zrc_ind_cb.zrc_cmd_indication_cb
			= zrc_ind_callback->zrc_cmd_indication_cb;
	}
#endif
#if (defined ZRC_CMD_DISCOVERY)
	if (zrc_ind_callback->zrc_cmd_disc_indication_cb != NULL) {
		zrc_ind_cb.zrc_cmd_disc_indication_cb
			= zrc_ind_callback->zrc_cmd_disc_indication_cb;
	}
#endif
#ifdef VENDOR_DATA
	if (zrc_ind_callback->vendor_data_ind_cb != NULL) {
		zrc_ind_cb.vendor_data_ind_cb
			= zrc_ind_callback->vendor_data_ind_cb;
	}
#endif
}

uint8_t nwk_get_nib_attribute_size(uint8_t nib_attribute_id)
{
	return GET_NIB_ATTR_SIZE(nib_attribute_id);
}

/* ! @} */
#endif /*SIO_HUB*/
