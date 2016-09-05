/**
 * \file user_interface.c
 *
 * \brief LED, Button and terminal print functions - Perfoamnce Analyzer
 * application of AT86RF215
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
# include "asf.h"
# include "app_init.h"
#include "user_interface.h"

/**
 * \addtogroup group_user_interface
 * \{
 */
/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/**
 * This defines the count to detect a valid key press
 */
#define COUNT_FOR_VALID_KEY_PRESS       (20)

/**
 * This defines the LED on duration
 */
#define LED_ON_DURAION_MICRO_SEC        (100000)

/* === PROTOTYPES ========================================================== */
#if (LED_COUNT > 0)
static void app_switch_off_tx_led_cb(void *parameter);
static void app_switch_off_rx_led_cb(void *parameter);

#endif
/* === GLOBALS ============================================================= */

/**
 * This table holds the Messages to be printed on the UART Terminal at each
 * event
 */
print_event_msg_t print_event_table[NUM_MAX_PRINT_EVENTS] = {
	{ /* PRINT_KEY_PRESS_WHILE_POWER_ON_CONFIG_MODE */
		"\r\nButton press while power-on - CONFIGURATION_MODE",
		NULL
	},
	{ /* PRINT_KEY_PRESS_PEER_SEARCH_INITIATOR */
		"\r\nKey press detected: Range Measurement mode",
		NULL
	},
	{ /* PRINT_UART_CHAR_PEER_SEARCH_INITIATOR */
		"\r\nCharacter received on UART: PER Measurement mode",
		NULL
	},
	{ /* PRINT_KEY_PRESS_PEER_SEARCH_RECEPTOR */
		"\r\nFrames received on air: Range Measurement mode",
		NULL
	},
	{ /* PRINT_UART_CHAR_PEER_SEARCH_RECEPTOR */
		"\r\nFrames received on air: PER Measurement mode",
		NULL
	},
	{ /* PRINT_PEER_SEARCH_RECEPTOR_BAD_FRAME */
		"\r\nFrames received on air: Bad frame received",
		NULL
	},
	{ /* PRINT_PEER_SEARCH_INITATED */
		"\r\nSearch for Peer device initiated",
		NULL
	},
	{ /* PRINT_PEER_SEARCH_IN_PROGRESS */
		".",
		NULL
	},
	{ /* PRINT_PEER_SEARCH_SUCCESS */
		"\r\nPeer device found",
		NULL
	},
	{ /* PRINT_PEER_SEARCH_FAILED */
		"\r\nSearch for Peer Device failed",
		NULL
	},
	{ /* PRINT_PEER_SEARCH_ABORTED */
		"\r\nSearch for Peer Device aborted",
		NULL
	},
	{ /* PRINT_PEER_SEARCH_ABORTABLE */
		"\r\nPress ENTER to Abort Search for Peer Device and to start single node operation mode",
		NULL
	},
	{ /* PRINT_PEER_FOUND_PER_INITIATOR  */
		"\r\nStarting PER Measurement mode as Transmitter",
		NULL
	},
	{ /* PRINT_PEER_FOUND_PER_RECEPTOR  */
		"\r\nStarting PER Measurement mode as Reflector",
		NULL
	},
	{ /* PRINT_SINGLE_NODE_TESTS */
		"\r\nStarting Performance Test Application in single node operation mode",
		NULL
	},
	{ /* PRINT_RANGE_MEASURE_TX_START */
		"\r\nStarting TX in Range Measurement mode",
		NULL
	},
	{ /* PRINT_RANGE_MEASURE_TX_STOP */
		"\r\nStopping TX in Range Measurement mode",
		NULL
	},
	{ /* PRINT_RANGE_MEASURE_STATSTICS */
		NULL,
		app_print_statistics
	}
};

/* ! \} */
/* === IMPLEMENTATION ====================================================== */

/* Button related functions */

/*
 * \brief Button debounce routine.
 *
 * Helper function for debouncing the transmit button.
 * \return ret 1 if a button event is detected, 0 otherwise.
 *
 */
bool app_debounce_button(void)
{
	uint8_t ret = 0;
	static uint8_t key_cnt;
	/*Read the current state of the button*/

	if (button_pressed()) { /* Button Pressed */
		if (key_cnt != COUNT_FOR_VALID_KEY_PRESS) {
			key_cnt++;
		}
	} else if (!(button_pressed()) &&
			(key_cnt == COUNT_FOR_VALID_KEY_PRESS)) {           /*
		                                                             * Button
		                                                             *
		                                                             *released
		                                                             **/
		ret = 1;
		key_cnt = 0;
	} else {
		key_cnt = 0;
	}

	return ret;
}

/*
 * Determine if button is pressed
 *
 * \return true if button is pressed, else false
 */
bool button_pressed(void)
{
#if defined GPIO_PUSH_BUTTON_0
	/*Read the current state of the button*/
	if (ioport_get_pin_level(GPIO_PUSH_BUTTON_0)) {
		return false;
	} else {
		return true;
	}

#else
	return false;
#endif
}

/* LED related functions */

/*
 * \brief Turns LEDs on/off to indicate specific events.
 *
 * \param ev the event to be indicated.
 *
 */
void app_led_event(led_event_t ev)
{
#if (LED_COUNT > 0)
	switch (ev) {
	case LED_EVENT_TX_FRAME:
		LED_On(TX_LED);
		sw_timer_start(APP_TIMER_TO_TX_LED_OFF,
				LED_ON_DURAION_MICRO_SEC,
				SW_TIMEOUT_RELATIVE,
				(FUNC_PTR)app_switch_off_tx_led_cb,
				NULL);
		break;

	case LED_EVENT_RX_FRAME:
		LED_On(RX_LED);
		sw_timer_start(APP_TIMER_TO_RX_LED_OFF,
				LED_ON_DURAION_MICRO_SEC,
				SW_TIMEOUT_RELATIVE,
				(FUNC_PTR)app_switch_off_rx_led_cb,
				NULL);
		break;

	case LED_EVENT_POWER_ON:
		LED_Off(STATUS_LED);
		LED_Off(RX_LED);
		LED_On(TX_LED);
		break;

	case LED_EVENT_START_PEER_SEARCH:
		LED_Off(STATUS_LED);
		LED_Off(TX_LED);
		LED_Off(RX_LED);
		break;

	case LED_EVENT_PEER_SEARCH_DONE:

		LED_Off(TX_LED);
		LED_Off(RX_LED);
		LED_On(STATUS_LED);
		break;

	case LED_EVENT_ALL_ON:
		LED_On(STATUS_LED);
		LED_On(TX_LED);
		LED_On(RX_LED);

		break;

	case LED_EVENT_ALL_OFF:
		LED_Off(STATUS_LED);
		LED_Off(TX_LED);
		LED_Off(RX_LED);
		break;
	}
#endif
}

#if (LED_COUNT > 0)

/**
 * \brief Callback function switching off the TX_LED
 *
 * \param parameter Pointer to callback parameter
 *                  (not used in this application, but could be if desired).
 */
static void app_switch_off_tx_led_cb(void *parameter)
{
	/* switch off the LED */
	LED_Off(TX_LED);

	/* keep the compiler happy */
	parameter = parameter;
}

/**
 * \brief Callback function switching off the RX_LED
 *
 * \param parameter Pointer to callback parameter
 *                  (not used in this application, but could be if desired).
 */
static void app_switch_off_rx_led_cb(void *parameter)
{
	/* switch off the LED */
	LED_Off(RX_LED);

	/* keep the compiler happy */
	parameter = parameter;
}

#endif

/* Print related functions */

/*
 * \brief Print various messages from the table as per the event notified.
 *
 * \param ev the event to be printed on the Terminal.
 *
 */
void print_event(trx_id_t trx, print_event_t ev)
{
	if (print_event_table[ev].item_desc) {
		printf("%s", print_event_table[ev].item_desc);
	}

	if (print_event_table[ev].additional_print_func) {
		print_event_table[ev].additional_print_func(trx);
	}
}
