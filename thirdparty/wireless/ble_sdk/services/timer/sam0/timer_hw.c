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

/* === TYPES =============================================================== */
static struct hw_timer_struct
{
	struct tc_module timer_instance;
	void            *timer_id;
	uint32_t         timer_frequency;
	uint32_t         timer_usage;
}hw_timers[STACK_HW_TIMERS]= { { {0}, TMRID0, 0, 0},
							   { {0}, TMRID1, 0, 0} };

static uint32_t timeout_count_ms;
static uint32_t tc_count_ms;
uint32_t timer_count_per_ms;
hw_timer_callback_t timer_callback;
platform_hw_timer_callback_t platform_cc1_cb;
/* === MACROS ============================================================== */

void tc_cc0_cb(struct tc_module *const module_inst);
void tc_cc1_cb(struct tc_module *const module_inst);

void hw_timer_init(void)
{
	struct tc_config config_tc;

	tc_get_config_defaults(&config_tc);

	config_tc.counter_size = TC_COUNTER_SIZE_16BIT;
	config_tc.clock_source = GCLK_GENERATOR_0;
	config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV1024;
	config_tc.counter_8_bit.period = 0;
	config_tc.counter_16_bit.compare_capture_channel[0] = TC_COUNT_1SEC;
	config_tc.counter_16_bit.compare_capture_channel[1] = 0xFFFF;

	tc_init(&tc_instance, APP_TIMER, &config_tc);

	tc_enable(&tc_instance);
	
	tc_register_callback(&tc_instance, tc_cc0_cb,
						TC_CALLBACK_CC_CHANNEL0);
}

void hw_timer_register_callback(hw_timer_callback_t cb_ptr)
{
	timer_callback = cb_ptr;
}

/* Ticks for every 1s or for the configured ms units */
void tc_cc0_cb(struct tc_module *const module_inst)
{
	if (tc_count_ms <= 1000)
	{ 
		// Timer is expired
		if (timer_callback != NULL)
		{
			timer_callback();
		}
		//  Reload the value
		tc_count_ms = timeout_count_ms;
	} else {
		tc_count_ms -= 1000;
	}

	if (tc_count_ms >= 1000) {
		// Start the timer for 1second.
		tc_set_count_value(&tc_instance, 0);
	} else {
		// Start the timer for given ms.
		tc_set_count_value(&tc_instance, TC_COUNT_1SEC - (TC_COUNT_1SEC * tc_count_ms / 1000UL));
	}
}

void hw_timer_start(uint32_t timer_val_s)
{
	hw_timer_start_ms(timer_val_s * 1000);
}

void hw_timer_start_ms(uint32_t timer_val_ms)
{
	timeout_count_ms = timer_val_ms;
	// Load the required timeout value
	tc_count_ms = timeout_count_ms;
	if (timeout_count_ms >= 1000) {
		// Start the timer for 1second.
		tc_set_count_value(&tc_instance, 0);
	} else {
		// Start the timer for given ms.
		tc_set_count_value(&tc_instance, TC_COUNT_1SEC - (TC_COUNT_1SEC * timeout_count_ms / 1000UL));
	}
	tc_enable_callback(&tc_instance, TC_CALLBACK_CC_CHANNEL0);
}

void hw_timer_stop(void)
{
	tc_disable_callback(&tc_instance, TC_CALLBACK_CC_CHANNEL0);
}

void tc_cc1_cb(struct tc_module *const module_inst)
{
	platform_cc1_cb(module_inst);
}


void *platform_create_hw_timer(platform_hw_timer_callback_t bus_tc_cb_ptr)
{
	 struct tc_config timer_config;
	 uint32_t prescaler;
	 uint32_t i;
	 for (i = 0; i < sizeof(hw_timers) / sizeof(struct hw_timer_struct); i++)
	 {
		 if (hw_timers[i].timer_usage == 0)
		 {
			 hw_timers[i].timer_usage = 1;
			 tc_get_config_defaults(&timer_config);
			 timer_config.oneshot                = true;
			 timer_config.counter_size           = TC_COUNTER_SIZE_32BIT;
			 timer_config.count_direction        = TC_COUNT_DIRECTION_DOWN;
			 tc_init(&hw_timers[i].timer_instance, (Tc *)hw_timers[i].timer_id, &timer_config);
			 prescaler = 1 << (timer_config.clock_prescaler >> TC_CTRLA_PRESCALER_Pos);
			 hw_timers[i].timer_frequency = (system_gclk_gen_get_hz(timer_config.clock_source) / prescaler);
			 tc_set_count_value(&hw_timers[i].timer_instance, 0xFFFFFFFF);
			 tc_enable(&hw_timers[i].timer_instance);
			 tc_stop_counter(&hw_timers[i].timer_instance);
			 tc_register_callback(&hw_timers[i].timer_instance, (tc_callback_t) bus_tc_cb_ptr, TC_CALLBACK_OVERFLOW);
			 tc_enable_callback(&hw_timers[i].timer_instance, TC_CALLBACK_OVERFLOW);
			 tc_set_count_value(&hw_timers[i].timer_instance, (hw_timers[i].timer_frequency / 1000));
			 tc_start_counter(&hw_timers[i].timer_instance);
			 return &hw_timers[i];
		 }
	 }
	 return NULL;
}


void platform_start_bus_timer(void *timer_handle, uint32_t ms)
{
    struct hw_timer_struct *hw_timer_instance = (struct hw_timer_struct *)timer_handle;
    uint32_t top_value;
    top_value = (hw_timer_instance->timer_frequency / 1000) * ms;
    tc_set_count_value(&(hw_timer_instance->timer_instance), top_value);
    tc_start_counter(&(hw_timer_instance->timer_instance));
}

void platform_delete_bus_timer(void *timer_handle)
{
    struct hw_timer_struct *hw_timer_instance = (struct hw_timer_struct *)timer_handle;
    tc_stop_counter(&(hw_timer_instance->timer_instance));
    tc_reset(&(hw_timer_instance->timer_instance));
    hw_timer_instance->timer_usage = 0;
}

void platform_stop_bus_timer(void *timer_handle)
{
    struct hw_timer_struct *hw_timer_instance = (struct hw_timer_struct *)timer_handle;
    tc_stop_counter(&(hw_timer_instance->timer_instance));
}

void platform_stop_stack_timers(void)
{
	tc_stop_counter(&hw_timers[0].timer_instance);
	tc_stop_counter(&hw_timers[1].timer_instance);
}

#ifdef BTLC_REINIT_SUPPORT
void platform_reset_hw_timer(void)
{
	tc_reset(&hw_timers[0].timer_instance);
	tc_reset(&hw_timers[1].timer_instance);
	hw_timers[0].timer_usage = 0;
	hw_timers[1].timer_usage = 0;
}
#endif

/* EOF */
