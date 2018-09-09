/**
 * \file
 *
 * \brief HID Mouse Device Profile Application declarations
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

/**
 * \mainpage HID Mouse Application
 * \section Introduction
 * ******************************Introduction ***********************************
 *
 * The HID Mouse example application bring-up the HID over GATT (HOGP) profile
 * defined by the Bluetooth SIG.
 * The HID over GATT (HOGP) profile defined by the Bluetooth SIG enables support
 * of HID services over a Bluetooth Low Energy (BLE) protocol stack using 
 * Generic Attribute profile (GATT). This allows devices like keyboard or mouse
 * implementing HOGP to connect to a compatible HOGP/BLE host device 
 * (e.g.: Mobile Phone, Tablet, TV, etc.). 
 *
 * - Running the demo -
 *  + 1. Build and flash the binary into supported evaluation board.
 *  + 2. Open the console using TeraTerm or any serial port monitor.
 *  + 3. Press the Reset button.
 *  + 4. Wait for around 10 seconds for the patches to be downloaded device will initialize and start-up.
 *  + 5. The device is now in advertising mode.
 *  + 6. The demo requires use of an Android mobile phone supporting HOGP.
 *       The HOGP profile is natively supported in Android from version 4.4 
 *      (Android KitKat) and higher versions. The phone must include support a Bluetooth
 *       chipset supporting BT 4.0 or higher version on the mobile phone, In Bluetooth 
 *       settings scan for the devices, device with “ATMEL-HID” will be found 
 *       as shown below. Click on “ATMEL-HID” to get connected.
 *  + 7. Once connected, the client side will request for the pairing procedure . 
 *       The console log provides a guidance for the user to enter the pass-key.
 *  + 8. Once the device connected to host (Phone). User can click on SW0 button
 *       on supported platform for simulating mouse movement.
 *  + 9. For every press on button, the user can see corresponding cursor movement
 *       on HID host as described below:
 *       + First 5 Button Press:	 Cursor moves right
 *       + Next 5 Button Press:	 Cursor moved down
 *       + Next 5 Button Press:	 Cursor moves left
 *       + Next 5 Button Press:	 Cursor moved up.
 *
 * \section Modules
 * ***************************** MODULES ***************************************** 
 * - BLE Manager - 
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
 * - BLE Profile-
 *  + This profile defines how a device with Bluetooth low energy wireless 
 *    communications can support HID services over the Bluetooth low energy
 *    protocol stack using the Generic Attribute Profile.
 *    HID Over GATT Profile supports two services:
 *    + Human Interface Device - 
 *      + The HID Service exposes characteristics required for a HID Device to
 *        transfer HID report descriptors and reports to a HID Host. This also
 *        exposes the characteristics for a HID Host to write to a Device. 
 *        The Human Interface Device Service is instantiated as a Primary Service.
 *    + Device Information service -
 *      + The Device Information Service exposes manufacturer and/or vendor 
 *        information about a device.
 *  
 * - BLE Platform Services -
 *  +  Serial Console COM port settings -
 *    + Baudrate 115200
 *	  + Parity None, Stop Bit 1, Start Bit 1
 *	  + No Hardware Handshake
 *
 * \section BluSDK Package
 * ***************************** BluSDK Package *****************************************
 * - Links for Docs -
 *		+ http://www.microchip.com/wwwproducts/en/ATSAMB11
 *		+ http://www.microchip.com/developmenttools/productdetails.aspx?partno=atsamb11-xpro
 * - Support and FAQ - visit -
 *		+ <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

/*- Includes -----------------------------------------------------------------------*/
#include <asf.h>
#include "platform.h"
#include "at_ble_api.h"
#include "ble_manager.h"
#include "hid.h"
#include "hid_mouse_app.h"
#include "device_info.h"
#include "ble_utils.h"
#include "console_serial.h"
#include "timer_hw.h"
#include "button.h"
#include "samb11_delay.h"

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

/* Mouse report value */
int8_t app_mouse_report[4] = {0x00, 0x00, 0x00, 0x00};		
	
/* Profile connection status */
uint8_t conn_status = 0;
	
/*Counter*/
uint8_t cnt = 0;

/*Mouse Movement Position*/
uint8_t mouse_pos = MOUSE_RIGHT_MOVEMENT;
extern bool button_debounce;

/* Mouse report info*/
static uint8_t hid_app_mouse_report_map[] =
{
	0x05, 0x01,	/*	Usage Page (Generic Desktop),       */
	0x09, 0x02,	/*	Usage (Mouse),                      */
	0xA1, 0x01,	/*	Collection (Application),          */
	0x85, 0x01, /*	REPORT ID (1) - MANDATORY		  */
	0x09, 0x01,	/*	Usage (Pointer),                  */
	0xA1, 0x00,	/*	Collection (Physical),             */
	0x05, 0x09,	/*    Usage Page (Buttons),           */
	0x19, 0x01,	/*	Usage Minimum (01),             */
	0x29, 0x03,	/*    Usage Maximum (03),             */
	0x15, 0x00,	/*    Logical Minimum (0),            */
	0x25, 0x01,	/*    Logical Maximum (1),            */
	0x75, 0x01,	/*    Report Size (1),                */
	0x95, 0x03,	/*	Report Count (3),               */
	0x81, 0x02,	/*	Input (Data, Variable, Absolute) */
	0x75, 0x05,	/*	Report Size (5),                */
	0x95, 0x01,	/*	Report Count (1),               */
	0x81, 0x01,	/*	Input (Constant),               */
	0x05, 0x01,	/*	Usage Page (Generic Desktop),   */
	0x09, 0x30,	/*	Usage (X),                      */
	0x09, 0x31,	/*	Usage (Y),                      */
	0x09, 0x38,	/*	Usage (Scroll),                 */
	0x15, 0x81,	/*	Logical Minimum (-127),         */
	0x25, 0x7F,	/*	Logical Maximum (127),          */
	0x75, 0x08,	/*	Report Size (8),                */
	0x95, 0x03,	/*	Report Count (3),               */
	0x81, 0x06,	/*	Input (Data, Variable, Relative) */
	0xC0,		/*  End Collection,                    */
	0xC0,		/* End Collection                      */
};

static const ble_gap_event_cb_t hid_app_gap_handle = {
	.connected = hid_connect_cb,
	.disconnected = hid_disconnect_cb
};

static const ble_gatt_server_event_cb_t hid_app_gatt_server_handle = {
	.notification_confirmed = hid_notification_confirmed_cb,

};

void button_cb(void);

/* All BLE Manager Custom Event callback */
static const ble_custom_event_cb_t hid_custom_event_cb = {
	.platform_event_ready = hid_platform_event
};



/* Callback called during connection */
static at_ble_status_t hid_connect_cb(void *params)
{
	mouse_pos = MOUSE_RIGHT_MOVEMENT;
	conn_status = 1;
	cnt = 0;
	app_mouse_report[0] = 0;
	app_mouse_report[1] = 0;
	app_mouse_report[2] = 0;
	app_mouse_report[3] = 0;
	ALL_UNUSED(&params);
	return AT_BLE_SUCCESS;
}

/* Callback called during disconnect */
static at_ble_status_t hid_disconnect_cb(void *params)
{
	mouse_pos = MOUSE_RIGHT_MOVEMENT;
	conn_status = 0;
	cnt = 0;
	app_mouse_report[0] = 0;
	app_mouse_report[1] = 0;
	app_mouse_report[2] = 0;
	app_mouse_report[3] = 0;
	ALL_UNUSED(&params);
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
	DBG_LOG_DEV("Mouse report send to host status %d", notification_status->status);
	return AT_BLE_SUCCESS;
}

/* Callback called when user press the button for writing new characteristic value */
void button_cb()
{
	if(conn_status){
	send_plf_int_msg_ind(RAM_ISR_TABLE_PORT0_COMB_INDEX, PORTINT_CALLBACK_TYPE_DETECT, NULL, 0);
	}
}

/* Initialize the application information for HID profile*/
static void hid_mouse_app_init(void)
{
#ifdef ENABLE_PTS
	uint16_t i=0;
#endif
	hid_prf_data.hid_serv_instance = 1;
	hid_prf_data.hid_device = HID_MOUSE_MODE; 
	hid_prf_data.protocol_mode = HID_REPORT_PROTOCOL_MODE; 
	hid_prf_data.num_of_report = HID_NUM_OF_REPORT;
	/*Update the report information based on report id, User can allocate maximum HID_MAX_REPORT_NUM number of report*/
	hid_prf_data.report_id[0] = 1;  
	hid_prf_data.report_type[0] = INPUT_REPORT;  
	hid_prf_data.report_val[0] = (uint8_t *)&app_mouse_report[0];	
	hid_prf_data.report_len[0] = sizeof(app_mouse_report);
	hid_prf_data.report_map_info.report_map = hid_app_mouse_report_map;
	hid_prf_data.report_map_info.report_map_len = sizeof(hid_app_mouse_report_map);
	hid_prf_data.hid_device_info.bcd_hid = 0x0111;        
	hid_prf_data.hid_device_info.bcountry_code = 0x00;
	hid_prf_data.hid_device_info.flags = 0x02; 
	
#ifdef ENABLE_PTS	
	DBG_LOG_PTS("Report Map Characteristic Value");
	DBG_LOG_PTS("\r\n");
	for (i=0; i<sizeof(hid_app_mouse_report_map); i++)
	{
		DBG_LOG_PTS(" 0x%02X ", hid_app_mouse_report_map[i]);
	}
	DBG_LOG_PTS("\r\n");
	DBG_LOG_PTS("HID Information Characteristic Value");
	DBG_LOG_PTS("bcdHID 0x%02X, bCountryCode 0x%02X Flags 0x%02X", hid_prf_data.hid_device_info.bcd_hid, hid_prf_data.hid_device_info.bcountry_code, hid_prf_data.hid_device_info.flags);
#endif
	
	if(hid_prf_conf(&hid_prf_data)==HID_PRF_SUCESS){
		DBG_LOG("HID Profile Configured");
	}else{
		DBG_LOG("HID Profile Configuration Failed");
	}
}

/* Function used for HID mouse movement*/
static bool hid_mouse_move(int8_t pos, uint8_t index_report)
{
	int16_t s16_newpos;
	s16_newpos = (int8_t) app_mouse_report[index_report];
	s16_newpos += pos;
	if ((-127 > s16_newpos) || (127 < s16_newpos)) {
		return false;
	}
	app_mouse_report[index_report] = (uint8_t) s16_newpos;
	return true;
}

static at_ble_status_t hid_platform_event(void *param)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
	platform_isr_event_t *plf_isr_event = (platform_isr_event_t *)param;
	if(plf_isr_event->event_type == ((PORTINT_CALLBACK_TYPE_DETECT << 8)  | RAM_ISR_TABLE_PORT0_COMB_INDEX))
	{
		/* Check for key status */
		if(conn_status){

			switch(mouse_pos)
			{
				case MOUSE_RIGHT_MOVEMENT:
				{
					if(hid_mouse_move(MOUSE_MOVEMENT_POS, 1)){
						++cnt;
						app_mouse_report[2] = 0;
						DBG_LOG("Mouse Right Movement");
						if(cnt == MOUSE_CHANGE_DIRECTION){
							mouse_pos = MOUSE_DOWN_MOVEMENT;
							cnt = 0;
						}
					}
				}
				break;
				
				case MOUSE_LEFT_MOVEMENT:
				{
					if(hid_mouse_move(-MOUSE_MOVEMENT_POS, 1)){
						++cnt;
						app_mouse_report[2] = 0;
						DBG_LOG("Mouse Left  Movement");
						if(cnt == MOUSE_CHANGE_DIRECTION){
							mouse_pos = MOUSE_UP_MOVEMENT;
							cnt = 0;
						}
					}
				}
				break;
				
				case MOUSE_UP_MOVEMENT:
				{
					if(hid_mouse_move(-MOUSE_MOVEMENT_POS, 2)){
						++cnt;
						app_mouse_report[1] = 0;
						DBG_LOG("Mouse UP    Movement");
						if(cnt == MOUSE_CHANGE_DIRECTION)
						{
							mouse_pos = MOUSE_RIGHT_MOVEMENT;
							cnt = 0;
						}
					}
				}
				break;
				
				case MOUSE_DOWN_MOVEMENT:
				{
					if(hid_mouse_move(MOUSE_MOVEMENT_POS, 2)){
						++cnt;
						app_mouse_report[1] = 0;
						DBG_LOG("Mouse Down  Movement");
						if(cnt == MOUSE_CHANGE_DIRECTION){
							mouse_pos = MOUSE_LEFT_MOVEMENT;
							cnt = 0;
						}
					}
				}
				break;
			}
			hid_prf_report_update(report_ntf_info.conn_handle, report_ntf_info.serv_inst, 1, (uint8_t *)app_mouse_report, sizeof(app_mouse_report));
			delay_ms(200);
	}
	else
	{
		status = AT_BLE_FAILURE;
	}
	button_debounce = true;
	return status;
	}
}

int main(void )
{
	/*intialize system driver */
	platform_driver_init();
	acquire_sleep_lock();

	/* Initialize serial console */
	serial_console_init();
	
	DBG_LOG("Initializing HID Mouse Application");

	/* Initialize button*/
	gpio_init();
	button_init();
	button_register_callback(button_cb);

	/* Initialize the profile based on user input */
	hid_mouse_app_init();
	
	/* initialize the ble chip  and Set the device mac address */
	ble_device_init(NULL);
	
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
#ifdef ENABLE_ULP
	ble_set_ulp_mode(BLE_ULP_MODE_CLEAR);
#endif
	/* Capturing the events  */
	while(1){
		ble_event_task(0xFFFFFFFF);
		}
	}



