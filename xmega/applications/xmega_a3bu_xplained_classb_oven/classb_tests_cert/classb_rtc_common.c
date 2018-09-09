/**
 * \file
 *
 * \brief RTC callback function.
 *
 * \par Application note:
 *      AVR1610: Guide to IEC60730 Class B compliance with XMEGA
 *
 * Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries.
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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include "classb_rtc_common.h"
#include "error_handler.h"
#include <rtc32.h>

/**
 * \addtogroup rtc_driver
 * @{
 */

/**
 * \brief This is the RTC compare interrupt.
 *
 * Actions required in Class B tests are implemented as callbacks. Note that
 * this requires a number of symbols to be defined, either in a header file or
 * at the compiler level.
 *
 * It is possible to add user-defined code to the RTC interrupt through
 * \ref CLASSB_ACTIONS_RTC().
 *
 * \param time Time parameter passed by RTC driver. Unused here.
 */
void classb_rtc_callback(uint32_t time)
{
#ifdef CLASSB_FREQ_TEST
	classb_freq_callback();
#endif

#ifdef CLASSB_INT_MON
	classb_intmon_callback();
#endif

	/* User-configurable actions */
	rtc_set_alarm_relative(CLASSB_RTC_INT_PERIOD);
	CLASSB_ACTIONS_RTC();
}

/** @} */
