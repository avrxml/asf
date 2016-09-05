/**
 * \file per_mode_initiator.c
 *
 * \brief Initiator/Transmitter functionalities in PER Measurement mode -
 * Performance Analyzer application
 * This is the source code of a Packet Error Rate Measurement mode as Initiator.
 *
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
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* === INCLUDES ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "app_init.h"
#include "tfa.h"
#include "tal_internal.h"
#include "tal_helper.h"
#include "tal_constants.h"
#include "tal_pib.h"
#include "app_frame_format.h"
#include "perf_api.h"
#include "conf_board.h"
#include "perf_api_serial_handler.h"

/**
 * \addtogroup group_per_mode_init
 * \{
 */

/* === TYPES =============================================================== */

/**
 * \brief Structure used for updating the parameters after sending them to peer
 *
 */
typedef struct {
	uint8_t param_type;
	uint8_t param_value;
} set_param_cb_t;

/*=====EXTERNALS============================================================*/

/* === MACROS ============================================================== */

/* === PROTOTYPES ========================================================== */
static void configure_frame_sending(void);
static void send_parameters_changed(uint8_t param, uint8_t val);
static bool send_result_req(void);
static bool send_peer_info_req(void);
static void wait_for_reply_timer_handler_cb(void *parameter);
static bool send_identify_command(void);
static bool send_disconnect_command(void);
static void set_parameter_on_transmitter_node(retval_t status);
static void start_test(void);
static void start_range_test(void);

#ifdef CRC_SETTING_ON_REMOTE_NODE
static bool send_crc_set_req(crc_set_req_t crc_msg);
static bool send_crc_status_req(void);
static void get_crc_settings_peer_node(void);
static void config_crc_peer_node(bool config_value);

#endif /* End of CRC_SETTING_ON_REMOTE_NODE */

#if (ANTENNA_DIVERSITY == 1)
static bool send_diversity_set_req(div_set_req_t div_msg);
static bool send_diversity_status_req(void);
static void get_diversity_settings_peer_node(void);

#endif /* End of #if (ANTENNA_DIVERSITY == 1) */

#ifdef ENABLE_DEEP_SLEEP
static void toggle_trx_sleep(uint8_t sleep_mode);

#else  /* No DEEP_SLEEP */
static void toggle_trx_sleep(void);

#endif /* End of ENABLE_DEEP_SLEEP */

#if ((TAL_TYPE == AT86RF233) || (TAL_TYPE == ATMEGARFR2))
static void config_rpc_mode(bool config_value);

#endif

#if (TAL_TYPE == AT86RF233)
static void config_frequency(float frequency);

#endif /*End of #if (TAL_TYPE == AT86RF233) */

#if ((TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B))
static bool validate_tx_power(int8_t dbm_value);

#endif

static float calculate_time_duration(void);
static float calculate_net_data_rate(float per_test_duration_sec);
static void set_channel(uint16_t channel);
static void set_channel_page(uint8_t channel_page);
static void set_tx_power(uint8_t tx_power_format, int8_t power_value);
static void config_ack_request(bool config_value);
static void config_csma(bool config_value);
static void config_frame_retry(bool config_value);

#if (ANTENNA_DIVERSITY == 1)
static void set_antenna_diversity_settings(uint8_t config_value);
static void config_antenna_diversity_peer_node(uint8_t config_value);

#endif /* #if (ANTENNA_DIVERSITY == 1) */

#if (TAL_TYPE != AT86RF230B)
static void config_rx_desensitization(bool config_value);

#endif /* (TAL_TYPE != AT86RF230B) */

static void set_transceiver_state(uint8_t trx_state);
static void set_phy_frame_length(uint16_t frame_len);
static bool send_set_default_config_command(void);
static bool send_per_test_start_cmd(void);
static bool send_range_test_start_cmd(void);
static bool send_range_test_stop_cmd(void);
static void configure_range_test_frame_sending(void);
static bool send_range_test_marker_rsp(void);

/* === GLOBALS ============================================================= */
static bool scanning = false;
static bool trx_sleep_status = false;
static bool range_test_in_progress = false;

static uint8_t scan_duration;
static uint8_t seq_num_initiator;
static uint8_t channel_before_scan;
uint8_t op_mode = TX_OP_MODE;

/*This flag determines if the pkt stream mode is in progress*/
bool pkt_stream_stop = true;

static uint16_t no_of_roll_overs;
static uint32_t start_time;
static uint32_t end_time;
static uint32_t restart_time;
static uint32_t scan_channel_mask;
static uint32_t frame_no_ack;
static uint32_t frame_access_failure;
static uint32_t frame_failure;
static uint32_t frames_to_transmit;
static set_param_cb_t set_param_cb;
static uint16_t num_channels;

/**
 * This is variable is to keep track of the specific features supported
 */
static uint32_t fw_feature_mask = 0;

#if ((TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B))
static uint8_t phy_tx_power;
#endif

static uint32_t range_test_frame_cnt = 0;

/* Size constants for PERformance configuration parameters */
FLASH_DECLARE(uint8_t perf_config_param_size[]) = {
	sizeof(uint16_t),            /* channel */
	sizeof(uint8_t),            /* channel page */
	sizeof(uint8_t),            /* TX power_reg */
	sizeof(int8_t),             /* TX_power_dBm */
	sizeof(uint8_t),            /* csma */
	sizeof(uint8_t),            /* frame retry */
	sizeof(uint8_t),            /* Ack request */
	sizeof(uint8_t),            /* Antenna diversity */
	sizeof(uint8_t),            /* Antenna diversity on remote node */
	sizeof(uint8_t),            /* desensitization */
	sizeof(uint8_t),            /* Transceiver state */
	sizeof(uint8_t),            /* CRC on remote node */
	sizeof(uint32_t),           /* No. of test frames */
	sizeof(uint16_t),            /* Physical frame length */
	sizeof(uint8_t),            /* RPC */
	sizeof(float),              /* ISM frequency */
};

/* ! \} */

/* === IMPLEMENTATION ====================================================== */

/*
 * \brief Initialize the application in PER Measurement mode as Initiator
 * \param parameter Pointer to the parameter to be carried , if any
 */
void per_mode_initiator_init(void *parameter)
{
#ifdef EXT_RF_FRONT_END_CTRL
	pib_value_t pib_value;
#endif

	/* PER TEST Initiator sequence number */
	do {
		seq_num_initiator = rand();
	} while (!seq_num_initiator);

	/* Node connection status */
	peer_found = node_info.peer_found;

	config_per_test_parameters();

	if (PER_TEST_INITIATOR == node_info.main_state) {
		/* Get Peer device information */
		if (send_peer_info_req()) {
			sw_timer_start(APP_TIMER_TO_TX,
					TIMEOUT_FOR_RESPONSE_IN_MICRO_SEC,
					SW_TIMEOUT_RELATIVE,
					(FUNC_PTR)wait_for_reply_timer_handler_cb,
					NULL);
			op_mode = PEER_INFO_RSP_WAIT;
		}
	} else {
		app_led_event(LED_EVENT_POWER_ON);

		/* Put the transceiver in TRX OFF state- default state for
		 * Single node tests */
		set_trx_state(CMD_TRX_OFF);

		/* Send the confirmation to the PC application via Serial
		 * interface */
		usr_perf_start_confirm(MAC_SUCCESS,
				START_MODE_SINGLE_NODE,
				&default_trx_config_params,
				NUL_VAL,
				NULL,
				NULL,
				NULL,
				NUL_VAL, NUL_VAL, NUL_VAL);
	}

#ifdef EXT_RF_FRONT_END_CTRL
	/* Enable RF front end control in PER Measurement mode*/
	tal_ext_pa_ctrl(PA_EXT_ENABLE);
	/* set the TX power to default level */
	pib_value.pib_value_8bit = TAL_TRANSMIT_POWER_DEFAULT;
	tal_pib_set(phyTransmitPower, &pib_value);
#endif /* End of EXT_RF_FRONT_END_CTRL */

	/* keep the compiler happy */
	parameter = parameter;
}

/*
 * \brief Application task for PER Measurement mode as initiator
 *
 * This function
 * - Implements the PER Measurement mode state machine.
 *
 * - On user inputs through serial app executes various tests
 */
void per_mode_initiator_task(void)
{
	/* If any packets need to be transferred */
	if (frames_to_transmit > 0 ||
			((op_mode == PKT_STREAM_MODE) && (rdy_to_tx))) {
		/* PER measurement test is on as more frames has to
		 * be transmitted. On completion of the test result also has to
		 * be
		 * queried from the remote node */
		if (!node_info.transmitting) {
			node_info.transmitting = true;
			if (node_info.main_state == PER_TEST_INITIATOR) {
				node_info.tx_frame_info->mpdu[PL_POS_SEQ_NUM]++;
			}

			if ((op_mode == PKT_STREAM_MODE)) {
				tal_tx_frame(stream_pkt, NO_CSMA_NO_IFS, false);
				if (pkt_stream_gap_time) {
					sw_timer_start(T_APP_TIMER,
							pkt_stream_gap_time * 1E3, SW_TIMEOUT_RELATIVE,
							(FUNC_PTR)pkt_stream_gap_timer,
							NULL);
					rdy_to_tx = false;
				}
			} else {
				if (curr_trx_config_params.csma_enabled) {
					tal_tx_frame(node_info.tx_frame_info,
							CSMA_UNSLOTTED,
							curr_trx_config_params.retry_enabled );
				} else {
					tal_tx_frame(node_info.tx_frame_info,
							NO_CSMA_NO_IFS,
							curr_trx_config_params.retry_enabled );
				}
			}
		}
	} else {
		switch (op_mode) {
		case CONTINUOUS_TX_MODE:

		/* While CW transmission wait for stop cmd to stop transmitting
		**/
		case SET_PARAMETER:

			/* While setting parameter on remote node wait for call
			 * back
			 *  from the TAL specifying the result
			 */
#if (ANTENNA_DIVERSITY == 1)
		case DIVERSITY_STATUS_REQ:

		/* While querying the diversity settings on remote node wait for
		 * result */
		case DIVERSITY_SET_REQ:

			/* While changing the diversity setting on remote node
			 * wait
			 * for call back from the TAL specifying the result
			 */
#endif /* #if (ANTENNA_DIVERSITY == 1) */
#ifdef CRC_SETTING_ON_REMOTE_NODE
		case CRC_STATUS_REQ_WAIT:

		/* While querying the CRC settings on remote node wait for
		 * result */
		case CRC_SET_REQ_WAIT:

			/* While changing the CRC setting on remote node wait
			 * for
			 * call back from the TAL specifying the result
			 */
#endif /* #ifdef CRC_SETTING_ON_REMOTE_NODE */
		case IDENTIFY_PEER:
		/* Wait for result while identify peer command is sent */
		case DISCONNECT_PEER:
		/* Wait for result while disconnect peer command is sent */
		case WAIT_FOR_TEST_RES:
			/* Wait for PER test result */
			break;

		case TEST_FRAMES_SENT:
		{
			/* Test frames has been sent, now ask for the result */
			if (send_result_req()) {
				sw_timer_start(APP_TIMER_TO_TX,
						TIMEOUT_FOR_RESPONSE_IN_MICRO_SEC,
						SW_TIMEOUT_RELATIVE,
						(FUNC_PTR)wait_for_reply_timer_handler_cb,
						NULL);
				op_mode = WAIT_FOR_TEST_RES;
			} else {
				op_mode = TX_OP_MODE;
			}
		}
		break;

		default:
			break;
		}
	}
}

/** \brief This function is called periodically by the range test
 * timer to initiate the transmission of range test packets to the receptor
 * \param parameter pass parameters to timer handler
 */
static void  range_test_timer_handler_cb(void *parameter)
{
	if (!node_info.transmitting) {
		/* Update the FCF and payload before transmission */
		configure_range_test_frame_sending();

		/* Transmit the Range Test Packet */
		if (curr_trx_config_params.csma_enabled) {
			tal_tx_frame(node_info.tx_frame_info,
					CSMA_UNSLOTTED,
					curr_trx_config_params.retry_enabled );
		} else {
			tal_tx_frame(node_info.tx_frame_info,
					NO_CSMA_NO_IFS,
					curr_trx_config_params.retry_enabled );
		}

		node_info.transmitting = true;

		sw_timer_start(T_APP_TIMER_RANGE,
				RANGE_TX_BEACON_INTERVAL,
				SW_TIMEOUT_RELATIVE,
				(FUNC_PTR)range_test_timer_handler_cb,
				NULL);
	}
}

/**
 * \brief Wait for reply timer handler is called if any command sent on air
 * times out before any response message is received.
 * This function handles all the scenarios if the remote node is not accessible
 * \param parameter pass parameters to timer handler
 */
static void wait_for_reply_timer_handler_cb(void *parameter)
{
	param_value_t param_value;
	switch (op_mode) {
	case WAIT_FOR_TEST_RES:
	{
		/* Send the PER test Indication as failure as it is not able
		 * to contact peer node after completion of the test
		 */
		usr_per_test_end_indication(UNABLE_TO_CONTACT_PEER,
				NUL_VAL, NUL_VAL, NUL_VAL, NUL_VAL,
				NUL_VAL, NUL_VAL, NUL_VAL, NUL_VAL,
				NUL_VAL, NUL_VAL);
		break;
	}

	case REMOTE_TEST_MODE:
	{
		uint8_t *sio_data;
		sio_data = (uint8_t *)parameter;
		remote_serial_tx_failure = true;
		convert_ota_serial_frame_rx(sio_data, *sio_data);
		break;
	}

#if (ANTENNA_DIVERSITY == 1)
	case DIVERSITY_STATUS_REQ:
	{
		uint8_t ant_div_settings = NUL_VAL;

		/* Send Get confirmation with status UNABLE_TO_CONTACT_PEER */
		usr_perf_get_confirm(UNABLE_TO_CONTACT_PEER,
				PARAM_ANT_DIVERSITY_ON_PEER,
				(param_value_t *)&ant_div_settings);
		break;
	}

#endif /* #if (ANTENNA_DIVERSITY == 1) */
#ifdef CRC_SETTING_ON_REMOTE_NODE
	case CRC_STATUS_REQ_WAIT:
	{
		bool crc_settings = false;
		/* Send Get confirmation with status UNABLE_TO_CONTACT_PEER */
		param_value.param_value_bool = crc_settings;
		usr_perf_get_confirm(UNABLE_TO_CONTACT_PEER,
				PARAM_CRC_ON_PEER,
				&param_value);
		break;
	}

#endif /* #ifdef CRC_SETTING_ON_REMOTE_NODE */
	case PEER_INFO_RSP_WAIT:
	{
		/* Send the confirmation with status as SUCCESS */
		usr_perf_start_confirm(NO_PEER_FOUND,
				START_MODE_PER,
				&default_trx_config_params,
				NUL_VAL,
				NULL,
				NULL,
				NULL,
				NUL_VAL, NUL_VAL, NUL_VAL );
	}
		/* As the peer did not responds with its details */
		app_reset();
		break;

	default:
		break;
	}
	/* notifies remote node has problems */
	op_mode = TX_OP_MODE;
	parameter = parameter;
}

/*
 * \brief Callback that is called once tx is done in the
 * PER_FOUND_PER_INITIATOR state .
 *
 * \param status    Status of the transmission procedure
 * \param frame     Pointer to the transmitted frame structure
 */
void per_mode_initiator_tx_done_cb(retval_t status, frame_info_t *frame)
{
	static uint8_t tx_count;
	param_value_t param_value;
	switch (op_mode) {
	case SET_PARAMETER:
	{
		/* After successful transmission, set the params on Initiator
		 * node */
		set_parameter_on_transmitter_node(status);
		op_mode = TX_OP_MODE;
	}
	break;

	case REMOTE_TEST_MODE:
	{
		if (status != MAC_SUCCESS) {
			app_payload_t *msg;
			uint8_t serial_data_len;

			/* Point to the message : 1 =>size is first byte and
			 * 2=>FCS*/
			msg
				= (app_payload_t *)(frame->mpdu +
					LENGTH_FIELD_LEN +
					FRAME_OVERHEAD - FCS_LEN);
			serial_data_len
				= *(frame->mpdu + LENGTH_FIELD_LEN +
					FRAME_OVERHEAD + CMD_ID_LEN +
					SEQ_NUM_LEN - FCS_LEN);
			remote_serial_tx_failure = true;
			sw_timer_stop(CW_TX_TIMER);
			convert_ota_serial_frame_rx(
					msg->payload.remote_test_req_data.remote_serial_data,
					serial_data_len);
		}

		op_mode = TX_OP_MODE;
		break;
	}

	case WAIT_FOR_TEST_RES:
	{
		/* If no ack received from remote for the send_result_req sent
		**/
		if (MAC_SUCCESS != status) {
			/* if PER test result request fails it is enunciated to
			 * the user and waits for inputs from user
			 */
			sw_timer_stop(APP_TIMER_TO_TX);

			/* Send the PER test Indication as failure as it is not
			 * able
			 * to contact peer node after completion of the test
			 */
			usr_per_test_end_indication(UNABLE_TO_CONTACT_PEER,
					NUL_VAL,
					NUL_VAL, NUL_VAL, NUL_VAL, NUL_VAL,
					NUL_VAL, NUL_VAL, NUL_VAL, NUL_VAL,
					NUL_VAL);

			op_mode = TX_OP_MODE;
		}

		break;
	}

#if (ANTENNA_DIVERSITY == 1)
	case DIVERSITY_SET_REQ:
	{
		app_payload_t *msg;

		op_mode = TX_OP_MODE;

		/* Point to the message : 1 =>size is first byte and 2=>FCS*/
		msg
			= (app_payload_t *)(frame->mpdu + LENGTH_FIELD_LEN +
				FRAME_OVERHEAD - FCS_LEN);

		div_set_req_t *ant_div_set_req
			= (div_set_req_t *)&(msg->payload.
				div_set_req_data);

		if (MAC_SUCCESS == status) {
			/* Update the database with this change  */
			curr_trx_config_params.antenna_diversity_on_peer
				= ant_div_set_req->status;
			curr_trx_config_params.antenna_selected_on_peer
				= ant_div_set_req->ant_sel;
			/* Send Confirmation with status SUCCESS */
			usr_perf_set_confirm(status,
					PARAM_ANT_DIVERSITY_ON_PEER,
					(param_value_t *)&curr_trx_config_params.antenna_selected_on_peer);
		} else {
			/* if Antenna diversity set request fails it is
			 * enunciated
			 * to the user and waits for inputs from user
			 */
			sw_timer_stop(APP_TIMER_TO_TX);

			/* Send Get confirmation with status
			 * UNABLE_TO_CONTACT_PEER */
			usr_perf_set_confirm(UNABLE_TO_CONTACT_PEER,
					PARAM_ANT_DIVERSITY_ON_PEER,
					(param_value_t *)&curr_trx_config_params.antenna_selected_on_peer);
		}

		break;
	}

	case DIVERSITY_STATUS_REQ:
		/* Do nothing */
		break;

#endif /* #if (ANTENNA_DIVERSITY == 1) */
#ifdef CRC_SETTING_ON_REMOTE_NODE
	case CRC_SET_REQ_WAIT:
	{
		app_payload_t *msg;
		op_mode = TX_OP_MODE;

		/* Point to the message : 1 =>size is first byte and 2=>FCS*/
		msg
			= (app_payload_t *)(frame->mpdu + LENGTH_FIELD_LEN +
				FRAME_OVERHEAD - FCS_LEN);

		crc_set_req_t *crc_set_req
			= (crc_set_req_t *)&(msg->payload.crc_set_req_data);

		if (MAC_SUCCESS == status) {
			/* Update the database with this change  */
			curr_trx_config_params.crc_settings_on_peer
				= crc_set_req->status;
			/* Send Confirmation with status SUCCESS */
			param_value.param_value_bool
				= curr_trx_config_params.crc_settings_on_peer;
			usr_perf_set_confirm(status, PARAM_CRC_ON_PEER,
					&param_value);
		} else {
			/* if set request fails it is enunciated to
			 * the user and waits for inputs from user
			 */
			sw_timer_stop(APP_TIMER_TO_TX);

			/* Send Get confirmation with status
			 * UNABLE_TO_CONTACT_PEER */
			usr_perf_set_confirm(UNABLE_TO_CONTACT_PEER,
					PARAM_CRC_ON_PEER,
					&param_value);
		}

		break;
	}

	case CRC_STATUS_REQ_WAIT:
		/* Do nothing */
		break;

#endif /* #ifdef CRC_SETTING_ON_REMOTE_NODE */
	case IDENTIFY_PEER:
	{
		op_mode = TX_OP_MODE;
		if (MAC_SUCCESS == status) {
			usr_identify_peer_node_confirm(MAC_SUCCESS);
		} else {
			/* if Identify peer request fails it is enunciated to
			 * the user and waits for inputs from user
			 */
			usr_identify_peer_node_confirm(UNABLE_TO_CONTACT_PEER);
		}

		break;
	}

	case DISCONNECT_PEER:
	{
		op_mode = TX_OP_MODE;
		if (MAC_SUCCESS == status) {
			usr_peer_disconnect_confirm(MAC_SUCCESS);

			/* app reset on source node as the disconnect peer is
			 * successful */

			/* This is to make the node to restart as boot up and
			 * open for fresh peer search
			 */
			app_reset();
		} else {
			/* if Disconnect peer request fails it is enunciated to
			 * the user and waits for inputs from user
			 */
			usr_peer_disconnect_confirm(UNABLE_TO_CONTACT_PEER);
		}

		break;
	}

	case SET_DEFAULT_CONFIG_PEER:
	{
		op_mode = TX_OP_MODE;
		if (MAC_SUCCESS == status) {
			/* Set the default values for all configurable
			 * parameters */
			config_per_test_parameters();

			/* Send the confirmation with the status as SUCCESS */
			usr_set_default_config_confirm(MAC_SUCCESS,
					&default_trx_config_params);
		} else { /* Failure */
			 /* Send the confirmation with the status as
			  * UNABLE_TO_CONTACT_PEER */
			usr_set_default_config_confirm(UNABLE_TO_CONTACT_PEER,
					&default_trx_config_params);
		}

		break;
	}

	case PER_TEST_START:
	{
		op_mode = TX_OP_MODE;

		/* As start indication is successful start the actual
		 * PER Test*/
		start_test();
	}
	break;

	case RANGE_TEST_START:
	{
		if (MAC_SUCCESS == status) {
			/* As start indication is successful start the actual
			 * RANGE Test in PER Mode*/
			start_range_test();
		} else {
			op_mode = TX_OP_MODE;
			usr_range_test_start_confirm(UNABLE_TO_CONTACT_PEER);
		}
	}
	break;

	case RANGE_TEST_STOP:
	{
		/* Stop the Range Test Timer */
		sw_timer_stop(T_APP_TIMER_RANGE);
		/* Set the falg to default */
		range_test_in_progress = false;
		/* reset the frame sount */
		range_test_frame_cnt = 0;
		/* Send Stop Confirmation to Host */
		usr_range_test_stop_confirm(MAC_SUCCESS);
		/* Reset the OPMODE */
		op_mode = TX_OP_MODE;
		app_led_event(LED_EVENT_PEER_SEARCH_DONE);
	}
	break;

	case RANGE_TEST_TX:
	{
		app_payload_t *msg;
		/* Point to the message : 1 =>size is first byte and 2=>FCS*/
		msg
			= (app_payload_t *)(frame->mpdu + LENGTH_FIELD_LEN +
				FRAME_OVERHEAD - FCS_LEN);
		/* Check whether the tx frame was a range test packet*/
		if (msg->cmd_id == RANGE_TEST_PKT) {
			app_led_event(LED_EVENT_TX_FRAME);

			/* Send the transmitted OTA frame to Host UI for disply
			**/
			usr_range_test_beacon_tx(node_info.tx_frame_info->mpdu);
		}
	}
	break;

	case TX_OP_MODE:
	{
		if (MAC_SUCCESS != status) {
			if (status == MAC_NO_ACK) {
				frame_no_ack++;
			} else if (status == MAC_CHANNEL_ACCESS_FAILURE) {
				frame_access_failure++;
			} else {
				frame_failure++;
			}
		}

		/* PER test is ON after sending each frame the number of
		 * frames to be sent is decremented
		 */

		/* Added to handle the the peer search case abort case */
		if (frames_to_transmit > 0) {
			frames_to_transmit--;
		}

		/* Count when the roll-over happens */
		end_time = sw_timer_get_time();
		end_time = (uint32_t)(end_time * MICRO_SEC_MULTIPLIER);

		if (end_time == (restart_time - 1)) {
			no_of_roll_overs += 1;
			restart_time--;
		}

		/* Led is toggled indicating the test in progress at the count
		 * of
		 * LED_TOGGLE_COUNT_FOR_PER
		 */
		if (tx_count >= LED_TOGGLE_COUNT_FOR_PER) {
			/* Indicate a TX */
			app_led_event(LED_EVENT_TX_FRAME);
			tx_count = 0;
		} else {
			tx_count++;
		}

		/* If no frames to be transmitted */
		if (frames_to_transmit == 0) {
			end_time = sw_timer_get_time();
			op_mode = TEST_FRAMES_SENT;
			tx_count = 0;
			app_led_event(LED_EVENT_PEER_SEARCH_DONE);

			/* For Boards with two leds LED0 is used as both status
			 * and TX LED,hence the timer may switch off TX_LED
			 * after per test
			 * hence this timer should be stopped */
			sw_timer_stop(APP_TIMER_TO_TX_LED_OFF);
		}

		break;
	}

	case PKT_STREAM_MODE:
	{
		if (pkt_stream_stop) {
			op_mode = TX_OP_MODE;
		}
	}
	break;

	case TEST_FRAMES_SENT:
	case CONTINUOUS_TX_MODE:
		/* Do nothing */
		break;

	default:
		break;
	} /* End of  switch */

	/* After transmission is completed, allow next transmission. */
	node_info.transmitting = false;

	/* Keep compiler happy. */
	frame = frame;
}

/**
 * \brief Set the parameters like channel,tx power etc on transmitter node
 * \param status Transmission Status of the node.
 */
static void set_parameter_on_transmitter_node(retval_t status)
{
	param_value_t param_value;
	pib_value_t pib_value;

	/* set the parameter on this node */
	if (MAC_SUCCESS != status) {
		param_value.param_value_8bit = set_param_cb.param_value;
		usr_perf_set_confirm(UNABLE_TO_CONTACT_PEER,
				set_param_cb.param_type,
				&param_value);
		return;
	}

	switch (set_param_cb.param_type) {
	case CHANNEL:
	{
		/* Set back the Tx power to default value when
		 * the channel changed from 26 to other channel
		 */
#ifdef EXT_RF_FRONT_END_CTRL
		uint16_t chn_before_set;
		tal_pib_get(phyCurrentChannel, (uint8_t *)&chn_before_set);
#endif

#if ((TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B))
		int8_t dbm_val = 0;
		uint8_t tx_pwr = 0;
#endif

#if (TAL_TYPE == AT86RF233)
		/* Set the CC_BAND register value to 0 */
		tal_set_frequency_regs(CC_BAND_0, CC_NUMBER_0);

		/* As the transceiver set to work in compliant IEEE channels,
		 * set
		 * the ISM frequency as INVALID
		 */
		curr_trx_config_params.ism_frequency = 0.0;
#endif /* End of #if(TAL_TYPE == AT86RF233) */
		pib_value.pib_value_8bit = set_param_cb.param_value;
		tal_pib_set(phyCurrentChannel,
				&pib_value);

		/* update the data base with this value */
		curr_trx_config_params.channel = set_param_cb.param_value;
#if ((TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B))
		tal_pib_get(phyTransmitPower, &tx_pwr);
		dbm_val = CONV_phyTransmitPower_TO_DBM(tx_pwr);
		curr_trx_config_params.tx_power_dbm = dbm_val;
#endif

#ifdef EXT_RF_FRONT_END_CTRL

		/* Limit the tx power below the default power for ch26 to meet
		 * FCC Compliance
		 */
		limit_tx_power_in_ch26(set_param_cb.param_value,
				(uint8_t)chn_before_set);
#endif /* End of EXT_RF_FRONT_END_CTRL */

		/* Send the confirmation for Set request as Success */
		param_value.param_value_16bit = set_param_cb.param_value;
		usr_perf_set_confirm(MAC_SUCCESS,
				PARAM_CHANNEL,
				&param_value);
		break;
	}

#if (TAL_TYPE == AT86RF233)
	/* Handle changing of ISM frequencies in Band 0x08 */
	case FREQ_BAND_08:
	{
		float frequency;
		/* Set the Transceiver ISM frequency */
		tal_set_frequency_regs(CC_BAND_8, set_param_cb.param_value);
		tal_calculate_frequency(CC_BAND_8, set_param_cb.param_value,
				&frequency);

		/* Update the data base with this value */
		curr_trx_config_params.ism_frequency = frequency;

		/* As the transceiver is set to work in the non compliant
		 * ISM frequencies set the channel as INVALID
		 */
		curr_trx_config_params.channel = INVALID_VALUE;

		/* Send the confirmation for Set request with the status as
		 * MAC_SUCCESS */
		param_value.param_value_float = frequency;
		usr_perf_set_confirm(MAC_SUCCESS,
				PARAM_ISM_FREQUENCY,
				&param_value);
	}
	break;

	/* Handle changing of ISM frequencies in Band 0x09 */
	case FREQ_BAND_09:
	{
		float frequency;
		/* Set the Transceiver ISM frequency */
		tal_set_frequency_regs(CC_BAND_9, set_param_cb.param_value);
		tal_calculate_frequency(CC_BAND_9, set_param_cb.param_value,
				&frequency);

		curr_trx_config_params.ism_frequency = frequency;

		/* As the transceiver is set to work in the non compliant ISM
		 * frequencies set
		 * the channel as INVALID
		 */
		curr_trx_config_params.channel = INVALID_VALUE;

		/* Send the confirmation for Set request with the status as
		 * MAC_SUCCESS */
		param_value.param_value_float = frequency;
		usr_perf_set_confirm(MAC_SUCCESS,
				PARAM_ISM_FREQUENCY,
				&param_value);
	}
	break;

#endif /* End of #if(TAL_TYPE == AT86RF233) */
	/* Handle changing of channel page */
	case CHANNEL_PAGE:
	{
#if ((TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B))
		uint16_t channel;
		int8_t dbm_val = 0;
		uint8_t tx_pwr = 0;
#endif
		pib_value.pib_value_8bit = set_param_cb.param_value;
		tal_pib_set(phyCurrentPage,
				&pib_value);

		/* update the data base with this value */
		curr_trx_config_params.channel_page = set_param_cb.param_value;
#if ((TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B))
		tal_pib_get(phyCurrentChannel, (uint8_t *)&channel);
		curr_trx_config_params.channel = (uint8_t)channel;

		tal_pib_get(phyTransmitPower, &tx_pwr);
		dbm_val = CONV_phyTransmitPower_TO_DBM(tx_pwr);
		curr_trx_config_params.tx_power_dbm = dbm_val;
#endif

		/*Send the confirmation with status as SUCCESS */
		param_value.param_value_8bit = set_param_cb.param_value;
		usr_perf_set_confirm(MAC_SUCCESS,
				PARAM_CHANNEL_PAGE,
				&param_value);
		break;
	}

	default:
		break;
	}
}

/*
 * \brief Callback that is called if data has been received by trx
 * in the PER_FOUND_PER_INITIATOR state
 *
 * \param frame_info  Frame Pointer to received frame
 */
void per_mode_initiator_rx_cb(frame_info_t *mac_frame_info)
{
	static uint8_t range_test_seq_num, remote_cmd_seq_num;
	app_payload_t *msg;
	param_value_t param_value;

	if (rx_on_mode) {
		/* do not handle requests in continuous rx on mode */
		return;
	}

	/* Point to the message : 1 =>size is first byte and 2=>FCS*/
	msg = (app_payload_t *)(mac_frame_info->mpdu +
			LENGTH_FIELD_LEN + FRAME_OVERHEAD - FCS_LEN);

	switch ((msg->cmd_id)) {
	/* The below case handles reply/confirmation messages received from
	* the remote node for the Remote Test Requests sent from the host*/
	case REMOTE_TEST_REPLY_CMD:
	{
		if (remote_cmd_seq_num == msg->seq_num) {
			return;
		}

		/*The CW_TX_TIMER is stopped in case it was started for the
		 * RX_ON Req mode */
		if (sw_timer_is_running(CW_TX_TIMER)) {
			sw_timer_stop(CW_TX_TIMER);
		}

		remote_cmd_seq_num = msg->seq_num;
		uint8_t serial_data_len;
		serial_data_len
			= *(mac_frame_info->mpdu + LENGTH_FIELD_LEN +
				FRAME_OVERHEAD + CMD_ID_LEN +
				SEQ_NUM_LEN - FCS_LEN);

		/* The received over-the-air payload is converted into serial
		 * message
		 * sent to the PC HOST */
		convert_ota_serial_frame_tx(
				msg->payload.remote_test_req_data.remote_serial_data,
				serial_data_len);
	}

	case RESULT_RSP:
	{
		if (op_mode == WAIT_FOR_TEST_RES) {
			if (*(mac_frame_info->mpdu) == (FRAME_OVERHEAD +
					((sizeof(app_payload_t) -
					sizeof(general_pkt_t)) +
					sizeof(result_rsp_t)))) {
				uint32_t aver_lqi;
				uint32_t aver_rssi;
				uint32_t number_rx_frames;
				float per_test_duration_sec;
				float net_data_rate;

				uint32_t frames_with_wrong_crc;

				int8_t rssi_val = tal_get_rssi_base_val();
				sw_timer_stop(APP_TIMER_TO_TX);
				number_rx_frames
					= (msg->payload.test_result_rsp_data
						.num_of_frames_rx);
				aver_lqi = CCPU_ENDIAN_TO_LE32(
						msg->payload.test_result_rsp_data.lqi_avrg_rx);
				aver_rssi = CCPU_ENDIAN_TO_LE32(
						msg->payload.test_result_rsp_data.rssi_avrg_rx);
				rssi_val += aver_rssi;

#ifdef CRC_SETTING_ON_REMOTE_NODE
				frames_with_wrong_crc
					= (msg->payload.test_result_rsp_data
						.frames_with_wrong_crc);

				/* Value of 0xffffffff means that CRC errors
				 * were not counted */

#else
				frames_with_wrong_crc = 0XFFFFFFFF;
#endif /* #ifdef CRC_SETTING_ON_REMOTE_NODE */
				/* get the per test duration */
				per_test_duration_sec
					= calculate_time_duration();
				/* Calculate the net data rate */
				net_data_rate
					= (calculate_net_data_rate(
						per_test_duration_sec));

				net_data_rate = reverse_float(net_data_rate);
				per_test_duration_sec = reverse_float(
						per_test_duration_sec);
				usr_per_test_end_indication(MAC_SUCCESS,
						rssi_val,
						aver_lqi,
						CCPU_ENDIAN_TO_LE32(
						curr_trx_config_params.
						number_test_frames),
						number_rx_frames,
						CCPU_ENDIAN_TO_LE32(
						frame_failure),
						CCPU_ENDIAN_TO_LE32(
						frame_no_ack),
						CCPU_ENDIAN_TO_LE32(
						frame_access_failure),
						frames_with_wrong_crc,
						per_test_duration_sec,
						net_data_rate
						);
				op_mode = TX_OP_MODE;
			}
		}

		break;
	}

#if (ANTENNA_DIVERSITY == 1)
	case DIV_STAT_RSP:
	{
		uint8_t ant_div_settings;
		if (op_mode == DIVERSITY_STATUS_REQ) {
			if (*(mac_frame_info->mpdu) == (FRAME_OVERHEAD +
					((sizeof(app_payload_t) -
					sizeof(general_pkt_t)) +
					sizeof(div_stat_rsp_t)))) {
				sw_timer_stop(APP_TIMER_TO_TX);
				/* Antenna diversity is enabled on peer node */
				if (msg->payload.div_stat_rsp_data.status) {
					ant_div_settings = ANT_CTRL_0;
				} else { /* Antenna diversity is disabled on
						 * peer node */
					if (ENABLE_ANTENNA_1 ==
							msg->payload.
							div_stat_rsp_data
							.ant_sel) {
						/*Antenna Diversity Disabled on
						 * Remote Node and ANT2 is
						 * selected*/
						ant_div_settings = ANT_CTRL_1;
					} else if (ENABLE_ANTENNA_2 ==
							msg->payload.
							div_stat_rsp_data
							.ant_sel) {
						/*Antenna Diversity Disabled on
						 * Remote Node and ANT2 is
						 * selected*/
						ant_div_settings = ANT_CTRL_2;
					} else {
						ant_div_settings
							= INVALID_VALUE;
					}
				}

				/* Send Get confirmation with status SUCCESS */
				usr_perf_get_confirm(MAC_SUCCESS,
						PARAM_ANT_DIVERSITY_ON_PEER,
						(param_value_t *)&ant_div_settings);
			}

			op_mode = TX_OP_MODE;
		}
	}
	break;

#endif /* #if (ANTENNA_DIVERSITY == 1) */
#ifdef CRC_SETTING_ON_REMOTE_NODE
	case CRC_STAT_RSP:
	{
		bool crc_settings;
		if (op_mode == CRC_STATUS_REQ_WAIT) {
			if (*(mac_frame_info->mpdu) == (FRAME_OVERHEAD +
					((sizeof(app_payload_t) -
					sizeof(general_pkt_t)) +
					sizeof(crc_stat_rsp_t)))) {
				sw_timer_stop(APP_TIMER_TO_TX);
				if (msg->payload.crc_stat_rsp_data.status) {
					crc_settings = true;
				} else {
					crc_settings = false;
				}

				/* Send Get confirmation with status SUCCESS */
				param_value.param_value_bool = crc_settings;
				usr_perf_get_confirm(MAC_SUCCESS,
						PARAM_CRC_ON_PEER,
						&param_value);
			}

			op_mode = TX_OP_MODE;
		}
	}
	break;

#endif /* #ifdef CRC_SETTING_ON_REMOTE_NODE */
	case PEER_INFO_RSP:
	{
		if (op_mode == PEER_INFO_RSP_WAIT) {
			if (*(mac_frame_info->mpdu) == (FRAME_OVERHEAD +
					((sizeof(app_payload_t) -
					sizeof(general_pkt_t)) +
					sizeof(peer_info_rsp_t)))) {
				sw_timer_stop(APP_TIMER_TO_TX);

				/* Send the confirmation with status as SUCCESS
				**/
				usr_perf_start_confirm(MAC_SUCCESS,
						START_MODE_PER,
						&default_trx_config_params,
						msg->payload.peer_info_rsp_data.ic_type,
						msg->payload.peer_info_rsp_data.soc_mcu_name,
						msg->payload.peer_info_rsp_data.trx_name,
						msg->payload.peer_info_rsp_data.board_name,
						msg->payload.peer_info_rsp_data.mac_address,
						msg->payload.peer_info_rsp_data.fw_version,
						msg->payload.peer_info_rsp_data.feature_mask );
			}

			op_mode = TX_OP_MODE;
		}
	}
	break;

	case RANGE_TEST_RSP:
	{
		/*Verify if the response is recieved in the correct Operating
		 * mode*/
		if (op_mode == RANGE_TEST_TX) {
			/* Verify if the frame was already processed*/
			if (range_test_seq_num == msg->seq_num) {
				return;
			}

			/* Calculate the ED value and LQI for the received frame
			 * and
			 * also derrive the LQI and ED values sent by the
			 * receptor from the received payload */
			int8_t rssi_base_val, ed_value;
			rssi_base_val = tal_get_rssi_base_val();
			uint8_t phy_frame_len = mac_frame_info->mpdu[0];
			/* Map the register ed value to dbm values */
			ed_value
				= mac_frame_info->mpdu[phy_frame_len + LQI_LEN +
					ED_VAL_LEN] + rssi_base_val;
			app_led_event(LED_EVENT_RX_FRAME);

			/* Send the range test rsp indication to Host UI with
			 * the two set of ED and LQI values */

			usr_range_test_beacon_rsp(mac_frame_info->mpdu,
					mac_frame_info->mpdu[phy_frame_len
					+ LQI_LEN],
					ed_value,
					msg->payload.range_tx_data.lqi,
					msg->payload.range_tx_data.ed);
			range_test_seq_num = msg->seq_num;
		}
	}
	break;

	case RANGE_TEST_MARKER_CMD:
	{
		if (op_mode == RANGE_TEST_TX) {
			/* Calculate the ED value and LQI for the received
			 * marker frame */
			int8_t rssi_base_val, ed_value;
			rssi_base_val = tal_get_rssi_base_val();
			uint8_t phy_frame_len = mac_frame_info->mpdu[0];
			/* Map the register ed value to dbm values */
			ed_value
				= mac_frame_info->mpdu[phy_frame_len + LQI_LEN +
					ED_VAL_LEN] + rssi_base_val;

			/* Timer to Perform LED indication for received Marker
			 * indication */
			sw_timer_start(T_APP_TIMER,
					LED_BLINK_RATE_IN_MICRO_SEC,
					SW_TIMEOUT_RELATIVE,
					(FUNC_PTR)marker_rsp_timer_handler_cb,
					NULL);

			/* Send response to the receptor on receiving the marker
			 * packet */
			send_range_test_marker_rsp();
			/*send marker indication to Host UI */
			usr_range_test_marker_ind(mac_frame_info->mpdu,
					mac_frame_info->mpdu[phy_frame_len
					+ LQI_LEN], ed_value);
		}
	}
	break;

	default:
		break;
	}
}

/*
 * *\brief prints the hardware details
 */

void get_board_details(void)
{
	float fw_version = reverse_float(FIRMWARE_VERSION);

	/* Enable the mask bit for single/multi channel selection
	 * feature is available in the firmware
	 */
	fw_feature_mask |= MULTI_CHANNEL_SELECT;
	fw_feature_mask |= PER_RANGE_TEST_MODE;
	fw_feature_mask |= PER_REMOTE_CONFIG_MODE;
	fw_feature_mask |= PKT_STREAMING_MODE;
	fw_feature_mask |= CONTINUOUS_RX_ON_MODE;

	/* Send the Confirmation with the status as SUCCESS */
	usr_identify_board_confirm(MAC_SUCCESS,
			IC_TYPE,
			MCU_SOC_NAME,
			TRANSCEIVER_NAME,
			BOARD_NAME,
			(uint64_t)tal_pib.IeeeAddress,
			fw_version,
			fw_feature_mask);
}

/**
 * \brief Set Transceiver state as per the state given
 * \param trx_state Transceiver state to be set
 */
static void set_transceiver_state(uint8_t trx_state)
{
	param_value_t param_value;
	/* Check whether trasnceiver is in sleep state */
	if (true == trx_sleep_status) {
		switch (trx_state) {
			/* Wake up the trasceiver as Toggle sleep cmd is
			 * received */
#ifdef ENABLE_DEEP_SLEEP
		case TRX_SLEEP:
		{
			toggle_trx_sleep(SLEEP_MODE_1);
		}
		break;

		/* Wake up the trasceiver as Toggle deep sleep cmd is received
		**/
		case TRX_DEEP_SLEEP:
		{
			toggle_trx_sleep(DEEP_SLEEP_MODE);

			/* After wakeup from deep sleep,trx registers shall be
			 * restored, Antenna diversity is also enabled by
			 * default,
			 * It has to be disabled to demonstrate RPC
			 */
			recover_all_settings();
		}
		break;

#else
		/* Wake up the trasceiver as Toggle sleep cmd is received */
		case TRX_SLEEP:
		{
			toggle_trx_sleep();
		}
		break;
#endif /* End of ENABLE_DEEP_SLEEP */

		/* For any cmd other than toggle sleep, return with error code
		**/
		default:
		{
			/* Send confirmation with status Failure because the
			 * transceiver is in sleep */
			param_value.param_value_8bit = trx_state;
			usr_perf_set_confirm(TRANSCEIVER_IN_SLEEP,
					PARAM_TRX_STATE,
					&param_value);
			return;
		}
		break;
		}

		/* Send Set confirmation with status SUCCESS */
		trx_state = tal_get_trx_status();
		curr_trx_config_params.trx_state = trx_state;
		param_value.param_value_8bit = trx_state;
		usr_perf_set_confirm(MAC_SUCCESS,
				PARAM_TRX_STATE,
				&param_value);
		return;
	}

	switch (trx_state) {
	case TRX_RESET:  /* RESET */
	{
		/* save user setting before reset */
#if (TAL_TYPE != AT86RF230B)

		/* Save user settings like antenna diversity  & Rx sensitivity
		**/
		save_all_settings();
#endif
		if (MAC_SUCCESS == tal_reset(false)) {
			curr_trx_config_params.trx_state = TRX_OFF;
#if (TAL_TYPE != AT86RF230B)
			/* Restore all user settings after reset */
			recover_all_settings();
#endif
		}
	}
	break;

	case TRX_OFF: /* TRX_OFF */
	{
#if (ANTENNA_DIVERSITY == 1)

		/*
		 *  Disable antenna diversity: to reduce the power consumption
		 * or
		 *  avoid leakage current of an external RF switch during SLEEP.
		 */
		tal_ant_div_config(ANT_DIVERSITY_DISABLE, ANTENNA_DEFAULT);
#endif
		/* Set the state to TRX_OFF */
		set_trx_state(CMD_TRX_OFF);
		curr_trx_config_params.trx_state = TRX_OFF;
	}
	break;

	case PLL_ON: /*PLL_ON*/
	{
#if (ANTENNA_DIVERSITY == 1)
		/*  Enable antenna diversity */
		tal_ant_div_config(ANT_DIVERSITY_ENABLE, ANTENNA_DEFAULT);
#endif

		/*set the state to PLL_ON*/
		set_trx_state(CMD_PLL_ON);
		curr_trx_config_params.trx_state = PLL_ON;
	}
	break;

	case RX_AACK_ON: /*RX_AACK_ON*/
	{
#if (ANTENNA_DIVERSITY == 1)
		/*  Enable antenna diversity */
		tal_ant_div_config(ANT_DIVERSITY_ENABLE, ANT_AUTO_SEL);
#endif /* End of #if (ANTENNA_DIVERSITY == 1) */
		/*set the state to RX_AACK_ON */
		set_trx_state(CMD_RX_AACK_ON);
		curr_trx_config_params.trx_state = RX_AACK_ON;
	}
	break;

#ifdef ENABLE_DEEP_SLEEP
	case TRX_SLEEP:
	{
		/* Put the transceiver in sleep mode */
		toggle_trx_sleep(SLEEP_MODE_1);
		curr_trx_config_params.trx_state = TRX_SLEEP;
	}
	break;

	/* Handle deep sleep */
	case TRX_DEEP_SLEEP:
	{
		/* Save user settings like antenna diversity  & Rx sensitivity
		**/
		save_all_settings();
		toggle_trx_sleep(DEEP_SLEEP_MODE);
		curr_trx_config_params.trx_state = TRX_DEEP_SLEEP;
	}
	break;

#else
	case TRX_SLEEP: /* SLEEP */
	{
		/* Put the transceiver in sleep */
		toggle_trx_sleep();
		curr_trx_config_params.trx_state = TRX_SLEEP;
	}
	break;

#endif /* End of ENABLE_DEEP_SLEEP */
	default: /* Do nothing */
		break;
	}
	/* Send Set confirmation with status SUCCESS & and current trx state*/
	param_value.param_value_8bit = curr_trx_config_params.trx_state;
	usr_perf_set_confirm(MAC_SUCCESS,
			PARAM_TRX_STATE,
			&param_value);
}

/**
 *  \brief To toggle between sleep /deep state
 *
 *  \param sleep_mode Type of sleep mode as input in case of RF233
 */
#ifdef ENABLE_DEEP_SLEEP
static void toggle_trx_sleep(uint8_t sleep_mode)
#else  /* No DEEP_SLEEP */
static void toggle_trx_sleep(void)
#endif /* End of ENABLE_DEEP_SLEEP */
{
	if (false == trx_sleep_status) {
#ifdef ENABLE_DEEP_SLEEP
		switch (sleep_mode) {
		/* intentional fall through */
		case SLEEP_MODE_1:
		case DEEP_SLEEP_MODE:
		{
			/* Sleep cmd is successful */
			if (MAC_SUCCESS ==
					tal_trx_sleep((sleep_mode_t)sleep_mode))
			{
				trx_sleep_status = true;
			}
		}
		break;

		default:
			break;
		}
#else
		/* Sleep cmd is successful */
		if (MAC_SUCCESS == tal_trx_sleep(SLEEP_MODE_1)) {
			trx_sleep_status = true;
		}

#endif /* End of ENABLE_DEEP_SLEEP */
	} else {
		/* Wakeup from sleep or deep sleep is successful */
		if (MAC_SUCCESS == tal_trx_wakeup()) {
			trx_sleep_status = false;
		}
	}
}

/*
 * \brief User call back function after ED Scan completion
 * \param energy_level  Energy detected in a channel
 */
void per_mode_initiator_ed_end_cb(uint8_t energy_level)
{
	uint8_t ch_cnt;
	pib_value_t pib_value_temp;
	static uint8_t p_in_index;
	uint16_t channel;

#if ((TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B))
	uint8_t page;
#endif /* End of (TAL_TYPE != AT86RF212) && (TAL_TYPE != AT86RF212B) */
	static ed_scan_result_t ed_scan_result[16];

	/* Print result */
	tal_pib_get(phyCurrentChannel, (uint8_t *)&channel);

#ifndef TRX_REG_RAW_VALUE
	/* Re-scale ED value to Pin(dBm)- for more details refer tal_ed.c */
#if ((TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B))
	float reg_val = (float)(62 / 255) * energy_level;
	int8_t p_in = (uint8_t)((reg_val * 1.03) - 99);
#elif (TAL_TYPE == ATMEGARFA1) || (TAL_TYPE == ATMEGARFR2)
	float reg_val = (float)(55 / 255) * energy_level;
	int8_t p_in =  (int8_t)reg_val + (RSSI_BASE_VAL_DBM));
#elif (TAL_TYPE == AT86RF230B) || (TAL_TYPE == AT86RF231) || \
	(TAL_TYPE == AT86RF233) || (TAL_TYPE == AT86RF232)
	float reg_val = (float)(56 / 255) * energy_level;
	int8_t p_in = (int8_t)(reg_val + (RSSI_BASE_VAL_DBM));
#endif

#else
	/* Use Pure reg values */
#if  ((TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B))
	int8_t p_in = (uint8_t)(energy_level - 99);
#elif (TAL_TYPE == ATMEGARFA1) || (TAL_TYPE == ATMEGARFR2)
	int8_t p_in = (energy_level + (RSSI_BASE_VAL_DBM));

#elif  (TAL_TYPE == AT86RF230B) || (TAL_TYPE == AT86RF231) || (TAL_TYPE == \
	AT86RF232) || (TAL_TYPE == \
	AT86RF233)
#ifdef EXT_RF_FRONT_END_CTRL
	int8_t p_in =  (energy_level + (RSSI_BASE_VAL_EXT));
#else
	int8_t p_in = (energy_level + (RSSI_BASE_VAL_DBM));
#endif
#endif /* End of ((TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B) */
#endif /* End of TRX_REG_RAW_VALUE */

	/* get the channel number and its corresponding Pin value */
	ed_scan_result[p_in_index].channel_no = channel;
	ed_scan_result[p_in_index].p_in = p_in;

	/* Increment the index  */
	p_in_index++;

	/* Check for next channel */
	channel = INVALID_VALUE;
	for (ch_cnt = MIN_CHANNEL;
			ch_cnt <= MAX_CHANNEL;
			ch_cnt++) {
		if ((scan_channel_mask & ((uint32_t)1 << ch_cnt)) > 0) {
			channel = ch_cnt;
			scan_channel_mask &= ~((uint32_t)1 << ch_cnt);
			break;
		}

#if ((TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B))
		tal_pib_get(phyCurrentPage, &page);
		if (5 == page) {
			if (4 == ch_cnt) {
				channel = INVALID_VALUE;
			}
		}

#endif /* End of (TAL_TYPE != AT86RF212) && (TAL_TYPE != AT86RF212B) */
	}
	/* Check if all channels were scanned. */
	if (INVALID_VALUE == channel) {
		p_in_index = 0;
		/* Set original channel. */
		pib_value_temp.pib_value_8bit = channel_before_scan;
		tal_pib_set(phyCurrentChannel,
				&pib_value_temp);
		scanning = false;
#if  ((TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B))
		tal_pib_set(phyTransmitPower, (pib_value_t *)&phy_tx_power);
#endif

		/**
		 * Send the ED_END_INDICATION with the no. of channels and
		 * the list with channel no and pin values
		 */
		usr_ed_scan_end_indication(num_channels, ed_scan_result);
	} else {
		/* Scan next channel */
		pib_value_temp.pib_value_8bit = (uint8_t)channel;
		tal_pib_set(phyCurrentChannel, &pib_value_temp);
		tal_ed_start(scan_duration);
	}
}

/*
 * *\brief Function to set the various configuration parameters for PER Test
 *
 ******\param param_type   Type of the parameter to be set
 ******\param param_value  Pointer to the value to be set
 */

void perf_set_req(uint8_t set_param_type, param_value_t *param_value)
{
	param_value_t param_value_temp;
	switch (set_param_type) { /* parameter type */
	case PARAM_CHANNEL:
	{   /* Channel Set request */
		set_channel(param_value->param_value_16bit);
	}
	break;

	case PARAM_CHANNEL_PAGE:
	{    /* Channel Page Set request */
		set_channel_page(param_value->param_value_8bit);
	}
	break;

#if ((TAL_TYPE != AT86RF212) && (TAL_TYPE != AT86RF212B))
	case PARAM_TX_POWER_REG:
	{   /* TX power in Register value Set request  */
		set_tx_power(0, param_value->param_value_8bit);
		last_tx_power_format_set = 0;
	}
	break;

#endif /* End of (TAL_TYPE != AT86RF212) && (TAL_TYPE != AT86RF212B) */
	case PARAM_TX_POWER_DBM:
	{   /* TX power in dBm value Set request  */
		set_tx_power(1, (int8_t)param_value->param_value_8bit);
#if ((TAL_TYPE != AT86RF212) && (TAL_TYPE != AT86RF212B))
		last_tx_power_format_set = 1;
#endif /* End of (TAL_TYPE != AT86RF212) && (TAL_TYPE != AT86RF212B) */
	}
	break;

	case PARAM_CSMA:
	{ /* CSMA configuration request- Enable/Disable */
		config_csma(param_value->param_value_bool);
	}
	break;

	case PARAM_FRAME_RETRY:
	{   /* Frame Retry configuration request-Enable/Disable */
		config_frame_retry(param_value->param_value_bool);
	}
	break;

	case PARAM_ACK_REQUEST:

	{    /* Auto Ack Request configuration request- Enable/Disable */
		config_ack_request(param_value->param_value_bool);
	}
	break;

#if (ANTENNA_DIVERSITY == 1)
	case PARAM_ANT_DIVERSITY:
	{    /* Antenna Diversity Configuration request */
		set_antenna_diversity_settings(param_value->param_value_8bit);
	}
	break;

	case PARAM_ANT_DIVERSITY_ON_PEER:

	{   /* Antenna Diversity Configuration on * remote request */
		config_antenna_diversity_peer_node(param_value->param_value_8bit);
	}
	break;

#endif /* #if (ANTENNA_DIVERSITY ==1) */
#if (TAL_TYPE != AT86RF230B)
	case PARAM_DESENSITIZATION:

	{     /* Configure Rx Desensitization request- Enable /Disable*/
		config_rx_desensitization(param_value->param_value_bool);
	}
	break;

#endif /* (TAL_TYPE != AT86RF230B) */
#ifdef CRC_SETTING_ON_REMOTE_NODE
	case PARAM_CRC_ON_PEER:
	{   /* Set CRC settings on Peer node request */
		config_crc_peer_node(param_value->param_value_bool);
	}
	break;

#endif
	case PARAM_TRX_STATE:
	{    /* Set Transceiver state request */
		set_transceiver_state(param_value->param_value_8bit);
	}
	break;

	case PARAM_NO_OF_TEST_FRAMES:

	{   /* Set No.Of test Frames for PER test  request */
		uint32_t no_of_test_frames = 0;

		memcpy(&(no_of_test_frames),
				&(param_value->param_value_32bit), 4);

		if ((no_of_test_frames) == NUL_VAL) {
			param_value_temp.param_value_32bit = no_of_test_frames;
			usr_perf_set_confirm(VALUE_OUT_OF_RANGE,
					PARAM_NO_OF_TEST_FRAMES,
					&param_value_temp);
		} else {
			curr_trx_config_params.number_test_frames
				= no_of_test_frames;
			/* Send Set confirmation with status SUCCESS */
			param_value_temp.param_value_32bit
				= curr_trx_config_params.
					number_test_frames;
			usr_perf_set_confirm(MAC_SUCCESS,
					PARAM_NO_OF_TEST_FRAMES,
					&param_value_temp);
		}
	}
	break;

	case PARAM_PHY_FRAME_LENGTH:

	{   /* Set PHY frame length for PER test  request */
		set_phy_frame_length(param_value->param_value_16bit);
	}
	break;

#if ((TAL_TYPE == AT86RF233) || (TAL_TYPE == ATMEGARFR2))
	case PARAM_RPC:
	{
		config_rpc_mode(param_value->param_value_bool);
	}
	break;

#endif
#if (TAL_TYPE == AT86RF233)
	case PARAM_ISM_FREQUENCY:
	{
		config_frequency(param_value->param_value_float);
	}
	break;

#endif /* End of (TAL_TYPE == AT86RF233) */
	default:
	{
		/* Send Set confirmation with status INVALID ARGUMENT */
		usr_perf_set_confirm(INVALID_ARGUMENT, set_param_type,
				param_value);
	}
	break;
	}
}

/*
 * *\brief Function to get the various configuration parameters for PER Test
 *
 ******\param param_type Parameter type to be read
 */
void perf_get_req(uint8_t param_type_data)
{
	param_value_t param_value;
	/*  check for parameter type */
	switch (param_type_data) {
	case PARAM_CHANNEL:
	{   /* Channel Get request */
		uint16_t current_channel = 0;
		tal_pib_get(phyCurrentChannel, (uint8_t *)&current_channel);
		/* Send Get confirmation with status SUCCESS */
		param_value.param_value_16bit = current_channel;
		usr_perf_get_confirm(MAC_SUCCESS, PARAM_CHANNEL,
				&param_value);
	}
	break;

	case PARAM_CHANNEL_PAGE:
	{   /* Channel Page Get request */
		uint8_t ch_page = 0;
		tal_pib_get(phyCurrentPage, &ch_page);

		/* Send Get confirmation with status SUCCESS */
		param_value.param_value_8bit = ch_page;
		usr_perf_get_confirm(MAC_SUCCESS, PARAM_CHANNEL_PAGE,
				&param_value);
	}
	break;

#if ((TAL_TYPE != AT86RF212) && (TAL_TYPE != AT86RF212B))
	case PARAM_TX_POWER_REG:
	{     /* TX power in Register value Set request  */
		uint8_t tx_pwr_reg = 0;

		tal_get_curr_trx_config(TX_PWR, &(tx_pwr_reg));

		/* Send Get confirmation with status SUCCESS */
		param_value.param_value_8bit = tx_pwr_reg;
		usr_perf_get_confirm(MAC_SUCCESS, PARAM_TX_POWER_REG,
				&param_value);
	}
	break;

#endif /* End of (TAL_TYPE != AT86RF212) && (TAL_TYPE != AT86RF212B)  */
	case PARAM_TX_POWER_DBM:
	{   /* TX power in dBm value Set request  */
		uint8_t tx_pwr = 0;
		int8_t tx_pwr_dbm = 0;
		tal_pib_get(phyTransmitPower, &tx_pwr);
		tx_pwr_dbm = CONV_phyTransmitPower_TO_DBM(tx_pwr);

		/* Send Get confirmation with status SUCCESS */
		param_value.param_value_8bit = tx_pwr_dbm;
		usr_perf_get_confirm(MAC_SUCCESS,
				PARAM_TX_POWER_DBM,
				&param_value);
	}
	break;

	case PARAM_CSMA:
	{
		/* CSMA configuration request- Enable/Disable */
		/* Send Get confirmation with status SUCCESS */
		param_value.param_value_bool
			= curr_trx_config_params.csma_enabled;
		usr_perf_get_confirm(MAC_SUCCESS,
				PARAM_CSMA,
				&param_value);
	}
	break;

	case PARAM_FRAME_RETRY:

	{   /* Frame Retry configuration request- Enable/Disable */
		/* Send Get confirmation with status SUCCESS */
		param_value.param_value_bool
			= curr_trx_config_params.retry_enabled;
		usr_perf_get_confirm(MAC_SUCCESS,
				PARAM_FRAME_RETRY,
				&param_value);
	}
	break;

	case PARAM_ACK_REQUEST:

	{   /* Auto Ack Request configuration request-  Enable/Disable */
		/* Send Get confirmation with status SUCCESS */
		param_value.param_value_bool
			= curr_trx_config_params.ack_request;
		usr_perf_get_confirm(MAC_SUCCESS,
				PARAM_ACK_REQUEST,
				&param_value);
	}
	break;

#if (ANTENNA_DIVERSITY == 1)
	case PARAM_ANT_DIVERSITY:
	{
		/* Antenna Diversity Configuration request */
		/* Send Get confirmation with status SUCCESS */
		param_value.param_value_8bit
			= curr_trx_config_params.antenna_selected;
		usr_perf_get_confirm(MAC_SUCCESS,
				PARAM_ANT_DIVERSITY,
				&param_value);
	}
	break;

	case PARAM_ANT_DIVERSITY_ON_PEER:

	{   /* Antenna Diversity Configuration on remote request */
		get_diversity_settings_peer_node();
	}
	break;

#endif /* #if (ANTENNA_DIVERSITY == 1) */
#if (TAL_TYPE != AT86RF230B)
	case PARAM_DESENSITIZATION:
	{
		/* Configure Rx Desensitization request- Enable /Disable*/

		/* Send Get confirmation with status SUCCESS */
		param_value.param_value_bool
			= curr_trx_config_params.rx_desensitize;
		usr_perf_get_confirm(MAC_SUCCESS,
				PARAM_DESENSITIZATION,
				&param_value);
	}
	break;

#endif /* (TAL_TYPE != AT86RF230B) */
#ifdef CRC_SETTING_ON_REMOTE_NODE
	case PARAM_CRC_ON_PEER:
	{
		/* Set CRC settings on Peer node request */
		get_crc_settings_peer_node();
	}
	break;

#endif
	case PARAM_TRX_STATE:
	{
		/* Set Transceiver state request */
		uint8_t trx_state;
		trx_state = tal_get_trx_status();

		/* Send Get confirmation with status SUCCESS */
		param_value.param_value_8bit = trx_state;
		usr_perf_get_confirm(MAC_SUCCESS,
				PARAM_TRX_STATE,
				&param_value);
	}
	break;

	case PARAM_NO_OF_TEST_FRAMES:
	{
		/* Set No.Of test Frames for PER test request */

		/* Send get confirmation with status SUCCESS */
		param_value.param_value_32bit
			= curr_trx_config_params.number_test_frames;
		usr_perf_get_confirm(MAC_SUCCESS,
				PARAM_NO_OF_TEST_FRAMES,
				&param_value);
	}
	break;

	case PARAM_PHY_FRAME_LENGTH:
	{
		/* Set PHY frame length for PER test request */

		/* Send get confirmation with status SUCCESS */
		param_value.param_value_16bit
			= curr_trx_config_params.phy_frame_length;
		usr_perf_get_confirm(MAC_SUCCESS,
				PARAM_PHY_FRAME_LENGTH,
				&param_value);
	}
	break;

#if ((TAL_TYPE == AT86RF233) || (TAL_TYPE == ATMEGARFR2))
	case PARAM_RPC:
	{
		/* Send Get confirmation with status SUCCESS */
		param_value.param_value_bool
			= curr_trx_config_params.rpc_enable;
		usr_perf_get_confirm(MAC_SUCCESS,
				PARAM_RPC,
				&param_value);
	}
	break;

#endif
#if ((TAL_TYPE == AT86RF233))
	case PARAM_ISM_FREQUENCY:
	{
		param_value.param_value_float
			= curr_trx_config_params.ism_frequency;
		usr_perf_get_confirm(MAC_SUCCESS,
				PARAM_ISM_FREQUENCY,
				&param_value);
	}
	break;

#endif /* End of (TAL_TYPE == AT86RF233) */
	default:
	{
		uint8_t dummy_val = 0;
		/* Send Get confirmation with status INVALID ARGUMENT */
		param_value.param_value_8bit = dummy_val;
		usr_perf_get_confirm(INVALID_ARGUMENT, param_type_data,
				&param_value);
	}
	break;
	}
}

/**
 * \brief To set PER Test frame length
 *
 * \param frame_len Length of the frame to be set
 */
static void set_phy_frame_length(uint16_t frame_len)
{
	param_value_t param_value;
	/* Check for maximum allowed IEEE 802.15.4 frame length. */
	if (frame_len > aMaxPHYPacketSize) {
		curr_trx_config_params.phy_frame_length = aMaxPHYPacketSize;
	} else if (frame_len < (FRAME_OVERHEAD + 1)) { /* 1=> cmdID*/
		curr_trx_config_params.phy_frame_length = (FRAME_OVERHEAD + 1);
	} else {
		curr_trx_config_params.phy_frame_length = frame_len;
	}

	/* The FCF has to be updated. */
	configure_frame_sending();

	/* Send Set confirmation with status SUCCESS */
	param_value.param_value_16bit = curr_trx_config_params.phy_frame_length;
	usr_perf_set_confirm(MAC_SUCCESS,
			PARAM_PHY_FRAME_LENGTH,
			&param_value);
}

/**
 * \brief To set the Channel
 *
 * \param channel channel number to be set
 */
static void set_channel(uint16_t channel)
{
	uint32_t supported_channels;
	param_value_t param_value;
	pib_value_t pib_value;

	tal_pib_get(phyChannelsSupported, (uint8_t *)&supported_channels);

	/* Check the channel is a valid one and is a supported channel */
	if ((channel < BIT_COUNT) &&
			(supported_channels & ((uint32_t)0x01) << channel)) {
		if (true == peer_found) {
			send_parameters_changed(CHANNEL, (uint8_t)channel);
		} else {
			/* Set back the Tx power to default value when
			 * the channel changed from 26 to other channel
			 */
#ifdef EXT_RF_FRONT_END_CTRL
			uint16_t chn_before_set;
			tal_pib_get(phyCurrentChannel,
					(uint8_t *)&chn_before_set);
#endif
#if ((TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B))
			int8_t dbm_val = 0;
			uint8_t tx_pwr = 0;
#endif
#if (TAL_TYPE == AT86RF233)
			/* Set the CC_BAND to zero before setting the channel */
			tal_set_frequency_regs(CC_BAND_0, CC_NUMBER_0);
#endif
			pib_value.pib_value_8bit = (uint8_t)channel;
			tal_pib_set(phyCurrentChannel, &pib_value);

			/* Update the database */
			curr_trx_config_params.channel = channel;
#if ((TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B))
			tal_pib_get(phyTransmitPower, &tx_pwr);
			dbm_val = CONV_phyTransmitPower_TO_DBM(tx_pwr);
			curr_trx_config_params.tx_power_dbm = dbm_val;
#endif

#ifdef EXT_RF_FRONT_END_CTRL

			/* Limit the tx power below the default power for ch26
			 * to meet
			 * FCC Compliance */
			limit_tx_power_in_ch26((uint8_t)channel,
					(uint8_t)chn_before_set);

			/* update the curr_tx_power_reg to use in trx_config
			 * print menu  */
#endif

			/* Send Set confirmation with status SUCCESS */
			param_value.param_value_16bit = channel;
			usr_perf_set_confirm(MAC_SUCCESS,
					PARAM_CHANNEL,
					&param_value);
		}
	} else {
		/* Send Set confirmation with status MAC_INVALID_PARAMETER */
		param_value.param_value_16bit = channel;
		usr_perf_set_confirm(VALUE_OUT_OF_RANGE,
				PARAM_CHANNEL,
				&param_value);
	}
}

/**
 * \brief To set the Channel Page
 *
 * \param channel_page  Channel page to be set
 */
static void set_channel_page(uint8_t channel_page)
{
	param_value_t param_value;
	pib_value_t pib_value;
#if ((TAL_TYPE == AT86RF230B) || (TAL_TYPE == AT86RF232))
	if (channel_page != 0) {
		/* Send the confirmation with status as VALUE_OUT_OF_RANGE */
		usr_perf_set_confirm(VALUE_OUT_OF_RANGE,
				PARAM_CHANNEL_PAGE,
				(param_value_t *)&channel_page);
		return;
	}

#endif /* End of #if((TAL_TYPE == AT86RF230B) ) */

	switch (channel_page) {
	case 0:
	case 2:
	case 16:
	case 17:
#if ((TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B))
	case 5:
	case 18:
	case 19:
#endif /* End of (TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B) */
		{
			if (true == peer_found) {
				send_parameters_changed(CHANNEL_PAGE,
						channel_page);
			} else {
#if ((TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B))
				uint16_t channel;
				int8_t dbm_val = 0;
				uint8_t tx_pwr = 0;
#endif
				pib_value.pib_value_8bit = channel_page;
				tal_pib_set(phyCurrentPage,
						&pib_value);

				/* update the data base with this value */
				curr_trx_config_params.channel_page
					= channel_page;
#if ((TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B))
				tal_pib_get(phyCurrentChannel, (uint8_t *)
						&channel);
				curr_trx_config_params.channel
					= (uint8_t)channel;
				tal_pib_get(phyTransmitPower, &tx_pwr);
				dbm_val = CONV_phyTransmitPower_TO_DBM(tx_pwr);
				curr_trx_config_params.tx_power_dbm = dbm_val;
#endif

				/* Send the confirmation with status as SUCCESS
				**/
				param_value.param_value_8bit = channel_page;
				usr_perf_set_confirm(MAC_SUCCESS,
						PARAM_CHANNEL_PAGE,
						&param_value);
			}
		}
		break;

	default:
	{
		/* Send the confirmation with status as VALUE_OUT_OF_RANGE */
		param_value.param_value_8bit = channel_page;
		usr_perf_set_confirm(VALUE_OUT_OF_RANGE,
				PARAM_CHANNEL_PAGE,
				&param_value);
	}
	break;
	}
}

/**
 * \brief To set the Tx power of the Transceiver
 *
 * \param tx_power_format Format in which input is given whether in dBm or reg
 * \param power_value     Tx power value in the given format
 */
static void set_tx_power(uint8_t tx_power_format, int8_t power_value)
{
	int8_t tx_pwr_dbm;
	uint8_t temp_var;
	param_value_t param_value;
	pib_value_t pib_value;

	switch (tx_power_format) {
#if ((TAL_TYPE != AT86RF212) && (TAL_TYPE != AT86RF212B))
	/* To handle TX_PWR reg value input */
	/* Input is in the Register value */
	case TX_POWER_FORMAT_REG:
	{
		uint8_t tx_pwr_reg = (uint8_t)power_value;

#ifdef EXT_RF_FRONT_END_CTRL

		/* check for the valid range of TX_PWR register values based on
		 * the channel */

		if (((curr_trx_config_params.channel == CHANNEL_26) &&
				(tx_pwr_reg >= MAX_TX_PWR_REG_VAL_CH26) &&
				(tx_pwr_reg <= MIN_TX_PWR_REG_VAL)) ||
				((curr_trx_config_params.channel !=
				CHANNEL_26) &&
				(tx_pwr_reg <= MIN_TX_PWR_REG_VAL))
				)
#else
		if (tx_pwr_reg <= MIN_TX_PWR_REG_VAL)
#endif
		{
			/* set the Tx power on source node in case of no
			 * peer */
			if (MAC_SUCCESS ==
					tal_convert_reg_value_to_dBm(tx_pwr_reg,
					&tx_pwr_dbm)) {
				temp_var = CONV_DBM_TO_phyTransmitPower(
						tx_pwr_dbm);
				pib_value.pib_value_8bit = temp_var;
				tal_pib_set(phyTransmitPower, &pib_value);

				/* To make sure that TX_PWR register is
				 * updated with the
				 * value whatever user povided.Otherwise
				 * lowest dBm power
				 * (highest reg value will be taken)
				 */
				tal_set_tx_pwr(REGISTER_VALUE, tx_pwr_reg);

				/* update the data base with this value */
				curr_trx_config_params.tx_power_reg
					= tx_pwr_reg;

				/*Tx power in dBm also need to be
				 * updated as it changes with reg value
				 **/
				curr_trx_config_params.tx_power_dbm
					= tx_pwr_dbm;

				/* Send Set confirmation with status SUCCESS */
				param_value.param_value_8bit = tx_pwr_reg;
				usr_perf_set_confirm(MAC_SUCCESS,
						PARAM_TX_POWER_REG,
						&param_value);
			}
		} else {
			/* Send confirmation as VALUE_OUT_OF_RANGE */
			param_value.param_value_8bit = tx_pwr_reg;
			usr_perf_set_confirm(VALUE_OUT_OF_RANGE,
					PARAM_TX_POWER_REG,
					&param_value);
		}
	}
	break;

#endif /* End of ( (TAL_TYPE != AT86RF212) && (TAL_TYPE != AT86RF212B) ) */
	/* To handle Absolute power(dBm) input */
	/* Input is in dBm values */
	case TX_POWER_FORMAT_DBM:
	{
		tx_pwr_dbm = power_value;
#if ((TAL_TYPE != AT86RF212) && (TAL_TYPE != AT86RF212B))
		int8_t min_dbm_val;
		int8_t max_dbm_val;
		/* Check for the valid range of tx power in dBm */
		tal_convert_reg_value_to_dBm(MIN_TX_PWR_REG_VAL, &min_dbm_val);

		/* get max tx power in dbM allowed */
		tal_convert_reg_value_to_dBm(0x00, &max_dbm_val);

#ifdef EXT_RF_FRONT_END_CTRL
		int8_t ch26_max_dbm_val;

		/* get max tx power in dbM allowed */
		tal_convert_reg_value_to_dBm(0x00, &max_dbm_val);
		tal_convert_reg_value_to_dBm(MAX_TX_PWR_REG_VAL_CH26,
				&ch26_max_dbm_val);

		/* check for the valid range of TX_PWR register values*/
		if (((curr_trx_config_params.channel == CHANNEL_26) &&
				(tx_pwr_dbm <= ch26_max_dbm_val) &&
				(tx_pwr_dbm >= min_dbm_val)) ||
				((curr_trx_config_params.channel !=
				CHANNEL_26) &&
				(tx_pwr_dbm <= max_dbm_val) &&
				(tx_pwr_dbm >= min_dbm_val))
				)
#else
		/* get max tx power in dbM allowed */
		tal_convert_reg_value_to_dBm(0x00, &max_dbm_val);

		if ((tx_pwr_dbm >= min_dbm_val) && (tx_pwr_dbm <= max_dbm_val))
#endif /* End of EXT_RF_FRONT_END_CTRL */

		{
			temp_var = CONV_DBM_TO_phyTransmitPower(
					tx_pwr_dbm);

			pib_value.pib_value_8bit = temp_var;
			tal_pib_set(phyTransmitPower, &pib_value);

			/* update the data base with this value */
			curr_trx_config_params.tx_power_dbm
				= tx_pwr_dbm;

			/*Tx power in Reg also need to be updated as it
			 * changes with dBm value */
			tal_get_curr_trx_config(TX_PWR,
					&(curr_trx_config_params
					.tx_power_reg));

			/* Send Set confirmation with status SUCCESS */
			param_value.param_value_8bit = tx_pwr_dbm;
			usr_perf_set_confirm(MAC_SUCCESS,
					PARAM_TX_POWER_DBM,
					&param_value);
		} else {
			/* Send Set confirmation with status VALUE_OUT_OF_RANGE
			**/
			param_value.param_value_8bit = tx_pwr_dbm;
			usr_perf_set_confirm(VALUE_OUT_OF_RANGE,
					PARAM_TX_POWER_DBM,
					&param_value);
		}

#else   /* Handle Tx power for Rf212 & rf212B */

		/* Check for the valid range of tx power in dBm based on channel
		 *& page */
		if (validate_tx_power(tx_pwr_dbm)) {
			/* set the Tx power on source node in case of no
			 * peer */
			temp_var = CONV_DBM_TO_phyTransmitPower(
					tx_pwr_dbm);
			tal_pib_set(phyTransmitPower,
					(pib_value_t *)&temp_var);

			/* update the data base with this value */
			curr_trx_config_params.tx_power_dbm
				= tx_pwr_dbm;

			/* Send Set confirmation with status SUCCESS */
			usr_perf_set_confirm(MAC_SUCCESS,
					PARAM_TX_POWER_DBM,
					(param_value_t *)&tx_pwr_dbm);
		} else {
			/* Send Set confirmation with status SUCCESS */
			usr_perf_set_confirm(VALUE_OUT_OF_RANGE,
					PARAM_TX_POWER_DBM,
					(param_value_t *)&tx_pwr_dbm);
		}

#endif /* End of ((TAL_TYPE != AT86RF212) && (TAL_TYPE != AT86RF212B)) */
	}     /* case end */
	break;

	default:
		break;
	}
}

/**
 * \brief To start the ED scan
 *
 * \param ed_scan_duration  Scan duration parameter which is used to calculate
 *                          the scan time on each channel
 * \param channel_sel_mask  Selected channel mask for which the Energy should
 *                          be detected
 */
void start_ed_scan(uint8_t ed_scan_duration, uint32_t channel_sel_mask)
{
	uint16_t first_channel = 0;
	uint8_t ch_cnt;
	pib_value_t pib_value;

	float scan_time;
	/* Initialize the no. of channels to 0 */
	num_channels = 0;

	scan_duration = ed_scan_duration;
	scan_channel_mask = (channel_sel_mask & tal_pib.SupportedChannels);

#if ((TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B))
	/* saving the current transmit power to restore after scan*/
	tal_pib_get(phyTransmitPower, &phy_tx_power);
#endif
	/* Check the range for the scan duration */
	if (scan_duration > MAX_SCAN_DURATION) {
		/* Send confirmation with the error code - invalid parameter */
		usr_ed_scan_start_confirm(VALUE_OUT_OF_RANGE, NUL_VAL, NUL_VAL);
		return;
	}

	scanning = true;
	for (ch_cnt = MIN_CHANNEL; ch_cnt <= MAX_CHANNEL; ch_cnt++) {
		num_channels
			+= (scan_channel_mask &
				((uint32_t)1 << ch_cnt)) ? 1 : 0;
	}

	scan_time = (float)aBaseSuperframeDuration *
			((1 << scan_duration) + 1) *
			(TAL_CONVERT_SYMBOLS_TO_US(1)) * num_channels / (1e6);

	if (scan_time >= 60) {
		uint8_t scan_time_min = (uint8_t)(scan_time / 60);
		float scan_time_sec = (scan_time -  (scan_time_min * 60));

		/* Send confirm with the status as SUCCESS and scan time in
		 * minutes and secs */
		usr_ed_scan_start_confirm(MAC_SUCCESS, scan_time_min, reverse_float(
				scan_time_sec));
	} else {
		/* Send conform with the status as SUCCESS and scan time in secs
		 *& minutes = 0 */
		usr_ed_scan_start_confirm(MAC_SUCCESS, NUL_VAL,
				reverse_float(scan_time));
	}

	uint16_t temp_var;
	tal_pib_get(phyCurrentChannel, (uint8_t *)&temp_var);
	channel_before_scan = (uint8_t)temp_var;

	/* Identify first channel */
	for (ch_cnt = MIN_CHANNEL; ch_cnt <= MAX_CHANNEL; ch_cnt++) {
		if ((scan_channel_mask & ((uint32_t)1 << ch_cnt)) > 0) {
			first_channel = ch_cnt;
			scan_channel_mask &= ~((uint32_t)1 << ch_cnt);
			break;
		}
	}
	pib_value.pib_value_8bit = (uint8_t)first_channel;
	tal_pib_set(phyCurrentChannel, &pib_value);
	tal_ed_start(scan_duration);
}

/**
 * \brief To get the Sensor data like Battery voltage
 */
void get_sensor_data(void)
{
	uint16_t bat_mon;
	double bat_voltage;
	double temperature;

	bat_mon = tfa_get_batmon_voltage();
	bat_voltage = reverse_float((double)(bat_mon * MILLI_VOLT_MULTIPLIER));

#if (TAL_TYPE == ATMEGARFR2)
	temperature = tfa_get_temperature();
	temperature = reverse_float(temperature);
#else
	temperature = reverse_float(0.0);
#endif /* End of #if (TAL_TYPE == ATMEGARFR2) */
	/* Send the confirmation with status as SUCCESS */
	usr_sensor_data_get_confirm(MAC_SUCCESS, bat_voltage, temperature);
}

/**
 * \brief To set the default values of  all configurable
 * parameters on source and peer node
 */
void set_default_configuration(void)
{
	trx_config_params_t dummy_params = {0};

	/* Send set_default_config_req command if the peer device is connected
	**/
	if (true == peer_found) {
		/* CRC set request sent successfully */
		if (send_set_default_config_command()) {
			op_mode = SET_DEFAULT_CONFIG_PEER;
		} else {
			/* Send confirmation with TRANSMISSION_FAILURE */
			usr_set_default_config_confirm(TRANSMISSION_FAILURE,
					&dummy_params);
			op_mode = TX_OP_MODE;
		}
	} else { /* Single node Tests case,set default values only on source
		  * node */
		 /* Set the default values for all configurable parameters */
		config_per_test_parameters();

		/* Send the confirmation with the status as SUCCESS */
		usr_set_default_config_confirm(MAC_SUCCESS,
				&default_trx_config_params);
	}
}

/**
 * \brief To Get the updated/latest values of  all configurable
 * parameters on source and peer node
 */
void get_current_configuration(void)
{
	/* Send the confirmation with the status as SUCCESS */
	usr_get_current_config_confirm(MAC_SUCCESS, &curr_trx_config_params);
}

/**
 * \brief To set or reset ACK request value
 *
 * \param config_value value to be configured
 */
static void config_ack_request(bool config_value)
{
	param_value_t param_value;
	/* Set the ack request configuration as per the config value */
	if (true == config_value) {
		curr_trx_config_params.ack_request = true;
	} else {
		curr_trx_config_params.ack_request = false;
	}

	/* Send Set confirmation with status SUCCESS */
	param_value.param_value_bool = curr_trx_config_params.ack_request;
	usr_perf_set_confirm(MAC_SUCCESS,
			PARAM_ACK_REQUEST,
			&param_value);
}

/**
 * \brief To configure CSMA Setting in the transceiver
 *
 * \param config_value value to be configured
 */
static void config_csma(bool config_value)
{
	param_value_t param_value;
	/* Set the csma configuration as per the config value */
	if (true == config_value) {
		curr_trx_config_params.csma_enabled = true;
	} else {
		curr_trx_config_params.csma_enabled = false;
	}

	/* Send Set confirmation with status SUCCESS */
	param_value.param_value_bool = curr_trx_config_params.csma_enabled;
	usr_perf_set_confirm(MAC_SUCCESS,
			PARAM_CSMA,
			&param_value);
}

/**
 * \brief To configure frame retry Setting
 *
 * \param config_value value to be configured
 */
static void config_frame_retry(bool config_value)
{
	param_value_t param_value;
	/* Set the auto transmission configuration as per the config value */
	if (true == config_value) {
		curr_trx_config_params.retry_enabled = true;
	} else {
		curr_trx_config_params.retry_enabled = false;
	}

	/* Send Set confirmation with status SUCCESS */
	param_value.param_value_bool = curr_trx_config_params.retry_enabled;
	usr_perf_set_confirm(MAC_SUCCESS,
			PARAM_FRAME_RETRY,
			&param_value);
}

#if (ANTENNA_DIVERSITY == 1)

/**
 * \brief To configure antenna diversity setting
 * \param config_value Value to be configured for Antenna diversity
 */
static void set_antenna_diversity_settings(uint8_t config_value)
{
	uint8_t curr_state = INVALID_VALUE;

	if (ENABLE_ANT_DIVERSITY == config_value) { /* Enable antenna diversity
		                                     * request */
		/* Update the data base with these values*/
		curr_trx_config_params.antenna_diversity = true;
		curr_trx_config_params.antenna_selected = ANT_CTRL_0;

		tal_ant_div_config(ANT_DIVERSITY_ENABLE, ANTENNA_DEFAULT);
	} else { /* Disable antenna diversity request */
		 /* To avoid the transition time from antenna diversity enabled
		  * to disabled
		  * during receiver on state,switch off the transceiver and
		  * restore the
		  * state after the antenna diversity settings are done */
		curr_state = tal_get_trx_status();

		if ((RX_ON == curr_state) || (RX_AACK_ON == curr_state)) {
			set_trx_state(CMD_TRX_OFF);
		}

		switch (config_value) {
		/* Disable antenna Diversity request & Enable ANT1 */
		case ENABLE_ANTENNA_1:
		{
			/* Update the data base with these values*/
			curr_trx_config_params.antenna_diversity = false;
			curr_trx_config_params.antenna_selected = ANT_CTRL_1;
		}
		break;

		/* Disable antenna Diversity request & Enable ANT2 */
		case ENABLE_ANTENNA_2:
		{
			/* Update the data base with these values*/
			curr_trx_config_params.antenna_diversity = false;
			curr_trx_config_params.antenna_selected = ANT_CTRL_2;
		}
		break;

		default:
		{
			/* Send Set confirmation with status VALUE_OUT_OF_RANGE
			**/
			usr_perf_set_confirm(VALUE_OUT_OF_RANGE,
					PARAM_ANT_DIVERSITY,
					(param_value_t *)&config_value);
			return;
		}
		}
		tal_ant_div_config(ANT_DIVERSITY_DISABLE, config_value);

		/* Restore the current state back again */
		if (RX_ON == curr_state) {
			set_trx_state(CMD_RX_ON);
		} else if (RX_AACK_ON == curr_state) {
			set_trx_state(CMD_RX_AACK_ON);
		} else {
			/* No need to to anything */
		}
	}

	/* Send Set confirmation with status SUCCESS */
	usr_perf_set_confirm(MAC_SUCCESS,
			PARAM_ANT_DIVERSITY,
			(param_value_t *)&config_value);
}

/**
 * \brief To configure antenna diversity on peer node
 * \param config_value  Value used to configure Antenna diversity
 */
static void config_antenna_diversity_peer_node(uint8_t config_value)
{
	/* This cmd is valid only if the peer device is present */
	if (true == peer_found) {
		div_set_req_t div_msg;
		switch (config_value) {
		/* Enable Antenna diversity on peer node request */
		case ENABLE_ANT_DIVERSITY:
		{
			div_msg.status = ANT_DIV_ENABLE;
			/* No antenna selected */
			div_msg.ant_sel = ANT_CTRL_0;
		}
		break;

		/* Disable Antenna diversity on peer node request & select
		 * ANT1*/
		case ENABLE_ANTENNA_1:
		{
			div_msg.status = ANT_DIV_DISABLE;
			div_msg.ant_sel = ANT_CTRL_1;
		}
		break;

		/* Disable Antenna diversity on peer node request & select
		 * ANT2*/
		case ENABLE_ANTENNA_2:
		{
			div_msg.status = ANT_DIV_DISABLE;
			div_msg.ant_sel = ANT_CTRL_2;
		}
		break;

		default:
		{
			op_mode = TX_OP_MODE;
			/* Send Confirmation with VALUE_OUT_OF_RANGE */
			usr_perf_set_confirm(VALUE_OUT_OF_RANGE,
					PARAM_ANT_DIVERSITY_ON_PEER,
					(param_value_t *)&config_value);
			return;
		}
		}

		/* Diversity set request sent successfully  */
		if (send_diversity_set_req(div_msg)) {
			op_mode = DIVERSITY_SET_REQ;
		} else {
			/* Send confirmation with TRANSMISSION_FAILURE */
			usr_perf_set_confirm(TRANSMISSION_FAILURE,
					PARAM_ANT_DIVERSITY_ON_PEER,
					(param_value_t *)&config_value);
			op_mode = TX_OP_MODE;
		}
	} else {
		/* The node is operating in the SINGLE NODE TESTS mode*/
		usr_perf_set_confirm(INVALID_CMD,
				PARAM_ANT_DIVERSITY_ON_PEER,
				(param_value_t *)&config_value);
	}
}

#endif /* End of #if (ANTENNA_DIVERSITY == 1) */

#ifdef CRC_SETTING_ON_REMOTE_NODE

/**
 * \brief To configure CRC settings on peer node
 * \param config_value Value used to configure CRC on peer
 */
static void config_crc_peer_node(bool config_value)
{
	param_value_t param_value;
	/* This cmd is valid only if the peer device is present */
	if (true == peer_found) {
		crc_set_req_t crc_msg;

		crc_msg.status = config_value;

		/* CRC set request sent successfully */
		if (send_crc_set_req(crc_msg)) {
			op_mode = CRC_SET_REQ_WAIT;
		} else {
			/* Send confirmation with TRANSMISSION_FAILURE */
			param_value.param_value_bool = config_value;
			usr_perf_set_confirm(TRANSMISSION_FAILURE,
					PARAM_CRC_ON_PEER,
					&param_value);
			op_mode = TX_OP_MODE;
		}
	} else { /* The node is operating in the SINGLE NODE TESTS mode*/
		 /* Send the confirmation with status INVALID_CMD */
		param_value.param_value_bool = config_value;
		usr_perf_set_confirm(INVALID_CMD,
				PARAM_CRC_ON_PEER,
				&param_value);
	}
}

#endif

#if (TAL_TYPE != AT86RF230B)

/**
 * \brief To configure rx desensitization level in the transceiver.
 * \param config_value Value used to configure Rx desensitization
 */
static void config_rx_desensitization(bool config_value)
{
	param_value_t param_value;
	if (false == config_value) {
		curr_trx_config_params.rx_desensitize = false;
		tal_set_rx_sensitivity_level(NO_RX_DESENSITIZE_LEVEL);
	}
	/* Receiver desensitization is disabled, enable it */
	else {
		curr_trx_config_params.rx_desensitize = true;
		tal_set_rx_sensitivity_level(RX_DESENSITIZE_LEVEL);
	}

	/* Send confirmation with status MAC_SUCCESS */
	param_value.param_value_bool = curr_trx_config_params.rx_desensitize;
	usr_perf_set_confirm(MAC_SUCCESS,
			PARAM_DESENSITIZATION,
			&param_value);
}

#endif /* (TAL_TYPE != AT86RF230B) */

/*
 * \brief Function to send Identify command to peer node
 */
void identify_peer_node(void)
{
	/* Send the Identify command to the peer node */
	if (send_identify_command()) {
		op_mode = IDENTIFY_PEER;
	} else {
		op_mode = TX_OP_MODE;

		/* Send confirmation with status as TRANSMISSION_FAILURE */
		usr_identify_peer_node_confirm(TRANSMISSION_FAILURE);
	}
}

/*
 * \brief Function to request the peer to get disconnected from source node
 */
void disconnect_peer_node(void)
{
	/* check whether the disconnection requested when peer is present */
	if (true == node_info.peer_found) {
		/* Send Disconnect command to the peer node */
		if (send_disconnect_command()) {
			op_mode = DISCONNECT_PEER;
		} else {
			op_mode = TX_OP_MODE;

			/* Send confirmation with status as TRANSMISSION_FAILURE
			**/
			usr_peer_disconnect_confirm(TRANSMISSION_FAILURE);
		}
	} else { /* non PER mode case */
		usr_peer_disconnect_confirm(MAC_SUCCESS);

		/* app reset on source node in single node tests mode */

		/* This is to make the node to restart as boot up and
		 * open for fresh peer search
		 */
		app_reset();
	}
}

/*
 * \brief To Initiate the PER test
 */
void initiate_per_test(void)
{
	if (TX_OP_MODE == op_mode) {
		/* Initiate a packet to tell the receptor that a new PER test is
		 * going to be started */
		if (send_per_test_start_cmd()) {
			op_mode = PER_TEST_START;
		}
	}
}

/*
 * \brief To Initiate the Range  test
 */
void initiate_range_test(void)
{
	if (TX_OP_MODE == op_mode) {
		/* Initiate a packet to tell the receptor that a new Range test
		 * is going to be started */
		if (send_range_test_start_cmd()) {
			op_mode = RANGE_TEST_START;
		}
	} else {
		/* Send the confirmation with the status as INVALID_CMD
		 * as the state is not correct
		 */
		usr_range_test_start_confirm(INVALID_CMD);
	}
}

/*
 * \brief To start the PER test
 */
static void start_test(void)
{
	/* Check for the current operating mode */
	if (TX_OP_MODE == op_mode) {
		frames_to_transmit = curr_trx_config_params.number_test_frames;
		if (true == curr_trx_config_params.ack_request) {
			frame_no_ack = 0;
		} else {
			frame_no_ack = CCPU_ENDIAN_TO_LE32((uint32_t)(-1));
		}

		/* CSMA/CA is enabled by default in 230B */

#if (TAL_TYPE != AT86RF230B)
		if (true == curr_trx_config_params.csma_enabled) {
			frame_access_failure = 0;
		} else {
			frame_access_failure = CCPU_ENDIAN_TO_LE32(
					(uint32_t)(-1));
		}

#else
		frame_access_failure = 0;
#endif
		frame_failure = 0;
		configure_frame_sending();
		start_time = sw_timer_get_time();
		restart_time = (uint32_t)(start_time * MICRO_SEC_MULTIPLIER);
		no_of_roll_overs = 0;

		/* Send the confirmation with the status as SUCCESS */
		usr_per_test_start_confirm(MAC_SUCCESS);
	} else {
		/* Send the confirmation with the status as INVALID_CMD
		 * as the state is not correct
		 */
		usr_per_test_start_confirm(INVALID_CMD);
	}
}

/*
 * \brief Function to Start the Range test
 */
static void start_range_test(void)
{
	/* set the range_test_in_progress flag to true to indicate range test is
	 * in progress */
	range_test_in_progress = true;

	/* Send the confirmation with the status as SUCCESS */
	usr_range_test_start_confirm(MAC_SUCCESS);

	/* Change the OPMODE to Range Test TX */
	op_mode = RANGE_TEST_TX;

	/* Start a Range test timer  to start the  transmission of range test
	 * packets periodically*/
	sw_timer_start(T_APP_TIMER_RANGE,
			RANGE_TX_BEACON_START_INTERVAL,
			SW_TIMEOUT_RELATIVE,
			(FUNC_PTR)range_test_timer_handler_cb,
			NULL);
}

/*
 * \brief To stop the Range test
 */
void stop_range_test(void)
{
	/* Check for the current operating mode and send stop range test command
	 * to the receptor*/
	if ((RANGE_TEST_TX == op_mode) && (true == range_test_in_progress) &&
			send_range_test_stop_cmd()) {
		/* Change the opmode to RANGE_TEST_STOP so that once the cmd is
		 * sent succesfully
		 * the mode can be changed back to TX_OP_MODE and flags can be
		 * cleared and send confirmation to Host*/
		op_mode = RANGE_TEST_STOP;
	} else {
		/* Send the confirmation with the status as INVALID_CMD
		 * as the state is not correct
		 */
		usr_range_test_start_confirm(INVALID_CMD);
	}
}

/**
 * \brief To Configure the frame sending
 */
static void configure_frame_sending(void)
{
	uint8_t index;
	uint16_t app_frame_length;
	uint8_t *frame_ptr;
	uint8_t *temp_frame_ptr;
	uint16_t fcf = 0;
	uint16_t temp_value;
	app_payload_t *tmp;

	/*
	 * Fill in PHY frame.
	 */

	/* Get length of current frame. */
	app_frame_length
		= (curr_trx_config_params.phy_frame_length  -
			FRAME_OVERHEAD);

	/* Set payload pointer. */
	frame_ptr = temp_frame_ptr
				= (uint8_t *)node_info.tx_frame_info +
					LARGE_BUFFER_SIZE -
					app_frame_length - FCS_LEN;
	tmp = (app_payload_t *)temp_frame_ptr;

	(tmp->cmd_id) = PER_TEST_PKT;

	temp_frame_ptr++;

	/*
	 * Assign dummy payload values.
	 * Payload is stored to the end of the buffer avoiding payload copying
	 * by TAL.
	 */
	for (index = 0; index < (app_frame_length - 1); index++) {
		/* dummy values */
		*temp_frame_ptr++ = index;
	}

	/* Source Address */
	temp_value =  tal_pib.ShortAddress;
	frame_ptr -= SHORT_ADDR_LEN;
	convert_16_bit_to_byte_array(temp_value, frame_ptr);

	/* Source PAN-Id */
#if (DST_PAN_ID == SRC_PAN_ID)
	/* No source PAN-Id included, but FCF updated. */
	fcf |= FCF_PAN_ID_COMPRESSION;
#else
	frame_ptr -= PAN_ID_LEN;
	temp_value = CCPU_ENDIAN_TO_LE16(SRC_PAN_ID);
	convert_16_bit_to_byte_array(temp_value, frame_ptr);
#endif

	/* Destination Address */
	temp_value = node_info.peer_short_addr;
	frame_ptr -= SHORT_ADDR_LEN;
	convert_16_bit_to_byte_array(temp_value, frame_ptr);

	/* Destination PAN-Id */
	temp_value = CCPU_ENDIAN_TO_LE16(DST_PAN_ID);
	frame_ptr -= PAN_ID_LEN;
	convert_16_bit_to_byte_array(temp_value, frame_ptr);

	/* Set DSN. */
	frame_ptr--;
	*frame_ptr = (uint8_t)rand();

	/* Set the FCF. */
	fcf |= FCF_FRAMETYPE_DATA | FCF_SET_SOURCE_ADDR_MODE(FCF_SHORT_ADDR) |
			FCF_SET_DEST_ADDR_MODE(FCF_SHORT_ADDR);
	if (curr_trx_config_params.ack_request) {
		fcf |= FCF_ACK_REQUEST;
	}

	frame_ptr -= FCF_LEN;
	convert_16_bit_to_byte_array(CCPU_ENDIAN_TO_LE16(fcf), frame_ptr);

	/* First element shall be length of PHY frame. */
	frame_ptr--;
	*frame_ptr = curr_trx_config_params.phy_frame_length;

	/* Finished building of frame. */
	node_info.tx_frame_info->mpdu = frame_ptr;
}

/**
 * \brief To Configure the frame sending
 */
static void configure_range_test_frame_sending(void)
{
	uint8_t index;
	uint16_t app_frame_length;
	uint8_t *frame_ptr;
	uint8_t *temp_frame_ptr;
	uint16_t fcf = 0;
	uint16_t temp_value;
	app_payload_t *tmp;
	range_tx_t *data;

	/* Increment the seq_num of the initiator node */
	seq_num_initiator++;

	/*
	 * Fill in PHY frame.
	 */

	/* Get length of current frame. */
	app_frame_length = (RANGE_TEST_PKT_LENGTH - FRAME_OVERHEAD);

	/* Set payload pointer. */
	frame_ptr = temp_frame_ptr
				= (uint8_t *)node_info.tx_frame_info +
					LARGE_BUFFER_SIZE -
					app_frame_length - FCS_LEN;

	tmp = (app_payload_t *)temp_frame_ptr;

	data = (range_tx_t *)&tmp->payload;

	(tmp->cmd_id) = RANGE_TEST_PKT;
	temp_frame_ptr++;
	(tmp->seq_num) = seq_num_initiator; /* to be incremented for every frame
	                                    **/
	temp_frame_ptr++;
	range_test_frame_cnt++;
	data->frame_count = (CCPU_ENDIAN_TO_LE32(range_test_frame_cnt));
	temp_frame_ptr += 4;
	data->ed = 0;
	temp_frame_ptr++;
	data->lqi = 0;
	temp_frame_ptr++;

	/*
	 * Assign dummy payload values.
	 * Payload is stored to the end of the buffer avoiding payload copying
	 * by TAL.
	 */
	for (index = 0; index < (app_frame_length - 8); index++) {
		*temp_frame_ptr++ = index;
	}

	/* Source Address */
	temp_value =  tal_pib.ShortAddress;
	frame_ptr -= SHORT_ADDR_LEN;
	convert_16_bit_to_byte_array(temp_value, frame_ptr);

	/* Source PAN-Id */
#if (DST_PAN_ID == SRC_PAN_ID)
	/* No source PAN-Id included, but FCF updated. */
	fcf |= FCF_PAN_ID_COMPRESSION;
#else
	frame_ptr -= PAN_ID_LEN;
	temp_value = CCPU_ENDIAN_TO_LE16(SRC_PAN_ID);
	convert_16_bit_to_byte_array(temp_value, frame_ptr);
#endif

	/* Destination Address */
	temp_value = node_info.peer_short_addr;
	frame_ptr -= SHORT_ADDR_LEN;
	convert_16_bit_to_byte_array(temp_value, frame_ptr);

	/* Destination PAN-Id */
	temp_value = CCPU_ENDIAN_TO_LE16(DST_PAN_ID);
	frame_ptr -= PAN_ID_LEN;
	convert_16_bit_to_byte_array(temp_value, frame_ptr);

	/* Set DSN. */
	frame_ptr--;
	*frame_ptr = seq_num_initiator;

	/* Set the FCF. */
	fcf |= FCF_FRAMETYPE_DATA | FCF_SET_SOURCE_ADDR_MODE(FCF_SHORT_ADDR) |
			FCF_SET_DEST_ADDR_MODE(FCF_SHORT_ADDR);
	if (curr_trx_config_params.ack_request) {
		fcf |= FCF_ACK_REQUEST;
	}

	frame_ptr -= FCF_LEN;
	convert_16_bit_to_byte_array(CCPU_ENDIAN_TO_LE16(fcf), frame_ptr);

	/* First element shall be length of PHY frame. */
	frame_ptr--;
	*frame_ptr = RANGE_TEST_PKT_LENGTH; /* to be changed */

	/* Finished building of frame. */
	node_info.tx_frame_info->mpdu = frame_ptr;
}

/**
 * \brief Function to send the parameters which has been updated to
 * the receptor node.
 * \param param  Parameters being modified i.e. Channel,channel page,etc
 * \param val    Value of the parameter being modified
 *
 */
static void send_parameters_changed(uint8_t param, uint8_t val)
{
	uint16_t payload_length;
	app_payload_t msg;
	set_parm_req_t *data;

	/* maintain a copy of the parameter to be used to set on this node */
	set_param_cb.param_type = param;
	set_param_cb.param_value = val;

	/* Create the payload */
	msg.cmd_id = SET_PARAM;
	seq_num_initiator++;
	msg.seq_num = seq_num_initiator;
	data = (set_parm_req_t *)&msg.payload;
	data->param_type = param;
	data->param_value = val;

	payload_length = ((sizeof(app_payload_t) -
			sizeof(general_pkt_t)) +
			sizeof(set_parm_req_t));

	/* Send the frame to Peer node */
	if (MAC_SUCCESS == transmit_frame(FCF_SHORT_ADDR,
			(uint8_t *)&(node_info.peer_short_addr),
			FCF_SHORT_ADDR,
			seq_num_initiator,
			(uint8_t *)&msg,
			payload_length,
			true)
			) {
		op_mode = SET_PARAMETER;
	}
}

static bool send_per_test_start_cmd(void)
{
	uint16_t payload_length;
	app_payload_t msg;
	result_req_t *data;

	/* Create the payload */
	msg.cmd_id = PER_TEST_START_PKT;
	seq_num_initiator++;
	msg.seq_num = seq_num_initiator;
	data = (result_req_t *)&msg.payload;
	/* Just a dummy value */
	data->cmd = DUMMY_PAYLOAD;

	payload_length = ((sizeof(app_payload_t) -
			sizeof(general_pkt_t)) +
			sizeof(result_req_t));

	/* Send the frame to Peer node */
	if (MAC_SUCCESS == transmit_frame(FCF_SHORT_ADDR,
			(uint8_t *)&(node_info.peer_short_addr),
			FCF_SHORT_ADDR,
			seq_num_initiator,
			(uint8_t *)&msg,
			payload_length,
			true)
			) {
		return(true);
	}

	return(false);
}

/**
 * \brief Function to send the range test start command to the receptor to start
 * the mode in the receptor
 */
static bool send_range_test_start_cmd(void)
{
	uint16_t payload_length;
	app_payload_t msg;
	result_req_t *data;

	/* Create the payload */
	msg.cmd_id = RANGE_TEST_START_PKT;
	seq_num_initiator++;
	msg.seq_num = seq_num_initiator;
	data = (result_req_t *)&msg.payload;
	/* Just a dummy value */
	data->cmd = DUMMY_PAYLOAD;

	payload_length = ((sizeof(app_payload_t) -
			sizeof(general_pkt_t)) +
			sizeof(result_req_t));

	/* Send the frame to Peer node */
	if (MAC_SUCCESS == transmit_frame(FCF_SHORT_ADDR,
			(uint8_t *)&(node_info.peer_short_addr),
			FCF_SHORT_ADDR,
			seq_num_initiator,
			(uint8_t *)&msg,
			payload_length,
			true)
			) {
		return(true);
	}

	return(false);
}

/**
 * \brief Function to send the range test stop command to the receptor to stop
 * the mode in the receptor
 */
static bool send_range_test_stop_cmd(void)
{
	uint16_t payload_length;
	app_payload_t msg;
	result_req_t *data;

	/* Create the payload */
	msg.cmd_id = RANGE_TEST_STOP_PKT;
	seq_num_initiator++;
	msg.seq_num = seq_num_initiator;
	data = (result_req_t *)&msg.payload;
	/* Just a dummy value */
	data->cmd = DUMMY_PAYLOAD;

	payload_length = ((sizeof(app_payload_t) -
			sizeof(general_pkt_t)) +
			sizeof(result_req_t));

	/* Send the frame to Peer node */
	if (MAC_SUCCESS == transmit_frame(FCF_SHORT_ADDR,
			(uint8_t *)&(node_info.peer_short_addr),
			FCF_SHORT_ADDR,
			seq_num_initiator,
			(uint8_t *)&msg,
			payload_length,
			true)
			) {
		return(true);
	}

	return(false);
}

/**
 * \brief Function to send the response packet for the marker sent from the
 * receptor
 */
static bool send_range_test_marker_rsp(void)
{
	static uint8_t marker_seq_num;
	uint16_t payload_length;
	app_payload_t msg;
	result_req_t *data;

	/* Create the payload */
	msg.cmd_id = RANGE_TEST_MARKER_RSP;
	seq_num_initiator++;
	msg.seq_num = marker_seq_num++;
	data = (result_req_t *)&msg.payload;
	/* Just a dummy value */
	data->cmd = DUMMY_PAYLOAD;

	payload_length = ((sizeof(app_payload_t) -
			sizeof(general_pkt_t)) +
			sizeof(result_req_t));

	/* Send the frame to Peer node */
	if (MAC_SUCCESS == transmit_frame(FCF_SHORT_ADDR,
			(uint8_t *)&(node_info.peer_short_addr),
			FCF_SHORT_ADDR,
			seq_num_initiator,
			(uint8_t *)&msg,
			payload_length,
			true)
			) {
		sw_timer_start(APP_TIMER_TO_TX,
				LED_BLINK_RATE_IN_MICRO_SEC,
				SW_TIMEOUT_RELATIVE,
				(FUNC_PTR)marker_tx_timer_handler_cb,
				NULL);
		return(true);
	}

	return(false);
}

/**
 * \brief Function used to request PER test result.
 * \return true if request was sent successfully, false if not.
 */
static bool send_result_req(void)
{
	uint16_t payload_length;
	app_payload_t msg;
	result_req_t *data;

	/* Create the payload */
	msg.cmd_id = RESULT_REQ;
	seq_num_initiator++;
	msg.seq_num = seq_num_initiator;
	data = (result_req_t *)&msg.payload;
	/* Just a dummy value */
	data->cmd = DUMMY_PAYLOAD;

	payload_length = ((sizeof(app_payload_t) -
			sizeof(general_pkt_t)) +
			sizeof(result_req_t));

	/* Send the frame to Peer node */
	if (MAC_SUCCESS == transmit_frame(FCF_SHORT_ADDR,
			(uint8_t *)&(node_info.peer_short_addr),
			FCF_SHORT_ADDR,
			seq_num_initiator,
			(uint8_t *)&msg,
			payload_length,
			true)
			) {
		return(true);
	}

	return(false);
}

#if (ANTENNA_DIVERSITY == 1)

/**
 * \brief Function used to query diversity settings in remote node
 */
static void get_diversity_settings_peer_node(void)
{
	/* Send Diversity settings query request to remote node*/
	if (send_diversity_status_req()) {
		/* start the timer to wait for the reply*/
		sw_timer_start(APP_TIMER_TO_TX,
				TIMEOUT_FOR_RESPONSE_IN_MICRO_SEC,
				SW_TIMEOUT_RELATIVE,
				(FUNC_PTR)wait_for_reply_timer_handler_cb,
				NULL);
		op_mode = DIVERSITY_STATUS_REQ;
	} else {
		op_mode = TX_OP_MODE;
	}
}

/**
 * \brief Function used to send the diversity status request to get the
 * diversity settings on peer node
 *
 *  \return    Transmission status - success/failure
 */
static bool send_diversity_status_req(void)
{
	uint16_t payload_length;
	app_payload_t msg;
	div_stat_req_t *data;

	/* Create the payload */
	msg.cmd_id = DIV_STAT_REQ;
	seq_num_initiator++;
	msg.seq_num = seq_num_initiator;
	data = (div_stat_req_t *)&msg.payload;
	/* Dummy value */
	data->status = DUMMY_PAYLOAD;

	payload_length = ((sizeof(app_payload_t) -
			sizeof(general_pkt_t)) +
			sizeof(div_stat_req_t));

	/* Send the frame to Peer node */
	if (MAC_SUCCESS == transmit_frame(FCF_SHORT_ADDR,
			(uint8_t *)&(node_info.peer_short_addr),
			FCF_SHORT_ADDR,
			seq_num_initiator,
			(uint8_t *)&msg,
			payload_length,
			true)
			) {
		return(true);
	}

	return (false);
}

/**
 * \brief Function used to change diversity settings on remote node
 *
 * \param div_msg   Diversity settings for the remote node
 *
 * \return          Trasmission status - success/failure
 */
static bool send_diversity_set_req(div_set_req_t div_msg)
{
	uint16_t payload_length;
	app_payload_t msg;

	/* Create the payload */
	msg.cmd_id = DIV_SET_REQ;
	seq_num_initiator++;
	msg.seq_num = seq_num_initiator;
	msg.payload.div_set_req_data = div_msg;

	payload_length = ((sizeof(app_payload_t) -
			sizeof(general_pkt_t)) +
			sizeof(div_set_req_t));

	/* Send the frame to Peer node */
	if (MAC_SUCCESS == transmit_frame(FCF_SHORT_ADDR,
			(uint8_t *)&(node_info.peer_short_addr),
			FCF_SHORT_ADDR,
			seq_num_initiator,
			(uint8_t *)&msg,
			payload_length,
			true)
			) {
		return(true);
	}

	return(false);
}

#endif /* #if (ANTENNA_DIVERSITY == 1) */

#ifdef CRC_SETTING_ON_REMOTE_NODE

/**
 * \brief Function used to query CRC settings in remote node
 */
static void get_crc_settings_peer_node(void)
{
	if (send_crc_status_req()) {
		sw_timer_start(APP_TIMER_TO_TX,
				TIMEOUT_FOR_RESPONSE_IN_MICRO_SEC,
				SW_TIMEOUT_RELATIVE,
				(FUNC_PTR)wait_for_reply_timer_handler_cb,
				NULL);
		op_mode = CRC_STATUS_REQ_WAIT;
	} else {
		op_mode = TX_OP_MODE;
	}
}

/**
 * \brief Function used to send crc status request to get
 * CRC settings on the peer node
 * \return    Transmission status - success/failure
 */
static bool send_crc_status_req(void)
{
	uint16_t payload_length;
	app_payload_t msg;
	crc_stat_req_t *data;

	/* Create the payload */
	msg.cmd_id = CRC_STAT_REQ;
	seq_num_initiator++;
	msg.seq_num = seq_num_initiator;
	data = (crc_stat_req_t *)&msg.payload;

	/* Dummy value */
	data->status = DUMMY_PAYLOAD;

	payload_length = ((sizeof(app_payload_t) -
			sizeof(general_pkt_t)) +
			sizeof(div_stat_req_t));

	/* Send the frame to Peer node */
	if (MAC_SUCCESS == transmit_frame(FCF_SHORT_ADDR,
			(uint8_t *)&(node_info.peer_short_addr),
			FCF_SHORT_ADDR,
			seq_num_initiator,
			(uint8_t *)&msg,
			payload_length,
			true)
			) {
		return(true);
	}

	return(false);
}

/**
 * \brief Function used to change CRC settings on peer node
 *
 * \param crc_msg   CRC settings for peer node
 *
 * \return    Transmission status - success/failure
 */
static bool send_crc_set_req(crc_set_req_t crc_msg)
{
	uint16_t payload_length;
	app_payload_t msg;

	/* Create the payload */
	msg.cmd_id = CRC_SET_REQ;
	seq_num_initiator++;
	msg.seq_num = seq_num_initiator;
	msg.payload.crc_set_req_data = crc_msg;

	payload_length = ((sizeof(app_payload_t) -
			sizeof(general_pkt_t)) +
			sizeof(crc_set_req_t));

	/* Send the frame to Peer node */
	if (MAC_SUCCESS == transmit_frame(FCF_SHORT_ADDR,
			(uint8_t *)&(node_info.peer_short_addr),
			FCF_SHORT_ADDR,
			seq_num_initiator,
			(uint8_t *)&msg,
			payload_length,
			true)
			) {
		return(true);
	}

	return(false);
}

#endif /* #ifdef CRC_SETTING_ON_REMOTE_NODE */

/**
 * \brief Function used to send identify peer node command
 *
 * \return    Transmission status - success/failure
 */
static bool send_identify_command(void)
{
	uint16_t payload_length;
	app_payload_t msg;

	/* Create the payload */
	msg.cmd_id = IDENTIFY_NODE;
	seq_num_initiator++;
	msg.seq_num = seq_num_initiator;

	payload_length = ((sizeof(app_payload_t) -
			sizeof(general_pkt_t)));

	/* Send the frame to Peer node */
	if (MAC_SUCCESS == transmit_frame(FCF_SHORT_ADDR,
			(uint8_t *)&(node_info.peer_short_addr),
			FCF_SHORT_ADDR,
			seq_num_initiator,
			(uint8_t *)&msg,
			payload_length,
			true)
			) {
		return(true);
	}

	return(false);
}

/**
 * \brief Function used to send disconnect peer node command
 *
 * \return    Transmission status - success/failure
 */
static bool send_disconnect_command(void)
{
	uint16_t payload_length;
	app_payload_t msg;

	/* Create the payload */
	msg.cmd_id = DISCONNECT_NODE;
	seq_num_initiator++;
	msg.seq_num = seq_num_initiator;

	payload_length = ((sizeof(app_payload_t) -
			sizeof(general_pkt_t)));

	/* Send the frame to Peer node */
	if (MAC_SUCCESS == transmit_frame(FCF_SHORT_ADDR,
			(uint8_t *)&(node_info.peer_short_addr),
			FCF_SHORT_ADDR,
			seq_num_initiator,
			(uint8_t *)&msg,
			payload_length,
			true)
			) {
		return(true);
	}

	return(false);
}

/**
 * \brief Function to configure and send the peer node info request.
 *
 * \return    Transmission status - success/failure
 */
static bool send_peer_info_req(void)
{
	uint16_t payload_length;
	app_payload_t msg;

	/* Create the payload */
	msg.cmd_id = PEER_INFO_REQ;
	seq_num_initiator++;
	msg.seq_num = seq_num_initiator;

	payload_length = ((sizeof(app_payload_t) -
			sizeof(general_pkt_t)));

	/* Send the frame to Peer node */
	if (MAC_SUCCESS == transmit_frame(FCF_SHORT_ADDR,
			(uint8_t *)&(node_info.peer_short_addr),
			FCF_SHORT_ADDR,
			seq_num_initiator,
			(uint8_t *)&msg,
			payload_length,
			true)
			) {
		return(true);
	}

	return(false);
}

/**
 * \brief Function to send the command to Remote node to perform remote test
 *
 * \param  serial_buf Pointer to the serial buffer
 * \param  len        Length of the message
 * \param  ack_req    specifies ack requested for frame if set to 1
 */

void send_remote_cmd(uint8_t *serial_buf, uint8_t len, bool ack_req)
{
	uint16_t payload_length;
	app_payload_t msg;

	/* Create the payload */
	msg.cmd_id = REMOTE_TEST_CMD;
	seq_num_initiator++;
	msg.seq_num = seq_num_initiator;

	payload_length
		= ((sizeof(app_payload_t) - sizeof(general_pkt_t) +
			sizeof(remote_test_req_t) - REMOTE_MSG_BUF_SIZE + len +
			1));
	memcpy(&msg.payload.remote_test_req_data.remote_serial_data, serial_buf,
			len + 1);
	/* Send the frame to Peer node */
	if (MAC_SUCCESS == transmit_frame(FCF_SHORT_ADDR,
			(uint8_t *)&(node_info.peer_short_addr),
			FCF_SHORT_ADDR,
			seq_num_initiator,
			(uint8_t *)&msg,
			payload_length,
			ack_req)
			) {
		op_mode = REMOTE_TEST_MODE;
		if (!ack_req) {
			sw_timer_start(CW_TX_TIMER,
					TIMEOUT_FOR_RESPONSE_IN_MICRO_SEC,
					SW_TIMEOUT_RELATIVE,
					(FUNC_PTR)wait_for_reply_timer_handler_cb,
					&msg.payload.remote_test_req_data.remote_serial_data);
		}
	} else {
		op_mode = TX_OP_MODE;
	}
}

/**
 * \brief Function used to send set default config peer command
 *
 * \return    Transmission status - success/failure
 */
static bool send_set_default_config_command(void)
{
	uint16_t payload_length;
	app_payload_t msg;

	/* Create the payload */
	msg.cmd_id = SET_DEFAULT_REQ;
	seq_num_initiator++;
	msg.seq_num = seq_num_initiator;

	payload_length = ((sizeof(app_payload_t) -
			sizeof(general_pkt_t)));

	/* Send the frame to Peer node */
	if (MAC_SUCCESS == transmit_frame(FCF_SHORT_ADDR,
			(uint8_t *)&(node_info.peer_short_addr),
			FCF_SHORT_ADDR,
			seq_num_initiator,
			(uint8_t *)&msg,
			payload_length,
			true)
			) {
		return(true);
	}

	return(false);
}

/**
 * \brief Function to Calculate the time taken for
 * PER Test
 *
 * \return Time duration as float value
 */
static float calculate_time_duration(void)
{
	uint32_t duration;
	float duration_s;
	uint64_t ll_temp = 1;

	if (0 == no_of_roll_overs) {
		duration = (SUB_TIME(end_time, start_time));
		duration_s  = (float)duration * MICRO_SEC_MULTIPLIER;
	} else {
		if (end_time >= start_time) {
			uint64_t total_duration
				= (((no_of_roll_overs) *
					((ll_temp) << 32)) +
					((end_time) - (start_time)) +
					(no_of_roll_overs * (1000000)));

			duration_s  = (float)total_duration *
					MICRO_SEC_MULTIPLIER;
		} else {
			uint64_t total_duration
				= (((no_of_roll_overs) *
					((ll_temp) << 32))  +
					((((ll_temp) <<
					32) - start_time) + (end_time))
					+ (no_of_roll_overs * (1000000)));

			duration_s  = (float)total_duration *
					MICRO_SEC_MULTIPLIER;
		}
	}

	return(duration_s);
}

/**
 * \brief Function to calculate net data rate for the current PER test.
 *
 * \param per_test_duration_sec   Time duration for PER test
 * \return net data rate as float value
 */
static float calculate_net_data_rate(float per_test_duration_sec)
{
	float data_volume;
	float data_rate;

	/* Data volume i.e total no.of bits transmitted */
	data_volume = curr_trx_config_params.phy_frame_length *
			curr_trx_config_params.number_test_frames * 8;
	/* Net data rate in Kbps*/
	data_rate = (data_volume / per_test_duration_sec) / 1000;

	return data_rate;
}

/**
 * \brief Function  to check the error conditions before
 *
 * processing the received command
 *
 * \return the error code based on the currently ongoing operation,if any
 */
uint8_t check_error_conditions(void)
{
	uint8_t error_code;

	/* Check whether transceiver is in sleep */
	if (true == trx_sleep_status) {
		error_code = TRANSCEIVER_IN_SLEEP;
	} else if (true == scanning) {
		/* Check whether ED scan is in progress */

		error_code = ED_SCAN_UNDER_PROGRESS;
	} else if (CONTINUOUS_TX_MODE == op_mode) {
		/* Check CW transmission is going on */
		error_code = CW_TRANSMISSION_UNDER_PROGRESS;
	} else if (pkt_stream_stop == false) {
		error_code = PKT_STREAM_IN_PROGRESS;
	} else if (frames_to_transmit > 0) {
		/* Check currently Transmission is initiated */

		error_code = TRANSMISSION_UNDER_PROGRESS;
	} else if (true == range_test_in_progress) {
		/* Check if Range Mode is  initiated */
		error_code = RANGE_TEST_IN_PROGRESS;
	} else if (true == remote_serial_tx_failure) {
		error_code = UNABLE_TO_CONTACT_PEER;
	} else if (true == rx_on_mode) {
		error_code = RX_ON_MODE_IN_PROGRESS;
	} else {
		error_code = MAC_SUCCESS;
	}

	return error_code;
}

#if ((TAL_TYPE == AT86RF233) || (TAL_TYPE == ATMEGARFR2))

/**
 * \brief toggle RPC mode request
 * \param config_value  Value to be configured for RPC
 */
static void config_rpc_mode(bool config_value)
{
	param_value_t param_value;
	/* if RPC feature is to be disabled */
	if (false == config_value) {
		curr_trx_config_params.rpc_enable = false;
		/* Disable RPC feature configuration. */
		tal_rpc_mode_config(DISABLE_ALL_RPC_MODES);
		/* Set the the default configuration for TRX_CTRL_2*/
		tal_trx_reg_write(RG_TRX_CTRL_2, ENABLE_RX_SAFE_MODE);

#if (ANTENNA_DIVERSITY == 1)
		/*  Enable antenna diversity */
		tal_ant_div_config(ANT_DIVERSITY_ENABLE, ANTENNA_DEFAULT);

		/* Update this changes in the data base */
		curr_trx_config_params.antenna_diversity = true;
		curr_trx_config_params.antenna_selected = ANT_CTRL_0;
#endif /* End of #if (ANTENNA_DIVERSITY == 1) */
	} else { /* if RPC feature is to be enabled */
		uint8_t curr_state = tal_get_trx_status();

		curr_trx_config_params.rpc_enable = true;

		/*
		 *  To avoid delay in transition during RPC enabled and disabled
		 *  continuously when trx is in RX_ON or RX_AACK_ON. Added this
		 * code
		 *  to set the trx state to TRX_OFF before enabling RPC and set
		 * back
		 * to RX_ON or RX_AACK_ON state after
		 */
		if ((RX_ON == curr_state) || (RX_AACK_ON == curr_state)) {
			set_trx_state(CMD_TRX_OFF);
		}

		tal_rpc_mode_config(ENABLE_ALL_RPC_MODES); /* RPC feature
		                                            * configuration. */
#if (ANTENNA_DIVERSITY == 1)
		/*  Disable antenna diversity & Enable A1/X2 */
		tal_ant_div_config(ANT_DIVERSITY_DISABLE, ANT_CTRL_1);

		/* Update this changes in the data base */
		curr_trx_config_params.antenna_diversity = false;
		curr_trx_config_params.antenna_selected = ANT_CTRL_1;
#endif /* End of #if (ANTENNA_DIVERSITY == 1) */
		/* Reset RX_SAFE Mode in TRX_CTRL_2 */
		tal_trx_reg_write(RG_TRX_CTRL_2, DISABLE_RX_SAFE_MODE);

		/* Restore the current state back again */
		if (RX_ON == curr_state) {
			set_trx_state(CMD_RX_ON);
		} else if (RX_AACK_ON == curr_state) {
			set_trx_state(CMD_RX_AACK_ON);
		}
	}

	/* Send the confirmation for Set request as Success */
	param_value.param_value_bool = curr_trx_config_params.rpc_enable;
	usr_perf_set_confirm(MAC_SUCCESS,
			PARAM_RPC,
			&param_value);
}

#endif
#if ((TAL_TYPE == AT86RF233))

/**
 * \brief Function to configure the ISM frequency based on the request.
 * \param frequency   Frequency value to be configured
 */
static void config_frequency(float frequency)
{
	uint8_t cc_number, cc_band;
	param_value_t param_value;

	if ((frequency < MIN_ISM_FREQUENCY_MHZ) ||
			(frequency > MAX_ISM_FREQUENCY_MHZ)) {
		/* Send the confirmation for Set request with the status as
		 * VALUE_OUT_OF_RANGE  */
		param_value.param_value_float = frequency;
		usr_perf_set_confirm(VALUE_OUT_OF_RANGE, PARAM_ISM_FREQUENCY,
				&param_value);
		return;
	}

	/* Calculate CC_NUMBER & CC_BAND reg values based on received frequency
	**/
	if (frequency < MID_ISM_FREQUENCY_MHZ) {
		cc_band = CC_BAND_8;
		cc_number = (uint8_t)((frequency - BASE_ISM_FREQUENCY_MHZ) * 2);
	} else {
		cc_band = CC_BAND_9;
		cc_number = (uint8_t)((frequency - MID_ISM_FREQUENCY_MHZ) * 2);
	}

	/* In case the devices are connected */
	if (true == peer_found) {
		/* Send the CC_NUMBER and CC_BAND values to the peer node */
		if (CC_BAND_8 == cc_band) {
			send_parameters_changed(FREQ_BAND_08, cc_number);
		} else if (CC_BAND_9 == cc_band) {
			send_parameters_changed(FREQ_BAND_09, cc_number);
		}
	} else {
		/* write the CC_BAND,CC_NUMBER registers as transceiver is in
		 * TRX_OFF */
		trx_bit_write(SR_CC_BAND, cc_band);
		trx_bit_write(SR_CC_NUMBER, cc_number);
		tal_calculate_frequency(cc_band, cc_number,
				&(curr_trx_config_params.ism_frequency));

		/* As the transceiver is set to work in the non compliant ISM
		 * frequencies set
		 * the channel as INVALID
		 */
		curr_trx_config_params.channel = INVALID_VALUE;

		/* Send the confirmation for Set request with the status as
		 * SUCCESS */
		param_value.param_value_float = frequency;
		usr_perf_set_confirm(MAC_SUCCESS,
				PARAM_ISM_FREQUENCY,
				&param_value);
	}
}

#endif /* End of #if(TAL_TYPE == AT86RF233) */

/*
 * \brief get the parameter length based on the parameter type
 *
 * \param param_type    parameter type
 */
uint8_t get_param_length(uint8_t parameter_type)
{
	return (uint8_t)PGM_READ_BYTE(&perf_config_param_size[parameter_type]);
}

#if ((TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B))

/**
 * \brief validating the tx power input based on the current
 * channel and page configuration
 *
 * \param dbm_value   Tx power in dBm as input to be validated
 */
static bool validate_tx_power(int8_t dbm_value)
{
#if (TAL_TYPE == AT86RF212)
	uint8_t ch_page;
	tal_pib_get(phyCurrentPage, &ch_page);
	/* Check for MIN Tx power for any case */
	if (dbm_value < -11) {
		return(false);
	} else {
		switch (ch_page) {
		/*  If BPSK */
		case 0:
		{
			if (0 == tal_pib.CurrentChannel) {
				if (dbm_value > MAX_TX_PWR_BPSK_20) { /*MAX_TX_PWR_BPSK_20*/
					return (false);
				}
			} else { /* channels 1-10 */
				if (dbm_value > MAX_TX_PWR) { /* MAX_TX_PWR */
					return (false);
				}
			}
		}
		break;

		case 2: /* O-QPSK */
		{
			if (0 == tal_pib.CurrentChannel) {
				if (dbm_value > MAX_TX_PWR_OQPSK_100) { /*
						                         *
						                         *
						                         *MAX_TX_PWR_OQPSK_100
						                         **/
					return (false);
				}
			} else {
				/* channels 1-10 */
				if (dbm_value > MAX_TX_PWR) { /* MAX_TX_PWR */
					return (false);
				}
			}
		}
		break;

		case 5: /* China, O-QPSK */
#ifdef HIGH_DATA_RATE_SUPPORT
		case 18: /* Chinese O-QPSK 500 */
#endif
			{
				if (dbm_value > MAX_TX_PWR_CHINA) { /*
					                             *
					                             *
					                             *MAX_TX_PWR_CHINA
					                             **/
					return (false);
				}
			}
			break;

#ifdef HIGH_DATA_RATE_SUPPORT
		case 19: /* Chinese O-QPSK 1000 */

		{
			if (dbm_value > MAX_TX_PWR_CHINA_1000) { /*
					                          *
					                          *
					                          *MAX_TX_PWR_CHINA_1000
					                          **/
				return (false);
			}
		}
		break;

#endif
#ifdef HIGH_DATA_RATE_SUPPORT
		case 16: /* O-QPSK 200, 500 */
		{
			if (0 == tal_pib.CurrentChannel) {
				if (dbm_value >  MAX_TX_PWR_OQPSK_200) { /*
						                          *
						                          *
						                          *MAX_TX_PWR_OQPSK_200
						                          **/
					return (false);
				}
			} else { /* channels 1-10 */
				if (dbm_value > MAX_TX_PWR_OQPSK_500) { /*
						                         *
						                         *
						                         *MAX_TX_PWR_OQPSK_500
						                         **/
					return (false);
				}
			}
		}
		break;

		case 17: /* O-QPSK 400, 1000 */
		{
			if (0 == tal_pib.CurrentChannel) {
				if (dbm_value >  MAX_TX_PWR_OQPSK_400) { /*
						                          *
						                          *
						                          *MAX_TX_PWR_OQPSK_400
						                          **/
					return (false);
				}
			} else { /* channels 1-10 */
				if (dbm_value > MAX_TX_PWR_OQPSK_1000) { /*
						                          *
						                          *
						                          *MAX_TX_PWR_OQPSK_1000
						                          **/
					return (false);
				}
			}
		}
		break;

		default: /* Do nothing */
			break;
#endif  /* #ifdef HIGH_DATA_RATE_SUPPORT */
		}
	}

	return true;

#elif (TAL_TYPE == AT86RF212B)
	/* Check for MIN Tx power for any case */
	if (dbm_value < -25) {
		return(false);
	} else if (dbm_value > MAX_TX_PWR) {
		return (false);
	} else {
		return true;
	}

#endif /* TAL_TYPE=212B */
}

#endif

/* EOF */
