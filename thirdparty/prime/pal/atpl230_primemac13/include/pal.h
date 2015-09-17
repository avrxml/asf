/**
 * \file
 *
 * \brief Physical Abstraction Layer
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

#ifndef PAL_H_INCLUDED
#define PAL_H_INCLUDED

#include "atpl230.h"

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/* @endcond */

/**
 * \ingroup prime_group
 * \defgroup pal_prime_group Physical Abstraction Layer
 *
 * This module provides configuration and utils for the PAL layer,
 * which is the interface between the PLC PHY layer and the PRIME
 * MAC layer.
 *
 * @{
 */

/* \name Impedance states */
/* @{ */
/* High Impedance */
#define PAL_HI_STATE                 HI_STATE
/* Low Impedance */
#define PAL_LO_STATE                 LO_STATE
/* Very Low Impedance */
#define PAL_VLO_STATE                VLO_STATE
/* Low Impedance + Peak Cut On */
#define PAL_LO_STATE_PK              LO_STATE_PK
/* @} */

/* \name QT null value */
/* @{ */
#define PAL_QT_UNKNOW                0xFF
/* @} */

/* \name PAL config success result */
/* @{ */
#define PAL_CFG_SUCCESS              PHY_CFG_SUCCESS
#define PAL_CFG_INVALID_INPUT        PHY_CFG_INVALID_INPUT
/* @} */

/* \name TX Result values */
/* @{ */
/* Transmission result: already in process */
#define PAL_TX_RESULT_PROCESS        PHY_TX_RESULT_PROCESS
/* Transmission result: end successfully */
#define PAL_TX_RESULT_SUCCESS        PHY_TX_RESULT_SUCCESS
/* Transmission result: invalid length error */
#define PAL_TX_RESULT_INV_LENGTH     PHY_TX_RESULT_INV_LENGTH
/* Transmission result: busy channel error */
#define PAL_TX_RESULT_BUSY_CH        PHY_TX_RESULT_BUSY_CH
/* Transmission result: busy transmission error */
#define PAL_TX_RESULT_BUSY_TX        PHY_TX_RESULT_BUSY_TX
/* Transmission result: busy reception error */
#define PAL_TX_RESULT_BUSY_RX        PHY_TX_RESULT_BUSY_RX
/* Transmission result: invalid scheme error */
#define PAL_TX_RESULT_INV_SCHEME     PHY_TX_RESULT_INV_SCHEME
/* Transmission result: timeout error */
#define PAL_TX_RESULT_TIMEOUT        PHY_TX_RESULT_TIMEOUT
/* Transmission result: invalid buffer identifier error */
#define PAL_TX_RESULT_INV_BUFFER     PHY_TX_RESULT_INV_BUFFER
/* Transmission result: invalid Prime Mode error */
#define PAL_TX_RESULT_INV_PRIME_MODE PHY_TX_RESULT_INV_PRIME_MODE
/* @} */

/* \name Protocol values */
/* @{ */
/* Modulation scheme of the payload: Differential BPSK */
#define PAL_PROTOCOL_DBPSK           PROTOCOL_DBPSK
/* Modulation scheme of the payload: Differential QPSK */
#define PAL_PROTOCOL_DQPSK           PROTOCOL_DQPSK
/* Modulation scheme of the payload: Differential 8PSK */
#define PAL_PROTOCOL_D8PSK           PROTOCOL_D8PSK
/* Modulation scheme of the payload: Differential BPSK with Convolutional Coding
 **/
#define PAL_PROTOCOL_DBPSK_VTB       PROTOCOL_DBPSK_VTB
/* Modulation scheme of the payload: Differential QPSK with Convolutional Coding
 **/
#define PAL_PROTOCOL_DQPSK_VTB       PROTOCOL_DQPSK_VTB
/* Modulation scheme of the payload: Differential 8PSK with Convolutional Coding
 **/
#define PAL_PROTOCOL_D8PSK_VTB       PROTOCOL_D8PSK_VTB
/* Modulation scheme of the payload: Differential BPSK with ROBO Mode */
#define PAL_PROTOCOL_DBPSK_ROBO      PROTOCOL_DBPSK_ROBO
/* Modulation scheme of the payload: Differential QPSK with ROBO Mode */
#define PAL_PROTOCOL_DQPSK_ROBO      PROTOCOL_DQPSK_ROBO
/* @} */

/* \name TX scheduling mode values */
/* @{ */
/* Absolute TX scheduling mode (absolute TX time specified) */
#define PAL_TIME_MODE_ABSOLUTE       PHY_TX_SCHEDULING_MODE_ABSOLUTE
/* Relative TX scheduling mode (delay for TX time specified) */
#define PAL_TIME_MODE_RELATIVE       PHY_TX_SCHEDULING_MODE_RELATIVE
/* @} */

/* \name  Mode values */
/* @{ */
/* PRIME v1.3 */
#define MODE_PRIME_V1_3              0x00
/* PRIME PLUS */
#define MODE_PRIME_PLUS              0x02
/* @} */

/* Valid chip configuration key */
#define PAL_VALID_PHY_CFG_KEY        ATPL230_VALID_CFG_KEY

/* \name PAL commands to manage ATPL230 registers */
/* @{ */
/* AND operation */
#define PAL_CMD_REG_AND              PHY_CMD_CFG_AND
/*  OR operation */
#define PAL_CMD_REG_OR               PHY_CMD_CFG_OR
/*  XOR operation */
#define PAL_CMD_REG_XOR              PHY_CMD_CFG_XOR
/* @} */

/* \name Parameters requested from MAC layer */
/* @{ */
enum pal_ids {
	PAL_ID_MAC_EN = 0,
	PAL_ID_STATS_RX_BAD_CRC = 1,
	PAL_ID_STATS_RX_FALSE_POSITIVE = 2,
	PAL_ID_TX_TEST_SEND = 3,
	PAL_ID_TX_TEST_SEND_BCN = 4,
	PAL_ID_QT = 5,
	PAL_ID_EX_SNR = 6,
	PAL_ID_CHECK_CFG = 7,
	PAL_ID_RESET_CFG = 8,
	PAL_ID_DISABLE_BCN_TX = 9,
	PAL_ID_ENABLE_CRC = 10,
	PAL_ID_INFO_VERSION = PHY_ID_INFO_VERSION,
	PAL_ID_IMPEDANCE = PHY_ID_CFG_IMPEDANCE,
	PAL_ID_AUTODETECT_LOAD = PHY_ID_CFG_AUTODETECT_BRANCH,
	PAL_ID_TX_LEVEL = PHY_ID_TX_INFO_LEVEL,
	PAL_ID_TX_SCHEME = PHY_ID_TX_INFO_SCHEME,
	PAL_ID_TXRX_CHANNEL = PHY_ID_CFG_TXRX_CHANNEL,
	PAL_ID_RX_BUFFER_ID = PHY_ID_RX_BUFFER_ID,
	PAL_ID_RX_SCHEME = PHY_ID_RX_INFO_SCHEME,
	PAL_ID_RX_RSSI_AVG_RX0 = REG_ATPL230_TXRXBUF_RSSIAVG_RX0,
	PAL_ID_RX_TIME_RX0 = REG_ATPL230_TXRXBUF_RECTIME1_RX0,
	PAL_ID_EVM_HEADER = REG_ATPL230_TXRXBUF_EVM_HEADER1_RX0,
	PAL_ID_EVM_PAYLOAD = REG_ATPL230_TXRXBUF_EVM_PAYLOAD1_RX0,
	PAL_ID_EVM_HEADER_ACUM = REG_ATPL230_TXRXBUF_EVM_HEADACUM1_RX0,
	PAL_ID_EVM_PAYLOAD_ACUM = REG_ATPL230_TXRXBUF_EVM_PAYLACUM1_RX0,
	PAL_ID_RX_PAYLOAD_LEN = PHY_ID_RX_PAYLOAD_LEN,
	PAL_ID_STATS_RX_TOTAL = PHY_ID_STATS_RX_TOTAL,
	PAL_ID_TX_BUFFER_ID = PHY_ID_TX_BUFFER_ID,
	PAL_ID_RMSCALC1_TX0 = REG_ATPL230_TXRXBUF_RMSCALC1_TX0,
	PAL_ID_EMITIME1_TX0 = REG_ATPL230_TXRXBUF_EMITIME1_TX0,
	PAL_ID_TIME_BEFORE_TX_HIGHZ = PHY_ID_CFG_TIME_BEFORE_TX_HIGHZ,
	PAL_ID_TIME_BEFORE_TX_LOWZ = PHY_ID_CFG_TIME_BEFORE_TX_LOWZ,
	PAL_ID_SNA = REG_ATPL230_SNA0,
};
/* @} */

/* \name Extended Threshold */
/* @{ */
#define PHY_EVM_TRESHOLD_EX_LEVELS      60

#define PHY_EVM_THRESHOLD_EX_0          0x5EC6B7
#define PHY_EVM_THRESHOLD_EX_1          0x5363F2
#define PHY_EVM_THRESHOLD_EX_2          0x495F57
#define PHY_EVM_THRESHOLD_EX_3          0x408ED4
#define PHY_EVM_THRESHOLD_EX_4          0x38CD66
#define PHY_EVM_THRESHOLD_EX_5          0x31FA7D
#define PHY_EVM_THRESHOLD_EX_6          0x2BF970
#define PHY_EVM_THRESHOLD_EX_7          0x26B10B
#define PHY_EVM_THRESHOLD_EX_8          0x220B1D
#define PHY_EVM_THRESHOLD_EX_9          0x1DF422
#define PHY_EVM_THRESHOLD_EX_10         0x1A5AEF
#define PHY_EVM_THRESHOLD_EX_11         0x173066
#define PHY_EVM_THRESHOLD_EX_12         0x14673D
#define PHY_EVM_THRESHOLD_EX_13         0x11F3C0
#define PHY_EVM_THRESHOLD_EX_14         0xFCBA5
#define PHY_EVM_THRESHOLD_EX_15         0xDE5DE
#define PHY_EVM_THRESHOLD_EX_16         0xC3A73
#define PHY_EVM_THRESHOLD_EX_17         0xAC260
#define PHY_EVM_THRESHOLD_EX_18         0x9777B
#define PHY_EVM_THRESHOLD_EX_19         0x85457
#define PHY_EVM_THRESHOLD_EX_20         0x7542D
#define PHY_EVM_THRESHOLD_EX_21         0x672C8
#define PHY_EVM_THRESHOLD_EX_22         0x5AC78
#define PHY_EVM_THRESHOLD_EX_23         0x4FDFA
#define PHY_EVM_THRESHOLD_EX_24         0x46473
#define PHY_EVM_THRESHOLD_EX_25         0x3DD5D
#define PHY_EVM_THRESHOLD_EX_26         0x36682
#define PHY_EVM_THRESHOLD_EX_27         0x2FDEE
#define PHY_EVM_THRESHOLD_EX_28         0x2A1EB
#define PHY_EVM_THRESHOLD_EX_29         0x250F5
#define PHY_EVM_THRESHOLD_EX_30         0x209B9
#define PHY_EVM_THRESHOLD_EX_31         0x1CB0C
#define PHY_EVM_THRESHOLD_EX_32         0x193E6
#define PHY_EVM_THRESHOLD_EX_33         0x16360
#define PHY_EVM_THRESHOLD_EX_34         0x138AF
#define PHY_EVM_THRESHOLD_EX_35         0x1131F
#define PHY_EVM_THRESHOLD_EX_36         0xF211
#define PHY_EVM_THRESHOLD_EX_37         0xD4FD
#define PHY_EVM_THRESHOLD_EX_38         0xBB66
#define PHY_EVM_THRESHOLD_EX_39         0xA4E3
#define PHY_EVM_THRESHOLD_EX_40         0x9114
#define PHY_EVM_THRESHOLD_EX_41         0x7FA6
#define PHY_EVM_THRESHOLD_EX_42         0x7050
#define PHY_EVM_THRESHOLD_EX_43         0x62D2
#define PHY_EVM_THRESHOLD_EX_44         0x56F3
#define PHY_EVM_THRESHOLD_EX_45         0x4C81
#define PHY_EVM_THRESHOLD_EX_46         0x4350
#define PHY_EVM_THRESHOLD_EX_47         0x3B3A
#define PHY_EVM_THRESHOLD_EX_48         0x341C
#define PHY_EVM_THRESHOLD_EX_49         0x2DDA
#define PHY_EVM_THRESHOLD_EX_50         0x2857
#define PHY_EVM_THRESHOLD_EX_51         0x237F
#define PHY_EVM_THRESHOLD_EX_52         0x1F3B
#define PHY_EVM_THRESHOLD_EX_53         0x1B7B
#define PHY_EVM_THRESHOLD_EX_54         0x182D
#define PHY_EVM_THRESHOLD_EX_55         0x1546
#define PHY_EVM_THRESHOLD_EX_56         0x12B7
#define PHY_EVM_THRESHOLD_EX_57         0x1078
#define PHY_EVM_THRESHOLD_EX_58         0xE7D
#define PHY_EVM_THRESHOLD_EX_59         0xCC0
/* @} */

/*  \name Data struct used for PLC transmission
 * \note In case of configure Absolute TX scheduling mode, tdelay means the
 * absolute time in 10's of us when TX starts, in case of configure Relative
 * Absolute TX scheduling mode, tdelay means the delay in 10's of us until
 * transmission begins. In case of reset both field (tdelay and tmode) the
 * transmission begins inmediately. */
/* @{ */
typedef struct {
	uint8_t uc_buff_id;  /*  Buffer identifier */
	uint8_t att_level;   /*  Attenuation level with which the message must
	                      * be transmitted */
	uint8_t scheme;      /*  Modulation scheme of last transmitted message
	                      **/
	uint8_t disable_rx;  /*  Disable reception */
	uint8_t mode;        /*  Mode PRIME v1.3, PRIME v1.4 or PRIME v1.4
	                      * backward compatible */
	uint16_t data_len;   /*  Length of the data buffer. */
	uint8_t *data_buf;   /*  Pointer to data buffer */
	uint32_t tdelay;     /*  Delay for transmission in 10's of us */
	uint8_t tmode;       /*  Time mode: 0: Absolute mode, 1: Differential
	                      * mode */
} xPalMsgTx_t;
/* @} */

/* \name Data struct used for PLC reception */
/* @{ */
typedef struct {
	uint8_t uc_buff_id;           /*  Buffer identifier */
	uint8_t scheme;               /*  Modulation scheme of the last
	                               * received message */
	uint8_t mode;                 /*  Mode PRIME v1.3, PRIME v1.4 or Noise
	                               * Capture */
	uint8_t header_type;          /*  Header Type of the last received
	                               * message */
	uint8_t noise_result;         /*  Noise result in case of noise capture
	                               * mode */
	uint8_t rssi_avg;             /*  Average RSSI (Received Signal
	                               * Strength Indication) (valid only when
	                               * rxQRMode is enable) */
	uint8_t cinr_avg;             /*  Average CNIR (Carrier to Interference
	                               * + Noise ratio) (valid only when
	                               * rxQRMode is enable) */
	uint16_t evm_header;          /*  Error Vector Magnitude for header
	                               * (valid only when rxQRMode is enable) */
	uint16_t evm_payload;         /*  Error Vector Magnitude for payload
	                               * (valid only when rxQRMode is enable) */
	uint16_t data_len;            /*  Length of the data buffer. */
	uint32_t evm_header_acum;     /*  Accumulated Error Vector Magnitude
	                               * for header (valid only when rxQRMode is
	                               * enable) */
	uint32_t evm_payload_acum;    /*  Accumulated Error Vector Magnitude
	                               * for payload (valid only when rxQRMode
	                               * is enable) */
	uint8_t *data_buf;            /*  Pointer to data buffer */
	uint32_t rx_time;             /*  Reception time in 10us. */
} xPalMsgRx_t;
/* @} */

/* \name PAL process interface */
/* @{ */
void pal_init(void);
void pal_process(void);

/* @} */

/* \name PAL standard interface */
/* @{ */
uint8_t pal_snr_get(uint8_t *puc_snr, uint8_t uc_qt);
uint8_t pal_zct_get(uint32_t *pul_zct);
uint8_t pal_timer_get(uint32_t *pul_timer);
uint8_t pal_cd_get(uint8_t *puc_cd, uint8_t *puc_rssi, uint32_t *pul_time,
		uint8_t *puc_header);
uint8_t pal_nl_get(uint8_t *puc_noise);
uint8_t pal_agc_get(uint8_t *puc_mode, uint8_t *puc_gain);
uint8_t pal_agc_set(uint8_t uc_mode, uint8_t uc_gain);

uint8_t pal_data_request(uint8_t *puc_mpdu, uint16_t us_len, uint8_t uc_level,
		uint8_t uc_scheme, uint32_t ul_time);
void pal_data_indication(uint8_t *puc_psdu, uint16_t *pus_len,
		uint8_t *puc_level, uint8_t *puc_scheme, uint32_t *pul_time);

/* @} */

/* \name PAL extended interface */
/* @{ */
uint8_t pal_data_request_ex(xPalMsgTx_t *px_msg);
void pal_data_indication_ex(xPalMsgRx_t *px_msg);
uint8_t pal_get_cfg_ex(uint16_t us_id, void *p_val, uint16_t uc_len);
uint8_t pal_set_cfg_ex(uint16_t us_id, void *p_val, uint16_t uc_len);
uint8_t pal_cmd_cfg_ex(uint16_t us_id, uint8_t uc_cmd, uint8_t uc_mask);

/* @} */

/* @} */

/* @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/* @endcond */

#endif /* PAL_H_INCLUDED */
