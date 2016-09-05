/**
 * \file perf_api_serial_handler.c
 *
 * \brief SIO service implementation - Performance Analyzer application for
 * AT86RF215
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
#include "tal.h"
#include "return_val.h"
#include "ieee_const.h"
#include "app_init.h"
#include "tal_helper_2.h"
#include "perf_msg_const.h"
#include "sio2host.h"
#include "sio2ncp.h"
#include "perf_api.h"
#include "perf_api_serial_handler.h"

/**
 * \addtogroup group_serial_parser
 * \{
 */
/* === MACROS ============================================================== */

/**
 * \name UART COMMUNICATION FRAMING
 * \{
 */

/** The start of transmission delimiter. */
#define SOT                             (1)

/** The end of transmission delimiter. */
#define EOT                             (4)

/**
 * A UART state that expects a \ref SOT to be received as the next character.
 */
#define UART_RX_STATE_SOT               (1)

/**
 * A UART state that expects the length to be received as the next character.
 */
#define UART_RX_STATE_LENGTH            (2)

/**
 * A UART state that expects the next data character to be received.
 */
#define UART_RX_STATE_DATA              (3)

/**
 * A UART state that expects a \ref EOT to be received as the next character.
 */
#define UART_RX_STATE_EOT               (4)

/* ! \} */
#define SIO_BUF_COUNT                   (3)

#define SIO_RX_BUF_SIZE                 (32)
#define SIO_TX_BUF_SIZE                 (156)

/* === Globals ============================================================= */

/**
 * This is the receive buffer of the UART.
 */
static uint8_t sio_rx_buf[NUM_TRX][SIO_RX_BUF_SIZE];

/**
 * This is the transmit buffer of the UART.
 */
static uint8_t sio_tx_buf[NUM_TRX][SIO_BUF_COUNT][SIO_TX_BUF_SIZE];

/**
 * This pointer points to the next free element inside the
 * receive buffer of the UART.
 */
static uint8_t *sio_rx_ptr[NUM_TRX];

/**
 * This is the receiver state of the UART. (\ref UART_RX_STATE_SOT,
 *   \ref UART_RX_STATE_LENGTH, \ref UART_RX_STATE_DATA, or
 *   \ref UART_RX_STATE_EOT)
 */
static uint8_t sio_rx_state[NUM_TRX];

/**
 * This is the transmit state of the UART. (\ref UART_TX_STATE_SOT,
 *   \ref UART_TX_STATE_LENGTH, \ref UART_TX_STATE_DATA, or
 *   \ref UART_TX_STATE_EOT)
 */

/**
 * This is the length of the message should be received.
 */
static uint8_t sio_rx_length[NUM_TRX];

/**
 * This is the buffer to hold the frame received through serial interface
 */
static uint8_t data[NUM_TRX][SIO_RX_BUF_SIZE];

/**
 * This is length variable to keep track of no of received bytes
 */
static uint8_t data_length[NUM_TRX] = {0, 0};

/**
 * This is index used to validate the received bytes based on their position
 */
static uint8_t rx_index[NUM_TRX] = {0, 0};

/**
 * This is head of the queue of buffers to be transmitted
 */
static uint8_t head[NUM_TRX] = {0, 0};

/**
 * This is buffer count to keep track of the available bufer for transmission
 */
static uint8_t buf_count[NUM_TRX] = {0, 0};

/* This variable is to save the selected channels mask,
 * which is a 4byte value received through serial interface
 */
uint32_t rcvd_channel_mask[NUM_TRX] = {0, 0};

/* === Prototypes ========================================================== */

static inline void process_incoming_sio_data(trx_id_t trx);
static uint8_t *get_next_tx_buffer(trx_id_t trx);
static inline void handle_incoming_msg(trx_id_t trx);

static uint8_t curr_tx_buffer_index[NUM_TRX] = {0, 0};

/* ! \} */
/* === Implementation ====================================================== */

/*
 * \brief This function does the initialization of the Serial handler state
 * Machine.
 */
void init_sio(trx_id_t trx)
{
	sio_rx_state[trx] = UART_RX_STATE_SOT;
}

/*
 * \brief Function to handle the state machine serial data exchange.
 */
void serial_data_handler(void)
{
	for (uint8_t trx = 0; trx < 2; trx++) {
		/* Serial data handler is to handle only PER mode msg commands
		**/
		if ((RANGE_TEST_TX_ON == node_info[trx].main_state) ||
				(RANGE_TEST_TX_OFF ==
				node_info[trx].main_state)) {
			return;
		}

		/* Rx processing */
		if (data_length[trx] == 0) {
			/* No data to process, read the stream IO */
			rx_index[trx] = 0;
			if (trx == 1) {
				data_length[trx] = sio2host_rx(data[trx],
						SIO_RX_BUF_SIZE);
			} else {
				data_length[trx] = sio2ncp_rx(data[trx],
						SIO_RX_BUF_SIZE);
			}
		} else { /* Data has been received, process the data */
			 /* Process each single byte */
			process_incoming_sio_data(trx);
			data_length[trx]--;
			rx_index[trx]++;
		}

		/* Tx processing */
		if (buf_count[trx] != 0) {
			uint8_t no_of_bytes_transmitted;
			if (trx == 1) {
				no_of_bytes_transmitted = sio2host_tx(
						&sio_tx_buf[trx][head[trx]][
							curr_tx_buffer_index[trx
							]],
						(sio_tx_buf[trx][head[trx]][1] +
						3) -
						curr_tx_buffer_index[trx] );
			} else {
				no_of_bytes_transmitted = sio2ncp_tx(
						&sio_tx_buf[trx][head[trx]][
							curr_tx_buffer_index[trx
							]],
						(sio_tx_buf[trx][head[trx]][1] +
						3) -
						curr_tx_buffer_index[trx] );
			}

			/* Transmission of the current buffer is done completely
			**/
			if (no_of_bytes_transmitted ==
					((sio_tx_buf[trx][head[trx]][1] +
					3) - curr_tx_buffer_index[trx])) {
				head[trx]++;
				head[trx] %= SIO_BUF_COUNT;
				buf_count[trx]--;
				curr_tx_buffer_index[trx] = 0;
			} else { /* If none or some of the bytes in tx buffer is
				  * not transmitted */
				curr_tx_buffer_index[trx]
					+= no_of_bytes_transmitted;
			}
		}
	}
}

/**
 * \brief Process data received from SIO
 */
static inline void process_incoming_sio_data(trx_id_t trx)
{
	switch (sio_rx_state[trx]) {
	case UART_RX_STATE_SOT:
		sio_rx_ptr[trx] = sio_rx_buf[trx];

		/* A valid SOT is received when the sio rx state is in Idle
		 * state  */
		if (SOT == data[trx][rx_index[trx]]) {
			sio_rx_state[trx] = UART_RX_STATE_LENGTH;
		}

		break;

	case UART_RX_STATE_LENGTH:
		/* Length byte has been received */
		sio_rx_length[trx] = data[trx][rx_index[trx]];

		/* Change the sio rx state to receive the payload, if the length
		 * is a nonzero
		 */
		if (sio_rx_length[trx]) {
			sio_rx_state[trx] = UART_RX_STATE_DATA;
			*sio_rx_ptr[trx] = sio_rx_length[trx];
			sio_rx_ptr[trx]++;
		} else {
			/* NULL message */
			sio_rx_ptr[trx] = sio_rx_buf[trx];
			sio_rx_state[trx] = UART_RX_STATE_SOT;
		}

		break;

	case UART_RX_STATE_DATA:
		/* Receive the data payload of 'length' no. of  bytes */
		*sio_rx_ptr[trx] = data[trx][rx_index[trx]];
		sio_rx_ptr[trx]++;
		sio_rx_length[trx]--;
		if (!sio_rx_length[trx]) {
			sio_rx_state[trx] = UART_RX_STATE_EOT;
		}

		break;

	case UART_RX_STATE_EOT:

		/* Valid EOT is received after reception of 'length' no of bytes
		**/
		if (EOT == data[trx][rx_index[trx]]) {
			/* Message received successfully */
			handle_incoming_msg(trx);
		}

		/* Make rx buffer ready for next reception before handling
		 * received data. */
		sio_rx_ptr[trx] = sio_rx_buf[trx];
		sio_rx_state[trx] = UART_RX_STATE_SOT;
		break;

	default:
		/* Handling of invalid sio rx state */
		sio_rx_ptr[trx] = sio_rx_buf[trx];
		sio_rx_state[trx] = UART_RX_STATE_SOT;
		break;
	}
}

/**
 * \brief get the new buffer for next transmission through serial
 *
 ***\return unsigned integer pointer to buf
 */
static uint8_t *get_next_tx_buffer(trx_id_t trx)
{
	if (buf_count[trx] != SIO_BUF_COUNT) {
		uint8_t *buf;
		uint8_t tail;

		tail = (head[trx] + buf_count[trx]) % SIO_BUF_COUNT;
		buf = (uint8_t *)(&sio_tx_buf[trx][tail]);
		buf_count[trx]++;
		/* Add message start character */
		*buf++ = SOT;
		return buf;
	}

	return NULL;
}

/**
 * \brief Parses the Received Data in the Buffer and Process the Commands
 * accordingly.
 */
static inline void handle_incoming_msg(trx_id_t trx)
{
	uint8_t error_code = MAC_SUCCESS;

	/* Check for protocol id is Perofomance Analyzer */
	if (PROTOCOL_ID != sio_rx_buf[trx][1]) { /* protocol id */
		return;
	}

	/* If the node is in any of these state dont respond to any serial
	 * commands */
	if ((PEER_SEARCH_PER_RX == node_info[trx].main_state) ||
			(PEER_SEARCH_RANGE_RX == node_info[trx].main_state) ||
			(PEER_SEARCH_RANGE_TX == node_info[trx].main_state) ||
			(node_info[trx].main_state == PER_TEST_RECEPTOR) ||
			(RANGE_TEST_TX_ON == node_info[trx].main_state) ||
			(RANGE_TEST_TX_OFF == node_info[trx].main_state)) {
		return;
	}

	/* Check for the error conditions */
	error_code = check_error_conditions(trx);

	/* Process the commands */
	switch (sio_rx_buf[trx][MESSAGE_ID_POS]) { /* message id */
	/* Process Board identification command to get the board details */
	case IDENTIFY_BOARD_REQ:
	{
		/* For GUI development purpose removed the valid
		 * state checking to execute this cmd
		 */

		get_board_details(trx);
	}
	break;

	/* Process Performance test start Request */
	case PERF_START_REQ:
	{
		/* Order of reception:
		 *  size;
		 *  protocol_id;
		 *  msg_id;
		 *  start_mode;
		 */
		trx_config_params_t dummy_param_values = {0};

		/* Make sure that the node is in WAIT_FOR_EVENT state to process
		 * this command */
		if (WAIT_FOR_EVENT == node_info[trx].main_state) {
			if (START_MODE_PER == sio_rx_buf[trx][START_MODE_POS]) { /*
					                                          *
					                                          *PER
					                                          *
					                                          *mode
					                                          **/
				/* start_req received on UART - so change to
				 * state PEER_SEARCH_PER_TX */
				set_main_state(trx, PEER_SEARCH_PER_TX, NULL);
			} else if (START_MODE_SINGLE_NODE ==
					sio_rx_buf[trx][START_MODE_POS]) {           /*
					                                              *
					                                              *Single
					                                              *
					                                              *Node
					                                              *
					                                              *tests
					                                              **/
				/* start_req received on UART - so change to
				 * single node tests state directly */
				set_main_state(trx, SINGLE_NODE_TESTS, NULL);
			} else {
				/* Send the confirmation with status as
				 * INVALID_ARGUMENT */
				usr_perf_start_confirm(trx,
						INVALID_ARGUMENT,
						sio_rx_buf[trx][START_MODE_POS],
						&dummy_param_values,
						NUL_VAL,
						NULL,
						NULL,
						NULL,
						(uint64_t)NUL_VAL, NUL_VAL,
						NUL_VAL);
			}
		} else {
			/* Send the confirmation with status as INVALID_CMD as
			 * this
			 * command is not allowed in this state
			 */
			usr_perf_start_confirm(trx,
					INVALID_CMD,
					sio_rx_buf[trx][START_MODE_POS],
					&dummy_param_values,
					NUL_VAL,
					NULL,
					NULL,
					NULL,
					(uint64_t)NUL_VAL, NUL_VAL, NUL_VAL);
		}
	}
	break;

	/* Process Perf Parameters Set Request command */
	case PERF_SET_REQ:
	{
		/* Order of reception:
		 * size;
		 * protocol_id;
		 * msg_id;
		 * param_type;
		 * param_value;
		 */

		/* check for length  */
		/* Check any ongoing transaction in place */
		if (((error_code != MAC_SUCCESS) &&
				(error_code != TRANSCEIVER_IN_SLEEP)) ||
				((error_code == TRANSCEIVER_IN_SLEEP) &&
				((sio_rx_buf[trx][PARAM_TYPE_POS] !=
				PARAM_TRX_STATE) ||
				(sio_rx_buf[trx][PARAM_VALUE_POS] !=
				TRX_SLEEP)))

				) {
			/* Send the confirmation with status as Failure
			 * with error code as the reason for failure
			 */
			usr_perf_set_confirm(trx, error_code,
					sio_rx_buf[trx][PARAM_TYPE_POS],
					(param_value_t *)&sio_rx_buf[trx][
						PARAM_VALUE_POS]);
			return;
		}

		/* The node should be in the PER_TEST_INITIATOR
		 * or SINGLE_NODE_TESTS state to allow this req
		 */
		if ((PER_TEST_INITIATOR == node_info[trx].main_state) ||
				(SINGLE_NODE_TESTS == node_info[trx].main_state)
				) {
			if ((sio_rx_buf[trx][PARAM_TYPE_POS] ==
					PARAM_CHANNEL_PAGE) &&
					(sio_rx_buf[trx][PARAM_LEN_POS]
					> 1)) {                                                                                           /*
					                                                                                                   * sun
					                                                                                                   *
					                                                                                                   *phy
					                                                                                                   *
					                                                                                                   *page
					                                                                                                   **/
				/* set sun phy page */
				perf_set_sun_page(trx,
						(uint8_t *)&sio_rx_buf[trx][
							PARAM_VALUE_POS]);
			} else {
				perf_set_req(trx,
						sio_rx_buf[trx][PARAM_TYPE_POS],
						(param_value_t *)&sio_rx_buf[trx][
							PARAM_VALUE_POS]);             /*
					                                                *
					                                                *parameter
					                                                *
					                                                *type
					                                                *
					                                                *followed
					                                                *
					                                                *by
					                                                *
					                                                *parameter
					                                                *
					                                                *value
					                                                **/
			}
		} else {
			/* Send the confirmation with status as INVALID_CMD as
			 * this command is not allowed in this state
			 */
			usr_perf_set_confirm(trx, INVALID_CMD,
					sio_rx_buf[trx][PARAM_TYPE_POS],
					(param_value_t *)&sio_rx_buf[trx][
						PARAM_VALUE_POS]);
		}
	}
	break;

	/* Process Perf Parameters Get Request command */
	case PERF_GET_REQ:
	{
		/* Order of reception:
		 * size;
		 * protocol_id;
		 * msg_id;
		 * param_type;
		 */

		param_value_t param_value;
		param_value.param_value_8bit = NUL_VAL;
		/* Check any ongoing transaction in place */
		if (error_code) {
			/* Send the confirmation with status as Failure
			 * with error code as the reason for failure
			 */
			usr_perf_get_confirm(trx, error_code,
					sio_rx_buf[trx][PARAM_TYPE_POS],
					(param_value_t *)&param_value);
			return;
		}

		/* The node should be in the PER_TEST_INITIATOR
		 * or SINGLE_NODE_TESTS state to allow this req
		 */
		if ((PER_TEST_INITIATOR == node_info[trx].main_state) ||
				(SINGLE_NODE_TESTS == node_info[trx].main_state)
				) {
			perf_get_req(trx, sio_rx_buf[trx][PARAM_TYPE_POS]); /*
				                                             *
				                                             *parameter
				                                             *
				                                             *type
				                                             **/
		} else {
			/* Send the confirmation with status as INVALID_CMD as
			 * this command is not allowed in this state
			 */
			usr_perf_get_confirm(trx, INVALID_CMD,
					sio_rx_buf[trx][PARAM_TYPE_POS],
					(param_value_t *)&param_value);
		}
	}
	break;

	/* Process Peer Identification command */
	case IDENTIFY_PEER_NODE_REQ: /* 0x04 */
	{
		/* Order of reception:
		 * size;
		 * protocol_id;
		 * msg_id;
		 * dummy_byte;
		 */

		/* Check any ongoing transaction in place */
		if (error_code) {
			/* Send the confirmation with status as Failure
			 * with error code as the reason for failure
			 */
			usr_identify_peer_node_confirm(trx, error_code);
			return;
		}

		/* The node should be in the PER_TEST_INITIATOR state to allow
		 * this req */
		if (PER_TEST_INITIATOR == node_info[trx].main_state) {
			identify_peer_node(trx);
		} else {
			/* Send the confirmation with status as INVALID_CMD as
			 * this command is not allowed in this state
			 */
			usr_identify_peer_node_confirm(trx, INVALID_CMD);
		}
	}
	break;

	/* Proces Continuous Pulse transmission req command */
	case CONT_PULSE_TX_REQ:
	{
		/* Order of reception:
		 * size;
		 * protocol_id;
		 * msg_id;
		 * dummy_byte;
		 */
		/* Check any ongoing transaction in place */
		if (error_code) {
			/* Send the confirmation with status as Failure
			 * with error code as the reason for failure
			 */
			usr_cont_pulse_tx_confirm(trx, error_code);
			return;
		}

		/* The node should be in the PER_TEST_INITIATOR
		 * or SINGLE_NODE_TESTS state to allow this req
		 */
		if ((PER_TEST_INITIATOR == node_info[trx].main_state) ||
				(SINGLE_NODE_TESTS == node_info[trx].main_state)
				) {
			pulse_cw_transmission(trx);
		} else {
			/* Send the confirmation with status as INVALID_CMD as
			 * this command is not allowed in this state
			 */
			usr_cont_pulse_tx_confirm(trx, INVALID_CMD);
		}
	}
	break;

	/* Process Continuous wave transmission req command */
	case CONT_WAVE_TX_REQ:
	{ /* Order of reception:
			 * size;
			 * protocol_id;
			 * msg_id;
			 * start_stop_parameter;
			 * Tx_mode;
			 */
		static uint8_t cont_tx_mode;

		/* Check any ongoing transaction in place or
		 * Check Continuous transmission is already running and
		 * received start_stop parameter as STOP and
		 * received CW TX mode is same as currently running CW TX mode
		 */
		if (((error_code != MAC_SUCCESS) &&
				(error_code !=
				CW_TRANSMISSION_UNDER_PROGRESS)) ||
				((error_code ==
				CW_TRANSMISSION_UNDER_PROGRESS) &&
				((sio_rx_buf[trx][START_STOP_POS] !=
				STOP_CWT) ||
				(sio_rx_buf[trx][TX_MODE_POS] !=
				cont_tx_mode)))) {
			/* Send the confirmation with status as Failure
			 * with error code as the reason for failure
			 */
			usr_cont_wave_tx_confirm(trx, error_code,
					sio_rx_buf[trx][START_STOP_POS],
					sio_rx_buf[trx][TX_MODE_POS]);
			return;
		}

		/* The node should be in the PER_TEST_INITIATOR
		 * or SINGLE_NODE_TESTS state to allow this req
		 */
		if ((PER_TEST_INITIATOR == node_info[trx].main_state) ||
				(SINGLE_NODE_TESTS == node_info[trx].main_state)
				) {
			if (START_CWT == sio_rx_buf[trx][START_STOP_POS]) { /*
					                                     *
					                                     *Start_Stop
					                                     *
					                                     *mode,
					                                     *
					                                     *Start
					                                     *=
					                                     *
					                                     *0x01,
					                                     *
					                                     *Stop
					                                     *=
					                                     *
					                                     *0x00
					                                     **/
				start_cw_transmission(trx,
						sio_rx_buf[trx][TX_MODE_POS]);    /*
					                                           *
					                                           *tx_mode,
					                                           *
					                                           *CW
					                                           *=
					                                           *
					                                           *0x00
					                                           *
					                                           *PRBS
					                                           *=
					                                           *
					                                           *0x01
					                                           **/
			} else if (STOP_CWT ==
					sio_rx_buf[trx][START_STOP_POS]) {
				stop_cw_transmission(trx,
						sio_rx_buf[trx][TX_MODE_POS]);   /*
					                                          *
					                                          *tx_mode,
					                                          *
					                                          *CW
					                                          *=
					                                          *
					                                          *0x00
					                                          *
					                                          *PRBS
					                                          *=
					                                          *
					                                          *0x01
					                                          **/
			} else {
				/* Send the confirmation with status as
				 * INVALID_ARGUMENT
				 * as the start stop mode argument is not a
				 * valid one
				 */
				usr_cont_wave_tx_confirm(trx, INVALID_ARGUMENT,
						sio_rx_buf[trx][START_STOP_POS],
						sio_rx_buf[trx][TX_MODE_POS]);
			}

			/* Update currently running Continuous wave transmission
			 * mode */
			cont_tx_mode = sio_rx_buf[trx][TX_MODE_POS];
		} else {
			/* Send the confirmation with status as INVALID_CMD
			 * as this command is not allowed in this state
			 */
			usr_cont_wave_tx_confirm(trx, INVALID_CMD,
					sio_rx_buf[trx][START_STOP_POS],
					sio_rx_buf[trx][TX_MODE_POS]);
		}
	}
	break;

	/* Process Register read request command */
	case REGISTER_READ_REQ:
	{
		/* Order of reception:
		 * size;
		 * protocol_id;
		 * msg_id;
		 * register_address;
		 */
		/* Check any ongoing transaction in place */
		if (error_code) {
			/* Send the confirmation with status as Failure
			 * with error code as the reason for failure
			 */
			usr_register_read_confirm(trx, error_code,
					((uint16_t)(sio_rx_buf[trx][
						REGISTER_ADDR_POS +
						1] <<
					8) |
					(sio_rx_buf[trx][REGISTER_ADDR_POS])),
					NUL_VAL);
			return;
		}

		/* The node should be in the PER_TEST_INITIATOR
		 * or SINGLE_NODE_TESTS state to allow this req
		 */
		if ((PER_TEST_INITIATOR == node_info[trx].main_state) ||
				(SINGLE_NODE_TESTS == node_info[trx].main_state)
				) {
			read_trx_registers(trx,
					((uint16_t)(sio_rx_buf[trx][
						REGISTER_ADDR_POS
						+ 1] <<
					8) |
					(sio_rx_buf[trx][REGISTER_ADDR_POS])));
		} else {
			/* Send the confirmation with status as INVALID_CMD
			 * as this command is not allowed in this state
			 */
			usr_register_read_confirm(trx, INVALID_CMD,
					((uint16_t)(sio_rx_buf[trx][
						REGISTER_ADDR_POS +
						1] <<
					8) |
					(sio_rx_buf[trx][REGISTER_ADDR_POS])),
					NUL_VAL);
		}
	}
	break;

	/* Process Register write request command */
	case REGISTER_WRITE_REQ:
	{
		/* Order of reception:
		 * size;
		 * protocol_id;
		 * msg_id;
		 * register_address;
		 * register_value;
		 */
		/* Check any ongoing transaction in place */
		if (error_code) {
			/* Send the confirmation with status as Failure
			 * with error code as the reason for failure
			 */
			usr_register_write_confirm(trx, error_code,
					((uint16_t)(sio_rx_buf[trx][
						REGISTER_ADDR_POS +
						1] <<
					8) |
					(sio_rx_buf[trx][REGISTER_ADDR_POS])),
					NUL_VAL);
			return;
		}

		/* The node should be in the PER_TEST_INITIATOR
		 * or SINGLE_NODE_TESTS state to allow this req
		 */
		if ((PER_TEST_INITIATOR == node_info[trx].main_state) ||
				(SINGLE_NODE_TESTS == node_info[trx].main_state)
				) {
			write_trx_registers(trx,
					((uint16_t)(sio_rx_buf[trx][
						REGISTER_ADDR_POS
						+ 1] <<
					8) |
					(sio_rx_buf[trx][REGISTER_ADDR_POS])),
					sio_rx_buf[trx][REGISTER_VAL_POS] );
		} else {
			/* Send the confirmation with status as INVALID_CMD
			 * as this command is not allowed in this state
			 */
			usr_register_write_confirm(trx, INVALID_CMD,
					((uint16_t)(sio_rx_buf[trx][
						REGISTER_ADDR_POS +
						1] <<
					8) |
					(sio_rx_buf[trx][REGISTER_ADDR_POS])),
					NUL_VAL);
		}
	}
	break;

	/* Process Register dump request command */
	case REGISTER_DUMP_REQ:
	{
		/* Order of reception:
		 * size;
		 * protocol_id;
		 * msg_id;
		 * start_reg_address;
		 * end_reg_address;
		 */
		/* Check any ongoing transaction in place */
		if (error_code) {
			/* Send the confirmation with status as Failure with
			 * error code as the reason for failure
			 */
			usr_register_dump_confirm(trx, error_code,
					((uint16_t)(sio_rx_buf[trx][
						START_REG_ADDR_POS +
						1] <<
					8) |
					(sio_rx_buf[trx][START_REG_ADDR_POS])),
					(((uint16_t)sio_rx_buf[trx][
						END_REG_ADDR_POS +
						1] <<
					8) |
					(sio_rx_buf[trx][END_REG_ADDR_POS])),
					NULL);
			return;
		}

		/* The node should be in the PER_TEST_INITIATOR
		 * or SINGLE_NODE_TESTS state to allow this req
		 */
		if ((PER_TEST_INITIATOR == node_info[trx].main_state) ||
				(SINGLE_NODE_TESTS == node_info[trx].main_state)
				) {
			dump_trx_register_values(trx,
					((uint16_t)(sio_rx_buf[trx][
						START_REG_ADDR_POS +
						1] <<
					8) |
					(sio_rx_buf[trx][START_REG_ADDR_POS])),
					(((uint16_t)sio_rx_buf[trx][
						END_REG_ADDR_POS +
						1] <<
					8) |
					(sio_rx_buf[trx][END_REG_ADDR_POS])));
		} else {
			/* Send the confirmation with status as INVALID_CMD
			 * as this command is not allowed in this state
			 */
			usr_register_dump_confirm(trx, INVALID_CMD,
					((uint16_t)(sio_rx_buf[trx][
						START_REG_ADDR_POS +
						1] <<
					8) |
					(sio_rx_buf[trx][START_REG_ADDR_POS])),
					(((uint16_t)sio_rx_buf[trx][
						END_REG_ADDR_POS +
						1] <<
					8) |
					(sio_rx_buf[trx][END_REG_ADDR_POS])),
					NULL);
		}
	}
	break;

	/* Process ED Scan Start Request command */
	case ED_SCAN_START_REQ:
	{
		/* Order of reception:
		 * size;
		 * protocol_id;
		 * msg_id;
		 * scan_duration;
		 */
		if (CH_PG_SUN == tal_pib[trx].CurrentPage) {
			/* ED Scan is not supported for SUN Phy pages... */
			error_code = INVALID_CMD;
		}

		/* Clear for every ED SCAN REQ MSG */
		/* Check any ongoing transaction in place */
		if (error_code) {
			/* Send the confirmation with status as Failure
			 * with error code as the reason for failure
			 */
			usr_ed_scan_start_confirm(trx, error_code, NUL_VAL,
					NUL_VAL);
			return;
		}

		/* The node should be in the PER_TEST_INITIATOR
		 * or SINGLE_NODE_TESTS state to allow this req
		 */
		if ((PER_TEST_INITIATOR == node_info[trx].main_state) ||
				(SINGLE_NODE_TESTS == node_info[trx].main_state)
				) {
			if (sio_rx_buf[trx][MSG_LEN_ED_SCAN_POS] ==
					MSG_LEN_ED_SCAN_REQ) {
				/* Extract the 4bytes of selected channel mask
				 * from the sio_rx_buf */
				MEMCPY_ENDIAN(&(rcvd_channel_mask[trx]),
						&(sio_rx_buf[trx][
							CHANNELS_SELECT_POS]),
						4);
			} else {
				rcvd_channel_mask[trx]
					= TRX_SUPPORTED_CHANNELS_LEG(
						trx);
			}

			start_ed_scan(trx, sio_rx_buf[trx][SCAN_DURATION_POS],
					rcvd_channel_mask[trx]);
		} else {
			/* Send the confirmation with status as INVALID_CMD
			 * as this command is not allowed in this state
			 */
			usr_ed_scan_start_confirm(trx, INVALID_CMD, NUL_VAL,
					NUL_VAL);
		}
	}
	break;

	case SENSOR_DATA_REQ:
	{
		/* Order of reception:
		 * size;
		 * protocol_id;
		 * msg_id;
		 * dummy_byte;
		 */
		/* Check any ongoing transaction in place */
		if (error_code) {
			usr_sensor_data_get_confirm(trx, error_code, NUL_VAL,
					NUL_VAL);
			return;
		}

		/* The node should be in the PER_TEST_INITIATOR
		 * or SINGLE_NODE_TESTS state to allow this req
		 */
		if ((PER_TEST_INITIATOR == node_info[trx].main_state) ||
				(SINGLE_NODE_TESTS == node_info[trx].main_state)
				) {
			get_sensor_data(trx);
		} else {
			usr_sensor_data_get_confirm(trx, INVALID_CMD, NUL_VAL,
					NUL_VAL);
		}
	}
	break;

	/* Process PER test Start Request command */
	case PER_TEST_START_REQ:
	{
		/* Order of reception:
		 * size;
		 * protocol_id;
		 * msg_id;
		 * dummy_byte;
		 */
		/* Check any ongoing transaction in place */
		if (error_code) {
			/* Send the confirmation with status as Failure
			 * with error code as the reason for failure
			 */
			usr_per_test_start_confirm(trx, error_code);
			return;
		}

		/* The node should be in the PER_TEST_INITIATOR state to start
		 * PER TEST */
		if (PER_TEST_INITIATOR == node_info[trx].main_state) {
			initiate_per_test(trx);
		} else {
			/* Send the confirmation with status as INVALID_CMD
			 * as this command is not allowed in this state
			 */
			usr_per_test_start_confirm(trx, INVALID_CMD);
		}
	}
	break;

	/* Process Peer Disconnect Request command */
	case PEER_DISCONNECT_REQ:
	{
		/* Order of reception:
		 * size;
		 * protocol_id;
		 * msg_id;
		 * dummy_byte;
		 */

		/* Check any ongoing transaction in place */
		if (error_code) {
			/* Send the confirmation with status as Failure
			 * with error code as the reason for failure
			 */
			usr_peer_disconnect_confirm(trx, error_code);
			return;
		}

		/* The node can be in any sate to do a app reset */
		/* Disconnect the peer node*/
		disconnect_peer_node(trx);
	}
	break;

	/* Process Set default configuration request command */
	case SET_DEFAULT_CONFIG_REQ:
	{
		/* Order of reception:
		 * size;
		 * protocol_id;
		 * msg_id;
		 * dummy_byte;
		 */

		trx_config_params_t dummy_params = {0};
		/* Check any ongoing transaction in place */
		if (error_code) {
			/* Send the confirmation with status as Failure
			 * with error code as the reason for failure
			 */
			usr_set_default_config_confirm(trx, error_code,
					&dummy_params);
			return;
		}

		/* The node should be in the PER_TEST_INITIATOR
		 * or SINGLE_NODE_TESTS state to allow this req
		 */
		if ((PER_TEST_INITIATOR == node_info[trx].main_state) ||
				(SINGLE_NODE_TESTS == node_info[trx].main_state)
				) {
			set_default_configuration(trx);
		} else {
			/* Send the confirmation with status as INVALID_CMD
			 * as this command is not allowed in this state
			 */
			usr_set_default_config_confirm(trx, INVALID_CMD,
					&dummy_params);
		}
	}
	break;

	/* Process Get current configuration request command */
	case GET_CURRENT_CONFIG_REQ:
	{
		/* Order of reception:
		 * size;
		 * protocol_id;
		 * msg_id;
		 * dummy_byte;
		 */

		trx_config_params_t dummy_params = {0};
		/* Check any ongoing transaction in place */
		if (error_code) {
			/* Send the confirmation with status as Failure
			 * with error code as the reason for failure */
			usr_get_current_config_confirm(trx, error_code,
					&dummy_params);
			return;
		}

		/* The node should be in the PER_TEST_INITIATOR
		 * or SINGLE_NODE_TESTS state to allow this req
		 */
		if ((PER_TEST_INITIATOR == node_info[trx].main_state) ||
				(SINGLE_NODE_TESTS == node_info[trx].main_state)
				) {
			get_current_configuration(trx);
		} else {
			/* Send the confirmation with status as INVALID_CMD
			 * as this command is not allowed in this state */
			usr_get_current_config_confirm(trx, INVALID_CMD,
					&dummy_params);
		}
	}
	break;

	/* Process Range Test start command */
	case RANGE_TEST_START_REQ: /* 0x50 */
	{
		if (error_code) {
			/* Send the confirmation with status as Failure
			 * with error code as the reason for failure
			 */
			usr_range_test_start_confirm(trx, error_code);
			return;
		}

		/* The node should be in the PER_TEST_INITIATOR state to start
		 * the GUI based RANGE TEST */
		if (PER_TEST_INITIATOR == node_info[trx].main_state) {
			initiate_range_test(trx);
		} else {
			/* Send the confirmation with status as INVALID_CMD
			 * as this command is not allowed in this state
			 */
			usr_range_test_start_confirm(trx, INVALID_CMD);
		}
	}
	break;

	case RANGE_TEST_STOP_REQ: /* 0x52 */
	{
		/* Change the mode of initiator and send stop cmd to receptor
		 * once the range test stop command is received */
		stop_range_test(trx);
	}
	break;

	default:

		break;
	}
}

/*
 * Function to generate Perf Start confirmation frame that must be sent to
 * host application via serial interface.
 * Called by Performance application as confirmation for perf_start_req
 * \param status                Result for requested Perf start request
 * \param start_mode            In which mode the transmitter is started
 *                              i.e.single node or PER mode
 * \param trx_config_params     configurable param values structure
 * \param peer_ic_type          Peer Ic Type whether it is SOC or MCU
 * \param peer_soc_mcu_name     Peer MCU or SOC name as string
 * \param peer_trx_name         Peer Transceiver's name as string
 * \param peer_board_name Name  Peer HW Board name as string
 * \param peer_mac_address      MAC address of the Peer node
 * \return void
 */
void usr_perf_start_confirm(trx_id_t trx,
		uint8_t status,
		uint8_t start_mode,
		trx_config_params_t *trx_config_params,
		uint8_t peer_ic_type,
		char *peer_soc_mcu_name,
		char *peer_trx_name,
		char *peer_board_name,
		uint64_t peer_mac_address,
		float peer_fw_version, uint32_t peer_feature_mask)
{
	uint8_t byte_cnt;
	uint8_t *msg_buf;
	/* Pointer to size element - the content is written later. */
	uint8_t *ptr_to_msg_size;

	msg_buf = get_next_tx_buffer(trx);

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return;
	}

	/* Store pointer to size element. */
	ptr_to_msg_size = msg_buf;

	/* Copy Len, Protocol Id, Msg Id parameters */
	*msg_buf++ = PROTOCOL_ID_LEN + PERF_START_CONFIRM_LEN;
	*msg_buf++ = PROTOCOL_ID;
	*msg_buf++ = PERF_START_CONFIRM;

	/* Copy confirmation payload */
	*msg_buf++ = status;
	*msg_buf++ = start_mode;
	/* Copy all configuration parameters */
	*msg_buf++ = (uint8_t)trx_config_params->channel;
	*msg_buf++ = (uint8_t)trx_config_params->channel >> 8;
	*msg_buf++ = trx_config_params->channel_page;
	*msg_buf++ = trx_config_params->tx_power_dbm;
	*msg_buf++ = trx_config_params->tx_power_reg;
	*msg_buf++ = (uint8_t)trx_config_params->csma_enabled;
	*msg_buf++ = (uint8_t)trx_config_params->retry_enabled;
	*msg_buf++ = (uint8_t)trx_config_params->ack_request;
	*msg_buf++ = (uint8_t)trx_config_params->rx_desensitize;
	*msg_buf++ = FIELD_DOES_NOT_EXIST; /*Filled with 0xff to indicate this
	                                    * parameter is not available for
	                                    *this
	                                    * transceiver (RPC)*/
	*msg_buf++ = FIELD_DOES_NOT_EXIST; /* Filled with 0xff to indicate this
	                                    * parameter is not available for
	                                    *this
	                                    * transceiver (ANTENNA DIVERSITY)*/
	*msg_buf++ = trx_config_params->trx_state;
	*msg_buf++ = (uint8_t)trx_config_params->number_test_frames;
	*msg_buf++ = (uint8_t)(trx_config_params->number_test_frames >> 8);
	*msg_buf++ = (uint8_t)(trx_config_params->number_test_frames >> 16);
	*msg_buf++ = (uint8_t)(trx_config_params->number_test_frames >> 24);
	*msg_buf++ = (uint8_t)(trx_config_params->phy_frame_length);
	*msg_buf++ = (uint8_t)(trx_config_params->phy_frame_length >> 8);

	/*Peer settings for parameters like CRC and ant diversity */
	*msg_buf++ = FIELD_DOES_NOT_EXIST; /* Filled with 0xff to indicate this
	                                    * parameter is not available for
	                                    *this
	                                    * transceiver (ANTENNA DIVERSITY)*/
	*msg_buf++ = trx_config_params->crc_settings_on_peer;

	if (START_MODE_PER == start_mode) {
		*msg_buf++ = peer_ic_type;
		*ptr_to_msg_size += 1;
		/* Copy SoC or MCU name */
		*msg_buf++ = strlen(peer_soc_mcu_name);
		memcpy(msg_buf, peer_soc_mcu_name, strlen(peer_soc_mcu_name));
		*ptr_to_msg_size += 1 + strlen(peer_soc_mcu_name);
		msg_buf += strlen(peer_soc_mcu_name);
		/* Copy Transceiver name */
		*msg_buf++ = strlen(peer_trx_name);
		memcpy(msg_buf, peer_trx_name, strlen(peer_trx_name));
		*ptr_to_msg_size += 1 + strlen(peer_trx_name);
		msg_buf += strlen(peer_trx_name);
		/* Copy Board name */
		*msg_buf++ = strlen(peer_board_name);
		memcpy(msg_buf, peer_board_name, strlen(peer_board_name));
		*ptr_to_msg_size += 1 + strlen(peer_board_name);
		msg_buf += strlen(peer_board_name);
		/* Copy MAC address */
		memcpy(msg_buf, &peer_mac_address, EXT_ADDR_LEN);
		*ptr_to_msg_size += EXT_ADDR_LEN;
		msg_buf += EXT_ADDR_LEN;
		memcpy(msg_buf, &peer_fw_version, sizeof(float));
		msg_buf += sizeof(float);
		memcpy(msg_buf, &peer_feature_mask, sizeof(uint32_t));
		msg_buf += sizeof(uint32_t);
	} else {
		/* IC type */
		*msg_buf++ = 0x00;  /* Length byte 0 as no string present */
		*ptr_to_msg_size += 1;
		/* SoC MCU Name */
		*msg_buf++ = 0x00;  /* Length byte 0 as no string present */
		*ptr_to_msg_size += 1;
		/* Transceiver name */
		*msg_buf++ = 0x00;  /* Length byte 0 as no string present */
		*ptr_to_msg_size += 1;
		/* Board name */
		*msg_buf++ = 0x00;  /* Length byte 0 as no string present */
		*ptr_to_msg_size += 1;
		for (byte_cnt = 0x00; byte_cnt < EXT_ADDR_LEN; byte_cnt++) {
			*msg_buf++ = FIELD_DOES_NOT_EXIST;
		}
		*ptr_to_msg_size += EXT_ADDR_LEN;
		memset(msg_buf, 0X00, sizeof(float));
		msg_buf += sizeof(float);
		memset(msg_buf, 0X00, sizeof(uint32_t));
		msg_buf += sizeof(uint32_t);
	}

	*msg_buf = EOT;
}

/*
 * Function to generate Per Test Start confirmation frame that must be sent to
 * host application via serial interface.
 * Called by Performance application as confirmation for per_test_start_req
 * request
 * \param status      Result for requested per_test_start_req
 *
 * \return void
 */
void usr_per_test_start_confirm(trx_id_t trx, uint8_t status)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer(trx);

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return;
	}

	/* Copy Len, Protocol Id, Msg Id parameters */
	*msg_buf++ = PROTOCOL_ID_LEN + PER_TEST_START_CONFIRM_LEN;
	*msg_buf++ = PROTOCOL_ID;
	*msg_buf++ = PER_TEST_START_CONFIRM;

	/* Copy confirmation payload */
	*msg_buf++ = status;

	*msg_buf = EOT;
}

/**
 * \brief Function to send  the transmitted frame to the Host application
 * \param frame Pointer to the actual frame transmitted
 */
void usr_range_test_beacon_tx(trx_id_t trx, frame_info_t *frame)
{
	uint8_t *msg_buf;
	uint8_t frame_len = ((uint8_t)frame->len_no_crc) + tal_pib[trx].FCSLen; /*
	                                                                        * start
	                                                                        *
	                                                                        *from
	                                                                        *
	                                                                        *length
	                                                                        *
	                                                                        *field
	                                                                        *
	                                                                        *,range
	                                                                        *
	                                                                        *test
	                                                                        *
	                                                                        *length
	                                                                        *
	                                                                        *doesnot
	                                                                        *
	                                                                        *cross
	                                                                        *
	                                                                        *over
	                                                                        *
	                                                                        *8bits
	                                                                        *
	                                                                        *,complying
	                                                                        *
	                                                                        *with
	                                                                        *
	                                                                        *older
	                                                                        *
	                                                                        *trx
	                                                                        **/
	uint8_t *frame_mpdu = frame->mpdu;
	msg_buf = get_next_tx_buffer(trx);

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return;
	}

	/* Copy Len, Protocol Id, Msg Id parameters */
	*msg_buf++ = PROTOCOL_ID_LEN + RANGE_TEST_PAYLOAD_LENGTH +
			FRAME_OVERHEAD + LENGTH_FIELD_LEN + 1;
	*msg_buf++ = PROTOCOL_ID;
	*msg_buf++ = RANGE_TEST_BEACON;

	*msg_buf++ = frame_len;
	/* Copy OTA payload */
	for (uint8_t i = 0; i < (RANGE_TEST_PAYLOAD_LENGTH + FRAME_OVERHEAD);
			i++) {
		*msg_buf++ = *frame_mpdu++;
	}
	*msg_buf = EOT;
}

/**
 * \brief Function to send  the Received Range Test Response frame to the Host
 * application
 * \param frame Pointer to the actual frame Received
 * \param lqi_h LQI of the received response calculated at host
 * \param ed_h ED value  of the received response calculated at host
 * \param lqi_r LQI of the sent range test packet calculated at receptor
 * \param ed_r ED value  of the sent range test packet calculated at receptor
 */
void usr_range_test_beacon_rsp(trx_id_t trx, frame_info_t *frame, uint8_t lqi_h,
		int8_t ed_h,
		uint8_t lqi_r, int8_t ed_r)
{
	uint8_t *msg_buf;

	/* First byte of mpdu is the frame length (RF215)
	 *  //max length of range test pkt cannot exceed 8b hence maintaining
	 * the same variable**/

	uint8_t frame_len = (uint8_t)frame->len_no_crc + tal_pib[trx].FCSLen;
	uint8_t *frame_mpdu = frame->mpdu;
	msg_buf = get_next_tx_buffer(trx);

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return;
	}

	/* Copy Len, Protocol Id, Msg Id parameters */
	*msg_buf++ = PROTOCOL_ID_LEN + RANGE_TEST_RSP_PKT_LEN +
			LENGTH_FIELD_LEN +
			(frame_len - tal_pib[trx].FCSLen);
	*msg_buf++ = PROTOCOL_ID;
	*msg_buf++ = RANGE_TEST_BEACON_RESPONSE;
	*msg_buf++ = frame_len;

	/* send ota frame */
	for (uint8_t i = 0; i < frame_len - tal_pib[trx].FCSLen; i++) {
		*msg_buf++ = *frame_mpdu++;
	}

	*msg_buf++ = lqi_r;
	*msg_buf++ = ed_r;
	*msg_buf++ = lqi_h;
	*msg_buf++ = ed_h;

	*msg_buf = EOT;
}

/**
 * \brief Function to send  the Marker Indication frame to the Host application
 * \param mpdu Pointer to the actual marker frame Received
 * \param lqi LQI of the received marker packet
 * \param ed_value ED value  of the received marker packet
 */
void usr_range_test_marker_ind(trx_id_t trx, frame_info_t *frame, uint8_t lqi,
		int8_t ed_value)
{
	uint8_t *msg_buf;

	/* First byte of mpdu is the frame length (RF215)
	 * //max length of range test pkt cannot exceed 8b hence maintaining the
	 * same variable**/
	uint8_t frame_len = (uint8_t)frame->len_no_crc + tal_pib[trx].FCSLen;
	uint8_t *frame_mpdu = frame->mpdu;
	msg_buf = get_next_tx_buffer(trx);

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return;
	}

	/* Copy Len, Protocol Id, Msg Id parameters */
	*msg_buf++ = PROTOCOL_ID_LEN + RANGE_TEST_MARKER_IND_LEN +
			(frame_len - tal_pib[trx].FCSLen + LENGTH_FIELD_LEN);
	*msg_buf++ = PROTOCOL_ID;
	*msg_buf++ = RANGE_TEST_MARKER_INDICATION;
	*msg_buf++ = frame_len;

	/* send marker ota frame */
	for (uint8_t i = 0; i < frame_len - tal_pib[trx].FCSLen; i++) {
		*msg_buf++ = *frame_mpdu++;
	}
	*msg_buf++ = lqi;
	*msg_buf++ = ed_value;

	*msg_buf = EOT;
}

/*
 * Function to generate Range Test Start confirmation  that must be sent to
 * host application via serial interface.
 * Called by Performance application as confirmation for range_test_start_req
 * request
 * \param status      Result for requested range_test_start_req
 *
 * \return void
 */
void usr_range_test_start_confirm(trx_id_t trx, uint8_t status)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer(trx);

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return;
	}

	/* Copy Len, Protocol Id, Msg Id parameters */
	*msg_buf++ = PROTOCOL_ID_LEN + RANGE_TEST_START_CONFIRM_LEN;
	*msg_buf++ = PROTOCOL_ID;
	*msg_buf++ = RANGE_TEST_START_CONFIRM;

	/* Copy confirmation payload */
	*msg_buf++ = status;

	*msg_buf = EOT;
}

/*
 * Function to  Range Test stop confirmation frame that must be sent to
 * host application via serial interface.
 * Called by Performance application as confirmation for range_test_stop_req
 * request
 * \param status      Result for requested range_test_stop_req
 *
 * \return void
 */
void usr_range_test_stop_confirm(trx_id_t trx, uint8_t status)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer(trx);

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return;
	}

	/* Copy Len, Protocol Id, Msg Id parameters */
	*msg_buf++ = PROTOCOL_ID_LEN + RANGE_TEST_START_CONFIRM_LEN;
	*msg_buf++ = PROTOCOL_ID;
	*msg_buf++ = RANGE_TEST_STOP_CONFIRM;

	/* Copy confirmation payload */
	*msg_buf++ = status;

	*msg_buf = EOT;
}

/*
 * Function to generate Perf Set confirmation frame that must be sent to
 * host application via serial interface.
 * Called by Performance application as confirmation for perf_set_req request
 * \param status        Result for requested perf_set_req
 * \param param_type    Type of the parameter that has been set
 * \param param_value   Pointer to the value of the parameter that has been set
 * \return void
 */
void usr_perf_set_confirm(trx_id_t trx, uint8_t status, uint8_t parameter_type,
		param_value_t *param_value)
{
	uint8_t *msg_buf;
	uint8_t param_len = 0;

	/* Pointer to size element - the content is written later. */
	uint8_t *ptr_to_msg_size;

	msg_buf = get_next_tx_buffer(trx);

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return;
	}

	/* Store pointer to size element. */
	ptr_to_msg_size = msg_buf;

	/* Copy Len, Protocol Id, Msg Id parameters */
	*msg_buf++ = PROTOCOL_ID_LEN + PERF_SET_CONFIRM_LEN;
	*msg_buf++ = PROTOCOL_ID;
	*msg_buf++ = PERF_SET_CONFIRM;

	/* Copy the payload of the Set confirmation */
	*msg_buf++ = status;
	/* Copy Parameter type */
	*msg_buf++ = parameter_type;
	if ((parameter_type == PARAM_CHANNEL_PAGE) &&
			((param_value->param_value_8bit) == SUN_PAGE_NO)) {
		if ((uint8_t)((param_value->param_value_32bit) >> 16) == OFDM) {
			*msg_buf++ = param_len = 6;
		} else if ((uint8_t)((param_value->param_value_32bit) >> 16) ==
				OQPSK) {
			*msg_buf++ = param_len = 5;
		} else if ((uint8_t)((param_value->param_value_32bit) >> 16) ==
				FSK) {
			*msg_buf++ = param_len = 9;
		} else if ((uint8_t)((param_value->param_value_32bit) >> 16) ==
				LEG_OQPSK) {
			*msg_buf++ = param_len = 5;
		}
	} else {
		*msg_buf++ = param_len = get_param_length(parameter_type);
	}

	/* Update the Length field */
	*ptr_to_msg_size += OCTET_STR_LEN_BYTE_LEN;

	/* Copy Parameter value */
	MEMCPY_ENDIAN(msg_buf, param_value, param_len);
	msg_buf += param_len;

	/* Update the Length field */
	*ptr_to_msg_size += param_len;

	*msg_buf = EOT;
}

/*
 * Function to generate Perf Get confirmation frame that must be sent to
 * host application via serial interface.
 * Called by Performance application as confirmation for perf_get_req request
 * \param status        Result for requested perf_get_req
 * \param param_type    Type of the parameter that has been read
 * \param param_value   Pointer to the value of the parameter that has been read
 * \return void
 */
void usr_perf_get_confirm(trx_id_t trx, uint8_t status, uint8_t parameter_type,
		param_value_t *param_value)
{
	uint8_t *msg_buf;
	uint8_t param_len = 0;

	/* Pointer to size element - the content is written later. */
	uint8_t *ptr_to_msg_size;

	msg_buf = get_next_tx_buffer(trx);

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return;
	}

	/* Store pointer to size element. */
	ptr_to_msg_size = msg_buf;

	/* Copy Len, Protocol Id, Msg Id parameters */
	*msg_buf++ = PROTOCOL_ID_LEN + PERF_GET_CONFIRM_LEN;
	*msg_buf++ = PROTOCOL_ID;
	*msg_buf++ = PERF_GET_CONFIRM;

	/* Copy the payload of the Get confirmation */
	*msg_buf++ = status;
	/* Copy Parameter type */
	*msg_buf++ = parameter_type;
	if ((parameter_type == PARAM_CHANNEL_PAGE) &&
			((param_value->param_value_8bit) == SUN_PAGE_NO)) {
		if ((uint8_t)((param_value->param_value_32bit) >> 16) == OFDM) {
			*msg_buf++ = param_len = 6;
		} else if ((uint8_t)((param_value->param_value_32bit) >> 16) ==
				OQPSK) {
			*msg_buf++ = param_len = 5;
		} else if ((uint8_t)((param_value->param_value_32bit) >> 16) ==
				FSK) {
			*msg_buf++ = param_len = 9;
		} else if ((uint8_t)((param_value->param_value_32bit) >> 16) ==
				LEG_OQPSK) {
			*msg_buf++ = param_len = 5;
		}
	} else {
		*msg_buf++ = param_len = get_param_length(parameter_type);
	}

	/* Update the Length field */
	*ptr_to_msg_size += OCTET_STR_LEN_BYTE_LEN;
	/* Copy Parameter value */
	MEMCPY_ENDIAN(msg_buf, param_value, param_len);
	msg_buf += param_len;
	/* Update the Length field */
	*ptr_to_msg_size += param_len;

	*msg_buf = EOT;
}

/*
 * Function to generate Identify_peer_node_confirm that must be sent to
 * host application via serial interface.
 * Called by Performance application as reply to the identify_peer_node_req
 *
 * \param status  Result for identify_peer_node_req
 */
void usr_identify_peer_node_confirm(trx_id_t trx, uint8_t status)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer(trx);

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return;
	}

	/* Copy Len, Protocol Id, Msg Id parameters */
	*msg_buf++ = PROTOCOL_ID_LEN + IDENTIFY_PEER_NODE_CONFIRM_LEN;
	*msg_buf++ = PROTOCOL_ID;
	*msg_buf++ = IDENTIFY_PEER_NODE_CONFIRM;

	/* Copy confirmation payload */
	*msg_buf++ = status;

	*msg_buf = EOT;
}

/*
 * Function to generate cont_pulse_tx_confirm that must be sent to
 * host application via serial interface.
 * Called by Performance application as reply to the cont_pulse_tx_req
 *
 * \param status  Result for cont_pulse_tx_req
 */
void usr_cont_pulse_tx_confirm(trx_id_t trx, uint8_t status)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer(trx);

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return;
	}

	/* Copy Len, Protocol Id, Msg Id parameters */
	*msg_buf++ = PROTOCOL_ID_LEN + CONT_PULSE_TX_CONFIRM_LEN;
	*msg_buf++ = PROTOCOL_ID;
	*msg_buf++ = CONT_PULSE_TX_CONFIRM;

	/* Copy confirmation payload */
	*msg_buf++ = status;

	*msg_buf = EOT;
}

/*
 * Function to generate cont_wave_tx_confirm that must be sent to
 * host application via serial interface.
 * Called by Performance application as reply to the cont_wave_tx_req
 *
 * \param status            Result for cont_wave_tx_req
 * \param start_stop_param  Parameter to start or stop cont tx
 * \param tx_mode           Indicates CW or PRBS
 */
void usr_cont_wave_tx_confirm(trx_id_t trx, uint8_t status,
		bool start_stop_param, uint8_t tx_mode)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer(trx);

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return;
	}

	/* Copy Len, Protocol Id, Msg Id parameters */
	*msg_buf++ = PROTOCOL_ID_LEN + CONT_WAVE_TX_CONFIRM_LEN;
	*msg_buf++ = PROTOCOL_ID;
	*msg_buf++ = CONT_WAVE_TX_CONFIRM;

	/* Copy confirmation payload */
	*msg_buf++ = status;
	*msg_buf++ = (uint8_t)start_stop_param;
	*msg_buf++ = tx_mode;

	*msg_buf = EOT;
}

/*
 * Function to generate usr_register_read_confirm that must be sent to
 * host application via serial interface.
 * Called by Performance application as reply to the usr_register_read_req
 *
 * \param status    Result for usr_register_read_req
 * \param reg_addr  Register address that has been read
 * \param reg_val   Register value
 */
void usr_register_read_confirm(trx_id_t trx, uint8_t status, uint16_t reg_addr,
		uint8_t reg_val)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer(trx);

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return;
	}

	/* Copy Len, Protocol Id, Msg Id parameters */
	*msg_buf++ = PROTOCOL_ID_LEN + REGISTER_READ_CONFIRM_LEN;
	*msg_buf++ = PROTOCOL_ID;
	*msg_buf++ = REGISTER_READ_CONFIRM;

	/* Copy confirmation parameters*/
	*msg_buf++ = status;
	*msg_buf++ = (uint8_t)reg_addr;
	*msg_buf++ = (uint8_t)(reg_addr >> BYTE_LEN);
	*msg_buf++ = reg_val;
	*msg_buf = EOT;
}

/*
 * Function to generate usr_register_write_confirm that must be sent to
 * host application via serial interface.
 * Called by Performance application as reply to the usr_register_write_req
 *
 * \param status    Result for usr_register_write_req
 * \param reg_addr  Register address that has been set
 * \param reg_val   Register value
 */
void usr_register_write_confirm(trx_id_t trx, uint8_t status, uint16_t reg_addr,
		uint8_t reg_val)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer(trx);

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return;
	}

	/* Copy Len, Protocol Id, Msg Id parameters */
	*msg_buf++ = PROTOCOL_ID_LEN + REGISTER_WRITE_CONFIRM_LEN;
	*msg_buf++ = PROTOCOL_ID;
	*msg_buf++ = REGISTER_WRITE_CONFIRM;

	/* Copy confirmation parameters*/
	*msg_buf++ = status;
	*msg_buf++ = (uint8_t)reg_addr;
	*msg_buf++ = (uint8_t)(reg_addr >> BYTE_LEN);
	*msg_buf++ = reg_val;
	*msg_buf = EOT;
}

/*
 * Function to generate usr_register_dump_confirm that must be sent to
 * host application via serial interface.
 * Called by Performance application as reply to the usr_register_dump_req
 *
 * \param status          Result for usr_register_dump_req
 * \param start_reg_addr  Start Register address that has to be read
 * \param end_reg_addr    End Register address that has to be read
 * \param reg_val         Pointer to array of Register values
 */
void usr_register_dump_confirm(trx_id_t trx, uint8_t status,
		uint16_t start_reg_addr, uint16_t end_reg_addr,
		uint8_t *reg_val)
{
	uint8_t index;
	uint8_t *msg_buf;

	/* Pointer to size element - the content is written later. */
	uint8_t *ptr_to_msg_size;

	msg_buf = get_next_tx_buffer(trx);

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return;
	}

	/* Store pointer to size element. */
	ptr_to_msg_size = msg_buf;

	/* Copy Len, Protocol Id, Msg Id parameters */
	*msg_buf++ = PROTOCOL_ID_LEN + REGISTER_DUMP_CONFIRM_LEN;
	*msg_buf++ = PROTOCOL_ID;
	*msg_buf++ = REGISTER_DUMP_CONFIRM;

	/* Copy confirmation payload */
	*msg_buf++ = status;
	*msg_buf++ = (uint8_t)start_reg_addr;
	*msg_buf++ = (uint8_t)(start_reg_addr >> BYTE_LEN);
	*msg_buf++ = (uint8_t)end_reg_addr;
	*msg_buf++ = (uint8_t)(end_reg_addr >> BYTE_LEN);
	if (reg_val != NULL) {
		*msg_buf++ = reg_val[0];

		/* Update the length field with length field of the octet string
		**/
		*ptr_to_msg_size += 1;

		for (index = 0; index < reg_val[0]; index++) {
			*msg_buf++ = reg_val[index + 1];
		}
		*ptr_to_msg_size += reg_val[0];
	} else {
		*msg_buf++ = NUL_VAL;
		*ptr_to_msg_size += 1;
	}

	*msg_buf = EOT;
}

/*
 * Function to generate Per test End Indication frame that must be sent to
 * host application via serial interface.
 * Called by Performance application as Indication after completion of PER test
 * \param status                Result for PER test initiated by
 * per_test_start_req
 * \param avg_rssi              Average RSSI measured for PER Test
 * \param avg_lqi               Average LQI measured for PER Test
 * \param frames_transmitted    No.of transmitted pkts in the PER Test
 * \param frames_received       No.of received pkts in the PER Test
 * \param frames_failure        No. Of pkts failed to transmit
 * \param frames_no_ack         No. Of pkts transmitted without ACK
 * \param frames_channel_access_failure  No.of Channel access failures
 * \param test_duration_sec     Time taken for the PER test
 * \param test_net_data_rate    Net data rate for the test
 *
 * \return void
 */
void usr_per_test_end_indication(trx_id_t trx,
		uint8_t status,
		int8_t avg_rssi,
		uint8_t avg_lqi,
		uint32_t frames_transmitted,
		uint32_t frames_received,
		uint32_t frames_failure,
		uint32_t frames_no_ack,
		uint32_t frames_channel_access_failure,
		uint32_t frames_with_wrong_crc,
		float test_duration_sec,
		float test_net_data_rate)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer(trx);

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return;
	}

	/* Copy Len, Protocol Id, Msg Id parameters */
	*msg_buf++ = PROTOCOL_ID_LEN + PER_TEST_END_INDICATION_LEN;
	*msg_buf++ = PROTOCOL_ID;
	*msg_buf++ = PER_TEST_END_INDICATION;

	/* Copy Indication payload */
	*msg_buf++ = status;
	*msg_buf++ = avg_rssi;
	*msg_buf++ = avg_lqi;

	memcpy(msg_buf, &frames_transmitted, sizeof(uint32_t));
	msg_buf +=  sizeof(uint32_t);

	memcpy(msg_buf, &frames_received, sizeof(uint32_t));
	msg_buf +=  sizeof(uint32_t);

	memcpy(msg_buf, &frames_failure, sizeof(uint32_t));
	msg_buf +=  sizeof(uint32_t);

	memcpy(msg_buf, &frames_no_ack, sizeof(uint32_t));
	msg_buf +=  sizeof(uint32_t);

	memcpy(msg_buf, &frames_channel_access_failure, sizeof(uint32_t));
	msg_buf +=  sizeof(uint32_t);

	memcpy(msg_buf, &frames_with_wrong_crc, sizeof(uint32_t));
	msg_buf +=  sizeof(uint32_t);

	memcpy(msg_buf, &test_duration_sec, sizeof(float));
	msg_buf +=  sizeof(float);

	memcpy(msg_buf, &test_net_data_rate, sizeof(float));
	msg_buf +=  sizeof(float);

	*msg_buf = EOT;
}

/*
 * Function to generate ED scan confirm test End Indication frame that must be
 * sent to
 * host application via serial interface.
 * Called by Performance application as Indication before starting the ED scan
 * \param status                Confirmation to the ED scan req
 * \param scan_time_min         Approx time to be taken for ed can if time is
 * more than a minute
 * \param scan_time_sec         Approx time to be taken for ed can if time is
 * less than a minute
 *
 * \return void
 */
void usr_ed_scan_start_confirm(trx_id_t trx, uint8_t status,
		uint8_t scan_time_min, float scan_time_sec)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer(trx);

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return;
	}

	/* Copy Len, Protocol Id, Msg Id parameters */
	*msg_buf++ = PROTOCOL_ID_LEN + ED_SCAN_START_CONFIRM_LEN;
	*msg_buf++ = PROTOCOL_ID;
	*msg_buf++ = ED_SCAN_START_CONFIRM;

	/* Copy confirmation payload */
	*msg_buf++ = status;
	*msg_buf++ = scan_time_min;
	memcpy(msg_buf, &scan_time_sec, sizeof(float));
	msg_buf +=  sizeof(float);

	*msg_buf = EOT;
}

/*
 * Function to generate ED scan Indication frame that must be sent to
 * host application via serial interface.
 * Called by Performance application as Indication after completion of the ED
 * scan
 * \param no_of_channels  No. of channels scanned
 * \param ed_scan_result  List of Energy values along with the channel numbers
 *
 * \return void
 */

void usr_ed_scan_end_indication(trx_id_t trx, uint8_t no_of_channels,
		ed_scan_result_t *ed_scan_result )
{
	uint8_t *msg_buf;
	uint8_t ch_cnt;

	/* Pointer to size element - the content is written later */
	uint8_t *ptr_to_msg_size;

	msg_buf = get_next_tx_buffer(trx);

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return;
	}

	/* Store pointer to size element. */
	ptr_to_msg_size = msg_buf;

	/* Copy Len, Protocol Id, Msg Id parameters */
	*msg_buf++ = PROTOCOL_ID_LEN + ED_SCAN_END_INDICATION_LEN;
	*msg_buf++ = PROTOCOL_ID;
	*msg_buf++ = ED_SCAN_END_INDICATION;

	/* Copy Indication payload */
	*msg_buf++ = no_of_channels;
	for (ch_cnt = 0; ch_cnt < no_of_channels; ch_cnt++) {
		*msg_buf++ = ed_scan_result[ch_cnt].channel_no;
		*msg_buf++ = ed_scan_result[ch_cnt].p_in;
	}
	/* Update the length field  */
	*ptr_to_msg_size += (RESULT_SIZE_PER_CHANNEL * no_of_channels);

	*msg_buf = EOT;
}

/*
 * Function to generate Sensor data confirm frame that must be sent to
 * host application via serial interface.
 * Called by Performance application as confirmation after getting the sensor
 * data
 * \param status           Result for the Sensor data get req
 * \param bat_voltage      Battery voltage value
 * \param temperature      Temperature value
 *
 * \return void
 */
void usr_sensor_data_get_confirm(trx_id_t trx, uint8_t status,
		float bat_voltage, float temperature)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer(trx);

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return;
	}

	/* Copy Len, Protocol Id, Msg Id parameters */
	*msg_buf++ = PROTOCOL_ID_LEN + SENSOR_DATA_CONFIRM_LEN;
	*msg_buf++ = PROTOCOL_ID;
	*msg_buf++ = SENSOR_DATA_CONFIRM;

	/* Copy confirmation payload */
	*msg_buf++ = status;
	memcpy(msg_buf, &bat_voltage, sizeof(float));
	msg_buf +=  sizeof(float);

	memcpy(msg_buf, &temperature, sizeof(float));
	msg_buf +=  sizeof(float);

	*msg_buf = EOT;
}

/*
 * Function to generate Disconnect Confirm frame that must be sent to
 * host application via serial interface.
 * Called by Performance application as confirmation after disconnecting from
 * the
 * peer node, if exists
 * \param status           Result for the Peer Disconnect Req
 *
 * \return void
 */
void usr_peer_disconnect_confirm(trx_id_t trx, uint8_t status)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer(trx);

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return;
	}

	/* Copy Len, Protocol Id, Msg Id parameters */
	*msg_buf++ = PROTOCOL_ID_LEN + PEER_DISCONNECT_CONFIRM_LEN;
	*msg_buf++ = PROTOCOL_ID;
	*msg_buf++ = PEER_DISCONNECT_CONFIRM;

	/* Copy confirmation payload */
	*msg_buf++ = status;
	*msg_buf = EOT;
}

/*
 * Function to generate Set default config Confirm frame that must be sent to
 * host application via serial interface.
 * Called by Performance application as confirmation after setting the all
 * configurable parameters on source and peer node, if exists
 * \param status              Result for the set default config req
 *
 * \param trx_config_params   Structure of configurable parameters with
 *                            default values
 * \return void
 */
void usr_set_default_config_confirm(trx_id_t trx, uint8_t status,
		trx_config_params_t *default_trx_config_params)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer(trx);

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return;
	}

	/* Copy Len, Protocol Id, Msg Id parameters */
	*msg_buf++ = PROTOCOL_ID_LEN + SET_DEFAULT_CONFIG_CONFIRM_LEN;
	*msg_buf++ = PROTOCOL_ID;
	*msg_buf++ = SET_DEFAULT_CONFIG_CONFIRM;

	/* Copy confirmation payload */
	*msg_buf++ = status;
	/* configuration parameters */
	*msg_buf++ = (uint8_t)(default_trx_config_params->channel);
	*msg_buf++ = (uint8_t)(default_trx_config_params->channel >> 8);
	*msg_buf++ = default_trx_config_params->channel_page;
	*msg_buf++ = default_trx_config_params->tx_power_dbm;

	*msg_buf++ = default_trx_config_params->tx_power_reg;
	*msg_buf++ = (uint8_t)default_trx_config_params->csma_enabled;
	*msg_buf++ = (uint8_t)default_trx_config_params->retry_enabled;
	*msg_buf++ = (uint8_t)default_trx_config_params->ack_request;

	*msg_buf++ = (uint8_t)default_trx_config_params->rx_desensitize;
	*msg_buf++ = FIELD_DOES_NOT_EXIST; /*Filled with 0xff to indicate this
	                                    * parameter is not available for
	                                    *this
	                                    * transceiver RPC */
	*msg_buf++ = FIELD_DOES_NOT_EXIST; /*Filled with 0xff to indicate this
	                                    * parameter is not available for
	                                    *this
	                                    * transceiver ANTENNA DIVERSITY */

	*msg_buf++ = default_trx_config_params->trx_state;
	*msg_buf++ = (uint8_t)default_trx_config_params->number_test_frames;
	*msg_buf++
		= (uint8_t)(default_trx_config_params->number_test_frames >>
			8);
	*msg_buf++
		= (uint8_t)(default_trx_config_params->number_test_frames >>
			16);
	*msg_buf++
		= (uint8_t)(default_trx_config_params->number_test_frames >>
			24);
	*msg_buf++ = (uint8_t)(default_trx_config_params->phy_frame_length);
	*msg_buf++
		= (uint8_t)(default_trx_config_params->phy_frame_length >> 8);

	/*Peer settings for parameters like CRC and ant diversity */
	*msg_buf++ = FIELD_DOES_NOT_EXIST; /*Filled with 0xff to indicate this
	                                    * parameter is not available for
	                                    *this
	                                    * transceiver ANTENNA DIVERSITY */
	*msg_buf++ = default_trx_config_params->crc_settings_on_peer;
	*msg_buf = EOT;
}

/*
 * Function to generate Identify Board confirm frame that must be sent to
 * host application via serial interface.
 * Called by Performance application as confirmation to the board identify req
 *
 * \param status           Result for the set default config req
 *
 * \param ic_type         Ic Type whether it is SOC or MCU
 * \param mcu_soc_name    MCU or SOC name as string
 * \param trx_name        Transceiver's name as string
 * \param board_name      HW Board name as string
 * \param mac_address     MAC address of the source node
 * \param fw_version      Firmware version
 *
 * \return void
 */
void usr_identify_board_confirm(trx_id_t trx, uint8_t status,
		uint8_t ic_type,
		const char *mcu_soc_name,
		const char *trx_name,
		const char *board_name,
		uint64_t mac_address,
		float fw_version,
		uint32_t fw_feature_mask)
{
	uint8_t *msg_buf;
	/* Pointer to size element - the content is written later. */
	uint8_t *ptr_to_msg_size;

	msg_buf = get_next_tx_buffer(trx);

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return;
	}

	/* Store pointer to size element. */
	ptr_to_msg_size = msg_buf;

	/* Copy Len, Protocol Id, Msg Id parameters */
	*msg_buf++ = PROTOCOL_ID_LEN + IDENTIFY_BOARD_CONFIRM_LEN;
	*msg_buf++ = PROTOCOL_ID; /* protocol id */
	*msg_buf++ = IDENTIFY_BOARD_CONFIRM;

	/* Copy confirmation payload */
	*msg_buf++ = status;
	*msg_buf++ = ic_type;
	if (mcu_soc_name != NULL) {
		*msg_buf++ = strlen(mcu_soc_name);
		memcpy(msg_buf, mcu_soc_name, strlen(mcu_soc_name));
		/* Update the length field with mcu_soc_name length */
		*ptr_to_msg_size += OCTET_STR_LEN_BYTE_LEN +
				strlen(mcu_soc_name);
		msg_buf += strlen(mcu_soc_name);
	} else {
		*msg_buf++ = 0x00; /* Length byte 0 as no string present */
		/* Update the length field with trx_name length */
		*ptr_to_msg_size += OCTET_STR_LEN_BYTE_LEN;
	}

	if (trx_name != NULL) {
		*msg_buf++ = strlen(trx_name);
		memcpy(msg_buf, trx_name, strlen(trx_name));
		/* Update the length field with trx_name length */
		*ptr_to_msg_size += OCTET_STR_LEN_BYTE_LEN + strlen(trx_name);
		msg_buf += strlen(trx_name);
	} else {
		*msg_buf++ = 0x00; /* Length byte 0 as no string present */
		/* Update the length field with trx_name length */
		*ptr_to_msg_size += OCTET_STR_LEN_BYTE_LEN;
	}

	if (board_name != NULL) {
		*msg_buf++ = strlen(board_name);
		memcpy(msg_buf, board_name, strlen(board_name));
		/* Update the length field with trx_name length */
		*ptr_to_msg_size += OCTET_STR_LEN_BYTE_LEN + strlen(board_name);
		msg_buf += strlen(board_name);
	} else {
		*msg_buf++ = 0x00; /* Length byte 0 as no string present */
		/* Update the length field with trx_name length */
		*ptr_to_msg_size += OCTET_STR_LEN_BYTE_LEN;
	}

	/* Copy mac address */
	memcpy(msg_buf, &mac_address, sizeof(mac_address));
	msg_buf += sizeof(mac_address);

	/* copy fw version */
	memcpy(msg_buf, &fw_version, sizeof(fw_version));
	msg_buf += sizeof(fw_version);

	/* copy fw_feature_mask value */
	fw_feature_mask = CCPU_ENDIAN_TO_LE32(fw_feature_mask);
	memcpy(msg_buf, &fw_feature_mask, sizeof(fw_feature_mask));
	msg_buf += sizeof(fw_feature_mask);

	*msg_buf = EOT;
}

/*
 * Function to generate Get Current config Confirm frame that must be sent to
 * host application via serial interface.
 * Called by Performance application as confirmation after getting the all
 * configurable parameters on source and peer node, if exists
 * \param status                  Result for the set default config req
 *
 * \param curr_trx_config_params  Structure of configurable parameters with
 *                                current values
 * \return void
 */
void usr_get_current_config_confirm(trx_id_t trx, uint8_t status,
		trx_config_params_t *curr_trx_conf_params)
{
	uint8_t *msg_buf;

	/* Pointer to size element - the content is written later. */
	uint8_t *ptr_to_msg_size;

	msg_buf = get_next_tx_buffer(trx);

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return;
	}

	/* Store pointer to size element. */
	ptr_to_msg_size = msg_buf;
	/* Copy Len, Protocol Id, Msg Id parameters */
	*msg_buf++ = PROTOCOL_ID_LEN + GET_CURRENT_CONFIG_CONFIRM_LEN;
	*msg_buf++ = PROTOCOL_ID;
	*msg_buf++ = GET_CURRENT_CONFIG_CONFIRM;

	/* Copy confirmation payload */
	*msg_buf++ = status;
	/* configuration parameters */
	*msg_buf++ = (uint8_t)curr_trx_conf_params->channel;
	*msg_buf++ = (uint8_t)(curr_trx_conf_params->channel >> 8);
	if (curr_trx_conf_params->channel_page == CH_PG_SUN) {
		*msg_buf++ = curr_trx_conf_params->sun_phy_page.page_no;
		*msg_buf++ = curr_trx_conf_params->sun_phy_page.freq_band;
		*msg_buf++ = curr_trx_conf_params->sun_phy_page.modulation;
		*ptr_to_msg_size += 3;
		if (curr_trx_conf_params->sun_phy_page.modulation == OFDM) {
			*msg_buf++
				= curr_trx_conf_params->sun_phy_page.
					sun_phy_mode
					.mr_ofdm.option;
			*msg_buf++
				= curr_trx_conf_params->sun_phy_page.
					sun_phy_mode
					.mr_ofdm.mcs_val;
			*msg_buf++
				= curr_trx_conf_params->sun_phy_page.
					sun_phy_mode
					.mr_ofdm.interl;
			*ptr_to_msg_size += 3;
		} else if (curr_trx_conf_params->sun_phy_page.modulation ==
				OQPSK) {
			*msg_buf++
				= curr_trx_conf_params->sun_phy_page.
					sun_phy_mode
					.mr_oqpsk.rate_mode;
			*msg_buf++
				= curr_trx_conf_params->sun_phy_page.
					sun_phy_mode
					.mr_oqpsk.chip_rate;

			*ptr_to_msg_size += 2;
		} else if (curr_trx_conf_params->sun_phy_page.modulation ==
				FSK) {
			*msg_buf++
				= curr_trx_conf_params->sun_phy_page.
					sun_phy_mode
					.mr_fsk.mod_type;
			*msg_buf++
				= curr_trx_conf_params->sun_phy_page.
					sun_phy_mode
					.mr_fsk.mod_idx;
			*msg_buf++
				= curr_trx_conf_params->sun_phy_page.
					sun_phy_mode
					.mr_fsk.sym_rate;
			*msg_buf++
				= curr_trx_conf_params->sun_phy_page.
					sun_phy_mode
					.mr_fsk.op_mode;
			*msg_buf++
				= curr_trx_conf_params->sun_phy_page.
					sun_phy_mode
					.mr_fsk.bt;
			*msg_buf++
				= curr_trx_conf_params->sun_phy_page.
					sun_phy_mode
					.mr_fsk.fec_enabled;

			*ptr_to_msg_size += 6;
		} else if (curr_trx_conf_params->sun_phy_page.modulation ==
				LEG_OQPSK) {
			*msg_buf++
				= curr_trx_conf_params->sun_phy_page.
					sun_phy_mode
					.leg_oqpsk.data_rate;
			*msg_buf++
				= curr_trx_conf_params->sun_phy_page.
					sun_phy_mode
					.leg_oqpsk.chip_rate;

			*ptr_to_msg_size += 2;
		}
	} else {
		*msg_buf++ = curr_trx_conf_params->channel_page;
		*ptr_to_msg_size += 1;
	}

	*msg_buf++ = curr_trx_conf_params->tx_power_dbm;

	*msg_buf++ = curr_trx_conf_params->tx_power_reg;

	*msg_buf++ = (uint8_t)curr_trx_conf_params->csma_enabled;
	*msg_buf++ = (uint8_t)curr_trx_conf_params->retry_enabled;
	*msg_buf++ = (uint8_t)curr_trx_conf_params->ack_request;
#if (TAL_TYPE != AT86RF230B)
	*msg_buf++ = (uint8_t)curr_trx_conf_params->rx_desensitize;
#else
	*msg_buf++ = FIELD_DOES_NOT_EXIST; /*Filled with 0xff to indicate this
	                                    * parameter is not available for
	                                    *this
	                                    * transceiver */
#endif

	*msg_buf++ = FIELD_DOES_NOT_EXIST; /*Filled with 0xff to indicate this
	                                    * parameter is not available for
	                                    *this
	                                    * transceiver RPC */
	*msg_buf++ = FIELD_DOES_NOT_EXIST; /*Filled with 0xff to indicate this
	                                    * parameter is not available for
	                                    *this
	                                    * transceiver ANTENNA DIVERSITY */

	*msg_buf++ = curr_trx_conf_params->trx_state;
	*msg_buf++ = (uint8_t)curr_trx_conf_params->number_test_frames;
	*msg_buf++ = (uint8_t)(curr_trx_conf_params->number_test_frames >> 8);
	*msg_buf++ = (uint8_t)(curr_trx_conf_params->number_test_frames >> 16);
	*msg_buf++ = (uint8_t)(curr_trx_conf_params->number_test_frames >> 24);
	*msg_buf++ = (uint8_t)(curr_trx_conf_params->phy_frame_length);
	*msg_buf++ = (uint8_t)(curr_trx_conf_params->phy_frame_length >> 8);

	/*peer node settings need to be added */
	/*Peer settings for parameters like CRC and ant diversity */
	*msg_buf++ = FIELD_DOES_NOT_EXIST; /*Filled with 0xff to indicate this
	                                    * parameter is not available for
	                                    *this
	                                    * transceiver ANTENNA DIVERSITY */
	*msg_buf++ = curr_trx_conf_params->crc_settings_on_peer;

	float temp = 0.0;
	memcpy(msg_buf, &temp, sizeof(float)); /* ism freq */

	msg_buf += sizeof(float);
	*msg_buf = EOT;
}

/* EOF */
