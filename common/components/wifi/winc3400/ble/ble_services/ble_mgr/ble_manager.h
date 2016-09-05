/**
 * \file
 *
 * \brief BLE Manager.
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

#ifndef BLE_MANAGER_H_INCLUDED
#define BLE_MANAGER_H_INCLUDED

#include <asf.h>
#include <string.h>
#include <stddef.h>
#include "ble/include/at_ble_api.h"
#include "ble/utils/ble_utils.h"

#include "ble/ble_stack/include/wifiprov_task.h"

#define APP_PROV_IDLE			0xFF
#define APP_PROV_IN_PROGRESS	0xFE
#define APP_PROV_SUCCESS		0x0		// Should be same as what is in at_ble_wifiprov_complete_ind

extern at_ble_wifiprov_complete_ind gwifiprov_complete_ind;

#ifndef BLE_DEVICE_ROLE
#define BLE_DEVICE_ROLE BLE_CENTRAL_AND_PERIPHERAL
#endif

#if defined HID_DEVICE
#include "hid_device.h"
#ifdef HID_KEYBOARD_DEVICE
#define BLE_DEVICE_NAME				"ATMEL-HIDK"
#define BLE_AUTHENTICATION_LEVEL	(AT_BLE_NO_SEC)
#define BLE_IO_CAPABALITIES			(AT_BLE_IO_CAP_NO_INPUT_NO_OUTPUT)
#define BLE_MITM_REQ				(false)
#define BLE_BOND_REQ				(false)
#define BLE_PAIR_ENABLE				(false)
#endif

#ifdef HID_MOUSE_DEVICE
#define BLE_DEVICE_NAME				"ATMEL-HIDM"
#endif
#endif /* HID_DEVICE */

#if defined PROXIMITY_REPORTER
#include "pxp_reporter.h"
#define BLE_DEVICE_NAME				"ATMEL-PXP"
#endif /* PROXIMITY_REPORTER */

#if defined HR_SENSOR
#include "hr_sensor.h"
#define BLE_DEVICE_NAME				"ATMEL-HR"
#endif

#if defined PROXIMITY_MONITOR
#include "pxp_monitor.h"
#define BLE_DEVICE_NAME				"ATMEL-MON"
#endif /* PROXIMITY_MONITOR */

#if defined BLP_SENSOR
#include "blp_sensor.h"
#define BLE_DEVICE_NAME				"ATMEL-BLP"
#endif /* Blood_Pressure_Sensor*/

#if (BLE_DEVICE_ROLE == BLE_OBSERVER)
#include "ble_observer.h"
#define BLE_DEVICE_NAME				"ATMEL-OBS"
#endif /* BLE_DEVICE_ROLE == BLE_OBSERVER) */

#if defined ANP_CLIENT
#include "ancs_profile.h"
#define BLE_DEVICE_NAME				"ATMEL-ANCS"
#endif /* ANCS_CLIENT */

#if defined TIP_CLIENT
#include "time_info.h"
#define BLE_DEVICE_NAME				"ATMEL-TIP"
#endif /* TIP_CLIENT */

#if defined FIND_ME_TARGET
#include "find_me_target.h"
#define BLE_DEVICE_NAME				"ATMEL-FMP"
#endif /* PROXIMITY_REPORTER */

#if defined ANP_SIG_CLIENT
#include "alert_notification_profile.h"
#define BLE_DEVICE_NAME				"ATMEL-ANP"
#define BLE_PAIR_ENABLE					(false)
#endif /* ANP_SIG_CLIENT */

#if defined PAS_CLIENT
#include "pas_client.h"
#define BLE_DEVICE_NAME				"ATMEL-PAS"
#endif /* PAS_CLIENT */

#if defined CSC_DEVICE
#include "cscp.h"
#define BLE_DEVICE_NAME				"ATMEL-CSC"
#endif /* CSC_DEVICE */

/** @brief default device name */
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME				"ATMEL-BLE"
#endif

/** @brief event timeout */
#define BLE_EVENT_TIMEOUT			(20)

/* Dummy BLE handler's for unused functions */
static inline void ble_dummy_handler(void *param)
{
	UNUSED(param);
	DBG_LOG_DEV("!:(:(");
}

/* Unused variable - remove compiler warning */
static inline at_ble_status_t BLE_UNUSED2_VAR(void *param1_var, void *param2_var)
{
	UNUSED(param1_var);
	UNUSED(param2_var);
	DBG_LOG_DEV("!!:(");
	return AT_BLE_SUCCESS;
}

/** @brief Observer related declarations */
#define LE_LIMITED_DISCOVERABLE_MODE  ((uint8_t) 1 << 0)
#define LE_GENERAL_DISCOVERABLE_MODE  ((uint8_t) 1 << 1)
#define BREDR_NOT_SUPPORTED			  ((uint8_t) 1 << 2)
#define	LE_BREDR_CAPABLE_CONTROLLER   ((uint8_t) 1 << 3)
#define LE_BREDR_CAPABLE_HOST		  ((uint8_t) 1 << 4)

/** @brief Length of Adv data types*/
#define APPEARANCE_SIZE					2
#define TX_POWER_LEVEL_SIZE				1
#define ADV_INTERVAL_SIZE				2

#ifndef BLE_MITM_REQ
#define BLE_MITM_REQ					(true)
#endif

#ifndef BLE_BOND_REQ
#define BLE_BOND_REQ					(true)
#endif

#ifndef BLE_PAIR_ENABLE
#define BLE_PAIR_ENABLE					(true)
#endif

#ifndef	BLE_AUTHENTICATION_LEVEL
#define BLE_AUTHENTICATION_LEVEL		(AT_BLE_MODE1_L2_AUTH_PAIR_ENC)
#endif

#ifndef	BLE_IO_CAPABALITIES
#define BLE_IO_CAPABALITIES				(AT_BLE_IO_CAP_DISPLAY_ONLY)
#endif

#ifndef BLE_OOB_REQ
#define BLE_OOB_REQ						(false)
#endif

/** @brief Gap Advertisement Types */
typedef enum
{
	/// flags 
	FLAGS = 0x01,
	/// incomplete list of 16bits service uuids
	INCOMPLETE_LIST_16BIT_SERV_UUIDS,
	/// complete list of 16bits service uuids
	COMPLETE_LIST_16BIT_SERV_UUIDS,
	/// incomplete list of 32bits service uuids
	INCOMPLETE_LIST_32BIT_SERV_UUIDS,
	/// complete list of 32bits service uuids
	COMPLETE_LIST_32BIT_SERV_UUIDS,
	/// incomplete list of 128bits service uuids
	INCOMPLETE_LIST_128BIT_SERV_UUIDS,
	/// complete list of 128bits of service uuids
	COMPLETE_LIST_128BIT_SERV_UUIDS,
	/// shortened local name of the device 
	SHORTENED_LOCAL_NAME,
	/// complete local name of the device
	COMPLETE_LOCAL_NAME,
	/// TX power level
	TX_POWER_LEVEL,
	/// class of the device
	CLASS_OF_DEVICE = 0x0D,
	/// simple pairing hashing
	SIMPLE_PAIRING_HASHING,
	///simple pairing randomizer
	SIMPLE_PAIRING_RANDOMIZER,
	/// device id
	DEVICE_ID,
	/// security manager out of band flags
	SECURITY_MANAGER_OOB_FLAGS,
	/// slave connection interval range
	SLAVE_CONNECTION_INTERVAL_RANGE,
	/// list of 16bits service solicitation uuid 
	LIST_16BIT_SERV_SOLICITATION_UUIDS = 0x14,
	/// list of 128bits service solicitation uuids
	LIST_128BIT_SERV_SOLICITATION_UUIDS,
	///	service data
	SERVICE_DATA,
	/// public target address
	PUBLIC_TARGET_ADDRESS,
	/// random target address
	RANDOM_TARGET_ADDRESS,
	/// appearance
	APPEARANCE,
	///advertising interval
	ADVERTISING_INTERVAL,
	/// le bluetooth device address
	LE_BLUETOOTH_DEVICE_ADDRESS,
	/// le role
	LE_ROLE,
	/// simple pairing hashing c256
	SIMPLE_PAIRING_HASHING_C256,
	/// simple pairing randomizer r256
	SIMPLE_PAIRING_RANDOMIZER_R256,
	/// service data 32bits
	SERVICE_DATA_32BIT = 0x20,
	/// service data 128bits
	SERVICE_DATA_128BIT,
	/// le secure connection confirmation value
	LE_SECURE_CONNECTIONS_CONFIRMATION_VALUE,
	/// le secure connection random value
	LE_SECURE_CONNECTIONS_RANDOM_VALUE,
	/// threed information data
	THREED_INFORMATION_DATA = 0x3D,
	///manufacturer specific data
	MANUFACTURER_SPECIFIC_DATA = 0xFF
}gap_ad_type;

#if (BLE_DEVICE_ROLE == BLE_OBSERVER)
/* scan report handler */
#define BLE_SCAN_REPORT_HANDLER								ble_scan_report_handler
/* scan information handler */
#define BLE_SCAN_INFO_HANDLER(param)						ble_observer_scan_info_handler(param);\
															ble_scan_info_handler(param);
/* scan data handler */															
#define	BLE_SCAN_DATA_HANDLER								ble_observer_scan_data_handler
#endif /* (BLE_DEVICE_ROLE == BLE_OBSERVER) */


/** @brief Service UUID's */

/* Immediate Alert service UUID  */
#define IMMEDIATE_ALERT_SERVICE_UUID			(0x1802)

/* Link Loss service UUID */
#define LINK_LOSS_SERVICE_UUID					(0x1803)

/* Tx Power service UUID */
#define TX_POWER_SERVICE_UUID					(0x1804)

/* Current time service UUID */
#define CURRENT_TIME_SERVICE_UUID				(0x1805)

/* Reference Time Update service UUID */
#define REFERENCE_TIME_SERVICE_UUID				(0x1806)

/* Next DST Change service UUID */
#define NEXT_DST_SERVICE_UUID					(0x1807)

/* device information service uuid */
#define DIS_SERVICE_UUID 						(0x180A)

/** battery service uuid */
#define BAT_SERVICE_UUID 						(0x180F)
/** Alert notification service uuid */
#define ANP_SERVICE_UUID						(0x1811)

/** HID Service UUID. */
#define HID_SERV_UUID							(0x1812)

/** Scan param service uuid */
#define SPS_SERVICE_UUID 						(0x1813)

/** CSC Service UUID. */
#define CSC_SERVICE_UUID				("\x1b\xc5\xd5\xa5\x02\x00\xa6\x85\xe5\x11\x35\x39\xa0\xbb\x5a\xfd")

/* Characteristics UUID's */
/* Alert Level Characteristic UUID */
#define ALERT_LEVEL_CHAR_UUID					(0x2A06)

/* Tx Power Level Characteristic UUID */
#define TX_POWER_LEVEL_CHAR_UUID				(0x2A07)

/** battery level characteristic uuid */
#define BAT_CHAR_BAT_LEVEL_UUID 				(0x2A19)

/* device information service characteristics uuids */
#define DIS_CHAR_SYSTEM_ID_UUID					(0x2A23)

#define DIS_CHAR_MODEL_NUMBER_UUID				(0x2A24)

#define DIS_CHAR_SERIAL_NUMBER_UUID				(0x2A25)

#define DIS_CHAR_FIRMWARE_REIVSION_UUID			(0x2A26)

#define DIS_CHAR_HARDWARE_REVISION_UUID			(0x2A27)

#define DIS_CHAR_SOFTWARE_REVISION_UUID			(0x2A28)

#define DIS_CHAR_MANUFACTURER_NAME_UUID			(0x2A29)

#define DIS_CHAR_IEEE_REG_CERT_DATA_LIST_UUID	(0x2A2A)

/* Current Time char UUID */
#define CURRENT_TIME_CHAR_UUID					(0x2A2B)

/* Local Time Information char UUID */
#define LOCAL_TIME_CHAR_UUID					(0x2A0F)

/* Reference Time Information char UUID */
#define REF_TIME_CHAR_UUID						(0x2A14)

/* Time with DST char UUID */
#define TIME_WITH_DST_CHAR_UUID					(0x2A11)

/* Time Update Control Point char UUID */
#define TIME_UPDATE_CP_CHAR_UUID				(0x2A16)

/* Time Update State char UUID */
#define TIME_UPDATE_STATE_CHAR_UUID				(0x2A17)

/** scan refresh characteristic uuid */
#define SPS_CHAR_SCAN_REFRESH_UUID 				(0x2A31)
/** scan interval characteristic uuid */
#define SPS_CHAR_SCAN_INT_VALUE_UUID 			(0x2A4F)

#define DIS_CHAR_PNP_ID_UUID					(0x2A50)

#define HID_REPORT_REF_DESC						(0x2908)
/** HID Protocol Mode Characteristic UUID. */
#define HID_UUID_CHAR_PROTOCOL_MODE				(0x2A4E)

/** HID Protocol Mode Characteristic UUID. */
#define HID_UUID_CHAR_REPORT_MAP				(0x2A4B)

/** HID Report Characteristic UUID. */
#define HID_UUID_CHAR_REPORT					(0x2A4D)

/** HID Boot Keyboard Input Report UUID. */
#define HID_UUID_CHAR_BOOT_KEY_INPUT_REPORT		(0x2A22)

/** HID Boot Keyboard Output Report UUID. */
#define HID_UUID_CHAR_BOOT_KEY_OUTPUT_REPORT    (0x2A32)

/** HID Boot Mouse Input Report UUID. */
#define HID_UUID_CHAR_BOOT_MOUSE_INPUT_REPORT	(0x2A33)

/** HID Information UUID. */
#define HID_UUID_CHAR_HID_INFORMATION			(0x2A4A)

/** HID Control Point UUID. */
#define HID_UUID_CHAR_HID_CONTROL_POINT			(0x2A4C)

/** CSC Endpoint Characteristic UUID. */
#define CSC_ENDPOINT_CHAR_UUID			("\x1b\xc5\xd5\xa5\x02\x00\xa6\x85\xe5\x11\x35\x39\xa1\xbb\x5a\xfd")

/** gatt discover start handle */
#define GATT_DISCOVERY_STARTING_HANDLE	(0x0001)
/** gatt discover end handle */
#define GATT_DISCOVERY_ENDING_HANDLE	(0xFFFF)

/* All GAP Connection Parameter defined */
#if ((BLE_DEVICE_ROLE == BLE_CENTRAL) || (BLE_DEVICE_ROLE == BLE_CENTRAL_AND_PERIPHERAL) || (BLE_DEVICE_ROLE == BLE_OBSERVER))

/** minimum connection interval */
//	<o> GAP Minimum Connection Interval in msec <0-1000:50>
//	<i> Defines Minimum inteval for GAP Connection.
//	<i> Default: 20
//	<id> gap_conn_interval_min
#define GAP_CONN_INTERVAL_MIN			(20)        //Connection interval min 20ms

/** maximum connection interval */
//	<o> GAP Maximum Connection Interval in msec <0-1000:50>
//	<i> Defines Maximum inteval for GAP Connection.
//	<i> Default: 40
//	<id> gap_conn_interval_max
#define GAP_CONN_INTERVAL_MAX			(40)		//Connection interval max 40ms

/** connection slave latency */
//	<o> GAP Slave Latency <0-4>
//	<i> Defines Slave Latency for GAP Connection.
//	<i> Default: 0
//	<id> gap_conn_slave_latency
#define GAP_CONN_SLAVE_LATENCY			(0)

/** minimum length of local info parameters when using connection establishment proc */ 
#define GAP_CE_LEN_MIN					(0)
/** maximum length of local info parameters when using connection establishment proc */
#define GAP_CE_LEN_MAX					(0)

/** supervision time-out */
//	<o> GAT Super Vison Timeout in msec <0-3000:50>
//	<i> Defines SuperVision Timeout for GAP Connection.
//	<i> Default: 0x1f4
//	<id> gap_supervision_timout												
#define GAP_SUPERVISION_TIMOUT			(0x1f4)		// 500 for supervision time-out

/** number of connections */ 
#define GAP_CONNECT_PEER_COUNT			(1)

/* Max number of scan device */
//	<o> Maximum Scan Device Buffer <0-30>
//	<i> Defines maximum number of Scan device can have buffer .
//	<i> Default: 10
//	<id> max_scan_device
#define MAX_SCAN_DEVICE					(10)			  //Max number of scan device

/* Scan interval 30ms in term of 625us */
//	<o> Scan Interval in units of 625us <1-1000:50>
//	<i> Defines inteval to Scan device .
//	<i> Default: 96
//	<id> gap_scan_interval
#define SCAN_INTERVAL					(96)              //Scan interval 30ms in term of 625us

/* Scan window 30ms values in term of 625ms */
//	<o> Scan Window in term of 625us <1-1000:50>
//	<i> Defines Scan Window .
//	<i> Default: 96
//	<id> gap_scan_window
#define SCAN_WINDOW						(96)              //Scan window 30ms values in term of 625ms
#define SCAN_TIMEOUT					(0x0000)          //Timeout  Scan time-out, 0x0000 disables time-out
#define SCAN_TYPE						(AT_BLE_SCAN_ACTIVE)

#endif //((BLE_DEVICE_ROLE == BLE_CENTRAL) || (BLE_DEVICE_ROLE == BLE_CENTRAL_AND_PERIPHERAL) || (BLE_DEVICE_ROLE == BLE_OBSERVER))

/** maximum number of devices connected */
//	<o> Maximum number of device to connect <1-5>
//	<i> Defines the central to connect maximun number to devices
//	<i> Default: 1
//	<id> gap_max_device_connected
#define MAX_DEVICE_CONNECTED			(1)


#if ((BLE_DEVICE_ROLE == BLE_PERIPHERAL) || (BLE_DEVICE_ROLE == BLE_CENTRAL_AND_PERIPHERAL))

#if defined CSC_DEVICE
#define BLE_PROFILE_INIT(param)								csc_prf_init(param); \
															csc_prf_dev_adv();
															
#define BLE_ADDITIONAL_CONNECTED_STATE_HANDLER				csc_prf_connected_state_handler
#define BLE_CHARACTERISTIC_CHANGED							csc_prf_char_changed_handler
#define BLE_PRIMARY_SERVICE_FOUND_HANDLER					csc_prf_service_found_handler
#define BLE_CHARACTERISTIC_FOUND_HANDLER					csc_prf_characteristic_found_handler
#define BLE_DESCRIPTOR_FOUND_HANDLER						csc_prf_descriptor_found_handler
#define BLE_DISCOVERY_COMPLETE_HANDLER						csc_prf_discovery_complete_handler
#define BLE_NOTIFICATION_RECEIVED_HANDLER					csc_prf_notification_handler
#define BLE_ADDITIONAL_PAIR_DONE_HANDLER					csc_prf_write_notification_handler
#define BLE_ADDITIONAL_DISCONNECTED_STATE_HANDLER(param)	csc_prf_disconnect_event_handler(param);
#define BLE_ADDITIONAL_ENCRYPTION_CHANGED_HANDLER(param)    csc_prf_write_notification_handler(param);
#define BLE_NOTIFICATION_CONFIRMED_HANDLER					csc_notification_confirmation_handler

#endif


#if defined HID_DEVICE
#define BLE_PROFILE_INIT(param)								hid_prf_init(param); \
															hid_prf_dev_adv();
#define BLE_ADDITIONAL_DISCONNECTED_STATE_HANDLER(param)	hid_prf_disconnect_event_handler(param);

#define BLE_CHARACTERISTIC_CHANGED				hid_prf_char_changed_handler
#endif

#if defined PROXIMITY_REPORTER
#define BLE_PROFILE_INIT							pxp_reporter_init 
#define BLE_ADDITIONAL_CONNECTED_STATE_HANDLER		pxp_reporter_connected_state_handler
#define BLE_ADDITIONAL_DISCONNECTED_STATE_HANDLER	pxp_disconnect_event_handler
#define BLE_CHARACTERISTIC_CHANGED					pxp_reporter_char_changed_handler
#endif	/* PROXIMITY_REPORTER	 */

#if defined BLP_SENSOR
#define BLE_PROFILE_INIT							blp_sensor_init
#define BLE_ADDITIONAL_CONNECTED_STATE_HANDLER		blp_sensor_connected_state_handler
#define BLE_ADDITIONAL_DISCONNECTED_STATE_HANDLER	blp_sensor_disconnect_event_handler
#define BLE_CHARACTERISTIC_CHANGED					blp_sensor_char_changed_handler
#define BLE_NOTIFICATION_CONFIRMED_HANDLER			blp_notification_confirmation_handler
#define BLE_INDICATION_CONFIRMED_HANDLER			blp_indication_confirmation_handler
#endif 

#if defined HR_SENSOR

#define BLE_PROFILE_INIT							hr_sensor_init
#define BLE_ADDITIONAL_CONNECTED_STATE_HANDLER		hr_sensor_connected_state_handler
#define BLE_ADDITIONAL_DISCONNECTED_STATE_HANDLER	hr_sensor_disconnect_event_handler
#define BLE_CHARACTERISTIC_CHANGED					hr_sensor_char_changed_handler
#define BLE_NOTIFICATION_CONFIRMED_HANDLER			hr_notification_confirmation_handler
#endif	/* HR_SENSOR*/

#if defined FIND_ME_TARGET
#define BLE_PROFILE_INIT							fmp_target_init
#define BLE_ADDITIONAL_CONNECTED_STATE_HANDLER		fmp_target_connected_state_handler
#define BLE_ADDITIONAL_DISCONNECTED_STATE_HANDLER	fmp_target_disconnect_event_handler
#define BLE_CHARACTERISTIC_CHANGED					fmp_target_char_changed_handler
#endif	/* FIND_ME */

#define BLE_CONN_PARAM_UPDATE_REQ_HANDLER			ble_conn_param_update_req
#define BLE_CONN_PARAM_UPDATE_DONE					ble_conn_param_update
#define	BLE_PAIR_REQUEST							ble_pair_request_handler
#define BLE_PAIR_KEY_REQUEST						ble_pair_key_request_handler

#define BLE_PAIR_DONE(param)						ble_pair_done_handler(param);\
													BLE_ADDITIONAL_PAIR_DONE_HANDLER(param);
													
#define BLE_ENCRYPTION_REQUEST						ble_encryption_request_handler

#define BLE_ENCRYPTION_STATUS_CHANGED(param)		ble_encryption_status_change_handler(param);\
													BLE_ADDITIONAL_ENCRYPTION_CHANGED_HANDLER(param);
#endif /* (BLE_DEVICE_ROLE == BLE_PERIPHERAL) */


#if ((BLE_DEVICE_ROLE == BLE_CENTRAL) || (BLE_DEVICE_ROLE == BLE_CENTRAL_AND_PERIPHERAL))
#define BLE_SCAN_REPORT_HANDLER						ble_scan_report_handler
#define BLE_SCAN_INFO_HANDLER						ble_scan_info_handler
#define BLE_SLAVE_SEC_REQUEST						ble_slave_security_handler
#define BLE_PAIR_KEY_REQUEST						ble_pair_key_request_handler

/** @brief Function handlers for proximity monitor */
#if defined PROXIMITY_MONITOR
#define BLE_PROFILE_INIT							pxp_monitor_init
#define BLE_ADDITIONAL_CONNECTED_STATE_HANDLER		pxp_monitor_connected_state_handler
#define BLE_ADDITIONAL_DISCONNECTED_STATE_HANDLER	pxp_disconnect_event_handler																								
#define BLE_PRIMARY_SERVICE_FOUND_HANDLER			pxp_monitor_service_found_handler
#define BLE_DISCOVERY_COMPLETE_HANDLER				pxp_monitor_discovery_complete_handler
#define	BLE_SCAN_DATA_HANDLER						pxp_monitor_scan_data_handler
#define BLE_CHARACTERISTIC_READ_RESPONSE			pxp_monitor_characteristic_read_response
#define BLE_CHARACTERISTIC_FOUND_HANDLER			pxp_monitor_characteristic_found_handler
#define BLE_SLAVE_SEC_REQUEST						ble_slave_security_handler
#define BLE_PAIR_KEY_REQUEST						ble_pair_key_request_handler
#endif /* PROXIMITY_MONITOR */

/** @brief initializing function handlers for ANP client*/
#ifdef ANP_CLIENT
#define BLE_PROFILE_INIT										anp_client_init
#define BLE_ADDITIONAL_CONNECTED_STATE_HANDLER(param)			anp_client_connected_state_handler(param);
#define BLE_ADDITIONAL_DISCONNECTED_STATE_HANDLER(param)		anp_client_disconnected_event_handler(param);
#define BLE_CHARACTERISTIC_WRITE_RESPONSE						anp_client_write_response_handler
#define BLE_CHARACTERISTIC_FOUND_HANDLER						anp_client_characteristic_found_handler
#define BLE_NOTIFICATION_RECEIVED_HANDLER						anp_client_notification_handler
#define BLE_DESCRIPTOR_FOUND_HANDLER							anp_client_descriptor_found_handler
#define BLE_PRIMARY_SERVICE_FOUND_HANDLER						anp_client_service_found_handler
#define BLE_DISCOVERY_COMPLETE_HANDLER							anp_client_discovery_complete_handler
#define BLE_ADDITIONAL_PAIR_DONE_HANDLER(param)					anp_client_write_notification_handler(param)
#define BLE_ADDITIONAL_ENCRYPTION_CHANGED_HANDLER(param)		anp_client_write_notification_handler(param)
#endif /* ANP_CLIENT */

/** @brief initializing function handlers for TIP client*/
#ifdef TIP_CLIENT
#define BLE_PROFILE_INIT										time_info_init
#define BLE_ADDITIONAL_CONNECTED_STATE_HANDLER(param)			time_info_service_discover(param);
#define BLE_ADDITIONAL_DISCONNECTED_STATE_HANDLER(param)		time_info_disconnected_event_handler(param);
#define BLE_CHARACTERISTIC_FOUND_HANDLER						time_info_characteristic_found_handler
#define BLE_PRIMARY_SERVICE_FOUND_HANDLER						time_info_service_found_handler
#define BLE_DISCOVERY_COMPLETE_HANDLER							time_info_discovery_complete_handler
#define BLE_CHARACTERISTIC_READ_RESPONSE						time_info_characteristic_read_response
#endif /* TIP_CLIENT */

/** @brief initializing function handlers for ANP client*/
#ifdef ANP_SIG_CLIENT
#define BLE_PROFILE_INIT										anp_client_init
#define BLE_ADDITIONAL_CONNECTED_STATE_HANDLER(param)			anp_info_service_discover(param);
#define BLE_ADDITIONAL_DISCONNECTED_STATE_HANDLER(param)		anp_client_disconnected_event_handler(param);
#define BLE_CHARACTERISTIC_WRITE_RESPONSE						anp_client_write_response_handler
#define BLE_CHARACTERISTIC_READ_RESPONSE						anp_client_read_response_handler
#define BLE_CHARACTERISTIC_FOUND_HANDLER						anp_client_characteristic_found_handler
#define BLE_NOTIFICATION_RECEIVED_HANDLER						anp_client_notification_handler
#define BLE_DESCRIPTOR_FOUND_HANDLER							anp_client_descriptor_found_handler
#define BLE_PRIMARY_SERVICE_FOUND_HANDLER						anp_client_service_found_handler
#define BLE_DISCOVERY_COMPLETE_HANDLER							anp_client_discovery_complete_handler
#define BLE_ADDITIONAL_PAIR_DONE_HANDLER(param)					anp_client_security_done_handler(param)
#define BLE_ADDITIONAL_ENCRYPTION_CHANGED_HANDLER(param)		anp_client_security_done_handler(param)
#endif /* ANP_CLIENT */
#ifdef PAS_CLIENT
#define BLE_PROFILE_INIT										pas_client_init
#define BLE_ADDITIONAL_CONNECTED_STATE_HANDLER(param)			pas_client_service_discovery(param);
#define BLE_ADDITIONAL_DISCONNECTED_STATE_HANDLER(param)		pas_client_disconnected_event_handler(param);
#define BLE_CHARACTERISTIC_WRITE_RESPONSE						pas_client_char_write_response_handler
#define BLE_CHARACTERISTIC_READ_RESPONSE						pas_client_char_read_response_handler
#define BLE_CHARACTERISTIC_FOUND_HANDLER						pas_client_characteristic_found_handler
#define BLE_NOTIFICATION_RECEIVED_HANDLER						pas_client_notification_handler
#define BLE_DESCRIPTOR_FOUND_HANDLER							pas_client_descriptor_found_handler

#define BLE_PRIMARY_SERVICE_FOUND_HANDLER						pas_client_service_found_handler
#define BLE_DISCOVERY_COMPLETE_HANDLER							pas_client_discovery_complete_handler
#define BLE_ADDITIONAL_PAIR_DONE_HANDLER(param)					pas_client_write_notifications(param)
#define BLE_ADDITIONAL_ENCRYPTION_CHANGED_HANDLER(param)		pas_client_write_notifications(param)
#endif /* PAS_CLIENT */

#endif /* ((BLE_DEVICE_ROLE == BLE_CENTRAL) || (BLE_DEVICE_ROLE == BLE_CENTRAL_AND_PERIPHERAL)) */

/* Common functions */
#define BLE_CONNECTED_STATE_HANDLER(param)			ble_connected_state_handler(param);\
													BLE_ADDITIONAL_CONNECTED_STATE_HANDLER(param);

#define BLE_DISCONNECTED_STATE_HANDLER(param)		ble_disconnected_state_handler(param);\
													BLE_ADDITIONAL_DISCONNECTED_STATE_HANDLER(param);

#define BLE_EVENT_PARAM_MAX_SIZE					512

#ifndef BLE_PROFILE_INIT
#define BLE_PROFILE_INIT										ble_dummy_handler
#endif

#ifndef BLE_ADDITIONAL_CONNECTED_STATE_HANDLER
#define BLE_ADDITIONAL_CONNECTED_STATE_HANDLER					ble_dummy_handler
#endif

#ifndef BLE_ADDITIONAL_DISCONNECTED_STATE_HANDLER
#define BLE_ADDITIONAL_DISCONNECTED_STATE_HANDLER				ble_dummy_handler
#endif

#ifndef BLE_CHARACTERISTIC_CHANGED
#define BLE_CHARACTERISTIC_CHANGED								ble_dummy_handler
#endif

#ifndef BLE_CONN_PARAM_UPDATE_DONE
#define BLE_CONN_PARAM_UPDATE_DONE								ble_dummy_handler
#endif

#ifndef BLE_PAIR_REQUEST
#define	BLE_PAIR_REQUEST										ble_dummy_handler
#endif

#ifndef BLE_PAIR_KEY_REQUEST
#define BLE_PAIR_KEY_REQUEST									ble_dummy_handler
#endif

#ifndef BLE_PAIR_DONE
#define BLE_PAIR_DONE											ble_dummy_handler
#endif
 
#ifndef BLE_ENCRYPTION_REQUEST
#define BLE_ENCRYPTION_REQUEST									ble_dummy_handler
#endif

#ifndef BLE_ENCRYPTION_STATUS_CHANGED
#define BLE_ENCRYPTION_STATUS_CHANGED							ble_dummy_handler
#endif
													
#ifndef BLE_CONN_PARAM_UPDATE_DONE
#define BLE_CONN_PARAM_UPDATE_DONE								ble_dummy_handler
#endif

#ifndef	BLE_PROFILE_INIT
#define BLE_PROFILE_INIT										ble_dummy_handler
#endif

#ifndef BLE_ADDITIONAL_CONNECTED_STATE_HANDLER
#define BLE_ADDITIONAL_CONNECTED_STATE_HANDLER					ble_dummy_handler
#endif

#ifndef BLE_ADDITIONAL_DISCONNECTED_STATE_HANDLER
#define BLE_ADDITIONAL_DISCONNECTED_STATE_HANDLER				ble_dummy_handler
#endif

#ifndef BLE_CHARACTERISTIC_CHANGED
#define BLE_CHARACTERISTIC_CHANGED								ble_dummy_handler
#endif

#ifndef BLE_CONN_PARAM_UPDATE_DONE
#define BLE_CONN_PARAM_UPDATE_DONE								ble_dummy_handler
#endif

#ifndef BLE_PAIR_REQUEST
#define	BLE_PAIR_REQUEST										ble_dummy_handler
#endif

#ifndef BLE_PAIR_KEY_REQUEST
#define BLE_PAIR_KEY_REQUEST									ble_dummy_handler
#endif

#ifndef BLE_PAIR_DONE
#define BLE_PAIR_DONE											ble_dummy_handler
#endif

#ifndef BLE_ENCRYPTION_REQUEST
#define BLE_ENCRYPTION_REQUEST									ble_dummy_handler
#endif

#ifndef BLE_ENCRYPTION_STATUS_CHANGED
#define BLE_ENCRYPTION_STATUS_CHANGED							ble_dummy_handler
#endif

#ifndef BLE_SCAN_REPORT_HANDLER
#define BLE_SCAN_REPORT_HANDLER									ble_dummy_handler
#endif

#ifndef BLE_SCAN_DATA_HANDLER
#define BLE_SCAN_DATA_HANDLER(x,y)								BLE_UNUSED2_VAR(x, &y)
#endif

#ifndef BLE_SCAN_INFO_HANDLER
#define BLE_SCAN_INFO_HANDLER									ble_dummy_handler
#endif

#ifndef BLE_CHARACTERISTIC_WRITE_RESPONSE
#define BLE_CHARACTERISTIC_WRITE_RESPONSE						ble_dummy_handler
#endif

#ifndef BLE_ADDITIONAL_PAIR_DONE_HANDLER
#define BLE_ADDITIONAL_PAIR_DONE_HANDLER						ble_dummy_handler
#endif

#ifndef BLE_ADDITIONAL_ENCRYPTION_CHANGED_HANDLER
#define BLE_ADDITIONAL_ENCRYPTION_CHANGED_HANDLER				ble_dummy_handler
#endif

#ifndef BLE_DESCRIPTOR_FOUND_HANDLER
#define BLE_DESCRIPTOR_FOUND_HANDLER							ble_dummy_handler
#endif

#ifndef BLE_NOTIFICATION_RECEIVED_HANDLER
#define BLE_NOTIFICATION_RECEIVED_HANDLER						ble_dummy_handler
#endif

#ifndef BLE_NOTIFICATION_CONFIRMED_HANDLER
#define BLE_NOTIFICATION_CONFIRMED_HANDLER						ble_dummy_handler
#endif

#ifndef BLE_CHARACTERISTIC_READ_RESPONSE
#define BLE_CHARACTERISTIC_READ_RESPONSE						ble_dummy_handler
#endif

#ifndef BLE_DESCRIPTOR_FOUND_HANDLER
#define BLE_DESCRIPTOR_FOUND_HANDLER							ble_dummy_handler
#endif

#ifndef BLE_PRIMARY_SERVICE_FOUND_HANDLER
#define BLE_PRIMARY_SERVICE_FOUND_HANDLER						ble_dummy_handler
#endif

#ifndef BLE_DISCOVERY_COMPLETE_HANDLER
#define BLE_DISCOVERY_COMPLETE_HANDLER							ble_dummy_handler
#endif

#ifndef BLE_CHARACTERISTIC_FOUND_HANDLER
#define BLE_CHARACTERISTIC_FOUND_HANDLER						ble_dummy_handler
#endif

#ifndef AT_BLE_MTU_CHANGED_INDICATION_HANDLER					
#define AT_BLE_MTU_CHANGED_INDICATION_HANDLER					ble_dummy_handler
#endif

#ifndef BLE_INDICATION_CONFIRMED_HANDLER
#define BLE_INDICATION_CONFIRMED_HANDLER						ble_dummy_handler
#endif

#ifndef BLE_SLAVE_SEC_REQUEST
#define BLE_SLAVE_SEC_REQUEST									ble_dummy_handler
#endif

#ifndef BLE_CONN_PARAM_UPDATE_REQ_HANDLER
#define BLE_CONN_PARAM_UPDATE_REQ_HANDLER						ble_dummy_handler
#endif

/** @brief advertisement data element
*/
typedef struct adv_element
{
	/// length of the advertisement data
	uint8_t len;
	/// advertisement type
	uint8_t type;
	/// advertisement data
	uint8_t *data;
}adv_element_t;

/** @brief GATT services handles
*/
#if defined HID_SERVICE
typedef struct gatt_service_handler
{
	at_ble_service_t		  serv;
	at_ble_chr_t		      serv_chars[HID_CHARACTERISTIC_NUM];
	at_ble_generic_att_desc_t serv_desc[HID_NUM_OF_REPORT];   /*Report descriptor*/
}gatt_service_handler_t;
#else
typedef struct gatt_service_handler
{
	/// service uuid
	at_ble_uuid_t	serv_uuid;
	/// service handle
	at_ble_handle_t	serv_handle;
	/// service characteristic
	at_ble_characteristic_t	serv_chars;
}gatt_service_handler_t;
#endif

/* Typedef for GAP event callbacks */
typedef void (*ble_gap_event_callback_t)(at_ble_handle_t);

/* Typedef for characteristic value changed event callback */
typedef at_ble_status_t (*ble_characteristic_changed_callback_t)(at_ble_characteristic_changed_t *);

/* Typedef for notification confirmed event callback */
//typedef void (*ble_notification_confirmed_callback_t)(at_ble_cmd_complete_event_t *);

/* Typedef for indication confirmed event callback */
typedef void (*ble_indication_confirmed_callback_t)(at_ble_indication_confirmed_t *);

/** @brief function to set the device name.
  *
  * @param[in] name name of the device.
  * @param[in] name_len length of the device name.
  *
  * @return @ref AT_BLE_SUCCESS operation completed successfully.
  * @return @ref AT_BLE_FAILURE Generic error.
  *
  */
at_ble_status_t ble_set_device_name(uint8_t *name, uint8_t name_len);

/** @brief function used to update the connection parameter.
  *
  * @param[in] conn_param_update @ref at_ble_conn_param_update_done_t parameters to be updated.
  *
  * @return none.
  *
  */
void ble_conn_param_update(at_ble_conn_param_update_done_t *conn_param_update);

/** @brief function triggered on receiving a connection parameter update request from the peer.
  *
  * @param[in] conn_param_req @ref at_ble_conn_param_update_request_t parameters received.
  *
  * @return none.
  *
  */
void ble_conn_param_update_req(at_ble_conn_param_update_request_t * conn_param_req);

/** @brief function called when the AT_BLE_PAIR_REQUEST event is received from stack.
  *
  * @param[in] pair_key @ref at_ble_pair_request_t details for the pairing request from the peer.
  *
  * @return none.
  *
  */
void ble_pair_request_handler(at_ble_pair_request_t *at_ble_pair_req);

void ble_slave_security_handler(at_ble_slave_sec_request_t* slave_sec_req);

/** @brief function called when the AT_BLE_PAIR_KEY_REQUEST event is received from stack.
  *
  * @param[in] pair_key @ref at_ble_pair_key_request_t key details for the pairing request.
  *
  * @return none.
  *
  */
void ble_pair_key_request_handler(at_ble_pair_key_request_t *pair_key);

/** @brief function called when the AT_BLE_PAIR_DONE event is received from stack.
  *
  * @param[in] pairing_params @ref at_ble_pair_done_t pair done information.
  *
  * @return @ref AT_BLE_SUCCESS operation completed successfully.
  * @return @ref AT_BLE_FAILURE Generic error.
  *
  */
at_ble_status_t ble_pair_done_handler(at_ble_pair_done_t *pairing_params);

/** @brief function called when the AT_BLE_ENCRYPTION_REQUEST event is received from stack.
  *
  * @param[in] encry_req encryption request from the peer device.
  *
  * @return none.
  *
  */
void ble_encryption_request_handler (at_ble_encryption_request_t *encry_req);

/** @brief function called when the AT_BLE_ENCRYPTION_STATUS_CHANGED event is received from stack.
  *
  * @param[in] encry_status changed encryption status.
  *
  * @return none.
  *
  */
void ble_encryption_status_change_handler(at_ble_encryption_status_changed_t *encry_status);

#if ((BLE_DEVICE_ROLE == BLE_CENTRAL) || (BLE_DEVICE_ROLE == BLE_CENTRAL_AND_PERIPHERAL) || (BLE_DEVICE_ROLE == BLE_OBSERVER))
/** @brief function requesting the device for the connection.
  *
  * @param[in] dev_addr address of the the peer device.
  *
  * @return @ref AT_BLE_SUCCESS operation completed successfully.
  * @return @ref AT_BLE_FAILURE Generic error.
  *
  */
at_ble_status_t gap_dev_connect(at_ble_addr_t dev_addr[]);

/** @brief function to start scan.
  *
  * @param[in] none.
  *
  * @return @ref AT_BLE_SUCCESS operation completed successfully.
  * @return @ref AT_BLE_FAILURE Generic error.
  *
  */
at_ble_status_t gap_dev_scan(void);

/** @brief function to handle the scan information.
  *
  * @param[in] scan_param scanned data.
  *
  * @return @ref AT_BLE_SUCCESS operation completed successfully.
  * @return @ref AT_BLE_FAILURE Generic error.
  *
  */
at_ble_status_t ble_scan_info_handler(at_ble_scan_info_t *scan_param);

/** @brief function to handle the scan status.
  *
  * @param[in] scan_report status of the scan.
  *
  * @return @ref AT_BLE_SUCCESS operation completed successfully.
  * @return @ref AT_BLE_FAILURE Generic error.
  *
  */
at_ble_status_t ble_scan_report_handler(at_ble_scan_report_t *scan_report);

/** @brief function parses the received advertising data for service and local name.
  *
  * @param[in] scan_info_data the received data.
  * @param[in] ble_service_uuid uuid to be searched in the received data.
  * @param[in] adv_type advertisement type
  *
  * @return @ref AT_BLE_SUCCESS operation completed successfully.
  * @return @ref AT_BLE_FAILURE Generic error.
  *
  */
uint8_t scan_info_parse(at_ble_scan_info_t *scan_info_data, at_ble_uuid_t *ble_service_uuid, uint8_t adv_type);

/** @brief function called when the AT_BLE_CHARACTERISTIC_FOUND event is received from stack.
  *
  * @param[in] characteristic_found @ref at_ble_characteristic_found_t information related to the characteristic found.
  *
  * @return none.
  *
  */
void ble_characteristic_found_handler(at_ble_characteristic_found_t *characteristic_found);
#endif

/** @brief function to handle the BLE event task.
  *
  * @param[in] none.
  *
  * @return @ref AT_BLE_SUCCESS operation completed successfully
  * @return @ref AT_BLE_FAILURE Generic error.
  *
  */
at_ble_status_t ble_event_task(void);

/** @brief function sets both device address and device name which are exposed to all other devices.
  *
  * @param[in] addr address to be set as a device address.
  * @param[in] args configuration required for initialization.
  *
  * @return none.
  *
  */
void ble_device_init(at_ble_addr_t *addr);

/** @brief function handling all the events from the stack, responsible
  * for calling the respective functions initialized for the events.
  *
  * @param[in] events events received from the stack @ref at_ble_events_t.
  * @param[in] event_params data received from stack for the respective event received.
  *
  * @return none.
  *
  */
void ble_event_manager(at_ble_events_t events , void *event_params);

/** @brief function called when the AT_BLE_DISCOVERY_COMPLETE event is received from the stack.
  *
  * @param[in]	discover_status status of the discovery.
  *
  * @return none.
  *
  */
void ble_discovery_complete_handler(at_ble_discovery_complete_t *discover_status);

/** @brief function called when the AT_BLE_DISCONNECTED event is received from the stack.
  *
  * @param[in] disconnect @ref at_ble_disconnected_t information about the disconnection.
  *
  * @return none.
  *
  */
void ble_disconnected_state_handler(at_ble_disconnected_t *disconnect);

/** @brief function to send slave security request.
  *
  * @param[in] conn_handle connection handle.
  *
  * @return @ref AT_BLE_SUCCESS operation completed successfully
  * @return @ref AT_BLE_FAILURE Generic error.
  *
  */
at_ble_status_t ble_send_slave_sec_request(at_ble_handle_t conn_handle);

/** @brief function called when the AT_BLE_CONNECTED event is received from the stack.
  *
  * @param[in] conn_params connection parameters @ref at_ble_connected_t.
  *
  * @return none.
  *
  */
void ble_connected_state_handler(at_ble_connected_t *conn_params);

/** @brief Register callback function, to be triggered when connected to the device.
  * 
  * @param[in] connected_cb_fn function called when disconnected from the device.
  *
  * @return none.
  *
  */
void register_ble_connected_event_cb(ble_gap_event_callback_t connected_cb_fn);

/** @brief Register callback function, to be triggered when disconnected from the peer device.
  * 
  * @param[in] disconnected_cb_fn function called when disconnected from the peer device.
  *
  * @return none.
  *
  */
void register_ble_disconnected_event_cb(ble_gap_event_callback_t disconnected_cb_fn);

/** @brief Register callback function, to be triggered when pairing procedure is completed.
  * 
  * @param[in] paired_cb_fn function called when pairing is completed.
  *
  * @return none.
  *
  */
void register_ble_paired_event_cb(ble_gap_event_callback_t paired_cb_fn);

/** @brief Register callback function, to be triggered when characteristic value is changed.
  * 
  * @param[in]  function called when characteristic value is changed.
  *
  * @return none.
  *
  */
void register_ble_characteristic_changed_cb(ble_characteristic_changed_callback_t char_changed_cb_fn);

void register_ble_indication_confirmed_cb(ble_indication_confirmed_callback_t indic_conf_cb_fn);

#endif /* BLE_MANAGER_H_INCLUDED */
