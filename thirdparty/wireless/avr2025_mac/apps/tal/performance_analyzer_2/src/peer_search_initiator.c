/**
 * \file peer_search_initiator.c
 *
 * \brief Initiator/Transmitter functionalities in Peer Search Process -
 * Performance Analyzer application for AT86RF215
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
/* === INCLUDES ============================================================ */

#include <stdlib.h>
#include "tal.h"
#include "tal_pib.h"
#include "tal_helper_2.h"
#include "ieee_const.h"
#include "sio2host.h"
#include "sio2ncp.h"
#include "app_frame_format.h"
#include "app_init.h"
#include "user_interface.h"
#include "app_peer_search.h"
#include "perf_api.h"
/* === TYPES =============================================================== */

/**
 * \addtogroup group_peer_search_initiator
 * \{
 */

/* Peer search tx states */
typedef enum {
	PEER_REQ_SEND,
	PEER_RSP_RCVD,
	NUM_PEER_SEARCH_INITIATOR_STATES
} peer_search_initiator_state_t;

/* === MACROS ============================================================== */

#define MAX_NUMBER_PEER_REQ_RETRY (0xFF)

/* Timer values for the timer used in peer search */
#define PEER_REQUEST_SEND_INTERVAL_IN_MICRO_SEC      (50000)

/**
 * \addtogroup group_config_mode
 * \{
 */

/** Lowest power -17dBm */
#define CONFIG_MODE_TX_PWR  (0xEF)

/* ! \} */

/* === PROTOTYPES ========================================================== */
/* Sub state PEER_REQ_SEND functions */
static void peer_req_send_task(trx_id_t trx);
static void peer_req_send_rx_cb(trx_id_t trx, frame_info_t *frame);
static void peer_req_send_exit(trx_id_t trx);

/* Sub state PEER_RSP_RCVD functions */
static void peer_rsp_rcvd_init(trx_id_t trx, void *arg);
static void peer_rsp_rcvd_tx_cb(trx_id_t trx, retval_t status,
		frame_info_t *frame);
static void peer_rsp_rcvd_exit(trx_id_t trx);

/* Peer search functions */
static void app_peer_req_tmr_handler_cb(void *parameter);
static retval_t send_peer_req(trx_id_t trx);
static retval_t send_peer_conf(trx_id_t trx);

/* === GLOBALS ============================================================= */
/* Peer search process seq number */
static uint8_t seq_num[NUM_TRX];

static peer_state_function_t const peer_search_initiator_state_table[
	NUM_PEER_SEARCH_INITIATOR_STATES] = {
	{ /* PEER_REQ_SEND */
		NULL,
		peer_req_send_task,
		NULL,
		peer_req_send_rx_cb,
		peer_req_send_exit,
	},
	{ /* PEER_RSP_RCVD */
		peer_rsp_rcvd_init,
		NULL,
		peer_rsp_rcvd_tx_cb,
		NULL,
		peer_rsp_rcvd_exit,
	},
};

/* ! \} */

/* === IMPLEMENTATION ====================================================== */

/* Main state tasks for Peer Search */

/*
 * \brief Application task to start peer search
 *
 * \param arg arguments to start the peer search
 */
void peer_search_initiator_init(trx_id_t trx, void *arg)
{
	/* Change LED pattern */
	app_led_event(LED_EVENT_START_PEER_SEARCH);

	/* Print the message if it is Range measurement mode */
	if (PEER_SEARCH_RANGE_TX == node_info[trx].main_state) {
		print_event(trx, PRINT_PEER_SEARCH_INITATED);
	}

	/* Peer search process seq number */
	seq_num[trx] = rand();

	/* assign a random address */
	do {
		node_info[trx].peer_short_addr = (uint16_t)rand();
		/* Make sure random number is not zero */
	} while (!node_info[trx].peer_short_addr);

	/* Reduce the TX power level to minium,if configuration mode is enabled
	**/
	if (true == node_info[trx].configure_mode) {
		/* set the tx power to lowest in configuration mode */

		uint8_t config_tx_pwr = CONFIG_MODE_TX_PWR;
		tal_pib_set(trx, phyTransmitPower,
				(pib_value_t *)&config_tx_pwr);
	}

	/* Keep compiler happy */
	arg = arg;
}

/*
 * \brief Application task handling peer search
 */
void peer_search_initiator_task(trx_id_t trx)
{
	peer_search_initiator_state_t sub_state
		= (peer_search_initiator_state_t)node_info[trx].
			sub_state;
	void (*handler_func)(trx_id_t trx)
		= peer_search_initiator_state_table[sub_state].
			peer_state_task;

	if (handler_func) {
		handler_func(trx);
	}
}

/*
 * \brief Callback that is called once tx is done.
 *
 * \param status    Status of the transmission procedure
 * \param frame     Pointer to the transmitted frame structure
 */
void peer_search_initiator_tx_done_cb(trx_id_t trx, retval_t status,
		frame_info_t *frame)
{
	peer_search_initiator_state_t sub_state
		= (peer_search_initiator_state_t)node_info[trx].
			sub_state;
	void (*handler_func)(trx_id_t trx, retval_t status,
			frame_info_t *frame);

	handler_func
		= peer_search_initiator_state_table[sub_state].
			peer_state_tx_frame_done_cb;

	if (handler_func) {
		handler_func(trx, status, frame);
	}
}

/*
 * \brief Callback that is called if data has been received by trx
 * peer search initiator state.
 * \param frame Pointer to received frame
 */
void peer_search_initiator_rx_cb(trx_id_t trx, frame_info_t *frame)
{
	peer_search_initiator_state_t sub_state
		= (peer_search_initiator_state_t)node_info[trx].
			sub_state;
	void (*handler_func)(trx_id_t trx, frame_info_t *frame);

	handler_func
		= peer_search_initiator_state_table[sub_state].
			peer_state_rx_frame_cb;

	if (handler_func) {
		handler_func(trx, frame);
	}
}

/*
 * Function to set the sub state of state machine
 *
 * \param state     Sub state to be set
 * \param arg       arguments to be set as part of set sub state
 */
void peer_search_initiator_set_sub_state(trx_id_t trx, uint8_t state, void *arg)
{
	peer_search_initiator_state_t new_state
		= (peer_search_initiator_state_t)state;

	void (*handler_func_exit)(trx_id_t trx);
	void (*handler_func_init)(trx_id_t trx, void *arg);

	handler_func_exit
		= peer_search_initiator_state_table[node_info[trx].sub_state
			].peer_state_exit;

	/* Exit the old state */
	if (new_state && handler_func_exit) {
		handler_func_exit(trx);
	}

	/* Change and welcome to new sub state */
	node_info[trx].sub_state = new_state;

	handler_func_init
		= peer_search_initiator_state_table[new_state].peer_state_init;

	if (handler_func_init) {
		handler_func_init(trx, arg);
	}
}

/*
 * \brief Function to exit peer search initiator exit state
 */
void peer_search_initiator_exit(trx_id_t trx)
{
	void (*handler_func)(trx_id_t trx);

	handler_func
		= peer_search_initiator_state_table[node_info[trx].sub_state
			].peer_state_exit;

	/* Exit the old sub state */
	if (handler_func) {
		handler_func(trx);
	}
}

/**
 * \brief Application task handling peer request send
 *
 */
static void peer_req_send_task(trx_id_t trx)
{
	static uint8_t count = 0;

	if (sw_timer_is_running(APP_TIMER_TO_TX)) {
		return;
	}

	if (MAX_NUMBER_PEER_REQ_RETRY == count) {
		count = 0;
		switch (node_info[trx].main_state) {
		case PEER_SEARCH_PER_TX:
		{
			/* In confiuration mode allow the user to do peer search
			 * again */
			if (node_info[trx].configure_mode == true) {
				/* PEER REQ command failed - so change to
				 * WAIT_FOR_EVENT state */
				set_main_state(trx, WAIT_FOR_EVENT, NULL);
			} else {
				/* Peer search was failed in PER mode - so
				 * change to SINGLE_NODE_TESTS state */
				set_main_state(trx, SINGLE_NODE_TESTS, NULL);
			}
		}
		break;

		case PEER_SEARCH_RANGE_TX:
		{
			print_event(trx, PRINT_PEER_SEARCH_FAILED);

			/* PEER REQ command failed - so change to WAIT_FOR_EVENT
			 * state */
			set_main_state(trx, WAIT_FOR_EVENT, NULL);
		}
		break;

		/* To keep the GCC compiler happy */
		case INIT:
		case WAIT_FOR_EVENT:
		case PEER_SEARCH_RANGE_RX:
		case PEER_SEARCH_PER_RX:
		case RANGE_TEST_TX_ON:
		case RANGE_TEST_TX_OFF:
		case SINGLE_NODE_TESTS:
		case PER_TEST_INITIATOR:
		case PER_TEST_RECEPTOR:
		case NUM_MAIN_STATES:
			break;

		default:
			/* Do nothing */
			break;
		}
		return;
	}

	count++;

	/* Print message if the Peer search is in progress in Range mode  */
	if (PEER_SEARCH_RANGE_TX == node_info[trx].main_state) {
		print_event(trx, PRINT_PEER_SEARCH_IN_PROGRESS);
	}

	/* Send Peer Requests */
	if (!send_peer_req(trx)) {
		sw_timer_start(APP_TIMER_TO_TX,
				PEER_REQUEST_SEND_INTERVAL_IN_MICRO_SEC,
				SW_TIMEOUT_RELATIVE,
				(FUNC_PTR)app_peer_req_tmr_handler_cb,
				NULL);
	}
}

/**
 * \brief Callback that is called if data has been received by trx in
 * PEER_REQ_SEND State.
 * \param frame Pointer to received frame
 */
static void peer_req_send_rx_cb(trx_id_t trx, frame_info_t *mac_frame_info)
{
	app_payload_t *msg;
	uint16_t my_addr_from_peer;

	if ((mac_frame_info->len_no_crc) == (FRAME_OVERHEAD_DST_IEEE_ADDR
			+ ((sizeof(app_payload_t)
			- sizeof(general_pkt_t))
			+ sizeof(peer_rsp_t)))) {
		/* Point to the message : 1 =>size is first byte and 2=>FCS*/
		msg
			= (app_payload_t *)(mac_frame_info->mpdu +
				FRAME_OVERHEAD_DST_IEEE_ADDR);
		if (PEER_RESPONSE == (msg->cmd_id)) {
			my_addr_from_peer
				= (msg->payload.peer_rsp_data.nwk_addr);

			/* PEER_RESPONSE received - so change to PEER_RSP_RCVD
			 * state */
			peer_search_initiator_set_sub_state(trx, PEER_RSP_RCVD,
					&my_addr_from_peer);
		}
	}
}

/**
 * \brief Timer handler for supporting peer search
 *
 * \param parameter pass parameters to timer handler
 */
static void app_peer_req_tmr_handler_cb(void *parameter)
{
	/* keep compiler happy */
	parameter = parameter;
}

/**
 * \brief function to exit peer req send state
 */
static void peer_req_send_exit(trx_id_t trx)
{
	/* Off timer as node leaves this sub state */
	sw_timer_stop(APP_TIMER_TO_TX);
}

/**
 * \brief Send peer search request.
 */
static retval_t send_peer_req(trx_id_t trx)
{
	app_payload_t msg;
	peer_req_t *data;
	uint8_t payload_length;
	uint16_t dst_addr = BROADCAST;

	/* Fill the payload */
	msg.cmd_id = PEER_REQUEST;

	seq_num[trx]++;
	msg.seq_num = seq_num[trx];

	data = (peer_req_t *)&msg.payload;

	/* Fill the Mode of Peer search */
	switch (node_info[trx].main_state) {
	case PEER_SEARCH_RANGE_TX:
		data->op_mode = RANGE_MEASURE_MODE;
		break;

	case PEER_SEARCH_PER_TX:
		data->op_mode = PER_TEST_MODE;
		break;

	/* To keep the GCC compiler happy */
	case INIT:
	case WAIT_FOR_EVENT:
	case PEER_SEARCH_RANGE_RX:
	case PEER_SEARCH_PER_RX:
	case RANGE_TEST_TX_ON:
	case RANGE_TEST_TX_OFF:
	case SINGLE_NODE_TESTS:
	case PER_TEST_INITIATOR:
	case PER_TEST_RECEPTOR:
	case NUM_MAIN_STATES:
		break;

	default:
		/* Do nothing */
		break;
	}

	/* Issues an address for the peer. If this node gets connected then
	 * peer node changes its short address to this value
	 */
	data->nwk_addr = node_info[trx].peer_short_addr;

	/* payload flag for config mode */
	data->config_mode = node_info[trx].configure_mode;

	payload_length = ((sizeof(app_payload_t) -
			sizeof(general_pkt_t)) +
			sizeof(peer_req_t));
	return(app_transmit_frame(trx, FCF_SHORT_ADDR,
	       (uint8_t *)(&dst_addr),            /* dst_addr is braodcast */
	       FCF_LONG_ADDR,                     /* src_addr_mode use IEEE addr
	                                          **/
	       seq_num[trx],                           /* seq_num used as msdu
	                                                * handle */
	       (uint8_t *)&msg,
	       payload_length,
	       1));
}

/**
 * \brief Peer rsp received state init function
 *
 * \arg argument to be used in init function
 */
static void peer_rsp_rcvd_init(trx_id_t trx, void *arg)
{
	/* Set the newly assigned address */

	tal_pib_set(trx, macShortAddress, (pib_value_t *)arg);

	if (send_peer_conf(trx)) {
		/* Print message if the Peer search failed in Range mode  */
		if (PEER_SEARCH_RANGE_TX == node_info[trx].main_state) {
			print_event(trx, PRINT_PEER_SEARCH_FAILED);
		} else if (PEER_SEARCH_PER_TX == node_info[trx].main_state) {
			/* Send the confirmation to the PC application via
			 * Serial interface */
			usr_perf_start_confirm(trx,
					NO_PEER_FOUND,
					START_MODE_PER,
					NULL,
					NUL_VAL,
					NULL,
					NULL,
					NULL,
					NUL_VAL, NUL_VAL, NUL_VAL);
		}

		/* PEER CONF send failed - so change to WAIT_FOR_EVENT state*/
		set_main_state(trx, WAIT_FOR_EVENT, NULL);
	}
}

/**
 * \brief Callback that is called once tx is done in peer rsp rcvd state.
 *
 * \param status    Status of the transmission procedure
 * \param frame     Pointer to the transmitted frame structure
 */
static void peer_rsp_rcvd_tx_cb(trx_id_t trx, retval_t status,
		frame_info_t *frame)
{
	if (MAC_SUCCESS == status) {
		node_info[trx].peer_found = true;
		/* Change LED pattern */
		app_led_event(LED_EVENT_PEER_SEARCH_DONE);

		switch (node_info[trx].main_state) {
		case PEER_SEARCH_RANGE_TX:
		{
			print_event(trx, PRINT_PEER_SEARCH_SUCCESS);

			/* Peer success - set the board to RANGE_TEST_TX_ON
			 * state */
			set_main_state(trx, RANGE_TEST_TX_ON, NULL);
		}
		break;

		case PEER_SEARCH_PER_TX:
		{
			/* Peer success - set the board to PER_TEST_INITIATOR
			 * state */
			set_main_state(trx, PER_TEST_INITIATOR, NULL);
		}
		break;

		/* To keep the GCC compiler happy */
		case INIT:
		case WAIT_FOR_EVENT:
		case PEER_SEARCH_RANGE_RX:
		case PEER_SEARCH_PER_RX:
		case RANGE_TEST_TX_ON:
		case RANGE_TEST_TX_OFF:
		case SINGLE_NODE_TESTS:
		case PER_TEST_INITIATOR:
		case PER_TEST_RECEPTOR:
		case NUM_MAIN_STATES:
			break;

		default:
			/* Do nothing */
			break;
		}
	} else {
		/* Print message if the Peer search failed in Range mode  */
		if (PEER_SEARCH_RANGE_TX == node_info[trx].main_state) {
			print_event(trx, PRINT_PEER_SEARCH_FAILED);
		} else if (PEER_SEARCH_PER_TX == node_info[trx].main_state) {
			/* Send the confirmation to the PC application via
			 * Serial interface */
			usr_perf_start_confirm(trx,
					NO_PEER_FOUND,
					START_MODE_PER,
					NULL,
					NUL_VAL,
					NULL,
					NULL,
					NULL,
					NUL_VAL, NUL_VAL, NUL_VAL);
		}

		/* PEER CONF send failed so change to WAIT_FOR_EVENT state*/
		set_main_state(trx, WAIT_FOR_EVENT, NULL);
	}

	/* Keep compiler happy */
	frame = frame;
}

/**
 * \brief Function to exit the peer rsp rcvd state.
 *
 * This function
 * - Implements the peer search state machine.
 */
static void peer_rsp_rcvd_exit(trx_id_t trx)
{
	/* Disable the configuration mode if the peer search is failed*/
	if (true == node_info[trx].configure_mode) {
		/* set the TX power to default level */
		uint8_t config_tx_pwr = TAL_TRANSMIT_POWER_DEFAULT;
		node_info[trx].configure_mode = false;

		tal_pib_set(trx, phyTransmitPower,
				(pib_value_t *)&config_tx_pwr);
	}
}

/**
 * \brief Send the peer confirmation confirming the node which has been
 * connected
 *        This frame is sent as a unicast to peer node. All other nodes which
 *        took part in the peer search process times out and resets peer search
 */
static retval_t send_peer_conf(trx_id_t trx)
{
	uint8_t payload_length;
	app_payload_t msg;
	peer_conf_t *data;

	msg.cmd_id = PEER_CONFIRM;

	seq_num[trx]++;
	msg.seq_num = seq_num[trx];

	data = (peer_conf_t *)&msg.payload;

	/* The payload is this nodes address. The receptor of frame compares
	 * the address to the address it has issued and if it matches then
	 * declares itself as peer found */

	data->nwk_addr = tal_pib[trx].ShortAddress;

	payload_length = ((sizeof(app_payload_t) -
			sizeof(general_pkt_t)) +
			sizeof(peer_conf_t));

	return(app_transmit_frame(trx, FCF_SHORT_ADDR,
	       (uint8_t *)(&node_info[trx].peer_short_addr),
	       FCF_SHORT_ADDR,
	       seq_num[trx],                           /* seq_num used as msdu
	                                                * handle */
	       (uint8_t *)&msg,
	       payload_length,
	       1));
}

/* EOF */
