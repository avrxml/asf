/**
 * \file
 *
 * \brief This file contains the function implementations for XMEGA RTC
 *        calibration demo application.It explain on how to calibrate the
 *        RTC clock source by using an external accurate clock source.
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
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

#include <compiler.h>
#include <sleepmgr.h>
#include <sysclk.h>
#include <conf_rtc_calibration.h>
#include <rtc_calib_procedure.h>
#include <asf.h>

/* ! \brief Static variable to store timer-0 capture value */
static uint16_t rtccalib_tim1_capture_value = 0;

/* ! \brief Static variable to store timer-1 capture value */
static uint16_t rtccalib_tim2_capture_value = 0;

/**
 * \brief Static variable to store MSB of timer-0 and timer-1 combined
 *        capture value( i.e. MSB of 32 bit timer capture value)
 */
static uint32_t rtccalib_32bit_timer_msb_value = 0;

/**
 * \brief Static variable to store 32 bit timer capture value
 *        ( i.e. timer-0 and timer-1 combined capture value)
 */
static uint32_t rtccalib_32bit_timer_value = 0;

/**
 * \brief Static variable to store difference between expected number of 32 bit
 *        timer value and actual captured 32 bit timer value at RTC overflow
 */
static int32_t rtccalib_rtc_clk_diff = 0;

/* ! \brief Static variable to store sign of calibration value */
static uint8_t rtccalib_correc_sign = 0;

/* ! \brief Static variable to store RTC clock error in ppm */
static uint32_t rtccalib_rtc_clk_error_inppm = 0;

/**
 * \brief Static variable to store RTC calibration value with sign bit.This
 *        value will be stored in EEPROM
 */
static uint8_t rtccalib_rtc_clk_calib_val = 0;

/**
 * \brief Static variable to store status of calibration,success or failure.
 *        If error in ppm is greater than 127,means unable to calibrate and
 *        hence status will be failure.
 *        Success indicated as 0x01 and failure as 0x02
 */
static uint8_t rtccalib_rtc_clk_calib_status = 0;

/**
 * \brief This function configure the RTC clock source
 */
void rtccalib_config_rtc_clock(void)
{
	/* Select RTC clock source and enable RTC */
	CLK.RTCCTRL = RTCCALIB_RTC_CLK_SRC | CLK_RTCEN_bm;
}

/**
 * \brief This function configure and start timer1,which is TC4
 *  - Enable timer-1
 *  - Configure as pulse-width capture event action with event channel 1
 *    as event selection
 *  - Configure clock source as system clock with prescalar 1 and start on
 *    next event
 *  - Enable capture channel A
 */
void rtccalib_config_start_timer_one(void)
{
	/* Enable Timer-1 */
	tc45_enable(&TIMER1_FOR_CALIB);

	/* Configure event action as pulse-width capture with event channel 1
	 * as event selection */
	tc45_set_input_capture(&TIMER1_FOR_CALIB, TC45_EVSEL_CH1_gc,
			TC45_EVACT_PWF_gc);

	/* Configure timer-1 clock source */
	tc45_write_clock_source(&TIMER1_FOR_CALIB, TC45_CLKSEL_DIV1_gc);

	/* Configure to start on next event */
	tc45_set_evstart(&TIMER1_FOR_CALIB);

	/* Enable capture channel A */
	tc45_enable_cc_channels(&TIMER1_FOR_CALIB, TC45_CCACAPT);
}

/**
 * \brief This function configure and start timer2,which is TC5
 *  - Enable timer-2
 *  - Configure as pulse-width capture event action with event channel 1
 *    as event selection
 *	- Configure for timer delay event
 *  - Configure clock source as event channel 2 and start on next event
 *  - Enable capture channel A
 */
void rtccalib_config_start_timer_two(void)
{
	/* Enable Timer-1 */
	tc45_enable(&TIMER2_FOR_CALIB);

	/* Configure event action as pulse-width capture with event channel 1
	 * as event selection */
	tc45_set_input_capture(&TIMER2_FOR_CALIB, TC45_EVSEL_CH1_gc,
			TC45_EVACT_PWF_gc);

	/* Configure for timer delay event */
	tc45_enable_delay(&TIMER2_FOR_CALIB);

	/* Configure timer-1 clock source event channel 2 */
	tc45_write_clock_source(&TIMER2_FOR_CALIB, TC45_CLKSEL_EVCH2_gc);

	/* Configure to start on next event */
	tc45_set_evstart(&TIMER2_FOR_CALIB);

	/* Enable capture channel A */
	tc45_enable_cc_channels(&TIMER2_FOR_CALIB, TC45_CCACAPT);
}

/**
 * \brief This function configure port pin PC7
 *  - Set PC7 direction as output
 *  - Configure PC7 for rising edge as it's input sense configuration
 *  - Configure to out event channel 0 on PC7
 */
void rtccalib_config_portpin_pc7(void)
{
	/* Set PC7 direction as output */
	PORTC.DIRSET = PIN7_bm;

	/* Configure PC7 for input sense configuration in rising edge */
	PORTC.PIN7CTRL = PORT_ISC_RISING_gc;

	/* Configure to out event channel 0 on PC7 */
	PORTCFG.ACEVOUT |= PORTCFG_EVOUT_PC7_gc;
}

/**
 * \brief This function configure the event channels 0, 1 and 2
 *  - Configure Event channel 0 as RTC overflow
 *  - Configure Event channel 1 as PortC, Pin7 (PC7) change
 *  - Configure Event channel 2 as timer-1(TC4) overflow
 */
void rtccalib_config_event_channels(void)
{
	/* Configure Event channel 0 as RTC overflow */
	EVSYS.CH0MUX = EVSYS_CHMUX_RTC_OVF_gc;

	/* Configure Event channel 1 as PortC, Pin7 (PC7) change  */
	EVSYS.CH1MUX = EVSYS_CHMUX_PORTC_PIN7_gc;

	/* Configure Event channel 2 as timer-1(TC4) overflow */
	EVSYS.CH2MUX = EVSYS_CHMUX_TCC4_OVF_gc;
}

/**
 * \brief This function will start RTC timer
 *  - Load RTC period register
 *  - Configure RTC clock prescalar
 */
void rtccalib_start_rtc(void)
{
	/* Load RTC period register so that the duration of the period is equal
	 * to at least 1 million ticks of the system clock. Here RTC clock
	 * selected as 32768 Hz.So configure RTC overflow at 1 sec,
	 * which is 0x8000 counts
	 * For this configuration, with 1MHz system clock, timer captured ticks
	 * will be 1 million. If 2 MHz, it will be 2 millions ticks etc..
	 */
	RTC.PER = RTC_PER_VAL_FOR_1_SEC;

	/* Configure RTC clock prescalar */
	RTC.CTRL = RTC_PRESCALER_DIV1_gc;
}

/**
 * \brief This function will synchronize between RTC start and Timer start
 *  - Restart timer 1 and 2
 *  - Wait for timer1 capture interrupt flag on channel CCA is set
 *	- Clear timer1 capture interrupt flag on channel CCA
 */
void rtccalib_synch_capture(void)
{
	/* Restart timer 1 (TC4) */
	tc45_restart(&TIMER1_FOR_CALIB);

	/* Restart timer 2 (TC5) */
	tc45_restart(&TIMER2_FOR_CALIB);

	/* Wait until timer1 capture interrupt flag on channel CCA is set */
	do {} while ((tc45_is_cc_interrupt(&TIMER1_FOR_CALIB, TC45_CCA)) == 0);

	/* Clear timer1 capture interrupt flag on channel CCA */
	tc45_clear_cc_interrupt(&TIMER1_FOR_CALIB, TC45_CCA);
}

/**
 * \brief This function will Read timer-1 and timer-2 capture values
 *        and disable RTC
 *  - Wait until timer1 capture interrupt flag on channel CCA is set
 *  - Read timer-1 and timer-2 capture values and store in variables
 *	- Disable RTC
 */
void rtccalib_store_capture_val(void)
{
	/* Wait until timer1 capture interrupt flag on channel CCA is set */
	do {} while ((tc45_is_cc_interrupt(&TIMER1_FOR_CALIB, TC45_CCA)) == 0);

	/* Read timer-1 capture values and store */
	rtccalib_tim1_capture_value = tc45_read_cc(&TIMER1_FOR_CALIB, TC45_CCA);

	/* Read timer-2 capture values and store */
	rtccalib_tim2_capture_value = tc45_read_cc(&TIMER2_FOR_CALIB, TC45_CCA);

	/* Disable RTC  */
	sysclk_rtcsrc_disable();
}

/**
 * \brief This function will calculate RTC clock error in ppm
 *        (calibration value) from timer captured values
 *  - Make 32 bit timer capture values from 16 bit timer-1 and 2 capture values
 *  - Find difference between expected number of 32 bit timer value and
 *    actual captured 32 bit timer value at RTC overflow
 *  - Find the sign of calibration value from the above difference
 *  - Find RTC calibration value in ppm that should be loaded to CALIB register
 */
void rtccalib_ppm_error_calc(void)
{
	/* Find MSB (Higher 2 bytes of 32 bit) of timer-0 and timer-1 combined
	 * capture value
	 */
	rtccalib_32bit_timer_msb_value
		= (uint32_t)rtccalib_tim2_capture_value <<
			SIXTEEN_TIMES;

	/* Find 32 bit timer capture value*/
	rtccalib_32bit_timer_value = (uint32_t)
			(((uint32_t)rtccalib_tim1_capture_value) |
			rtccalib_32bit_timer_msb_value);

	/* Find difference between expected number of 32 bit timer value and
	 * actual captured 32 bit timer value at RTC overflow.
	 * That is,Error = Expected value - Read value
	 */
	rtccalib_rtc_clk_diff
		= (int32_t)((RTCCALIB_SYSTEM_CLOCK_IN_MHZ * ONE_MILLION) -
			rtccalib_32bit_timer_value);

	/* Find sign of calibration value from the above difference
	 * Case1 :If read value is more than expected ,means RTC clock is
	 * slower,
	 * Hence need to make RTC clock fast. That is sing bit is 1.
	 * Case2 :If read value is less than expected ,means RTC clock is
	 * faster,
	 * Hence need to make RTC clock slower. That is sing bit is 0.
	 */
	if (rtccalib_rtc_clk_diff < 0) {
		/* Make sing bit as 1, to make RTC clock faster */
		rtccalib_correc_sign = 1;
	} else {
		/* Make sing bit as 0, to make RTC clock slower */
		rtccalib_correc_sign = 0;
	}

	/* Find RTC calibration value in ppm, i.e error per million */
	rtccalib_rtc_clk_error_inppm = (abs(rtccalib_rtc_clk_diff)) /
			RTCCALIB_SYSTEM_CLOCK_IN_MHZ;

	/* Check calibration status is success or failure
	 * If error in ppm is greater than 127,means unable to calibrate and
	 * hence status will be failure.
	 * Success indicated as 0x01 and failure as 0x02
	 */
	if (rtccalib_rtc_clk_error_inppm <= MAXIMUM_CALIB_VAL) {
		/* Calibration status is success */
		rtccalib_rtc_clk_calib_status = RTC_CALIB_STAT_SUCCESS;
	} else {
		/* Calibration status is failure */
		rtccalib_rtc_clk_calib_status = RTC_CALIB_STAT_FAILURE;
	}

	/* Find RTC calibration value in ppm by adding sign bit,
	 * which will be loaded to CALIB register
	 */
	if (rtccalib_rtc_clk_calib_status == RTC_CALIB_STAT_SUCCESS) {
		/* Find calibration value, only if calibration is sucess */
		rtccalib_rtc_clk_calib_val
			= (rtccalib_correc_sign << RTC_SIGN_bp) |
				rtccalib_rtc_clk_error_inppm;
	} else {
		/* Assign zero if failure */
		rtccalib_rtc_clk_calib_val = CLEAR_ALL_BITS;
	}
}

/**
 * \brief This function will store the calculated calibration value in EEPROM
 *  - Erase all eeprom locations
 *  - Store the calculated calibration value in EEPROM
 */
void rtccalib_store_ppm_val(void)
{
	/* Erase all eeprom locations */
	nvm_eeprom_erase_all();

	/* Store the calculated calibration value in EEPROM at configured
	 * EEPROM address
	 */
	nvm_eeprom_write_byte(RTCCALIB_EEPROM_ADDR_TO_STORE,
			rtccalib_rtc_clk_calib_val);

	/* Store the status of RTC calibration in EEPROM at configured
	 * EEPROM address
	 * Success indicated as 0x01 and failure as 0x02
	 */
	nvm_eeprom_write_byte(RTCCALIB_STAT_EEPROM_ADDR,
			rtccalib_rtc_clk_calib_status);
}
