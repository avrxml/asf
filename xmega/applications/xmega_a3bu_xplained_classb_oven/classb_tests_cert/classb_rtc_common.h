/**
 * \file
 *
 * \brief Settings for the RTC driver.
 *
 * \par Application note:
 *      AVR1610: Guide to IEC60730 Class B compliance with XMEGA
 *
 * Copyright (C) 2011-2015 Atmel Corporation. All rights reserved.
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef CLASSB_RTC_COMMON_H
#define CLASSB_RTC_COMMON_H

#include <asf.h>

/**
 * \defgroup rtc_driver Real Time Counter Driver
 *
 * \brief This is a basic driver for the Real Time Counter (RTC).
 *
 * The RTC is used to generate interrupts periodically for both the CPU
 * frequency and the interrupt self-diagnostic routines. Depending on the
 * specific device, an RTC or an RTC32 will be used. In both cases the RTC is
 * clocked independently from the CPU with a 32.768 kHz oscillator scaled down
 * to 1024 Hz.
 *
 * The interrupt period can be configured in the driver through
 * \ref CLASSB_RTC_INT_PERIOD. Further, the RTC module is setup with
 * \ref classb_rtc_setup(). Note that interrupts should be enabled in the main
 * application with \c sei().
 *
 * Note that it is possible to use a different RTC driver. The only requirement
 * is that it defines the constants for the interrupt period
 * \ref CLASSB_RTC_INT_PERIOD and the RTC frequency \ref CLASSB_RTC_FREQ.
 *
 * @{
 */

/**
 * \name RTC Settings and Class B Interface.
 *
 * These two constants give information about the RTC to the Class B tests.
 * Therefore, they need to be defined with correct values. As long as these are
 * defined, any RTC driver can be used.
 *
 * @{
 */

/**
 * \brief RTC interrupt period.
 *
 * This constant is used to configure the period of the RTC compare interrupt.
 * In addition, it is used in the Class B tests. An example value is \c 2048UL.
 *
 * \note This is a 16- or 32-bit value depending on the RTC hardware module.
 */
#define CLASSB_RTC_INT_PERIOD			2048UL

/**
 * \brief RTC frequency.
 *
 * This gives information to the Class B tests. However, the RTC frequency is
 * hard coded in this driver to 1024 Hz. More details in
 * \ref classb_rtc_setup().
 */
#define CLASSB_RTC_FREQ				1024UL

/** @} */

#ifdef __DOXYGEN__
/**
 * \name Test declarations.
 *
 * These symbols set which Class B tests are included in the RTC interrupt, and
 * can be defined at the compiler level or in this file.
 *
 * @{
 */

/** \brief Test for the CPU frequency. */
#  define CLASSB_FREQ_TEST
/** \brief Interrupt monitor. */
#  define CLASSB_INT_MON

/** @} */
#else
/* #define CLASSB_FREQ_TEST */
/* #define CLASSB_INT_MON */
#endif


/* Include header files for the callbacks in the RTC interrupt. */
#ifdef CLASSB_FREQ_TEST
#  include "classb_freq.h"
#endif
#ifdef CLASSB_INT_MON
#  include "classb_interrupt_monitor.h"
#endif


/* Workaround for missing CNT, PER and COMP in device header files. */
#ifdef RTC32
#  ifdef __ICCAVR__
#    define _DWORDREGISTER DWORDREGISTER
#  endif

	typedef struct RTC32_struct2 {
		register8_t CTRL;
		register8_t SYNCCTRL;
		register8_t INTCTRL;
		register8_t INTFLAGS;
		_DWORDREGISTER(CNT);
		_DWORDREGISTER(PER);
		_DWORDREGISTER(COMP);
	} RTC32_t2;

#  undef RTC32
#  define RTC32 (*(RTC32_t2 *)0x0420)
#endif


/**
 * \name Compatibility macros for RTC and RTC32.
 *
 * Depending on the XMEGA device there will be a 16- or 32-bits RTC.
 * To make this transparent we define the following constants.
 *
 * @{
 */

#if defined(RTC32) || defined(__DOXYGEN__)
/** Name of the RTC module. */
#  define RTC_TEST RTC32
/** Bit mask used to start RTC. */
#  define RTC_TEST_START_bm RTC32_ENABLE_bm
/** Compare interrupt level group configuration */
#  define RTC_TEST_COMPINTLVL_LO_gc RTC32_COMPINTLVL_LO_gc
/** Compare interrupt disable group configuration */
#  define RTC_TEST_COMPINTLVL_OFF_gc RTC32_COMPINTLVL_OFF_gc
/** Compare interrupt flag bit mask */
#  define RTC_TEST_COMPIF_bm RTC32_COMPIF_bm
/** Compare interrupt vector */
#  define RTC_TEST_COMP_vect RTC32_COMP_vect
/** Overflow Interrupt Flag bit mask */
#  define RTC_TEST_OVFIF_bm RTC32_OVFIF_bm
#else
#  define RTC_TEST RTC
#  define RTC_TEST_START_bm RTC_PRESCALER_DIV1_gc
#  define RTC_TEST_COMPINTLVL_LO_gc RTC_COMPINTLVL_LO_gc
#  define RTC_TEST_COMPINTLVL_OFF_gc RTC_COMPINTLVL_OFF_gc
#  define RTC_TEST_COMPIF_bm RTC_COMPIF_bm
#  define RTC_TEST_COMP_vect RTC_COMP_vect
#  define RTC_TEST_OVFIF_bm RTC_OVFIF_bm
#endif

/** @} */

void classb_rtc_callback(uint32_t time);

/** @} */

#endif /* CLASSB_RTC_COMMON_H */
