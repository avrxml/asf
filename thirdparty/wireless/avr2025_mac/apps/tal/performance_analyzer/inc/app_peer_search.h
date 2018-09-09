/**
 * \file app_peer_search.h
 *
 * \brief Function prototypes for Peera Search process -
 * Performance Analyzer application
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
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
 */

/*
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */
/* Prevent double inclusion */
#ifndef APP_PEER_SEARCH_H
#define APP_PEER_SEARCH_H

/**
 * \ingroup group_perf_analyzer
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
	void (*peer_state_init)(void *arg);
	void (*peer_state_task)(void);
	void (*peer_state_tx_frame_done_cb)(retval_t status,
			frame_info_t *frame);
	void (*peer_state_rx_frame_cb)(frame_info_t *frame);
	void (*peer_state_exit)(void);
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
void peer_search_receptor_set_sub_state(uint8_t state, void *arg);

/**
 * \brief Application task to start peer search
 *
 * \param arg       Pointer to the arguments to be carried, if any
 */
void peer_search_receptor_init(void *arg);

/**
 * \brief Application task handling peer search
 *
 * This function
 * - Implements the peer search state machine.
 */
void peer_search_receptor_task(void);

/**
 * \brief Callback that is called once tx is done in peer search receptor state.
 *
 * \param status    Status of the transmission procedure
 * \param frame     Pointer to the transmitted frame structure
 */
void peer_search_receptor_tx_done_cb(retval_t status, frame_info_t *frame);

/**
 * \brief Callback that is called if data has been received by trx in
 *  peer search receptor state.
 *
 * \param frame Pointer to received frame
 */
void peer_search_receptor_rx_cb(frame_info_t *frame);

/**
 * \brief Function to exit peer search receptor state.
 *
 * This function
 * - Implements the peer search state machine.
 */
void peer_search_receptor_exit(void);

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
void peer_search_initiator_set_sub_state(uint8_t state, void *arg);

/**
 * \brief Application task to start peer search
 *
 * \param arg arguments to start the peer search
 */
void peer_search_initiator_init(void *arg);

/**
 * \brief Application task handling peer search
 */
void peer_search_initiator_task(void);

/**
 * \brief Callback that is called once tx is done in peer search initiator
 * state.
 *
 * \param status    Status of the transmission procedure
 * \param frame     Pointer to the transmitted frame structure
 */
void peer_search_initiator_tx_done_cb(retval_t status, frame_info_t *frame);

/**
 * \brief Callback that is called if data has been received by trx.
 *
 * \param frame Pointer to received frame
 */
void peer_search_initiator_rx_cb(frame_info_t *frame);

/**
 * \brief Function to exit peer search initiator exit state
 */
void peer_search_initiator_exit(void);

/* ! \} */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* APP_STATE_H */
/* EOF */
