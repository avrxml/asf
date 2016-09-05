/**
 * \file
 *
 * \brief OTAU Profile
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel
 *Support</a>
 */

/****************************************************************************************
*							        Includes											*
*																						*
****************************************************************************************/
#include <asf.h>
#include <string.h>
#include "timer_hw.h"
#include "ble_utils.h"
#include "at_ble_api.h"
#include "ble_manager.h"
#include "otau_service.h"
#include "otau_profile.h"
#include "otau_flash_mgr.h"
#include "ofid.h"

/****************************************************************************************
*							        Globals												*
*																						*
****************************************************************************************/
otau_gatt_service_handler_t otau_gatt_service;

otau_profile_config_t *otau_profile_instance = NULL;

extern ofid_flash_info_t *flash_inst;

/** Initialize the OTAU Process structure members */
otau_process_t otau_process_status = {
	/* OTAU Mode set to normal upgrade mode by default */
	.mode = OTAU_IMAG_NORMAL_UPGRADE_MODE,
	/* OTAU State to defaults state at initial */
	.cstate = OTAU_IDLE_STATE,
	/* Set the default OTAU process Status */
	.status = AT_OTAU_CMD_SUCCESS
};

/* BLE GAP event callback handlers for OTAU profile */
static /*const*/ ble_event_callback_t otau_gap_handle[] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	otau_connected_state_handler,
	otau_disconnect_event_handler,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};


/* BLE GATT Server event callback handlers for OTAU profile */
static /*const*/ ble_event_callback_t otau_gatt_server_handle[] = {
	NULL,
	otau_indication_confirmation_handler,
	otau_char_changed_handler,
	NULL,
	NULL,
	NULL,
	otau_mtu_changed_indication,
	NULL,
	NULL,
	NULL
};


/* BLE Custom event callback handlers for OTAU profile */
static /*const*/ ble_event_callback_t otau_custom_event_handle[] = {
	/* BLE custom event post callback handler */
	otau_custom_event_handler,
	/* BLE device ready internal event callback handler */
	NULL,
	/* BLE device max event reached callback handler */
	NULL
};

/**	OTAU Flash layout information - 
	Flash layout information will be populated by the OTAU Flash Interface driver based on the OTAU flash configuration 
*/
ofid_flash_info_t dev_flash_info;

/** @brief calculate_total_image_crc function will calculate the whole downloaded 
 *			image crc based on the sections present as part of the current OTAU
 *
 *  @param[in] total_sections total no of sections for calculating the CRC
 *  @param[in] total_crc total crc of the downloaded image
 *  @param[in] total_size total downloaded image size
 *	@return AT_BLE_SUCCESS in case the CRC's are matched or AT_BLE_FAILURE
 *
 */
static at_ble_status_t calculate_total_image_crc(uint8_t total_sections, image_crc_t total_crc, uint32_t total_size);

/** @brief otau_download_status which will check all section of the OTAU image download status
 *			If the download is incomplete then the download process will start from the particular
 *			section id, page no and block no
 *
 *	@param[in] resume_page_no resume page number needs to be copied into this location
 *	@param[in] resume_block_no resume block number needs to be copied into this location
 *	@param[in] resume_section_id resume section id will be copied
 *	@param[in] update_fw_ver Firmware version will be reset by boot loader when device is power cycled, 
 *			   it needs to be set to 0xFFFFFFFF
 *  @return	  returns AT_BLE_SUCCESS in case of resume required or at_ble_err_status_t in case of failure
 */
static at_ble_status_t otau_download_status(uint16_t *resume_page_no,
											uint16_t *resume_block_no,
											section_id_t *resume_section_id, 
											bool update_fw_ver);

/** @brief otau_state_machine_controller function controls each state of OTAU 
 *			before executing the functions of the OTAU
 *  @param[in] next_state next state that requires to be changed
 *	@return AT_BLE_SUCCESS state execution is allowed, AT_BLE_FAILURE when state executions are not allowed
 *
 */
at_ble_status_t otau_state_machine_controller(otau_state_t next_state);



/****************************************************************************************
*							        Implementations										*
*                                       												*
****************************************************************************************/

/** @brief otau_state_machine_controller function controls each state of OTAU 
 *			before executing the functions of the OTAU
 *  @param[in] next_state next state that requires to be changed
 *	@return AT_BLE_SUCCESS state execution is allowed, AT_BLE_FAILURE when state executions are not allowed
 *
 */
at_ble_status_t otau_state_machine_controller(otau_state_t next_state)
{
	at_ble_status_t status = AT_BLE_SUCCESS;

	DBG_OTAU("[OTAU] CState:%d, NState:%d Status:%d", otau_process_status.cstate, next_state, otau_process_status.status);
	switch(otau_process_status.cstate)
	{
		/* OTAU state machine is in idle state by default (default variable value) */
		case OTAU_IDLE_STATE:
		{
			/* Idle state to next State */
			if ((otau_process_status.status == AT_OTAU_CMD_SUCCESS) && (next_state == OTAU_INIT_STATE))
			{
				otau_process_status.cstate = OTAU_INIT_STATE;				
			}
			else
			{
				otau_process_status.cstate = OTAU_ABORT_STATE;
				status = AT_BLE_FAILURE;
			}
		}
		break;

		/* OTAU state machine is in initialization state, during OTAU initialization */
		case OTAU_INIT_STATE:
		{
			if ((otau_process_status.status == AT_OTAU_CMD_SUCCESS) && (next_state == OTAU_OFD_INIT_STATE))
			{
				otau_process_status.cstate = OTAU_OFD_INIT_STATE;
			}
			else
			{
				otau_process_status.cstate = OTAU_ABORT_STATE;
				status = AT_BLE_FAILURE;
			}
		}
		break;
		
		/* OTAU OFD Initialization state */
		case OTAU_OFD_INIT_STATE:
		{
			if ((otau_process_status.status == AT_OTAU_CMD_SUCCESS) && (next_state == OTAU_IMAGE_NOTIFICATION_STATE))
			{
				otau_process_status.cstate = OTAU_IMAGE_NOTIFICATION_STATE;
			}
			else
			{
				otau_process_status.cstate = OTAU_ABORT_STATE;
				status = AT_BLE_FAILURE;
			}
		}
		break;
		
		/* OTAU Image notification state */
		case OTAU_IMAGE_NOTIFICATION_STATE:
		{
			if ((otau_process_status.status == AT_OTAU_CMD_SUCCESS) && (next_state == OTAU_IMAGE_INFO_WAIT_STATE))
			{
				otau_process_status.cstate = OTAU_IMAGE_INFO_WAIT_STATE;
			}
			else if ((otau_process_status.status == AT_OTAU_CMD_SUCCESS) && (next_state == OTAU_IMAGE_NOTIFICATION_STATE))
			{
				otau_process_status.cstate = OTAU_IMAGE_NOTIFICATION_STATE;
			}
			else
			{
				otau_process_status.cstate = OTAU_ABORT_STATE;
				status = AT_BLE_FAILURE;
			}
		}
		break;
		
		/* OTAU Image information wait state */
		case OTAU_IMAGE_INFO_WAIT_STATE:
		{
			if ((otau_process_status.status == AT_OTAU_CMD_SUCCESS) && (next_state == OTAU_PAGE_DOWNLOAD_START_STATE))
			{
				otau_process_status.cstate = OTAU_PAGE_DOWNLOAD_START_STATE;
			}
			else if ((otau_process_status.status == AT_OTAU_CMD_SUCCESS) && (next_state == OTAU_BLOCK_DOWNLOAD_STATE))
			{
				otau_process_status.cstate = OTAU_BLOCK_DOWNLOAD_STATE;
			}
			else if ((otau_process_status.status == AT_OTAU_CMD_SUCCESS) && (next_state == OTAU_IMAGE_NOTIFICATION_STATE))
			{
				otau_process_status.cstate = OTAU_IMAGE_NOTIFICATION_STATE;
			}
			else
			{
				otau_process_status.cstate = OTAU_ABORT_STATE;
				status = AT_BLE_FAILURE;
			}
		}
		break;
		
		/* OTAU state machine will set to page downstate during page mode */
		case OTAU_PAGE_DOWNLOAD_START_STATE:
		{
			if ((otau_process_status.status == AT_OTAU_CMD_SUCCESS) && (next_state == OTAU_PAGE_DOWNLOADING_STATE))
			{
				otau_process_status.cstate = OTAU_PAGE_DOWNLOADING_STATE;
			}
			else
			{
				otau_process_status.cstate = OTAU_ABORT_STATE;
				status = AT_BLE_FAILURE;
			}
		}
		break;
		
		case OTAU_PAGE_DOWNLOADING_STATE:
		{
			if ((otau_process_status.status == AT_OTAU_CMD_SUCCESS) && (next_state == OTAU_PAGE_DOWNLOADING_STATE))
			{
				otau_process_status.cstate = OTAU_PAGE_DOWNLOADING_STATE;
			}
			else if ((otau_process_status.status == AT_OTAU_CMD_SUCCESS) && (next_state == OTAU_SECTION_END_STATE))
			{
				otau_process_status.cstate = OTAU_SECTION_END_STATE;
			}
			else
			{
				otau_process_status.cstate = OTAU_ABORT_STATE;
				status = AT_BLE_FAILURE;
			}
		}
		break;
		
		/* OTAU state machine will set to section end state if it receives the complete section */
		case OTAU_SECTION_END_STATE:
		{
			if ((otau_process_status.status == AT_OTAU_CMD_SUCCESS) && (next_state == OTAU_SECTION_END_CONFIRM_STATE))
			{
				otau_process_status.cstate = OTAU_SECTION_END_CONFIRM_STATE;
			}
			else
			{
				otau_process_status.cstate = OTAU_ABORT_STATE;
				status = AT_BLE_FAILURE;
			}
		}
		break;
		
		/* OTAU state machine will set to section end confirm state, if it sends the complete section */
		case OTAU_SECTION_END_CONFIRM_STATE:
		{
			if ((otau_process_status.status == AT_OTAU_CMD_SUCCESS) && (next_state == OTAU_PAGE_DOWNLOAD_START_STATE))
			{
				otau_process_status.cstate = OTAU_PAGE_DOWNLOAD_START_STATE;
			}
			else if ((otau_process_status.status == AT_OTAU_CMD_SUCCESS) && (next_state == OTAU_PAGE_DOWNLOADING_STATE))
			{
				otau_process_status.cstate = OTAU_PAGE_DOWNLOADING_STATE;
			}
			else if ((otau_process_status.status == AT_OTAU_CMD_SUCCESS) && (next_state == OTAU_IMAGE_END_STATE))
			{
				otau_process_status.cstate = OTAU_IMAGE_END_STATE;
			}
			else
			{
				otau_process_status.cstate = OTAU_ABORT_STATE;
				status = AT_BLE_FAILURE;
			}
		}
		break;
		
		/* OTAU state machine will set to image end state, when target device receives the complete image */
		case OTAU_IMAGE_END_STATE:
		{
			if ((otau_process_status.status == AT_OTAU_CMD_SUCCESS) && (next_state == OTAU_IMAGE_END_CONFIRM_STATE))
			{
				otau_process_status.cstate = OTAU_IMAGE_END_CONFIRM_STATE;
			}
			else
			{
				otau_process_status.cstate = OTAU_ABORT_STATE;
				status = AT_BLE_FAILURE;
			}
		}
		break;
		
		/* OTAU state machine will set to image end confirm state, when target device receives the complete image and respond back */
		case OTAU_IMAGE_END_CONFIRM_STATE:
		{
			if ((otau_process_status.status == AT_OTAU_CMD_SUCCESS) && (next_state == OTAU_IMAGE_SWITCH_STATE))
			{
				otau_process_status.cstate = OTAU_IMAGE_SWITCH_STATE;
			}
			else
			{
				otau_process_status.cstate = OTAU_ABORT_STATE;
				status = AT_BLE_FAILURE;
			}
		}
		break;
		
		/* OTAU state machine will be set to image switch state once the image end confirm received */
		case OTAU_IMAGE_SWITCH_STATE:
		{
			if ((otau_process_status.status == AT_OTAU_CMD_SUCCESS) && (next_state == OTAU_IMAGE_SWITCH_CONFIRM_STATE))
			{
				otau_process_status.cstate = OTAU_IMAGE_SWITCH_CONFIRM_STATE;
			}
			else
			{
				otau_process_status.cstate = OTAU_ABORT_STATE;
				status = AT_BLE_FAILURE;
			}
		}
		break;
		
		/* OTAU image switch confirm received from application */
		case OTAU_IMAGE_SWITCH_CONFIRM_STATE:
		{
			if (otau_process_status.status == AT_OTAU_CMD_SUCCESS)
			{
				otau_process_status.cstate = OTAU_INIT_STATE;
			}
			else
			{
				otau_process_status.cstate = OTAU_ABORT_STATE;
				status = AT_BLE_FAILURE;
			}
		}
		break;
		
		/* OTAU manager or application is requested to pause the OTAU process */
		case OTAU_PAUSE_STATE:
		{
			if ((otau_process_status.status == AT_OTAU_CMD_SUCCESS) && (next_state == OTAU_IMAGE_RESUME_STATE))
			{
				otau_process_status.cstate = OTAU_IMAGE_RESUME_STATE;
			}
			else
			{
				otau_process_status.cstate = OTAU_ABORT_STATE;
				status = AT_BLE_FAILURE;
			}
		}
		break;
		
		/* OTAU image might be interrupted due to various reasons, but again it needs to resume from where it left off */
		case OTAU_IMAGE_RESUME_STATE:
		{
			if ((otau_process_status.status == AT_OTAU_CMD_SUCCESS) && (next_state == OTAU_IMAGE_NOTIFICATION_STATE))
			{
				otau_process_status.cstate = OTAU_IMAGE_NOTIFICATION_STATE;
			}
			else if (otau_process_status.status == AT_OTAU_CMD_SUCCESS)
			{
				otau_process_status.cstate = OTAU_INIT_STATE;
			}
			else
			{
				otau_process_status.cstate = OTAU_ABORT_STATE;
				status = AT_BLE_FAILURE;
			}
		}
		break;
				
		/* OTAU image confirmation received from the right source */
		case OTAU_IMAGE_RESUME_CONFIRM_STATE:
		{
			if (otau_process_status.status == AT_OTAU_CMD_SUCCESS)
			{
				otau_process_status.cstate = OTAU_INIT_STATE;
			}
			else
			{
				otau_process_status.cstate = OTAU_ABORT_STATE;
				status = AT_BLE_FAILURE;
			}
		}
		break;
		
		/* OTAU state machine will set to block download state during block mode */
		case OTAU_BLOCK_DOWNLOAD_STATE:
		{
			if (otau_process_status.status == AT_OTAU_CMD_SUCCESS)
			{
				otau_process_status.cstate = OTAU_INIT_STATE;
			}
			else
			{
				otau_process_status.cstate = OTAU_ABORT_STATE;
				status = AT_BLE_FAILURE;
			}
		}
		break;
		
		/* OTAU state machine will be in section download state during beginning of one section or end of section */
		case OTAU_SECTION_DOWNLOAD_STATE:
		{
			if (otau_process_status.status == AT_OTAU_CMD_SUCCESS)
			{
				otau_process_status.cstate = OTAU_INIT_STATE;
			}
			else
			{
				otau_process_status.cstate = OTAU_ABORT_STATE;
				status = AT_BLE_FAILURE;
			}
		}
		break;
		
		/* OTAU state machine will set to page end state if it receives the complete page */
		case OTAU_PAGE_END_STATE:
		{
			if (otau_process_status.status == AT_OTAU_CMD_SUCCESS)
			{
				otau_process_status.cstate = OTAU_INIT_STATE;
			}
			else
			{
				otau_process_status.cstate = OTAU_ABORT_STATE;
				status = AT_BLE_FAILURE;
			}
		}
		break;
		
		/* OTAU page end confirmed state */
		case OTAU_PAGE_END_CONFIRM_STATE:
		{
			if (otau_process_status.status == AT_OTAU_CMD_SUCCESS)
			{
				otau_process_status.cstate = OTAU_INIT_STATE;
			}
			else
			{
				otau_process_status.cstate = OTAU_ABORT_STATE;
				status = AT_BLE_FAILURE;
			}
		}
		break;		
				
		/* OTAU state machine is in abort state, in case of any OTAU abort send by OTAU manager */
		case OTAU_ABORT_STATE:
		{
			if (otau_process_status.status == AT_OTAU_CMD_SUCCESS)
			{
				otau_process_status.cstate = OTAU_INIT_STATE;
			}
			else
			{
				otau_process_status.cstate = OTAU_ABORT_STATE;
				status = AT_BLE_FAILURE;
			}
		}
		break;
		
		/* OTAU state machine is in stopped state, in case the device is disconnected or error */
		case OTAU_STOPPED_STATE:
		{
			if (otau_process_status.status == AT_OTAU_CMD_SUCCESS)
			{
				otau_process_status.cstate = OTAU_INIT_STATE;
			}
			else
			{
				otau_process_status.cstate = OTAU_ABORT_STATE;
				status = AT_BLE_FAILURE;
			}
		}
		break;
		
		/* OTAU Disconnected state */
		case OTAU_DISCONNECTED_STATE:
		{
			if((otau_process_status.status == AT_OTAU_CMD_SUCCESS) && (next_state == OTAU_IMAGE_NOTIFICATION_STATE))
			{
				otau_process_status.cstate = OTAU_IMAGE_NOTIFICATION_STATE;
			}
			else if (otau_process_status.status == AT_OTAU_CMD_SUCCESS)
			{
				otau_process_status.cstate = OTAU_INIT_STATE;
			}
			else
			{
				otau_process_status.cstate = OTAU_ABORT_STATE;
				status = AT_BLE_FAILURE;
			}
		}
		break;
		
		default:
		{
			otau_process_status.cstate = OTAU_ABORT_STATE;
			otau_process_status.status = AT_OTAU_UNKNOWN_ERROR;
		}
		break;
		
		otau_process_status.status = AT_OTAU_CMD_FAILED;		
	}
	return status;
}

/** @brief otau_profile_init function initialize and defines the service of the OTAU profile
 *			also initializes the OTAU flash interface driver populates the flash information into 
 *			dev_flash_info global variable
 *  @param[in] params of type otau_profile_config_t to configure the OTAU
 *	@return	returns AT_BLE_SUCCESS on success or at_ble_err_status_t on failure
 */
at_ble_status_t otau_profile_init(void *params)
{
	at_ble_status_t status;
	otau_profile_config_t *otau_config = NULL;
	uint16_t resume_page_no = 0;
	uint16_t resume_block_no = 0;
	uint8_t resume_section_id = 0;
		
	OTAU_CHECK_NULL(params);
	
	otau_config = (otau_profile_config_t *)params;
	otau_profile_instance = otau_config;	

	otau_process_status.status = AT_OTAU_CMD_SUCCESS;	
	status = otau_state_machine_controller(OTAU_INIT_STATE);
		
	OTAU_CHECK_ERROR(status);
		
	/* Initialize the OFD  and get the Flash layout information */
	status = ofm_init(&dev_flash_info);
	
	if((otau_download_status(&resume_page_no, &resume_block_no, &resume_section_id, true)) == AT_BLE_SUCCESS)
	{
		DBG_OTAU("Image Restored to OTAU state");
	}
		
	OTAU_CHECK_ERROR(status);
	
	otau_process_status.status = AT_OTAU_CMD_SUCCESS;
	status = otau_state_machine_controller(OTAU_OFD_INIT_STATE);	
	OTAU_CHECK_ERROR(status);
		
	/* Register OTAU service into GATT Server database */
	status = otau_service_define((void *)&otau_config->service_config);
		
	OTAU_CHECK_ERROR(status);
	
	status = AT_BLE_FAILURE;
	
	otau_gap_handle[0] = NULL;
	otau_gap_handle[1] = NULL;
	otau_gap_handle[2] = NULL;
	otau_gap_handle[3] = NULL;
	otau_gap_handle[4] = NULL;
	otau_gap_handle[5] = otau_connected_state_handler;
	otau_gap_handle[6] = otau_disconnect_event_handler;
	otau_gap_handle[7] = NULL;
	otau_gap_handle[8] = NULL;
	otau_gap_handle[9] = NULL;
	otau_gap_handle[10] = NULL;
	otau_gap_handle[11] = NULL;
	otau_gap_handle[12] = NULL;
	otau_gap_handle[13] = NULL;
	otau_gap_handle[14] = NULL;
	otau_gap_handle[15] = NULL;
	otau_gap_handle[16] = NULL;
	otau_gap_handle[17] = NULL;
	otau_gap_handle[18] = NULL;
	
	otau_gatt_server_handle[0] = NULL;
	otau_gatt_server_handle[1] = otau_indication_confirmation_handler;
	otau_gatt_server_handle[2] = otau_char_changed_handler;
	otau_gatt_server_handle[3] = NULL;
	otau_gatt_server_handle[4] = NULL;
	otau_gatt_server_handle[5] = NULL;
	otau_gatt_server_handle[6] = otau_mtu_changed_indication;
	otau_gatt_server_handle[7] = NULL;
	otau_gatt_server_handle[8] = NULL;
	otau_gatt_server_handle[9] = NULL;
	
	otau_custom_event_handle[0] = otau_custom_event_handler;
	otau_custom_event_handle[1] = NULL;
	otau_custom_event_handle[2] = NULL;
		
	/* Register for BLE GAP event callbacks */
	if(ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_GAP_EVENT_TYPE,
	otau_gap_handle))
	{
		/* Register for BLE GATT Server event callbacks */
		if(ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
		BLE_GATT_SERVER_EVENT_TYPE,
		otau_gatt_server_handle))
		{
			/* Register for BLE custom event callbacks */
			if (ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
			BLE_CUSTOM_EVENT_TYPE,
			otau_custom_event_handle))
			{	
				/* All OTAU Events are successfully registered */			
				otau_process_status.status = AT_OTAU_CMD_SUCCESS;					
				DBG_OTAU("OTAU Service Initialization completed");
			}
		}
	}	
		
	return status;
}

/** @brief otau_restore_from_sleep restore the SPI and Flash once the device wakes up from sleep
 *
 *	@param[in] params of type void * reserved for future use
 *	@return	returns AT_BLE_SUCCESS on success or at_ble_err_status_t on failure
 */
at_ble_status_t otau_restore_from_sleep(void *params)
{
	at_ble_status_t status = AT_BLE_FAILURE;	
	
	if(ofid_init(params) == AT_OFID_OP_SUCESS)
	{
		status = AT_BLE_SUCCESS;
	}
	
	return status;
}

/** @brief otau_firmware_version gets the OTAU Firmware version
 *
 *	@param[in] params of type firmware_version_t
 *	@return	returns AT_BLE_SUCCESS on success or at_ble_err_status_t on failure
 */
at_ble_status_t otau_firmware_version(firmware_version_t *fw_version)
{
	at_ble_status_t status = AT_BLE_SUCCESS;	
	fw_version->major_number = OTAU_MAJOR_VERSION;
	fw_version->minor_number = OTAU_MINOR_VERSION;
	fw_version->build_number = OTAU_BUILD_NUMBER;
	return status;
}

/** @brief otau_service_define defines the OTAU service and creates the database for OTAU profile
 *
 *	@param[in] params of type otau_service_config_t
 *	@return	returns AT_BLE_SUCCESS on success or at_ble_err_status_t on failure
 */
at_ble_status_t otau_service_define(void *params)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	otau_service_config_t *service_config = NULL;
		
	OTAU_CHECK_NULL(params);
	service_config = (otau_service_config_t *)params;
	
	/* application defined Service permission configuration */
	at_ble_attr_permissions_t *char_permission = service_config->char_endpoint_permission;
	
	/* OTAU GATT Service initialize to default */
	status = otau_service_get_default(&otau_gatt_service);
	
	OTAU_CHECK_ERROR(status);
	
	/* Change the default to permission value to requested permission value */
	otau_gatt_service.chars[OTAU_INDICATION_CHAR_IDX].client_config_permissions = char_permission[OTAU_INDICATION_CHAR_IDX];
	otau_gatt_service.chars[OTAU_WRITE_CHAR_IDX].value_permissions = char_permission[OTAU_WRITE_CHAR_IDX];
	otau_gatt_service.chars[OTAU_READ_CHAR_IDX].value_permissions = char_permission[OTAU_READ_CHAR_IDX];
	
	/* Register the GATT Server service */
	status = otau_gatt_service_define(&otau_gatt_service, service_config->service_type);
	
	OTAU_CHECK_ERROR(status);

	return status;
}

/** @brief otau_disconnect_event_handler disconnects with the peer device called by the
 *			ble manager. OTAU state needs to be changed and status has to be modified.
 *
 *	@param[in] params of at_ble_disconnected_t type
 *
 *  @return	returns AT_BLE_SUCCESS on success or at_ble_err_status_t on failure
 */
at_ble_status_t otau_disconnect_event_handler(void *params)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	
	at_ble_disconnected_t *disconnect_param = NULL;
	
	OTAU_CHECK_NULL(params);
	disconnect_param = (at_ble_disconnected_t *)params;
	
	/* Check the OTAU manager is disconnected */
	if (disconnect_param->handle == otau_gatt_service.conn_hanle)
	{
		ble_set_ulp_mode(BLE_ULP_MODE_SET);
		/* set connection handle to default handle */
		otau_gatt_service.conn_hanle = DEFAULT_CONNECTION_HANDLE;		
	
		/* OTAU state machine to disconnected state */
		otau_process_status.cstate = OTAU_DISCONNECTED_STATE;
		otau_process_status.status = AT_OTAU_CMD_SUCCESS;
	}
	
	return status;
}

/** @brief otau_custom_event_handler OTAU custom event needs to be identified
 *			and processed. The custom events are identified with at_ble_custom_event_t
 *			
 *	@param[in] params of at_ble_custom_event_t type
 *  @return	returns AT_BLE_SUCCESS on success or at_ble_err_status_t on failure
 */
at_ble_status_t otau_custom_event_handler(void *params)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	
	otau_device_reset_request_t	*device_reset_req = NULL;
	
	OTAU_CHECK_NULL(params);
	
	device_reset_req = (otau_device_reset_request_t *)params;
	//if ((device_reset_req->req.length == (sizeof(otau_device_reset_request_t) -2)) &&
		//(device_reset_req->req.cmd == AT_OTAU_RESET_DEVICE))
	//{
		otau_device_reset();
		status = AT_BLE_SUCCESS;
	//}
	ALL_UNUSED(device_reset_req);
	return status;
}

/** @brief otau_send_indication sets the otau characteristic value and
 *			sends the indication
 *
 *  @param[in] conn_handle connection handle
 *  @param[in] att_handle attribute handle of the characteristics that needs to be updated
 *  @param[in] buf the new characteristic value
 *  @param[in] length length of the new characteristic value
 *  @return	  returns AT_BLE_SUCCESS on success or at_ble_err_status_t on failure
 */
at_ble_status_t otau_send_indication(at_ble_handle_t conn_handle, at_ble_handle_t att_handle, uint8_t *buf, uint16_t length)
{
	at_ble_status_t status;
	
	/** Updating the new characteristic value */
	status = at_ble_characteristic_value_set(att_handle,
											buf, length);
	
	OTAU_CHECK_ERROR(status);
	
	/** Write Success send indication */
	
	/** Sending the indication for the updated characteristic */
	status = at_ble_indication_send(conn_handle, att_handle);
	
	OTAU_CHECK_ERROR(status);

	return status;
}

/** @brief otau_indication_confirmation_handler called on indication confirmation
 *  event by the ble manager
 *
 *	@param[in] params of type at_ble_indication_confirmed_t
 *  @return	  returns AT_BLE_SUCCESS on success or at_ble_err_status_t on failure
 */
at_ble_status_t otau_indication_confirmation_handler(void * params)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	at_ble_indication_confirmed_t *indication_confirmed = NULL;
	
	OTAU_CHECK_NULL(params);
	indication_confirmed = (at_ble_indication_confirmed_t *)params;
	
	if ((indication_confirmed->conn_handle == otau_gatt_service.conn_hanle) &&
		(indication_confirmed->char_handle == otau_gatt_service.chars[OTAU_INDICATION_CHAR_IDX].char_val_handle))
	{
		/* Value sent to the OTAU manager and update the state machine */
		status = AT_BLE_SUCCESS;
	}
	
	return status;
}

/** @brief memcpy_nreverse copy the memory content from source location to destination memory location
 *			and reverse the copied content
 *
 *	@param[in] dst destination memory location
 *	@param[in] src source memory location
 *	@param[in] len length of the memory operation
 *  @return	  None
 */
void memcpy_nreverse(uint8_t *dst, uint8_t *src, uint32_t len)
{
	memcpy(dst, src, len);
	reverse_byte_buffer(dst, len);
}

static void por_update_firmware_version(section_id_t section_id, uint32_t section_addr)
{
	uint8_t page_buf[dev_flash_info.page_size];
	uint32_t backup_addr = flash_inst->metadata_addr + (3 * FLASH_SECTOR_SIZE);
	
	if((backup_addr + FLASH_SECTOR_SIZE) > flash_inst->flash_size )
	{
		DBG_OTAU("POR Error: Invalid memory location!!!");
	}
	
	DBG_OTAU("POR Backup Address: 0x%6X, section_addr: 0x%6X", backup_addr, section_addr);
	
	/* Erase the backup image section: Note backup location currently above the meta data and sector size */
	if(ofm_erase_memory(backup_addr, FLASH_SECTOR_SIZE) == AT_BLE_SUCCESS)
	{
		uint32_t index;						
		/* Read the image from actual location and Write to backup location */
		
		for (index = 0; index < FLASH_SECTOR_SIZE; index += dev_flash_info.page_size)
		{
			if(ofm_read_page(section_id, section_addr+index, page_buf, dev_flash_info.page_size) == AT_BLE_SUCCESS)
			{
				if (!index)
				{
					uint32_t ver = ((page_buf[0] << 24) | (page_buf[1] << 16) | \
								   (page_buf[2] << 8) | (page_buf[3]));
					uint32_t size = ((page_buf[4] << 24) | (page_buf[5] << 16) | \
									(page_buf[6] << 8) | (page_buf[7]));
					if((ver == 0) && (size == 0))
					{
						DBG_OTAU("Cannot Update the FW version");
						return;
					}
				}
				if(ofm_write_page(section_id, backup_addr+index, page_buf, dev_flash_info.page_size) != AT_BLE_SUCCESS)
				{
					DBG_OTAU("POR: Backup Write Page failed");
				}
			}
			else
			{
				DBG_OTAU("POR: Read Page failed");
			}
		}
		
		/* Erase the actual image section */
		if(ofm_erase_memory(section_addr, FLASH_SECTOR_SIZE) == AT_BLE_SUCCESS)
		{
			for (index = 0; index < FLASH_SECTOR_SIZE; index += dev_flash_info.page_size)
			{
				/* copy from backup section into OTA section */	
				if(ofm_read_page(section_id, backup_addr+index, page_buf, dev_flash_info.page_size) == AT_BLE_SUCCESS)
				{
					/* Read First page from backup location  and update the firmware version */
					if (!index)
					{
						memset(page_buf, 0xFF, sizeof(firmware_version_t));
					}
					if(ofm_write_page(section_id, section_addr+index, page_buf, dev_flash_info.page_size) != AT_BLE_SUCCESS)
					{
						DBG_OTAU("POR: Write Page failed");
					}
				}
				else
				{
					DBG_OTAU("POR: Backup Read Page failed");
				}
			}
			ofm_erase_memory(backup_addr, FLASH_SECTOR_SIZE);
		}
	}
	else
	{
		DBG_OTAU("POR: Backup memory erase failed");
	}
}

/** @brief otau_download_status which will check all section of the OTAU image download status
 *			If the download is incomplete then the download process will start from the particular
 *			section id, page no and block no
 *
 *	@param[in] resume_page_no resume page number needs to be copied into this location
 *	@param[in] resume_block_no resume block number needs to be copied into this location
 *	@param[in] resume_section_id resume section id will be copied
 *	@param[in] update_fw_ver Firmware version will be reset by boot loader when device is power cycled, 
 *			   it needs to be set to 0xFFFFFFFF
 *  @return	  returns AT_BLE_SUCCESS in case of resume required or at_ble_err_status_t in case of failure
 */
static at_ble_status_t otau_download_status(uint16_t *resume_page_no, 
											uint16_t *resume_block_no,
											section_id_t *resume_section_id, bool update_fw_ver)
{
	/* Check the Firmware needs to be resumed */
	image_meta_data_t meta_data;
	at_ble_status_t status = AT_BLE_FAILURE;
	
	/* No section needs to be  updated */
	*resume_section_id = 0xFF;
	
	if(ofm_read_meta_data(&meta_data, OTAU_IMAGE_META_DATA_ID) == AT_BLE_SUCCESS)
	{
		uint32_t data_len = 0;
		uint32_t downloaded_image_size = 0;
		
		/* OTAU must have at least one section */
		if (!meta_data.dev_info.total_sections)
		{
			return status;
		}
			
			
		if (meta_data.patch_section.section_id == 0x01)
		{
			/* Check for patch section */
			memcpy((uint8_t *)&downloaded_image_size, meta_data.patch_downloaded_info.size, 3);
			memcpy((uint8_t *)&data_len, meta_data.patch_section.size, 3);
			
			if (update_fw_ver)
			{
				uint32_t section_addr;
				section_addr = meta_data.patch_section.start_address;
				
				if((meta_data.section_image_id & SECTION1_IMAGE_IDENTIFIER_MASK) == SECTION1_BOTTOM_IMAGE_IDENTIFIER)
				{
					/* Calculate the address */
					section_addr += (dev_flash_info.section_info[meta_data.patch_section.section_id - 1].size/2);
				}
				
				/* Update the Patch Section */
				por_update_firmware_version(meta_data.patch_section.section_id, section_addr);
			}
			if (data_len > downloaded_image_size)
			{
				/* Resume from the Patch section */
				*resume_page_no = (downloaded_image_size / dev_flash_info.page_size);
				*resume_section_id = meta_data.patch_section.section_id;				
				status = AT_BLE_SUCCESS;
			}
		}
									
		if (meta_data.app_header_section.section_id == 0x02)
		{
			/* Check for App header section */
			memcpy((uint8_t *)&downloaded_image_size, meta_data.app_hdr_downloaded_info.size, 3);
			memcpy((uint8_t *)&data_len, meta_data.app_header_section.size, 3);
			
			if(update_fw_ver)
			{
				uint32_t section_addr;
				section_addr = meta_data.app_header_section.start_address;
				if((meta_data.section_image_id & SECTION1_IMAGE_IDENTIFIER_MASK) == SECTION1_BOTTOM_IMAGE_IDENTIFIER)
				{
					/* Calculate the address */
					section_addr += (dev_flash_info.section_info[meta_data.app_header_section.section_id - 1].size/2);
				}
				por_update_firmware_version(meta_data.app_header_section.section_id, section_addr);
			}
				
			if ((data_len > downloaded_image_size) && (*resume_section_id == 0xFF))
			{
				/* Resume from the App header section */
				*resume_page_no = (downloaded_image_size / dev_flash_info.page_size);
				*resume_section_id = meta_data.app_header_section.section_id;				
				status = AT_BLE_SUCCESS;
			}
		}
			
		if (meta_data.app_section.section_id == 0x03)
		{
			/* Check for Application section */
			memcpy((uint8_t *)&downloaded_image_size, meta_data.app_downloaded_info.size, 3);
			memcpy((uint8_t *)&data_len, meta_data.app_section.size, 3);
			
			if(update_fw_ver)
			{
				uint32_t section_addr;
				section_addr = meta_data.app_section.start_address;
				if((meta_data.section_image_id & SECTION1_IMAGE_IDENTIFIER_MASK) == SECTION1_BOTTOM_IMAGE_IDENTIFIER)
				{
					/* Calculate the address */
					section_addr += (dev_flash_info.section_info[meta_data.app_section.section_id - 1].size/2);
				}
				por_update_firmware_version(meta_data.app_section.section_id, section_addr);
			}
					
			if ((data_len > downloaded_image_size) && (*resume_section_id == 0xFF))
			{
				/* Resume from the App header section */
				*resume_page_no = (downloaded_image_size / dev_flash_info.page_size);
				*resume_section_id = meta_data.app_section.section_id;				
				status = AT_BLE_SUCCESS;
			}
		}
	}				

	return status;
}

/** @brief otau_image_notify_request_handler image notification will be sent by OTAU manager
 *			in case of new image needs to be notified or old image needs to be downloaded or 
 *			force upgrade is requested by OTAU manager
 *
 *	@param[in] params of type image_notification_req_t 
 *  @return	  returns AT_BLE_SUCCESS in case of image notification processed successfully or at_ble_err_status_t in case of failure
 */
at_ble_status_t otau_image_notify_request_handler(void *params)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	image_notification_req_t *image_notification = NULL;
	
	OTAU_CHECK_NULL(params);
	image_notification = (image_notification_req_t *)params;
	device_info_t dev_info;		
	uint32_t fw_ver;
	uint32_t new_fw_ver;
	uint16_t new_hw_ver, hw_ver;
	
	/* Validate the incoming frame */
	if (image_notification->req.length != (sizeof(image_notification_req_t)-OTAU_FRAME_LEN_SIZE))
	{
		otau_invalid_frame_resp(NULL);
		return status;
	}
	
	status = otau_state_machine_controller(OTAU_IMAGE_NOTIFICATION_STATE);
	OTAU_CHECK_ERROR(status);
	
	DBG_OTAU("Image Notification received");
	
	/* Read current image info and device information */
	if (ofm_get_device_info(&dev_info) == AT_BLE_SUCCESS)
	{
		bool otau_permission = false;
		 memcpy_nreverse((uint8_t *)&fw_ver, (uint8_t *)&dev_info.fw_version, sizeof(fw_ver));
		 
		 memcpy_nreverse((uint8_t *)&hw_ver, (uint8_t *)&dev_info.hw_version, sizeof(hw_ver));	 
		 
		 memcpy_nreverse((uint8_t *)&new_fw_ver, (uint8_t *)&image_notification->fw_ver, sizeof(new_fw_ver));
		 
		 memcpy_nreverse((uint8_t *)&new_hw_ver, (uint8_t *)&image_notification->hw_ver, sizeof(new_hw_ver));
		 
		 DBG_OTAU("Get Dev Info Success");
		 
		 /* Check the permission from application for updating the firmware */
		 if (otau_profile_instance->app_cb.otau_image_notification != NULL)
		 {
			 firmware_version_t new_ver, old_ver;
			 
			 /* Send the Firmware version in aligned format */
			 memcpy((uint8_t *)&new_ver, (uint8_t *)&image_notification->fw_ver, sizeof(firmware_version_t));
			 memcpy((uint8_t *)&old_ver, (uint8_t *)&dev_info.fw_version, sizeof(firmware_version_t));	
			 
			  if(image_notification->req.cmd == AT_OTAU_FORCE_UDPATE_REQUEST)
			  {
				  otau_permission = true;
			  }	 
			 
			 otau_profile_instance->app_cb.otau_image_notification(&new_ver,
																 &old_ver,
																 &otau_permission);
		 }
		 else
		 {
			 otau_permission = true;
		 }
		 
		/* Compare the Product ID and vendor ID */
		if ((dev_info.product_id == image_notification->product_id) && 
			(dev_info.vendor_id == image_notification->vendor_id) &&
			(otau_permission || (image_notification->req.cmd == AT_OTAU_FORCE_UDPATE_REQUEST)) &&
			(hw_ver == new_hw_ver) &&
			(dev_info.hw_revision == image_notification->hw_rev))
		{
			
			image_meta_data_t meta_data;
			image_notification_resp_t image_notify_response; //For correct request
			uint16_t resume_page_no;
			uint16_t resume_block_no;
			section_id_t resume_section_id;	
			
			image_notify_response.fw_ver = dev_info.fw_version;
			image_notify_response.hardware_version = dev_info.hw_version;
			image_notify_response.hardware_revision = dev_info.hw_revision;
			if (image_notification->req.cmd == AT_OTAU_FORCE_UDPATE_REQUEST)
			{
				image_notify_response.resp.cmd = AT_OTAU_FORCE_UDPATE_RESP;
			}
			else
			{
				image_notify_response.resp.cmd = AT_OTAU_IMAGE_NOTIFY_RESP;
			}
			
			image_notify_response.resp.length = (sizeof(image_notification_resp_t) - sizeof(image_notify_response.resp.length));
			
			image_notify_response.image_option = OTAU_START_DOWNLOAD;						
				
			if(ofm_read_meta_data(&meta_data, OTAU_IMAGE_META_DATA_ID) == AT_BLE_SUCCESS)
			{
					memcpy_nreverse((uint8_t *)&fw_ver, (uint8_t *)&meta_data.dev_info.fw_version, sizeof(fw_ver));
						 
					memcpy_nreverse((uint8_t *)&hw_ver, (uint8_t *)&meta_data.dev_info.hw_version, sizeof(hw_ver));
						 
				if((meta_data.dev_info.product_id == image_notification->product_id) &&
				(meta_data.dev_info.vendor_id == image_notification->vendor_id) &&
				((fw_ver == new_fw_ver) && (image_notification->req.cmd != AT_OTAU_FORCE_UDPATE_REQUEST)) &&
				(hw_ver == new_hw_ver) &&
				(meta_data.dev_info.hw_revision == image_notification->hw_rev))
				{
					DBG_OTAU("OTAU Resume Firmware Available: Firmware Version->%d.%d.%d",
									image_notification->fw_ver.major_number,
									image_notification->fw_ver.minor_number,
									image_notification->fw_ver.build_number);
					if((otau_download_status(&resume_page_no, &resume_block_no, &resume_section_id, false)) != AT_BLE_SUCCESS)
					{
						/* Update need not to be resumed and need to erase the meta data and sections */
						memcpy((uint8_t *)&meta_data.dev_info, (uint8_t *)&dev_info, sizeof(device_info_t));
						memcpy((uint8_t *)&meta_data.dev_info.fw_version, (uint8_t *)&image_notification->fw_ver, sizeof(firmware_version_t));
						meta_data.security_level = image_notification->security_level;						
						if(ofm_otau_meta_data_update(&meta_data) == AT_BLE_SUCCESS)
						{
							DBG_OTAU("Failed to update meta data");
						}
					}
					else
					{
						image_notify_response.image_option = OTAU_RESUME_DOWNLOAD;
					}
				}
				else
				{
					if(ofm_read_meta_data(&meta_data, OTAU_IMAGE_META_DATA_ID) == AT_BLE_SUCCESS)
					{
						memcpy((uint8_t *)&meta_data.dev_info, (uint8_t *)&dev_info, sizeof(device_info_t));
						memcpy((uint8_t *)&meta_data.dev_info.fw_version, (uint8_t *)&image_notification->fw_ver, sizeof(firmware_version_t));
						meta_data.security_level = image_notification->security_level;
						
						meta_data.dev_info.total_sections = 0;						
						memset(meta_data.app_header_section.size, 0, 3);
						memset(meta_data.patch_section.size, 0, 3);
						memset(meta_data.app_section.size, 0, 3);						
						memset(meta_data.app_hdr_downloaded_info.size, 0, 3);
						memset(meta_data.patch_downloaded_info.size, 0, 3);
						memset(meta_data.app_downloaded_info.size, 0, 3);
					}
					
					
					DBG_OTAU("Firmware Update Available: Firmware Version->%d.%d.%d",
					image_notification->fw_ver.major_number,
					image_notification->fw_ver.minor_number,
					image_notification->fw_ver.build_number);
					
					/* Update meta data */
					if(ofm_otau_meta_data_update(&meta_data) != AT_BLE_SUCCESS)
					{
						status = AT_BLE_FAILURE;
					}
				}
			 }
			 else
			 {
				 DBG_OTAU("OTAU Meta data read failed");
			 }
			if(otau_send_indication(otau_gatt_service.conn_hanle,
			otau_gatt_service.chars[OTAU_INDICATION_CHAR_IDX].char_val_handle,
			(uint8_t *)&image_notify_response,
			sizeof(image_notification_resp_t)) == AT_BLE_SUCCESS)
			{
				otau_process_status.status = AT_OTAU_CMD_SUCCESS;
				status = AT_BLE_SUCCESS;
			}
		}
		else
		{
			image_notification_failure_resp_t image_notify_failure_resp; //For incorrect request
			
			DBG_OTAU("OTAU Command Failed");
			image_notify_failure_resp.error = AT_OTAU_INVALID_IMAGE;
			image_notify_failure_resp.resp.cmd = AT_OTAU_FAILURE;
			image_notify_failure_resp.resp.length = (sizeof(image_notification_failure_resp_t) - 2);
			otau_send_indication(otau_gatt_service.conn_hanle,
								otau_gatt_service.chars[OTAU_INDICATION_CHAR_IDX].char_val_handle,
								(uint8_t *)&image_notify_failure_resp,
								sizeof(image_notification_failure_resp_t));
			otau_process_status.status = AT_OTAU_CMD_FAILED;
		}
		
	}
	else
	{
		DBG_OTAU("Get Device info failed");
	}		
	
	return status;
}

/** @brief otau_image_info_request_handler image details will be sent by OTAU manager
 *			in case of image notification accepted by OTAU Target Profile 
 *
 *	@param[in] params of type image_info_request_t 
 *  @return	  returns AT_BLE_SUCCESS in case of image info accepted or at_ble_err_status_t in case of failure
 */
at_ble_status_t otau_image_info_request_handler(void *params)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	image_info_request_t *image_info_request = NULL;
	
	OTAU_CHECK_NULL(params);
	image_info_request = (image_info_request_t *)params;
	image_meta_data_t meta_data;	

	/* Validate the incoming frame */
	if (image_info_request->req.length != (sizeof(image_info_request_t) - OTAU_FRAME_LEN_SIZE))
	{
		otau_invalid_frame_resp(NULL);
		return status;
	}
	
	status = otau_state_machine_controller(OTAU_IMAGE_INFO_WAIT_STATE);
	OTAU_CHECK_ERROR(status);
	
	if (image_info_request->req.cmd == AT_OTAU_IMAGE_INFO_NOTIFY_REQUEST)
	{
		if (image_info_request->total_image_size <= dev_flash_info.flash_size)
		{			
			/* Write into metadata */
			image_info_resp_t image_info_resp;//For correct request				
			uint16_t resume_page_no = 0;
			uint16_t resume_block_no = 0;
			section_id_t resume_section_id;			
			image_info_resp.block_size = dev_flash_info.page_size;
			
			
			image_info_resp.page_size = dev_flash_info.page_size;
			image_info_resp.resp.cmd = AT_OTAU_IMAGE_INFO_NOTIFY_RESP;
			image_info_resp.resp.length = (sizeof(image_info_resp_t)-2);
			
			if((otau_download_status(&resume_page_no, &resume_block_no, &resume_section_id, false)) == AT_BLE_SUCCESS)
			{
				/* Check firmware needs to be resumed */
				image_info_resp.block_no = resume_block_no;
				image_info_resp.page_no = resume_page_no;
				image_info_resp.image_option = OTAU_RESUME_DOWNLOAD;
				image_info_resp.section_id = resume_section_id;
				
				if(otau_send_indication(otau_gatt_service.conn_hanle,
				otau_gatt_service.chars[OTAU_INDICATION_CHAR_IDX].char_val_handle,
				(uint8_t *)&image_info_resp,
				sizeof(image_info_resp_t)) == AT_BLE_SUCCESS)
				{
					DBG_OTAU("Resuming Firmware Download from section id:%d, page no:%d, block no:%d",
					resume_section_id, resume_page_no, resume_block_no);
					status = AT_BLE_SUCCESS;
					otau_process_status.status = AT_OTAU_CMD_SUCCESS;
					if(otau_state_machine_controller(OTAU_PAGE_DOWNLOAD_START_STATE) == AT_BLE_SUCCESS)
					{
						otau_process_status.status = AT_OTAU_CMD_SUCCESS;
					}
				}
			}		
			else if(ofm_read_meta_data((void *)&meta_data, OTAU_IMAGE_META_DATA_ID) == AT_BLE_SUCCESS)
			{
				image_info_resp.block_no = 0;
				image_info_resp.page_no = 0;
				image_info_resp.image_option = OTAU_START_DOWNLOAD;
				image_info_resp.section_id = 0x01;
									
				meta_data.dev_info.total_sections = image_info_request->total_sections;									
				meta_data.total_image_size = image_info_request->total_image_size;
				memcpy((uint8_t *)&meta_data.app_header_section, (uint8_t *)&image_info_request->app_header_section, sizeof(image_section_info_t));
				memcpy((uint8_t *)&meta_data.patch_section, (uint8_t *)&image_info_request->patch_section, sizeof(image_section_info_t));
				memcpy((uint8_t *)&meta_data.app_section, (uint8_t *)&image_info_request->app_section, sizeof(image_section_info_t));
				meta_data.total_image_crc = image_info_request->total_image_crc;
				meta_data.patch_crc = image_info_request->patch_crc;
				meta_data.app_crc = image_info_request->app_crc;
				meta_data.app_header_crc = image_info_request->app_header_crc;
				meta_data.patch_downloaded_info.section_id = image_info_request->patch_section.section_id;
				memset(meta_data.patch_downloaded_info.size, 0, 3);
				meta_data.app_hdr_downloaded_info.section_id = image_info_request->app_header_section.section_id;
				memset(meta_data.app_hdr_downloaded_info.size, 0, 3);
				meta_data.app_downloaded_info.section_id = image_info_request->app_section.section_id;
				memset(meta_data.app_downloaded_info.size, 0, 3);
				
				DBG_OTAU("Updating to New Firmware: Total image Size->%d, Total Sections->%d",
				image_info_request->total_image_size, image_info_request->total_sections);
					
				if(ofm_otau_meta_data_update(&meta_data) == AT_BLE_SUCCESS)
				{
					/* Erase sections image */
					if(ofm_erase_section_image(image_info_request->patch_section.section_id) == AT_BLE_SUCCESS)
					{
						if(ofm_erase_section_image(image_info_request->app_header_section.section_id) == AT_BLE_SUCCESS)
						{
							if(ofm_erase_section_image(image_info_request->app_section.section_id) == AT_BLE_SUCCESS)
							{
								if(otau_send_indication(otau_gatt_service.conn_hanle,
								otau_gatt_service.chars[OTAU_INDICATION_CHAR_IDX].char_val_handle,
								(uint8_t *)&image_info_resp,
								sizeof(image_info_resp_t)) == AT_BLE_SUCCESS)
								{
									status = AT_BLE_SUCCESS;
									otau_process_status.status = AT_OTAU_CMD_SUCCESS;
									if(otau_state_machine_controller(OTAU_PAGE_DOWNLOAD_START_STATE) == AT_BLE_SUCCESS)
									{
										otau_process_status.status = AT_OTAU_CMD_SUCCESS;
									}
								}
							}
							else
							{
								DBG_OTAU("Failed to erase section %d:", image_info_request->app_section.section_id);
							}	
						}
						else
						{
							DBG_OTAU("Failed to erase section %d:", image_info_request->app_header_section.section_id);
						}	
					}
					else
					{
						DBG_OTAU("Failed to erase section %d:", image_info_request->patch_section.section_id);
					}					
				}
				else
				{
					DBG_OTAU("Image info req:meta data update failed");
				}
			}
			else
			{
				DBG_OTAU("Image info req: meta data read failed");
			}					
		}
		
		if(status != AT_BLE_SUCCESS)
		{
			image_info_failure_resp_t image_info_failure_resp;//For incorrect request
			image_info_failure_resp.resp.cmd = AT_OTAU_INVALID_IMAGE_SIZE;
			image_info_failure_resp.resp.length = (sizeof(image_info_failure_resp_t)-2);
			image_info_failure_resp.error = AT_OTAU_INVALID_IMAGE_SIZE;
			
			otau_send_indication(otau_gatt_service.conn_hanle,
			otau_gatt_service.chars[OTAU_INDICATION_CHAR_IDX].char_val_handle,
			(uint8_t *)&image_info_failure_resp,
			sizeof(image_info_failure_resp_t));
			
			otau_process_status.status = AT_OTAU_CMD_FAILED;
		}
	}
		
	return status;
}

/** @brief otau_page_data_request_handler image page data will be sent by OTAU manager
 *			page data will be stored into the Flash memory of OTAU
 *
 *	@param[in] params of type page_data_notify_request_t 
 *  @return	  returns AT_BLE_SUCCESS page data is correct or at_ble_err_status_t in case of failure
 */
at_ble_status_t otau_page_data_request_handler(void *params)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	page_data_notify_request_t *page_data_notify_req = NULL;
	
	OTAU_CHECK_NULL(params);
	page_data_notify_req = (page_data_notify_request_t *)params;
	
	/* Read meta data */
	image_meta_data_t meta_data;
	uint32_t write_addr = 0xFFFFFFFF;
	uint32_t data_len = 0;
	volatile bool write_into_flash = false;
	uint32_t index;
	uint8_t *param_buf = (uint8_t *)params;
	
	DBG_OTAU("OTAU Page Data:");
	for (index = 0; index < page_data_notify_req->req.length+sizeof(page_data_notify_req->req.length); index++)
	{
		DBG_OTAU_CONT("0x%2X, ", param_buf[index]);
	}
	
	status = otau_state_machine_controller(OTAU_PAGE_DOWNLOADING_STATE);
	OTAU_CHECK_ERROR(status);

	if(ofm_read_meta_data((void *)&meta_data, OTAU_IMAGE_META_DATA_ID) == AT_BLE_SUCCESS)
	{
		uint32_t percent = 0;
		/* Check page no and calculate the address to update */
		/* Check the section id */
		
		if (((page_data_notify_req->req.length-4) >  dev_flash_info.page_size) || ((page_data_notify_req->req.length-4) == 0))
		{
			DBG_OTAU("Invalid image page size");
			return AT_BLE_FAILURE;
		}
		
		if(page_data_notify_req->section_id == 0x01)
		{		
			memcpy((uint8_t *)&data_len, meta_data.patch_downloaded_info.size, 3);
			data_len += (page_data_notify_req->req.length - 4);
			memcpy(meta_data.patch_downloaded_info.size, (uint8_t *)&data_len, 3);
			memcpy((uint8_t *)&percent, meta_data.patch_section.size, 3);
			if((meta_data.section_image_id & SECTION1_IMAGE_IDENTIFIER_MASK) == SECTION1_BOTTOM_IMAGE_IDENTIFIER)
			{
				/* Calculate the address */
				write_addr = (meta_data.patch_section.start_address+ (page_data_notify_req->page_no * dev_flash_info.page_size));
				write_addr += (dev_flash_info.section_info[page_data_notify_req->section_id -1].size/2);									
			}
			else if((meta_data.section_image_id & SECTION1_IMAGE_IDENTIFIER_MASK) == SECTION1_TOP_IMAGE_IDENTIFIER)
			{
				write_addr = (meta_data.patch_section.start_address+ (page_data_notify_req->page_no * dev_flash_info.page_size));
			}
			write_into_flash = true;			
		}
		else if (page_data_notify_req->section_id == 0x02)
		{			
			memcpy((uint8_t *)&data_len, meta_data.app_hdr_downloaded_info.size, 3);
			data_len += (page_data_notify_req->req.length - 4);
			memcpy(meta_data.app_hdr_downloaded_info.size, (uint8_t *)&data_len, 3);
			memcpy((uint8_t *)&percent, meta_data.app_header_section.size, 3);
			if((meta_data.section_image_id & SECTION2_IMAGE_IDENTIFIER_MASK) == SECTION2_BOTTOM_IMAGE_IDENTIFIER)
			{
				/* Calculate the address */
				write_addr = (meta_data.app_header_section.start_address+ (page_data_notify_req->page_no * dev_flash_info.page_size));
				write_addr += ((dev_flash_info.section_info[page_data_notify_req->section_id -1].size/2));
							
			}
			else if((meta_data.section_image_id & SECTION2_IMAGE_IDENTIFIER_MASK) == SECTION2_TOP_IMAGE_IDENTIFIER)
			{
				write_addr = (meta_data.app_header_section.start_address+ (page_data_notify_req->page_no * dev_flash_info.page_size));
			}
			write_into_flash = true;
		}
		else if (page_data_notify_req->section_id == 0x03)
		{			
			memcpy((uint8_t *)&data_len, meta_data.app_downloaded_info.size, 3);
			data_len += (page_data_notify_req->req.length - 4);
			memcpy(meta_data.app_downloaded_info.size, (uint8_t *)&data_len, 3);
			memcpy((uint8_t *)&percent, meta_data.app_section.size, 3);
			if((meta_data.section_image_id & SECTION3_IMAGE_IDENTIFIER_MASK) == SECTION3_BOTTOM_IMAGE_IDENTIFIER)
			{
				/* Calculate the address */
				write_addr = (meta_data.app_section.start_address+ (page_data_notify_req->page_no * dev_flash_info.page_size));
				write_addr += ((dev_flash_info.section_info[page_data_notify_req->section_id-1].size/2));
								
			}
			else if((meta_data.section_image_id & SECTION3_IMAGE_IDENTIFIER_MASK) == SECTION3_TOP_IMAGE_IDENTIFIER)
			{
				write_addr = (meta_data.app_section.start_address+ (page_data_notify_req->page_no * dev_flash_info.page_size));
			}
			write_into_flash = true;
		}
		else
		{
			DBG_OTAU("Page Data req: Invalid section");
		}
		
		if(write_into_flash)
		{
			uint32_t page_data_byte;			
			memcpy((uint8_t *)&page_data_byte, page_data_notify_req->page_data, sizeof(page_data_byte));
			DBG_OTAU("Page Write:Address->0x%4X, sectonid->%d, data_len->%d, page no->0x%4X", write_addr, page_data_notify_req->section_id, data_len, page_data_notify_req->page_no);
			if(ofm_write_page(page_data_notify_req->section_id, write_addr, page_data_notify_req->page_data, dev_flash_info.page_size) == AT_BLE_SUCCESS)
			{			
				if(ofm_otau_meta_data_update((void *)&meta_data) == AT_BLE_SUCCESS)
				{
					page_data_notify_resp_t page_data_notify_resp; //For correct response
					page_data_notify_resp.page_no = page_data_notify_req->page_no;
					page_data_notify_resp.resp.cmd = AT_OTAU_PAGE_DATA_NOTIFY_RESP;
					page_data_notify_resp.resp.length = (sizeof(page_data_notify_resp_t) - sizeof(page_data_notify_resp.resp.length));
					page_data_notify_resp.section_id = page_data_notify_req->section_id;	
					
					DBG_OTAU("[DBG] Total Section Size:%d, Downloaded Size:%d", percent, data_len);

					percent = ((data_len * 100)/percent);
					if (otau_profile_instance->app_cb.otau_progress_cb != NULL)
					{
						otau_profile_instance->app_cb.otau_progress_cb(page_data_notify_req->section_id, percent);
					}
					
					DBG_OTAU("OTAU Upgrading Section->%d, Completed->%d%%", page_data_notify_req->section_id, percent);		
					
					if(otau_send_indication(otau_gatt_service.conn_hanle,
					otau_gatt_service.chars[OTAU_INDICATION_CHAR_IDX].char_val_handle,
					(uint8_t *)&page_data_notify_resp,
					sizeof(page_data_notify_resp_t)) == AT_BLE_SUCCESS)
					{
						DBG_OTAU("Page Data Write Response");
						status = AT_BLE_SUCCESS;
						otau_process_status.status = AT_OTAU_CMD_SUCCESS;
					}
				}
				else
				{
					DBG_OTAU("Page Data Req: meta data update failed");
				}				
			}
			else
			{
				DBG_OTAU("Page Data Req: page write failed");
			}
		}
		else
		{
			DBG_OTAU("Page Data Req: page address incorrect");
		}
		
		if(status != AT_BLE_SUCCESS)
		{
			page_data_failure_resp_t page_data_notify_failure; //For incorrect response
			page_data_notify_failure.page_no = page_data_notify_req->page_no;
			page_data_notify_failure.resp.cmd = AT_OTAU_IMAGE_PAGE_INFO_ERROR;
			page_data_notify_failure.resp.length = (sizeof(page_data_notify_resp_t) - sizeof(page_data_notify_failure.resp.length));
			page_data_notify_failure.section_id = page_data_notify_req->section_id;
			
			otau_send_indication(otau_gatt_service.conn_hanle,
			otau_gatt_service.chars[OTAU_INDICATION_CHAR_IDX].char_val_handle,
			(uint8_t *)&page_data_notify_failure,
			sizeof(page_data_failure_resp_t));
			DBG_OTAU("Page Data Req: failure response");
		}
	}
	else
	{
		DBG_OTAU("Page Data Req: meta data read failed");
	}
	return status;
}

/** @brief otau_block_data_request_handler page data will be divided into block size 
 *			and sent by OTAU manager to OTAU target. Block data 
 *			will be stored into the Flash memory of OTAU image section
 *
 *	@param[in] params of type block_data_notify_request_t 
 *  @return	  returns AT_BLE_SUCCESS block data is correct or at_ble_err_status_t in case of failure
 */
at_ble_status_t otau_block_data_request_handler(void *params)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	block_data_notify_request_t *block_data_notify_request = NULL;
	
	OTAU_CHECK_NULL(params);
	block_data_notify_resp_t block_data_resp;
	block_data_failure_resp_t block_failure_resp;
	block_data_notify_request = (block_data_notify_request_t *)params;
	
	ALL_UNUSED(block_data_notify_request);
	ALL_UNUSED(block_data_resp);
	ALL_UNUSED(block_failure_resp);
	
	return status;
}

/** @brief otau_abort_handler OTAU abort request will be sent by OTAU manager to OTAU target
 *
 *	@param[in] params of type 
 *  @return	  returns AT_BLE_SUCCESS otau abort request is correct or at_ble_err_status_t in case of failure
 */
at_ble_status_t otau_abort_handler(void *params)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	
	OTAU_CHECK_NULL(params);
	
	return status;
}

/** @brief otau_pause_update_process OTAU pause request will be sent by Application to OTAU Manager
 *			in case of Application needs to pause the image download
 *
 *	@param[in] params of type 
 *  @return	  returns AT_BLE_SUCCESS otau pause request is correct and send to OTAU manager or at_ble_err_status_t in case of failure
 */
at_ble_status_t otau_pause_update_process(void *params)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	otau_pause_process_t pause_req;

	/* Send Pause command to OTAU manager */
	pause_req.req.cmd = AT_OTAU_PAUSE_OTAU_REQUEST;

		
	if(otau_send_indication(otau_gatt_service.conn_hanle,
	otau_gatt_service.chars[OTAU_INDICATION_CHAR_IDX].char_val_handle,
	(uint8_t *)&pause_req,
	sizeof(otau_pause_process_t)) == AT_BLE_SUCCESS)
	{
		DBG_OTAU("Pause OTAU process request sent");
		status = AT_BLE_SUCCESS;			
	}	

	return status;
}

/** @brief otau_resume_update_process OTAU resume request will be sent by Application to OTAU Manager
 *			in case of Application needs to resume the image download
 *
 *	@param[in] params of type 
 *  @return	  returns AT_BLE_SUCCESS otau resume request is correct and send to OTAU manager or at_ble_err_status_t in case of failure
 */
at_ble_status_t otau_resume_update_process(void *params)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	image_meta_data_t meta_data;
	otau_resume_process_t resume_req;
	
	if(ofm_read_meta_data((void *)&meta_data, OTAU_IMAGE_META_DATA_ID) == AT_BLE_SUCCESS)
	{
		/* Send resume command with OTAU FW information */
		resume_req.req.cmd = AT_OTAU_RESUME_OTAU_REQUEST;
		
		/* Read the device Information and send to OTAU manager */
		memcpy((uint8_t *)&resume_req.dev_info, (uint8_t *)&meta_data.dev_info, sizeof(device_info_t));
		
		if(otau_send_indication(otau_gatt_service.conn_hanle,
		otau_gatt_service.chars[OTAU_INDICATION_CHAR_IDX].char_val_handle,
		(uint8_t *)&resume_req,
		sizeof(otau_resume_process_t)) == AT_BLE_SUCCESS)
		{
			DBG_OTAU("Pause OTAU process request sent");
			status = AT_BLE_SUCCESS;
			/* Restore the previous state here */
			otau_process_status.cstate = otau_process_status.pstate;
		}
	}
	return status;
}

/** @brief otau_pause_request_handler OTAU pause request will be sent by OTAU manager to OTAU target
 *			in case of OTAU needs to be paused by OTAU manager
 *
 *	@param[in] params of type 
 *  @return	  returns AT_BLE_SUCCESS otau abort request is correct or at_ble_err_status_t in case of failure
 */
at_ble_status_t otau_pause_request_handler(void *params)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	otau_pause_process_t *pause_req = NULL;
	
	OTAU_CHECK_NULL(params);
	pause_req = (otau_pause_process_t *)params;
	
	if (pause_req->req.cmd == AT_OTAU_PAUSE_OTAU_REQUEST)
	{		
		otau_pause_resp_t pause_response;		
		image_meta_data_t meta_data;
		
		pause_response.resp.cmd = AT_OTAU_PAUSE_OTAU_RESP;
		
		if(ofm_read_meta_data((void *)&meta_data, OTAU_IMAGE_META_DATA_ID) == AT_BLE_SUCCESS)
		{
			uint32_t data_len = 0;
			uint32_t percent = 0;
			uint16_t resume_block = 0;
			uint16_t resume_page = 0;
			section_id_t resume_section_id = 0;
			
			if (otau_download_status(&resume_page, &resume_block, &resume_section_id, false) == AT_BLE_SUCCESS)
			{
				DBG_OTAU("Pause at section id:%d", resume_section_id);
				if (resume_section_id == 0x03)
				{
					/* Check the percentage */
					memcpy((uint8_t *)&data_len, meta_data.app_downloaded_info.size, 3);
					memcpy((uint8_t *)&percent, meta_data.app_section.size, 3);
					percent = ((data_len * 100)/percent);
					if (percent > 98)
					{
						pause_response.resp.cmd = AT_OTAU_CMD_FAILED;
						DBG_OTAU("Pause Declined");
					}
				}				
			}						
		}
				
		if(otau_send_indication(otau_gatt_service.conn_hanle,
		otau_gatt_service.chars[OTAU_INDICATION_CHAR_IDX].char_val_handle,
		(uint8_t *)&pause_response,
		sizeof(otau_pause_resp_t)) == AT_BLE_SUCCESS)
		{	
			DBG_OTAU("OTAU Pause Response Sent");
			/* Update only Pause State here */
			if (pause_response.resp.cmd == AT_OTAU_PAUSE_OTAU_RESP)
			{
				otau_process_status.pstate = otau_process_status.cstate;
				otau_process_status.cstate = OTAU_PAUSE_STATE;				
			}
			status = AT_BLE_SUCCESS;			
		}
	}
	
	
	return status;
}

at_ble_status_t otau_pause_resp_handler(void *params)
{
	at_ble_status_t status = AT_BLE_FAILURE;		
	otau_pause_resp_t *pause_response = NULL;
	
	OTAU_CHECK_NULL(params);
	pause_response = (otau_pause_resp_t *)params;
	
	if(pause_response->resp.cmd == AT_OTAU_PAUSE_OTAU_RESP)
	{
		/* Update only Pause State here */
		otau_process_status.pstate = otau_process_status.cstate;
		otau_process_status.cstate = OTAU_PAUSE_STATE;
		status = AT_BLE_SUCCESS;
	}	
	return status;
}

/** @brief otau_resume_request_handler OTAU resume request will be sent by OTAU manager to OTAU target
 *			in case of OTAU needs to be resumed by OTAU manager
 *
 *	@param[in] params of type 
 *  @return	  returns AT_BLE_SUCCESS otau abort request is correct or at_ble_err_status_t in case of failure
 */
at_ble_status_t otau_resume_request_handler(void *params)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	otau_resume_req_t *resume_request = NULL;
	
	OTAU_CHECK_NULL(params);
	resume_request = (otau_resume_req_t *)params;
	
	if (resume_request->req.cmd == AT_OTAU_RESUME_OTAU_REQUEST)
	{
		image_meta_data_t meta_data;
		otau_resume_resp_t resume_resp;
		
		if(ofm_read_meta_data((void *)&meta_data, OTAU_IMAGE_META_DATA_ID) == AT_BLE_SUCCESS)
		{
			/* Send Resume response command with OTAU FW information */
			resume_resp.resp.cmd = AT_OTAU_RESUME_OTAU_RESP;
			
			/* Read the device Information and send to OTAU manager */
			memcpy((uint8_t *)&resume_resp.dev_info, (uint8_t *)&meta_data.dev_info, sizeof(device_info_t));
			
			if(otau_send_indication(otau_gatt_service.conn_hanle,
			otau_gatt_service.chars[OTAU_INDICATION_CHAR_IDX].char_val_handle,
			(uint8_t *)&resume_resp,
			sizeof(otau_resume_process_t)) == AT_BLE_SUCCESS)
			{
				DBG_OTAU("Resume OTAU Response sent");
				/* Update only Pause State here */
				otau_process_status.cstate = OTAU_IMAGE_RESUME_STATE;
				otau_process_status.pstate = OTAU_IMAGE_RESUME_STATE;
				otau_process_status.status = AT_OTAU_CMD_SUCCESS;
				status = AT_BLE_SUCCESS;
			}
		}
	}		
	return status;
}

/* Not implemented at mobile application, Response to resume req will triggered as image notification */
at_ble_status_t otau_resume_resp_handler(void *params)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	
	OTAU_CHECK_NULL(params);
	
	return status;
}

/** @brief otau_update_firmware_version OTAU image firmware version update required before switching to new firmware version
 *			It must be called after successfully completed the image download and verified the CRC
 *
 *	@param[in] params unused
 *  @return	  returns AT_BLE_SUCCESS OTAU image firmware version updated successfully or at_ble_err_status_t in case of failure
 */
at_ble_status_t otau_update_firmware_version(void *params)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	
	/* Read the Firmware version from meta data */
	image_meta_data_t meta_data;
	if(ofm_read_meta_data((void *)&meta_data, OTAU_IMAGE_META_DATA_ID) == AT_BLE_SUCCESS)
	{
		uint32_t firmware_version = 0x00000000;
		uint32_t idx;
		
		firmware_version = (uint32_t)(meta_data.dev_info.fw_version.major_number << 24);
		firmware_version |= (uint32_t)(meta_data.dev_info.fw_version.minor_number << 16);
		firmware_version |= (uint32_t)(meta_data.dev_info.fw_version.build_number);
		
		/* Update the Firmware version to all OTAU section image */		
		for ( idx = 0; idx < meta_data.dev_info.total_sections; ++idx)
		{
			uint32_t read_addr = 0xFFFFFFFF;
			uint32_t read_size = 0;
			/* get the address */
			/* Check the section id */
			if(meta_data.patch_section.section_id == (idx+1))
			{
				if((meta_data.section_image_id & SECTION1_IMAGE_IDENTIFIER_MASK) == SECTION1_BOTTOM_IMAGE_IDENTIFIER)
				{
					/* Calculate the address */
					read_addr = meta_data.patch_section.start_address;
					read_addr += (dev_flash_info.section_info[idx].size/2);
				}
				else if((meta_data.section_image_id & SECTION1_IMAGE_IDENTIFIER_MASK) == SECTION1_TOP_IMAGE_IDENTIFIER)
				{
					read_addr = meta_data.patch_section.start_address;
				}
				memcpy((uint8_t *)&read_size, meta_data.patch_downloaded_info.size, 3);
			}
			else if (meta_data.app_header_section.section_id == (idx+1))
			{
				if((meta_data.section_image_id & SECTION2_IMAGE_IDENTIFIER_MASK) == SECTION2_BOTTOM_IMAGE_IDENTIFIER)
				{
					/* Calculate the address */
					read_addr = meta_data.app_header_section.start_address;
					read_addr += (dev_flash_info.section_info[idx].size/2);
				}
				else if((meta_data.section_image_id & SECTION2_IMAGE_IDENTIFIER_MASK) == SECTION2_TOP_IMAGE_IDENTIFIER)
				{
					read_addr = meta_data.app_header_section.start_address;
				}
				memcpy((uint8_t *)&read_size, meta_data.app_header_section.size, 3);
			}
			else if (meta_data.app_section.section_id == (idx+1))
			{
				if((meta_data.section_image_id & SECTION3_IMAGE_IDENTIFIER_MASK) == SECTION3_BOTTOM_IMAGE_IDENTIFIER)
				{
					/* Calculate the address */
					read_addr = meta_data.app_section.start_address;
					read_addr += (dev_flash_info.section_info[idx].size/2);
				}
				else if((meta_data.section_image_id & SECTION3_IMAGE_IDENTIFIER_MASK) == SECTION3_TOP_IMAGE_IDENTIFIER)
				{
					read_addr = meta_data.app_section.start_address;
				}
				memcpy((uint8_t *)&read_size, meta_data.app_section.size, 3);
			}						
			
			if (read_size)
			{
				uint32_t offset_addr = 0;
				uint8_t page_buf[dev_flash_info.page_size];
				image_crc_t crc = IMAGE_CRC32_POLYNOMIAL;
				
				/* Read Actual Section size */
				if(ofm_read_page((idx+1), (read_addr+sizeof(firmware_version)), page_buf, sizeof(read_size)) == AT_BLE_SUCCESS)
				{
					memcpy((uint8_t *)&read_size, page_buf, sizeof(read_size));
					read_size += sizeof(image_crc_t);
					read_size += sizeof(read_size);
					read_size += sizeof(firmware_version);
					DBG_OTAU("Actual Firmware size: %4X", read_size);
				}						
				
				/* Update the Firmware version */
				if(ofm_read_page((idx+1), (read_addr+offset_addr), page_buf, dev_flash_info.page_size) == AT_BLE_SUCCESS)
				{
					/* Initial 4Bytes are Firmware and update the version here */
					memcpy(page_buf, (uint8_t *)&firmware_version, sizeof(firmware_version));
					
					if(ofm_write_page((idx+1), (read_addr+offset_addr), page_buf, dev_flash_info.page_size) == AT_BLE_SUCCESS)
					{					
						/* Calculate the CRC for Entire section of Image excluding the CRC at the end of the section */						
						crc = flash_crc32_compute(read_addr, (read_size - sizeof(image_crc_t)), (idx+1), true, crc);						
					
						uint8_t crca[sizeof(image_crc_t)] = {0,0,0,0 };
						if(ofm_read_page((idx+1), (read_addr+(read_size - sizeof(image_crc_t))), crca, sizeof(image_crc_t)) == AT_BLE_SUCCESS)
						{
							DBG_OTAU("FW Switch Actual CRC:%2X%2X%2X%2X", crca[0],crca[1],crca[2],crca[3]);
						}	
						
						uint32_t fw_version = 0x0;
						if(ofm_read_page((idx+1), (read_addr), (uint8_t *)&fw_version, sizeof(fw_version)) == AT_BLE_SUCCESS)
						{
							DBG_OTAU("FW Version Wrote:%4X", fw_version);
						}
						
						DBG_OTAU("FW Switch CRC:%4X", crc);				
						
						/* Update the CRC */
						memcpy(page_buf, (uint8_t *)&crc, sizeof(image_crc_t));
						if(ofm_write_page((idx+1), (read_addr+(read_size - sizeof(image_crc_t))), page_buf, sizeof(image_crc_t)) != AT_BLE_SUCCESS)
						{
							DBG_OTAU("Failed to Update the CRC");
						}
						
											
					}
					
				}
			}
		} /* Loop until Total sections */
	} /* meta data is read and correct */

	return status;
}

/** @brief otau_image_switch_request_handler OTAU image switch request will be sent by the OTAU Manager
 *			after successfully completing the image download and verified the CRC
 *
 *	@param[in] params of type image_switch_notify_request_t
 *  @return	  returns AT_BLE_SUCCESS image switch request processed successfully or at_ble_err_status_t in case of failure
 */
at_ble_status_t otau_image_switch_request_handler(void *params)
{
	at_ble_status_t status = AT_BLE_FAILURE;	
	image_switch_notify_request_t *image_switch_notify_req = NULL;
	uint32_t fw_version;
	bool permission = false;
	
	OTAU_CHECK_NULL(params);
	image_switch_notify_req = (image_switch_notify_request_t *)params;
	
	/* Validate the incoming frame */
	if (image_switch_notify_req->req.length != (sizeof(image_switch_notify_request_t)-OTAU_FRAME_LEN_SIZE))
	{
		otau_invalid_frame_resp(NULL);
		return status;
	}
	
	OTAU_CHECK_ERROR(otau_state_machine_controller(OTAU_IMAGE_SWITCH_STATE));
	
	memcpy_nreverse((uint8_t *)&fw_version, (uint8_t *)&image_switch_notify_req->fw_version, sizeof(fw_version));
	
	if (otau_profile_instance->app_cb.otau_image_switch != NULL)
	{
		otau_profile_instance->app_cb.otau_image_switch(&image_switch_notify_req->fw_version, &permission);
	}
	else
	{
		permission = true;
	}
	
	if((image_switch_notify_req->req.cmd == AT_OTAU_IMAGE_SWITCH_REQUEST) && (permission))
	{		
		DBG_OTAU("Image switch request received");
			
				
		image_meta_data_t meta_data;
		if(ofm_read_meta_data((void *)&meta_data, OTAU_IMAGE_META_DATA_ID) == AT_BLE_SUCCESS)
		{
			if ((meta_data.dev_info.fw_version.major_number == image_switch_notify_req->fw_version.major_number) &&
			(meta_data.dev_info.fw_version.minor_number == image_switch_notify_req->fw_version.minor_number) &&
			(meta_data.dev_info.fw_version.build_number == image_switch_notify_req->fw_version.build_number) &&
			(meta_data.dev_info.total_sections == image_switch_notify_req->total_sections))
			{
				image_switch_notify_resp_t image_switch_notify_resp;//Success Response
				
				/* Update the Firmware version into Section's */
				otau_update_firmware_version(NULL);
				
				
				image_switch_notify_resp.total_sections = meta_data.dev_info.total_sections;
				
				image_switch_notify_resp.resp.cmd = AT_OTAU_IMAGE_SWITCH_RESP;
				image_switch_notify_resp.resp.length = (sizeof(image_switch_notify_resp_t) - sizeof(image_switch_notify_resp.resp.length));
				if(otau_send_indication(otau_gatt_service.conn_hanle,
				otau_gatt_service.chars[OTAU_INDICATION_CHAR_IDX].char_val_handle,
				(uint8_t *)&image_switch_notify_resp,
				sizeof(image_switch_notify_resp_t)) == AT_BLE_SUCCESS)
				{
					DBG_OTAU("Image switch response...!!!");
					status = AT_BLE_SUCCESS;
					otau_process_status.status = AT_OTAU_CMD_SUCCESS;
					OTAU_CHECK_ERROR(otau_state_machine_controller(OTAU_IMAGE_SWITCH_CONFIRM_STATE));
					
					if(at_ble_disconnect(otau_gatt_service.conn_hanle, AT_BLE_TERMINATED_BY_USER) == AT_BLE_SUCCESS)
					{
						otau_device_reset_request_t	device_reset_req;
						
						device_reset_req.req.cmd = AT_OTAU_RESET_DEVICE;
						device_reset_req.req.length = (sizeof(otau_device_reset_request_t) - sizeof(device_reset_req.req.length));
						device_reset_req.fw_version.major_number = image_switch_notify_req->fw_version.major_number;
						device_reset_req.fw_version.minor_number = image_switch_notify_req->fw_version.minor_number;
						device_reset_req.fw_version.build_number = image_switch_notify_req->fw_version.build_number;		
						/* Post custom event to BLE Manager */	
						otau_process_status.status = AT_OTAU_CMD_SUCCESS;					
						if(at_ble_event_user_defined_post((void *)&device_reset_req) ==  AT_BLE_SUCCESS)
						{
							DBG_OTAU("Requesting to reboot the device...!!!");
							DBG_OTAU("Switching to New Firmware version");
						}					
					}
				}
			}
		}
	}
	
	if (status != AT_BLE_SUCCESS)
	{
		image_switch_failure_resp_t image_switch_failure_resp;//Failure Response
		image_switch_failure_resp.resp.cmd = AT_OTAU_FAILURE;
		image_switch_failure_resp.error = AT_OTAU_IMAGE_SWITCH_ERROR;
		image_switch_failure_resp.resp.length = (sizeof(image_switch_failure_resp_t) - sizeof(image_switch_failure_resp.resp.length));
		
		otau_send_indication(otau_gatt_service.conn_hanle,
		otau_gatt_service.chars[OTAU_INDICATION_CHAR_IDX].char_val_handle,
		(uint8_t *)&image_switch_failure_resp,
		sizeof(image_switch_failure_resp_t));
		DBG_OTAU("Image switch failed");
	}
	
	return status;
}

/** @brief otau_image_end_request_handler OTAU image end request will be sent by the OTAU Manager
 *			after successfully completing the image download and to verify the complete image CRC
 *
 *	@param[in] params of type image_end_notify_request_t
 *  @return	  returns AT_BLE_SUCCESS in case of the Total image CRC is correct or at_ble_err_status_t in case of failure
 */
at_ble_status_t otau_image_end_request_handler(void *params)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	image_end_notify_request_t *image_end_notify_req = NULL;
	
	OTAU_CHECK_NULL(params);
	image_end_notify_req = (image_end_notify_request_t *)params;
	
	/* Validate the incoming frame */
	if (image_end_notify_req->req.length != (sizeof(image_end_notify_request_t)-OTAU_FRAME_LEN_SIZE))
	{
		otau_invalid_frame_resp(NULL);
		return status;
	}
	
	OTAU_CHECK_ERROR(otau_state_machine_controller(OTAU_IMAGE_END_STATE));
	
	if(calculate_total_image_crc(image_end_notify_req->total_sections, 
								image_end_notify_req->total_image_crc,
								image_end_notify_req->total_image_size) == AT_BLE_SUCCESS)
	{		
		image_end_notify_resp_t image_end_resp; //For correct response
		image_end_resp.total_image_crc = image_end_notify_req->total_image_crc;
		image_end_resp.total_image_size = image_end_notify_req->total_image_size;
		image_end_resp.total_sections = image_end_notify_req->total_sections;
		image_end_resp.resp.cmd = AT_OTAU_IMAGE_END_NOTIFY_RESP;
		image_end_resp.resp.length = (sizeof(image_end_notify_resp_t) -2);
		if(otau_send_indication(otau_gatt_service.conn_hanle,
		otau_gatt_service.chars[OTAU_INDICATION_CHAR_IDX].char_val_handle,
		(uint8_t *)&image_end_resp,
		sizeof(image_end_notify_resp_t)) == AT_BLE_SUCCESS)
		{
			DBG_OTAU("Complete OTAU Image verified");
			DBG_OTAU("OTA Upgrade completed and Verified...!!!");
			status = AT_BLE_SUCCESS;
			otau_process_status.status = AT_OTAU_CMD_SUCCESS;
			OTAU_CHECK_ERROR(otau_state_machine_controller(OTAU_IMAGE_END_CONFIRM_STATE));
			otau_process_status.status = AT_OTAU_CMD_SUCCESS;
		}
	}
	else
	{
		image_end_failure_resp_t image_end_failure_resp;//Failure Response
		image_end_failure_resp.resp.cmd = AT_OTAU_FAILURE;
		image_end_failure_resp.error = AT_OTAU_IMAGE_VERIFICATION_FAILED;
		image_end_failure_resp.resp.length = (sizeof(image_end_failure_resp_t) -2);
		
		otau_send_indication(otau_gatt_service.conn_hanle,
		otau_gatt_service.chars[OTAU_INDICATION_CHAR_IDX].char_val_handle,
		(uint8_t *)&image_end_failure_resp,
		sizeof(image_end_failure_resp_t));
		DBG_OTAU("Image End Req:verification failed");
	}	
	return status;
}

/** @brief calculate_total_image_crc function will calculate the whole downloaded 
 *			image crc based on the sections present as part of the current OTAU
 *
 *  @param[in] total_sections total no of sections for calculating the CRC
 *  @param[in] total_crc total crc of the downloaded image
 *  @param[in] total_size total downloaded image size
 *	@return AT_BLE_SUCCESS in case the CRC's are matched or AT_BLE_FAILURE
 *
 */
static at_ble_status_t calculate_total_image_crc(uint8_t total_sections, image_crc_t total_crc, uint32_t total_size)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	/* Read meta data */
	image_meta_data_t meta_data;
	uint32_t idx;
	
	if(ofm_read_meta_data((void *)&meta_data, OTAU_IMAGE_META_DATA_ID) == AT_BLE_SUCCESS)
	{		
		image_crc_t crc = IMAGE_CRC32_POLYNOMIAL;
		for ( idx = 0; idx < total_sections; ++idx)
		{
			uint32_t read_addr = 0xFFFFFFFF;
			uint32_t read_size = 0;
			/* get the address */
			/* Check the section id */
			if(meta_data.patch_section.section_id == (idx+1))
			{							
				if((meta_data.section_image_id & 0x0F) == 0x1)
				{
					/* Calculate the address */
					read_addr = meta_data.patch_section.start_address;
					read_addr += (dev_flash_info.section_info[idx].size/2);
				}
				else if((meta_data.section_image_id & 0x0F) == 0x0)
				{
					read_addr = meta_data.patch_section.start_address;
				}
				memcpy((uint8_t *)&read_size, meta_data.patch_downloaded_info.size, 3);
			}
			else if (meta_data.app_header_section.section_id == (idx+1))
			{				
				if((meta_data.section_image_id & 0xF0) == 0x10)
				{
					/* Calculate the address */
					read_addr = meta_data.app_header_section.start_address;
					read_addr += (dev_flash_info.section_info[idx].size/2);
				}
				else if((meta_data.section_image_id & 0xF0) == 0x00)
				{
					read_addr = meta_data.app_header_section.start_address;
				}
				memcpy((uint8_t *)&read_size, meta_data.app_header_section.size, 3);
			}
			else if (meta_data.app_section.section_id == (idx+1))
			{				
				if((meta_data.section_image_id & 0xF00) == 0x100)
				{
					/* Calculate the address */
					read_addr = meta_data.app_section.start_address;
					read_addr += (dev_flash_info.section_info[idx].size/2);					
				}
				else if((meta_data.section_image_id & 0xF00) == 0x000)
				{
					read_addr = meta_data.app_section.start_address;
				}
				memcpy((uint8_t *)&read_size, meta_data.app_section.size, 3);
			}
		
			DBG_OTAU("Image End: Read Addr->%6X, Read size->%6X", read_addr, read_size);
			if (read_size)
			{
				uint32_t offset_addr = 0;
				uint8_t page_buf[dev_flash_info.page_size];
				while (read_size != 0)
				{					
					if (read_size >= (dev_flash_info.page_size))
					{
						if(ofm_read_page((idx+1), (read_addr+offset_addr), page_buf, dev_flash_info.page_size) == AT_BLE_SUCCESS)
						{
							crc = crc32_compute(page_buf, dev_flash_info.page_size,  true, crc);
						}
						offset_addr += dev_flash_info.page_size;
						read_size -= dev_flash_info.page_size;
					}
					else
					{
						if(ofm_read_page((idx+1), (read_addr+offset_addr), page_buf, read_size) == AT_BLE_SUCCESS)
						{
							crc = crc32_compute(page_buf, read_size,  true, crc);
						}
						read_size = 0;
					}				
				}				
			}
			else
			{
				DBG_OTAU("Image end: Invalid Read size");
			}			
		}
		if (total_crc == crc)
		{
			DBG_OTAU("CRC is correct");
			status = AT_BLE_SUCCESS;
		}
	}
	else
	{
		DBG_OTAU("Image End: Meta data read failed");
	}
	
	return status;
}

/** @brief otau_missed_block_resp_handler function will handle the missed block
 *
 *  @param[in] params
 *	@return AT_BLE_SUCCESS in case of success or AT_BLE_FAILURE
 *
 */
at_ble_status_t otau_missed_block_resp_handler(void *params)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	
	OTAU_CHECK_NULL(params);
	
	return status;
}

/** @brief otau_page_end_notify_request_handler page end notification will be received
 *			in block mode, once the complete page is sent by OTAU Manager
 *  @param[in] params
 *	@return AT_BLE_SUCCESS in case of success or AT_BLE_FAILURE
 *
 */
at_ble_status_t otau_page_end_notify_request_handler(void *params)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	page_end_notify_request_t *page_end_notify_req;
	page_end_notify_resp_t page_end_resp;
	page_end_failure_resp_t page_end_failure_response;
	
	OTAU_CHECK_NULL(params);
	page_end_notify_req = (page_end_notify_request_t *)params;
	
	/* Validate the incoming frame */
	if (page_end_notify_req->req.length != (sizeof(page_end_notify_request_t)-OTAU_FRAME_LEN_SIZE))
	{
		otau_invalid_frame_resp(NULL);
		return status;
	}
	
	ALL_UNUSED(page_end_notify_req);
	ALL_UNUSED(page_end_failure_response);
	ALL_UNUSED(page_end_resp);
	
	return status;
}

/** @brief flash_crc32_compute calculate a checksum for given flash memory address and length
 *			if resume is true then crc32 will be calculated from previously calculated crc32 value.
 *			This function will read from the flash memory and calculates the CRC
 *	@param[in] read_addr Flash memory address for CRC calculation
 *	@param[in] len length of the buffer
 *	@param[in] section_id section id of the flash memory
 *	@param[in] resume if true it will calculate the crc from previously calculated crc value 
 *	@param[in] crc previously computed crc value
 *
 *  @return	calculated crc32 for given data and len
 */
image_crc_t flash_crc32_compute(uint32_t read_addr, uint32_t len, section_id_t section_id, 
								bool resume, image_crc_t crc)
{
	uint8_t page_buf[dev_flash_info.page_size];
	
	if (!resume)
	{
		crc = IMAGE_CRC32_POLYNOMIAL;
	}
	
	while (len != 0)
	{
		DBG_OTAU("Section End:Read size->0x%4X", len);
		if (len >= (dev_flash_info.page_size))
		{												
			if(ofm_read_page(section_id, read_addr, page_buf, dev_flash_info.page_size) == AT_BLE_SUCCESS)
			{
					crc = crc32_compute(page_buf, dev_flash_info.page_size,  true, crc);
			}
			read_addr += dev_flash_info.page_size;
			len -= dev_flash_info.page_size;
		}
		else
		{					
			if(ofm_read_page(section_id, read_addr, page_buf, len) == AT_BLE_SUCCESS)
			{						  
				crc = crc32_compute(page_buf, len,  true, crc);
			}
			read_addr += len;
			len = 0;
		}				
	}
	return crc;
}

/** @brief otau_section_end_request_handler section end will be sent by the OTAU manager 
 *			to OTAU target at the each end of the section. Section end will be used to verify the CRC 
 *			of the each section before move to another section download
 *  @param[in] params of type section_end_notify_request_t
 *	@return AT_BLE_SUCCESS in case of success or at_ble_err_status_t in case of failure
 *
 */
at_ble_status_t otau_section_end_request_handler(void *params)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	section_end_notify_request_t *section_end_req = NULL;
	
	OTAU_CHECK_NULL(params);
	section_end_req = (section_end_notify_request_t *)params;	
	/* Read meta data */
	image_meta_data_t meta_data;
	uint32_t read_addr = 0xFFFFFFFF;
	uint32_t read_size = 0;
	image_crc_t crc_wrote = IMAGE_CRC32_POLYNOMIAL;
	
	/* Validate the incoming frame */
	if (section_end_req->req.length != (sizeof(section_end_notify_request_t)-OTAU_FRAME_LEN_SIZE))
	{
		otau_invalid_frame_resp(NULL);
		return status;
	}
	
	OTAU_CHECK_ERROR(otau_state_machine_controller(OTAU_SECTION_END_STATE));
	
	if(ofm_read_meta_data((void *)&meta_data, OTAU_IMAGE_META_DATA_ID) == AT_BLE_SUCCESS)
	{
		/* get the address */
		/* Check the section id of the OTAU */
		if(meta_data.patch_section.section_id == section_end_req->section_id)
		{						
			if((meta_data.section_image_id & SECTION1_IMAGE_IDENTIFIER_MASK) == SECTION1_BOTTOM_IMAGE_IDENTIFIER)
			{
				/* Calculate the address */
				read_addr = meta_data.patch_section.start_address;
				read_addr += (dev_flash_info.section_info[section_end_req->section_id-1].size/2);
			}
			else if((meta_data.section_image_id & SECTION1_IMAGE_IDENTIFIER_MASK) == SECTION1_TOP_IMAGE_IDENTIFIER)
			{
				read_addr = meta_data.patch_section.start_address;
			}
			memcpy((uint8_t *)&read_size, meta_data.patch_downloaded_info.size, 3);
			crc_wrote = meta_data.patch_crc;
			
		}
		else if (meta_data.app_header_section.section_id == section_end_req->section_id)
		{			
			if((meta_data.section_image_id & SECTION2_IMAGE_IDENTIFIER_MASK) == SECTION2_BOTTOM_IMAGE_IDENTIFIER)
			{
				/* Calculate the address */
				read_addr = meta_data.app_header_section.start_address;
				read_addr += (dev_flash_info.section_info[section_end_req->section_id-1].size/2);
			}
			else if((meta_data.section_image_id & SECTION2_IMAGE_IDENTIFIER_MASK) == SECTION2_TOP_IMAGE_IDENTIFIER)
			{
				read_addr = meta_data.app_header_section.start_address;
			}
			memcpy((uint8_t *)&read_size, meta_data.app_hdr_downloaded_info.size, 3);
			crc_wrote = meta_data.app_header_crc;
		}
		else if (meta_data.app_section.section_id == section_end_req->section_id)
		{			
			if((meta_data.section_image_id & SECTION3_IMAGE_IDENTIFIER_MASK) == SECTION3_BOTTOM_IMAGE_IDENTIFIER)
			{
				/* Calculate the address */
				read_addr = meta_data.app_section.start_address;
				read_addr += (dev_flash_info.section_info[section_end_req->section_id-1].size/2);
			}
			else if((meta_data.section_image_id & SECTION3_IMAGE_IDENTIFIER_MASK) == SECTION3_TOP_IMAGE_IDENTIFIER)
			{
				read_addr = meta_data.app_section.start_address;
			}
			memcpy((uint8_t *)&read_size, meta_data.app_downloaded_info.size, 3);
			crc_wrote = meta_data.app_crc;
		}
		DBG_OTAU("Section End CRC: Read Addr->%6X, Read size->0x%8X", read_addr, read_size);
		if (read_size)
		{
				image_crc_t crc = IMAGE_CRC32_POLYNOMIAL;
				crc = flash_crc32_compute(read_addr, read_size, section_end_req->section_id, false, crc);				
			
				if (crc == crc_wrote)
				{
					DBG_OTAU("Section end: CRC is correct");
					section_end_notify_resp_t section_end_notify_res;
					section_end_notify_res.resp.cmd = AT_OTAU_IMAGE_SECTION_END_NOTIFY_RESP;
					section_end_notify_res.resp.length = (sizeof(section_end_notify_resp_t) - section_end_notify_res.resp.length);
					section_end_notify_res.section_id = section_end_req->section_id;
								
					if(otau_send_indication(otau_gatt_service.conn_hanle,
					otau_gatt_service.chars[OTAU_INDICATION_CHAR_IDX].char_val_handle,
					(uint8_t *)&section_end_notify_res,
					sizeof(section_end_notify_resp_t)) == AT_BLE_SUCCESS)
					{
						DBG_OTAU("Section Verified!!!");
						DBG_OTAU("OTAU Section completed, Section ID:%d", section_end_req->section_id);
						status = AT_BLE_SUCCESS;
						otau_process_status.status = AT_OTAU_CMD_SUCCESS;
						OTAU_CHECK_ERROR(otau_state_machine_controller(OTAU_SECTION_END_CONFIRM_STATE));
						otau_process_status.status = AT_OTAU_CMD_SUCCESS;
					}
				}
				else
				{
					DBG_OTAU("Section end: CRC Verification failed, Original CRC->0x%8X, calculated crc->0x%8X", crc_wrote, crc);
					section_end_failure_resp_t section_end_failure;
					section_end_failure.resp.cmd = AT_OTAU_IMAGE_SECTION_END_ERROR;
					section_end_failure.resp.length = (sizeof(section_end_failure_resp_t) - sizeof(section_end_failure.resp.length));
					section_end_failure.section_id = section_end_req->section_id;
				
					otau_send_indication(otau_gatt_service.conn_hanle,
					otau_gatt_service.chars[OTAU_INDICATION_CHAR_IDX].char_val_handle,
					(uint8_t *)&section_end_failure,
					sizeof(section_end_failure_resp_t));
				}
			}
			else
			{
				DBG_OTAU("section end: Invalid Read size");
			}
			
		}
		else
		{
			DBG_OTAU("Section End: Meta data read failed");
		}
	
	return status;
}

/** @brief otau_invalid_frame_resp in case the frame basic checks are incorrect 
 *			i.e length or unsupported command
 *  @param[in] params not used
 *	@return AT_BLE_SUCCESS in case of response sent success or at_ble_err_status_t in case of failure
 *
 */
at_ble_status_t otau_invalid_frame_resp(void *params)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	otau_error_resp_t otau_general_error;
	otau_general_error.resp.cmd = AT_OTAU_FAILURE;
	otau_general_error.resp.length = (sizeof(otau_error_resp_t)-OTAU_FRAME_LEN_SIZE);
	otau_general_error.error = AT_OTAU_INVALID_COMMAND;
	
	if(otau_send_indication(otau_gatt_service.conn_hanle,
	otau_gatt_service.chars[OTAU_INDICATION_CHAR_IDX].char_val_handle,
	(uint8_t *)&otau_general_error,
	sizeof(otau_error_resp_t)) == AT_BLE_SUCCESS)
	{
		status = AT_BLE_SUCCESS;
	}
	
	ALL_UNUSED(params);	
	
	return status;
}

/** @brief otau_get_device_info_request_handler get device information will be requested by OTAU
 *			manager to OTAU target. OTAU manager will need to know about the device information before sending the
 *			otau image notification. This information may be used to identify the correct firmware file and 
 *			to check the version for upgrade. 
 *  @param[in] params of type get_device_info_request_t
 *	@return AT_BLE_SUCCESS in case of response sent success or at_ble_err_status_t in case of failure
 *
 */
at_ble_status_t otau_get_device_info_request_handler(void *params)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	get_device_info_request_t *device_info_req = NULL;
	device_info_resp_t device_info;
	
	OTAU_CHECK_NULL(params);
	device_info_req = (get_device_info_request_t *)params;
	
	/* Validate the incoming frame */
	if (device_info_req->length != (sizeof(get_device_info_request_t)-OTAU_FRAME_LEN_SIZE))
	{
		otau_invalid_frame_resp(NULL);
		return status;
	}
	
	if(ofm_get_device_info(&device_info.dev_info) == AT_BLE_SUCCESS)
	{
		uint16_t resume_page_no = 0;
		uint16_t resume_block_no = 0;
		section_id_t resume_section_id;
		
		device_info.resp.cmd = AT_OTAU_GET_DEVICE_INFO_RESP;
		device_info.resp.length = (sizeof(device_info_resp_t) - 2);
		
		device_info.otau_fw_version.major_number = 0xFF;
		device_info.otau_fw_version.minor_number = 0xFF;
		device_info.otau_fw_version.build_number = 0xFFFF;		
		
		if((otau_download_status(&resume_page_no, &resume_block_no, &resume_section_id, false)) == AT_BLE_SUCCESS)
		{
			/* Device needs to resume the firmware */
			image_meta_data_t meta_data;
			if(ofm_read_meta_data((void *)&meta_data, OTAU_IMAGE_META_DATA_ID) == AT_BLE_SUCCESS)
			{
				uint32_t otau_firmware_ver = 0;
				uint32_t device_firmware_ver = 0;
				
				otau_firmware_ver = (meta_data.dev_info.fw_version.major_number << 24);
				otau_firmware_ver |= (meta_data.dev_info.fw_version.minor_number << 16);
				otau_firmware_ver |= (meta_data.dev_info.fw_version.build_number);
				
				device_firmware_ver = (device_info.dev_info.fw_version.major_number << 24);
				device_firmware_ver |= (device_info.dev_info.fw_version.minor_number << 16);
				device_firmware_ver |= (device_info.dev_info.fw_version.build_number);
				/* Valid Firmware version needs to be present in the meta data */
				if ((otau_firmware_ver != 0xFFFFFFFF) && (otau_firmware_ver != 0x00000000) && (otau_firmware_ver > device_firmware_ver))
				{
					device_info.otau_fw_version.major_number = meta_data.dev_info.fw_version.major_number;
					device_info.otau_fw_version.minor_number = meta_data.dev_info.fw_version.minor_number;
					device_info.otau_fw_version.build_number = meta_data.dev_info.fw_version.build_number;
				}
			}			
		}

			
		if(otau_send_indication(otau_gatt_service.conn_hanle,
		otau_gatt_service.chars[OTAU_INDICATION_CHAR_IDX].char_val_handle,
		(uint8_t *)&device_info,
		sizeof(device_info_resp_t)) == AT_BLE_SUCCESS)
		{
			status = AT_BLE_SUCCESS;
			DBG_OTAU("OTAU:Device Info Response");
		}
	}
		
	return status;
}

/** @brief otau_char_changed_handler called by the ble manager after a
 *  change in the characteristic
 *  @param[in] at_ble_characteristic_changed_t which contains handle of
 *  characteristic, new value and connection handle
 *  @return AT_BLE_SUCCESS on success and AT_BLE_FAILURE on failure
 */
at_ble_status_t otau_char_changed_handler(void *params)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	at_ble_characteristic_changed_t *characteristic_changed = NULL;
	uint16_t new_cccd;
	
	OTAU_CHECK_NULL(params);
	characteristic_changed = (at_ble_characteristic_changed_t *)params;
	
	new_cccd = otau_characteritics_changed_handler(&otau_gatt_service, characteristic_changed);

	/* OTAU CCCD Write for Notification/Indication enabled or disabled */
	if ((new_cccd == CCCD_NOTIFICATION_INDICATION_DISABLED) || \
		(new_cccd == CCCD_NOTIFICATION_ENABLED) || \
		(new_cccd == CCCD_INDICATION_ENABLED))
	{
		/* OTAU CCCD Update status */
		status = characteristic_changed->status;
		if (new_cccd == CCCD_INDICATION_ENABLED)
		{
			ble_set_ulp_mode(BLE_ULP_MODE_CLEAR);
		}
		else
		{
			ble_set_ulp_mode(BLE_ULP_MODE_SET);
		}
		
	}
	
	if ((new_cccd == CCCD_WRITE_UNUSED) && (characteristic_changed->status == AT_BLE_SUCCESS))
	{
		if ((otau_gatt_service.chars[OTAU_WRITE_CHAR_IDX].char_val_handle == characteristic_changed->char_handle) && 
			 (otau_gatt_service.conn_hanle == characteristic_changed->conn_handle))
		{
			if(characteristic_changed->char_len >= sizeof(otau_req_t))
			{
				otau_req_t *otau_req = NULL;
				void *param = (void *)characteristic_changed->char_new_value;
				otau_req = (otau_req_t *)characteristic_changed->char_new_value;
				
				DBG_OTAU("OTAU Req cmd %d, char changed len->%d, offset->%d", otau_req->cmd, characteristic_changed->char_len, characteristic_changed->char_offset);
				
				switch(otau_req->cmd)
				{
					case AT_OTAU_IMAGE_NOTIFY_REQUEST:
					case AT_OTAU_FORCE_UDPATE_REQUEST:
					{
						/* Image (New/Old/Same) Notify Request */
						otau_image_notify_request_handler(param);
					}
					break;
					
					case AT_OTAU_IMAGE_INFO_NOTIFY_REQUEST:
					{
						at_ble_status_t status_info;						
						status_info = otau_image_info_request_handler(param);
						DBG_OTAU("Image info response:");
						(status_info == AT_BLE_SUCCESS) ? DBG_OTAU_CONT("success"):DBG_OTAU_CONT("failed");						
					}
					break;
					
					case AT_OTAU_PAGE_DATA_NOTIFY_REQUEST:
					{
						otau_page_data_request_handler(param);
					}
					break;
					
					case AT_OTAU_BLOCK_DATA_NOTIFY_REQUEST:
					{
						otau_block_data_request_handler(param);
					}
					break;
					
					case AT_OTAU_ABORT_OTAU:
					{
						otau_abort_handler(param);
					}
					break;
					
					case AT_OTAU_PAUSE_OTAU_REQUEST:
					{
						otau_pause_request_handler(param);
					}
					break;
					
					case AT_OTAU_PAUSE_OTAU_RESP:
					{
						otau_pause_resp_handler(param);
					}
					break;
					
					case AT_OTAU_RESUME_OTAU_REQUEST:
					{
						otau_resume_request_handler(param);
					}
					break;
					
					case AT_OTAU_RESUME_OTAU_RESP:
					{
						otau_resume_resp_handler(param);
					}
					break;
					
					case AT_OTAU_IMAGE_SWITCH_REQUEST:
					{
						otau_image_switch_request_handler(param);
					}
					break;
					
					case AT_OTAU_IMAGE_END_NOTIFY_REQUEST:
					{
						otau_image_end_request_handler(param);
					}
					break;
					
					case AT_OTAU_MISSED_BLOCK_RESP:
					{
						otau_missed_block_resp_handler(param);
					}
					break;
					
					case AT_OTAU_IMAGE_PAGE_END_NOTIFY_REQUEST:
					{
						otau_page_end_notify_request_handler(param);
					}
					break;
					
					case AT_OTAU_IMAGE_SECTION_END_NOTIFY_REQUEST:
					{
						otau_section_end_request_handler(param);
					}
					break;
					
					case AT_OTAU_GET_DEVICE_INFO_REQUEST:
					{
						otau_get_device_info_request_handler(param);
					}
					break;
					
					default:
					{
						DBG_OTAU("Unknown OTAU command received: %d", otau_req->cmd);
					}
					break;					
				}
			}
		}
	}

	return status;
}

/** @brief otau_connected_state_handler called by ble manager after a
 *			change in characteristic
 *  @param[in] params of type at_ble_connected_t which has connection handle and the peer
 *				device address
 *	@return AT_BLE_SUCCESS in case of response sent success or at_ble_err_status_t in case of failure
 *
 */
at_ble_status_t otau_connected_state_handler(void *params)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	at_ble_connected_t *connected = NULL;
	
	OTAU_CHECK_NULL(params);
	connected = (at_ble_connected_t *)params;
	if (connected->conn_status == AT_BLE_SUCCESS)
	{
		/* Exchange the MTU Size */
		if(at_ble_exchange_mtu(connected->handle) == AT_BLE_SUCCESS)
		{
			DBG_OTAU("MTU Change Requested");
		}
	}
	ALL_UNUSED(connected);
	return status;
}

/** @brief otau_mtu_changed_indication called by ble manager after a
 *			change in MTU value
 *  @param[in] params of type at_ble_mtu_changed_ind_t which has connection handle and new
 *				mtu value
 *	@return AT_BLE_SUCCESS in case of response is success or at_ble_err_status_t in case of failure
 *
 */
at_ble_status_t otau_mtu_changed_indication(void *params)
{
	at_ble_status_t status = AT_BLE_FAILURE;
	at_ble_mtu_changed_ind_t *mtu_changed_ind = NULL;	
	mtu_changed_ind = (at_ble_mtu_changed_ind_t *)params;
	DBG_LOG("New MTU Value: %d", mtu_changed_ind->mtu_value);
	
	status = AT_BLE_SUCCESS;
	
	return status;
}


/** @brief otau_device_reset for otau
 *  This code will restart the system and chooses upgraded firmware on boot time
 *  @param[in] None
 *	
 *	@return None
 */
void otau_device_reset(void)
{
	/* Add code to reset the device */
	DBG_OTAU("Restarting the device to new firmware\r\n");
	ofm_restart_device();
}
