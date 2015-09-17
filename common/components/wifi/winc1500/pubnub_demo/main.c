/**
 * \file
 *
 * \brief PubNub Example.
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
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
 *    Atmel microcontroller product.
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
 * This example demonstrates the use of the SAMW25 Xplained Pro with I/O1 Xplained Pro
 * board to communicate with pubnub. <br>
 * It uses the following hardware:
 * - SAMW25 Xplained Pro.
 * - I/O1 Xplained Pro on EXT1.
 *
 * \section files Main Files
 * - main.c : Initialize the SAMW25 and communicate with pubnub.
 *
 * \section usage Usage
 * -# Configure below code in the main.h for AP information to be connected.
 * \code
 *    #define MAIN_WIFI_M2M_WLAN_SSID     "SSID"
 *    #define MAIN_WIFI_M2M_WLAN_AUTH     M2M_WIFI_SEC_WPA_PSK
 *    #define MAIN_WIFI_M2M_WLAN_PSK      "PASSWORD"
 * \endcode
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
 *    -- SAMW25 PubNub example --
 *    -- SAMW25_XPLAINED_PRO --
 *    -- Compiled: xxx xx xxxx xx:xx:xx --
 * Wi-Fi connected
 * Wi-Fi IP is xxx.xxx.xxx.xxx
 * pubnubDemo: publish event.
 * pubnubDemo: publish event.
 * pubnubDemo: subscribe event.
 * pubnubDemo: Received message: {"temperature":"xx.xx"}
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
#include "conf_uart_serial.h"
#include "common/include/nm_common.h"
#include "driver/include/m2m_wifi.h"
#include "socket/include/socket.h"
#include "main.h"
#include "PubNub.h"

#define STRING_EOL    "\r\n"
#define STRING_HEADER "-- SAMW25 PubNub example --"STRING_EOL	\
	"-- "BOARD_NAME " --"STRING_EOL	\
	"-- Compiled: "__DATE__ " "__TIME__ " --"STRING_EOL

/** UART module for debug. */
static struct usart_module cdc_uart_module;

/** Wi-Fi connection state */
static volatile uint8_t wifi_connected;

/** SysTick counter to avoid busy wait delay. */
volatile uint32_t gu32MsTicks = 0;

/** Global variables of the pubnub */
static const char pubkey[] = "demo";
static const char subkey[] = "demo";
static const char channel[] = "AtmelGallery_Pubnub";
static pubnub_t *m_pb;

/**
 * \brief Callback to get the Socket event.
 *
 * \param[in] Socket descriptor.
 * \param[in] msg_type type of Socket notification. Possible types are:
 *  - [SOCKET_MSG_CONNECT](@ref SOCKET_MSG_CONNECT)
 *  - [SOCKET_MSG_BIND](@ref SOCKET_MSG_BIND)
 *  - [SOCKET_MSG_LISTEN](@ref SOCKET_MSG_LISTEN)
 *  - [SOCKET_MSG_ACCEPT](@ref SOCKET_MSG_ACCEPT)
 *  - [SOCKET_MSG_RECV](@ref SOCKET_MSG_RECV)
 *  - [SOCKET_MSG_SEND](@ref SOCKET_MSG_SEND)
 *  - [SOCKET_MSG_SENDTO](@ref SOCKET_MSG_SENDTO)
 *  - [SOCKET_MSG_RECVFROM](@ref SOCKET_MSG_RECVFROM)
 * \param[in] msg_data A structure contains notification informations.
 */
static void socket_event_cb(SOCKET sock, uint8_t msg_type, void *msg_data)
{
	handle_tcpip(sock, msg_type, msg_data);
}

/**
 * \brief Callback of gethostbyname function.
 *
 * \param[in] doamin_name Domain name.
 * \param[in] server_ip IP of server.
 */
static void socket_resolve_cb(uint8_t *domain_name, uint32_t server_ip)
{
	handle_dns_found((char *)domain_name, server_ip);
}

/**
 * \brief Callback to get the Wi-Fi status update.
 *
 * \param[in] u8MsgType type of Wi-Fi notification. Possible types are:
 *  - [M2M_WIFI_RESP_CURRENT_RSSI](@ref M2M_WIFI_RESP_CURRENT_RSSI)
 *  - [M2M_WIFI_RESP_CON_STATE_CHANGED](@ref M2M_WIFI_RESP_CON_STATE_CHANGED)
 *  - [M2M_WIFI_RESP_CONNTION_STATE](@ref M2M_WIFI_RESP_CONNTION_STATE)
 *  - [M2M_WIFI_RESP_SCAN_DONE](@ref M2M_WIFI_RESP_SCAN_DONE)
 *  - [M2M_WIFI_RESP_SCAN_RESULT](@ref M2M_WIFI_RESP_SCAN_RESULT)
 *  - [M2M_WIFI_REQ_WPS](@ref M2M_WIFI_REQ_WPS)
 *  - [M2M_WIFI_RESP_IP_CONFIGURED](@ref M2M_WIFI_RESP_IP_CONFIGURED)
 *  - [M2M_WIFI_RESP_IP_CONFLICT](@ref M2M_WIFI_RESP_IP_CONFLICT)
 *  - [M2M_WIFI_RESP_P2P](@ref M2M_WIFI_RESP_P2P)
 *  - [M2M_WIFI_RESP_AP](@ref M2M_WIFI_RESP_AP)
 *  - [M2M_WIFI_RESP_CLIENT_INFO](@ref M2M_WIFI_RESP_CLIENT_INFO)
 * \param[in] pvMsg A pointer to a buffer containing the notification parameters
 * (if any). It should be casted to the correct data type corresponding to the
 * notification type. Existing types are:
 *  - tstrM2mWifiStateChanged
 *  - tstrM2MWPSInfo
 *  - tstrM2MP2pResp
 *  - tstrM2MAPResp
 *  - tstrM2mScanDone
 *  - tstrM2mWifiscanResult
 */
static void wifi_cb(uint8_t msg_type, void *msg_data)
{
	switch (msg_type) {
	case M2M_WIFI_RESP_CON_STATE_CHANGED:
	{
		tstrM2mWifiStateChanged *msg_wifi_state = (tstrM2mWifiStateChanged *)msg_data;
		if (msg_wifi_state->u8CurrState == M2M_WIFI_CONNECTED) {
			printf("Wi-Fi connected\r\n");
			m2m_wifi_request_dhcp_client();
		} else if (msg_wifi_state->u8CurrState == M2M_WIFI_DISCONNECTED) {
			printf("Wi-Fi disconnected\r\n");
			wifi_connected = M2M_WIFI_DISCONNECTED;
			m2m_wifi_connect((char *)MAIN_WIFI_M2M_WLAN_SSID, strlen(MAIN_WIFI_M2M_WLAN_SSID),
					MAIN_WIFI_M2M_WLAN_AUTH, (void *)MAIN_WIFI_M2M_WLAN_PSK, M2M_WIFI_CH_ALL );
		}

		break;
	}

	case M2M_WIFI_REQ_DHCP_CONF:
	{
		uint8_t *msg_ip_addr = (uint8_t *)msg_data;
		printf("Wi-Fi IP is %u.%u.%u.%u\r\n",
				msg_ip_addr[0], msg_ip_addr[1], msg_ip_addr[2], msg_ip_addr[3]);
		wifi_connected = M2M_WIFI_CONNECTED;
		break;
	}

	default:
	{
		break;
	}
	}
}

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

/*
 * \brief SysTick handler used to measure precise delay. 
*/
void SysTick_Handler(void)
{
	gu32MsTicks++;
}

/**
 * \brief Main application function.
 *
 * Initialize board and WINC1500 Wi-Fi module.
 * Publish and subscribe event to pubnub.
 *
 * \return program return value.
 */
int main(void)
{
	tstrWifiInitParam param;
	int8_t ret;

	/* Initialize the board. */
	system_init();

	/* Initialize the UART console. */
	configure_console();

	/* Initialize the delay driver. */
	delay_init();

	/* Enable SysTick interrupt for non busy wait delay. */
	if (SysTick_Config(system_cpu_clock_get_hz() / 1000)) {
		puts("main: SysTick configuration error!");
		while (1);
	}

	/* Initialize Temperature Sensor */
	at30tse_init();

	/* Initialize the BSP. */
	nm_bsp_init();

	/* Output example information */
	printf(STRING_HEADER);

	/* Initialize Wi-Fi parameters structure. */
	memset((uint8_t *)&param, 0, sizeof(tstrWifiInitParam));
	param.pfAppWifiCb = wifi_cb;

	/* Initialize WINC1500 Wi-Fi driver with data and status callbacks. */
	ret = m2m_wifi_init(&param);
	if (M2M_SUCCESS != ret) {
		printf("main: m2m_wifi_init call error!(%d)\r\n", ret);
		while (1) {
		}
	}

	/* Initialize socket module */
	socketInit();
	registerSocketCallback(socket_event_cb, socket_resolve_cb);

	/* Connect to router. */
	ret = m2m_wifi_connect((char *)MAIN_WIFI_M2M_WLAN_SSID, strlen(MAIN_WIFI_M2M_WLAN_SSID),
			MAIN_WIFI_M2M_WLAN_AUTH, (void *)MAIN_WIFI_M2M_WLAN_PSK, M2M_WIFI_CH_ALL);
	if (M2M_SUCCESS != ret) {
		printf("main: failed to connect access point!(%d)\r\n", ret);
		while (1) {
		}
	}

	/* Get a handle of the pubnub */
	m_pb = pubnub_get_ctx(0);

	/* Initialize pubnub module */
	pubnub_init(m_pb, pubkey, subkey);

	while (1) {
		/* Handle pending events from network controller. */
		m2m_wifi_handle_events(NULL);

		if (wifi_connected == M2M_WIFI_CONNECTED) {
			if (m_pb->state == PS_IDLE) {
				char buf[256] = {0, };

				nm_bsp_sleep(1000);

				if (m_pb->trans == PBTT_NONE ||
						(m_pb->trans == PBTT_SUBSCRIBE && m_pb->last_result == PNR_OK) ||
						(m_pb->trans == PBTT_PUBLISH && m_pb->last_result == PNR_IO_ERROR)) {
					while (1) {
						char const *msg = pubnub_get(m_pb);
						if (NULL == msg) {
							break;
						}

						printf("pubnubDemo: Received message: %s\r\n", msg);
					}

					printf("pubnubDemo: publish event.\r\n");

					/* Get current temperature from the sensor of the IO1 Xplained Pro */
					double temperature = at30tse_read_temperature();
					sprintf(buf, "{\"temperature\":\"%d.%d\"}", (int)temperature, (int)((int)(temperature * 100) % 100));

					/* Publish data to pubnub */
					pubnub_publish(m_pb, channel, buf);
				} else {
					printf("pubnubDemo: subscribe event.\r\n");

					/* Subscribe to pubnub */
					pubnub_subscribe(m_pb, channel);
				}
			}
		}
	}

	return 0;
}
