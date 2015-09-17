#ifndef OTA_UPGRADE_H
#define OTA_UPGRADE_H

#include "ota.h"
#include "ota_mgr.h"
#include "compiler.h"

typedef enum {
	OTA_SERVER_DISCOVERY = 0,
	OTA_SERVER_NOTIFY,
	OTA_QUERY_IMAGE,
	OTA_IMAGE_NOTIFY,
	OTA_IMAGE_REQUEST,
	OTA_IMAGE_RESPONSE,
	OTA_SWITCH_REQUEST,
	OTA_SWITCH_RESPONSE
}ota_upgrade_msg_code_t;

COMPILER_PACK_SET (1)
typedef struct {
	uint8_t msg_id;
	uint8_t image_type;
	uint32_t image_start;
	uint32_t image_size;
	uint8_t image_crc;
	uint16_t req_interval;
	uint8_t firmware[10];
	uint8_t firmware_version[5];
	uint8_t board[10];
	uint8_t board_version[5];
}image_notify_request_t;

typedef struct {
	uint8_t msg_id;
	uint8_t image_resp_type;
	uint32_t block_start;
	uint16_t block_size;
	uint8_t block[100];
}image_response_t;

typedef struct {
	uint8_t msg_id;
	uint32_t switch_time;
}switch_image_response_t;

typedef struct {
	uint32_t image_start;
	uint32_t image_size;
	uint8_t firmware[10];
	uint8_t firmware_version[5];
	uint8_t board[10];
	uint8_t board_version[5];
}current_image_t;

typedef struct {
	uint8_t msg_id;
	uint32_t image_start;
	uint32_t image_size;
	uint8_t firmware[10];
	uint8_t firmware_version[5];
	uint8_t board[10];
	uint8_t board_version[5];
}query_image_request_t;

typedef struct {
	uint8_t msg_id;
	uint8_t req_type;
	uint32_t addr;
	uint32_t size;
}image_request_t;
COMPILER_PACK_RESET ()

void handle_upgrade_ota_msg(uint16_t len, uint8_t *msg);
void ota_upgrade_timer_handler(void);
void ota_upgrade_init(void);
void ota_upgrade_sent_frame(uint8_t addr_mode, uint8_t *addr, uint8_t status);
void ota_upgrade_rcvd_frame(uint8_t addr_mode, uint8_t *src_addr, uint8_t length, const uint8_t *payload, uint8_t lqi);
#endif /* OTA_UPGRADE_H */
