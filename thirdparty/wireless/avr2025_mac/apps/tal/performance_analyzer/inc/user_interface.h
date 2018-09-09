/**
 * \file user_interface.h
 *
 * \brief LED,Button,Terminal print event prototypes and defines
 * - Performance Analyzer application.
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
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
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */
/* Prevent double inclusion */
#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

/**
 * \ingroup group_perf_analyzer
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
	void (*additional_print_func)(void);
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
void print_event(print_event_t ev);

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
void app_print_statistics(void);

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

/* ! \} */
#endif /* USER_INTERFACE_H */
/* EOF */
