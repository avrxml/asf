/**
 * \file
 *
 * \brief AltBeacon Application
 * 				The AltBeacon example application bring-up the proximity beacon 
 *				advertisement packet defined by the open and interoperable proximity
 *				beacon specification. 
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
/**
 * \mainpage AltBeacon Application Example
 * \section Introduction
 * **************************** Introduction *********************************
 *	+ The AltBeacon example application bring-up the proximity beacon advertisement packet
 *    defined by the open and interoperable proximity beacon specification.
 *
 *	+ The Alert Notification example application supports the following Features:
 *	+ AltBeacon Advertisement
 *
 *- Running the Demo -
 *  + 1. Build and flash the binary into supported evaluation board.
 *  + 2. Open the console using TeraTerm or any serial port monitor.
 *  + 3. Press the Reset button.
 *  + 4. Wait for around 10 seconds for the patches to be downloaded device will initialize and start-up.
 *  + 5. The device is now in advertising mode.
 *  + 6. On a BLE compatible Android phone, enable Bluetooth in the Settings page. Start the 'Atmel Smart Connect' App and click on Beacon
 *       ranging icon.
 *- BLE Platform Services -
 *  +  Serial Console COM port settings -
 *	  + Baudrate 115200
 *	  + Parity None, Stop Bit 1, Start Bit 1
 *	  + No Hardware Handshake
 *
 *\section BLE SDK Package
 * ***************************** BLE SDK Package ******************************************
 *- Links for SAMB11-
 *		+ http://www.microchip.com/wwwproducts/en/ATSAMB11
 *		+ http://www.microchip.com/developmenttools/productdetails.aspx?partno=atsamb11-xpro
 *- Support and FAQ - visit -
 *		+ <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
/****************************************************************************************
*							        Header Inclusions						                											*
****************************************************************************************/
#include <asf.h>
#include "platform.h"
#include "at_ble_api.h"
#include "console_serial.h"
#include "altbeacon.h"
#include "ble_utils.h"


at_ble_init_config_t pf_cfg;
uint32_t event_pool_memory[256] = {0};
uint32_t event_params_memory[1024] = {0};
static uint8_t params[524];

struct uart_module uart_instance;

uint32_t ble_sdk_version(void);

static void configure_uart(void)
{
	struct uart_config config_uart;

	system_clock_config(CLOCK_RESOURCE_XO_26_MHZ, CLOCK_FREQ_26_MHZ);

	uart_get_config_defaults(&config_uart);

	config_uart.baud_rate = 115200;
	
	config_uart.pin_number_pad[0] = EDBG_CDC_PIN_PAD0;
	config_uart.pin_number_pad[1] = EDBG_CDC_PIN_PAD1;
	config_uart.pin_number_pad[2] = EDBG_CDC_PIN_PAD2;
	config_uart.pin_number_pad[3] = EDBG_CDC_PIN_PAD3;
	
	config_uart.pinmux_sel_pad[0] = EDBG_CDC_MUX_PAD0;
	config_uart.pinmux_sel_pad[1] = EDBG_CDC_MUX_PAD1;
	config_uart.pinmux_sel_pad[2] = EDBG_CDC_MUX_PAD2;
	config_uart.pinmux_sel_pad[3] = EDBG_CDC_MUX_PAD3;

	stdio_serial_init(&uart_instance, CONF_STDIO_USART_MODULE, &config_uart);
}
/***********************************************************************************
 *									Implementations                               *
 **********************************************************************************/
/**
 * @brief Set BLE Address, If address is NULL then it will use BD public address 
 */
static void ble_set_address(at_ble_addr_t *addr)
{
	at_ble_addr_t address = {AT_BLE_ADDRESS_PUBLIC, {0xAB, 0xCD, 0xEF, 0xAB, 0xCD, 0xEF}};
	at_ble_addr_t *address_ptr = addr;
	
	if (addr == NULL)
	{
		/* get BD address from BLE device */
		if(at_ble_addr_get(&address) != AT_BLE_SUCCESS)
		{
			DBG_LOG("BD address get failed");
		}
		address_ptr = &address;
	}
	
	/* set the BD address */
	if(at_ble_addr_set(address_ptr) != AT_BLE_SUCCESS)
	{
		DBG_LOG("BD address set failed");
	}
	
	DBG_LOG("BD Address:0x%02X%02X%02X%02X%02X%02X, Address Type:%d",
	address_ptr->addr[5],
	address_ptr->addr[4],
	address_ptr->addr[3],
	address_ptr->addr[2],
	address_ptr->addr[1],
	address_ptr->addr[0], address_ptr->type);
}

uint32_t ble_sdk_version(void)
{
	uint32_t fw_ver, rf_ver;
	if(at_ble_firmware_version_get(&fw_ver) == AT_BLE_SUCCESS)
	{
		/* Check the SDK and Library version compatibility */
		if ( (BLE_SDK_MAJOR_NO(fw_ver) == BLE_SDK_MAJOR_NO(BLE_SDK_VERSION)) && \
		(BLE_SDK_MINOR_NO(fw_ver) == BLE_SDK_MINOR_NO(BLE_SDK_VERSION)) )
		{
			DBG_LOG("BluSmartSDK Firmware Version:%X.%X.%X", (uint8_t)BLE_SDK_MAJOR_NO(fw_ver), \
			((uint8_t)BLE_SDK_MINOR_NO(fw_ver) + BLE_SDK_MINOR_NO_INC), (uint16_t)BLE_SDK_BUILD_NO(fw_ver));
		}
		else
		{
			DBG_LOG("Error:Library version doesn't match with SDK version. Please use %X.%X version of library", \
			BLE_SDK_MAJOR_NO(BLE_SDK_VERSION), BLE_SDK_MINOR_NO(BLE_SDK_VERSION));
			fw_ver = 0;
		}
		
		if(at_ble_rf_version_get(&rf_ver) == AT_BLE_SUCCESS)
		{
			DBG_LOG("SAMB11 RF Version:0x%8X", (unsigned int)rf_ver);
		}
		else
		{
			DBG_LOG("Error: Failed to get SAMB11 RF Version");
			rf_ver = 0;
		}
	}
	else
	{
		DBG_LOG("Error: Failed to get BluSmartSDK Firmware Version");
		fw_ver = 0;
	}

	return fw_ver;
}

static void resume_cb(void)
{
	init_port_list();
	configure_uart();
	spi_flash_turn_off();//for power consumption.
}

/**
 * @brief Initialize the BLE 
 */
static void ble_init(void)
{
	DBG_LOG("Initializing SAMB11 AltBeacon Application");
	
	uint32_t chip_id = 0xFFFFFFFF;
	/*Memory allocation required by GATT Server DB*/
	memset(&pf_cfg,0,sizeof(pf_cfg));
	memset((void *)event_pool_memory,0,sizeof(event_pool_memory));
	memset((void *)event_params_memory,0,sizeof(event_params_memory));

	pf_cfg.memPool.memSize = 0;
	pf_cfg.memPool.memStartAdd = NULL;
	pf_cfg.event_mem_pool.memStartAdd        = (uint8_t *)event_pool_memory;
	pf_cfg.event_mem_pool.memSize            = sizeof(event_pool_memory);
	pf_cfg.event_params_mem_pool.memStartAdd = (uint8_t *)event_params_memory;
	pf_cfg.event_params_mem_pool.memSize     = sizeof(event_params_memory);

	#if (BLE_MODULE == SAMB11_ZR)
		pf_cfg.samb11_module_version			 = AT_SAMB11_ZR;
		DBG_LOG("SAMB11 XPro Module: SAMB11-ZR");
	#elif (BLE_MODULE == SAMB11_MR)
		pf_cfg.samb11_module_version			 = AT_SAMB11_MR;
		DBG_LOG("SAMB11 XPro Module:MR SAMB11-MR");
	#else
		DBG_LOG("Error: Select a Valid SAMB11 XPro Module");
		return;
	#endif
	
	/* Initialize BLE device */
	if(at_ble_init(&pf_cfg) != AT_BLE_SUCCESS)
	{
		DBG_LOG("SAMB11 Initialization failed");
		DBG_LOG("Please check the power and connection / hardware connector");
		while(1);
	}

	if (at_ble_chip_id_get(&chip_id) == AT_BLE_SUCCESS)
	{
		DBG_LOG("SAMB11 Chip ID: 0x%6X", (unsigned int)chip_id);
	}
	else
	{
		DBG_LOG("SAMB11 Chip identification failed");
		while(1);
	}

	/* set its own public address */
	ble_set_address(NULL);

	ble_sdk_version();

}

/**
 * @brief Initialize the alt beacon advertisement data and start advertisement
 */
static void altbeacon_init(void)
{
	uint8_t idx = 0;
	static uint8_t adv_data[ALT_BEACON_ADV_LENGTH + ALT_BEACON_ADV_TYPE_LENGTH];
		
	/* Adding advertisement data length and advertisement 
	data type to advertisement data */
	adv_data[idx++] = ALT_BEACON_ADV_LENGTH;
	adv_data[idx++] = MANUFACTURER_SPECIFIC_TYPE;
	
	/* Adding manufacturer specific data(manufacture identifier code, beacon code, 
	16 byte UUID, Major Number and Minor Number) to advertisement data */
	memcpy(&adv_data[idx], MANUFACTURER_ID, MANUFACTURER_ID_LENGTH);
	idx += MANUFACTURER_ID_LENGTH;
	
	memcpy(&adv_data[idx], ALT_BEACON_CODE, ALT_BEACON_CODE_LENGTH);
	idx += ALT_BEACON_CODE_LENGTH;
	
	memcpy(&adv_data[idx], ALT_BEACON_16BYTE_UUID, ALT_BEACON_16BYTE_UUID_LENGTH);
	idx += ALT_BEACON_16BYTE_UUID_LENGTH;
	
	memcpy(&adv_data[idx], ALT_BEACON_MAJOR_NUM, ALT_BEACON_2BYTE_LEN);
	idx += ALT_BEACON_2BYTE_LEN;
	
	memcpy(&adv_data[idx], ALT_BEACON_MINOR_NUM, ALT_BEACON_2BYTE_LEN);
	idx += ALT_BEACON_2BYTE_LEN;
	
	adv_data[idx++] = REF_RSSI;
	adv_data[idx++] = MFG_RSVD;
	
	/* set alt beacon advertisement data */
	if(at_ble_adv_data_set(adv_data, idx, NULL, ALT_BEACON_SCAN_RESPONSE_LENGTH) != AT_BLE_SUCCESS)
	{
		DBG_LOG("AltBeacon advertisement data set failed");
	}
	
	/* BLE start advertisement */
	if(at_ble_adv_start(AT_BLE_ADV_TYPE_NONCONN_UNDIRECTED, AT_BLE_ADV_GEN_DISCOVERABLE, NULL, AT_BLE_ADV_FP_ANY, 
	   AT_BLE_ADV_NONCON_INTERVAL_MIN, ALT_BEACON_ADV_TIMEOUT, ALT_BEACON_ABSOLUTE_INTERVAL_ADV) != AT_BLE_SUCCESS)
	{
		DBG_LOG("AltBeacon advertisement failed");
	}
	else
	{
		DBG_LOG("AltBeacon Advertisement started");		
	}
}

int main(void)
{
	at_ble_events_t event;
	platform_driver_init();
	acquire_sleep_lock();

	configure_uart();		

	/* initialize the BLE chip */
	ble_init();
	
	/* Initialize the Beacon advertisement */
	altbeacon_init();

	memset(params, 0, sizeof(params));
	register_resume_callback(resume_cb);

	release_sleep_lock();
	
	while(1)
	{
		at_ble_event_get(&event, params, (uint32_t)(-1));
	}
}