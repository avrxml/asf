/**
 * \file
 *
 * \brief AST driver for AVR32 UC3.
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
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
