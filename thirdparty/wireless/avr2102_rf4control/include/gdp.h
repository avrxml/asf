/**
 * @file gdp.h
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

#if ((defined GDP_PROFILE) || (defined DOXYGEN))

/* Prevent double inclusion */
#ifndef GDP_H
#define GDP_H

/* === Includes ============================================================= */

#include "rf4ce.h"
#include "nwk_internal.h"
#include "gdp_config.h"

/* === Macros =============================================================== */

#define GDP_COMMAND_FRAME_BIT 0x40

#define GDP_PENDING_DATA_BIT 0x80

#define GDP_CONF_TX_OPTIONS  (TXO_UNICAST | TXO_DST_ADDR_IEEE |	\
	TXO_ACK_REQ | TXO_SEC_REQ | TXO_MULTI_CH | \
	TXO_CH_NOT_SPEC | TXO_VEND_NOT_SPEC)

#ifdef ZID_PROFILE
#define PROFILE_ID PROFILE_ID_ZID
#endif

/** The maximum time a device shall wait for a response command frame following
 *  a request command frame. */
#define aplcMaxResponseWaitTime     200 /* ms as per GDP*/
#define aplcMaxResponseWaitTime_us  (aplcMaxResponseWaitTime * 1000UL)

#define aplcMaxRxOnWaitTime         100 /* ms as per GDP  */
#define aplcMaxRxOnWaitTime_us      (aplcMaxRxOnWaitTime * 1000UL)
/* === Types ================================================================ */

/**
 * ZID command codes
 *
 * @ingroup apiRF4CONTROL_ZID_CONSTANTS
 */
typedef enum gdp_cmd_code_tag {
	GENERIC_RESPONSE          = 0x00,
	CONFIGURATION_COMPLETE,
	HEART_BEAT,
	GET_ATTRIBUTES,
	GET_ATTRIBUTES_RESPONSE,
	PUSH_ATTRIBUTES
} SHORTENUM gdp_cmd_code_t;

/**
 * Handshake response code
 *
 * @ingroup apiRF4CONTROL_ZID_CONSTANTS
 */
typedef enum gdp_response_code_tag {
	GDP_SUCCESSFUL     = 0x00,
	GDP_UNSUPPORTED_REQUEST,
	GDP_INVALID_PARAMETER,
	GDP_CONFIGURATION_FAILURE
#ifdef ZID_PROFILE
	,
	ZID_INVALID_REPORT_ID = 0x40,
	ZID_MISSING_FRAGMENT   = 0x41
#endif
} SHORTENUM gdp_response_code_t;

typedef enum gdp_attribute_status_tag {
	GDP_ATTRIBUTE_SUCCESS     = 0x00,
	GDP_UNSUPPORTED_ATTRIBUTE,
	GDP_ILLEGAL_REQUEST
} SHORTENUM gdp_attribute_status_t;

typedef enum gdp_state_tag {
	GDP_STATE_IDLE,
	GDP_SENDING_HEARTBEAT,
	GDP_SENDING_GET_ATTRIBUTES_RESPONSE,
	GDP_SENDING_GENERIC_RESPONSE,
	GDP_WAITING_FOR_ATTRIBUTE_RESPONSE,
	GDP_WAITING_FOR_GENERIC_RESPONSE
} SHORTENUM gdp_state_t;

typedef struct gdp_msg_struct_tag {
	gdp_cmd_code_t msg_code;
	uint8_t PairingRef;
	bool pendingData;
	void *msg_ptr;
} gdp_msg_struct_t;

typedef struct gdp_get_attr_req_ind_tag {
	uint8_t num_of_attr;
	uint8_t *attr_list;
} gdp_get_attr_req_ind_t;

typedef struct gdp_get_attrrsp_req_ind_tag {
	uint8_t payload_length;
	uint8_t *payload;
} gdp_get_attrrsp_req_ind_t;

typedef struct gdp_push_attr_req_ind_tag {
	uint8_t payload_length;
	uint8_t *payload;
} gdp_push_attr_req_ind_t;

typedef struct gdp_generic_response_req_ind_tag {
	gdp_response_code_t Status;
} gdp_generic_response_req_ind_t;

typedef struct gdp_conf_complete_req_ind_tag {
	gdp_response_code_t Status;
} gdp_conf_complete_req_ind_t;

#if (defined RF4CE_CALLBACK_PARAM) || (defined DOXYGEN)

typedef void (*gdp_heartbeat_confirm_cb_t)(nwk_enum_t Status,
		uint8_t PairingRef);

/**
 * struct for zrc indication callback.
 * App should use this struct to register indication callback functions
 *
 * @ingroup apiRF4Control_ZRC_API
 */

#endif /* (defined RF4CE_CALLBACK_PARAM) || (defined DOXYGEN) */

/* === Externals ============================================================ */

/* === Prototypes =========================================================== */

void gdp_init(void);

bool gdp_request(gdp_msg_struct_t *gdp_msg);
void gdp_indication(gdp_msg_struct_t *gdp_msg, uint8_t rxlinkquality,
		uint8_t rxflags);
void gdp_confirm(uint8_t PairingRef, nwk_enum_t status);

#if 0
void gdp_get_attribute_ind(uint8_t PairingRef, uint8_t num_attr,
		uint8_t *attr_list);
void gdp_get_attributes_response_ind(nwk_enum_t nwk_status, uint8_t PairingRef,
		uint8_t *nsdu);
void gdp_get_attributes_response_confirm(nwk_enum_t nwk_status,
		uint8_t PairingRef);
bool gdp_heartbeat_request(uint8_t PairingRef);

/* void gdp_get_attribute(uint8_t attribute_id,uint8_t *length, uint8_t *value);
 **/
void gdp_heartbeat_confirm(nwk_enum_t Status, uint8_t PairingRef);
void gdp_heartbeat_indication(uint8_t PairingRef);
void gdp_generic_response_indication(nwk_enum_t Status, uint8_t PairingRef,
		uint8_t response, bool pending_data);
void gdp_generic_response_confirm(nwk_enum_t Status, uint8_t PairingRef);

#if (GDP_CONF_COMPLETE_IND == 1)
void gdp_conf_complete_indication(uint8_t PairingRef, uint8_t status);

#endif

void gdp_push_attributes_indication(uint8_t PairingRef, uint8_t nsduLength,
		uint8_t *nsdu);

#endif

#ifndef RF4CE_CALLBACK_PARAM

void gdp_data_indication(uint8_t PairingRef, uint8_t nsduLength, uint8_t *nsdu,
		uint8_t RxLinkQuality, uint8_t RxFlags);

void gdp_data_confirm(nwk_enum_t Status, uint8_t PairingRef);

#endif /* #if (!defined RF4CE_CALLBACK_PARAM) */

/**
 * @brief Registering zrc indication callback
 *
 * The application needs to register the zrc indication call back functions
 * so that the corresponding indication function will get called
 * by the profile on receiving the indication from the network layer.
 *
 * @param zrc_indication_callback_t    indication callback struct
 *
 * @see zrc_indication_callback_t
 * @ingroup apiRF4Control_ZRC_API
 */
#endif /* GDP_H */

#endif  /* #if ((defined GDP_PROFILE) || (defined DOXYGEN)) */

/* EOF */
