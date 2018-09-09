/**
 * \file EdDemo.c
 *
 * \brief Energy Detection Demo application implementation
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
 *
 */

/**
 * \mainpage
 * \section preface Preface
 * This is the reference manual for the EDDemo  Application
 * The Lightweight Mesh EDDemo application demonstrates the energy detection(ED) feature of Atmel Transceivers and helps analysing the energy on all channels .
 * This application requires only one node for demonstration.
 * No Data communication is performed in this application and no networking API's are used.
 * A terminal needs to be connected to the node .
 * The node measures the energy values on all channels periodically and prints the results on the connected terminal.
 */
/*- Includes ---------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "phy.h"
#include "sys.h"
#include "nwk.h"
#include "sysTimer.h"
#include "sio2host.h"
#if SAMD || SAMR21
#include "system.h"
#else
#include "led.h"
#include "sysclk.h"
#endif
#include "asf.h"

/*- Types ------------------------------------------------------------------*/
typedef enum AppState_t {
	APP_STATE_INITIAL,
	APP_STATE_MEASURE_ED,
	APP_STATE_WAIT_SCAN_TIMER,
} AppState_t;

/*- Variables --------------------------------------------------------------*/
static AppState_t appState = APP_STATE_INITIAL;
static uint8_t appEdValue[APP_LAST_CHANNEL - APP_FIRST_CHANNEL + 1];
static SYS_Timer_t appScanTimer;

/*- Implementations --------------------------------------------------------*/

/*************************************************************************//**
*****************************************************************************/
static void appPrintEdValues(void)
{
	char hex[] = "0123456789abcdef";

	for (uint8_t i = 0; i < sizeof(appEdValue); i++) {
		uint8_t v = appEdValue[i] - PHY_RSSI_BASE_VAL;
		sio2host_putchar(hex[(v >> 4) & 0x0f]);
		sio2host_putchar(hex[v & 0x0f]);
		sio2host_putchar(' ');
	}

	sio2host_putchar('\r');
	sio2host_putchar('\n');
}

/*************************************************************************//**
*****************************************************************************/
static void appScanTimerHandler(SYS_Timer_t *timer)
{
	appState = APP_STATE_MEASURE_ED;
	(void)timer;
}

/*************************************************************************//**
*****************************************************************************/
static void appInit(void)
{
	PHY_SetRxState(true);

	appScanTimer.interval = APP_SCAN_INTERVAL;
	appScanTimer.mode = SYS_TIMER_PERIODIC_MODE;
	appScanTimer.handler = appScanTimerHandler;
	SYS_TimerStart(&appScanTimer);

	appState = APP_STATE_MEASURE_ED;
}

/*************************************************************************//**
*****************************************************************************/
static void APP_TaskHandler(void)
{
	switch (appState) {
	case APP_STATE_INITIAL:
	{
		appInit();
	}
	break;

	case APP_STATE_MEASURE_ED:
	{
		for (uint8_t i = 0; i < sizeof(appEdValue); i++) {
			PHY_SetChannel(APP_FIRST_CHANNEL + i);
			appEdValue[i] = PHY_EdReq();
		}

		appPrintEdValues();

		appState = APP_STATE_WAIT_SCAN_TIMER;
	}
	break;

	default:
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
	sio2host_init();
	cpu_irq_enable();
	LED_On(LED0);
	while (1) {
		SYS_TaskHandler();
		APP_TaskHandler();
	}
}
