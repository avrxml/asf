/**
* \file server_upgrade.c
*
* \brief Server Upgrade implementation
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
#include "server_upgrade.h"
#include "otau_upgrade.h"
#include "otau_parser.h"

upgrade_otau_state_t curr_upgrade_otau_state;

void otauUpgradeInit(void)
{

}

void otauUpgradeRcvdFrame(addr_mode_t addr_mode, uint8_t *src_addr, uint16_t length, uint8_t *payload)
{
	uint8_t msg_code = *(payload + 1);
	payload += 2;
	switch (msg_code)
	{
		case OTA_SERVER_DISCOVERY:
		{
			send_server_data(DOMAIN_OTAU_UPGRADE, addr_mode, src_addr, SERVER_DISCOVERY_REQUEST, payload, length - 2);
			break;
		}
		case OTA_QUERY_IMAGE:
		{
			send_server_data(DOMAIN_OTAU_UPGRADE, addr_mode, src_addr, QUERY_IMAGE_REQUEST, payload, length - 2);
			break;
		}
		case OTA_IMAGE_REQUEST:
		{
			send_server_data(DOMAIN_OTAU_UPGRADE, addr_mode, src_addr, IMAGE_REQUEST, payload, length - 2);
			break;
		}
		case OTA_SWITCH_REQUEST:
		{
			send_server_data(DOMAIN_OTAU_UPGRADE, addr_mode, src_addr, SWITCH_IMAGE_REQUEST, payload + 1, length - 2);
			break;
		}
		break;
	}
}

void otauUpgradeSentFrame(addr_mode_t addr_mode, uint8_t *addr, uint8_t status)
{
	switch (curr_upgrade_otau_state)
	{
		case SERVER_NOTIFY_SENT:
		send_server_data(DOMAIN_OTAU_UPGRADE, addr_mode, addr, SERVER_NOTIFY_CONFIRM, &status, 1);
		break;
		case IMAGE_NOTIFY_SENT:
		send_server_data(DOMAIN_OTAU_UPGRADE, addr_mode, addr, IMAGE_NOTIFY_CONFIRM, &status, 1);
		break;
		case IMAGE_RESPONSE_SENT:
		send_server_data(DOMAIN_OTAU_UPGRADE, addr_mode, addr, IMAGE_CONFIRM, &status, 1);
		break;
		case SWITCH_RESPONSE_SENT:
		send_server_data(DOMAIN_OTAU_UPGRADE, addr_mode, addr, SWITCH_IMAGE_CONFIRM, &status, 1);
		break;
		default:
		break;
		/*  */
	}
	curr_upgrade_otau_state = UPGRADE_OTAU_IDLE;
}



/**
 * \brief Parses the Received Data in the Buffer and Process the Commands
 *accordingly.
 */
void handle_upgrade_otau_msg(otau_domain_msg_t *otau_domain_msg)
{
	uint8_t *msg = &(otau_domain_msg->domain_msg);
	/* *msg is the mode specification for phy/app */
	uint8_t msgId = *(uint8_t *)(msg + 1);
	uint8_t temp;
	msg += 2;

	uint8_t addr_mode = *msg;
	uint8_t *addr = msg + 1;
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
		msg++;
	}

	switch (msgId)
	{ /* message type */
		case SERVER_NOTIFY_INDICATION:
		{
			server_notify_indication_t server_notify;
			server_notify.msgId = OTA_SERVER_NOTIFY;
			memcpy(&server_notify.ieee_addr, get_node_address(EXTENDED_ADDR_MODE), EXTENDED_ADDR_SIZE);
			memcpy(&server_notify.short_addr, get_node_address(NATIVE_ADDR_MODE), NATIVE_ADDR_SIZE);
			curr_upgrade_otau_state = SERVER_NOTIFY_SENT;
			server_notify.domainId = DOMAIN_OTAU_UPGRADE;
			otauDataSend(addr_mode, addr, &server_notify, sizeof(server_notify_indication_t));
			break;
		}
		case IMAGE_NOTIFY_REQUEST:
		{
			otauImageNotifyRequest_t imageNotify;
			imageNotify.msgId = OTA_IMAGE_NOTIFY;
			memcpy(&imageNotify.imageType, msg, sizeof(otauImageNotifyRequest_t) - 2);
			curr_upgrade_otau_state = IMAGE_NOTIFY_SENT;
			imageNotify.domainId = DOMAIN_OTAU_UPGRADE;
			otauDataSend(addr_mode, addr, &imageNotify, sizeof(otauImageNotifyRequest_t));
			break;
		}
		case IMAGE_RESPONSE:
		{
			otauImageResponse_t image_response;
			temp = sizeof(image_response.imageRespType)
					+ sizeof(image_response.blockAddr) + sizeof(image_response.blockSize);
			image_response.msgId = OTA_IMAGE_RESPONSE;
			memcpy(&image_response.imageRespType, msg, temp);
			msg += temp;
			memcpy(&image_response.block, msg, image_response.blockSize);
			curr_upgrade_otau_state = IMAGE_RESPONSE_SENT;
			image_response.domainId = DOMAIN_OTAU_UPGRADE;
			otauDataSend(addr_mode, addr, &image_response, temp + 2 + image_response.blockSize);
			break;
		}
		case SWITCH_IMAGE_RESPONSE:
		{
			otauSwitchImageResponse_t switch_image_resp;
			switch_image_resp.msgId = OTA_SWITCH_RESPONSE;
			memcpy(&switch_image_resp.switchTime, msg, sizeof(switch_image_resp.switchTime));
			curr_upgrade_otau_state = SWITCH_RESPONSE_SENT;
			switch_image_resp.domainId = DOMAIN_OTAU_UPGRADE;
			otauDataSend(addr_mode, addr, &switch_image_resp, sizeof(otauSwitchImageResponse_t));
			break;
		}
		default:
		{
			/* Do nothing...*/
		}
	}
}


void otauUpgradeTimerHandler(SYS_Timer_t *timer)
{

}

#endif //#if defined(OTAU_ENABLED) && defined(OTAU_SERVER)