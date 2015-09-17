/**
 * \file app_init.h
 *
 * \brief Initialization prototypes and enumerations - Performance Analyzer
 *        application of AT86RF215
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
#ifndef APP_INIT_H
#define APP_INIT_H

# include "tal.h"
# include "string.h"
# include "app_config.h"

/**
 * \defgroup group_perf_analyzer_2 Performance Analyzer Application
 * This  application Performance Analyzer  is a Serial interface based
 * application,
 * which communicates with Wireless Analyzer to demonstrate various features and
 * capabilities of Atmel Transceivers
 *
 */

/**
 * \ingroup group_perf_analyzer_2
 * \defgroup group_app_init Application Initialization Process
 * Initializes the Performance Analyzer Application
 * \{
 */
/* === Includes ============================================================= */

/* === Macros =============================================================== */
/* Version of the software */
#define FIRMWARE_VERSION    3.2

#define DEFAULT_CHANNEL_RF09         (1)
#define DEFAULT_CHANNEL_RF24         (21)
#define TAL_CURRENT_PAGE_DEFAULT_RF24            (0x00)
#define TAL_CURRENT_PAGE_DEFAULT_RF09            (0x02)
#define DEFAULT_PAN_ID          (0xCAFE)
#define DST_PAN_ID              (DEFAULT_PAN_ID)
#define SRC_PAN_ID              (DEFAULT_PAN_ID)
#define DEFAULT_ADDR            (0xFFFF)
#define DST_SHORT_ADDR          (0xFFFF)
#define SUN_PAGE_NO                             (0X09)
/* Frame overhead due to selected address scheme incl. FCS */
#if (DST_PAN_ID == SRC_PAN_ID)
#define FRAME_OVERHEAD          (9)
#else
#define FRAME_OVERHEAD          (11)
#endif

#define FRAME_OVERHEAD_SRC_IEEE_ADDR (FRAME_OVERHEAD + 6)
#define FRAME_OVERHEAD_DST_IEEE_ADDR (FRAME_OVERHEAD + 6)

#define OFFSET_FOR_SRC_IEEE_ADDR    (7)

#ifndef LED_COUNT
#define LED_COUNT 0
#endif

#if (LED_COUNT >= 3)
#define STATUS_LED              LED0_GPIO
#define TX_LED                  LED1_GPIO
#define RX_LED                  LED2_GPIO
#elif (LED_COUNT >= 2)
#define STATUS_LED              LED0_GPIO
#define TX_LED                  LED0_GPIO
#define RX_LED                  LED1_GPIO
#elif (LED_COUNT == 1)
#define STATUS_LED              LED0_GPIO
#define TX_LED                  LED0_GPIO
#define RX_LED                  LED0_GPIO
#endif

#ifdef LED0_ACTIVE_LEVEL
#define STATUS_LED_GPIO       LED0_GPIO
#define TX_LED_GPIO          LED0_GPIO
#define RX_LED_GPIO         LED0_GPIO
#define STATUS_LED_ACTIVE_LEVEL  LED0_ACTIVE_LEVEL
#define STATUS_LED_INACTIVE_LEVEL  LED0_INACTIVE_LEVEL
#define TX_LED_ACTIVE_LEVEL  LED0_ACTIVE_LEVEL
#define TX_LED_INACTIVE_LEVEL  LED0_INACTIVE_LEVEL
#define RX_LED_ACTIVE_LEVEL  LED0_ACTIVE_LEVEL
#define RX_LED_INACTIVE_LEVEL  LED0_INACTIVE_LEVEL
#endif

/* Macro to enable the feature of counting wrong CRC packets */
#if ((TAL_TYPE == ATMEGARFR2) || \
	(TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B) || \
	(TAL_TYPE == AT86RF231) || (TAL_TYPE == AT86RF233) || (TAL_TYPE == \
	AT86RF215))
#define CRC_SETTING_ON_REMOTE_NODE
#endif
#ifndef IC_TYPE
#define IC_TYPE      0x00
#endif

#ifndef BOARD_NAME
#define BOARD_NAME "USER_BOARD"
#endif
#ifndef MCU_SOC_NAME
#define MCU_SOC_NAME "USER_MCU"
#endif
/* === Types ================================================================ */
/* Main states */
typedef enum {
	INIT = 0,
	WAIT_FOR_EVENT,
	PEER_SEARCH_RANGE_TX,
	PEER_SEARCH_PER_TX,
	PEER_SEARCH_RANGE_RX,
	PEER_SEARCH_PER_RX,
	RANGE_TEST_TX_ON,
	RANGE_TEST_TX_OFF,
	SINGLE_NODE_TESTS,
	PER_TEST_INITIATOR,
	PER_TEST_RECEPTOR,
	NUM_MAIN_STATES
} main_state_t;

#define DUMMY_PAYLOAD                           (0xAA)

/**
 * \brief Structure to holds the information base for the node
 *
 */
typedef struct {
	bool configure_mode;
	bool peer_found;
	uint8_t sub_state;
	uint8_t transmitting;
	uint8_t msg_seq_num;
	uint16_t peer_short_addr;
	frame_info_t *tx_frame_info;
	main_state_t main_state;
} node_ib_t;

/* State change functions */

/**
 * \brief Function to set the main state of state machine
 *
 * \param state   main state to be set
 * \param arg     argument passed in the state
 */

void set_main_state(trx_id_t tranciever, main_state_t state, void *arg);

/* Application Alert to indicate something has gone wrong */

void app_alert(void);

/**
 * \brief Initialization task for INIT STATE. All hardware, PAL, TAL and stack
 *        level initialization must be done in this function
 *
 * \param arg arguments for INIT state
 */
void init_state_init(trx_id_t trx, void *arg);

/**
 * \brief Function to perform initialization task after disconnect request
 */
void init_after_disconnect(trx_id_t trx);

/**
 * \brief Function to init the information base for device
 */
void config_node_ib(trx_id_t trx);

/* WAIT_FOR_EVENT state functions */

/**
 * \brief Application task when the node is in the WAIT_FOR_EVENT state
 *
 * \param arg arguments for WAIT_FOR_EVENT state
 */
void wait_for_event_init(trx_id_t trx, void *arg);

/**
 * \brief Application task handling user events like key press or
 * character on UART
 *
 * This function
 * - Implements the event handling in WAIT_FOR_EVENT state.
 */
void wait_for_event_task(trx_id_t trx);

/**
 * \brief Callback that is called if data has been received by trx
 * in WAIT_FOR_EVENT state. This allow the node to participate in
 * Peer Search process as receptor
 *
 * \param frame Pointer to received frame
 */
void wait_for_event_rx_cb(trx_id_t trx, frame_info_t *frame);

extern uint8_t T_APP_TIMER;
extern uint8_t T_APP_TIMER_RANGE_RF09;
extern uint8_t T_APP_TIMER_RANGE_RF24;
extern uint8_t APP_TIMER_TO_TX;
extern uint8_t APP_TIMER_TO_TX_LED_OFF;
extern uint8_t APP_TIMER_TO_RX_LED_OFF;

/* === Externals ============================================================ */

extern volatile node_ib_t node_info[NUM_TRX];

/**
 * \brief Function to transmit frames as per 802.15.4 std.
 *
 * \param dst_addr_mode     destination address mode - can be 16 or 64 bit
 * \param dst_addr          destination address
 * \param src_addr_mode     source address mode - can be 16 or 64 bit
 * \param msdu_handle       msdu handle for the upper layers to track packets
 * \param payload           data payload pointer
 * \param payload_length    data length
 * \param ack_req           specifies ack requested for frame if set to 1
 *
 * \return MAC_SUCCESS      if the TAL has accepted the data for frame
 * transmission
 *         TAL_BUSY         if the TAL is busy servicing the previous tx request
 */
extern retval_t app_transmit_frame( trx_id_t trx, uint8_t dst_addr_mode,
		uint8_t *dst_addr,
		uint8_t src_addr_mode,
		uint8_t msdu_handle,
		uint8_t *payload,
		uint16_t payload_length,
		uint8_t ack_req);

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

/* ! \} */

#endif /* APP_INIT_H */
/* EOF */
