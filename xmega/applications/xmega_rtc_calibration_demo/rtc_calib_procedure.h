/**
 * \file
 *
 * \brief XMEGA RTC calibration header file
 *        This file contains the function prototypes and Macros.
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

#ifndef RTC_CALIB_PROCEDURE_H
#define RTC_CALIB_PROCEDURE_H

/* ! \brief Timer-1 used in calibration procedure, it it TCC4 */
#define TIMER1_FOR_CALIB        TCC4

/* ! \brief Timer-2 used in calibration procedure, it it TCC5 */
#define TIMER2_FOR_CALIB        TCC5

/**
 * \brief RTC Peripheral value to be loaded.
 *        Load RTC period register so that the duration of the period is equal
 *        to at least 1 million ticks of the system clock. Here RTC clock
 *        selected 32768 Hz. For simple calculation, configure RTC overflow
 *        at 1 sec,which is 0x8000 counts.
 *        For this configuration, with 1MHz system clock,timer captured ticks
 *		  will be 1 million. If 2 MHz, it will be 2 millions ticks etc..
 */
#define RTC_PER_VAL_FOR_1_SEC 0x8000

/* ! \brief Macro for sixteen */
#define SIXTEEN_TIMES 16

/* ! \brief Macro for one million */
#define ONE_MILLION 1000000

/* ! \brief Macro 0x00 */
#define CLEAR_ALL_BITS 0x00

/* ! \brief Maximum value of calibration, which can be calibrated */
#define MAXIMUM_CALIB_VAL 0x7F

/* ! \brief Calibrtion status for sucess */
#define RTC_CALIB_STAT_SUCCESS 0x01

/* ! \brief Calibrtion status for failure  */
#define RTC_CALIB_STAT_FAILURE 0x02

/* FUNCTION PROTOTYPES */

/*! \brief Function to configure RTC clock */
extern void rtccalib_config_rtc_clock(void);

/*! \brief Function to configure and start timer 1 */
extern void rtccalib_config_start_timer_one(void);

/*! \brief Function to configure and start timer 2 */
extern void rtccalib_config_start_timer_two(void);

/*! \brief Function to configure port pin PC7 */
extern void rtccalib_config_portpin_pc7(void);

/*! \brief Function to configure event channels */
extern void rtccalib_config_event_channels(void);

/*! \brief Function to start RTC */
extern void rtccalib_start_rtc(void);

/*! \brief Function to synchronize RTC start and timer start */
extern void rtccalib_synch_capture(void);

/*! \brief Function to store timer 1 and 2 capture value */
extern void rtccalib_store_capture_val(void);

/*! \brief Function calculate RTC clock error in ppm */
extern void rtccalib_ppm_error_calc(void);

/*! \brief Function store calculated ppm value in EEPROM */
extern void rtccalib_store_ppm_val(void);

#endif /* RTC_CALIB_PROCEDURE_H */
