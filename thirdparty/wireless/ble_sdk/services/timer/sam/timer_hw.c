/**
 * \file timer_hw.c
 *
 * \brief Handler timer functionalities
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
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
uint32_t timeout_count, bus_timeout_count;
hw_timer_callback_t timer_callback;
platform_hw_timer_callback_t bus_timer_callback;
static volatile bool platform_timer_used = false;
/* === MACROS ============================================================== */

void hw_timer_init(void)
{
	sysclk_enable_peripheral_clock(ID_TC0);
	// Init timer counter  channel.
	tc_init(BLE_APP_TIMER, BLE_APP_TIMER_CHANNEL_ID,						
			TC_CMR_TCCLKS_TIMER_CLOCK4 |
			TC_CMR_WAVSEL_UP);				
	
	tc_write_rc(BLE_APP_TIMER, BLE_APP_TIMER_CHANNEL_ID, UINT16_MAX);
	tc_get_status(BLE_APP_TIMER, BLE_APP_TIMER_CHANNEL_ID);
	tc_enable_interrupt(BLE_APP_TIMER, BLE_APP_TIMER_CHANNEL_ID, TC_IER_COVFS);		
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
	
	ul_status = tc_get_status(BLE_APP_TIMER, BLE_APP_TIMER_CHANNEL_ID);
	ul_status &= tc_get_interrupt_mask(BLE_APP_TIMER, BLE_APP_TIMER_CHANNEL_ID);
	
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
}

void hw_timer_start(uint32_t timer_val)
{
	timeout_count = (timer_val*TIMER_OVF_COUNT_1SEC);
	tc_start(BLE_APP_TIMER, BLE_APP_TIMER_CHANNEL_ID);
}

void hw_timer_stop(void)
{
	tc_stop(BLE_APP_TIMER, BLE_APP_TIMER_CHANNEL_ID);
}

void *platform_configure_timer(platform_hw_timer_callback_t bus_tc_cb_ptr)
{
	Disable_global_interrupt();
	if (platform_timer_used == false)
	{
		platform_timer_used = true;
		bus_timer_callback = bus_tc_cb_ptr;
		sysclk_enable_peripheral_clock(BUS_TIMER_ID);
		// Init timer counter channel.
		tc_init(BUS_TIMER, BUS_TIMER_CHANNEL_ID,
		TC_CMR_TCCLKS_TIMER_CLOCK4 |
		TC_CMR_WAVSEL_UP);
		
		tc_write_rc(BUS_TIMER, BUS_TIMER_CHANNEL_ID, UINT16_MAX);
		tc_get_status(BUS_TIMER, BUS_TIMER_CHANNEL_ID);
		tc_enable_interrupt(BUS_TIMER, BUS_TIMER_CHANNEL_ID, TC_IER_CPCS);
		NVIC_EnableIRQ(TC1_IRQn);
		tc_stop(BUS_TIMER, BUS_TIMER_CHANNEL_ID);
		Enable_global_interrupt();
		return (void *)bus_timer_callback;
	}
	Enable_global_interrupt();
	return NULL;	
}

void TC1_Handler(void)
{
	uint32_t ul_status;
	
	ul_status = tc_get_status(BUS_TIMER, BUS_TIMER_CHANNEL_ID);
	ul_status &= tc_get_interrupt_mask(BUS_TIMER, BUS_TIMER_CHANNEL_ID);
	
	/* RC Compare callback */
	if (TC_SR_CPCS == (ul_status & TC_SR_CPCS))
	{
		tc_stop(BUS_TIMER, BUS_TIMER_CHANNEL_ID);
		bus_timer_callback(BUS_TIMER);		
	}
}

void platform_start_bus_timer(void *timer_handle, uint32_t ms)
{
	(void) timer_handle;
	if (ms > 65)
	{
		/* handle using software timer. currently not supported by hardware timer for more than 65536ms */
		while(1);
	}
	bus_timeout_count = (ms*TIMER_OVF_COUNT_1MSEC) + tc_read_cv(BUS_TIMER, BUS_TIMER_CHANNEL_ID);
	tc_write_rc(BUS_TIMER, BUS_TIMER_CHANNEL_ID, bus_timeout_count);
	tc_start(BUS_TIMER, BUS_TIMER_CHANNEL_ID);
}

void platform_delete_bus_timer(void *timer_handle)
{
	(void) timer_handle;
	tc_stop(BUS_TIMER, BUS_TIMER_CHANNEL_ID);
}


void platform_stop_bus_timer(void *timer_handle)
{
	(void) timer_handle;
	tc_stop(BUS_TIMER, BUS_TIMER_CHANNEL_ID);
}

/* EOF */
