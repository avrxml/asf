/**
 * \file
 *
 * \brief HID Keyboard Device Profile Application declarations
 *
 * Copyright (c) 2017-2018 Microchip Technology Inc. and its subsidiaries.
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
/**
 * \mainpage HID KEY BOARD Example
 * \section Introduction
 * **************************** Introduction *********************************
 * + The HID Key board example application bring-up the HID over GATT (HOGP) profile defined by the Bluetooth SIG.
 * + The HID over GATT (HOGP) profile defined by the Bluetooth SIG enables support of HID services over a Bluetooth Low Energy (BLE) 
 *   protocol stack using Generic Attribute profile (GATT). This allows devices like keyboard or mouse implementing HOGP to connect 
 *   to a compatible HOGP/BLE host device (e.g.: Mobile Phone, Tablet, TV, etc.).
 *- Supported Evolution Kit -
 *	+ ATSAML21-XPRO-B + ATBTLC1000 XPRO
 *	+ ATSAMD21-XPRO + ATBTLC1000 XPRO
 *	+ ATSAMG55-XPRO + ATBTLC1000 XPRO
 *	+ ATSAM4S-XPRO + ATBTLC1000 XPRO
 *- Running the Demo -
 * + 1. Build and flash the binary into supported evaluation board.
 * + 2. Open the console using TeraTerm or any serial port monitor.
 * + 3. Press the Reset button.
 * + 4. Wait for around 10 seconds for the patches to be downloaded device will initialize and start-up.
 * + 5. The device is now in advertising mode.
 * + 6. The demo requires use of an Android mobile phone supporting HOGP.  The HOGP profile is natively supported in 
 *      Android from version 4.4 (Android KitKat) and higher versions. The phone must include support a Bluetooth chipset supporting BT 4.0 or 
 *      higher version on the mobile phone, In Bluetooth settings scan for the devices, device with “ATMEL-HID” will be found as shown below. 
 *      Click on “ATMEL-HID” to get connected.
 * + 7. Once connected, the client side will request for the pairing procedure . The console log provides a guidance for the user to 
        enter the pass-key.
 * + 8. Once the device connected to host (Phone). User can click on SW0 button on supported platform for simulating mouse movement.
 * + 9. For every press on button, the user can see corresponding string on HID host as described below: 
 *      + Hello Atmel
 * \section Modules
 * ***************************** Modules **************************************
 *- BLE Manger -  
 *  + The Event Manager is responsible for handling the following:
 *    + Generic BLE Event Handling:-
 *       + BLE Event Manager handles the events triggered by BLE stack and also responsible 
 *  	 for invoking all registered callbacks for respective events. BLE Manager 
 *  	 handles all GAP related functionality. In addition to that handles multiple connection 
 *  	 instances, Pairing, Encryption, Scanning.
 *    + Handling Multi-role/multi-connection:-
 *  	  + BLE Event Manager is responsible for handling multiple connection instances 
 *  	  and stores bonding information and Keys to retain the bonded device. 
 *  	  BLE Manager is able to identify and remove the device information when pairing/encryption 
 *		  gets failed. In case of multi-role, it handles the state/event handling of both central and peripheral in multiple contexts.
 *    + Controlling the Advertisement data:-
 *  	  + BLE Event Manager is responsible for generating the advertisement and scan response data
 *  	  for BLE profiles/services that are attached with BLE Manager.
 *
 *- BLE Profile -
 *  + This profile defines how a device with Bluetooth low energy wireless communications can support HID services over the Bluetooth 
 *    low energy protocol stack using the Generic Attribute Profile.
 *  + HID Over GATT Profile supports two services:
 *  + **Human Interface Device**: 
 *    + The HID Service exposes characteristics required for a HID Device to transfer HID report descriptors and reports to a HID Host. 
 *    This also exposes the characteristics for a HID Host to write to a Device. The Human Interface Device Service is instantiated as a Primary Service.
 *  + **Device Information service**: 
 *    + The Device Information Service exposes manufacturer and/or vendor information about a device.
 *
 *- BLE Platform Services -
 *  +  Interface Settings -
 *	  + Connect ATBTLC1000 XPRO to SAML21-XPRO-B -> EXT1
 *	  + Connect ATBTLC1000 XPRO to SAMD21-XPRO -> EXT1
 *	  + Connect ATBTLC1000 XPRO to SAMG55-XPRO -> EXT1
 *	  + Connect ATBTLC1000 XPRO to SAM4S-XPRO  -> EXT1
 *  +  Serial Console COM port settings -
 *    + Baudrate 115200
 *	  + Parity None, Stop Bit 1, Start Bit 1
 *	  + No Hardware Handshake
 *	+  6-Wire Mode Connection Setup -
 *    + Pins are 1:1 match with SAML21/D21 Xpro EXT1 Header to BTLC1000 XPro Header
 *	  + UART(No Flow Control)-SAM L21/D21 XPro Pins (Rx-Pin13, Tx-Pin14)
 *	  + UART(With Flow Control)-SAM G55 Xpro Pins (Rx-Pin13, Tx-Pin14, RTS-Pin5, CTS-Pin6, Rx-Pin16, Tx-Pin17)
 *	  + BTLC1000 Wakeup Pin-SAM G55 XPro Pins(Pin4)
 *	  + BTLC1000 Chip Enable Pin-SAM G55 XPro Pins(Pin10)
 *	  + BTLC1000 Vcc Pin-SAM L21/D21/G55 Xpro Pins(Pin20)
 *	  + BTLC1000 GND Pin-SAM L21/D21/G55 Xpro Pins(Pin19)
 *  +  4-Wire Mode Connection setup -
 * 	  + UART(With Flow Control)-SAM L21/D21 XPro Pins (Rx-Pin15, Tx-Pin17, RTS-Pin16, CTS-Pin18)
 * 	  + BTLC1000 Wakeup Pin-SAM L21/D21 XPro Pins (Rx-Pin6)
 * 	  + BTLC1000 Chip Enable Pin-SAM L21/D21 XPro Pins (Rx-Pin4)
 * 	  + UART(With Flow Control)-SAM G55/4S Xpro Pins (Rx-Pin13, Tx-Pin14, RTS-Pin5, CTS-Pin6)
 * 	  + BTLC1000 Wakeup Pin-SAM G55/4S XPro Pins(Pin4)
 * 	  + BTLC1000 Chip Enable Pin-SAM G55/4S XPro Pins(Pin10)
 * 	  + BTLC1000 Vcc Pin-SAM L21/D21/G55/4S Xpro Pins(Pin20)
 * 	  + BTLC1000 GND Pin-SAM L21/D21/G55/4S Xpro Pins(Pin19)
 *
 *\section BLE SDK Package
 * ***************************** BLE SDK Package ******************************************
 *- Links for BluSDK -
 *		+ https://www.microchip.com/wwwproducts/en/ATBTLC1000?tab=documents
 *- Links for ATBTLC1000 -
 *		+ https://www.microchip.com/wwwproducts/en/ATBTLC1000
 *- Development Kit -
 *		+ https://www.microchip.com/wwwproducts/en/ATBTLC1000?tab=tools
 *- SAM L21 + BTLC1000 XPro -
 *		+ http://www.microchip.com/developmenttools/productdetails/atbtlc1000-xstk
 *- BTLC1000 XPro -
 *		+ http://www.microchip.com/developmenttools/productdetails/atbtlc1000-xpro
 *- Applications -
 *		+ https://www.microchip.com/wwwproducts/en/ATBTLC1000?tab=applications
 *- Support and FAQ visit - 
 *		+ <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

/**
 * \mainpage
 * \section preface Preface
 * This is the reference manual for the HID Keyboard Device Profile Application declarations
 */
/*- Includes -----------------------------------------------------------------------*/
#include <asf.h>
#include "platform.h"
#include "at_ble_api.h"
#include "console_serial.h"
#include "ble_manager.h"
#include "hid.h"
#include "hid_keyboard_app.h"
#include "device_info.h"
#include "ble_utils.h"
#include "timer_hw.h"
#include "conf_extint.h"
#include "hid_device.h"


/* =========================== GLOBALS ============================================================ */

/* Control point notification structure */
hid_control_mode_ntf_t hid_control_point_value; 

/* Report notification structure */
hid_report_ntf_t report_ntf_info;

/* Boot notification structure */
hid_boot_ntf_t  boot_ntf_info;

/* Protocol mode notification structure */
hid_proto_mode_ntf_t hid_proto_mode_value;

/* HID profile structure for application */
hid_prf_info_t hid_prf_data;

/* Keyboard character info to be displayed during demo */
uint8_t keyb_disp[12] = {0x0B, 0x08, 0x0F, 0x0F, 0x12, 0x2C, 0x04, 0x17, 0x10, 0x08, 0x0F, 0x28};

/* Keyboard character to be printed */
uint8_t keyb_id = 0;

/* Profile connection status */
uint8_t conn_status = 0;

/* Keyboard report value */
uint8_t app_keyb_report[8] = {0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00};	
	
/* Keyboard key status */
volatile uint8_t key_status = 0;

/* Flag to avoid spurious interrupt posting  during/before reset and initialization completed */
volatile bool app_init_done = false;

bool app_exec = true;

user_custom_event_t app_button_event =
{
	.id = APP_BUTTON_EVENT_ID,
	.bptr = NULL
};

static const ble_gap_event_cb_t hid_app_gap_handle = {
	.connected = hid_connect_cb,
	.disconnected = hid_disconnect_cb
};

static const ble_gatt_server_event_cb_t hid_app_gatt_server_handle = {
	.notification_confirmed = hid_notification_confirmed_cb
};

/* All BLE Manager Custom Event callback */
static const ble_custom_event_cb_t hid_custom_event_cb = {
	.custom_event = hid_custom_event
};

/* keyboard report */
static uint8_t hid_app_keyb_report_map[] =
{
   0x05, 0x01,		/* Usage Page (Generic Desktop)      */
   0x09, 0x06,		/* Usage (Keyboard)                  */
   0xA1, 0x01,		/* Collection (Application)          */
   0x85, 0x01,		/* REPORT ID (1) - MANDATORY         */ 
   0x05, 0x07,		/* Usage Page (Keyboard)             */
   0x19, 224,		/* Usage Minimum (224)               */
   0x29, 231,		/* Usage Maximum (231)               */
   0x15, 0x00,		/* Logical Minimum (0)               */
   0x25, 0x01,		/* Logical Maximum (1)               */
   0x75, 0x01,		/* Report Size (1)                   */
   0x95, 0x08,		/* Report Count (8)                  */
   0x81, 0x02,		/* Input (Data, Variable, Absolute)  */
   0x81, 0x01,		/* Input (Constant)                  */
   0x19, 0x00,		/* Usage Minimum (0)                 */
   0x29, 101,		/* Usage Maximum (101)               */
   0x15, 0x00,		/* Logical Minimum (0)               */
   0x25, 101,		/* Logical Maximum (101)             */
   0x75, 0x08,		/* Report Size (8)                   */
   0x95, 0x06,		/* Report Count (6)                  */
   0x81, 0x00,		/* Input (Data, Array)               */
   0x05, 0x08,		/* Usage Page (LED)                  */
   0x19, 0x01,		/* Usage Minimum (1)                 */
   0x29, 0x05,		/* Usage Maximum (5)                 */
   0x15, 0x00,		/* Logical Minimum (0)               */
   0x25, 0x01,		/* Logical Maximum (1)               */
   0x75, 0x01,		/* Report Size (1)                   */
   0x95, 0x05,		/* Report Count (5)                  */
   0x91, 0x02,		/* Output (Data, Variable, Absolute) */
   0x95, 0x03,		/* Report Count (3)                  */
   0x91, 0x01,		/* Output (Constant)                 */
   0xC0				/* End Collection                    */
};

static at_ble_status_t hid_custom_event(void *param)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
	key_status = 1;
	return status;
}

/* Callback called during connection */
static at_ble_status_t hid_connect_cb(void *params)
{
	at_ble_handle_t *handle;
	handle = (at_ble_handle_t *)params;
	keyb_id = 0;
	conn_status = 1;
	ALL_UNUSED(&handle);
	
	return AT_BLE_SUCCESS;
}

/* Callback called during disconnect */
static at_ble_status_t hid_disconnect_cb(void *params)
{
	at_ble_handle_t *handle;
	handle =(at_ble_handle_t *)params;
	keyb_id = 0;
	conn_status = 0;
    ALL_UNUSED(&handle);
	return AT_BLE_SUCCESS;
}

/* Callback called when host change the control point value */
static void hid_prf_control_point_ntf_cb(hid_control_mode_ntf_t *hid_control_point_value_t)
{
	DBG_LOG_DEV("Control Point Notification Callback :: Service Instance %d Control Value %d", 
					hid_control_point_value_t->serv_inst, hid_control_point_value_t->control_value);
}

/* Callback called when host change the protocol mode value */
static void hid_prf_protocol_mode_ntf_cb(hid_proto_mode_ntf_t *protocol_mode)
{
	DBG_LOG_DEV("Protocol Mode Notification Callback :: Service Instance %d  New Protocol Mode  %d  Connection Handle %d", 
					protocol_mode->serv_inst, protocol_mode->mode, protocol_mode->conn_handle);
}

/* Callback called when host enable/disable the notification for boot report (Mouse/Keyboard)
   Mouse Boot Value 4
   Keyboard Boot Value 6
*/
static void hid_prf_boot_ntf_cb(hid_boot_ntf_t *boot_ntf_info_t)
{
	DBG_LOG_DEV("Boot Notification Callback :: Service Instance %d  Boot Value  %d  Notification(Enable/Disable) %d", 
					boot_ntf_info_t->serv_inst, boot_ntf_info_t->boot_value, boot_ntf_info_t->ntf_conf);
}

/* Callback called when host enable/disable the notification for report (Mouse/Keyboard) */
static void hid_prf_report_ntf_cb(hid_report_ntf_t *report_info)
{
	DBG_LOG_DEV("Report Notification Callback Service Instance %d  Report ID  %d  Notification(Enable/Disable) %d Connection Handle %d", 
					report_info->serv_inst, report_info->report_ID, report_info->ntf_conf, report_info->conn_handle);
    report_ntf_info.serv_inst = report_info->serv_inst;
	report_ntf_info.report_ID = report_info->report_ID;
	report_ntf_info.ntf_conf = report_info->ntf_conf;
	report_ntf_info.conn_handle = report_info->conn_handle;
}

/* Callback called when report send over the air */
static at_ble_status_t hid_notification_confirmed_cb(void *params)
{
	at_ble_cmd_complete_event_t *notification_status;
	notification_status = (at_ble_cmd_complete_event_t *)params;
	DBG_LOG_DEV("Keyboard report send to host status %d", notification_status->status);
	return AT_BLE_SUCCESS;
}

/* Callback called when user press the button for writing new characteristic value */
void button_cb(void)
{
	if(app_init_done && !key_status)
	{
		key_status = 1;
		at_ble_event_user_defined_post(&app_button_event);	
	}
}

/* Initialize the application information for HID profile*/
static void hid_keyboard_app_init(void)
{
#ifdef ENABLE_PTS	
	uint16_t i=0;
#endif	
	hid_prf_data.hid_serv_instance = 1;
	hid_prf_data.hid_device = HID_KEYBOARD_MODE; 
	hid_prf_data.protocol_mode = HID_REPORT_PROTOCOL_MODE; 
	hid_prf_data.num_of_report = HID_NUM_OF_REPORT;
	
	/*Update the report information based on report id, User can allocate maximum HID_MAX_REPORT_NUM number of report*/
	hid_prf_data.report_id[0] = 1;  
	hid_prf_data.report_type[0] = INPUT_REPORT;  
	
	hid_prf_data.report_val[0] = &app_keyb_report[0];	
	hid_prf_data.report_len[0] = sizeof(app_keyb_report);
	hid_prf_data.report_map_info.report_map = hid_app_keyb_report_map;
	hid_prf_data.report_map_info.report_map_len = sizeof(hid_app_keyb_report_map);
	hid_prf_data.hid_device_info.bcd_hid = 0x0111;        
	hid_prf_data.hid_device_info.bcountry_code = 0x00;
	hid_prf_data.hid_device_info.flags = 0x02; 
	
#ifdef ENABLE_PTS
	DBG_LOG_PTS("Report Map Characteristic Value");
	DBG_LOG_PTS("\r\n");
	for (i=0; i<sizeof(hid_app_keyb_report_map); i++)
	{
		DBG_LOG_PTS(" 0x%02X ", hid_app_keyb_report_map[i]);
	}
	DBG_LOG_PTS("\r\n");	
	DBG_LOG_PTS("HID Information Characteristic Value");
	DBG_LOG_PTS("bcdHID 0x%02X, bCountryCode 0x%02X Flags 0x%02X", hid_prf_data.hid_device_info.bcd_hid, hid_prf_data.hid_device_info.bcountry_code, hid_prf_data.hid_device_info.flags);
#endif // _DEBUG
	
	if(hid_prf_conf(&hid_prf_data)==HID_PRF_SUCESS){
		DBG_LOG("HID Profile Configured");
	}else{
		DBG_LOG("HID Profile Configuration Failed");
	}
}

int main(void )
{
	
#if SAMG55 || SAM4S
	/* Initialize the SAM system. */
	sysclk_init();
	board_init();
#elif SAM0
	system_init();
#endif
	
	/* Initialize serial console */
	serial_console_init();
	
	/* Initialize button*/
	button_init();
	
	DBG_LOG("Initializing HID Keyboard Application");
	
	/* Initialize the profile based on user input */
	hid_keyboard_app_init();
	
	/* initialize the ble chip  and Set the device mac address */
	ble_device_init(NULL);
	
	app_init_done = true;
	
	hid_prf_init(NULL);
	
	/* Register the notification handler */
	notify_report_ntf_handler(hid_prf_report_ntf_cb);
	notify_boot_ntf_handler(hid_prf_boot_ntf_cb);
	notify_protocol_mode_handler(hid_prf_protocol_mode_ntf_cb);
	notify_control_point_handler(hid_prf_control_point_ntf_cb);
	
	/* Callback registering for BLE-GAP Role */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK, 
									BLE_GAP_EVENT_TYPE, 
									&hid_app_gap_handle);
	
	/* Callback registering for BLE-GATT-Server Role */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK, 
									BLE_GATT_SERVER_EVENT_TYPE, 
									&hid_app_gatt_server_handle);
	
	/* Register callbacks for custom related events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK, 
									BLE_CUSTOM_EVENT_TYPE, 
									&hid_custom_event_cb);
	
	/* Capturing the events  */
	while(app_exec)
	{
		ble_event_task();
				
		/* Check for key status */
		if(key_status && conn_status){
			DBG_LOG("Key Pressed...");
			delay_ms(KEY_PAD_DEBOUNCE_TIME);
			if((keyb_id == POSITION_ZERO) || (keyb_id == POSITION_SIX)){
				app_keyb_report[0] = CAPS_ON;
			}else{
				app_keyb_report[0] = CAPS_OFF;
			}
			
			app_keyb_report[2] = keyb_disp[keyb_id];
			hid_prf_report_update(report_ntf_info.conn_handle, report_ntf_info.serv_inst, 1, app_keyb_report, sizeof(app_keyb_report));
			delay_ms(20);
			app_keyb_report[2] = 0x00;
			hid_prf_report_update(report_ntf_info.conn_handle, report_ntf_info.serv_inst, 1, app_keyb_report, sizeof(app_keyb_report));
				
			key_status = 0;
			
			if(keyb_id == MAX_TEXT_LEN)
			{
				keyb_id = 0;
			}
			else
			{
				++keyb_id;
			}
		}
	}
}
