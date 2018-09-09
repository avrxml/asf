/**
 * \file
 *
 * \brief Configuration file for RTC calibration demo application
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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
