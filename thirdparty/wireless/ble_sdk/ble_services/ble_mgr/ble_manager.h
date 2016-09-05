/**
* \file
*
* \brief BLE Manager declarations
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
* Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
*/

// <<< Use Configuration Wizard in Context Menu >>>
// <h> BLE Event Management Configuration
// =======================

#ifndef __BLE_MANAGER_H__
#define __BLE_MANAGER_H__

#include <asf.h>
#include <string.h>
#include <stddef.h>
#include "at_ble_api.h"
#include "ble_utils.h"

#ifndef BLE_DEVICE_ROLE
#warning "BLE Device role is not defined, BLE Device Role defaults to BLE_ROLE_ALL"
#define BLE_DEVICE_ROLE BLE_ROLE_ALL
#endif

#if defined HID_DEVICE
	#ifdef HID_KEYBOARD_DEVICE
	#ifndef BLE_DEVICE_NAME
		#define BLE_DEVICE_NAME				"ATMEL-HID"
	#endif	/* BLE_DEVICE_NAME */
	#define BLE_AUTHENTICATION_LEVEL	(AT_BLE_NO_SEC)
	#define BLE_IO_CAPABALITIES			(AT_BLE_IO_CAP_NO_INPUT_NO_OUTPUT)
	#define BLE_MITM_REQ				(false)
	#define BLE_BOND_REQ				(false)
	#define BLE_PAIR_ENABLE				(false)
	#endif

	#ifdef HID_MOUSE_DEVICE
	#ifndef BLE_DEVICE_NAME
		#define BLE_DEVICE_NAME				"ATMEL-HID"
	#endif	/* BLE_DEVICE_NAME */
	#endif
#endif /* HID_DEVICE */

#if defined PROXIMITY_REPORTER
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME				"ATMEL-PXP"
#endif	/* BLE_DEVICE_NAME */
#endif /* PROXIMITY_REPORTER */

#if defined HR_SENSOR
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME				"ATMEL-HRP"
#endif	/* BLE_DEVICE_NAME */
#endif /* HR_SENSOR */

#if defined BLP_SENSOR
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME				"ATMEL-BLP"
#endif	/* BLE_DEVICE_NAME */
#endif /* Blood_Pressure_Sensor*/

#if (BLE_DEVICE_ROLE == BLE_ROLE_OBSERVER)
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME				"ATMEL-OBS"
#endif	/* BLE_DEVICE_NAME */
#endif /* BLE_DEVICE_ROLE == BLE_ROLE_OBSERVER) */

#if defined ANP_CLIENT
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME				"ATMEL-ANCS"
#endif	/* BLE_DEVICE_NAME */
#endif /* ANCS_CLIENT */

#if defined TIP_CLIENT
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME				"ATMEL-TIP"
#endif	/* BLE_DEVICE_NAME */
#endif /* TIP_CLIENT */

#if defined FIND_ME_TARGET
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME				"ATMEL-FMP"
#endif	/* BLE_DEVICE_NAME */
#endif /* FIND_ME_TARGET */

#if defined ANP_SIG_CLIENT
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME				"ATMEL-ANS"
#endif	/* BLE_DEVICE_NAME */
#endif /* ANP_SIG_CLIENT */

#if defined PAS_CLIENT
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME				"ATMEL-PAS"
#endif	/* BLE_DEVICE_NAME */
#endif /* PAS_CLIENT */


#if defined CSC_DEVICE
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME				"AT-CSC"
#endif	/* BLE_DEVICE_NAME */
#endif /* CSC_DEVICE */

#if defined	BATTERY_SERVICE
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME				"ATMEL-BAS"
#endif	/* BLE_DEVICE_NAME */
#endif /*BATTERY_SERVICE*/


#if defined	SCAN_PARAM_SERVICE
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME				"ATMEL-SCP"
#endif	/* BLE_DEVICE_NAME */
#endif	/*SCAN_PARAM_SERVICE*/

#if defined HTPT_SERVICE
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME "ATMEL-HTP"
#endif  
#endif

#if defined	DEVICE_INFORMATION_SERVICE
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME				"ATMEL-DIS"
#endif	/* BLE_DEVICE_NAME */
#endif	/* DEVICE_INFORMATION_SERVICE */

#if defined PROXIMITY_MONITOR
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME				"ATMEL-MON"
#endif	/* BLE_DEVICE_NAME */
#endif /* PROXIMITY_MONITOR */

/** @brief default device name */
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME				"ATMEL-BLE"
#endif


typedef enum {
	BLE_GAP_EVENT_TYPE,
	BLE_GATT_CLIENT_EVENT_TYPE,
	BLE_GATT_SERVER_EVENT_TYPE,
	BLE_L2CAP_EVENT_TYPE,
	BLE_GATT_HTPT_EVENT_TYPE,
	BLE_DTM_EVENT_TYPE,
	BLE_CUSTOM_EVENT_TYPE
}ble_mgr_event_t;

typedef enum {
	REGISTER_CALL_BACK,
	UNREGISTER_CALL_BACK,
}ble_mgr_event_cb_t;

typedef enum ble_peripheral_state {
	PERIPHERAL_IDLE_STATE,
	PERIPHERAL_ADVERTISING_STATE,
	PERIPHERAL_CONNECTED_STATE,
	PERIPHERAL_PAIRED_STATE,
	PERIPHERAL_ENCRYPTION_STATE,
	PERIPHERAL_DISCONNECTED_STATE,
}ble_peripheral_state_t;

typedef at_ble_status_t (*ble_event_callback_t) (void *params);

/** @brief event timeout */
#define BLE_EVENT_TIMEOUT			(20)

/** @brief pin timeout */
#define PIN_TIMEOUT					30000

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

#ifndef BLE_ATT_DB_MEMORY_SIZE
#define BLE_ATT_DB_MEMORY_SIZE				(1250)  /* Maximum ATT DB memory in Bytes */
#endif

/* BLE Event array of function pointer maximum size */
#define	MAX_GAP_EVENT_SUBSCRIBERS    5
#define MAX_GATT_CLIENT_SUBSCRIBERS  5
#define MAX_GATT_SERVER_SUBSCRIBERS  5
#define MAX_L2CAP_EVENT_SUBSCRIBERS  1
#define MAX_HTPT_EVENT_SUBSCRIBERS   1
#define MAX_DTM_EVENT_SUBSCRIBERS	 5
#define MAX_CUSTOM_EVENT_SUBSCRIBERS 1


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
	/// list of 16bits service solicitation uuid
	LIST_32BIT_SERV_SOLICITATION_UUIDS,
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


/** @brief Service UUID's */

/* Blood Pressure Service UUID */
#define BLOOD_PRESSURE_SERVICE_UUID             (0x1810)

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

/**
 * @brief Heart rate service UUID
 */
#define HEART_RATE_SERVICE_UUID                 (0x180D)

/** HID Service UUID. */
#define HID_SERV_UUID							(0x1812)

/* Phone Alert Service UUID */
#define PAS_SERVICE_UUID						(0x180E)

/** Scan param service uuid */
#define SPS_SERVICE_UUID 						(0x1813)

/** CSC Service UUID. */
#define CSC_SERVICE_UUID						("\x1b\xc5\xd5\xa5\x02\x00\xa6\x85\xe5\x11\x35\x39\xa0\xbb\x5a\xfd")

/** ANCS service UUID. */
#define ANP_ANCS_SERVICE_UUID					("\xD0\x00\x2D\x12\x1E\x4B\x0F\xA4\x99\x4E\xCE\xB5\x31\xF4\x05\x79")

/** HTPT SERVICE UUID */
#define HTPT_SERVICE_UUID						(0x1809)

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

/** minimum connection interval */
//	<o> GAP Minimum Connection Interval in msec <0-1000:50>
//	<i> Defines Minimum interval for GAP Connection.
//	<i> Default: 20
//	<id> gap_conn_interval_min
#define GAP_CONN_INTERVAL_MIN			(20)        //Connection interval min 20ms

/** maximum connection interval */
//	<o> GAP Maximum Connection Interval in msec <0-1000:50>
//	<i> Defines Maximum interval for GAP Connection.
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
#define MAX_SCAN_DEVICE					(20)			  //Max number of scan device

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

/** maximum number of devices connected */
//	<o> Maximum number of device to connect <1-5>
//	<i> Defines the central to connect maximum number to devices
//	<i> Default: 1
//	<id> gap_max_device_connected
#define BLE_MAX_DEVICE_CONNECTED				(5)

#define BLE_EVENT_PARAM_MAX_SIZE		        524

typedef enum {
  BLE_DEVICE_DEFAULT_IDLE,
  BLE_DEVICE_DISCONNECTED,
  BLE_DEVICE_CONNECTED,
  BLE_DEVICE_PAIRING,
  BLE_DEVICE_PAIRING_FAILED,
  BLE_DEVICE_PAIRED,  
  BLE_DEVICE_ENCRYPTION_STATE,
  BLE_DEVICE_ENCRYPTION_FAILED,
  BLE_DEVICE_ENCRYPTION_COMPLETED  
}ble_device_state_t;

typedef struct ble_connected_dev_info
{
	at_ble_connected_t conn_info;
	at_ble_dev_role_t dev_role;
	at_ble_pair_done_t bond_info;
	ble_device_state_t conn_state;
	at_ble_LTK_t host_ltk;
}ble_connected_dev_info_t;


/***************************Advertisement/Scan Response Information configuration *****/

typedef enum {
	LE_PERIPHERAL_ROLE_ONLY = 0x00,
	LE_CENTRAL_ROLE_ONLY = 0x01,
	LE_PERIPHERAL_ROLE_CONN_CENTRAL = 0x02,
	LE_CENTRAL_ROLE_CONN_PERIPHERAL = 0x03	
}le_role_t;

#define SCAN_RESPONSE_ENABLE				0
#define SCAN_RESPONSE_DISABLE				1
#define SCAN_RESPONSE_ONLY_ENABLE			2

/** @brief Length of Adv data types*/
#define APPEARANCE_SIZE						2
#define TX_POWER_LEVEL_SIZE					1
#define ADV_INTERVAL_SIZE					2
#define ADV_TYPE_FLAG_SIZE					3
#define ADV_TYPE_SIZE						1
#define ADV_LENGTH_SIZE						1
#define ADV_ELEMENT_SIZE					2

#ifndef SERVICE_UUID16_MAX_NUM
#define SERVICE_UUID16_MAX_NUM				15
#endif

#ifndef PUBLIC_TARGET_ADDR_MAX_NUM
#define PUBLIC_TARGET_ADDR_MAX_NUM			5
#endif

#ifndef PUBLIC_RANDOM_ADDR_MAX_NUM
#define PUBLIC_RANDOM_ADDR_MAX_NUM			5
#endif

#ifndef SERVICE_UUID32_MAX_NUM
#define SERVICE_UUID32_MAX_NUM				5
#endif

#ifndef SERVICE_UUID128_MAX_NUM
#define SERVICE_UUID128_MAX_NUM				2
#endif

#ifdef TP_ANDROID
#ifdef CURRENT_TIME_SERVICE
#define BLE_GAP_ADV_SERVICE_16BIT_UUID_ENABLE			true
#endif
#else
#ifdef CURRENT_TIME_SERVICE
#define BLE_GAP_ADV_SERVICE_16BIT_UUID_ENABLE			false
#endif
#endif

/** UUID Type's and configuration's */
#ifndef BLE_GAP_ADV_SERVICE_16BIT_UUID_ENABLE
#define BLE_GAP_ADV_SERVICE_16BIT_UUID_ENABLE			true
#endif
#define BLE_GAP_ADV_SERVICE_16BIT_UUID_LENGTH			2
#if BLE_GAP_ADV_SERVICE_16BIT_UUID_ENABLE
#define BLE_GAP_ADV_SERVICE_16BIT_UUID_SCN_RSP_ENABLE	SCAN_RESPONSE_ENABLE

#ifndef IMMEDIATE_ALERT_SERVICE
#define BLE_GAP_ADV_SERVICE_16BIT_UUID0_ENABLE			false
#else
#define BLE_GAP_ADV_SERVICE_16BIT_UUID0_ENABLE			true
#endif
#define  BLE_GAP_ADV_DATA_SERVICE_16BIT_UUID0_VAL		IMMEDIATE_ALERT_SERVICE_UUID

#ifndef SCAN_PARAM_SERVICE
#define BLE_GAP_ADV_SERVICE_16BIT_UUID1_ENABLE			false
#else
#define BLE_GAP_ADV_SERVICE_16BIT_UUID1_ENABLE			true
#endif
#define  BLE_GAP_ADV_DATA_SERVICE_16BIT_UUID1_VAL		SPS_SERVICE_UUID

#ifndef LINK_LOSS_SERVICE
#define BLE_GAP_ADV_SERVICE_16BIT_UUID2_ENABLE			false
#else
#define BLE_GAP_ADV_SERVICE_16BIT_UUID2_ENABLE			true
#endif
#define  BLE_GAP_ADV_DATA_SERVICE_16BIT_UUID2_VAL		LINK_LOSS_SERVICE_UUID

#ifndef TX_POWER_SERVICE
#define BLE_GAP_ADV_SERVICE_16BIT_UUID3_ENABLE			false
#else
#define BLE_GAP_ADV_SERVICE_16BIT_UUID3_ENABLE			true
#endif
#define  BLE_GAP_ADV_DATA_SERVICE_16BIT_UUID3_VAL		TX_POWER_SERVICE_UUID

#ifndef CURRENT_TIME_SERVICE
#define BLE_GAP_ADV_SERVICE_16BIT_UUID4_ENABLE			false
#else
#define BLE_GAP_ADV_SERVICE_16BIT_UUID4_ENABLE			true
#endif
#define  BLE_GAP_ADV_DATA_SERVICE_16BIT_UUID4_VAL		CURRENT_TIME_SERVICE_UUID

#ifndef REFERENCE_TIME_SERVICE
#define BLE_GAP_ADV_SERVICE_16BIT_UUID5_ENABLE			false
#else
#define BLE_GAP_ADV_SERVICE_16BIT_UUID5_ENABLE			true
#endif
#define  BLE_GAP_ADV_DATA_SERVICE_16BIT_UUID5_VAL		REFERENCE_TIME_SERVICE_UUID

#ifndef	NEXT_DST_SERVICE
#define BLE_GAP_ADV_SERVICE_16BIT_UUID6_ENABLE			false
#else
#define BLE_GAP_ADV_SERVICE_16BIT_UUID6_ENABLE			true
#endif
#define  BLE_GAP_ADV_DATA_SERVICE_16BIT_UUID6_VAL		NEXT_DST_SERVICE_UUID

#ifndef DEVICE_INFORMATION_SERVICE
#define BLE_GAP_ADV_SERVICE_16BIT_UUID7_ENABLE			false
#else
#define BLE_GAP_ADV_SERVICE_16BIT_UUID7_ENABLE			true
#endif
#define  BLE_GAP_ADV_DATA_SERVICE_16BIT_UUID7_VAL		DIS_SERVICE_UUID

#ifndef BATTERY_SERVICE
#define BLE_GAP_ADV_SERVICE_16BIT_UUID8_ENABLE			false
#else
#define BLE_GAP_ADV_SERVICE_16BIT_UUID8_ENABLE			true
#endif
#define  BLE_GAP_ADV_DATA_SERVICE_16BIT_UUID8_VAL		BAT_SERVICE_UUID

#ifndef ANS_CLIENT_SERVICE
#define BLE_GAP_ADV_SERVICE_16BIT_UUID9_ENABLE			false
#else
#define BLE_GAP_ADV_SERVICE_16BIT_UUID9_ENABLE			true
#endif
#define  BLE_GAP_ADV_DATA_SERVICE_16BIT_UUID9_VAL		ANP_SERVICE_UUID

#ifndef HID_SERVICE
#define BLE_GAP_ADV_SERVICE_16BIT_UUID10_ENABLE			false
#else
#define BLE_GAP_ADV_SERVICE_16BIT_UUID10_ENABLE			true
#endif
#define  BLE_GAP_ADV_DATA_SERVICE_16BIT_UUID10_VAL		HID_SERV_UUID

#ifndef BLOOD_PRESSURE_SERVICE
#define BLE_GAP_ADV_SERVICE_16BIT_UUID11_ENABLE			false
#else
#define BLE_GAP_ADV_SERVICE_16BIT_UUID11_ENABLE			true
#endif
#define  BLE_GAP_ADV_DATA_SERVICE_16BIT_UUID11_VAL		BLOOD_PRESSURE_SERVICE_UUID

#ifndef HEART_RATE_SERVICE
#define BLE_GAP_ADV_SERVICE_16BIT_UUID12_ENABLE			false
#else
#define BLE_GAP_ADV_SERVICE_16BIT_UUID12_ENABLE			true
#endif
#define BLE_GAP_ADV_DATA_SERVICE_16BIT_UUID12_VAL		HEART_RATE_SERVICE_UUID


#ifndef PAS_CLIENT
#define BLE_GAP_ADV_SERVICE_16BIT_UUID13_ENABLE			false
#else
#define BLE_GAP_ADV_SERVICE_16BIT_UUID13_ENABLE			true
#endif
#define BLE_GAP_ADV_DATA_SERVICE_16BIT_UUID13_VAL		PAS_SERVICE_UUID

#ifndef HTPT_SERVICE
#define BLE_GAP_ADV_SERVICE_16BIT_UUID14_ENABLE			false
#else
#define BLE_GAP_ADV_SERVICE_16BIT_UUID14_ENABLE			true
#endif
#define  BLE_GAP_ADV_DATA_SERVICE_16BIT_UUID14_VAL		HTPT_SERVICE_UUID

#endif

#define BLE_GAP_ADV_SERVICE_32BIT_UUID_ENABLE			false
#define BLE_GAP_ADV_SERVICE_32BIT_UUID_LENGTH			4
#if BLE_GAP_ADV_SERVICE_32BIT_UUID_ENABLE
#define BLE_GAP_ADV_SERVICE_32BIT_UUID_SCN_RSP_ENABLE	SCAN_RESPONSE_ENABLE

#define BLE_GAP_ADV_SERVICE_32BIT_UUID0_ENABLE			true
#define  BLE_GAP_ADV_DATA_SERVICE_32BIT_UUID0_VAL		"\x12\x34\x56\x78"

#define BLE_GAP_ADV_SERVICE_32BIT_UUID1_ENABLE			false
#define  BLE_GAP_ADV_DATA_SERVICE_32BIT_UUID1_VAL		"\x12\x34\x56\x78"

#define BLE_GAP_ADV_SERVICE_32BIT_UUID2_ENABLE			false
#define  BLE_GAP_ADV_DATA_SERVICE_32BIT_UUID2_VAL		"\x12\x34\x56\x78"

#define BLE_GAP_ADV_SERVICE_32BIT_UUID3_ENABLE			false
#define  BLE_GAP_ADV_DATA_SERVICE_32BIT_UUID3_VAL		"\x12\x34\x56\x78"

#define BLE_GAP_ADV_SERVICE_32BIT_UUID4_ENABLE			false
#define  BLE_GAP_ADV_DATA_SERVICE_32BIT_UUID4_VAL		"\x12\x34\x56\x78"
#endif


#ifdef CSC_SERVICE
#define BLE_GAP_ADV_SERVICE_128BIT_UUID_ENABLE			true
#else
#define BLE_GAP_ADV_SERVICE_128BIT_UUID_ENABLE			false
#endif

#define BLE_GAP_ADV_SERVICE_128BIT_UUID_LENGTH			16
#if BLE_GAP_ADV_SERVICE_128BIT_UUID_ENABLE
#define BLE_GAP_ADV_SERVICE_128BIT_UUID_SCN_RSP_ENABLE	SCAN_RESPONSE_ENABLE

#ifndef ANCS_CLIENT_SERVICE 
#define BLE_GAP_ADV_SERVICE_128BIT_UUID0_ENABLE			false
#else
#define BLE_GAP_ADV_SERVICE_128BIT_UUID0_ENABLE			true
#endif
#define  BLE_GAP_ADV_DATA_SERVICE_128BIT_UUID0_VAL		ANP_ANCS_SERVICE_UUID

#ifndef CSC_SERVICE
#define BLE_GAP_ADV_SERVICE_128BIT_UUID1_ENABLE			false
#else
#define BLE_GAP_ADV_SERVICE_128BIT_UUID1_ENABLE			true
#endif
#define  BLE_GAP_ADV_DATA_SERVICE_128BIT_UUID1_VAL		CSC_SERVICE_UUID
#endif

/** Short Local Name configuration */
#define BLE_GAP_ADV_SHORTENED_LOCAL_NAME_ENABLE				false
#if BLE_GAP_ADV_SHORTENED_LOCAL_NAME_ENABLE
#define BLE_GAP_ADV_SHORTENED_LOCAL_NAME_SCN_RSP_ENABLE		SCAN_RESPONSE_DISABLE
#define BLE_GAP_ADV_DATA_SHORTENED_LOCAL_NAME				"ATMEL-BLE"
#define	 BLE_GAP_ADV_DATA_SHORTENED_LOCAL_NAME_LEN			strlen(BLE_GAP_ADV_DATA_SHORTENED_LOCAL_NAME)
#endif

/* Complete Local Name Configuration */
#define BLE_GAP_ADV_COMPLETE_LOCAL_NAME_ENABLE				true
#if BLE_GAP_ADV_COMPLETE_LOCAL_NAME_ENABLE
#define BLE_GAP_ADV_COMPLETE_LOCAL_NAME_SCN_RSP_ENABLE		SCAN_RESPONSE_ENABLE
#define  BLE_GAP_ADV_DATA_COMPLETE_LOCAL_NAME				BLE_DEVICE_NAME
#define BLE_GAP_ADV_DATA_COMPLETE_LOCAL_NAME_LENGTH			strlen(BLE_GAP_ADV_DATA_COMPLETE_LOCAL_NAME)
#endif

/* Tx Power Configuration */
#define BLE_GAP_ADV_TX_POWER_ENABLE							false
#if BLE_GAP_ADV_TX_POWER_ENABLE
#define BLE_GAP_ADV_TX_POWER_SCN_RSP_ENABLE					SCAN_RESPONSE_ENABLE
#define BLE_GAP_ADV_DATA_TX_POWER							AT_BLE_TX_PWR_LVL_NEG_20_DB
#define	BLE_GAP_ADV_DATA_TX_POWER_SIZE						1
#endif

/* Connection Interval Range configuration */
#define BLE_GAP_ADV_SLAVE_CONN_INTERVAL_RANGE_ENABLE			false
#define BLE_GAP_ADV_DATA_SLAVE_CONN_INTERVAL_RANGE_LENGTH		2
#if BLE_GAP_ADV_SLAVE_CONN_INTERVAL_RANGE_ENABLE
#define BLE_GAP_ADV_SLAVE_CONN_INTERVAL_RANGE_SCN_RSP_ENABLE	SCAN_RESPONSE_ENABLE

#define BLE_GAP_ADV_DATA_SLAVE_CONN_INTERVAL_RANGE_MIN			0x0006
#define BLE_GAP_ADV_DATA_SLAVE_CONN_INTERVAL_RANGE_MAX			0x0C80
#endif


#ifndef TP_ANDROID
#ifdef CURRENT_TIME_SERVICE
#define BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID_ENABLE				true
#endif
#endif

/* Service Solicitation UUID Configuration */
#ifndef BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID_ENABLE
#define BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID_ENABLE				false
#endif
#if BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID_ENABLE
#define BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID_SCN_RSP_ENABLE		SCAN_RESPONSE_ENABLE

#ifndef IMMEDIATE_ALERT_SERVICE
#define BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID0_ENABLE			false
#else
#define BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID0_ENABLE			true
#endif
#define  BLE_GAP_ADV_DATA_SERVICE_SOLTN_16BIT_UUID0_VAL			IMMEDIATE_ALERT_SERVICE_UUID

#ifndef SCAN_PARAM_SERVICE
#define BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID1_ENABLE			false
#else
#define BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID1_ENABLE			true
#endif
#define  BLE_GAP_ADV_DATA_SERVICE_SOLTN_16BIT_UUID1_VAL			SPS_SERVICE_UUID

#ifndef LINK_LOSS_SERVICE
#define BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID2_ENABLE			false
#else
#define BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID2_ENABLE			true
#endif
#define  BLE_GAP_ADV_DATA_SERVICE_SOLTN_16BIT_UUID2_VAL			LINK_LOSS_SERVICE_UUID

#ifndef TX_POWER_SERVICE
#define BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID3_ENABLE			false
#else
#define BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID3_ENABLE			true
#endif
#define  BLE_GAP_ADV_DATA_SERVICE_SOLTN_16BIT_UUID3_VAL			TX_POWER_SERVICE_UUID

#ifndef CURRENT_TIME_SERVICE
#define BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID4_ENABLE			false
#else
#define BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID4_ENABLE			true
#endif
#define  BLE_GAP_ADV_DATA_SERVICE_SOLTN_16BIT_UUID4_VAL			CURRENT_TIME_SERVICE_UUID

#ifndef REFERENCE_TIME_SERVICE
#define BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID5_ENABLE			false
#else
#define BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID5_ENABLE			true
#endif
#define  BLE_GAP_ADV_DATA_SERVICE_SOLTN_16BIT_UUID5_VAL			REFERENCE_TIME_SERVICE_UUID

#ifndef NEXT_DST_SERVICE
#define BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID6_ENABLE			false
#else
#define BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID6_ENABLE			true
#endif
#define  BLE_GAP_ADV_DATA_SERVICE_SOLTN_16BIT_UUID6_VAL			NEXT_DST_SERVICE_UUID

#ifndef DEVICE_INFORMATION_SERVICE
#define BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID7_ENABLE			false
#else
#define BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID7_ENABLE			true
#endif
#define  BLE_GAP_ADV_DATA_SERVICE_SOLTN_16BIT_UUID7_VAL			DIS_SERVICE_UUID

#ifndef BATTERY_SERVICE
#define BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID8_ENABLE			false
#else
#define BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID8_ENABLE			true
#endif
#define  BLE_GAP_ADV_DATA_SERVICE_SOLTN_16BIT_UUID8_VAL			BAT_SERVICE_UUID

#ifndef ANS_CLIENT_SERVICE
#define BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID9_ENABLE			false
#else
#define BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID9_ENABLE			true
#endif
#define  BLE_GAP_ADV_DATA_SERVICE_SOLTN_16BIT_UUID9_VAL			ANP_SERVICE_UUID

#ifndef HID_SERVICE
#define BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID10_ENABLE			false
#else
#define BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID10_ENABLE			true
#endif
#define  BLE_GAP_ADV_DATA_SERVICE_SOLTN_16BIT_UUID10_VAL		HID_SERV_UUID

#ifndef BLOOD_PRESSURE_SERVICE
#define BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID11_ENABLE			false
#else
#define BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID11_ENABLE			true
#endif
#define  BLE_GAP_ADV_DATA_SERVICE_SOLTN_16BIT_UUID11_VAL		BLOOD_PRESSURE_SERVICE_UUID

#ifndef HEART_RATE_SERVICE
#define BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID12_ENABLE			false
#else
#define BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID12_ENABLE			true
#endif
#define  BLE_GAP_ADV_DATA_SERVICE_SOLTN_16BIT_UUID12_VAL		HEART_RATE_SERVICE_UUID

#ifndef PAS_CLIENT
#define BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID13_ENABLE			false
#else
#define BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID13_ENABLE			true
#endif
#define  BLE_GAP_ADV_DATA_SERVICE_SOLTN_16BIT_UUID13_VAL		PAS_SERVICE_UUID

#ifndef HTPT_SERVICE
#define BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID14_ENABLE			false
#else
#define BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID14_ENABLE			true
#endif
#define  BLE_GAP_ADV_DATA_SERVICE_SOLTN_16BIT_UUID14_VAL		HTPT_SERVICE_UUID

#endif

#define BLE_GAP_ADV_SERVICE_SOLTN_32BIT_UUID_ENABLE				false
#if BLE_GAP_ADV_SERVICE_SOLTN_32BIT_UUID_ENABLE
#define BLE_GAP_ADV_SERVICE_SOLTN_32BIT_UUID_SCN_RSP_ENABLE		SCAN_RESPONSE_ENABLE

#define BLE_GAP_ADV_SERVICE_SOLTN_32BIT_UUID0_ENABLE			true
#define  BLE_GAP_ADV_DATA_SERVICE_SOLTN_32BIT_UUID0_VAL			"\x12\x34\x56\x78"

#define BLE_GAP_ADV_SERVICE_SOLTN_32BIT_UUID1_ENABLE			false
#define  BLE_GAP_ADV_DATA_SERVICE_SOLTN_32BIT_UUID1_VAL			"\x12\x34\x56\x78"

#define BLE_GAP_ADV_SERVICE_SOLTN_32BIT_UUID2_ENABLE			false
#define  BLE_GAP_ADV_DATA_SERVICE_SOLTN_32BIT_UUID2_VAL			"\x12\x34\x56\x78"

#define BLE_GAP_ADV_SERVICE_SOLTN_32BIT_UUID3_ENABLE			false
#define  BLE_GAP_ADV_DATA_SERVICE_SOLTN_32BIT_UUID3_VAL			"\x12\x34\x56\x78"

#define BLE_GAP_ADV_SERVICE_SOLTN_32BIT_UUID4_ENABLE			false
#define  BLE_GAP_ADV_DATA_SERVICE_SOLTN_32BIT_UUID4_VAL			"\x12\x34\x56\x78"
#endif


#define BLE_GAP_ADV_SERVICE_SOLTN_128BIT_UUID_ENABLE			true
#if BLE_GAP_ADV_SERVICE_SOLTN_128BIT_UUID_ENABLE
#define BLE_GAP_ADV_SERVICE_SOLTN_128BIT_UUID_SCN_RSP_ENABLE	SCAN_RESPONSE_ENABLE

#ifndef ANCS_CLIENT_SERVICE
#define BLE_GAP_ADV_SERVICE_SOLTN_128BIT_UUID0_ENABLE			false
#else
#define BLE_GAP_ADV_SERVICE_SOLTN_128BIT_UUID0_ENABLE			true
#endif
#define  BLE_GAP_ADV_DATA_SERVICE_SOLTN_128BIT_UUID0_VAL		ANP_ANCS_SERVICE_UUID

#ifndef CSC_SERVICE
#define BLE_GAP_ADV_SERVICE_SOLTN_128BIT_UUID1_ENABLE			false
#else
#define BLE_GAP_ADV_SERVICE_SOLTN_128BIT_UUID1_ENABLE			true
#endif
#define  BLE_GAP_ADV_DATA_SERVICE_SOLTN_128BIT_UUID1_VAL		CSC_SERVICE_UUID
#endif

/* Service Data Configuration */
#define BLE_GAP_ADV_SERVICE_DATA_16BIT_UUID_ENABLE					false
#if BLE_GAP_ADV_SERVICE_DATA_16BIT_UUID_ENABLE
#define BLE_GAP_ADV_SERVICE_DATA_16BIT_UUID_SCN_RSP_ENABLE			SCAN_RESPONSE_ENABLE
#define BLE_GAP_ADV_DATA_SERVICE_DATA_16BIT_UUID					TX_POWER_SERVICE_UUID
#define BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_16BIT_UUID			"\x12"
#define BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_16BIT_UUID_LENGTH	(sizeof(BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_16BIT_UUID) - 1)
#endif

#define BLE_GAP_ADV_SERVICE_DATA_32BIT_UUID_ENABLE					false
#if BLE_GAP_ADV_SERVICE_DATA_32BIT_UUID_ENABLE
#define BLE_GAP_ADV_SERVICE_DATA_32BIT_UUID_SCN_RSP_ENABLE			SCAN_RESPONSE_ENABLE
#define BLE_GAP_ADV_DATA_SERVICE_DATA_32BIT_UUID					"\x12\x34\x56\x78"
#define BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_32BIT_UUID			"\x12\x34"
#define BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_32BIT_UUID_LENGTH	(sizeof(BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_32BIT_UUID) - 1)
#endif

#define BLE_GAP_ADV_SERVICE_DATA_128BIT_UUID_ENABLE					false
#if BLE_GAP_ADV_SERVICE_DATA_128BIT_UUID_ENABLE
#define BLE_GAP_ADV_SERVICE_DATA_128BIT_UUID_SCN_RSP_ENABLE			SCAN_RESPONSE_ENABLE
#define BLE_GAP_ADV_DATA_SERVICE_DATA_128BIT_UUID					"\x1b\xc5\xd5\xa5\x02\x00\xa6\x85\xe5\x11\x35\x39\xa0\xbb\x5a\xfd"
#define BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_128BIT_UUID		"\x12\x34"
#define BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_128BIT_UUID_LENGTH	(sizeof(BLE_GAP_ADV_ADDITIONAL_DATA_SERVICE_DATA_128BIT_UUID) - 1)
#endif

/* Public Target Address Configuration */
#define BLE_GAP_ADV_PUBLIC_TARGET_ADDR_ENABLE					false
#define BLE_GAP_ADV_PUBLIC_TARGET_ADDR_LENGTH					sizeof(at_ble_addr_t)
#if BLE_GAP_ADV_PUBLIC_TARGET_ADDR_ENABLE
#define BLE_GAP_ADV_PUBLIC_TARGET_ADDR_SCN_RSP_ENABLE			SCAN_RESPONSE_ENABLE

#define BLE_GAP_ADV_DATA_PUBLIC_TARGET_ADDR0_ENABLE				true
#define BLE_GAP_ADV_DATA_PUBLIC_TARGET_ADDR0_VAL				"\x00\xAB\xCD\xEF\xAB\xCD\xEA"

#define BLE_GAP_ADV_DATA_PUBLIC_TARGET_ADDR1_ENABLE				false
#define BLE_GAP_ADV_DATA_PUBLIC_TARGET_ADDR1_VAL				"\x00\xAB\xCD\xEF\xAB\xCD\xEB"

#define BLE_GAP_ADV_DATA_PUBLIC_TARGET_ADDR2_ENABLE				false
#define BLE_GAP_ADV_DATA_PUBLIC_TARGET_ADDR2_VAL				"\x00\xAB\xCD\xEF\xAB\xCD\xEC"

#define BLE_GAP_ADV_DATA_PUBLIC_TARGET_ADDR3_ENABLE				false
#define BLE_GAP_ADV_DATA_PUBLIC_TARGET_ADDR3_VAL				"\x00\xAB\xCD\xEF\xAB\xCD\xED"

#define BLE_GAP_ADV_DATA_PUBLIC_TARGET_ADDR4_ENABLE				false
#define BLE_GAP_ADV_DATA_PUBLIC_TARGET_ADDR4_VAL				"\x00\xAB\xCD\xEF\xAB\xCD\xEE"
#endif

/* Random Target Address Configuration */
#define BLE_GAP_ADV_RANDOM_TARGET_ADDR_ENABLE					false
#define BLE_GAP_ADV_RANDOM_TARGET_ADDR_LENGTH					sizeof(at_ble_addr_t)
#if BLE_GAP_ADV_RANDOM_TARGET_ADDR_ENABLE
#define BLE_GAP_ADV_RANDOM_TARGET_ADDR_SCN_RSP_ENABLE			SCAN_RESPONSE_ENABLE

#define BLE_GAP_ADV_DATA_RANDOM_TARGET_ADDR0_ENABLE				true
#define BLE_GAP_ADV_DATA_RANDOM_TARGET_ADDR0_VAL				"\x00\xAB\xCD\xEF\xAB\xCD\xEA"

#define BLE_GAP_ADV_DATA_RANDOM_TARGET_ADDR1_ENABLE				false
#define BLE_GAP_ADV_DATA_RANDOM_TARGET_ADDR1_VAL				"\x00\xAB\xCD\xEF\xAB\xCD\xEB"

#define BLE_GAP_ADV_DATA_RANDOM_TARGET_ADDR2_ENABLE				false
#define BLE_GAP_ADV_DATA_RANDOM_TARGET_ADDR2_VAL				"\x00\xAB\xCD\xEF\xAB\xCD\xEC"

#define BLE_GAP_ADV_DATA_RANDOM_TARGET_ADDR3_ENABLE				false
#define BLE_GAP_ADV_DATA_RANDOM_TARGET_ADDR3_VAL				"\x00\xAB\xCD\xEF\xAB\xCD\xED"

#define BLE_GAP_ADV_DATA_RANDOM_TARGET_ADDR4_ENABLE				false
#define BLE_GAP_ADV_DATA_RANDOM_TARGET_ADDR4_VAL				"\x00\xAB\xCD\xEF\xAB\xCD\xEE"
#endif

/* Appearance configuration */
#ifdef HID_DEVICE
#define BLE_GAP_ADV_APPEARANCE_ENABLE							true
#else
#define BLE_GAP_ADV_APPEARANCE_ENABLE							false
#endif

#ifndef BLE_GAP_ADV_APPEARANCE_ENABLE
#define BLE_GAP_ADV_APPEARANCE_ENABLE							false
#endif
#if BLE_GAP_ADV_APPEARANCE_ENABLE
#define BLE_GAP_ADV_APPEARANCE_SCN_RSP_ENABLE					SCAN_RESPONSE_ENABLE
#ifdef HID_MOUSE_DEVICE
#define BLE_GAP_ADV_DATA_APPEARANCE								"\xc2\x03"
#else
#ifdef HID_KEYBOARD_DEVICE
#define BLE_GAP_ADV_DATA_APPEARANCE								"\xc1\x03"
#endif /*HID_MOUSE_DEVICE*/
#endif /*HID_KEYBOARD_DEVICE*/
#ifndef BLE_GAP_ADV_DATA_APPEARANCE
#define BLE_GAP_ADV_DATA_APPEARANCE								"\x80\x00"
#endif /*BLE_GAP_ADV_DATA_APPEARANCE*/
#define BLE_GAP_ADV_DATA_APPEARANCE_SIZE						2
#endif

/* Advertising Interval Configuration */
#define BLE_GAP_ADV_ADVERTISING_INTERVAL_ENABLE						false
#define BLE_GAP_ADV_ADVERTISING_INTERVAL_LENGTH						2
#if BLE_GAP_ADV_ADVERTISING_INTERVAL_ENABLE
#define BLE_GAP_ADV_ADVERTISING_INTERVAL_SCN_RSP_ENABLE				SCAN_RESPONSE_ENABLE
#define BLE_GAP_ADV_DATA_ADVERTISING_INTERVAL						0x64
#endif

/* ?LE Bluetooth Device Address Configuration */
#define BLE_GAP_ADV_LE_BT_DEVICE_ADDR_ENABLE						false
#if BLE_GAP_ADV_LE_BT_DEVICE_ADDR_ENABLE
#define BLE_GAP_ADV_LE_BT_DEVICE_ADDR_SCN_RSP_ENABLE				SCAN_RESPONSE_ENABLE
#define BLE_GAP_ADV_DATA_LE_BT_DEVICE_ADDR							"\x00\xAB\xCD\xEF\xAB\xCD\xEF"
#define BLE_GAP_ADV_DATA_LE_BT_DEVICE_ADDR_LENGTH					sizeof(at_ble_addr_t)
#endif

/* ?LE Role Configuration */
#define BLE_GAP_ADV_LE_ROLE_ENABLE									false
#if BLE_GAP_ADV_LE_ROLE_ENABLE
#define BLE_GAP_ADV_LE_ROLE_SCN_RSP_ENABLE							SCAN_RESPONSE_ENABLE
#ifdef BLE_DEVICE_ROLE == BLE_ROLE_PERIPHERAL
#define BLE_GAP_ADV_DATA_LE_ROLE									LE_PERIPHERAL_ROLE_ONLY
#elif BLE_DEVICE_ROLE == BLE_ROLE_CENTRAL
#define BLE_GAP_ADV_DATA_LE_ROLE									LE_CENTRAL_ROLE_ONLY
#elif BLE_DEVICE_ROLE == BLE_ROLE_ALL
#define BLE_GAP_ADV_DATA_LE_ROLE									LE_CENTRAL_ROLE_CONN_PERIPHERAL
//Note: LE Peripheral Preferred connection should be changed manually here
#endif
#define BLE_GAP_ADV_DATA_LE_ROLE_SIZE								1
#endif

/* Manufacturer Specific Data Configuration */
#define BLE_GAP_ADV_MANUFACTURER_SPECIFIC_DATA_ENABLE				true
#if	BLE_GAP_ADV_MANUFACTURER_SPECIFIC_DATA_ENABLE
#define BLE_GAP_ADV_MANUFACTURER_SPECIFIC_DATA_SCN_RSP_ENABLE		SCAN_RESPONSE_ENABLE
#define BLE_GAP_ADV_DATA_MANUFACTURER_SPECIFIC_DATA					"\x00\x06\xd6\xb2\xf0\x05\xf0\xf8"
#define BLE_GAP_ADV_DATA_MANUFACTURER_SPECIFIC_DATA_SIZE			(sizeof(BLE_GAP_ADV_DATA_MANUFACTURER_SPECIFIC_DATA) - 1)
#endif

typedef struct adv_element_container{
	uint8_t len;
	uint8_t *adv_ptr;
}adv_element_container_t;

typedef struct scan_resp_element{
	uint8_t len;
	uint8_t *scn_ptr;
}scan_resp_element_t;

#define  _CONF_SERVICE_16BIT_UUID(n, uuid_ptr) \
{ \
	if (BLE_GAP_ADV_SERVICE_16BIT_UUID##n##_ENABLE) \
	{\
		adv_element_container_t *adv_ptr;\
		adv_ptr = (adv_element_container_t *)uuid_ptr;\
		if((adv_ptr->len + BLE_GAP_ADV_SERVICE_16BIT_UUID_LENGTH) >  (AT_BLE_ADV_MAX_SIZE - ADV_TYPE_FLAG_SIZE))\
		{ \
			DBG_LOG("Adv Data too big");\
			while(1);\
		} \
		adv_ptr->adv_ptr[adv_ptr->len++] = (uint8_t) BLE_GAP_ADV_DATA_SERVICE_16BIT_UUID##n##_VAL; \
		adv_ptr->adv_ptr[adv_ptr->len++] = (uint8_t) ((BLE_GAP_ADV_DATA_SERVICE_16BIT_UUID##n##_VAL) >> 8); \
	}\
}

#define  _CONF_SERVICE_16BIT_UUID_SCAN_RSP(n, uuid_ptr) \
{ \
	if (BLE_GAP_ADV_SERVICE_16BIT_UUID##n##_ENABLE) \
	{\
		adv_element_container_t *adv_ptr;\
		adv_ptr = (adv_element_container_t *)uuid_ptr;\
		if((adv_ptr->len + BLE_GAP_ADV_SERVICE_16BIT_UUID_LENGTH) >  (AT_BLE_ADV_MAX_SIZE))\
		{ \
			DBG_LOG("Adv Data too big");\
			while(1);\
		} \
		adv_ptr->adv_ptr[adv_ptr->len++] = (uint8_t) BLE_GAP_ADV_DATA_SERVICE_16BIT_UUID##n##_VAL; \
		adv_ptr->adv_ptr[adv_ptr->len++] = (uint8_t) ((BLE_GAP_ADV_DATA_SERVICE_16BIT_UUID##n##_VAL) >> 8); \
	}\
}

#define  _CONF_SERVICE_32BIT_UUID(n, uuid_ptr) \
{ \
	if (BLE_GAP_ADV_SERVICE_32BIT_UUID##n##_ENABLE) \
	{\
		adv_element_container_t *adv_ptr;\
		adv_ptr = (adv_element_container_t *)uuid_ptr;\
		if((adv_ptr->len + BLE_GAP_ADV_SERVICE_32BIT_UUID_LENGTH) >  (AT_BLE_ADV_MAX_SIZE - ADV_TYPE_FLAG_SIZE))\
		{ \
			DBG_LOG("Adv Data too big");\
			while(1);\
		} \
		memcpy(&adv_ptr->adv_ptr[adv_ptr->len], BLE_GAP_ADV_DATA_SERVICE_32BIT_UUID##n##_VAL, BLE_GAP_ADV_SERVICE_32BIT_UUID_LENGTH);\
		adv_ptr->len += BLE_GAP_ADV_SERVICE_32BIT_UUID_LENGTH; \
	}\
}

#define  _CONF_SERVICE_32BIT_UUID_SCAN_RSP(n, uuid_ptr) \
{ \
	if (BLE_GAP_ADV_SERVICE_32BIT_UUID##n##_ENABLE) \
	{\
		adv_element_container_t *adv_ptr;\
		adv_ptr = (adv_element_container_t *)uuid_ptr;\
		if((adv_ptr->len + BLE_GAP_ADV_SERVICE_32BIT_UUID_LENGTH) >  (AT_BLE_ADV_MAX_SIZE))\
		{ \
			DBG_LOG("Adv Data too big");\
			while(1);\
		} \
		memcpy(&adv_ptr->adv_ptr[adv_ptr->len], BLE_GAP_ADV_DATA_SERVICE_32BIT_UUID##n##_VAL, BLE_GAP_ADV_SERVICE_32BIT_UUID_LENGTH);\
		adv_ptr->len += BLE_GAP_ADV_SERVICE_32BIT_UUID_LENGTH; \
	}\
}

#define  _CONF_SERVICE_128BIT_UUID(n, uuid_ptr) \
{ \
	if (BLE_GAP_ADV_SERVICE_128BIT_UUID##n##_ENABLE) \
	{\
		adv_element_container_t *adv_ptr;\
		adv_ptr = (adv_element_container_t *)uuid_ptr;\
		if((adv_ptr->len + BLE_GAP_ADV_SERVICE_128BIT_UUID_LENGTH) >  (AT_BLE_ADV_MAX_SIZE - ADV_TYPE_FLAG_SIZE))\
		{ \
			DBG_LOG("Adv Data too big");\
			while(1);\
		} \
		memcpy(&adv_ptr->adv_ptr[adv_ptr->len], BLE_GAP_ADV_DATA_SERVICE_128BIT_UUID##n##_VAL , BLE_GAP_ADV_SERVICE_128BIT_UUID_LENGTH);\
		adv_ptr->len += BLE_GAP_ADV_SERVICE_128BIT_UUID_LENGTH; \
	}\
}

#define  _CONF_SERVICE_128BIT_UUID_SCAN_RSP(n, uuid_ptr) \
{ \
	if (BLE_GAP_ADV_SERVICE_128BIT_UUID##n##_ENABLE) \
	{\
		adv_element_container_t *adv_ptr;\
		adv_ptr = (adv_element_container_t *)uuid_ptr;\
		if((adv_ptr->len + BLE_GAP_ADV_SERVICE_128BIT_UUID_LENGTH) >  (AT_BLE_ADV_MAX_SIZE))\
		{ \
			DBG_LOG("Adv Data too big");\
			while(1);\
		} \
		memcpy(&adv_ptr->adv_ptr[adv_ptr->len], BLE_GAP_ADV_DATA_SERVICE_128BIT_UUID##n##_VAL , BLE_GAP_ADV_SERVICE_128BIT_UUID_LENGTH);\
		adv_ptr->len += BLE_GAP_ADV_SERVICE_128BIT_UUID_LENGTH; \
	}\
}

#define  _CONF_SERVICE_SOLTN_16BIT_UUID(n, uuid_ptr) \
{ \
	if (BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID##n##_ENABLE) \
	{\
		adv_element_container_t *adv_ptr;\
		adv_ptr = (adv_element_container_t *)uuid_ptr;\
		if((adv_ptr->len + BLE_GAP_ADV_SERVICE_16BIT_UUID_LENGTH) >  (AT_BLE_ADV_MAX_SIZE - ADV_TYPE_FLAG_SIZE))\
		{ \
			DBG_LOG("Adv Data too big");\
			while(1);\
		} \
		adv_ptr->adv_ptr[adv_ptr->len++] = (uint8_t) BLE_GAP_ADV_DATA_SERVICE_SOLTN_16BIT_UUID##n##_VAL; \
		adv_ptr->adv_ptr[adv_ptr->len++] = (uint8_t) ((BLE_GAP_ADV_DATA_SERVICE_SOLTN_16BIT_UUID##n##_VAL) >> 8); \
	}\
}

#define  _CONF_SERVICE_SOLTN_16BIT_UUID_SCAN_RSP(n, uuid_ptr) \
{ \
	if (BLE_GAP_ADV_SERVICE_SOLTN_16BIT_UUID##n##_ENABLE) \
	{\
		adv_element_container_t *adv_ptr;\
		adv_ptr = (adv_element_container_t *)uuid_ptr;\
		if((adv_ptr->len + BLE_GAP_ADV_SERVICE_16BIT_UUID_LENGTH) >  (AT_BLE_ADV_MAX_SIZE))\
		{ \
			DBG_LOG("Adv Data too big");\
			while(1);\
		} \
		adv_ptr->adv_ptr[adv_ptr->len++] = (uint8_t) BLE_GAP_ADV_DATA_SERVICE_SOLTN_16BIT_UUID##n##_VAL; \
		adv_ptr->adv_ptr[adv_ptr->len++] = (uint8_t) ((BLE_GAP_ADV_DATA_SERVICE_SOLTN_16BIT_UUID##n##_VAL) >> 8); \
	}\
}

#define  _CONF_SERVICE_SOLTN_32BIT_UUID(n, uuid_ptr) \
{ \
	if (BLE_GAP_ADV_SERVICE_SOLTN_32BIT_UUID##n##_ENABLE) \
	{\
		adv_element_container_t *adv_ptr;\
		adv_ptr = (adv_element_container_t *)uuid_ptr;\
		if((adv_ptr->len + BLE_GAP_ADV_SERVICE_32BIT_UUID_LENGTH) >  (AT_BLE_ADV_MAX_SIZE - ADV_TYPE_FLAG_SIZE))\
		{ \
			DBG_LOG("Adv Data too big");\
			while(1);\
		} \
		memcpy(&adv_ptr->adv_ptr[adv_ptr->len], BLE_GAP_ADV_DATA_SERVICE_SOLTN_32BIT_UUID##n##_VAL, BLE_GAP_ADV_SERVICE_32BIT_UUID_LENGTH);\
		adv_ptr->len += BLE_GAP_ADV_SERVICE_32BIT_UUID_LENGTH; \
	}\
}

#define  _CONF_SERVICE_SOLTN_32BIT_UUID_SCAN_RSP(n, uuid_ptr) \
{ \
	if (BLE_GAP_ADV_SERVICE_SOLTN_32BIT_UUID##n##_ENABLE) \
	{\
		adv_element_container_t *adv_ptr;\
		adv_ptr = (adv_element_container_t *)uuid_ptr;\
		if((adv_ptr->len + BLE_GAP_ADV_SERVICE_32BIT_UUID_LENGTH) >  (AT_BLE_ADV_MAX_SIZE))\
		{ \
			DBG_LOG("Adv Data too big");\
			while(1);\
		} \
		memcpy(&adv_ptr->adv_ptr[adv_ptr->len], BLE_GAP_ADV_DATA_SERVICE_SOLTN_32BIT_UUID##n##_VAL, BLE_GAP_ADV_SERVICE_32BIT_UUID_LENGTH);\
		adv_ptr->len += BLE_GAP_ADV_SERVICE_32BIT_UUID_LENGTH; \
	}\
}

#define  _CONF_SERVICE_SOLTN_128BIT_UUID(n, uuid_ptr) \
{ \
	if (BLE_GAP_ADV_SERVICE_SOLTN_128BIT_UUID##n##_ENABLE) \
	{\
		adv_element_container_t *adv_ptr;\
		adv_ptr = (adv_element_container_t *)uuid_ptr;\
		if((adv_ptr->len + BLE_GAP_ADV_SERVICE_128BIT_UUID_LENGTH) >  (AT_BLE_ADV_MAX_SIZE - ADV_TYPE_FLAG_SIZE))\
		{ \
			DBG_LOG("Adv Data too big");\
			while(1);\
		} \
		memcpy(&adv_ptr->adv_ptr[adv_ptr->len], BLE_GAP_ADV_DATA_SERVICE_SOLTN_128BIT_UUID##n##_VAL , BLE_GAP_ADV_SERVICE_128BIT_UUID_LENGTH);\
		adv_ptr->len += BLE_GAP_ADV_SERVICE_128BIT_UUID_LENGTH; \
	}\
}

#define  _CONF_SERVICE_SOLTN_128BIT_UUID_SCAN_RSP(n, uuid_ptr) \
{ \
	if (BLE_GAP_ADV_SERVICE_SOLTN_128BIT_UUID##n##_ENABLE) \
	{\
		adv_element_container_t *adv_ptr;\
		adv_ptr = (adv_element_container_t *)uuid_ptr;\
		if((adv_ptr->len + BLE_GAP_ADV_SERVICE_128BIT_UUID_LENGTH) >  (AT_BLE_ADV_MAX_SIZE))\
		{ \
			DBG_LOG("Adv Data too big");\
			while(1);\
		} \
		memcpy(&adv_ptr->adv_ptr[adv_ptr->len], BLE_GAP_ADV_DATA_SERVICE_SOLTN_128BIT_UUID##n##_VAL , BLE_GAP_ADV_SERVICE_128BIT_UUID_LENGTH);\
		adv_ptr->len += BLE_GAP_ADV_SERVICE_128BIT_UUID_LENGTH; \
	}\
}

#define  _CONF_PUBLIC_TARGET_ADDR(n, uuid_ptr) \
{ \
	if (BLE_GAP_ADV_DATA_PUBLIC_TARGET_ADDR##n##_ENABLE) \
	{\
		adv_element_container_t *adv_ptr;\
		adv_ptr = (adv_element_container_t *)uuid_ptr;\
		if((adv_ptr->len + BLE_GAP_ADV_PUBLIC_TARGET_ADDR_LENGTH) >  (AT_BLE_ADV_MAX_SIZE - ADV_TYPE_FLAG_SIZE))\
		{ \
			DBG_LOG("Adv Data too big");\
			while(1);\
		} \
		memcpy(&adv_ptr->adv_ptr[adv_ptr->len], BLE_GAP_ADV_DATA_PUBLIC_TARGET_ADDR##n##_VAL, BLE_GAP_ADV_PUBLIC_TARGET_ADDR_LENGTH);\
		adv_ptr->len += BLE_GAP_ADV_PUBLIC_TARGET_ADDR_LENGTH; \
	}\
}

#define  _CONF_PUBLIC_TARGET_ADDR_SCAN_RSP(n, uuid_ptr) \
{ \
	if (BLE_GAP_ADV_DATA_PUBLIC_TARGET_ADDR##n##_ENABLE) \
	{\
		adv_element_container_t *adv_ptr;\
		adv_ptr = (adv_element_container_t *)uuid_ptr;\
		if((adv_ptr->len + BLE_GAP_ADV_PUBLIC_TARGET_ADDR_LENGTH) >  (AT_BLE_ADV_MAX_SIZE))\
		{ \
			DBG_LOG("Adv Data too big");\
			while(1);\
		} \
		memcpy(&adv_ptr->adv_ptr[adv_ptr->len], BLE_GAP_ADV_DATA_PUBLIC_TARGET_ADDR##n##_VAL, BLE_GAP_ADV_PUBLIC_TARGET_ADDR_LENGTH);\
		adv_ptr->len += BLE_GAP_ADV_PUBLIC_TARGET_ADDR_LENGTH; \
	}\
}

#define  _CONF_RANDOM_TARGET_ADDR(n, uuid_ptr) \
{ \
	if (BLE_GAP_ADV_DATA_RANDOM_TARGET_ADDR##n##_ENABLE) \
	{\
		adv_element_container_t *adv_ptr;\
		adv_ptr = (adv_element_container_t *)uuid_ptr;\
		if((adv_ptr->len + BLE_GAP_ADV_RANDOM_TARGET_ADDR_LENGTH) >  (AT_BLE_ADV_MAX_SIZE - ADV_TYPE_FLAG_SIZE))\
		{ \
			DBG_LOG("Adv Data too big");\
			while(1);\
		} \
		memcpy(&adv_ptr->adv_ptr[adv_ptr->len], BLE_GAP_ADV_DATA_RANDOM_TARGET_ADDR##n##_VAL, BLE_GAP_ADV_RANDOM_TARGET_ADDR_LENGTH);\
		adv_ptr->len += BLE_GAP_ADV_RANDOM_TARGET_ADDR_LENGTH; \
	}\
}

#define  _CONF_RANDOM_TARGET_ADDR_SCAN_RSP(n, uuid_ptr) \
{ \
	if (BLE_GAP_ADV_DATA_RANDOM_TARGET_ADDR##n##_ENABLE) \
	{\
		adv_element_container_t *adv_ptr;\
		adv_ptr = (adv_element_container_t *)uuid_ptr;\
		if((adv_ptr->len + BLE_GAP_ADV_RANDOM_TARGET_ADDR_LENGTH) >  (AT_BLE_ADV_MAX_SIZE))\
		{ \
			DBG_LOG("Adv Data too big");\
			while(1);\
		} \
		memcpy(&adv_ptr->adv_ptr[adv_ptr->len], BLE_GAP_ADV_DATA_RANDOM_TARGET_ADDR##n##_VAL, BLE_GAP_ADV_RANDOM_TARGET_ADDR_LENGTH);\
		adv_ptr->len += BLE_GAP_ADV_RANDOM_TARGET_ADDR_LENGTH; \
	}\
}



/****************************************************************************************
*							        Structures                                     		*
****************************************************************************************/

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
typedef struct gatt_service_handler
{
	/// service uuid
	at_ble_uuid_t	serv_uuid;
	/// service handle
	at_ble_handle_t	serv_handle;
	/// service characteristic
	at_ble_characteristic_t	serv_chars;
}gatt_service_handler_t;


/****************************************************************************************
*                                       Functions                                       *
****************************************************************************************/

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
at_ble_status_t ble_conn_param_update(void *params);

/** @brief function triggered on receiving a connection parameter update request from the peer.
  *
  * @param[in] conn_param_req @ref at_ble_conn_param_update_request_t parameters received.
  *
  * @return none.
  *
  */
at_ble_status_t ble_conn_param_update_req(void *params);

/** @brief function called when the AT_BLE_PAIR_REQUEST event is received from stack.
  *
  * @param[in] pair_key @ref at_ble_pair_request_t details for the pairing request from the peer.
  *
  * @return none.
  *
  */
at_ble_status_t ble_pair_request_handler(void *params);

at_ble_status_t ble_slave_security_request_handler(void* params);

/** @brief function called when the AT_BLE_PAIR_KEY_REQUEST event is received from stack.
  *
  * @param[in] pair_key @ref at_ble_pair_key_request_t key details for the pairing request.
  *
  * @return none.
  *
  */
at_ble_status_t ble_pair_key_request_handler(void *params);

/** @brief function called when the AT_BLE_PAIR_DONE event is received from stack.
  *
  * @param[in] pairing_params @ref at_ble_pair_done_t pair done information.
  *
  * @return @ref AT_BLE_SUCCESS operation completed successfully.
  * @return @ref AT_BLE_FAILURE Generic error.
  *
  */
at_ble_status_t ble_pair_done_handler(void *params);

/** @brief function called when the AT_BLE_ENCRYPTION_REQUEST event is received from stack.
  *
  * @param[in] encry_req encryption request from the peer device.
  *
  * @return none.
  *
  */
at_ble_status_t ble_encryption_request_handler (void *params);

/** @brief function called when the AT_BLE_ENCRYPTION_STATUS_CHANGED event is received from stack.
  *
  * @param[in] encry_status changed encryption status.
  *
  * @return none.
  *
  */
at_ble_status_t ble_encryption_status_change_handler(void *params);

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
at_ble_status_t ble_scan_info_handler(void *params);

/** @brief function to handle the scan status.
  *
  * @param[in] scan_report status of the scan.
  *
  * @return @ref AT_BLE_SUCCESS operation completed successfully.
  * @return @ref AT_BLE_FAILURE Generic error.
  *
  */
at_ble_status_t ble_scan_report_handler(void *params);

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
at_ble_status_t ble_disconnected_state_handler(void *params);

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

bool ble_check_ispheripheral(at_ble_handle_t handle);

bool ble_check_iscentral(at_ble_handle_t handle);

at_ble_status_t ble_connected_device_role(at_ble_handle_t conn_handle, at_ble_dev_role_t *dev_role);

bool ble_check_disconnected_iscentral(at_ble_handle_t handle);

at_ble_status_t ble_disconnected_device_role(at_ble_handle_t conn_handle, at_ble_dev_role_t *dev_role);

at_ble_status_t ble_check_device_state(at_ble_handle_t conn_handle, ble_device_state_t state);

at_ble_status_t ble_resolv_rand_addr_handler(void *params);

at_ble_status_t ble_connected_state_handler(void *params);

at_ble_status_t ble_advertisement_data_set(void);

at_ble_status_t ble_mtu_changed_indication_handler(void *params);

at_ble_status_t ble_mtu_changed_cmd_complete_handler(void *params);

at_ble_status_t ble_characteristic_write_cmd_complete_handler(void *params);

at_ble_status_t ble_undefined_event_handler(void *params);

bool ble_mgr_events_callback_handler(ble_mgr_event_cb_t event_cb_type,
							ble_mgr_event_t event_type,
							const ble_event_callback_t *ble_event_handler);
#endif /*__BLE_MANAGER_H__*/
// </h>

// <<< end of configuration section >>>
