/**
 * @file zid_adaptor.h
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

#if (((defined ZID_PROFILE) && (defined ZID_ADAPTOR)) || (defined DOXYGEN))

/* Prevent double inclusion */
#ifndef ZID_ADAPTOR_H
#define ZID_ADAPTOR_H

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
 * #define CONF_TX_OPTIONS   (TXO_UNICAST | TXO_DST_ADDR_IEEE | \
 *                   TXO_ACK_REQ | TXO_SEC_NOT_REQ | TXO_MULTI_CH | \
 *                   TXO_CH_NOT_SPEC | TXO_VEND_NOT_SPEC)
 */
#define CONF_TX_OPTIONS   (TXO_UNICAST | TXO_DST_ADDR_IEEE | \
	TXO_ACK_REQ | TXO_SEC_REQ | TXO_MULTI_CH | \
	TXO_CH_NOT_SPEC | TXO_VEND_NOT_SPEC)

/* === Types ================================================================ */

typedef enum zid_connect_state_tag {
	ZID_CONNECT_IDLE,
	ZID_PB_PAIRING,
	ZID_CONFIGURING,
	ZID_SENDING_HS_FOR_CONFIG_COMPLETE
} SHORTENUM zid_connect_state_t;

typedef enum zid_state_tag {
	ZID_STATE_IDLE,
	ZID_REPORT_TRANSMITTING,
	ZID_WAITING_FOR_REPORT_DATA,
	ZID_WAITING_FOR_ATTRIBUTE_RESPONSE,
	ZID_SET_REPORT_TRANSMITTING
} SHORTENUM zid_state_t;

/* === Externals ============================================================ */

extern zid_ib_t zid_ib;
extern zid_connect_state_t zid_connect_state;
extern uint8_t connecting_pair_ref;
extern zid_standby_t standby_state;
extern zid_state_t zid_state;
/* extern bool get_request; */

/* === Prototypes =========================================================== */

void connect_data_confirm(nwk_enum_t Status, uint8_t PairingRef);
void connect_data_indication(gdp_msg_struct_t *gdpmsg, uint8_t RxLinkQuality,
		uint8_t RxFlags);
bool send_get_attributes_request(uint8_t PairingRef,
		zid_attribute_t ZIDAttribute);

gdp_response_code_t handle_incoming_push_attribute(gdp_msg_struct_t *gdpmsg);

/* bool send_attributes(uint8_t PairingRef, uint8_t num_attributes, uint8_t
 * *attribute_list, zid_cmd_code_t cmd_code); */

/* void set_attribute_value(uint8_t PairingRef, zid_attribute_t attribute_id,
 * uint8_t *value); */
bool set_attribute_value(uint8_t PairingRef, zid_attribute_t attribute_id,
		uint8_t *value);

/*
 * bool config_completed(uint8_t PairingRef);
 * void connection_failed(nwk_enum_t Status, uint8_t PairingRef);
 */
/* void zid_config_timeout_cb(void *callback_parameter); */
void connect_data_confirm(nwk_enum_t Status, uint8_t PairingRef);

/* void connect_data_indication(uint8_t PairingRef, uint8_t nsduLength, uint8_t
 * *nsdu, */
/*                         uint8_t RxLinkQuality, uint8_t RxFlags); */

bool send_get_attribute_response(gdp_msg_struct_t *gdp_msg);

/* bool check_required_attributes(uint8_t nsduLength, uint8_t *nsdu); */
/* bool zid_get_idle_request(uint8_t PairingRef, uint8_t ReportId); */

/* void zid_get_idle_confirm(nwk_enum_t Status, uint8_t PairingRef, uint8_t
 * ReportId, uint8_t IdleRate); */

void clear_proxy_entry(uint8_t PairingRef);

void get_ib_from_nvm(void);
void store_ib_to_nvm(void);

void waiting_for_get_attribute_response_timeout_cb(void *callback_parameter);

#endif /* ZID_ADAPTOR_H */

#endif  /* #if (((defined ZID_PROFILE) && (defined ZID_ADAPTOR)) || (defined
         * DOXYGEN)) */

/* EOF */
