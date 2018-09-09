/**
* \file server_notify.h
*
* \brief Server Notify interface
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

#ifndef SERVER_NOTIFY_H
#define SERVER_NOTIFY_H

#include "otau.h"

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
	NOTIFY_ERROR_MSG = 0xF0
}otauServerNotifyMsgCode_t;

typedef enum {
	NOTIFY_OTAU_IDLE,
	CLIENT_DISCOVERY_REQUEST_SENT,
	CLIENT_INFO_REQUEST_SENT,
	CLIENT_IDENTIFY_REQUEST_SENT,
	CLIENT_RESET_REQUEST_SENT
}otauNotifyState_t;

COMPILER_PACK_SET(1)
typedef struct {
	addr_mode_t ieee_addr_mode;
	uint64_t ieee_addr;
	uint16_t pan_id;
	addr_mode_t short_addr_mode;
	uint16_t short_addr;
	uint8_t firmware[10];
	uint8_t firmware_version[5];
	uint8_t board[10];
	uint8_t board_version[5];
	uint16_t max_app_payload;
	uint16_t max_phy_payload;
	uint8_t supported_modes;
}otauServerInfoResp_t;

COMPILER_PACK_RESET()

#endif /* SERVER_NOTIFY_H */
