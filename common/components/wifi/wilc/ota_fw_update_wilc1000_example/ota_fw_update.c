/**
 *
 * \file
 *
 * \brief OTA Firmware update Example Implementation.
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
#include "ota_fw_update.h"
#include "FreeRTOS.h"
#include "iot/http/http_client.h"
//#include "osprintf.h"
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

#define SUCCESS 0
#define FAILURE -1

/** File download processing state. */
static download_state down_state = NOT_READY;
/** Http content length. */
static uint32_t http_file_size = 0;
/** Receiving content length. */
static uint32_t received_file_size = 0;

uint8 g_page_buffer[IFLASH1_PAGE_SIZE];
static uint32_t g_page_offset = 0;
static uint32_t g_page_addr = IFLASH1_ADDR;

/** Instance of Timer module. */
struct sw_timer_module swt_module_inst;

/** Instance of HTTP client module. */
struct http_client_module http_client_module_inst;

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
 * \brief Lock all sectors in flash1
 */
int lock_all_sectors(){
	uint32_t ul_rc;
	uint ul_sector_addr = IFLASH1_ADDR;
	/* lock smaller sectors of sector0 */
	for(int count = 0; count < SMALLERSECTOR_COUNT ; count++){
		ul_rc = flash_lock(ul_sector_addr, ul_sector_addr + SECT0_SMALLERSECTOR_SIZE - 1, 0, 0);
		if (ul_rc != FLASH_RC_OK) {
			printf("-F- Flash locking error %lu\n\r", ul_rc);
			return FAILURE;
		}
		ul_sector_addr += SECT0_SMALLERSECTOR_SIZE;
	}
	/* lock larger sector of sector0 */
	ul_rc = flash_lock(ul_sector_addr, ul_sector_addr + SECT0_LARGERSECTOR_SIZE - 1, 0, 0);
	if (ul_rc != FLASH_RC_OK) {
		printf("-F- Flash locking error %lu\n\r", ul_rc);
		return FAILURE;
	}
	ul_sector_addr += SECT0_LARGERSECTOR_SIZE;
	/* lock sector1 to sector15 */
	for(int count = 0; count < NO_OF_SECTORS -1 ; count++){	
		ul_rc = flash_lock(ul_sector_addr, ul_sector_addr + SECTOR_SIZE - 1, 0, 0);
		if (ul_rc != FLASH_RC_OK) {
			printf("-F- Flash locking error %lu\n\r", ul_rc);
			return FAILURE;
		}
		ul_sector_addr += SECTOR_SIZE;
	}
	return SUCCESS;
}


/**
 * \brief Unlock all sectors in flash1
 */
int unlock_all_sectors(){
	uint32_t ul_rc;
	uint ul_sector_addr = IFLASH1_ADDR;
	
	/* Unlock smaller sectors of sector0 */
	for(int count = 0; count < SMALLERSECTOR_COUNT ; count++){
		ul_rc = flash_unlock(ul_sector_addr, ul_sector_addr + SECT0_SMALLERSECTOR_SIZE - 1, 0, 0);
		if (ul_rc != FLASH_RC_OK) {
			printf("-F- Unlock error %lu\n\r", ul_rc);
			return FAILURE;
		}
		ul_sector_addr += SECT0_SMALLERSECTOR_SIZE;
	}
	/* Unlock larger sector of sector0 */
	ul_rc = flash_unlock(ul_sector_addr, ul_sector_addr + SECT0_LARGERSECTOR_SIZE - 1, 0, 0);
	if (ul_rc != FLASH_RC_OK) {
		printf("-F- Unlock error %lu\n\r", ul_rc);
		return FAILURE;
	}
	ul_sector_addr += SECT0_LARGERSECTOR_SIZE;
	
	/* unlock sector1 to sector15 */		
	for(int count = 0; count < NO_OF_SECTORS - 1 ; count++){
		/* Unlock sector */
		ul_rc = flash_unlock(ul_sector_addr, ul_sector_addr + SECTOR_SIZE - 1, 0, 0);
		if (ul_rc != FLASH_RC_OK) {
			printf("-F- Unlock error %lu\n\r", ul_rc);
			return FAILURE;
		}
		ul_sector_addr += SECTOR_SIZE;
	}
	printf("Flash1 unlocked successfully\r\n");
	return SUCCESS;
}

/**
 * \brief erase all sectors in flash1
 */
int erase_all_sectors(){
	uint32_t ul_rc;
	uint ul_sector_addr = IFLASH1_ADDR;
	/* Erase smaller sectors of sector0 */
	for(int count = 0; count < SMALLERSECTOR_COUNT ; count++){
		ul_rc = flash_erase_sector(ul_sector_addr);
		if (ul_rc != FLASH_RC_OK) {
			printf("-F- Flash programming erase error %lu in small0 sect\n\r", ul_rc);
			return FAILURE;
		}
		ul_sector_addr += SECT0_SMALLERSECTOR_SIZE;
	}
	/* Erase larger sector of sector0 */
	ul_rc = flash_erase_sector(ul_sector_addr);
	if (ul_rc != FLASH_RC_OK) {
		printf("-F- Flash programming erase error %lu in small1 sect\n\r", ul_rc);
		return FAILURE;
	}
	ul_sector_addr += SECT0_LARGERSECTOR_SIZE;
			
	/* Erase sector1 to sector15 */		
	for(int count = 0; count < NO_OF_SECTORS - 1 ; count++){
		/* Erase sector */
		ul_rc = flash_erase_sector(ul_sector_addr);
		if (ul_rc != FLASH_RC_OK) {
			printf("-F- Flash programming erase error  %lu in sector %d\n\r", ul_rc,count+1);
			return FAILURE;
		}
		ul_sector_addr += SECTOR_SIZE;
	}
	printf("Flash1 erased successfully\r\n");
	return SUCCESS;
}

/**
 * \brief Write data to Flash; page by page
 * \param[in] data  Chunk of bin file.
 * \param[in] length  Chunk length.
 * \param[in] is_last_chunk Last chunk of file, Indication to write all remaining data.
 */
int write_to_flash(char *data, uint32_t length, uint16_t is_last_chunk){
	uint32_t data_offset = 0;
	while(length > 0){
		if(g_page_offset + length >= IFLASH1_PAGE_SIZE){
			/* Arrange data in the size of flash page */
			memcpy(g_page_buffer + g_page_offset, data + data_offset, IFLASH1_PAGE_SIZE - g_page_offset);
			length -= (IFLASH1_PAGE_SIZE - g_page_offset);
			data_offset += (IFLASH1_PAGE_SIZE - g_page_offset);
			/* Write to a page in flash */
			uint32_t ul_rc = flash_write(g_page_addr, g_page_buffer, IFLASH1_PAGE_SIZE, 0);
			if (ul_rc != FLASH_RC_OK) {
				printf("-F- Flash programming write error %lu\n\r", ul_rc);
				return FAILURE;
			}
			g_page_addr += IFLASH1_PAGE_SIZE;  // Next page
			g_page_offset = 0;
		}
		else{
			/* Store the remaining data */
			memcpy(g_page_buffer + g_page_offset, data + data_offset, length);
			if(is_last_chunk){
				/* It is the last chunk, so write the remaining data to flash */
				uint32_t ul_rc = flash_write(g_page_addr, g_page_buffer, g_page_offset + length, 0);
				if (ul_rc != FLASH_RC_OK) {
					printf("-F- Flash programming write error %lu\n\r", ul_rc);
					return FAILURE;
				}
			}
			g_page_offset += length;
			length = 0;
		}		
	}

	return SUCCESS;
}

/**
 * \brief Start file download via HTTP connection.
 */
static void start_download(void)
{
	if (!is_state_set(WIFI_CONNECTED)) {
		printf("start_download: Wi-Fi is not connected.\r\n");
		return;
	}

	if (is_state_set(GET_REQUESTED)) {
		printf("start_download: request is sent already.\r\n");
		return;
	}

	if (is_state_set(DOWNLOADING)) {
		printf("start_download: running download already.\r\n");
		return;
	}
	int ret;
	/* Send the HTTP request. */
	printf("Sending HTTP request...\r\n");
	ret = http_client_send_request(&http_client_module_inst, MAIN_HTTP_FILE_URL, HTTP_METHOD_GET, NULL, NULL);
	if(ret != 0){
		printf("HTTP request send failed err %d\r\n",ret);
		return;
	}
	
	/* Receive the response*/
	printf("Receiving response...\r\n");
	while(!(is_state_set(COMPLETED) || is_state_set(CANCELED))){
		http_client_recv_response(&http_client_module_inst);
	
	}	
}

/**
 * \brief Write the received chunk to Flash
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
		printf("Received %lu\r",received_file_size);
				
		if (received_file_size >= http_file_size) {
			printf("Received %lu File size %lu\r\n",received_file_size,http_file_size);
			printf("store_file_packet: file downloaded successfully.\r\n");
			if(write_to_flash(data, length,1) != SUCCESS){
				printf("Failed to write to flash!\r\n");
				//lock_all_sectors();
				add_state(CANCELED);
				return;
			}
			printf("Flash write done\r\n");
			/* By default sectors are not locked, so not locking */
			//lock_all_sectors(); /* if locked, not able to program again using Atmel Studio*/
			add_state(COMPLETED);
			printf("Switching flash!\r\nReboot board\r\n");
			if(flash_is_gpnvm_set(2)){
				if(flash_clear_gpnvm(2)!=FLASH_RC_OK){
					printf("clear GPNVM failed\r\n");
				}
			}else{
				if(flash_set_gpnvm(2) != FLASH_RC_OK){
					printf("set GPNVM failed\r\n");
				} 
			}			
			
			return;
		}
		
		if(write_to_flash(data, length,0) != SUCCESS){
			printf("Failed to write to flash!!\r\n");
			//lock_all_sectors();
			add_state(CANCELED);
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
		printf("http_client_callback: HTTP client socket connected.\r\n");
		break;

	case HTTP_CLIENT_CALLBACK_REQUESTED:
		printf("http_client_callback: request completed.\r\n");
		add_state(GET_REQUESTED);
		break;

	case HTTP_CLIENT_CALLBACK_RECV_RESPONSE:
		printf("http_client_callback: received HTTP response %u data size %u\r\n",
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
		printf("http_client_callback: disconnection reason:%d\r\n", data->disconnected.reason);

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
				printf("Wi-Fi connected\r\n");
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
				printf("wifi_cb: reconnecting...\r\n");
				net_interface_down(NET_IF_STA);
				/* Connect to defined AP. */
				os_m2m_wifi_connect((char *)MAIN_WLAN_SSID, sizeof(MAIN_WLAN_SSID), MAIN_WLAN_AUTH, (void *)MAIN_WLAN_PSK, M2M_WIFI_CH_ALL);
			}
			break;
		}

		case NET_IF_REQ_DHCP_CONF:
		{
			tstrM2MIPConfig2 *strIpConfig = pvMsg;
			uint16_t *a = (void *)strIpConfig->u8StaticIPv6;
			printf("wifi_cb: IPv4 addr: %d.%d.%d.%d\r\n", strIpConfig->u8StaticIP[0], strIpConfig->u8StaticIP[1],
			strIpConfig->u8StaticIP[2], strIpConfig->u8StaticIP[3]);
			printf("wifi_cb: IPv6 addr: %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\r\n",
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
		printf("configure_http_client: HTTP client initialization failed! (res %d)\r\n", ret);
		while (1) {} /* Loop forever. */
	}

	http_client_register_callback(&http_client_module_inst, http_client_callback);
}



/**
 * \brief ota firmware update function.
 */
void ota_firmware_update(void *argument)
{
	uint32_t ul_rc;

	/*initialize download state as NOT_READY*/
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
		printf("main: m2m_wifi_init call error! (res %d)\r\n", ret);
		while (1) {
		}
	}

	/* Connect to router. */
	printf("connecting to WiFi AP %s...\r\n", (char *)MAIN_WLAN_SSID);
	os_m2m_wifi_connect((char *)MAIN_WLAN_SSID, sizeof(MAIN_WLAN_SSID), MAIN_WLAN_AUTH, (void *)MAIN_WLAN_PSK, M2M_WIFI_CH_ALL);
	/*wait for wifi connection*/
	while(!is_state_set(WIFI_CONNECTED)){
		vTaskDelay(20);
	}
	
	/*Initialize flash service*/
	ul_rc = flash_init(FLASH_ACCESS_MODE_128, 6);
	if (ul_rc != FLASH_RC_OK) {
		printf("-F- Initialization error %lu\n\r", ul_rc);
		return;
	}
	if(flash_is_locked( IFLASH1_ADDR, IFLASH1_ADDR + (NO_OF_SECTORS * SECTOR_SIZE) - 1)){
		/*Unlock flash1 to erase and write */
		if (unlock_all_sectors() != SUCCESS) {
			printf("-F- Unlock error %lu\n\r", ul_rc);
			return;
		}	
	}
	/*Erase flash1 to write later*/
	if (erase_all_sectors() != SUCCESS) {
		printf("-F- Erase error %lu\n\r", ul_rc);
		return;
	}
	/*start downloading*/
	start_download();
		
	while(1){}
}
