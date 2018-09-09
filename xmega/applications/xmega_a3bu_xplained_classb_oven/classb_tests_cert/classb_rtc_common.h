/**
 * \file
 *
 * \brief Settings for the RTC driver.
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
