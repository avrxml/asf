/**
 * \file
 *
 * \brief HID Device Profile Declarations
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
// <h> HID Device profile Configuration
// =======================

#ifndef __HID_DEVICE_H__
#define __HID_DEVICE_H__
#include "at_ble_api.h"

/****************************************************************************************
*							        Macros	                                     		*
****************************************************************************************/

/** @brief Number of Report : Configure by user. */
//	<o> HID Number of Reports <1-10>
//	<i> Defines numbers of Reporter available for HID.
//	<i> Default: 1
//	<id> hid_num_of_report
#define HID_NUM_OF_REPORT				(1)

/** @brief HID Service Instance : Configure by user. */
#define HID_SERV_INST 					(1) 

/** @brief HID Max Service Instance. */
#define HID_MAX_SERV_INST				(2)

/** @brief Number of Report. */
#define HID_MAX_REPORT_NUM				(10)

/** @brief Default number of characteristic. */
#define HID_DEFAULT_CHARACTERISITC_NUM	(4)

#ifndef HID_MOUSE_DEVICE
#ifndef HID_KEYBOARD_DEVICE
#define HID_KEYBOARD_DEVICE
#endif
#endif

/** @brief Number of characteristic for HID. */
#if defined HID_MOUSE_DEVICE
/** @brief Enable by user for mouse application : Configure by user. */
#define ADV_DATA_APPEARANCE_DATA		(0x03C2)  //Mouse
#if defined BOOT_MODE
#define HID_CHARACTERISTIC_NUM			(HID_DEFAULT_CHARACTERISITC_NUM + 1)
#else
#define HID_CHARACTERISTIC_NUM			(HID_DEFAULT_CHARACTERISITC_NUM + HID_NUM_OF_REPORT + 1)
#endif
#endif

#if defined HID_KEYBOARD_DEVICE
/** @brief Enable by user for keyboard application : Configure by user. */
#define ADV_DATA_APPEARANCE_DATA 		(0x03C1)    //Keyboard
#if defined BOOT_MODE
#define HID_CHARACTERISTIC_NUM			(HID_DEFAULT_CHARACTERISITC_NUM + 2)
#else
#define HID_CHARACTERISTIC_NUM			(HID_DEFAULT_CHARACTERISITC_NUM + HID_NUM_OF_REPORT + 2)
#endif
#endif

/** @brief APP_HID_FAST_ADV between 0x0020 and 0x4000 in 0.625 ms units (20ms to 10.24s). */
//	<o> Fast Advertisement Interval <100-1000:50>
//	<i> Defines interval of Fast advertisement in ms.
//	<i> Default: 100
//	<id> hid_fast_adv
#define APP_HID_FAST_ADV				(1600) //1000 ms

/** @brief APP_HID_ADV_TIMEOUT Advertising time-out between 0x0001 and 0x028F in seconds, 0x0000 disables time-out.*/
//	<o> Advertisement Timeout <1-655>
//	<i> Defines interval at which advertisement timeout in sec.
//	<i> Default: 655
//	<id> hid_adv_timeout
#define APP_HID_ADV_TIMEOUT				(655) // 10 min

/** @brief scan_resp_len is the length of the scan response data */
//	<o> Scan Response Buffer <1-20>
//	<i> Defines size of buffer for scan response.
//	<i> Default: 10
//	<id> hid_scan_resp_len
#define SCAN_RESP_LEN					(10)

/** @brief Advertisement data appearance len */
#define ADV_DATA_APPEARANCE_LEN			(2)

/** @brief Advertisement appearance type  */
#define ADV_DATA_APPEARANCE_TYPE		(0x19)

/** @brief Advertisement data name appearance len */
#define ADV_DATA_NAME_LEN				(9)

/** @brief Advertisement data name type */
#define ADV_DATA_NAME_TYPE				(0x09)

//	<s.9>	Advertising String
//	<i>	String Descriptor describing in advertising packet.
//	<id> hid_sensor_adv_data_name_data
#define ADV_DATA_NAME_DATA				("ATMEL-HID")

/** @brief Advertisement data UUID length */
#define ADV_DATA_UUID_LEN				(2)

/** @brief Advertisement type length */
#define ADV_TYPE_LEN					(0x01)

/** @brief Advertisement UUID type */
#define ADV_DATA_UUID_TYPE				(0x03)


/****************************************************************************************
*							        Enumerations	                                   	*
****************************************************************************************/
typedef struct hid_gatt_serv_handler
{
	at_ble_service_t		  serv;
	at_ble_chr_t		      serv_chars[HID_CHARACTERISTIC_NUM];
	at_ble_generic_att_desc_t serv_desc[HID_NUM_OF_REPORT];   /*Report descriptor*/
}hid_gatt_serv_handler_t;

/**@brief HID characteristic
*/
typedef enum
{
	PROTOCOL_MODE,
	REPORT_MAP,
	CHAR_REPORT,
	CHAR_REPORT_CCD,
	BOOT_MOUSE_INPUT_REPORT,
	BOOT_KEY_OUTPUT_REPORT,
	BOOT_KEY_INPUT_REPORT,
	HID_INFORMATION,
	CONTROL_POINT,
}hid_char_type_t;

/**@brief HID Profile error code
*/
typedef enum
{
	HID_PRF_SUCESS = 0,
	HID_PRF_INSTANCE_OUT_RANGE,
	HID_PRF_NO_INSTANCE,
}hid_prf_error_t;

/****************************************************************************************
*							        Structures                                     		*
****************************************************************************************/
/**@brief HID control point info, notify to user when control point change by user. */
typedef struct hid_control_mode_ntf
{
	uint8_t serv_inst;        /**< Service instance number. */
	uint16_t control_value;   /**< Suspend or exit suspend. */
}hid_control_mode_ntf_t;

/**@brief HID protocol mode info, notify to user when protocol mode change by user. */
typedef struct hid_proto_mode_ntf
{
	at_ble_handle_t conn_handle; /**< Connection Handle. */
	uint8_t serv_inst;			/**< Service instance number. */
	uint16_t mode;				/**< Protocol mode or report mode. */
}hid_proto_mode_ntf_t;

/**@brief HID boot notification or indication, notify to user. */
typedef struct hid_boot_ntf
{
	uint8_t serv_inst;				/**< Service instance number. */
	uint8_t boot_value;				/**< Keyboard or mouse. */
	uint16_t ntf_conf;				/**< Notification or Indication status. */
	at_ble_handle_t conn_handle;	/**< Connection Handle. */
}hid_boot_ntf_t;

/**@brief HID report notification or indication, notify to user. */
typedef struct hid_report_ntf
{
	uint8_t serv_inst;				/**< Service instance number. */
	uint8_t report_ID;				/**< Report ID to be notify or indicated. */
	uint16_t ntf_conf;				/**< Notification or Indication status. */
	at_ble_handle_t conn_handle;	/**< Connection Handle. */
}hid_report_ntf_t;

/**@brief HID info of device. */
typedef struct hid_info
{
	uint16_t bcd_hid;				/**< Version number. */
	uint8_t  bcountry_code;			/**< Country code. */
	uint8_t  flags;					/**< Wakeup and connectable info. */
}hid_info_t;

/**@brief HID Report Reference Descriptor. */
typedef struct hid_report_ref
{
	uint8_t  report_id;				/**< Country code. */
	uint8_t  report_type;           /**< Wakeup and connectable info. */
}hid_report_ref_t;

/**@brief HID Report Map Information. */
typedef struct hid_report_map
{
	uint8_t *report_map;			/**< Report map info. */
	uint16_t report_map_len;	    /**< Report map length. */
}hid_report_map_t;

/**@brief HID profile application info. */
typedef struct
{
	uint16_t serv_handle_info;                   /**< Service handle information. */
	uint8_t hid_serv_instance;                   /**< Number of HID service instance. */
	uint8_t hid_device;                          /**< Type of HID device. */
	uint8_t protocol_mode;						 /**< Protocol mode selected by user. */
	uint8_t num_of_report;						 /**< Number of report. */
	uint8_t report_id[HID_MAX_REPORT_NUM];       /**< Report id based on number of report. */
	uint8_t report_type[HID_MAX_REPORT_NUM];     /**< Report type based on number of report. */
	uint8_t *report_val[HID_MAX_REPORT_NUM];     /**< Report value based on number of report. */
	uint8_t report_len[HID_MAX_REPORT_NUM];		 /**< Report Length based on number of report. */
	hid_report_map_t report_map_info;            /**< Report map information based on device. */
	hid_info_t hid_device_info;					 /**< HID information. */
}hid_prf_info_t;

/****************************************************************************************
*							        Basic types                                         *
****************************************************************************************/
/**@brief Report notification callback */
typedef void (*report_ntf_callback_t)(hid_report_ntf_t *);

/**@brief Boot notification callback */
typedef void (*boot_ntf_callback_t)(hid_boot_ntf_t *);

/**@brief Protocol mode notification callback */
typedef void (*protocol_mode_ntf_callback_t)(hid_proto_mode_ntf_t *);

/**@brief COntrol point notification callback */
typedef void (*control_point_ntf_callback_t)(hid_control_mode_ntf_t *);

/****************************************************************************************
*                                       Functions                                       *
****************************************************************************************/

/** @brief HID device profile configuration function 
  * 
  * @param[in] ref	HID profile application info
  * @return @ref	HID_PRF_SUCESS operation completed successfully
  * @return @ref	HID_PRF_INSTANCE_OUT_RANGE Service cannot be registered
  * @return @ref	HID_PRF_NO_INSTANCE ref value is NULL
  */
uint8_t hid_prf_conf(hid_prf_info_t *ref);

/** @brief HID device profile initialization function 
 */
void hid_prf_init(void *param);

/** @brief HID device disconnected handler function
  * 
  * @param[in] disconnect disconnect info
  * @return @ref AT_BLE_SUCCESS operation completed successfully
  */
at_ble_status_t hid_prf_disconnect_event_handler(void *params);

/** @brief Report notification handler function
*/
void notify_report_ntf_handler(report_ntf_callback_t report_ntf_fn);

/** @brief Boot notification handler function 
 */
void notify_boot_ntf_handler(boot_ntf_callback_t boot_ntf_fn);

/** @brief Protocol mode handler function
 */
void notify_protocol_mode_handler(protocol_mode_ntf_callback_t proto_mode_ntf_fn);

/** @brief Control point notification handler function 
 */
void notify_control_point_handler(control_point_ntf_callback_t control_mode_ntf_fn);

/** @brief HID profile advertisement function
 */
void hid_prf_dev_adv(void);

/** @brief Service characteristic change handler function
 */
at_ble_status_t hid_prf_char_changed_handler(void *params);

/** @brief Called by user to notify report to HID host 
  * 
  * @param[in] conn_handle	Connection handle
  * @param[in] serv_inst	Service instance
  * @param[in] reportid		Report ID corresponding to report
  * @param[in] report		Report info that need to send to host
  * @param[in] len			Report length 
  */
void hid_prf_report_update(uint16_t conn_handle, uint8_t serv_inst, uint8_t reportid, uint8_t *report, uint16_t len);

/** @brief Called by user to notify boot report for mouse to HID host 
  * 
  * @param[in] conn_handle	Connection handle
  * @param[in] serv_inst	Service instance
  * @param[in] bootreport	Report info that need to send to host
  * @param[in] len			Report length 
  */
void hid_prf_boot_mousereport_update(at_ble_handle_t conn_handle, uint8_t serv_inst, uint8_t *bootreport, uint16_t len);

/** @brief Called by user to notify boot report for keyboard to HID host 
  * 
  * @param[in] conn_handle	Connection handle
  * @param[in] serv_inst	Service instance
  * @param[in] bootreport	Report info that need to send to host
  * @param[in] len			Report length 
  */
void hid_prf_boot_keyboardreport_update(at_ble_handle_t conn_handle, uint8_t serv_inst, uint8_t *bootreport, uint16_t len);

#endif /*__HID_DEVICE_H__*/
// </h>

// <<< end of configuration section >>>
