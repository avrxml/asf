#ifndef SERVER_COMMON_H
#define SERVER_COMMON_H

#include "ota.h"
#include "ota_mgr.h"
#include "compiler.h"

typedef enum {
	CLIENT_DISCOVERY_REQUEST = 0x00,
	CLIENT_DISCOVERY_CONFIRM,
	CLIENT_NOTIFY_INDICATION,
	CLIENT_INFO_REQUEST,
	CLIENT_INFO_CONFIRM,
	CLIENT_INFO_INDICATION,
	CLIENT_IDENTIFY_REQUEST,
	CLIENT_IDENTIFY_CONFIRM,
	CLIENT_RESET_REQUEST,
	CLIENT_RESET_CONFIRM,
	SERVER_INFO_REQUEST = 0x80,
	SERVER_INFO_RESPONSE,
	COMMON_ERROR_MSG = 0xF0
}common_msg_code_t;

typedef enum {
	COMMON_OTA_IDLE,
	CLIENT_DISCOVERY_REQUEST_SENT,
	CLIENT_INFO_REQUEST_SENT,
	CLIENT_IDENTIFY_REQUEST_SENT,
	CLIENT_RESET_REQUEST_SENT
}common_ota_state_t;

COMPILER_PACK_SET (1)
typedef struct {
	addr_mode_t ieee_addr_mode;
	uint64_t ieee_addr;
	uint16_t pan_id;
	addr_mode_t short_addr_mode;
	uint64_t short_addr;
	uint8_t firmware[10];
	uint8_t firmware_version[5];
	uint8_t board[10];
	uint8_t board_version[5];
	uint16_t max_app_payload;
	uint16_t max_phy_payload;
	uint8_t supported_modes;
}server_info_resp_t;


typedef struct {
	uint8_t msg_id;
	uint16_t interval;
}client_discovery_req_t;
COMPILER_PACK_RESET ()

#endif /* SERVER_COMMON_H */
