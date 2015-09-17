/* This file is auto-generated. Do not modify manually! */

/**
 * @file atmega256rfr2.h
 *
 * @brief File contains register and command defines specific for ATMEGA256RFR2.
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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

/*
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* Prevent double inclusion */
#ifndef ATMEGA256RFR2_H
#define ATMEGA256RFR2_H

/* === INCLUDES ============================================================ */

#include "tal_types.h"
#if (TAL_TYPE == ATMEGARFR2)

#define TRANSCEIVER_NAME    "ATMEGARFR2"
/* === EXTERNALS =========================================================== */

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/** Parameter definitions */

/** deactivate macros defined in iom header file */
#undef                       P_ON
#undef                       BUSY_RX
#undef                       BUSY_TX
#undef                       RX_ON
#undef                       TRX_OFF
#undef                       PLL_ON
#undef                       BUSY_RX_AACK
#undef                       BUSY_TX_ARET
#undef                       RX_AACK_ON
#undef                       TX_ARET_ON
#undef                       RX_ON_NOCLK
#undef                       RX_AACK_ON_NOCLK
#undef                       BUSY_RX_AACK_NOCLK
#undef                       STATE_TRANSITION_IN_PROGRESS
#undef                       CMD_NOP
#undef                       CMD_TX_START
#undef                       CMD_FORCE_TRX_OFF
#undef                       CMD_FORCE_PLL_ON
#undef                       CMD_RX_ON
#undef                       CMD_TRX_OFF
#undef                       CMD_PLL_ON
#undef                       CMD_RX_AACK_ON
#undef                       CMD_TX_ARET_ON
#undef                       TRAC_SUCCESS
#undef                       TRAC_SUCCESS_DATA_PENDING
#undef                       TRAC_SUCCESS_WAIT_FOR_ACK
#undef                       TRAC_CHANNEL_ACCESS_FAILURE
#undef                       TRAC_NO_ACK
#undef                       TRAC_INVALID

/** Important parameters. */
/* TRX Parameter: Bit mask of transceiver supported channel */
#define TRX_SUPPORTED_CHANNELS                             (0x07FFF800)

/*
 * TRX Parameter: Tolerance of the phyTransmitPower PIB attribute. This is
 * encoded into the two
 * MSBits of the attribute, and is effectively read-only.
 */
#define TX_PWR_TOLERANCE                                   (0x80)

/** Typical timing values. */
/* TRX Parameter: t10 */
#define RST_PULSE_WIDTH_US                                 (1)
/* TRX Parameter: tTR1 typical value */
#define P_ON_TO_CLKM_AVAILABLE_TYP_US                      (380)
/* TRX Parameter: tTR1 max. value */
#define P_ON_TO_CLKM_AVAILABLE_MAX_US                      (1000)
/* TRX Parameter: tTR2 typical value */
#define SLEEP_TO_TRX_OFF_TYP_US                            (240)
/* TRX Parameter: tTR2 max. value */
#define SLEEP_TO_TRX_OFF_MAX_US                            (1000)
/* TRX Parameter: tTR3 */
#define TRX_OFF_TO_SLEEP_TIME_CLKM_CYCLES                  (35)
/* TRX Parameter: tTR4 */
#define TRX_OFF_TO_PLL_ON_TIME_US                          (110)
/* TRX Parameter: tTR13 typical value */
#define RESET_TO_TRX_OFF_TYP_US                            (37)
/* TRX Parameter: TRX Parameter: tTR13 max. value */
#define RESET_TO_TRX_OFF_MAX_US                            (1000)
/* TRX Parameter: tIRQ */
#define TRX_IRQ_DELAY_US                                   (9)
/* TRX Parameter: IRQ processing delay */
#define IRQ_PROCESSING_DLY_US                              (32)

/** CSMA timimg parameters */
/* TRX Parameter: CSMA timimg parameters */
#define CCA_DETECTION_TIME_SYM                             (8)
/* TRX Parameter: RSSIBASE_VAL */
#define RSSI_BASE_VAL_DBM                                  (-90)
/* TRX Parameter: CCA processing time in symbols */
#define CCA_PROCESS_TIME_SYM                               (1)
/* TRX Parameter: Complete CCA Duration in symbols */
#define CCA_DURATION_SYM                                   ( \
		CCA_DETECTION_TIME_SYM + CCA_PROCESS_TIME_SYM)
/* TRX Parameter: Preparation time for CCA */
#define CCA_PREPARATION_DURATION_US                        (50)
/* TRX Parameter: Duration before start of CCA */
#define CCA_PRE_START_DURATION_US                          (20)
/* TRX Parameter: 16 us processing delay + 16 us software handling duration */
#define PRE_TX_DURATION_US                                 (32)

/** Register addresses */

/** Offset for register IRQ_MASK1 */
#define RG_IRQ_MASK1                    (0xBE)
/** Sub-registers of Register @ref RG_IRQ_MASK1 */

/** Access parameters for sub-register TX_START_EN in register @ref RG_IRQ_MASK1
**/
#define SR_TX_START_EN                  0xBE, 0x01, 0

/** Access parameters for sub-register MAF_0_AMI_EN in register @ref
 * RG_IRQ_MASK1 */
#define SR_MAF_0_AMI_EN                 0xBE, 0x02, 1

/** Access parameters for sub-register MAF_1_AMI_EN in register @ref
 * RG_IRQ_MASK1 */
#define SR_MAF_1_AMI_EN                 0xBE, 0x04, 2

/** Access parameters for sub-register MAF_2_AMI_EN in register @ref
 * RG_IRQ_MASK1 */
#define SR_MAF_2_AMI_EN                 0xBE, 0x08, 3

/** Access parameters for sub-register MAF_3_AMI_EN in register @ref
 * RG_IRQ_MASK1 */
#define SR_MAF_3_AMI_EN                 0xBE, 0x10, 4

/** Offset for register IRQ_STATUS1 */
#define RG_IRQ_STATUS1                  (0xBF)
/** Sub-registers of Register @ref RG_IRQ_STATUS1 */

/** Access parameters for sub-register TX_START in register @ref RG_IRQ_STATUS1
**/
#define SR_TX_START                     0xBF, 0x01, 0

/** Access parameters for sub-register MAF_0_AMI in register @ref RG_IRQ_STATUS1
**/
#define SR_MAF_0_AMI                    0xBF, 0x02, 1

/** Access parameters for sub-register MAF_1_AMI in register @ref RG_IRQ_STATUS1
**/
#define SR_MAF_1_AMI                    0xBF, 0x04, 2

/** Access parameters for sub-register MAF_2_AMI in register @ref RG_IRQ_STATUS1
**/
#define SR_MAF_2_AMI                    0xBF, 0x08, 3

/** Access parameters for sub-register MAF_3_AMI in register @ref RG_IRQ_STATUS1
**/
#define SR_MAF_3_AMI                    0xBF, 0x10, 4

/** Offset for register MAFCR0 */
#define RG_MAFCR0                       (0x10C)
/** Sub-registers of Register @ref RG_MAFCR0 */
/** Access parameters for sub-register MAF0EN in register @ref RG_MAFCR0 */
#define SR_MAF0EN                       0x10C, 0x01, 0
/** Access parameters for sub-register MAF1EN in register @ref RG_MAFCR0 */
#define SR_MAF1EN                       0x10C, 0x02, 1
/** Access parameters for sub-register MAF2EN in register @ref RG_MAFCR0 */
#define SR_MAF2EN                       0x10C, 0x04, 2
/** Access parameters for sub-register MAF3EN in register @ref RG_MAFCR0 */
#define SR_MAF3EN                       0x10C, 0x08, 3

/** Offset for register MAFCR1 */
#define RG_MAFCR1                       (0x10D)
/** Sub-registers of Register @ref RG_MAFCR1 */

/** Access parameters for sub-register AACK_0_I_AM_COORD in register @ref
 * RG_MAFCR1 */
#define SR_AACK_0_I_AM_COORD            0x10D, 0x01, 0

/** Access parameters for sub-register AACK_0_SET_PD in register @ref RG_MAFCR1
**/
#define SR_AACK_0_SET_PD                0x10D, 0x02, 1

/** Access parameters for sub-register AACK_1_I_AM_COORD in register @ref
 * RG_MAFCR1 */
#define SR_AACK_1_I_AM_COORD            0x10D, 0x04, 2

/** Access parameters for sub-register AACK_1_SET_PD in register @ref RG_MAFCR1
**/
#define SR_AACK_1_SET_PD                0x10D, 0x08, 3

/** Access parameters for sub-register AACK_2_I_AM_COORD in register @ref
 * RG_MAFCR1 */
#define SR_AACK_2_I_AM_COORD            0x10D, 0x10, 4

/** Access parameters for sub-register AACK_2_SET_PD in register @ref RG_MAFCR1
**/
#define SR_AACK_2_SET_PD                0x10D, 0x20, 5

/** Access parameters for sub-register AACK_3_I_AM_COORD in register @ref
 * RG_MAFCR1 */
#define SR_AACK_3_I_AM_COORD            0x10D, 0x40, 6

/** Access parameters for sub-register AACK_3_SET_PD in register @ref RG_MAFCR1
**/
#define SR_AACK_3_SET_PD                0x10D, 0x80, 7

/** Offset for register MAFSA0L */
#define RG_MAFSA0L                      (0x10E)

/** Offset for register MAFSA0H */
#define RG_MAFSA0H                      (0x10F)

/** Offset for register MAFPA0L */
#define RG_MAFPA0L                      (0x110)

/** Offset for register MAFPA0H */
#define RG_MAFPA0H                      (0x111)

/** Offset for register MAFSA1L */
#define RG_MAFSA1L                      (0x112)

/** Offset for register MAFSA1H */
#define RG_MAFSA1H                      (0x113)

/** Offset for register MAFPA1L */
#define RG_MAFPA1L                      (0x114)

/** Offset for register MAFPA1H */
#define RG_MAFPA1H                      (0x115)

/** Offset for register MAFSA2L */
#define RG_MAFSA2L                      (0x116)

/** Offset for register MAFSA2H */
#define RG_MAFSA2H                      (0x117)

/** Offset for register MAFPA2L */
#define RG_MAFPA2L                      (0x118)

/** Offset for register MAFPA2H */
#define RG_MAFPA2H                      (0x119)

/** Offset for register MAFSA3L */
#define RG_MAFSA3L                      (0x11A)

/** Offset for register MAFSA3H */
#define RG_MAFSA3H                      (0x11B)

/** Offset for register MAFPA3L */
#define RG_MAFPA3L                      (0x11C)

/** Offset for register MAFPA3H */
#define RG_MAFPA3H                      (0x11D)

/** Offset for register PARCR */
#define RG_PARCR                        (0x138)
/** Sub-registers of Register @ref RG_PARCR */
/** Access parameters for sub-register PARUFI in register @ref RG_PARCR */
#define SR_PARUFI                       0x138, 0x01, 0
/** Access parameters for sub-register PARDFI in register @ref RG_PARCR */
#define SR_PARDFI                       0x138, 0x02, 1
/** Access parameters for sub-register PALTU in register @ref RG_PARCR */
#define SR_PALTU                        0x138, 0x1C, 2
/** Access parameters for sub-register PALTD in register @ref RG_PARCR */
#define SR_PALTD                        0x138, 0xE0, 5

/** Offset for register AES_CTRL */
#define RG_AES_CTRL                     (0x13C)
/** Sub-registers of Register @ref RG_AES_CTRL */
/** Access parameters for sub-register AES_IM in register @ref RG_AES_CTRL */
#define SR_AES_IM                       0x13C, 0x04, 2
/** Access parameters for sub-register AES_DIR in register @ref RG_AES_CTRL */
#define SR_AES_DIR                      0x13C, 0x08, 3

/** Constant AES_DIR_DECRYPT for sub-register @ref SR_AES_DIR in register
 * AES_CTRL */
#define AES_DIR_DECRYPT                 (1)

/** Constant AES_DIR_ENCRYPT for sub-register @ref SR_AES_DIR in register
 * AES_CTRL */
#define AES_DIR_ENCRYPT                 (0)
/** Access parameters for sub-register AES_MODE in register @ref RG_AES_CTRL */
#define SR_AES_MODE                     0x13C, 0x20, 5

/** Access parameters for sub-register AES_REQUEST in register @ref RG_AES_CTRL
**/
#define SR_AES_REQUEST                  0x13C, 0x80, 7

/** Constant AES_START for sub-register @ref SR_AES_REQUEST in register AES_CTRL
**/
#define AES_START                       (1)

/** Offset for register AES_STATUS */
#define RG_AES_STATUS                   (0x13D)
/** Sub-registers of Register @ref RG_AES_STATUS */

/** Access parameters for sub-register AES_DONE in register @ref RG_AES_STATUS
**/
#define SR_AES_DONE                     0x13D, 0x01, 0

/** Constant AES_COMPLETED for sub-register @ref SR_AES_DONE in register
 * AES_STATUS */
#define AES_COMPLETED                   (1)

/** Constant AES_NOT_COMPLETED for sub-register @ref SR_AES_DONE in register
 * AES_STATUS */
#define AES_NOT_COMPLETED               (0)
/** Access parameters for sub-register AES_ER in register @ref RG_AES_STATUS */
#define SR_AES_ER                       0x13D, 0x80, 7

/** Offset for register AES_STATE */
#define RG_AES_STATE                    (0x13E)

/** Offset for register AES_KEY */
#define RG_AES_KEY                      (0x13F)

/** Offset for register TRX_STATUS */
#define RG_TRX_STATUS                   (0x141)
/** Sub-registers of Register @ref RG_TRX_STATUS */

/** Access parameters for sub-register TRX_STATUS in register @ref RG_TRX_STATUS
**/
#define SR_TRX_STATUS                   0x141, 0x1F, 0

/** Access parameters for sub-register TST_STATUS in register @ref RG_TRX_STATUS
**/
#define SR_TST_STATUS                   0x141, 0x20, 5

/** Access parameters for sub-register CCA_STATUS in register @ref RG_TRX_STATUS
**/
#define SR_CCA_STATUS                   0x141, 0x40, 6

/** Constant CCA_CH_BUSY for sub-register @ref SR_CCA_STATUS in register
 * TRX_STATUS */
#define CCA_CH_BUSY                     (0)

/** Constant CCA_CH_IDLE for sub-register @ref SR_CCA_STATUS in register
 * TRX_STATUS */
#define CCA_CH_IDLE                     (1)

/** Access parameters for sub-register CCA_DONE in register @ref RG_TRX_STATUS
**/
#define SR_CCA_DONE                     0x141, 0x80, 7

/** Constant CCA_COMPLETED for sub-register @ref SR_CCA_DONE in register
 * TRX_STATUS */
#define CCA_COMPLETED                   (1)

/** Constant CCA_NOT_COMPLETED for sub-register @ref SR_CCA_DONE in register
 * TRX_STATUS */
#define CCA_NOT_COMPLETED               (0)

/** Offset for register TRX_STATE */
#define RG_TRX_STATE                    (0x142)
/** Sub-registers of Register @ref RG_TRX_STATE */
/** Access parameters for sub-register TRX_CMD in register @ref RG_TRX_STATE */
#define SR_TRX_CMD                      0x142, 0x1F, 0

/** Access parameters for sub-register TRAC_STATUS in register @ref RG_TRX_STATE
**/
#define SR_TRAC_STATUS                  0x142, 0xE0, 5

/** Offset for register TRX_CTRL_0 */
#define RG_TRX_CTRL_0                   (0x143)
/** Sub-registers of Register @ref RG_TRX_CTRL_0 */

/** Access parameters for sub-register PMU_IF_INV in register @ref RG_TRX_CTRL_0
**/
#define SR_PMU_IF_INV                   0x143, 0x10, 4

/** Access parameters for sub-register PMU_START in register @ref RG_TRX_CTRL_0
**/
#define SR_PMU_START                    0x143, 0x20, 5
/** Access parameters for sub-register PMU_EN in register @ref RG_TRX_CTRL_0 */
#define SR_PMU_EN                       0x143, 0x40, 6

/** Offset for register TRX_CTRL_1 */
#define RG_TRX_CTRL_1                   (0x144)
/** Sub-registers of Register @ref RG_TRX_CTRL_1 */

/** Access parameters for sub-register PLL_TX_FLT in register @ref RG_TRX_CTRL_1
**/
#define SR_PLL_TX_FLT                   0x144, 0x10, 4

/** Access parameters for sub-register TX_AUTO_CRC_ON in register @ref
 * RG_TRX_CTRL_1 */
#define SR_TX_AUTO_CRC_ON               0x144, 0x20, 5

/** Constant TX_AUTO_CRC_DISABLE for sub-register @ref SR_TX_AUTO_CRC_ON in
 * register TRX_CTRL_1 */
#define TX_AUTO_CRC_DISABLE             (0)

/** Constant TX_AUTO_CRC_ENABLE for sub-register @ref SR_TX_AUTO_CRC_ON in
 * register TRX_CTRL_1 */
#define TX_AUTO_CRC_ENABLE              (1)

/** Access parameters for sub-register IRQ_2_EXT_EN in register @ref
 * RG_TRX_CTRL_1 */
#define SR_IRQ_2_EXT_EN                 0x144, 0x40, 6

/** Access parameters for sub-register PA_EXT_EN in register @ref RG_TRX_CTRL_1
**/
#define SR_PA_EXT_EN                    0x144, 0x80, 7

/** Offset for register PHY_TX_PWR */
#define RG_PHY_TX_PWR                   (0x145)
/** Sub-registers of Register @ref RG_PHY_TX_PWR */
/** Access parameters for sub-register TX_PWR in register @ref RG_PHY_TX_PWR */
#define SR_TX_PWR                       0x145, 0x0F, 0

/** Offset for register PHY_RSSI */
#define RG_PHY_RSSI                     (0x146)
/** Sub-registers of Register @ref RG_PHY_RSSI */
/** Access parameters for sub-register RSSI in register @ref RG_PHY_RSSI */
#define SR_RSSI                         0x146, 0x1F, 0
/** Access parameters for sub-register RND_VALUE in register @ref RG_PHY_RSSI */
#define SR_RND_VALUE                    0x146, 0x60, 5

/** Access parameters for sub-register RX_CRC_VALID in register @ref RG_PHY_RSSI
**/
#define SR_RX_CRC_VALID                 0x146, 0x80, 7

/** Constant CRC16_NOT_VALID for sub-register @ref SR_RX_CRC_VALID in register
 * PHY_RSSI */
#define CRC16_NOT_VALID                 (0)

/** Constant CRC16_VALID for sub-register @ref SR_RX_CRC_VALID in register
 * PHY_RSSI */
#define CRC16_VALID                     (1)

/** Offset for register PHY_ED_LEVEL */
#define RG_PHY_ED_LEVEL                 (0x147)

/** Offset for register PHY_CC_CCA */
#define RG_PHY_CC_CCA                   (0x148)
/** Sub-registers of Register @ref RG_PHY_CC_CCA */
/** Access parameters for sub-register CHANNEL in register @ref RG_PHY_CC_CCA */
#define SR_CHANNEL                      0x148, 0x1F, 0

/** Access parameters for sub-register CCA_MODE in register @ref RG_PHY_CC_CCA
**/
#define SR_CCA_MODE                     0x148, 0x60, 5

/** Constant CCA_MODE_0 for sub-register @ref SR_CCA_MODE in register PHY_CC_CCA
**/
#define CCA_MODE_0                      (0)

/** Constant CCA_MODE_1 for sub-register @ref SR_CCA_MODE in register PHY_CC_CCA
**/
#define CCA_MODE_1                      (1)

/** Constant CCA_MODE_2 for sub-register @ref SR_CCA_MODE in register PHY_CC_CCA
**/
#define CCA_MODE_2                      (2)

/** Constant CCA_MODE_3 for sub-register @ref SR_CCA_MODE in register PHY_CC_CCA
**/
#define CCA_MODE_3                      (3)

/** Access parameters for sub-register CCA_REQUEST in register @ref
 * RG_PHY_CC_CCA */
#define SR_CCA_REQUEST                  0x148, 0x80, 7

/** Constant CCA_START for sub-register @ref SR_CCA_REQUEST in register
 * PHY_CC_CCA */
#define CCA_START                       (1)

/** Offset for register CCA_THRES */
#define RG_CCA_THRES                    (0x149)
/** Sub-registers of Register @ref RG_CCA_THRES */

/** Access parameters for sub-register CCA_ED_THRES in register @ref
 * RG_CCA_THRES */
#define SR_CCA_ED_THRES                 0x149, 0x0F, 0

/** Access parameters for sub-register CCA_CS_THRES in register @ref
 * RG_CCA_THRES */
#define SR_CCA_CS_THRES                 0x149, 0xF0, 4

/** Offset for register RX_CTRL */
#define RG_RX_CTRL                      (0x14A)
/** Sub-registers of Register @ref RG_RX_CTRL */
/** Access parameters for sub-register PDT_THRES in register @ref RG_RX_CTRL */
#define SR_PDT_THRES                    0x14A, 0x0F, 0

/** Constant THRES_ANT_DIV_DISABLE for sub-register @ref SR_PDT_THRES in
 * register RX_CTRL */
#define THRES_ANT_DIV_DISABLE           (0x7)

/** Constant THRES_ANT_DIV_ENABLE for sub-register @ref SR_PDT_THRES in register
 * RX_CTRL */
#define THRES_ANT_DIV_ENABLE            (0x3)

/** Offset for register SFD_VALUE */
#define RG_SFD_VALUE                    (0x14B)

/** Offset for register TRX_CTRL_2 */
#define RG_TRX_CTRL_2                   (0x14C)
/** Sub-registers of Register @ref RG_TRX_CTRL_2 */

/** Access parameters for sub-register OQPSK_DATA_RATE in register @ref
 * RG_TRX_CTRL_2 */
#define SR_OQPSK_DATA_RATE              0x14C, 0x03, 0

/** Constant RATE_1_MBPS for sub-register @ref SR_OQPSK_DATA_RATE in register
 * TRX_CTRL_2 */
#define RATE_1_MBPS                     (2)

/** Constant RATE_250_KBPS for sub-register @ref SR_OQPSK_DATA_RATE in register
 * TRX_CTRL_2 */
#define RATE_250_KBPS                   (0)

/** Constant RATE_2_MBPS for sub-register @ref SR_OQPSK_DATA_RATE in register
 * TRX_CTRL_2 */
#define RATE_2_MBPS                     (3)

/** Constant RATE_500_KBPS for sub-register @ref SR_OQPSK_DATA_RATE in register
 * TRX_CTRL_2 */
#define RATE_500_KBPS                   (1)

/** Access parameters for sub-register RX_SAFE_MODE in register @ref
 * RG_TRX_CTRL_2 */
#define SR_RX_SAFE_MODE                 0x14C, 0x80, 7

/** Constant RX_SAFE_MODE_DISABLE for sub-register @ref SR_RX_SAFE_MODE in
 * register TRX_CTRL_2 */
#define RX_SAFE_MODE_DISABLE            (0)

/** Constant RX_SAFE_MODE_ENABLE for sub-register @ref SR_RX_SAFE_MODE in
 * register TRX_CTRL_2 */
#define RX_SAFE_MODE_ENABLE             (1)

/** Offset for register ANT_DIV */
#define RG_ANT_DIV                      (0x14D)
/** Sub-registers of Register @ref RG_ANT_DIV */
/** Access parameters for sub-register ANT_CTRL in register @ref RG_ANT_DIV */
#define SR_ANT_CTRL                     0x14D, 0x03, 0

/** Access parameters for sub-register ANT_EXT_SW_EN in register @ref RG_ANT_DIV
**/
#define SR_ANT_EXT_SW_EN                0x14D, 0x04, 2

/** Constant ANT_EXT_SW_DISABLE for sub-register @ref SR_ANT_EXT_SW_EN in
 * register ANT_DIV */
#define ANT_EXT_SW_DISABLE              (0)

/** Constant ANT_EXT_SW_ENABLE for sub-register @ref SR_ANT_EXT_SW_EN in
 * register ANT_DIV */
#define ANT_EXT_SW_ENABLE               (1)
/** Access parameters for sub-register ANT_DIV_EN in register @ref RG_ANT_DIV */
#define SR_ANT_DIV_EN                   0x14D, 0x08, 3

/** Constant ANT_DIV_DISABLE for sub-register @ref SR_ANT_DIV_EN in register
 * ANT_DIV */
#define ANT_DIV_DISABLE                 (0)

/** Constant ANT_DIV_ENABLE for sub-register @ref SR_ANT_DIV_EN in register
 * ANT_DIV */
#define ANT_DIV_ENABLE                  (1)
/** Access parameters for sub-register ANT_SEL in register @ref RG_ANT_DIV */
#define SR_ANT_SEL                      0x14D, 0x80, 7

/** Offset for register IRQ_MASK */
#define RG_IRQ_MASK                     (0x14E)
/** Sub-registers of Register @ref RG_IRQ_MASK */

/** Access parameters for sub-register PLL_LOCK_EN in register @ref RG_IRQ_MASK
**/
#define SR_PLL_LOCK_EN                  0x14E, 0x01, 0

/** Access parameters for sub-register PLL_UNLOCK_EN in register @ref
 * RG_IRQ_MASK */
#define SR_PLL_UNLOCK_EN                0x14E, 0x02, 1

/** Access parameters for sub-register RX_START_EN in register @ref RG_IRQ_MASK
**/
#define SR_RX_START_EN                  0x14E, 0x04, 2
/** Access parameters for sub-register RX_END_EN in register @ref RG_IRQ_MASK */
#define SR_RX_END_EN                    0x14E, 0x08, 3

/** Access parameters for sub-register CCA_ED_DONE_EN in register @ref
 * RG_IRQ_MASK */
#define SR_CCA_ED_DONE_EN               0x14E, 0x10, 4
/** Access parameters for sub-register AMI_EN in register @ref RG_IRQ_MASK */
#define SR_AMI_EN                       0x14E, 0x20, 5
/** Access parameters for sub-register TX_END_EN in register @ref RG_IRQ_MASK */
#define SR_TX_END_EN                    0x14E, 0x40, 6
/** Access parameters for sub-register AWAKE_EN in register @ref RG_IRQ_MASK */
#define SR_AWAKE_EN                     0x14E, 0x80, 7

/** Offset for register IRQ_STATUS */
#define RG_IRQ_STATUS                   (0x14F)
/** Sub-registers of Register @ref RG_IRQ_STATUS */

/** Access parameters for sub-register PLL_LOCK in register @ref RG_IRQ_STATUS
**/
#define SR_PLL_LOCK                     0x14F, 0x01, 0

/** Access parameters for sub-register PLL_UNLOCK in register @ref RG_IRQ_STATUS
**/
#define SR_PLL_UNLOCK                   0x14F, 0x02, 1

/** Access parameters for sub-register RX_START in register @ref RG_IRQ_STATUS
**/
#define SR_RX_START                     0x14F, 0x04, 2
/** Access parameters for sub-register RX_END in register @ref RG_IRQ_STATUS */
#define SR_RX_END                       0x14F, 0x08, 3

/** Access parameters for sub-register CCA_ED_DONE in register @ref
 * RG_IRQ_STATUS */
#define SR_CCA_ED_DONE                  0x14F, 0x10, 4
/** Access parameters for sub-register AMI in register @ref RG_IRQ_STATUS */
#define SR_AMI                          0x14F, 0x20, 5
/** Access parameters for sub-register TX_END in register @ref RG_IRQ_STATUS */
#define SR_TX_END                       0x14F, 0x40, 6
/** Access parameters for sub-register AWAKE in register @ref RG_IRQ_STATUS */
#define SR_AWAKE                        0x14F, 0x80, 7

/** Offset for register VREG_CTRL */
#define RG_VREG_CTRL                    (0x150)
/** Sub-registers of Register @ref RG_VREG_CTRL */
/** Access parameters for sub-register DVDD_OK in register @ref RG_VREG_CTRL */
#define SR_DVDD_OK                      0x150, 0x04, 2

/** Access parameters for sub-register DVREG_EXT in register @ref RG_VREG_CTRL
**/
#define SR_DVREG_EXT                    0x150, 0x08, 3
/** Access parameters for sub-register AVDD_OK in register @ref RG_VREG_CTRL */
#define SR_AVDD_OK                      0x150, 0x40, 6

/** Access parameters for sub-register AVREG_EXT in register @ref RG_VREG_CTRL
**/
#define SR_AVREG_EXT                    0x150, 0x80, 7

/** Offset for register BATMON */
#define RG_BATMON                       (0x151)
/** Sub-registers of Register @ref RG_BATMON */
/** Access parameters for sub-register BATMON_VTH in register @ref RG_BATMON */
#define SR_BATMON_VTH                   0x151, 0x0F, 0
/** Access parameters for sub-register BATMON_HR in register @ref RG_BATMON */
#define SR_BATMON_HR                    0x151, 0x10, 4

/** Constant BATMON_HIGH_RANGE for sub-register @ref SR_BATMON_HR in register
 * BATMON */
#define BATMON_HIGH_RANGE               (1)

/** Constant BATMON_LOW_RANGE for sub-register @ref SR_BATMON_HR in register
 * BATMON */
#define BATMON_LOW_RANGE                (0)
/** Access parameters for sub-register BATMON_OK in register @ref RG_BATMON */
#define SR_BATMON_OK                    0x151, 0x20, 5

/** Constant BATMON_ABOVE_THRES for sub-register @ref SR_BATMON_OK in register
 * BATMON */
#define BATMON_ABOVE_THRES              (1)

/** Constant BATMON_BELOW_THRES for sub-register @ref SR_BATMON_OK in register
 * BATMON */
#define BATMON_BELOW_THRES              (0)
/** Access parameters for sub-register BAT_LOW_EN in register @ref RG_BATMON */
#define SR_BAT_LOW_EN                   0x151, 0x40, 6
/** Access parameters for sub-register BAT_LOW in register @ref RG_BATMON */
#define SR_BAT_LOW                      0x151, 0x80, 7

/** Offset for register XOSC_CTRL */
#define RG_XOSC_CTRL                    (0x152)
/** Sub-registers of Register @ref RG_XOSC_CTRL */

/** Access parameters for sub-register XTAL_TRIM in register @ref RG_XOSC_CTRL
**/
#define SR_XTAL_TRIM                    0x152, 0x0F, 0

/** Access parameters for sub-register XTAL_MODE in register @ref RG_XOSC_CTRL
**/
#define SR_XTAL_MODE                    0x152, 0xF0, 4

/** Offset for register CC_CTRL_0 */
#define RG_CC_CTRL_0                    (0x153)

/** Offset for register CC_CTRL_1 */
#define RG_CC_CTRL_1                    (0x154)
/** Sub-registers of Register @ref RG_CC_CTRL_1 */
/** Access parameters for sub-register CC_BAND in register @ref RG_CC_CTRL_1 */
#define SR_CC_BAND                      0x154, 0x0F, 0

/** Offset for register RX_SYN */
#define RG_RX_SYN                       (0x155)
/** Sub-registers of Register @ref RG_RX_SYN */

/** Access parameters for sub-register RX_PDT_LEVEL in register @ref RG_RX_SYN
**/
#define SR_RX_PDT_LEVEL                 0x155, 0x0F, 0
/** Access parameters for sub-register RX_OVERRIDE in register @ref RG_RX_SYN */
#define SR_RX_OVERRIDE                  0x155, 0x40, 6
/** Access parameters for sub-register RX_PDT_DIS in register @ref RG_RX_SYN */
#define SR_RX_PDT_DIS                   0x155, 0x80, 7

/** Constant RX_DISABLE for sub-register @ref SR_RX_PDT_DIS in register RX_SYN
**/
#define RX_DISABLE                      (1)
/** Constant RX_ENABLE for sub-register @ref SR_RX_PDT_DIS in register RX_SYN */
#define RX_ENABLE                       (0)

/** Offset for register TRX_RPC */
#define RG_TRX_RPC                      (0x156)
/** Sub-registers of Register @ref RG_TRX_RPC */
/** Access parameters for sub-register XAH_RPC_EN in register @ref RG_TRX_RPC */
#define SR_XAH_RPC_EN                   0x156, 0x01, 0

/** Access parameters for sub-register IPAN_RPC_EN in register @ref RG_TRX_RPC
**/
#define SR_IPAN_RPC_EN                  0x156, 0x02, 1
/** Access parameters for sub-register PLL_RPC_EN in register @ref RG_TRX_RPC */
#define SR_PLL_RPC_EN                   0x156, 0x08, 3
/** Access parameters for sub-register PDT_RPC_EN in register @ref RG_TRX_RPC */
#define SR_PDT_RPC_EN                   0x156, 0x10, 4
/** Access parameters for sub-register RX_RPC_EN in register @ref RG_TRX_RPC */
#define SR_RX_RPC_EN                    0x156, 0x20, 5

/** Access parameters for sub-register RX_RPC_CTRL in register @ref RG_TRX_RPC
**/
#define SR_RX_RPC_CTRL                  0x156, 0xC0, 6

/** Offset for register XAH_CTRL_1 */
#define RG_XAH_CTRL_1                   (0x157)
/** Sub-registers of Register @ref RG_XAH_CTRL_1 */

/** Access parameters for sub-register AACK_PROM_MODE in register @ref
 * RG_XAH_CTRL_1 */
#define SR_AACK_PROM_MODE               0x157, 0x02, 1

/** Constant PROM_MODE_DISABLE for sub-register @ref SR_AACK_PROM_MODE in
 * register XAH_CTRL_1 */
#define PROM_MODE_DISABLE               (0)

/** Constant PROM_MODE_ENABLE for sub-register @ref SR_AACK_PROM_MODE in
 * register XAH_CTRL_1 */
#define PROM_MODE_ENABLE                (1)

/** Access parameters for sub-register AACK_ACK_TIME in register @ref
 * RG_XAH_CTRL_1 */
#define SR_AACK_ACK_TIME                0x157, 0x04, 2

/** Constant ACK_TIME_12_SYMBOLS for sub-register @ref SR_AACK_ACK_TIME in
 * register XAH_CTRL_1 */
#define ACK_TIME_12_SYMBOLS             (0)

/** Constant ACK_TIME_2_SYMBOLS for sub-register @ref SR_AACK_ACK_TIME in
 * register XAH_CTRL_1 */
#define ACK_TIME_2_SYMBOLS              (1)

/** Access parameters for sub-register AACK_UPLD_RES_FT in register @ref
 * RG_XAH_CTRL_1 */
#define SR_AACK_UPLD_RES_FT             0x157, 0x10, 4

/** Access parameters for sub-register AACK_FLTR_RES_FT in register @ref
 * RG_XAH_CTRL_1 */
#define SR_AACK_FLTR_RES_FT             0x157, 0x20, 5

/** Offset for register FTN_CTRL */
#define RG_FTN_CTRL                     (0x158)
/** Sub-registers of Register @ref RG_FTN_CTRL */
/** Access parameters for sub-register FTN_START in register @ref RG_FTN_CTRL */
#define SR_FTN_START                    0x158, 0x80, 7

/** Offset for register PLL_CF */
#define RG_PLL_CF                       (0x15A)
/** Sub-registers of Register @ref RG_PLL_CF */

/** Access parameters for sub-register PLL_CF_START in register @ref RG_PLL_CF
**/
#define SR_PLL_CF_START                 0x15A, 0x80, 7

/** Offset for register PLL_DCU */
#define RG_PLL_DCU                      (0x15B)
/** Sub-registers of Register @ref RG_PLL_DCU */

/** Access parameters for sub-register PLL_DCU_START in register @ref RG_PLL_DCU
**/
#define SR_PLL_DCU_START                0x15B, 0x80, 7

/** Offset for register PART_NUM */
#define RG_PART_NUM                     (0x15C)

/** Constant ATMEGARFR2_PART_NUM for sub-register @ref SR_PART_NUM in register
 * PART_NUM */
#define ATMEGARFR2_PART_NUM             (0x94)

/** Offset for register VERSION_NUM */
#define RG_VERSION_NUM                  (0x15D)

/** Offset for register MAN_ID_0 */
#define RG_MAN_ID_0                     (0x15E)

/** Offset for register MAN_ID_1 */
#define RG_MAN_ID_1                     (0x15F)

/** Offset for register SHORT_ADDR_0 */
#define RG_SHORT_ADDR_0                 (0x160)

/** Offset for register SHORT_ADDR_1 */
#define RG_SHORT_ADDR_1                 (0x161)

/** Offset for register PAN_ID_0 */
#define RG_PAN_ID_0                     (0x162)

/** Offset for register PAN_ID_1 */
#define RG_PAN_ID_1                     (0x163)

/** Offset for register IEEE_ADDR_0 */
#define RG_IEEE_ADDR_0                  (0x164)

/** Offset for register IEEE_ADDR_1 */
#define RG_IEEE_ADDR_1                  (0x165)

/** Offset for register IEEE_ADDR_2 */
#define RG_IEEE_ADDR_2                  (0x166)

/** Offset for register IEEE_ADDR_3 */
#define RG_IEEE_ADDR_3                  (0x167)

/** Offset for register IEEE_ADDR_4 */
#define RG_IEEE_ADDR_4                  (0x168)

/** Offset for register IEEE_ADDR_5 */
#define RG_IEEE_ADDR_5                  (0x169)

/** Offset for register IEEE_ADDR_6 */
#define RG_IEEE_ADDR_6                  (0x16A)

/** Offset for register IEEE_ADDR_7 */
#define RG_IEEE_ADDR_7                  (0x16B)

/** Offset for register XAH_CTRL_0 */
#define RG_XAH_CTRL_0                   (0x16C)
/** Sub-registers of Register @ref RG_XAH_CTRL_0 */

/** Access parameters for sub-register SLOTTED_OPERATION in register @ref
 * RG_XAH_CTRL_0 */
#define SR_SLOTTED_OPERATION            0x16C, 0x01, 0

/** Constant SLOTTED_OPERATION_DISABLE for sub-register @ref
 * SR_SLOTTED_OPERATION in register XAH_CTRL_0 */
#define SLOTTED_OPERATION_DISABLE       (0)

/** Constant SLOTTED_OPERATION_EnABLE for sub-register @ref SR_SLOTTED_OPERATION
 * in register XAH_CTRL_0 */
#define SLOTTED_OPERATION_EnABLE        (1)

/** Access parameters for sub-register MAX_CSMA_RETRIES in register @ref
 * RG_XAH_CTRL_0 */
#define SR_MAX_CSMA_RETRIES             0x16C, 0x0E, 1

/** Access parameters for sub-register MAX_FRAME_RETRIES in register @ref
 * RG_XAH_CTRL_0 */
#define SR_MAX_FRAME_RETRIES            0x16C, 0xF0, 4

/** Offset for register CSMA_SEED_0 */
#define RG_CSMA_SEED_0                  (0x16D)

/** Offset for register CSMA_SEED_1 */
#define RG_CSMA_SEED_1                  (0x16E)
/** Sub-registers of Register @ref RG_CSMA_SEED_1 */

/** Access parameters for sub-register CSMA_SEED_1 in register @ref
 * RG_CSMA_SEED_1 */
#define SR_CSMA_SEED_1                  0x16E, 0x07, 0

/** Access parameters for sub-register AACK_I_AM_COORD in register @ref
 * RG_CSMA_SEED_1 */
#define SR_AACK_I_AM_COORD              0x16E, 0x08, 3

/** Constant I_AM_COORD_DISABLE for sub-register @ref SR_AACK_I_AM_COORD in
 * register CSMA_SEED_1 */
#define I_AM_COORD_DISABLE              (0)

/** Constant I_AM_COORD_ENABLE for sub-register @ref SR_AACK_I_AM_COORD in
 * register CSMA_SEED_1 */
#define I_AM_COORD_ENABLE               (1)

/** Access parameters for sub-register AACK_DIS_ACK in register @ref
 * RG_CSMA_SEED_1 */
#define SR_AACK_DIS_ACK                 0x16E, 0x10, 4

/** Constant ACK_DISABLE for sub-register @ref SR_AACK_DIS_ACK in register
 * CSMA_SEED_1 */
#define ACK_DISABLE                     (1)

/** Constant ACK_ENABLE for sub-register @ref SR_AACK_DIS_ACK in register
 * CSMA_SEED_1 */
#define ACK_ENABLE                      (0)

/** Access parameters for sub-register AACK_SET_PD in register @ref
 * RG_CSMA_SEED_1 */
#define SR_AACK_SET_PD                  0x16E, 0x20, 5

/** Constant PD_ACK_BIT_SET_DISABLE for sub-register @ref SR_AACK_SET_PD in
 * register CSMA_SEED_1 */
#define PD_ACK_BIT_SET_DISABLE          (0)

/** Constant PD_ACK_BIT_SET_ENABLE for sub-register @ref SR_AACK_SET_PD in
 * register CSMA_SEED_1 */
#define PD_ACK_BIT_SET_ENABLE           (1)

/** Access parameters for sub-register AACK_FVN_MODE in register @ref
 * RG_CSMA_SEED_1 */
#define SR_AACK_FVN_MODE                0x16E, 0xC0, 6

/** Constant FRAME_VERSION_0 for sub-register @ref SR_AACK_FVN_MODE in register
 * CSMA_SEED_1 */
#define FRAME_VERSION_0                 (0)

/** Constant FRAME_VERSION_01 for sub-register @ref SR_AACK_FVN_MODE in register
 * CSMA_SEED_1 */
#define FRAME_VERSION_01                (1)

/** Constant FRAME_VERSION_012 for sub-register @ref SR_AACK_FVN_MODE in
 * register CSMA_SEED_1 */
#define FRAME_VERSION_012               (2)

/** Constant FRAME_VERSION_IGNORED for sub-register @ref SR_AACK_FVN_MODE in
 * register CSMA_SEED_1 */
#define FRAME_VERSION_IGNORED           (3)

/** Offset for register CSMA_BE */
#define RG_CSMA_BE                      (0x16F)
/** Sub-registers of Register @ref RG_CSMA_BE */
/** Access parameters for sub-register MIN_BE in register @ref RG_CSMA_BE */
#define SR_MIN_BE                       0x16F, 0x0F, 0
/** Access parameters for sub-register MAX_BE in register @ref RG_CSMA_BE */
#define SR_MAX_BE                       0x16F, 0xF0, 4

/** Offset for register TST_CTRL_DIGI */
#define RG_TST_CTRL_DIGI                (0x176)
/** Sub-registers of Register @ref RG_TST_CTRL_DIGI */

/** Access parameters for sub-register TST_CTRL_DIG in register @ref
 * RG_TST_CTRL_DIGI */
#define SR_TST_CTRL_DIG                 0x176, 0x0F, 0

/** Offset for register TST_RX_LENGTH */
#define RG_TST_RX_LENGTH                (0x17B)

/** Offset for register TRXFBST */
#define RG_TRXFBST                      (0x180)

/** Offset for register TRXFBEND */
#define RG_TRXFBEND                     (0x1FF)

/** Enumerations */

/** Transceiver states */
typedef enum tal_trx_status_tag {
	/** Constant P_ON for sub-register @ref SR_TRX_STATUS */
	P_ON =                              (0x00),

	/** Constant BUSY_RX for sub-register @ref SR_TRX_STATUS */
	BUSY_RX =                           (0x01),

	/** Constant BUSY_TX for sub-register @ref SR_TRX_STATUS */
	BUSY_TX =                           (0x02),

	/** Constant RX_ON for sub-register @ref SR_TRX_STATUS */
	RX_ON =                             (0x06),

	/** Constant TRX_OFF for sub-register @ref SR_TRX_STATUS */
	TRX_OFF =                           (0x08),

	/** Constant PLL_ON for sub-register @ref SR_TRX_STATUS */
	PLL_ON =                            (0x09),

	/** Constant TRX_SLEEP for sub-register @ref SR_TRX_STATUS */
	TRX_SLEEP =                         (0x0F),

	/** Constant BUSY_RX_AACK for sub-register @ref SR_TRX_STATUS */
	BUSY_RX_AACK =                      (0x11),

	/** Constant BUSY_TX_ARET for sub-register @ref SR_TRX_STATUS */
	BUSY_TX_ARET =                      (0x12),

	/** Constant RX_AACK_ON for sub-register @ref SR_TRX_STATUS */
	RX_AACK_ON =                        (0x16),

	/** Constant TX_ARET_ON for sub-register @ref SR_TRX_STATUS */
	TX_ARET_ON =                        (0x19),

	/** Constant RX_ON_NOCLK for sub-register @ref SR_TRX_STATUS */
	RX_ON_NOCLK =                       (0x1C),

	/** Constant RX_AACK_ON_NOCLK for sub-register @ref SR_TRX_STATUS */
	RX_AACK_ON_NOCLK =                  (0x1D),

	/** Constant BUSY_RX_AACK_NOCLK for sub-register @ref SR_TRX_STATUS */
	BUSY_RX_AACK_NOCLK =                (0x1E),

	/** Constant STATE_TRANSITION_IN_PROGRESS for sub-register @ref
	 * SR_TRX_STATUS */
	STATE_TRANSITION_IN_PROGRESS =      (0x1F)
} SHORTENUM tal_trx_status_t;

/** sub-register TRX_CMD in register TRX_STATE */
typedef enum trx_cmd_tag {
	/** Constant CMD_NOP for sub-register @ref SR_TRX_CMD */
	CMD_NOP =                           (0x00),

	/** Constant CMD_TX_START for sub-register @ref SR_TRX_CMD */
	CMD_TX_START =                      (0x02),

	/** Constant CMD_FORCE_TRX_OFF for sub-register @ref SR_TRX_CMD */
	CMD_FORCE_TRX_OFF =                 (0x03),

	/** Constant CMD_FORCE_PLL_ON for sub-register @ref SR_TRX_CMD */
	CMD_FORCE_PLL_ON =                  (0x04),

	/** Constant CMD_RX_ON for sub-register @ref SR_TRX_CMD */
	CMD_RX_ON =                         (0x06),

	/** Constant CMD_TRX_OFF for sub-register @ref SR_TRX_CMD */
	CMD_TRX_OFF =                       (0x08),

	/** Constant CMD_PLL_ON for sub-register @ref SR_TRX_CMD */
	CMD_PLL_ON =                        (0x09),

	/** Constant CMD_RX_AACK_ON for sub-register @ref SR_TRX_CMD */
	CMD_RX_AACK_ON =                    (0x16),

	/** Constant CMD_TX_ARET_ON for sub-register @ref SR_TRX_CMD */
	CMD_TX_ARET_ON =                    (0x19),

	/** Software implemented command */
	CMD_SLEEP =                         (0x0F)
} SHORTENUM trx_cmd_t;

/** sub-register TRAC_STATUS in register TRX_STATE */
typedef enum trx_trac_status_tag {
	/** Constant TRAC_SUCCESS for sub-register @ref SR_TRAC_STATUS */
	TRAC_SUCCESS =                      (0),

	/** Constant TRAC_SUCCESS_DATA_PENDING for sub-register @ref
	 * SR_TRAC_STATUS */
	TRAC_SUCCESS_DATA_PENDING =         (1),

	/** Constant TRAC_SUCCESS_WAIT_FOR_ACK for sub-register @ref
	 * SR_TRAC_STATUS */
	TRAC_SUCCESS_WAIT_FOR_ACK =         (2),

	/** Constant TRAC_CHANNEL_ACCESS_FAILURE for sub-register @ref
	 * SR_TRAC_STATUS */
	TRAC_CHANNEL_ACCESS_FAILURE =       (3),

	/** Constant TRAC_NO_ACK for sub-register @ref SR_TRAC_STATUS */
	TRAC_NO_ACK =                       (5),

	/** Constant TRAC_INVALID for sub-register @ref SR_TRAC_STATUS */
	TRAC_INVALID =                      (7)
} SHORTENUM trx_trac_status_t;

#endif /* #if (TAL_TYPE == ATMEGARFR2) */

#endif /* ATMEGA256RFR2_H */
