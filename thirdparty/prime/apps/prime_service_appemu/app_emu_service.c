/**
 * \file
 *
 * \brief Metering Application Emulator for Service Node
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
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
 *
 *
 */

/* System includes */
#include <stdio.h>
#include <string.h>

/* Application includes */
#include "app_emu_common.h"
#include "conf_app_emu.h"

#include "asf.h"

#define TIMETO_RECONNECT                        500L /* Time in 10 miliseconds **/
#define TIME_OUT_CONNECTION                     1000L /* Time in 10 miliseconds **/
#define TIME_OUT_TEST                           100000L /* Time in 10 miliseconds */

#define MLME_MACEUI48                           0x8100
#define MLME_MACSTATE                           0x0024

#define MACSTATE_DISCONNECTED                   0
#define MACSTATE_TERMINAL                       1
#define MACSTATE_SWITCH                         2

#define NUM_TEST                                11

/*  States of appemu_proccess */
enum {
	APPEMU_GET_SERIAL,
	APPEMU_WAIT_REGISTER,
	APPEMU_CONNECT_REQUEST,
	APPEMU_WAIT_RESPONSE_CONNECT,
	APPEMU_WAIT_MESSAGE,
	APPEMU_SEND_RESPONSE,
	APPEMU_TIMETO_RECONNECT
};

/*  States of _mlme_get_var */
enum {
	MLMEGET_SEND_REQUEST,
	MLMEGET_WAIT_ANSWER
};

/* Definition of the test */
typedef struct {
	uint8_t uc_test_number; /* Step of the test, to fill up the message **/
	uint16_t us_bytes_sent; /* Bytes sent by the Base Node */
	uint16_t us_bytes_expected; /* Bytes expected in the answer */
} test_info;

/* Test battery to execute */
static test_info conf_test [NUM_TEST + 1] = {
	{2, 24, 36},
	{3, 24, 31},
	{4, 75, 138},
	{4, 16, 52},
	{5, 17, 17},
	{5, 29, 29},
	{5, 65, 65},
	{5, 137, 137},
	{5, 209, 209},
	{5, 281, 281},
	{5, 353, 353},
	/* End of table */
	{0xff, 0, 0}
};

/* State of the machine state, appemu_Proccess */
static uint8_t uc_app_emu_st;
/* Pointer to the new command */
static MacSapCallBack *counter_new_cmd;
/* Temporal storage for the last command extracted */
static MacSapCallBack tmp_new_cmd;
/* us_handler of appEmu Connection */
static uint16_t us_meter_handler;
/* String to store the serial number */
static char pc_serial[SERIAL_SIZE + 1];
/* String to store a representation of the time in Hundreds of Milliseconds */
static char pc_timestamp[TIMESTAMP_SIZE + 1];
/* Var where the payload is built */
static uint8_t pc_request[MAX_COUNTER_DATA_SIZE + 1];
/* Status of the node, DISCONNECTED, CONNECTED */
static uint8_t uc_node_state;
/* State of mlmeGetVar proccess */
static uint8_t uc_mlme_get_var_st;

static uint8_t puc_buf_mlme_get[20];

extern uint32_t ul_time_app_emu;

const char *prime_get_app_identification_str(void);


/** @brief	Gets values through the MLME interface
 *
 *      @param us_var		   Number of the value read
 *      @param puc_result	 Pointer where the value of us_var is stored
 *
 *      @return	1	if OK
 *                                0	In other case
 **/

static int _mlme_get_var(uint16_t us_var, uint8_t *puc_result)
{
	uint16_t pibAttrib;
	int l_result = 0;

	switch (uc_mlme_get_var_st) {
	case MLMEGET_SEND_REQUEST:
		if (prime_MLME_GET_request(us_var) == 0) {
			uc_mlme_get_var_st = MLMEGET_WAIT_ANSWER;
		}

		break;

	case MLMEGET_WAIT_ANSWER:
		if (prime_MLME_callback(puc_result)) {
			/* Check cmd and result */
			if ((puc_result[0] == PRIME_MLME_GET_CONFIRM) && (puc_result[1] == PRIME_MLME_RESULT_SUCCESS)) {
				pibAttrib = (uint16_t)(puc_result[2] << 8) + (uint16_t)puc_result[3];
				if (pibAttrib == us_var) {
					l_result = 1;
				}
			}

			/* Next time we start again in MLMEGET_SEND_REQUEST or retry now */
			uc_mlme_get_var_st = MLMEGET_SEND_REQUEST;
		}

		break;
	}
	return l_result;
}

/** @brief	This function updates the Serial string
 *
 *      @param pc_sn    Pointer to the serial number
 *
 *      @return	1	if OK
 *                                0	In other case
 * This function updates the Serial string with the Serial of the Meter
 **/

static int _get_serial(char *pc_sn)
{
	uint8_t *p_get_array;

	int l_result = 0;

	if (_mlme_get_var(MLME_MACEUI48, puc_buf_mlme_get)) {
		p_get_array = &puc_buf_mlme_get[4]; /* address of buf[0] */
		app_emu_init_random(&p_get_array[0]);
		sprintf(pc_sn, "%s%02x%02x%02x", PROVIDER,
				(uint16_t)p_get_array[3],
				(uint16_t)p_get_array[4],
				(uint16_t)p_get_array[5]);
		l_result = 1;
#ifdef APPEMU_DEBUG
		printf("Serial: %s\n", pc_sn);
#endif
	}

	return l_result;
}


/** @brief	Return the last value of uc_node_state updated by
 * _service_node_registered
 *
 *      @return	uc_node_state
 **/

static int _node_connected(void)
{
	return uc_node_state;
}


/** @brief	It tells us when a node is already connected to a base node
 *
 *      @return	1	Node Connected
 *                                0 Non contected, pooling
 *
 * This function also updates uc_node_state
 **/

static int _service_node_registered(void)
{
	int l_result = 0;

	if (_mlme_get_var(MLME_MACSTATE, puc_buf_mlme_get)) {
		uc_node_state = puc_buf_mlme_get[7]; /* LSB of pibValue */
		if ((uc_node_state == MACSTATE_TERMINAL) || (uc_node_state  == MACSTATE_SWITCH)) {
			l_result = 1;
#ifdef APPEMU_DEBUG
			printf("Node Connected\n");
#endif
		}
	}

	return l_result;
}


/** @brief	Checks the received message to correspond with the programmed
 * test
 *
 *      @param p_cmd	    Pointer to the incomming message
 *      @param pus_rx_len	Pointer to return the length received
 *      @param puc_num_test Pointer to return the test number
 *
 *      @return	length to send
 **/

static uint16_t _get_len_answer_msg(MacSapCallBack *p_cmd, uint16_t *pus_rx_len,
		uint8_t *puc_num_test)
{
	uint16_t us_len_rcv, us_test;
	uint16_t us_len_tx = 0; /* Wrong message */

	uint16_t i = 0;

	us_len_rcv = p_cmd->bufLength;

	us_test = p_cmd->buf[5] - '0';

	if (us_test < 5) {
		for (i = 0; (conf_test[i].uc_test_number != 0xff); i++) {
			if ((conf_test[i].uc_test_number == us_test) && (conf_test[i].us_bytes_sent == us_len_rcv)) {
				us_len_tx = conf_test[i].us_bytes_expected;
				break;
			}
		}
	} else {
		us_len_tx = us_len_rcv;
	}

	*puc_num_test = us_test;
	*pus_rx_len = us_len_rcv;
	return us_len_tx;
}

/** @brief	Get App identification string
 *
 *      @return		Application identification
 *
 * Return string with:
 *      - Product name
 *      - Version
 *      - Copyright advice
 **/

const char *prime_get_app_identification_str(void)
{
	return (APP_ID " Rev." APP_VERSION_STR);
}


/** @brief	Start		application emulation
 *
 *      @param	bauds		Speed of transmission
 *
 * This function initializes variables for application
 **/

void app_emu_start(void)
{
#ifdef APPEMU_DEBUG
	printf("\n\n-- Service Node --\n\n");
	printf("%s\n", prime_get_app_identification_str());
#endif
	uc_app_emu_st = APPEMU_GET_SERIAL;
	uc_node_state = MACSTATE_DISCONNECTED;
	uc_mlme_get_var_st = MLMEGET_SEND_REQUEST;
}


/** @brief	This function implements the Meter
 *
 * This function implements the Meter for the simulated Counter application
 * as a CS sublayer of the Service Node.
 **************************************************************************/

void app_emu_process(void)
{
	uint8_t uc_num_test;

	static uint16_t us_len_rcv;
	static uint16_t us_len_tx;

	Bool new_command = false;

	/* Reception of Mac primitives */
	if (_node_connected() && prime_MAC_callback(&tmp_new_cmd, us_meter_handler, CON_TYPE)) {
		counter_new_cmd = &tmp_new_cmd;
		new_command = true;
		switch (counter_new_cmd->command) {
		case PRIME_MACSAP_RELEASE_INDICATION:
			prime_MAC_RELEASE_response(us_meter_handler, PRIME_MACSAP_RESULT_ACCEPT);
			uc_app_emu_st = APPEMU_TIMETO_RECONNECT;
			ul_time_app_emu = TIMETO_RECONNECT + (uint32_t)app_emu_random();
#ifdef APPEMU_DEBUG
#ifdef __GNUC__
			printf("Connection closed with Concentrator. Reconnecting in %lu milliseconds\n",
					ul_time_app_emu);
#endif
#ifdef __ICCARM__
			printf("Connection closed with Concentrator. Reconnecting in %u milliseconds\n",
					ul_time_app_emu);
#endif
#endif
			break;

		/* Ensure that connection is closed (If connection is already
		 * closed, no packet will be sent to network) */
		case PRIME_MACSAP_RELEASE_CONFIRM:
			uc_app_emu_st = APPEMU_WAIT_REGISTER;
#ifdef APPEMU_DEBUG
			printf("Release Confirm\n");
#endif
			break;

		default:
			break;
		}
	}

	switch (uc_app_emu_st) {
	case APPEMU_GET_SERIAL:
		if (_get_serial(pc_serial)) {
			uc_app_emu_st = APPEMU_WAIT_REGISTER;
		}

		break;

	case APPEMU_WAIT_REGISTER:
		if (_service_node_registered()) {
			uc_app_emu_st = APPEMU_CONNECT_REQUEST;
			ul_time_app_emu = TIME_OUT_CONNECTION;
		}

		break;

	case APPEMU_CONNECT_REQUEST:
#ifdef APPEMU_DEBUG
		printf("Serial: %s\n", pc_serial);
#endif
		prime_MAC_ESTABLISH_request(NULL, CON_TYPE, (unsigned char *)pc_serial, SERIAL_SIZE, 0, 0);
		uc_app_emu_st = APPEMU_WAIT_RESPONSE_CONNECT;
		ul_time_app_emu = TIME_OUT_CONNECTION;
		break;

	case APPEMU_WAIT_RESPONSE_CONNECT:
		if (new_command) {
			new_command = false;
			if (counter_new_cmd->command ==	PRIME_MACSAP_ESTABLISH_CONFIRM) {
				if (counter_new_cmd->answer == PRIME_MACSAP_RESULT_SUCCESS) {
					us_meter_handler = counter_new_cmd->handler;
#ifdef APPEMU_DEBUG
					printf("Connection opened with Concentrator (handler: %i)\n",
							us_meter_handler);
#endif
					counter_new_cmd->buf[counter_new_cmd->bufLength] = '\0';
#ifdef APPEMU_DEBUG
					printf("Connection data : %s\n", &counter_new_cmd->buf[0]);
#endif
					/* Start Test */
					uc_app_emu_st = APPEMU_WAIT_MESSAGE;
				} else {
					uc_app_emu_st = APPEMU_TIMETO_RECONNECT;
#ifdef APPEMU_DEBUG
#ifdef __GNUC__
					printf("Connection rejected. Reconnecting in %lu milliseconds\n",
							ul_time_app_emu);
#endif
#ifdef __ICCARM__
					printf("Connection rejected. Reconnecting in %u milliseconds\n",
							ul_time_app_emu);
#endif
#endif
				}

				ul_time_app_emu = TIME_OUT_TEST + (uint32_t)app_emu_random();
			}
		}

		break;

	/* It will be always in this state waiting to receive message until */
	/* a PRIME_MACSAP_RELEASE_INDICATION is received */
	case APPEMU_WAIT_MESSAGE:
		if (new_command) {
			new_command = false;
			if (counter_new_cmd->command == PRIME_MACSAP_DATA_INDICATION) {
				us_len_tx = _get_len_answer_msg(counter_new_cmd, &us_len_rcv, &uc_num_test);

				if (us_len_tx) {
#ifdef APPEMU_DEBUG
					printf("Received %i bytes\t Test:%i\t\n", us_len_rcv, (uint16_t)uc_num_test);
#endif
					app_emu_copy_timestamp(pc_timestamp, counter_new_cmd->buf);
					app_emu_fill_string((char *)pc_request, us_len_tx, UP_MSG, uc_num_test, pc_timestamp);
					uc_app_emu_st = APPEMU_SEND_RESPONSE;
				} else {
#ifdef APPEMU_DEBUG
					printf("Wrong Message received\n");
#endif
				}
			}
		}

		break;

	case APPEMU_SEND_RESPONSE:
		prime_MAC_DATA_request(us_meter_handler, pc_request, us_len_tx, 1);
#ifdef APPEMU_DEBUG
		printf("Sent:%i\n", us_len_tx);
#endif
		uc_app_emu_st = APPEMU_WAIT_MESSAGE;
		break;

	case APPEMU_TIMETO_RECONNECT:
		break;
	}

	if ((uc_app_emu_st != APPEMU_GET_SERIAL) && (!ul_time_app_emu)) {
		/* Ensure that connection is closed (If connection is already
		 * closed, no packet will be sent to network) */
		prime_MAC_RELEASE_request(us_meter_handler);
		uc_app_emu_st = APPEMU_WAIT_REGISTER;
		ul_time_app_emu = TIMETO_RECONNECT + (uint32_t)app_emu_random();
#ifdef APPEMU_DEBUG
#ifdef __GNUC__
		printf("Connection timeout. Reconnecting in %lu milliseconds\n",
				ul_time_app_emu);
#endif
#ifdef __ICCARM__
		printf("Connection timeout. Reconnecting in %u milliseconds\n",
				ul_time_app_emu);
#endif
#endif
	}
}
