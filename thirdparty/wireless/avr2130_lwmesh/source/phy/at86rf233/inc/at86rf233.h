/**
 * \file at86rf233.h
 *
 * \brief AT86RF233 registers description and interface
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

#ifndef _AT86RF233_H_
#define _AT86RF233_H_

/**
 * \ingroup group_phy
 * \defgroup group_phy_233 AT86RF233 PHY Layer
 * The AT86RF233 is a feature rich, low-power 2.4 GHz radio transceiver designed
 * for industrial
 *  and consumer ZigBee/IEEE 802.15.4, 6LoWPAN, RF4CE and high data rate sub
 * 1GHz  ISM band applications
 * \a Refer <A href="http://www.atmel.com/Images/doc8111.pdf">AT86RF233 Data
 * Sheet </A> \b for \b detailed \b information .
 * @{
 */

/*- Definitions ------------------------------------------------------------*/
#define AES_BLOCK_SIZE                 16
#define AES_CORE_CYCLE_TIME            24 /* us */
#define RANDOM_NUMBER_UPDATE_INTERVAL  1 /* us */

/*- Types ------------------------------------------------------------------*/
#define TRX_STATUS_REG    0x01
#define TRX_STATE_REG     0x02
#define TRX_CTRL_0_REG    0x03
#define TRX_CTRL_1_REG    0x04
#define PHY_TX_PWR_REG    0x05
#define PHY_RSSI_REG      0x06
#define PHY_ED_LEVEL_REG  0x07
#define PHY_CC_CCA_REG    0x08
#define CCA_THRES_REG     0x09
#define RX_CTRL_REG       0x0a
#define SFD_VALUE_REG     0x0b
#define TRX_CTRL_2_REG    0x0c
#define ANT_DIV_REG       0x0d
#define IRQ_MASK_REG      0x0e
#define IRQ_STATUS_REG    0x0f
#define VREG_CTRL_REG     0x10
#define BATMON_REG        0x11
#define XOSC_CTRL_REG     0x12
#define RX_SYN_REG        0x15
#define TRX_RPC_REG       0x16
#define XAH_CTRL_1_REG    0x17
#define FTN_CTRL_REG      0x18
#define XAH_CTRL_2_REG    0x19
#define PLL_CF_REG        0x1a
#define PLL_DCU_REG       0x1b
#define PART_NUM_REG      0x1c
#define VERSION_NUM_REG   0x1d
#define MAN_ID_0_REG      0x1e
#define MAN_ID_1_REG      0x1f
#define SHORT_ADDR_0_REG  0x20
#define SHORT_ADDR_1_REG  0x21
#define PAN_ID_0_REG      0x22
#define PAN_ID_1_REG      0x23
#define IEEE_ADDR_0_REG   0x24
#define IEEE_ADDR_1_REG   0x25
#define IEEE_ADDR_2_REG   0x26
#define IEEE_ADDR_3_REG   0x27
#define IEEE_ADDR_4_REG   0x28
#define IEEE_ADDR_5_REG   0x29
#define IEEE_ADDR_6_REG   0x2a
#define IEEE_ADDR_7_REG   0x2b
#define XAH_CTRL_0_REG    0x2c
#define CSMA_SEED_0_REG   0x2d
#define CSMA_SEED_1_REG   0x2e
#define CSMA_BE_REG       0x2f
#define TST_CTRL_DIGI_REG 0x36
#define PHY_TX_TIME_REG   0x3b
#define PHY_PMU_VALUE_REG 0x3b
#define TST_AGC_REG       0x3c
#define TST_SDM_REG       0x3d

#define AES_STATUS_REG    0x82
#define AES_CTRL_REG      0x83
#define AES_KEY_REG       0x84
#define AES_STATE_REG     0x84
#define AES_CTRL_M_REG    0x94

/* TRX_STATUS */
#define CCA_DONE          7
#define CCA_STATUS        6
#define TRX_STATUS        0

/* TRX_STATE */
#define TRAC_STATUS       5
#define TRX_CMD           0

/* TRX_CTRL_0 */
#define TOM_EN            7
#define PMU_EN            4
#define PMU_IF_INVERSE    4
#define CLKM_SHA_SEL      3
#define CLKM_CTRL         0

/* TRX_CTRL_1 */
#define PA_EXT_EN         7
#define IRQ_2_EXT_EN      6
#define TX_AUTO_CRC_ON    5
#define RX_BL_CTRL        4
#define SPI_CMD_MODE      2
#define IRQ_MASK_MODE     1
#define IRQ_POLARITY      0

/* PHY_TX_PWR */
#define TX_PWR            0

/* PHY_RSSI */
#define RX_CRC_VALID      7
#define RND_VALUE         5
#define RSSI              0

/* PHY_CC_CCA */
#define CCA_REQUEST       7
#define CCA_MODE          5
#define CHANNEL           0

/* CCA_THRES */
#define CCA_ED_THRES      0

/* RX_CTRL_REG */
#define PEL_SHIFT_VALUE   6
#define PDT_THRES         0

/* TRX_CTRL_2 */
#define RX_SAFE_MODE      7
#define OQPSK_SCRAM_EN    5
#define OQPSK_DATA_RATE   0

/* ANT_DIV */
#define ANT_SEL           7
#define ANT_DIV_EN        3
#define ANT_EXT_SW_EN     2
#define ANT_CTRL          0

/* IRQ_MASK, IRQ_STATUS */
#define BAT_LOW           7
#define TRX_UR            6
#define AMI               5
#define CCA_ED_DONE       4
#define TRX_END           3
#define RX_START          2
#define PLL_UNLOCK        1
#define PLL_LOCK          0

/* VREG_CTRL */
#define AVREG_EXT         7
#define AVDD_OK           6
#define DVREG_EXT         3
#define DVDD_OK           2

/* BATMON */
#define BATMON_OK         5
#define BATMON_HR         4
#define BATMON_VTH        0

/* XOSC_CTRL */
#define XTAL_MODE         4
#define XTAL_TRIM         0

/* RX_SYN */
#define RX_PDT_DIS        7
#define RX_PDT_LEVEL      0

/* TRX_RPC */
#define RX_RPC_CTRL       6
#define RX_RPC_EN         5
#define PDT_RPC_EN        4
#define PLL_RPC_EN        3
#define XAH_TX_RPC_EN     2
#define IPAN_RPC_EN       1

/* XAH_CTRL_1 */
#define ARET_TX_TS_EN     7
#define AACK_FLTR_RES_FT  5
#define AACK_UPLD_RES_FT  4
#define AACK_ACK_TIME     2
#define AACK_PROM_MODE    1
#define AACK_SPC_EN       0

/* FTN_CTRL */
#define FTN_START         7
#define FTNV              0

/* XAH_CTRL_2 */
#define ARET_FRAME_RETRIES 4
#define ARET_CSMA_RETRIES 1

/* PLL_CF */
#define PLL_CF_START      7
#define PLL_CF            0

/* PLL_DCU */
#define PLL_DCU_START     7

/* XAH_CTRL_0 */
#define MAX_FRAME_RETRES  4
#define MAX_CSMA_RETRES   1
#define SLOTTED_OPERATION 0

/* CSMA_SEED_1 */
#define AACK_FVN_MODE     6
#define AACK_SET_PD       5
#define AACK_DIS_ACK      4
#define AACK_I_AM_COORD   3
#define CSMA_SEED_1       0

/* CSMA_BE */
#define MAX_BE            4
#define MIN_BE            0

/* TST_CTRL_DIGI */
#define TST_CTRL_DIG      0

/* PHY_TX_TIME_REG */
#define IRC_TX_TIME       0

/* TST_AGC_REG */
#define AGC_HOLD_SEL      5
#define AGC_RST           4
#define AGC_OFF           3
#define AGC_HOLD          2
#define GC                0

/* TST_SDM_REG */
#define MOD_SEL           7
#define MOD               6
#define TX_RX             5
#define TX_RX_SEL         4

/* AES_CTRL */
#define AES_CTRL_DIR      3
#define AES_CTRL_MODE     4
#define AES_CTRL_REQUEST  7

/* AES_STATUS */
#define AES_STATUS_DONE   0
#define AES_STATUS_ER     7

#define RF_CMD_REG_W      ((1 << 7) | (1 << 6))
#define RF_CMD_REG_R      ((1 << 7) | (0 << 6))
#define RF_CMD_FRAME_W    ((0 << 7) | (1 << 6) | (1 << 5))
#define RF_CMD_FRAME_R    ((0 << 7) | (0 << 6) | (1 << 5))
#define RF_CMD_SRAM_W     ((0 << 7) | (1 << 6) | (0 << 5))
#define RF_CMD_SRAM_R     ((0 << 7) | (0 << 6) | (0 << 5))

#define TRX_CMD_NOP           0
#define TRX_CMD_TX_START      2
#define TRX_CMD_FORCE_TRX_OFF 3
#define TRX_CMD_FORCE_PLL_ON  4
#define TRX_CMD_RX_ON         6
#define TRX_CMD_TRX_OFF       8
#define TRX_CMD_PLL_ON        9
#define TRX_CMD_RX_AACK_ON    22
#define TRX_CMD_TX_ARET_ON    25

#define TRX_STATUS_P_ON               0
#define TRX_STATUS_BUSY_RX            1
#define TRX_STATUS_BUSY_TX            2
#define TRX_STATUS_RX_ON              6
#define TRX_STATUS_TRX_OFF            8
#define TRX_STATUS_PLL_ON             9
#define TRX_STATUS_SLEEP              15
#define TRX_STATUS_BUSY_RX_AACK       17
#define TRX_STATUS_BUSY_TX_ARET       18
#define TRX_STATUS_RX_AACK_ON         22
#define TRX_STATUS_TX_ARET_ON         25
#define TRX_STATUS_RX_ON_NOCLK        28
#define TRX_STATUS_RX_AACK_ON_NOCLK   29
#define TRX_STATUS_BUSY_RX_AACK_NOCLK 30
#define TRX_STATUS_STATE_TRANSITION   31
#define TRX_STATUS_MASK               0x1f

#define TRAC_STATUS_SUCCESS                0
#define TRAC_STATUS_SUCCESS_DATA_PENDING   1
#define TRAC_STATUS_SUCCESS_WAIT_FOR_ACK   2
#define TRAC_STATUS_CHANNEL_ACCESS_FAILURE 3
#define TRAC_STATUS_NO_ACK                 5
#define TRAC_STATUS_INVALID                7

/* ! @} */
#endif /* _AT86RF233_H_ */
