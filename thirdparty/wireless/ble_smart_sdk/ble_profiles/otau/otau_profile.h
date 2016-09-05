/**
 * \file
 *
 * \brief OTAU Profile declarations
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


#ifndef __OTAU_PROFILE_H__
#define __OTAU_PROFILE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ble_utils.h"

#define DBG_OTAU		ALL_UNUSED //DBG_LOG
#define DBG_OTAU_CONT	ALL_UNUSED //DBG_LOG_CONT
#define OTAU_LOG		DBG_LOG

/****************************************************************************************
*							        Macro                                               *
****************************************************************************************/
/* OTAU Length field size */
#define OTAU_FRAME_LEN_SIZE		(2)

/* OTAU Module version */
#define OTAU_MAJOR_VERSION		(1)

 /* 8-bit OTAU module major number */
#define OTAU_MINOR_VERSION		(0) /* 8-bit OTAU module minor number */
#define OTAU_BUILD_NUMBER		(0x0000) /* 16-bit OTAU module build number */

typedef struct otau_service_config
{
	/* Set the permission for each OTAU characteristics */
	at_ble_attr_permissions_t char_endpoint_permission[OTAU_MAX_CHARACTERISTICS];
	at_ble_service_type_t service_type;
}otau_service_config_t;

typedef struct firmware_version
{
	uint8_t major_number;
	uint8_t minor_number;
	uint16_t build_number;
}firmware_version_t;

/* List of API callback used to inform the OTAU process to application */
typedef struct otau_callback_api_list
{
	/* OTAU Download Progress Percentage based on the section id */
	void (*otau_progress_cb) (uint8_t section_id, uint8_t completed);
	
	/* Check application for update */
	void (*otau_image_notification) (firmware_version_t *new_version, firmware_version_t *old_version, bool *permission);
	
	/* Check the OTAU image switch permission */
	void (*otau_image_switch) (firmware_version_t *fw_version, bool *permission);
			
}otau_callback_api_list_t;

/* OTAU Profile configuration from application */
typedef struct otau_profile_config
{
	/* OTAU service configuration */
	otau_service_config_t service_config;
	
	/* Application callback API list */
	otau_callback_api_list_t app_cb;
		
}otau_profile_config_t;

/* OTAU States for OTAU state machine diagram */
typedef enum otau_state
{
	/* OTAU state machine is in idle state by default (default variable value) */
	OTAU_IDLE_STATE = 0,
	/* OTAU state machine is in init state, during OTAU initialization */
	OTAU_INIT_STATE = 1,
	/* OTAU state machine is in abort state, in case of any OTAU abort send by OTAU manager */
	OTAU_ABORT_STATE,
	/* OTAU state machine is in stopped state, in case the device is disconnected or error */
	OTAU_STOPPED_STATE,
	/* OTAU manager or application is requested to pause the OTAU process */
	OTAU_PAUSE_STATE,
	/* OTAU image notification state */
	OTAU_IMAGE_NOTIFICATION_STATE,
	/* OTAU image information wait state */
	OTAU_IMAGE_INFO_WAIT_STATE,
	/* OTAU state machine will set to page downstate during page mode */
	OTAU_PAGE_DOWNLOAD_START_STATE,
	/* OTAU state machine will set to page downstate during page mode */
	OTAU_PAGE_DOWNLOADING_STATE,
	/* OTAU state machine will set to block download state during block mode */
	OTAU_BLOCK_DOWNLOAD_STATE,
	/* OTAU state machine will be in section download state during beginning of one section or end of section */
	OTAU_SECTION_DOWNLOAD_STATE,
	/* OTAU state machine will set to page end state if it receives the complete page */	
	OTAU_PAGE_END_STATE,
	/* OTAU page end confirmed state */
	OTAU_PAGE_END_CONFIRM_STATE,
	/* OTAU state machine will set to section end state if it receives the complete section */	
	OTAU_SECTION_END_STATE,
	/* OTAU state machine will set to section end confirm state, if it sends the complete section */	
	OTAU_SECTION_END_CONFIRM_STATE,
	/* OTAU state machine will set to image end state, when target device receives the complete image */
	OTAU_IMAGE_END_STATE,
	/* OTAU state machine will set to image end confirm state, when target device receives the complete image and respond back */
	OTAU_IMAGE_END_CONFIRM_STATE,
	/* OTAU state machine will be set to image switch state once the image end confirm received */
	OTAU_IMAGE_SWITCH_STATE,
	/* OTAU image switch confirm received from application */
	OTAU_IMAGE_SWITCH_CONFIRM_STATE,
	/* OTAU image might be interrupted due to various reasons, but again it needs to resume from where it left off */
	OTAU_IMAGE_RESUME_STATE,
	/* OTAU image confirmation received from the right source */
	OTAU_IMAGE_RESUME_CONFIRM_STATE,
	/* OTAU flash driver init state */
	OTAU_OFD_INIT_STATE,
	/* OTAU Disconnected state */
	OTAU_DISCONNECTED_STATE
}otau_state_t;


typedef enum otau_cmd
{
	/* OTAU manager will notify the new image with firmware	information */
	AT_OTAU_IMAGE_NOTIFY_REQUEST = 0x01,
	/* OTAU target will send the response to the new image notification */
	AT_OTAU_IMAGE_NOTIFY_RESP = 0x02,
	/* Send the image information, sent from OTAU manager to OTAU target */
	AT_OTAU_IMAGE_INFO_NOTIFY_REQUEST,
	/* If image is valid, information are correct then OTAU target will send the response to OTAU manager */
	AT_OTAU_IMAGE_INFO_NOTIFY_RESP,
	/* Start image download */
	AT_OTAU_START_IMAGE_DOWNLOAD,
	/* Send page data from OTAU manager to OTAU target */
	AT_OTAU_PAGE_DATA_NOTIFY_REQUEST,
	/* OTAU target will send the page	image data response to OTAU manager. */
	AT_OTAU_PAGE_DATA_NOTIFY_RESP,
	/* Send block data from OTAU manager to OTAU target */
	AT_OTAU_BLOCK_DATA_NOTIFY_REQUEST,
	/* OTAU target will send the block image data response to	OTAU manager. */
	AT_OTAU_BLOCK_DATA_NOTIFY_RESP,
	/* Abort the current OTAU process */
	AT_OTAU_ABORT_OTAU,
	/* Pause the current OTAU process, this command sent by OTAU Target or OTAU manager */
	AT_OTAU_PAUSE_OTAU_REQUEST,
	/* Response will be sent for the received pause command */
	AT_OTAU_PAUSE_OTAU_RESP,
	/* Request to resume the OTAU process */
	AT_OTAU_RESUME_OTAU_REQUEST,
	/* Response for OTAU resume request */
	AT_OTAU_RESUME_OTAU_RESP,
	/* The OTAU manager can send the force update request to device to downgrade or update the OTAU
	Target firmware version */
	AT_OTAU_FORCE_UDPATE_REQUEST,
	/* Force update response from OTAU target to OTAU manager */
	AT_OTAU_FORCE_UDPATE_RESP,
	/* OTAU image switch request to switch to upgraded image */
	AT_OTAU_IMAGE_SWITCH_REQUEST,
	/* OTAU target will send the response for image switch request */
	AT_OTAU_IMAGE_SWITCH_RESP,
	/* Image end notify request from OTAU manager to OTAU target when image download is complete */
	AT_OTAU_IMAGE_END_NOTIFY_REQUEST,
	/* Image end notify response, in case target has verified the complete image */
	AT_OTAU_IMAGE_END_NOTIFY_RESP,
	/* Target device can request for missed block to OTAU manager */
	AT_OTAU_MISSED_BLOCK_REQUEST,
	/* OTAU manager will send the missed block in response */
	AT_OTAU_MISSED_BLOCK_RESP,
	/* OTAU image update check triggered by OTAU target device */
	AT_OTAU_IMAGE_UPDATE_CHECK,
	/* OTAU page end notification in case of block download mode */
	AT_OTAU_IMAGE_PAGE_END_NOTIFY_REQUEST,
	/* OTAU manager has sent the complete image section to the OTAU Target, 
	at the end OTAU manager will send the section end to target to verify the image CRC */
	AT_OTAU_IMAGE_SECTION_END_NOTIFY_REQUEST,
	/* Target has verified the image size and CRC of the image and respond to the OTAU manager is its correct */
	AT_OTAU_IMAGE_SECTION_END_NOTIFY_RESP,	
	/* OTAU Target reads device information	like firmware version, hardware	version, product id and vendor id */
	AT_OTAU_GET_DEVICE_INFO_REQUEST,
	/* OTAU Target Device gives info like firmware version, hardware version, product id and vendor id.*/
	AT_OTAU_GET_DEVICE_INFO_RESP,
	
	/* OTAU is completed, application can reset the device after getting this command */
	AT_OTAU_RESET_DEVICE,
	
	/* Generic OTAU Error */
	AT_OTAU_FAILURE = 0x30
}otau_cmd_t;

typedef enum otau_status
{
	/* Image details shared with OTAU target is incorrect or incompatible */
	AT_OTAU_INVALID_IMAGE = 0x81,
	/* Image switch is not allowed by OTAU application */
	AT_OTAU_IMAGE_SWITCH_ERROR = 0x82,
	/* The CRC for the image page is incorrect */
	AT_OTAU_BAD_PAGE_CRC = 0x83,
	/* The CRC for block image is incorrect */
	AT_OTAU_BAD_BLOCK_CRC,
	/* OTAU image is older version */
	AT_OTAU_IMAGE_OLDER_VERSION,
	/* OTAU image verification failed */
	AT_OTAU_IMAGE_VERIFICATION_FAILED,
	/* OTAU image out of memory */
	AT_OTAU_IMAGE_OUT_OF_MEMORY,
	/* OTAU image details contains invalid vendor ID */
	AT_OTAU_INVALID_VENDOR_ID,
	/* OTAU image details contains invalid Product ID */
	AT_OTAU_INVALID_PRODCUT_ID,
	/* OTAU image details contains invalid hardware version */
	AT_OTAU_INVALID_HARDWARE_VERSION,
	/* OTAU image details contains invalid hardware revision */
	AT_OTAU_INVALID_HARDWARE_REVISION,
	/* OTAU target receives NO update in case of OTAU  target send update request */
	AT_OTAU_NO_UPDATE,
	/* OTAU security level not supported by current OTAU in target device */
	AT_OTAU_SECURITY_LEVEL_NOT_SUPPORTED,
	/* OTAU invalid  image section */
	AT_OTAU_INVALID_SECTION,
	/* OTAU target receives the invalid image size */
	AT_OTAU_INVALID_IMAGE_SIZE,
	/* OTAU unspecified state and gives unknown error */
	AT_OTAU_UNKNOWN_ERROR,
	/* OTAU image page information is incorrect */
	AT_OTAU_IMAGE_PAGE_INFO_ERROR,
	/* OTAU image block information is incorrect */
	AT_OTAU_IMAGE_BLOCK_INFO_ERROR,
	/* OTAU image section end receives error */
	AT_OTAU_IMAGE_SECTION_END_ERROR,
	/* OTAU No Status */
	AT_OTAU_IMAGE_NO_STATUS,
	/* OTAU invalid command error */
	AT_OTAU_INVALID_COMMAND,
	/* OTAU OFD Initialization Failed */
	AT_OTAU_OFD_FAILED,
	/* OTAU command Success */
	AT_OTAU_CMD_SUCCESS,
	/* OTAU command Failed */
	AT_OTAU_CMD_FAILED
		
}otau_status_t;

typedef enum otau_mode
{
	/* OTAU is in force upgrade mode, this mode is set by OTAU manager, 
	in case the OTAU target and OTAU manager image versions are same */
	OTAU_FORCE_UPGRADE_MODE = 0,
	/* OTAU downgrades to older firmware version. 
	OTAU manager decides to downgrade the OTAU target image to older version */
	OTAU_FORCE_DOWNGRADE_MODE = 1,
	/* OTAU image update in normal upgrade mode. 
	Generally this mode is used to upgrade the OTAU target image to new higher firmware version */
	OTAU_IMAG_NORMAL_UPGRADE_MODE
}otau_mode_t;

typedef enum flash_id
{
	IMAGE_META_DATA_ID = 0,
	OTAU_IMAGE_META_DATA_ID = 1
}flash_id_t;

typedef enum otau_image_option
{
	OTAU_START_DOWNLOAD = AT_OTAU_START_IMAGE_DOWNLOAD,
	OTAU_RESUME_DOWNLOAD = AT_OTAU_RESUME_OTAU_REQUEST
}otau_image_option_t;

/* image section mask will used to identify the OTAU is 
	upper part of section or lower part of the section */
#define SECTION1_IMAGE_IDENTIFIER_MASK		(0x0F)
#define SECTION1_TOP_IMAGE_IDENTIFIER		(0x00)
#define SECTION1_BOTTOM_IMAGE_IDENTIFIER	(0x01)

#define SECTION2_IMAGE_IDENTIFIER_MASK		(0xF0)
#define SECTION2_TOP_IMAGE_IDENTIFIER		(0x00)
#define SECTION2_BOTTOM_IMAGE_IDENTIFIER	(0x10)

#define SECTION3_IMAGE_IDENTIFIER_MASK		(0xF00)
#define SECTION3_TOP_IMAGE_IDENTIFIER		(0x000)
#define SECTION3_BOTTOM_IMAGE_IDENTIFIER	(0x100)

#define IMAGE_CRC32_POLYNOMIAL				(0xFFFFFFFF)

typedef uint8_t	section_id_t;
typedef uint32_t otau_flash_addr_t;

typedef uint8_t hardware_revision_t;

BLE_PACK_SET

typedef struct hardware_version
{
	uint8_t major;
	uint8_t minor;
}hardware_version_t;

typedef struct otau_process
{
	/* OTAU current state will be set here */
	otau_state_t cstate;
	/* OTAU previous state will be set here; Used for Pause and Resume */
	otau_state_t pstate;
	/* OTAU status will be captured */
	otau_status_t status;
	/* OTAU mode */
	otau_mode_t mode;
}otau_process_t;


typedef struct otau_req
{
	uint16_t length;
	/* otau_cmd_t type */
	uint8_t cmd;
	/* payload will be appended along with structure */			
}otau_req_t;


typedef struct otau_resp
{
	uint16_t length;
	/* otau_status_t */
	uint8_t cmd;
	/* payload will be appended along with structure */
}otau_resp_t;


typedef struct otau_error_resp
{
	otau_resp_t resp;
	otau_status_t error;
}otau_error_resp_t;

typedef struct image_notification_req
{
	otau_req_t req;
	uint16_t product_id;
	uint16_t vendor_id;
	uint8_t image_sections;
	firmware_version_t fw_ver;
	hardware_version_t hw_ver;
	hardware_revision_t hw_rev;
	uint8_t security_level;
}image_notification_req_t;

typedef struct image_notification_resp
{
	otau_resp_t resp;
	hardware_revision_t hardware_revision;
	firmware_version_t fw_ver;
	hardware_version_t hardware_version;
	otau_image_option_t image_option;
}image_notification_resp_t;

typedef otau_error_resp_t image_notification_failure_resp_t;

typedef struct image_section_info
{
	section_id_t section_id;
	uint8_t size[3];
	otau_flash_addr_t start_address;	
}image_section_info_t;

typedef struct image_download_info
{
	section_id_t section_id;
	uint8_t size[3];
}image_download_info_t;

typedef uint32_t image_crc_t;

typedef struct image_info_request
{
	otau_req_t req;
	uint8_t total_sections;
	uint32_t total_image_size;
	image_section_info_t patch_section;
	image_section_info_t app_header_section;
	image_section_info_t app_section;
	image_crc_t total_image_crc;
	image_crc_t patch_crc;
	image_crc_t app_header_crc;
	image_crc_t app_crc;
}image_info_request_t;

typedef struct image_info_resp
{
	otau_resp_t resp;
	otau_image_option_t image_option;
	section_id_t section_id;
	uint16_t page_no;
	uint8_t block_no;
	uint16_t page_size;
	uint16_t block_size;
}image_info_resp_t;

typedef otau_error_resp_t image_info_failure_resp_t;

/* Page Data Notification from OTAU manager to OTAU Target */
typedef struct page_data_notify_request
{
	otau_req_t req;
	section_id_t section_id;
	uint16_t page_no;
	uint8_t page_data[OTAU_CHARACTERISTICS_ENDPOINT2_MAX_SIZE-6];
}page_data_notify_request_t;

typedef struct page_data_notify_resp
{
	otau_resp_t resp;
	section_id_t section_id;
	uint16_t page_no;
}page_data_notify_resp_t;

typedef page_data_notify_resp_t page_data_failure_resp_t;


/* Block Data Notification from OTAU manager to OTAU Target */
typedef struct block_data_notify_request
{
	otau_req_t req;
	section_id_t section_id;
	uint16_t page_no;
	uint16_t block_no;
	uint8_t *block_data;
}block_data_notify_request_t;

typedef struct block_data_notify_resp
{
	otau_resp_t resp;
	section_id_t section_id;
	uint16_t page_no;
	uint16_t block_no;
}block_data_notify_resp_t;

typedef block_data_notify_resp_t block_data_failure_resp_t;


/* Page Data End Notification from OTAU manager to OTAU Target */
typedef struct page_end_notify_request
{
	otau_req_t req;
	section_id_t section_id;
	uint16_t page_no;
}page_end_notify_request_t;

typedef struct page_end_notify_resp
{
	otau_resp_t resp;
	section_id_t section_id;
	uint16_t page_no;
}page_end_notify_resp_t;

typedef page_end_notify_resp_t page_end_failure_resp_t;

/* Image Section End Notification from OTAU manager to OTAU Target */
typedef struct section_end_notify_request
{
	otau_req_t req;
	section_id_t section_id;
}section_end_notify_request_t;

typedef struct section_end_notify_resp
{
	otau_resp_t resp;
	section_id_t section_id;
}section_end_notify_resp_t;

typedef section_end_notify_resp_t section_end_failure_resp_t;

/* Image End Notification from OTAU manager to OTAU Target */
typedef struct image_end_notify_request
{
	otau_req_t req;
	uint8_t total_sections;
	uint32_t total_image_size;
	image_crc_t total_image_crc;
}image_end_notify_request_t;

typedef struct image_end_notify_resp
{
	otau_resp_t resp;
	uint8_t total_sections;
	uint32_t total_image_size;
	image_crc_t total_image_crc;
}image_end_notify_resp_t;

typedef otau_error_resp_t image_end_failure_resp_t;

/* Image switch Notification from OTAU manager to OTAU Target */
typedef struct image_switch_notify_request
{
	otau_req_t req;
	uint8_t total_sections;
	firmware_version_t fw_version;
}image_switch_notify_request_t;

/* OTAU is completed and application can reset the device at any time. 
   No response is expected for this command */
typedef struct otau_device_reset_request
{
	otau_req_t req;
	firmware_version_t fw_version;
}otau_device_reset_request_t;

typedef struct image_switch_notify_resp
{
	otau_resp_t resp;
	uint8_t total_sections;
}image_switch_notify_resp_t;

typedef otau_error_resp_t image_switch_failure_resp_t;

// Response to the above request will be image notification request

/* Device Information request from OTAU manager to OTAU Target */
typedef otau_req_t get_device_info_request_t;

typedef struct device_info
{
	uint16_t product_id;
	uint16_t vendor_id;
	uint8_t total_sections;
	firmware_version_t fw_version;
	hardware_version_t hw_version;
	hardware_revision_t hw_revision;
}device_info_t;

typedef struct device_info_resp
{
	otau_resp_t resp;
	device_info_t dev_info;
	firmware_version_t otau_fw_version;
}device_info_resp_t;

typedef struct otau_pause_process
{
	otau_req_t req;
}otau_pause_process_t;

/* Image resume request from OTAU Target to OTAU manager */
typedef struct otau_resume_req
{
	otau_req_t req;
}otau_resume_req_t;

/* Resume response from OTAU manager */
typedef struct otau_pause_resp
{
	otau_resp_t resp;
}otau_pause_resp_t;

/* Resume response handler */
typedef struct otau_resume_resp
{
	otau_resp_t resp;
	device_info_t dev_info;
}otau_resume_resp_t;

typedef struct otau_resume_process
{
	otau_req_t req;
	device_info_t dev_info;
}otau_resume_process_t;

typedef otau_error_resp_t device_info_failure_resp_t;



typedef uint32_t section_image_id_t;

typedef struct image_meta_data
{
	uint16_t len;
	flash_id_t flashid;
	device_info_t dev_info;
	uint8_t security_level;
	section_image_id_t section_image_id;
	uint32_t total_image_size;
	image_section_info_t patch_section;
	image_section_info_t app_header_section;
	image_section_info_t app_section;
	image_crc_t total_image_crc;
	image_crc_t patch_crc;
	image_crc_t app_header_crc;
	image_crc_t app_crc;
	image_download_info_t patch_downloaded_info;
	image_download_info_t app_hdr_downloaded_info;
	image_download_info_t app_downloaded_info;
	image_crc_t header_crc;	
}image_meta_data_t;


typedef struct meta_data_header
{
	uint16_t len;
	flash_id_t flashid;
}meta_data_header_t;

BLE_PACK_RESET

/* Check and return in case of error */
#define OTAU_CHECK_ERROR(status)	{ if(status != AT_BLE_SUCCESS) {DBG_OTAU("#Error"); return status;}}

#define OTAU_CHECK_NULL(params)	{ if(params == NULL) {return AT_BLE_FAILURE;}}

/****************************************************************************************
*							        Function Prototypes	                                *
****************************************************************************************/

/** @brief otau_profile_init function initialize and defines the service of the OTAU profile
 *			also initializes the OTAU flash interface driver populates the flash information into 
 *			dev_flash_info global variable
 *  @param[in] params of type otau_profile_config_t to configure the OTAU
 *	@return	returns AT_BLE_SUCCESS on success or at_ble_err_status_t on failure
 */
at_ble_status_t otau_profile_init(void *params);

/** @brief otau_restore_from_sleep restore the SPI and Flash once the device wakes up from sleep
 *
 *	@param[in] params of type void * reserved for future use
 *	@return	returns AT_BLE_SUCCESS on success or at_ble_err_status_t on failure
 */
at_ble_status_t otau_restore_from_sleep(void *params);

/** @brief otau_service_define defines the services and creates the database for OTAU profile
 *	@param[in] params of type otau_service_config_t
 *	@return	returns AT_BLE_SUCCESS on success or at_ble_err_status_t on failure
 */
at_ble_status_t otau_service_define(void *params);

/** @brief otau_firmware_version gets the OTAU Firmware version
 *
 *	@param[in] params of type firmware_version_t
 *	@return	returns AT_BLE_SUCCESS on success or at_ble_err_status_t on failure
 */
at_ble_status_t otau_firmware_version(firmware_version_t *fw_version);

/** @brief otau_disconnect_event_handler disconnects with the peer device called by the
 *			ble manager. OTAU state needs to be changed and status has to be modified.
 *
 *	@param[in] params of at_ble_disconnected_t type
 *  @return	returns AT_BLE_SUCCESS on success or at_ble_err_status_t on failure
 */
at_ble_status_t otau_disconnect_event_handler(void *params);

/** @brief otau_custom_event_handler OTAU custom event needs to be identified
 *			and processed. The custom events are identified with at_ble_custom_event_t
 *			
 *	@param[in] params of at_ble_custom_event_t type
 *  @return	returns AT_BLE_SUCCESS on success or at_ble_err_status_t on failure
 */
at_ble_status_t otau_custom_event_handler(void *params);

/** @brief otau_send_indication sets the otau characteristic value and
 *			sends the indication
 *
 *  @param[in] conn_handle connection handle
 *  @param[in] att_handle attribute handle of the characteristics that needs to be updated
 *  @param[in] buf the new characteristic value
 *  @param[in] length length of the new characteristic value
 *  @return	  returns AT_BLE_SUCCESS on success or at_ble_err_status_t on failure
 */
at_ble_status_t otau_send_indication(at_ble_handle_t conn_handle, at_ble_handle_t att_handle,
									 uint8_t *buf, uint16_t length);

/** @brief otau_indication_confirmation_handler called on indication confirmation
 *  event by the ble manager
 *
 *	@param[in] params of type at_ble_indication_confirmed_t
 *  @return	  returns AT_BLE_SUCCESS on success or at_ble_err_status_t on failure
 */
at_ble_status_t otau_indication_confirmation_handler(void * params);

/** @brief otau_char_changed_handler called by the ble manager after a
 *  change in the characteristic
 *  @param[in] at_ble_characteristic_changed_t which contains handle of
 *  characteristic, new value and connection handle
 *  @return AT_BLE_SUCCESS on success and AT_BLE_FAILURE on failure
 */
at_ble_status_t otau_char_changed_handler(void *params);

/** @brief otau_connected_state_handler called by ble manager after a
 *			change in characteristic
 *  @param[in] params of type at_ble_connected_t which has connection handle and the peer
 *				device address
 *	@return AT_BLE_SUCCESS in case of response sent success or at_ble_err_status_t in case of failure
 *
 */
at_ble_status_t otau_connected_state_handler(void *params);

/** @brief otau_mtu_changed_indication called by ble manager after a
 *			change in MTU value
 *  @param[in] params of type at_ble_mtu_changed_ind_t which has connection handle and new
 *				mtu value
 *	@return AT_BLE_SUCCESS in case of response is success or at_ble_err_status_t in case of failure
 *
 */
at_ble_status_t otau_mtu_changed_indication(void *params);

/** @brief otau_image_notify_request_handler image notification will be sent by OTAU manager
 *			in case of new image needs to be notified or old image needs to be downloaded or 
 *			force upgrade is requested by OTAU manager
 *
 *	@param[in] params of type image_notification_req_t 
 *  @return	  returns AT_BLE_SUCCESS in case of image notification processed successfully or at_ble_err_status_t in case of failure
 */	
at_ble_status_t otau_image_notify_request_handler(void *params);

/** @brief otau_image_info_request_handler image details will be sent by OTAU manager
 *			in case of image notification accepted by OTAU Target Profile 
 *
 *	@param[in] params of type image_info_request_t 
 *  @return	  returns AT_BLE_SUCCESS in case of image info accepted or at_ble_err_status_t in case of failure
 */
at_ble_status_t otau_image_info_request_handler(void *params);

/** @brief otau_page_data_request_handler image page data will be sent by OTAU manager
 *			page data will be stored into the Flash memory of OTAU
 *
 *	@param[in] params of type page_data_notify_request_t 
 *  @return	  returns AT_BLE_SUCCESS page data is correct or at_ble_err_status_t in case of failure
 */
at_ble_status_t otau_page_data_request_handler(void *params);

/** @brief otau_block_data_request_handler page data will be divided into block size 
 *			and sent by OTAU manager to OTAU target. Block data 
 *			will be stored into the Flash memory of OTAU image section
 *
 *	@param[in] params of type block_data_notify_request_t 
 *  @return	  returns AT_BLE_SUCCESS block data is correct or at_ble_err_status_t in case of failure
 */
at_ble_status_t otau_block_data_request_handler(void *params);

/** @brief otau_abort_handler OTAU abort request will be sent by OTAU manager to OTAU target
 *
 *	@param[in] params of type 
 *  @return	  returns AT_BLE_SUCCESS otau abort request is correct or at_ble_err_status_t in case of failure
 */
at_ble_status_t otau_abort_handler(void *params);

/** @brief otau_pause_request_handler OTAU pause request will be sent by OTAU manager to OTAU target
 *			in case of OTAU needs to be paused by OTAU manager
 *
 *	@param[in] params of type 
 *  @return	  returns AT_BLE_SUCCESS otau abort request is correct or at_ble_err_status_t in case of failure
 */
at_ble_status_t otau_pause_request_handler(void *params);

at_ble_status_t otau_pause_resp_handler(void *params);

/** @brief otau_resume_request_handler OTAU resume request will be sent by OTAU manager to OTAU target
 *			in case of OTAU needs to be resumed by OTAU manager
 *
 *	@param[in] params of type 
 *  @return	  returns AT_BLE_SUCCESS otau abort request is correct or at_ble_err_status_t in case of failure
 */
at_ble_status_t otau_resume_request_handler(void *params);

at_ble_status_t otau_resume_resp_handler(void *params);

/** @brief otau_update_firmware_version OTAU image firmware version update required before switching to new firmware version
 *			It must be called after successfully completed the image download and verified the CRC
 *
 *	@param[in] params unused
 *  @return	  returns AT_BLE_SUCCESS OTAU image firmware version updated successfully or at_ble_err_status_t in case of failure
 */
at_ble_status_t otau_update_firmware_version(void *params);

/** @brief otau_image_switch_request_handler OTAU image switch request will be sent by the OTAU Manager
 *			after successfully completing the image download and verified the CRC
 *
 *	@param[in] params of type image_switch_notify_request_t
 *  @return	  returns AT_BLE_SUCCESS image switch request processed successfully or at_ble_err_status_t in case of failure
 */
at_ble_status_t otau_image_switch_request_handler(void *params);

/** @brief otau_image_end_request_handler OTAU image end request will be sent by the OTAU Manager
 *			after successfully completing the image download and to verify the complete image CRC
 *
 *	@param[in] params of type image_end_notify_request_t
 *  @return	  returns AT_BLE_SUCCESS in case of the Total image CRC is correct or at_ble_err_status_t in case of failure
 */
at_ble_status_t otau_image_end_request_handler(void *params);

/** @brief otau_missed_block_resp_handler function will handle the missed block
 *
 *  @param[in] params
 *	@return AT_BLE_SUCCESS in case of success or AT_BLE_FAILURE
 *
 */
at_ble_status_t otau_missed_block_resp_handler(void *params);

/** @brief otau_page_end_notify_request_handler page end notification will be received
 *			in block mode, once the complete page is sent by OTAU Manager
 *  @param[in] params
 *	@return AT_BLE_SUCCESS in case of success or AT_BLE_FAILURE
 *
 */
at_ble_status_t otau_page_end_notify_request_handler(void *params);

/** @brief otau_section_end_request_handler section end will be sent by the OTAU manager 
 *			to OTAU target at the each end of the section. Section end will be used to verify the CRC 
 *			of the each section before move to another section download
 *  @param[in] params of type section_end_notify_request_t
 *	@return AT_BLE_SUCCESS in case of success or at_ble_err_status_t in case of failure
 *
 */
at_ble_status_t otau_section_end_request_handler(void *params);

/** @brief otau_get_device_info_request_handler get device information will be requested by OTAU
 *			manager to OTAU target. OTAU manager will need to know about the device information before sending the
 *			otau image notification. This information may be used to identify the correct firmware file and 
 *			to check the version for upgrade. 
 *  @param[in] params of type get_device_info_request_t
 *	@return AT_BLE_SUCCESS in case of response sent success or at_ble_err_status_t in case of failure
 *
 */
at_ble_status_t otau_get_device_info_request_handler(void *params);

/** @brief otau_invalid_frame_resp in case the frame basic checks are incorrect 
 *			i.e length or unsupported command
 *  @param[in] params not used
 *	@return AT_BLE_SUCCESS in case of response sent success or at_ble_err_status_t in case of failure
 *
 */
at_ble_status_t otau_invalid_frame_resp(void *params);

/** @brief memcpy_nreverse copy the memory content from one location to another memory location
 *			and reverse the copied content
 *
 *	@param[in] dst destination memory location
 *	@param[in] src source memory location
 *	@param[in] len length of the memory operation
 *  @return	  None
 */
void memcpy_nreverse(uint8_t *data, uint8_t *buf, uint32_t len);

/** @brief otau_pause_update_process OTAU pause request will be sent by Application to OTAU Manager
 *			in case of Application needs to pause the image download
 *
 *	@param[in] params of type 
 *  @return	  returns AT_BLE_SUCCESS otau pause request is correct and send to OTAU manager or at_ble_err_status_t in case of failure
 */
at_ble_status_t otau_pause_update_process(void *params);

/** @brief otau_resume_update_process OTAU resume request will be sent by Application to OTAU Manager
 *			in case of Application needs to resume the image download
 *
 *	@param[in] params of type 
 *  @return	  returns AT_BLE_SUCCESS otau resume request is correct and send to OTAU manager or at_ble_err_status_t in case of failure
 */
at_ble_status_t otau_resume_update_process(void *params);

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
								bool resume, image_crc_t crc);

#ifdef __cplusplus
}
#endif

#endif /*__OTAU_PROFILE_H__ */

