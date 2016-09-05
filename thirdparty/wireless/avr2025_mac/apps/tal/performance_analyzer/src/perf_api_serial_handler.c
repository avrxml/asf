/**
 * \file perf_api_serial_handler.c
 *
 * \brief SIO service implementation - Performance Analyzer application
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

/* === INCLUDES ============================================================ */
#include "compiler.h"
#include "tal.h"
#include "return_val.h"
#include "ieee_const.h"
#include "app_init.h"
#include "perf_msg_const.h"
#include "sio2host.h"
#include "perf_api.h"
#include "perf_api_serial_handler.h"
#include "tal_helper.h"

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
static uint8_t sio_rx_buf[SIO_RX_BUF_SIZE];

/**
 * This is the transmit buffer of the UART.
 */
static uint8_t sio_tx_buf[SIO_BUF_COUNT][SIO_TX_BUF_SIZE];

/**
 * This pointer points to the next free element inside the
 * receive buffer of the UART.
 */
static uint8_t *sio_rx_ptr;

/**
 * This is the receiver state of the UART. (\ref UART_RX_STATE_SOT,
 *   \ref UART_RX_STATE_LENGTH, \ref UART_RX_STATE_DATA, or
 *   \ref UART_RX_STATE_EOT)
 */
static uint8_t sio_rx_state;

/**
 * This is the transmit state of the UART. (\ref UART_TX_STATE_SOT,
 *   \ref UART_TX_STATE_LENGTH, \ref UART_TX_STATE_DATA, or
 *   \ref UART_TX_STATE_EOT)
 */

/**
 * This is the length of the message should be received.
 */
static uint8_t sio_rx_length;

/**
 * This is the buffer to hold the frame received through serial interface
 */
static uint8_t data[SIO_RX_BUF_SIZE];

/**
 * This is length variable to keep track of no of received bytes
 */
static uint8_t data_length = 0;

/**
 * This is index used to validate the received bytes based on their position
 */
static uint8_t rx_index = 0;

/**
 * This is head of the queue of buffers to be transmitted
 */
static uint8_t head = 0;

/**
 * This is buffer count to keep track of the available bufer for transmission
 */
static uint8_t buf_count = 0;

/**
 * This flag is used to check if a remote_test command has been received or not
 */
bool remote_cmd_rcvd = false;

/* === Prototypes ========================================================== */

static inline void process_incoming_sio_data(void);
static uint8_t *get_next_tx_buffer(void);
static inline void handle_incoming_msg(void);

static uint8_t curr_tx_buffer_index = 0;

/* ! \} */
/* === Implementation ====================================================== */

/*
 * \brief This function does the initialization of the Serial handler state
 * Machine.
 */
void init_sio(void)
{
	sio_rx_state = UART_RX_STATE_SOT;
}

/*
 * \brief Function to handle the state machine serial data exchange.
 */
void serial_data_handler(void)
{
	/* Serial data handler is to handle only PER mode msg commands */
	if ((RANGE_TEST_TX_ON == node_info.main_state) ||
			(RANGE_TEST_TX_OFF == node_info.main_state)) {
		return;
	}

	/*Remote Node Tests*/
	if ((PER_TEST_RECEPTOR == node_info.main_state) &&  buf_count) {
		remote_cmd_rcvd = false;
		sio_tx_buf[head][CMD_POS] |= REMOTE_CMD_MASK;
		if (send_remote_reply_cmd(&sio_tx_buf[head][curr_tx_buffer_index
				], (sio_tx_buf[head][1]))) {
			buf_count--;
		}

		return;
	}

	if (remote_serial_tx_failure) {
		sio_tx_buf[head][CMD_POS] |= REMOTE_CMD_MASK;
		/*reset the flag*/
		remote_serial_tx_failure = false;
	}

	/* Rx processing */
	if (data_length == 0) {
		/* No data to process, read the stream IO */
		rx_index = 0;
		data_length = sio2host_rx(data, SIO_RX_BUF_SIZE);
	} else { /* Data has been received, process the data */
		 /* Process each single byte */
		process_incoming_sio_data();
		data_length--;
		rx_index++;
	}

	/* Tx processing */
	if (buf_count != 0) {
		/* Check Continuous transmission is finished at remote node,
		 * if so Blink LED at initiator */
		uint8_t cont_tx_remote  = sio_tx_buf[head][CMD_POS];
		if ((cont_tx_remote ==
				(REMOTE_CMD_MASK | PKT_STREAM_CONFIRM)) ||
				(cont_tx_remote ==
				(REMOTE_CMD_MASK | CONT_WAVE_TX_CONFIRM))) {
			uint8_t stop_cont_tx = sio_tx_buf[head][5];
			if (stop_cont_tx == 0) {
				sw_timer_start(T_APP_TIMER,
						LED_BLINK_RATE_IN_MICRO_SEC,
						SW_TIMEOUT_RELATIVE,
						(FUNC_PTR)led_blinker_timer_handler_cb,
						NULL);
			}
		}

		uint8_t no_of_bytes_transmitted
			= sio2host_tx(
				&sio_tx_buf[head][curr_tx_buffer_index],
				(sio_tx_buf[head][1] +
				3) - curr_tx_buffer_index );

		/* Transmission of the current buffer is done completely */
		if (no_of_bytes_transmitted ==
				((sio_tx_buf[head][1] +
				3) - curr_tx_buffer_index)) {
			head++;
			head %= SIO_BUF_COUNT;
			buf_count--;
			curr_tx_buffer_index = 0;
		} else { /* If none or some of the bytes in tx buffer is not
			  * transmitted */
			curr_tx_buffer_index += no_of_bytes_transmitted;
		}
	}
}

/**
 * \brief Process data received from SIO
 */
static inline void process_incoming_sio_data(void)
{
	switch (sio_rx_state) {
	case UART_RX_STATE_SOT:
		sio_rx_ptr = sio_rx_buf;

		/* A valid SOT is received when the sio rx state is in Idle
		 * state  */
		if (SOT == data[rx_index]) {
			sio_rx_state = UART_RX_STATE_LENGTH;
		}

		break;

	case UART_RX_STATE_LENGTH:
		/* Length byte has been received */
		sio_rx_length = data[rx_index];

		/* Change the sio rx state to receive the payload, if the length
		 * is a nonzero
		 */
		if (sio_rx_length) {
			sio_rx_state = UART_RX_STATE_DATA;
			*sio_rx_ptr = sio_rx_length;
			sio_rx_ptr++;
		} else {
			/* NULL message */
			sio_rx_ptr = sio_rx_buf;
			sio_rx_state = UART_RX_STATE_SOT;
		}

		break;

	case UART_RX_STATE_DATA:
		/* Receive the data payload of 'length' no. of  bytes */
		*sio_rx_ptr = data[rx_index];
		sio_rx_ptr++;
		sio_rx_length--;
		if (!sio_rx_length) {
			sio_rx_state = UART_RX_STATE_EOT;
		}

		break;

	case UART_RX_STATE_EOT:

		/* Valid EOT is received after reception of 'length' no of bytes
		**/
		if (EOT == data[rx_index]) {
			/* Message received successfully */
			handle_incoming_msg();
		}

		/* Make rx buffer ready for next reception before handling
		 * received data. */
		sio_rx_ptr = sio_rx_buf;
		sio_rx_state = UART_RX_STATE_SOT;
		break;

	default:
		/* Handling of invalid sio rx state */
		sio_rx_ptr = sio_rx_buf;
		sio_rx_state = UART_RX_STATE_SOT;
		break;
	}
}

/**
 * \brief get the new buffer for next transmission through serial
 *
 ******\return unsigned integer pointer to buf
 */
static uint8_t *get_next_tx_buffer(void)
{
	if (buf_count != SIO_BUF_COUNT) {
		uint8_t *buf;
		uint8_t tail;

		tail = (head + buf_count) % SIO_BUF_COUNT;
		buf = (uint8_t *)(&sio_tx_buf[tail]);
		buf_count++;
		/* Add message start character */
		*buf++ = SOT;
		return buf;
	}

	return NULL;
}

/**
 * *\brief  The received over the air payload containing the serial data request
 * for the remote node is converter into serial data to be used by
 * the Serial Handler for processing further requests
 * \param buf pointer to the payload
 * len Length of the Serial Data Payload
 */
void convert_ota_serial_frame_rx(uint8_t *buf, uint8_t len)
{
	/* Copy the serial data payload to sio_rx_buf ,len+1 denotes copying
	 * payload
	 * including the length field*/
	memcpy(sio_rx_buf, buf, len + 1);
	handle_incoming_msg();
}

/**
 * \brief Parses the Received Data in the Buffer and Process the Commands
 * accordingly.
 */
static inline void handle_incoming_msg(void)
{
	uint8_t error_code = MAC_SUCCESS;
	param_value_t param_value_temp;

	/* Check for protocol id is Performance Analyzer */
	if (PROTOCOL_ID != sio_rx_buf[1]) {
		return;
	}

	/* If the node is in any of these state dont respond to any serial
	 * commands */
	if ((PEER_SEARCH_RANGE_RX == node_info.main_state) ||
			(PEER_SEARCH_RANGE_TX == node_info.main_state) ||
			(RANGE_TEST_TX_ON == node_info.main_state) ||
			(RANGE_TEST_TX_OFF == node_info.main_state)) {
		return;
	}

	/* Check for the error conditions */
	error_code = check_error_conditions();

	/* Check if the message needs to be sent over the air to the remote node
	 * for performing tests on the remote node*/
	if ((sio_rx_buf[MESSAGE_ID_POS] & REMOTE_CMD_MASK) &&
			(PER_TEST_INITIATOR == node_info.main_state)) {
		if (error_code ==  MAC_SUCCESS) {
			if (((sio_rx_buf[MESSAGE_ID_POS] & MESSAGE_ID_MASK) ==
					RX_ON_REQ) &&
					(sio_rx_buf[START_STOP_POS] ==
					RX_ON_STOP)) {
				send_remote_cmd(sio_rx_buf, *sio_rx_buf, false);
			} else {
				send_remote_cmd(sio_rx_buf, *sio_rx_buf, true);
			}

			return;
		} else { /*Send Error Code back*/
			sio_rx_buf[MESSAGE_ID_POS] &=  MESSAGE_ID_MASK;
		}
	}
	/*Perform test on the receptor*/
	else if ((sio_rx_buf[MESSAGE_ID_POS] &&
			REMOTE_CMD_MASK) &&
			(PER_TEST_RECEPTOR == node_info.main_state)) {
		sio_rx_buf[MESSAGE_ID_POS] &=  MESSAGE_ID_MASK;
		remote_cmd_rcvd = true;
	}

	/* Process the commands */
	/* Check for message id */
	switch (sio_rx_buf[MESSAGE_ID_POS]) {
	/* Process Board identification command to get the board details */
	case IDENTIFY_BOARD_REQ:
	{
		/* For GUI development purpose removed the valid
		 * state checking to execute this cmd
		 */
		get_board_details();
	}
	break;

	/* Process Performance test start Request */
	case PERF_START_REQ:
	{
		/* Order of reception:
		 * size;
		 * protocol_id;
		 * msg_id;
		 * start_mode;
		 */
		trx_config_params_t dummy_param_values = {0};

		/* Make sure that the node is in WAIT_FOR_EVENT state to process
		 * this command */
		if (WAIT_FOR_EVENT == node_info.main_state) {
			/* Check for PER mode */
			if (START_MODE_PER == sio_rx_buf[START_MODE_POS]) {
				/* start_req received on UART - so change to
				 * state PEER_SEARCH_PER_TX */
				set_main_state(PEER_SEARCH_PER_TX, NULL);
			}
			/* Check for Single node tests */
			else if (START_MODE_SINGLE_NODE ==
					sio_rx_buf[START_MODE_POS]) {
				/* start_req received on UART - so change to
				 * single node tests state directly */
				set_main_state(SINGLE_NODE_TESTS, NULL);
			} else {
				/* Send the confirmation with status as
				 * INVALID_ARGUMENT */
				usr_perf_start_confirm(INVALID_ARGUMENT,
						sio_rx_buf[START_MODE_POS],
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
			usr_perf_start_confirm(INVALID_CMD,
					sio_rx_buf[START_MODE_POS],
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
		 * parameter_type;
		 * param_value;
		 */

		/* check for length  */
		/* Check any ongoing transaction in place */
		if (((error_code != MAC_SUCCESS) &&
				(error_code != TRANSCEIVER_IN_SLEEP)) ||
#if (TAL_TYPE != AT86RF233)
				((error_code == TRANSCEIVER_IN_SLEEP) &&
				((sio_rx_buf[PARAM_TYPE_POS] !=
				PARAM_TRX_STATE) ||
				(sio_rx_buf[PARAM_VALUE_POS] !=
				TRX_SLEEP)))
#else
				((error_code == TRANSCEIVER_IN_SLEEP) &&
				((sio_rx_buf[PARAM_TYPE_POS] !=
				PARAM_TRX_STATE) ||
				((sio_rx_buf[PARAM_VALUE_POS] != TRX_SLEEP) &&
				(sio_rx_buf[PARAM_VALUE_POS] !=
				TRX_DEEP_SLEEP))))
#endif
				) {
			/* Send the confirmation with status as Failure
			 * with error code as the reason for failure
			 */
			param_value_temp.param_value_8bit
				= sio_rx_buf[PARAM_VALUE_POS];
			usr_perf_set_confirm(error_code,
					sio_rx_buf[PARAM_TYPE_POS],
					&param_value_temp);
			return;
		}

		/* The node should be in the PER_TEST_INITIATOR
		 * or SINGLE_NODE_TESTS state to allow this req
		 */
		if ((PER_TEST_INITIATOR == node_info.main_state) ||
				(PER_TEST_RECEPTOR ==
				node_info.main_state) ||
				(SINGLE_NODE_TESTS == node_info.main_state)
				) {
			MEMCPY_ENDIAN(&param_value_temp,
					&sio_rx_buf[PARAM_VALUE_POS],
					get_param_length(sio_rx_buf[
						PARAM_TYPE_POS]));
			/* Parameter type followed by parameter value */
			perf_set_req(sio_rx_buf[PARAM_TYPE_POS],
					&param_value_temp
					);
		} else {
			/* Send the confirmation with status as INVALID_CMD as
			 * this command is not allowed in this state
			 */
			param_value_temp.param_value_8bit
				= sio_rx_buf[PARAM_VALUE_POS];
			usr_perf_set_confirm(INVALID_CMD,
					sio_rx_buf[PARAM_TYPE_POS],
					&param_value_temp);
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
		 * parameter_type;
		 */

		param_value_t param_value;
		param_value.param_value_8bit = NUL_VAL;
		/* Check any ongoing transaction in place */
		if (error_code) {
			/* Send the confirmation with status as Failure
			 * with error code as the reason for failure
			 */
			usr_perf_get_confirm(error_code,
					sio_rx_buf[PARAM_TYPE_POS],
					(param_value_t *)&param_value);
			return;
		}

		/* The node should be in the PER_TEST_INITIATOR
		 * or SINGLE_NODE_TESTS state to allow this req
		 */
		if ((PER_TEST_INITIATOR == node_info.main_state) ||
				(PER_TEST_RECEPTOR ==
				node_info.main_state) ||
				(SINGLE_NODE_TESTS == node_info.main_state)
				) {
			perf_get_req(sio_rx_buf[PARAM_TYPE_POS]);
		} else {
			/* Send the confirmation with status as INVALID_CMD as
			 * this command is not allowed in this state
			 */
			usr_perf_get_confirm(INVALID_CMD,
					sio_rx_buf[PARAM_TYPE_POS],
					(param_value_t *)&param_value);
		}
	}
	break;

	/* Process Peer Identification command */
	case IDENTIFY_PEER_NODE_REQ:
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
			usr_identify_peer_node_confirm(error_code);
			return;
		}

		/* The node should be in the PER_TEST_INITIATOR state to allow
		 * this req */
		if (PER_TEST_INITIATOR == node_info.main_state) {
			identify_peer_node();
		} else {
			/* Send the confirmation with status as INVALID_CMD as
			 * this command is not allowed in this state
			 */
			usr_identify_peer_node_confirm(INVALID_CMD);
		}
	}
	break;

	/* Process Continuous Pulse transmission req command */
	case CONT_PULSE_TX_REQ:
	{
#if ((TAL_TYPE != AT86RF230B) || ((TAL_TYPE == AT86RF230B) && \
		(defined CW_SUPPORTED)))

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
			usr_cont_pulse_tx_confirm(error_code);
			return;
		}

		/* The node should be in the PER_TEST_INITIATOR
		 * or SINGLE_NODE_TESTS state to allow this req
		 */
		if ((PER_TEST_INITIATOR == node_info.main_state) ||
				(SINGLE_NODE_TESTS == node_info.main_state) ||
				(PER_TEST_RECEPTOR ==
				node_info.main_state)
				) {
			pulse_cw_transmission();
		} else {
			/* Send the confirmation with status as INVALID_CMD as
			 * this command is not allowed in this state
			 */
			usr_cont_pulse_tx_confirm(INVALID_CMD);
		}

#else
		usr_cont_pulse_tx_confirm(INVALID_CMD);
#endif /* ((TAL_TYPE != AT86RF230B) || ((TAL_TYPE == AT86RF230B) && (defined
			 * CW_SUPPORTED))) */
	}
	break;

	/* Process Continuous wave transmission req command */
	case CONT_WAVE_TX_REQ:
	{
#if ((TAL_TYPE != AT86RF230B) || ((TAL_TYPE == AT86RF230B) && \
		(defined CW_SUPPORTED)))

		/* Order of reception:
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
				((sio_rx_buf[START_STOP_POS] !=
				STOP_CWT) ||
				(sio_rx_buf[TX_MODE_POS] !=
				cont_tx_mode)))) {
			/* Send the confirmation with status as Failure
			 * with error code as the reason for failure
			 */
			usr_cont_wave_tx_confirm(error_code,
					sio_rx_buf[START_STOP_POS],
					sio_rx_buf[TX_MODE_POS]);
			return;
		}

		/* The node should be in the PER_TEST_INITIATOR
		 * or SINGLE_NODE_TESTS state to allow this req
		 */
		if ((PER_TEST_INITIATOR == node_info.main_state) ||
				(PER_TEST_RECEPTOR ==
				node_info.main_state) ||
				(SINGLE_NODE_TESTS == node_info.main_state)
				) {
			/*  Check for Start or Stop CWT ,
			 *  Start_Stop mode - Start =0x01, Stop = 0x00 */
			if (START_CWT == sio_rx_buf[START_STOP_POS]) {
				/* Check for transmission mode,
				 *  tx_mode - CW = 0x00, PRBS = 0x01 */
				start_cw_transmission(sio_rx_buf[TX_MODE_POS],
						((uint16_t)(sio_rx_buf[TMR_VAL_2
						] << 8) |
						(sio_rx_buf[TMR_VAL_1])));
			} else if (STOP_CWT == sio_rx_buf[START_STOP_POS]) {
				/* tx_mode - CW = 0x00, PRBS = 0x01 */
				stop_cw_transmission(&(sio_rx_buf[TX_MODE_POS]));
			} else {
				/* Send the confirmation with status as
				 * INVALID_ARGUMENT
				 * as the start stop mode argument is not a
				 * valid one
				 */
				usr_cont_wave_tx_confirm(INVALID_ARGUMENT,
						sio_rx_buf[START_STOP_POS],
						sio_rx_buf[TX_MODE_POS]);
			}

			/* Update currently running Continuous wave transmission
			 * mode */
			cont_tx_mode = sio_rx_buf[TX_MODE_POS];
		} else {
			/* Send the confirmation with status as INVALID_CMD
			 * as this command is not allowed in this state
			 */
			usr_cont_wave_tx_confirm(INVALID_CMD,
					sio_rx_buf[START_STOP_POS],
					sio_rx_buf[TX_MODE_POS]);
		}

#else
		usr_cont_wave_tx_confirm(INVALID_CMD,
				sio_rx_buf[START_STOP_POS],
				sio_rx_buf[TX_MODE_POS]);
#endif /* ((TAL_TYPE != AT86RF230B) || ((TAL_TYPE == AT86RF230B) && (defined
			 * CW_SUPPORTED))) */
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
			usr_register_read_confirm(error_code,
					((uint16_t)(sio_rx_buf[REGISTER_ADDR_POS
					+ 1] <<
					8) |
					(sio_rx_buf[REGISTER_ADDR_POS])),
					NUL_VAL);
			return;
		}

		/* The node should be in the PER_TEST_INITIATOR
		 * or SINGLE_NODE_TESTS state to allow this req
		 */
		if ((PER_TEST_INITIATOR == node_info.main_state) ||
				(PER_TEST_RECEPTOR ==
				node_info.main_state) ||
				(SINGLE_NODE_TESTS == node_info.main_state)
				) {
			read_trx_registers(((uint16_t)(sio_rx_buf[
						REGISTER_ADDR_POS +
						1] <<
					8) |
					(sio_rx_buf[REGISTER_ADDR_POS])));
		} else {
			/* Send the confirmation with status as INVALID_CMD
			 * as this command is not allowed in this state
			 */
			usr_register_read_confirm(INVALID_CMD,
					((uint16_t)(sio_rx_buf[REGISTER_ADDR_POS
					+ 1] <<
					8) |
					(sio_rx_buf[REGISTER_ADDR_POS])),
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
			usr_register_write_confirm(error_code,
					((uint16_t)(sio_rx_buf[REGISTER_ADDR_POS
					+ 1] <<
					8) |
					(sio_rx_buf[REGISTER_ADDR_POS])),
					NUL_VAL);
			return;
		}

		/* The node should be in the PER_TEST_INITIATOR
		 * or SINGLE_NODE_TESTS state to allow this req
		 */
		if ((PER_TEST_INITIATOR == node_info.main_state) ||
				(PER_TEST_RECEPTOR ==
				node_info.main_state) ||
				(SINGLE_NODE_TESTS == node_info.main_state)
				) {
			write_trx_registers(((uint16_t)(sio_rx_buf[
						REGISTER_ADDR_POS +
						1] <<
					8) |
					(sio_rx_buf[REGISTER_ADDR_POS])),
					sio_rx_buf[REGISTER_VAL_POS] );
		} else {
			/* Send the confirmation with status as INVALID_CMD
			 * as this command is not allowed in this state
			 */
			usr_register_write_confirm(INVALID_CMD,
					((uint16_t)(sio_rx_buf[REGISTER_ADDR_POS
					+ 1] <<
					8) |
					(sio_rx_buf[REGISTER_ADDR_POS])),
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
			usr_register_dump_confirm(error_code,
					((uint16_t)(sio_rx_buf[
						START_REG_ADDR_POS +
						1] <<
					8) |
					(sio_rx_buf[START_REG_ADDR_POS])),
					(((uint16_t)sio_rx_buf[END_REG_ADDR_POS
					+ 1] <<
					8) |
					(sio_rx_buf[END_REG_ADDR_POS])),
					NULL);
			return;
		}

		/* The node should be in the PER_TEST_INITIATOR
		 * or SINGLE_NODE_TESTS state to allow this req
		 */
		if ((PER_TEST_INITIATOR == node_info.main_state) ||
				(PER_TEST_RECEPTOR ==
				node_info.main_state) ||
				(SINGLE_NODE_TESTS == node_info.main_state)
				) {
			dump_trx_register_values(((uint16_t)(sio_rx_buf[
						START_REG_ADDR_POS +
						1] <<
					8) |
					(sio_rx_buf[START_REG_ADDR_POS])),
					(((uint16_t)sio_rx_buf[END_REG_ADDR_POS
					+ 1] <<
					8) |
					(sio_rx_buf[END_REG_ADDR_POS])));
		} else {
			/* Send the confirmation with status as INVALID_CMD
			 * as this command is not allowed in this state
			 */
			usr_register_dump_confirm(INVALID_CMD,
					((uint16_t)(sio_rx_buf[
						START_REG_ADDR_POS +
						1] <<
					8) |
					(sio_rx_buf[START_REG_ADDR_POS])),
					(((uint16_t)sio_rx_buf[END_REG_ADDR_POS
					+ 1] <<
					8) |
					(sio_rx_buf[END_REG_ADDR_POS])),
					NULL);
		}
	}
	break;

	/* Process PacketStream test Start Request command */
	case PKT_STREAM_REQ:
	{
		if ((PER_TEST_INITIATOR == node_info.main_state) ||
				(PER_TEST_RECEPTOR ==
				node_info.main_state) ||
				(SINGLE_NODE_TESTS == node_info.main_state)) {
			/* Check any ongoing transaction in place */
			if ((error_code &&
					(error_code ==
					PKT_STREAM_IN_PROGRESS) &&
					(sio_rx_buf[START_STOP_POS] ==
					PKTSTREAM_STOP)) ||
					(error_code == MAC_SUCCESS)) {
				uint16_t frame_len
					= (uint16_t)(sio_rx_buf[FRAME_LEN_1
						]  |
						sio_rx_buf[FRAME_LEN_2]
						<< 8);
				uint16_t gap_time
					= (uint16_t)(sio_rx_buf[GAP_TIME_1
						]  |
						sio_rx_buf[GAP_TIME_2]
						<< 8);
				uint16_t timeout
					= (uint16_t)(sio_rx_buf[TIMEOUT_VAL_1
						]  |
						sio_rx_buf[TIMEOUT_VAL_2
						]  << 8);
				pktstream_test(gap_time, timeout,
						sio_rx_buf[START_STOP_POS],
						frame_len);
			} else {
				usr_pkt_stream_confirm(error_code,
						sio_rx_buf[START_STOP_POS]);
				return;
			}
		} else {
			usr_pkt_stream_confirm(INVALID_CMD,
					sio_rx_buf[START_STOP_POS]);
		}
	}
	break;

	case RX_ON_REQ:
	{
		if ((PER_TEST_INITIATOR == node_info.main_state) ||
				(PER_TEST_RECEPTOR ==
				node_info.main_state) ||
				(SINGLE_NODE_TESTS == node_info.main_state)) {
			/* Check any ongoing transaction in place */
			if ((error_code &&
					(error_code ==
					RX_ON_MODE_IN_PROGRESS) &&
					(sio_rx_buf[START_STOP_POS] ==
					RX_ON_STOP)) ||
					(error_code == MAC_SUCCESS)) {
				rx_on_test(sio_rx_buf[START_STOP_POS]);
			} else {
				usr_rx_on_confirm(error_code,
						sio_rx_buf[START_STOP_POS]);
				return;
			}
		} else {
			usr_rx_on_confirm(INVALID_CMD,
					sio_rx_buf[START_STOP_POS]);
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

		/* This variable is to save the selected channels mask,
		 * which is a 4byte value received through serial interface
		 */
		uint32_t rcvd_channel_mask = 0;
		/* Clear for every ED SCAN REQ MSG */

		/* Check any ongoing transaction in place */
		if (error_code) {
			/* Send the confirmation with status as Failure
			 * with error code as the reason for failure
			 */
			usr_ed_scan_start_confirm(error_code, NUL_VAL, NUL_VAL);
			return;
		}

		/* The node should be in the PER_TEST_INITIATOR
		 * or SINGLE_NODE_TESTS state to allow this req
		 */
		if ((PER_TEST_INITIATOR == node_info.main_state) ||
				(SINGLE_NODE_TESTS == node_info.main_state)
				) {
			if (sio_rx_buf[MSG_LEN_ED_SCAN_POS] ==
					MSG_LEN_ED_SCAN_REQ) {
				/* Extract the 4bytes of selected channel mask
				 * from the sio_rx_buf */
				rcvd_channel_mask
					|= (((uint32_t)sio_rx_buf[
							CHANNELS_SELECT_POS
						]) & 0x000000FF);
				rcvd_channel_mask
					|= (((uint32_t)sio_rx_buf[
							CHANNELS_SELECT_POS
							+ 1] <<
						8) & 0x0000FF00);
				rcvd_channel_mask
					|= (((uint32_t)sio_rx_buf[
							CHANNELS_SELECT_POS
							+ 2] <<
						16) & 0x00FF0000);
				rcvd_channel_mask
					|= (((uint32_t)sio_rx_buf[
							CHANNELS_SELECT_POS
							+ 3] <<
						24) & 0xFF000000);
			} else {
				rcvd_channel_mask = TRX_SUPPORTED_CHANNELS;
			}

			start_ed_scan(sio_rx_buf[SCAN_DURATION_POS],
					rcvd_channel_mask);
		} else {
			/* Send the confirmation with status as INVALID_CMD
			 * as this command is not allowed in this state
			 */
			usr_ed_scan_start_confirm(INVALID_CMD, NUL_VAL,
					NUL_VAL);
		}
	}
	break;

	/* Process Sensor data get Request command */
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
			usr_sensor_data_get_confirm(error_code, NUL_VAL,
					NUL_VAL);
			return;
		}

		/* The node should be in the PER_TEST_INITIATOR
		 * or SINGLE_NODE_TESTS state to allow this req
		 */
		if ((PER_TEST_INITIATOR == node_info.main_state) ||
				(PER_TEST_RECEPTOR ==
				node_info.main_state) ||
				(SINGLE_NODE_TESTS == node_info.main_state)
				) {
			get_sensor_data();
		} else {
			usr_sensor_data_get_confirm(INVALID_CMD, NUL_VAL,
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
			usr_per_test_start_confirm(error_code);
			return;
		}

		/* The node should be in the PER_TEST_INITIATOR state to start
		 * PER TEST */
		if (PER_TEST_INITIATOR == node_info.main_state) {
			initiate_per_test();
		} else {
			/* Send the confirmation with status as INVALID_CMD
			 * as this command is not allowed in this state
			 */
			usr_per_test_start_confirm(INVALID_CMD);
		}
	}
	break;

	/* Process Range Test start command */
	case RANGE_TEST_START_REQ:
	{
		if (error_code) {
			/* Send the confirmation with status as Failure
			 * with error code as the reason for failure
			 */
			usr_range_test_start_confirm(error_code);
			return;
		}

		/* The node should be in the PER_TEST_INITIATOR state to start
		 * the GUI based RANGE TEST */
		if (PER_TEST_INITIATOR == node_info.main_state) {
			initiate_range_test();
		} else {
			/* Send the confirmation with status as INVALID_CMD
			 * as this command is not allowed in this state
			 */
			usr_range_test_start_confirm(INVALID_CMD);
		}
	}
	break;

	case RANGE_TEST_STOP_REQ:
	{
		/* Change the mode of initiator and send stop cmd to receptor
		 * once the range test stop command is received */
		stop_range_test();
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
			usr_peer_disconnect_confirm(error_code);
			return;
		}

		/* The node can be in any sate to do a app reset */
		/* Disconnect the peer node*/
		disconnect_peer_node();
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
			usr_set_default_config_confirm(error_code,
					&dummy_params);
			return;
		}

		/* The node should be in the PER_TEST_INITIATOR
		 * or SINGLE_NODE_TESTS state to allow this req
		 */
		if ((PER_TEST_INITIATOR == node_info.main_state) ||
				(PER_TEST_RECEPTOR ==
				node_info.main_state) ||
				(SINGLE_NODE_TESTS == node_info.main_state)
				) {
			set_default_configuration();
		} else {
			/* Send the confirmation with status as INVALID_CMD
			 * as this command is not allowed in this state
			 */
			usr_set_default_config_confirm(INVALID_CMD,
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
			usr_get_current_config_confirm(error_code,
					&dummy_params);
			return;
		}

		/* The node should be in the PER_TEST_INITIATOR
		 * or SINGLE_NODE_TESTS state to allow this req
		 */
		if ((PER_TEST_INITIATOR == node_info.main_state) ||
				(PER_TEST_RECEPTOR ==
				node_info.main_state) ||
				(SINGLE_NODE_TESTS == node_info.main_state)
				) {
			get_current_configuration();
		} else {
			/* Send the confirmation with status as INVALID_CMD
			 * as this command is not allowed in this state */
			usr_get_current_config_confirm(INVALID_CMD,
					&dummy_params);
		}
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
void usr_perf_start_confirm(uint8_t status,
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

	msg_buf = get_next_tx_buffer();

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
	*msg_buf++ = (uint8_t)(trx_config_params->channel >> 8);
	*msg_buf++ = trx_config_params->channel_page;
	*msg_buf++ = trx_config_params->tx_power_dbm;
#if ((TAL_TYPE != AT86RF212) && (TAL_TYPE != AT86RF212B))
	*msg_buf++ = trx_config_params->tx_power_reg;
#else

	/* Tx Power in reg support is not given for RF212 and 212B transceivers
	**/
	*msg_buf++ = FIELD_DOES_NOT_EXIST;
#endif
	*msg_buf++ = (uint8_t)trx_config_params->csma_enabled;
	*msg_buf++ = (uint8_t)trx_config_params->retry_enabled;
	*msg_buf++ = (uint8_t)trx_config_params->ack_request;
#if (TAL_TYPE != AT86RF230B)
	*msg_buf++ = (uint8_t)trx_config_params->rx_desensitize;
#else

	/*Filled with 0xff to indicate this parameter is not available for
	 * this transceiver */
	*msg_buf++ = FIELD_DOES_NOT_EXIST;
#endif

#if ((TAL_TYPE == AT86RF233) || (TAL_TYPE == ATMEGARFR2))
	*msg_buf++ = (uint8_t)trx_config_params->rpc_enable;
#else

	/*Filled with 0xff to indicate this parameter is not available for
	 * this transceiver */
	*msg_buf++ = FIELD_DOES_NOT_EXIST;
#endif

#if (ANTENNA_DIVERSITY == 1)
	*msg_buf++ = trx_config_params->antenna_selected;
#else
	*msg_buf++ = FIELD_DOES_NOT_EXIST;
#endif

	*msg_buf++ = trx_config_params->trx_state;
	*msg_buf++ = (uint8_t)trx_config_params->number_test_frames;
	*msg_buf++ = (uint8_t)(trx_config_params->number_test_frames >> 8);
	*msg_buf++ = (uint8_t)(trx_config_params->number_test_frames >> 16);
	*msg_buf++ = (uint8_t)(trx_config_params->number_test_frames >> 24);
	*msg_buf++ = trx_config_params->phy_frame_length;
	*msg_buf++ = (uint8_t)(trx_config_params->phy_frame_length >> 8);

	/*Peer settings for parameters like CRC and ant diversity */
#if (ANTENNA_DIVERSITY == 1)
	*msg_buf++ = trx_config_params->antenna_selected_on_peer;
#else
	*msg_buf++ = FIELD_DOES_NOT_EXIST;
#endif

#ifdef CRC_SETTING_ON_REMOTE_NODE
	*msg_buf++ = trx_config_params->crc_settings_on_peer;
#else
	*msg_buf++ = FIELD_DOES_NOT_EXIST;
#endif

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
		*msg_buf++ = 0x00; /* Length byte 0 as no string present */
		*ptr_to_msg_size += 1;
		/* SoC MCU Name */
		*msg_buf++ = 0x00; /* Length byte 0 as no string present */
		*ptr_to_msg_size += 1;
		/* Transceiver name */
		*msg_buf++ = 0x00; /* Length byte 0 as no string present */
		*ptr_to_msg_size += 1;
		/* Board name */
		*msg_buf++ = 0x00; /* Length byte 0 as no string present */
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
void usr_per_test_start_confirm(uint8_t status)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();

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
void usr_range_test_beacon_tx(uint8_t *frame)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return;
	}

	/* Copy Len, Protocol Id, Msg Id parameters */
	*msg_buf++ = PROTOCOL_ID_LEN + RANGE_TEST_PKT_LEN;
	*msg_buf++ = PROTOCOL_ID;
	*msg_buf++ = RANGE_TEST_BEACON;

	/* Copy OTA payload */
	for (uint8_t i = 0; i < (RANGE_TEST_PKT_LEN - 1); i++) {
		*msg_buf++ = *frame++;
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
void usr_range_test_beacon_rsp(uint8_t *mpdu, uint8_t lqi_h, int8_t ed_h,
		uint8_t lqi_r, int8_t ed_r)
{
	uint8_t *msg_buf;
	/* First byte of mpdu is the frame length */
	uint8_t phy_frame_len = *mpdu;

	msg_buf = get_next_tx_buffer();

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return;
	}

	/* Copy Len, Protocol Id, Msg Id parameters */
	*msg_buf++ = PROTOCOL_ID_LEN + RANGE_TEST_RSP_PKT_LEN +
			(phy_frame_len - FCS_LEN + LENGTH_FIELD_LEN);
	*msg_buf++ = PROTOCOL_ID;
	*msg_buf++ = RANGE_TEST_BEACON_RESPONSE;
	/* send ota frame */
	for (uint8_t i = 0; i < phy_frame_len - 1; i++) {
		*msg_buf++ = *mpdu++;
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
void usr_range_test_marker_ind(uint8_t *mpdu, uint8_t lqi, int8_t ed_value)
{
	uint8_t *msg_buf;
	/* First byte of mpdu is the frame length */
	uint8_t phy_frame_len = *mpdu;

	msg_buf = get_next_tx_buffer();

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return;
	}

	/* Copy Len, Protocol Id, Msg Id parameters */
	*msg_buf++ = PROTOCOL_ID_LEN + RANGE_TEST_MARKER_IND_LEN +
			(phy_frame_len - FCS_LEN + LENGTH_FIELD_LEN);
	*msg_buf++ = PROTOCOL_ID;
	*msg_buf++ = RANGE_TEST_MARKER_INDICATION;
	/* send marker ota frame */
	for (uint8_t i = 0; i < phy_frame_len - 1; i++) {
		*msg_buf++ = *mpdu++;
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
void usr_range_test_start_confirm(uint8_t status)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();

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

/**
 * The Received Remote Reply Command is converted into Serial Data
 * and sent to the Host interface
 */
void convert_ota_serial_frame_tx(uint8_t *buf, uint8_t len)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return;
	}

	memcpy(msg_buf, buf, len + 1);
	*(msg_buf + len + 1) = EOT;
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
void usr_range_test_stop_confirm(uint8_t status)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();

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
 * \param parameter_type    Type of the parameter that has been set
 * \param param_value   Pointer to the value of the parameter that has been set
 * \return void
 */
void usr_perf_set_confirm(uint8_t status, uint8_t parameter_type,
		param_value_t *param_value)
{
	uint8_t *msg_buf;
	uint8_t param_len;

	/* Pointer to size element - the content is written later. */
	uint8_t *ptr_to_msg_size;

	msg_buf = get_next_tx_buffer();

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
	*msg_buf++ = param_len = get_param_length(parameter_type);
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
 * \param parameter_type    Type of the parameter that has been read
 * \param param_value  Pointer to the value of the parameter that has been read
 * \return void
 */
void usr_perf_get_confirm(uint8_t status, uint8_t parameter_type,
		param_value_t *param_value)
{
	uint8_t *msg_buf;
	uint8_t param_len;

	/* Pointer to size element - the content is written later. */
	uint8_t *ptr_to_msg_size;

	msg_buf = get_next_tx_buffer();

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
	*msg_buf++ = param_len = get_param_length(parameter_type);
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
void usr_identify_peer_node_confirm(uint8_t status)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();

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
void usr_cont_pulse_tx_confirm(uint8_t status)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();

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
void usr_cont_wave_tx_confirm(uint8_t status, bool start_stop_param,
		uint8_t tx_mode)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();

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
void usr_register_read_confirm(uint8_t status, uint16_t reg_addr,
		uint8_t reg_val)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();

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
void usr_register_write_confirm(uint8_t status, uint16_t reg_addr,
		uint8_t reg_val)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();

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
void usr_register_dump_confirm(uint8_t status, uint16_t start_reg_addr,
		uint16_t end_reg_addr, uint8_t *reg_val)
{
	uint8_t index;
	uint8_t *msg_buf;

	/* Pointer to size element - the content is written later. */
	uint8_t *ptr_to_msg_size;

	msg_buf = get_next_tx_buffer();

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
void usr_per_test_end_indication(uint8_t status,
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

	msg_buf = get_next_tx_buffer();

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
void usr_ed_scan_start_confirm(uint8_t status, uint8_t scan_time_min,
		float scan_time_sec)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();

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
 * Function to generate Packet stream confirm frame that must be
 * sent to host application via serial interface.
 * Called by Performance application as Indication before starting/stopping
 * the packet stream.
 * \param status       Confirmation to the packet stream request
 * \param start_stop   Parameter to indicate whether Packet streaming
 *                     is started or stopped
 * \return void
 */
void usr_pkt_stream_confirm(uint8_t status, bool start_stop)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return;
	}

	/* Copy Len, Protocol Id, Msg Id parameters */
	*msg_buf++ = PROTOCOL_ID_LEN + PKT_STREAM_CONFIRM_LEN;
	*msg_buf++ = PROTOCOL_ID;
	*msg_buf++ = PKT_STREAM_CONFIRM;

	/* Copy confirmation payload */
	*msg_buf++ = status;
	*msg_buf++ = start_stop;

	*msg_buf = EOT;
}

/*
 * Function to generate RX_ON confirm frame that must be
 * sent to host application via serial interface.
 * Called by Performance application as Indication before
 * starting/stopping the rx_on mode
 * \param status       Confirmation to the rx_on_req request
 * \param start_stop   Parameter to indicate whether rx_on test
 *                     is started or stopped
 * \return void
 */
void usr_rx_on_confirm(uint8_t status, bool start_stop)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return;
	}

	/* Copy Len, Protocol Id, Msg Id parameters */
	*msg_buf++ = PROTOCOL_ID_LEN + RX_ON_CONFIRM_LEN;
	*msg_buf++ = PROTOCOL_ID;
	*msg_buf++ = RX_ON_CONFIRM;

	/* Copy confirmation payload */
	*msg_buf++ = status;
	*msg_buf++ = start_stop;

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
void usr_ed_scan_end_indication(uint8_t no_of_channels,
		ed_scan_result_t *ed_scan_result )
{
	uint8_t *msg_buf;
	uint8_t ch_cnt;

	/* Pointer to size element - the content is written later */
	uint8_t *ptr_to_msg_size;

	msg_buf = get_next_tx_buffer();

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
void usr_sensor_data_get_confirm(uint8_t status, float bat_voltage,
		float temperature)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();

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
void usr_peer_disconnect_confirm(uint8_t status)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();

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
void usr_set_default_config_confirm(uint8_t status,
		trx_config_params_t *default_config_params)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();

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
	*msg_buf++ = (uint8_t)default_config_params->channel;
	*msg_buf++ = (uint8_t)(default_config_params->channel >> 8);
	*msg_buf++ = default_config_params->channel_page;
	*msg_buf++ = default_config_params->tx_power_dbm;
#if ((TAL_TYPE != AT86RF212) && (TAL_TYPE != AT86RF212B))
	*msg_buf++ = default_config_params->tx_power_reg;
#else
	*msg_buf++ = FIELD_DOES_NOT_EXIST; /* Tx Power in reg support is given
	                                    * for RF212 and 212B transceivers */
#endif
	*msg_buf++ = (uint8_t)default_config_params->csma_enabled;
	*msg_buf++ = (uint8_t)default_config_params->retry_enabled;
	*msg_buf++ = (uint8_t)default_config_params->ack_request;
#if (TAL_TYPE != AT86RF230B)
	*msg_buf++ = (uint8_t)default_config_params->rx_desensitize;
#else

	/*Filled with 0xff to indicate this parameter is not available for
	 * this  transceiver */
	*msg_buf++ = FIELD_DOES_NOT_EXIST;
#endif

#if ((TAL_TYPE == AT86RF233) || (TAL_TYPE == ATMEGARFR2))
	*msg_buf++ = (uint8_t)default_config_params->rpc_enable;
#else

	/*Filled with 0xff to indicate this parameter is not available for
	 * this  transceiver */
	*msg_buf++ = FIELD_DOES_NOT_EXIST;
#endif

#if (ANTENNA_DIVERSITY == 1)
	*msg_buf++ = default_config_params->antenna_selected;
#else
	*msg_buf++ = FIELD_DOES_NOT_EXIST;
#endif

	*msg_buf++ = default_config_params->trx_state;
	*msg_buf++ = (uint8_t)default_config_params->number_test_frames;
	*msg_buf++
		= (uint8_t)(default_config_params->number_test_frames >>
			8);
	*msg_buf++
		= (uint8_t)(default_config_params->number_test_frames >>
			16);
	*msg_buf++
		= (uint8_t)(default_config_params->number_test_frames >>
			24);
	*msg_buf++ = default_config_params->phy_frame_length;
	*msg_buf++ = (uint8_t)(default_config_params->phy_frame_length >> 8);

	/*Peer settings for parameters like CRC and ant diversity */
#if (ANTENNA_DIVERSITY == 1)
	*msg_buf++ = default_config_params->antenna_selected_on_peer;
#else
	*msg_buf++ = FIELD_DOES_NOT_EXIST;
#endif
#ifdef CRC_SETTING_ON_REMOTE_NODE
	*msg_buf++ = default_config_params->crc_settings_on_peer;
#else
	*msg_buf++ = FIELD_DOES_NOT_EXIST;
#endif
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
void usr_identify_board_confirm(uint8_t status,
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

	msg_buf = get_next_tx_buffer();

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
void usr_get_current_config_confirm(uint8_t status,
		trx_config_params_t *curr_trx_conf_params)
{
	uint8_t *msg_buf;

	msg_buf = get_next_tx_buffer();

	/* Check if buffer could not be allocated */
	if (NULL == msg_buf) {
		return;
	}

	/* Copy Len, Protocol Id, Msg Id parameters */
	*msg_buf++ = PROTOCOL_ID_LEN + GET_CURRENT_CONFIG_CONFIRM_LEN;
	*msg_buf++ = PROTOCOL_ID;
	*msg_buf++ = GET_CURRENT_CONFIG_CONFIRM;

	/* Copy confirmation payload */
	*msg_buf++ = status;
	/* configuration parameters */
	*msg_buf++ = (uint8_t)curr_trx_conf_params->channel;
	*msg_buf++ = (uint8_t)(curr_trx_conf_params->channel >> 8);
	*msg_buf++ = curr_trx_conf_params->channel_page;
	*msg_buf++ = curr_trx_conf_params->tx_power_dbm;
#if ((TAL_TYPE != AT86RF212) && (TAL_TYPE != AT86RF212B))
	*msg_buf++ = curr_trx_conf_params->tx_power_reg;
#else
	/* Tx Power in reg support is given for RF212 and 212B transceivers */
	*msg_buf++ = FIELD_DOES_NOT_EXIST;
#endif
	*msg_buf++ = (uint8_t)curr_trx_conf_params->csma_enabled;
	*msg_buf++ = (uint8_t)curr_trx_conf_params->retry_enabled;
	*msg_buf++ = (uint8_t)curr_trx_conf_params->ack_request;
#if (TAL_TYPE != AT86RF230B)
	*msg_buf++ = (uint8_t)curr_trx_conf_params->rx_desensitize;
#else

	/*Filled with 0xff to indicate this parameter is not available for
	 * this  transceiver */
	*msg_buf++ = FIELD_DOES_NOT_EXIST;
#endif

#if ((TAL_TYPE == AT86RF233) || (TAL_TYPE == ATMEGARFR2))
	*msg_buf++ = (uint8_t)curr_trx_conf_params->rpc_enable;
#else

	/*Filled with 0xff to indicate this parameter is not available for
	 * this  transceiver */
	*msg_buf++ = FIELD_DOES_NOT_EXIST;
#endif

#if (ANTENNA_DIVERSITY == 1)
	*msg_buf++ = curr_trx_conf_params->antenna_selected;
#else
	*msg_buf++ = FIELD_DOES_NOT_EXIST;
#endif

	*msg_buf++ = curr_trx_conf_params->trx_state;
	*msg_buf++ = (uint8_t)curr_trx_conf_params->number_test_frames;
	*msg_buf++ = (uint8_t)(curr_trx_conf_params->number_test_frames >> 8);
	*msg_buf++
		= (uint8_t)(curr_trx_conf_params->number_test_frames >> 16);
	*msg_buf++
		= (uint8_t)(curr_trx_conf_params->number_test_frames >> 24);
	*msg_buf++ = curr_trx_conf_params->phy_frame_length;
	*msg_buf++ = (uint8_t)(curr_trx_conf_params->phy_frame_length >> 8);

	/*peer node settings need to be added */
	/*Peer settings for parameters like CRC and ant diversity */
#if (ANTENNA_DIVERSITY == 1)
	*msg_buf++ = curr_trx_conf_params->antenna_selected_on_peer;
#else
	*msg_buf++ = FIELD_DOES_NOT_EXIST;
#endif
#ifdef CRC_SETTING_ON_REMOTE_NODE
	*msg_buf++ = curr_trx_conf_params->crc_settings_on_peer;
#else
	*msg_buf++ = FIELD_DOES_NOT_EXIST;
#endif

#if (TAL_TYPE == AT86RF233)
	memcpy(msg_buf, &curr_trx_conf_params->ism_frequency, sizeof(float));
#else
	float temp = 0.0;
	memcpy(msg_buf, &temp, sizeof(float));
#endif

	msg_buf += sizeof(float);
	*msg_buf = EOT;
}

/* EOF */
