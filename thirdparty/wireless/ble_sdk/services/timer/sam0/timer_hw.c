/**
 * \file timer_hw.c
 *
 * \brief Handler timer functionalities
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
 *
 * \asf_license_stop
 */

/* === INCLUDES ============================================================ */

#include "asf.h"
#include "timer_hw.h"
#include "conf_timer.h"

/* === TYPES =============================================================== */
uint32_t timeout_count;
hw_timer_callback_t timer_callback;
/* === MACROS ============================================================== */

void tc_cc0_cb(struct tc_module *const module_inst);


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

	tc_init(&tc_instance, CONF_TC_MODULE, &config_tc);

	tc_enable(&tc_instance);
	
	tc_register_callback(&tc_instance, tc_cc0_cb,
						TC_CALLBACK_CC_CHANNEL0);
}

void hw_timer_register_callback(hw_timer_callback_t cb_ptr)
{
	timer_callback = cb_ptr;
}

void tc_cc0_cb(struct tc_module *const module_inst)
{
	static uint16_t tc_count;
	tc_set_count_value(&tc_instance, 0);
	tc_count += 1;
	if (tc_count >= timeout_count)
	{
		tc_count = 0;
		if (timer_callback != NULL)
		{
			timer_callback();
		}
	}
}

void hw_timer_start(uint32_t timer_val)
{
	timeout_count = timer_val;
	tc_set_count_value(&tc_instance, 0);
	tc_enable_callback(&tc_instance, TC_CALLBACK_CC_CHANNEL0);
}

void hw_timer_stop(void)
{
	tc_disable_callback(&tc_instance, TC_CALLBACK_CC_CHANNEL0);
}


/* EOF */
