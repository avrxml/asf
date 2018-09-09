/**
* \file server_notify.c
*
* \brief Server Notify implementation
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

#if defined(OTAU_ENABLED) && defined(OTAU_SERVER)

#include "stddef.h"
#include "string.h"
#include "stdint.h"
#include "server_notify.h"
#include "otau_notify.h"
#include "otau_parser.h"
#include "miwi_config.h"

otauNotifyState_t curr_notify_otau_state;

otauServerInfoResp_t server_info = {
	.ieee_addr_mode = EXTENDED_ADDR_MODE,
	.short_addr_mode = NATIVE_ADDR_MODE,
	.max_app_payload = APP_MAX_PAYLOAD_SIZE,
	.max_phy_payload = PHY_MAX_PAYLOAD_SIZE,
	.supported_modes = SUPPORTED_MODE,
	.firmware = "MiWi Mesh ",
	.firmware_version = "6.1.0",
	.board = "SAMR21XPro",
	.board_version = "1.0.0"
};

void otauNotifyInit(void)
{
	curr_notify_otau_state = NOTIFY_OTAU_IDLE;
	memcpy(&server_info.ieee_addr, get_node_address(EXTENDED_ADDR_MODE), EXTENDED_ADDR_SIZE);
	server_info.pan_id = MY_PAN_ID;
}

void otauNotifyRcvdFrame(addr_mode_t addr_mode, uint8_t *src_addr, uint16_t length, uint8_t *payload)
{
	uint8_t msg_code = *(payload + 1);
	payload += 2;
	switch (msg_code)
	{
	case OTA_CLIENT_NOTIFY:
	{
		send_server_data(DOMAIN_OTAU_NOTIFY, addr_mode, src_addr, CLIENT_NOTIFY_INDICATION, payload, length - 2);
		break;
	}
	case OTA_CLIENT_INFO:
		send_server_data(DOMAIN_OTAU_NOTIFY, addr_mode, src_addr, CLIENT_INFO_INDICATION, payload, length - 2);
		break;
	}
}

void otauNotifySentFrame(addr_mode_t addr_mode, uint8_t *addr, uint8_t status)
{
	switch (curr_notify_otau_state)
	{
		case CLIENT_DISCOVERY_REQUEST_SENT:
			send_pc_data(DOMAIN_OTAU_NOTIFY, CLIENT_DISCOVERY_CONFIRM, &status, 1);
			break;
		case CLIENT_INFO_REQUEST_SENT:
			send_server_data(DOMAIN_OTAU_NOTIFY, addr_mode, addr, CLIENT_INFO_CONFIRM, &status, 1);
			break;
		case CLIENT_IDENTIFY_REQUEST_SENT:
			send_server_data(DOMAIN_OTAU_NOTIFY, addr_mode, addr, CLIENT_IDENTIFY_CONFIRM, &status, 1);
			break;
		case CLIENT_RESET_REQUEST_SENT:
			send_server_data(DOMAIN_OTAU_NOTIFY, addr_mode, addr, CLIENT_RESET_CONFIRM, &status, 1);
			break;
		default:
			break;
			/*  */
	}
	curr_notify_otau_state = NOTIFY_OTAU_IDLE;
}


/**
 * \brief Parses the Received Data in the Buffer and Process the Commands
 *accordingly.
 */
void otauHandleNotifyMsg(otau_domain_msg_t *otau_domain_msg)
{
	uint8_t *msg = &(otau_domain_msg->domain_msg);
	/* *msg is the mode specification for phy/app */
	uint8_t msgId = *(uint8_t *)(msg + 1);
	msg += 2;

	uint8_t addr_mode = *msg;
	uint8_t *addr = msg + 1;

	if (!(msgId & 0x80))
	{
		if(NATIVE_ADDR_MODE == addr_mode)
		{
			msg += (1 + NATIVE_ADDR_SIZE);
		}
		else if(EXTENDED_ADDR_MODE == addr_mode)
		{
			msg += (1 + EXTENDED_ADDR_SIZE);
		}
		else
		{
			addr = NULL;
			msg++;
		}
	}
	switch (msgId)
	{ /* message type */
		case SERVER_INFO_REQUEST:
		{
			memcpy(&server_info.short_addr, get_node_address(NATIVE_ADDR_MODE), NATIVE_ADDR_SIZE);
			send_pc_data(DOMAIN_OTAU_NOTIFY, SERVER_INFO_RESPONSE, (uint8_t *)&server_info, sizeof(otauServerInfoResp_t));
			break;
		}
		case CLIENT_DISCOVERY_REQUEST:
		{
			otauClientDiscoveryReq_t clientDiscReq;
			clientDiscReq.msgId = OTA_CLIENT_DISCOVERY;
			memcpy((uint8_t *)&clientDiscReq.interval, msg, sizeof(clientDiscReq) - 2);
			curr_notify_otau_state = CLIENT_DISCOVERY_REQUEST_SENT;
			clientDiscReq.domainId = DOMAIN_OTAU_NOTIFY;
			otauDataSend(addr_mode, addr, &clientDiscReq, sizeof(otauClientDiscoveryReq_t));
			break;
		}
		case CLIENT_INFO_REQUEST:
		{
			otauClientInfoRequest_t clientInfoReq;
			clientInfoReq.domainId = DOMAIN_OTAU_NOTIFY;
			clientInfoReq.msgId = OTA_CLIENT_INFO_REQ;
			curr_notify_otau_state = CLIENT_INFO_REQUEST_SENT;
			otauDataSend(addr_mode, addr, &clientInfoReq, sizeof(otauClientInfoRequest_t));
			break;
		}
		case CLIENT_IDENTIFY_REQUEST:
		{
			otauClientIdentifyRequest_t clientIdentifyReq;
			clientIdentifyReq.domainId = DOMAIN_OTAU_NOTIFY;
			clientIdentifyReq.msgId = OTA_IDENTIFY_REQ;
			curr_notify_otau_state = CLIENT_IDENTIFY_REQUEST_SENT;
			otauDataSend(addr_mode, addr, &clientIdentifyReq, sizeof(otauClientIdentifyRequest_t));
			break;
		}
		case CLIENT_RESET_REQUEST:
		{
			otauClientResetRequest_t clientResetReq;
			clientResetReq.domainId = DOMAIN_OTAU_NOTIFY;
			clientResetReq.msgId = OTA_RESET_REQ;
			curr_notify_otau_state = CLIENT_RESET_REQUEST_SENT;
			otauDataSend(addr_mode, addr, &clientResetReq, sizeof(otauClientResetRequest_t));
			break;
		}
		default:
		{
			/* Do nothing...*/
		}
	}
}

void otauNotifyTimerHandler(struct SYS_Timer_t *timer)
{

}

#endif //#if defined(OTAU_ENABLED) && defined(OTAU_SERVER)