/**
 * \file app_peer_search.h
 *
 * \brief Function prototypes for Peera Search process -
 * Performance Analyzer application for AT86RF215.
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
#ifndef APP_PEER_SEARCH_H
#define APP_PEER_SEARCH_H

/**
 * \ingroup group_perf_analyzer_2
 * \defgroup group_peer_search  Peer Search Process
 * Handles the functionalities of peer searching process.
 */

/**
 * \ingroup group_peer_search
 * \defgroup group_peer_search_initiator Peer Search Initiator
 * This module handles the Peer search Initiation Process
 */

/**
 * \ingroup group_peer_search_initiator
 * \defgroup group_config_mode  Configuration Mode
 * This Module handles the Functionalities of Configuration Mode
 *
 */

/**
 * \ingroup group_peer_search
 * \defgroup group_peer_search_receptor Peer Search Receptor
 * This module handles the Peer search Receptor Process
 */

/* === Includes ============================================================= */

/* === Macros =============================================================== */

/**
 * \addtogroup group_peer_search
 * \{
 */

/* Commands involved in peer search */
#define PEER_REQUEST          (0x01)
#define PEER_RESPONSE         (0x02)
#define PEER_CONFIRM          (0x03)

/* Op modes commands send as packet payload during peer search */
#define RANGE_MEASURE_MODE    (0x01)
#define PER_TEST_MODE         (0x02)

/* === Types ================================================================ */

/**
 * \brief Structure to hold the the function pointers to handle
 * various Peer Search Process tasks
 *
 */
typedef struct {
	void (*peer_state_init)(trx_id_t trx, void *arg);
	void (*peer_state_task)(trx_id_t trx);
	void (*peer_state_tx_frame_done_cb)(trx_id_t trx, retval_t status,
			frame_info_t *frame);
	void (*peer_state_rx_frame_cb)(trx_id_t trx, frame_info_t *frame);
	void (*peer_state_exit)(trx_id_t trx);
} peer_state_function_t;

/**
 * \brief Structure to hold the peer device's short address and ieee address
 *
 */
typedef struct {
	uint16_t my_short_addr;
	uint64_t peer_ieee_addr;
} peer_search_receptor_arg_t;

/* ! \} */

/* === Externals ============================================================ */

/* === Prototypes =========================================================== */

/**
 * \addtogroup group_peer_search_receptor
 * \{
 */

/* PEER_SEARCH_RANGE_RX and PEER_SEARCH_PER_RX state functions */

/**
 * \brief Function to set the sub state of state machine
 * \param state     Sub state to which the Peer search receptor to be entered
 * \param arg       Pointer to the arguments to be carried, if any
 */
void peer_search_receptor_set_sub_state(trx_id_t trx, uint8_t state, void *arg);

/**
 * \brief Application task to start peer search
 *
 * \param arg       Pointer to the arguments to be carried, if any
 */
void peer_search_receptor_init(trx_id_t trx, void *arg);

/**
 * \brief Application task handling peer search
 *
 * This function
 * - Implements the peer search state machine.
 */
void peer_search_receptor_task(trx_id_t trx);

/**
 * \brief Callback that is called once tx is done in peer search receptor state.
 *
 * \param status    Status of the transmission procedure
 * \param frame     Pointer to the transmitted frame structure
 */
void peer_search_receptor_tx_done_cb(trx_id_t trx, retval_t status,
		frame_info_t *frame);

/**
 * \brief Callback that is called if data has been received by trx in
 *  peer search receptor state.
 *
 * \param frame Pointer to received frame
 */
void peer_search_receptor_rx_cb(trx_id_t trx, frame_info_t *frame);

/**
 * \brief Function to exit peer search receptor state.
 *
 * This function
 * - Implements the peer search state machine.
 */
void peer_search_receptor_exit(trx_id_t trx);

/* ! \} */

/**
 * \addtogroup group_peer_search_initiator
 * \{
 */

/* PEER_SEARCH_RANGE_TX and PEER_SEARCH_PER_TX state functions */

/**
 * Function to set the sub state of state machine
 *
 * \param state     Sub state to be set
 * \param arg       arguments to be set as part of set sub state
 */
void peer_search_initiator_set_sub_state(trx_id_t trx, uint8_t state,
		void *arg);

/**
 * \brief Application task to start peer search
 *
 * \param arg arguments to start the peer search
 */
void peer_search_initiator_init(trx_id_t trx, void *arg);

/**
 * \brief Application task handling peer search
 */
void peer_search_initiator_task(trx_id_t trx);

/**
 * \brief Callback that is called once tx is done in peer search initiator
 * state.
 *
 * \param status    Status of the transmission procedure
 * \param frame     Pointer to the transmitted frame structure
 */
void peer_search_initiator_tx_done_cb(trx_id_t trx, retval_t status,
		frame_info_t *frame);

/**
 * \brief Callback that is called if data has been received by trx.
 *
 * \param frame Pointer to received frame
 */
void peer_search_initiator_rx_cb(trx_id_t trx, frame_info_t *frame);

/**
 * \brief Function to exit peer search initiator exit state
 */
void peer_search_initiator_exit(trx_id_t trx);

/* ! \} */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* APP_PEER_SEARCH_H */
/* EOF */
