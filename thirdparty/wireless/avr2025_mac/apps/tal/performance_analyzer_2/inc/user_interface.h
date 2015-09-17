/**
 * \file user_interface.h
 *
 * \brief LED,Button,Terminal print event prototypes and defines
 * - Performance Analyzer application for AT86RF215
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

/* Prevent double inclusion */
#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "tal_rf215.h"

/**
 * \ingroup group_perf_analyzer_2
 * \defgroup group_user_interface  User Interface
 * Handles all User interface related functions such as Button, Print,LED
 * events.
 *  \{
 */

/* === Includes ============================================================= */

/* === Macros =============================================================== */

/* === Types ================================================================ */

/**
 * \name LED EVENTS
 *  \{
 */
/* Various LED events */
typedef enum {
	LED_EVENT_TX_FRAME,
	LED_EVENT_RX_FRAME,
	LED_EVENT_POWER_ON,
	LED_EVENT_START_PEER_SEARCH,
	LED_EVENT_PEER_SEARCH_DONE,
	LED_EVENT_ALL_ON,
	LED_EVENT_ALL_OFF
} led_event_t;

/* ! \} */

/**
 * \name PRINT EVENTS
 *  \{
 */
/* Various print events */
typedef enum {
	PRINT_KEY_PRESS_WHILE_POWER_ON_CONFIG_MODE,
	PRINT_KEY_PRESS_PEER_SEARCH_INITIATOR,
	PRINT_UART_CHAR_PEER_SEARCH_INITIATOR,
	PRINT_KEY_PRESS_PEER_SEARCH_RECEPTOR,
	PRINT_UART_CHAR_PEER_SEARCH_RECEPTOR,
	PRINT_PEER_SEARCH_RECEPTOR_BAD_FRAME,
	PRINT_PEER_SEARCH_INITATED,
	PRINT_PEER_SEARCH_IN_PROGRESS,
	PRINT_PEER_SEARCH_SUCCESS,
	PRINT_PEER_SEARCH_FAILED,
	PRINT_PEER_SEARCH_ABORTED,
	PRINT_PEER_SEARCH_ABORTABLE,
	PRINT_PEER_FOUND_PER_INITIATOR,
	PRINT_PEER_FOUND_PER_RECEPTOR,
	PRINT_SINGLE_NODE_TESTS,
	PRINT_RANGE_MEASURE_TX_START,
	PRINT_RANGE_MEASURE_TX_STOP,
	PRINT_RANGE_MEASURE_STATSTICS,
	NUM_MAX_PRINT_EVENTS
} print_event_t;

/* ! \} */

/**
 * \brief Print event message stucture to organize the prints
 *
 */
typedef struct {
	const char *item_desc;
	void (*additional_print_func)(int trx);
} print_event_msg_t;

/* === Externals ============================================================ */

/* === Prototypes =========================================================== */

/**
 * \brief Turns LEDs on / off to indicate specific events.
 *
 * \param ev the event to be indicated.
 *
 */
void app_led_event(led_event_t ev);

/**
 * \brief Print various messages from the table as per the event notified.
 *
 * \param ev the event to be printed on the Terminal.
 *
 */
void print_event(trx_id_t trx, print_event_t ev);

/**
 * \brief Button debounce routine.
 *
 * Helper function for debouncing the transmit button.
 * \return ret 1 if a button event is detected, 0 otherwise.
 */
bool app_debounce_button(void);

/**
 * Determine if button is pressed
 *
 * \return true if button is pressed, else false
 */
bool button_pressed(void);

/* Support functions */

/**
 * \brief Print the statistics for Range Measurement on the UART Terminal.
 */
void app_print_statistics(int trx);

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

/* ! \} */
#endif /* USER_INTERFACE_H */
/* EOF */
