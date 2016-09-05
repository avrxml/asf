/**
 * \file
 *
 * \brief SAM AON Sleep Timer Driver basic for SAMB11
 *
 * Copyright (C) 2016 Atmel Corporation. All rights reserved.
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
#include "aon_sleep_timer.h"
#include "aon_sleep_timer_basic.h"
#include "conf_aon_sleep_timer.h"

/**
 * \aon timemr init & start function
 */

aon_timer_status aon_status = AON_TIMER_IDLE;

bool aon_sleep_timer_service_init(uint32_t _sec)
{
	#define CONF_AON_SLEEP_COUNTER      32768    /* About 1s */
	struct aon_sleep_timer_config config;
	
	if(_sec <= 0)
		_sec = 1;

	aon_sleep_timer_get_config_defaults(&config);
	config.wakeup = AON_SLEEP_TIMER_WAKEUP_ARM_BLE;
	config.mode = AON_SLEEP_TIMER_RELOAD_MODE;//AON_SLEEP_TIMER_SINGLE_MODE
	config.counter = _sec * CONF_AON_SLEEP_COUNTER;
	aon_sleep_timer_init(&config);
	
	return true;
}

bool aon_sleep_timer_service_start(aon_sleep_timer_callback_t callback)
{
	aon_sleep_timer_register_callback(callback);
	NVIC_EnableIRQ(AON_SLEEP_TIMER0_IRQn);
		
	while(!aon_sleep_timer_sleep_timer_active())
	
	aon_status = AON_TIMER_ACTIVE;
	return true;
}

/**
 * \aon timemr stop
 */
void aon_sleep_timer_service_stop()
{	
	if( aon_status != AON_TIMER_ACTIVE )
		return ;
	
	aon_status = AON_TIMER_IDLE;
	
	aon_sleep_timer_disable();
	aon_sleep_timer_unregister_callback();
	NVIC_DisableIRQ(AON_SLEEP_TIMER0_IRQn);
}

