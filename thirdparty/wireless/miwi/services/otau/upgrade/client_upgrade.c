/**
* \file client_upgrade.c
*
* \brief Client Upgrade implementation
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

#if defined(OTAU_ENABLED) && !defined(OTAU_SERVER)

#include "compiler.h"
#include "string.h"
#include "client_upgrade.h"
#include "otau_upgrade.h"
#include "common_nvm.h"
#include "delay.h"

static void send_image_req(uint32_t index);
static void send_switch_req(void);
static uint8_t calcCrc(uint8_t *pcBlock, uint8_t length);

uint32_t upgradeImageStart;
uint32_t image_size;
uint32_t image_end;
uint8_t image_crc;
uint16_t block_size;
uint32_t image_index = 0;

uint16_t upgradeImageReqInterval;

upgradeState_t currUpgradeState = STATE_IDLE;
otauUpgradeState_t currOtauUpgradeState = UPGRADE_OTAU_IDLE;

uint16_t block[APP_MAX_PAYLOAD_SIZE / 2];

uint8_t otauUpgradeConfirmWait = 0;
uint8_t imageReqRetry = 0;

void otauUpgradeInit(void)
{
	currUpgradeState = STATE_IDLE;
	currOtauUpgradeState = UPGRADE_OTAU_IDLE;
	otauUpgradeConfirmWait = 0;
	imageReqRetry = 0;
	upgradeImageReqInterval = MIN_IMAGE_REQ_INTERVAL_MILLI_SEC;
}

void otauUpgradeRcvdFrame(addr_mode_t addr_mode, uint8_t *src_addr, uint16_t length, uint8_t *payload)
{
	uint8_t msg_code = *(payload + 1);
	switch (msg_code)
	{
		case OTA_IMAGE_NOTIFY:
		{
			if(STATE_IDLE == currUpgradeState || STATE_WAITING_FOR_SWITCH == currUpgradeState)
			{
				otauImageNotifyRequest_t *image_notify = (otauImageNotifyRequest_t *)payload;
				otauSetServerDetails(src_addr);
				block_size = APP_MAX_PAYLOAD_SIZE;

				memcpy(&upgradeImageStart, &image_notify->imageStart, sizeof(uint32_t));
				memcpy(&image_size, &image_notify->imageSize, sizeof(uint32_t));
				image_crc = image_notify->imageCrc;
				memcpy(&upgradeImageReqInterval, &image_notify->reqInterval, sizeof(upgradeImageReqInterval));
				if (upgradeImageReqInterval < MIN_IMAGE_REQ_INTERVAL_MILLI_SEC)
				{
					upgradeImageReqInterval = MIN_IMAGE_REQ_INTERVAL_MILLI_SEC;
				}

				image_index = upgradeImageStart;
				image_end = upgradeImageStart + image_size;
				currUpgradeState = STATE_START_DOWNLOAD;
				otauTimerStart(DOMAIN_OTAU_UPGRADE, upgradeImageReqInterval, TIMER_MODE_SINGLE);
			}
			break;
		}
		case OTA_IMAGE_RESPONSE:
		{
			if(STATE_IMAGE_REQUESTED == currUpgradeState)
			{
				otauImageResponse_t *image_resp = (otauImageResponse_t *)payload;
				block_size = APP_MAX_PAYLOAD_SIZE;
				if(image_index == image_resp->blockAddr)
				{
					imageReqRetry = 0;
					memcpy(&block, &(image_resp->block), image_resp->blockSize);
					nvm_write(INT_FLASH, UPGRADE_OFFSET_ADDRESS + (image_resp->blockAddr - upgradeImageStart), (uint8_t *)&block, image_resp->blockSize);
					image_index += image_resp->blockSize;
					if(image_index < image_end)
					{
						otauTimerStart(DOMAIN_OTAU_UPGRADE, upgradeImageReqInterval, TIMER_MODE_SINGLE);
					}
					else
					{
						const uint8_t *byte = (uint8_t *)UPGRADE_OFFSET_ADDRESS;
						uint32_t index = 0;
						uint8_t crc = 0;
						while(index < image_size)
						{
							crc ^= *byte;
							byte++;
							index++;
						}
						if(crc == image_crc)
						{
							otauTimerStop(DOMAIN_OTAU_UPGRADE);
							send_switch_req();
						}
						else
						{
							image_index = upgradeImageStart;
							currUpgradeState = STATE_START_DOWNLOAD;
							otauTimerStart(DOMAIN_OTAU_UPGRADE, upgradeImageReqInterval, TIMER_MODE_SINGLE);
						}
					}
				}
			}
			break;
		}
		case OTA_SWITCH_RESPONSE:
		{
			if(STATE_WAITING_FOR_SWITCH == currUpgradeState)
			{
				otauSwitchImageResponse_t *switch_resp = (otauSwitchImageResponse_t *)payload;
				if (0 == switch_resp->switchTime)
				{
					currUpgradeState = STATE_SWITCH_SUCCESS;
					otauTimerStart(DOMAIN_OTAU_UPGRADE, SWITCH_INTERVAL_MILLI_SEC, TIMER_MODE_SINGLE);
				}
				else
				{
					currUpgradeState = STATE_SWITCH_SUCCESS;
					otauTimerStart(DOMAIN_OTAU_UPGRADE, switch_resp->switchTime, TIMER_MODE_SINGLE);
				}
			}
			break;
		}
		default:
		{
			break;
		}
	}
}

void otauUpgradeSentFrame(addr_mode_t addr_mode, uint8_t *addr, uint8_t status)
{
	otauUpgradeConfirmWait = 0;
	if (currOtauUpgradeState == IMAGE_REQUEST_SENT)
	{
		otauTimerStart(DOMAIN_OTAU_UPGRADE, upgradeImageReqInterval, TIMER_MODE_SINGLE);
	}
	if (OTAU_SUCCESS != status && currOtauUpgradeState == SWITCH_REQUEST_SENT)
	{
		otauTimerStart(DOMAIN_OTAU_UPGRADE, SWITCH_REQ_INTERVAL_MILLI_SEC, TIMER_MODE_SINGLE);
	}
	else
	{
		currOtauUpgradeState = UPGRADE_OTAU_IDLE;
	}
}

void otauUpgradeTimerHandler(SYS_Timer_t *timer)
{
	if (STATE_IMAGE_REQUESTED == currUpgradeState || STATE_START_DOWNLOAD == currUpgradeState || STATE_IMAGE_RESPONDED == currUpgradeState)
	{
		if (otauUpgradeConfirmWait)
		{
			otauTimerStart(DOMAIN_OTAU_UPGRADE, upgradeImageReqInterval, TIMER_MODE_SINGLE);
		}
		else
		{
			send_image_req(image_index);
		}
	}
	else if(STATE_WAITING_FOR_SWITCH == currUpgradeState)
	{
		send_switch_req();
	}
	else if (STATE_SWITCH_SUCCESS == currUpgradeState)
	{
		app_info_t app_info;
		otauTimerStop(DOMAIN_OTAU_UPGRADE);
		memset((uint8_t *)&app_info, 0x00, sizeof(app_info_t));
		app_info.default_app = 0;
		app_info.required_features.mask = 0;
		app_info.app_cnt = 1;
		memcpy(app_info.appTable[0].app_name, "MiWi Mesh", sizeof("MiWi Mesh"));
		app_info.appTable[0].app_start_addr = APP_START_ADDRESS;
		app_info.appTable[0].src_addr = UPGRADE_OFFSET_ADDRESS;
		app_info.appTable[0].dest_addr = APP_START_ADDRESS;

#if defined(ENABLE_NETWORK_FREEZER)
		app_info.appTable[0].skip_pds_start = 0x00002100;
		app_info.appTable[0].skip_pds_end = 0x000060FF;
#else
		app_info.appTable[0].skip_pds_start = 0;
		app_info.appTable[0].skip_pds_end = 0;
#endif
		app_info.appTable[0].img_size = image_size;

		app_info.appTable[0].existingImageInfo.memtype = TYPE_INTERNAL;
		app_info.appTable[0].existingImageInfo.active = 1;

		app_info.appTable[0].bootInfo.update_action = 1;
		app_info.appTable[0].bootInfo.src_memtype = TYPE_INTERNAL;
		app_info.appTable[0].bootInfo.dest_memtype = TYPE_INTERNAL;
		app_info.appTable[0].bootInfo.img_type = 0;

		app_info.crc = calcCrc(&(app_info.default_app), sizeof(app_info_t) - 1);
		
		nvm_write(INT_FLASH, APP_INFO_START_ADDRESS, (uint8_t *)&app_info, sizeof(app_info_t));
		delay_ms(50);
		otauResetDevice();
	}
	else
	{

	}
}

static void send_image_req(uint32_t index)
{
	otauImageRequest_t image_request;
	uint8_t *address = NULL;

	imageReqRetry++;
	if ((IMAGE_REQ_RETRY_COUNT + 1) == imageReqRetry)
	{
		otauTimerStop(DOMAIN_OTAU_UPGRADE);
		otauUpgradeInit();
	}
	else
	{
		if ((index + block_size) > image_end)
		{
			block_size = image_end - index;
		}
		else
		{
			block_size = APP_MAX_PAYLOAD_SIZE;
		}
		image_request.msgId = OTA_IMAGE_REQUEST;
		image_request.reqType = 0x00;
		image_request.blockAddr = index;
		image_request.blockSize = block_size;
		currOtauUpgradeState = IMAGE_REQUEST_SENT;
		otauUpgradeConfirmWait = 1;
		currUpgradeState = STATE_IMAGE_REQUESTED;
		image_request.domainId = DOMAIN_OTAU_UPGRADE;
		otauDataSend(NATIVE_ADDR_MODE, address, &image_request.domainId, sizeof(otauImageRequest_t));
		otauTimerStart(DOMAIN_OTAU_UPGRADE, IMAGE_RESP_WAIT_INTERVAL_MILLI_SEC, TIMER_MODE_SINGLE);
	}
}


static void send_switch_req(void)
{
	addr_mode_t addr_mode;
	uint8_t *addr = NULL;
	otauSwitchImageRequest_t switch_image_req;
	switch_image_req.domainId = DOMAIN_OTAU_UPGRADE;
	switch_image_req.msgId = OTA_SWITCH_REQUEST;
	addr_mode = NATIVE_ADDR_MODE;
	otauGetServerDetails(addr);
	otauUpgradeConfirmWait = 1;
	currOtauUpgradeState = SWITCH_REQUEST_SENT;
	currUpgradeState = STATE_WAITING_FOR_SWITCH;
	otauDataSend(addr_mode, addr, &switch_image_req.domainId, 2);
}

static uint8_t calcCrc(uint8_t *pcBlock, uint8_t length)
{
	uint8_t i;
	uint8_t crc = 0;
	while (length--)
	{
		crc ^= *pcBlock++;
		for (i = 0; i < 8; i++)
		crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
	}
	return crc;
}


#endif //#if defined(OTAU_ENABLED) && !defined(OTAU_SERVER)