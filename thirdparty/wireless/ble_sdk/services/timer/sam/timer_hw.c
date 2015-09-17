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

void hw_timer_init(void)
{
	sysclk_enable_peripheral_clock(ID_TC);
	// Init timer counter  channel.
	tc_init(TIMER, TIMER_CHANNEL_ID,						
			TC_CMR_TCCLKS_TIMER_CLOCK1 | TC_CMR_WAVE |
			TC_CMR_WAVSEL_UP);				
	
	tc_write_rc(TIMER, TIMER_CHANNEL_ID, UINT16_MAX);
	tc_get_status(TIMER, TIMER_CHANNEL_ID);
	tc_enable_interrupt(TIMER, TIMER_CHANNEL_ID, TC_IER_COVFS);		
	NVIC_EnableIRQ(TC0_IRQn);
}

void hw_timer_register_callback(hw_timer_callback_t cb_ptr)
{
	timer_callback = cb_ptr;
}

void TC0_Handler(void)
{
	uint32_t ul_status;
	static uint16_t tc_count;
	uint8_t flags = cpu_irq_save();
	
	ul_status = tc_get_status(TIMER, TIMER_CHANNEL_ID);
	ul_status &= tc_get_interrupt_mask(TIMER, TIMER_CHANNEL_ID);
	
	/* ovf callback */
	if (TC_SR_COVFS == (ul_status & TC_SR_COVFS)) 
	{		
		tc_count++;
		
		if (tc_count >= timeout_count)
		{
			tc_count = 0;
			if (timer_callback != NULL)
			{
				timer_callback();
			}
		}
	}
	
	cpu_irq_restore(flags);
}

void hw_timer_start(uint32_t timer_val)
{
	timeout_count = (timer_val*TIMER_OVF_COUNT_1SEC);
	tc_start(TIMER, TIMER_CHANNEL_ID);
}

void hw_timer_stop(void)
{
	tc_stop(TIMER, TIMER_CHANNEL_ID);
}


/* EOF */
