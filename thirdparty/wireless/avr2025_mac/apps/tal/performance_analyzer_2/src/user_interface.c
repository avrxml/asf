/**
 * \file user_interface.c
 *
 * \brief LED, Button and terminal print functions - Perfoamnce Analyzer
 * application of AT86RF215
 *
 * Copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 */

/*
 * Copyright (c) 2015-2018, Microchip Technology Inc All rights reserved.
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
