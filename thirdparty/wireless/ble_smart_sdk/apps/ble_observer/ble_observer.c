/**
 * \file
 *
 * \brief BLE Observer application
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
 * \mainpage BLE OBSERVER Example
 * \section Introduction
 * **************************** Introduction *********************************
 * + The BLE Observer Application that bring-up Bluetooth Observer Role on supported platforms.
 * + Observer Application continuously listens for BLE advertisement data over the air.
 * + This example application supports 12 advertisement data types. They are listed below: 
 * + Incomplete List of 16-bit Service Class UUIDs
 * + Complete List of 16-bit Service Class UUIDs
 * + Incomplete List of 32-bit Service Class UUIDs
 * + Complete List of 32-bit Service Class UUIDs
 * + Incomplete List of 128-bit Service Class UUIDs
 * + Complete List of 128-bit Service Class UUIDs
 * + Shortened Local Name
 * + Complete Local Name
 * + Appearance
 * + Manufacturer Specific Data
 * + TX Power
 * + Advertisement Interval
 *- Running the Demo -
 *  + 1. Build and flash the binary into supported evaluation board.
 *  + 2. Open the console using TeraTerm or any serial port monitor.
 *  + 3. Press the Reset button.
 *  + 4. Wait for around 10 seconds for the patches to be downloaded device will initialize and start-up.
 *  + 5. The device is now in advertising mode.
 *	+ 6. The device is now ready to be used as Observer and starts scaning for nearby BLE devices.
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
 *- BLE Platform Services -
 *  +  Serial Console COM port settings -
 *    + Baudrate 921600
 *	  + Parity None, Stop Bit 1, Start Bit 1
 *	  + No Hardware Handshake
 *
 *\section BLE SDK Package
 * ***************************** BLE SDK Package ******************************************
 * -Links for Docs -
 *		+ http://www.microchip.com/wwwproducts/en/ATSAMB11
 *		+ http://www.microchip.com/developmenttools/productdetails.aspx?partno=atsamb11-xpro
 *- Support and FAQ - visit -
 *		+ <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

/**
 * \mainpage
 * \section preface Preface
 * This is the reference manual for the Observer Application
 */
/*- Includes ---------------------------------------------------------------*/

#include <asf.h>
#include "platform.h"
#include "at_ble_api.h"
#include "console_serial.h"
#include "ble_observer.h"
#include "ble_utils.h"
#include "ble_manager.h"
#include "button.h"
#include "timer_hw.h"
#include "samb11_delay.h"
/** @brief Button event ID */
#define APP_BUTTON_EVENT_ID		(1)

uint32_t scan_device_count;

extern bool button_debounce;

volatile uint8_t scanning_state = false;

/* BLE GAP event callback handlers for Ble_Observer profile */
static const ble_gap_event_cb_t observer_app_gap_cb = {
	.scan_info = ble_observer_scan_info_handler,
	.scan_report = ble_observer_scan_data_handler
};

static void button_cb(void)
{
	send_plf_int_msg_ind(RAM_ISR_TABLE_PORT0_COMB_INDEX, PORTINT_CALLBACK_TYPE_DETECT, NULL, 0);
		
}

static at_ble_status_t ble_observer_platform_event(void *param)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
    uint8_t scan_status = AT_BLE_FAILURE;
	platform_isr_event_t *plf_isr_event = (platform_isr_event_t *)param;
	if(plf_isr_event->event_type == ((PORTINT_CALLBACK_TYPE_DETECT << 8)  | RAM_ISR_TABLE_PORT0_COMB_INDEX))
	{
	if(!scanning_state)
		{
			/* Initialize the scanning procedure */
			scan_status = gap_dev_scan();

			/* Check for scan status */
			if (scan_status == AT_BLE_SUCCESS)
			{
				DBG_LOG("Scanning process initiated");
				DBG_LOG("Press button to stop scanning..");
				scanning_state = true;
			}
			else if (scan_status == AT_BLE_INVALID_PARAM)
			{
				DBG_LOG("Scan parameters are invalid");
			}
			else if (scan_status == AT_BLE_FAILURE)
			{
				DBG_LOG("Scanning Failed Generic error");
			}
		}
		else
		{
			if(at_ble_scan_stop() != AT_BLE_SUCCESS)
			{
				/* If scan not started stop scan failed message will be displayed on terminal */
				/* If the scan already started then stop scan will return AT_BLE_SUCCESS */
				DBG_LOG_DEV("Stop scan failed");
			}
			else
			{
				DBG_LOG("Scanning process terminated");
				DBG_LOG("Press button to start scanning..");
				scanning_state = false;
			}
		}
		delay_ms(200);
		button_debounce = true;
	}	
	else
	{
		status = AT_BLE_FAILURE;
	}
	
	return status;
	
}

/* Custom events like user defined event, platform event callback handlers */
static const ble_custom_event_cb_t ble_observer_custom_event_cb = {
	.platform_event_ready = ble_observer_platform_event
};

/* Handler for Scan info data */
at_ble_status_t ble_observer_scan_info_handler(void * param)
{
	
	int8_t iterator;
	at_ble_scan_info_t *scan_info_data = (at_ble_scan_info_t *)param;
    ALL_UNUSED("%-28s", "\r\n\r\nAdvertisement type");
	ALL_UNUSED("    :  ");
	switch (scan_info_data->type) {
	case AT_BLE_ADV_TYPE_UNDIRECTED:
		ALL_UNUSED("ADV_IND");
		break;

	case AT_BLE_ADV_TYPE_DIRECTED:
		ALL_UNUSED("ADV_DIRECT_IND");
		break;

	case AT_BLE_ADV_TYPE_SCANNABLE_UNDIRECTED:
		ALL_UNUSED("ADV_SCANNABLE_UNDIRECTED");
		break;

	case AT_BLE_ADV_TYPE_NONCONN_UNDIRECTED:
		ALL_UNUSED("ADV_NONCONN_UNDIRECTED");
		break;

	case AT_BLE_ADV_TYPE_SCAN_RESPONSE:
		ALL_UNUSED("ADV_SCAN_RESPONSE");
		break;

	default:
		ALL_UNUSED("\r\n Unknown");
		break;
	}
	
	if(scan_info_data->type != AT_BLE_ADV_TYPE_SCAN_RESPONSE)
	{
		scan_device_count++;
		DBG_LOG("Scan Device Count");
		DBG_LOG_CONT(":%lu",scan_device_count);
	}

	/*Device Adress Type */
	ALL_UNUSED("%-28s", "Device address type");
	ALL_UNUSED(":  ");
	switch (scan_info_data->dev_addr.type) {
	case AT_BLE_ADDRESS_PUBLIC:
		ALL_UNUSED("PUBLIC_ADDRESS");
		break;

	case AT_BLE_ADDRESS_RANDOM_STATIC:
		ALL_UNUSED("STATIC_RANDOM_ADDRESS");
		break;

	case AT_BLE_ADDRESS_RANDOM_PRIVATE_RESOLVABLE:
		ALL_UNUSED("RANDOM_RESOLVABLE_PRIVATE_ADDRESS");
		break;

	case AT_BLE_ADDRESS_RANDOM_PRIVATE_NON_RESOLVABLE:
		ALL_UNUSED("RANDOM_NON_RESOLVABLE_PRIVATE_ADDRESS");
		break;

	default:
		ALL_UNUSED(" Unknown");
	}

	/*Device Address */
	DBG_LOG("BD Addr :");
	for (iterator = 5; iterator >= 0; iterator--) {
		DBG_LOG_CONT("%02x", scan_info_data->dev_addr.addr[iterator]);
	}
	
	DBG_LOG("RSSI");
	DBG_LOG_CONT(":%d",scan_info_data->rssi);
	
	if (scan_info_data->adv_data_len) {
		uint8_t adv_data_size;
		uint8_t index = 0;
		volatile adv_element_t *adv_element_p;
		volatile adv_element_t adv_element_data;

		adv_data_size = scan_info_data->adv_data_len;
		adv_element_data.len = 0;
		
		while (adv_data_size) {
			adv_element_data.len = scan_info_data->adv_data[index];
			adv_element_data.type
				= scan_info_data->adv_data[index + 1];
			adv_element_data.data
				= &scan_info_data->adv_data[index + 2];
			adv_element_p = &adv_element_data;

			/*Parsing the Advertisement Data */

			switch (adv_element_p->type) {
			/*Adv type: Flags*/
			case FLAGS:
			{
				ALL_UNUSED("%-28s", "Flags");
				ALL_UNUSED(":  ");
				if (LE_LIMITED_DISCOVERABLE_MODE &
						*(adv_element_p->data)) {
					ALL_UNUSED(
							"LE_LIMITED_DISCOVERABLE_MODE ");
				}

				if (LE_GENERAL_DISCOVERABLE_MODE &
						*(adv_element_p->data)) 
				{
					ALL_UNUSED(
							"LE_GENERAL_DISCOVERABLE_MODE ");
				}
		
				if (BREDR_NOT_SUPPORTED &
						*(adv_element_p->data)) {
					ALL_UNUSED("BREDR_NOT_SUPPORTED ");
				}

				if (LE_BREDR_CAPABLE_CONTROLLER &
						*(adv_element_p->data)) {
				}

				ALL_UNUSED("LE_BREDR_CAPABLE_CONTROLLER ");
				if (LE_BREDR_CAPABLE_HOST &
						*(adv_element_p->data)) {
					ALL_UNUSED("LE_BREDR_CAPABLE_HOST ");
				}
			}
			break;

			/*Adv type: Complete list of 16 bit service uuids*/
			case COMPLETE_LIST_16BIT_SERV_UUIDS:
			/*Adv type: Inomplete list of 16 bit service uuids*/
			case INCOMPLETE_LIST_16BIT_SERV_UUIDS:
			{
				uint16_t uuid_16;
				/* passing the length of data type */
				uint8_t adv_type_size = adv_element_p->len;
				/* actual size of the data */
				adv_type_size -= 1;
				if (COMPLETE_LIST_16BIT_SERV_UUIDS == adv_element_p->type)
				{
				ALL_UNUSED("%-28s",
						"Complete_16bit_service_uuids");
				}
				else
				{
					ALL_UNUSED("%-28s",
							"Incomplete_16bit_serv_uuids");
				}
				ALL_UNUSED(":  ");
				while (adv_type_size >= AT_BLE_UUID_16_LEN) {
					memcpy(&uuid_16, adv_element_p->data,
							AT_BLE_UUID_16_LEN);
					adv_element_p->data
						+= AT_BLE_UUID_16_LEN;
					adv_type_size -= AT_BLE_UUID_16_LEN;
					ALL_UNUSED("0x%02x ", uuid_16);
				}
			}
			break;

			/*Adv type: Complete list of 32 bit service uuids*/
			case COMPLETE_LIST_32BIT_SERV_UUIDS:
			{
				uint16_t uuid_32;
				/* passing the length of data type */
				uint8_t adv_type_size = adv_element_p->len;
				/* actual size of the data */
				adv_type_size -= 1;
				ALL_UNUSED("%-28s",
						"Complete_32bit_service_uuids");
				ALL_UNUSED(":  ");
				while (adv_type_size >= AT_BLE_UUID_32_LEN) {
					memcpy(&uuid_32, adv_element_p->data,
							AT_BLE_UUID_32_LEN);
					adv_element_p->data
						+= AT_BLE_UUID_32_LEN;
					adv_type_size -= AT_BLE_UUID_32_LEN;
					ALL_UNUSED(" 0x%02x", uuid_32);
				}
			}
			break;

			/*Adv type: Incomplete list of 32 bit service uuids*/
			case INCOMPLETE_LIST_32BIT_SERV_UUIDS:
			{
				uint16_t uuid_32;
				/* passing the length of data type */
				uint8_t adv_type_size = adv_element_p->len;
				/* actual size of the data */
				adv_type_size -= 1;
				ALL_UNUSED("%-28s",
						"Incomplete_32bit_serv_uuids");
				while (adv_type_size >= AT_BLE_UUID_32_LEN) {
					memcpy(&uuid_32, adv_element_p->data,
							AT_BLE_UUID_32_LEN);
					adv_element_p->data
						+= AT_BLE_UUID_32_LEN;
					adv_type_size -= AT_BLE_UUID_32_LEN;
					ALL_UNUSED(" 0x%02x", uuid_32);
				}
			}
			break;

			/*Adv type: Complete list of 128 bit service uuids*/
			case COMPLETE_LIST_128BIT_SERV_UUIDS:
			{
				uint8_t uuid_128[16];
				/* passing the length of data type */
				uint8_t adv_type_size = adv_element_p->len;
				/* actual size of the data */
				adv_type_size -= 1;
				ALL_UNUSED("%-28s",
						"Complete_128bit_service_uuid");
				ALL_UNUSED(":  ");
				while (adv_type_size >= AT_BLE_UUID_128_LEN) {
					memcpy(&uuid_128, adv_element_p->data,
							AT_BLE_UUID_128);
					adv_element_p->data
						+= AT_BLE_UUID_128_LEN;
					adv_type_size -= AT_BLE_UUID_128_LEN;
					for (iterator = 0; iterator < 16;
							iterator++) {
						ALL_UNUSED(" 0x%02x",
								uuid_128[
									iterator]);
					}
				}
			}
			break;

			/*Adv type: Incomplete list of 128 bit service uuids*/
			case INCOMPLETE_LIST_128BIT_SERV_UUIDS:
			{
				uint8_t uuid_128[16];
				/* passing the length of data type */
				uint8_t adv_type_size = adv_element_p->len;
				/* actual size of the data */
				adv_type_size -= 1;
				ALL_UNUSED("%-28s",
						"InComplete_128bit_serv_uuids");
				ALL_UNUSED(":  ");
				while (adv_type_size >= AT_BLE_UUID_128_LEN) {
					memcpy(&uuid_128, adv_element_p->data,
							AT_BLE_UUID_128);
					adv_element_p->data
						+= AT_BLE_UUID_128_LEN;
					adv_type_size -= AT_BLE_UUID_128_LEN;
					for (iterator = 0; iterator < 16;
							iterator++) {
						ALL_UNUSED(" 0x%02x",
								uuid_128[
									iterator]);
					}
				}
			}
			break;

			/*Adv type: shortened local name*/
			case SHORTENED_LOCAL_NAME:
			{
				/* passing the length of data type */
				int8_t adv_type_size = adv_element_p->len;
				/* actual size of the data */
				adv_type_size -= 1;
				ALL_UNUSED("%-28s", "Shortend Local Name");
				ALL_UNUSED(":  ");
				for (iterator = 0; iterator < adv_type_size;
						iterator++) {
					ALL_UNUSED("%c",
							adv_element_p->data[
								iterator]);
				}

				adv_element_p->data += adv_type_size;
				adv_type_size = 0;
			}
			break;

			/*Adv type: Complete local name*/
			case COMPLETE_LOCAL_NAME:
			{
				/* passing the length of data type */
				int8_t adv_type_size = adv_element_p->len;
				/* actual size of the data */
				adv_type_size -= 1;
				ALL_UNUSED("%-28s", "Complete Local Name");
				ALL_UNUSED(":  ");
				for (iterator = 0; iterator < adv_type_size;
						iterator++) {
					ALL_UNUSED("%c",
							adv_element_p->data[
								iterator]);
				}

				adv_element_p->data += adv_type_size;
				adv_type_size = 0;
			}
			break;

			/*Adv type: Manufacturer Specific data*/
			case MANUFACTURER_SPECIFIC_DATA:
			{
				/* passing the length of data type */
				int8_t adv_type_size = adv_element_p->len;
				/* actual size of the data */
				adv_type_size -= 1;
				ALL_UNUSED("%-28s",
						"Manufacturer Specific Data");
				ALL_UNUSED(":  ");
				for (iterator = 0; iterator < adv_type_size;
						iterator++) {
					ALL_UNUSED("0x%02x ",
							adv_element_p->data[
								iterator]);
				}

				adv_element_p->data += adv_type_size;
				adv_type_size = 0;
			}
			break;			

			/*Adv type: Appearance*/
			case APPEARANCE:
			{
				/* passing the length of data type */
				int8_t adv_type_size = adv_element_p->len;

				adv_type_size -= 1;
				/* actual size of the data */
				uint16_t appearance_t;
				ALL_UNUSED("%-28s", "Appearance");
				ALL_UNUSED(":  ");

				memcpy(&appearance_t, adv_element_p->data,
						APPEARANCE_SIZE);
				ALL_UNUSED("0x%02x", appearance_t);
				adv_element_p->data += APPEARANCE_SIZE;
				adv_type_size -= APPEARANCE_SIZE;
			}
			break;

			/*Adv type: Tx power level*/
			case TX_POWER_LEVEL:
			{
				/* passing the length of data type */
				int8_t adv_type_size = adv_element_p->len;
				adv_type_size -= 1;
				/* actual size of the data */
				uint16_t tx_power_level_t;
				ALL_UNUSED("%-28s", "Tx power level");
				ALL_UNUSED(":  ");
				memcpy(&tx_power_level_t, adv_element_p->data,
						TX_POWER_LEVEL_SIZE);
				ALL_UNUSED("0x%02x", tx_power_level_t);
				adv_element_p->data += TX_POWER_LEVEL_SIZE;
				adv_type_size -= TX_POWER_LEVEL_SIZE;
			}
			break;

			/*Adv type: Advertising Interval*/
			case ADVERTISING_INTERVAL:
			{
				/* passing the length of data type */
				int8_t adv_type_size = adv_element_p->len;
				adv_type_size -= 1;
				/* actual size of the data */
				uint16_t adv_interval_t;
				ALL_UNUSED("%-28s", "Advertisement Interval");
				ALL_UNUSED(":  ");

				memcpy(&adv_interval_t, adv_element_p->data,
						ADV_INTERVAL_SIZE);
				ALL_UNUSED("0x%02x", adv_interval_t);
				adv_element_p->data += ADV_INTERVAL_SIZE;
				adv_type_size -= ADV_INTERVAL_SIZE;
			}
			break;

			default:
				ALL_UNUSED("Unknown ad type");
				return AT_BLE_FAILURE;
				break;
			}
			index += (adv_element_data.len + 1);
			adv_element_data.len += 1;
			adv_data_size -= adv_element_data.len;
		}
	}
  
	return AT_BLE_SUCCESS;
}

/* Handler for AT_BLE_SCAN_REPORT event from stack */
at_ble_status_t ble_observer_scan_data_handler(void *param)
{
	DBG_LOG("Scan Complete. Total No.of device scanned:%lu", scan_device_count);
	ALL_UNUSED(param);
	return AT_BLE_SUCCESS;
}


int main(void )
{
    platform_driver_init();
	acquire_sleep_lock();
	
	/* Initialize serial console */
	serial_console_init();
	DBG_LOG("Initializing Ble_Observer Application");
	gpio_init();
	/* Initialize the button */
	button_init();
	button_register_callback(button_cb);

	/* initialize the ble chip  and Set the device mac address */
	ble_device_init(NULL);
	//app_init_done = true;
	DBG_LOG("Press button to start scanning..");
	/* Register callbacks for gap related events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_GAP_EVENT_TYPE,
									&observer_app_gap_cb);
									
	/* Register callbacks for custom related events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_CUSTOM_EVENT_TYPE,
									&ble_observer_custom_event_cb);
	
	/* Receiving events */
	while (1) {
		
		ble_event_task(0xFFFFFFFF);
		
	}
}
