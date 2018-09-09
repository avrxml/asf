/**
* \file otau_notify.h
*
* \brief OTAU Notify Interface
*
* Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries.
*
* \asf_license_start
*
* \page License
*
* Subject to your compliance with these terms, you may use Microchip
* software and any derivatives exclusively with Microchip products.
* It is your responsibility to comply with third party license terms applicable
* to your use of third party software (including open source software) that
* may accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
* INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
* AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
* LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
* LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
* SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
* POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
* ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
* RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*
* \asf_license_stop
*
*/
/*
* Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
*/

#ifndef OTAU_NOTIFY_H
#define OTAU_NOTIFY_H

#include "otau.h"
#include "sysTimer.h"

#define MIN_CLIENT_NOTIFY_INTERVAL_SEC   (5)

#define CLIENT_NOTIFY_INTERVAL_SEC       (10)

#define CLIENT_NOTIFY_JITTER_PERCENT     (0)

#define CLIENT_NOTIFY_RETRY_COUNT        (3)

#define LED_TOGGLE_COUNT                 (20)

#define LED_TOGGLE_INTERVAL_MILLISEC     (100)

#define SERVER_DISCOVERY_INTERVAL_SEC    (10)


typedef enum {
	OTA_CLIENT_DISCOVERY = 0,
	OTA_CLIENT_NOTIFY,
	OTA_CLIENT_INFO_REQ,
	OTA_CLIENT_INFO,
	OTA_IDENTIFY_REQ,
	OTA_RESET_REQ,
	OTA_ERROR_MSG = 0xF0
}otauNotifyMsgCode_t;

COMPILER_PACK_SET(1)
typedef struct {
	uint8_t domainId;
	uint8_t msgId;
	uint16_t interval;
	uint8_t jitter_percent;
	uint8_t retry_count;
}otauClientDiscoveryReq_t;

typedef struct {
	uint8_t domainId;
	uint8_t msgId;
	uint8_t supported_modes;
	uint8_t arch_type;
	addr_mode_t ieee_addr_mode;
	uint64_t ieee_addr;
	uint16_t pan_id;
	addr_mode_t parent_addr_mode;
	uint16_t parent_short_addr;
}otauClientNotifyIndication_t;

typedef struct {
	uint8_t domainId;
	uint8_t msgId;
	uint8_t device_type[10];
	uint8_t firmware[10];
	uint8_t firmware_version[5];
	uint8_t board[10];
	uint8_t board_version[5];
}otauClientInfoIndication_t;

typedef struct {
	uint8_t domainId;
	uint8_t msgId;
}otauClientInfoRequest_t;

typedef struct {
	uint8_t domainId;
	uint8_t msgId;
}otauClientIdentifyRequest_t;

typedef struct {
	uint8_t domainId;
	uint8_t msgId;
}otauClientResetRequest_t;
COMPILER_PACK_RESET()

void otauHandleNotifyMsg(otau_domain_msg_t *otau_domain_msg);
void otauNotifyTimerHandler(struct SYS_Timer_t *timer);
void otauNotifyInit(void);
void otauNotifySentFrame(addr_mode_t addr_mode, uint8_t *addr, uint8_t status);
void otauNotifyRcvdFrame(addr_mode_t addr_mode, uint8_t *src_addr, uint16_t length, uint8_t *payload);
#endif /* OTAU_NOTIFY_H */
