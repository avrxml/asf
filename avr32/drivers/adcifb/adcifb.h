/**
 * \file
 *
 * \brief ADCIFB Software driver interface for AVR32 UC3.
 *
 * Copyright (C) 2009-2015 Atmel Corporation. All rights reserved.
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

#ifndef _ADCIFB_H_
#define _ADCIFB_H_

/**
 * \defgroup group_avr32_drivers_adcifb ADCIFB - ADC Interface B
 *
 * Driver for the ADCIFB (Analog-to-Digital Converter Interface B).
 * Provides functions for configuration of conversion parameters
 * (up to 12-bit signed with 16x interpolation at 6 MHz), enabling channels
 * (max. 32, see device datasheet for details) and conversion triggering.
 * Resistive touch screen mode is not supported by this driver.
 *
 * @{
 */

#include <avr32/io.h>
#include "compiler.h"

/*! The timeguard used for polling. */
#define ADCIFB_POLL_TIMEOUT         10000

/*! Parameters for initialization of the ADCIFB module */
typedef struct {
	/*! Resolution mode */
	uint16_t resolution;

	/*! Channels Sample & Hold Time in [0,15], where
	 *  Ts&h = shtim * Tclk_adc. */
	uint16_t shtim;

	/*! Ratio Fclk_adcifb/Fclk_adc = 2*i with i an integer in [1,64] */
	uint16_t ratio_clkadcifb_clkadc;

	/**
	 * ADC Startup time in [0,127],
	 * where Tstartup = startup * 8 * Tclk_adc
	 */
	uint16_t startup;

	/*! ADCIFB Sleep Mode enable/disable */
	bool sleep_mode_enable;
} adcifb_opt_t;

int32_t adcifb_configure(volatile avr32_adcifb_t *adcifb,
		const adcifb_opt_t *p_adcifb_opt);

void adcifb_enable_pen_detect_mode(volatile avr32_adcifb_t *adcifb,
		uint8_t pen_dbc_period);

void adcifb_enable_touch_screen_mode(volatile avr32_adcifb_t *adcifb,
		uint8_t touch_screen_pin_offset, bool analog_pin_output_enable);

int32_t adcifb_disable(volatile avr32_adcifb_t *adcifb);

int32_t adcifb_configure_trigger(volatile avr32_adcifb_t *adcifb,
		uint8_t trgmod, uint32_t trgper);

uint32_t adcifb_get_last_data(volatile avr32_adcifb_t *adcifb);

int32_t adcifb_sr_statushigh_wait(volatile avr32_adcifb_t *adcifb,
		uint32_t statusMask);

bool adcifb_is_ready(volatile avr32_adcifb_t *adcifb);

bool adcifb_is_busy(volatile avr32_adcifb_t *adcifb);

bool adcifb_is_drdy(volatile avr32_adcifb_t *adcifb);

bool adcifb_is_ovre(volatile avr32_adcifb_t *adcifb);

/**
 * \brief Enable one or several channels.
 *
 * \param *adcifb         Base address of the ADCIFB module
 * \param  channels_mask  Bitmask of channels to enable
 */
static inline void adcifb_channels_enable(volatile avr32_adcifb_t *adcifb,
		uint32_t channels_mask)
{
	adcifb->cher |= channels_mask;
}

/**
 * \brief Disable one or several channels.
 *
 * \param *adcifb         Base address of the ADCIFB module
 * \param  channels_mask  Bitmask of channels to disable
 */
static inline void adcifb_channels_disable(volatile avr32_adcifb_t *adcifb,
		uint32_t channels_mask)
{
	adcifb->chdr |= channels_mask;
}

/**
 * \brief Start an ADC conversion sequence (Software trigger)
 *
 * \param *adcifb Base address of the ADCIFB module
 */
static inline void adcifb_start_conversion_sequence(
		volatile avr32_adcifb_t *adcifb)
{
	adcifb->cr = AVR32_ADCIFB_CR_START_MASK;
}

/**
 * \brief Resets the ADCIFB module(software reset)
 *
 * \param *adcifb Base address of the ADCIFB module
 */
static inline void adcifb_reset(volatile avr32_adcifb_t *adcifb)
{
	adcifb->cr = AVR32_ADCIFB_CR_SWRST_MASK;
}

/**
 * \brief Enable Analog Compare mode in ADCIFB
 *
 * \param *adcifb       Base address of the ADCIFB module
 */
static inline void adcifb_enable_analog_compare_mode(
		volatile avr32_adcifb_t *adcifb)
{
	/* Enable Analog Compare Mode */
	adcifb->mr  |= AVR32_ADCIFB_MR_ACE_MASK;
}

/**
 * \brief Set the ADCIFB Compare Low Value Register.
 *
 * \param *adcifb         Base address of the ADCIFB module
 * \param  compare_value  Value to be compared
 */
static inline void adcifb_set_low_compare_value(
		volatile avr32_adcifb_t *adcifb, uint32_t compare_value)
{
	adcifb->cvr |= (compare_value << AVR32_ADCIFB_CVR_LV_OFFSET)
			& AVR32_ADCIFB_CVR_LV_MASK;
}

/**
 * \brief Set the ADCIFB Compare High Value Register.
 *
 * \param *adcifb         Base address of the ADCIFB module
 * \param  compare_value  Value to be compared
 */
static inline void adcifb_set_high_compare_value(
		volatile avr32_adcifb_t *adcifb,
		uint32_t compare_value)
{
	adcifb->cvr |= (compare_value << AVR32_ADCIFB_CVR_HV_OFFSET)
			& AVR32_ADCIFB_CVR_HV_MASK;
}

/**
 * \brief Set the ADCIFB Pen Detect Debouncing Period.
 *
 * \param *adcifb          Base address of the ADCIFB module
 * \param  pen_dbc_period  Debouncing period
 */
static inline void adcifb_set_pen_debouncing_period(
		volatile avr32_adcifb_t *adcifb,
		uint8_t pen_dbc_period)
{
	adcifb->cvr |= ((pen_dbc_period << AVR32_ADCIFB_MR_PENDBC_OFFSET)
			& AVR32_ADCIFB_MR_PENDBC_MASK);
}

/**
 * \brief Set the ADCIFB Touch Screen Pin Offset value.
 *
 * \param *adcifb                   Base address of the ADCIFB module
 * \param  touch_screen_pin_offset  Offset value
 */
static inline void adcifb_set_touch_screen_pin_offset(
		volatile avr32_adcifb_t *adcifb,
		uint8_t touch_screen_pin_offset)
{
	adcifb->cvr |= ((touch_screen_pin_offset
			<< AVR32_ADCIFB_MR_TSPO_OFFSET) &
			AVR32_ADCIFB_MR_TSPO_MASK);
}

/**
 * \brief Enable Analog Pin output when using touch screen mode in ADCIFB
 *
 * \param *adcifb       Base address of the ADCIFB module
 */
static inline void adcifb_enable_analog_pin_output(
		volatile avr32_adcifb_t *adcifb)
{
	/* Enable Analog Pin Output in Touch Screen Mode */
	adcifb->mr  |= AVR32_ADCIFB_MR_APOE_MASK;
}

/**
 * \brief Enable ADCIFB interrupts.
 *
 * \param *adcifb         Base address of the ADCIFB module
 * \param  interrupt_mask Bitmask of interrupts to enable
 */
static inline void adcifb_enable_interrupt(volatile avr32_adcifb_t *adcifb,
		uint32_t interrupt_mask)
{
	adcifb->ier = interrupt_mask;
}

/**
 * \brief Enable ADCIFB Compare Else interrupt.
 *
 * \param *adcifb Base address of the ADCIFB module
 */
static inline void adcifb_enable_compare_else_interrupt(
		volatile avr32_adcifb_t *adcifb)
{
	adcifb->ier = AVR32_ADCIFB_IER_CELSE_MASK;
}

/**
 * \brief Enable ADCIFB Compare Less than interrupt.
 *
 * \param *adcifb Base address of the ADCIFB module
 */
static inline void adcifb_enable_compare_lt_interrupt(
		volatile avr32_adcifb_t *adcifb)
{
	adcifb->ier = AVR32_ADCIFB_IER_CLT_MASK;
}

/**
 * \brief Enable ADCIFB Compare Greater than interrupt.
 *
 * \param *adcifb Base address of the ADCIFB module
 */
static inline void adcifb_enable_compare_gt_interrupt(
		volatile avr32_adcifb_t *adcifb)
{
	adcifb->ier = AVR32_ADCIFB_IER_CGT_MASK;
}

/**
 * \brief Enable ADCIFB Busy Interrupt.
 *
 * \param *adcifb Base address of the ADCIFB module
 */
static inline void adcifb_enable_busy_interrupt(
		volatile avr32_adcifb_t *adcifb)
{
	adcifb->ier = AVR32_ADCIFB_IER_BUSY_MASK;
}

/**
 * \brief Enable ADCIFB Ready Interrupt.
 *
 * \param *adcifb Base address of the ADCIFB module
 */
static inline void adcifb_enable_ready_interrupt(
		volatile avr32_adcifb_t *adcifb)
{
	adcifb->ier = AVR32_ADCIFB_IER_READY_MASK;
}

/**
 * \brief Enable ADCIFB No Contact Interrupt.
 *
 * \param *adcifb Base address of the ADCIFB module
 */
static inline void adcifb_enable_no_contact_interrupt(
		volatile avr32_adcifb_t *adcifb)
{
	adcifb->ier = AVR32_ADCIFB_IER_NOCNT_MASK;
}

/**
 * \brief Enable ADCIFB Pen Contact Interrupt.
 *
 * \param *adcifb Base address of the ADCIFB module
 */
static inline void adcifb_enable_pen_contact_interrupt(
		volatile avr32_adcifb_t *adcifb)
{
	adcifb->ier = AVR32_ADCIFB_IER_PENCNT_MASK;
}

/**
 * \brief Enable ADCIFB Overrun Error Interrupt.
 *
 * \param *adcifb Base address of the ADCIFB module
 */
static inline void adcifb_enable_overrun_error_interrupt(
		volatile avr32_adcifb_t *adcifb)
{
	adcifb->ier = AVR32_ADCIFB_IER_OVRE_MASK;
}

/**
 * \brief Enable ADCIFB Data Ready Interrupt.
 *
 * \param *adcifb Base address of the ADCIFB module
 */
static inline void adcifb_enable_data_ready_interrupt(
		volatile avr32_adcifb_t *adcifb)
{
	adcifb->ier = AVR32_ADCIFB_IER_DRDY_MASK;
}

/**
 * \brief Disable all ADCIFB interrupts.
 *
 * \param *adcifb         Base address of the ADCIFB module
 */
static inline void adcifb_disable_all_interrupts(
		volatile avr32_adcifb_t *adcifb)
{
	adcifb->idr = 0xFFFFFFFF;
}

/**
 * \brief Disable ADCIFB interrupts.
 *
 * \param *adcifb         Base address of the ADCIFB module
 * \param  interrupt_mask Bitmask of interrupts to disable
 */
static inline void adcifb_disable_interrupt(volatile avr32_adcifb_t *adcifb,
		uint32_t interrupt_mask)
{
	adcifb->idr = interrupt_mask;
}

/**
 * \brief Disable ADCIFB Compare Else interrupt.
 *
 * \param *adcifb Base address of the ADCIFB module
 */
static inline void adcifb_disable_compare_else_interrupt(
		volatile avr32_adcifb_t *adcifb)
{
	adcifb->idr = AVR32_ADCIFB_IDR_CELSE_MASK;
}

/**
 * \brief Disable ADCIFB Compare Less than interrupt.
 *
 * \param *adcifb Base address of the ADCIFB module
 */
static inline void adcifb_disable_compare_lt_interrupt(
		volatile avr32_adcifb_t *adcifb)
{
	adcifb->idr = AVR32_ADCIFB_IDR_CLT_MASK;
}

/**
 * \brief Disable ADCIFB Compare Greater than interrupt.
 *
 * \param *adcifb Base address of the ADCIFB module
 */
static inline void adcifb_disable_compare_gt_interrupt(
		volatile avr32_adcifb_t *adcifb)
{
	adcifb->idr = AVR32_ADCIFB_IDR_CGT_MASK;
}

/**
 * \brief Disable ADCIFB Busy Interrupt.
 *
 * \param *adcifb Base address of the ADCIFB module
 */
static inline void adcifb_disable_busy_interrupt(
		volatile avr32_adcifb_t *adcifb)
{
	adcifb->idr = AVR32_ADCIFB_IDR_BUSY_MASK;
}

/**
 * \brief Disable ADCIFB Ready Interrupt.
 *
 * \param *adcifb Base address of the ADCIFB module
 */
static inline void adcifb_disable_ready_interrupt(
		volatile avr32_adcifb_t *adcifb)
{
	adcifb->idr = AVR32_ADCIFB_IDR_READY_MASK;
}

/**
 * \brief Disable ADCIFB No Contact Interrupt.
 *
 * \param *adcifb Base address of the ADCIFB module
 */
static inline void adcifb_disable_no_contact_interrupt(
		volatile avr32_adcifb_t *adcifb)
{
	adcifb->idr = AVR32_ADCIFB_IDR_NOCNT_MASK;
}

/**
 * \brief Disable ADCIFB Pen Contact Interrupt.
 *
 * \param *adcifb Base address of the ADCIFB module
 */
static inline void adcifb_disable_pen_contact_interrupt(
		volatile avr32_adcifb_t *adcifb)
{
	adcifb->idr = AVR32_ADCIFB_IDR_PENCNT_MASK;
}

/**
 * \brief Disable ADCIFB Overrun Error Interrupt.
 *
 * \param *adcifb Base address of the ADCIFB module
 */
static inline void adcifb_disable_overrun_error_interrupt(
		volatile avr32_adcifb_t *adcifb)
{
	adcifb->idr = AVR32_ADCIFB_IDR_OVRE_MASK;
}

/**
 * \brief Disable ADCIFB Data Ready Interrupt.
 *
 * \param *adcifb Base address of the ADCIFB module
 */
static inline void adcifb_disable_data_ready_interrupt(
		volatile avr32_adcifb_t *adcifb)
{
	adcifb->idr = AVR32_ADCIFB_IDR_DRDY_MASK;
}

/**
 * \brief Clear all ADCIFB interrupt flags.
 *
 * \param *adcifb         Base address of the ADCIFB module
 */
static inline void adcifb_clear_all_interrupt_flags(
		volatile avr32_adcifb_t *adcifb)
{
	adcifb->icr = 0xFFFFFFFF;
}

/**
 * \brief Clear ADCIFB interrupt flags.
 *
 * \param *adcifb         Base address of the ADCIFB module
 * \param  interrupt_mask Bitmask of interrupts to clear
 */
static inline void adcifb_clear_interrupt_flag(volatile avr32_adcifb_t *adcifb,
		uint32_t interrupt_mask)
{
	adcifb->icr = interrupt_mask;
}

/**
 * \brief Clear ADCIFB Compare Else interrupt flag.
 *
 * \param *adcifb Base address of the ADCIFB module
 */
static inline void adcifb_clear_compare_else_interrupt_flag(
		volatile avr32_adcifb_t *adcifb)
{
	adcifb->icr = AVR32_ADCIFB_ICR_CELSE_MASK;
}

/**
 * \brief Clear ADCIFB Compare Less than interrupt flag.
 *
 * \param *adcifb Base address of the ADCIFB module
 */
static inline void adcifb_clear_compare_lt_interrupt_flag(
		volatile avr32_adcifb_t *adcifb)
{
	adcifb->icr = AVR32_ADCIFB_ICR_CLT_MASK;
}

/**
 * \brief Clear ADCIFB Compare Greater than interrupt flag
 *
 * \param *adcifb Base address of the ADCIFB module
 */
static inline void adcifb_clear_compare_gt_interrupt_flag(
		volatile avr32_adcifb_t *adcifb)
{
	adcifb->icr = AVR32_ADCIFB_ICR_CGT_MASK;
}

/**
 * \brief Clear ADCIFB Busy Interrupt flag.
 *
 * \param *adcifb Base address of the ADCIFB module
 */
static inline void adcifb_clear_busy_interrupt_flag(
		volatile avr32_adcifb_t *adcifb)
{
	adcifb->icr = AVR32_ADCIFB_ICR_BUSY_MASK;
}

/**
 * \brief Clear ADCIFB Ready Interrupt flag.
 *
 * \param *adcifb Base address of the ADCIFB module
 */
static inline void adcifb_clear_ready_interrupt_flag(
		volatile avr32_adcifb_t *adcifb)
{
	adcifb->icr = AVR32_ADCIFB_ICR_READY_MASK;
}

/**
 * \brief Clear ADCIFB No Contact Interrupt flag.
 *
 * \param *adcifb Base address of the ADCIFB module
 */
static inline void adcifb_clear_no_contact_interrupt_flag(
		volatile avr32_adcifb_t *adcifb)
{
	adcifb->icr = AVR32_ADCIFB_ICR_NOCNT_MASK;
}

/**
 * \brief Clear ADCIFB Pen Contact Interrupt flag.
 *
 * \param *adcifb Base address of the ADCIFB module
 */
static inline void adcifb_clear_pen_contact_interrupt_flag(
		volatile avr32_adcifb_t *adcifb)
{
	adcifb->icr = AVR32_ADCIFB_ICR_PENCNT_MASK;
}

/**
 * \brief Clear ADCIFB Overrun Error Interrupt flag.
 *
 * \param *adcifb Base address of the ADCIFB module
 */
static inline void adcifb_clear_overrun_error_interrupt_flag(
		volatile avr32_adcifb_t *adcifb)
{
	adcifb->icr = AVR32_ADCIFB_ICR_OVRE_MASK;
}

/**
 * \brief Clear ADCIFB Data Ready Interrupt flag.
 *
 * \param *adcifb Base address of the ADCIFB module
 */
static inline void adcifb_clear_data_ready_interrupt_flag(
		volatile avr32_adcifb_t *adcifb)
{
	adcifb->icr = AVR32_ADCIFB_ICR_DRDY_MASK;
}

/**
 * @}
 */

#endif  /* _ADCIFB_H_ */
