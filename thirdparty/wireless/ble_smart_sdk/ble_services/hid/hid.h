/**
 * \file
 *
 * \brief HID Service Declarations
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

// <<< Use Configuration Wizard in Context Menu >>>
// <h> HID Service Configuration
// =======================
#ifndef __HID_H__
#define __HID_H__

#include "hid_device.h"

/****************************************************************************************
*							        Enumerations	                                   	*
****************************************************************************************/

/**@brief Error Code for HID service operation
*/
typedef enum
{
	/// HID Service Registration Failed
	HID_SERV_FAILED = -1,
	
	/// HID Service Operation Pass
	HID_SERV_SUCESS = 0,
	
	/// HID Service Registration Failed
	HID_SERV_REG_FAILED,
	
	/// HID Report ID Not Found
	HID_INVALID_INST = 0xFF 
	
}hid_serv_status;


/**@brief Protocol Mode Value
*/
typedef enum
{
	/// Enable boot protocol mode
	HID_MOUSE_MODE    = 1,
	
	/// Enable report protocol mode
	HID_KEYBOARD_MODE,
}hid_dev_type;

/**@brief Protocol Mode Value
*/
typedef enum
{
	/// Enable boot protocol mode
	HID_BOOT_PROTOCOL_MODE    = 0,
	
	/// Enable report protocol mode
	HID_REPORT_PROTOCOL_MODE,
}hid_proto_mode;

/**@brief HID control point
*/
typedef enum
{
	/// Enable suspend mode
	SUSPEND    = 0,
	
	/// Exit suspend mode
	EXIT_SUSPEND,
}hid_control_mode;

/**@brief HID control point
*/
typedef enum
{
	/// Input report
	INPUT_REPORT    = 1,
	
	/// Input report
	OUTPUT_REPORT,
	
	/// Input report
	FEATURE_REPORT,
}hid_report_type;

/****************************************************************************************
*							        Structures                                     		*
****************************************************************************************/

/**@brief HID service instance for device role. */
typedef struct hid_serv
{
	at_ble_handle_t *hid_dev_serv_handle;				/**< HID service handle. */
	at_ble_uuid_t *hid_dev_serv_uuid;					/**< HID service info (0x1812). */
	at_ble_chr_t *hid_dev_proto_mode_char;				/**< HID protocol mode (0x2A4E). */
	at_ble_chr_t *hid_dev_report_map_char;				/**< HID report map (0x2A4B). */
	at_ble_chr_t *hid_dev_report_val_char[HID_NUM_OF_REPORT];      /**< HID report value (0x2A4D). */
	
#ifdef HID_KEYBOARD_DEVICE	
	at_ble_chr_t *hid_dev_boot_keyboard_in_report;   /**< HID boot keyboard input report (0x2A22). */
	at_ble_chr_t *hid_dev_boot_keyboard_out_report;   /**< HID boot keyboard output report (0x2A32). */
#endif	

#ifdef HID_MOUSE_DEVICE	
	at_ble_chr_t *hid_dev_boot_mouse_in_report;		/**< HID boot mouse input report (0x2A33). */
#endif	
	
	at_ble_chr_t *hid_dev_info;						/**< HID information (0x2A4A). */
	at_ble_chr_t *hid_control_point;				/**< HID information (0x2A4C). */

} hid_serv_t;

/**@brief HID report reference descriptor instance for device role. */
typedef struct hid_reportref_desc
{
	at_ble_handle_t reportref_desc_handle;
	uint8_t reportid;
	uint8_t reporttype;
}hid_reportref_desc_t;

/****************************************************************************************
*                                       Functions                                       *
****************************************************************************************/
/** @brief Initialize the hid profile based on user input
  * 
  * @param[in] servinst		HID service instance
  * @param[in] device		HID device
  * @param[in] mode			HID mode
  * @param[in] report_num	Number of report
  * @param[in] report_type	Report Type
  * @param[in] report_val	Report Value
  * @param[in] report_len	Report Length
  * @param[in] info			HID device info 
  */
void hid_serv_init(uint8_t servinst, uint8_t device, uint8_t *mode, uint8_t report_num, uint8_t *report_type, uint8_t **report_val, uint8_t *report_len, hid_info_t *info);

/** @brief Function update the report descriptor for HID device
  * 
  * @param[in] servinst		HID service instance
  * @param[in] report_info	Report Info
  * @param[in] len			Length of report
  */
void hid_serv_report_map(uint8_t servinst, uint8_t *report_info, uint16_t len);

/** @brief service definition initialization function
  * 
  * @param[in] servinst		HID service instance
  */
void hid_serv_def_init(uint8_t servnum);

/** @brief Register the service into database
  * 
  * @param[in] inst			HID service instance
  * @param[in] report_type	Report Type
  * @param[in] report_id    Report ID
  * @param[in] report_num   Number of report
  */
uint16_t hid_service_dbreg(uint8_t inst, uint8_t *report_type, uint8_t *report_id, uint8_t report_num);

/** @brief Function to get the report ID.
  * 
  * @param[in] serv			HID service instance
  * @param[in] handle	    Connection handle
  * @param[in] report_num   Number of report
  */
uint8_t hid_get_reportid(uint8_t serv, uint16_t handle, uint8_t reportnum);

/** @brief Function to get service instance.
  * 
  * @param[in] handle	    Connection handle
  */
uint8_t hid_serv_get_instance(uint16_t handle);

/** @brief Function to update the report.
  * 
  * @param[in] conn_handle	Connection handle
  * @param[in] serv_inst	Service Instance
  * @param[in] report_id    Report ID
  * @param[in] report       Report to be send
  * @param[in] len			Length of report
  */
void hid_serv_report_update( uint16_t conn_handle, uint8_t serv_inst, uint8_t reportid, uint8_t *report, uint16_t len);

/** @brief Function to get the notification instance.
  * 
  * @param[in] serv_num	Service Instance
  * @param[in] char_handle    Character Handle
  */
uint8_t hid_ntf_instance(uint8_t serv_num, uint16_t char_handle);

/** @brief Function to update the boot mouse report.
  * 
   * @param[in] conn_handle		Connection handle
   * @param[in] serv_inst		Service Instance
   * @param[in] bootreport      Report to be send
   * @param[in] len			    Length of report
  */
void hid_boot_mousereport_update(at_ble_handle_t conn_handle, uint8_t serv_inst, uint8_t *bootreport, uint16_t len);

/** @brief Function to update the boot keyboard report.
  * 
   * @param[in] conn_handle		Connection handle
   * @param[in] serv_inst		Service Instance
   * @param[in] bootreport      Report to be send
   * @param[in] len			    Length of report
  */
void hid_boot_keyboardreport_update(at_ble_handle_t conn_handle, uint8_t serv_inst, uint8_t *bootreport, uint16_t len);

/** @brief Function to get report characteristic id.
  * 
   * @param[in] handle			Connection handle
   * @param[in] serv			Service Instance
   * @param[in] reportid		Report id
  */
uint8_t hid_get_reportchar(uint16_t handle, uint8_t serv, uint8_t reportid);

#endif /*__HID_H__*/
// </h>

// <<< end of configuration section >>>
