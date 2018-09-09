/**
* \file
*
* \brief Device Information Service - Application
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
 * \mainpage DEVICE INFO Example
 * \section Introduction
 * **************************** Introduction *********************************
 * + The Device Information Application allow the user to define and use the BLE Dévice Information Service(DIS). 
 *   Any application discovering the database can access the DIS instance during discovery services.
 * + Supported Characteristics in Device Information Service:
 *   + Manufacturer name string 
 *   + Model number string 
 *   + Serial number string 
 *   + Hardware revision string 
 *   + Firmware revision string 
 *   + Software revision string 
 *   + System ID 
 *   + IEEE® 11073-20601 Regulatory Certification Data List 
 *   + PnP ID 
 *- Running the Demo -
 *  + 1. Build and flash the binary into supported evaluation board.
 *  + 2. Open the console using TeraTerm or any serial port monitor.
 *  + 3. Press the Reset button.
 *  + 4. Wait for around 10 seconds for the patches to be downloaded device will initialize and start-up.
 *  + 5. The device is now in advertising mode.
 *	+ 6. Enable Bluetooth® from Settings page on BLE compatible Android phone or iPhone®. Use the Atmel Smart Connect mobile application 
 *       to scan for peripheral devices. A device with name ‘ATMEL-DIS’ will appear among the list of scanned devices.
 *	+ 7. Click on 'ATMEL-DIS' device. A pop-up will appear requesting pass-key. Enter “123456” and click on ‘Pair’.
 *	+ 8. Once the Device Information service is clicked, User can see the DIS characteristics and device information.
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
 *  + The Device Information Service exposes manufacturer and/or vendor information about a device.
 *  + This service exposes manufacturer information about a device. 
 *  + The Device Information Service is instantiated as a Primary Service. 
 *  + Only one instance of the Device Information Service is exposed on a device.
 *  + Device Information service has nine characteristics:
 *  + **Manufacturer Name String**: 
 *    + This characteristic represents the name of the manufacturer of the device.
 *  + **Model Number String**: 
 *    + This characteristic represents the model number that is assigned by the device vendor.
 *  + **Serial Number String**: 
 *    + This characteristic represents the serial number for a particular instance of the device.
 *  + **Hardware Revision String**: 
 *    + This characteristic represents the hardware revision for the hardware within the device.
 *  + **Firmware Revision String**: 
 *    + This characteristic represents the firmware revision for the firmware within the device.
 *  + **Software Revision String**: 
 *    + This characteristic represents the software revision for the software within the device.
 *  + **System ID**: 
 *    + This characteristic represents a structure containing an Organizationally Unique Identifier (OUI) followed by a 
 *      manufacturer-defined identifier and is unique for each individual instance of the product.
 *  + **IEEE 11073-20601 Regulatory Certification Data List**: 
 *    + This characteristic represents regulatory and certification information for the product in a list defined in IEEE 11073-20601..
 *  + **PnP ID**: 
 *    + The PnP_ID characteristic is a set of values used to create a device ID value that is unique for this device.
 *- BLE Platform Services -
 *  +  Serial Console COM port settings -
 *    + Baudrate 115200
 *	  + Parity None, Stop Bit 1, Start Bit 1
 *	  + No Hardware Handshake
*
 *\section BLE SDK Package
 * ***************************** BLE SDK Package ******************************************
 * - Links for Docs -
 *		+ http://www.microchip.com/wwwproducts/en/ATSAMB11
 *		+ http://www.microchip.com/developmenttools/productdetails.aspx?partno=atsamb11-xpro
 *- Support and FAQ - visit -
 *		+ <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

/****************************************************************************************
*							        Includes	                                     	*
****************************************************************************************/


#include <asf.h>
/* #include "conf_extint.h" */
#include "console_serial.h"
#include "at_ble_api.h"
#include "platform.h"
#include "device_info.h"
#include "device_info_app.h"
#include "timer_hw.h"
#include "ble_utils.h"

#include "ble_manager.h"
#include "button.h"
#include "led.h"

/* === Macros ============================================================ */
#define FW_VERSION_SIZE (12)

/* === GLOBALS ============================================================ */

uint8_t fw_version[FW_VERSION_SIZE];
at_ble_handle_t dis_conn_handle;
dis_gatt_service_handler_t dis_service_handler;
/* To keep the application in execution continuously*/
bool app_exec = true;
volatile bool app_init_done = false;

/* Callback registered for AT_PLATFORM_EVENT event from stack */
static at_ble_status_t platform_interrupt_app_event(void *param)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
	platform_isr_event_t *plf_isr_event = (platform_isr_event_t *)param;
	if (plf_isr_event->event_type == ((TIMER_EXPIRED_CALLBACK_TYPE_DETECT << 8)  | RAM_ISR_TABLE_TIMER0_INDEX))
	{
		static uint8_t fw_update;
		dis_info_data fw_info_data;
		strcpy((char *)fw_version, "FW_VER-000");
		fw_update = fw_update %100;
		fw_version[8] = (fw_update / 10)+'0';
		fw_version[9] = (fw_update % 10)+'0';
		fw_info_data.info_data = (uint8_t *)fw_version;
		fw_info_data.data_len = 10;
		UPDATE_FIRMWARE_REVISION(&dis_service_handler, &fw_info_data, dis_conn_handle);
		fw_update++;
		DBG_LOG("Updating Firmware to ver:%s", fw_version);
		hw_timer_start(FIRMWARE_UPDATE_INTERVAL);
	}
	return status;
}

/**
* \Timer callback handler called on timer expiry
*/

static void timer_callback_handler(void)
{
	//Timer call back
	send_plf_int_msg_ind(RAM_ISR_TABLE_TIMER0_INDEX, TIMER_EXPIRED_CALLBACK_TYPE_DETECT, NULL, 0);;
}

/* Custom events like user defined event, platform event callback handlers for OTAU profile */
static const ble_custom_event_cb_t dis_sensor_app_custom_event_handle = {
	.platform_event_ready = platform_interrupt_app_event /* This event not handled in BLE Manager */
};

/* Advertisement data set and Advertisement start */
static at_ble_status_t device_information_advertise(void)
{	
	at_ble_status_t status = AT_BLE_FAILURE;
	
	if((status = ble_advertisement_data_set()) != AT_BLE_SUCCESS)
	{
		DBG_LOG("advertisement data set failed reason :%d",status);
		return status;
	}
	
	/* Start of advertisement */
	if((status = at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED, AT_BLE_ADV_GEN_DISCOVERABLE, NULL, AT_BLE_ADV_FP_ANY, APP_DIS_FAST_ADV, APP_DIS_ADV_TIMEOUT, 0)) == AT_BLE_SUCCESS)
	{
		DBG_LOG("BLE Started Adv");
		return AT_BLE_SUCCESS;
	}
	else
	{
		DBG_LOG("BLE Adv start Failed status :%d",status);
	}
	return status;
}

/* Callback registered for AT_BLE_PAIR_DONE event from stack */
static at_ble_status_t ble_paired_app_event(void *param)
{
	at_ble_pair_done_t *at_ble_pair_done = (at_ble_pair_done_t *)param;
	LED_On(LED0);
	hw_timer_start(FIRMWARE_UPDATE_INTERVAL);
	dis_conn_handle = at_ble_pair_done->handle;
	return AT_BLE_SUCCESS;
}

/* Callback registered for AT_BLE_ENCRYPTION_STATUS_CHANGED event from stack */
static at_ble_status_t ble_encryption_app_event(void *param)
{
	at_ble_encryption_status_changed_t *encryp_done = (at_ble_encryption_status_changed_t *)param;
	LED_On(LED0);
	hw_timer_start(FIRMWARE_UPDATE_INTERVAL);
	dis_conn_handle = encryp_done->handle;
	return AT_BLE_SUCCESS;
}

/* Callback registered for AT_BLE_DISCONNECTED event from stack */
static at_ble_status_t ble_disconnected_app_event(void *param)
{
	hw_timer_stop();
	LED_Off(LED0);
	device_information_advertise();
    ALL_UNUSED(param);
	return AT_BLE_SUCCESS;
}

/* Callback registered for AT_BLE_CONNECTED event from stack */
static at_ble_status_t ble_connected_app_event(void *param)
{
	#if !BLE_PAIR_ENABLE
		ble_paired_app_event(param);
	#else
		ALL_UNUSED(param);
	#endif
	return AT_BLE_SUCCESS;
}

static void button_cb(void)
{
	/* For user usage */
	DBG_LOG("button_cb");
}

static const ble_gap_event_cb_t device_info_app_gap_cb = {
	.connected = ble_connected_app_event,
	.disconnected = ble_disconnected_app_event,
	.pair_done = ble_paired_app_event,
	.encryption_status_changed = ble_encryption_app_event
};
 
/**
* \Device Information Service Application main function
*/
int main(void)
{
	at_ble_status_t status;

	platform_driver_init();
	acquire_sleep_lock();

	/* initialize the button */
	gpio_init();
	button_init();
	button_register_callback(button_cb);

	/* initialize the LED */
	led_init();

	/* Initialize serial console */
	serial_console_init();
	
	/* Initialize the hardware timer */
	hw_timer_init();
	
	/* Register the callback */
	hw_timer_register_callback(timer_callback_handler);
	
	DBG_LOG("Initializing Device Information Service Application");
	
	/* initialize the ble chip  and Set the device mac address */
	ble_device_init(NULL);



	/* Initialize the dis */
	dis_init_service(&dis_service_handler);
	
	/* Define the primary service in the GATT server database */
	if ((status = dis_primary_service_define(&dis_service_handler)) != AT_BLE_SUCCESS)
	{
		DBG_LOG("Device Information Service definition failed,reason %x",status);
	}
	
	device_information_advertise();
	
	/* Register callbacks for gap related events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_GAP_EVENT_TYPE,
									&device_info_app_gap_cb);

	/* Register callbacks for user defined custom events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_CUSTOM_EVENT_TYPE,
	&dis_sensor_app_custom_event_handle);
										
	/* Capturing the events  */ 
	while (app_exec) {
		/* BLE Event Task */
		ble_event_task(0xFFFFFFFF);	
	}	
	return 0;
}

