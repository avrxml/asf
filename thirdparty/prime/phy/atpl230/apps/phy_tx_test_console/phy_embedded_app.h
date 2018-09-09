/**
 * \file
 *
 * \brief ATMEL PLC PHY TX Test Console Application
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
 */

#ifndef APPATPL230_H_INCLUDED
#define APPATPL230_H_INCLUDED

#include "atpl230.h"

#ifdef __cplusplus
extern "C" {
#endif

/* TX task parameters */
typedef struct {
	xPhyMsgTx_t xPhyMsg;
	uint8_t uc_channel;
	uint8_t uc_coupling;
	uint32_t ul_tx_period;
	uint8_t uc_autodetect;
	uint8_t uc_impedance;
} txPhyEmbeddedConfig_t;

/* Commands in GPBR to define behaviuor of the application at start */
#define PHY_APP_CMD_DEFAULT_MODE       0
#define PHY_APP_CMD_MENU_START_MODE    1
#define PHY_APP_CMD_TX_START_MODE      2

/* FreeRTOS configuration */
/* Tasks priorities */
#define TASK_APP_PHY_PRIO              (tskIDLE_PRIORITY + 1)
#define TASK_APP_GET_TX_RESULT_PRIO    (tskIDLE_PRIORITY + 1)
#define TASK_APP_GET_RX_PRIO           (tskIDLE_PRIORITY + 1)

/* Stack definitions */
#define TASK_APP_PHY_STACK             (configMINIMAL_STACK_SIZE * 5)
#define TASK_APP_GET_TX_RESULT_STACK   (configMINIMAL_STACK_SIZE * 2)
#define TASK_APP_GET_RX_STACK          (configMINIMAL_STACK_SIZE * 2)

/* App Task Period */
#define PRIME_APP_PHY_TIMER_RATE       (100 / portTICK_RATE_MS)  /* 100 msec. */

/* Functions */
void vPhyEmbeddedAppTask(txPhyEmbeddedConfig_t *xAppPhyCfgTx);

#ifdef __cplusplus
}
#endif

#endif          /* APPATPL230_H_INCLUDED */
