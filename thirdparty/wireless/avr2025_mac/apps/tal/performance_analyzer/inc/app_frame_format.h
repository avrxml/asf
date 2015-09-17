/**
 * \file app_frame_format.h
 *
 * \brief Defines various structure definitions, used to communicate with
 * Peer node over the air - Performance Analyzer application
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
#ifndef APP_FRAME_FORMAT_H
#define APP_FRAME_FORMAT_H

/* === Includes ============================================================= */

/* === Macros =============================================================== */
#define REMOTE_MSG_BUF_SIZE                 (116)
/* === Types ================================================================ */
COMPILER_PACK_SET(1)

/**
 * \brief Peer request frame structure used in Peer Search process
 *
 */
typedef struct {
	uint8_t op_mode;
	uint16_t nwk_addr;
	uint8_t config_mode : 1;
} peer_req_t;

/**
 * \brief Peer response frame structure used in Peer Search process
 *
 */
typedef struct {
	uint16_t nwk_addr;
} peer_rsp_t;

/**
 * \brief Peer confirm frame structure used in Peer Search process
 *
 */
typedef struct {
	uint16_t nwk_addr;
} peer_conf_t;

/**
 * \brief Data packet frame structure to send data in Range Measurement mode
 *
 */
typedef struct {
	uint32_t pkt_count;
} data_pkt_range_test_t;

/**
 * \brief Set paramter Request frame structure to set the the parameters like
 * channel on peer node
 *
 */
typedef struct {
	uint8_t param_type;
	uint16_t param_value;
} set_parm_req_t;

/**
 * \brief Result request frame structure to request the results of the PER test
 *
 */
typedef struct {
	uint8_t cmd;
} result_req_t;

/**
 * \brief Result response frame structure to send the results of the PER test
 *
 */
typedef struct {
	uint32_t num_of_frames_rx;
	uint32_t lqi_avrg_rx;
	uint32_t rssi_avrg_rx;
	uint32_t frames_with_wrong_crc;
} result_rsp_t;

/**
 * \brief Antenna Diversity status request frame structure to request the peer
 * node's antenna diversity settings
 *
 */
typedef struct {
	uint8_t status;
} div_stat_req_t;

/**
 * \brief Antenna Diversity status response frame structure to send the peer
 * node's antenna diversity settings
 *
 */
typedef struct {
	uint8_t status    : 1;
	uint8_t ant_sel   : 2;
} div_stat_rsp_t;

/**
 * \brief Antenna Diversity set request frame structure to change the peer
 * node's
 * antenna diversity settings
 *
 */
typedef struct {
	uint8_t status    : 1;
	uint8_t ant_sel   : 2;
} div_set_req_t;

/**
 * \brief Antenna Diversity set response frame structure to send the status of
 * div_set_req
 *
 */
typedef struct {
	uint8_t status;
} div_set_rsp_t;

/**
 * \brief CRC status request frame structure to request the peer
 * node's CRC settings
 *
 */
typedef struct {
	uint8_t status;
} crc_stat_req_t;

/**
 * \brief Remote Test Start Request Structure
 *
 */
typedef struct {
	uint8_t remote_serial_data[REMOTE_MSG_BUF_SIZE];
} remote_test_req_t;

/**
 * \brief CRC status response frame structure to send the peer
 * node's CRC settings
 *
 */
typedef struct {
	uint8_t status;
} crc_stat_rsp_t;

/**
 * \brief CRC set request frame structure to change the peer node's
 * CRC settings
 *
 */
typedef struct {
	uint8_t status;
} crc_set_req_t;

/**
 * \brief CRC set response frame structure to send the status of
 * crc_set_req
 *
 */
typedef struct {
	uint8_t status;
} crc_set_rsp_t;

/**
 * \brief Peer information response frame structure to send the peer details
 *
 */
typedef struct {
	uint8_t status;
	uint8_t ic_type;
	char soc_mcu_name[16];
	char trx_name[16];
	char board_name[25];
	uint64_t mac_address;
	float fw_version;
	uint32_t feature_mask;
} peer_info_rsp_t;

typedef struct {
	uint32_t frame_count;
	int8_t ed;
	uint8_t lqi;
} range_tx_t;

/**
 * \brief Union of all request and and response structures
 *
 */
typedef union {
	peer_req_t peer_req_data;
	peer_rsp_t peer_rsp_data;
	peer_conf_t peer_conf_data;
	data_pkt_range_test_t data_payload;
	set_parm_req_t set_parm_req_data;
	result_rsp_t test_result_rsp_data;
	div_stat_rsp_t div_stat_rsp_data;
	div_set_req_t div_set_req_data;
	crc_stat_rsp_t crc_stat_rsp_data;
	crc_set_req_t crc_set_req_data;
	peer_info_rsp_t peer_info_rsp_data;
	range_tx_t range_tx_data;
	result_req_t result_req_data;
	crc_stat_req_t crc_stat_req_data;
	remote_test_req_t remote_test_req_data;
} general_pkt_t;

/**
 * \brief application payload frame structure
 *
 */
typedef struct {
	uint8_t cmd_id;
	uint8_t seq_num;
	general_pkt_t payload;
} app_payload_t;

COMPILER_PACK_RESET()

/* === Externals ============================================================ */

/* === Prototypes =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* APP_FRAME_FORMAT_H */
/* EOF */
