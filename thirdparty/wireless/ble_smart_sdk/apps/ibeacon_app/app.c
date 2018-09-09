/**
 * \file
 *
 * \brief iBeacon Application
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
 * \mainpage
 * \section preface Preface
 * This is the reference manual for the iBeacon Application
 */
/*- Includes ---------------------------------------------------------------*/
#include "at_ble_api.h"
#include <asf.h>
#include <string.h>
#include <conf_console_serial.h>
#include "platform.h"
#include <common.h>
#include <spi_flash.h>
#include "ibeacon.h"
#include "ble_utils.h"

at_ble_init_config_t pf_cfg;
static uint32_t event_pool_memory[256] = {0};
static uint32_t event_params_memory[1024] = {0};
static uint8_t params[524];
struct uart_module uart_instance;

/* Initialize the BLE */
static void ble_init(void);

/* Set BLE Address, If address is NULL then it will use BD public address */
static void ble_set_address(at_ble_addr_t *addr);

/* Beacon initialization */
static void beacon_init(void);

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

static void resume_cb(void)
{
	init_port_list();
	configure_uart();
	spi_flash_turn_off();
}

/* Set BLE Address, If address is NULL then it will use BD public address */
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
	
/* Initialize the BLE */
static void ble_init(void)
{
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

	/* Init BLE device */
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

/* Define service, advertisement data set and start advertisement */
static void beacon_init(void)
{
	uint8_t idx = 0;
	static uint8_t adv_data[BEACON_ADV_LENGTH + BEACON_ADV_TYPE_LENGTH];
	
	/* Adding advertisement data length and advertisement 
	data type to advertisement data */
	adv_data[idx++] = BEACON_ADV_LENGTH;
	adv_data[idx++] = BEACON_ADV_TYPE;
	
	/* Adding manufacturer specific data(company identifier code and 
	additional manufacturer specific data) to advertisement data */
	memcpy(&adv_data[idx], COMPANY_IDENTIFIER_CODE, COMPANY_IDENTIFIER_CODE_LENGTH);
	idx += COMPANY_IDENTIFIER_CODE_LENGTH;
	
	memcpy(&adv_data[idx], ADDTIONAL_MANUFACTURER_SPECIFIC_DATA, ADDTIONAL_MANUFACTURER_SPECIFIC_DATA_LENGTH);
	idx += ADDTIONAL_MANUFACTURER_SPECIFIC_DATA_LENGTH;

	/* set beacon advertisement data */
	if(at_ble_adv_data_set(adv_data, idx, NULL, BEACON_SCAN_RESPONSE_LENGTH) != AT_BLE_SUCCESS)
	{
		DBG_LOG("iBeacon advertisement data set failed");
	}
	
	/* BLE start advertisement */
	if(at_ble_adv_start(AT_BLE_ADV_TYPE_NONCONN_UNDIRECTED, AT_BLE_ADV_GEN_DISCOVERABLE, NULL, AT_BLE_ADV_FP_ANY, 
	   BEACON_ADV_INTERVAL, BEACON_ADV_TIMEOUT, BEACON_ABSOLUTE_INTERVAL_ADV) != AT_BLE_SUCCESS)
	{
		DBG_LOG("iBeacon advertisement failed");
	}
	else
	{
		DBG_LOG("iBeacon Advertisement started");		
	}
}

int main(void)
{
	at_ble_events_t event;
	platform_driver_init();	
	acquire_sleep_lock();	
	configure_uart();
		
	DBG_LOG("Initializing iBeacon SAMB11 Application");
	
	/* initialize the BLE chip */
	ble_init();	
	
	/* Initialize the Beacon advertisement */
	beacon_init();

	memset(params, 0, sizeof(params));
	register_resume_callback(resume_cb);

	release_sleep_lock();
	
	while(1)
	{
		at_ble_event_get(&event, params, (uint32_t)(-1));
	}
}
