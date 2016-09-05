/**
 * \file wait_for_event.c
 *
 * \brief Event handling functionalities to start Peer Search - Performance
 * Analyzer application
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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

#include "app_init.h"
#include "ieee_const.h"
#include "app_frame_format.h"
#include "sio2host.h"
#include "app_peer_search.h"
/* === TYPES =============================================================== */

/* === MACROS ============================================================== */
/* ED threshold to accept a frame in the configuaration mode  */
#ifdef EXT_RF_FRONT_END_CTRL
#define CONFIG_ED_THRESHOLD             (0x10)
#else
#if ((TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B))
#define CONFIG_ED_THRESHOLD             (0x3a)
#else
#define CONFIG_ED_THRESHOLD             (0x1f)
#endif /* End of ((TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B)) */
#endif /* End of EXT_RF_FRONT_END_CTRL */

/* === PROTOTYPES ========================================================== */
static void configure_pibs(void);

/* === GLOBALS ============================================================= */

/* === IMPLEMENTATION ====================================================== */

/*
 * \brief Application task when the node is in the WAIT_FOR_EVENT state
 *
 * \param arg arguments for WAIT_FOR_EVENT state
 */
void wait_for_event_init(void *arg)
{
	/* Configure the TAL PIBs; e.g. set short address */
	configure_pibs();

	/* Leave the radio in RX mode in default channel */
	tal_rx_enable(PHY_RX_ON);

	/* indicating application is started */
	app_led_event(LED_EVENT_POWER_ON);

	/* Keep compiler happy */
	arg = arg;
}

/*
 * \brief Application task handling user events like key press or
 * character on UART
 *
 * This function
 * - Implements the event handling in WAIT_FOR_EVENT state.
 */
void wait_for_event_task(void)
{
	uint8_t key_press;

	/* Check for any key press */
	key_press = app_debounce_button();

	if (key_press != 0) {
		print_event(PRINT_KEY_PRESS_PEER_SEARCH_INITIATOR);

		/* key press detected - so change to state PEER_SEARCH_RANGE_TX
		**/
		set_main_state(PEER_SEARCH_RANGE_TX, NULL);
	}
}

/*
 * \brief Callback that is called if data has been received by trx
 * in WAIT_FOR_EVENT state. This allow the node to participate in
 * Peer Search process as receptor
 *
 * \param frame Pointer to received frame
 */
void wait_for_event_rx_cb(frame_info_t *mac_frame_info)
{
	app_payload_t *msg;
	peer_search_receptor_arg_t peer_info;
	uint8_t expected_frame_size = FRAME_OVERHEAD_SRC_IEEE_ADDR
			+ ((sizeof(app_payload_t)
			- sizeof(general_pkt_t))
			+ sizeof(peer_req_t));

	/* Frame received on air: Processing the same */
	if (*(mac_frame_info->mpdu) == expected_frame_size) {
		/* Point to the message : 1 =>size is first byte and 2=>FCS*/
		msg = (app_payload_t *)(mac_frame_info->mpdu + LENGTH_FIELD_LEN
				+ FRAME_OVERHEAD_SRC_IEEE_ADDR - FCS_LEN);

		/* Is this a peer request cmd */
		if ((msg->cmd_id) == PEER_REQUEST) {
			uint8_t frame_len = mac_frame_info->mpdu[0];
			uint8_t ed_val
				= mac_frame_info->mpdu[frame_len +
					LQI_LEN + ED_VAL_LEN];

			/* Check the threshold if the configuarion mode is
			 * enabled, not otherwise */
			if (((msg->payload.peer_req_data.config_mode == true) &&
					(ed_val > CONFIG_ED_THRESHOLD)) ||
					(msg->payload.peer_req_data.config_mode
					== false)) {
				peer_info.my_short_addr
					= (msg->payload.peer_req_data.
						nwk_addr);
				memcpy(&(peer_info.peer_ieee_addr),
						(mac_frame_info->mpdu +
						LENGTH_FIELD_LEN +
						OFFSET_FOR_SRC_IEEE_ADDR),
						sizeof(peer_info.peer_ieee_addr));

				switch (msg->payload.peer_req_data.op_mode) {
				case RANGE_MEASURE_MODE:

					/*
					 * Frame found to be peer search for
					 * range test
					 * so change to state
					 * PEER_SEARCH_RANGE_RX
					 */
					set_main_state(PEER_SEARCH_RANGE_RX,
							&peer_info);
					print_event(
							PRINT_KEY_PRESS_PEER_SEARCH_RECEPTOR);
					break;

				case PER_TEST_MODE:

					/*
					 * Frame found to be peer search for
					 * range test
					 * so change to state PEER_SEARCH_PER_RX
					 */
					set_main_state(PEER_SEARCH_PER_RX,
							&peer_info);
					print_event(
							PRINT_UART_CHAR_PEER_SEARCH_RECEPTOR);
					break;

				default:

					/* The node has got a wrong frame: No
					 * change of mode */
					print_event(
							PRINT_PEER_SEARCH_RECEPTOR_BAD_FRAME);
					break;
				}
			}
		}
	}
}

/**
 * \brief Configure the TAL PIB's relevant to the Performance analyzer
 * application
 * \ingroup group_app_init
 */
static void configure_pibs(void)
{
	uint16_t temp_word;
	pib_value_t pib_value;
	uint8_t temp_byte;

	/* Set Default address. */
	temp_word = CCPU_ENDIAN_TO_LE16(DEFAULT_ADDR);
	pib_value.pib_value_16bit = temp_word;
	tal_pib_set(macShortAddress, &pib_value);

	/* Set PAN ID. */
	temp_word = CCPU_ENDIAN_TO_LE16(SRC_PAN_ID);
	pib_value.pib_value_16bit = temp_word;
	tal_pib_set(macPANId, &pib_value);

	/* Set channel. */
	temp_byte = (uint8_t)DEFAULT_CHANNEL;
	pib_value.pib_value_8bit = temp_byte;
	tal_pib_set(phyCurrentChannel, &pib_value);

	/* Set channel page. */
	temp_byte = (uint8_t)DEFAULT_PAGE;
	pib_value.pib_value_8bit = temp_byte;
	tal_pib_set(phyCurrentPage, &pib_value);

	/* Set IEEE address - To make sure that trx registers written properly
	**/
	tal_pib_set(macIeeeAddress, (pib_value_t *)&tal_pib.IeeeAddress);
}
