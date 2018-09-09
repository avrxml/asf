/**
 * \file app_init.h
 *
 * \brief Initialization prototypes and enumerations - Performance Analyzer
 * application
 *
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
#ifndef APP_INIT_H
#define APP_INIT_H

#include "tal.h"
#include "string.h"
#include "app_config.h"
#include "user_interface.h"
#include "conf_perf_analyzer.h"

/**
 * \defgroup group_perf_analyzer Performance Analyzer Application
 * This  application Performance Analyzer  is a Serial interface based
 * application,
 * which communicates with Wireless Analyzer to demonstrate various features and
 * capabilities of Atmel Transceivers
 *
 */

/**
 * \ingroup group_perf_analyzer
 * \defgroup group_app_init Application Initialization Process
 * Initializes the Performance Analyzer Application
 * \{
 */
/* === Includes ============================================================= */

/* === Macros =============================================================== */
/* Version of the software */
#define FIRMWARE_VERSION   3.2

#if ((TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B))
#define DEFAULT_CHANNEL         (DEFAULT_CHANNEL_OFFSET)
#else
#define DEFAULT_CHANNEL         (11 + DEFAULT_CHANNEL_OFFSET)
#endif

#define DEFAULT_PAN_ID          (0xCAFE)
#define DST_PAN_ID              (DEFAULT_PAN_ID)
#define SRC_PAN_ID              (DEFAULT_PAN_ID)
#define DEFAULT_ADDR            (0xFFFF)
#define DST_SHORT_ADDR          (0xFFFF)

/* Frame overhead due to selected address scheme incl. FCS */
#if (DST_PAN_ID == SRC_PAN_ID)
#define FRAME_OVERHEAD          (11)
#else
#define FRAME_OVERHEAD          (13)
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
	(TAL_TYPE == AT86RF231) || (TAL_TYPE == AT86RF233))
#define CRC_SETTING_ON_REMOTE_NODE
#endif

#if ((TAL_TYPE == ATMEGARFA1) || (TAL_TYPE == ATMEGARFR2))
#define IC_TYPE 0X01
#elif SAMR21
#define IC_TYPE 0X01
#else
#define IC_TYPE 0X00
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

void set_main_state(main_state_t state, void *arg);

/* Application Alert to indicate something has gone wrong */

void app_alert(void);

/**
 * \brief Save all user settings before Start of CW transmission
 */
void save_all_settings(void);

/**
 * \brief Recover all user settings before Start of CW transmission
 */
void recover_all_settings(void);

/* INIT state functions */

/**
 * \brief Initialization task for INIT STATE. All hardware, PAL, TAL and stack
 *        level initialization must be done in this function
 *
 * \param arg arguments for INIT state
 */
void init_state_init(void *arg);

/**
 * \brief Function to init the information base for device
 */
void config_node_ib(void);

/* WAIT_FOR_EVENT state functions */

/**
 * \brief Application task when the node is in the WAIT_FOR_EVENT state
 *
 * \param arg arguments for WAIT_FOR_EVENT state
 */
void wait_for_event_init(void *arg);

/**
 * \brief Application task handling user events like key press or
 * character on UART
 *
 * This function
 * - Implements the event handling in WAIT_FOR_EVENT state.
 */
void wait_for_event_task(void);

/**
 * \brief Callback that is called if data has been received by trx
 * in WAIT_FOR_EVENT state. This allow the node to participate in
 * Peer Search process as receptor
 *
 * \param frame Pointer to received frame
 */
void wait_for_event_rx_cb(frame_info_t *frame);

extern uint8_t T_APP_TIMER;
extern uint8_t T_APP_TIMER_RANGE;
extern uint8_t APP_TIMER_TO_TX;
extern uint8_t APP_TIMER_TO_TX_LED_OFF;
extern uint8_t APP_TIMER_TO_RX_LED_OFF;
extern uint8_t CW_TX_TIMER;

/* === Externals ============================================================ */

extern volatile node_ib_t node_info;

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
extern retval_t transmit_frame( uint8_t dst_addr_mode,
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

#endif /* APP_STATE_H */
/* EOF */
