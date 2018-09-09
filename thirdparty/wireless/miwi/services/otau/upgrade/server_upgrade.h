/**
* \file server_upgrade.h
*
* \brief Server Upgrade interface
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

#ifndef SERVER_UPGRADE_H
#define SERVER_UPGRADE_H

#include "otau.h"

typedef enum {
	SERVER_DISCOVERY_REQUEST = 0x00,
	SERVER_NOTIFY_INDICATION,
	SERVER_NOTIFY_CONFIRM,
	QUERY_IMAGE_REQUEST,
	IMAGE_NOTIFY_REQUEST,
	IMAGE_NOTIFY_CONFIRM,
	IMAGE_REQUEST,
	IMAGE_RESPONSE,
	IMAGE_CONFIRM,
	SWITCH_IMAGE_REQUEST,
	SWITCH_IMAGE_RESPONSE,
	SWITCH_IMAGE_CONFIRM,
	UPGRADE_ERROR_MSG = 0xF0
}upgrade_msg_code_t;

typedef enum {
	UPGRADE_OTAU_IDLE,
	SERVER_NOTIFY_SENT,
	IMAGE_NOTIFY_SENT,
	IMAGE_RESPONSE_SENT,
	SWITCH_RESPONSE_SENT,
}upgrade_otau_state_t;

COMPILER_PACK_SET(1)
typedef struct {
	uint8_t domainId;
	uint8_t msgId;
	uint64_t ieee_addr;
	uint16_t short_addr;
}server_notify_indication_t;
COMPILER_PACK_RESET()

#endif /* SERVER_UPGRADE_H */
