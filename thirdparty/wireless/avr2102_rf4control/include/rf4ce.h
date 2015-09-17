/**
 * @file rf4ce.h
 *
 * @brief This header file declares the interface for the RF4Control stack.
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

/* Prevent double inclusion */
#ifndef RF4CE_H
#define RF4CE_H

/* === Includes ============================================================= */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "compiler.h"
#include "return_val.h"
#include "nwk_build_config.h"

/**
 * \defgroup group_rf4control RF4Control v2.0.1
 * The RF4Control stack is a ZigBeeRF4CE Certified Platform implementing the
 * ZigBee
 * RF4CE standard.
 *
 *
 */

/**
 * \ingroup group_rf4control
 * \defgroup group_rf4control_NWK NWK Modules
 *  This module defines all Nwk layer core apis
 */

/**
 * \ingroup group_rf4control
 * \defgroup group_rf4control_ZRC ZRC Modules
 *  This module defines all ZRC layer core apis
 */

/**
 * \ingroup group_rf4control_NWK
 * \defgroup group_RF4CONTROL_NWK_API NWK APIs
 *  This module defines all Nwk layer core apis
 */

/**
 * \ingroup group_rf4control_NWK
 * \defgroup group_RF4CONTROL_GENERIC_API NWK Generic APIs
 *  This module defines all generic apis used by nwk layer.
 */

/**
 * \ingroup group_rf4control_NWK
 * \defgroup group_RF4CONTROL_NWK_CONSTANTS NWK Constants
 *  This module defines all Nwk layer constants.
 */

/**
 * \ingroup group_rf4control_ZRC
 * \defgroup group_RF4CONTROL_PBP_API Push button pairing APIs.
 *  This module defines all the Push button pairing  APIs.
 */

/**
 * \ingroup group_rf4control_ZRC
 * \defgroup group_RF4Control_ZRC_API ZRC APIs
 *  This module defines all the ZRC core APIs.
 */

/**
 * \ingroup group_rf4control_ZRC
 * \defgroup group_RF4CONTROL_GENERIC_API ZRC Generic APIs
 *  This module defines all the ZRC generic APIs.
 */

/**
 * \ingroup group_rf4control_ZRC
 * \defgroup group_RF4CONTROL_ZRC_CONSTANTS ZRC Constants
 *  This module defines all the Constatnts used by zrc layer.
 */

/* === Macros =============================================================== */

#define BAND_900_BASE_CHANNEL 1
#define BAND_900_CHANNEL_OFFSET 3

#define BAND_2400_BASE_CHANNEL 15
#define BAND_2400_CHANNEL_OFFSET 5

/* The capabilities of this node. Implementation specific according to the
 * format
 * illustrated in Figure 26.
 * Implementation specific */
/** Node capability - constant for node type target */
#define NodeTypeTarget                      0x01
/** Node capability - constant node type controller */
#define NodeTypeController                  0x00
/** Node capability - constant power source main available */
#define PowerSourceMainsTrue                0x02
/** Node capability - constant power source main not available */
#define PowerSourceMainsFalse               0x00
/** Node capability - constant security capable true */
#define SecurityCapableTrue                 0x04
/** Node capability - constant security capable false */
#define SecurityCapableFalse                0x00
/** Node capability - constant channel normalization capable true */
#define ChannelNormalizationCapableTrue     0x08
/** Node capability - constant channel normalization capable false */
#define ChannelNormalizationCapableFalse    0x00

/** Define used as parameters for NLME-RX-ENABLE.request: receiver off */
#define RX_DURATION_OFF                     0
/** Define used as parameters for NLME-RX-ENABLE.request: receiver on */
#define RX_DURATION_INFINITY                0xFFFFFF

/* Constant defines used for TxOption field */
/** Tx option - unicast */
#define TXO_UNICAST         0x00
/** Tx option - broadcast */
#define TXO_BROADCAST       0x01
/** Tx option - destination address: short */
#define TXO_DST_ADDR_NET    0x00
/** Tx option - destination address: long */
#define TXO_DST_ADDR_IEEE   0x02
/** Tx option - ACK not requested */
#define TXO_ACK_NOT_REQ     0x00
/** Tx option - ACK requested */
#define TXO_ACK_REQ         0x04
/** Tx option - security not requested */
#define TXO_SEC_NOT_REQ     0x00
/** Tx option - security requested */
#define TXO_SEC_REQ         0x08
/** Tx option - multi-channel transmission */
#define TXO_MULTI_CH        0x00
/** Tx option - single-channel transmission */
#define TXO_SINGLE_CH       0x10
/** Tx option - channel not specified */
#define TXO_CH_NOT_SPEC     0x00
/** Tx option - channel specified */
#define TXO_CH_SPEC         0x20
/** Tx option - no vendor-specific data */
#define TXO_VEND_NOT_SPEC   0x00
/** Tx option - vendor-specific data */
#define TXO_VEND_SPEC       0x40

/* Constant defines used for RxFlags field */
/* reception mode */
/** Rx flag - unicast */
#define RX_FLAG_UNICAST     0x00
/** Rx flag - broadcast */
#define RX_FLAG_BROADCAST   0x01
/** Rx flag - without security */
#define RX_FLAG_NO_SEC      0x00
/** Rx flag - with security */
#define RX_FLAG_WITH_SEC    0x02
/** Rx flag - no vendor-specific data */
#define RX_FLAG_NOT_VENDOR  0x00
/** Rx flag - vendor-specific data */
#define RX_FLAG_VENDOR_SPEC 0x04

/* NWK layer constants */
/** NWK layer constant - max duty cycle */
#define nwkcMaxDutyCycle        62500   /* symbols = 1 s */
/** NWK layer constant - min active period */
#define nwkcMinActivePeriod     1050    /* symbols = 16.8 ms */

/* Additional constants */

/** Calculate the maximum data frame payload that is allowed:
*  12 = FCF + frame counter + profile id + vendor id + MIC */
#define MAX_SAFE_DATA_FRAME_PAYLOAD     (aMaxMACSafePayloadSize - 12)

#define ALIGN8BIT
/* === Types ================================================================ */

__PACK__DATA__

/**
 * NWK enumerations as defined by
 * 094945r00ZB_RF4CE-Specification.pdf, Table 45 - NWK enumerations description
 *
 * @ingroup group_RF4CONTROL_NWK_CONSTANTS
 */
typedef enum nwk_enum_tag {
	/** The requested operation was completed successfully. */
	NWK_SUCCESS                 = 0x00,

	/** A pairing link cannot be established since the originator node has
	 * reached its maximum number of entries in its pairing table.*/
	NWK_NO_ORG_CAPACITY         = 0xb0,

	/** A pairing link cannot be established since the recipient node has
	 * reached its maximum number of entries in its pairing table. */
	NWK_NO_REC_CAPACITY         = 0xb1,

	/** A pairing table entry could not be found that corresponds to the
	 * supplied pairing reference. */
	NWK_NO_PAIRING              = 0xb2,
	/** A response frame was not received within nwkResponseWaitTime. */
	NWK_NO_RESPONSE             = 0xb3,

	/** A pairing request was denied by the recipient node or an attempt to
	 * update a security link key was not possible due to one or more nodes
	 * not
	 * supporting security. */
	NWK_NOT_PERMITTED           = 0xb4,

	/** A duplicate pairing table entry was detected following the receipt
	 * of a
	 * pairing request command frame. */
	NWK_DUPLICATE_PAIRING       = 0xb5,
	/** The frame counter has reached its maximum value. */
	NWK_FRAME_COUNTER_EXPIRED   = 0xb6,

	/** Too many unique matched discovery request or valid response command
	 * frames were received than requested. */
	NWK_DISCOVERY_ERROR         = 0xb7,

	/** No discovery request or response command frames were received during
	 * discovery. */
	NWK_DISCOVERY_TIMEOUT       = 0xb8,

	/** The security link key exchange or recovery procedure did not
	 * complete
	 * within the required time. */
	NWK_SECURITY_TIMEOUT        = 0xb9,

	/** A security link key was not successfully established between both
	 * ends
	 * of a pairing link. */
	NWK_SECURITY_FAILURE        = 0xba,

	/** A parameter in the primitive is either not supported or is out of
	 * the
	 * valid range. */
	NWK_INVALID_PARAMETER       = 0xe8,

	/** A SET/GET request was issued with the identifier of a NIB attribute
	 * that
	 * is not supported. */
	NWK_UNSUPPORTED_ATTRIBUTE   = 0xf4,

	/** An attempt to write to a NIB attribute that is in a table failed
	 * because
	 * the specified table index was out of range. */
	NWK_INVALID_INDEX           = 0xf9,
	/** Enumerations from MAC layer */

	/** A transmission could not take place due to activity on the channel,
	 * i.e.,
	 * the CSMA-CA mechanism has failed. */
	NWK_MAC_CHANNEL_ACCESS_FAILURE  = 0xE1,
	/** The attempt to disable the transceiver has failed. */
	NWK_MAC_DISABLE_TRX_FAILURE     = 0xE3,

	/** The frame resulting from secure processing has a length that is
	 * greater than
	 * aMACMaxFrameSize. */
	NWK_MAC_FRAME_TOO_LONG          = 0xE5,

	/** The requested GTS transmission failed because the specified GTS
	 * either did
	 * not have a transmit GTS direction or was not defined. */
	NWK_MAC_INVALID_GTS             = 0xE6,

	/** A request to purge an MSDU from the transaction queue was made using
	 * an MSDU
	 * handle that was not found in the transaction table. */
	NWK_MAC_INVALID_HANDLE         = 0xE7,
	/** No acknowledgment was received after aMaxFrameRetries. */
	NWK_MAC_NO_ACK                 = 0xE9,
	/** A scan operation failed to find any network beacons. */
	NWK_MAC_NO_BEACON              = 0xEA,
	/** No response data were available following a request. */
	NWK_MAC_NO_DATA                = 0xEB,
	/** The operation failed because a short address was not allocated. */
	NWK_MAC_NO_SHORT_ADDRESS       = 0xEC,

	/** A receiver enable request was unsuccessful because it could not be
	 *  completed within the CAP. */
	NWK_MAC_OUT_OF_CAP             = 0xED,

	/** A PAN identifier conflict has been detected and communicated to the
	 *  PAN coordinator. */
	NWK_MAC_PAN_ID_CONFLICT        = 0xEE,
	/** A coordinator realignment command has been received. */
	NWK_MAC_REALIGNMENT            = 0xEF,
	/** The transaction has expired and its information discarded. */
	NWK_MAC_TRANSACTION_EXPIRED    = 0xF0,
	/** There is no capacity to store the transaction. */
	NWK_MAC_TRANSACTION_OVERFLOW   = 0xF1,

	/** The transceiver was in the transmitter enabled state when the
	 * receiver
	 * was requested to be enabled. */
	NWK_MAC_TX_ACTIVE              = 0xF2,

	/** A request to send data was unsuccessful because neither the source
	 *  address parameters nor the destination address parameters were
	 * present.
	 */
	NWK_MAC_INVALID_ADDRESS        = 0xF5
} SHORTENUM nwk_enum_t;

/**
 * Elements of the NodeDesc type as defined by
 * 094945r00ZB_RF4CE-Specification.pdf
 *
 * @ingroup group_RF4CONTROL_NWK_CONSTANTS
 */

typedef struct NodeDesc_tag {
	/** The status of the discovery request as reported by the responding
	 * device.
	 * NWK_SUCCESS or NO_REC_CAPACITY */
	nwk_enum_t Status;
	/** The logical channel of the responding device. */
	uint8_t LogicalChannel;
	/** The PAN identifier of the responding device. */
	uint16_t PANId;
	/** The IEEE address of the responding device. */
	uint64_t IEEEAddr;
	/** The capabilities of the responding node. */
	uint8_t NodeCapabilities;
	/** The vendor identifier of the responding node. */
	uint16_t VendorId;
	/** The vendor string of the responding node. */
	uint8_t VendorString[7];
	/** The application capabilities of the responding node. */
	uint8_t AppCapabilities;

	/** The user defined identification string of the responding node.
	 * This field is present only if the user string specified subfield
	 * of the AppCapabilities field is set to one. */
	uint8_t UserString[15];
	/** The list of device types supported by the responding node. */
	uint8_t DevTypeList[3];
	/** The list of profile identifiers supported by the responding node. */
	uint8_t ProfileIdList[7];

	/** The LQI of the discovery request command frame reported by the
	 * responding device. */
	uint8_t DiscReqLQI;
} node_desc_t;

/**
 * Network Information Base attributes as defined by
 * 094945r00ZB_RF4CE-Specification.pdf
 *
 * @ingroup group_RF4CONTROL_NWK_CONSTANTS
 */
typedef enum nib_attribute_tag {
	/** The active period of a device in MAC symbols. */
	nwkActivePeriod                     = 0x60,
	/** The logical channel that was chosen when the RC PAN was formed. */
	nwkBaseChannel                      = 0x61,

	/** The LQI threshold below which discovery requests will be rejected.
	**/
	nwkDiscoveryLQIThreshold            = 0x62,

	/** The interval at which discovery attempts are made on all channels.
	**/
	nwkDiscoveryRepetitionInterval      = 0x63,

	/** The duty cycle of a device in MAC symbols. A value of 0x000000
	 * indicates
	 * the device
	 * is not using power saving. */
	nwkDutyCycle                        = 0x64,
	/** The frame counter added to the transmitted NPDU. */
	nwkFrameCounter                     = 0x65,

	/** Indicates whether the NLME indicates the reception of discovery
	 * request
	 * command frames to the application. TRUE indicates that the NLME
	 * notifies
	 * the application. */
	nwkIndicateDiscoveryRequests        = 0x66,

	/** The power save mode of the node. TRUE indicates that the device is
	 * operating in power save mode. */
	nwkInPowerSave                      = 0x67,

	/** List of pairing table entries (see Table 41)
	 * The pairing table managed by the device. */
	nwkPairingTable                     = 0x68,

	/** The maximum number of discovery attempts made at the
	 * nwkDiscoveryRepetitionInterval rate. */
	nwkMaxDiscoveryRepetitions          = 0x69,

	/** The maximum number of backoffs the MAC CSMACA algorithm will attempt
	 * before declaring a channel access failure for the first transmission
	 * attempt.*/
	nwkMaxFirstAttemptCSMABackoffs      = 0x6a,

	/** The maximum number of MAC retries allowed after a transmission
	 * failure
	 * for the first transmission attempt. */
	nwkMaxFirstAttemptFrameRetries      = 0x6b,

	/** The maximum number of node descriptors that can be obtained before
	 * reporting to the application. */
	nwkMaxReportedNodeDescriptors       = 0x6c,

	/** The maximum time in MAC symbols, a device shall wait for a response
	 * command frame following a request command frame. */
	nwkResponseWaitTime                 = 0x6d,

	/** A measure of the duration of a scanning operation, according to
	 *[R1]. */
	nwkScanDuration                     = 0x6e,
	/** The user defined character string used to identify this node. */
	nwkUserString                       = 0x6f,
	/** Private/Proprietary PIB value used for set/get primitive */
	/** IEEE address */
	nwkPrivateIeeeAddr                  = 0x70,
	/** Vendor identifier */
	nwkPrivateVendorIdentifier          = 0x71,
	/** Vendor string */
	nwkPrivateVendorString              = 0x72,
	/** Node capabilities (security and node type bits are read-only) */
	nwkPrivateNodeCapabilities          = 0x73,
	/** Target Pan Identifier (read-only) */
	nwkPrivatePanIdentifier             = 0x74,
	/** Target Short address (read-only) */
	nwkPrivateShortAddress              = 0x75,
	/** Private NIB */

	/** Maximum number of allowed pairing table entries (set only); used for
	 * testing */
	nwkPrivateMaxPairingTableEntries    = 0x76,
	/** Channel agility feature enabled - read-only */
	nwkPrivateChAgEnabled               = 0x77,
	/** Channel agility scan interval */
	nwkPrivateChAgScanInterval          = 0x78,
	/** Channel agility ED threshold value */
	nwkPrivateChAgEdThreshold           = 0x79
#ifdef ZRC_PROFILE
	,

	/** The interval in ms at which user command repeat frames will be
	 * transmitted. */
	aplKeyRepeatInterval                = 0x80,

	/** The duration that a recipient of a user control repeated command
	 * frame
	 *  waits before terminating a repeated operation. */
	aplKeyRepeatWaitTime                = 0x81

#if (defined PBP_ORG) || (defined PBP_REC)
	,

	/** The minimum value of the KeyExTransferCount parameter passed to the
	 * pair
	 *  request primitive during the push button pairing procedure. */
	aplKeyExchangeTransferCount         = 0x82
#endif
#endif
} SHORTENUM nib_attribute_t;

/**
 * Structure implementing the pairing table entry as defined by
 * 094945r00ZB_RF4CE-Specification.pdf
 *
 * @ingroup group_RF4CONTROL_NWK_CONSTANTS
 */
typedef struct pairing_table_tag {
	/** The network address to be assumed by the source device. */
	uint16_t SourceNetworkAddress;
	/** The expected channel of the destination device. */
	uint8_t DestinationLogicalChannel;
	/** The IEEE address of the destination device. */
	uint64_t DestinationIEEEaddress;
	/** The PAN identifier of the destination device. */
	uint16_t DestinationPANidentifier;
	/** The network address of the destination device. */
	uint16_t DestinationNetworkAddress;
	/** The node capabilities of the recipient node. */
	uint8_t RecipientCapabilities;
	/** The frame counter last received from the recipient node. */
	uint32_t RecipientFrameCounter;
#if (defined RF4CE_SECURITY) || (defined DOXYGEN) || (defined RF4CE_PLATFORM)
	/** The link key to be used to secure this pairing link. */
	uint8_t SecurityLinkKey[16];
#endif
} pairing_table_t;

/**
 * Enumeration identifiying the address mode as defined by
 * 094945r00ZB_RF4CE-Specification.pdf
 *
 * @ingroup group_RF4CONTROL_NWK_CONSTANTS
 */
typedef enum nwk_addr_mode_tag {
	/** Address mode 16-bit network address */
	NWK_ADDR_MODE_16    = 0,
	/** Address mode 64-bit IEEE address */
	NWK_ADDR_MODE_64    = 1
} SHORTENUM nwk_addr_mode_t;

/**
 * Enumeration used for implementation-specific channel agility
 *
 * @ingroup group_RF4CONTROL_NWK_CONSTANTS
 */
typedef enum nwk_agility_mode_tag {
	/** One shot scan type for channel agility. */
	AG_ONE_SHOT     = 0,
	/** Periodic scan */
	AG_PERIODIC     = 1,
	/** Stop channel agility. */
	AG_STOP         = 2
} SHORTENUM nwk_agility_mode_t;

/**
 * Device types as defined by 094950r00ZB_RF4CE-Device-Type-List.pdf
 *
 * @ingroup group_RF4CONTROL_NWK_CONSTANTS
 */
typedef enum dev_type_tag {
	DEV_TYPE_RESERVED           = 0x00,
	DEV_TYPE_REMOTE_CONTROL     = 0x01,
	DEV_TYPE_TELEVISION         = 0x02,
	DEV_TYPE_PROJECTON          = 0x03,
	DEV_TYPE_PLAYER             = 0x04,
	DEV_TYPE_RECORDER           = 0x05,
	DEV_TYPE_VIDEO              = 0x06,
	DEV_TYPE_AUDIO              = 0x07,
	DEV_TYPE_AV_RECORDER        = 0x08,
	DEV_TYPE_SET_TOP_BOX        = 0x09,
	DEV_TYPE_HOME_THEATER       = 0x0A,
	DEV_TYPE_MEDIA_CENTER       = 0x0B,
	DEV_TYPE_GAME_CONSOLE       = 0x0C,
	DEV_TYPE_SAT_RADIO          = 0x0D,
	DEV_TYPE_IR_EXTENDER        = 0x0E,
	DEV_TYPE_MONITOR            = 0x0F,
	/* 0x10 - 0xFD RESERVED */
	DEV_TYPE_GENERIC            = 0xFE,
	DEV_TYPE_WILDCARD           = 0xFF
} SHORTENUM dev_type_t;

/**
 * Profile IDs as defined by 094951r00ZB_RF4CE-Profile-ID-List.pdf
 *
 * @ingroup group_RF4CONTROL_NWK_CONSTANTS
 */
typedef enum profile_id_tag {
	PROFILE_ID_RESERVED_00      = 0x00,
	/** Consumer Electronics Remote control */
	PROFILE_ID_ZRC              = 0x01,
	/** ZigBee Input Device */
	PROFILE_ID_ZID              = 0x02,
	/* 0x03 - 0xbf Reserved for future standard ZigBee RF4CE profiles */
	/* 0xc0 - 0xfe Manufacturer specific profiles */
	PROFILE_ID_VENDOR_DATA      = 0xFE,
	/** Wildcard profile id */
	PROFILE_ID_WILDCARD         = 0xFF
} SHORTENUM profile_id_t;

__PACK__RST_DATA__

/**
 * @brief Confirms the previous channel agility request, i.e.
 * nwk_ch_agility_req()
 *
 * @param Status Result of last request
 *               NWK_SUCCESS Channel agility request is performed successfully.
 *               NWK_NOT_PERMITTED Channel agility is not permitted, because
 *               power save mode is currently enabled.
 * @param ChannelChanged True if channel changed due to agility feature
 * @param LogicalChannel Current channel
 *
 * @see nwk_ch_agility_request
 * @ingroup group_RF4CONTROL_NWK_API
 */
#if (defined CHANNEL_AGILITY) || (defined DOXYGEN) || (defined RF4CE_PLATFORM)
typedef void (*nwk_ch_agility_confirm_cb_t)(
	nwk_enum_t Status, bool ChannelChanged, uint8_t LogicalChannel);
#endif

/**
 * @brief Indicates a channel agility event
 *
 * The function indicates that a channel agility event has occured, i.e. the
 * base channel has been changed automatically. The new channel is indicated
 * by the parameter LogicalChannel.
 *
 * @param LogicalChannel New/Current channel.
 *
 * @see nwk_ch_agility_request
 * @ingroup group_RF4CONTROL_NWK_API
 */
#if (defined CHANNEL_AGILITY) || (defined DOXYGEN) || (defined RF4CE_PLATFORM)
typedef void (*nwk_ch_agility_indication_cb_t)(uint8_t LogicalChannel);
#endif

/**
 * @brief NLME-UPDATE-KEY.confirm primitive API
 *
 * The NLME-UPDATE-KEY.confirm primitive allows the NLME to notify the
 * application of the status of its request to change the security link key of
 * a pairing table entry.
 *
 * @param Status        The status of the request to update the security link
 *                      key.
 * @param PairingRef    The reference into the local pairing table of the entry
 *                      whose key is to be updated.
 *
 * @see nlme_update_key_request
 * @ingroup group_RF4CONTROL_NWK_API
 */
#if (NWK_UPDATE_KEY == 1) || (defined DOXYGEN) || (defined RF4CE_PLATFORM)
typedef void (*nlme_update_key_confirm_cb_t)(nwk_enum_t Status,
		uint8_t PairingRef);
#endif

/**
 * @brief NLME-UNPAIR.confirm primitive API
 *
 * The NLME-UNPAIR.confirm primitive allows the NLME to notify the application
 * of
 * the status of its request to remove a pair with another device.
 *
 * @param Status        The status of the unpair attempt.
 * @param PairingRef   The pairing table reference for this pairing link.
 *
 * @see nlme_unpair_request
 * @ingroup group_RF4CONTROL_NWK_API
 */
#if (NWK_UNPAIR_REQ_CONF == 1) || (defined DOXYGEN) || (defined RF4CE_PLATFORM)
typedef void (*nlme_unpair_confirm_cb_t)(uint8_t Status, uint8_t PairingRef);
#endif

/**
 * @brief NLME-START.confirm primitive API
 *
 * The NLME-START.confirm primitive allows the NLME to notify the application of
 * the status of its request to start a network.
 *
 * @param Status    The status of the start attempt.
 *
 * @see nlme_start_request
 * @ingroup group_RF4CONTROL_NWK_API
 */
typedef void (*nlme_start_confirm_cb_t)(nwk_enum_t Status);

/**
 * @brief NLME-SET.confirm primitive API
 *
 * The NLME-SET.confirm primitive allows the NLME to notify the application of
 * the
 * status of its request to change the value of a NIB attribute.
 *
 * @param Status            The status of the request to set PIB attribute
 *                          information.
 * @param NIBAttribute      The identifier of the NIB attribute that was
 *                          written.
 * @param NIBAttributeIndex The index within the table or array of the
 *                          specified NIB attribute that was written.
 *
 * @see nlme_set_request
 * @ingroup group_RF4CONTROL_NWK_API
 */
#if (NWK_SET == 1) || (defined DOXYGEN) || (defined RF4CE_PLATFORM)
typedef void (*nlme_set_confirm_cb_t)(nwk_enum_t Status,
		nib_attribute_t NIBAttribute,
		uint8_t NIBAttributeIndex);
#endif

/**
 * @brief Confirmation callback pointer for NLME-RX-ENABLE request
 *
 * The NLME-RX-ENABLE.confirm primitive reports the results of the attempt to
 * enable or disable the receiver.
 *
 * @param Status    The result of the request to enable or disable the receiver.
 *
 * @see nlme_rx_enable_request
 * @ingroup group_RF4CONTROL_NWK_API
 */
typedef void (*nlme_rx_enable_confirm_cb_t)(nwk_enum_t Status);

/**
 * @brief NLME-RESET.confirm primitive API
 *
 * The NLME-RESET.confirm primitive allows the NLME to notify the application of
 * the status of its request to reset the NWK layer.
 *
 * @param Status    The status of the reset request.
 *
 * @see nlme_reset_request
 * @ingroup group_RF4CONTROL_NWK_API
 */
typedef void (*nlme_reset_confirm_cb_t)(nwk_enum_t Status);

/**
 * @brief NLME-PAIR.confirm primitive API
 *
 * The NLME-PAIR.confirm primitive allows the NLME to notify the application of
 * the
 * status of its request to pair with another device.
 *
 * @param Status                The status of the pair attempt.
 * @param PairingRef            The pairing table reference for this pairing
 *                              link.
 * @param RecVendorId           The vendor identifier of the originator of the
 *                              pair response.
 * @param RecVendorString       The vendor string of the originator of the pair
 *                              response.
 * @param RecAppCapabilities    The application capabilities of the originator
 *                              of the pair response.
 * @param RecUserString         The user defined identification string of the
 *                              originator of the pair response.
 * @param RecDevTypeList        The list of device types supported by the
 *                              originator of the pair response.
 * @param RecProfileIdList      The list of profile identifiers supported by
 *                              the originator of the pair response.
 *
 * @see nlme_pair_request
 * @ingroup group_RF4CONTROL_NWK_API
 */
#if (NWK_PAIR_REQ_CONF == 1) || (defined DOXYGEN)
typedef void (*nlme_pair_confirm_cb_t)(nwk_enum_t Status, uint8_t PairingRef,
		uint16_t RecVendorId,
		uint8_t RecVendorString[7], uint8_t RecAppCapabilities,
		uint8_t RecUserString[15], dev_type_t RecDevTypeList[3],
		profile_id_t RecProfileIdList[7]);
#endif

/**
 * @brief NLME-GET.confirm primitive API
 *
 * The NLME-GET.confirm primitive allows the NLME to notify the application of
 * the
 * status of its request for the value of a NIB attribute.
 *
 * @param Status            The status of the request for NIB attribute
 *                          information.
 * @param NIBAttribute      The identifier of the NIB attribute that was read.
 * @param NIBAttributeIndex The index within the table or array of the
 *                          specified NIB attribute that was read.
 * @param NIBAttributeValue The value of the NIB attribute that was read.
 *
 * @see nlme_get_request, nwk_get_nib_attribute_size
 * @ingroup group_RF4CONTROL_NWK_API
 */
#if (NWK_GET == 1) || (defined DOXYGEN) || (defined RF4CE_PLATFORM)
typedef void (*nlme_get_confirm_cb_t)(nwk_enum_t Status,
		nib_attribute_t NIBAttribute,
		uint8_t NIBAttributeIndex, void *NIBAttributeValue);
#endif

#if (NWK_DISCOVERY_REQ_CONF == 1) || (defined DOXYGEN) || \
	(defined RF4CE_PLATFORM)
typedef void (*nlme_discovery_confirm_cb_t)(nwk_enum_t Status, uint8_t NumNodes,
		node_desc_t *NodeDescList);
#endif

/**
 * @brief NLME-AUTO-DISCOVERY.confirm primitive API
 *
 * The NLME-AUTO-DISCOVERY.confirm primitive allows the NLME to notify the
 * application of the status of its request to enter auto discovery response
 * mode.
 *
 * @param Status        Status of the completed auto-discovery procedure
 * @param SrcIEEEAddr   Source IEEE address from which the discovery request
 *                     was received.
 *
 * @see nlme_auto_discovery_request
 * @ingroup group_RF4CONTROL_NWK_API
 */
#if (defined RF4CE_TARGET) || (defined DOXYGEN) || (defined RF4CE_PLATFORM)
typedef void (*nlme_auto_discovery_confirm_cb_t)(nwk_enum_t Status,
		uint64_t SrcIEEEAddr);
#endif

/**
 * @brief NLDE-DATA.confirm primitive API
 *
 * The NLDE-DATA.confirm primitive is generated by the NWK layer entity in
 * response to an NLDE-DATA.request primitive.
 *
 * @param Status      Status for the handled data request
 * @param PairingRef  Pairing reference used for the data request
 * @param ProfileId   Profile Id used for the data request
 * @param Handle      Data retry handle
 *
 * @see nlde_data_request
 * @ingroup group_RF4CONTROL_NWK_API
 */
typedef void (*nlde_data_confirm_cb_t)(nwk_enum_t Status, uint8_t PairingRef,
		profile_id_t ProfileId,
		uint8_t Handle);

/**
 * @brief NLME-PAIR.indication primitive API
 *
 * The NLME-PAIR.indication primitive allows the NLME to notify the application
 * of
 * the reception of a pairing request command.
 *
 * @param Status                The status of the provisional pairing.
 * @param SrcPANId              The PAN identifier of the device requesting the
 *                              pair.
 * @param SrcIEEEAddr           The IEEE address of the device requesting the
 *                              pair.
 * @param OrgNodeCapabilities   The capabilities of the originator of the pair
 *                              request.
 * @param OrgVendorId           The vendor identifier of the originator of the
 *                              pair request.
 * @param OrgVendorString       The vendor string of the originator of the pair
 *                              request.
 * @param OrgAppCapabilities    The application capabilities of the originator
 *                              of the pair request.
 * @param OrgUserString         The user defined identification string of the
 *                              originator of the pair request.
 * @param OrgDevTypeList        The list of device types supported by the
 *                              originator of the pair request.
 * @param OrgProfileIdList      The list of profile identifiers supported by
 *                              the originator of the pair request.
 * @param KeyExTransferCount    The number of transfers being requested to
 *                              exchange the link key.
 * @param ProvPairingRef        The pairing reference that will be used if this
 *                              pairing request is successful.
 *
 * @see nlme_pair_request, nlme_pair_response
 * @ingroup group_RF4CONTROL_NWK_API     */

#if (defined RF4CE_TARGET) || (defined DOXYGEN) || (defined RF4CE_PLATFORM)
typedef void (*nlme_pair_indication_cb_t)(nwk_enum_t Status, uint16_t SrcPANId,
		uint64_t SrcIEEEAddr,
		uint8_t OrgNodeCapabilities, uint16_t OrgVendorId,
		uint8_t OrgVendorString[7], uint8_t OrgAppCapabilities,
		uint8_t OrgUserString[15], dev_type_t OrgDevTypeList[3],
		profile_id_t OrgProfileIdList[7], uint8_t KeyExTransferCount,
		uint8_t ProvPairingRef);
#endif

/**
 * @brief NLME-UNPAIR.indication primitive API
 *
 * The NLME-UNPAIR.indication primitive allows the NLME to notify the
 * application
 * of the removal of a pairing link by another device.
 *
 * @param PairingRef    The pairing table reference that has been removed from
 *                      the pairing table.
 *
 * @see nlme_unpair_response
 * @ingroup group_RF4CONTROL_NWK_API
 */
#if (NWK_UNPAIR_IND_RESP == 1) || (defined DOXYGEN) || (defined RF4CE_PLATFORM)
typedef void (*nlme_unpair_indication_cb_t)(uint8_t PairingRef);
#endif

/**
 * @brief NLME-DISCOVERY.indication primitive API
 *
 * The NLME-DISCOVERY.indication primitive allows the NLME to notify the
 * application that a discovery request command has been received.
 *
 * @param Status                The status of the pairing table.
 * @param SrcIEEEAddr           The IEEE address of the device requesting the
 *                              discovery.
 * @param OrgNodeCapabilities   The capabilities of the originator of the
 *                              discovery request.
 * @param OrgVendorId           The vendor identifier of the originator of the
 *                              discovery request.
 * @param OrgVendorString       The vendor string of the originator of the
 *                              discovery request.
 * @param OrgAppCapabilities    The application capabilities of the originator
 *                              of the discovery request.
 * @param OrgUserString         The user defined identification string of the
 *                              originator of the discovery request.
 * @param OrgDevTypeList        The list of device types supported by the
 *                              originator of the discovery request.
 * @param OrgProfileIdList      The list of profile identifiers supported by
 *                              the originator of the discovery request.
 * @param SearchDevType         The device type being discovered. If this is
 *                              0xff, any type is being requested.
 * @param RxLinkQuality         LQI value of the discovery request command
 *                              frame.
 *
 * @see nlme_discovery_request, nlme_discovery_confirm, nlme_discovery_response
 * @ingroup group_RF4CONTROL_NWK_API
 */
#if (((defined RF4CE_TARGET) && (NWK_DISCOVERY_IND_RESP == 1)) || \
	(defined DOXYGEN)) || (defined RF4CE_PLATFORM)
typedef void (*nlme_discovery_indication_cb_t)(nwk_enum_t Status,
		uint64_t SrcIEEEAddr,
		uint8_t OrgNodeCapabilities, uint16_t OrgVendorId,
		uint8_t OrgVendorString[7], uint8_t OrgAppCapabilities,
		uint8_t OrgUserString[15], dev_type_t OrgDevTypeList[3],
		profile_id_t OrgProfileIdList[7], dev_type_t SearchDevType,
		uint8_t RxLinkQuality);
#endif

/**
 * @brief NLME-COMM-STATUS.indication primitive API
 *
 * The NLME-COMM-STATUS.indication primitive allows the NLME to notify the
 * application of a communication status.
 *
 * @param Status        Status of the transmission
 * @param PairingRef    Pairing reference used for the transmission
 * @param DstPANId      Destination PAN Id used for the transmission
 * @param DstAddrMode   Destination adress mode used for the transmission
 * @param DstAddr       Destination address used for the transmission
 *
 * @ingroup group_RF4CONTROL_NWK_API
 */
#if (defined RF4CE_TARGET) || (defined DOXYGEN) || (defined RF4CE_PLATFORM)
typedef void (*nlme_comm_status_indication_cb_t)(nwk_enum_t Status,
		uint8_t PairingRef,
		uint16_t DstPANId, uint8_t DstAddrMode,
		uint64_t DstAddr);
#endif

/**
 * @brief NLDE-DATA.indication primitive API
 *
 * The NLDE-DATA.indication primitive is generated by the NLDE and issued to the
 * application on receipt of a data frame at the local NWK layer entity.
 *
 * @param PairingRef    Pairing reference attached to the incoming frame
 * @param ProfileId     Profile id used for the frame
 * @param VendorId      Vendor id used for the frame
 * @param nsduLength    Payload length
 * @param nsdu          Pointer to the network payload
 * @param RxLinkQuality Link quality of the received frame
 * @param RxFlags       Receive flags
 *
 * @see nlde_data_request, nlde_data_confirm
 * @ingroup group_RF4CONTROL_NWK_API
 */

typedef void (*nlde_data_indication_cb_t)(uint8_t PairingRef,
		profile_id_t ProfileId,
		uint16_t VendorId, uint8_t nsduLength, uint8_t *nsdu,
		uint8_t RxLinkQuality, uint8_t RxFlags);

#if (defined ZRC_PROFILE)
typedef void (*zrc_data_indication_cb_t)(uint8_t PairingRef, uint16_t VendorId,
		uint8_t nsduLength,
		uint8_t *nsdu, uint8_t RxLinkQuality, uint8_t RxFlags);
#endif

#if (defined ZID_PROFILE)
typedef void (*zid_data_indication_cb_t)(uint8_t PairingRef, uint16_t VendorId,
		uint8_t nsduLength, uint8_t *nsdu,
		uint8_t RxLinkQuality, uint8_t RxFlags);
#endif

#if (defined GDP_PROFILE)
typedef void (*gdp_data_indication_cb_t)(uint8_t PairingRef, uint8_t nsduLength,
		uint8_t *nsdu,
		uint8_t RxLinkQuality, uint8_t RxFlags);
#endif

/**
 * struct for network indication callback.
 * App should use this struct to register indication callback functions
 *
 * @ingroup group_RF4CONTROL_NWK_API
 */
typedef struct nwk_indication_callback_tag {
#if (defined CHANNEL_AGILITY) || (defined DOXYGEN) || (defined RF4CE_PLATFORM)
	nwk_ch_agility_indication_cb_t nwk_ch_agility_indication_cb;
#endif
#if (NWK_UNPAIR_IND_RESP == 1) || (defined DOXYGEN) || (defined RF4CE_PLATFORM)
	nlme_unpair_indication_cb_t nlme_unpair_indication_cb;
#endif
#if (defined RF4CE_TARGET) || (defined DOXYGEN) || (defined RF4CE_PLATFORM)
	nlme_pair_indication_cb_t nlme_pair_indication_cb;
#endif
#if (((defined RF4CE_TARGET) && (NWK_DISCOVERY_IND_RESP == 1)) || \
	(defined DOXYGEN)) || (defined RF4CE_PLATFORM)
	nlme_discovery_indication_cb_t nlme_discovery_indication_cb;
#endif
#if (defined RF4CE_TARGET) || (defined DOXYGEN) || (defined RF4CE_PLATFORM)
	nlme_comm_status_indication_cb_t nlme_comm_status_indication_cb;
#endif
#ifdef ZRC_PROFILE
	zrc_data_indication_cb_t zrc_data_indication_cb;
#endif
#ifdef ZID_PROFILE
	zid_data_indication_cb_t zid_data_indication_cb;
#endif
#ifdef GDP_PROFILE
	gdp_data_indication_cb_t gdp_data_indication_cb;
#endif
	nlde_data_indication_cb_t nlde_data_indication_cb;
} nwk_indication_callback_t;

/* === Externals ============================================================ */

/* === Prototypes =========================================================== */
#ifdef __cplusplus
extern "C" {
#endif

/* Network primitives */

/**
 * @brief NLDE-DATA.request primitive API
 *
 * The NLDE-DATA.request primitive requests the transfer of a data APDU (i.e.
 * NSDU)
 * from a local application entity to a peer application entity.
 *
 * @param PairingRef    Pairing reference used for the data transmission
 * @param ProfileId     Profile Id used for the data transmission
 * @param VendorId      Vendor Id used for the data transmission
 * @param nsduLength    Length of the network payload
 * @param nsdu          Pointer to the payload
 * @param TxOptions     Transmission options
 * @param Handle        Data retry handle
 * @param confirm_cb    Call back pointer for the confirmation
 *
 * @return true if request has been added to NHLE-NWK queue successfully; else
 * false
 *
 * @see nlde_data_confirm
 * @ingroup group_RF4CONTROL_NWK_API
 */
bool nlde_data_request(uint8_t PairingRef, profile_id_t ProfileId,
		uint16_t VendorId, uint8_t nsduLength, uint8_t *nsdu,
		uint8_t TxOptions,
		uint8_t Handle,
		FUNC_PTR confirm_cb
		);

/**
 * @brief NLME-AUTO-DISCOVERY.request primitive API
 *
 * The NLME-AUTO-DISCOVERY.request primitive allows the application to request
 * the NLME automatically handles the receipt of discovery request command
 * frames.
 *
 * @param RecAppCapabilities    Recipient application capabilities
 * @param RecDevTypeList        Recipient device type list
 * @param RecProfileIdList      Recipient profile id list
 * @param AutoDiscDuration      auto-discovery duration
 * @param confirm_cb            Call back pointer for the confirmation
 *
 * @return true if request has been added to NHLE-NWK queue successfully; else
 * false
 *
 * @see nlme_auto_discovery_confirm
 * @ingroup group_RF4CONTROL_NWK_API
 */
#if (defined RF4CE_TARGET) || (defined DOXYGEN) || (defined RF4CE_PLATFORM)
bool nlme_auto_discovery_request(uint8_t RecAppCapabilities,
dev_type_t RecDevTypeList[3],
profile_id_t RecProfileIdList[7],
uint32_t AutoDiscDuration,
FUNC_PTR confirm_cb
);
#endif

/**
 * @brief NLME-DISCOVERY.request primitive API
 *
 * The NLME-DISCOVERY.request primitive allows the application to request the
 * NLME discover other devices of interest operating in the POS of the device.
 *
 * @param DstPANId                  The PAN identifier of the destination
 *                                  device for the discovery. This value can be
 *                                  set to 0xffff to indicate a wildcard.
 * @param DstNwkAddr                The address of the destination device for
 *                                  the discovery. This value can be set to
 *                                  0xffff to indicate a wildcard.
 * @param OrgAppCapabilities        The application capabilities of the node.
 * @param OrgDevTypeList            The list of device types supported by the
 *                                  node.
 * @param OrgProfileIdList          The list of profile identifiers disclosed
 *                                  as supported by the node.
 * @param SearchDevType             The device type to discover. This value can
 *                                  be set to 0xff to indicate a wildcard.
 * @param DiscProfileIdListSize     The number of profile identifiers contained
 *                                  in the DiscProfileIdList parameter.
 * @param DiscProfileIdList         The list of profile identifiers against
 *                                  which profile identifiers contained in
 *                                  received discovery response command frames
 *                                  will be matched for acceptance.
 * @param DiscDuration              The maximum number of MAC symbols to wait
 *                                  for discovery responses.
 * @param confirm_cb                Call back pointer for the confirmation
 *
 * @return true if request has been added to NHLE-NWK queue successfully; else
 * false
 *
 * @see nlme_discovery_confirm
 * @ingroup group_RF4CONTROL_NWK_API
 */
#if (NWK_DISCOVERY_REQ_CONF == 1) || (defined DOXYGEN) || \
	(defined RF4CE_PLATFORM)
bool nlme_discovery_request(uint16_t DstPANId, uint16_t DstNwkAddr,
uint8_t OrgAppCapabilities,
dev_type_t OrgDevTypeList[3],
profile_id_t OrgProfileIdList[7],
dev_type_t SearchDevType,
uint8_t DiscProfileIdListSize,
profile_id_t DiscProfileIdList[7],
uint32_t DiscDuration,
FUNC_PTR confirm_cb
);
#endif

/**
 * @brief NLME-DISCOVERY.response primitive API
 *
 * The NLME-DISCOVERY.response primitive allows the application to request that
 * the NLME respond to the discovery request command.
 *
 * @param Status                The status of the discovery request.
 * @param DstIEEEAddr           The IEEE address of the device requesting
 *                              discovery.
 * @param RecAppCapabilities    The application capabilities of the node
 *                              issuing this primitive.
 * @param RecDevTypeList        The list of device types supported by the node
 *                              issuing this primitive.
 * @param RecProfileIdList      The list of profile identifiers supported by
 *                              the node issuing this primitive.
 * @param DiscReqLQI            The LQI value from the associated
 *                              NLME-DISCOVERY.indication primitive.
 *
 * @return true if request has been added to NHLE-NWK queue successfully; else
 * false
 *
 * @see nlme_discovery_indication
 * @ingroup group_RF4CONTROL_NWK_API
 */
#if (((defined RF4CE_TARGET) && (NWK_DISCOVERY_IND_RESP == 1)) || \
	(defined DOXYGEN)) || (defined RF4CE_PLATFORM)
bool nlme_discovery_response(nwk_enum_t Status, uint64_t DstIEEEAddr,
uint8_t RecAppCapabilities, dev_type_t RecDevTypeList[3],
profile_id_t RecProfileIdList[7], uint8_t DiscReqLQI);
#endif

/**
 * @brief NLME-GET.request primitive API
 *
 * The NLME-GET.request primitive allows the application to request the value of
 * a
 * NIB attribute from the NLME.
 *
 * @param NIBAttribute      The identifier of the NIB attribute to read.
 * @param NIBAttributeIndex The index within the table or array of the
 *                          specified NIB attribute to read.
 * @param confirm_cb        Call back pointer for the confirmation
 *
 * @return true if request has been added to NHLE-NWK queue successfully; else
 * false
 *
 * @see nlme_get_confirm
 * @ingroup group_RF4CONTROL_NWK_API
 */
#if (NWK_GET == 1) || (defined DOXYGEN) || (defined RF4CE_PLATFORM)
bool nlme_get_request(nib_attribute_t NIBAttribute, uint8_t NIBAttributeIndex,
		FUNC_PTR confirm_cb
		);

#endif

/**
 * @brief NLME-PAIR.request primitive API
 *
 * The NLME-PAIR.request primitive allows the application to request the NLME
 * pair
 * with another device. This primitive would normally be issued following a
 * discovery
 * operation via the NLME-DISCOVERY.request primitive.
 *
 * @param LogicalChannel        The logical channel of the device with which to
 *                              pair.
 * @param DstPANId              The PAN identifier of the device with which to
 *                              pair.
 * @param DstIEEEAddr           The IEEE address of the device with which to
 *                              pair.
 * @param OrgAppCapabilities    The application capabilities of the node
 *                              issuing this primitive.
 * @param OrgDevTypeList        The list of device types supported by the node
 *                              issuing this primitive.
 * @param OrgProfileIdList      The list of profile identifiers supported by
 *                              the node issuing this primitive.
 * @param KeyExTransferCount    The number of transfers the target should use
 *                              to exchange the link key with the pairing
 *                              originator.
 * @param confirm_cb            Call back pointer for the confirmation
 *
 * @return true if request has been added to NHLE-NWK queue successfully; else
 * false
 *
 * @see nlme_pair_confirm
 * @ingroup group_RF4CONTROL_NWK_API
 */
#if (NWK_PAIR_REQ_CONF == 1) || (defined DOXYGEN)
bool nlme_pair_request(uint8_t LogicalChannel, uint16_t DstPANId,
uint64_t DstIEEEAddr,
uint8_t OrgAppCapabilities, dev_type_t OrgDevTypeList[3],
profile_id_t OrgProfileIdList[7], uint8_t KeyExTransferCount,
FUNC_PTR confirm_cb
);
#endif

/**
 * @brief NLME-PAIR.response primitive API
 *
 * The NLME-PAIR.response primitive allows the application to request that the
 * NLME
 * respond to a pairing request command.
 *
 * @param Status                The status of the pairing request.
 * @param DstPANId              The PAN identifier of the device requesting the
 *                              pair.
 * @param DstIEEEAddr           The IEEE address of the device requesting the
 *                              pair.
 * @param RecAppCapabilities    The application capabilities of the node
 *                              issuing this primitive.
 * @param RecDevTypeList        The list of device types supported by the node
 *                              issuing this primitive.
 * @param RecProfileIdList      The list of profile identifiers supported by
 *                              the node issuing this primitive.
 * @param ProvPairingRef        The reference to the provisional pairing entry
 *                              if the pair was accepted.
 *
 * @return true if request has been added to NHLE-NWK queue successfully; else
 * false
 *
 * @see nlme_pair_indication
 * @ingroup group_RF4CONTROL_NWK_API
 */
#if (defined RF4CE_TARGET) || (defined DOXYGEN) || (defined RF4CE_PLATFORM)
bool nlme_pair_response(nwk_enum_t Status, uint16_t DstPANId,
uint64_t DstIEEEAddr,
uint8_t RecAppCapabilities, dev_type_t RecDevTypeList[3],
profile_id_t RecProfileIdList[7], uint8_t ProvPairingRef);
#endif

/**
 * @brief NLME-RESET.request primitive API
 *
 * The NLME-RESET.request primitive allows the application entity to request a
 * reset
 * of the NWK layer.
 *
 * @param SetDefaultNIB If TRUE, the NWK layer is reset and all NIB attributes
 *                      are set to their default values. If FALSE, the NWK
 *                      layer is reset but all NIB attributes retain their
 *                      values prior to the generation of the
 *                      NLME-RESET.request primitive.
 * @param confirm_cb    Call back pointer for the confirmation
 *
 * @return true if request has been added to NHLE-NWK queue successfully; else
 * false
 *
 * @see nlme_reset_confirm
 * @ingroup group_RF4CONTROL_NWK_API
 */
bool nlme_reset_request(bool SetDefaultNIB,
		FUNC_PTR confirm_cb
		);

/**
 * @brief NLME-RX-ENABLE.request primitive API
 *
 * The NLME-RX-ENABLE.request primitive allows the application to request that
 * the
 * receiver is either enabled (for a finite period or until further notice) or
 * disabled.
 *
 * @param RxOnDuration  The number of MAC symbols for which the receiver is to
 *                      be enabled. To activate power saving mode, this value
 *                      should correspond to the value of nwkActivePeriod and
 *                      nwkActivePeriod should not be equal to 0x000000.
 *                      If this parameter is equal to 0x000000, the receiver is
 *                      disabled, allowing the node to enter dormant power save
 *                      mode. If this parameter is equal to 0xffffff, the
 *                      receiver is enabled, allowing the node to come out of
 *                      power save mode.
 * @param confirm_cb    Call back pointer for the confirmation
 *
 * @return true if request has been added to NHLE-NWK queue successfully; else
 * false
 *
 * @see nlme_rx_enable_confirm
 * @ingroup group_RF4CONTROL_NWK_API
 */
bool nlme_rx_enable_request(uint32_t RxOnDuration,
		FUNC_PTR confirm_cb
		);

/**
 * @brief NLME-SET.request primitive API
 *
 * The NLME-SET.request primitive allows the application to request the NLME
 * change
 * the value of a NIB attribute.
 *
 * @param NIBAttribute      The identifier of the NIB attribute to write.
 * @param NIBAttributeIndex The index within the table or array of the
 *                          specified NIB attribute to write.
 * @param NIBAttributeValue The value of the indicated attribute to write.
 * @param confirm_cb        Call back pointer for the confirmation
 *
 * @return true if request has been added to NHLE-NWK queue successfully; else
 * false
 *
 * @see nlme_set_confirm
 * @ingroup group_RF4CONTROL_NWK_API
 */
#if (NWK_SET == 1) || (defined DOXYGEN) || (defined RF4CE_PLATFORM)
bool nlme_set_request(nib_attribute_t NIBAttribute, uint8_t NIBAttributeIndex,
		uint8_t *NIBAttributeValue,
		FUNC_PTR confirm_cb
		);

#endif

/**
 * @brief NLME-START.request primitive API
 *
 * The NLME-START.request primitive allows the application to request the NLME
 * start a network.
 *
 * @param confirm_cb        Call back pointer for the confirmation
 *
 * @return true if request has been added to NHLE-NWK queue successfully; else
 * false
 *
 * @see nlme_start_confirm
 * @ingroup group_RF4CONTROL_NWK_API
 */
bool nlme_start_request(FUNC_PTR confirm_cb);

/**
 * @brief NLME-UNPAIR.request primitive API
 *
 * The NLME-UNPAIR.request primitive allows the application to request the NLME
 * removes a pairing link with another device both in the local and remote
 * pairing tables.
 *
 * @param PairingRef    The reference into the local pairing table of the entry
 *                      that is to be removed.
 * @param confirm_cb    Call back pointer for the confirmation
 *
 * @return true if request has been added to NHLE-NWK queue successfully; else
 * false
 *
 * @see nlme_unpair_confirm
 * @ingroup group_RF4CONTROL_NWK_API
 */
#if (NWK_UNPAIR_REQ_CONF == 1) || (defined DOXYGEN) || (defined RF4CE_PLATFORM)
bool nlme_unpair_request(uint8_t PairingRef,
		FUNC_PTR confirm_cb
		);

#endif

/**
 * @brief NLME-UNPAIR.response primitive API
 *
 * The NLME-UNPAIR.response primitive allows the application to notify the NLME
 * that the pairing link indicated via the NLME-UNPAIR.indication primitive can
 * be
 * removed from the pairing table.
 *
 * @param PairingRef    The reference into the local pairing table of the entry
 *                      that is to be removed.
 *
 * @return true if request has been added to NHLE-NWK queue successfully; else
 * false
 *
 * @see nlme_unpair_indication
 * @ingroup group_RF4CONTROL_NWK_API
 */
#if (NWK_UNPAIR_IND_RESP == 1) || (defined DOXYGEN) || (defined RF4CE_PLATFORM)
bool nlme_unpair_response(uint8_t PairingRef);

#endif

/**
 * @brief NLME-UPDATE-KEY.request primitive API
 *
 * The NLME-UPDATE-KEY.request primitive allows the application to request the
 * NLME change the security link key of an entry in the pairing table.
 *
 * @param PairingRef    The reference into the local pairing table of the entry
 *                      whose key is to be updated.
 * @param NewLinkKey    The security link key to replace the key in the pairing
 *                      table.
 * @param confirm_cb    Call back pointer for the confirmation
 *
 * @return true if request has been added to NHLE-NWK queue successfully; else
 * false
 *
 * @see nlme_update_key_confirm
 * @ingroup group_RF4CONTROL_NWK_API
 */
#if (NWK_UPDATE_KEY == 1) || (defined DOXYGEN) || (defined RF4CE_PLATFORM)
bool nlme_update_key_request(uint8_t PairingRef, uint8_t NewLinkKey[16],
FUNC_PTR confirm_cb
);
#endif

/**
 * @brief Sets channel agility mode
 *
 * The function configures the channel agility mode.
 * Parameters of the channel agility feature, like ScanInterval, EdThreshold,
 * IndicationCountThreshold and AutoChannelMode, can be set via
 * NLME-SET.request.
 * If power save mode is enabled, the channel agility mode is suspended until
 * power save mode is left again.
 *
 * @param AgilityMode   Channel agility mode: AG_ONE_SHOT - scans once
 *                                            AG_PERIODIC - starts periodic
 * scanning
 *                                            AG_STOP - stops periodic scanning
 * @param confirm_cb    Call back pointer for the confirmation
 *
 * @return true if request has been added to NHLE-NWK queue successfully; else
 * false
 *
 * @see nwk_ch_agility_confirm
 * @ingroup group_RF4CONTROL_NWK_API
 */
#if (defined CHANNEL_AGILITY) || (defined DOXYGEN) || (defined RF4CE_PLATFORM)
bool nwk_ch_agility_request(nwk_agility_mode_t AgilityMode,
		FUNC_PTR confirm_cb
		);

#endif

/**
 * @brief Initialization of RF4CE network layer
 *
 * The RF4CE network layer needs to initialize after power-up of the MCU. The
 * function nwk_init() initializes the network layer including the underlying
 * layers, like MAC and queue and buffer management.
 *
 * @return Status of the initialization
 *
 * @ingroup group_RF4CONTROL_GENERIC_API
 */
nwk_enum_t nwk_init(void);

/**
 * @brief Task handling of RF4CE network layer
 *
 * The function nwk_task() handles the RF4CE network layer and layers below.
 * It needs to called frequently.
 *
 * @return true if request has been added to NHLE-NWK queue successfully; else
 * false
 *
 * @ingroup group_RF4CONTROL_GENERIC_API
 */
bool nwk_task(void);

/**
 * @brief Checks if the entire stack is idle
 *
 * The function nwk_ready_to_sleep() checks the RF4CE network layer and layers
 * below if they are busy or idle. The function can be used to determine if the
 * MCU can be set to a power save mode.
 *
 * @return true if all layers are idle; else false
 *
 * @ingroup group_RF4CONTROL_GENERIC_API
 */
bool nwk_ready_to_sleep(void);

/**
 * @brief Checks if the network stack is idle
 *
 * The function nwk_state_idle() checks the RF4CE network layer is busy or idle.
 * @return true if nwk layer is idle; else false
 *
 * @ingroup group_RF4CONTROL_GENERIC_API
 */
bool nwk_stack_idle(void);

/**
 * @brief Reads pairing reference from EEPROM
 *
 * The function read_pairing_ref() reads the pairing references from the EEPROM
 * for the application scope. It's a function supporting the application - not
 * used by NWK.
 * If a pairing reference is equal to 0xFF, it is unused.
 *
 * @param pairing_ref   Pointer to storage location
 * @param length        Number of entries to read
 *
 * @see store_pairing_ref
 * @ingroup group_RF4CONTROL_GENERIC_API
 */
void read_pairing_ref(uint8_t *pairing_ref, uint8_t length);

/**
 * @brief Stores pairing references to EEPROM
 *
 * The function store_pairing_ref() stores the pairing references to the EEPROM
 * for the application scope. It's a function supporting the application - not
 * used by NWK.
 *
 * @param pairing_ref   Pointer to storage location
 * @param length        Number of entries to store
 *
 * @see read_pairing_ref
 * @ingroup group_RF4CONTROL_GENERIC_API
 */
void store_pairing_ref(uint8_t *pairing_ref, uint8_t length);

/**
 * @brief Clears pairing references within EEPROM
 *
 * The function clear_pairing_ref() clears pairing references within the
 * EEPROM for the application scope. It's a function supporting the application
 * - not used by NWK.
 *
 * @param first_ref     First entry to clear
 * @param length        Number of entries to clear
 *
 * @see store_pairing_ref
 * @ingroup group_RF4CONTROL_GENERIC_API
 */
void clear_pairing_ref(uint8_t first_ref, uint8_t length);

/**
 * @brief Gets the NIB attribute value size
 *
 * @param nib_attribute_id NIB attribute ID
 *
 * @return Size of NIB attribute value in bytes
 *
 * @see nlme_get_confirm
 * @ingroup group_RF4CONTROL_GENERIC_API
 */
uint8_t nwk_get_nib_attribute_size(uint8_t nib_attribute_id);

/**
 * @brief Registering network indication callback
 *
 * The application/profile needs to register the network indication callback
 * functions so that the corresponding function will get called
 * by the network layer on receiving the indication
 *
 * @param nwk_indication_callback_t    nwk_indication_cb
 *
 * @see nwk_indication_callback_t
 * @ingroup group_RF4CONTROL_NWK_API
 */

void register_nwk_indication_callback(
		nwk_indication_callback_t *nwk_indication_cb);

#endif /* RF4CE_H */
