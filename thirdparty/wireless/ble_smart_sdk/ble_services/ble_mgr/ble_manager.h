/**
* \file
*
* \brief BLE Manager declarations
*
* Copyright (c) 2016-2018 Microchip Technology Inc. and its subsidiaries.
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
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
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

// <<< Use Configuration Wizard in Context Menu >>>
// <h> BLE Event Management Configuration
// =======================

#ifndef __BLE_MANAGER_H__
#define __BLE_MANAGER_H__

/**
*  \defgroup ble_manager BLE Manager
*  \brief  BLE Manager provides Common Framework for Application, BLE Profiles, BLE Services, Library and Platform
*  		This group includes functionality of GAP, GATT, HTPT, DTM Events and handling the GAP activities
* @{
*/

/**
*  \defgroup ble_manager_gap_event BLE GAP Events
*  \brief This group includes functionality of GAP Events and handling the GAP activities
* 
*/

/**
*  \defgroup ble_manager_event_subscription BLE Event Subscription
*  \brief This group includes functionality of registering the BLE Events from profile/service/application
*/

/**
*  \defgroup ble_manager_event_unsubscribe BLE Event Un-Subscribe
*  \brief This group includes functionality of un-subscribe the BLE Events from BLE Manager
*/

/**
*  \defgroup ble_manager_pairing BLE Pairing
*  \brief This group includes functionality of pair request from/to peer device based on the device role
*/

/**
*  \defgroup ble_manager_advertisement_structure Dynamic Advertisement/Scan Data
*  \brief This group includes functionality of creating the advertisement data and scan data
*/

/**
*  \defgroup ble_manager_service_addition BLE Services Add/Remove
*  \brief This group includes functionality of adding or removing the components, service, BLE services from ASF Wizard
*/

/**
*  \defgroup ble_manager_platform_initialization Platform Initialization
*  \brief This group includes functionality of initializing the SAMB11 and initializing the Host interface
*/
/** @}*/

/**
 *	\addtogroup ble_manager_gap_event BLE Manager - GAP Events
 *	\brief BLE-Manager is handles the GAP activities and delivers the events to the subscribers.
 *		   it handles multiple connection and bonding information which includes the device address, LTK, IRK,
 *		   authentication information etc.,
 *	
 *	\addtogroup ble_manager_event_subscription BLE Manager - Event Subscription	
 * 	\brief The BLE Profile or  BLE Services can subscribe the events like GATT Server Event, GATT Client Events,
 * 		   GAP Events, L2CAP Events, HTPT Profile Events, DTM Events and CUSTOM Events by registering its call-backs to BLE-Manager.
 * 	
 *	\addtogroup ble_manager_event_subscription BLE Manager - Event Subscription description
 *	\brief BLE-Manager checks the registered callbacks with respective events. If the event and registered callbacks is valid
 *		   ble manager will calls the respective call back to handle the events. The more number of profile can also subscribe the same event
 *		   for process. ex. If BLE battery service and BLE blood pressure profile are present in the same application then it can subscribe the
 *		   same events.
 *	
 *	\addtogroup ble_manager_event_unsubscribe BLE Manager - Event Subscription Dynamics
 *	\brief The number of event subscriptions can be increased based on the number of  event subscribers. The Number of
 *		   GATT Server Event, GATT Client Events, GAP Events, L2CAP Events, Custom Events can be increased or decreased by following
 *		   MAX_GATT_SERVER_SUBSCRIBERS, MAX_GATT_CLIENT_SUBSCRIBERS, MAX_GAP_EVENT_SUBSCRIBERS, MAX_L2CAP_EVENT_SUBSCRIBERS,
 *		   MAX_HTPT_EVENT_SUBSCRIBERS, MAX_DTM_EVENT_SUBSCRIBERS and MAX_CUSTOM_EVENT_SUBSCRIBERS respectively.
 *	
 *	\addtogroup ble_manager_event_unsubscribe BLE Manager - Event UnSubscription
 *	\brief The BLE Events like GATT Server Event, GATT Client Events, GAP Events, L2CAP Events, HTPT Profile Events, DTM Events and 
 *		   CUSTOM Events can be un subscribed by dynamically from the BLE-Manager. BLE-Manager won't call the callbacks after un-subscribing the 
 *		   Events.	
 *	
 *	\addtogroup ble_manager_pairing BLE Manager - Pairing related activities
 *	\brief BLE-Manager handles the Pairing related activities like Slave security request, Triggering Pair-request, Pair-Key request on Central Role
 *		   handling the pair request and pair key request in peripheral role. BLE-Manager uses the bonding information for Encryption and also for resolving the 
 *		   random address of Peer device.
 *	
 *	\addtogroup ble_manager_advertisement_structure BLE Manager - Advertisement/Scan Packet Structure
 *	\brief BLE Manager creates the advertisement/scan packet structure for each profiles based on the Bluetooth standard service inclusion. For custom service inclusion
 *		   user can add the service UUID in BLE Manager also option to enable 16-bit or 128-bit UUID. Application/Profile doesn't to add advertisement packet in this case.
 *	 
 *	 \addtogroup ble_manager_service_addition BLE Manager - Addition/Removal of Service using ASF Wizard
 *	\brief ASF Wizard can be used to pull the BLE services and other services which exists in the ASF. As on when the ASF services are pulled in or out the respective 
 *		   advertisement structure will changed according to the services and profiles present currently in the application.
 *	
 *	 \addtogroup ble_manager_platform_initialization BLE Manager - SAMB11 Device Initialization
 *	\brief BLE Manager also handles the SAMB11 device initialization which includes SAMB11 Patch downloading,  device role, BD address configuration,
 *		   platform callback API registration for Library, Platform initialization, memory pool allocation
 *		   
 */	

#include <asf.h>
#include <string.h>
#include <stddef.h>
#include "at_ble_api.h"
#include "ble_utils.h"

/*! \def The SAMB11 device role must be defined for device configuration. if the device configuration is not selected then   
	default configuration of All role will be applicable
 */
#ifndef BLE_DEVICE_ROLE
#warning "BLE Device role is not defined, BLE Device Role defaults to BLE_ROLE_ALL"
#define BLE_DEVICE_ROLE BLE_ROLE_ALL
#endif

#ifndef OTAU_FEATURE
#define OTAU_FEATURE false
#endif

#if OTAU_FEATURE
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME				"ATMEL-OTA"
#endif /* BLE_DEVICE_NAME */
#endif /* OTA_FEATURE */

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

/*! \def  HID Mouse Profile GAP configurations
*/
	#ifdef HID_MOUSE_DEVICE
	#ifndef BLE_DEVICE_NAME
		#define BLE_DEVICE_NAME				"ATMEL-HID"
	#endif	/* BLE_DEVICE_NAME */
	#define BLE_PAIR_ENABLE				(false)
	#endif
#endif /* HID_DEVICE */

/*! \def  PxP Reporter Profile Device Name configurations
*/
#if defined PROXIMITY_REPORTER
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME				"ATMEL-PXP"
#endif	/* BLE_DEVICE_NAME */
#endif /* PROXIMITY_REPORTER */

/*! \def  HR Sensor Profile Device Name configurations
*/
#if defined HR_SENSOR
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME				"ATMEL-HRP"
#endif	/* BLE_DEVICE_NAME */
#endif /* HR_SENSOR */

/*! \def  Blood Pressure Profile Device Name configurations
*/
#if defined BLP_SENSOR
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME				"ATMEL-BLP"
#endif	/* BLE_DEVICE_NAME */
#endif /* Blood_Pressure_Sensor*/

/*! \def  Observer Device Name configurations
*/
#if (BLE_DEVICE_ROLE == BLE_ROLE_OBSERVER)
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME				"ATMEL-OBS"
#endif	/* BLE_DEVICE_NAME */
#endif /* BLE_DEVICE_ROLE == BLE_ROLE_OBSERVER) */

/*! \def  ANCS Profile Device Name configurations
*/
#if defined ANP_CLIENT
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME				"ATMEL-ANCS"
#endif	/* BLE_DEVICE_NAME */
#endif /* ANCS_CLIENT */

/*! \def  Time Information Profile Device Name configurations
*/
#if defined TIP_CLIENT
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME				"ATMEL-TIP"
#endif	/* BLE_DEVICE_NAME */
#endif /* TIP_CLIENT */

/*! \def  Find me Profile Device Name configurations
*/
#if defined FIND_ME_TARGET
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME				"ATMEL-FMP"
#endif	/* BLE_DEVICE_NAME */
#endif /* FIND_ME_TARGET */

/*! \def  Alert Notification Profile Device Name configurations
*/
#if defined ANP_SIG_CLIENT
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME				"ATMEL-ANS"
#endif	/* BLE_DEVICE_NAME */
#endif /* ANP_SIG_CLIENT */

/*! \def  Phone Alert Status Profile Device Name configurations
*/
#if defined PAS_CLIENT
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME				"ATMEL-PAS"
#endif	/* BLE_DEVICE_NAME */
#endif /* PAS_CLIENT */

/*! \def  Custom Serial Chat Profile Device Name configurations
*/
#if defined CSC_DEVICE
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME				"AT-CSC"
#endif	/* BLE_DEVICE_NAME */
#endif /* CSC_DEVICE */

/*! \def  BLE Battery Service Device Name configurations
*/
#if defined	BATTERY_SERVICE
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME				"ATMEL-BAS"
#endif	/* BLE_DEVICE_NAME */
#endif /*BATTERY_SERVICE*/

/*! \def L2CAP Peripheral Test Device Name configurations
*/
#if defined	L2CAP_PERIPHERAL
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME				"ATMEL-L2CAP-P"
#define BLE_PAIR_ENABLE				(false)
#endif	/* BLE_DEVICE_NAME */
#endif /*L2CAP_PERIPHERAL*/

/*! \def L2CAP Central Test Device Name configurations
*/
#if defined	L2CAP_CENTRAL
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME				"ATMEL-L2CAP-C"
	#define BLE_PAIR_ENABLE				(false)
#endif	/* BLE_DEVICE_NAME */
#endif /*L2CAP_CENTRAL*/

/*! \def Scan Parameter profile Device Name configurations
*/
#if defined	SCAN_PARAM_SERVICE
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME				"ATMEL-SCP"
#endif	/* BLE_DEVICE_NAME */
#endif	/*SCAN_PARAM_SERVICE*/

/*! \def HTP Profile Device Name configurations
*/
#if defined HTPT_SERVICE
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME "ATMEL-HTP"
#endif  
#endif

/*! \def  Device Information service Device Name configurations
*/
#if defined	DEVICE_INFORMATION_SERVICE
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME				"ATMEL-DIS"
#endif	/* BLE_DEVICE_NAME */
#endif	/* DEVICE_INFORMATION_SERVICE */

/*! \def  PxP-Monitor Profile Device Name configurations
*/
#if defined PROXIMITY_MONITOR
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME				"ATMEL-MON"
#endif	/* BLE_DEVICE_NAME */
#endif /* PROXIMITY_MONITOR */

/*! \def  Custom Profile default Device Name configurations
*/
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME				"ATMEL-BLE"
#endif

/**   \enum ble_mgr_event_t 
	\brief BLE Manager event type. This event types are used to
		subscribe the Group of BLE Event callbacks or 
		un-subscribe the group of BLE Event callbacks
*/
typedef enum {
	/// BLE GAP Events types
	BLE_GAP_EVENT_TYPE,
	/// BLE GATT Client Event types
	BLE_GATT_CLIENT_EVENT_TYPE,
	/// BLE GATT Server Event types
	BLE_GATT_SERVER_EVENT_TYPE,
	/// BLE L2CAP Event types
	BLE_L2CAP_EVENT_TYPE,
	/// BLE HTPT Profile Event types
	BLE_GATT_HTPT_EVENT_TYPE,
	/// BLE DTM Event types
	BLE_DTM_EVENT_TYPE,
	/// Custom Event types (Application, ISR etc.,)
	BLE_CUSTOM_EVENT_TYPE
}ble_mgr_event_t;

/**   \enum ble_mgr_event_cb_t 
	\brief Subscribe the events or Un-subscribe the events
*/
typedef enum {
	/// Subscribe for the BLE events
	REGISTER_CALL_BACK,
	/// Un Subscribe for the BLE events
	UNREGISTER_CALL_BACK,
}ble_mgr_event_cb_t;

/**   \enum ble_device_ll_state_t 
	\brief Advertisement/Scanning Link Layer states and Paired Encryption states used by BLE manager
*/
typedef enum ble_device_ll_state {
	/// Device is in idle state
	BLE_DEVICE_IDLE_STATE,
	/// Peripheral state in advertisement
	PERIPHERAL_ADVERTISING_STATE,
	/// Central in scanning state
	CENTRAL_SCANNING_STATE,
	/// Device stopped advertisement/scanning and Connection established
	PERIPHERAL_CONNECTED_STATE,
	/// Device paired with peer device
	PERIPHERAL_PAIRED_STATE,
	/// Device GAP state in Encryption state
	PERIPHERAL_ENCRYPTION_STATE,
	/// Device disconnected from peer device
	PERIPHERAL_DISCONNECTED_STATE,
}ble_device_ll_state_t;

typedef enum ble_ulp_mode {
	BLE_ULP_MODE_CLEAR,
	BLE_ULP_MODE_SET
}ble_ulp_mode_t;
/**   \enum ble_gatt_cccd_status_t 
	\brief Client Characteristics Configuration Descriptor States
*/
typedef enum ble_gatt_cccd_status {
	/// GATT Server characteristics Notification and Indications are disabled
	CCCD_NOTIFICATION_INDICATION_DISABLED = 0x00,
	/// GATT Server characteristics Notification enabled and Indications are disabled
	CCCD_NOTIFICATION_ENABLED_INDICATION_DISABLED = 0x01,
	/// GATT Server characteristics Notification disabled and Indications enabled
	CCCD_NOTIFICATION_DISABLED_INDICATION_ENABLED = 0x02,
	/// GATT Server characteristics Notification and Indications are enabled
	CCCD_INIDCATION_NOTIFICATION_ENABLED = 0x03
}ble_gatt_cccd_status_t;

/* \typedef ble_event_callback_t 
    \brief BLE Event callback generic type */
typedef at_ble_status_t (*ble_event_callback_t) (void *params);

/**   \struct ble_gap_event_cb_t 
	\brief BLE GAP Event callback types
*/
typedef struct ble_gap_event_cb {
	/// BLE Undefined events received from SAMB11, which is not handled in Library
	ble_event_callback_t undefined;
	/// BLE scan information contains the peer device scan information Refer \ref at_ble_scan_info_t
	ble_event_callback_t scan_info; 
	///  Once the BLE scan timeout or scan stop will trigger the scan report event Refer \ref at_ble_scan_report_t
	ble_event_callback_t scan_report;
	/// Advertisement complete event Refer \ref at_ble_adv_cmp_evt_t
	ble_event_callback_t advt_cmp;
	/// Advertisement report in case of advertisement stopped due to advertisement timeout Refer \ref at_ble_adv_report_t
	ble_event_callback_t adv_report;
	/// BLE device own random address changed event Refer \ref at_ble_rand_addr_changed_t
	ble_event_callback_t rand_addr_changed;
	/// BLE connected event, device connected to peer device Refer \ref at_ble_connected_t
	ble_event_callback_t connected;
	/// BLE disconnected event, device disconnected from peer device Refer \ref at_ble_disconnected_t
	ble_event_callback_t disconnected;
	/// Connection parameter update done on peer device and own device Refer \ref at_ble_conn_param_update_done_t
	ble_event_callback_t conn_parameter_update_done;
	/// Connection parameter update request from peer device Refer \ref at_ble_conn_param_update_request_t
	ble_event_callback_t conn_param_update_request;
	/// Pair complete event as part of end of pairing process Refer \ref at_ble_pair_done_t
	ble_event_callback_t pair_done;
	/// Pair request event from Central device Refer \ref at_ble_pair_request_t
	ble_event_callback_t pair_request;
	/// Slave security request received from peer Peripheral device Refer \ref at_ble_slave_sec_request_t
	ble_event_callback_t slave_sec_request;
	/// Pair Key request event from Peer central device Refer \ref at_ble_pair_key_request_t 
	ble_event_callback_t pair_key_request;
	/// encryption request comes from the peer device. Needs to supply the bonding information which is stored earlier \ref at_ble_encryption_request_t
	ble_event_callback_t encryption_request;
	/// encryption status changed event received from SMP once the encryption is started \ref at_ble_encryption_status_changed_t
	ble_event_callback_t encryption_status_changed;
	/// Event triggered once the Resolve random address is resolved using supplied IRK's. \ref at_ble_resolv_rand_addr_status_t
	ble_event_callback_t resolv_rand_addr_status;
	/// Event is triggered once the connection signature information indication received. \ref at_ble_sign_counter_t
	ble_event_callback_t sign_counters_ind;
	/// Peer attribute information indication received Event. \ref at_ble_peer_att_info_ind_t
	ble_event_callback_t peer_att_info_ind;
	/// Peer device channel map received indication. \ref at_ble_channel_map_t
	ble_event_callback_t con_channel_map_ind;
}ble_gap_event_cb_t;

/**   \struct ble_gatt_client_event_cb_t 
	\brief All BLE GATT Client callback types
*/
typedef struct ble_gatt_client_event_cb {
	/// If GATT Client founds the primary service as part of service discovery process this event will be triggered Refer \ref at_ble_primary_service_found_t
	ble_event_callback_t primary_service_found;
	/// If GATT Client founds the included service as part of service discovery process this event will be triggered Refer \ref at_ble_included_service_found_t
	ble_event_callback_t included_service_found;
	/// If GATT Client founds the characteristics as part of char discovery process this event will be triggered Refer \ref at_ble_characteristic_found_t
	ble_event_callback_t characteristic_found;
	/// If GATT Client founds the descriptor as part of discovery process this event will be triggered Refer \ref at_ble_descriptor_found_t
	ble_event_callback_t descriptor_found;
	/// If GATT Client Discovery completes this event will be triggered Refer \ref at_ble_discovery_complete_t
	ble_event_callback_t discovery_complete;
	/// If GATT Client discovers the characteristic using UUID this event will be triggered Refer \ref at_ble_characteristic_read_response_t
	ble_event_callback_t characteristic_read_by_uuid_response;
	/// If GATT Client characteristic read with multiple response this event will be triggered Refer \ref at_ble_characteristic_read_response_t
	ble_event_callback_t characteristic_read_multible_response;
	///  If GATT Client writes onto GATT server characteristics this event will be triggered Refer \ref at_ble_characteristic_write_response_t
	ble_event_callback_t characteristic_write_response;
	/// GATT Client receives the notification from GATT Server. \ref at_ble_notification_recieved_t
	ble_event_callback_t notification_recieved;
	/// GATT Client receives the indication from GATT Server. \ref at_ble_indication_recieved_t
	ble_event_callback_t indication_recieved;
}ble_gatt_client_event_cb_t;

/**   \struct ble_gatt_server_event_cb_t 
	\brief All BLE GATT Server callback types
*/
typedef struct ble_gatt_server_event_cb {
	/// GATT Server sends the notification confirmation on the event of notification send. Refer \ref at_ble_cmd_complete_event_t
	ble_event_callback_t notification_confirmed;
	/// GATT Server sends the indication to GATT Client. Refer \ref at_ble_indication_confirmed_t
	ble_event_callback_t indication_confirmed;
	/// If GATT Client updates the GATT Server characteristics, the GATT server will send this event. Refer \ref at_ble_characteristic_changed_t
	ble_event_callback_t characteristic_changed;
	/// GATT Client changes GATT Server Characteristics configuration. Refer \ref at_ble_characteristic_configuration_changed_t
	ble_event_callback_t characteristic_configuration_changed;
	/// GATT Server add/remove the service dynamically. If GATT Client subscribes for this event then service changed indication will be sent. Refer \ref at_ble_cmd_complete_event_t
	ble_event_callback_t service_changed_indication_sent;
	/// Peer(GATT Client) asks authorization for writing into GATT Server database. Refer \ref at_ble_characteristic_write_request_t
	ble_event_callback_t write_authorize_request;
	/// Peer device changes the MTU size. Refer \ref at_ble_mtu_changed_ind_t
	ble_event_callback_t mtu_changed_indication;
	/// Device request peer to change the MTU size. Refer \ref at_ble_cmd_complete_event_t
	ble_event_callback_t mtu_changed_cmd_complete;
	/// Characteristics write command complete. Refer \ref at_ble_cmd_complete_event_t
	ble_event_callback_t characteristic_write_cmd_cmp;
	/// Peer(GATT Client) asks authorization for reading GATT Server database. Refer \ref at_ble_characteristic_read_req_t
	ble_event_callback_t read_authorize_request;
}ble_gatt_server_event_cb_t;

/**   \struct ble_l2cap_event_cb_t 
	\brief All BLE L2CAP callback types
*/
typedef struct ble_l2cap_event_cb {
	/// L2CAP Low Energy Credit based connection request received from server, Refer \ref at_ble_lecb_conn_req_t
	ble_event_callback_t lecb_conn_req;
	/// L2CAP Peer device connected successfully, Refer \ref at_ble_lecb_connected_t
	ble_event_callback_t lecb_connected;
	/// L2CAP Peer device disconnected, Refer \ref at_ble_lecb_disconnected_t
	ble_event_callback_t lecb_disconnected;
	/// L2CAP Indication on Peer device added credit, Refer \ref at_ble_lecb_add_credit_ind_t
	ble_event_callback_t lecb_add_credit_ind;
	/// L2CAP Response for the data send command, Refer \ref at_ble_lecb_send_resp_t
	ble_event_callback_t lecb_send_resp;
	/// L2CAP data received from peer device, Refer \ref at_ble_lecb_data_recieved_t
	ble_event_callback_t lecb_data_recieved;
}ble_l2cap_event_cb_t;

/**   \struct ble_htpt_event_cb_t 
	\brief All BLE HTPT event callback types
*/
typedef struct ble_htpt_event_cb {
	/// HTPT database creation confirmation event callback, Refer \ref at_ble_htpt_create_db_cfm_t
	ble_event_callback_t htpt_create_db_cfm;
	/// HTPT error indication to application event callback, Refer \ref at_ble_prf_server_error_ind_t
	ble_event_callback_t htpt_error_ind;
	/// HTPT indication disable to application event, Refer \ref at_ble_htpt_disable_ind_t
	ble_event_callback_t htpt_disable_ind;
	/// HTPT temperature send confirmation event, Refer \ref htpt_temp_send_cfm_t
	ble_event_callback_t htpt_temp_send_cfm;
	/// HTPT temperature measurement interval changed indication event, Refer \ref htpt_meas_intv_chg_ind_t
	ble_event_callback_t htpt_meas_intv_chg_ind;
	/// HTPT Inform application about new configuration value, Refer \ref at_ble_htpt_cfg_indntf_ind_t
	ble_event_callback_t htpt_cfg_indntf_ind;
	/// HTPT profile enable confirmation response event to application, Refer \ref at_ble_htpt_enable_rsp_t
	ble_event_callback_t htpt_enable_rsp;
	/// HTPT measurement interval update response event to application, Refer \ref at_ble_htpt_meas_intv_upd_rsp_t
	ble_event_callback_t htpt_meas_intv_upd_rsp;
	/// HTPT new measurement interval change request value request by peer device, Refer \ref at_ble_htpt_meas_intv_chg_req_t
	ble_event_callback_t htpt_meas_intv_chg_req;
}ble_htpt_event_cb_t;

/**   \struct ble_dtm_event_cb_t 
	\brief All BLE DTM event callback types
*/
typedef struct ble_dtm_event_cb {
	/// DTM command test status event, Refer \ref at_ble_dtm_t
	ble_event_callback_t le_test_status;
	/// DTM Rx Packet status event, Refer \ref at_ble_dtm_t
	ble_event_callback_t le_packet_report;
}ble_dtm_event_cb_t;

/**   \struct ble_custom_event_cb_t 
	\brief All BLE Custom event callback types
*/
typedef struct ble_custom_event_cb {
	/// Custom Event are used by application or in IRQ's to post the task which can be handled at user application. \ref at_ble_event_user_defined_post
	ble_event_callback_t custom_event;
	/// Device Ready event's are used to inform the application the BLE device is ready for accepting the user request, \ref AT_BLE_DEVICE_READY
	ble_event_callback_t device_ready;
	///  BLE Platform (interrupt) event sent from peripheral ISR callback. This interrupt event is processed at application on reception on platform event, \ref AT_PLATFORM_EVENT
	ble_event_callback_t platform_event_ready; /* Event that is received will be of type ((<callback_id << 8) |intr_index). */
	/// BLE Maximum events are reached \ref AT_BLE_EVENT_MAX
	ble_event_callback_t event_max;
}ble_custom_event_cb_t;

/** \brief event timeout */
#define BLE_EVENT_TIMEOUT			(20)

/** \brief pin timeout */
#define PIN_TIMEOUT					30000

/** \brief Observer related  definitions */
/** \brief LE Limited discoverable mode. The device can discover with time limit of 30seconds */
#define LE_LIMITED_DISCOVERABLE_MODE  ((uint8_t) 1 << 0)
/** \brief LE General discoverable mode. The device can discover without any time limit*/
#define LE_GENERAL_DISCOVERABLE_MODE  ((uint8_t) 1 << 1)
/** \brief Flag to indicate BR/EDR Not supported */
#define BREDR_NOT_SUPPORTED			  ((uint8_t) 1 << 2)
/** \brief Flag to indicate BR/EDR supported controller */
#define	LE_BREDR_CAPABLE_CONTROLLER   ((uint8_t) 1 << 3)
/** \brief Flag to indicate BR/EDR supported Host */
#define LE_BREDR_CAPABLE_HOST		  ((uint8_t) 1 << 4)

/** \brief Length of Adv data types*/
#define APPEARANCE_SIZE					2
#define TX_POWER_LEVEL_SIZE				1
#define ADV_INTERVAL_SIZE				2

/// \brief Default configuration for Pairing */
/// \brief Man-in-the-middle attack protection required
#ifndef BLE_MITM_REQ
#define BLE_MITM_REQ					(true)
#endif

/// \brief bonding bit will be present in the authentication requirement of pairing request message 
#ifndef BLE_BOND_REQ
#define BLE_BOND_REQ					(true)
#endif

/// \brief Enable the pairing will initiate slave security request from Peripheral device.
#ifndef BLE_PAIR_ENABLE
#define BLE_PAIR_ENABLE					(true)
#endif

/// \brief Choose BLE security Modes, Refer to \ref at_ble_auth_levels_t
#ifndef	BLE_AUTHENTICATION_LEVEL
#define BLE_AUTHENTICATION_LEVEL		(AT_BLE_MODE1_L2_AUTH_PAIR_ENC)
#endif

/// \brief Input and Output capabilities of this device, Refer to \ref at_ble_iocab_t
#ifndef	BLE_IO_CAPABALITIES
#define BLE_IO_CAPABALITIES				(AT_BLE_IO_CAP_DISPLAY_ONLY)
#endif

/// \brief Out of Band pairing feature support
#ifndef BLE_OOB_REQ
#define BLE_OOB_REQ						(false)
#endif

/// \brief Maximum Attribute database memory size in Bytes, Default value 2048-Bytes
#ifndef BLE_ATT_DB_MEMORY_SIZE
#define BLE_ATT_DB_MEMORY_SIZE				(2048)
#endif

/* \brief BLE Events are posted to subscribers(profiles/services/applications) If particular profile or service needs  particular BLE
    event they register a callbacks to BLE Manager. The group of events can be registered by the subscribers. The maximum 
    subscriber limit for each individual group can be customized here */
/// \brief Maximum event subscribers for BLE GAP events, Based on number of services/Profiles this number can be changed
#define	MAX_GAP_EVENT_SUBSCRIBERS    5

/// \brief Maximum GATT client event subscribers, Based on number of GATT Client services/Profiles this number can be changed
#define MAX_GATT_CLIENT_SUBSCRIBERS  5

/// \brief Maximum GATT Server event subscribers, Based on number of GATT Server services/Profiles this number can be changed
#define MAX_GATT_SERVER_SUBSCRIBERS  5

/// \brief Maximum L2CAP event subscribers. Based on No of  L2CAP event handlers this number can be changed
#define MAX_L2CAP_EVENT_SUBSCRIBERS  1

/// \brief Maximum HTPT event subscribers.  Based on No of  HTPT event handlers this number can be changed
#define MAX_HTPT_EVENT_SUBSCRIBERS   1

/// \brief Maximum DTM event subscribers.  Based on No of  DTM event handlers this number can be changed
#define MAX_DTM_EVENT_SUBSCRIBERS	 5

/// \brief Maximum Custom event subscribers.  Based on No of  Custom event handlers this number can be changed
#define MAX_CUSTOM_EVENT_SUBSCRIBERS 2

/* \brief Invalid connection handle value for Link not established state */
#define BLE_INVALID_CONNECTION_HANDLE (0xFF)


///  enum gap_ad_type
/// brief Gap Advertisement Types
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


/** \brief Service UUID's */

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
 * \brief Heart rate service UUID
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
/// \brief device information service system characteristics UUID
#define DIS_CHAR_SYSTEM_ID_UUID					(0x2A23)

/// \brief device information service model number characteristics UUID
#define DIS_CHAR_MODEL_NUMBER_UUID				(0x2A24)

/// \brief device information service serial number characteristics UUID
#define DIS_CHAR_SERIAL_NUMBER_UUID				(0x2A25)

/// \brief device information service firmware revision number characteristics UUID
#define DIS_CHAR_FIRMWARE_REIVSION_UUID			(0x2A26)

/// \brief device information service hardware revision characteristics UUID
#define DIS_CHAR_HARDWARE_REVISION_UUID			(0x2A27)

/// \brief device information service software revision characteristics UUID
#define DIS_CHAR_SOFTWARE_REVISION_UUID			(0x2A28)

/// \brief device information service manufacturer name characteristics UUID
#define DIS_CHAR_MANUFACTURER_NAME_UUID			(0x2A29)

/// \brief device information service  IEEE registration certificate list characteristics UUID
#define DIS_CHAR_IEEE_REG_CERT_DATA_LIST_UUID	(0x2A2A)

/* \brief  TIP Current Time char UUID */
#define CURRENT_TIME_CHAR_UUID					(0x2A2B)

/* \brief TIP Local Time Information char UUID */
#define LOCAL_TIME_CHAR_UUID					(0x2A0F)

/* \brief TIP Reference Time Information char UUID */
#define REF_TIME_CHAR_UUID						(0x2A14)

/* \brief TIP Time with DST char UUID */
#define TIME_WITH_DST_CHAR_UUID					(0x2A11)

/* \brief TIP Time Update Control Point char UUID */
#define TIME_UPDATE_CP_CHAR_UUID				(0x2A16)

/* \brief TIP Time Update State char UUID */
#define TIME_UPDATE_STATE_CHAR_UUID				(0x2A17)

/** \brief scan refresh characteristic uuid */
#define SPS_CHAR_SCAN_REFRESH_UUID 				(0x2A31)

/** \brief scan interval characteristic uuid */
#define SPS_CHAR_SCAN_INT_VALUE_UUID 			(0x2A4F)

/// \brief Product Number ID characteristic uuid
#define DIS_CHAR_PNP_ID_UUID					(0x2A50)

/// \brief HID report reference descriptor characteristic uuid
#define HID_REPORT_REF_DESC						(0x2908)

/** \brief HID Protocol Mode Characteristic UUID. */
#define HID_UUID_CHAR_PROTOCOL_MODE				(0x2A4E)

/** \brief HID Protocol Mode Characteristic UUID. */
#define HID_UUID_CHAR_REPORT_MAP				(0x2A4B)

/** \brief HID Report Characteristic UUID. */
#define HID_UUID_CHAR_REPORT					(0x2A4D)

/** \brief HID Boot Keyboard Input Report UUID. */
#define HID_UUID_CHAR_BOOT_KEY_INPUT_REPORT		(0x2A22)

/** \brief HID Boot Keyboard Output Report UUID. */
#define HID_UUID_CHAR_BOOT_KEY_OUTPUT_REPORT    (0x2A32)

/** \brief HID Boot Mouse Input Report UUID. */
#define HID_UUID_CHAR_BOOT_MOUSE_INPUT_REPORT	(0x2A33)

/** \brief HID Information UUID. */
#define HID_UUID_CHAR_HID_INFORMATION			(0x2A4A)

/** \brief HID Control Point UUID. */
#define HID_UUID_CHAR_HID_CONTROL_POINT			(0x2A4C)


/** \brief CSC Endpoint Characteristic UUID. */
#define CSC_ENDPOINT_CHAR_UUID			("\x1b\xc5\xd5\xa5\x02\x00\xa6\x85\xe5\x11\x35\x39\xa1\xbb\x5a\xfd")

/** \brief GATT Database discover default start handle */
#define GATT_DISCOVERY_STARTING_HANDLE	(0x0001)

/** \brief GATT Database discover default end handle */
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

/** \brief minimum length of local info parameters when using connection establishment proc */ 
#define GAP_CE_LEN_MIN					(0)
/** \brief maximum length of local info parameters when using connection establishment proc */
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
//	<o> Maximum Scan Device Buffer <0-50>
//	<i> Defines maximum number of Scan device can have buffer .
//	<i> Default: 10
//	<id> max_scan_device
#ifdef USE_SCAN_SOFT_FILTER
#define MAX_SCAN_DEVICE					(50)			  //Max number of scan device
#else
#define MAX_SCAN_DEVICE					(20)			  //Max number of scan device
#endif
/* Scan interval 30ms in term of 625us */
//	<o> Scan Interval in units of 625us <1-1000:50>
//	<i> Defines interval to Scan device .
//	<i> Default: 96
//	<id> gap_scan_interval
#define SCAN_INTERVAL					(96)              //Scan interval 30ms in term of 625us

/* Scan window 30ms values in term of 625ms */
//	<o> Scan Window in term of 625us <1-1000:50>
//	<i> Defines Scan Window .
//	<i> Default: 96
//	<id> gap_scan_window
#define SCAN_WINDOW						(96)              //Scan window 30ms values in term of 625ms

/// \brief Timeout  Scan time-out, 0x0000 disables time-out
#define SCAN_TIMEOUT					(0x0000)       

/// \brief Scan method, Refer to \ref at_ble_scan_type_t
#define SCAN_TYPE						(AT_BLE_SCAN_ACTIVE)

/** maximum number of devices connected */
//	<o> Maximum number of device to connect <1-5>
//	<i> Defines the central to connect maximum number to devices
//	<i> Default: 1
//	<id> gap_max_device_connected
#ifndef BLE_MAX_DEVICE_CONNECTION
#define BLE_MAX_DEVICE_CONNECTION				(8)			// Maximum No.of device information that BLE-Device can Handle
#endif

#if ((BLE_MAX_DEVICE_CONNECTION < 1) || (BLE_MAX_DEVICE_CONNECTION > 8))
#error "BLE Device connection Range is (1 to 8), Invalid BLE_MAX_DEVICE_CONNECTION range defined"
#endif

/// Maximum event parameter size that receives in \ref at_ble_event_get
#define BLE_EVENT_PARAM_MAX_SIZE		        524			

/**   \enum ble_device_state_t 
	\brief In BLE Manager device state may exist in the following one of the state
*/
typedef enum ble_device_state{
  /// Device is not connected and device information table contains invalid information
  BLE_DEVICE_DEFAULT_IDLE,
  /// Device is in disconnected from the peer device
  BLE_DEVICE_DISCONNECTED,
  /// Device connected to peer device
  BLE_DEVICE_CONNECTED,
  /// Device is in pairing state
  BLE_DEVICE_PAIRING,
  /// Device pairing is not success with peer device
  BLE_DEVICE_PAIRING_FAILED,
  /// Device successfully paired with peer device
  BLE_DEVICE_PAIRED,  
  /// Device is in pairing state
  BLE_DEVICE_ENCRYPTION_STATE,
  /// Device encryption is not success with peer device
  BLE_DEVICE_ENCRYPTION_FAILED,
  /// Device successfully started the encryption with peer device
  BLE_DEVICE_ENCRYPTION_COMPLETED  
}ble_device_state_t;

/**   \struct ble_connected_dev_info_t 
	\brief BLE manager maintains the device connection information, device role,
	bonding information, device connection state and Long Term Key
*/
typedef struct ble_connected_dev_info
{
	/// Connection information, Refer to \ref at_ble_connected_t
	at_ble_connected_t conn_info;
	/// Device role, Refer to \ref at_ble_dev_role_t
	at_ble_dev_role_t dev_role;
	/// Device bonding information, Refer to \ref at_ble_pair_done_t
	at_ble_pair_done_t bond_info;
	/// Device connection state information, Refer to \ref ble_device_state_t
	ble_device_state_t conn_state;
	/// Host LTK, refer to \ref at_ble_LTK_t
	at_ble_LTK_t host_ltk;
}ble_connected_dev_info_t;

#ifdef PDS_SERVICE
COMPILER_PACK_SET(1)  /**< \brief Start of compiler pack */

/**   \struct pds_bond_info_t 
	\brief BLE manager stores the bonding information using PDS
*/
typedef struct pds_bond_info{
	/// Peer device address, Refer to \ref  at_ble_addr_t
	at_ble_addr_t peer_addr;
	/// Authorization level , refer to \ref at_ble_auth_t
	at_ble_auth_t auth;
	/// Peer LTK, refer to \ref at_ble_LTK_t
	at_ble_LTK_t peer_ltk;
	/// CSR Key, refer to \ref at_ble_CSRK_t
	at_ble_CSRK_t peer_csrk;
	/// Identity Resolving Key, refer to \ref at_ble_IRK_t
	at_ble_IRK_t peer_irk;
	/// Host LTK, refer to \ref at_ble_LTK_t
	at_ble_LTK_t host_ltk;
}pds_bond_info_t;

COMPILER_PACK_RESET() /*!< End of compiler pack */

#endif


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

/** \brief Length of Adv data types*/
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

/**   \struct adv_element_t 
	\brief advertisement data element
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


/**   \struct gatt_service_handler_t type
	\brief GATT services handles
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

typedef struct user_custom_event
{
	/* Custom event ID */
	uint32_t id;
	/* event data pointer */
	void *bptr;
}user_custom_event_t;

typedef struct platform_isr_event
{
	uint16_t event_type;
	uint16_t event_data_len;
	#ifdef CSC_SERVICE
	uint8_t event_data[150];
	#else
	uint8_t event_data[16];
	#endif
}platform_isr_event_t;


/****************************************************************************************
*                                       Functions                                       *
****************************************************************************************/

/** \brief function to set the device name.
  *
  * \param[in] name name of the device.
  * \param[in] name_len length of the device name.
  *
  * \return \ref AT_BLE_SUCCESS operation completed successfully.
  * \return \ref AT_BLE_FAILURE Generic error.
  *
  */
at_ble_status_t ble_set_device_name(uint8_t *name, uint8_t name_len);

/** \brief function used to update the connection parameter.
  *
  * \param[in] conn_param_update \ref at_ble_conn_param_update_done_t parameters to be updated.
  *
  * \return \ref AT_BLE_SUCCESS operation completed successfully.
  * \return \ref AT_BLE_FAILURE Generic error.
  *
  */
at_ble_status_t ble_conn_param_update(void *params);

/** \brief function triggered on receiving a connection parameter update request from the peer.
  *
  * \param[in] conn_param_req \ref at_ble_conn_param_update_request_t parameters received.
  *
  * \return \ref AT_BLE_SUCCESS operation completed successfully.
  * \return \ref AT_BLE_FAILURE Generic error.
  *
  */
at_ble_status_t ble_conn_param_update_req(void *params);

/** \brief function called when the AT_BLE_PAIR_REQUEST event is received from stack.
  *
  * \param[in] pair_key \ref at_ble_pair_request_t details for the pairing request from the peer.
  *
  * \return \ref AT_BLE_SUCCESS operation completed successfully.
  * \return \ref AT_BLE_FAILURE Generic error.
  *
  */
at_ble_status_t ble_pair_request_handler(void *params);

at_ble_status_t ble_slave_security_request_handler(void* params);

/** \brief function called when the AT_BLE_PAIR_KEY_REQUEST event is received from stack.
  *
  * \param[in] pair_key \ref at_ble_pair_key_request_t key details for the pairing request.
  *
  * \return \ref AT_BLE_SUCCESS operation completed successfully.
  * \return \ref AT_BLE_FAILURE Generic error.
  *
  */
at_ble_status_t ble_pair_key_request_handler(void *params);

/** \brief function called when the AT_BLE_PAIR_DONE event is received from stack.
  *
  * \param[in] pairing_params \ref at_ble_pair_done_t pair done information.
  *
  * \return \ref AT_BLE_SUCCESS operation completed successfully.
  * \return \ref AT_BLE_FAILURE Generic error.
  *
  */
at_ble_status_t ble_pair_done_handler(void *params);

/** \brief function called when the AT_BLE_ENCRYPTION_REQUEST event is received from stack.
  *
  * \param[in] encry_req encryption request from the peer device.
  *
  * \return \ref AT_BLE_SUCCESS operation completed successfully.
  * \return \ref AT_BLE_FAILURE Generic error.
  *
  */
at_ble_status_t ble_encryption_request_handler (void *params);

/** \brief function called when the AT_BLE_ENCRYPTION_STATUS_CHANGED event is received from stack.
  *
  * \param[in] encry_status changed encryption status.
  *
  * \return \ref AT_BLE_SUCCESS operation completed successfully.
  * \return \ref AT_BLE_FAILURE Generic error.
  *
  */
at_ble_status_t ble_encryption_status_change_handler(void *params);

/** \brief function requesting the device for the connection.
  *
  * \param[in] dev_addr address of the the peer device.
  *
  * \return \ref AT_BLE_SUCCESS operation completed successfully.
  * \return \ref AT_BLE_FAILURE Generic error.
  *
  */
at_ble_status_t gap_dev_connect(at_ble_addr_t dev_addr[]);

/** \brief function to start scan.
  *
  * \param[in] none.
  *
  * \return \ref AT_BLE_SUCCESS operation completed successfully.
  * \return \ref AT_BLE_FAILURE Generic error.
  *
  */
at_ble_status_t gap_dev_scan(void);

/** \brief function to handle the scan information.
  *
  * \param[in] scan_param scanned data.
  *
  * \return \ref AT_BLE_SUCCESS operation completed successfully.
  * \return \ref AT_BLE_FAILURE Generic error.
  *
  */
at_ble_status_t ble_scan_info_handler(void *params);

/** \brief function to handle the scan status.
  *
  * \param[in] scan_report status of the scan.
  *
  * \return \ref AT_BLE_SUCCESS operation completed successfully.
  * \return \ref AT_BLE_FAILURE Generic error.
  *
  */
at_ble_status_t ble_scan_report_handler(void *params);

/** \brief function parses the received advertising data for service and local name.
  *
  * \param[in] scan_info_data the received data.
  * \param[in] ble_service_uuid uuid to be searched in the received data.
  * \param[in] adv_type advertisement type
  *
  * \return \ref AT_BLE_SUCCESS operation completed successfully.
  * \return \ref AT_BLE_FAILURE Generic error.
  *
  */
at_ble_status_t scan_info_parse(at_ble_scan_info_t *scan_info_data, at_ble_uuid_t *ble_service_uuid, uint8_t adv_type);

/** \brief function called when the AT_BLE_CHARACTERISTIC_FOUND event is received from stack.
  *
  * \param[in] characteristic_found \ref at_ble_characteristic_found_t information related to the characteristic found.
  *
  * \return none.
  *
  */
void ble_characteristic_found_handler(at_ble_characteristic_found_t *characteristic_found);


/** \brief function to handle the BLE event task.
  *
  * \param[in] none.
  *
  * \return \ref AT_BLE_SUCCESS operation completed successfully
  * \return \ref AT_BLE_FAILURE Generic error.
  *
  */
at_ble_status_t ble_event_task(uint32_t timeout);

/** \brief function sets both device address and device name which are exposed to all other devices.
  *
  * \param[in] addr address to be set as a device address.
  * \param[in] args configuration required for initialization.
  *
  * \return none.
  *
  */
void ble_device_init(at_ble_addr_t *addr);

/** \brief function handling all the events from the stack, responsible
  * for calling the respective functions initialized for the events.
  *
  * \param[in] events events received from the stack \ref at_ble_events_t.
  * \param[in] event_params data received from stack for the respective event received.
  *
  * \return none.
  *
  */
void ble_event_manager(at_ble_events_t events , void *event_params);

/** \brief function called when the AT_BLE_DISCOVERY_COMPLETE event is received from the stack.
  *
  * \param[in]	discover_status status of the discovery.
  *
  * \return none.
  *
  */
void ble_discovery_complete_handler(at_ble_discovery_complete_t *discover_status);

/** \brief function called when the AT_BLE_DISCONNECTED event is received from the stack.
  *
  * \param[in] disconnect \ref at_ble_disconnected_t information about the disconnection.
  *
  * \return none.
  *
  */
at_ble_status_t ble_disconnected_state_handler(void *params);

/** \brief function to send slave security request.
  *
  * \param[in] conn_handle \ref at_ble_handle_t connection handle.
  *
  * \return \ref AT_BLE_SUCCESS operation completed successfully
  * \return \ref AT_BLE_FAILURE Generic error.
  *
  */
at_ble_status_t ble_send_slave_sec_request(at_ble_handle_t conn_handle);

/** \brief Check the passed connection handle of the device is peripheral or central device
  *
  * \param[in] handle \ref at_ble_handle_t  Device connection handle
  *
  * \return \ref true if the passed handle of the device is peripheral
  * \return \ref false if the passed handle of the device is not peripheral.
  *
  */
bool ble_check_ispheripheral(at_ble_handle_t handle);

/** \brief Check the passed connection handle of the device is central device
  *
  * \param[in] handle \ref at_ble_handle_t  Device connection handle
  *
  * \return \ref true if the passed handle of the device is central
  * \return \ref false if the passed handle of the device is not central.
  *
  */
bool ble_check_iscentral(at_ble_handle_t handle);

/** \brief Get the passed connection handle device role
  *
  * \param[in] conn_handle \ref at_ble_handle_t  Device connection handle
  * \param[in] dev_role \ref at_ble_dev_role_t device role Central or Peripheral
  *
  * \return \ref AT_BLE_SUCCESS if the passed handle of the device gets the device role
  * \return \ref AT_BLE_FAILURE if the passed handle doesn't match and failure of getting device role
  *
  */
at_ble_status_t ble_connected_device_role(at_ble_handle_t conn_handle, at_ble_dev_role_t *dev_role);

/** \brief Check the passed connection handle of the device is disconnected and central device
  *
  * \param[in] handle \ref at_ble_handle_t  Device connection handle
  *
  * \return \ref true if the passed handle of the device is disconnected and central
  * \return \ref false if the passed handle of the device is not central or not disconnected.
  *
  */
bool ble_check_disconnected_iscentral(at_ble_handle_t handle);

/** \brief Check the passed connection handle of the device is disconnected and peripheral device
  *
  * \param[in] handle \ref at_ble_handle_t  Device connection handle
  *
  * \return \ref true if the passed handle of the device is disconnected and peripheral
  * \return \ref false if the passed handle of the device is not peripheral or not disconnected.
  *
  */
bool ble_check_disconnected_isperipheral(at_ble_handle_t handle);

/** \brief Get the device role of disconnected device
  *
  * \param[in] conn_handle \ref at_ble_handle_t  Device connection handle
  * \param[in] dev_role \ref at_ble_dev_role_t device role Central or Peripheral
  *
  * \return \ref AT_BLE_SUCCESS if the passed handle of the device gets the device role
  * \return \ref AT_BLE_FAILURE if the passed handle doesn't match and failure of getting device role
  *
  */
at_ble_status_t ble_disconnected_device_role(at_ble_handle_t conn_handle, at_ble_dev_role_t *dev_role);

/** \brief Check the device state
  *
  * \param[in] conn_handle \ref at_ble_handle_t  Device connection handle
  * \param[in] state \ref ble_device_state_t device state
  *
  * \return \ref AT_BLE_SUCCESS if the passed handle of device states are matches
  * \return \ref AT_BLE_FAILURE  if the passed handle of device states are doesn't matches
  *
  */
at_ble_status_t ble_check_device_state(at_ble_handle_t conn_handle, ble_device_state_t state);

/** \brief ble_resolv_rand_addr_handler handles the resolved random address event
  *
  * \param[in] params of at_ble_resolv_rand_addr_status_t  type, Refer to \ref at_ble_resolv_rand_addr_status_t
  *
  * \return \ref AT_BLE_SUCCESS Resolved random address matches with existing device
  * \return \ref AT_BLE_FAILURE Resolved random address doesn't match with any device
  *
  */
at_ble_status_t ble_resolv_rand_addr_handler(void *params);

/** \brief ble_connected_state_handler handles the connected event. stores the peer device information into device info table
  *
  * \param[in] params of at_ble_connected_t  type, Refer to \ref at_ble_connected_t
  *
  * \return \ref AT_BLE_SUCCESS Connected event handled successfully
  * \return \ref AT_BLE_FAILURE Connected event with error status
  *
  */
at_ble_status_t ble_connected_state_handler(void *params);

/** \brief ble_advertisement_data_set  set the BLE advertisement data based on the available   
  *		available services and profiles which is present in the current application.
  *
  * \param[in] None
  *
  * \return \ref AT_BLE_SUCCESS BLE advertisement data set successfully 
  * \return \ref AT_BLE_FAILURE Failed to set the advertisement data
  *
  */
at_ble_status_t ble_advertisement_data_set(void);

/** \brief ble_mtu_changed_indication_handler handler for Peer device MTU size change
  *
  * \param[in] params of at_ble_mtu_changed_ind_t  type, Refer to \ref at_ble_mtu_changed_ind_t
  *
  * \return \ref AT_BLE_SUCCESS event handled successfully
  * \return \ref AT_BLE_FAILURE event with error status
  *
  */
at_ble_status_t ble_mtu_changed_indication_handler(void *params);

/** \brief ble_mtu_changed_indication_handler handler for Peer device MTU size change
  *
  * \param[in] params of at_ble_mtu_changed_ind_t  type, Refer to \ref at_ble_mtu_changed_ind_t
  *
  * \return \ref AT_BLE_SUCCESS event handled successfully
  * \return \ref AT_BLE_FAILURE event with error status
  *
  */
at_ble_status_t ble_mtu_changed_cmd_complete_handler(void *params);

/** \brief ble_characteristic_write_cmd_complete_handler handler for Char write command complete event
  *
  * \param[in] params of at_ble_cmd_complete_event_t  type, Refer to \ref at_ble_cmd_complete_event_t
  *
  * \return \ref AT_BLE_SUCCESS event handled successfully
  * \return \ref AT_BLE_FAILURE event with error status
  *
  */
at_ble_status_t ble_characteristic_write_cmd_complete_handler(void *params);

/** \brief ble_undefined_event_handler handler for undefined events which is not expected in Library
  *
  * \param[in] params of void * type
  *
  * \return \ref AT_BLE_SUCCESS event handled successfully
  * \return \ref AT_BLE_FAILURE event with error status
  *
  */
at_ble_status_t ble_undefined_event_handler(void *params);

/** \brief ble_mgr_events_callback_handler handler for Profile/Service/application to register the BLE events callbacks
  *
  * \param[in] param of ble_mgr_event_cb_t  type, Refer to \ref ble_mgr_event_cb_t
  * \param[in] param of ble_mgr_event_t  type, Refer to \ref ble_mgr_event_t
  * \param[in] ble_event_handler of ble_mgr_event_t  type, Refer to \ref ble_mgr_event_t
  *
  * \return \ref true callback registered or un-register from the event handlers
  * \return \ref false failed to register or un-register the events from the event handlers
  *
  */
bool ble_mgr_events_callback_handler(ble_mgr_event_cb_t event_cb_type,
							ble_mgr_event_t event_type,
							const void *ble_event_handler);
							
at_ble_status_t ble_set_ulp_mode(ble_ulp_mode_t mode);

ble_ulp_mode_t ble_get_ulp_status(void);

/** \brief ble_sdk_version gets the version of BluSmartSDK and also checks the SDK and Library version compatability  
  *
  * \param[in] None
  *
  * \return BluSmartSDK version
  *
  */
uint32_t ble_sdk_version(void);

#if defined PDS_SERVICE

/**
 * \brief  Initialize the PDS Module.
 *
 *  IInitialize the PDS & NVM Memory 
 *
 * \param[in] None
 *
 * \return Status of the PDS Module Initialize procedure
 *
 * \retval AT_BLE_SUCCESS PDS Module Initialize is completed
 * 
 * \retval  AT_BLE_FAILURE PDS Module Initialization Failure
 * 
 */
at_ble_status_t pds_module_init(void);

/**
 * \brief  Store the bonding information
 *
 *
 * \param[in] None
 *
 * \return Status of the PDS bonding information store procedure
 *
 * \retval AT_BLE_SUCCESS Stored the bonding information in PDS
 * 
 * \retval  AT_BLE_FAILURE Failed to store the bonding information in PDS
 * 
 */
at_ble_status_t ble_store_bonding_info(void *params);

/**
 * \brief  Restore the bonding information from PDS
 *
 *  Restore all the valid bonding information from PDS to device info table
 *
 * \param[in] None
 *
 * \return Status of the Restore bonding information
 *
 * \retval AT_BLE_SUCCESS PDS Restore bonding information is completed.
 * 
 * \retval  AT_BLE_FAILURE Restore bonding information failed
 * 
 */
at_ble_status_t ble_restore_bonding_info(void);

/**
 * \brief  Remove all bonding information from PDS
 *
 *  Remove all the valid bonding informations from PDS
 *
 * \param[in] None
 *
 * \return Status of the Remove bonding information procedure
 *
 * \retval AT_BLE_SUCCESS bonding information removed successfully from the PDS
 * 
 * \retval  AT_BLE_FAILURE Failed to remove the bonding information from PDS
 * 
 */
at_ble_status_t ble_remove_bonding_info(void);

#endif /* defined PDS_SERVICE */


#endif /*__BLE_MANAGER_H__*/
// </h>

// <<< end of configuration section >>>
