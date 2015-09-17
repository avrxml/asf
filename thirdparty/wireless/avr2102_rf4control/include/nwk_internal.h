/**
 * @file nwk_internal.h
 *
 * @brief
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
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* Prevent double inclusion */
#ifndef NWK_INTERNAL_H
#define NWK_INTERNAL_H

/* === Includes ============================================================= */

#include "rf4ce.h"
#include "node_config.h"
#include "qmm.h"

/* === Macros =============================================================== */

#ifndef FALSE
#define FALSE false
#endif

#ifndef TRUE
#define TRUE true
#endif

/* NKW constants */

#ifndef RF4CE_VERSION
#define RF4CE_VERSION   100 /* = 1.0 */
#endif

extern uint8_t Base_Channel;
extern uint8_t Channel_Offset;

/* The channel mask to use in all scanning operations. */
#define nwkcChannelMask             (((uint32_t)1 << Base_Channel) | \
	((uint32_t)1 << (Base_Channel + Channel_Offset)) | \
	((uint32_t)1 << (Base_Channel + Channel_Offset + Channel_Offset)))

/* The amount that needs to be added to the frame counter if a device is reset.
**/
#define nwkcFrameCounterWindow      1024

/* The length, in octets, of the MAC beacon payload field, as used by the RF4CE
 * protocol. */
#define nwkcMACBeaconPayloadLength  2

/* The maximum number of entries supported in the node descriptors list
 * generated through the discovery process.
 * Implementation specific but >= nwkcMinNodeDescListSize */
#define nwkcMaxNodeDescListSize     3  /* 3 = (LARGE_BUFFER_SIZE - 3) /
	                                * sizeof(node_desc_t) */

/* The maximum duty cycle in MAC symbols, permitted for a power saving device.
**/
/* (1s) */
#define nwkcMaxDutyCycle            62500

/* The maximum time, in MAC symbols, to wait for each security link key seed
 * exchange. */
/* (see Equation 1) (60ms) */
#define nwkcMaxKeySeedWaitTime      3750

/* The maximum number of entries supported in the pairing table.
 * Implementation specific. */
#define nwkcMaxPairingTableEntries  NWKC_MAX_PAIRING_TABLE_ENTRIES

/* The maximum acceptable power, in dBm, at which commands relating to security
 * should be sent. */
#define nwkcMaxSecCmdTxPower        -15

/* The minimum receiver on time, in MAC symbols, permitted for a power saving
 * device. */
/*  (16.8ms) (see Equation 2) */
#define nwkcMinActivePeriod         1050

/* The minimum number of pairing table entries that a controller device shall
 * support. */
#define nwkcMinControllerPairingTableSize   1

/* The minimum number of entries that must be supported in the node descriptor
 * table
 * generated through the discovery process. */
#define nwkcMinNodeDescTableSize    3

/* The minimum overhead the NWK layer adds to an application payload. */
#define nwkcMinNWKHeaderOverhead    5

/* The minimum number of pairing table entries that a target device shall
 * support. */
#define nwkcMinTargetPairingTableSize 5

/* The capabilities of this node. Implementation specific according to the
 * format
 * illustrated in Figure 26.
 * Implementation specific */
#define NODE_TYPE_MASK                      0x01
#define PWR_SRC_MASK                        0x02
#define SECURITY_CAPABLE_MASK               0x04
#define CH_NORM_CAPABLE_MASK                0x08

#define nwkcNodeCapabilities                NWKC_NODE_CAPABILITIES

/* The identifier of the NWK layer protocol being used by this device. */
#define nwkcProtocolIdentifier  0xce

/* The version of the RF4CE protocol implemented on this device. */
/* 0b01 */
#define nwkcProtocolVersion     1

/* The manufacturer specific vendor identifier for this node.
 * Vendor ID corresponding to the device manufacturer */
#define nwkcVendorIdentifier    NWKC_VENDOR_IDENTIFIER

/* The manufacturer specific identification string for this node.
 * Implementation specific format of 7 octets. */
#define nwkcVendorString        NWKC_VENDOR_STRING

/* Network Information Base attributes default values */

/* The active period of a device in MAC symbols. */
/* Default value: (268.44s) */
#define nwkActivePeriod_def         0xffffff

/* The logical channel that was chosen when the RC PAN was formed. (Target
 * device only) */
#define nwkBaseChannel_def          Base_Channel    /* 15 */

/* The LQI threshold below which discovery requests will be rejected. */
#define nwkDiscoveryLQIThreshold_def    0xff

/* The interval at which discovery attempts are made on all channels. */
#define nwkDiscoveryRepetitionInterval_def  0x0030d4

/* The duty cycle of a device in MAC symbols. A value of 0x000000 indicates the
 * device
 * is not using power saving. */
#define nwkDutyCycle_def            0x000000

/* The frame counter added to the transmitted NPDU. */
#define nwkFrameCounter_def         0x00000001

/* Indicates whether the NLME indicates the reception of discovery request
 * command frames to the application. TRUE indicates that the NLME notifies
 * the application. */
#define nwkIndicateDiscoveryRequests_def    FALSE

/* The power save mode of the node. TRUE indicates that the device is operating
 * in power save mode. */
#define nwkInPowerSave_def              FALSE

/* List of pairing table entries (see Table 41)
 * The pairing table managed by the device.
 * Default value: Empty list. */
#define nwkPairingTable_def             NULL

/* The maximum number of discovery attempts made at the
 * nwkDiscoveryRepetitionInterval rate. */
#define nwkMaxDiscoveryRepetitions_def  0x01

/* The maximum number of backoffs the MAC CSMA-CA algorithm will attempt before
 * declaring a channel access failure for the first transmission attempt.*/
#define nwkMaxFirstAttemptCSMABackoffs_def 4

/* The maximum number of MAC retries allowed after a transmission failure
 * for the first transmission attempt. */
#define nwkMaxFirstAttemptFrameRetries_def  3

/* The maximum number of node descriptors that can be obtained before reporting
 * to the application. */
#define nwkMaxReportedNodeDescriptors_def 0x03

/* The maximum time in MAC symbols, a device shall wait for a response command
 * frame
 * following a request command frame. */
/* Default value: (100ms) */
#define nwkResponseWaitTime_def 0x00186a

/* A measure of the duration of a scanning operation, according to [R1]. */
#define nwkScanDuration_def 6   /* 960 * (2^n + 1) symbols; here: ~1sec */

/* The user defined character string used to identify this node. */
/* Default value: Empty string. */
#define nwkUserString_def   NULL

/* Constants used for channel agility */
#define nwkChAgScanInterval_def     0x00393870 /* MAC symbols => 60sec */
#define nwkChAgEdThreshold_def      10  /* 10 -> -80 dBm */
#define nwkChAgEnabled_def          false

/* Frame type sub-field values, see table 33, 34 */
#define NWK_FRAME_TYPE_RESERVED     0
#define NWK_FRAME_TYPE_DATA         1
#define NWK_FRAME_TYPE_CMD          2
#define NWK_FRAME_TYPE_VENDOR       3
#define NWK_FRAME_TYPE_MASK         0x03
/* #define NWK_FCF_PROTOCOL_ID (nwkcProtocolVersion << 3) */
#define NWK_FCF_SEC_ENABLED (1 << 2)
#define NWK_FCF_SEC_DISABLED (0 << 2)
#define NWK_FCF_CH_NOT_SPEC (0 << 6)
#define NWK_FCF_CH_15       (1 << 6)
#define NWK_FCF_CH_20       (2 << 6)
#define NWK_FCF_CH_25       (3 << 6)
#define NWK_FCF_CH_MASK     0xC0
#define NWK_FCF_PROTOCOL_VERSION    (nwkcProtocolVersion << 3)
#define NWK_FCF_RESERVED    (1 << 5)

/* Offset of NIB storage location within the EEPROM */
#define EE_NIB_OFFSET                (8)

/* Offset of pairing reference storage location within the EEPROM */
#define EE_PAIRING_REF_OFFSET        (EE_NIB_OFFSET + sizeof(nib_t))

/* Off set of own frame counter storage location within the EEPROM */
#define EE_OWN_FRAME_CNTR_OFFSET    (EE_NIB_OFFSET + \
	offsetof(struct nib_tag, FrameCounter))

/* === Types ================================================================ */

/* typedef void (*handler_t)(uint8_t *); */

/* packing of the data types starts here. Some architecture requires the data
 * type to be aligned, like CM0 */
__PACK__DATA__

/* Structure implementing the NIB */
typedef struct nib_tag {
	uint32_t ActivePeriod;
	uint8_t BaseChannel;
#if (nwkcNodeCapabilities & NodeTypeTarget) || (defined RF4CE_PLATFORM)
	uint16_t PanIdentifier;
	uint16_t ShortAddress;
#endif
	uint8_t DiscoveryLQIThreshold;
	uint32_t DiscoveryRepetitionInterval;
	uint32_t DutyCycle;
	uint32_t FrameCounter;
	bool IndicateDiscoveryRequests;
	bool InPowerSave;
	uint8_t MaxDiscoveryRepetitions;
	uint8_t MaxFirstAttemptCSMABackoffs;
	uint8_t MaxFirstAttemptFrameRetries;
	uint8_t MaxReportedNodeDescriptors;
	uint32_t ResponseWaitTime;
	uint8_t ScanDuration;
	uint8_t UserString[15];
	pairing_table_t PairingTable[nwkcMaxPairingTableEntries];

	/**
	 * NWK variables used to store private NIB attribute values
	 */
#if (defined PRIVATE_NIB) || (defined RF4CE_PLATFORM)
	uint16_t PrivateVendorIdentifier;
	uint8_t PrivateVendorString[7];
	uint8_t PrivateNodeCapabilities;
#endif
#if (defined CHANNEL_AGILITY) || (defined RF4CE_PLATFORM)
	bool ChAgEnabled;
	uint32_t ChAgScanInterval;
	uint8_t ChAgEdThreshold;
#endif
#ifdef ZRC_PROFILE
#ifndef RF4CE_TARGET
	uint8_t aplKeyRepeatInterval;
#endif
#ifdef RF4CE_TARGET
	uint8_t aplKeyRepeatWaitTime;
#endif
#endif /* #ifdef ZRC_PROFILE */

#if (defined PBP_ORG) || (defined PBP_REC)
	uint8_t aplKeyExchangeTransferCount;
#endif
} nib_t;

/* Timer ID's used by NWK */
typedef struct network_timer_tag {
	uint8_t network;
#if ((defined CHANNEL_AGILITY) || (defined RF4CE_PLATFORM))
	uint8_t channel_agility;
#endif  /* #if (defined CHANNEL_AGILITY) || (defined RF4CE_PLATFORM) */

#ifdef PBP_REC
	uint8_t pbp;
#endif

#ifdef ZRC_PROFILE
	uint8_t zrc_profile;
#endif
#ifdef ZID_PROFILE
	uint8_t zid_profile;
#endif
} network_timer_t;

/* The command frames identifier, Table 36 */
typedef enum {
	DISCOVERY_REQUEST   = 0x01,
	DISCOVERY_RESPONSE  = 0x02,
	PAIR_REQUEST        = 0x03,
	PAIR_RESPONSE       = 0x04,
	UNPAIR_REQUEST      = 0x05,
	KEY_SEED            = 0x06,
	PING_REQUEST        = 0x07,
	PING_RESPONSE       = 0x08
} SHORTENUM nwk_cmd_id_t;

/* Main NWK state machine */
typedef enum nwk_state_tag {
	NWK_STATE_IDLE,
	NWK_COLD_START,
	NWK_WARM_START,

	NWK_NLDE_DATA_REQUEST,
	NWK_NLDE_DATA_INDICATION,
	NWK_NLDE_DATA_CONFIRM,

#if (defined RF4CE_TARGET) || (defined RF4CE_PLATFORM)
	NWK_NLME_AUTO_DISCOVERY_REQUEST,
	NWK_NLME_AUTO_DISCOVERY_CONFIRM,

	NWK_NLME_COMM_STATUS_INDICATION,
#endif
	NWK_NLME_DISCOVERY_REQUEST,
#if (defined RF4CE_TARGET) || (defined RF4CE_PLATFORM)
	NWK_NLME_DISCOVERY_INDICATION,
	NWK_NLME_DISCOVERY_RESPONSE,
#endif
	NWK_NLME_DISCOVERY_CONFIRM,

	NWK_NLME_GET_REQUEST,
	NWK_NLME_GET_CONFIRM,

	NWK_NLME_PAIR_REQUEST,
#if (defined RF4CE_TARGET) || (defined RF4CE_PLATFORM)
	NWK_NLME_PAIR_INDICATION,
	NWK_NLME_PAIR_RESPONSE,
#endif
	NWK_NLME_PAIR_CONFIRM,

	NWK_NLME_RESET_REQUEST,
	NWK_NLME_RESET_CONFIRM,

	NWK_NLME_RX_ENABLE_REQUEST,
	NWK_NLME_RX_ENABLE_CONFIRM,

	NWK_NLME_SET_REQUEST,
	NWK_NLME_SET_CONFIRM,

	NWK_NLME_START_REQUEST,
	NWK_NLME_START_CONFIRM,

	NWK_NLME_UNPAIR_REQUEST,
	NWK_NLME_UNPAIR_INDICATION,
	NWK_NLME_UNPAIR_RESPONSE,
	NWK_NLME_UNPAIR_CONFIRM
#if (defined CHANNEL_AGILITY) || (defined RF4CE_PLATFORM)
	, NWK_CH_AGILITY_REQ,
	NWK_CH_AG_SCAN
#endif
#if (defined RF4CE_SECURITY) || (defined RF4CE_PLATFORM)
	, NWK_NLME_UPDATE_KEY_REQUEST,
	NWK_NLME_UPDATE_KEY_CONFIRM,
	NWK_SEEDING,
	NWK_PING_REQUEST,
	NWK_WAITING_FOR_PING_REQ,
	NWK_WAITING_FOR_PING_RESP,
	NWK_SENDING_PING_RESP
#endif
} SHORTENUM nwk_state_t;

typedef struct nwk_frame_header_tag {
	uint8_t frame_control;
	uint32_t frame_counter;
	profile_id_t profile_id;
	uint8_t vendor_id;
} nwk_frame_header_t;

typedef struct nwk_frame_tag {
	nwk_frame_header_t header;
	uint8_t *payload;
	uint8_t *mic;
} nwk_frame_t;

typedef struct nwk_cmd_frame_hdr_tag {
	uint8_t frame_control;
	uint32_t frame_counter;
} nwk_cmd_frame_hdr_t;

typedef struct nwk_cmd_frame_tag {
	nwk_cmd_frame_hdr_t hdr;
	/* nwk_cmd_id_t cmd_id; */
	uint8_t cmd_payload[1];
} nwk_cmd_frame_t;

typedef struct vendor_info_tag {
	uint16_t vendor_id;
	uint8_t vendor_string[7];
} vendor_info_t;

typedef struct app_info_tag {
	uint8_t app_cap;
	uint8_t user_string[15];
	dev_type_t dev_type_list[3];
	profile_id_t profile_id_list[7];
} app_info_t;

typedef struct nwk_disc_req_cmd_tag {
	uint8_t cmd_id;
	uint8_t node_cap;
	vendor_info_t vendor_info;
	app_info_t app_info;
	uint8_t req_dev_type;
} nwk_disc_req_cmd_t;

typedef struct nwk_pair_req_cmd_tag {
	uint8_t cmd_id;
	uint16_t net_addr;
	uint8_t node_cap;
	vendor_info_t vendor_info;
	app_info_t app_info;
	uint8_t key_ex_cnt;
} nwk_pair_req_cmd_t;

typedef struct nwk_pair_resp_cmd_tag {
	uint8_t cmd_id;
	uint8_t Status;
	uint16_t allo_net_addr;
	uint16_t remote_net_addr;
	uint8_t node_cap;
	vendor_info_t vendor_info;
	app_info_t app_info;
} nwk_pair_resp_cmd_t;

typedef struct nwk_seed_cmd_tag {
	uint8_t cmd_id;
	uint8_t seed_seq_no;
	uint8_t seed_data[80];
} nwk_seed_cmd_t;

typedef struct nwk_ping_req_cmd_tag {
	uint8_t cmd_id;
	uint8_t ping_opt;
	uint8_t ping_payload[4];
	uint8_t nfr[4];
} nwk_ping_req_cmd_t;

typedef struct nwk_ping_req_frm_tag {
	uint8_t frame_control;
	uint32_t frame_counter;
	nwk_ping_req_cmd_t ping_cmd;
} nwk_ping_req_frm_t;

typedef struct nwk_ping_resp_cmd_tag {
	uint8_t cmd_id;
	uint8_t ping_opt;
	uint8_t ping_payload[4];
	uint8_t nfr[4];
} nwk_ping_resp_cmd_t;

typedef struct nwk_ping_resp_frm_tag {
	uint8_t frame_control;
	uint32_t frame_counter;
	nwk_ping_req_cmd_t ping_cmd;
} nwk_ping_resp_frm_t;

/** type used for internal autodiscovery state machine */
typedef enum auto_disc_state_tag {
	WAITING_FOR_FIRST_REQUEST,
	WAITING_FOR_SECOND_REQUEST,
	AUTO_DISC_DONE
} SHORTENUM auto_disc_state_t;

typedef struct auto_disc_var_tag {
	auto_disc_state_t auto_disc_state;
	uint8_t *auto_disc_req_buf_ptr;
	uint8_t *first_disc_req;
} auto_disc_var_t;

typedef struct ch_ag_var_tag {
	uint8_t *ch_agility_req_buf;
} ch_ag_var_t;

typedef struct data_var_tag {
	uint8_t *data_req_ptr;
	uint8_t *current_mcps_data_req_msg;
	bool tx_timer_expired;
} data_var_t;

typedef struct disc_var_tag {
	uint8_t *disc_conf_buf_ptr;
	uint8_t disc_repetition;
	uint8_t *mcps_data_buf_ptr;
	uint8_t *disc_req_buf_ptr;
	uint32_t disc_trail_start_time;
#if (defined RF4CE_TARGET) || (defined RF4CE_PLATFORM)
	uint64_t DiscRespDstIEEEAddr;
#endif
} disc_var_t;

typedef struct pair_var_tag {
	uint8_t *pair_req_msg_buf;
	uint8_t provisional_entry;
	uint8_t ieee_addr[8];
	uint8_t *incoming_pair_resp_buf;
#if (defined RF4CE_TARGET) || (defined RF4CE_PLATFORM)
	uint8_t *pair_resp_msg_ptr;
	bool pairing_granted;
	uint64_t originator_ieee_addr;
	uint16_t originator_pan_id;
#endif
#if (defined RF4CE_SECURITY) || (defined RF4CE_PLATFORM)  /* target and
	                                                   * controller */
	uint8_t key_ex_transfer_count;
#endif
#ifdef RF4CE_PLATFORM
	uint8_t *seed_mcps_data_buf;
	bool seed_wait_timer_expired;
	uint8_t *last_seed_data_buf;
	uint8_t *accumulated_seed_data_buf; /* if target -> == pair_resp_msg_ptr
	                                    **/
	uint8_t seed_seq_num;
#elif (defined RF4CE_SECURITY)
#if (defined RF4CE_TARGET)
	uint8_t *seed_mcps_data_buf;
	bool seed_wait_timer_expired;
#endif
	/* target and controller */
	uint8_t *last_seed_data_buf;
	uint8_t *accumulated_seed_data_buf; /* if target -> == pair_resp_msg_ptr
	                                    **/
	uint8_t seed_seq_num;
#endif  /* #ifdef RF4CE_PLATFORM */
} pair_var_t;

typedef struct reset_var_tag {
	bool set_default_value;
	uint8_t *nlme_reset_req_ptr;
} reset_var_t;

typedef struct rx_en_var_tag {
	uint8_t *nlme_rx_enable_req_buf;
} rx_en_var_t;

typedef struct start_var_tag {
	uint8_t *nlme_start_req_buf_ptr;
#if (defined RF4CE_TARGET) || (defined RF4CE_PLATFORM)
	bool duplicate_pan_id_found;
#endif
} start_var_t;

typedef struct unpair_var_tag {
	uint8_t pair_ref;
} unpair_var_t;

typedef union nwk_var_tag {
	auto_disc_var_t auto_disc;
	ch_ag_var_t ch_ag;
	data_var_t data;
	disc_var_t disc;
	pair_var_t pair;
	reset_var_t reset;
	rx_en_var_t rx_en;
	start_var_t start;
	unpair_var_t unpair;
} nwk_var_t;

typedef struct phy_layer_attrib_tag {
	/**
	 * 64-bit (IEEE) address of the node.
	 */
	uint64_t IeeeAddress;

	uint32_t ChannelsSupported;

	/**
	 * 16-bit short address of the node.
	 */
	uint16_t ShortAddress;

	/**
	 * 16-bit PAN ID
	 */
	uint16_t PANId;

	/**
	 * Current RF channel to be used for all transmissions and receptions.
	 */
	uint8_t CurrentChannel;
} phy_layer_attrib_t;

/* Reset the Packing of data types here */
__PACK__RST_DATA__

/* === Externals ============================================================ */

/* nwk.c */
extern nwk_state_t nwk_state;
extern nib_t nib;
extern queue_t nhle_nwk_q;
extern queue_t nwk_nhle_q;
extern nwk_var_t nwk_var;

extern phy_layer_attrib_t phy_layer_attrib;

extern network_timer_t network_timer;

/* main.c */
FLASH_EXTERN(uint8_t app_user_string[15]);
FLASH_EXTERN(uint16_t VendorIdentifier);
FLASH_EXTERN(uint8_t vendor_string[7]);

/* === Prototypes =========================================================== */

/* nwk.c */
void store_nib(void);
void read_nib(void);
void enter_normal_operation(void);

/* nwk_dispatcher.c */
void nwk_dispatch_event(uint8_t *event);

/* nwk_start.c */
void issue_start_conf(uint8_t status);

#if (defined RF4CE_TARGET) || (defined CHANNEL_AGILITY) || \
	(defined RF4CE_PLATFORM)
void start_scan(uint8_t scan_type, uint8_t channel);

#endif

/* nwk_init.c */
void issue_nlme_reset_conf(uint8_t status);

#if (defined RF4CE_TARGET) || (defined RF4CE_PLATFORM)
void warm_start_conf_cb(uint8_t *msg_buf);

#endif

/* nwk_rx_enable.c */
void handle_rx_duty_cycle(void);
void duty_cycle_end_cb(void *callback_parameter);

/* nwk_discovery.c */
#if (defined RF4CE_TARGET) || (defined RF4CE_PLATFORM)
void discovery_response_frm_conf_cb(uint8_t *msg_buf);
void incoming_discovery_request(uint8_t *msg_buf);

#endif
void handle_discovery_frm_conf_cb(void);
void incoming_discovery_response(uint8_t *msg_buf);

#if ((NWK_DISCOVERY_IND_RESP == 1) || (NWK_PAIR_REQ_CONF == 1) || \
	(defined RF4CE_TARGET))
bool is_pairing_table_full(void);

#endif

/* nwk_auto_discovery.c */
#if (defined RF4CE_TARGET) || (defined RF4CE_PLATFORM)
void auto_disc_incoming_disc_req(uint8_t *msg_buf);
void auto_disc_resp_mcps_data_conf_cb(uint8_t *msg_buf);

#endif

/* nwk_pair.c */
void pair_req_mcps_data_conf_cb(uint8_t *msg_buf);
void issue_pair_confirm(nwk_enum_t status);

void incoming_pair_response(uint8_t *msg_buf);
bool get_pairing_table_entry(uint8_t *entry, uint64_t *ieee_addr);
void remove_pairing_entry(uint8_t entry);

#if (defined RF4CE_TARGET) || (defined RF4CE_PLATFORM)
void incoming_pair_request(uint8_t *msg_buf);
void pair_resp_mcps_data_conf_cb(uint8_t *msg_buf);
void issue_comm_status_msg(uint8_t status);

#endif  /* #ifdef RF4CE_TARGET */

/* nwk_seed.c */
#if (defined RF4CE_SECURITY) || (defined RF4CE_PLATFORM)
bool start_waiting_for_seeding(void);
void incoming_seed_msg(uint8_t *msg_buf);

#if (defined RF4CE_TARGET) || (defined RF4CE_PLATFORM)
nwk_enum_t target_start_seeding(void);
void seed_mcps_data_conf_cb(uint8_t *msg_buf);

#endif
#endif

/* nwk_ping.c */
#if (defined RF4CE_SECURITY) || (defined RF4CE_PLATFORM)
void send_ping_request(void);
void ping_req_mcps_data_conf_cb(uint8_t *msg_buf);
void incoming_ping_response(uint8_t *msg_buf);

#if (defined RF4CE_TARGET) || (defined RF4CE_PLATFORM)
void incoming_ping_msg(uint8_t *msg_buf);
void ping_resp_mcps_data_conf_cb(uint8_t *msg_buf);
void wait_for_ping_request_expired_cb(void *callback_parameter);

#endif
#endif

/* nwk_unpair.c */
void unpair_req_mcps_data_conf_cb(uint8_t *msg_buf);
void incoming_unpair_request(uint8_t *msg_buf);

#if (defined CHANNEL_AGILITY) || (defined RF4CE_PLATFORM)
void issue_ch_agility_confirm(nwk_enum_t status, bool ChannelChanged);
void ch_ag_mlme_scan_conf_cb(uint8_t *msg_buf);
void ch_ag_interval_timer_expired_cb(void *callback_parameter);

#endif

/* Function pointers used by the dispatcher */

void nlde_data_request_intern(uint8_t *msg_buf);
void nlde_data_indication_intern(uint8_t *msg_buf);
void nlde_data_confirm_intern(uint8_t *msg);

#if (defined RF4CE_TARGET) || (defined RF4CE_PLATFORM)
void nlme_auto_discovery_request_intern(uint8_t *msg_buf);
void nlme_auto_discovery_confirm_intern(uint8_t *msg_buf);

#endif

void nlme_comm_status_indication_intern(uint8_t *msg_buf);

void nlme_discovery_request_intern(uint8_t *msg_buf);

#if (defined RF4CE_TARGET) || (defined RF4CE_PLATFORM)
void nlme_discovery_indication_intern(uint8_t *msg_buf);
void nlme_discovery_response_intern(uint8_t *msg_buf);

#endif
void nlme_discovery_confirm_intern(uint8_t *msg_buf);

void nlme_get_request_intern(uint8_t *msg_buf);
void nlme_get_confirm_intern(uint8_t *msg_buf);

void nlme_pair_request_intern(uint8_t *msg_buf);

#if (defined RF4CE_TARGET) || (defined RF4CE_PLATFORM)
void nlme_pair_indication_intern(uint8_t *msg_buf);
void nlme_pair_response_intern(uint8_t *msg_buf);

#endif
void nlme_pair_confirm_intern(uint8_t *msg_buf);

void nlme_reset_request_intern(uint8_t *msg_buf);
void nlme_reset_confirm_intern(uint8_t *msg_buf);

void nlme_rx_enable_request_intern(uint8_t *msg_buf);
void nlme_rx_enable_confirm_intern(uint8_t *msg_buf);

void nlme_set_request_intern(uint8_t *msg_buf);
void nlme_set_confirm_intern(uint8_t *msg_buf);

void nlme_start_request_intern(uint8_t *msg_buf);
void nlme_start_confirm_intern(uint8_t *msg_buf);

void nlme_unpair_request_intern(uint8_t *msg_buf);
void nlme_unpair_indication_intern(uint8_t *msg_buf);
void nlme_unpair_response_intern(uint8_t *msg_buf);
void nlme_unpair_confirm_intern(uint8_t *msg_buf);

#if (defined RF4CE_SECURITY) || (defined RF4CE_PLATFORM)
void nlme_update_key_request_intern(uint8_t *msg_buf);
void nlme_update_key_confirm_intern(uint8_t *msg_buf);

#endif

#if (defined CHANNEL_AGILITY) || (defined RF4CE_PLATFORM)
void nwk_ch_agility_request_intern(uint8_t *msg_buf);
void nwk_ch_agility_indication_intern(uint8_t *msg_buf);
void nwk_ch_agility_confirm_intern(uint8_t *msg_buf);

#endif

/* Messages from MAC to NWK */
void mcps_data_conf_cb(uint8_t *msg);
void mcps_data_ind_cb(uint8_t *msg);

#if (defined RF4CE_TARGET) || (defined RF4CE_PLATFORM)
void mlme_beacon_notify_ind_cb(uint8_t *msg);

#endif

#if (defined RF4CE_TARGET) || (defined CHANNEL_AGILITY) || \
	(defined RF4CE_PLATFORM)
void mlme_scan_conf_cb(uint8_t *msg);

#endif

void mlme_reset_conf_cb(uint8_t *msg);
void mlme_rx_enable_conf_cb(uint8_t *msg);

#if (defined RF4CE_TARGET) || (defined RF4CE_PLATFORM)
void mlme_start_conf_cb(uint8_t *msg);

#endif

#endif /* NWK_INTERNAL_H */
