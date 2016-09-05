/**
 *
 * \file
 *
 * \brief WINC1500 Mode Change Example.
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
 * This example demonstrates the use of the WINC1500 with the SAM Xplained Pro
 * board to bring up as a station, an AP or a P2P mode.<br>
 * It uses the following hardware:
 * - the SAM Xplained Pro.
 * - the WINC1500 on EXT1.
 *
 * \section files Main Files
 * - main.c : Initialize the WINC1500. For the first time, the device act as
 * station mode. Then switch to AP mode and also P2P mode. For the details in each
 * mode, refer to the MODE_STA, MODE_AP and MODE_P2P example.
 *
 * \section usage Usage
 * -# Configure below code in the main.h for AP information.
 * \code
 *    #define MAIN_WLAN_SSID              "DEMO_AP"
 *    #define MAIN_WLAN_AUTH              M2M_WIFI_SEC_OPEN
 *    #define MAIN_WLAN_AP_CHANNEL        (6)
 * \endcode
 * -# Configure below code in the main.h for P2P information.
 * \code
 *    #define MAIN_WLAN_DEVICE_NAME       "WINC1500_P2P"
 *    #define MAIN_WLAN_P2P_CHANNEL       (6)
 * \endcode
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
 *    -- WINC1500 mode change example --
 *    -- SAMD21_XPLAINED_PRO --
 *    -- Compiled: xxx xx xxxx xx:xx:xx --
 *    AP mode, start
 *    AP mode, end
 *    P2P mode, start
 *    P2P mode, end
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

#include "asf.h"
#include "main.h"
#include <string.h>
#include "driver/include/m2m_wifi.h"
#include "driver/source/nmasic.h"

#define STRING_EOL    "\r\n"
#define STRING_HEADER "-- WINC1500 mode change example --"STRING_EOL \
	"-- "BOARD_NAME " --"STRING_EOL	\
	"-- Compiled: "__DATE__ " "__TIME__ " --"STRING_EOL

#define HOLD_TIME_IN_MODE               (30000)
#define DELAY_FOR_MODE_CHANGE           (500)


/**
 * \brief Configure UART console.
 */
static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate =		CONF_UART_BAUDRATE,
		.charlength =	CONF_UART_CHAR_LENGTH,
		.paritytype =	CONF_UART_PARITY,
		.stopbits =		CONF_UART_STOP_BITS,
	};

	/* Configure UART console. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

/**
 * \brief AP mode
 *
 * Initializes the AP mode for a while and terminate.
 */
static int8_t enable_disable_ap_mode(void)
{
	int8_t ret;
	tstrM2MAPConfig strM2MAPConfig;

	printf("AP mode, start\r\n");

	/* Configure AP. */
	memset(&strM2MAPConfig, 0x00, sizeof(tstrM2MAPConfig));
	strcpy((char *)&strM2MAPConfig.au8SSID, MAIN_WLAN_SSID);
	strM2MAPConfig.u8ListenChannel = MAIN_WLAN_AP_CHANNEL;
	strM2MAPConfig.u8SecType = MAIN_WLAN_AUTH;
	strM2MAPConfig.au8DHCPServerIP[0] = 0xC0; /* 192 */
	strM2MAPConfig.au8DHCPServerIP[1] = 0xA8; /* 168 */
	strM2MAPConfig.au8DHCPServerIP[2] = 0x01; /* 1 */
	strM2MAPConfig.au8DHCPServerIP[3] = 0x01; /* 1 */

	/* Start AP mode. */
	ret = m2m_wifi_enable_ap(&strM2MAPConfig);
	if (M2M_SUCCESS != ret) {
		return ret;
	}

	/* Keep in AP mode for a while. */
	nm_bsp_sleep(HOLD_TIME_IN_MODE);

	/* Stop AP mode. */
	ret = m2m_wifi_disable_ap();
	if (M2M_SUCCESS != ret) {
		return ret;
	}

	printf("AP mode, end\r\n");

	return ret;
}

/**
 * \brief P2P mode
 *
 * Initializes the P2P mode for a while and terminate.
 */
static int8_t enable_disable_p2p_mode(void)
{
	int8_t ret;

	printf("P2P mode, start\r\n");

	/* Set device name. */
	ret = m2m_wifi_set_device_name((uint8_t *)MAIN_WLAN_DEVICE_NAME, strlen(MAIN_WLAN_DEVICE_NAME));
	if (M2M_SUCCESS != ret) {
		return ret;
	}

	/* Start P2P with channel number. */
	ret = m2m_wifi_p2p(MAIN_WLAN_P2P_CHANNEL);
	if (M2M_SUCCESS != ret) {
		return ret;
	}

	/* Keep in P2P mode for a while. */
	nm_bsp_sleep(HOLD_TIME_IN_MODE);

	/* Stop P2P mode. */
	ret = m2m_wifi_p2p_disconnect();
	if (M2M_SUCCESS != ret) {
		return ret;
	}

	printf("P2P mode, end\r\n");

	return ret;
}

/**
 * \brief Main application function.
 *
 * Application entry point.
 *
 * \return program return value.
 */
int main(void)
{
	tstrWifiInitParam param;
	int8_t ret;

	/* Initialize the board. */
	sysclk_init();
	board_init();

	/* Initialize the UART console. */
	configure_console();
	printf(STRING_HEADER);

	/* Initialize the BSP. */
	nm_bsp_init();

	/* Initialize Wi-Fi parameters structure. */
	memset((uint8_t *)&param, 0, sizeof(tstrWifiInitParam));

	/* Initialize Wi-Fi driver with data and status callbacks. */
	ret = m2m_wifi_init(&param);
	if (M2M_SUCCESS != ret) {
		printf("main: m2m_wifi_init call error!(%d)\r\n", ret);
		while (1) {
		}
	}

	/**
	 * Station mode.
	 * Device started as station mode basically.
	 */
	if (1) {
	}

	/**
	 * AP mode.
	 * Turn On and off AP mode.
	 */
	ret = enable_disable_ap_mode();
	if (M2M_SUCCESS != ret) {
		printf("main: enable_disable_ap_mode call error!\r\n");
		while (1) {
		}
	}

	nm_bsp_sleep(DELAY_FOR_MODE_CHANGE);

	/**
	 * P2P mode.
	 * Turn On and off P2P mode.
	 */
	ret = enable_disable_p2p_mode();
	if (M2M_SUCCESS != ret) {
		printf("main: enable_disable_p2p_mode call error!\r\n");
		while (1) {
		}
	}

	return 0;
}
