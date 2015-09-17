/**
 * @file zid.h
 *
 * @brief API for ZID profile
 *
 *
 * @author    Atmel Corporation: http://www.atmel.com
 * @author    Support: http://www.atmel.com/design-support/
 */

/*
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

#if (((defined ZID_PROFILE) && (defined ZID_DEVICE)) || (defined DOXYGEN))

/* Prevent double inclusion */
#ifndef ZID_DEVICE_H
#define ZID_DEVICE_H

/* === Includes ============================================================= */

#include "rf4ce.h"
#include "zid.h"

/* === Macros =============================================================== */

/**
 * Adds two time values
 */
#define ADD_TIME(a, b)                  ((a) + (b))

/**
 * Subtracts two time values
 */
#define SUB_TIME(a, b)                  ((a) - (b))

/* TxOptions during configuration phase */

/*
 * #define CONF_TX_OPTIONS   (TXO_UNICAST | TXO_DST_ADDR_NET | \
 *                   TXO_ACK_REQ | TXO_SEC_NOT_REQ | TXO_MULTI_CH | \
 *                   TXO_CH_NOT_SPEC | TXO_VEND_NOT_SPEC)
 */
#define CONF_TX_OPTIONS   (TXO_UNICAST | TXO_DST_ADDR_NET | \
	TXO_ACK_REQ | TXO_SEC_REQ | TXO_MULTI_CH | \
	TXO_CH_NOT_SPEC | TXO_VEND_NOT_SPEC)

/* === Types ================================================================ */

typedef enum zid_state_tag {
	ZID_STATE_IDLE,
	ZID_SENDING_HEARTBEAT,
	ZID_REPORT_TRANSMITTING,
	ZID_HANDSHARE_RESPONSE_TRANSMITTING,
	ZID_GET_IDLE_RESPONSE_TRANSMITTING,
	ZID_WAITING_FOR_GET_ATTR_RESPONSE
} SHORTENUM zid_state_t;

typedef enum zid_connect_state_tag {
	ZID_CONNECT_IDLE,
	ZID_PB_PAIRING,
	ZID_SENDING_GET_ATTRIBUTES,
	ZID_SENDING_PUSH_ATTRIBUTES,
	ZID_SENDING_CONFIG_COMPLETE,
	ZID_WAITING_FOR_HS_FOR_CONFIG_COMPLETE
	/* ZID_WAITING_FOR_RX_WINDOW_END */
} SHORTENUM zid_connect_state_t;

/* === Externals ============================================================ */

extern zid_ib_t zid_ib;
extern zid_connect_state_t zid_connect_state;
extern zid_state_t zid_state;
extern uint8_t connecting_pair_ref;
extern bool rx_on;

/* === Prototypes =========================================================== */

void connect_data_confirm(nwk_enum_t Status, uint8_t PairingRef);
void connect_data_indication(gdp_msg_struct_t *gdp_msg, uint8_t RxLinkQuality,
		uint8_t RxFlags);

bool send_attributes(uint8_t PairingRef, uint8_t num_attributes,
		uint8_t *attribute_list);
bool send_get_attribute_response(uint8_t num_attributes, uint8_t *list);

/* void zid_configuration_handshake_response_timeout_cb(void
 * *callback_parameter); */
/* void zid_heartbeat_interval_cb(void *callback_parameter); */
/* void zid_heartbeat_rx_window_timeout_cb(void *callback_parameter); */
/* void zid_heartbeat_indication(nwk_enum_t Status, uint8_t PairingRef); */
/* void zid_config_done_timeout_cb(void *callback_parameter); */
/* void connection_failed(nwk_enum_t Status, uint8_t PairingRef); */

/*
 * bool send_nonstandard_descriptors(void);
 * bool send_touch_attributes(void);
 * bool send_config_complete(void);
 */

void get_ib_from_nvm(void);
void store_ib_to_nvm(void);
void waiting_for_get_attribute_response_timeout_cb(void *callback_parameter);

#endif /* ZID_DEVICE_H */

#endif  /* #if (((defined ZID_PROFILE) && (defined ZID_DEVICE)) || (defined
         * DOXYGEN)) */

/* EOF */
