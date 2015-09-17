/**
 * \file
 *
 * \brief XMEGA RTC calibration demo application main file
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

/**
 * \mainpage
 *
 * \section intro Introduction
 *  XMEGA E provides a feature that RTC peripheral can do an internal
 *  correction on the RTC clock by taking the PPM error value from
 *  the CALIB Register : \n
 *  Prior to this user has to find calibration value in PPM and same
 *  has to be loaded in CALIB register : \n
 *  This example project explains how to calibrate the RTC clock source by
 *  by using an external accurate clock source.The calculated calibration value
 *  will be stored in EEPROM location. Later, user application code has to
 *  read this calibration value from EEPROM and load in CALIB register.
 *
 * \section files Main Files
 * - rtc_calib_main.c              : RTC calibration demo application main file
 * - rtc_calib_procedure.c         : Function implementations for XMEGA RTC
 *                                   calibration demo application
 * - conf_rtc_calibration.h        : Configuration Macros for RTC calibration
 *
 * \section device_info Device Info
 *  AVR XMEGA E device. This example have been tested with following setup:
 * - STK600-ATXmega32E5
 *
 * \section description Description of the example
 *
 * - All configurations related such as system clock value, EEPROM location
 *   to store calibration value are available from conf_rtc_calibration.h
 * - To calculate the RTC clock error in ppm, it uses an accurate external
 *   clock which used as system clock.By using TC4,TC5 and event channels along
 *   with external clock, RTC clock error in ppm will be calculated.
 * - The calculated calibration value will be stored in EEPROM location
 * - Also the status of calibration will be stored in EEPROM,success or failure.
 * - Later, user application code has to read this calibration value from
 *   EEPROM and load in CALIB register, if status of calibration is success.
 *
 * \section compinfo Compilation info
 *  This software was written for the GNU GCC and IAR for AVR. Other compilers
 *  may or may not work.
 *
 * \section contactinfo Contact Information
 *  For further information, visit <a href="http://www.atmel.com/">Atmel</a>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <compiler.h>
#include <sleepmgr.h>
#include <sysclk.h>
#include <rtc_calib_procedure.h>
#include <asf.h>

/**
 * \brief Main application routine
 *  - Configure system clock
 *  - Configure and start RTC and Timers
 *  - Configure events and port pin PC7
 *  - Synchronize between RTC start and Timer start
 *  - Read Timer capture values
 *  - Calculate RTC error in ppm(calibration value) from captured values
 *  - Store the calculated calibration value in EEPROM.
 */
int main( void )
{
	/* Configure the system clock */
	sysclk_init();

	/* Configure the RTC clock */
	rtccalib_config_rtc_clock();

	/* Configure and start Timer-1,which is TC4 */
	rtccalib_config_start_timer_one();

	/* Configure and start Timer-2,which is TC5 */
	rtccalib_config_start_timer_two();

	/* Configure port pin PC7 for it's input sense configuration
	 * and to out event channel 0 on port pin PC7 */
	rtccalib_config_portpin_pc7();

	/* Configure the event channels 0, 1 and 2 */
	rtccalib_config_event_channels();

	/* Load RTC period register and start the RTC */
	rtccalib_start_rtc();

	/* Synchronize between RTC start and Timer start */
	rtccalib_synch_capture();

	/* Read Timer-1 and Timer-2 capture values and disable RTC */
	rtccalib_store_capture_val();

	/* Calculate RTC clock error in ppm(calibration value) from captured
	 * values */
	rtccalib_ppm_error_calc();

	/* Store the calculated calibration value in EEPROM */
	rtccalib_store_ppm_val();

	/* Continuous Execution Loop */
	while (1) {
	}
}
