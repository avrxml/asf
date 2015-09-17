/**
 * \file
 *
 * \brief Configuration file for RTC calibration demo application
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

#ifndef CONF_RTC_CALIBRATION_H
#define CONF_RTC_CALIBRATION_H

/**
 * \name RTC calibration configuration
 */
/* ! @{ */

/**
 * \brief System clock in MHz value. For example,
 *        if system clock is 1 MHz, this macro should be 1
 *        if system clock is 2 MHz, this macro should be 2, etc..
 *        Please note that configure system clock only in whole number
 *		  in MHz.Example, 1MHz or 2MHz or 3MHz etc...
 */
#define RTCCALIB_SYSTEM_CLOCK_IN_MHZ    2

/**
 * \brief EEPROM address at which RTC calibration value will be stored.
 *        At the end of calibration procedure,calibration value will be stored
 *		  in this EEPROM address of device.
 *        Later, user application code has to read this calibration value from
 *        this EEPROM location and load in CALIB register.
 */
#define RTCCALIB_EEPROM_ADDR_TO_STORE    0

/* ! \brief EEPROM address at which RTC calibration status will be stored. */
#define RTCCALIB_STAT_EEPROM_ADDR    1

/* ! \brief RTC clock source configuration */
#define RTCCALIB_RTC_CLK_SRC CLK_RTCSRC_EXTCLK_gc

/* ! @} */

#endif /* CONF_RTC_CALIBRATION_H */
