/**
 * \file atmegarfa1.h
 *
 * \brief ATMEGAxxxRFA1 registers description
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

#ifndef _ATMEGARFA1_H_
#define _ATMEGARFA1_H_

/*- Includes ---------------------------------------------------------------*/

/**
 * \ingroup group_phy
 * \defgroup group_phy_rfa1 ATMEGARFA1 PHY Layer
 * The ATmega128RFA1 is a low-power CMOS 8-bit microcontroller based on the AVR
 * enhanced RISC architecture combined with a high data rate transceiver for the
 * 2.4 GHz
 *  ISM band. It is derived from the ATmega1281 microcontroller and the
 * AT86RF231 radio transceiver.
 * \a Refer <A href="http://www.atmel.com/Images/doc8266.pdf">ATMEGARFA1 Data
 * Sheet </A> \b for \b detailed \b information .
 * @{
 */

/*- Definitions ------------------------------------------------------------*/
#define AES_BLOCK_SIZE                 16
#define RANDOM_NUMBER_UPDATE_INTERVAL  1 /* us */

/*- Types ------------------------------------------------------------------*/

/* Typed memory access macro */
#define MMIO_REG(mem_addr, type) (*(volatile type *)(mem_addr))

/* Transceiver Pin Register */
#define TRXPR_REG       MMIO_REG(0x139, uint8_t)
#define TRXPR_REG_s     MMIO_REG(0x139, struct __struct_TRXPR_REG)
struct __struct_TRXPR_REG {
	uint8_t trxrst : 1; /* Force Transceiver Reset */
	uint8_t slptr  : 1; /* Multi-purpose Transceiver Control Bit */
	uint8_t        : 6;
};

/* Transceiver Status Register */
#define TRX_STATUS_REG   MMIO_REG(0x141, uint8_t)
#define TRX_STATUS_REG_s MMIO_REG(0x141, struct __struct_TRX_STATUS_REG)
struct __struct_TRX_STATUS_REG {
	uint8_t trxStatus : 5; /* Transceiver Main Status */
	uint8_t tstStatus : 1; /* Test mode status */
	uint8_t ccaStatus : 1; /* CCA Status Result */
	uint8_t ccaDone   : 1; /* CCA Algorithm Status */
};

enum /* trxStatus values */
{
	TRX_STATUS_P_ON                         = 0,
	TRX_STATUS_BUSY_RX                      = 1,
	TRX_STATUS_BUSY_TX                      = 2,
	TRX_STATUS_RX_ON                        = 6,
	TRX_STATUS_TRX_OFF                      = 8,
	TRX_STATUS_PLL_ON                       = 9,
	TRX_STATUS_SLEEP                        = 15,
	TRX_STATUS_BUSY_RX_AACK                 = 17,
	TRX_STATUS_BUSY_TX_ARET                 = 18,
	TRX_STATUS_RX_AACK_ON                   = 22,
	TRX_STATUS_TX_ARET_ON                   = 25,
	TRX_STATUS_RX_ON_NOCLK                  = 28,
	TRX_STATUS_RX_AACK_ON_NOCLK             = 29,
	TRX_STATUS_BUSY_RX_AACK_NOCLK           = 30,
	TRX_STATUS_STATE_TRANSITION_IN_PROGRESS = 31
};

/* Transceiver State Control Register */
#define TRX_STATE_REG   MMIO_REG(0x142, uint8_t)
#define TRX_STATE_REG_s MMIO_REG(0x142, struct __struct_TRX_STATE_REG)
struct __struct_TRX_STATE_REG {
	uint8_t trxCmd     : 5; /* State Control Command */
	uint8_t tracStatus : 3; /* Transaction Status */
};

enum /* trxCmd values */
{
	TRX_CMD_NOP           = 0,
	TRX_CMD_TX_START      = 2,
	TRX_CMD_FORCE_TRX_OFF = 3,
	TRX_CMD_FORCE_PLL_ON  = 4,
	TRX_CMD_RX_ON         = 6,
	TRX_CMD_TRX_OFF       = 8,
	TRX_CMD_PLL_ON        = 9,
	TRX_CMD_RX_AACK_ON    = 22,
	TRX_CMD_TX_ARET_ON    = 25
};

enum /* tracStatus values */
{
	TRAC_STATUS_SUCCESS                = 0,
	TRAC_STATUS_SUCCESS_DATA_PENDING   = 1,
	TRAC_STATUS_SUCCESS_WAIT_FOR_ACK   = 2,
	TRAC_STATUS_CHANNEL_ACCESS_FAILURE = 3,
	TRAC_STATUS_NO_ACK                 = 5,
	TRAC_STATUS_INVALID                = 7,
};

/* Transceiver Control Register 1 */
#define TRX_CTRL_1_REG   MMIO_REG(0x144, uint8_t)
#define TRX_CTRL_1_REG_s MMIO_REG(0x144, struct __struct_TRX_CTRL_1_REG)
struct __struct_TRX_CTRL_1_REG {
	uint8_t              : 5;
	uint8_t txAutoCrcOn  : 1; /* Enable Automatic CRC Calculation */
	uint8_t irq2extEn    : 1; /* Connect Frame Start IRQ to TC1 */
	uint8_t paExtEn      : 1; /* External PA support enable */
};

/* Transceiver Transmit Power Control Register */
#define PHY_TX_PWR_REG   MMIO_REG(0x145, uint8_t)
#define PHY_TX_PWR_REG_s MMIO_REG(0x145, struct __struct_PHY_TX_PWR_REG)
struct __struct_PHY_TX_PWR_REG {
	uint8_t txPwr   : 4; /* Transmit Power Setting */
	uint8_t paLt    : 2; /* Power Amplifier Lead Time */
	uint8_t paBufLt : 2; /* Power Amplifier Buffer Lead Time */
};

enum /* txPwr values */
{
	TX_PWR_3_5_DBM      = 0x00,
	TX_PWR_3_3_DBM      = 0x01,
	TX_PWR_2_8_DBM      = 0x02,
	TX_PWR_2_3_DBM      = 0x03,
	TX_PWR_1_8_DBM      = 0x04,
	TX_PWR_1_2_DBM      = 0x05,
	TX_PWR_0_5_DBM      = 0x06,
	TX_PWR_MIN_0_5_DBM  = 0x07,
	TX_PWR_MIN_1_5_DBM  = 0x08,
	TX_PWR_MIN_2_5_DBM  = 0x09,
	TX_PWR_MIN_3_5_DBM  = 0x0A,
	TX_PWR_MIN_4_5_DBM  = 0x0B,
	TX_PWR_MIN_6_5_DBM  = 0x0C,
	TX_PWR_MIN_8_5_DBM  = 0x0D,
	TX_PWR_MIN_11_5_DBM = 0x0E,
	TX_PWR_MIN_16_5_DBM = 0x0F,
};

enum /* paLt values */
{
	TX_PWR_PA_LT_2US = 0,
	TX_PWR_PA_LT_4US = 1,
	TX_PWR_PA_LT_6US = 2,
	TX_PWR_PA_LT_8US = 3,
};

enum /* paBufLt values */
{
	TX_PWR_PA_BUF_LT_0US = 0,
	TX_PWR_PA_BUF_LT_2US = 1,
	TX_PWR_PA_BUF_LT_4US = 2,
	TX_PWR_PA_BUF_LT_6US = 3,
};

/* Receiver Signal Strength Indicator Register */
#define PHY_RSSI_REG    MMIO_REG(0x146, uint8_t)
#define PHY_RSSI_REG_s  MMIO_REG(0x146, struct __struct_PHY_RSSI_REG)
struct __struct_PHY_RSSI_REG {
	uint8_t rssi       : 5; /* Receiver Signal Strength Indicator */
	uint8_t rndValue   : 2; /* Random Value */
	uint8_t rxCrcValid : 1; /* Received Frame CRC Status */
};

/* Transceiver Energy Detection Level Register */
#define PHY_ED_LEVEL_REG MMIO_REG(0x147, uint8_t)

/* Transceiver Clear Channel Assessment (CCA) Control Register */
#define PHY_CC_CCA_REG   MMIO_REG(0x148, uint8_t)
#define PHY_CC_CCA_REG_s MMIO_REG(0x148, struct __struct_PHY_CC_CCA_REG)
struct __struct_PHY_CC_CCA_REG {
	uint8_t channel    : 5; /* RX/TX Channel Selection */
	uint8_t ccaMode    : 2; /* Select CCA Measurement Mode */
	uint8_t ccaRequest : 1; /* Manual CCA Measurement Request */
};

/* Transceiver CCA Threshold Setting Register */
#define CCA_THRES_REG   MMIO_REG(0x149, uint8_t)
#define CCA_THRES_REG_s MMIO_REG(0x149, struct __struct_CCA_THRES_REG)
struct __struct_CCA_THRES_REG {
	uint8_t ccaEdThres : 4; /* ED Threshold Level for CCA Measurement */
	uint8_t ccaCsThres : 4; /* CS Threshold Level for CCA Measurement */
};

/* Transceiver Receive Control Register */
#define RX_CTRL_REG     MMIO_REG(0x14A, uint8_t)
#define RX_CTRL_REG_s   MMIO_REG(0x14A, struct __struct_RX_CTRL_REG)
struct __struct_RX_CTRL_REG {
	uint8_t pdtThres : 4; /* Receiver Sensitivity Control */
	uint8_t          : 4;
};

/* Start of Frame Delimiter Value Register */
#define SFD_VALUE_REG   MMIO_REG(0x14B, uint8_t)

/* Transceiver Control Register 2 */
#define TRX_CTRL_2_REG   MMIO_REG(0x14C, uint8_t)
#define TRX_CTRL_2_REG_s MMIO_REG(0x14C, struct __struct_TRX_CTRL_2_REG)
struct __struct_TRX_CTRL_2_REG {
	uint8_t oqpskDataRate : 2; /* Data Rate Selection */
	uint8_t               : 5;
	uint8_t rxSafeMode    : 1; /* RX Safe Mode */
};

/* Antenna Diversity Control Register */
#define ANT_DIV_REG     MMIO_REG(0x14D, uint8_t)
#define ANT_DIV_REG_s   MMIO_REG(0x14D, struct __struct_ANT_DIV_REG)
struct __struct_ANT_DIV_REG {
	uint8_t antCtrl    : 2; /* Static Antenna Diversity Switch Control */
	uint8_t antExtSwEn : 1; /* Enable External Antenna Switch Control */
	uint8_t antDivEn   : 1; /* Enable Antenna Diversity */
	uint8_t            : 3;
	uint8_t antSel     : 1; /* Antenna Diversity Antenna Status */
};

/* Transceiver Interrupt Enable Register */
#define IRQ_MASK_REG    MMIO_REG(0x14E, uint8_t)
#define IRQ_MASK_REG_s  MMIO_REG(0x14E, struct __struct_IRQ_MASK_REG)
struct __struct_IRQ_MASK_REG {
	uint8_t pllLockEn    : 1; /* PLL Lock Interrupt Enable */
	uint8_t pllUnlockEn  : 1; /* PLL Unlock Interrupt Enable */
	uint8_t rxStartEn    : 1; /* RX_START Interrupt Enable */
	uint8_t rxEndEn      : 1; /* RX_END Interrupt Enable */
	uint8_t ccaEdDoneEn  : 1; /* End of ED Measurement Interrupt Enable */
	uint8_t amiEn        : 1; /* Address Match Interrupt Enable */
	uint8_t txEndEn      : 1; /* TX_END Interrupt Enable */
	uint8_t awakeEn      : 1; /* Awake Interrupt Enable */
};

/* Transceiver Interrupt Status Register */
#define IRQ_STATUS_REG   MMIO_REG(0x14F, uint8_t)
#define IRQ_STATUS_REG_s MMIO_REG(0x14F, struct __struct_IRQ_STATUS_REG)
struct __struct_IRQ_STATUS_REG {
	uint8_t pllLock    : 1; /* PLL Lock Interrupt Status */
	uint8_t pllUnlock  : 1; /* PLL Unlock Interrupt Status */
	uint8_t rxStart    : 1; /* RX_START Interrupt Status */
	uint8_t rxEnd      : 1; /* RX_END Interrupt Status */
	uint8_t ccaEdDone  : 1; /* End of ED Measurement Interrupt Status */
	uint8_t ami        : 1; /* Address Match Interrupt Status */
	uint8_t txEnd      : 1; /* TX_END Interrupt Status */
	uint8_t awake      : 1; /* Awake Interrupt Status */
};

#define IRQ_STATUS_CLEAR_VALUE    0xff

/* Voltage Regulator Control and Status Register */
#define VREG_CTRL_REG   MMIO_REG(0x150, uint8_t)
#define VREG_CTRL_REG_s MMIO_REG(0x150, struct __struct_VREG_CTRL_REG)
struct __struct_VREG_CTRL_REG {
	uint8_t           : 2;
	uint8_t dvddOk    : 1; /* DVDD Supply Voltage Valid */
	uint8_t           : 3;
	uint8_t avddOk    : 1; /* AVDD Supply Voltage Valid */
	uint8_t avregExt  : 1; /* Use External AVDD Regulator */
};

/* Battery Monitor Control and Status Register */
#define BATMON_REG      MMIO_REG(0x151, uint8_t)
#define BATMON_REG_s    MMIO_REG(0x151, struct __struct_BATMON_REG)
struct __struct_BATMON_REG {
	uint8_t batmonVth : 4; /* Battery Monitor Threshold Voltage */
	uint8_t batmonHr  : 1; /* Battery Monitor Voltage Range */
	uint8_t batmonOk  : 1; /* Battery Monitor Status */
	uint8_t batLowEn  : 1; /* Battery Monitor Interrupt Enable */
	uint8_t batLow    : 1; /* Battery Monitor Interrupt Status */
};

/* Crystal Oscillator Control Register */
#define XOSC_CTRL_REG   MMIO_REG(0x152, uint8_t)
#define XOSC_CTRL_REG_s MMIO_REG(0x152, struct __struct_XOSC_CTRL_REG)
struct __struct_XOSC_CTRL_REG {
	uint8_t xtalTrim : 4; /* Crystal Oscillator Load Capacitance Trimming */
	uint8_t xtalMode : 4; /* Crystal Oscillator Operating Mode */
};

/* Transceiver Receiver Sensitivity Control Register */
#define RX_SYN_REG      MMIO_REG(0x155, uint8_t)
#define RX_SYN_REG_s    MMIO_REG(0x155, struct __struct_RX_SYN_REG)
struct __struct_RX_SYN_REG {
	uint8_t rxPdtLevel : 4; /* Reduce Receiver Sensitivity */
	uint8_t            : 3;
	uint8_t rxPdtDis   : 1; /* Prevent Frame Reception */
};

/* Transceiver Acknowledgment Frame Control Register 1 */
#define XAH_CTRL_1_REG   MMIO_REG(0x157, uint8_t)
#define XAH_CTRL_1_REG_s MMIO_REG(0x157, struct __struct_XAH_CTRL_1_REG)
struct __struct_XAH_CTRL_1_REG {
	uint8_t               : 1;
	uint8_t aackPromMode  : 1; /* Enable Acknowledgment in Promiscuous Mode
	                            **/
	uint8_t aackAckTime   : 1; /* Reduce Acknowledgment Time */
	uint8_t               : 1;
	uint8_t aackUpldResFt : 1; /* Process Reserved Frames */
	uint8_t aackFltrResFt : 1; /* Filter Reserved Frames */
	uint8_t               : 2;
};

/* Transceiver Filter Tuning Control Register */
#define FTN_CTRL_REG    MMIO_REG(0x158, uint8_t)
#define FTN_CTRL_REG_s  MMIO_REG(0x158, struct __struct_FTN_CTRL_REG)
struct __struct_FTN_CTRL_REG {
	uint8_t          : 7;
	uint8_t ftnStart : 1; /* Start Calibration Loop of Filter Tuning Network
	                       **/
};

/* Transceiver Center Frequency Calibration Control Register */
#define PLL_CF_REG      MMIO_REG(0x15A, uint8_t)
#define PLL_CF_REG_s    MMIO_REG(0x15A, struct __struct_PLL_CF_REG)
struct __struct_PLL_CF_REG {
	uint8_t            : 7;
	uint8_t pllCfStart : 1; /* Start Center Frequency Calibration */
};

/* Transceiver Delay Cell Calibration Control Register */
#define PLL_DCU_REG     MMIO_REG(0x15B, uint8_t)
#define PLL_DCU_REG_s   MMIO_REG(0x15B, struct __struct_PLL_DCU_REG)
struct __struct_PLL_DCU_REG {
	uint8_t             : 7;
	uint8_t pllDcuStart : 1; /* Start Delay Cell Calibration */
};

/* Device Identification Register (Part Number) */
#define PART_NUM_REG    MMIO_REG(0x15C, uint8_t)

/* Device Identification Register (Version Number) */
#define VERSION_NUM_REG MMIO_REG(0x15D, uint8_t)

/* Device Identification Registers */
#define MAN_ID_0_REG    MMIO_REG(0x15E, uint8_t)
#define MAN_ID_1_REG    MMIO_REG(0x15F, uint8_t)

/* Transceiver MAC Short Address Registers */
#define SHORT_ADDR_0_REG MMIO_REG(0x160, uint8_t)
#define SHORT_ADDR_1_REG MMIO_REG(0x161, uint8_t)

/* Transceiver Personal Area Network ID Registers */
#define PAN_ID_0_REG    MMIO_REG(0x162, uint8_t)
#define PAN_ID_1_REG    MMIO_REG(0x163, uint8_t)

/* Transceiver MAC IEEE Address Registers */
#define IEEE_ADDR_0_REG MMIO_REG(0x164, uint8_t)
#define IEEE_ADDR_1_REG MMIO_REG(0x165, uint8_t)
#define IEEE_ADDR_2_REG MMIO_REG(0x166, uint8_t)
#define IEEE_ADDR_3_REG MMIO_REG(0x167, uint8_t)
#define IEEE_ADDR_4_REG MMIO_REG(0x168, uint8_t)
#define IEEE_ADDR_5_REG MMIO_REG(0x169, uint8_t)
#define IEEE_ADDR_6_REG MMIO_REG(0x16A, uint8_t)
#define IEEE_ADDR_7_REG MMIO_REG(0x16B, uint8_t)

/* Transceiver Extended Operating Mode Control Register */
#define XAH_CTRL_0_REG   MMIO_REG(0x16C, uint8_t)
#define XAH_CTRL_0_REG_s MMIO_REG(0x16C, struct __struct_XAH_CTRL_0_REG)
struct __struct_XAH_CTRL_0_REG {
	uint8_t slottedOperation : 1; /* Set Slotted Acknowledgment */
	uint8_t maxCsmaRetries   : 3; /* Maximum Number of CSMA-CA Procedure
	                               * Repetition Attempts */
	uint8_t maxFrameRetries  : 4; /* Maximum Number of Frame Re-transmission
	                               * Attempts */
};

/* Transceiver CSMA-CA Random Number Generator Seed Register */
#define CSMA_SEED_0_REG MMIO_REG(0x16D, uint8_t)

/* Transceiver Acknowledgment Frame Control Register 2 */
#define CSMA_SEED_1_REG   MMIO_REG(0x16E, uint8_t)
#define CSMA_SEED_1_REG_s MMIO_REG(0x16E, struct __struct_CSMA_SEED_1_REG)
struct __struct_CSMA_SEED_1_REG {
	uint8_t csmaSeed_1   : 3; /* Seed Value for CSMA Random Number Generator
	                           **/
	uint8_t aackIAmCoord : 1; /* Set Personal Area Network Coordinator */
	uint8_t aackDisAck   : 1; /* Disable Acknowledgment Frame Transmission
	                           **/
	uint8_t aackSetPd    : 1; /* Set Frame Pending Sub-field */
	uint8_t aackFvnMode  : 2; /* Acknowledgment Frame Filter Mode */
};

/* Transceiver CSMA-CA Back-off Exponent Control Register */
#define CSMA_BE_REG     MMIO_REG(0x16F, uint8_t)
#define CSMA_BE_REG_s   MMIO_REG(0x16F, struct __struct_CSMA_BE_REG)
struct __struct_CSMA_BE_REG {
	uint8_t minBe : 4; /* Minimum Back-off Exponent */
	uint8_t maxBe : 4; /* Maximum Back-off Exponent */
};

/* Transceiver Digital Test Control Register */
#define TST_CTRL_DIGI_REG MMIO_REG(0x176, uint8_t)

/* Transceiver Received Frame Length Register */
#define TST_RX_LENGTH_REG MMIO_REG(0x17B, uint8_t)

/* Frame buffer access macro */
#define TRX_FRAME_BUFFER(index) MMIO_REG(0x180 + (index), uint8_t)

#define AES_CTRL_IM             2
#define AES_CTRL_DIR            3
#define AES_CTRL_MODE           5
#define AES_CTRL_REQUEST        7

#define AES_STATUS_RY           0
#define AES_STATUS_ER           7
/** @} */
#endif /* _ATMEGARFA1_H_ */
