/**
 * \file peer_search_receptor.c
 *
 * \brief Receptor functionalities in Peer Search Process - Performance Analyzer
 *  application
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
/* === INCLUDES ============================================================ */

#include <stdlib.h>
#include "tal.h"
#include "tal_helper.h"
#include "ieee_const.h"
#include "app_init.h"
#include "app_frame_format.h"
#include "app_peer_search.h"

/**
 * \addtogroup group_peer_search_receptor
 * \{
 */

/* === TYPES =============================================================== */

/* Peer search rx states */
typedef enum {
	PEER_RSP_SEND,
	WAIT_FOR_PEER_CONF,
	PEER_SEARCH_RECEPTOR_SUCCESS,
	NUM_PEER_SEARCH_RECEPTOR_STATES
} peer_search_receptor_state_t;

/* === MACROS ============================================================== */

/* Timer values for the timer used in peer search */
#define PEER_RESPONSE_TIMEOUT_IN_MICRO_SEC           (200000)

/* === PROTOTYPES ========================================================== */

static void peer_rsp_send_init(void *arg);
static void peer_rsp_send_tx_done_cb(retval_t status, frame_info_t *frame);

static void app_peer_conf_tmr_handler_cb(void *parameter);
static int send_peer_rsp(uint64_t *addr);

static void wait_for_conf_init(void *arg);
static void wait_for_conf_rx_cb(frame_info_t *frame);
static void wait_for_conf_exit(void);

/* === GLOBALS ============================================================= */
/* Peer process seq number */
static uint8_t seq_num;

static peer_state_function_t const peer_search_receptor_state_table[
	NUM_PEER_SEARCH_RECEPTOR_STATES] = {
	{ /* PEER_RSP_SEND */
		peer_rsp_send_init,
		NULL,
		peer_rsp_send_tx_done_cb,
		NULL,
		NULL,
	},
	{ /* WAIT_FOR_PEER_CONF */
		wait_for_conf_init,
		NULL,
		NULL,
		wait_for_conf_rx_cb,
		wait_for_conf_exit,
	},
};
/* ! \} */
/* === IMPLEMENTATION ====================================================== */

/* Main state tasks for Peer Search */

/*
 * \brief Application task to start peer search
 *
 * \param mode starts the peer search in this particular mode
 */
void peer_search_receptor_init(void *arg)
{
	pib_value_t pib_value;
	peer_search_receptor_arg_t *arg_ptr = (peer_search_receptor_arg_t *)arg;

	/* Change LED pattern */
	app_led_event(LED_EVENT_START_PEER_SEARCH);

	/* Peer process seq number */
	seq_num = rand();

	/* assign a random address */
	do {
		node_info.peer_short_addr = rand();
		/* Make sure random number is not zero */
	} while (!node_info.peer_short_addr);

	/* Set my address which my peer send me */
	pib_value.pib_value_16bit = arg_ptr->my_short_addr;
	tal_pib_set(macShortAddress, &pib_value);

#ifdef EXT_RF_FRONT_END_CTRL
	/* Disable RF front end control during peer search process*/
	tal_ext_pa_ctrl(PA_EXT_DISABLE);
	/* Make sure that Tx power is at max, when PA_EXT is disabled */
	tal_set_tx_pwr(REGISTER_VALUE, 0x00);
#endif
}

/*
 * \brief Application task handling peer search
 *
 * This function
 * - Implements the peer search state machine.
 */
void peer_search_receptor_task()
{
	peer_search_receptor_state_t sub_state
		= (peer_search_receptor_state_t)node_info.sub_state;

	if (peer_search_receptor_state_table[sub_state].peer_state_task) {
		peer_search_receptor_state_table[sub_state].peer_state_task();
	}
}

/*
 * \brief Callback that is called once tx is done.
 *
 * \param status    Status of the transmission procedure
 * \param frame     Pointer to the transmitted frame structure
 */
void peer_search_receptor_tx_done_cb(retval_t status, frame_info_t *frame)
{
	peer_search_receptor_state_t sub_state
		= (peer_search_receptor_state_t)node_info.sub_state;

	if (peer_search_receptor_state_table[sub_state].
			peer_state_tx_frame_done_cb) {
		peer_search_receptor_state_table[sub_state].
		peer_state_tx_frame_done_cb(status, frame);
	}
}

/*
 * \brief Callback that is called if data has been received by trx.
 *
 * \param frame Pointer to received frame
 */
void peer_search_receptor_rx_cb(frame_info_t *frame)
{
	peer_search_receptor_state_t sub_state
		= (peer_search_receptor_state_t)node_info.sub_state;

	if (peer_search_receptor_state_table[sub_state].peer_state_rx_frame_cb)
	{
		peer_search_receptor_state_table[sub_state].
		peer_state_rx_frame_cb(frame);
	}
}

/*
 * Function to set the sub state of state machine
 */
void peer_search_receptor_set_sub_state(uint8_t state, void *arg)
{
	peer_search_receptor_state_t new_state
		= (peer_search_receptor_state_t)state;

	/* Exit the old state */
	if (new_state &&
			peer_search_receptor_state_table[node_info.sub_state].
			peer_state_exit) {
		peer_search_receptor_state_table[node_info.sub_state].
		peer_state_exit();
	}

	/* Change and welcome to new sub state */
	node_info.sub_state = new_state;

	if (peer_search_receptor_state_table[new_state].peer_state_init) {
		peer_search_receptor_state_table[new_state].peer_state_init(arg);
	}
}

/*
 * \brief Function to exit peer search receptor state.
 *
 * This function
 * - Implements the peer search state machine.
 */
void peer_search_receptor_exit(void)
{
	/* Exit the old sub state */
	if (peer_search_receptor_state_table[node_info.sub_state].
			peer_state_exit) {
		peer_search_receptor_state_table[node_info.sub_state].
		peer_state_exit();
	}
}

/* Sub state tasks for Peer Search */

/**
 * \brief Application task handling peer search
 *
 * This function
 * - Implements the peer search state machine.
 */
static void peer_rsp_send_init(void *arg)
{
	peer_search_receptor_arg_t *arg_ptr = (peer_search_receptor_arg_t *)arg;

	if (send_peer_rsp(&(arg_ptr->peer_ieee_addr))) {
		print_event(PRINT_PEER_SEARCH_FAILED);
		/* PEER RSP send failed - so change to WAIT_FOR_EVENT state*/
		set_main_state(WAIT_FOR_EVENT, 0);
	}
}

/**
 * \brief Callback that is called once tx is done in PEER_RSP_SEND state.
 *
 * \param status    Status of the transmission procedure
 * \param frame     Pointer to the transmitted frame structure
 */
static void peer_rsp_send_tx_done_cb(retval_t status, frame_info_t *frame)
{
	if (status == MAC_SUCCESS) {
		peer_search_receptor_set_sub_state(WAIT_FOR_PEER_CONF, 0);
	} else {
		print_event(PRINT_PEER_SEARCH_FAILED);
		/* No PEER RSP send failed so change to WAIT_FOR_EVENT state*/
		set_main_state(WAIT_FOR_EVENT, 0);
	}

	/* Keep compiler happy */
	frame = frame;
}

/**
 * \brief Send peer response. This is a unicast send to the node which
 *        had earlier send the peer request
 *
 * \param src_addr The address which had been sent as payload of peer request
 *                it becomes the source address of node which sent peer rsp
 * \param seq_num Sequence number of the Peer request frame
 */
static int send_peer_rsp(uint64_t *dst_addr)
{
	uint16_t payload_length;
	app_payload_t msg;
	peer_rsp_t *data;

	/* Fill the payload */
	msg.cmd_id = PEER_RESPONSE;

	seq_num++;
	msg.seq_num = seq_num;

	data = (peer_rsp_t *)&msg.payload;

	/* Issues an address for the peer. If tis node gets connected then
	 * peer node changes its short address to this value
	 */
	data->nwk_addr = node_info.peer_short_addr;

	payload_length = ((sizeof(app_payload_t) -
			sizeof(general_pkt_t)) +
			sizeof(peer_rsp_t));

	return(transmit_frame(FCF_LONG_ADDR,
	       (uint8_t *)(dst_addr),
	       FCF_SHORT_ADDR,
	       seq_num,                          /* seq_num used as msdu handle
	                                         **/
	       (uint8_t *)&msg,
	       payload_length,
	       1));
}

/**
 * \brief Function to initialize wait for peer search conf state.
 *
 * This function
 * - Implements the peer search state machine.
 */
static void wait_for_conf_init(void *arg)
{
	sw_timer_start(APP_TIMER_TO_TX,
			PEER_RESPONSE_TIMEOUT_IN_MICRO_SEC,
			SW_TIMEOUT_RELATIVE,
			(FUNC_PTR)app_peer_conf_tmr_handler_cb,
			NULL);

	/* Keep compiler happy */
	arg = arg;
}

/**
 * \brief Callback that is called once tx is done in wait for peer search conf
 * state.
 *
 * \param status    Status of the transmission procedure
 * \param frame     Pointer to the transmitted frame structure
 */
static void wait_for_conf_rx_cb(frame_info_t *mac_frame_info)
{
	app_payload_t *msg;

	if (*(mac_frame_info->mpdu) == (FRAME_OVERHEAD
			+ ((sizeof(app_payload_t)
			- sizeof(general_pkt_t))
			+ sizeof(peer_conf_t)))) {
		/* Point to the message : 1 =>size is first byte and 2=>FCS*/
		msg
			= (app_payload_t *)(mac_frame_info->mpdu + 1 +
				FRAME_OVERHEAD - 2);
		if ((msg->cmd_id) == PEER_CONFIRM) {
			if (node_info.peer_short_addr ==
					(msg->payload.peer_conf_data.nwk_addr))
			{
				print_event(PRINT_PEER_SEARCH_SUCCESS);
				app_led_event(LED_EVENT_PEER_SEARCH_DONE);
				switch (node_info.main_state) {
				case PEER_SEARCH_RANGE_RX:

					/* Peer success - set the board to
					 * RANGE_TEST_TX_OFF state */
					set_main_state(RANGE_TEST_TX_OFF, 0);
					break;

				case PEER_SEARCH_PER_RX:

					/* Peer success - set the board to
					 * RANGE_TEST_TX_OFF state */
					set_main_state(PER_TEST_RECEPTOR, 0);
					break;

				/* To keep the GCC compiler happy */
				case INIT:
				case WAIT_FOR_EVENT:
				case PEER_SEARCH_RANGE_TX:
				case PEER_SEARCH_PER_TX:
				case RANGE_TEST_TX_ON:
				case RANGE_TEST_TX_OFF:
				case SINGLE_NODE_TESTS:
				case PER_TEST_INITIATOR:
				case PER_TEST_RECEPTOR:
				case NUM_MAIN_STATES:
					break;

				default: /* Do nothing */
					break;
				}
			}
		}
	}
}

/**
 * \brief Function to exit wait for peer search conf state.
 *
 * This function
 * - Implements the peer search state machine.
 */
static void wait_for_conf_exit(void)
{
	/* stop the timer as node leaves this sub state */
	sw_timer_stop(APP_TIMER_TO_TX);
}

/**
 * \brief Timer handler for supporting peer search
 *
 * \param parameter pass parameters to timer handler
 */
static void app_peer_conf_tmr_handler_cb(void *parameter)
{
	print_event(PRINT_PEER_SEARCH_FAILED);
	/* No PEER CONF so change to WAIT_FOR_EVENT state*/
	set_main_state(WAIT_FOR_EVENT, 0);

	/* keep compiler happy */
	parameter = parameter;
}

/* EOF */
