/**
* \file
*
* \brief AltBeacon Application
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
 * \mainpage AltBeacon Application Example
 * \section Introduction
 * **************************** Introduction *********************************
 *	+ The AltBeacon example application bring-up the proximity beacon advertisement packet
 *    defined by the open and interoperable proximity beacon specification.
 *
 *	+ The Alert Notification example application supports the following Features:
 *	+ AltBeacon Advertisement
 *
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
 *  + 6. On a BLE compatible Android phone, enable Bluetooth in the Settings page. Start the 'Atmel Smart Connect' App and click on Beacon
 *       ranging icon. 
 *
 * \section Modules
 * ***************************** Modules **************************************
 *- BLE Platform Services -
 *  +  Interface Settings -
 *	  + Connect ATBTLC1000 XPRO to SAML21-XPRO-B -> EXT1
 *	  + Connect ATBTLC1000 XPRO to SAMD21-XPRO -> EXT1
 *	  + Connect ATBTLC1000 XPRO to SAMG55-XPRO -> EXT1
 *	  + Connect ATBTLC1000 XPRO to SAM4S-XPRO  -> EXT1
 *  +  Serial Console COM port settings -
 *	  + Baudrate 115200
 *	  + Parity None, Stop Bit 1, Start Bit 1
 *	  + No Hardware Handshake
 *	+  6-Wire Mode Connection Setup -
 *	  + Pins are 1:1 match with SAML21/D21 Xpro EXT1 Header to BTLC1000 XPro Header
 *	  + UART(No Flow Control)-SAM L21/D21 XPro Pins (Rx-Pin13, Tx-Pin14)
 *	  + UART(With Flow Control)-SAM G55 Xpro Pins (Rx-Pin13, Tx-Pin14, RTS-Pin5, CTS-Pin6, Rx-Pin16, Tx-Pin17)
 *	  + BTLC1000 Wakeup Pin-SAM G55 XPro Pins(Pin4)
 *	  + BTLC1000 Chip Enable Pin-SAM G55 XPro Pins(Pin10)
 *	  + BTLC1000 Vcc Pin-SAM L21/D21/G55 Xpro Pins(Pin20)
 *	  + BTLC1000 GND Pin-SAM L21/D21/G55 Xpro Pins(Pin19)
 *  +  4-Wire Mode Connection setup -
 *	  + UART(With Flow Control)-SAM L21/D21 XPro Pins (Rx-Pin15, Tx-Pin17, RTS-Pin16, CTS-Pin18)
 *	  + BTLC1000 Wakeup Pin-SAM L21/D21 XPro Pins (Rx-Pin6)
 *	  + BTLC1000 Chip Enable Pin-SAM L21/D21 XPro Pins (Rx-Pin4)
 *	  + UART(With Flow Control)-SAM G55/4S Xpro Pins (Rx-Pin13, Tx-Pin14, RTS-Pin5, CTS-Pin6)
 *	  + BTLC1000 Wakeup Pin-SAM G55/4S XPro Pins(Pin4)
 *	  + BTLC1000 Chip Enable Pin-SAM G55/4S XPro Pins(Pin10)
 *	  + BTLC1000 Vcc Pin-SAM L21/D21/G55/4S Xpro Pins(Pin20)
 *	  + BTLC1000 GND Pin-SAM L21/D21/G55/4S Xpro Pins(Pin19)
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

/****************************************************************************************
*							        Includes	                                     	*
****************************************************************************************/

#include <asf.h>
#include "platform.h"
#include "at_ble_api.h"
#include "console_serial.h"
#include "conf_extint.h"
#include "altbeacon.h"
#include "ble_utils.h"

#ifndef HOST_UART_BAUDRATE_CONFIG_VALUE
#define HOST_UART_BAUDRATE_CONFIG_VALUE 115200
#endif

at_ble_init_config_t pf_cfg;
uint32_t event_pool_memory[256] = {0};
uint32_t event_params_memory[1024] = {0};
uint32_t rx_fifo_memory[256] = {0};	

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

/**
 * @brief Initialize the BLE 
 */
static void ble_init(void)
{
	DBG_LOG("Initializing BTLC1000");
	
	/*Memory allocation required by GATT Server DB*/
	pf_cfg.memPool.memSize = 0;
	pf_cfg.memPool.memStartAdd = NULL;	
	pf_cfg.event_mem_pool.memStartAdd        = (uint8_t *)event_pool_memory;
	pf_cfg.event_mem_pool.memSize            = sizeof(event_pool_memory);
	pf_cfg.event_params_mem_pool.memStartAdd = (uint8_t *)event_params_memory;
	pf_cfg.event_params_mem_pool.memSize     = sizeof(event_params_memory);
	pf_cfg.rx_fifo_mem_pool.memStartAdd      = (uint8_t *)rx_fifo_memory;
	pf_cfg.rx_fifo_mem_pool.memSize			 = sizeof(rx_fifo_memory);	
	
/* BTLC1000 XPro Module ZR/MR */
	#if (BLE_MODULE == BTLC1000_ZR)
	pf_cfg.btlc1000_module_version			 = AT_BTLC1000_ZR;

	DBG_LOG("BTLC1000 XPro Module: BTLC1000-ZR");
	#if ((UART_FLOWCONTROL_6WIRE_MODE == false) && (UART_FLOWCONTROL_4WIRE_MODE == true))
	DBG_LOG("BTLC1000 Host Interface UART Mode:4-Wire, Baudrate:%d", (unsigned int)HOST_UART_BAUDRATE_CONFIG_VALUE);
	#else
	DBG_LOG("Error: Invalid BTLC1000 Host Interface UART Mode, BTLC1000-ZR supports only 4-WIRE UART mode");
	return;
	#endif
	#elif (BLE_MODULE == BTLC1000_MR)
	pf_cfg.btlc1000_module_version			 = AT_BTLC1000_MR;

	DBG_LOG("BTLC1000 XPro Module: BTLC1000-MR");
	#if ((UART_FLOWCONTROL_6WIRE_MODE == false) && (UART_FLOWCONTROL_4WIRE_MODE == true))
	DBG_LOG("BTLC1000 Host Interface UART Mode:4-Wire(works only when eFuse enabled), Baudrate:%d", \
	(unsigned int)HOST_UART_BAUDRATE_CONFIG_VALUE);
	#elif ((UART_FLOWCONTROL_6WIRE_MODE == true) && (UART_FLOWCONTROL_4WIRE_MODE == false))
	DBG_LOG("BTLC1000 Host Interface UART Mode:6-Wire(without Efuse Enabled), Baudrate:%d", \
	(unsigned int)HOST_UART_BAUDRATE_CONFIG_VALUE);
	#else
	DBG_LOG("Error: Invalid BTLC1000 Host Interface UART Mode, BTLC1000-MR supports only 4-Wire or 6-Wire UART mode");
	return;
	#endif
	#else
	DBG_LOG("Error: Select a Valid BTLC1000 XPro Module");
	return;
	#endif
		
	/*Bus configuration*/
	pf_cfg.bus_info.bus_type = AT_BLE_UART;
		
#if UART_FLOWCONTROL_6WIRE_MODE == true
	/* Enable Hardware Flow-control on BTLC1000 */
	pf_cfg.bus_info.btlc1000_uart_pinout_switch = true; // enable flow control
#else
	/* Disable Hardware Flow-control on BTLC1000 */
	pf_cfg.bus_info.btlc1000_uart_pinout_switch = false; // Disable flow control
#endif
    pf_cfg.bus_info.bus_baudrate = HOST_UART_BAUDRATE_CONFIG_VALUE; // 9600, 19200, 38400, 57600, 115200, 230400, 460800
	pf_cfg.bus_info.host_external_wakeup_gpio = BTLC1000_EXT_PIN9;	
	
	/* Register Platform callback API's */
	pf_cfg.platform_api_list.at_ble_create_timer = platform_create_timer;
	pf_cfg.platform_api_list.at_ble_delete_timer = platform_delete_timer;
	pf_cfg.platform_api_list.at_ble_start_timer	 = platform_start_timer;
	pf_cfg.platform_api_list.at_ble_stop_timer	 = platform_stop_timer;
	pf_cfg.platform_api_list.at_ble_sleep		 = platform_sleep;
	pf_cfg.platform_api_list.at_ble_gpio_set	 = platform_gpio_set;
	pf_cfg.platform_api_list.at_ble_send_sync	 = platform_send_sync;
	pf_cfg.platform_api_list.at_ble_recv_async   = platform_recv_async;
	pf_cfg.platform_api_list.at_ble_wait_for_signal = platform_wait_for_signal;
	pf_cfg.platform_api_list.at_ble_create_signal = platform_create_signal;
	pf_cfg.platform_api_list.at_ble_delete_signal = platform_delete_signal;
	pf_cfg.platform_api_list.at_ble_trigger_signal = platform_trigger_signal;
	pf_cfg.platform_api_list.at_ble_reset_signal = platform_reset_signal;
	
	pf_cfg.platform_api_list.at_ble_reconfigure_usart = pf_cfg.bus_info.btlc1000_uart_pinout_switch ? platform_configure_hw_fc_uart : platform_configure_primary_uart;
	
	platform_init(pf_cfg.bus_info.bus_type, pf_cfg.bus_info.btlc1000_uart_pinout_switch);
	
	/* Initialize BLE device */
	if(at_ble_init(&pf_cfg) != AT_BLE_SUCCESS)
	{
		DBG_LOG("BTLC1000 Initialization failed");
		DBG_LOG("Please check the power and connection / hardware connector");
		while(1);
	}

}

/**
 * @brief Initialize the alt beacon advertisement data and start advertisement
 */
static void altbeacon_init(void)
{
	uint8_t idx = 0;
	static uint8_t adv_data[ALT_BEACON_ADV_LENGTH + ALT_BEACON_ADV_TYPE_LENGTH];
	
	DBG_LOG("Initializing AltBeacon Application");
		
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
		DBG_LOG("BLE AltBeacon advertisement data set failed");
	}
	
	/* BLE start advertisement */
	if(at_ble_adv_start(AT_BLE_ADV_TYPE_NONCONN_UNDIRECTED, AT_BLE_ADV_GEN_DISCOVERABLE, NULL, AT_BLE_ADV_FP_ANY, 
	   AT_BLE_ADV_NONCON_INTERVAL_MIN, ALT_BEACON_ADV_TIMEOUT, ALT_BEACON_ABSOLUTE_INTERVAL_ADV) != AT_BLE_SUCCESS)
	{
		DBG_LOG("BLE AltBeacon advertisement failed");
		LED_Off(LED0);		
	}
	else
	{
		DBG_LOG("BLE AltBeacon Advertisement started");
		LED_On(LED0);
	}
}

int main (void)
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
	
	/* initialize the BLE chip */
	ble_init();
	
	/* set its own public address */
	ble_set_address(NULL);
	
	/* Initialize the Beacon advertisement */
	altbeacon_init();
	
	while(1){ 		
 	}
}
