/**
 * \file
 *
 * \brief BLE Observer application
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
 *- Supported Evolution Kit -
 *	+ ATSAML21-XPRO-B + ATBTLC1000 XPRO
 *	+ ATSAMD21-XPRO + ATBTLC1000 XPRO
 *	+ ATSAMG55-XPRO + ATBTLC1000 XPRO
 *	+ ATSAM4S-XPRO + ATBTLC1000 XPRO
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
 *		+ http://www.microchip.com/wwwproducts/en/ATBTLC1000?tab=documents
 *- Links for ATBTLC1000 -
 *		+ http://www.microchip.com/wwwproducts/en/ATBTLC1000
 *- Development Kit -
 *		+ http://www.microchip.com/wwwproducts/en/ATBTLC1000?tab=tools
 *- SAM L21 + BTLC1000 XPro -
 *		+ http://www.microchip.com/developmenttools/productdetails/atbtlc1000-xstk
 *- BTLC1000 XPro -
 *		+ http://www.microchip.com/developmenttools/productdetails/atbtlc1000-xpro
 *- Applications -
 *		+ http://www.microchip.com/wwwproducts/en/ATBTLC1000?tab=applications
 *- Support and FAQ - visit 
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
#include "timer_hw.h"
#include "at_ble_api.h"
#include "conf_extint.h"
#include "console_serial.h"
#include "ble_observer.h"
#include "ble_utils.h"
#include "ble_manager.h"

/** @brief Button event ID */
#define APP_BUTTON_EVENT_ID		(1)

extern uint32_t scan_response_count;
uint8_t scan_device_count;

volatile uint8_t scanning_state = false;

/* Flag to avoid spurious interrupt posting  during/before reset and initialization completed */
volatile bool app_init_done = false;
volatile bool button_press = false;

user_custom_event_t app_button_event =
{
	.id = APP_BUTTON_EVENT_ID,
	.bptr = NULL
};

/* All BLE Manager Custom Event callback */
static const ble_custom_event_cb_t ble_observer_custom_event_cb = {
	.custom_event = ble_observer_custom_event
};

static at_ble_status_t ble_observer_custom_event(void *param)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
	uint8_t scan_status = AT_BLE_FAILURE;
	user_custom_event_t **app_custom_event = (user_custom_event_t **)param;
	
	if ((((*app_custom_event)->id) == APP_BUTTON_EVENT_ID))
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
		delay_ms(300); //Button de-bounce delay
	}
	else
	{
		status = AT_BLE_FAILURE;
	}
	button_press = false;
	return status;
}

void button_cb(void)
{
	if(app_init_done && !button_press)
	{
		button_press = true;
		at_ble_event_user_defined_post(&app_button_event);
	}
}

/* Handler for Scan info data */
at_ble_status_t ble_observer_scan_info_handler(void * param)
{
	int8_t iterator;
	at_ble_scan_info_t *scan_info_data = (at_ble_scan_info_t *)param;

	DBG_LOG("%-28s", "\r\n\r\nAdvertisement type");
	DBG_LOG_CONT("    :  ");
	switch (scan_info_data->type) {
	case AT_BLE_ADV_TYPE_UNDIRECTED:
		DBG_LOG_CONT("ADV_IND");
		break;

	case AT_BLE_ADV_TYPE_DIRECTED:
		DBG_LOG_CONT("ADV_DIRECT_IND");
		break;

	case AT_BLE_ADV_TYPE_SCANNABLE_UNDIRECTED:
		DBG_LOG_CONT("ADV_SCANNABLE_UNDIRECTED");
		break;

	case AT_BLE_ADV_TYPE_NONCONN_UNDIRECTED:
		DBG_LOG_CONT("ADV_NONCONN_UNDIRECTED");
		break;

	case AT_BLE_ADV_TYPE_SCAN_RESPONSE:
		DBG_LOG_CONT("ADV_SCAN_RESPONSE");
		break;

	default:
		DBG_LOG_CONT("\r\n Unknown");
		break;
	}
	
	if(scan_info_data->type != AT_BLE_ADV_TYPE_SCAN_RESPONSE)
	{
		scan_device_count++;
	}

	/*Device Adress Type */
	DBG_LOG("%-28s", "Device address type");
	DBG_LOG_CONT(":  ");
	switch (scan_info_data->dev_addr.type) {
	case AT_BLE_ADDRESS_PUBLIC:
		DBG_LOG_CONT("PUBLIC_ADDRESS");
		break;

	case AT_BLE_ADDRESS_RANDOM_STATIC:
		DBG_LOG_CONT("STATIC_RANDOM_ADDRESS");
		break;

	case AT_BLE_ADDRESS_RANDOM_PRIVATE_RESOLVABLE:
		DBG_LOG_CONT("RANDOM_RESOLVABLE_PRIVATE_ADDRESS");
		break;

	case AT_BLE_ADDRESS_RANDOM_PRIVATE_NON_RESOLVABLE:
		DBG_LOG_CONT("RANDOM_NON_RESOLVABLE_PRIVATE_ADDRESS");
		break;

	default:
		DBG_LOG_CONT(" Unknown");
	}

	/*Device Address */
	DBG_LOG("%-28s", "Device address");
	DBG_LOG_CONT(":  0x");
	for (iterator = 5; iterator >= 0; iterator--) {
		DBG_LOG_CONT("%02x", scan_info_data->dev_addr.addr[iterator]);
	}
	
	DBG_LOG("%-28s", "RSSI");
	DBG_LOG_CONT(":  %d",scan_info_data->rssi);
	
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
				DBG_LOG("%-28s", "Flags");
				DBG_LOG_CONT(":  ");
				if (LE_LIMITED_DISCOVERABLE_MODE &
						*(adv_element_p->data)) {
					DBG_LOG_CONT(
							"LE_LIMITED_DISCOVERABLE_MODE ");
				}

				if (LE_GENERAL_DISCOVERABLE_MODE &
						*(adv_element_p->data)) 
				{
					DBG_LOG_CONT(
							"LE_GENERAL_DISCOVERABLE_MODE ");
				}
		
				if (BREDR_NOT_SUPPORTED &
						*(adv_element_p->data)) {
					DBG_LOG_CONT("BREDR_NOT_SUPPORTED ");
				}

				if (LE_BREDR_CAPABLE_CONTROLLER &
						*(adv_element_p->data)) {
				}

				DBG_LOG_CONT("LE_BREDR_CAPABLE_CONTROLLER ");
				if (LE_BREDR_CAPABLE_HOST &
						*(adv_element_p->data)) {
					DBG_LOG_CONT("LE_BREDR_CAPABLE_HOST ");
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
				DBG_LOG("%-28s",
						"Complete_16bit_service_uuids");
				}
				else
				{
					DBG_LOG("%-28s",
							"Incomplete_16bit_serv_uuids");
				}
				DBG_LOG_CONT(":  ");
				while (adv_type_size >= AT_BLE_UUID_16_LEN) {
					memcpy(&uuid_16, adv_element_p->data,
							AT_BLE_UUID_16_LEN);
					adv_element_p->data
						+= AT_BLE_UUID_16_LEN;
					adv_type_size -= AT_BLE_UUID_16_LEN;
					DBG_LOG_CONT("0x%02x ", uuid_16);
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
				DBG_LOG("%-28s",
						"Complete_32bit_service_uuids");
				DBG_LOG_CONT(":  ");
				while (adv_type_size >= AT_BLE_UUID_32_LEN) {
					memcpy(&uuid_32, adv_element_p->data,
							AT_BLE_UUID_32_LEN);
					adv_element_p->data
						+= AT_BLE_UUID_32_LEN;
					adv_type_size -= AT_BLE_UUID_32_LEN;
					DBG_LOG_CONT(" 0x%02x", uuid_32);
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
				DBG_LOG("%-28s",
						"Incomplete_32bit_serv_uuids");
				while (adv_type_size >= AT_BLE_UUID_32_LEN) {
					memcpy(&uuid_32, adv_element_p->data,
							AT_BLE_UUID_32_LEN);
					adv_element_p->data
						+= AT_BLE_UUID_32_LEN;
					adv_type_size -= AT_BLE_UUID_32_LEN;
					DBG_LOG_CONT(" 0x%02x", uuid_32);
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
				DBG_LOG("%-28s",
						"Complete_128bit_service_uuid");
				DBG_LOG_CONT(":  ");
				while (adv_type_size >= AT_BLE_UUID_128_LEN) {
					memcpy(&uuid_128, adv_element_p->data,
							AT_BLE_UUID_128);
					adv_element_p->data
						+= AT_BLE_UUID_128_LEN;
					adv_type_size -= AT_BLE_UUID_128_LEN;
					for (iterator = 0; iterator < 16;
							iterator++) {
						DBG_LOG_CONT(" 0x%02x",
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
				DBG_LOG("%-28s",
						"InComplete_128bit_serv_uuids");
				DBG_LOG_CONT(":  ");
				while (adv_type_size >= AT_BLE_UUID_128_LEN) {
					memcpy(&uuid_128, adv_element_p->data,
							AT_BLE_UUID_128);
					adv_element_p->data
						+= AT_BLE_UUID_128_LEN;
					adv_type_size -= AT_BLE_UUID_128_LEN;
					for (iterator = 0; iterator < 16;
							iterator++) {
						DBG_LOG_CONT(" 0x%02x",
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
				DBG_LOG("%-28s", "Shortend Local Name");
				DBG_LOG_CONT(":  ");
				for (iterator = 0; iterator < adv_type_size;
						iterator++) {
					DBG_LOG_CONT("%c",
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
				DBG_LOG("%-28s", "Complete Local Name");
				DBG_LOG_CONT(":  ");
				for (iterator = 0; iterator < adv_type_size;
						iterator++) {
					DBG_LOG_CONT("%c",
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
				DBG_LOG("%-28s",
						"Manufacturer Specific Data");
				DBG_LOG_CONT(":  ");
				for (iterator = 0; iterator < adv_type_size;
						iterator++) {
					DBG_LOG_CONT("0x%02x ",
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
				DBG_LOG("%-28s", "Appearance");
				DBG_LOG_CONT(":  ");

				memcpy(&appearance_t, adv_element_p->data,
						APPEARANCE_SIZE);
				DBG_LOG_CONT("0x%02x", appearance_t);
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
				DBG_LOG("%-28s", "Tx power level");
				DBG_LOG_CONT(":  ");
				memcpy(&tx_power_level_t, adv_element_p->data,
						TX_POWER_LEVEL_SIZE);
				DBG_LOG_CONT("0x%02x", tx_power_level_t);
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
				DBG_LOG("%-28s", "Advertisement Interval");
				DBG_LOG_CONT(":  ");

				memcpy(&adv_interval_t, adv_element_p->data,
						ADV_INTERVAL_SIZE);
				DBG_LOG_CONT("0x%02x", adv_interval_t);
				adv_element_p->data += ADV_INTERVAL_SIZE;
				adv_type_size -= ADV_INTERVAL_SIZE;
			}
			break;

			default:
				DBG_LOG_DEV("Unknown ad type");
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
	DBG_LOG("Scan Complete. Total No.of device scanned:%d", scan_device_count);
	ALL_UNUSED(param);
	return AT_BLE_SUCCESS;
}

static const ble_gap_event_cb_t observer_app_gap_cb = {
	.scan_info = ble_observer_scan_info_handler,
	.scan_report = ble_observer_scan_data_handler
};


int main(void )
{
	#if SAMG55 || SAM4S
	/* Initialize the SAM system. */
	sysclk_init();
	board_init();
	#elif SAM0
	system_init();
	#endif
	
	/* Initialize the button */
	button_init();
	
	/* Initialize serial console */
	serial_console_init();

	/* initialize the ble chip  and Set the device mac address */
	ble_device_init(NULL);
	
	app_init_done = true;
	
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
		ble_event_task();
	}
}
