/**
 * @file sleep_mgr.c
 *
 * @brief
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
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
 *
 */

/*
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

#include "sleep_mgr.h"
#include "rtc_count.h"
#include "system.h"
#include "rtc_count_interrupt.h"
#include "asf.h"

struct rtc_module rtc_instance;

/**
 * @brief Configuring RTC Callback Function on Overflow
 *
 * @param void
 */
static void configure_rtc_callbacks(void);

/**
 * @brief Callback Function indicating RTC Overflow
 *
 * @param void
 */
static void rtc_overflow_callback(void);

/**
 * \brief This function Initializes the Sleep functions
 * Enable RTC Clock in conf_clocks.h
 */
void sm_init(void)
{
	struct rtc_count_config config_rtc_count;

	rtc_count_get_config_defaults(&config_rtc_count);
	config_rtc_count.prescaler           = RTC_COUNT_PRESCALER_DIV_1;
	config_rtc_count.mode                = RTC_COUNT_MODE_16BIT;

#ifdef FEATURE_RTC_CONTINUOUSLY_UPDATED
	/** Continuously update the counter value so no synchronization is
	 *  needed for reading. */
	config_rtc_count.continuously_update = true;
#endif
	rtc_count_init(&rtc_instance, RTC, &config_rtc_count);
	configure_rtc_callbacks();
}

/**
 * \brief This function puts the transceiver and device to sleep
 */
void sm_sleep(uint32_t interval)
{
	interval = interval * 1000;
	rtc_count_set_period(&rtc_instance, interval);
	rtc_count_enable(&rtc_instance);
	/*put the MCU in standby mode with RTC as wakeup source*/
	system_set_sleepmode(SYSTEM_SLEEPMODE_STANDBY);
	system_sleep();
}

static void configure_rtc_callbacks(void)
{
	/*Register rtc callback*/
	rtc_count_register_callback(
			&rtc_instance, rtc_overflow_callback,
			RTC_COUNT_CALLBACK_OVERFLOW);
	rtc_count_enable_callback(&rtc_instance, RTC_COUNT_CALLBACK_OVERFLOW);
}

static void rtc_overflow_callback(void)
{
	/* Do something on RTC overflow here */
	rtc_count_disable(&rtc_instance);
}
