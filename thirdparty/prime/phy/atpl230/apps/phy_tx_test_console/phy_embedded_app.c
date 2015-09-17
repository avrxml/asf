/**
 * \file
 *
 * \brief ATMEL PLC PHY TX Test Console Application
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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

/* System includes */
#include <stdio.h>
#include "string.h"

/* ASF includes */
#include "asf.h"

#include "conf_pplc_if.h"

/* App includes */
#include "conf_example.h"
#include "phy_embedded_app.h"

/* Phy includes */
#include "atpl230.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

/* Headers len = 32 = 2 * PHY_DMA_OFFSET worst case (prime1.4 compatible)*/

/* 776 = 8 * 97 -> In noise mode: num symbols in rx noise mode: 8 and 97
 * bytes/symbol */
static uint8_t uc_data_buf[800];    /*  Receive working buffer */

/* Tasks handlers */
xTaskHandle xappPhyHand;
xTaskHandle xappTxResHand;
xTaskHandle xappRxHand;
xTaskHandle xappConsoleHand;

/* TX configuration parameters */
static txPhyEmbeddedConfig_t xTxPhyCfg;
static uint8_t sna[6] = {0x00, 0x80, 0xE1, 0x00, 0x00, 0x6F};

/* check reset on atpl230 modem */
#define APP_CHECK_RESET_VALUE           0xA500005A
#define APP_CHECK_RESET_PERIOD          5

/**
 * \brief Reset internal PLC Modem.
 *
 */
static void _reset_plc_modem(void)
{
	uint32_t ul_delay = 0x00FFFFFF;

#if BOARD == SAM4CP16BMB
	/* Reset on ARST of modem PLC */
	ioport_set_pin_level(PPLC_ARST_GPIO, PPLC_ARST_ACTIVE_LEVEL);
	/* Clear ARST of modem PLC */
	ioport_set_pin_level(PPLC_ARST_GPIO, PPLC_ARST_INACTIVE_LEVEL);
#elif  BOARD == ATPL230AMB /* Reset on ARST of modem PLC */
	gpio_set_pin_low(PPLC_ARST_GPIO);
	/* Clear ARST of modem PLC */
	gpio_set_pin_high(PPLC_ARST_GPIO);
#else
	;
#endif

	/* Wait to initialize system */
	while (ul_delay) {
		ul_delay--;
	}
}

static void prvAppPhyTask(void *pvParameters)
{
	static portTickType xLastWakeTime;
	static portTickType xPeriod;
	uint8_t uc_choice;
	uint32_t ul_gpbr_value;
	uint32_t ul_check_reset_value;

	UNUSED(pvParameters);

	phy_set_cfg_param(PHY_ID_CFG_COUPLING_BOARD, &xTxPhyCfg.uc_coupling, 1);
	phy_set_cfg_param(PHY_ID_CFG_TXRX_CHANNEL, &xTxPhyCfg.uc_channel, 1);
	if (xTxPhyCfg.uc_autodetect) {
		phy_set_cfg_param(PHY_ID_CFG_AUTODETECT_BRANCH, &xTxPhyCfg.uc_autodetect, 1);
		xTxPhyCfg.uc_impedance = HI_STATE;
		phy_set_cfg_param(PHY_ID_CFG_IMPEDANCE, &xTxPhyCfg.uc_impedance, 1);
	} else {
		phy_set_cfg_param(PHY_ID_CFG_IMPEDANCE, &xTxPhyCfg.uc_impedance, 1);
		xTxPhyCfg.uc_autodetect = false;
		phy_set_cfg_param(PHY_ID_CFG_AUTODETECT_BRANCH, &xTxPhyCfg.uc_autodetect, 1);
	}

	/* Init RESET check validation value */
	ul_check_reset_value = APP_CHECK_RESET_VALUE;
	phy_set_cfg_param(0xFFAC, &ul_check_reset_value, sizeof(ul_check_reset_value));

	xPeriod = xTxPhyCfg.ul_tx_period / portTICK_RATE_MS;
	xLastWakeTime = xTaskGetTickCount();
	for (;;) {
		vTaskDelayUntil(&xLastWakeTime, xPeriod);

		/* Check Reset config value */
		phy_get_cfg_param(0xFFAC, &ul_check_reset_value, sizeof(ul_check_reset_value));
		if (ul_check_reset_value != APP_CHECK_RESET_VALUE) {
			_reset_plc_modem();
			/* reset phy layer */
			phy_reset(PHY_RESET_SOFT_TYPE);
			/* restore tx configuration */
			phy_mac_crc_disable();
			phy_set_cfg_param(PHY_ID_CFG_COUPLING_BOARD, &xTxPhyCfg.uc_coupling, 1);
			phy_set_cfg_param(PHY_ID_CFG_TXRX_CHANNEL, &xTxPhyCfg.uc_channel, 1);
			if (xTxPhyCfg.uc_autodetect) {
				phy_set_cfg_param(PHY_ID_CFG_AUTODETECT_BRANCH, &xTxPhyCfg.uc_autodetect, 1);
				xTxPhyCfg.uc_impedance = HI_STATE;
				phy_set_cfg_param(PHY_ID_CFG_IMPEDANCE, &xTxPhyCfg.uc_impedance, 1);
			} else {
				phy_set_cfg_param(PHY_ID_CFG_IMPEDANCE, &xTxPhyCfg.uc_impedance, 1);
				xTxPhyCfg.uc_autodetect = false;
				phy_set_cfg_param(PHY_ID_CFG_AUTODETECT_BRANCH, &xTxPhyCfg.uc_autodetect, 1);
			}

			/* config reset detect value */
			ul_check_reset_value = APP_CHECK_RESET_VALUE;
			phy_set_cfg_param(0xFFAC, &ul_check_reset_value, sizeof(ul_check_reset_value));
		}

		if (uart_read(CONSOLE_UART, &uc_choice)) {
			if ((uc_choice == 'x') || (uc_choice == 'X')) {
				ul_gpbr_value = gpbr_read(GPBR0);
				ul_gpbr_value &= 0xFFFFFFF0;
				ul_gpbr_value |= PHY_APP_CMD_MENU_START_MODE;
				gpbr_write(GPBR0, ul_gpbr_value);
				rstc_start_software_reset(RSTC);
			}
		}

		/* Restart watchdog */
		wdt_restart(WDT);
		phy_tx_frame(&xTxPhyCfg.xPhyMsg);
		printf("->Send message\r\n");
		fflush(stdout);
	}
}

static void prvGetTxResultTask(void *pvParameters)
{
	static portTickType xLastWakeTime;
	static portTickType xPeriod;
	xPhyMsgTxResult_t x_write_result;

	UNUSED(pvParameters);

	xPeriod = (xTxPhyCfg.ul_tx_period + 100) / portTICK_RATE_MS;
	/* xPeriod = PRIME_APP_PHY_TIMER_RATE; */
	xLastWakeTime = xTaskGetTickCount();
	for (;;) {
		vTaskDelayUntil(&xLastWakeTime, xPeriod);

		phy_tx_frame_result_cb(&x_write_result);

		/* insert code to manage response */
		switch (x_write_result.uc_result) {
		case ATPL230_TXRXBUF_RESULT_SUCCESSFUL:
			printf("<-ATPL230_TXRXBUF_RESULT_SUCCESSFUL\r\n");
			break;

		case ATPL230_TXRXBUF_RESULT_INPROCESS:
			printf("<-ATPL230_TXRXBUF_RESULT_INPROCESS\r\n");
			break;

		case ATPL230_TXRXBUF_RESULT_WRONG_LEN:
			printf("<-ATPL230_TXRXBUF_RESULT_WRONG_LEN\r\n");
			break;

		case ATPL230_TXRXBUF_RESULT_BUSY_CHANNEL:
			printf("<-ATPL230_TXRXBUF_RESULT_BUSY_CHANNEL\r\n");
			break;

		case ATPL230_TXRXBUF_RESULT_PREV_TX_INPROCESS:
			printf("<-ATPL230_TXRXBUF_RESULT_PREV_TX_INPROCESS\r\n");
			break;

		case ATPL230_TXRXBUF_RESULT_RX_INPROCESS:
			printf("<-ATPL230_TXRXBUF_RESULT_RX_INPROCESS\r\n");
			break;

		case ATPL230_TXRXBUF_RESULT_INVALID_SCHEME:
			printf("<-ATPL230_TXRXBUF_RESULT_INVALID_SCHEME\r\n");
			break;

		case ATPL230_TXRXBUF_RESULT_TIMEOUT:
			printf("<-ATPL230_TXRXBUF_RESULT_TIMEOUT\r\n");
			break;

		default:
			printf("<-ERROR: NOT FOUND\r\n");
			break;
		}
	}
}

static void prvGetRxTask(void *pvparameters)
{
	static portTickType xLastWakeTime;
	static portTickType xPeriod;
	xPhyMsgRx_t x_read_msg;
	uint32_t ul_wait_counter;

	unused(pvparameters);

	/* initialize read msg structure */
	memset(&x_read_msg, 0, sizeof(xPhyMsgRx_t));
	/* set pointer to reception data buffer */
	x_read_msg.data_buf = uc_data_buf;

	xPeriod = PRIME_APP_PHY_TIMER_RATE;
	xLastWakeTime = xTaskGetTickCount();
	for (;;) {
		vTaskDelayUntil(&xLastWakeTime, xPeriod);

		/* block until get phy queue result */
		phy_rx_frame_cb(&x_read_msg);

		/* build response */
		if (x_read_msg.data_len) {
			/* blink Reception LED */
#if (BOARD != SAM4CMP_DB && BOARD != SAM4CMS_DB)
			LED_Toggle(LED1);
#endif
			ul_wait_counter = 0xFFFF;
			while (ul_wait_counter--) {
			}
#if (BOARD != SAM4CMP_DB && BOARD != SAM4CMS_DB)
			LED_Toggle(LED1);
#endif
		}
	}
}

void vPhyEmbeddedAppTask(txPhyEmbeddedConfig_t *xAppPhyCfgTx)
{
	phy_mac_crc_disable();
	phy_set_cfg_param(REG_ATPL230_SNA0, sna, sizeof(sna));

	/* Store tx configuration parameters */
	memcpy(&xTxPhyCfg, xAppPhyCfgTx, sizeof(txPhyEmbeddedConfig_t));

	/* Create App Phy task */
	xTaskCreate(prvAppPhyTask, (const signed char *const)"AppPhy",
			TASK_APP_PHY_STACK, NULL, TASK_APP_PHY_PRIO, &xappPhyHand);

	/* Create TX result management task */
	xTaskCreate(prvGetTxResultTask, (const signed char *const)"AppGetRes",
			TASK_APP_GET_TX_RESULT_STACK, NULL, TASK_APP_GET_TX_RESULT_PRIO, &xappTxResHand);

	/* Create RX data management task */
	xTaskCreate(prvGetRxTask, (const signed char *const)"AppGetRxData",
			TASK_APP_GET_RX_STACK, NULL, TASK_APP_GET_RX_PRIO, &xappRxHand);
}
