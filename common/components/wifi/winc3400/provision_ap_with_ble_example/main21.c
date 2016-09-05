/**
 * \file
 *
 * \brief Provision AP with BLE example.
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
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

/** \mainpage
 * \section intro Introduction
 * This example demonstrates how to provision Wi-Fi via BLE using the WINC3400
 * Wi-Fi/BLE module. <br>
 * It uses the following hardware:
 * - SAM Xplained Pro.
 * - the WIN3400 module on EXT1.
 * - thirdparty Android device.
 *
 * \section files Main Files
 * - main21.c : Initialize the SAM board and perform BLE provisioning.
 *
 * \section usage Usage
 * -# To connect the SAM board to the internet Access Point over Wi-Fi, the user must
 * -# provide credentials (SSID and passphrase) over BLE. As a first step, it is required
 * -# that the user install the Atmel BLE provision application available in the doc folder
 * -# (Atmel_BLE_prov.apk) on an Android device.
 * -# Then, power up the SAM board and run the Android application: perform a scan, select
 * -# an SSID and enter the passphrase.
 * -# Finally, press connect and enter the PIN code defined in main.h file.
 * -# If connection to AP is successful, WINC3400 BLE will start beaconing.
 * -# Tracking the beacon can be easily done using the Atmel_Beacon_Radar.apk Android
 * -# application also provided in the doc folder.
 *
 * -# Build the program and download it into the board.
 * -# On the computer, open and configure a terminal application as the follows.
 * \code
 *    Baud Rate : 115200
 *    Data : 8bit
 *    Parity bit : none
 *    Stop bit : 1bit
 *    Flow control : none
 * \endcode
 * -# Start the application.
 * -# In the terminal window, the following text should appear:
 * \code
 *    -- WINC3400 Provision AP with BLE example --
 *    -- SAMXXX_XPLAINED_PRO --
 *    -- Compiled: xxx  x xxxx xx:xx:xx --
 *    
 *    main: started WiFi.
 *    (APP)(INFO)Chip ID 3000d0
 *    (APP)(INFO)Firmware ver   : x.x.x
 *    (APP)(INFO)Min driver ver : x.x.x
 *    (APP)(INFO)Curr driver ver: x.x.x
 *    (APP)(INFO)M2M_NO_PS
 *    (APP)(INFO)POWER SAVE 0
 *    main: started BLE.
 *    main: started BLE provisioning.
 *    AT_BLE_WIFIPROV_SCAN_MODE_CHANGE_IND
 *    wifi_cb: M2M_WIFI_RESP_SCAN_DONE
 *    wifi_cb: M2M_WIFI_RESP_SCAN_RESULT: found SSID "XXXXXX"
 *    ...
 *    Please open Android application to provision Wi-Fi over BLE...
 *
 *    AT_BLE_WIFIPROV_COMPLETE_IND
 *    Provisioning Successful
 *    Sec type   : 2
 *    SSID       : XXXXXXXX
 *    SSID Lenght: 8
 *    Passphrase : XXXXXXXX
 *    (APP)(INFO)Connecting..
 *    wifi_cb: M2M_WIFI_RESP_CON_STATE_CHANGED: CONNECTED
 *    wifi_cb: M2M_WIFI_REQ_DHCP_CONF: IP is 192.168.5.124
 *    beacon_init: started
 *    beacon_init: primary service defined.
 *    beacon_init: advertisement data set.
 *    beacon_init: beacon advertisement started.
 *    beacon_init: done.
 * \endcode
 *
 * \section compinfo Compilation Information
 * This software was written for the GNU GCC compiler using Atmel Studio 6.2
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com">Atmel</A>.\n
 */

#include "driver/include/m2m_wifi.h"
#include "driver/include/m2m_periph.h"
#include "socket/include/socket.h"
#include "stdio_serial.h"
#include "ble/platform/include/platform.h"
#include "ble/include/at_ble_api.h"
#include "ble/ble_services/ble_mgr/ble_manager.h"
#include "ble/utils/ble_utils.h"
#include "ble/ble_stack/include/wifiprov_task.h"
#include "main.h"

#define STRING_EOL    "\r\n"
#define STRING_HEADER "-- WINC3400 Provision AP with BLE example --"STRING_EOL \
	"-- "BOARD_NAME " --"STRING_EOL	\
	"-- Compiled: "__DATE__ " "__TIME__ " --"STRING_EOL

/** Beacon identifier value. */
#define BEACON_IDENTIFIER             (0x46)

/** Beacon advertisement interval (in 0.625ms). */
#define BEACON_ADV_INTERVAL           (100)

/** Beacon advertisement timeout (in 0.625ms). */
#define BEACON_ADV_TIMEOUT            (1000)

/** Beacon advertise at exact interval. */
#define BEACON_ABSOLUTE_INTERVAL_ADV  (true)

/** UART module for debug. */
static struct usart_module cdc_uart_module;

#define CONF_STDIO_USART_MODULE  EDBG_CDC_MODULE
#define CONF_STDIO_MUX_SETTING   EDBG_CDC_SERCOM_MUX_SETTING
#define CONF_STDIO_PINMUX_PAD0   EDBG_CDC_SERCOM_PINMUX_PAD0
#define CONF_STDIO_PINMUX_PAD1   EDBG_CDC_SERCOM_PINMUX_PAD1
#define CONF_STDIO_PINMUX_PAD2   EDBG_CDC_SERCOM_PINMUX_PAD2
#define CONF_STDIO_PINMUX_PAD3   EDBG_CDC_SERCOM_PINMUX_PAD3
#define CONF_STDIO_BAUDRATE      115200

/**
 * \brief Configure UART console.
 */
static void configure_console(void)
{
	struct usart_config usart_conf;

	usart_get_config_defaults(&usart_conf);
	usart_conf.mux_setting = CONF_STDIO_MUX_SETTING;
	usart_conf.pinmux_pad0 = CONF_STDIO_PINMUX_PAD0;
	usart_conf.pinmux_pad1 = CONF_STDIO_PINMUX_PAD1;
	usart_conf.pinmux_pad2 = CONF_STDIO_PINMUX_PAD2;
	usart_conf.pinmux_pad3 = CONF_STDIO_PINMUX_PAD3;
	usart_conf.baudrate    = CONF_STDIO_BAUDRATE;

	stdio_serial_init(&cdc_uart_module, CONF_STDIO_USART_MODULE, &usart_conf);
	usart_enable(&cdc_uart_module);
}

/** BLE scan mode. */
extern uint8 ble_wifi_scan_mode;

/** BLE pin code. */
static uint8_t ble_pin[6] = MAIN_BLE_PIN;

/* BLE advertisement data. */
static uint8_t adv_data[] = {
	0x1a, 0xff, 0x4c, 0x00, 0x02, 0x15, 0x21, 0x8A,
	0xF6, 0x52, 0x73, 0xE3, 0x40, 0xB3, 0xB4, 0x1C,
	0x19, 0x53, 0x24, 0x2C, 0x72, 0xf4, 0x00, 0xbb,
	0x00, 0x45, 0xc5
};

/** BLE scan response data. */
static uint8_t scan_rsp_data[] = {
	0x11, 0x07, 0x1b, 0xc5, 0xd5, 0xa5, 0x02, 0x00,
	0x37, 0xaa, 0xe3, 0x11, 0x2a, 0xdc, 0x00, 0xcd,
	0x30, 0x57
};

/* BLE service UUID definition. */
static at_ble_uuid_t service_uuid = {
	AT_BLE_UUID_128,
	{
		0x1b, 0xc5, 0xd5, 0xa5, 0x02, 0x00, 0x37, 0xaa,
		0xe3, 0x11, 0x2a, 0xdc, 0x00, 0xcd, 0x30, 0x57
	}
};

/* characteristics definitions */
static at_ble_characteristic_t chars[] = {
	{
		0, /* handle stored here */
		{	
			AT_BLE_UUID_128, {0x1b, 0xc5, 0xd5, 0xa5, 0x02, 0x00, 0x3b, 0x8e, 0xe3, 0x11, 0x2a, 0xdc, 0xa0, 0xd3, 0x20, 0x8e}
		}, /* UUID */
		AT_BLE_CHAR_READ | AT_BLE_CHAR_WRITE | AT_BLE_CHAR_NOTIFY, /* Properties */
		(uint8_t *)"char1", sizeof("char1"), 100, /* value */
		AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR, /* permissions */

		NULL, 0, 0, /* user defined name */
		AT_BLE_ATTR_NO_PERMISSIONS, AT_BLE_ATTR_NO_PERMISSIONS, AT_BLE_ATTR_NO_PERMISSIONS, 0,0,0, /* Handles */
		NULL /* presentation format */
	},
	{
		0, /* handle stored here */
		{
			AT_BLE_UUID_128, {0x1b, 0xc5, 0xd5, 0xa5, 0x02, 0x00, 0xff, 0xbb, 0xe3, 0x11, 0x2a, 0xdc, 0xe0, 0x40, 0xd2, 0x98}
		}, /* UUID */
		AT_BLE_CHAR_READ | AT_BLE_CHAR_WRITE | AT_BLE_CHAR_RELIABLE_WRITE, /* Properties */
		(uint8_t *)"char2", sizeof("char1"), 100, /* value */
		AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR, /* permissions */

		NULL, 0, 0, /* user defined name */
		AT_BLE_ATTR_NO_PERMISSIONS, AT_BLE_ATTR_NO_PERMISSIONS, AT_BLE_ATTR_NO_PERMISSIONS, 0, 0, 0, /* Handles */
		NULL /* presentation format */
	}
};

/**
 * \brief Callback that takes a blob from the BLE API and writes it
 * over the HIF.
 */
static void ble_api_write_func(uint8 *msg, uint32 len)
{
	/* Hooks into bottom of BLE API and receives serialized data */
	/* ready to transmit over cor2cor. */
	m2m_wifi_ble_api_send(msg, len);
}

/**
 * \brief Platform specific wait function that is called by the BLE
 * API when it is awaiting responses.
 *
 * \note We need to ensure we handle incoming HIF events during this
 * time to actually process the BLE response.
 */
static void plf_wait_func(void)
{
	while (m2m_wifi_handle_events(NULL) != M2M_SUCCESS) {
	}
}

/**
 * \brief Initialize the BLE beacon.
 */
static void beacon_init(void)
{
	static at_ble_handle_t service;

	printf("beacon_init: started\n");
	/* Establish peripheral database. */
	if (at_ble_primary_service_define(&service_uuid, &service,
			NULL, 0, chars, 2) != AT_BLE_SUCCESS) {
		printf("beacon_init: failed to define the primary service!\n");
	} else {
		printf("beacon_init: primary service defined.\n");
	}

	/* Set beacon advertisement data. */
	if (at_ble_adv_data_set(adv_data, sizeof(adv_data), scan_rsp_data, sizeof(scan_rsp_data)) != AT_BLE_SUCCESS) {
		printf("beacon_init: failed to set BLE beacon advertisement data!\n");
	} else {
		printf("beacon_init: advertisement data set.\n");
	}

	/* BLE start advertisement. */
	if (at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED, AT_BLE_ADV_GEN_DISCOVERABLE, NULL, AT_BLE_ADV_FP_ANY,
			BEACON_ADV_INTERVAL, BEACON_ADV_TIMEOUT, BEACON_ABSOLUTE_INTERVAL_ADV) != AT_BLE_SUCCESS) {
		printf("beacon_init: failed to start beacon advertisement!\n");
	} else {
		printf("beacon_init: beacon advertisement started.\n");
	}

	printf("beacon_init: done.\n");
}

/** Wi-Fi status variable. */
static uint8 gu8IsWiFiConnected = M2M_WIFI_DISCONNECTED;

/** Wi-Fi scan variables. */
static uint8 gu8Index = 0;
static struct wifiprov_scanlist_ind ble_scan_list;

/**
 * \brief Callback to get the Wi-Fi status update.
 *
 * \param[in] u8MsgType type of Wi-Fi notification. Possible types are:
 *  - [M2M_WIFI_RESP_CON_STATE_CHANGED](@ref M2M_WIFI_RESP_CON_STATE_CHANGED)
 *  - [M2M_WIFI_RESP_SCAN_DONE](@ref M2M_WIFI_RESP_SCAN_DONE)
 *  - [M2M_WIFI_RESP_SCAN_RESULT](@ref M2M_WIFI_RESP_SCAN_RESULT)
 *  - [M2M_WIFI_REQ_DHCP_CONF](@ref M2M_WIFI_REQ_DHCP_CONF)
 * \param[in] pvMsg A pointer to a buffer containing the notification parameters
 * (if any). It should be casted to the correct data type corresponding to the
 * notification type.
 */
static void wifi_cb(uint8 u8MsgType, void *pvMsg)
{
	switch (u8MsgType) {
	case M2M_WIFI_RESP_CON_STATE_CHANGED:
	{
		tstrM2mWifiStateChanged *pstrWifiState = (tstrM2mWifiStateChanged *)pvMsg;
		if (pstrWifiState->u8CurrState == M2M_WIFI_CONNECTED) {
			printf("wifi_cb: M2M_WIFI_RESP_CON_STATE_CHANGED: CONNECTED\r\n");
			gwifiprov_complete_ind.status = APP_PROV_IDLE;
		} else {
			printf("wifi_cb: M2M_WIFI_RESP_CON_STATE_CHANGED: DISCONNECTED\r\n");
			gu8IsWiFiConnected = M2M_WIFI_DISCONNECTED;
			wifiprov_wifi_con_update(WIFIPROV_CON_STATE_DISCONNECTED);
		}
		break;
	} 
	
	case M2M_WIFI_REQ_DHCP_CONF:
	{
		gu8IsWiFiConnected = M2M_WIFI_CONNECTED;
		tstrM2MIPConfig *pstrM2MIpConfig = (tstrM2MIPConfig *)pvMsg;
		uint8 *pu8IPAddress = (uint8 *)&pstrM2MIpConfig->u32StaticIP;
		printf("wifi_cb: M2M_WIFI_REQ_DHCP_CONF: IP is %u.%u.%u.%u\r\n",
				pu8IPAddress[0], pu8IPAddress[1], pu8IPAddress[2], pu8IPAddress[3]);
		wifiprov_wifi_con_update(WIFIPROV_CON_STATE_CONNECTED);
		beacon_init();
		break;
	} 

	case M2M_WIFI_RESP_SCAN_DONE:
	{
		tstrM2mScanDone *pstrInfo = (tstrM2mScanDone *)pvMsg;
		printf("wifi_cb: M2M_WIFI_RESP_SCAN_DONE\n");
		if (gu8IsWiFiConnected == M2M_WIFI_DISCONNECTED) {
			gu8Index = 0;
			if (pstrInfo->u8NumofCh >= 1) {
				m2m_wifi_req_scan_result(gu8Index);
				gu8Index++;
			}
		}
		break;
	}
	
	case M2M_WIFI_RESP_SCAN_RESULT:
	{
		uint8 u8NumFoundAPs = m2m_wifi_get_num_ap_found();
		tstrM2mWifiscanResult *pstrScanResult = (tstrM2mWifiscanResult *)pvMsg;
		printf("wifi_cb: M2M_WIFI_RESP_SCAN_RESULT: found SSID \"%s\"\n", pstrScanResult->au8SSID);
		if (gu8IsWiFiConnected == M2M_WIFI_DISCONNECTED) {
			if (gu8Index < u8NumFoundAPs) {
				if (gu8Index < MAX_WIPROVTASK_AP_NUM) {
					ble_scan_list.scandetails[gu8Index].rssi = pstrScanResult->s8rssi;
					ble_scan_list.scandetails[gu8Index].sec_type = pstrScanResult->u8AuthType;
					memcpy(ble_scan_list.scandetails[gu8Index].ssid, pstrScanResult->au8SSID, sizeof(ble_scan_list.scandetails[gu8Index].ssid));
					ble_scan_list.num_valid = gu8Index + 1;
				}
				m2m_wifi_req_scan_result(gu8Index);
				gu8Index++;
			} else {
				if (ble_wifi_scan_mode == 1) {
					wifiprov_scan_list_ind_send(&ble_scan_list);
					printf("\nPlease open Android application to provision Wi-Fi over BLE...\n\n");
				}
			}
		}
		break;
	}
	
	case M2M_WIFI_RESP_BLE_API_RECV:
	{
		tstrM2mBleApiMsg *rx = (tstrM2mBleApiMsg *)pvMsg;
		platform_receive(rx->data, rx->u16Len);
		break;
	}
	}
}

/**
 * \brief Disable WINC3400 pull ups.
 */
static void disable_pullups(void)
{
	uint32 pinmask;

	pinmask = (M2M_PERIPH_PULLUP_HOST_WAKEUP | M2M_PERIPH_PULLUP_GPIO_3 | M2M_PERIPH_PULLUP_GPIO_5 | M2M_PERIPH_PULLUP_SD_DAT0_SPI_TXD | M2M_PERIPH_PULLUP_GPIO_6);

	m2m_periph_pullup_ctrl(pinmask, 0);
}

/**
 * \brief Main application function.
 *
 * \return program return value.
 */
int main(void)
{
	tstrWifiInitParam param;
	sint8 ret;
	plf_params_t plf;
	
	/* Initialize the board. */
	system_init();

	/* Initialize the UART console. */
	configure_console();

	/* Output example information */
	puts(STRING_HEADER);

	/* Initialize the WiFi BSP. */
	nm_bsp_init();

	/* Initialize WiFi driver with data and status callbacks. */
	printf("main: started WiFi.\n");
	m2m_memset((uint8 *)&param, 0, sizeof(param));
	param.pfAppWifiCb = wifi_cb;
	ret = m2m_wifi_init(&param);
	if (M2M_SUCCESS != ret) {
		printf("main: failed to initialize WiFi! (%d)\n", ret);
		while (1);
	}

	/* Disable low power mode. */
	disable_pullups();
	m2m_wifi_set_sleep_mode(M2M_NO_PS, 1);
	gu8IsWiFiConnected = M2M_WIFI_DISCONNECTED;
	nm_bsp_sleep(2000);

	/* Initialize the BLE chip  and Set the Device Address */
	printf("main: started BLE.\n");
	m2m_memset((uint8 *)&plf, 0, sizeof(plf));
	plf.ble_write_cb = ble_api_write_func;
	plf.plf_wait_cb = plf_wait_func;
	if (at_ble_init(&plf) != AT_BLE_SUCCESS) {
		printf("main: failed to initialize BLE!\n");
		while (1);
	}

	/* Setup the provisioning service. */
	if (wifiprov_configure_provisioning() != AT_BLE_SUCCESS) {
		printf("main: failed to configure BLE provisioning!\n");
		while (1);
	}
	/* Fill in the advertisement data. */
	if (wifiprov_create_db() != AT_BLE_SUCCESS) {
		printf("main: failed to create BLE provisioning database!\n");
		while (1);
	}
	
	/* Start provisioning and set PIN code. */
	printf("main: started BLE provisioning.\n");
	if (wifiprov_start(ble_pin, 6) != AT_BLE_SUCCESS) {
		printf("main: failed to start BLE provisioning!\n");
		while (1);
	}

	while (1) {
		/* Handle BLE events. */
		ble_event_task();

		/* Handle WiFi events. */
		m2m_wifi_handle_events(NULL);
	}
	
	return 0;
}
