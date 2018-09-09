/**
 * \file phy.h
 *
 * \brief ATMEGAxxxRFR2 PHY interface
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

#ifndef _PHY_H_
#define _PHY_H_

/*- Includes ---------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

/**
 * \ingroup group_phy_rfr2
 * @{
 */

/*- Definitions ------------------------------------------------------------*/
#define PHY_RSSI_BASE_VAL                     (-90)

#define PHY_HAS_RANDOM_NUMBER_GENERATOR
#define PHY_HAS_AES_MODULE

/*- Types ------------------------------------------------------------------*/
typedef struct PHY_DataInd_t {
	uint8_t *data;
	uint8_t size;
	uint8_t lqi;
	int8_t rssi;
} PHY_DataInd_t;

enum {
	PHY_STATUS_SUCCESS                = 0,
	PHY_STATUS_CHANNEL_ACCESS_FAILURE = 1,
	PHY_STATUS_NO_ACK                 = 2,
	PHY_STATUS_ERROR                  = 3,
};

/*- Prototypes -------------------------------------------------------------*/
void PHY_Init(void);
void PHY_SetRxState(bool rx);
void PHY_SetChannel(uint8_t channel);
void PHY_SetBand(uint8_t band);
void PHY_SetPanId(uint16_t panId);
void PHY_SetShortAddr(uint16_t addr);
void PHY_SetTxPower(uint8_t txPower);
void PHY_Sleep(void);
void PHY_Wakeup(void);
void PHY_DataReq(uint8_t *data);
void PHY_DataConf(uint8_t status);
void PHY_DataInd(PHY_DataInd_t *ind);
void PHY_TaskHandler(void);
void PHY_SetIEEEAddr(uint8_t *ieee_addr);
uint16_t PHY_RandomReq(void);
void PHY_EncryptReq(uint8_t *text, uint8_t *key);

int8_t PHY_EdReq(void);

/** @} */
#endif /* _PHY_H_ */
