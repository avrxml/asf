/**
 * \file perf_msg_const.h
 *
 * \brief This file defines all Req, confirm, Indication message constants.
 * - Performance Analyzer application
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
 */

/*
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* Prevent double inclusion */
#ifndef PERF_MSG_CONST_H
#define PERF_MSG_CONST_H

/**
 * \addtogroup group_serial_parser
 * \{
 */
/* === Includes ============================================================= */

/* === Macros =============================================================== */

/**
 * This indicates the PROTOCOL_ID field in the Serial Parser frame format.
 *
 */

#define PROTOCOL_ID                     (0x00) /*i.e TAL- Performance Analyzer
	                                        * Application*/

/**
 * \name Command Lengths
 * \{
 */

#define PROTOCOL_ID_LEN                 (1)
#define OCTET_STR_LEN_BYTE_LEN          (1)
#define CMD_ID_LEN                      (1)

#define IDENTIFY_BOARD_CONFIRM_LEN      (19)
#define PERF_START_CONFIRM_LEN          (31)
#define PERF_SET_CONFIRM_LEN            (3)
#define PERF_GET_CONFIRM_LEN            (3)
#define IDENTIFY_PEER_NODE_CONFIRM_LEN  (2)
#define CONT_PULSE_TX_CONFIRM_LEN       (2)
#define CONT_WAVE_TX_CONFIRM_LEN        (4)
#define REGISTER_READ_CONFIRM_LEN       (5)
#define REGISTER_WRITE_CONFIRM_LEN      (5)
#define REGISTER_DUMP_CONFIRM_LEN       (6)
#define PKT_STREAM_CONFIRM_LEN          (3)
#define RX_ON_CONFIRM_LEN                           (3)
#define ED_SCAN_START_CONFIRM_LEN       (7)
#define ED_SCAN_END_INDICATION_LEN      (2)
#define SENSOR_DATA_CONFIRM_LEN         (10)
#define PER_TEST_START_CONFIRM_LEN      (2)
#define RANGE_TEST_START_CONFIRM_LEN    (2)
#define RANGE_TEST_PKT_LEN              (RANGE_TEST_PKT_LENGTH - FCS_LEN + \
	LENGTH_FIELD_LEN + 1)
#define RANGE_TEST_RSP_PKT_LEN          (5) /*Excluding ota packet*/
#define RANGE_TEST_MARKER_IND_LEN       (3) /*Excluding ota packet*/
#define RANGE_TEST_STOP_CONFIRM_LEN     (2)
#define PER_TEST_END_INDICATION_LEN     (36)
#define PEER_DISCONNECT_CONFIRM_LEN     (2)
#define SET_DEFAULT_CONFIG_CONFIRM_LEN  (22)
#define GET_CURRENT_CONFIG_CONFIRM_LEN  (26)
#define RANGE_MEASURE_STATS_CONFIRM     (10)

/* ! \} */

/**
 * \name Error Codes
 * \{
 */

/* error codes */
#define INVALID_CMD                       (0x20)
#define ED_SCAN_UNDER_PROGRESS            (0x21)
#define TRANSMISSION_UNDER_PROGRESS       (0x22)
#define CW_TRANSMISSION_UNDER_PROGRESS    (0x23)
#define NO_PEER_FOUND                     (0x24)
#define UNABLE_TO_CONTACT_PEER            (0x25)
#define INVALID_ARGUMENT                  (0x26)
#define VALUE_OUT_OF_RANGE                (0x27)
#define INVALID_REGISTER_ORDER            (0x28)
#define TRANSCEIVER_IN_SLEEP              (0x29)
#define TRANSMISSION_FAILURE              (0x30)
#define RANGE_TEST_IN_PROGRESS            (0x31)
#define PKT_STREAM_IN_PROGRESS            (0x32)
#define RX_ON_MODE_IN_PROGRESS            (0x33)

/* ! \} */

/**
 * \name Parameter Identifiers
 * \{
 */

/* Parameter Identifiers */
#define PARAM_CHANNEL                     (0x00)
#define PARAM_CHANNEL_PAGE                (0x01)
#if ((TAL_TYPE != AT86RF212) && (TAL_TYPE != AT86RF212B))
#define PARAM_TX_POWER_REG                (0x02)
#endif
#define PARAM_TX_POWER_DBM                (0x03)
#define PARAM_CSMA                        (0x04)
#define PARAM_FRAME_RETRY                 (0x05)
#define PARAM_ACK_REQUEST                 (0x06)
#define PARAM_ANT_DIVERSITY               (0x07)
#define PARAM_ANT_DIVERSITY_ON_PEER       (0x08)
#define PARAM_DESENSITIZATION             (0x09)
#define PARAM_TRX_STATE                   (0x0a)
#define PARAM_CRC_ON_PEER                 (0x0b)
#define PARAM_NO_OF_TEST_FRAMES           (0x0c)
#define PARAM_PHY_FRAME_LENGTH            (0x0d)
#define PARAM_RPC                         (0x0e)
#define PARAM_ISM_FREQUENCY               (0x0f)

/* ! \} */

/**
 * \name MACROS to set the specific features supported
 * \{
 */

/* MACROS to set the specific features supported */
#define MULTI_CHANNEL_SELECT              ((uint32_t)(1) << 0)
#define PER_RANGE_TEST_MODE               ((uint32_t)(1) << 1)
#define PER_REMOTE_CONFIG_MODE            ((uint32_t)(1) << 2)
#define PKT_STREAMING_MODE                ((uint32_t)(1) << 3)
#define CONTINUOUS_RX_ON_MODE             ((uint32_t)(1) << 4)

/* ! \} */

/* Defines used for transceiver operation mode */
#define TX_OP_MODE                              (0x01)
#define TEST_FRAMES_SENT                        (0x02)
#define WAIT_FOR_TEST_RES                       (0x03)
#define SET_PARAMETER                           (0x04)
#if (ANTENNA_DIVERSITY == 1)
#define DIVERSITY_STATUS_REQ                    (0x05)
#define DIVERSITY_SET_REQ                       (0x06)
#endif /* #if (ANTENNA_DIVERSITY == 1) */
#define CONTINUOUS_TX_MODE                      (0x07)
#define IDENTIFY_PEER                           (0x08)
#ifdef CRC_SETTING_ON_REMOTE_NODE
#define CRC_STATUS_REQ_WAIT                     (0x09)
#define CRC_SET_REQ_WAIT                        (0x0A)
#endif /* #ifdef CRC_SETTING_ON_REMOTE_NODE */
#define PEER_INFO_RSP_WAIT                      (0x0B)
#define DISCONNECT_PEER                         (0x0C)
#define SET_DEFAULT_CONFIG_PEER                 (0x0D)
#define PER_TEST_START                          (0x0E)
#define RANGE_TEST_START                        (0x0F)
#define RANGE_TEST_TX                           (0x10)
#define RANGE_TEST_STOP                         (0x11)
#define REMOTE_TEST_MODE                        (0X12)
#define PKT_STREAM_MODE                                                 (0X13)

#define RANGE_TST_PKT_SEQ_POS                    (11)

#if (TAL_TYPE == ATMEGARFR2)
#define MAX_REG_ADDRESS                         (0x1ff)
#define MIN_REG_ADDRESS                         (0x141)
#else
#define MAX_REG_ADDRESS                         (0x3f)
#define MIN_REG_ADDRESS                         (0x00)
#endif /* End of #if (TAL_TYPE == ATMEGARFR2) */

#define RX_DESENSITIZE_LEVEL                    (0x08)
#define NO_RX_DESENSITIZE_LEVEL                 (0x00)
#define INVALID_VALUE                           (0xff)

#if (TAL_TYPE == AT86RF233)
#define FREQUENCY_MULTIPLIER                     (2)
#endif /* End of (TAL_TYPE == AT86RF233) */

#if (ANTENNA_DIVERSITY == 1)
#define ENABLE_ANT_DIVERSITY                      (0)
#define ENABLE_ANTENNA_1                          (1)
#define ENABLE_ANTENNA_2                          (2)
#endif /* End of ANETENNA_DIVERSITY */

/* To handle Transceiver reset */
#define TRX_RESET                               (0)
#define BIT_COUNT                               (32)
#define MAX_SCAN_DURATION                       (14)
#define NO_OF_REGISTERS                         (65)

#define TIMEOUT_FOR_RESPONSE_IN_MICRO_SEC       (200000)
#define RANGE_TX_BEACON_INTERVAL                (3000000)
#define RANGE_TX_BEACON_START_INTERVAL          (100000)
#define MICRO_SEC_MULTIPLIER                    (1.0 / 1000000)
#define MILLI_VOLT_MULTIPLIER                   (1.0 / 1000)

/**
 * \name Generic
 * \{
 */
/* generic macros */
#define REMOTE_CMD_MASK                   (0x80)
#define MESSAGE_ID_MASK                   (0x7F)
#define FIELD_DOES_NOT_EXIST              (0xFF)
#define BYTE_LEN                          (0x08)

#define NUL_VAL                           (0)
#define START_MODE_PER                    (1)
#define START_MODE_SINGLE_NODE            (2)
#define RESULT_SIZE_PER_CHANNEL           (2)

#define START_CWT                         (1)
#define STOP_CWT                          (0)

#define TX_POWER_FORMAT_REG               (0)
#define TX_POWER_FORMAT_DBM               (1)

#define MESSAGE_ID_POS                    (2)
#define CMD_POS                           (3)

/* Field positions - PERF_SET_REQ frame */
#define PARAM_TYPE_POS                    (3)
#define PARAM_VALUE_POS                   (5)

/* Field positions - PERF_START_REQ frame */
#define START_MODE_POS                    (3)

/* Field positions - CONT_WAVE_TX_REQ frame */
#define START_STOP_POS                    (3)
#define TX_MODE_POS                       (4)
#define TMR_VAL_1                         (5)
#define TMR_VAL_2                         (6)

/* Field positions - REGISTER_READ, WRITE */
#define REGISTER_ADDR_POS                 (3)
#define REGISTER_VAL_POS                  (5)
#define PKTSTREAM_STOP                                    (0)
#define RX_ON_STOP                                            (0)
/* Field positions - REGISTER_DUMP_REQ */
#define START_REG_ADDR_POS                (3)
#define END_REG_ADDR_POS                  (5)

/* Field positions - ED_SCAN_START_REQ */
#define MSG_LEN_ED_SCAN_POS               (0)
#define SCAN_DURATION_POS                 (3)
#define CHANNELS_SELECT_POS               (4)
#define MSG_LEN_ED_SCAN_REQ               (7)

/* Field positions - PKT_STREAM_REQ frame */
#define FRAME_LEN_1                       (4)
#define FRAME_LEN_2                       (5)
#define GAP_TIME_1                        (6)
#define GAP_TIME_2                        (7)
#define TIMEOUT_VAL_1                     (8)
#define TIMEOUT_VAL_2                     (9)

/* ! \} */
/* === Types ================================================================ */

/**
 * \name Request , Confirm and Indication Message Codes
 * \{
 */

enum msg_code {
	IDENTIFY_BOARD_REQ              =     (0x00),
	PERF_START_REQ                  =     (0x01),
	PERF_SET_REQ                    =     (0x02),
	PERF_GET_REQ                    =     (0x03),
	IDENTIFY_PEER_NODE_REQ          =     (0x04),
	CONT_PULSE_TX_REQ               =     (0x05),
	CONT_WAVE_TX_REQ                =     (0x06),
	REGISTER_READ_REQ               =     (0x07),
	REGISTER_WRITE_REQ              =     (0x08),
	REGISTER_DUMP_REQ               =     (0x09),
	ED_SCAN_START_REQ               =     (0x0a),
	SENSOR_DATA_REQ                 =     (0x0b),
	PER_TEST_START_REQ              =     (0x0c),
	PEER_DISCONNECT_REQ             =     (0x0d),
	SET_DEFAULT_CONFIG_REQ          =     (0x0e),
	GET_CURRENT_CONFIG_REQ          =     (0x0f),
	RANGE_TEST_START_REQ            =     (0X50),
	RANGE_TEST_STOP_REQ             =     (0x52),
	PKT_STREAM_REQ                  =     (0X22),
	RX_ON_REQ                                               =     (0X24),

	/* Confirms and Indications */

	IDENTIFY_BOARD_CONFIRM          =     (0x10),
	PERF_START_CONFIRM              =     (0x11),
	PERF_SET_CONFIRM                =     (0x12),
	PERF_GET_CONFIRM                =     (0x13),
	IDENTIFY_PEER_NODE_CONFIRM      =     (0x14),
	CONT_PULSE_TX_CONFIRM           =     (0x15),
	CONT_WAVE_TX_CONFIRM            =     (0x16),
	REGISTER_READ_CONFIRM           =     (0x17),
	REGISTER_WRITE_CONFIRM          =     (0x18),
	REGISTER_DUMP_CONFIRM           =     (0x19),
	ED_SCAN_START_CONFIRM           =     (0x1a),
	ED_SCAN_END_INDICATION          =     (0x1b),
	SENSOR_DATA_CONFIRM             =     (0x1c),
	PER_TEST_START_CONFIRM          =     (0x1d),
	PER_TEST_END_INDICATION         =     (0x1e),
	PEER_DISCONNECT_CONFIRM         =     (0x1f),
	SET_DEFAULT_CONFIG_CONFIRM      =     (0x20),
	GET_CURRENT_CONFIG_CONFIRM      =     (0x21),
	RANGE_TEST_START_CONFIRM        =     (0x51),
	RANGE_TEST_STOP_CONFIRM         =     (0x53),
	RANGE_TEST_BEACON_RESPONSE      =     (0x54),
	RANGE_TEST_BEACON               =     (0x55),
	RANGE_TEST_MARKER_INDICATION    =     (0x56),
	PKT_STREAM_CONFIRM              =     (0x23),
	RX_ON_CONFIRM                                   =     (0X25)
}
SHORTENUM;

/* ! \} */

/**
 * Parameter value types
 */
typedef union {
	/** Parameter Bool */
	bool param_value_bool;
	/** Parameter 8-bit */
	uint8_t param_value_8bit;
	/** Parameter 16-bit */
	uint16_t param_value_16bit;
	/** Parameter 32-bit */
	uint32_t param_value_32bit;
	/** Parameter 64-bit */
	uint64_t param_value_64bit;
	/** Parameter float*/
	float param_value_float;
} param_value_t;

/* ! \} */
#endif
