/**
 * \file app_per_mode.h
 *
 * \brief PER Measurement mode prototypes and defines
 * -Performance Analyzer application for AT86RF215
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
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
 * Copyright (c) 2015, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* Prevent double inclusion */
#ifndef APP_PER_MODE_H
#define APP_PER_MODE_H

/**
 * \ingroup group_perf_analyzer_2
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

#define LED_TOGGLE_COUNT_FOR_PER            (50)
#define MAX_TX_PWR_REG_VAL                  (0x1f)

#define  MAX_ED_REG_VAL (0X53)
#define  MIN_ED_REG_VAL (0X00)
#define  MAX_ED_VAL              4
#define  MIN_ED_VAL             -127

/**
 * \addtogroup group_per_mode
 * \{
 */

/**
 * \name Packet Error rate Measurement Generic Macros
 * \{
 */
#define SET_PARAM                           (0x01)
#define RESULT_REQ                          (0x02)
#define RESULT_RSP                          (0x03)

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
#define RANGE_TEST_MARKER_CMD                (0x15)
#define RANGE_TEST_MARKER_RSP                (0x16)
#define SET_SUN_PAGE                                            (0X17)
#define RANGE_TEST_PAYLOAD_LENGTH                (0X08)
#define LED_BLINK_RATE_IN_MICRO_SEC           (50000)
/* \} */

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

#if (TAL_TYPE == AT86RF233)
	bool rpc_enable;
#endif

	uint16_t channel;
	uint8_t channel_page;
	sun_phy_t sun_phy_page;
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

extern trx_config_params_t curr_trx_config_params[NUM_TRX];

/* ! \} */
/* === Prototypes =========================================================== */

/**
 * \addtogroup group_per_mode_init
 * \{
 */
int8_t scale_reg_value_to_ed(uint8_t reg_val);
uint8_t scale_ed_to_reg_val(int8_t ed_val);

/* PEER_FOUND_PER_INITIATOR state functions */

/**
 * \brief Initialize the application in PER Measurement mode as Initiator
 * \param parameter Pointer to the paramter to be carried, if any.
 */
void per_mode_initiator_init(trx_id_t trx, void *parameter);

/**
 * \brief Application task for PER Measurement mode as initiator
 *
 * This function
 * - Implements the PER Measurement mode state machine.
 * - On start up as initiator application will print menu on screen
 * - On user inputs through menu app executes various tests
 */
void per_mode_initiator_task(trx_id_t trx);

/**
 * \brief Callback that is called once tx is done in the
 * PER_FOUND_PER_INITIATOR state .
 *
 * \param status    Status of the transmission procedure
 * \param frame     Pointer to the transmitted frame structure
 */
void per_mode_initiator_tx_done_cb(trx_id_t trx, retval_t status,
		frame_info_t *frame);

/**
 * \brief Callback that is called if data has been received by trx
 * in the PER_FOUND_PER_INITIATOR state
 *
 * \param frame Pointer to received frame
 */
void per_mode_initiator_rx_cb(trx_id_t trx, frame_info_t *frame);

/**
 * \brief User call back function after ED Scan completion
 * \param energy level as input
 */
void per_mode_initiator_ed_end_cb(trx_id_t trx, uint8_t energy_level);

/* ! \} */

/**
 * \addtogroup group_per_mode_receptor
 * \{
 */
/* PEER_FOUND_PER_RECEPTOR state functions */

/**
 * \brief Initialize the application in PER Measurement mode as Receptor
 ****\param parameter Pointer to the paramter to be carried, if any.
 */
void per_mode_receptor_init(trx_id_t trx, void *parameter);

/**
 * \brief The application task when the node is in PER_TEST_RECEPTOR state
 * i.e.PER Measurement mode as Receptor
 *
 */
void per_mode_receptor_task(trx_id_t trx);

/**
 * \brief Callback that is called once tx is done in PER_TEST_RECEPTOR state
 *        This call back is used when the node is operating as
 *        PER Measurement mode Receptor
 *
 * \param status    Status of the transmission procedure
 * \param frame     Pointer to the transmitted frame structure
 */
void per_mode_receptor_tx_done_cb(trx_id_t trx, retval_t status,
		frame_info_t *frame);

/**
 * \brief Callback that is called if data has been received by trx
 * in the PER_TEST_RECEPTOR state
 * This call back is used when the node is operating as
 * PER Measurement mode Receptor
 * \param frame Pointer to received frame
 */
void per_mode_receptor_rx_cb(trx_id_t trx, frame_info_t *frame);

/* ! \} */

/*
 * \brief To reset the application (equivalent to soft reset)
 *
 */
void app_reset(trx_id_t trx);

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

/* ! \} */
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* APP_PER_MODE_H */
/* EOF */
