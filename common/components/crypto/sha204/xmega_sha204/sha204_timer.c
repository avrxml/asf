/*
 * \file
 *
 * \brief XMEGA-A1 Xplained timer utility functions.
 *
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "tc.h"

#define DELAY_COUNTER (&TCC1)

//! timer expired flag set by overflow interrupt call back function
volatile bool sha204_timer_expired = false;

void sha204h_timer_init(void);
void sha204h_start_timer(uint16_t delay, uint8_t is_blocking);
void sha204h_delay_ms(uint16_t delay);
void sha204h_start_timeout_timer_ms(uint16_t timeout);


/**
 * \brief Timer Counter Overflow interrupt call back function
 *
 * This function is called when an overflow interrupt has occurred on
 * \ref DELAY_COUNTER.
 */
static void sha204h_timer_overflow_interrupt_callback(void)
{
	sha204_timer_expired = true;
}


/** 
 * \brief This function initializes a delay timer.
 */
void sha204h_timer_init(void)
{
	// Enable timer.
	tc_enable(DELAY_COUNTER);
	
	// Enable overflow interrupt
	tc_set_overflow_interrupt_level(DELAY_COUNTER, PMIC_LVL_LOW);
	
	// Configure TC in normal mode
	tc_set_wgm(DELAY_COUNTER, TC_WG_NORMAL);
	
	// Set call back function for timer interrupt.
	tc_set_overflow_interrupt_callback(DELAY_COUNTER, sha204h_timer_overflow_interrupt_callback);
}


/** \brief This function delays for a number of microseconds (not compiled).
 * \param[in] delay number of microseconds to delay
 */
#if 0
void sha204_delay_us(uint16_t delay)
{
	// Configure TC resolution and period (value at which overflow is triggered).
	tc_set_resolution(DELAY_COUNTER, 1000000); // causes a divisor value of 8 => 250 ns for system clock = 32 MHz
	
	// Get actual resolution.
	uint32_t resolution = tc_get_resolution(DELAY_COUNTER);
	
	// Write value for delay to counter register.
	tc_write_period(DELAY_COUNTER, (resolution * (uint32_t) delay) / 1000000);
	
	// Restart counter.
	tc_restart(DELAY_COUNTER);
	
	// Reset timeout flag.
	sha204_timer_expired = false;
	
	// Wait for timer to expire.
	while (sha204_timer_expired == false);
}
#endif


/** 
 * \brief This function starts a milliseconds timer.
 *
 * \param[in] delay number of milliseconds to delay
 * \param[in] is_blocking true: returns after delay; false: returns immediately
 */
void sha204h_start_timer(uint16_t delay, uint8_t is_blocking)
{
	// Configure TC resolution and period (value at which overflow is triggered).
	tc_set_resolution(DELAY_COUNTER, 1000); // causes a divisor value of 1024 => 32 us for system clock = 32 MHz

	// Get actual resolution.
	uint32_t resolution = tc_get_resolution(DELAY_COUNTER);
	
	// Write value for delay to counter register.
	tc_write_period(DELAY_COUNTER, (resolution * (uint32_t) delay) / 1000);
	
	// Restart counter.
	tc_restart(DELAY_COUNTER);
	
	// Reset timeout flag.
	sha204_timer_expired = false;
	
	if (is_blocking)
	   // Wait for timer to expire.
	   while (sha204_timer_expired == false);
}


/** 
 * \brief This function delays for a number of milliseconds.
 *
 *         You can override this function if you like to do
 *         something else in your system while delaying.
 * \param[in] delay number of milliseconds to delay
 */
void sha204h_delay_ms(uint16_t delay)
{
	sha204h_start_timer(delay, true);
}


/** 
 * \brief This function starts a timeout timer.
 * \param[in] timeout number of milliseconds before timing out
 */
void sha204h_start_timeout_timer_ms(uint16_t timeout)
{
	sha204h_start_timer(timeout, false);
}
