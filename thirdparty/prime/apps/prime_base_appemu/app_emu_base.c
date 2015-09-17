/**
 * \file
 *
 * \brief Metering Application Emulator for Base Node
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

/* Prime includes */
#include "asf.h"

/* Application includes */
#include "app_emu_common.h"
#include "conf_app_emu.h"

extern uint32_t ul_tens_of_ms;

/* *** Declarations ************************************************************
 **/
/* Tasks priorities */
#define TASK_APPEMU_LAYER_PRIO             (tskIDLE_PRIORITY + 1)

/* Stack definitions */
#define TASK_APPEMU_LAYER_STACK            (configMINIMAL_STACK_SIZE * 4)

/* Main Task Period */
#define APPEMU_TIMER_RATE                  (5 / portTICK_RATE_MS)

/* Update Timer Period */
#define UPDATE_APPEMU_TIMER_RATE           (1 / portTICK_RATE_MS)

#define MAX_TBL_NODE_NUM                   64

#define IDCHAR_BASE                        '5'
#define IDCHAR_SERVICE                     '6'
#define IDCHAR_CON_BASE                    '7'
#define IDCHAR_CON_SERVICE                 '8'

/* Modulation scheme of the payload: Differential BPSK */
#define PROTOCOL_DBPSK                     0x00
/* Modulation scheme of the payload: Differential QPSK */
#define PROTOCOL_DQPSK                     0x01
/* Modulation scheme of the payload: Differential 8PSK */
#define PROTOCOL_D8PSK                     0x02

/* Modulation scheme of the payload: Differential BPSK with Convolutional Coding */
#define PROTOCOL_DBPSK_VTB                 0x04

/* Modulation scheme of the payload: Differential QPSK with Convolutional Coding */
#define PROTOCOL_DQPSK_VTB                 0x05

/* Modulation scheme of the payload: Differential 8PSK with Convolutional Coding */
#define PROTOCOL_D8PSK_VTB                 0x06

/* Time from the last connection detected to start test */
#define TIMETO_START_TEST          500L /* 75 seconds in 10ms. */
#define TIMETO_REG_CHEK            100  /* 1 second in 10ms. */

/* Value of the Register Devices list */
#define MLME_LIST_REG_DEVICES    0x50
/* Values of communications */
#define MLME_LIST_PHY_COMM       0x57
/* Maximum test defined */
#define NUM_TEST                 10  /* Tecnalia 20130502 */
/* Retries for a test */
#define NUM_RETRIES              3

#define VTB(A)                   (A & 0x04)
#define MOD(A)                   (A & 0x03)

#define _getTimeMs()             (ul_tens_of_ms * 10)

/* State of machine state _get_registered_nodes */
enum {
	MLME_SEND_REQUEST,
	MLME_WAIT_ANSWER
};

/* State of appemu_Process */
enum {
	APPEMU_WAIT_NEW_CONNECTIONS,
	APPEMU_GET_NODES_REGISTERED,
	APPEMU_WAITTO_START_TEST,

	/* All test states must be listed under APPEMU_START_TEST (to reject
	 * connections during test) */
	APPEMU_START_TEST,
	APPEMU_WAIT_RECEIVED_TXCODING,
	APPEMU_BUILD_MESSAGE_TEST,
	APPEMU_SEND_MESSAGE_TEST,
	APPEMU_WAIT_CONFIRM_MESSAGE_TEST,
	APPEMU_WAIT_RECEIVE_MESSAGE_TEST,
	APPEMU_RETRY_MESSAGE_TEST,
	APPEMU_END_TEST_NODE,
	APPEMU_DISCONNECT_NODES
};

/* *** Static Variables ********************************************************
 **/

/* Definition of the stored fields required by appEmu */
typedef struct {
	uint16_t us_handler;                           /* Handler of the connection */
	uint8_t puc_mac_addr[PRIME_MAC_LENGTH];        /* Mac address of the connected node */
	uint32_t ul_nid;                               /* NID of the node */
	uint8_t puc_serial[SERIAL_SIZE + 1];           /* String with the serial number */
	uint32_t ul_start_time_test;                   /* Ini of the test in Hundred of miliseconds, since base node is started */
	uint32_t pul_round_trip[NUM_TEST];             /* Time from a petition is done until the answer is received */
	uint8_t puc_success_attempt[NUM_TEST];         /* Number of successfully attempt */
	uint8_t uc_rx_coding;                          /* Reception scheme */
	uint8_t uc_tx_coding;                          /* Transmission scheme */
	uint8_t uc_app_emu_coding;                     /* Transmission mode */
} test_node;

/* Definition of the test */
typedef    struct {
	uint8_t uc_test_number;         /* Step of the test, to fill up the message */
	uint16_t us_bytes_sent;         /* Bytes sent by the Base Node */
	uint16_t us_bytes_expected;     /* Bytes expected in the answer */
} test_info;

typedef struct {
	uint8_t uc_n_attempts;
	uint32_t ul_round_trip_time;
} type_result;

typedef struct {
	uint32_t ul_nid;
	uint8_t puc_serial[SERIAL_SIZE + 1];
	uint32_t ul_start_time;
	type_result step_result[N_LENGTHS_TEST];
} type_report;

typedef struct {
	uint8_t puc_serial[SERIAL_SIZE + 1];
} type_meter_serial;

/* Test battery to execute */
static test_info p_conf_test [NUM_TEST + 1] = {
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
	/* End of table */
	{0xff, 0, 0}
};

/* Array with the test node */
test_node p_node[MAX_TBL_NODE_NUM];
/* State of the proccess _get_registered_nodes */
static uint8_t uc_get_nodes_st;
/* State of the proccess _info_tx_coding */
static uint8_t uc_info_tx_coding_st;
/* State of the proccess appemu_Process */
static uint8_t uc_app_emu_st;
/* Pointer to the new command */
static MacSapCallBack *p_counter_new_cmd;
/* Temporal storage for the last command extracted */
static MacSapCallBack tmp_new_cmd;
/* Number of connections opened */
static uint16_t us__num_connections;
/* Number of nodes register in the base node */
static uint16_t us__num_nodes_registered;
/* Device under test */
static uint16_t us__dut;
/* Number of test en execution */
static uint16_t us__numTest;
/* Number of attempt for the current test */
static uint8_t uc__num_attempt;
/* String where the time is stored */
static char pc_time_stamp[TIMESTAMP_SIZE + 1];
/* Var where the payload is built */
static uint8_t puc_request[MAX_COUNTER_DATA_SIZE + 1];
/* Boolean to get registered nodes */
static Bool b_check_reg;

extern uint32_t ul_time_app_emu;
extern uint32_t ul_mili_seconds;

static uint8_t _most_robust_scheme_mode(uint8_t uc_sch1, uint8_t uc_sch2);

void appemu_init(void);

/* ************************************************************************** */

/** @brief    give us the time in hundreds of Miliseconds, take into acount
*   if the counter has been reset
*   @param    ul_end_time
*   @param    ul_ini_time
*
*   @return    Real value in tenths of seconds
*
**************************************************************************/

static uint32_t _get_round_trip(uint32_t ul_end_time, uint32_t ul_ini_time)
{
	uint32_t ul_result;

	if (ul_end_time < ul_ini_time) {
		ul_result = ul_end_time + (0xffffffff - ul_ini_time);
	} else {
		ul_result = ul_end_time - ul_ini_time;
	}

	return ul_result;
}

/* ************************************************************************** */

/** @brief    This function updates the var us__num_nodes_registered
 * calling the MLME interface
 **************************************************************************/
static void _update_nid(MlmeRegDevice *p_dev)
{
	uint8_t i;

	for (i = 0; i < us__num_connections; i++) {
		if (memcmp(&p_node[i].puc_mac_addr[0], &p_dev->mac[0], PRIME_MAC_LENGTH) == 0) {
			/* Update NID */
			p_node[i].ul_nid = ((uint32_t)p_dev->sid & 0x000000ff) << 14;
			p_node[i].ul_nid |= ((uint32_t)p_dev->lnid & 0x00003fff);
		}
	}
}

/* ************************************************************************** */

/** @brief    This function updates the var uc_tx_coding and uc_rx_coding
 * from structure Nodes
 *    @param pr_comm  pointer to MlmePhyCommDevice
 **************************************************************************/
static void _update_tx_rx_coding(MlmePhyCommDevice *pr_comm)
{
	uint8_t i;

	for (i = 0; i < us__num_connections; i++) {
		if (memcmp(&p_node[i].puc_mac_addr[0], &pr_comm->eui48[0], PRIME_MAC_LENGTH) == 0) {
			/* Update uc_tx_coding */
			p_node[i].uc_tx_coding = pr_comm->txCodingMod;
			p_node[i].uc_rx_coding = pr_comm->rxCodingMod;
		}
	}
}

/* ************************************************************************** */

/** @brief    This function updates the var uc_tx_coding and uc_rx_coding
 * from structure Nodes
 **************************************************************************/
static void _fill_tx_rx_coding_undirect_nodes(void)
{
	uint8_t i;

	for (i = 0; i < us__num_connections; i++) {
		if (p_node[i].uc_rx_coding == 0) {
			/* Update uc_tx_coding */
			p_node[i].uc_tx_coding = PROTOCOL_DBPSK_VTB;
			p_node[i].uc_rx_coding = PROTOCOL_DBPSK_VTB;
		}
	}
}

/* ************************************************************************** */

/** @brief    it choose the most robust mode to transmit to node
 **************************************************************************/
static void _choose_app_emu_coding(void)
{
	uint8_t i;

	for (i = 0; i < us__num_connections; i++) {
		p_node[i].uc_app_emu_coding	= _most_robust_scheme_mode(p_node[i].uc_rx_coding,
															   p_node[i].uc_tx_coding);
#ifdef APPEMU_DEBUG
		printf("%s\tModeAppEmu:%hu\n", p_node[i].puc_serial,
				(uint16_t)p_node[i].uc_app_emu_coding);
#endif
	}
}

/* ************************************************************************** */

/** @brief    Prints the shcemes for all the nodes, ONLY in debug mode
 **************************************************************************/
#ifdef _CS_DEBUG_
static void _print_all_schemes(void)
{
	uint8_t i;

	for (i = 0; i < us__num_connections; i++) {
#ifdef APPEMU_DEBUG
		printf("%s\trxCoding:%u\ttxCoding:%u\n", p_node[i].puc_serial,
				(uint16_t)p_node[i].uc_rx_coding,
				(uint16_t)p_node[i].uc_tx_coding);
#endif
	}
}

#endif

/* ************************************************************************** */

/** @brief    This function updates the var us__num_nodes_registered
 *
 *  @return    1    when it finishes
 *             0    In other case
 *
 * This function updates the us__num_nodes_registered  calling the MLME
 * interface
 **************************************************************************/

static int _get_registered_nodes(void)
{
	MlmeListGetRegDevices list;
	int i;
	int result = 0;

	switch (uc_get_nodes_st) {
	case MLME_SEND_REQUEST:
#ifdef APPEMU_DEBUG
		printf("Getting MLME_LIST_REG_DEVICES\n");
#endif
		if (prime_MLME_LIST_GET_request(MLME_LIST_REG_DEVICES) == 0) {
			us__num_nodes_registered = 0;
			uc_get_nodes_st = MLME_WAIT_ANSWER;
		}

		break;

	case MLME_WAIT_ANSWER:
		if (prime_MLME_callback((uint8_t *)&list)) {
			/* Check that is a valid answer */
			if ((list.cmd == PRIME_MLME_LIST_GET_CONFIRM) &&
					(list.pibAttrib == MLME_LIST_REG_DEVICES)) {
				for (i = 0; i < list.numItems; i++) {
					/* Updates the NID for the connected nodes */
					_update_nid(&list.dev[i]);
				}
				us__num_nodes_registered += list.numItems;
			}

			/* Next time we start againg in MLMEGET_SEND_REQUEST or
			 * retry now */
			if (list.isLast) {
#ifdef APPEMU_DEBUG
				printf("End list get MLME_LIST_REG_DEVICES\n");
#endif
				uc_get_nodes_st = MLME_SEND_REQUEST;
				if (list.result == PRIME_MLME_RESULT_SUCCESS) {
					result = 1;
				}
			}
		}

		break;
	}
	return result;
}

/* ************************************************************************** */

/** @brief    This function updates the var us__num_nodes_registered
 *
 *  @return    1    when it finishes
 *                0    In other case
 *
 * This function updates the us__num_nodes_registered  calling the MLME
 * interface
 **************************************************************************/

static int _info_tx_coding(void)
{
	MlmeListGetPhyCommTable list;
	int i;

	int result = 0;

	switch (uc_info_tx_coding_st) {
	case MLME_SEND_REQUEST:
#ifdef APPEMU_DEBUG
		printf("Getting MLME_LIST_PHY_COMM\n");
#endif
		if (prime_MLME_LIST_GET_request(MLME_LIST_PHY_COMM) == 0) {
			us__num_nodes_registered = 0;
			uc_info_tx_coding_st = MLME_WAIT_ANSWER;
		}

		break;

	case MLME_WAIT_ANSWER:
		if (prime_MLME_callback((uint8_t *)&list)) {
			/* Check that is a valid answer */
			if ((list.cmd == PRIME_MLME_LIST_GET_CONFIRM) &&
					(list.pibAttrib == MLME_LIST_PHY_COMM)) {
				for (i = 0; i < list.numItems; i++) {
					/* Updates the NID for the connected nodes */
					_update_tx_rx_coding(&list.dev[i]);
				}
				us__num_nodes_registered += list.numItems;
			}

			/* Next time we start againg in MLMEGET_SEND_REQUEST or
			 * retry now */
			if (list.isLast) {
#ifdef APPEMU_DEBUG
				printf("End list get MLME_LIST_PHY_COMM\n");
#endif
				uc_info_tx_coding_st = MLME_SEND_REQUEST;
				/* Fill up the rest of nodes */
				_fill_tx_rx_coding_undirect_nodes(); 
				_choose_app_emu_coding();
#ifdef _CS_DEBUG_
				_print_all_schemes();
#endif
				result = 1;
			}
		}

		break;
	}
	return result;
}

/* ************************************************************************** */

/** @brief    It decides if a new incomming connection has to be aceppted or
 * rejected
 *
 *    @param p_cmd     pointer to the incomming message
 *
 * It is based in two criteria, first, if the node has already an open
 * connection
 * Second if the test has already started. In these two cases the connection is
 * rejected. It will be aceppted in other case
 **************************************************************************/

static void _check_new_connections(MacSapCallBack *p_cmd)
{
	uint16_t i;

	for (i = 0; i < us__num_connections; i++) {
		if (memcmp(&p_node[i].puc_mac_addr[0], &p_cmd->macAddr[0], PRIME_MAC_LENGTH) == 0) {
			break;
		}
	}

	/* There was a connection previously open or we are in the middle of
	 * test */
	if (uc_app_emu_st >= APPEMU_START_TEST) {
		if (i < us__num_connections) {
			p_node[i].us_handler = CON_HANDLER_INIT_VALUE;
			p_node[i].puc_serial[0] = '\0';
			p_node[i].ul_start_time_test = 0;
		}

#ifdef APPEMU_DEBUG
		printf("Rejecting connection with node %hu\n", i + 1);
#endif
		prime_MAC_ESTABLISH_response(p_cmd->handler, PRIME_MACSAP_RESULT_REJECT, NULL, 0);
	} else {
#ifdef APPEMU_DEBUG
		printf("Accepting connection with node %hu\n", i + 1);
#endif
		prime_MAC_ESTABLISH_response(p_cmd->handler, PRIME_MACSAP_RESULT_ACCEPT, NULL, 0);
		p_node[i].us_handler = p_cmd->handler;
		memcpy(&p_node[i].puc_serial[0], &p_cmd->buf[0], p_cmd->bufLength);
		memcpy(&p_node[i].puc_mac_addr[0], &p_cmd->macAddr[0], PRIME_MAC_LENGTH);
		/* Only increment connections if it is a new connection */
		if (i == us__num_connections) {
			us__num_connections++;
		}
	}
}

/* ************************************************************************** */

/** @brief    Sends the results of the tests in the node dut
 *
 *  @param us_dut Device under Test
 **************************************************************************/
static void _send_result_node(uint16_t us_dut)
{
	uint8_t i;
	static uint8_t msg[1024];

#ifdef __GNUC__
	sprintf((char *)msg, "%08lx,%s,%lu \n",
			p_node[us_dut].ul_nid,
			p_node[us_dut].puc_serial,
			p_node[us_dut].ul_start_time_test);
#endif
#ifdef __ICCARM__
	sprintf((char *)msg, "%08x,%s,%u \n",
			p_node[us_dut].ul_nid,
			p_node[us_dut].puc_serial,
			p_node[us_dut].ul_start_time_test);
#endif

	for (i = 0; i < NUM_TEST; i++) {
#ifdef __GNUC__
		sprintf((char *)&msg[ strlen((char *)msg)], ",%hu,%lu \n",
				(uint16_t)p_node[us_dut].puc_success_attempt[i],
				 p_node[us_dut].pul_round_trip[i]);
#endif
#ifdef __ICCARM__
		sprintf((char *)&msg[ strlen((char *)msg)], ",%hu,%u \n",
				(uint16_t)p_node[us_dut].puc_success_attempt[i],
				 p_node[us_dut].pul_round_trip[i]);
#endif
	}
	sprintf((char *)&msg[strlen((char *)msg)], "%c%c \n", 10, 13);

	printf("%s", msg);
}

/* ************************************************************************** */

/** @brief    Most robust Scheme module between the two given
 *
 *  @param uc_sch1     modulation scheme 1
 *  @param uc_sch2     modulation scheme 2
 *
 *  @return Most robust Scheme module between the two given
 **************************************************************************/
static uint8_t _most_robust_scheme_mode(uint8_t uc_sch1, uint8_t uc_sch2)
{
	uint8_t mostRobustMode;

	if (VTB(uc_sch1) != VTB(uc_sch2)) {
		if (VTB(uc_sch1)) {
			mostRobustMode = uc_sch1;
		} else {
			mostRobustMode = uc_sch2;
		}
	} else {
		if (uc_sch1 < uc_sch2) {
			mostRobustMode = uc_sch1;
		} else {
			mostRobustMode = uc_sch2;
		}
	}

	return mostRobustMode;
}

/* ************************************************************************** */

/** @brief    Get length of the transmission
 *
 *  @param us_dut
 *  @param pus_num_test
 *  @param pus_len_rx
 *
 * @return transmission length
 *
 **************************************************************************/
static uint16_t _get_length_transmission(uint16_t us_dut,
		uint16_t *pus_num_test, uint16_t *pus_len_rx)
{
	uint16_t us_len_tx;

	if (p_conf_test[*pus_num_test].uc_test_number < 5) {
		us_len_tx = p_conf_test[*pus_num_test].us_bytes_sent;
		*pus_len_rx = p_conf_test[*pus_num_test].us_bytes_expected;
	} else { /* Test Number == 5, adapt transmission and reception */
		if ((MOD(p_node[us_dut].uc_app_emu_coding) == PROTOCOL_D8PSK) &&
				(p_conf_test[*pus_num_test].us_bytes_expected == 17)) {
			*pus_num_test = *pus_num_test + 1;
		}

		if (VTB(p_node[us_dut].uc_app_emu_coding)) {
			us_len_tx = p_conf_test[*pus_num_test].us_bytes_sent;
		} else {
			us_len_tx = (p_conf_test[*pus_num_test].us_bytes_sent + 1);
		}

		*pus_len_rx = us_len_tx;
	}

	return us_len_tx;
}

/* ************************************************************************** */

/** @brief    End test with current node
 *
 * Check if dut is the last one and then change state machine to disconnect
 * all nodes
 **************************************************************************/
static void _end_dut(void)
{
	if (++us__dut >= us__num_connections) {
#ifdef APPEMU_DEBUG
		printf("Disconnecting %i nodes\n", us__num_connections);
#endif
		us__dut = 0;
		uc_app_emu_st = APPEMU_DISCONNECT_NODES;
	} else {
		uc_app_emu_st = APPEMU_START_TEST;
	}
}

/* *** Public Functions ******************************************************
 **/
/* ************************************************************************** */

/** @brief    Init application emulation
 *
 * This function initializes the application
 **************************************************************************/

void appemu_init(void)
{
	uint8_t mac[6];

	mac[0] = 0x00;
	mac[1] = 0x01;
	mac[2] = 0x02;
	mac[3] = 0x03;
	mac[4] = 0x04;
	mac[5] = 0x05;
	app_emu_init_random(mac);
}

/* ************************************************************************** */

/** @brief    Start application emulation
 *
 * This function initializes variables for application
 **************************************************************************/

void app_emu_start()
{
	uint16_t i;

	/* Init appemu Vars */
	uc_get_nodes_st = MLME_SEND_REQUEST;
	uc_info_tx_coding_st = MLME_SEND_REQUEST;
	uc_app_emu_st = APPEMU_WAIT_NEW_CONNECTIONS;
	us__num_connections = 0;
	us__num_nodes_registered = 0;
	b_check_reg = false;

	for (i = 0; i < MAX_TBL_NODE_NUM; i++) {
		p_node[i].us_handler = CON_HANDLER_INIT_VALUE;
		p_node[i].puc_mac_addr[0] = '\0';
		p_node[i].ul_nid = 0;
		p_node[i].puc_serial[0] = '\0';
		p_node[i].ul_start_time_test = 0;
		memset(p_node[i].pul_round_trip, 0, NUM_TEST);
		memset(p_node[i].puc_success_attempt, 0, NUM_TEST);
		p_node[i].uc_rx_coding = PROTOCOL_DBPSK_VTB;
		p_node[i].uc_tx_coding = PROTOCOL_DBPSK_VTB;
		p_node[i].uc_app_emu_coding = PROTOCOL_DBPSK_VTB;
	}
}

/* ************************************************************************** */

/** @brief    app Emu main proccess
 *
 *
 **************************************************************************/
void app_emu_process()
{
	Bool b_new_command = false;

	static uint16_t us_length_transmission;
	static uint16_t us_length_reception;

	/* Reception of Mac primitives */
	if (prime_MAC_callback(&tmp_new_cmd, GENERIC_CALLBACK_HANDLER, GENERIC_CALLBACK_TYPE)) {
		p_counter_new_cmd = &tmp_new_cmd;
		b_new_command = true;
		/* New connection checking if there is new nodw or not */
		switch (p_counter_new_cmd->command) {
		case PRIME_MACSAP_ESTABLISH_INDICATION:
#ifdef APPEMU_DEBUG
			printf("<- ESTABLISH_indication()\n");
#endif
			_check_new_connections(p_counter_new_cmd);
			b_check_reg = true;
			b_new_command = false;
			break;

		case PRIME_MACSAP_RELEASE_INDICATION:
			if (p_counter_new_cmd->answer == PRIME_MACSAP_RESULT_ERROR) {
#ifdef APPEMU_DEBUG
				printf( "APPEMU abnormally aborted with node %hu\n", us__dut);
#endif
				_end_dut();
			} else {
				prime_MAC_RELEASE_response(p_counter_new_cmd->handler, PRIME_MACSAP_RESULT_ACCEPT);
			}

			break;

		case PRIME_MACSAP_RELEASE_CONFIRM:
#ifdef APPEMU_DEBUG
			printf("Release confirm %hu with node %hu\n",
					(uint16_t)p_counter_new_cmd->errorType,
					us__dut);
#endif
			if (p_counter_new_cmd->errorType != PRIME_MACSAP_ERROR_TIMEOUT) {
#ifdef APPEMU_DEBUG
				printf("APPEMU closed with node %hu\n",	us__dut);
#endif
			}

			break;
		}
	}

	if (b_check_reg) {
		if (_get_registered_nodes()) {
			b_check_reg = false;
		}
	}

	switch (uc_app_emu_st) {
	case APPEMU_WAIT_NEW_CONNECTIONS:
		if (us__num_connections) {
			ul_time_app_emu = TIMETO_START_TEST;
			uc_app_emu_st = APPEMU_WAITTO_START_TEST;
		}

		break;

	case APPEMU_WAITTO_START_TEST:
		if (!ul_time_app_emu) {
			us__dut = 0;
			uc_app_emu_st = APPEMU_START_TEST;
		}

		break;

	case APPEMU_START_TEST:
#ifdef APPEMU_DEBUG
		printf("---INIT TEST : %i nodes connected---\n", us__num_connections);
		printf("Getting information for transmission Schemes\n");
#endif
		uc_app_emu_st = APPEMU_WAIT_RECEIVED_TXCODING;
		break;

	case APPEMU_WAIT_RECEIVED_TXCODING:
		if (_info_tx_coding()) {
			us__numTest = 0;
			p_node[us__dut].ul_start_time_test = _getTimeMs();
			uc_app_emu_st = APPEMU_BUILD_MESSAGE_TEST;
		}

		break;

	case APPEMU_BUILD_MESSAGE_TEST:
		uc__num_attempt = 1;
		app_emu_build_timestamp(pc_time_stamp);
		us_length_transmission = _get_length_transmission(us__dut, &us__numTest, &us_length_reception);
		app_emu_fill_string((char *)puc_request, us_length_transmission, DW_MSG,
							p_conf_test[us__numTest].uc_test_number, pc_time_stamp);
		uc_app_emu_st = APPEMU_SEND_MESSAGE_TEST;
		ul_time_app_emu = 0;
		break;

	case APPEMU_SEND_MESSAGE_TEST:
		if (p_node[us__dut].us_handler == CON_HANDLER_INIT_VALUE) {
			_end_dut();
		} else {
#ifdef APPEMU_DEBUG
			printf("-> Sending data to node %hu...\n", us__dut + 1);
#endif
			prime_MAC_DATA_request(p_node[us__dut].us_handler, puc_request, us_length_transmission, 1);
#ifdef APPEMU_DEBUG
			printf("%s: Test:%i\tAttempt:%i\tSent:%i\t\n",
					p_node[us__dut].puc_serial,
					(uint16_t)us__numTest,
					(uint16_t)uc__num_attempt,
					(uint16_t)p_conf_test[us__numTest].us_bytes_sent);
#endif
			p_node[us__dut].pul_round_trip[us__numTest]	= _getTimeMs();
			p_node[us__dut].puc_success_attempt[us__numTest] = uc__num_attempt;
			uc_app_emu_st = APPEMU_WAIT_CONFIRM_MESSAGE_TEST;
			ul_time_app_emu = TIME_REPEAT;
		}

		break;

	case APPEMU_WAIT_CONFIRM_MESSAGE_TEST:
		if (b_new_command) {
			b_new_command = false;
			if (p_counter_new_cmd->command == PRIME_MACSAP_DATA_CONFIRM) {
				if (p_counter_new_cmd->answer == PRIME_MACSAP_RESULT_SUCCESS) {
					uc_app_emu_st = APPEMU_WAIT_RECEIVE_MESSAGE_TEST;
				} else {
					switch (p_counter_new_cmd->errorType) {
					case PRIME_MACSAP_ERROR_TIMEOUT:
#ifdef APPEMU_DEBUG
						printf("Timeout. Retrying page...\n");
#endif
						uc_app_emu_st =	APPEMU_RETRY_MESSAGE_TEST;
						break;

					case PRIME_MACSAP_ERROR_TX_BUSY:
					case PRIME_MACSAP_ERROR_NO_ACKED:
#ifdef APPEMU_DEBUG
						printf("ARQ transmission error %hu. Retrying packet...\n",
								(uint16_t)p_counter_new_cmd->errorType);
#endif
						uc_app_emu_st =	APPEMU_RETRY_MESSAGE_TEST;
						break;

					case PRIME_MACSAP_ERROR_INVALID_HANDLER:
#ifdef APPEMU_DEBUG
						printf("Warning: Invalid Handler %hu %hu\n",
								p_counter_new_cmd->handler, (uint16_t)p_counter_new_cmd->type);
#endif
						if (p_counter_new_cmd->handler == p_node[us__dut].us_handler) {
							_end_dut();
						} else {
							uc_app_emu_st = APPEMU_RETRY_MESSAGE_TEST;
						}

						break;

					case PRIME_MACSAP_ERROR_NOT_REGISTERED:
					default:
#ifdef APPEMU_DEBUG
						printf("ErrorType %hu\n",
								(uint16_t)(p_counter_new_cmd->errorType));
#endif
						_end_dut();
						break;
					}
				}
			}
		}

		if (!ul_time_app_emu) {
			uc_app_emu_st = APPEMU_RETRY_MESSAGE_TEST;
		}

		break;

	case APPEMU_WAIT_RECEIVE_MESSAGE_TEST:
		if (b_new_command) {
			b_new_command = false;
			if ((p_counter_new_cmd->command == PRIME_MACSAP_DATA_INDICATION) &&
				(p_counter_new_cmd->handler == p_node[us__dut].us_handler)) {
				/* It is the expected message */
				if (p_counter_new_cmd->bufLength == us_length_reception) {
					/* Good message, Next Test */
					p_node[us__dut].pul_round_trip[us__numTest]	= _get_round_trip(_getTimeMs(), p_node[us__dut].pul_round_trip[us__numTest]);

					/* check it is not the last test of the node */
#ifdef APPEMU_DEBUG
					printf( "<- Received data from node %hu ... OK\n", us__dut + 1);
					printf("Received:%i\n", p_conf_test[us__numTest].us_bytes_expected);
#endif
					if (p_conf_test[++us__numTest].uc_test_number != 0xFF) {
						uc_app_emu_st = APPEMU_BUILD_MESSAGE_TEST;
					} else {
						uc_app_emu_st = APPEMU_END_TEST_NODE;
					}
				} else {
					/* Bad message, Retry */
					uc_app_emu_st = APPEMU_RETRY_MESSAGE_TEST;
#ifdef APPEMU_DEBUG
					printf( "<- Received data from node %hu ... FAIL\n", us__dut + 1);
#endif
				}
			}
		}

		if (!ul_time_app_emu) {
			uc_app_emu_st = APPEMU_RETRY_MESSAGE_TEST;
		}

		break;

	case APPEMU_RETRY_MESSAGE_TEST:
#ifdef APPEMU_DEBUG
		printf("Received:FAIL\n");
#endif
		if (uc__num_attempt++ < NUM_RETRIES) {
			uc_app_emu_st = APPEMU_SEND_MESSAGE_TEST;
		} else {
			/* Test Fail, next test */
#ifdef APPEMU_DEBUG
			printf("---TEST FAILED---\n");
#endif
			p_node[us__dut].pul_round_trip[us__numTest] = 0;
			p_node[us__dut].puc_success_attempt[us__numTest] = 0;

			/* check it is not the last test of the node */
			if (p_conf_test[++us__numTest].uc_test_number != 0xFF) {
				uc_app_emu_st = APPEMU_BUILD_MESSAGE_TEST;
			} else {
				uc_app_emu_st = APPEMU_END_TEST_NODE;
			}
		}

		break;

	case APPEMU_END_TEST_NODE:
		/* Send through serial line test results */
#ifdef APPEMU_DEBUG
		printf("TEST RESULT [%s]:\n", p_node[us__dut].puc_serial);
#endif
		_send_result_node(us__dut);
		_end_dut();
		break;

	case APPEMU_DISCONNECT_NODES:
#ifdef APPEMU_DEBUG
		printf("Closing connection with node %hu\n", us__dut + 1);
#endif
		prime_MAC_RELEASE_request(p_node[us__dut].us_handler);
		p_node[us__dut].us_handler = CON_HANDLER_INIT_VALUE;
		p_node[us__dut].puc_serial[0] = '\0';
		p_node[us__dut].ul_start_time_test = 0;
		if (++us__dut >= us__num_connections) {
#ifdef APPEMU_DEBUG
			printf("All nodes disconnected: Restart\n");
#endif
			us__num_connections = 0;
			uc_app_emu_st = APPEMU_WAIT_NEW_CONNECTIONS;
		}

		break;
	}
}
