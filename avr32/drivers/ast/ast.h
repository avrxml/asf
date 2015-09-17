/**
 * \file
 *
 * \brief AST driver for AVR32 UC3.
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

#ifndef _AST_H_
#define _AST_H_

/**
 * \defgroup group_avr32_drivers_ast AST - Asynchronous Timer
 *
 * Driver for the AST (Asynchronous Timer).
 * Provides functions for configuring and operating the AST in the calendar or
 * timer/counter modes.
 *
 * @{
 */

#include "compiler.h"
#include <avr32/io.h>

/** Timeout to prevent code hang in clock initialization */
#define AST_POLL_TIMEOUT 1000

/**
 * \name Oscillator Types
 *
 * @{
 */

/** 1KHz clock from 32KHz oscillator (CLK_1K) */
#define AST_OSC_1KHZ     4
/** Generic clock (GCLK) */
#define AST_OSC_GCLK     3
/** Peripheral Bus Clock */
#define AST_OSC_PB       2
/** 32KHz oscillator (OSC32K) */
#define AST_OSC_32KHZ    1
/** System RC oscillator (RCSYS) */
#define AST_OSC_RC       0

/** @} */

/** \name Predefined PSEL Values
 *
 * @{
 */

/**
 * The PSEL value to set the AST source clock (after the prescaler) to 1 Hz,
 * when using an external 32-kHz crystal.
 */
#define AST_PSEL_32KHZ_1HZ    14

/**
 * The PSEL value to set the AST source clock (after the prescaler)
 * to 1.76 Hz when using the internal RC oscillator (~ 115 kHz)
 */
#define AST_PSEL_RC_1_76HZ    15

/** @} */

/**
 * \name AST Mode
 *
 * @{ */

/** AST in Counter Mode */
#define AST_MODE_COUNTER  0
/** AST in Calendar Mode */
#define AST_MODE_CALENDAR 1

/** @} */

/** Roundup operation for Digital Tuner in AST */
#define ROUNDUP_DIV(x, y) ((x % y) ? ((x / y) + 1) : (x / y))

/** Input when initializing AST in calendar mode. */
typedef struct ast_calendar_t {
	union {
		uint32_t field;
		avr32_ast_calv_t FIELD;
	};
} ast_calendar_t;

/* Function Declarations */

void ast_enable(volatile avr32_ast_t *ast);

void ast_disable(volatile avr32_ast_t *ast);

bool ast_init_calendar(volatile avr32_ast_t *ast, uint8_t osc_type,
		uint8_t psel, ast_calendar_t ast_calendar);

bool ast_init_counter(volatile avr32_ast_t *ast, uint8_t osc_type,
		uint8_t psel, uint32_t ast_counter);

bool ast_configure_digital_tuner(volatile avr32_ast_t *ast,
		uint32_t input_freq, uint32_t tuned_freq);

void ast_init_digital_tuner(volatile avr32_ast_t *ast, bool add,
		uint8_t value, uint8_t exp);

void ast_disable_digital_tuner(volatile avr32_ast_t *ast);

bool ast_change_clk_source(volatile avr32_ast_t *ast, uint8_t osc_type,
		uint8_t psel);

void ast_set_calendar_value(volatile avr32_ast_t *ast,
		ast_calendar_t ast_calendar);

void ast_set_counter_value(volatile avr32_ast_t *ast,
		uint32_t ast_counter);

ast_calendar_t ast_get_calendar_value(volatile avr32_ast_t *ast);

uint32_t ast_get_counter_value(volatile avr32_ast_t *ast);

void ast_set_alarm0_value(volatile avr32_ast_t *ast,
		uint32_t alarm_value);

void ast_set_alarm1_value(volatile avr32_ast_t *ast,
		uint32_t alarm_value);

void ast_enable_alarm0(volatile avr32_ast_t *ast);

void ast_disable_alarm0(volatile avr32_ast_t *ast);

void ast_enable_alarm1(volatile avr32_ast_t *ast);

void ast_disable_alarm1(volatile avr32_ast_t *ast);

void ast_set_periodic0_value(volatile avr32_ast_t *ast,
		avr32_ast_pir0_t pir);

void ast_set_periodic1_value(volatile avr32_ast_t *ast,
		avr32_ast_pir1_t pir);

void ast_enable_periodic0(volatile avr32_ast_t *ast);

void ast_disable_periodic0(volatile avr32_ast_t *ast);

void ast_enable_periodic1(volatile avr32_ast_t *ast);

void ast_disable_periodic1(volatile avr32_ast_t *ast);

void ast_clear_status_flag(volatile avr32_ast_t *ast,
		uint32_t status_mask);

void ast_clear_alarm_status_flag(volatile avr32_ast_t *ast,
		uint32_t alarm_channel);

void ast_clear_periodic_status_flag(volatile avr32_ast_t *ast,
		uint32_t periodic_channel);

void ast_clear_ovf_status_flag(volatile avr32_ast_t *ast);

void ast_clear_ready_status_flag(volatile avr32_ast_t *ast);

void ast_clear_clkrdy_status_flag(volatile avr32_ast_t *ast);

void ast_clear_all_status_flags(volatile avr32_ast_t *ast);

void ast_enable_interrupt(volatile avr32_ast_t *ast,
		uint32_t interrupt_mask);

void ast_enable_alarm_interrupt(volatile avr32_ast_t *ast,
		uint8_t alarm_channel);

void ast_enable_periodic_interrupt(volatile avr32_ast_t *ast,
		uint8_t periodic_channel);

void ast_enable_ovf_interrupt(volatile avr32_ast_t *ast);

void ast_enable_ready_interrupt(volatile avr32_ast_t *ast);

void ast_enable_clkrdy_interrupt(volatile avr32_ast_t *ast);

void ast_disable_interrupt(volatile avr32_ast_t *ast,
		uint32_t interrupt_mask);

void ast_disable_alarm_interrupt(volatile avr32_ast_t *ast,
		uint8_t alarm_channel);

void ast_disable_periodic_interrupt(volatile avr32_ast_t *ast,
		uint8_t periodic_channel);

void ast_disable_ovf_interrupt(volatile avr32_ast_t *ast);

void ast_disable_ready_interrupt(volatile avr32_ast_t *ast);

void ast_disable_clkrdy_interrupt(volatile avr32_ast_t *ast);

void ast_disable_all_interrupts(volatile avr32_ast_t *ast);

void ast_enable_async_wakeup(volatile avr32_ast_t *ast,
		uint32_t wakeup_mask);

void ast_enable_alarm_async_wakeup(volatile avr32_ast_t *ast,
		uint8_t alarm_channel);

void ast_enable_periodic_async_wakeup(volatile avr32_ast_t *ast,
		uint8_t periodic_channel);

void ast_enable_ovf_async_wakeup(volatile avr32_ast_t *ast);

void ast_disable_async_wakeup(volatile avr32_ast_t *ast,
		uint32_t wakeup_mask);

void ast_disable_alarm_async_wakeup(volatile avr32_ast_t *ast,
		uint8_t alarm_channel);

void ast_disable_periodic_async_wakeup(volatile avr32_ast_t *ast,
		uint8_t periodic_channel);

void ast_disable_ovf_async_wakeup(volatile avr32_ast_t *ast);

void ast_enable_counter_clear_on_alarm(volatile avr32_ast_t *ast,
		uint8_t alarm_channel);

void ast_clear_prescalar(volatile avr32_ast_t *ast);

/** @} */

#endif  /* _AST_H_ */
