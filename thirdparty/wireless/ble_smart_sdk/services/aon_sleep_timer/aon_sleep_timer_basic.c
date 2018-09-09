/**
 * \file
 *
 * \brief SAM AON Sleep Timer Driver basic for SAMB11
 *
 * Copyright (c) 2016-2018 Microchip Technology Inc. and its subsidiaries.
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
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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

