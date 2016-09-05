/**
 * \file range_measure.c
 *
 * \brief Range Measurement mode functionalities - Performance Analyzer
 *  application of AT86RF215
 * This implements the range measurement mode functionality
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

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <ctype.h>
#include "tal.h"
#include "tal_helper_2.h"
#include "app_init.h"
#include "ieee_const.h"
#include "app_frame_format.h"
#include "sio2host.h"
#include "app_range_mode.h"
#include "user_interface.h"

/**
 * \addtogroup group_range_measure
 * \{
 */

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */
#define DATA_PKT                                  (0x00)

#define APP_SEND_TEST_PKT_INTERVAL_IN_MICRO_SEC   (250000)

/* === PROTOTYPES ========================================================== */
static void range_test_tx_timer_handler_cb(void *parameter);
static int range_test_frame_tx(void);

/* === GLOBALS ============================================================= */
/* Peer process seq number */
static uint8_t seq_num;

static uint32_t num_of_frames_send;
static uint32_t num_of_frames_rec;

/* ! \} */
/* === IMPLEMENTATION ====================================================== */

/* Main state tasks for Peer Search */

/*
 * \brief Initialization of Range Measurement mode in RANGE_TX_ON State
 *
 * \param arg arguments to init RANGE_TX_ON state
 */
void range_test_tx_on_init(trx_id_t trx, void *arg)
{
	print_event(RF24, PRINT_RANGE_MEASURE_TX_START);

	/* Peer process seq number */
	seq_num = rand();

	sw_timer_start(APP_TIMER_TO_TX,
			APP_SEND_TEST_PKT_INTERVAL_IN_MICRO_SEC,
			SW_TIMEOUT_RELATIVE,
			(FUNC_PTR)range_test_tx_timer_handler_cb,
			NULL);

	/* Keep compiler happy */
	arg = arg;
}

/*
 * \brief Application task handling Range Measurement when the node is in
 *
 * RANGE_TEST_TX_ON state, where both TX and RX is enabled
 */
void range_test_tx_on_task(trx_id_t trx)
{
	int char_received;
	uint8_t key_press;

	/* See if any character received on the UART */
	char_received = sio2host_getchar_nowait();

	/* Check for any key press */
	key_press = app_debounce_button();

	if (key_press != 0) {
		/* key press detected - so change to state RANGE_TEST_TX_OFF */
		set_main_state(RF24, RANGE_TEST_TX_OFF, NULL);
	} else if (char_received != -1) {
		print_event(RF24, PRINT_RANGE_MEASURE_STATSTICS);
	}
}

/*
 * \brief Callback that is called if data has been received by trx when the .
 * node is in Range Measurement mode i.e RANGE_TEST_TX_ON or RANGE_TEST_TX_OFF
 * states
 * \param frame Pointer to received frame
 */
void range_test_rx_cb(trx_id_t trx, frame_info_t *mac_frame_info)
{
	app_payload_t *msg;
	if ((mac_frame_info->len_no_crc) == (FRAME_OVERHEAD +
			((sizeof(app_payload_t) -
			sizeof(general_pkt_t)) +
			sizeof(data_pkt_range_test_t)))) {
		/* Point to the message : 1 =>size is first byte and 2=>FCS*/
		msg = (app_payload_t *)(mac_frame_info->mpdu + FRAME_OVERHEAD);
		if ((msg->cmd_id) == DATA_PKT) {
			/* Correct packet received so toggle the LED on board */
			app_led_event(LED_EVENT_RX_FRAME);
			/* Increment the frames received count */
			num_of_frames_rec++;
		}
	}
}

/*
 * \brief To exit the Application task from RANGE_TEST_TX_ON state,
 *
 * where both TX and RX is enabled
 */
void range_test_tx_on_exit(trx_id_t trx)
{
	print_event(RF24, PRINT_RANGE_MEASURE_TX_STOP);
	/* Off timer as node leaves this sub state */
	sw_timer_stop(APP_TIMER_TO_TX);
}

/*
 * \brief Application task handling Range Measurement when the node is in
 *
 * RANGE_TEST_TX_OFF state, where no TX is enabled and only RX is ON
 */
void range_test_tx_off_task(trx_id_t trx)
{
	int char_received;
	uint8_t key_press;

	/* See if any character received on the UART */
	char_received = sio2host_getchar_nowait();

	/* Check for any key press */
	key_press = app_debounce_button();

	if (key_press != 0) {
		/* key press detected - so change to state RANGE_TEST_TX_OFF */
		set_main_state(RF24, RANGE_TEST_TX_ON, 0);
	} else if (char_received != -1) {
		print_event(RF24, PRINT_RANGE_MEASURE_STATSTICS);
	}
}

/**
 * \brief Callback function that handles timer in Range Measurement Mode
 *
 * \param parameter pass parameters to timer handler
 */
static void range_test_tx_timer_handler_cb(void *parameter)
{
	/* Make compiler happy */
	parameter = parameter;

	/* Transmit range test frame on Air */
	range_test_frame_tx();

	/* Toggle the LED indicating transmission */
	app_led_event(LED_EVENT_TX_FRAME);

	/* Increment the frames transmitted counter */
	num_of_frames_send++;

	/* Restart the timer */
	sw_timer_start(APP_TIMER_TO_TX,
			APP_SEND_TEST_PKT_INTERVAL_IN_MICRO_SEC,
			SW_TIMEOUT_RELATIVE,
			(FUNC_PTR)range_test_tx_timer_handler_cb,
			NULL);
}

/**
 * \brief Send Range Measurement mode test frames to the peer device.
 *
 * This is a unicast with source address and destination addreses which were set
 * during peer search process
 */
static int range_test_frame_tx(void)
{
	uint8_t payload_length;
	app_payload_t msg;
	data_pkt_range_test_t *data;

	/* Form the payload for the test frames */
	msg.cmd_id = DATA_PKT;

	seq_num++;
	msg.seq_num = seq_num;

	data = (data_pkt_range_test_t *)&msg.payload;
	data->pkt_count = num_of_frames_send;

	payload_length = ((sizeof(app_payload_t) -
			sizeof(general_pkt_t)) +
			sizeof(data_pkt_range_test_t));

	return(app_transmit_frame(RF24, FCF_SHORT_ADDR,
	       (uint8_t *)(&node_info[RF24].peer_short_addr),
	       FCF_SHORT_ADDR,
	       seq_num,                          /* seq_num used as msdu handle
	                                         **/
	       (uint8_t *)&msg,
	       payload_length,
	       1));
}

/*
 * \brief Print the statistics for Range Measurement on the UART Terminal.
 */
void app_print_statistics(int trx)
{
	/* Print number of frames Transmitted */
	printf("\r\nNo. of Frames TX: %" PRIu32, num_of_frames_send);
	/* Print number of frames received */
	printf("\r\nNo. of Frames RX: %" PRIu32, num_of_frames_rec);
}

/* EOF */
