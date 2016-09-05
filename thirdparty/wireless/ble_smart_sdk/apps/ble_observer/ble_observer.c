/**
 * \file
 *
 * \brief BLE Observer application
 *
 * Copyright (c) 2014-2016 Atmel Corporation. All rights reserved.
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
 *    Atmel micro controller product.
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

extern uint8_t scan_response_count;

/*Initialization function for Ble Observer */

static void ble_observer_init(void)
{
	at_ble_status_t scan_status;
	
	if(at_ble_scan_stop() != AT_BLE_SUCCESS)
	{
		/* If scan not started stop scan failed message will be displayed on terminal */
		/* If the scan already started then stop scan will return AT_BLE_SUCCESS */
		DBG_LOG_DEV("Stop scan failed");
	}

	/* Initialize the scanning procedure */
	scan_status = gap_dev_scan();

	/* Check for scan status */
	if (scan_status == AT_BLE_SUCCESS) {
		DBG_LOG("Scanning process initiated");
	} else if (scan_status == AT_BLE_INVALID_PARAM) {
		DBG_LOG("Scan parameters are invalid");
	} else if (scan_status == AT_BLE_FAILURE) {
		DBG_LOG("Scanning Failed Generic error");
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
				uint16_t temp_uuid16;
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
				while (adv_type_size) {
					memcpy(&temp_uuid16, adv_element_p->data,
							AT_BLE_UUID_16_LEN);
					uuid_16 = ((temp_uuid16 << 8) | (temp_uuid16 >> 8));
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
				while (adv_type_size) {
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
						"Incomplete_16bit_serv_uuids");
				while (adv_type_size) {
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
				while (adv_type_size) {
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
				while (adv_type_size) {
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
				DBG_LOG("Unknown ad type");
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
	DBG_LOG("Scan Complete. Total No.of device scanned:%d", scan_response_count);
	ble_observer_init();
	ALL_UNUSED(param);
	return AT_BLE_SUCCESS;
}

static const ble_event_callback_t observer_app_gap_cb[] = {
	NULL,
	ble_observer_scan_info_handler,
	ble_observer_scan_data_handler,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};


int main(void )
{
	platform_driver_init();
	acquire_sleep_lock();

	/* Initialize serial console */
	serial_console_init();

	/* initialize the ble chip  and Set the device mac address */
	ble_device_init(NULL);
	
	/* observer init */
	ble_observer_init();
	
	/* Register callbacks for gap related events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
									BLE_GAP_EVENT_TYPE,
									observer_app_gap_cb);
	
	/* Receiving events */
	while (1) {
		ble_event_task(BLE_EVENT_TIMEOUT);
	}
}
