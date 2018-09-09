/**
 *
 * \file
 *
 * \brief HTTP File Downloader Example Implementation.
 *
 * Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries.
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

#include <errno.h>
#include "asf.h"
#include "httpdownloader.h"
#include "FreeRTOS.h"
#include "iot/http/http_client.h"
#include "osprintf.h"
#include "lwip/ip.h"
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "lwip/sockets.h"
#include "os/include/net_init.h"
#include "os/include/m2m_wifi_ex.h"
#include "tinyservices.h"
#include "conf_wilc.h"

#include <string.h>
#include <stdio.h>

/** File download processing state. */
static download_state down_state = NOT_READY;
/** Http content length. */
static uint32_t http_file_size = 0;
/** Receiving content length. */
static uint32_t received_file_size = 0;
/** File name to download. */
static char save_file_name[MAIN_MAX_FILE_NAME_LENGTH + 1] = "0:";

/** Instance of Timer module. */
struct sw_timer_module swt_module_inst;

/** Instance of HTTP client module. */
struct http_client_module http_client_module_inst;

/** WEP Security parameters. */
tstrM2mWifiWepParams  gstrWepParam = WEP_CONN_PARAM;

/**
 * \brief Initialize download state to not ready.
 */
static void init_state(void)
{
	down_state = NOT_READY;
}


/**
 * \brief Clear state parameter at download processing state.
 * \param[in] mask Check download_state.
 */
static void clear_state(download_state mask)
{
	down_state &= ~mask;
}

/**
 * \brief Add state parameter at download processing state.
 * \param[in] mask Check download_state.
 */
static void add_state(download_state mask)
{
	down_state |= mask;
}

/**
 * \brief File download processing state check.
 * \param[in] mask Check download_state.
 * \return true if this state is set, false otherwise.
 */

static inline bool is_state_set(download_state mask)
{
	return ((down_state & mask) != 0);
}

/**
 * \brief Start file download via HTTP connection.
 */
static void start_download(void)
{
	if (!is_state_set(WIFI_CONNECTED)) {
		osprintf("start_download: Wi-Fi is not connected.\r\n");
		return;
	}

	if (is_state_set(GET_REQUESTED)) {
		osprintf("start_download: request is sent already.\r\n");
		return;
	}

	if (is_state_set(DOWNLOADING)) {
		osprintf("start_download: running download already.\r\n");
		return;
	}
	int ret;
	/* Send the HTTP request. */
	osprintf("Sending HTTP request...\r\n");
	ret = http_client_send_request(&http_client_module_inst, MAIN_HTTP_FILE_URL, HTTP_METHOD_GET, NULL, NULL);
	if(ret != 0){
		osprintf("HTTP request send failed err %d\r\n",ret);
		return;
	}
	
	/* Receive the response*/
	osprintf("Receiving response...\r\n");
	while(!(is_state_set(COMPLETED) || is_state_set(CANCELED))){
		http_client_recv_response(&http_client_module_inst);
	
	}	
}

/**
 * \brief Ignore data, but calculate the total received size TODO: Store received packet to file
 * \param[in] data Packet data.
 * \param[in] length Packet data length.
 */
static void store_file_packet(char *data, uint32_t length)
{
	if ((data == NULL) || (length < 1)) {
		return;
	}

	if (!is_state_set(DOWNLOADING)) {
		received_file_size = 0;
		add_state(DOWNLOADING);
	}

	if (data != NULL) {
		received_file_size += length;
		osprintf("Received %d\r",received_file_size);
		if (received_file_size >= http_file_size) {
			osprintf("Received %d File size %d\r\n",received_file_size,http_file_size);
			osprintf("store_file_packet: file downloaded successfully.\r\n");
			//char * temp  = (char*)pvPortMalloc(MAIN_BUFFER_MAX_SIZE);
			//memcpy(temp,data,length);
			add_state(COMPLETED);
			return;
		}
	}
}


/**
 * \brief Callback of the HTTP client.
 *
 * \param[in]  module_inst     Module instance of HTTP client module.
 * \param[in]  type            Type of event.
 * \param[in]  data            Data structure of the event. \refer http_client_data
 */
static void http_client_callback(struct http_client_module *module_inst, int type, union http_client_data *data)
{
	switch (type) {
	case HTTP_CLIENT_CALLBACK_SOCK_CONNECTED:
		osprintf("http_client_callback: HTTP client socket connected.\r\n");
		break;

	case HTTP_CLIENT_CALLBACK_REQUESTED:
		osprintf("http_client_callback: request completed.\r\n");
		add_state(GET_REQUESTED);
		break;

	case HTTP_CLIENT_CALLBACK_RECV_RESPONSE:
		osprintf("http_client_callback: received HTTP response %u data size %u\r\n",
				(unsigned int)data->recv_response.response_code,
				(unsigned int)data->recv_response.content_length);
		if ((unsigned int)data->recv_response.response_code == 200) {
			http_file_size = data->recv_response.content_length;
			received_file_size = 0;
		} 
		else {
			add_state(CANCELED);
			return;
		}
		if (data->recv_response.content_length <= MAIN_BUFFER_MAX_SIZE) {
			store_file_packet(data->recv_response.content, data->recv_response.content_length);
			add_state(COMPLETED);
		}
		break;

	case HTTP_CLIENT_CALLBACK_RECV_CHUNKED_DATA:
		store_file_packet(data->recv_chunked_data.data, data->recv_chunked_data.length);
		if (data->recv_chunked_data.is_complete) {
			add_state(COMPLETED);
		}

		break;

	case HTTP_CLIENT_CALLBACK_DISCONNECTED:
		osprintf("http_client_callback: disconnection reason:%d\r\n", data->disconnected.reason);

		/* If disconnect reason is equal to -ECONNRESET(-104),
		 * It means the server has closed the connection (timeout).
		 * This is normal operation.
		 */
		if (data->disconnected.reason == -EAGAIN) {
			/* Server has not responded. Retry immediately. */
			if (is_state_set(DOWNLOADING)) {
				clear_state(DOWNLOADING);
			}

			if (is_state_set(GET_REQUESTED)) {
				clear_state(GET_REQUESTED);
			}

			start_download();
		}

		break;
	}
}


/**
 * \brief Callback to get the Wi-Fi status update.
 *
 * \param[in] u8MsgType type of Wi-Fi notification. Possible types are:
 *  - [M2M_WIFI_RESP_CON_STATE_CHANGED](@ref M2M_WIFI_RESP_CON_STATE_CHANGED)
 *  - [M2M_WIFI_REQ_DHCP_CONF](@ref M2M_WIFI_REQ_DHCP_CONF)
 * \param[in] pvMsg A pointer to a buffer containing the notification parameters
 * (if any). It should be casted to the correct data type corresponding to the
 * notification type.
 */
static void wifi_cb(uint8_t u8MsgType, void *pvMsg)
{
	switch (u8MsgType) {
		case M2M_WIFI_RESP_CON_STATE_CHANGED:
		{			
			tstrM2mWifiStateChanged *pstrWifiState = (tstrM2mWifiStateChanged *)pvMsg;
			if (pstrWifiState->u8CurrState == M2M_WIFI_CONNECTED) {
				osprintf("Wi-Fi connected\r\n");
				net_interface_up(NET_IF_STA);
				m2m_wifi_request_dhcp_client_ex();
			} else if (pstrWifiState->u8CurrState == M2M_WIFI_DISCONNECTED) {
				printf("Wifi disconnected\r\n");
				clear_state(WIFI_CONNECTED);
				if (is_state_set(DOWNLOADING)) {
					clear_state(DOWNLOADING);
				}

				if (is_state_set(GET_REQUESTED)) {
					clear_state(GET_REQUESTED);
				}
				osprintf("wifi_cb: reconnecting...\r\n");
				net_interface_down(NET_IF_STA);
				/* Connect to defined AP. */
				if (MAIN_WLAN_AUTH == M2M_WIFI_SEC_WPA_PSK) {
					os_m2m_wifi_connect((char *)MAIN_WLAN_SSID, sizeof(MAIN_WLAN_SSID), MAIN_WLAN_AUTH, (tuniM2MWifiAuth *)MAIN_WLAN_PSK, M2M_WIFI_CH_ALL);
				} else if (MAIN_WLAN_AUTH == M2M_WIFI_SEC_WEP){
					os_m2m_wifi_connect((char *)MAIN_WLAN_SSID, sizeof(MAIN_WLAN_SSID), MAIN_WLAN_AUTH, (tuniM2MWifiAuth *)&gstrWepParam, M2M_WIFI_CH_ALL);
				}else{
					os_m2m_wifi_connect((char *)MAIN_WLAN_SSID, sizeof(MAIN_WLAN_SSID), MAIN_WLAN_AUTH, NULL, M2M_WIFI_CH_ALL);
				}
			}
			break;
		}

		case NET_IF_REQ_DHCP_CONF:
		{
			tstrM2MIPConfig2 *strIpConfig = pvMsg;
			uint16_t *a = (void *)strIpConfig->u8StaticIPv6;
			osprintf("wifi_cb: IPv4 addr: %d.%d.%d.%d\r\n", strIpConfig->u8StaticIP[0], strIpConfig->u8StaticIP[1],
			strIpConfig->u8StaticIP[2], strIpConfig->u8StaticIP[3]);
			osprintf("wifi_cb: IPv6 addr: %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\r\n",
			htons(a[0]), htons(a[1]), htons(a[2]), htons(a[3]),
			htons(a[4]), htons(a[5]), htons(a[6]), htons(a[7]));
		
			add_state(WIFI_CONNECTED);
			
			break;
		}

		default:
		{
			break;
		}
	}
}


/**
 * \brief Configure Timer module.
 */
static void configure_timer(void)
{
	struct sw_timer_config swt_conf;
	sw_timer_get_config_defaults(&swt_conf);

	sw_timer_init(&swt_module_inst, &swt_conf);
	sw_timer_enable(&swt_module_inst);
}

/**
 * \brief Configure HTTP client module.
 */
static void configure_http_client(void)
{
	struct http_client_config httpc_conf;
	int ret;

	http_client_get_config_defaults(&httpc_conf);

	httpc_conf.recv_buffer_size = MAIN_BUFFER_MAX_SIZE;
	httpc_conf.timer_inst = &swt_module_inst;

	ret = http_client_init(&http_client_module_inst, &httpc_conf);
	if (ret < 0) {
		osprintf("configure_http_client: HTTP client initialization failed! (res %d)\r\n", ret);
		while (1) {} /* Loop forever. */
	}

	http_client_register_callback(&http_client_module_inst, http_client_callback);
}
/**
 * \brief http downloader function.
 */
void http_downloader(void *argument)
{
	/*initialize state as NOT_READY*/
	init_state();

	/* Initialize the network stack. */
	net_init();
	
	/* Initialize the Timer. */
	configure_timer();
	
	/* Initialize the HTTP client service. */
	configure_http_client();
	
	/* Initialize the WILC driver. */
	tstrWifiInitParam param;
	/* Initialize Wi-Fi parameters structure. */
	memset(&param, 0, sizeof(param));
	/* Initialize Wi-Fi driver with data and status callbacks. */
	param.pfAppWifiCb = wifi_cb;
	sint8 ret = os_m2m_wifi_init(&param);
	if (M2M_SUCCESS != ret) {
		osprintf("main: m2m_wifi_init call error! (res %d)\r\n", ret);
		while (1) {
		}
	}

	/* Connect to router. */
	osprintf("connecting to WiFi AP %s...\r\n", (char *)MAIN_WLAN_SSID);
	if (MAIN_WLAN_AUTH == M2M_WIFI_SEC_WPA_PSK) {
		os_m2m_wifi_connect((char *)MAIN_WLAN_SSID, sizeof(MAIN_WLAN_SSID), MAIN_WLAN_AUTH, (tuniM2MWifiAuth *)MAIN_WLAN_PSK, M2M_WIFI_CH_ALL);
	} else if (MAIN_WLAN_AUTH == M2M_WIFI_SEC_WEP){
		os_m2m_wifi_connect((char *)MAIN_WLAN_SSID, sizeof(MAIN_WLAN_SSID), MAIN_WLAN_AUTH, (tuniM2MWifiAuth *)&gstrWepParam, M2M_WIFI_CH_ALL);
	}else{
		os_m2m_wifi_connect((char *)MAIN_WLAN_SSID, sizeof(MAIN_WLAN_SSID), MAIN_WLAN_AUTH, NULL, M2M_WIFI_CH_ALL);
	}
	
	/*wait for wifi connection*/
	while(!is_state_set(WIFI_CONNECTED)){
		vTaskDelay(20);
	}
	/*start downloading*/
	start_download();
	
	while(1){}
}
