/**
 * @file common_sw_timer.c
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

#include "compiler.h"
#include "conf_common_sw_timer.h"
#include "common_hw_timer.h"
#include "common_sw_timer.h"
#if SAMD || SAMR21 || SAML21
#include "system.h"
#endif /* SAMD || SAMR21 || SAML21*/
#include "board.h"

#if (TOTAL_NUMBER_OF_SW_TIMERS > 0)

/**
 * This is the most significant part of the system time. The least one is
 * timer1 itself.  Exported since input capture units might want
 * to know about this value as well.
 */
volatile uint16_t sys_time;
extern void wakeup_cb(void *parameter);

extern bool sys_sleep;

/*
 * This is the timer array.
 *
 * TOTAL_NUMBER_OF_TIMERS is calculated in file app_config.h within the Inc
 * directory of each application depending on the number of timers required
 * by the stack and the application.
 */
timer_info_t timer_array[TOTAL_NUMBER_OF_SW_TIMERS];

/* This is the counter of all running timers. */
static uint8_t running_timers;

/* This flag indicates an expired timer. */
volatile bool timer_trigger;

/* This is the reference to the head of the running timer queue. */
static uint_fast8_t running_timer_queue_head;

/* This is the reference to the head of the expired timer queue. */
static uint_fast8_t expired_timer_queue_head;

/* This is the reference to the tail of the expired timer queue. */
static uint_fast8_t expired_timer_queue_tail;

static uint8_t alloc_timer_id = 0;

/* === Prototypes =========================================================== */

static void prog_ocr(void);
static void start_absolute_timer(uint8_t timer_id,
		uint32_t point_in_time,
		FUNC_PTR handler_cb,
		void *parameter);
static inline uint32_t gettime(void);

static void internal_timer_handler(void);
static inline bool compare_time(uint32_t t1, uint32_t t2);
static void load_hw_timer(uint8_t timer_id);

status_code_t sw_timer_get_id(uint8_t *timer_id)
{
	if (alloc_timer_id < TOTAL_NUMBER_OF_SW_TIMERS) {
		*timer_id = alloc_timer_id;
		alloc_timer_id++;
		return STATUS_OK;
	}

	return ERR_NO_TIMER;
}

status_code_t sw_timer_start(uint8_t timer_id,
		uint32_t timer_count,
		sw_timeout_type_t timeout_type,
		FUNC_PTR timer_cb,
		void *param_cb)
{
	uint32_t now;
	uint32_t point_in_time;

	if (TOTAL_NUMBER_OF_SW_TIMERS <= timer_id || NULL == timer_cb) {
		return ERR_INVALID_ARG;
	}

	if (NULL != timer_array[timer_id].timer_cb) {
		/*
		 * Timer is already running if the callback function of the
		 * corresponding timer index in the timer array is not NULL.
		 */
		return ERR_TIMER_ALREADY_RUNNING;
	}

	now = gettime();

	switch (timeout_type) {
	case SW_TIMEOUT_RELATIVE:
	{
		if ((timer_count > MAX_TIMEOUT) ||
				(timer_count < MIN_TIMEOUT)) {
			return ERR_INVALID_ARG;
		}

		point_in_time = ADD_TIME(timer_count, now);
	}
	break;

	case SW_TIMEOUT_ABSOLUTE:
	{
		uint32_t timeout;
		timeout = SUB_TIME(timer_count, now);

		if ((timeout > MAX_TIMEOUT) || (timeout < MIN_TIMEOUT)) {
			return ERR_INVALID_ARG;
		}

		point_in_time = timer_count;
	}
	break;

	default:
		return ERR_INVALID_ARG;
	}

	start_absolute_timer(timer_id, point_in_time, timer_cb, param_cb);
	return STATUS_OK;
}

uint32_t sw_timer_get_residual_time(uint8_t timer_id)
{
	uint32_t res_time;
	uint32_t current_time;
	current_time = gettime();
	if (current_time < timer_array[timer_id].abs_exp_timer) {
		res_time = timer_array[timer_id].abs_exp_timer - current_time;
	} else {
		res_time = 0;
	}

	return res_time;
}

static void start_absolute_timer(uint8_t timer_id,
		uint32_t point_in_time,
		FUNC_PTR handler_cb,
		void *parameter)
{
	uint8_t flags = cpu_irq_save();

	/* Check is done to see if any timer has expired */
	internal_timer_handler();

	timer_array[timer_id].abs_exp_timer = point_in_time;
	timer_array[timer_id].timer_cb = (FUNC_PTR)handler_cb;
	timer_array[timer_id].param_cb = parameter;
	timer_array[timer_id].loaded = false;
	running_timers++;

	if (NO_TIMER == running_timer_queue_head) {
		running_timer_queue_head = timer_id;

		load_hw_timer(running_timer_queue_head);
	} else {
		uint8_t index;
		bool timer_inserted = false;
		uint8_t curr_index = running_timer_queue_head;
		uint8_t prev_index = running_timer_queue_head;

		for (index = 0; index < running_timers; index++) {
			if (NO_TIMER != curr_index) {
				if (compare_time(timer_array[curr_index].
						abs_exp_timer,
						point_in_time)) {
					/*
					 * Requested absolute time value is
					 * greater than the time
					 * value pointed by the curr_index in
					 * the timer array
					 */
					prev_index = curr_index;
					curr_index
						= timer_array[curr_index].
							next_timer_in_queue;
				} else {
					timer_array[timer_id].
					next_timer_in_queue
						= curr_index;
					timer_array[curr_index].loaded = false;
					if (running_timer_queue_head ==
							curr_index) {
						/* Insertion at the head of the
						 * timer queue. */
						running_timer_queue_head
							= timer_id;

						load_hw_timer(
								running_timer_queue_head);
					} else {
						timer_array[prev_index].
						next_timer_in_queue
							= timer_id;
					}

					timer_inserted = true;
					break;
				}
			}
		}
		if (!timer_inserted) {
			/* Insertion at the tail of the timer queue. */
			timer_array[prev_index].next_timer_in_queue = timer_id;
			timer_array[timer_id].next_timer_in_queue = NO_TIMER;
		}
	}

	cpu_irq_restore(flags);
}

static void load_hw_timer(uint8_t timer_id)
{
	if (NO_TIMER != timer_id) {
		uint32_t now = gettime();
		uint32_t point_in_time = timer_array[timer_id].abs_exp_timer;
		if (compare_time(now, point_in_time)) {
			if (!timer_array[timer_id].loaded) {
				uint32_t timediff = point_in_time - now;

				if (timediff <= UINT16_MAX) {
					common_tc_delay(timediff);
					timer_array[timer_id].loaded = true;
				} else {
					timer_array[timer_id].loaded = false;
				}
			}
		} else {
			timer_trigger = true;
		}
	} else {
		common_tc_compare_stop();
	}
}

bool sw_timer_is_running(uint8_t timer_id)
{
	if (NULL == timer_array[timer_id].timer_cb) {
		return false;
	}

	return true;
}

status_code_t sw_timer_stop(uint8_t timer_id)
{
	bool timer_stop_request_status = false;
	uint8_t curr_index;
	uint8_t prev_index;

	if (TOTAL_NUMBER_OF_SW_TIMERS <= timer_id) {
		return ERR_INVALID_ARG;
	}

	uint8_t flags = cpu_irq_save();

	/* Check if any timer has expired. */
	internal_timer_handler();

	/* The requested timer is first searched in the running timer queue */
	if (running_timers > 0) {
		uint8_t timer_count = running_timers;
		prev_index = curr_index = running_timer_queue_head;
		while (timer_count > 0) {
			if (timer_id == curr_index) {
				timer_stop_request_status = true;

				if (timer_id == running_timer_queue_head) {
					common_tc_compare_stop();

					running_timer_queue_head
						= timer_array[timer_id].
							next_timer_in_queue;

					/*
					 * The value in OCR corresponds to the
					 * timeout pointed
					 * by the 'running_timer_queue_head'. As
					 * the head has
					 * changed here, OCR needs to be loaded
					 * by the new
					 * timeout value, if any.
					 */
					load_hw_timer(running_timer_queue_head);
					/*                    prog_ocr(); */
				} else {
					timer_array[prev_index].
					next_timer_in_queue
						= timer_array[timer_id].
							next_timer_in_queue;
				}

				/*
				 * The next timer element of the stopped timer
				 * is updated
				 * to its default value.
				 */
				timer_array[timer_id].next_timer_in_queue
					= NO_TIMER;
				break;
			} else {
				prev_index = curr_index;
				curr_index
					= timer_array[curr_index].
						next_timer_in_queue;
			}

			timer_count--;
		}
		if (timer_stop_request_status) {
			running_timers--;
		}
	}

	/*
	 * The requested timer is not present in the running timer queue.
	 * It will be now searched in the expired timer queue
	 */
	if (!timer_stop_request_status) {
		prev_index = curr_index = expired_timer_queue_head;
		while (NO_TIMER != curr_index) {
			if (timer_id == curr_index) {
				if (timer_id == expired_timer_queue_head) {
					/*
					 * The requested timer is the head of
					 * the expired timer
					 * queue
					 */
					if (expired_timer_queue_head ==
							expired_timer_queue_tail)
					{
						/* Only one timer in expired
						 * timer queue */
						expired_timer_queue_head
							=
								expired_timer_queue_tail
									=
										NO_TIMER;
					} else {
						/*
						 * The head of the expired timer
						 * queue is moved to next
						 * timer in the expired timer
						 * queue.
						 */
						expired_timer_queue_head
							= timer_array[
							expired_timer_queue_head
								].
								next_timer_in_queue;
					}
				} else {
					/*
					 * The requested timer is present in the
					 * middle or at the
					 * end of the expired timer queue.
					 */
					timer_array[prev_index].
					next_timer_in_queue
						= timer_array[timer_id].
							next_timer_in_queue;

					/*
					 * If the stopped timer is the one which
					 * is at the tail of
					 * the expired timer queue, then the
					 * tail is updated.
					 */
					if (timer_id ==
							expired_timer_queue_tail)
					{
						expired_timer_queue_tail
							= prev_index;
					}
				}

				timer_stop_request_status = true;
				break;
			} else {
				prev_index = curr_index;
				curr_index
					= timer_array[curr_index].
						next_timer_in_queue;
			}
		}
	}

	if (timer_stop_request_status) {
		/*
		 * The requested timer is stopped, hence the structure elements
		 * of the
		 * timer are updated.
		 */
		timer_array[timer_id].timer_cb = NULL;
		timer_array[timer_id].param_cb = NULL;
		timer_array[timer_id].loaded = false;
	}

	cpu_irq_restore(flags);

	if (timer_stop_request_status) {
		return STATUS_OK;
	}

	return ERR_TIMER_NOT_RUNNING;
}

uint32_t sw_timer_get_time(void)
{
	return gettime();
}

static inline bool compare_time(uint32_t t1, uint32_t t2)
{
	return ((t2 - t1) < INT32_MAX);
}

static inline uint32_t gettime(void)
{
	uint16_t current_sys_time;
	uint32_t current_time;

	do {
		current_sys_time = sys_time;
		current_time = current_sys_time;
		current_time = current_time << 16;
		current_time = current_time | common_tc_read_count();

		/*
		 * This calculation is valid only if the timer has not rolled
		 * over.
		 * The sys_time variable may have changed in the timer overflow
		 * ISR.
		 */
	} while (current_sys_time != sys_time);

	return current_time;
}

static void prog_ocr(void)
{
	uint16_t timeout_high;
	uint16_t timeout_low;
	uint32_t timeout;

	uint8_t flags = cpu_irq_save();

	if (NO_TIMER != running_timer_queue_head &&
			!timer_array[running_timer_queue_head].loaded) {
		timeout = timer_array[running_timer_queue_head].abs_exp_timer;
		timeout_high = (uint16_t)(timeout >> SYS_TIME_SHIFT_MASK);

		if (timeout_high == sys_time) {
			timeout_low = (uint16_t)(timeout & HW_TIME_MASK);
			common_tc_delay((uint16_t)timeout_low);
			timer_array[running_timer_queue_head].loaded = true;
		}
	}

	cpu_irq_restore(flags);
}

static void internal_timer_handler(void)
{
	/*
	 * Flag was set once a timer has expired by the timer ISR or
	 * by function prog_rc().
	 */
	if (timer_trigger) {
		timer_trigger = false;

		if (running_timers > 0) { /* Holds the number of running timers
			                  **/
			if ((expired_timer_queue_head == NO_TIMER) &&
					(expired_timer_queue_tail ==
					NO_TIMER)) {
				expired_timer_queue_head
					= expired_timer_queue_tail
							=
								running_timer_queue_head;
			} else {
				timer_array[expired_timer_queue_tail].
				next_timer_in_queue
					= running_timer_queue_head;

				expired_timer_queue_tail
					= running_timer_queue_head;
			}

			running_timer_queue_head
				= timer_array[running_timer_queue_head].
					next_timer_in_queue;

			timer_array[expired_timer_queue_tail].
			next_timer_in_queue
				= NO_TIMER;

			if ((--running_timers) > 0) {
				load_hw_timer(running_timer_queue_head);
			}
		}
	}
}

void sw_timer_run_residual_time(uint32_t offset)
{
	/* Run the software timer call back now */
	FUNC_PTR timer_cb = timer_array[running_timer_queue_head].timer_cb;
	void *param_cb = timer_array[running_timer_queue_head].param_cb;
	uint8_t timer_id = running_timer_queue_head;
	if (sw_timer_stop(running_timer_queue_head) == STATUS_OK) {
		sw_timer_start(timer_id,
				offset,
				SW_TIMEOUT_RELATIVE,
				timer_cb,
				param_cb);
	}
}

uint32_t sw_timer_next_timer_expiry_duration(void)
{
	return ((NO_TIMER ==
	       running_timer_queue_head) ? false : (sw_timer_get_residual_time(
	       running_timer_queue_head)));
}

void hw_overflow_cb(void)
{
	/*	ioport_toggle_pin(J2_PIN3); */
	sys_time++;
	prog_ocr();
	#ifdef ENABLE_SLEEP
	if (sys_sleep == true) {
		sys_sleep = true;
		system_set_sleepmode(SYSTEM_SLEEPMODE_IDLE_2);

		/* Enter into sleep*/
		system_sleep();
	}

	#endif
}

void hw_expiry_cb(void)
{
	if (running_timers > 0) {
		timer_trigger = true;
	#ifdef ENABLE_SLEEP
		if (sys_sleep == true) {
			sys_sleep = false;
			sw_timer_service();
		}

	#endif
	}
}

#endif /* #if (TOTAL_NUMBER_OF_SW_TIMERS > 0) */

void sw_timer_init(void)
{
#if (TOTAL_NUMBER_OF_SW_TIMERS > 0)

	/*
	 * Initialize the timer resources like timer arrays
	 * queues, timer registers
	 */
	uint8_t index;

	running_timers = 0;
	timer_trigger = false;
	sys_time = 0;

	running_timer_queue_head = NO_TIMER;
	expired_timer_queue_head = NO_TIMER;
	expired_timer_queue_tail = NO_TIMER;

	for (index = 0; index < TOTAL_NUMBER_OF_SW_TIMERS; index++) {
		timer_array[index].next_timer_in_queue = NO_TIMER;
		timer_array[index].timer_cb = NULL;
	}

	alloc_timer_id = 0;
	set_common_tc_overflow_callback(hw_overflow_cb);
	set_common_tc_expiry_callback(hw_expiry_cb);
	common_tc_init();
#endif /* #if (TOTAL_NUMBER_OF_SW_TIMERS > 0) */
}

void sw_timer_service(void)
{
#if (TOTAL_NUMBER_OF_SW_TIMERS > 0)
	uint8_t flags = cpu_irq_save();
	internal_timer_handler();
	cpu_irq_restore(flags);

	/*
	 * Process expired timers.
	 * Call the callback functions of the expired timers in the order of
	 * their
	 * expiry.
	 */
	{
		timer_expiry_cb_t callback;
		void *callback_param;
		uint8_t next_expired_timer;

		/* Expired timer if any will be processed here */
		while (NO_TIMER != expired_timer_queue_head) {
			flags = cpu_irq_save();

			next_expired_timer
				= timer_array[expired_timer_queue_head].
					next_timer_in_queue;

			/* Callback is stored */
			callback
				= (timer_expiry_cb_t)timer_array[
				expired_timer_queue_head
					].timer_cb;

			/* Callback parameter is stored */
			callback_param
				= timer_array[expired_timer_queue_head].param_cb;

			/*
			 * The expired timer's structure elements are updated
			 * and the timer
			 * is taken out of expired timer queue
			 */
			timer_array[expired_timer_queue_head].
			next_timer_in_queue = NO_TIMER;
			timer_array[expired_timer_queue_head].timer_cb = NULL;
			timer_array[expired_timer_queue_head].param_cb = NULL;
			timer_array[expired_timer_queue_head].loaded = false;

			/*
			 * The expired timer queue head is updated with the next
			 * timer in the
			 * expired timer queue.
			 */
			expired_timer_queue_head = next_expired_timer;

			if (NO_TIMER == expired_timer_queue_head) {
				expired_timer_queue_tail = NO_TIMER;
			}

			cpu_irq_restore(flags);

			if (NULL != callback) {
				/* Callback function is called */
				callback(callback_param);
			}
		}
	}
#endif /* #if (TOTAL_NUMBER_OF_SW_TIMERS > 0) */
}

/* EOF */
