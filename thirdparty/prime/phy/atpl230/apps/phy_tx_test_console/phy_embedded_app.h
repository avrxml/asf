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
