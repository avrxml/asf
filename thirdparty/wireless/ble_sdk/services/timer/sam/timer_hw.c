/**
 * \file timer_hw.c
 *
 * \brief Handler timer functionalities
 *
 * Copyright (c) 2017-2018 Microchip Technology Inc. and its subsidiaries.
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

/* === INCLUDES ============================================================ */

#include "asf.h"
#include "timer_hw.h"
#include "conf_timer.h"

/* === MACROS ============================================================== */
#define MAX_TIMEOUT_MS		(65)

/* === TYPES =============================================================== */
typedef struct hw_timer_tag
{
	Tc *instance;
	uint32_t tc_id;
	uint32_t channel_id;
	enum {
		UNUSED = 0,
		USED = 1,
	} state;
	platform_hw_timer_callback_t callback;
	
	uint32_t timeout_ms;
	
} hw_timer_t; 

static hw_timer_t hw_timers[STACK_HW_TIMERS] = {
	{
		.instance = STACK_TIMER_0,
		.tc_id = STACK_TIMER_0_ID,
		.channel_id = STACK_TIMER_0_CHANNEL_ID,
		.state = UNUSED,
		.callback = NULL,
	},
	{
		.instance = STACK_TIMER_1,
		.tc_id = STACK_TIMER_1_ID,
		.channel_id = STACK_TIMER_1_CHANNEL_ID,
		.state = UNUSED,
		.callback = NULL,
	},
};

/* === Globals  ============================================================== */
static uint32_t timeout_count_ms;
static uint32_t tc_count_ms;
hw_timer_callback_t timer_callback;
platform_hw_timer_callback_t bus_timer_callback;

/* === Macros ============================================================== */
#define MAX_TIMEOUT_MS		(65)

/* === Functions ============================================================== */
static inline void _start_timer(Tc *tc, uint32_t channel_id, uint32_t timeout_ms)
{
	static uint32_t timeout_val;
	timeout_val = (timeout_ms * TIMER_OVF_COUNT_1MSEC);// + tc_read_cv(tc, channel_id);
	tc_write_rc(tc, channel_id, timeout_val);
	tc_start(tc, channel_id);
}

void hw_timer_init(void)
{
	sysclk_enable_peripheral_clock(ID_TC0);
	// Init timer counter  channel.
	tc_init(BLE_APP_TIMER, BLE_APP_TIMER_CHANNEL_ID,						
			TC_CMR_TCCLKS_TIMER_CLOCK4 |
			TC_CMR_WAVSEL_UP);				
	
	tc_write_rc(BLE_APP_TIMER, BLE_APP_TIMER_CHANNEL_ID, UINT16_MAX);
	tc_get_status(BLE_APP_TIMER, BLE_APP_TIMER_CHANNEL_ID);
	tc_enable_interrupt(BLE_APP_TIMER, BLE_APP_TIMER_CHANNEL_ID, TC_IER_CPCS);		
	NVIC_EnableIRQ(TC0_IRQn);
	tc_stop(BLE_APP_TIMER, BLE_APP_TIMER_CHANNEL_ID);
}

void hw_timer_register_callback(hw_timer_callback_t cb_ptr)
{
	timer_callback = cb_ptr;
}

void TC0_Handler(void)
{
	uint32_t ul_status;
	
	ul_status = tc_get_status(BLE_APP_TIMER, BLE_APP_TIMER_CHANNEL_ID);
	ul_status &= tc_get_interrupt_mask(BLE_APP_TIMER, BLE_APP_TIMER_CHANNEL_ID);
	
	/* RC Compare callback */
	if (TC_SR_CPCS == (ul_status & TC_SR_CPCS))
	{		
		if (tc_count_ms <= MAX_TIMEOUT_MS) {
			// Timer has expired;
			if (timer_callback != NULL)
			{
				timer_callback();
			}
			// Reload the value to start over
			tc_count_ms = timeout_count_ms;
			
		} else {
			tc_count_ms -= MAX_TIMEOUT_MS;
		}
		
		// Start the timer
		if (tc_count_ms >= MAX_TIMEOUT_MS) {
			_start_timer(BLE_APP_TIMER, BLE_APP_TIMER_CHANNEL_ID, MAX_TIMEOUT_MS);
		} else {
			_start_timer(BLE_APP_TIMER, BLE_APP_TIMER_CHANNEL_ID, tc_count_ms);
		}
	}
}

void hw_timer_start(uint32_t timer_val_s)
{
	hw_timer_start_ms(timer_val_s * 1000);
}

void hw_timer_start_ms(uint32_t timer_val_ms)
{
	static uint32_t cur_ms;
	timeout_count_ms = timer_val_ms;
	// Load the required ms.
	tc_count_ms = timeout_count_ms;
	if (timeout_count_ms >= MAX_TIMEOUT_MS) {
		cur_ms = MAX_TIMEOUT_MS;
	} else {
		cur_ms = timeout_count_ms;
	}
	// Start the timer 
	_start_timer(BLE_APP_TIMER, BLE_APP_TIMER_CHANNEL_ID, cur_ms);
}

void hw_timer_stop(void)
{
	tc_stop(BLE_APP_TIMER, BLE_APP_TIMER_CHANNEL_ID);
}

void *platform_create_hw_timer(platform_hw_timer_callback_t tc_cb_ptr)
{
	uint32_t i;
	Disable_global_interrupt();
	for (i = 0; i < (sizeof(hw_timers) / sizeof(hw_timer_t)); i++)
	{
		if (hw_timers[i].state == UNUSED) {
			// Found a free timer; Configure it
			hw_timers[i].state = USED;
			hw_timers[i].callback = tc_cb_ptr;
			sysclk_enable_peripheral_clock(hw_timers[i].tc_id);
			// Initialize timer counter channel.
			tc_init(hw_timers[i].instance, hw_timers[i].channel_id,
					(TC_CMR_TCCLKS_TIMER_CLOCK4 | TC_CMR_WAVSEL_UP));
			// 16-bit timer
			tc_write_rc(hw_timers[i].instance, hw_timers[i].channel_id, UINT16_MAX);
			tc_get_status(hw_timers[i].instance, hw_timers[i].channel_id);
			tc_enable_interrupt(hw_timers[i].instance, hw_timers[i].channel_id, TC_IER_CPCS);
			NVIC_EnableIRQ(hw_timers[i].tc_id);
			tc_stop(hw_timers[i].instance, hw_timers[i].channel_id);
			Enable_global_interrupt();
			return (void *)&(hw_timers[i]);
		}
	}
	Enable_global_interrupt();
	return NULL;	
}

void TC1_Handler(void)
{
	uint32_t ul_status;
	
	ul_status = tc_get_status(hw_timers[0].instance, hw_timers[0].channel_id);
	ul_status &= tc_get_interrupt_mask(hw_timers[0].instance, hw_timers[0].channel_id);
	
	/* RC Compare callback */
	if (TC_SR_CPCS == (ul_status & TC_SR_CPCS))
	{
		tc_stop(hw_timers[0].instance, hw_timers[0].channel_id);
		if (hw_timers[0].timeout_ms > MAX_TIMEOUT_MS) {
			// Configure timer for another MAX_TIMEOUT
			hw_timers[0].timeout_ms -= MAX_TIMEOUT_MS;
			if (hw_timers[0].timeout_ms > MAX_TIMEOUT_MS) {
				_start_timer(hw_timers[0].instance, hw_timers[0].channel_id, MAX_TIMEOUT_MS);
			} else {
				_start_timer(hw_timers[0].instance, hw_timers[0].channel_id, hw_timers[0].timeout_ms);
			}
		} else {
			// Timer has expired;
			hw_timers[0].callback((void *)hw_timers[0].instance);
		}
	}
}

void TC2_Handler(void)
{
	uint32_t ul_status;
	
	ul_status = tc_get_status(hw_timers[1].instance, hw_timers[1].channel_id);
	ul_status &= tc_get_interrupt_mask(hw_timers[1].instance, hw_timers[1].channel_id);
	
	/* RC Compare callback */
	if (TC_SR_CPCS == (ul_status & TC_SR_CPCS))
	{
		tc_stop(hw_timers[1].instance, hw_timers[1].channel_id);
		if (hw_timers[1].timeout_ms > MAX_TIMEOUT_MS) {
			// Configure timer for another MAX_TIMEOUT
			hw_timers[1].timeout_ms -= MAX_TIMEOUT_MS;
			if (hw_timers[1].timeout_ms > MAX_TIMEOUT_MS) {
				_start_timer(hw_timers[1].instance, hw_timers[1].channel_id, MAX_TIMEOUT_MS);
				} else {
				_start_timer(hw_timers[1].instance, hw_timers[1].channel_id, hw_timers[1].timeout_ms);
			}
			} else {
			// Timer has expired;
			hw_timers[1].callback((void *)hw_timers[1].instance);
		}
	}
}

void platform_start_bus_timer(void *timer_handle, uint32_t ms)
{
	static uint32_t cur_ms = 0;
	hw_timer_t *tmr = (hw_timer_t *) timer_handle; 

	if (ms > MAX_TIMEOUT_MS)
	{
		cur_ms = MAX_TIMEOUT_MS;

	} else {
		// <= 65ms
		cur_ms = ms;
	}
	tmr->timeout_ms = ms;
	_start_timer(tmr->instance, tmr->channel_id, cur_ms);
}

void platform_delete_bus_timer(void *timer_handle)
{
	hw_timer_t *tmr = (hw_timer_t *) timer_handle;
	tc_stop(tmr->instance, tmr->channel_id);
	tmr->state = UNUSED;
}


void platform_stop_bus_timer(void *timer_handle)
{
	hw_timer_t *tmr = (hw_timer_t *) timer_handle;
	tc_stop(tmr->instance, tmr->channel_id);
}

void platform_stop_stack_timers(void)
{
	tc_stop(STACK_TIMER_0, STACK_TIMER_0_CHANNEL_ID);
	tc_stop(STACK_TIMER_1, STACK_TIMER_1_CHANNEL_ID);
}

#ifdef BTLC_REINIT_SUPPORT
void platform_reset_hw_timer(void)
{
	uint32_t idx;
	platform_stop_stack_timers();
	for (idx = 0; idx < (sizeof(hw_timers) / sizeof(hw_timer_t)); idx++)
	{
		hw_timers[idx].state = UNUSED;
	}
}
#endif

/* EOF */
