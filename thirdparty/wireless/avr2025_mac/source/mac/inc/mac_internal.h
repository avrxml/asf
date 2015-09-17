/**
 * @file mac_internal.h
 *
 * @brief Declares MAC internal functions, globals, and macros.
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

/* Prevent double inclusion */
#ifndef MAC_INTERNAL_H
#define MAC_INTERNAL_H

/* === Includes ============================================================= */
#include "pal.h"
#include "tal.h"
#include "ieee_const.h"
#include "mac_data_structures.h"
#include "return_val.h"

#if (_DEBUG_ > 0)
/* Needs to be included for make_mac_disp_not_busy() while debugging */
#include "tal_internal.h"
#endif

#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006) || \
	(defined GTS_SUPPORT))
#include "mac_msg_types.h"
#endif

/* === Macros =============================================================== */

#ifdef GTS_DEBUG
/* #define DEBUG_PIN1 IOPORT_CREATE_PIN(PORTE, 6) */
/* #define DEBUG_PIN2 IOPORT_CREATE_PIN(PORTD, 4) */
/* #define DEBUG_PIN3 IOPORT_CREATE_PIN(PORTD, 1) */
/* #define DEBUG_PIN4 IOPORT_CREATE_PIN(PORTD, 0) */
/* #define DEBUG_PIN5 IOPORT_CREATE_PIN(PORTD, 6) */
/* #define DEBUG_PIN6 IOPORT_CREATE_PIN(PORTB, 2) */
/* #define DEBUG_PIN7 IOPORT_CREATE_PIN(PORTB, 3) */
/* #define DEBUG_PIN8 IOPORT_CREATE_PIN(PORTB, 1) */
/* #define DEBUG_PIN9 IOPORT_CREATE_PIN(PORTE, 5) */
/* #define DEBUG_PIN10 IOPORT_CREATE_PIN(PORTG, 0) */
/* #define DEBUG_PIN11 IOPORT_CREATE_PIN(PORTD, 7) */
/* #define DEBUG_PIN12 IOPORT_CREATE_PIN(PORTG, 2) */
/* #define DEBUG_PIN13 IOPORT_CREATE_PIN(PORTE, 7) */
/* #define DEBUG_PIN14 IOPORT_CREATE_PIN(PORTD, 5) */
/* #define DEBUG_PIN15 IOPORT_CREATE_PIN(PORTE, 2)// */
/* #define DEBUG_PIN16 IOPORT_CREATE_PIN(PORTE, 3)// */
/* #define DEBUG_PIN17 IOPORT_CREATE_PIN(PORTB, 7) */

#define DEBUG_PIN1 EXT1_PIN_3
#define DEBUG_PIN2 EXT1_PIN_4
#define DEBUG_PIN3 EXT1_PIN_5
#define DEBUG_PIN4 EXT1_PIN_6
#define DEBUG_PIN5 EXT1_PIN_7
#define DEBUG_PIN6 EXT1_PIN_8
#define DEBUG_PIN7 EXT1_PIN_9
#define DEBUG_PIN8 EXT1_PIN_10
#define DEBUG_PIN9 EXT1_PIN_11
#define DEBUG_PIN10 EXT1_PIN_12
#define DEBUG_PIN11 EXT1_PIN_13
#define DEBUG_PIN12 EXT1_PIN_14
#define DEBUG_PIN13 EXT1_PIN_15
#define DEBUG_PIN14 EXT1_PIN_16
#define DEBUG_PIN15 EXT1_PIN_17 /*  */
#define DEBUG_PIN16 EXT1_PIN_18 /*  */
#define DEBUG_PIN17 EXT3_PIN_5

#define GTS_DEBUG_SLOT_PIN  0
#define GTS_DEBUG_DATA_PIN  1
#define GTS_DEBUG_SET       1
#define GTS_DEBUG_TOGGLE    0
#define GTS_DEBUG_VALUE_HIGH 1
#define GTS_DEBUG_VALUE_LOW 0
#endif

/**
 * \addtogroup group_mac_def
 * @{
 */

/*
 * Beacon order used as timer interval for checking the expiration of indirect
 * transactions in a nonbeacon-enabled network
 */
#define BO_USED_FOR_MAC_PERS_TIME       (0)

/*
 * Final CAP slot in standard superframe without GTS
 */
#define FINAL_CAP_SLOT_DEFAULT          (0x0F)

#if (RF_BAND == BAND_2400)

/**
 * Default value for MIB macAckWaitDuration
 */
#define macAckWaitDuration_def          (54)
#elif (RF_BAND == BAND_900)

/**
 * Default value for MIB macAckWaitDuration
 */

/**
 * The default value for this PIB attribute depends on the current channel page
 * (i.e. the modulation scheme: BPSK or O-QPSK).
 */
#define macAckWaitDuration_def \
	(tal_pib.CurrentPage == 0 ? 120 : 54)

#else   /* Can't happen because RF_BAND is checked above. */
#error "You have got no license for that RF band."
#endif /* RF_BAND */

#if (RF_BAND == BAND_2400)
/* O-QPSK 2.4 GHz */
#define macMaxFrameTotalWaitTime_def    (1986)

#elif (RF_BAND == BAND_900)
/* BPSK 900 MHz */
#define macMaxFrameTotalWaitTime_def    (2784)

#else   /* Can't happen because RF_BAND is checked above. */
#error "You have got no license for that RF band."
#endif /* RF_BAND */

#ifdef GTS_SUPPORT
#ifdef FFD
#define MAX_GTS_ON_PANC       (7)
#define GTS_EXPIRY_BO_0_TO_8  ((1 << ((8 - tal_pib.BeaconOrder) + 1)) + 1)
#define GTS_EXPIRY_BO_9_TO_14 (2 + 1)
#endif /* FFD */

#define GTS_REQ_PAYLOAD_LEN  (2)

#define PANC_SLOT            (1)

/* !!! Warning !!!
 * Do not change the index, mapping for update is done based on this...*/
#define DEV_TX_SLOT_INDEX    (GTS_TX_SLOT)
#define DEV_RX_SLOT_INDEX    (GTS_RX_SLOT)
#define PAN_TX_SLOT_INDEX    ((PANC_SLOT << 1) | GTS_TX_SLOT)
#define PAN_RX_SLOT_INDEX    ((PANC_SLOT << 1) | GTS_RX_SLOT)

#define MAX_GTS_ON_DEV       (4)

#endif /* GTS_SUPPORT */

/* === Types ================================================================ */

/**
 * MAC state type.
 */
__PACK__DATA__
typedef enum mac_state_tag {
	/*
	 * IEEE 802.15.4-defined MAC states.
	 */

	/**
	 * Node is idle,
	 * i.e. it is neither associated nor has started its own network
	 */
	MAC_IDLE = 0,

	/* Device has successfully associated */
	MAC_ASSOCIATED = 1,

	/**
	 * Coordinator successfully associated with PAN Coordinator
	 * and successfully started network with same PAN-Id
	 * (not as PAN Coordinator)
	 */
	MAC_COORDINATOR = 2,

	/** PAN coordinator successfully started */
	MAC_PAN_COORD_STARTED = 3
} SHORTENUM mac_state_t;

/**
 * MAC state type.
 */
typedef enum mac_superframe_state_tag {
	/*
	 * IEEE 802.15.4-defined Superframe states.
	 */
	MAC_ACTIVE_CAP = 0,
#ifdef GTS_SUPPORT

	/* Warning!!! Do not change the order
	 * of below GTS state definitions */
#ifdef FFD
	MAC_ACTIVE_CFP_GTS1,
	MAC_ACTIVE_CFP_GTS2,
	MAC_ACTIVE_CFP_GTS3,
	MAC_ACTIVE_CFP_GTS4,
	MAC_ACTIVE_CFP_GTS5,
	MAC_ACTIVE_CFP_GTS6,
	MAC_ACTIVE_CFP_GTS7,
#endif /* FFD */
	MAC_DEV_GTS_TX,
	MAC_DEV_GTS_IDLE,
	MAC_DEV_GTS_IDLE1,
	MAC_DEV_GTS_IDLE2,
#endif
	MAC_INACTIVE,
	MAC_NOBEACON
} SHORTENUM mac_superframe_state_t;

/**
 * MAC poll states.
 * These states describe the current status of the MAC for polling
 * for devices or coordinators, not for PAN coordinator.
 */
typedef enum mac_poll_state_tag {
	/**
	 * No polling ongoing.
	 */
	MAC_POLL_IDLE = 0,

	/**
	 * Successful transmission of association request frame,
	 * wait for Association response.
	 */
	MAC_AWAIT_ASSOC_RESPONSE,

	/**
	 * Explicit poll ongoing (MLME_POLL.request),
	 * Ack after Data request frame transmission received,
	 * awaiting data response. */
	MAC_POLL_EXPLICIT,

	/**
	 * Implicit poll ongoing (more pending data detected, either in beacon
	 * or
	 * data frame),
	 * awaiting data response, */
	MAC_POLL_IMPLICIT
} SHORTENUM mac_poll_state_t;

/**
 * Device or coordinator scan states.
 */
typedef enum mac_scan_state_tag {
	/**
	 * No scanning ongoing.
	 */
	MAC_SCAN_IDLE = 0,

	/* Scanning in progress. */
	/** ED scan ongoing */
	MAC_SCAN_ED,
	/** Active scan proceeding */
	MAC_SCAN_ACTIVE,
	/** Orphan scan proceeding */
	MAC_SCAN_ORPHAN,
	/** Passive scan proceeding */
	MAC_SCAN_PASSIVE
} SHORTENUM mac_scan_state_t;

/**
 * Device or coordinator sync states.
 */
typedef enum mac_sync_state_tag {
	/** Do not track beacons */
	MAC_SYNC_NEVER = 0,
	/** Track the next beacon */
	MAC_SYNC_ONCE,
	/** Track beacons continuously */
	MAC_SYNC_TRACKING_BEACON,

	/**
	 * Track beacons continuously before beeing associated in order to
	 * obtain
	 * synchronization with desired network
	 */
	MAC_SYNC_BEFORE_ASSOC
} SHORTENUM mac_sync_state_t;

/**
 * MAC sleep state type.
 */
typedef enum mac_radio_sleep_state_tag {
	/**< Radio is awake */
	RADIO_AWAKE = 0,
	/**< Radio is in sleep mode */
	RADIO_SLEEPING
} SHORTENUM mac_radio_sleep_state_t;

/* ! @} */
typedef void (*handler_t)(uint8_t *);

/**
 * \addtogroup group_mac_ds
 * @{
 */

/* Structure implementing the MAC PIBs */
typedef struct mac_pib_tag {
	/**
	 * Holds the 64 bit address of the coordinator with which the
	 * device is associated.
	 */
	uint64_t mac_CoordExtendedAddress;

#if (MAC_INDIRECT_DATA_FFD == 1)

	/**
	 * Holds the maximum time (in superframe periods) that a indirect
	 * transaction
	 * is stored by a PAN coordinator.
	 */
	uint16_t mac_TransactionPersistenceTime;
#endif /* (MAC_INDIRECT_DATA_FFD == 1) */

	/**
	 * Holds the 16 bit short address of the coordinator with which the
	 * device is
	 * associated. A value of 0xfffe indicates that the coordinator is only
	 * using
	 * its 64 bit extended address. A value of 0xffff indicates that this
	 * value is unknown.
	 */
	uint16_t mac_CoordShortAddress;

#if ((MAC_INDIRECT_DATA_BASIC == 1) || defined(BEACON_SUPPORT))

	/**
	 * The maximum number of CAP symbols in a beaconenabled PAN, or symbols
	 * in a
	 * nonbeacon-enabled PAN, to wait either for a frame intended as a
	 * response to
	 * a data request frame or for a broadcast frame following a beacon with
	 * the
	 * Frame Pending subfield set to one.
	 * This attribute, which shall only be set by the next higher layer, is
	 * dependent upon macMinBE, macMaxBE, macMaxCSMABackoffs and the number
	 * of
	 * symbols per octet. See 7.4.2 for the formula relating the attributes.
	 * Maximum values:
	 * O-QPSK (2.4 GHz and 900 MHz for Channel page 2): 25766
	 * BPSK (900 MHz for Channel page 0): 26564
	 * Both values are valid for
	 * macMinBE = 8
	 * macMaxBE = 8
	 * macMaxCSMABackoffs = 5
	 *
	 * This PIB attribute is only used if basic indirect data transmission
	 * is used
	 * or if beacon enabled network is enabled.
	 */
	uint16_t mac_MaxFrameTotalWaitTime;
#endif  /* ((MAC_INDIRECT_DATA_BASIC == 1) || defined(BEACON_SUPPORT)) */

	/**
	 * The maximum time, in multiples of aBaseSuperframeDuration, a device
	 * shall
	 * wait for a response command frame to be available following a request
	 * command frame.
	 */
	uint16_t mac_ResponseWaitTime;

#if (MAC_ASSOCIATION_INDICATION_RESPONSE == 1)

	/**
	 * Holds the value which states whether a coordinator is currently
	 * allowing
	 * association. A value of true indicates that association is permitted.
	 */
	uint8_t mac_AssociationPermit;
#endif /* (MAC_ASSOCIATION_INDICATION_RESPONSE == 1) */

#ifdef GTS_SUPPORT
	uint8_t mac_GTSPermit;
#endif /* GTS_SUPPORT */

#if (MAC_START_REQUEST_CONFIRM == 1)

	/**
	 * Holds the length, in octets, of the beacon payload.
	 */
	uint8_t mac_BeaconPayloadLength;

	/**
	 * Holds the sequence number added to the transmitted beacon frame.
	 */
	uint8_t mac_BSN;
#endif  /* (MAC_START_REQUEST_CONFIRM == 1) */

#if (MAC_ASSOCIATION_REQUEST_CONFIRM == 1)

	/**
	 * Indication of whether the device is associated to the PAN through the
	 * PAN
	 * coordinator. A value of TRUE indicates the device has associated
	 * through the
	 * PAN coordinator. Otherwise, the value is set to FALSE.
	 */
	uint8_t mac_AssociatedPANCoord;
#endif /* (MAC_ASSOCIATION_REQUEST_CONFIRM == 1) */

	/**
	 * Holds the value which states whether a device automatically sends a
	 * data
	 * request command if its address is listed in the beacon frame. A value
	 * of true
	 * indicates that the data request command is automatically sent.
	 */
	uint8_t mac_AutoRequest;

	/**
	 * Holds the value which states the number of backoff periods during
	 * which the
	 * receiver is enabled following a beacon in battery life extension
	 * mode.
	 * This value is dependent on the currently selected logical channel.
	 */
	uint8_t mac_BattLifeExtPeriods;

	/**
	 * Holds the sequence number of the transmitted data or command frame.
	 */
	uint8_t mac_DSN;

	/**
	 * Holds the value which states whether the MAC sublayer is to enable
	 * its
	 * receiver during idle periods.
	 */
	bool mac_RxOnWhenIdle;

	/**
	 * Indication of whether the MAC sublayer has security enabled. A value
	 * of
	 * TRUE indicates that security is enabled, while a value of FALSE
	 * indicates
	 * that security is disabled.
	 */
	bool mac_SecurityEnabled;

#ifdef TEST_HARNESS
	/* Private MAC PIB variables, only valid for testing purposes */

	/**
	 * Holds the private MAC PIB attribute to generate a frame with an
	 * illegale
	 * frame type.
	 */
	uint8_t privateIllegalFrameType;

	/**
	 * Holds the private MAC PIB attribute which suppresses the initiation
	 * of a
	 * data request frame after association request.
	 */
	uint8_t privateNoDataAfterAssocReq;

	/**
	 * Holds the private MAC PIB attribute to pretend virtual Beacon-enabled
	 * PANs.
	 */
	uint8_t privateVirtualPANs;
#endif /* TEST_HARNESS */
} mac_pib_t;
__PACK__RST_DATA__
/* ! @} */
/* === Externals ============================================================ */

/* Global data variables */
extern uint8_t *mac_conf_buf_ptr;

#ifdef BEACON_SUPPORT
extern uint8_t mac_final_cap_slot;
extern bool mac_bc_data_indicated;
#endif  /* BEACON_SUPPORT */

#if (MAC_SCAN_SUPPORT == 1)
extern uint8_t *mac_scan_cmd_buf_ptr;
extern uint8_t mac_scan_orig_channel;
extern uint8_t mac_scan_orig_page;
#if ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) || \
	(MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1))
extern uint16_t mac_scan_orig_panid;
#endif  /* ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) ||
         *(MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1)) */
#endif /* (MAC_SCAN_SUPPORT == 1) */

#if (MAC_START_REQUEST_CONFIRM == 1)
extern uint8_t mac_beacon_payload[];
#endif  /* (MAC_START_REQUEST_CONFIRM == 1) */

extern queue_t tal_mac_q;

#if (MAC_INDIRECT_DATA_FFD == 1)
extern queue_t indirect_data_q;
#endif /* (MAC_INDIRECT_DATA_FFD == 1) */

#if (MAC_START_REQUEST_CONFIRM == 1)
#ifdef BEACON_SUPPORT
extern queue_t broadcast_q;
#endif  /* BEACON_SUPPORT */
#endif /* (MAC_START_REQUEST_CONFIRM == 1) */

/* === Externals ============================================================ */
extern uint8_t mac_last_dsn;
extern uint64_t mac_last_src_addr;
extern parse_t mac_parse_data;
extern mac_radio_sleep_state_t mac_radio_sleep_state;
extern bool mac_busy;
extern bool mac_rx_enabled;
extern mac_state_t mac_state;
extern mac_scan_state_t mac_scan_state;
extern mac_sync_state_t mac_sync_state;
extern mac_poll_state_t mac_poll_state;
extern mac_pib_t mac_pib;
#ifdef BEACON_SUPPORT
extern mac_superframe_state_t mac_superframe_state;
#ifdef GTS_SUPPORT
extern mac_dev_gts_mgmt_t mac_dev_gts_table[];
extern uint8_t mac_dev_gts_table_len;
#ifdef FFD
extern mac_pan_gts_mgmt_t mac_pan_gts_table[];
extern uint8_t mac_pan_gts_table_len;
#endif /* FFD */
#endif /* GTS_SUPPORT */
#endif /* BEACON_SUPPORT*/
/* === Prototypes =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup group_mac
 * \defgroup group_mcl  MAC Core Layer(MCL)
 *
 * The MAC Core Layer (MCL) abstracts and implements IEEE 802.15.4-2006
 * compliant behavior for non-beacon enabled and beacon-enabled network support.
 *
 */
/*@{*/

/* MAC-internal functions -- please keep them alphabetically sorted. */

#if (MAC_START_REQUEST_CONFIRM == 1)
#ifdef BEACON_SUPPORT
void mac_build_and_tx_beacon(bool beacon_enabled);

#else /* No BEACON_SUPPORT */
void mac_build_and_tx_beacon(bool beacon_enabled,
		buffer_t *beacon_buffer_header);

#endif /* BEACON_SUPPORT / No BEACON_SUPPORT */
#endif /* (MAC_START_REQUEST_CONFIRM == 1) */

#if (MAC_INDIRECT_DATA_BASIC == 1)
bool mac_build_and_tx_data_req(bool expl_poll,
		bool force_own_long_addr,
		uint8_t expl_dest_addr_mode,
		address_field_t *expl_dest_addr,
		uint16_t expl_dest_pan_id);

#endif /* (MAC_INDIRECT_DATA_BASIC == 1) */

#if (MAC_START_REQUEST_CONFIRM == 1)
void mac_coord_realignment_command_tx_success(uint8_t tx_status,
		buffer_t *buf_ptr);

#endif /* (MAC_START_REQUEST_CONFIRM == 1) */

#ifdef ENABLE_TSTAMP
void mac_gen_mcps_data_conf(buffer_t *buf_ptr,
		uint8_t status,
		uint8_t handle,
		uint32_t timestamp);

#else
void mac_gen_mcps_data_conf(buffer_t *buf_ptr,
		uint8_t status,
		uint8_t handle);

#endif  /* ENABLE_TSTAMP */

#if (MAC_ASSOCIATION_REQUEST_CONFIRM == 1)
void mac_gen_mlme_associate_conf(buffer_t *buf_ptr,
		uint8_t status,
		uint16_t assoc_short_addr);

#endif /* (MAC_ASSOCIATION_REQUEST_CONFIRM == 1) */

#if (MAC_INDIRECT_DATA_FFD == 1)
void mac_handle_tx_null_data_frame(void);

#endif /* (MAC_INDIRECT_DATA_FFD == 1) */

void mac_idle_trans(void);

#if (MAC_COMM_STATUS_INDICATION == 1)
void mac_mlme_comm_status(uint8_t status,
		buffer_t *buf_ptr);

#endif /* (MAC_COMM_STATUS_INDICATION == 1) */

#if (MAC_DISASSOCIATION_BASIC_SUPPORT == 1)
void mac_prep_disassoc_conf(buffer_t *buf_ptr,
		uint8_t status);

#endif /* (MAC_DISASSOCIATION_BASIC_SUPPORT == 1) */

#if (MAC_ASSOCIATION_INDICATION_RESPONSE == 1)
void mac_process_associate_request(buffer_t *buf_ptr);

#endif /* (MAC_ASSOCIATION_INDICATION_RESPONSE == 1) */

#if (MAC_ASSOCIATION_REQUEST_CONFIRM == 1)
void mac_process_associate_response(buffer_t *buf_ptr);

#endif /* (MAC_ASSOCIATION_REQUEST_CONFIRM == 1) */

#if ((MAC_SCAN_SUPPORT == 1) || (MAC_SYNC_REQUEST == 1))
void mac_process_beacon_frame(buffer_t *msg);

#endif /* ((MAC_SCAN_SUPPORT == 1) || (MAC_SYNC_REQUEST == 1)) */

#if (MAC_START_REQUEST_CONFIRM == 1)
void mac_process_beacon_request(buffer_t *buf_ptr);

#endif /* (MAC_START_REQUEST_CONFIRM == 1) */

#if (MAC_SYNC_LOSS_INDICATION == 1)
void mac_process_coord_realign(buffer_t *buf_ptr);
void mac_sync_loss(uint8_t loss_reason);

#endif /* (MAC_SYNC_LOSS_INDICATION == 1) */

#if (MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1)
void mac_process_orphan_realign(buffer_t *buf_ptr);

#endif /* (MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1) */

#if (MAC_INDIRECT_DATA_FFD == 1)
void mac_process_data_request(buffer_t *buf_ptr);

#endif /* (MAC_INDIRECT_DATA_FFD == 1) */

#if (MAC_INDIRECT_DATA_BASIC == 1)
void mac_process_data_response(void);

#endif /* (MAC_INDIRECT_DATA_BASIC == 1) */

void mac_process_data_frame(buffer_t *buf_ptr);

#if (MAC_DISASSOCIATION_BASIC_SUPPORT == 1)
void mac_process_disassociate_notification(buffer_t *buf_ptr);

#endif /* (MAC_DISASSOCIATION_BASIC_SUPPORT == 1) */

#if (MAC_ORPHAN_INDICATION_RESPONSE == 1)
void mac_process_orphan_notification(buffer_t *buf_ptr);

#endif /* (MAC_ORPHAN_INDICATION_RESPONSE == 1) */

void mac_process_tal_data_ind(uint8_t *msg);

void mac_sleep_trans(void);

#if ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) || \
	(MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1))
void mac_scan_send_complete(retval_t status);

#endif /* ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) ||
        *(MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1)) */

#ifdef BEACON_SUPPORT
#if (MAC_START_REQUEST_CONFIRM == 1)
void mac_start_beacon_timer(void);

#endif  /* (MAC_START_REQUEST_CONFIRM == 1) */
#endif  /* BEACON_SUPPORT */

#if (MAC_INDIRECT_DATA_FFD == 1)
void mac_start_persistence_timer(void);

#endif /* (MAC_INDIRECT_DATA_FFD == 1) */

#if (MAC_SYNC_REQUEST == 1)
void mac_start_missed_beacon_timer(void);

#endif /* (MAC_SYNC_REQUEST == 1) */

#if (MAC_ASSOCIATION_REQUEST_CONFIRM == 1)
void mac_t_assocresponsetime_cb(void *callback_parameter);

#endif /* (MAC_ASSOCIATION_REQUEST_CONFIRM == 1) */

#if ((MAC_ORPHAN_INDICATION_RESPONSE == 1) || (MAC_START_REQUEST_CONFIRM == 1))
bool mac_tx_coord_realignment_command(frame_msgtype_t cmd_type,
		buffer_t *buf_ptr,
		uint16_t new_panid,
		uint8_t new_channel,
		uint8_t new_page);

#endif /* ((MAC_ORPHAN_INDICATION_RESPONSE == 1) || (MAC_START_REQUEST_CONFIRM
        *== 1)) */

#ifdef BEACON_SUPPORT
#if (MAC_START_REQUEST_CONFIRM == 1)
void mac_tx_pending_bc_data(void);

#endif  /* (MAC_START_REQUEST_CONFIRM == 1) */
#endif  /* BEACON_SUPPORT */

void mac_trx_init_sleep(void);
void mac_trx_wakeup(void);

/* Timer callbacks */
#if (MAC_INDIRECT_DATA_BASIC == 1)
void mac_t_poll_wait_time_cb(void *callback_parameter);

#endif /* (MAC_INDIRECT_DATA_BASIC == 1) */
#if (MAC_ASSOCIATION_REQUEST_CONFIRM == 1)
void mac_t_response_wait_cb(void *callback_parameter);

#endif /* (MAC_ASSOCIATION_REQUEST_CONFIRM == 1) */
#if (MAC_SYNC_REQUEST == 1)
void mac_t_start_inactive_device_cb(void *callback_parameter);
void mac_t_tracking_beacons_cb(void *callback_parameter);

#endif /* (MAC_SYNC_REQUEST == 1) */

void dispatch_event(uint8_t *event);

retval_t set_tal_pib_internal(uint8_t attribute, pib_value_t *attribute_value);

#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
retval_t mac_build_aux_sec_header(uint8_t **frame_ptr, mcps_data_req_t *pmdr,
		uint8_t *frame_len);
retval_t mac_secure(frame_info_t *frame, uint8_t *mac_payload_ptr,
		mcps_data_req_t *pmdr);
retval_t mac_unsecure(parse_t *mac_parse_data_buf, uint8_t *mpdu,
		uint8_t *mac_payload, uint8_t *payload_index);

#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */

#ifdef GTS_SUPPORT
void mac_gen_mlme_gts_conf(buffer_t *buf_ptr, uint8_t status,
		gts_char_t gts_char);
void mac_process_gts_request(buffer_t *gts_req);
uint8_t mac_add_gts_info(uint8_t *frame_ptr);
void mac_gts_table_update(void);
void mac_parse_bcn_gts_info(uint8_t gts_count, uint8_t gts_dir,
		mac_gts_list_t *gts_list_ptr);
void handle_gts_data_req(mcps_data_req_t *data_req, uint8_t *msg);
void reset_gts_globals(void);
void mac_t_gts_cb(void *callback_parameter);
void init_gts_queues(void);
void mac_tx_gts_data(queue_t *gts_data);
void handle_gts_data_tx_end(void);
void flush_gts_queues(void);
void reset_gts_expiry(mac_pan_gts_mgmt_t *mac_pan_gts_entry);
void handle_gts_sync_loss(void);

#endif /* GTS_SUPPORT */
#if (MAC_INDIRECT_DATA_FFD == 1)

/*
 * @brief Checks whether the Persistence timer needs to be started
 *
 * If an FFD does have pending data, the MAC persistence timer
 * needs to be started.
 */
static inline void mac_check_persistence_timer(void)
{
	if (!pal_is_timer_running(T_Data_Persistence)) {
		mac_start_persistence_timer();
	}
}

#endif /* (MAC_INDIRECT_DATA_FFD == 1) */

/**
 * This macro sets the MAC to busy
 */
#if (_DEBUG_ > 0)
#define MAKE_MAC_BUSY()             do { \
		if (mac_busy) \
		{ \
			Assert("MAC is already busy" == 0); \
		} \
		mac_busy = true; \
} \
	while (0)
#else
#define MAKE_MAC_BUSY()             do { \
		mac_busy = true; \
} \
	while (0)
#endif

/**
 * This macro sets the MAC to not busy
 */
#if (_DEBUG_ > 0)
#define MAKE_MAC_NOT_BUSY()         do { \
		if (!mac_busy) \
		{ \
			Assert("MAC was not busy" == 0); \
		} \
		mac_busy = false; \
} \
	while (0)
#else
#define MAKE_MAC_NOT_BUSY()         do { \
		mac_busy = false; \
} \
	while (0)
#endif

/*@}*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MAC_INTERNAL_H */
/* EOF */
