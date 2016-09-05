/**
 *
 * \file
 *
 * \brief WINC1500 packet monitoring example.
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
 * board to monitor all wireless data packets.<br>
 * It uses the following hardware:
 * - the SAM Xplained Pro.
 * - the WINC1500 on EXT1.
 *
 * \section files Main Files
 * - main.c : Initialize the WINC1500, configure to monitoring parameters and start
 * the packet monitoring.
 *
 * \section usage Usage
 * -# Please set the below MAC address in the main.c that you want to monitor.
 * \code
 *    static const uint8_t SRC_MAC_ADDR[6]={0x78, 0xF7, 0xBE, 0xFA, 0x92, 0x5A};
 *    static const uint8_t DST_MAC_ADDR[6]={0x78, 0xF7, 0xBE, 0xFA, 0x92, 0x5A};
 * \endcode
 *
 * -# Activate one of the following codes in the start_packet_monitoring function.
 * \code
 *    memcpy(wifi_monitor_conf.au8SrcMacAddress, SRC_MAC_ADDR, sizeof(SRC_MAC_ADDR));
 *    memcpy(wifi_monitor_conf.au8DstMacAddress, DST_MAC_ADDR, sizeof(DST_MAC_ADDR));
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
 *    -- WINC1500 packet monitoring example --
 *    -- SAMD21_XPLAINED_PRO --
 *    -- Compiled: xxx xx xxxx xx:xx:xx --
 *    Packet monitoring started.
 *    ------------
 *    FrameType : 0xFF
 *    FrameSubtype : 0xFF
 *    Source MAC address : 00:00:00:00:00:00
 *    Destination MAC address : 78:F7:BE:FA:92:5A
 *    ------------
 *    monitoring_cb() MONITOR PACKET u8FrameType:0x00, u8FrameSubtype:0xD0
 *    monitoring_cb() SRC Mac address : 00:26:66:1A:08:5C
 *    monitoring_cb() DST Mac address : 78:F7:BE:FA:92:5A
 *    monitoring_cb() MONITOR PACKET u8FrameType:0x00, u8FrameSubtype:0x50
 *    monitoring_cb() SRC Mac address : 00:26:66:C6:00:2A
 *    monitoring_cb() DST Mac address : 78:F7:BE:FA:92:5A
 *    monitoring_cb() MONITOR PACKET u8FrameType:0x08, u8FrameSubtype:0x88
 *    monitoring_cb() SRC Mac address : 00:26:66:1A:08:5D
 *    monitoring_cb() DST Mac address : 78:F7:BE:FA:92:5A
 *    ...
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
#include "stdio_serial.h"
#include <string.h>
#include "driver/include/m2m_wifi.h"
#include "socket/include/socket.h"

/** Maximum size for packet buffer. */
#define MAIN_BUFFER_MAX_SIZE                 (1024)

/** End of line. */
#define STRING_EOL                      "\r\n"
/** Example header. */
#define STRING_HEADER                   "-- WINC1500 packet monitoring example --"STRING_EOL \
	"-- "BOARD_NAME " --"STRING_EOL	\
	"-- Compiled: "__DATE__ " "__TIME__ " --"STRING_EOL

/** Source MAC address for monitoring. */
static const uint8_t SRC_MAC_ADDR[6] = {0xa4, 0xeb, 0xd3, 0xfc, 0x9f, 0x0d};
/** Destination MAC address for monitoring. */
static const uint8_t DST_MAC_ADDR[6] = {0xa4, 0xeb, 0xd3, 0xfc, 0x9f, 0x0d};
/** Packet monitoring state flag. */
static bool monitoring_enabled = false;
/** Configuration for packet monitoring. */
static tstrM2MWifiMonitorModeCtrl wifi_monitor_conf = {0};
/** Packet buffer for monitoring. */
static uint8_t payload_buffer[MAIN_BUFFER_MAX_SIZE];

/**
 * \brief Start the packet monitoring.
 */
void start_packet_monitoring(void);

/**
 * \brief Stop the packet monitoring.
 */
void stop_packet_monitoring(void);

/**
 * \brief Wi-Fi monitoring callback function.
 *	\param[in] pstrWifiRxPacket
 *	Pointer to a structure holding the Wi-Fi packet header parameters.
 *	\param[in] pu8Payload
 *	Buffer holding the Wi-Fi packet payload information required by the application starting from the
 *	defined OFFSET by the application (when calling m2m_wifi_enable_monitoring_mode). It shall be NULL
 *	if the application does not need any data from the payload.
 *	\param[in]	u16PayloadSize
 *	The size of the payload in bytes. It cannot exceed the buffer size given
 *	through m2m_wifi_enable_monitoring_mode.
 */
void monitoring_cb(tstrM2MWifiRxPacketInfo *pstrWifiRxPacket, uint8_t *pu8Payload, uint16 u16PayloadSize);

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

void start_packet_monitoring()
{
	if (!monitoring_enabled) {
		wifi_monitor_conf.u8ChannelID    = M2M_WIFI_CH_1;
		wifi_monitor_conf.u8FrameType    = M2M_WIFI_FRAME_TYPE_ANY;
		wifi_monitor_conf.u8FrameSubtype = M2M_WIFI_FRAME_SUB_TYPE_ANY;
		/* memcpy(wifi_monitor_conf.au8SrcMacAddress, SRC_MAC_ADDR, sizeof(SRC_MAC_ADDR)); */
		memcpy(wifi_monitor_conf.au8DstMacAddress, DST_MAC_ADDR, sizeof(DST_MAC_ADDR));

		m2m_wifi_enable_monitoring_mode(&wifi_monitor_conf, payload_buffer, sizeof(payload_buffer), 0);
		monitoring_enabled = true;
		printf("Packet monitoring started.\r\n");
		printf("------------\r\n");
		printf("FrameType : 0x%02X\r\n", wifi_monitor_conf.u8FrameType);
		printf("FrameSubtype : 0x%02X\r\n", wifi_monitor_conf.u8FrameSubtype);
		printf("Source MAC address : %02X:%02X:%02X:%02X:%02X:%02X\r\n",
				wifi_monitor_conf.au8SrcMacAddress[0], wifi_monitor_conf.au8SrcMacAddress[1], wifi_monitor_conf.au8SrcMacAddress[2],
				wifi_monitor_conf.au8SrcMacAddress[3], wifi_monitor_conf.au8SrcMacAddress[4], wifi_monitor_conf.au8SrcMacAddress[5]);
		printf("Destination MAC address : %02X:%02X:%02X:%02X:%02X:%02X\r\n",
				wifi_monitor_conf.au8DstMacAddress[0], wifi_monitor_conf.au8DstMacAddress[1], wifi_monitor_conf.au8DstMacAddress[2],
				wifi_monitor_conf.au8DstMacAddress[3], wifi_monitor_conf.au8DstMacAddress[4], wifi_monitor_conf.au8DstMacAddress[5]);
		printf("------------\r\n");
	}
}

void stop_packet_monitoring()
{
	if (monitoring_enabled) {
		m2m_wifi_disable_monitoring_mode();
		monitoring_enabled = false;
		printf("Packet monitoring stopped.\r\n");
	}
}

void monitoring_cb(tstrM2MWifiRxPacketInfo *pstrWifiRxPacket, uint8 *pu8Payload, uint16 u16PayloadSize)
{
	if ((pstrWifiRxPacket != NULL) && (u16PayloadSize != 0)) {
		printf("monitoring_cb() MONITOR PACKET u8FrameType:0x%02X, u8FrameSubtype:0x%02X\r\n", pstrWifiRxPacket->u8FrameType, pstrWifiRxPacket->u8FrameSubtype);
		printf("monitoring_cb() SRC Mac address : %02X:%02X:%02X:%02X:%02X:%02X\r\n",
				pstrWifiRxPacket->au8SrcMacAddress[0], pstrWifiRxPacket->au8SrcMacAddress[1], pstrWifiRxPacket->au8SrcMacAddress[2],
				pstrWifiRxPacket->au8SrcMacAddress[3], pstrWifiRxPacket->au8SrcMacAddress[4], pstrWifiRxPacket->au8SrcMacAddress[5]);
		printf("monitoring_cb() DST Mac address : %02X:%02X:%02X:%02X:%02X:%02X\r\n",
				pstrWifiRxPacket->au8DstMacAddress[0], pstrWifiRxPacket->au8DstMacAddress[1], pstrWifiRxPacket->au8DstMacAddress[2],
				pstrWifiRxPacket->au8DstMacAddress[3], pstrWifiRxPacket->au8DstMacAddress[4], pstrWifiRxPacket->au8DstMacAddress[5]);
	}
}

/**
 * \brief Main application function.
 *
 * Application entry point.
 * Initialize board and WINC1500 Wi-Fi module.
 *
 * \return program return value.
 */
int main(void)
{
	tstrWifiInitParam param = {0};
	int8_t ret;

	/* Initialize variables. */
	monitoring_enabled = false;

	/* Initialize monitoring configuration parameters. */
	memset(&wifi_monitor_conf, 0, sizeof(tstrM2MWifiMonitorModeCtrl));

	/* Initialize Wi-Fi parameters structure. */
	param.pfAppMonCb = monitoring_cb; /* Register monitoring callback function. */

	/* Initialize the board. */
	sysclk_init();
	board_init();

	/* Initialize the UART console. */
	configure_console();
	printf(STRING_HEADER);

	/* Initialize the BSP. */
	nm_bsp_init();

	/* Initialize Wi-Fi driver with data and status callbacks. */
	ret = m2m_wifi_init(&param);
	if (M2M_SUCCESS != ret) {
		printf("main: m2m_wifi_init call error!(%d)\r\n", ret);
		while (1) {
		}
	}

	/* Start the packet monitoring. */
	start_packet_monitoring();

	while (1) {
		/* Handle pending events from network controller. */
		m2m_wifi_handle_events(NULL);
	}

	return 0;
}
