/**
 * @file common_sw_timer.h
 *
 * @brief
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
 */

/* Prevent double inclusion */
#ifndef COMMON_SW_TIMER_H
#define COMMON_SW_TIMER_H

#include "compiler.h"
#include "status_codes.h"
#include "conf_common_sw_timer.h"

/**
 * \ingroup group_pal
 * \defgroup group_pal_common_sw_timer Common SW Timer
 * Implements the software Algorithm for Timer Handling in the Stack
 * @{
 */

/*
 * Type definition for callbacks for timer functions
 */
typedef void (*timer_expiry_cb_t)(void *);

#define TOTAL_TIMERS    TOTAL_NUMBER_OF_TIMERS
#define MAX_NO_OF_TIMERS          (25)

/*
 * Value to indicate end of timer in the array or queue
 */
#define NO_TIMER                (0xFF)

/*
 * Shift mask to obtain the 16-bit system time out of a 32-bit timeout
 */
#define SYS_TIME_SHIFT_MASK     (16)

/*
 * Mask to obtain the 16-bit H/W time out of a 32-bit timeout
 */
#define HW_TIME_MASK            (0xFFFF)

/*
 * The smallest timeout in microseconds
 */
#define MIN_TIMEOUT                     (0x80)

/*
 * The largest timeout in microseconds
 */
#define MAX_TIMEOUT                     (0x7FFFFFFF)

/**
 * Adds two time values
 */
#define ADD_TIME(a, b)                  ((a) + (b))

/**
 * Subtracts two time values
 */
#define SUB_TIME(a, b)                  ((a) - (b))

/**
 * Timeout type
 */
typedef enum sw_timeout_type_tag {
	/** The timeout is relative to the current time. */
	SW_TIMEOUT_RELATIVE,
	/** The timeout is an absolute value. */
	SW_TIMEOUT_ABSOLUTE
} sw_timeout_type_t;

/*
 * This defines the structure of the time type.
 */
typedef struct timer_info_tag {
	/* Timeout in microseconds */
	uint32_t abs_exp_timer;

	/* Callback function to be executed on expiry of the timer */
	FUNC_PTR timer_cb;

	/* Parameter to be passed to the callback function of the expired timer
	**/
	void *param_cb;

	/* Next timer which was started or has expired */
	uint8_t next_timer_in_queue;

	bool loaded;
} timer_info_t;

/**
 * @brief Returns a timer id to be used before starting a timer
 * @param timer_id Value of the id returned by the function
 */
status_code_t sw_timer_get_id(uint8_t *timer_id);

/**
 * \brief Starts a  timer
 *
 * This function starts a regular timer and installs the corresponding
 * callback function handle the timeout event.
 *
 * \param timer_id Timer identifier
 * \param timer_count Timeout in microseconds
 * \param timeout_type \ref TIMEOUT_RELATIVE or \ref TIMEOUT_ABSOLUTE
 * \param timer_cb Callback handler invoked upon timer expiry
 * \param param_cb Argument for the callback handler
 *
 */
status_code_t sw_timer_start(uint8_t timer_id,
		uint32_t timer_count,
		sw_timeout_type_t timeout_type,
		FUNC_PTR timer_cb,
		void *param_cb);

/**
 * \brief Stops a running timer
 *
 * This function stops a running timer with specified timer_id
 *
 * \param timer_id Timer identifier
 *
 */
status_code_t sw_timer_stop(uint8_t timer_id);

/**
 * \brief Gets current time
 *
 * This function returns the current time.
 *
 * \param[out] current_time Returns current system time
 */
uint32_t sw_timer_get_time(void);

/**
 * \brief Checks whether a given timer is running or not
 *
 *
 * \param[out] True if the timer is running else False
 */
bool sw_timer_is_running(uint8_t timer_id);

/**
 * \brief Handles Queues and Callbacks for Expired Timers
 */
void sw_timer_service(void);

/**
 * \brief Initializes the Software Timer module
 *
 */
void sw_timer_init(void);

uint32_t sw_timer_get_residual_time(uint8_t timer_id);

void sw_timer_run_residual_time(uint32_t offset);

uint32_t sw_timer_next_timer_expiry_duration(void);

/* ! @} */
#endif /* COMMON_SW_TIMER_H */
/* EOF */
