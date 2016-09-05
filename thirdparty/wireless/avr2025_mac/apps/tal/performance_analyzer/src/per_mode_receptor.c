/**
 * \file per_mode_receptor.c
 *
 * \brief Receptor functionalities in PER Measurement mode - Performance
 * Analyzer
 * application
 * Copyright (c) 2013-2016 Atmel Corporation. All rights reserved.
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
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <ctype.h>
#include "app_init.h"
#include "tal_constants.h"
#include "tal_internal.h"
#include "tal_helper.h"
#include "ieee_const.h"
#include "app_frame_format.h"
#include "app_per_mode.h"
#include "conf_board.h"
#include "perf_api.h"
#if (LED_COUNT > 0)
#if !(SAMD || SAMR21 || SAML21 || SAMR30)
#include "led.h"
#endif
#endif

/**
 * \addtogroup group_per_mode_receptor
 * \{
 */

/* === TYPES =============================================================== */

/*=====EXTERBNALS============================================================*/

/* === MACROS ============================================================== */

/* LED Blink count for Marker command */

#define LED_BLINK_COUNT_FOR_BUTTON_PRESS      (10)

/* At every LED_TOGGLE_COUNT_FOR_PER count the Led toggles when PER test
 * is in progress */
#define LED_BLINK_RATE_IN_MICRO_SEC           (50000)
#define DELAY_BEFORE_APP_RESET_IN_MICRO_SEC   (5000)

/* === PROTOTYPES ========================================================== */
static void app_reset_cb(void *parameter);
static void send_result_rsp(void);
static void send_peer_info_rsp(void);
static void set_paramter_on_recptor_node(app_payload_t *msg);
static void get_node_info(peer_info_rsp_t *data);

#if (ANTENNA_DIVERSITY == 1)
static void send_diversity_status_rsp(void);

#endif /* End of ANTENNA_DIVERSITY */
static void send_range_test_rsp(uint8_t seq_num, uint32_t frame_count,
		int8_t ed, uint8_t lqi);
static bool send_range_test_marker_cmd(void);

#ifdef CRC_SETTING_ON_REMOTE_NODE
static void send_crc_status_rsp(void);
static bool crc_check_ok(frame_info_t *mac_frame_info);
static uint16_t crc_test(uint16_t crc, uint8_t data);

#endif /* End of CRC_SETTING_ON_REMOTE_NODE */
static bool range_test_in_progress = false;
/* === GLOBALS ============================================================= */

static uint32_t number_rx_frames;
static uint32_t aver_lqi;
static uint32_t aver_rssi;
static uint8_t seq_num_receptor;
#ifdef CRC_SETTING_ON_REMOTE_NODE
uint32_t frames_with_wrong_crc;
bool manual_crc = false;
#endif
static uint8_t marker_seq_num = 0;

/*Length of the Remote Command Request Received*/
static uint8_t remote_cmd_len;
/*Flag to denote whether a remote test request is received or not */
static bool remote_cmd_rx = false;
/*Pointer to the remote test request serial data payload received*/
static uint8_t *remote_cmd_ptr;

/* ! \} */

/* === IMPLEMENTATION ====================================================== */

/*
 * \brief Initialize the application in PER Measurement mode as Receptor
 *
 */
void per_mode_receptor_init(void *parameter)
{
#ifdef EXT_RF_FRONT_END_CTRL
	pib_value_t pib_value;
#endif
	/* PER TEST Receptor sequence number */
	do {
		seq_num_receptor = rand();
	} while (!seq_num_receptor);

	printf("\r\n Starting PER Measurement mode as Reflector");
	config_per_test_parameters();
#ifdef EXT_RF_FRONT_END_CTRL
	/* Enable RF front end control in PER Measurement mode*/
	trx_bit_write(SR_PA_EXT_EN, PA_EXT_ENABLE);
	/* set the TX power to default level */
	pib_value.pib_value_8bit = TAL_TRANSMIT_POWER_DEFAULT;
	tal_pib_set(phyTransmitPower, &pib_value);
#endif

	/* keep the compiler happy */
	parameter = parameter;
}

/*
 * \brief The application task when the node is in PER_TEST_RECEPTOR state
 * i.e.PER Measurement mode as Receptor
 *
 */
void per_mode_receptor_task(void)
{
	/* If a remote test request is received the received over the air
	 * payload
	 * is converted into a serial data frame which will be handled by
	 * the Serial handler as if it was received from the UI*/
	if (remote_cmd_rx) {
		convert_ota_serial_frame_rx(remote_cmd_ptr, remote_cmd_len);
		remote_cmd_rx = false;
	}

	/* For Range Test  in PER Mode the receptor has to poll for a button
	 * press to initiate marker transmission
	 * If CW Transmission or Packet Streaming is initiated then this could
	 * be aborted by pressing the push button
	 */
	if (range_test_in_progress || remote_cw_start ||
			(!pkt_stream_stop && !node_info.transmitting)) {
		static uint8_t key_press;
		/* Check for any key press */
		key_press = app_debounce_button();

		if (key_press != 0) {
			printf("\r\n\nButton Pressed...");
			if (range_test_in_progress) {
				if (send_range_test_marker_cmd()) {
					printf(
							"\r\nInitiating Marker Transmission...");

					/* Timer for LED Blink for Marker
					 * Transmission*/
					sw_timer_start(APP_TIMER_TO_TX,
							LED_BLINK_RATE_IN_MICRO_SEC,
							SW_TIMEOUT_RELATIVE,
							(FUNC_PTR)marker_tx_timer_handler_cb,
							NULL);
				}
			} else if (remote_cw_start) {
				remote_cw_start = false;
				sw_timer_stop(CW_TX_TIMER);
				stop_cw_transmission(&(cw_start_mode));
			} else {
				stop_pkt_streaming(NULL);
			}
		}
	}

	/* If the node is in packet streaming mode and ready to transmit
	 * then initiate the next frame*/
	if (!pkt_stream_stop && rdy_to_tx && !node_info.transmitting) {
		tal_tx_frame(stream_pkt, NO_CSMA_NO_IFS, false);
		node_info.transmitting = true;
		if (pkt_stream_gap_time) {
			sw_timer_start(T_APP_TIMER, pkt_stream_gap_time * 1E3,
					SW_TIMEOUT_RELATIVE,
					(FUNC_PTR)pkt_stream_gap_timer, NULL);
			rdy_to_tx = false;
		}
	}
}

/**
 * \brief Function to send the range test marker command to the initiator node
 */
static bool send_range_test_marker_cmd(void)
{
	uint16_t payload_length;
	app_payload_t msg;
	result_req_t *data;

	/* Create the payload */
	msg.cmd_id = RANGE_TEST_MARKER_CMD;
	seq_num_receptor++;
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
			seq_num_receptor,
			(uint8_t *)&msg,
			payload_length,
			true)
			) {
		return(true);
	}

	return(false);
}

/*
 * \brief Callback that is called once tx is done in PER_TEST_RECEPTOR state
 *        This call back is used when the node is operating as
 *        PER Measurement mode Receptor
 *
 * \param status    Status of the transmission procedure
 * \param frame     Pointer to the transmitted frame structure
 */

void per_mode_receptor_tx_done_cb(retval_t status, frame_info_t *frame)
{
	status = status;
	frame = frame;

	if (remote_cw_start) {
		cw_ack_sent = true;
		start_cw_transmission(cw_start_mode, cw_tmr_val);
	}

	if (remote_pulse_cw_start) {
		cw_ack_sent = true;
		pulse_cw_transmission();
	}

	/*enable rx on mode in receptor after sending rxon confirm to the
	 * initiator*/
	if (rx_on_mode) {
		set_trx_state(CMD_RX_ON);
	}

	/* Allow the next transmission to happen */
	node_info.transmitting = false;
}

/**
 * \brief This function is used to send a remote test response command back to
 *  the initiator
 */
bool send_remote_reply_cmd(uint8_t *serial_buf, uint8_t len)
{
	uint16_t payload_length;
	app_payload_t msg;

	/* Create the payload */
	msg.cmd_id = REMOTE_TEST_REPLY_CMD;
	seq_num_receptor++;
	msg.seq_num = seq_num_receptor;

	payload_length
		= ((sizeof(app_payload_t) - sizeof(general_pkt_t) +
			sizeof(remote_test_req_t) - REMOTE_MSG_BUF_SIZE + len +
			1));
	memcpy(&msg.payload.remote_test_req_data.remote_serial_data,
			serial_buf + 1, len + 1);
	/* Send the frame to Peer node */
	if (MAC_SUCCESS ==
			transmit_frame(FCF_SHORT_ADDR,
			(uint8_t *)&(node_info.peer_short_addr), FCF_SHORT_ADDR,
			seq_num_receptor,
			(uint8_t *)&msg,
			payload_length,
			true)) {
		return true;
	} else {
		return false;
	}
}

/*
 * \brief Callback that is called if data has been received by trx
 * in the PER_TEST_RECEPTOR state
 * This call back is used when the node is operating as
 * PER Measurement mode Receptor
 * \param frame Pointer to received frame
 */
void per_mode_receptor_rx_cb(frame_info_t *mac_frame_info)
{
	app_payload_t *msg;
	static uint8_t remote_cmd_seq_num;
	static uint8_t rx_count;
	uint8_t expected_frame_size;

	/* Point to the message : 1 =>size is first byte and 2=>FCS*/
	msg
		= (app_payload_t *)(mac_frame_info->mpdu + LENGTH_FIELD_LEN +
			FRAME_OVERHEAD - FCS_LEN);

	/* If the remote node is in RX_ON mode then do not accept any request
	 * unless
	 * the request is to stop the RX_ON mode*/
	if (rx_on_mode) {
		if (!((msg->cmd_id == REMOTE_TEST_CMD) &&
				(msg->payload.remote_test_req_data.
				remote_serial_data[MESSAGE_ID_POS]) ==
				(RX_ON_REQ | REMOTE_CMD_MASK))) {
			return;
		}
	}

#ifdef CRC_SETTING_ON_REMOTE_NODE
	/* If counting of wrong CRC packets is enabled on receptor node */
	if (manual_crc) {
		uint32_t my_addr_temp;
		uint16_t my_addr;
		uint16_t dest_addr;
		memcpy(&dest_addr, &mac_frame_info->mpdu[PL_POS_DST_ADDR_START],
				SHORT_ADDR_LEN);
		tal_pib_get(macShortAddress, (uint8_t *)&my_addr_temp);

		my_addr = (uint16_t)my_addr_temp;
		/* Check the destination address of the packet is my address  */
		if (dest_addr != (uint16_t)my_addr) {
			return;
		}

		/* Counting of wrong crc packets option enabled and received crc
		 * is not OK */
		if (false == crc_check_ok(mac_frame_info)) {
			if (msg->cmd_id != PER_TEST_PKT) {
				/* Don't let in any packets with wrong CRC
				 * unless it is a PER
				 * test packet */
				return;
			}

			frames_with_wrong_crc++;
			return;
		}
	}
#endif /* #ifdef CRC_SETTING_ON_REMOTE_NODE */

	switch ((msg->cmd_id)) {
	case SET_PARAM:
	{
		/* Calculate the expected frame size in case of SET_PARAM cmd */
		expected_frame_size
			= (FRAME_OVERHEAD + ((sizeof(app_payload_t) -
				sizeof(general_pkt_t)) +
				sizeof(set_parm_req_t)));
		if (*(mac_frame_info->mpdu) == expected_frame_size) {
			/* Extract and process the cmd received */
			set_paramter_on_recptor_node(msg);
		}

		break;
	}     /* case SET_PARAM */

	case PER_TEST_PKT:
	{
		static uint8_t cur_seq_no, prev_seq_no;

		/* if PER test frames received then increment number_rx_frames
		**/
		if (number_rx_frames == 0) {
			printf("\r\nReceiving..");
			aver_lqi
				+= mac_frame_info->mpdu[mac_frame_info->mpdu[0]
					+
					LQI_LEN];
			aver_rssi
				+= mac_frame_info->mpdu[mac_frame_info->mpdu[0]
					+ LQI_LEN + 1];
#ifdef CRC_SETTING_ON_REMOTE_NODE
			frames_with_wrong_crc = 0;
#endif /* #ifdef CRC_SETTING_ON_REMOTE_NODE */
			number_rx_frames++;
			/* Get the seq no. of the first packet */
			prev_seq_no = mac_frame_info->mpdu[PL_POS_SEQ_NUM];
		} else {
			cur_seq_no = mac_frame_info->mpdu[PL_POS_SEQ_NUM];
			/* Check for the duplicate packets */
			if (prev_seq_no == cur_seq_no) {
				frames_with_wrong_crc++;
			} else {
				number_rx_frames++;
				prev_seq_no = cur_seq_no;
				/* Extract LQI and  RSSI */
				aver_lqi
					+= mac_frame_info->mpdu[mac_frame_info->
						mpdu[0] + LQI_LEN];
				aver_rssi
					+= mac_frame_info->mpdu[mac_frame_info->
						mpdu[0] + LQI_LEN + 1];
			}
		}

		/* Led is toggled indicating the test in progress at the count
		 * of
		 * LED_TOGGLE_COUNT_FOR_PER
		 */
		if (rx_count >= LED_TOGGLE_COUNT_FOR_PER) {
			app_led_event(LED_EVENT_RX_FRAME);
			rx_count = 0;
		} else {
			rx_count++;
		}

		break;
	}     /* case PER_TEST_PKT */

	case RESULT_REQ:
	{
		/* Calculate the expected frame size in case of RESULT_REQ cmd
		**/
		expected_frame_size
			=  (FRAME_OVERHEAD + ((sizeof(app_payload_t) -
				sizeof(general_pkt_t)) +
				sizeof(result_req_t)));
		if (*(mac_frame_info->mpdu) == expected_frame_size) {
			if (number_rx_frames != 0) {
				aver_lqi = aver_lqi / number_rx_frames;
				aver_rssi = aver_rssi / number_rx_frames;
			}

			send_result_rsp();

			int8_t rssi_val = tal_get_rssi_base_val();
			rssi_val += aver_rssi;
			printf("\r\nNumber of received frames = %"
					PRIu32 "; average LQI = %d, average RSSI = %d dBm",
					number_rx_frames, (uint8_t)aver_lqi,
					(int8_t)rssi_val );
#ifdef CRC_SETTING_ON_REMOTE_NODE
			if (manual_crc) {
				printf(
						"\r\nNumber of received frames with wrong CRC = %" PRIu32,
						frames_with_wrong_crc);
			}
#endif /* #ifdef CRC_SETTING_ON_REMOTE_NODE */

			number_rx_frames = 0;
			aver_lqi = 0;
			aver_rssi = 0;
			rx_count = 0;
			app_led_event(LED_EVENT_PEER_SEARCH_DONE);
		}

		break;
	}     /* RESULT_REQ */

	case PER_TEST_START_PKT:
	{
#ifdef CRC_SETTING_ON_REMOTE_NODE
		frames_with_wrong_crc = 0;
#endif /* #ifdef CRC_SETTING_ON_REMOTE_NODE */
		number_rx_frames = 0;
		aver_lqi = 0;
		aver_rssi = 0;
		rx_count = 0;
	}
	break;

#if (ANTENNA_DIVERSITY == 1)
	case DIV_STAT_REQ:
	{
		/* Calculate the expected frame size in case of DIV_STAT_REQ cmd
		**/
		expected_frame_size
			= (FRAME_OVERHEAD + ((sizeof(app_payload_t) -
				sizeof(general_pkt_t)) +
				sizeof(div_stat_req_t)));
		if (*(mac_frame_info->mpdu) == expected_frame_size) {
			send_diversity_status_rsp();
		}

		break;
	}     /* DIV_STAT_REQ */

	case DIV_SET_REQ:
	{
		/* Calculate the expected frame size in case of DIV_SET_REQ cmd
		**/
		expected_frame_size
			= (FRAME_OVERHEAD + ((sizeof(app_payload_t) -
				sizeof(general_pkt_t)) +
				sizeof(div_set_req_t)));
		if (*(mac_frame_info->mpdu) == expected_frame_size) {
			/* Antenna diversity need to be enabled */
			if (msg->payload.div_set_req_data.status) {
				tal_ant_div_config(ANT_DIVERSITY_ENABLE,
						ANTENNA_DEFAULT);
				printf("\r\n Antenna diversity enabled");
			} else { /* Antenna diversity need to be disabled */
				 /* Set the selected antenna & disable the
				  * antenna diversity */
				tal_ant_div_config(ANT_DIVERSITY_DISABLE,
						msg->payload.div_set_req_data.ant_sel);
				printf(
						"\r\n Antenna diversity disabled and antenna");
				if (msg->payload.div_set_req_data.ant_sel ==
						ANT_CTRL_1) {
					printf(" selected is A1/X2");
				} else if (msg->payload.div_set_req_data.ant_sel
						== ANT_CTRL_2) {
					printf(" selected is A2/X3");
				} else {
					printf(" selected is invalid");
				}
			}
		}

		/* Update database with this values */
		curr_trx_config_params.antenna_selected
			= msg->payload.div_set_req_data.ant_sel;
		curr_trx_config_params.antenna_diversity
			= msg->payload.div_set_req_data.status;

		break;
	}     /* DIV_SET_REQ */
#endif /* #if (ANTENNA_DIVERSITY == 1) */

#ifdef CRC_SETTING_ON_REMOTE_NODE
	case CRC_STAT_REQ:
	{
		/* Calculate the expected frame size in case of CRC_STAT_REQ cmd
		**/
		expected_frame_size
			=  (FRAME_OVERHEAD + ((sizeof(app_payload_t) -
				sizeof(general_pkt_t)) +
				sizeof(crc_stat_req_t)));
		if (*(mac_frame_info->mpdu) == expected_frame_size) {
			send_crc_status_rsp();
		}

		break;
	}     /* CRC_STAT_REQ */

	case CRC_SET_REQ:
	{
		/* Calculate the expected frame size in case of CRC_SET_REQ cmd
		**/
		expected_frame_size
			= (FRAME_OVERHEAD + ((sizeof(app_payload_t) -
				sizeof(general_pkt_t)) +
				sizeof(crc_set_req_t)));
		if (*(mac_frame_info->mpdu) == expected_frame_size) {
			if (msg->payload.crc_set_req_data.status) {
				/* Enable the Promiscuous Mode */
				tal_rxaack_prom_mode_ctrl(AACK_PROM_ENABLE);

				printf(
						"\r\n Counting packets with CRC error enabled");

				manual_crc = true;
			} else {
				/* Disable the Promiscuous Mode */
				tal_rxaack_prom_mode_ctrl(AACK_PROM_DISABLE);

				printf(
						"\r\n Counting packets with CRC error disabled");

				manual_crc = false;
			}
		}

		break;
	}     /* CRC_SET_REQ */
#endif /* #ifdef CRC_SETTING_ON_REMOTE_NODE */

	case IDENTIFY_NODE:
	{
		printf(" \r\n Blinking LED: identify frame received");

		sw_timer_start(APP_TIMER_TO_TX,
				LED_BLINK_RATE_IN_MICRO_SEC,
				SW_TIMEOUT_RELATIVE,
				(FUNC_PTR)led_blinker_timer_handler_cb,
				NULL);
		break;
	}

	case DISCONNECT_NODE:
	{
		/* app reset on remote node as the disconnect peer is successful
		 * This is to make the node to restart as boot up and
		 * open for fresh peer search */
		sw_timer_start(T_APP_TIMER,
				DELAY_BEFORE_APP_RESET_IN_MICRO_SEC,
				SW_TIMEOUT_RELATIVE,
				(FUNC_PTR)app_reset_cb,
				NULL );
	}
	break;

	case SET_DEFAULT_REQ:
	{
		config_per_test_parameters();
	}
	break;

	/* The REMOTE_TEST_CMD handles the remote test requests
	 * sent from the initiator node.
	 * The received payload is extracted and sent to the serial handler*/
	case REMOTE_TEST_CMD:
	{
		if (remote_cmd_seq_num == msg->seq_num) {
			return;
		}

		remote_cmd_seq_num = msg->seq_num;

		/*Command received by the receptor to start a test ,from the
		 * initiator node*/
		remote_cmd_len
			= *(mac_frame_info->mpdu + LENGTH_FIELD_LEN +
				FRAME_OVERHEAD + CMD_ID_LEN +
				SEQ_NUM_LEN - FCS_LEN);
		remote_cmd_ptr
			= msg->payload.remote_test_req_data.remote_serial_data;
		remote_cmd_rx = true;
		break;
	}

	case RANGE_TEST_START_PKT:
	{
		/* set the flag to indicate that the receptor node is in range
		 * test mode */
		range_test_in_progress = true;
		printf("\r\nStarting Range Test in PER Mode...");
	}
	break;

	case RANGE_TEST_STOP_PKT:
	{
		/* reset the flag to indicate that the range test mode is
		 * stopped*/
		range_test_in_progress = false;
		marker_seq_num = 0;
		printf("\r\nStopping Range Test...");
		app_led_event(LED_EVENT_PEER_SEARCH_DONE);
	}
	break;

	case RANGE_TEST_PKT:
	{
		/* On reception of the range test packet calculate the ed and
		 * lqi values of
		 * the received pkt and add it as the payload of the response
		 * frame*/
		uint8_t phy_frame_len = mac_frame_info->mpdu[0];
		uint32_t frame_count;
		/* Get the frame count in correct format */
		frame_count
			= (CCPU_ENDIAN_TO_LE32(msg->payload.range_tx_data.
				frame_count));
		int8_t rssi_base_val, ed_value;
		rssi_base_val = tal_get_rssi_base_val();
		app_led_event(LED_EVENT_RX_FRAME);
		/* Map the register ed value to dbm values */
		ed_value
			= mac_frame_info->mpdu[phy_frame_len + LQI_LEN +
				ED_VAL_LEN] + rssi_base_val;

		/* Send Response cmd to the received Range Test packet with the
		 * lqi and ed values */
		send_range_test_rsp(msg->seq_num,
				msg->payload.range_tx_data.frame_count,	\
				ed_value,
				mac_frame_info->mpdu[phy_frame_len +
				LQI_LEN]);
		/* Print the received values to the terminal */
		printf(
				"\r\nRange Test Packet Received...\tFrame No : %" PRIu32 "\tLQI : %d\tED : %d",
				frame_count,
				mac_frame_info->mpdu[phy_frame_len + LQI_LEN],
				ed_value);
	}
	break;

	case RANGE_TEST_MARKER_RSP:
	{
		/* On reception of the Response frame to the Marker cmd sent ,
		 * get the lqi and ed values and print it on the terminsl */
		int8_t rssi_base_val, ed_value;
		rssi_base_val = tal_get_rssi_base_val();
		uint8_t phy_frame_len = mac_frame_info->mpdu[0];
		/* Map the register ed value to dbm values */
		ed_value
			= mac_frame_info->mpdu[phy_frame_len + LQI_LEN +
				ED_VAL_LEN] + rssi_base_val;
		printf("\r\nMarker Response Received... LQI : %d\t ED %d \n",
				mac_frame_info->mpdu[phy_frame_len + LQI_LEN],
				ed_value);
		/* Timer for LED Blink for Reception of Marker Response*/
		sw_timer_start(T_APP_TIMER,
				LED_BLINK_RATE_IN_MICRO_SEC,
				SW_TIMEOUT_RELATIVE,
				(FUNC_PTR)marker_rsp_timer_handler_cb,
				NULL);
	}
	break;

	case PEER_INFO_REQ:
	{
		send_peer_info_rsp();
	}
	break;

	default:
		break;
	}
}

/**
 * \brief Extract the received command frame and set the parameters like
 * channel, tx power etc on Receptor node
 * \param pointer to cmd payload
 */
static void set_paramter_on_recptor_node(app_payload_t *msg)
{
	uint8_t param_val;
	pib_value_t pib_value;

	switch (msg->payload.set_parm_req_data.param_type) {
	/* If Parameter = channel */
	case CHANNEL:
	{
#ifdef EXT_RF_FRONT_END_CTRL
		uint16_t chn_before_set;
		tal_pib_get(phyCurrentChannel, (uint8_t *)&chn_before_set);
#endif

		param_val = msg->payload.set_parm_req_data.param_value;

#if (TAL_TYPE == AT86RF233)
		tal_set_frequency_regs(CC_BAND_0, CC_NUMBER_0);
#endif
		pib_value.pib_value_8bit = param_val;
		/* set the channel on receptor with the received value */
		tal_pib_set(phyCurrentChannel, &pib_value);
		curr_trx_config_params.channel = (uint16_t)param_val;
		printf("\r\n Channel changed to %d", param_val);
#ifdef EXT_RF_FRONT_END_CTRL

		/* Limit the tx power below the default power for ch26 to meet
		 * FCC Compliance */
		limit_tx_power_in_ch26(param_val, (uint8_t)chn_before_set);
#endif
	}
	break;

#if (TAL_TYPE == AT86RF233)
	/* If Parameter = frequency in band 8 */
	case FREQ_BAND_08:
	{
		float frequency;
		param_val = msg->payload.set_parm_req_data.param_value;
		/* Set the transceiver ISM frequency */
		tal_set_frequency_regs(CC_BAND_8, param_val);
		tal_calculate_frequency(CC_BAND_8, param_val, &frequency);
		printf("\r\n Frequency changed to %0.1fMHz", (double)frequency);
	}
	break;

	/* If Parameter = frequency in band 9 */
	case FREQ_BAND_09:
	{
		float frequency;
		param_val = msg->payload.set_parm_req_data.param_value;
		/* Set the transceiver ISM frequency */
		tal_set_frequency_regs(CC_BAND_9, param_val);
		tal_calculate_frequency(CC_BAND_9, param_val, &frequency);
		printf("\r\n Frequency changed to %0.1fMHz", (double)frequency);
	}
	break;
#endif
	case CHANNEL_PAGE:
	{
		param_val = msg->payload.set_parm_req_data.param_value;
		pib_value.pib_value_8bit = param_val;
		retval_t status  = tal_pib_set(phyCurrentPage,
				&pib_value);

		if (status == MAC_SUCCESS) {
			curr_trx_config_params.channel_page = param_val;
			printf("\r\n Channel page changed to %d", param_val);
		} else {
			printf("\r\n Channel page change failed");
		}
	}
	break;

	/* Handle Tx power value in dBm */
	/* If parameter = Tx power in dBm */
	case TX_POWER_DBM:
	{
		uint8_t temp_var;
#if (TAL_TYPE == AT86RF233)
		uint8_t previous_RPC_value;
#endif
		/* Get the the received tx power in dBm */
		param_val = msg->payload.set_parm_req_data.param_value;
		temp_var = CONV_DBM_TO_phyTransmitPower((int8_t)param_val);

		/* If RPC enabled, disable RPC to change the TX power value
		 * refer sec 9.2.4 */
#if (TAL_TYPE == AT86RF233)
		/* Store currents RPC settings */
		tal_trx_reg_read(RG_TRX_RPC, &previous_RPC_value);

		tal_rpc_mode_config(DISABLE_ALL_RPC_MODES);
#endif
		pib_value.pib_value_8bit = temp_var;
		tal_pib_set(phyTransmitPower, &pib_value);
#if (TAL_TYPE == AT86RF233)
		/* Restore RPC settings. */
		tal_trx_reg_write(RG_TRX_RPC, previous_RPC_value);
#endif

#if ((TAL_TYPE != AT86RF212) && (TAL_TYPE != AT86RF212B))
#if (TAL_TYPE != AT86RF233)
		uint8_t tx_pwr_reg;
		tal_get_curr_trx_config(TX_PWR, &(tx_pwr_reg));
		int8_t min_dbm_val;
		int8_t max_dbm_val;
		tal_convert_reg_value_to_dBm(MIN_TX_PWR_REG_VAL, &min_dbm_val);
		tal_convert_reg_value_to_dBm(0x00, &max_dbm_val);
		if (((int8_t)param_val < min_dbm_val) ||
				((int8_t)param_val > max_dbm_val)) {
			int8_t temp_val;

			tal_convert_reg_value_to_dBm(tx_pwr_reg, &temp_val);
			printf("\r\n Tx Power value out of valid Range.");
			printf(
					"\r\n Tx Power set to %d dBm (TX_PWR=0x%x) on the node",
					(int8_t)temp_val, tx_pwr_reg);
			/* update the data base with this value*/

			/*Tx power in dBm also need to be
			 * updated as it changes with reg value
			 **/
			curr_trx_config_params.tx_power_dbm = temp_val;
		} else {
			printf(
					"\r\n Tx Power set to %d dBm (TX_PWR=0x%x) on the node",
					(int8_t)param_val, tx_pwr_reg);
			curr_trx_config_params.tx_power_dbm = param_val;
		}
		curr_trx_config_params.tx_power_reg = tx_pwr_reg;
#endif /* (TAL_TYPE != AT86RF233) */

#else               /* In case of AT86Rf212 */
		printf("\r\n Tx Power set to %d dBm on the node",
				(int8_t)param_val);
#endif
	}

	break;

#if ((TAL_TYPE != AT86RF212) && (TAL_TYPE != AT86RF212B))
	/* Handle Tx power value in dBm */
	/* If Parameter = TX power in Reg value */
	case TX_POWER_REG:
	{
		int8_t tx_pwr_dbm = 0;
#if (TAL_TYPE == AT86RF233)
		uint8_t previous_RPC_value;
#endif
		/* get the the received tx power as reg value */
		param_val = msg->payload.set_parm_req_data.param_value;
		if (MAC_SUCCESS ==
				tal_convert_reg_value_to_dBm(param_val,
				&tx_pwr_dbm)) {
			uint8_t temp_var = CONV_DBM_TO_phyTransmitPower(
					tx_pwr_dbm);

			/* If RPC enabled, disable RPC to change the TX power
			 * value refer sec 9.2.4 */
#if (TAL_TYPE == AT86RF233)
			/* Store currents RPC settings */
			tal_trx_reg_read(RG_TRX_RPC, &previous_RPC_value);

			tal_rpc_mode_config(DISABLE_ALL_RPC_MODES);
#endif
			pib_value.pib_value_8bit = temp_var;
			tal_pib_set(phyTransmitPower, &pib_value);
			tal_set_tx_pwr(REGISTER_VALUE, param_val);

#if (TAL_TYPE == AT86RF233)
			/* Restore RPC settings. */
			tal_trx_reg_write(RG_TRX_RPC, previous_RPC_value);
#endif

#if (TAL_TYPE != AT86RF233)

			/* With RPC enabled Txpower change will take effect
			 * after trx
			 * state change only, so printed TX power will not
			 * synchronize
			 * with user set values, do not print to avoid confusion
			 *  */
			printf(
					"\r\n Tx Power set to %d dBm (TX_PWR=0x%x) on the node",
					tx_pwr_dbm, param_val);
			/* update the data base with this value*/
			curr_trx_config_params.tx_power_reg = param_val;

			/*Tx power in dBm also need to be
			 * updated as it changes with reg value
			 **/
			curr_trx_config_params.tx_power_dbm = tx_pwr_dbm;
#endif /* End of (TAL_TYPE != AT86RF233) */
		}
	}
	break;
#endif /* End of (TAL_TYPE != AT86RF212) */
	default:
		printf(" \r\nUnsupported Parameter");
		break;
	}
}

/**
 * \brief Timer Callback function  if marker response command is transmitted on
 * air
 *  This is used to blink the LED and thus identify that the transmission is
 * done
 * \param parameter pass parameters to timer handler
 */
void marker_tx_timer_handler_cb(void *parameter)
{
#if (LED_COUNT > 0)
	static uint8_t led_count;
	parameter = parameter;
	/* LED Blinking sequence is completed */
	if (led_count > LED_BLINK_COUNT_FOR_BUTTON_PRESS) {
		led_count = 0;
		app_led_event(LED_EVENT_PEER_SEARCH_DONE);
	} else { /* Blink count is not completed  */
		 /* For every timeout switch off and on all LEDs alternatively
		 **/
		if (led_count & 0x01) {
			led_count++;
			LED_Off(TX_LED);
			sw_timer_stop(APP_TIMER_TO_TX);
		} else {
			led_count++;
			LED_On(TX_LED);
		}

		sw_timer_start(APP_TIMER_TO_TX,
				LED_BLINK_RATE_IN_MICRO_SEC,
				SW_TIMEOUT_RELATIVE,
				(FUNC_PTR)marker_tx_timer_handler_cb,
				NULL);
	}
#endif
	return;
}

/**
 * \brief Timer Callback function  if marker command is received on air
 * This is used to blink the LED and thus identify that the marker frame is
 * received
 * \param parameter pass parameters to timer handler
 */
void marker_rsp_timer_handler_cb(void *parameter)
{
#if (LED_COUNT > 0)
	static uint8_t led_count;
	parameter = parameter;
	/* LED Blinking sequence is completed */
	if (led_count > LED_BLINK_COUNT_FOR_BUTTON_PRESS) {
		led_count = 0;
		app_led_event(LED_EVENT_PEER_SEARCH_DONE);
	} else { /* Blink count is not completed  */
		 /* For every timeout switch off and on all LEDs alternatively
		 **/
		if (led_count & 0x01) {
			led_count++;
			LED_Off(RX_LED);
			sw_timer_stop(T_APP_TIMER);
		} else {
			led_count++;
			LED_On(RX_LED);
		}

		sw_timer_start(T_APP_TIMER,
				LED_BLINK_RATE_IN_MICRO_SEC,
				SW_TIMEOUT_RELATIVE,
				(FUNC_PTR)marker_rsp_timer_handler_cb,
				NULL);
	}
#endif
	return;
}

/**
 * \brief Timer Callback function  if disconnect command is received on air
 *  by a node in reflector configuration.This is used to do a software reset
 *  on the reflector side.
 *
 * \param parameter pass parameters to timer handler
 */
static void app_reset_cb(void *parameter)
{
	app_reset();

	printf("\r\nDisconnected from Transmitter node");

	parameter = parameter;
}

#ifdef CRC_SETTING_ON_REMOTE_NODE

/**
 * \brief Calculates CRC for a given byte of data
 */
static uint16_t crc_test(uint16_t crc, uint8_t data)
{
	data ^= crc & 0xFF;
	data ^= data << 4;
	return ((((uint16_t)data << 8) | ((crc & 0xFF00) >> 8)) ^ \
	       ((uint8_t)(data >> 4)) ^	\
	       ((uint16_t)data << 3));
}

/**
 * \brief Calculates CRC manually and compares with the received
 * and returns true if both are same,false otherwise.
 */
static bool crc_check_ok(frame_info_t *mac_frame_info)
{
	/* Calculate CRC manually since we are bypassing hardware CRC */
	uint8_t number_of_bytes_rec = (mac_frame_info->mpdu)[0];
	uint16_t cal_crc = 0;
	uint8_t *rec_crc_ptr
		= (uint8_t *)&(mac_frame_info->mpdu)[number_of_bytes_rec - 1 ];
	uint16_t rec_crc;
	memcpy((uint8_t *)&rec_crc, rec_crc_ptr, sizeof(rec_crc));
	uint8_t i;
	for (i = 1; i <= (number_of_bytes_rec - FCS_LEN); i++) {
		cal_crc = crc_test(cal_crc, (mac_frame_info->mpdu)[i]);
	}
	if (rec_crc != cal_crc) {
		return(false);
	}

	return(true);
}

#endif /* End of #ifdef CRC_SETTING_ON_REMOTE_NODE */

/**
 * \brief Function used to send PER test result.
 *
 */
static void send_result_rsp(void)
{
	uint16_t payload_length;
	app_payload_t msg;
	result_rsp_t *data;

	/* Create the payload */
	msg.cmd_id = RESULT_RSP;
	seq_num_receptor++;
	msg.seq_num = seq_num_receptor;
	data = (result_rsp_t *)&msg.payload;
	data->num_of_frames_rx = CCPU_ENDIAN_TO_LE32(number_rx_frames);
	data->lqi_avrg_rx = CCPU_ENDIAN_TO_LE32(aver_lqi);
	data->rssi_avrg_rx = CCPU_ENDIAN_TO_LE32(aver_rssi);

#ifdef CRC_SETTING_ON_REMOTE_NODE
	if (manual_crc) {
		data->frames_with_wrong_crc = CCPU_ENDIAN_TO_LE32(
				frames_with_wrong_crc);
	} else
#endif /* #ifdef CRC_SETTING_ON_REMOTE_NODE */
	{
		/* Set a value of 0xffffffff if we are not counting CRC errors*/
		data->frames_with_wrong_crc
			= CCPU_ENDIAN_TO_LE32((uint32_t)(-1));
	}

	/* Calculate the payload length */
	payload_length = ((sizeof(app_payload_t) -
			sizeof(general_pkt_t)) +
			sizeof(result_rsp_t));

	/* Send the frame to Peer node */
	transmit_frame(FCF_SHORT_ADDR,
			(uint8_t *)&(node_info.peer_short_addr),
			FCF_SHORT_ADDR,
			seq_num_receptor,
			(uint8_t *)&msg,
			payload_length,
			true);
}

/**
 * \brief Function used to send peer_info_rsp command
 *
 */
static void send_peer_info_rsp(void)
{
	uint16_t payload_length;
	app_payload_t msg;
	peer_info_rsp_t *data;

	/* Create the payload */
	msg.cmd_id = PEER_INFO_RSP;
	seq_num_receptor++;
	msg.seq_num = seq_num_receptor;
	data = (peer_info_rsp_t *)&msg.payload;
	get_node_info(data);

	payload_length = ((sizeof(app_payload_t) -
			sizeof(general_pkt_t)) +
			sizeof(peer_info_rsp_t));

	/* Send the frame to Peer node */
	transmit_frame(FCF_SHORT_ADDR,
			(uint8_t *)&(node_info.peer_short_addr),
			FCF_SHORT_ADDR,
			seq_num_receptor,
			(uint8_t *)&msg,
			payload_length,
			true);
}

/**
 * \brief Function used to send response to the received range test packet
 * \param seq_num sequence number of the range test packet received
 * \param frame_count Count of the received Range Test Packet
 * \param ed ED value of the received range test packet which has to be
 *  uploaded into the response payload
 * \param lqi LQI value of the received range test packet which has to be
 * uploaded into the response payload
 */
static void send_range_test_rsp(uint8_t seq_num, uint32_t frame_count,
		int8_t ed, uint8_t lqi)
{
	uint16_t payload_length;
	app_payload_t msg;
	range_tx_t *data;

	/* Create the payload */
	msg.cmd_id = RANGE_TEST_RSP;
	seq_num_receptor++;
	msg.seq_num = seq_num;
	data = (range_tx_t *)&msg.payload;
	data->frame_count = frame_count;
	data->ed = ed;
	data->lqi = lqi;
	app_led_event(LED_EVENT_TX_FRAME);
	payload_length = ((sizeof(app_payload_t) -
			sizeof(general_pkt_t)) +
			sizeof(range_tx_t));

	/* Send the frame to Peer node */
	transmit_frame(FCF_SHORT_ADDR,
			(uint8_t *)&(node_info.peer_short_addr),
			FCF_SHORT_ADDR,
			seq_num_receptor,
			(uint8_t *)&msg,
			payload_length,
			true);
}

/**
 * \brief Function used to get the board details of peer node
 *
 * \param pointer to the peer_info_rsp_t structure
 */
static void get_node_info(peer_info_rsp_t *data)
{
	strcpy(data->trx_name, TRANSCEIVER_NAME);
	data->ic_type = IC_TYPE;
	strcpy(data->soc_mcu_name, MCU_SOC_NAME);
	strcpy(data->board_name, BOARD_NAME);
	/* Get the MAC address of the node */
	data->mac_address = tal_pib.IeeeAddress;
	data->fw_version = reverse_float(FIRMWARE_VERSION);
	data->feature_mask
		= CCPU_ENDIAN_TO_LE32(
			(MULTI_CHANNEL_SELECT) | (PER_RANGE_TEST_MODE) |
			(PER_REMOTE_CONFIG_MODE) | (PKT_STREAMING_MODE) |
			(CONTINUOUS_RX_ON_MODE));
}

#if (ANTENNA_DIVERSITY == 1)

/**
 * \brief Function used to send diversity settings on remote node to host node
 */
static void send_diversity_status_rsp(void)
{
	uint16_t payload_length;
	app_payload_t msg;
	uint8_t temp;
	div_stat_rsp_t *data;

	/* Create the payload */
	msg.cmd_id = DIV_STAT_RSP;
	seq_num_receptor++;
	msg.seq_num = seq_num_receptor;
	data = (div_stat_rsp_t *)&msg.payload;

	uint8_t curr_ant_div;
	tal_get_curr_trx_config(ANT_DIVERSITY, &curr_ant_div);
	if (curr_ant_div) {
		data->status = 1;
		data->ant_sel = 0;
	} else {
		data->status = 0;
		tal_get_curr_trx_config(ANT_CTRL, &temp);
		data->ant_sel = temp;
	}

	/* Calculate the payload length */
	payload_length = ((sizeof(app_payload_t) -
			sizeof(general_pkt_t)) +
			sizeof(div_stat_rsp_t));
	/* Send the frame to Peer node */
	transmit_frame(FCF_SHORT_ADDR,
			(uint8_t *)&(node_info.peer_short_addr),
			FCF_SHORT_ADDR,
			seq_num_receptor,
			(uint8_t *)&msg,
			payload_length,
			true);
}

#endif /* End of (ANTENNA_DIVERSITY == 1) */

#ifdef CRC_SETTING_ON_REMOTE_NODE

/**
 * \brief Function used to send crc settings on remote node to host node
 */
static void send_crc_status_rsp(void)
{
	uint16_t payload_length;
	app_payload_t msg;
	crc_stat_rsp_t *data;

	/* Create the payload */
	msg.cmd_id = CRC_STAT_RSP;
	seq_num_receptor++;
	msg.seq_num = seq_num_receptor;
	data = (crc_stat_rsp_t *)&msg.payload;

	tal_get_curr_trx_config(AACK_PROMSCS_MODE, &(data->status));

	/* Calculate the payload length */
	payload_length = ((sizeof(app_payload_t) -
			sizeof(general_pkt_t)) +
			sizeof(crc_stat_rsp_t));

	/* Send the frame to Peer node */
	transmit_frame(FCF_SHORT_ADDR,
			(uint8_t *)&(node_info.peer_short_addr),
			FCF_SHORT_ADDR,
			seq_num_receptor,
			(uint8_t *)&msg,
			payload_length,
			true);
}

#endif /* End of #ifdef CRC_SETTING_ON_REMOTE_NODE */

/*EOF */
