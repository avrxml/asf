/**
 * \file app_per_mode.h
 *
 * \brief PER Measuremnt mode prototypes and defines
 *
 * -Performance Analyzer application
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
 */

/*
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */
/* Prevent double inclusion */
#ifndef APP_PER_MODE_H
#define APP_PER_MODE_H

/**
 * \ingroup group_perf_analyzer
 * \defgroup group_per_mode  Packet error rate measurement
 *  Handles the functionalities of Packet Error Rate Measurement(PER) Mode,
 *  User can set and get various paramters of Transceiver like Channel,Antenna
 * Diversity,CSMA
 *  and do the Packet Error Rate Measurement.
 */

/**
 * \ingroup group_per_mode
 * \defgroup group_per_mode_init  PER mode Initiator
 * This module handles the functionalities of the Initiator in PER mode.
 *
 */

/**
 * \ingroup group_per_mode
 * \defgroup group_per_mode_receptor  PER mode Receptor
 * This module handles the  functionalities of  the receptor in PER mode
 *
 */

/**
 * \ingroup group_per_mode
 * \defgroup group_per_mode_utils  PER mode Common Utilities
 * This module handles the PER mode Common utilities used by Initiator and
 * Receptor.
 *
 */

/* === Includes ============================================================= */

/* === Macros =============================================================== */

#define  CHANNEL          (0)
#define  CHANNEL_PAGE     (1)
#define  TX_POWER_DBM     (2)
#if ((TAL_TYPE != AT86RF212) && (TAL_TYPE != AT86RF212B))
#define  TX_POWER_REG     (3)
#endif
#if (TAL_TYPE == AT86RF233)
#define  FREQ_BAND_08     (4)
#define  FREQ_BAND_09     (5)
#endif
#ifdef EXT_RF_FRONT_END_CTRL
#define CHANNEL_26                          (0x1A)
#define MAX_TX_PWR_REG_VAL                  (0x09)
#define MAX_TX_PWR_REG_VAL_CH26             (0x0d)
#endif
#define LED_TOGGLE_COUNT_FOR_PER            (50)
#define MIN_TX_PWR_REG_VAL                  (0x0f)

#if (TAL_TYPE == AT86RF233)
#define BASE_ISM_FREQUENCY_MHZ              (2306)
#define MIN_ISM_FREQUENCY_MHZ               (2322)
#define MAX_ISM_FREQUENCY_MHZ               (2527)
#define MID_ISM_FREQUENCY_MHZ               (2434)
#define CC_BAND_0                           (0x00)
#define CC_BAND_8                           (0x08)
#define CC_BAND_9                           (0x09)
#define CC_NUMBER_0                         (0x00)
#endif

#if ((TAL_TYPE == AT86RF233) || (TAL_TYPE == ATMEGARFR2))
#define ENABLE_ALL_RPC_MODES                     (0xff)
#define DISABLE_ALL_RPC_MODES                    (0xC1)
#define ENABLE_RX_SAFE_MODE                      (0xA0)
#define DISABLE_RX_SAFE_MODE                     (0x60)
#endif

/**
 * \addtogroup group_per_mode
 * \{
 */

/**
 * \name Request and response Ids which sent over the air
 * \{
 */
#define SET_PARAM                           (0x01)
#define RESULT_REQ                          (0x02)
#define RESULT_RSP                          (0x03)
#if (ANTENNA_DIVERSITY == 1)
#define DIV_STAT_REQ                        (0x04)
#define DIV_STAT_RSP                        (0x05)
#define DIV_SET_REQ                         (0x06)
#endif /* #if (ANTENNA_DIVERSITY == 1) */
#define IDENTIFY_NODE                       (0x07)
#ifdef  CRC_SETTING_ON_REMOTE_NODE
#define CRC_STAT_REQ                        (0x08)
#define CRC_STAT_RSP                        (0x09)
#define CRC_SET_REQ                         (0x0A)
#endif /* #ifdef CRC_SETTING_ON_REMOTE_NODE */
#define PER_TEST_PKT                        (0x0B)
#define PEER_INFO_REQ                       (0x0C)
#define PEER_INFO_RSP                       (0x0D)
#define DISCONNECT_NODE                     (0x0E)
#define SET_DEFAULT_REQ                     (0x0F)
#define PER_TEST_START_PKT                  (0x10)
#define RANGE_TEST_START_PKT                (0x11)
#define RANGE_TEST_PKT                      (0x12)
#define RANGE_TEST_RSP                      (0x13)
#define RANGE_TEST_STOP_PKT                 (0x14)
#define RANGE_TEST_MARKER_CMD               (0x15)
#define RANGE_TEST_MARKER_RSP               (0x16)
#define REMOTE_TEST_CMD                         (0x17)
#define REMOTE_TEST_REPLY_CMD               (0x18)
#define PKT_STREAM_PKT                                          (0x18)

/* Range test packet length */
#define RANGE_TEST_PKT_LENGTH               (19)
/* Timer value in us for LED blinking */
#define LED_BLINK_RATE_IN_MICRO_SEC         (50000)
/* \} */
/* Pulse CW transmission time in us */
#define PULSE_CW_TX_TIME_IN_MICRO_SEC       (50000)
/* === Types ================================================================ */

/**
 * \brief Structure to hold all configurable parameter values
 *
 */
typedef struct {
	bool ack_request;
	bool csma_enabled;
	bool retry_enabled;

#if (TAL_TYPE != AT86RF230B)
	bool rx_desensitize;
#endif /* End of #if(TAL_TYPE != AT86RF230B)*/
#ifdef CRC_SETTING_ON_REMOTE_NODE
	bool crc_settings_on_peer;
#endif

#if ((TAL_TYPE == AT86RF233) || (TAL_TYPE == ATMEGARFR2))
	bool rpc_enable;
#endif

#if (ANTENNA_DIVERSITY == 1)
	bool antenna_diversity;
	bool antenna_diversity_on_peer;
	uint8_t antenna_selected;
	uint8_t antenna_selected_on_peer;
#endif

	uint16_t channel;
	uint8_t channel_page;
#if ((TAL_TYPE != AT86RF212) && (TAL_TYPE != AT86RF212B))
	uint8_t tx_power_reg;
#endif
	int8_t tx_power_dbm;
	uint8_t trx_state;

	uint16_t phy_frame_length;
	uint32_t number_test_frames;

#if (TAL_TYPE == AT86RF233)
	float ism_frequency;
#endif
} trx_config_params_t;

/**
 * \brief ED scan result structure to hold the channel and its ED value
 *
 */
typedef struct {
	uint8_t channel_no;
	int8_t p_in;
} ed_scan_result_t;

/* === Externals ============================================================ */
#if ((TAL_TYPE != AT86RF212) && (TAL_TYPE != AT86RF212B))
FLASH_EXTERN(int8_t tx_pwr_table[16]);
#endif

extern trx_config_params_t curr_trx_config_params;

/* Database to maintain the default settings of the configurable parameter */
extern trx_config_params_t default_trx_config_params;

/**
 * \brief Configure the frame to be used for Packet Streaming
 * \param frame_len Length of the frame to be used for Packet Streaming
 */
void configure_pkt_stream_frames(uint16_t frame_len);

/* ! \} */
/* === Prototypes =========================================================== */

/**
 * \addtogroup group_per_mode_init
 * \{
 */

/* PEER_FOUND_PER_INITIATOR state functions */

/**
 * \brief Initialize the application in PER Measurement mode as Initiator
 * \param parameter Pointer to the parameter to be carried, if any.
 */
void per_mode_initiator_init(void *parameter);

/**
 * \brief Application task for PER Measurement mode as initiator
 *
 * This function
 * - Implements the PER Measurement mode state machine.
 * - On start up as initiator application will print menu on screen
 * - On user inputs through menu app executes various tests
 */
void per_mode_initiator_task(void);

/**
 * \brief Callback that is called once tx is done in the
 * PER_FOUND_PER_INITIATOR state .
 *
 * \param status    Status of the transmission procedure
 * \param frame     Pointer to the transmitted frame structure
 */
void per_mode_initiator_tx_done_cb(retval_t status, frame_info_t *frame);

/**
 * \brief Callback that is called if data has been received by trx
 * in the PER_FOUND_PER_INITIATOR state
 *
 * \param frame Pointer to received frame
 */
void per_mode_initiator_rx_cb(frame_info_t *frame);

/**
 * \brief User call back function after ED Scan completion
 * \param energy level as input
 */
void per_mode_initiator_ed_end_cb(uint8_t energy_level);

/**
 * \brief This function is used to send a remote test repsonse command back to
 * the initiator
 */
bool send_remote_reply_cmd(uint8_t *serial_buf, uint8_t len);

/* ! \} */

/**
 * \addtogroup group_per_mode_receptor
 * \{
 */
/* PEER_FOUND_PER_RECEPTOR state functions */

/**
 * \brief Initialize the application in PER Measurement mode as Receptor
 *****\param parameter Pointer to the paramter to be carried, if any.
 */
void per_mode_receptor_init(void *parameter);

/**
 * \brief The application task when the node is in PER_TEST_RECEPTOR state
 * i.e.PER Measurement mode as Receptor
 *
 */
void per_mode_receptor_task(void);

/**
 * \brief Callback that is called once tx is done in PER_TEST_RECEPTOR state
 *        This call back is used when the node is operating as
 *        PER Measurement mode Receptor
 *
 * \param status    Status of the transmission procedure
 * \param frame     Pointer to the transmitted frame structure
 */
void per_mode_receptor_tx_done_cb(retval_t status, frame_info_t *frame);

/**
 * \brief Callback that is called if data has been received by trx
 * in the PER_TEST_RECEPTOR state
 * This call back is used when the node is operating as
 * PER Measurement mode Receptor
 * \param frame Pointer to received frame
 */
void per_mode_receptor_rx_cb(frame_info_t *frame);

/* ! \} */

/*
 * \brief To reset the application (equivalent to soft reset)
 *
 */
void app_reset(void);

/**
 * \brief Timer Callback function  if marker response command is transmitted on
 * air
 *  This is used to blink the LED and thus identify that the transmission is
 * done
 * \param parameter pass parameters to timer handler
 */
void marker_tx_timer_handler_cb(void *parameter);

/**
 * \brief Timer Callback function  if marker command is received on air
 * This is used to blink the LED and thus identify that the marker frame is
 * received
 * \param parameter pass parameters to timer handler
 */
void marker_rsp_timer_handler_cb(void *parameter);

#ifdef EXT_RF_FRONT_END_CTRL

/**
 * \brief handle the tx power settings in case of External PA enabled,
 * and the channel changes from or to 26.This is to meet the FCC compliance
 *
 * \param Curr_chnl Current Channel
 * \param prev_chnl Previous Channel
 */
void limit_tx_power_in_ch26(uint8_t curr_chnl, uint8_t prev_chnl);

#endif

/**
 * \brief The reverse_float is used for reversing a float variable for
 * supporting BIG ENDIAN systems
 * \param float_val Float variable to be reversed
 */
float reverse_float( const float float_val );

/**
 * \brief Timer used in Packet Streaming Mode to add gap in between consecutive
 * frames
 */
void pkt_stream_gap_timer(void *parameter);

/**
 * \brief This function is called to abort the packet streaming mode in progress
 */
void stop_pkt_streaming(void *parameter);

/* ! \} */
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* APP_STATE_H */
/* EOF */
