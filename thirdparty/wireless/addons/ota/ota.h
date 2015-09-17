

#ifndef OTA_H
#define OTA_H

#include "compiler.h"

#define MODE_MASK     0x03

#define PHY_MODE_SUPPORTED_BIT 0x02
#define APP_MODE_SUPPORTED_BIT 0x01

#define SUPPORTED_MODE (PHY_MODE_SUPPORTED_BIT | APP_MODE_SUPPORTED_BIT)

#define LOG_MASK					(0x0F)

#define APP_MODE             ()

typedef enum {
	COMMON = 0,
	UPGRADE
}module_code_t;

typedef enum {
	SUCCESS = 0x00,
	CCA_FAILURE,
	NO_ACK,
	ERROR,
	INVALID_MSG = 0x80,
	MSG_IN_PROGRESS
}error_status_t;

typedef enum {
	TIMER_MODE_SINGLE,
	TIMER_MODE_PERIODIC,
} ota_timer_mode_t;

typedef enum {
	BROADCAST_MODE = 0,
	NATIVE_ADDR_MODE,
	EXTENDED_ADDR_MODE,
}addr_mode_t;

typedef enum {
	DOMAIN_OTA_COMMON = 0x80,
	DOMAIN_OTA_UPGRADE
}ota_domain_t;

typedef enum {
	OTA_LED_ON,
	OTA_LED_OFF,
	OTA_LED_TOGGLE
}ota_led_t;

typedef enum {
	MEMORY_ABSOLUTE_ADDRESS,
	MEMORY_OFFSET_ADDRESS,
	MEMORY_NODE_INFO_ADDRESS
}address_type_t;

void ota_init(void);
void ota_task(void);
void handle_ota_msg(uint16_t length, uint8_t *msg);
void reverse_memcpy(uint8_t *dst, uint8_t *src, uint8_t len);

#endif /* OTA_H */

