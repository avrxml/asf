/**
 * \file
 *
 * \brief Class B application error insertion
 *
 * Copyright (c) 2010-2018 Microchip Technology Inc. and its subsidiaries.
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
 * \page tests_page Class B Test files
 *
 * The following files are taken from Application Note 1610:
 *
 * - Common
 *   - classb_rtc_common.h     Settings for the Real Time Counter.
 *   - classb_rtc_common.c     Optional driver for the Real Time Counter.
 *   - error_handler.h         Macros and definitions related to error handlers
 *                             and configurable actions.
 *
 * - Analog I/O
 *   - classb_analog.h         Header file with settings for the ADC and DAC.
 *   - classb_analog.c         ADC and DAC test.
 *
 * - CRC Test
 *   - classb_crc.h            Header file with settings for the CRC tests.
 *   - classb_crc_sw.h         Header file for software implementation of CRC.
 *   - classb_crc_sw.c         Software implementation of CRC.
 *   - classb_crc_hw.h         Header file for the CRC hardware module.
 *   - classb_crc_hw.c         Driver for the CRC hardware module.
 *
 * - CPU Register Test
 *   - classb_cpu.h            Header file with settings for the CPU registers
 *                             test.
 *   - classb_cpu_gcc.c        Register test (GCC compiler).
 *
 * - CPU Frequency Test
 *   - classb_freq.h           Header file with settings for the frequency test
 *   - classb_freq.c           ISR and routine for checking the CPU frequency.
 *
 * - Interrupt frequency monitor
 *   - classb_interrupt_monitor.h Header file with settings for the interrupt
 *                                monitor.
 *   - classb_interrupt_monitor.c Functions related to the interrupt monitor.
 *
 * - SRAM MarchX Test
 *   - classb_sram.h           Header file with settings for the SRAM test.
 *   - classb_sram.c           Internal SRAM test.
 *
 * - Watchdog Timer Test
 *   - classb_wdt_test.h       Header file with settings for the WDT test.
 *   - classb_wdt_test.c       WDT test that executes after reset.
 */

/**
 * \page classb_modules Class B Modules
 *
 * Class B Test routines
 *
 * Documentation about the class B tests can be found here:
 * \li \ref rtc_driver
 * \li \ref error_handler
 * \li \ref adc_dac
 * \li \ref classb_crc
 * \li \ref classb_registers
 * \li \ref cpu_freq
 * \li \ref int_mon
 * \li \ref classb_sram
 * \li \ref classb_wdt
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef OVEN_CLASSB_H
#define OVEN_CLASSB_H

void oven_classb_init_tests(void);
void oven_classb_run_tests(void);
void oven_classb_error_insertion(void);
void oven_classb_display_error(void);

#endif /* OVEN_CLASSB_H */
