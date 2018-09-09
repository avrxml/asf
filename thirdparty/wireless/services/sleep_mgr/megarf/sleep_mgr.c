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
#include "sleepmgr.h"
#include "conf_sleepmgr.h"
#include "macsc_megarf.h"
# include "sysclk.h"

#define COMPARE_MODE                               MACSC_RELATIVE_CMP

#ifdef SLEEP_MGR_TIMER_RES
#define CONFIG_MACSC_HZ                                                    (1)
#else
#define CONFIG_MACSC_HZ                            (62500)
#endif

static void cmp3_int_cb(void)
{
	/*Disable The MAC Symbol Counter*/
	macsc_disable();
}

/**
 * \brief This function Initializes the Sleep functions
 */
void sm_init(void)
{
	/* Set the sleep mode to initially lock. */
	sleep_set_mode(SLEEP_SMODE_PSAVE);
	sysclk_enable_peripheral_clock(&TCCR2A);
	macsc_write_clock_source(MACSC_32KHz);
	macsc_sleep_clk_enable();
	macsc_set_cmp3_int_cb(cmp3_int_cb);
	macsc_enable_cmp_int(MACSC_CC3);
}

/**
 * \brief This function puts the  device to sleep
 * \param interval : in seconds for the device to sleep.Range of Interval is
 * 1-68719s
 */
void sm_sleep(uint32_t interval)
{
	/*Enable MAC Symbol Counter*/
	macsc_enable();
	/*Timestamp the current symbol counter value for Comparison*/
	macsc_enable_manual_bts();
	macsc_use_cmp(COMPARE_MODE, interval * CONFIG_MACSC_HZ, MACSC_CC3);
	sleep_enable();
	sleep_enter();
}
