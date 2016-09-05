/**
 * \file performance_main.c
 *
 * \brief  Main of TAL Examples - Performance Analyzer application for AT86RF215
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

/**
 * \page license License
 * Copyright(c) 2015, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* === INCLUDES ============================================================ */
#include <stdlib.h>
#include "tal.h"
#include "tal_helper_2.h"
#include "ieee_const.h"
#include "app_init.h"
#include "perf_api.h"
#include "app_peer_search.h"
#include "app_per_mode.h"
#include "app_range_mode.h"
#include "perf_api_serial_handler.h"
#include "asf.h"
#include "sio2host.h"
#include "sio2ncp.h"
#include "conf_board.h"
#include "performance_main.h"
void get_chat_input(char input);

/**
 * \mainpage
 * \section preface Preface
 * This is the reference manual for the Performance Analyzer Application
 * \section toc Table of Contents
 *  - \subpage overview
 *  - \ref group_perf_analyzer_2
 *  -  \b Application \b Interface(API)
 *    - \ref group_pal
 *    - \ref group_tal
 *    - \ref group_tfa
 *    - \ref group_resources
 *  - \subpage main_files
 *  - \subpage devsup
 *  - \subpage compinfo
 *  - \subpage references
 *  - \subpage contactinfo
 */

/**
 * \page overview Overview
 * \section intro Introduction
 * This  application Performance Analyzer is a serial interface based
 * application,
 * which communicates with Performance Analyzer to demonstrate various features
 * and
 * capabilities of Atmel Transceivers such as:
 * - Range of the Transceiver for peer-to-peer communication (Range Measurement)
 * - Robust Link Quality
 * - Antenna Diversity
 * - TX Power of Radio
 * - Rx Sensitivity
 * - CSMA-CA Transmission
 * - Read / Write Transceiver Registers
 * - Continuous transmit test modes
 * - Reduced Power Consumption mode
 * - Energy Detection
 * - Cyclic Redundancy Check
 * - Battery Monitor
 *
 * Based on the roles and functionalities the wireless node can be configured
 * as:\n
 * 1) \b Transmitter which is connected to the performance analyzer,takes the
 * user events like key press to find its peer node(i.e. Reflector)
 * in Range measurement mode or 'Initiate Peer search' command from the
 * Performance Analyzer
 * to find its peer node in PER mode.\n
 * 2) \b Reflector which is not connected to the Performance Analyzer, receives
 * peer request from the transmitter and participates in the peer search process
 * to get paired with the transmitter node.\n
 * 3) \b Single \b Node  which is connected to the performance analyzer and
 * receives 'Continue As single node'
 * command from the user, will start as a standalone node in which only minimal
 * set of testing can be performed.\n
 *
 * Performance Analyzer mainly works in three operating modes which are
 * Range Measurement mode, Packet Error rate measurement (PER) mode and single
 * node mode.\n
 *
 *
 * - \b Range \b Measurement \b mode is to evaluate the communication coverage
 * area of the transceiver.
 * During Range Measurement, on key press the Transmitter node will initiate
 * a procedure to find a peer node. Once peer node is found, packet transmission
 * is initiated by the node on which the key press was detected initially, acts
 * as Transmitter node. The other node acts as Reflector, receives the packets
 * and
 * acknowledges each packet received. The LED on the Reflector will blink
 * sequentially
 * and repeat at the rate at which the packets are received. The LED on the
 * Transmitter
 * will blink sequentially and repeat at the rate at which the packets are
 * transmitted.
 * By moving the Reflector away from Transmitter, user can find the
 * transceiver's
 * communication coverage area by observing the LED blinking on the Reflector
 * node.
 * The LED on Reflector will stop blinking when Reflector node is out of range.
 * A key press on the Reflector node also initiates packet transmission
 * continuously
 * from Reflector node. Once the transmission is initiated from a node, any time
 * it can be stopped or started by pressing the same key again. The number of
 * frames
 * transmitted and received in a particular node can be checked by connecting
 * the nodes
 * to HyperTerminal anytime.\n
 *
 *
 * - \b Packet \b Error \b Rate \b Measurement \b(PER) is to evaluate
 * the packet transmission and reception capabilities of the wireless nodes.
 * The Transmitter node shall be connected to the Performance Analyzer.
 * If ‘Initiate Peer search’ command is received from the analyzer after the
 * board
 * is connected, then the node (Transmitter) tries to find its peer node (i.e.
 * Reflector).
 * The Procedure to find and get paired with the Peer node is explained in the
 * user guide.
 * Once peer node is found and paired, a menu will appear on the Performance
 * Analyzer including
 * the details of the Board it is paired with. Using the menu options user can
 * configure various
 * parameters like channel, channel page, frame length, no. of frames etc. and
 * start the PER test.
 * User also can evaluate the transceiver by enabling, disabling various
 * features like CSMA-CA, RPC,
 * antenna diversity etc. Whenever the parameters are changed from the
 * transmitter
 * side through analyzer, the same will be reflected in the reflector node also.
 * User can see the changes in reflector node if it is connected to
 * HyperTerminal.\n
 *
 *
 * - \b Single \b Node \b mode If the user wants to evaluate some of the
 * transceiver
 * features with single node by giving the command 'Continue as Single Node' in
 * Performance Analyzer or If the peer search process by the transmitter is
 * timed out
 * (No Peer Response received within the time out) the node starts as a single
 * node.
 * In this state user will be able to evaluate the transceiver features for
 * which no peer node is needed e.g. Continuous Wave transmission, Energy scan
 * on all
 * channels, TX power settings,reading/writing transceiver registers,etc.\n
 *
 *
 * - \b Configuration \b mode Configuration  mode  is  the  startup  mode  in
 * which  two  nodes(i.e Transmitter and
 * Reflector) can connect each other if they are only within in the vicinity of
 * one meter
 * approximately.This  is  to  restrict  the  distance  range  for  connecting
 * devices.
 * User  can  enter  into  configuration  mode  by  pressing  the  button  while
 * Power  on
 * /reset.After this the node can pair with any other node in PER or range
 * measurement mode as usual.
 * Then the device (transmitter) shall go to the low TX level (TX_PWR = 0x0F)
 * and
 * sends the peer request with the config_mode  bit  set  to  true.  On  the
 * other  device  (reflector),  if  the  peer  request received with
 * config_mode bit true, it checks the ED level and if it is above defined
 * threshold, it will connect with the transmitter device.
 * Once the Peer Search is done successfully, the nodes
 * shall come to the normal mode where the nodes can be kept far.
 *
 * \page main_files Application Files
 * - main.c\n                      Application main file.
 * - init_state.c\n                Initialization functions and utilities of
 * Performance Analyzer application.
 * - peer_search_initiator.c\n     Handles Initiator functionalities in Peer
 * search process.
 * - peer_search_receptor.c\n      Handles Receptor functionalities in Peer
 * search process.
 * - per_mode_common_utils.c\n     Contains Common utilities for both Initiator
 * and Receptor in PER Measurement mode
 * - per_mode_initiator.c\n        Initiator/Transmitter functionalities in PER
 * Measurement mode.
 * - per_mode_receptor.c\n         Receptor functionalities in PER Measurement
 * mode.
 * - range_measure.c\n             Performs Range Measurement mode
 * functionalities.
 * - perf_api_serial_handler.c\n   Handles all the Serial input and output
 * commands.
 * - user_interface.c\n            Handles all User interface related functions
 * such as Button,Print,LED events.
 * - wait_for_event.c\n            Event handling functionalities to start Peer
 * Search.
 * \page devsup Device Support
 * - \b ATXMEGA256A3BU
 *                     - <A
 * href="http://www.atmel.com/tools/xmega-a3buxplained.aspx"> \b   XMEGA-A3BU
 * Xplained  </A>  <A
 * href="http://store.atmel.com/PartDetail.aspx?q=p:10500293">\a Buy </A>\n
 * - \b ATXMEGA256A3U-Zigbit (USB and Carrier)
 * - \b Atmega256rfr2 Xplained Pro
 * - \b Atmega256rfr2 Zigbit (Carrier)
 *
 * - \b UC3A3256S
 *                      - <A href="http://www.atmel.com/tools/rz600.aspx"> \b
 * RZ600 </A> <A
 * href="http://store.atmel.com/PartDetail.aspx?q=p:10500245;c:100118">\a Buy
 *</A>\n
 * \page compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for AVR.
 * Other compilers may or may not work.
 *
 * \page references References
 * 1)  IEEE Std 802.15.4-2006 Part 15.4: Wireless Medium Access Control (MAC)
 *     and Physical Layer (PHY) Specifications for Low-Rate Wireless Personal
 * Area
 *     Networks (WPANs).\n\n
 * 2)  AVR Wireless Support <A href="http://avr@atmel.com">avr@atmel.com</A>.\n
 * \page contactinfo Contact Information
 * For further information,visit
 * <A href="http://www.atmel.com/avr">www.atmel.com</A>.\n
 */

/* === TYPES =============================================================== */

/**
 * \addtogroup group_perf_analyzer_2
 * \{
 */

/**
 * \brief This structure forms the jump table to address various main states in
 * this application.
 */
typedef struct {
	/* Function to initialize the main state */
	void (*func_main_state_init)(trx_id_t trx, void *arg);
	/* Task function of main state */
	void (*func_task)(trx_id_t trx);
	/* Tx done call back for main state */
	void (*func_tx_frame_done_cb)(trx_id_t trx, retval_t status,
			frame_info_t *frame);
	/* Frame received call back for main state */
	void (*func_rx_frame_cb)(trx_id_t trx, frame_info_t *frame);
	/* Energy scan result call back for main state */
	void (*func_ed_end_cb)(trx_id_t trx, uint8_t energy_level);

	/* main state exit function : all timers should be stopped and other
	 * resources used in the state must be freed which is done here */
	void (*func_main_state_exit)(trx_id_t trx);

	/* if main state has sub state, it can be initialized using this
	 * function */
	void (*func_sub_state_set)(trx_id_t trx, uint8_t state, void *arg);
} state_function_t;
/* === MACROS ============================================================== */

/* === LOCALS ============================================================== */
static uint8_t storage_buffer[NUM_TRX][LARGE_BUFFER_SIZE];

/* === PROTOTYPES ========================================================== */
static void app_task(void);

/* === GLOBALS ============================================================= */
static state_function_t const state_table[NUM_MAIN_STATES] = {
	{ /* INIT */
		init_state_init,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
	},
	{ /* WAIT_FOR_EVENT */
		wait_for_event_init,
		wait_for_event_task,
		NULL,
		wait_for_event_rx_cb,
		NULL,
		NULL,
		NULL,
	},
	{ /* PEER_SEARCH_RANGE_TX */
		peer_search_initiator_init,
		peer_search_initiator_task,
		peer_search_initiator_tx_done_cb,
		peer_search_initiator_rx_cb,
		NULL,
		peer_search_initiator_exit,
		peer_search_initiator_set_sub_state,
	},
	{ /* PEER_SEARCH_PER_TX */
		peer_search_initiator_init,
		peer_search_initiator_task,
		peer_search_initiator_tx_done_cb,
		peer_search_initiator_rx_cb,
		NULL,
		peer_search_initiator_exit,
		peer_search_initiator_set_sub_state,
	},
	{ /* PEER_SEARCH_RANGE_RX */
		peer_search_receptor_init,
		peer_search_receptor_task,
		peer_search_receptor_tx_done_cb,
		peer_search_receptor_rx_cb,
		NULL,
		peer_search_receptor_exit,
		peer_search_receptor_set_sub_state,
	},
	{ /* PEER_SEARCH_PER_RX */
		peer_search_receptor_init,
		peer_search_receptor_task,
		peer_search_receptor_tx_done_cb,
		peer_search_receptor_rx_cb,
		NULL,
		peer_search_receptor_exit,
		peer_search_receptor_set_sub_state,
	},
	{ /* RANGE_TEST_TX_ON */
		range_test_tx_on_init,
		range_test_tx_on_task,
		NULL,
		range_test_rx_cb,
		NULL,
		range_test_tx_on_exit,
		NULL
	},
	{ /* RANGE_TEST_TX_OFF */
		NULL,
		range_test_tx_off_task,
		NULL,
		range_test_rx_cb,
	},
	{ /* SINGLE_NODE_TESTS */
		per_mode_initiator_init,
		per_mode_initiator_task,
		per_mode_initiator_tx_done_cb,
		per_mode_initiator_rx_cb,
		per_mode_initiator_ed_end_cb,
		NULL,
		NULL,
	},
	{ /* PER_TEST_INITIATOR  */
		per_mode_initiator_init,
		per_mode_initiator_task,
		per_mode_initiator_tx_done_cb,
		per_mode_initiator_rx_cb,
		per_mode_initiator_ed_end_cb,
		NULL,
		NULL,
	},
	{
		/* PER_TEST_RECEPTOR */
		per_mode_receptor_init,
		per_mode_receptor_task,
		per_mode_receptor_tx_done_cb,
		per_mode_receptor_rx_cb,
		NULL,
		NULL,
		NULL,
	}
};

volatile node_ib_t node_info[NUM_TRX];

/* ! \} */
/* === IMPLEMENTATION ====================================================== */

/**
 * \brief Init function of the Performance Analyzer application
 * \ingroup group_app_init
 */
void performance_analyzer_init(void)
{
	/*
	 * Power ON - so set the board to INIT state. All hardware, PAL, TAL and
	 * stack level initialization must be done using this function
	 */
	set_main_state(RF09, INIT, NULL);
	set_main_state(RF24, INIT, NULL);

	sio2host_init();
	sio2ncp_init();

	/* INIT was a success - so change to WAIT_FOR_EVENT state */

	set_main_state(RF09, WAIT_FOR_EVENT, NULL);
	set_main_state(RF24, WAIT_FOR_EVENT, NULL);
}

/**
 * \brief This task needs to be called in a while(1) for performing
 *  Performance Analyzer tasks
 */
void performance_analyzer_task(void)
{
	pal_task(); /* Handle platform specific tasks, like serial
	             * interface */
	tal_task(); /* Handle transceiver specific tasks */
	app_task(); /* Application task */
	serial_data_handler();
}

/**
 * \brief Application task
 */
static void app_task(void)
{
	for (uint8_t trx_id = 0; trx_id < 2; trx_id++) {
		void (*handler_func)(trx_id_t trx)
			= state_table[node_info[trx_id].main_state].
				func_task;

		if (handler_func) {
			handler_func(trx_id);
		}
	}
}

/*
 * \brief Callback that is called if data has been received by trx.
 *
 * \param frame Pointer to received frame
 */
void tal_rx_frame_cb(trx_id_t trx, frame_info_t *frame)
{
	void (*handler_func)(trx_id_t trx, frame_info_t *frame);

	handler_func = state_table[node_info[trx].main_state].func_rx_frame_cb;
	if (handler_func) {
		handler_func(trx, frame);
	}

	/* free buffer that was used for frame reception */
	bmm_buffer_free((buffer_t *)(frame->buffer_header));
}

/*
 * \brief Callback that is called once tx is done.
 *
 * \param status    Status of the transmission procedure
 * \param frame     Pointer to the transmitted frame structure
 */
void tal_tx_frame_done_cb(trx_id_t trx, retval_t status, frame_info_t *frame)
{
	void (*handler_func)(trx_id_t trx, retval_t status,
			frame_info_t *frame);

	/* some spurious transmissions call back or app changed its state
	 * so neglect this call back */
	if (!node_info[trx].transmitting) {
		return;
	}

	/* After transmission is completed, allow next transmission.
	 * Locking to prevent multiple transmissions simultaneously */

	node_info[trx].transmitting = false;

	handler_func
		= state_table[node_info[trx].main_state].func_tx_frame_done_cb;

	if (handler_func) {
		handler_func(trx, status, frame);
	}
}

/*
 * \brief User call back function for finished ED Scan
 *
 * \param energy_level Measured energy level during ED Scan
 */
void tal_ed_end_cb(trx_id_t trx, uint8_t energy_level)
{
	void (*handler_func)(trx_id_t trx, uint8_t energy_level);

	handler_func = state_table[node_info[trx].main_state].func_ed_end_cb;

	if (handler_func) {
		handler_func(trx, energy_level);
	}
}

/*
 * \brief function to init the information base for device
 */
void config_node_ib(trx_id_t trx)
{
	node_info[trx].transmitting = false;

	/* Init tx frame info structure value that do not change during program
	 * execution */
	node_info[trx].tx_frame_info = (frame_info_t *)storage_buffer[trx];

	/* random number initialized for the sequence number */
	node_info[trx].msg_seq_num = rand();

	/* Set peer addr to zero */
	node_info[trx].peer_short_addr = 0;

	/* Set peer_found status as false */
	node_info[trx].peer_found = false;

	/* Set config_mode to false */
	node_info[trx].configure_mode = false;
}

/*
 * \brief Function to set the main state of state machine
 *
 * \param state   main state to be set
 * \param arg     argument passed in the state
 */
void set_main_state(trx_id_t trx, main_state_t state, void *arg)
{
	void (*handler_func_exit)(trx_id_t trx);
	void (*handler_func_init)(trx_id_t trx, void *arg);
	void (*handler_sub_state_set)(trx_id_t trx, uint8_t state, void *arg);

	handler_func_exit
		= state_table[node_info[trx].main_state].func_main_state_exit;
	/* Exit the old state if not init state */
	if (handler_func_exit && state) {
		handler_func_exit(trx);
	}

	/* Nullify all the previous tx call backs. In case of change in main
	 * state
	 * TX call back prevention is taken care here. If the state has sub
	 * states
	 * TX call back during sub state change must be taken care during sub
	 * state
	 * set exclusively
	 */
	node_info[trx].transmitting = false;

	/* Welcome to new state */
	node_info[trx].main_state = state;

	handler_func_init = state_table[state].func_main_state_init;

	/* Do init for new state and then change state */
	if (handler_func_init) {
		handler_func_init(trx, arg);
	}

	handler_sub_state_set = state_table[state].func_sub_state_set;

	if (handler_sub_state_set) {
		handler_sub_state_set(trx, 0, arg);
	}
}

/*
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

retval_t app_transmit_frame(trx_id_t trx,
		uint8_t dst_addr_mode,
		uint8_t *dst_addr,
		uint8_t src_addr_mode,
		uint8_t msdu_handle,
		uint8_t *payload,
		uint16_t payload_length,
		uint8_t ack_req)
{
	uint16_t i;
	uint16_t temp_value;
	uint16_t frame_length;
	uint8_t *frame_ptr;
	uint8_t *temp_frame_ptr;
	uint16_t fcf = 0;

	/* Prevent multiple transmissions , this code is not reentrant*/
	if (node_info[trx].transmitting) {
		return FAILURE;
	}

	node_info[trx].transmitting = true;

	/* Get length of current frame. */
	frame_length = (9 + payload_length); /* 9->frame overhead */

	/* Set payload pointer. */
	frame_ptr = temp_frame_ptr = (uint8_t *)node_info[trx].tx_frame_info +
					LARGE_BUFFER_SIZE -
					payload_length - tal_pib[trx].FCSLen;

	/*
	 * Payload is stored to the end of the buffer avoiding payload
	 * copying by TAL.
	 */
	for (i = 0; i < payload_length; i++) {
		*temp_frame_ptr++ = *(payload + i);
	}

	/* Source address */
	if (FCF_SHORT_ADDR == src_addr_mode) {
		frame_ptr -= SHORT_ADDR_LEN;

		convert_16_bit_to_byte_array(tal_pib[trx].ShortAddress,
				frame_ptr);

		fcf |= FCF_SET_SOURCE_ADDR_MODE(FCF_SHORT_ADDR);
	} else {
		frame_ptr -= EXT_ADDR_LEN;
		frame_length += FCF_2_SOURCE_ADDR_OFFSET;

		convert_64_bit_to_byte_array(tal_pib[trx].IeeeAddress,
				frame_ptr);

		fcf |= FCF_SET_SOURCE_ADDR_MODE(FCF_LONG_ADDR);
	}

	/* Source PAN-Id */
#if (DST_PAN_ID == SRC_PAN_ID)
	/* No source PAN-Id included, but FCF updated. */
	fcf |= FCF_PAN_ID_COMPRESSION;
#else
	frame_ptr -= PAN_ID_LEN;
	temp_value = CCPU_ENDIAN_TO_LE16(SRC_PAN_ID);
	convert_16_bit_to_byte_array(temp_value, frame_ptr);
#endif

	/* Destination address */
	if (FCF_SHORT_ADDR == dst_addr_mode) {
		frame_ptr -= SHORT_ADDR_LEN;
		memcpy(frame_ptr, (uint8_t *)dst_addr, sizeof(uint16_t));
		fcf |= FCF_SET_DEST_ADDR_MODE(FCF_SHORT_ADDR);
	} else {
		frame_ptr -= EXT_ADDR_LEN;
		frame_length += PL_POS_DST_ADDR_START;
		memcpy(frame_ptr, (uint8_t *)dst_addr, sizeof(uint64_t));
		fcf |= FCF_SET_DEST_ADDR_MODE(FCF_LONG_ADDR);
	}

	/* Destination PAN-Id */
	temp_value = CCPU_ENDIAN_TO_LE16(DST_PAN_ID);
	frame_ptr -= PAN_ID_LEN;

	convert_16_bit_to_byte_array(temp_value, frame_ptr);

	/* Set DSN. */
	frame_ptr--;
	*frame_ptr = node_info[trx].msg_seq_num;
	node_info[trx].msg_seq_num++;

	/* Set the FCF. */
	fcf |= FCF_FRAMETYPE_DATA;
	if (ack_req) {
		fcf |= FCF_ACK_REQUEST;
	}

	frame_ptr -= FCF_LEN;
	convert_16_bit_to_byte_array(CCPU_ENDIAN_TO_LE16(fcf), frame_ptr);

	/* Finished building of frame. */
	node_info[trx].tx_frame_info->mpdu = frame_ptr;

	node_info[trx].tx_frame_info->len_no_crc = frame_length;

	/* Place msdu handle for tracking */
	node_info[trx].tx_frame_info->msduHandle = msdu_handle;

	/* transmit the frame */

	return(tal_tx_frame(trx, node_info[trx].tx_frame_info, CSMA_UNSLOTTED,
	       true));
}

void app_alert()
{
	while (1) {
		#if LED_COUNT > 0
		#ifdef LED0_ACTIVE_LEVEL
		LED_Toggle(LED0);
		#else
		LED_Toggle(LED0_GPIO);
		#endif
		#endif

		#if LED_COUNT > 1
		LED_Toggle(LED1_GPIO);
		#endif

		#if LED_COUNT > 2
		LED_Toggle(LED2_GPIO);
		#endif

		#if LED_COUNT > 3
		LED_Toggle(LED3_GPIO);
		#endif

		#if LED_COUNT > 4
		LED_Toggle(LED4_GPIO);
		#endif

		#if LED_COUNT > 5
		LED_Toggle(LED5_GPIO);
		#endif

		#if LED_COUNT > 6
		LED_Toggle(LED6_GPIO);
		#endif

		#if LED_COUNT > 7
		LED_Toggle(LED7_GPIO);
		#endif
		delay_us(0xFFFF);
	}
}

/* EOF */
