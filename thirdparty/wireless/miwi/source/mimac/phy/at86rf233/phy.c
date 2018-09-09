/**
* \file  phy.c
*
* \brief Physical Layer Abstraction for AT86RF233 implementation
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
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, 
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
/*
* Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
*/

#include <string.h>
#include <stdbool.h>
#include "trx_access.h"
#include "miwi_config.h"
#if defined(PROTOCOL_P2P) || defined (PROTOCOL_STAR)
#include "miwi_config_p2p.h"      //MiWi Protocol layer configuration file
#endif

#include "delay.h"
#include "sal.h"
#include "phy.h"

/*- Definitions ------------------------------------------------------------*/
#define PHY_CRC_SIZE    2
/*- Definitions ------------------------------------------------------------*/
#define PHY_RSSI_BASE_VAL                     (-91)

/*- Types ------------------------------------------------------------------*/
typedef enum {
	PHY_STATE_INITIAL,
	PHY_STATE_IDLE,
	PHY_STATE_SLEEP,
	PHY_STATE_TX_WAIT_END,
} PhyState_t;

/*- Prototypes -------------------------------------------------------------*/
static void phyWriteRegister(uint8_t reg, uint8_t value);
static uint8_t phyReadRegister(uint8_t reg);
static void phyWaitState(uint8_t state);
static void phyTrxSetState(uint8_t state);
static void phySetRxState(void);

/*- Variables --------------------------------------------------------------*/
static PhyState_t phyState = PHY_STATE_INITIAL;
static uint8_t phyRxBuffer[128];
static bool phyRxState;
RxBuffer_t RxBuffer[BANK_SIZE];
/*************************************************************************//**
*****************************************************************************/
// Trigger to Transmit Packet
void PHY_DataReq(uint8_t *data)
{
	phyTrxSetState(TRX_CMD_TX_ARET_ON);

	phyReadRegister(IRQ_STATUS_REG);

	/* size of the buffer is sent as first byte of the data
	 * and data starts from second byte.
	 */
	data[0] += 2;// 2
	trx_frame_write(&data[0], (data[0]-1 ) /* length value*/);
	phyState = PHY_STATE_TX_WAIT_END;

	TRX_SLP_TR_HIGH();
	TRX_TRIG_DELAY();
	TRX_SLP_TR_LOW();
}

/*************************************************************************//**
*****************************************************************************/
// Random Number Generator
uint16_t PHY_RandomReq(void)
{
	uint16_t rnd = 0;
	uint8_t rndValue;

	phyTrxSetState(TRX_CMD_RX_ON);

	for (uint8_t i = 0; i < 16; i += 2) {
		delay_cycles_us(1);
		rndValue = (phyReadRegister(PHY_RSSI_REG) >> RND_VALUE) & 3;
		rnd |= rndValue << i;
	}
	phySetRxState();

	return rnd;
}


/*************************************************************************//**
*****************************************************************************/
// Radio Initialization
void PHY_Init(void)
{
	trx_spi_init();
	PhyReset();
	phyRxState = false;

	do {phyWriteRegister(TRX_STATE_REG, TRX_CMD_TRX_OFF);
	} while (TRX_STATUS_TRX_OFF !=
	(phyReadRegister(TRX_STATUS_REG) & TRX_STATUS_MASK));


	phyWriteRegister(TRX_CTRL_1_REG,
	(1 << TX_AUTO_CRC_ON) | (3 << SPI_CMD_MODE) |
	(1 << IRQ_MASK_MODE));

	phyWriteRegister(TRX_CTRL_2_REG,
	(1 << RX_SAFE_MODE) | (1 << OQPSK_SCRAM_EN));
}


/*************************************************************************//**
*****************************************************************************/
// Set Radio to Receive State
void PHY_SetRxState(bool rx)
{
	phyRxState = rx;
	phySetRxState();
}

/*************************************************************************//**
*****************************************************************************/
// Set Channel
void PHY_SetChannel(uint8_t channel)
{
	uint8_t reg;

	if (PHY_STATE_SLEEP == phyState)
	{
		PHY_Wakeup();
	}

	reg = phyReadRegister(PHY_CC_CCA_REG) & ~0x1f;
	phyWriteRegister(PHY_CC_CCA_REG, reg | channel);
}

/*************************************************************************//**
*****************************************************************************/
// Set Pan ID
void PHY_SetPanId(uint16_t panId)
{
	uint8_t *d = (uint8_t *)&panId;
	phyWriteRegister(PAN_ID_0_REG, d[0]);
	phyWriteRegister(PAN_ID_1_REG, d[1]);
}

/*************************************************************************//**
*****************************************************************************/
// Set Short Address
void PHY_SetShortAddr(uint16_t addr)
{
	uint8_t *d = (uint8_t *)&addr;

	phyWriteRegister(SHORT_ADDR_0_REG, d[0]);
	phyWriteRegister(SHORT_ADDR_1_REG, d[1]);
	phyWriteRegister(CSMA_SEED_0_REG, d[0] + d[1]);
}

/*************************************************************************//**
*****************************************************************************/
// Set Transmit Power
void PHY_SetTxPower(uint8_t txPower)
{
	uint8_t reg;
	reg = phyReadRegister(PHY_TX_PWR_REG) & ~0x0f;
	phyWriteRegister(PHY_TX_PWR_REG, reg | txPower);
}

/*************************************************************************//**
*****************************************************************************/
// Radio Sleep
void PHY_Sleep(void)
{
	if (PHY_STATE_SLEEP != phyState)
	{
		phyTrxSetState(TRX_CMD_TRX_OFF);
		TRX_SLP_TR_HIGH();
		phyState = PHY_STATE_SLEEP;	
	}
}

/*************************************************************************//**
*****************************************************************************/
// Radio Wake Up
void PHY_Wakeup(void)
{
	if (PHY_STATE_SLEEP == phyState)
	{
		TRX_SLP_TR_LOW();
	 	phySetRxState();
	 	phyState = PHY_STATE_IDLE;
	}
}

/*************************************************************************//**
*****************************************************************************/
// Encrypt Block
void PHY_EncryptReq(uint8_t *text, uint8_t *key)
{
	sal_aes_setup(key, AES_MODE_ECB, AES_DIR_ENCRYPT);
	#if (SAL_TYPE == AT86RF2xx)
	sal_aes_wrrd(text, NULL);
	#else
	sal_aes_exec(text);
	#endif
	sal_aes_read(text);
}

void PHY_EncryptReqCBC(uint8_t *text, uint8_t *key)
{
	sal_aes_setup(key, AES_MODE_CBC, AES_DIR_ENCRYPT);
	#if (SAL_TYPE == AT86RF2xx)
	sal_aes_wrrd(text, NULL);
	#else
	sal_aes_exec(text);
	#endif
	sal_aes_read(text);
}

/*************************************************************************//**
*****************************************************************************/
// Decrypt Block
void PHY_DecryptReq(uint8_t *text, uint8_t *key)
{
	sal_aes_setup(key, AES_MODE_ECB, AES_DIR_DECRYPT);
	sal_aes_wrrd(text, NULL);
	sal_aes_read(text);
}

/*************************************************************************//**
*****************************************************************************/
// Energy Detection
uint8_t PHY_EdReq(void)
{
	uint8_t ed;
	uint8_t prev_rx_pdt_dis;

	phyTrxSetState(TRX_CMD_PLL_ON);
	phyReadRegister(IRQ_STATUS_REG);
	/*Ensure that register bit RX_PDT_DIS is set to 0*/
	prev_rx_pdt_dis = phyReadRegister(RX_SYN_REG);

	phyWriteRegister(RX_SYN_REG, (prev_rx_pdt_dis | (1<<7)));

	phyTrxSetState(TRX_CMD_RX_ON);

	phyWriteRegister(PHY_ED_LEVEL_REG, 0xFF);

	while (0 == (phyReadRegister(IRQ_STATUS_REG) & (1 << CCA_ED_DONE))) {
	}

	ed = (uint8_t)phyReadRegister(PHY_ED_LEVEL_REG);

	phySetRxState();
	phyWriteRegister(RX_SYN_REG, prev_rx_pdt_dis);

	/*	*Adding the base value gets the real value for ED which will be negative.
		*Since the RSSI values will be used within the Same radio for comparisons
		*Normalised value from the register is sufficient
	*/

	return ed; // + PHY_RSSI_BASE_VAL;
}

/*************************************************************************//**
*****************************************************************************/
static void phyWriteRegister(uint8_t reg, uint8_t value)
{
	trx_reg_write(reg, value);
}

/*************************************************************************//**
*****************************************************************************/
static uint8_t phyReadRegister(uint8_t reg)
{
	uint8_t value;

	value = trx_reg_read(reg);

	return value;
}

/*************************************************************************//**
*****************************************************************************/
static void phyWaitState(uint8_t state)
{
	while (state != (phyReadRegister(TRX_STATUS_REG) & TRX_STATUS_MASK)) {
	}
}

/*************************************************************************//**
*****************************************************************************/
static void phySetRxState(void)
{
	phyTrxSetState(TRX_CMD_TRX_OFF);

	phyReadRegister(IRQ_STATUS_REG);

	if (phyRxState) {
		phyTrxSetState(TRX_CMD_RX_AACK_ON);
		phyState = PHY_STATE_IDLE;
	}
}

/*************************************************************************//**
*****************************************************************************/
static void phyTrxSetState(uint8_t state)
{
    if (PHY_STATE_SLEEP == phyState)
	{
		TRX_SLP_TR_LOW();
	}
	do { phyWriteRegister(TRX_STATE_REG, TRX_CMD_FORCE_TRX_OFF);
	} while (TRX_STATUS_TRX_OFF !=
			(phyReadRegister(TRX_STATUS_REG) & TRX_STATUS_MASK));

	do { phyWriteRegister(TRX_STATE_REG,
			     state); } while (state !=
			(phyReadRegister(TRX_STATUS_REG) & TRX_STATUS_MASK));
}

/*************************************************************************//**
*****************************************************************************/
// Setting the IEEE address
void PHY_SetIEEEAddr(uint8_t *ieee_addr)
{
	uint8_t *ptr_to_reg = ieee_addr;
	for (uint8_t i = 0; i < 8; i++) {
		trx_reg_write((IEEE_ADDR_0_REG + i), *ptr_to_reg);
		ptr_to_reg++;
	}
}

/*************************************************************************//**
*****************************************************************************/
// Handle Packet Received
void PHY_TaskHandler(void)
{
	if (PHY_STATE_SLEEP == phyState)
	{
		return;
	}

	if (phyReadRegister(IRQ_STATUS_REG) & (1 << TRX_END))
	{
		if (PHY_STATE_IDLE == phyState)
		{
			uint8_t size,i,RxBank=0xFF;
			for (i = 0; i < BANK_SIZE; i++)
			{
				if (RxBuffer[i].PayloadLen == 0)
				{
					RxBank = i;
					break;
				}
			}

			if (RxBank < BANK_SIZE)
			{
				trx_frame_read(&size, 1);

			 	trx_frame_read(phyRxBuffer, size + 2);
				RxBuffer[RxBank].PayloadLen = size+2;
				if (RxBuffer[RxBank].PayloadLen < RX_PACKET_SIZE)
			 	{
					//copy all of the data from the FIFO into the RxBuffer, plus RSSI and LQI
					for (i = 1; i <= size+2; i++)
					{
						RxBuffer[RxBank].Payload[i-1] = phyRxBuffer[i];
					}
			    }
				phyWaitState(TRX_STATUS_RX_AACK_ON);
			}
		}
		else if (PHY_STATE_TX_WAIT_END == phyState)
		{
			uint8_t status = (phyReadRegister(TRX_STATE_REG) >>  TRAC_STATUS) & 7;
   			if (TRAC_STATUS_SUCCESS == status)
			{
				status = PHY_STATUS_SUCCESS;
			}
			else if (TRAC_STATUS_CHANNEL_ACCESS_FAILURE == status)
			{
				status = PHY_STATUS_CHANNEL_ACCESS_FAILURE;
			}
			else if (TRAC_STATUS_NO_ACK == status)
			{
				status = PHY_STATUS_NO_ACK;
			}
			else
			{
				status = PHY_STATUS_ERROR;
			}

			phySetRxState();
			phyState = PHY_STATE_IDLE;
			PHY_DataConf(status);
		}
	}
}
