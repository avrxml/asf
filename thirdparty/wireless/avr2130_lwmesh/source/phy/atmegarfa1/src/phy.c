/**
 * \file phy.c
 *
 * \brief ATMEGAxxxRFA1 PHY implementation
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
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
 *
 */

#ifdef PHY_ATMEGARFA1

/*- Includes ---------------------------------------------------------------*/
#include "phy.h"
#include "sal.h"
#include "delay.h"
#include "atmegarfa1.h"

/*- Definitions ------------------------------------------------------------*/
#define PHY_CRC_SIZE          2
#define IRQ_CLEAR_VALUE       0xff
/*- Types ------------------------------------------------------------------*/
typedef enum {
	PHY_STATE_INITIAL,
	PHY_STATE_IDLE,
	PHY_STATE_SLEEP,
	PHY_STATE_TX_WAIT_END,
} PhyState_t;

/*- Prototypes -------------------------------------------------------------*/
static void phyTrxSetState(uint8_t state);
static void phySetRxState(void);

/*- Variables --------------------------------------------------------------*/
static PhyState_t phyState = PHY_STATE_INITIAL;
static uint8_t phyRxBuffer[128];
static bool phyRxState;

/*- Implementations --------------------------------------------------------*/

/*************************************************************************//**
*****************************************************************************/
void PHY_Init(void)
{
	sysclk_enable_peripheral_clock(&TRX_CTRL_0);

	TRXPR_REG_s.trxrst = 1;

	phyRxState = false;
	phyState = PHY_STATE_IDLE;

	phyTrxSetState(TRX_CMD_TRX_OFF);

	TRX_CTRL_2_REG_s.rxSafeMode = 1;

#ifdef PHY_ENABLE_RANDOM_NUMBER_GENERATOR
	CSMA_SEED_0_REG = (uint8_t)PHY_RandomReq();
#endif
}

/*************************************************************************//**
*****************************************************************************/
void PHY_SetRxState(bool rx)
{
	phyRxState = rx;
	phySetRxState();
}

/*************************************************************************//**
*****************************************************************************/
void PHY_SetChannel(uint8_t channel)
{
	PHY_CC_CCA_REG_s.channel = channel;
}

/*************************************************************************//**
*****************************************************************************/
void PHY_SetPanId(uint16_t panId)
{
	uint8_t *d = (uint8_t *)&panId;

	PAN_ID_0_REG = d[0];
	PAN_ID_1_REG = d[1];
}

/*************************************************************************//**
*****************************************************************************/
void PHY_SetShortAddr(uint16_t addr)
{
	uint8_t *d = (uint8_t *)&addr;

	SHORT_ADDR_0_REG = d[0];
	SHORT_ADDR_1_REG = d[1];

#ifndef PHY_ENABLE_RANDOM_NUMBER_GENERATOR
	CSMA_SEED_0_REG = d[0] + d[1];
#endif
}

/*************************************************************************//**
*****************************************************************************/
void PHY_SetTxPower(uint8_t txPower)
{
	PHY_TX_PWR_REG_s.txPwr = txPower;
}

/*************************************************************************//**
*****************************************************************************/
void PHY_Sleep(void)
{
	phyTrxSetState(TRX_CMD_TRX_OFF);
	TRXPR_REG_s.slptr = 1;
	phyState = PHY_STATE_SLEEP;
}

/*************************************************************************//**
*****************************************************************************/
void PHY_Wakeup(void)
{
	TRXPR_REG_s.slptr = 0;
	phySetRxState();
	phyState = PHY_STATE_IDLE;
}

/*************************************************************************//**
*****************************************************************************/
void PHY_DataReq(uint8_t *data)
{
	phyTrxSetState(TRX_CMD_TX_ARET_ON);

	IRQ_STATUS_REG = IRQ_CLEAR_VALUE;

	TRX_FRAME_BUFFER(0) = data[0] + PHY_CRC_SIZE;
	for (uint8_t i = 0; i < data[0]; i++) {
		TRX_FRAME_BUFFER(i + 1) = data[i + 1];
	}

	phyState = PHY_STATE_TX_WAIT_END;
	TRX_STATE_REG = TRX_CMD_TX_START;
}

/*************************************************************************//**
*****************************************************************************/
uint16_t PHY_RandomReq(void)
{
	uint16_t rnd = 0;

	phyTrxSetState(TRX_CMD_RX_ON);

	for (uint8_t i = 0; i < 16; i += 2) {
		delay_us(RANDOM_NUMBER_UPDATE_INTERVAL);
		rnd |= PHY_RSSI_REG_s.rndValue << i;
	}

	phySetRxState();

	return rnd;
}

/*************************************************************************//**
*****************************************************************************/
void PHY_EncryptReq(uint8_t *text, uint8_t *key)
{
	sal_aes_setup(key, AES_MODE_ECB, AES_DIR_ENCRYPT);
	sal_aes_exec(text);
	sal_aes_read(text);
}

/*************************************************************************//**
*****************************************************************************/
int8_t PHY_EdReq(void)
{
	int8_t ed;

	phyTrxSetState(TRX_CMD_RX_ON);

	IRQ_STATUS_REG_s.ccaEdDone = 1;
	PHY_ED_LEVEL_REG = 0;
	while (0 == IRQ_STATUS_REG_s.ccaEdDone) {
	}

	ed = (int8_t)PHY_ED_LEVEL_REG + PHY_RSSI_BASE_VAL;

	phySetRxState();

	return ed;
}

/*************************************************************************//**
*****************************************************************************/
static void phySetRxState(void)
{
	phyTrxSetState(TRX_CMD_TRX_OFF);

	IRQ_STATUS_REG = IRQ_CLEAR_VALUE;

	if (phyRxState) {
		phyTrxSetState(TRX_CMD_RX_AACK_ON);
	}
}

/*************************************************************************//**
*****************************************************************************/
static void phyTrxSetState(uint8_t state)
{
	do {TRX_STATE_REG = TRX_CMD_FORCE_TRX_OFF;
	} while (TRX_STATUS_TRX_OFF !=
			TRX_STATUS_REG_s.trxStatus);

	do {TRX_STATE_REG = state; } while (state !=
			TRX_STATUS_REG_s.trxStatus);
}

/*************************************************************************//**
*****************************************************************************/
void PHY_SetIEEEAddr(uint8_t *ieee_addr)
{
	uint8_t *ptr_to_reg = ieee_addr;
	IEEE_ADDR_0_REG = *ptr_to_reg++;
	IEEE_ADDR_1_REG = *ptr_to_reg++;
	IEEE_ADDR_2_REG = *ptr_to_reg++;
	IEEE_ADDR_3_REG = *ptr_to_reg++;
	IEEE_ADDR_4_REG = *ptr_to_reg++;
	IEEE_ADDR_5_REG = *ptr_to_reg++;
	IEEE_ADDR_6_REG = *ptr_to_reg++;
	IEEE_ADDR_7_REG = *ptr_to_reg;
}

/*************************************************************************//**
*****************************************************************************/
void PHY_TaskHandler(void)
{
	if (PHY_STATE_SLEEP == phyState) {
		return;
	}

	if (IRQ_STATUS_REG_s.rxEnd) {
		PHY_DataInd_t ind;
		uint8_t size = TST_RX_LENGTH_REG;

		for (uint8_t i = 0; i < size + 1 /*lqi*/; i++) {
			phyRxBuffer[i] = TRX_FRAME_BUFFER(i);
		}

		ind.data = phyRxBuffer;
		ind.size = size - PHY_CRC_SIZE;
		ind.lqi  = phyRxBuffer[size];
		ind.rssi = (int8_t)PHY_ED_LEVEL_REG + PHY_RSSI_BASE_VAL;
		PHY_DataInd(&ind);

		while (TRX_STATUS_RX_AACK_ON != TRX_STATUS_REG_s.trxStatus) {
		}

		IRQ_STATUS_REG_s.rxEnd = 1;
		TRX_CTRL_2_REG_s.rxSafeMode = 0;
		TRX_CTRL_2_REG_s.rxSafeMode = 1;
	} else if (IRQ_STATUS_REG_s.txEnd) {
		if (TRX_STATUS_TX_ARET_ON == TRX_STATUS_REG_s.trxStatus) {
			uint8_t status = TRX_STATE_REG_s.tracStatus;

			if (TRAC_STATUS_SUCCESS == status) {
				status = PHY_STATUS_SUCCESS;
			} else if (TRAC_STATUS_CHANNEL_ACCESS_FAILURE ==
					status) {
				status = PHY_STATUS_CHANNEL_ACCESS_FAILURE;
			} else if (TRAC_STATUS_NO_ACK == status) {
				status = PHY_STATUS_NO_ACK;
			} else {
				status = PHY_STATUS_ERROR;
			}

			phySetRxState();
			phyState = PHY_STATE_IDLE;

			PHY_DataConf(status);
		}

		IRQ_STATUS_REG_s.txEnd = 1;
	}
}

#endif /* PHY_ATMEGARFA1 */
