/**
 * \file tester.c
 *
 * \brief Tester application
 *
 * Copyright (c) 2012-2018 Microchip Technology Inc. and its subsidiaries.
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
 *
 */

/**
 * \mainpage
 * \section preface Preface
 * This is the reference manual for the LWMesh Tester Application
 * //TODO
 */

/*- Includes ---------------------------------------------------------------*/
#include "config.h"
#include "phy.h"
#include "sys.h"
#include "nwk.h"
#if SAMD || SAMR21
#include "system.h"
#else
#include "led.h"
#include "sysclk.h"
#endif
#include "tester.h"
#include "commands.h"
#include "filter.h"
#include "events.h"
#include "time.h"
#include "sio2host.h"
# include "asf.h"

/*- Types ------------------------------------------------------------------*/
typedef enum AppState_t {
	APP_STATE_INITIAL,
	APP_STATE_IDLE,
} AppState_t;

typedef enum AppUartState_t {
	APP_UART_STATE_IDLE,
	APP_UART_STATE_READ_SIZE_1,
	APP_UART_STATE_READ_SIZE_2,
	APP_UART_STATE_READ_DATA,
	APP_UART_STATE_READ_CRC_1,
	APP_UART_STATE_READ_CRC_2,
} AppUartState_t;

/*- Prototypes -------------------------------------------------------------*/

static void Serial_Handler(void);

/*- Variables --------------------------------------------------------------*/
static AppState_t appState = APP_STATE_INITIAL;
static AppUartState_t appUartState = APP_UART_STATE_IDLE;
static uint8_t appUartBuffer[APP_UART_BUFFER_SIZE];
static uint16_t appUartBufferPtr;
static uint16_t appUartBufferCrc;
static uint16_t appUartSize;
static uint16_t appUartCrc;
static uint8_t sio_rx_length;
static uint8_t rx_data[APP_RX_BUF_SIZE];

/*- Implementations --------------------------------------------------------*/

/*************************************************************************//**
*****************************************************************************/
static uint16_t appCrcCcittUpdate(uint16_t crc, uint8_t byte)
{
	byte ^= (byte ^ (crc & 0xff)) << 4;
	crc = (((uint16_t)byte << 8) | ((crc >> 8) & 0xff)) ^
			(uint8_t)(byte >> 4) ^ ((uint16_t)byte << 3);
	return crc;
}

/*************************************************************************//**
*****************************************************************************/
void appUartSendCommand(uint8_t *buf, uint16_t size)
{
	uint16_t crc = 0xffff;

	sio2host_putchar(APP_UART_START_SYMBOL);
	sio2host_putchar(size & 0xff);
	sio2host_putchar(size >> 8);

	for (uint16_t i = 0; i < size; i++) {
		sio2host_putchar(buf[i]);
		crc = appCrcCcittUpdate(crc, buf[i]);
	}

	sio2host_putchar(crc & 0xff);
	sio2host_putchar(crc >> 8);
}

/*************************************************************************//**
*****************************************************************************/
static void appUartSendAck(AppStatus_t status)
{
	AppCommandAck_t ack;

	ack.id = APP_COMMAND_ACK;
	ack.status = status;
	ack.time = appTimeGet();

	appUartSendCommand((uint8_t *)&ack, sizeof(ack));
}

/*************************************************************************//**
*****************************************************************************/
static void appUartStateMachine(uint8_t byte)
{
	switch (appUartState) {
	case APP_UART_STATE_IDLE:
	{
		if (APP_UART_START_SYMBOL == byte) {
			appUartState = APP_UART_STATE_READ_SIZE_1;
		}
	}
	break;

	case APP_UART_STATE_READ_SIZE_1:
	{
		appUartSize = byte;
		appUartState = APP_UART_STATE_READ_SIZE_2;
	}
	break;

	case APP_UART_STATE_READ_SIZE_2:
	{
		appUartSize |= (uint16_t)byte << 8;
		appUartBufferPtr = 0;
		appUartBufferCrc = 0xffff;
		appUartState = APP_UART_STATE_READ_DATA;
	}
	break;

	case APP_UART_STATE_READ_DATA:
	{
		appUartBuffer[appUartBufferPtr++] = byte;
		appUartBufferCrc = appCrcCcittUpdate(appUartBufferCrc, byte);

		if (appUartBufferPtr == appUartSize) {
			appUartState = APP_UART_STATE_READ_CRC_1;
		}
	}
	break;

	case APP_UART_STATE_READ_CRC_1:
	{
		appUartCrc = byte;
		appUartState = APP_UART_STATE_READ_CRC_2;
	}
	break;

	case APP_UART_STATE_READ_CRC_2:
	{
		appUartCrc |= (uint16_t)byte << 8;

		if (appUartCrc == appUartBufferCrc) {
			appUartSendAck(appCommandReceived(appUartBuffer,
					appUartSize));
		} else {
			appUartSendAck(APP_STATUS_INVALID_CRC);
		}

		appUartState = APP_UART_STATE_IDLE;
	}
	break;
	}
}

/*************************************************************************//**
*****************************************************************************/
void Serial_Handler(void)
{
	sio_rx_length = sio2host_rx(rx_data, APP_RX_BUF_SIZE);

	if (sio_rx_length) {
		for (uint16_t i = 0; i < sio_rx_length; i++) {
			appUartStateMachine(rx_data[i]);
		}
	}
}

/*************************************************************************//**
*****************************************************************************/
static void APP_TaskHandler(void)
{
	switch (appState) {
	case APP_STATE_INITIAL:
	{
		appCommandsInit();
    #ifdef NWK_ENABLE_ADDRESS_FILTER
		appFilterInit();
    #endif
    #ifdef APP_ENABLE_EVENTS_BUFFER
		appEventsInit();
    #endif
		appState = APP_STATE_IDLE;
	}
	break;

	case APP_STATE_IDLE:
	{
	}
	break;
	}
}

/*************************************************************************//**
*****************************************************************************/
int main(void)
{
	irq_initialize_vectors();
	#if SAMD || SAMR21
	system_init();
	delay_init();
	#else
	sysclk_init();
	board_init();
	#endif
	SYS_Init();
	appTimeInit();
	sio2host_init();
	cpu_irq_enable();
	LED_On(LED0);
	while (1) {
		SYS_TaskHandler();
		APP_TaskHandler();
		Serial_Handler();
	}
}
