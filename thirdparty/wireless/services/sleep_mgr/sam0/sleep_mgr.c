/**
 * @file sleep_mgr.c
 *
 * @brief
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
 *
 */

/*
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
	configure_rtc_callbacks();
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
