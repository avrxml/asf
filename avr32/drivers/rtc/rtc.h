/*****************************************************************************
 *
 * \file
 *
 * \brief RTC driver for AVR32 UC3.
 *
 * AVR32 Real Time Counter driver module.
 *
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


#ifndef _RTC_H_
#define _RTC_H_

/**
 * \defgroup group_avr32_drivers_rtc RTC - Real Time Counter
 *
 * Driver for the RTC (32-bit Real Time Counter).
 * The RTC enables periodic interrupts at long intervals,
 * or accurate measurement of real-time sequences. Can be clocked from
 * internal RC oscillator or external 32 kHz crystal.
 *
 * \{
 */

#include "compiler.h"
#include <avr32/io.h>


/*! \name Oscillator Types
 */
//! @{
#define RTC_OSC_32KHZ    1
#define RTC_OSC_RC       0
//! @}

/*! \name Predefined PSEL Values
 */
//! @{

//! The PSEL value to set the RTC source clock (after the prescaler) to 1 Hz,
//! when using an external 32-kHz crystal.
#define RTC_PSEL_32KHZ_1HZ    14

//! The PSEL value to set the RTC source clock (after the prescaler) to 1.76 Hz,
//! when using the internal RC oscillator (~ 115 kHz).
#define RTC_PSEL_RC_1_76HZ    15

//! @}


/*!
 * \brief This function will initialize the RTC module.
 *        If you use the 32 KHz oscillator, it will enable this module.
 *        This function also set the top value of the RTC to 0xFFFFFFFF
 *        and the value to 0.
 * \param rtc Base address of the RTC (i.e. &AVR32_RTC).
 * \param osc_type The oscillator you want to use. If you need a better
 *        accuracy, use the 32 KHz oscillator (i.e. RTC_OSC_32KHZ).
 * \param psel The preselector value for the corresponding oscillator (4-bits).
 *        To obtain this value, you can use this formula:
 *        psel = log(Fosc/Frtc)/log(2)-1, where Fosc is the frequency of the
 *        oscillator you are using (32 KHz or 115 KHz) and Frtc the frequency
 *        desired.
 * \return 1 if the initialization succeeds otherwise it will return 0.
 */
extern int rtc_init(volatile avr32_rtc_t *rtc, unsigned char osc_type, unsigned char psel);

/*!
 * \brief Enable the RTC.
 * \param rtc Base address of the RTC (i.e. &AVR32_RTC).
 */
extern void rtc_enable(volatile avr32_rtc_t *rtc);

/*!
 * \brief Disable the RTC.
 * \param rtc Base address of the RTC (i.e. &AVR32_RTC).
 */
extern void rtc_disable(volatile avr32_rtc_t *rtc);

/*!
 * \brief Enable the wake up feature of the RTC.
 * \param rtc Base address of the RTC (i.e. &AVR32_RTC).
 */
extern void rtc_enable_wake_up(volatile avr32_rtc_t *rtc);

/*!
 * \brief Disable the wake up feature of the RTC.
 * \param rtc Base address of the RTC (i.e. &AVR32_RTC).
 */
extern void rtc_disable_wake_up(volatile avr32_rtc_t *rtc);

/*!
 * \brief Enable the interrupt feature of the RTC.
 *        An interrupt is raised when the value of the RTC
 *        is equal to its top value.
 * \param rtc Base address of the RTC (i.e. &AVR32_RTC).
 */
extern void rtc_enable_interrupt(volatile avr32_rtc_t *rtc);

/*!
 * \brief Disable the interrupt feature of the RTC.
 * \param rtc Base address of the RTC (i.e. &AVR32_RTC).
 */
extern void rtc_disable_interrupt(volatile avr32_rtc_t *rtc);

/*!
 * \brief Clear the interrupt flag.
 *        Call this function once you handled the interrupt.
 * \param rtc Base address of the RTC (i.e. &AVR32_RTC).
 */
extern void rtc_clear_interrupt(volatile avr32_rtc_t *rtc);

/*!
 * \brief Get the status of interrupts.
 * \param rtc Base address of the RTC (i.e. &AVR32_RTC).
 * \return 1 if the interrupts are enabled otherwise it returns 0.
 */
extern int rtc_interrupt_enabled(volatile avr32_rtc_t *rtc);

/*!
 * \brief Check if an interrupt is raised.
 * \param rtc Base address of the RTC (i.e. &AVR32_RTC).
 * \return 1 if an interrupt is currently raised otherwise it returns 0.
 */
extern int rtc_is_interrupt(volatile avr32_rtc_t *rtc);

/*!
 * \brief This function sets the RTC current top value.
 * \param rtc Base address of the RTC (i.e. &AVR32_RTC).
 * \param top The top value you want to store.
 */
extern void rtc_set_top_value(volatile avr32_rtc_t *rtc, unsigned long top);

/*!
 * \brief This function returns the RTC current top value.
 * \param rtc Base address of the RTC (i.e. &AVR32_RTC).
 * \return The RTC current top value.
 */
extern unsigned long rtc_get_top_value(volatile avr32_rtc_t *rtc);

/*!
 * \brief This function checks if the RTC is busy or not.
 * \param rtc Base address of the RTC (i.e. &AVR32_RTC).
 * \return 1 if the RTC is busy otherwise it will return 0.
 */
extern int rtc_is_busy(volatile avr32_rtc_t *rtc);

/*!
 * \brief This function sets the RTC current value.
 * \param rtc Base address of the RTC (i.e. &AVR32_RTC).
 * \param val The value you want to store.
 */
extern void rtc_set_value(volatile avr32_rtc_t *rtc, unsigned long val);

/*!
 * \brief This function returns the RTC current value.
 * \param rtc Base address of the RTC (i.e. &AVR32_RTC).
 * \return The RTC current value.
 */
extern unsigned long rtc_get_value(volatile avr32_rtc_t *rtc);

/**
 * \}
 */

#endif  // _RTC_H_
