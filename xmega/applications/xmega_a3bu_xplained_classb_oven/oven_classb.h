/**
 * \file
 *
 * \brief Class B application error insertion
 *
 * Copyright (C) 2010-2015 Atmel Corporation. All rights reserved.
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
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef OVEN_CLASSB_H
#define OVEN_CLASSB_H

void oven_classb_init_tests(void);
void oven_classb_run_tests(void);
void oven_classb_error_insertion(void);
void oven_classb_display_error(void);

#endif /* OVEN_CLASSB_H */
