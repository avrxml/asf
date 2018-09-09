/**
 * \file
 *
 * \brief XMEGA RTC calibration demo application main file
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
 *  For further information, visit <a href="http://www.microchip.com/">Microchip</a>.\n
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
