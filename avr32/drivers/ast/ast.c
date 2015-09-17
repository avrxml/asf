/**
 * \file
 *
 * \brief AST driver for AVR32 UC3.
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
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "compiler.h"
#include "ast.h"

/**
 * \brief Check the busy status of AST.
 *
 * \param ast  Base address of the AST (i.e. &AVR32_AST).
 *
 * \return Boolean true If AST is busy, false otherwise.
 */
static bool ast_is_busy(volatile avr32_ast_t *ast)
{
	return (ast->sr & AVR32_AST_SR_BUSY_MASK) != 0;
}

/**
 * \brief Check the busy status of AST clock
 *
 * \param ast  Base address of the AST (i.e. &AVR32_AST).
 *
 * \return Boolean true If AST clock is busy, false otherwise.
 */
static bool ast_is_clkbusy(volatile avr32_ast_t *ast)
{
	return (ast->sr & AVR32_AST_SR_CLKBUSY_MASK) != 0;
}

/**
 * \brief Enable the AST.
 *
 * \param ast  Base address of the AST (i.e. &AVR32_AST).
 */
void ast_enable(volatile avr32_ast_t *ast)
{
	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}

	/* Enable the AST */
	ast->cr |= AVR32_AST_CR_EN_MASK;

	/* Wait until write is done */
	while (ast_is_busy(ast)) {
	}
}

/**
 * \brief Disable the AST.
 *
 * \param ast Base address of the AST (i.e. &AVR32_AST).
 */
void ast_disable(volatile avr32_ast_t *ast)
{
	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}

	/* Disable the AST */
	ast->cr &= ~(AVR32_AST_CR_EN_MASK);

	/* Wait until write is done */
	while (ast_is_busy(ast)) {
	}
}

/**
 * \brief This function will initialize the AST module in calendar Mode.
 *
 * \note  If you use the 32 KHz oscillator, it must be enabled before calling
 *        this function.
 *
 * \param ast Base address of the AST (i.e. &AVR32_AST).
 * \param osc_type The oscillator you want to use. If you need a better
 *        accuracy, use the 32 KHz oscillator (i.e. AST_OSC_32KHZ).
 * \param psel The preselector value for the corresponding oscillator (4-bits).
 *        To obtain this value, you can use this formula:
 *        psel = log(Fosc/Fast)/log(2)-1, where Fosc is the frequency of the
 *        oscillator you are using (32 KHz or 115 KHz) and Fast the frequency
 *        desired.
 * \param ast_calendar Startup date
 *
 * \return Boolean true if the initialization succeeds, false otherwise.
 */
bool ast_init_calendar(volatile avr32_ast_t *ast, uint8_t osc_type,
		uint8_t psel, ast_calendar_t ast_calendar)
{
	uint32_t time_out = AST_POLL_TIMEOUT;
	while (ast_is_clkbusy(ast)) {
		if (--time_out == 0) {
			return false;
		}
	}

	ast->clock = osc_type << AVR32_AST_CLOCK_CSSEL_OFFSET;
	time_out = AST_POLL_TIMEOUT;
	while (ast_is_clkbusy(ast)) {
		if (--time_out == 0) {
			return false;
		}
	}

	ast->clock |= AVR32_AST_CLOCK_CEN_MASK;
	time_out = AST_POLL_TIMEOUT;
	while (ast_is_clkbusy(ast)) {
		if (--time_out == 0) {
			return false;
		}
	}
	/* Set the new AST configuration */
	ast->cr = AST_MODE_CALENDAR << AVR32_AST_CR_CAL_OFFSET |
			psel << AVR32_AST_CR_PSEL_OFFSET;

	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}

	/* Set the calendar */
	ast_set_calendar_value(ast, ast_calendar);

	return true;
}

/**
 * \brief This function will initialize the AST module in counter Mode.
 *
 * \note  If you use the 32 KHz oscillator, it must be enabled before calling
 *        this function.
 *
 * \param ast Base address of the AST (i.e. &AVR32_AST).
 * \param osc_type The oscillator you want to use. If you need a better
 *        accuracy, use the 32 KHz oscillator (i.e. AST_OSC_32KHZ).
 * \param psel The preselector value for the corresponding oscillator (4-bits).
 *        To obtain this value, you can use this formula:
 *        psel = log(Fosc/Fast)/log(2)-1, where Fosc is the frequency of the
 *        oscillator you are using (32 KHz or 115 KHz) and Fast the frequency
 *        desired.
 * \param ast_counter Startup counter value
 *
 * \return Boolean true if the initialization succeeds, false otherwise.
 */
bool ast_init_counter(volatile avr32_ast_t *ast, uint8_t osc_type,
		uint8_t psel, uint32_t ast_counter)
{
	uint32_t time_out = AST_POLL_TIMEOUT;
	while (ast_is_clkbusy(ast)) {
		if (--time_out == 0) {
			return false;
		}
	}
	
	ast->clock = osc_type << AVR32_AST_CLOCK_CSSEL_OFFSET;
	time_out = AST_POLL_TIMEOUT;
	while (ast_is_clkbusy(ast)) {
		if (--time_out == 0) {
			return false;
		}
	}

	ast->clock |= AVR32_AST_CLOCK_CEN_MASK;
	time_out = AST_POLL_TIMEOUT;
	while (ast_is_clkbusy(ast)) {
		if (--time_out == 0) {
			return false;
		}
	}

	/* Set the new AST configuration */
	ast->cr = (AST_MODE_COUNTER << AVR32_AST_CR_CAL_OFFSET) |
			(psel << AVR32_AST_CR_PSEL_OFFSET);

	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}

	/* Set the calendar */
	ast_set_counter_value(ast, ast_counter);

	return true;
}

/**
 * \brief Function to tune the AST prescalar frequency to the desired frequency
 *
 * \param ast         Base address of the AST (i.e. &AVR32_AST).
 * \param input_freq  Prescaled AST Clock Frequency
 * \param tuned_freq  Desired AST frequency
 *
 * \retval false If invalid frequency is passed
 * \retval true If configuration succeeds
 *
 * \note Parameter Calculation:
 * Formula: \n
 * ADD=0 -> ft= fi(1 - (1/((roundup(256/value) * (2^exp)) + 1))) \n
 * ADD=1 -> ft= fi(1 + (1/((roundup(256/value) * (2^exp)) - 1))) \n
 * Specifications: \n
 * exp > 0, value > 1 \n
 * Let X = (2 ^ exp), Y = roundup(256 / value) \n
 * Tuned Frequency -> ft \n
 * Input Frequency -> fi \n
 *
 * IF ft < fi \n
 * ADD = 0 \n
 * Z = (ft / (fi - ft)) \n
 * ELSE IF ft > fi \n
 * ADD = 1 \n
 * Z = (ft / (ft - fi)) \n
 * ELSE \n
 * exit function -> Tuned Frequency = Input Frequency \n
 *
 * The equation can be reduced to (X * Y) = Z
 *
 * Frequency Limits \n
 * (1/((roundup(256/value) * (2^exp)) + 1)) should be min to get the lowest
 * possible output from Digital Tuner.\n
 * (1/((roundup(256/value) * (2^exp)) - 1)) should be min to get the highest
 * possible output from Digital Tuner.\n
 * For that, roundup(256/value) & (2^exp) should be minimum \n
 * min (EXP) = 1 (Note: EXP > 0) \n
 * min (roundup(256/value)) = 2 \n
 * max (Ft) = (4*fi)/3 \n
 * min (Ft) = (4*fi)/5 \n
 *
 * Using the above details, X & Y that will closely satisfy the equation is
 * found in this function.
 */
bool ast_configure_digital_tuner(volatile avr32_ast_t *ast,
		uint32_t input_freq, uint32_t tuned_freq)
{
	bool add;
	uint8_t value;
	uint8_t exp;
	uint32_t x, y, z;
	uint32_t diff;

	if (tuned_freq < input_freq) {
		/* Check for Frequency Limit */
		if (tuned_freq < ((4 * input_freq) / 5)) {
			return false;
		}

		/* Set the ADD to 0 when freq less than input freq */
		add = false;
		/* Calculate the frequency difference */
		diff = input_freq - tuned_freq;
	} else if (tuned_freq > input_freq) {
		/* Check for Frequency Limit */
		if (tuned_freq > ((4 * input_freq) / 3)) {
			return false;
		}

		/* Set the ADD to 1 when freq greater than input freq */
		add = true;
		/* Calculate the frequency difference */
		diff = tuned_freq - input_freq;
	} else {
		/* required & input freq are equal */
		return true;
	}

	z = (tuned_freq) / (diff);
	if ((tuned_freq % diff) > (diff / 2)) {
		z++;
	}

	/*
	 * Initialize with minimum possible values.
	 * exp value should be greater than zero, min(exp) = 1 -> min(x)= (2^1)
	 *= 2
	 * y should be greater than one -> roundup(256/value) where value- 0 to
	 * 255
	 * min(y) = roundup(256/255) = 2
	 */
	y = 2;
	x = 2;
	exp = 1;

	/*
	 * Keep exp constant and increase y value until it reaches its limit.
	 * Increment exp and follow the same steps until we found the closest
	 * possible match for the required frequency.
	 */
	do {
		if (y < 255) {
			y++;
		} else {
			x = x << 1;
			y = 2;
			exp++;
		}
	} while (z > (x * y));

	/* Decrement y value after exit from loop */
	y = y - 1;
	/* Find VALUE from y */
	value = ROUNDUP_DIV(256, y);
	/* Initialize the Digital Tuner using the required parameters */
	ast_init_digital_tuner(ast, add, value, exp);

	return true;
}

/**
 * \brief This function will initialize the digital tuner of AST module.
 *
 * \param ast   Base address of the AST (i.e. &AVR32_AST).
 * \param add   set to true if frequency has to be increased, false if it
 *              has to be decreased.
 * \param value Parameter used in the formula
 * \param exp   Parameter used in the formula
 */
void ast_init_digital_tuner(volatile avr32_ast_t *ast, bool add,
		uint8_t value, uint8_t exp)
{
	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}

	ast->dtr = (((add << AVR32_AST_DTR_ADD_OFFSET) & AVR32_AST_DTR_ADD_MASK)
			| ((value <<
			AVR32_AST_DTR_VALUE_OFFSET) & AVR32_AST_DTR_VALUE_MASK)
			| ((exp <<
			AVR32_AST_DTR_EXP_OFFSET) & AVR32_AST_DTR_EXP_MASK));

	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}
}

/**
 * \brief This function will disable the digital tuner of AST module.
 *
 * \param ast  Base address of the AST (i.e. &AVR32_AST).
 */
void ast_disable_digital_tuner(volatile avr32_ast_t *ast)
{
	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}

	ast->dtr &= ~(AVR32_AST_DTR_VALUE_MASK);

	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}
}

/**
 * \brief This function will change the clock source of the AST module.
 *
 * \param ast Base address of the AST (i.e. &AVR32_AST).
 * \param osc_type The oscillator you want to use. If you need a better
 *        accuracy, use the 32 KHz oscillator (i.e. AST_OSC_32KHZ).
 * \param psel The preselector value for the corresponding oscillator (4-bits).
 *        To obtain this value, you can use this formula:
 *        psel = log(Fosc/Fast)/log(2)-1, where Fosc is the frequency of the
 *        oscillator you are using (32 KHz or 115 KHz) and Fast the frequency
 *        desired.
 *
 * \return Boolean true if the initialization succeeds, false otherwise.
 */
bool ast_change_clk_source(volatile avr32_ast_t *ast, uint8_t osc_type,
		uint8_t psel)
{
	uint32_t time_out = AST_POLL_TIMEOUT;
	while (ast_is_clkbusy(ast)) {
		if (--time_out == 0) {
			return false;
		}
	}

	/* Disable the clock */
	ast->clock &= ~(AVR32_AST_CLOCK_CEN_MASK);
	time_out = AST_POLL_TIMEOUT;
	while (ast_is_clkbusy(ast)) {
		if (--time_out == 0) {
			return false;
		}
	}

	/* Change the clock source */
	ast->clock = osc_type << AVR32_AST_CLOCK_CSSEL_OFFSET;
	time_out = AST_POLL_TIMEOUT;
	while (ast_is_clkbusy(ast)) {
		if (--time_out == 0) {
			return false;
		}
	}

	/* Enable the clock again */
	ast->clock |= AVR32_AST_CLOCK_CEN_MASK;
	time_out = AST_POLL_TIMEOUT;
	while (ast_is_clkbusy(ast)) {
		if (--time_out == 0) {
			return false;
		}
	}

	/* Set the new prescalar value */
	ast->cr = psel << AVR32_AST_CR_PSEL_OFFSET;
	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}

	return true;
}

/**
 * \brief This function sets the AST current calendar value.
 *
 * \param ast          Base address of the AST (i.e. &AVR32_AST).
 * \param ast_calendar Startup date
 */
void ast_set_calendar_value(volatile avr32_ast_t *ast,
		ast_calendar_t ast_calendar)
{
	/* Wait until we can write into the VAL register */
	while (ast_is_busy(ast)) {
	}

	/* Set the new val value */
	ast->calv = ast_calendar.field;

	/* Wait until write is done */
	while (ast_is_busy(ast)) {
	}
}

/**
 * \brief This function sets the AST current counter value.
 *
 * \param ast         Base address of the AST (i.e. &AVR32_AST).
 * \param ast_counter Startup counter value
 */
void ast_set_counter_value(volatile avr32_ast_t *ast,
		uint32_t ast_counter)
{
	/* Wait until we can write into the VAL register */
	while (ast_is_busy(ast)) {
	}

	/* Set the new val value */
	ast->cv = ast_counter;

	/* Wait until write is done */
	while (ast_is_busy(ast)) {
	}
}

/**
 * \brief This function returns the AST current calendar value.
 *
 * \param ast  Base address of the AST (i.e. &AVR32_AST).
 *
 * \return The AST current calendar value.
 */
ast_calendar_t ast_get_calendar_value(volatile avr32_ast_t *ast)
{
	ast_calendar_t ast_calendar;

	ast_calendar.field = ast->calv;

	return ast_calendar;
}

/**
 * \brief This function returns the AST current counter value.
 *
 * \param ast Base address of the AST (i.e. &AVR32_AST).
 *
 * \return The AST current counter value.
 */
uint32_t ast_get_counter_value(volatile avr32_ast_t *ast)
{
	return ast->cv;
}

/**
 * \brief This function Set the AST alarm0 value.
 *
 * \param ast         Base address of the AST (i.e. &AVR32_AST).
 * \param alarm_value AST alarm0 value.
 */
void ast_set_alarm0_value(volatile avr32_ast_t *ast, U32 alarm_value)
{
	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}

	/* Set the new alarm0 compare value */
	ast->ar0 = alarm_value;

	/* Wait until write is done */
	while (ast_is_busy(ast)) {
	}
}

/**
 * \brief This function Set the AST alarm1 value.
 *
 * \param ast         Base address of the AST (i.e. &AVR32_AST).
 * \param alarm_value AST alarm1 value.
 */
void ast_set_alarm1_value(volatile avr32_ast_t *ast, U32 alarm_value)
{
	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}

	/* Set the new alarm1 compare value */
	ast->ar1 = alarm_value;

	/* Wait until write is done */
	while (ast_is_busy(ast)) {
	}
}

/**
 * \brief This function Enable the AST alarm0 event.
 *
 * \param ast Base address of the AST (i.e. &AVR32_AST).
 */
void ast_enable_alarm0(volatile avr32_ast_t *ast)
{
	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}

	/* Enable the AST alarm0 peripheral event */
	ast->eve |= AVR32_AST_EVE_ALARM0_MASK;

	/* Wait until write is done */
	while (ast_is_busy(ast)) {
	}
}

/**
 * \brief This function Disable the AST alarm0.
 *
 * \param ast Base address of the AST (i.e. &AVR32_AST).
 */
void ast_disable_alarm0(volatile avr32_ast_t *ast)
{
	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}

	/* Disable the AST alarm0 peripheral event */
	ast->evd |= AVR32_AST_EVE_ALARM0_MASK;

	/* Wait until write is done */
	while (ast_is_busy(ast)) {
	}
}

/**
 * \brief This function Enable the AST alarm1.
 *
 * \param ast Base address of the AST (i.e. &AVR32_AST).
 */
void ast_enable_alarm1(volatile avr32_ast_t *ast)
{
	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}

	/* Enable the AST alarm1 peripheral event */
	ast->eve |= AVR32_AST_EVE_ALARM1_MASK;

	/* Wait until write is done */
	while (ast_is_busy(ast)) {
	}
}

/**
 * \brief This function Disable the AST alarm1 event.
 *
 * \param ast Base address of the AST (i.e. &AVR32_AST).
 */
void ast_disable_alarm1(volatile avr32_ast_t *ast)
{
	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}

	/* Disable the AST alarm1 peripheral event */
	ast->evd |= AVR32_AST_EVE_ALARM1_MASK;

	/* Wait until write is done */
	while (ast_is_busy(ast)) {
	}
}

/**
 * \brief This function Set the AST periodic0 value.
 *
 * \param ast Base address of the AST (i.e. &AVR32_AST).
 * \param pir AST periodic0.
 */
void ast_set_periodic0_value(volatile avr32_ast_t *ast, avr32_ast_pir0_t pir)
{
	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}

	/* Set the periodic prescaler value */
	ast->PIR0 = pir;

	/* Wait until write is done */
	while (ast_is_busy(ast)) {
	}
}

/**
 * \brief This function Set the AST periodic1 value.
 *
 * \param ast Base address of the AST (i.e. &AVR32_AST).
 * \param pir AST periodic1.
 */
void ast_set_periodic1_value(volatile avr32_ast_t *ast, avr32_ast_pir1_t pir)
{
	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}

	/* Set the periodic prescaler value */
	ast->PIR1 = pir;

	/* Wait until write is done */
	while (ast_is_busy(ast)) {
	}
}

/**
 * \brief This function Enable the AST periodic0 event.
 *
 * \param ast Base address of the AST (i.e. &AVR32_AST).
 */
void ast_enable_periodic0(volatile avr32_ast_t *ast)
{
	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}

	/* Enable the AST periodic0 peripheral event */
	ast->eve |= AVR32_AST_EVE_PER0_MASK;

	/* Wait until write is done */
	while (ast_is_busy(ast)) {
	}
}

/**
 * \brief This function Disable the AST periodic0 event.
 *
 * \param ast Base address of the AST (i.e. &AVR32_AST).
 */
void ast_disable_periodic0(volatile avr32_ast_t *ast)
{
	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}

	/* Disable the AST periodic0 peripheral event */
	ast->evd |= AVR32_AST_EVE_PER0_MASK;

	/* Wait until write is done */
	while (ast_is_busy(ast)) {
	}
}

/**
 * \brief This function Enable the AST periodic1 event.
 *
 * \param ast Base address of the AST (i.e. &AVR32_AST).
 */
void ast_enable_periodic1(volatile avr32_ast_t *ast)
{
	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}

	/* Enable the periodic1 peripheral event */
	ast->eve |= AVR32_AST_EVE_PER1_MASK;

	/* Wait until write is done */
	while (ast_is_busy(ast)) {
	}
}

/**
 * \brief This function Disable the AST periodic1 event.
 *
 * \param ast Base address of the AST (i.e. &AVR32_AST).
 */
void ast_disable_periodic1(volatile avr32_ast_t *ast)
{
	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}

	/* Disable the AST periodic1 peripheral event */
	ast->evd |= AVR32_AST_EVE_PER0_MASK;

	/* Wait until write is done */
	while (ast_is_busy(ast)) {
	}
}

/**
 * \brief This function clears the AST status flags.
 *
 * \param ast          Base address of the AST (i.e. &AVR32_AST).
 * \param status_mask  AST status flag to be cleared
 */
void ast_clear_status_flag(volatile avr32_ast_t *ast,
		uint32_t status_mask)
{
	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}

	/* Clear the AST status flags */
	ast->scr = status_mask;

	/* Wait until write is done */
	while (ast_is_busy(ast)) {
	}
}

/**
 * \brief This function clears the AST Alarm status flags.
 *
 * \param ast            Base address of the AST (i.e. &AVR32_AST).
 * \param alarm_channel  AST wake-up flag to be cleared.
 */
void ast_clear_alarm_status_flag(volatile avr32_ast_t *ast,
		uint32_t alarm_channel)
{
	if (alarm_channel) {
		/* Clear the AST alarm status flag */
		ast_clear_status_flag(ast, AVR32_AST_SCR_ALARM1_MASK);
	} else {
		/* Clear the AST alarm status flag */
		ast_clear_status_flag(ast, AVR32_AST_SCR_ALARM0_MASK);
	}
}

/**
 * \brief This function clears the AST Periodic event status flags.
 *
 * \param ast            Base address of the AST (i.e. &AVR32_AST).
 * \param periodic_channel  AST wake-up flag to be cleared.
 */
void ast_clear_periodic_status_flag(volatile avr32_ast_t *ast,
		uint32_t periodic_channel)
{
	if (periodic_channel) {
		/* Clear the AST periodic event status flag */
		ast_clear_status_flag(ast, AVR32_AST_SCR_PER1_MASK);
	} else {
		/* Clear the AST periodic event status flag */
		ast_clear_status_flag(ast, AVR32_AST_SCR_PER0_MASK);
	}
}

/**
 * \brief This function clears the AST Overflow status flags.
 *
 * \param ast          Base address of the AST (i.e. &AVR32_AST).
 */
void ast_clear_ovf_status_flag(volatile avr32_ast_t *ast)
{
	/* Clear the AST overflow status flag */
	ast_clear_status_flag(ast, AVR32_AST_SCR_OVF_MASK);
}

/**
 * \brief This function clears the AST Ready status flags.
 *
 * \param ast          Base address of the AST (i.e. &AVR32_AST).
 */
void ast_clear_ready_status_flag(volatile avr32_ast_t *ast)
{
	/* Clear the AST ready status flag */
	ast_clear_status_flag(ast, AVR32_AST_SCR_READY_MASK);
}

/**
 * \brief This function clears the AST Clock Ready status flags.
 *
 * \param ast          Base address of the AST (i.e. &AVR32_AST).
 */
void ast_clear_clkrdy_status_flag(volatile avr32_ast_t *ast)
{
	/* Clear the AST clock ready status flag */
	ast_clear_status_flag(ast, AVR32_AST_SCR_CLKRDY_MASK);
}

/**
 * \brief This function clears all the AST status flags.
 *
 * \param ast  Base address of the AST (i.e. &AVR32_AST).
 */
void ast_clear_all_status_flags(volatile avr32_ast_t *ast)
{
	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}

	/* Clear the AST status register */
	ast->scr = ~0;

	/* Wait until write is done */
	while (ast_is_busy(ast)) {
	}
}

/**
 * \brief This function enables the AST interrupt
 *
 * \param ast             Base address of the AST (i.e. &AVR32_AST).
 * \param interrupt_mask  AST Interrupts to be enabled
 */
void ast_enable_interrupt(volatile avr32_ast_t *ast,
		uint32_t interrupt_mask)
{
	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}

	/* Enable the AST interrupt */
	ast->ier |= interrupt_mask;

	/* Wait until write is done */
	while (ast_is_busy(ast)) {
	}
}

/**
 * \brief This function enables the AST Alarm interrupt.
 *
 * \param ast           Base address of the AST (i.e. &AVR32_AST).
 * \param alarm_channel AST Alarm Channel
 */
void ast_enable_alarm_interrupt(volatile avr32_ast_t *ast,
		uint8_t alarm_channel)
{
	/* Enable the AST Alarm Asynchronous Wake-up */
	if (alarm_channel) {
		ast_enable_interrupt(ast, AVR32_AST_IER_ALARM1_MASK);
	} else {
		ast_enable_interrupt(ast, AVR32_AST_IER_ALARM0_MASK);
	}
}

/**
 * \brief This function enables the AST Periodic interrupt.
 *
 * \param ast              Base address of the AST (i.e. &AVR32_AST).
 * \param periodic_channel AST Periodic Channel
 */
void ast_enable_periodic_interrupt(volatile avr32_ast_t *ast,
		uint8_t periodic_channel)
{
	/* Enable the AST Periodic Asynchronous Wake-up */
	if (periodic_channel) {
		ast_enable_interrupt(ast, AVR32_AST_IER_PER1_MASK);
	} else {
		ast_enable_interrupt(ast, AVR32_AST_IER_PER0_MASK);
	}
}

/**
 * \brief This function enables the AST Overflow interrupt.
 *
 * \param ast              Base address of the AST (i.e. &AVR32_AST).
 */
void ast_enable_ovf_interrupt(volatile avr32_ast_t *ast)
{
	/* Enable the AST Counter Overflow Asynchronous Wake-up */
	ast_enable_interrupt(ast, AVR32_AST_IER_OVF_MASK);
}

/**
 * \brief This function enables the AST Ready interrupt.
 *
 * \param ast              Base address of the AST (i.e. &AVR32_AST).
 */
void ast_enable_ready_interrupt(volatile avr32_ast_t *ast)
{
	/* Enable the AST Counter Overflow Asynchronous Wake-up */
	ast_enable_interrupt(ast, AVR32_AST_IER_READY_MASK);
}

/**
 * \brief This function enables the AST Ready interrupt.
 *
 * \param ast              Base address of the AST (i.e. &AVR32_AST).
 */
void ast_enable_clkrdy_interrupt(volatile avr32_ast_t *ast)
{
	/* Enable the AST Counter Overflow Asynchronous Wake-up */
	ast_enable_interrupt(ast, AVR32_AST_IER_CLKRDY_MASK);
}

/**
 * \brief This function disables the AST interrupts.
 *
 * \param ast              Base address of the AST (i.e. &AVR32_AST).
 * \param interrupt_mask   AST Interrupts to be disabled
 */
void ast_disable_interrupt(volatile avr32_ast_t *ast,
		uint32_t interrupt_mask)
{
	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}

	/* Disable the AST interrupt */
	ast->idr = interrupt_mask;

	/* Wait until write is done */
	while (ast_is_busy(ast)) {
	}
}

/**
 * \brief This function disables the AST Alarm interrupt.
 *
 * \param ast           Base address of the AST (i.e. &AVR32_AST).
 * \param alarm_channel AST Alarm Channel
 */
void ast_disable_alarm_interrupt(volatile avr32_ast_t *ast,
		uint8_t alarm_channel)
{
	/* Disable the AST Alarm Asynchronous Wake-up */
	if (alarm_channel) {
		ast_disable_interrupt(ast, AVR32_AST_IER_ALARM1_MASK);
	} else {
		ast_disable_interrupt(ast, AVR32_AST_IER_ALARM0_MASK);
	}
}

/**
 * \brief This function disables the AST Periodic interrupt.
 *
 * \param ast              Base address of the AST (i.e. &AVR32_AST).
 * \param periodic_channel AST Periodic Channel
 */
void ast_disable_periodic_interrupt(volatile avr32_ast_t *ast,
		uint8_t periodic_channel)
{
	/* Disable the AST Periodic Asynchronous Wake-up */
	if (periodic_channel) {
		ast_disable_interrupt(ast, AVR32_AST_IER_PER1_MASK);
	} else {
		ast_disable_interrupt(ast, AVR32_AST_IER_PER0_MASK);
	}
}

/**
 * \brief This function disables the AST Overflow interrupt.
 *
 * \param ast              Base address of the AST (i.e. &AVR32_AST).
 */
void ast_disable_ovf_interrupt(volatile avr32_ast_t *ast)
{
	/* Disable the AST Counter Overflow Asynchronous Wake-up */
	ast_disable_interrupt(ast, AVR32_AST_IER_OVF_MASK);
}

/**
 * \brief This function enables the AST Ready interrupt.
 *
 * \param ast              Base address of the AST (i.e. &AVR32_AST).
 */
void ast_disable_ready_interrupt(volatile avr32_ast_t *ast)
{
	/* Disable the AST Counter Overflow Asynchronous Wake-up */
	ast_disable_interrupt(ast, AVR32_AST_IER_READY_MASK);
}

/**
 * \brief This function disables the AST Ready interrupt.
 *
 * \param ast              Base address of the AST (i.e. &AVR32_AST).
 */
void ast_disable_clkrdy_interrupt(volatile avr32_ast_t *ast)
{
	/* Disable the AST Counter Overflow Asynchronous Wake-up */
	ast_disable_interrupt(ast, AVR32_AST_IER_CLKRDY_MASK);
}

/**
 * \brief This function disables all the AST interrupts.
 *
 * \param ast              Base address of the AST (i.e. &AVR32_AST).
 */
void ast_disable_all_interrupts(volatile avr32_ast_t *ast)
{
	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}

	/* Disable the AST interrupt */
	ast->idr = ~0;

	/* Wait until write is done */
	while (ast_is_busy(ast)) {
	}
}

/**
 * \brief This function enables the AST Asynchronous wake-up.
 *
 * \param ast          Base address of the AST (i.e. &AVR32_AST).
 * \param wakeup_mask  AST wake-up flag to be enabled.
 */
void ast_enable_async_wakeup(volatile avr32_ast_t *ast,
		uint32_t wakeup_mask)
{
	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}

	/* Enable the AST Asynchronous Wake-up */
	ast->wer |= wakeup_mask;

	/* Wait until write is done */
	while (ast_is_busy(ast)) {
	}
}

/**
 * \brief This function enables the AST Alarm Asynchronous wake-up.
 *
 * \param ast           Base address of the AST (i.e. &AVR32_AST).
 * \param alarm_channel AST Alarm Channel
 */
void ast_enable_alarm_async_wakeup(volatile avr32_ast_t *ast,
		uint8_t alarm_channel)
{
	/* Enable the AST Alarm Asynchronous Wake-up */
	if (alarm_channel) {
		ast_enable_async_wakeup(ast, AVR32_AST_WER_ALARM1_MASK);
	} else {
		ast_enable_async_wakeup(ast, AVR32_AST_WER_ALARM0_MASK);
	}
}

/**
 * \brief This function enables the AST Periodic Asynchronous wake-up.
 *
 * \param ast              Base address of the AST (i.e. &AVR32_AST).
 * \param periodic_channel AST Periodic Channel
 */
void ast_enable_periodic_async_wakeup(volatile avr32_ast_t *ast,
		uint8_t periodic_channel)
{
	/* Enable the AST Periodic Asynchronous Wake-up */
	if (periodic_channel) {
		ast_enable_async_wakeup(ast, AVR32_AST_WER_PER1_MASK);
	} else {
		ast_enable_async_wakeup(ast, AVR32_AST_WER_PER0_MASK);
	}
}

/**
 * \brief This function enables the AST Overflow Asynchronous wake-up.
 *
 * \param ast              Base address of the AST (i.e. &AVR32_AST).
 */
void ast_enable_ovf_async_wakeup(volatile avr32_ast_t *ast)
{
	/* Enable the AST Counter Overflow Asynchronous Wake-up */
	ast_enable_async_wakeup(ast, AVR32_AST_WER_OVF_MASK);
}

/**
 * \brief This function disables the AST Asynchronous wake-up.
 * 8
 * \param ast          Base address of the AST (i.e. &AVR32_AST).
 * \param wakeup_mask  AST wake-up flag to be disabled.
 */
void ast_disable_async_wakeup(volatile avr32_ast_t *ast,
		uint32_t wakeup_mask)
{
	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}

	/* Disable the AST Asynchronous Wake-up */
	ast->wer &= ~(wakeup_mask);

	/* Wait until write is done */
	while (ast_is_busy(ast)) {
	}
}

/**
 * \brief This function disables the AST Alarm Asynchronous wake-up.
 *
 * \param ast           Base address of the AST (i.e. &AVR32_AST).
 * \param alarm_channel AST Alarm Channel
 */
void ast_disable_alarm_async_wakeup(volatile avr32_ast_t *ast,
		uint8_t alarm_channel)
{
	/* Disable the AST Alarm Asynchronous Wake-up */
	if (alarm_channel) {
		ast_disable_async_wakeup(ast, AVR32_AST_WER_ALARM1_MASK);
	} else {
		ast_disable_async_wakeup(ast, AVR32_AST_WER_ALARM0_MASK);
	}
}

/**
 * \brief This function disables the AST Periodic Asynchronous wake-up.
 *
 * \param ast              Base address of the AST (i.e. &AVR32_AST).
 * \param periodic_channel AST Periodic Channel
 */
void ast_disable_periodic_async_wakeup(volatile avr32_ast_t *ast,
		uint8_t periodic_channel)
{
	/* Disable the AST Periodic Asynchronous Wake-up */
	if (periodic_channel) {
		ast_disable_async_wakeup(ast, AVR32_AST_WER_PER1_MASK);
	} else {
		ast_disable_async_wakeup(ast, AVR32_AST_WER_PER0_MASK);
	}
}

/**
 * \brief This function disables the AST Overflow Asynchronous wake-up.
 *
 * \param ast              Base address of the AST (i.e. &AVR32_AST).
 */
void ast_disable_ovf_async_wakeup(volatile avr32_ast_t *ast)
{
	/* Disable the AST Counter Overflow Asynchronous Wake-up */
	ast_disable_async_wakeup(ast, AVR32_AST_WER_OVF_MASK);
}

/**
 * \brief This function enables the option to clear the counter on AST alarm.
 *
 * \param ast            Base address of the AST (i.e. &AVR32_AST).
 * \param alarm_channel  AST Alarm Channel.
 */
void ast_enable_counter_clear_on_alarm(volatile avr32_ast_t *ast,
		uint8_t alarm_channel)
{
	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}

	/* Enable Clear Counter on Alarm */
	ast->cr
		|= (alarm_channel ?
			AVR32_AST_CR_CA1_MASK : AVR32_AST_CR_CA0_MASK);

	/* Wait until write is done */
	while (ast_is_busy(ast)) {
	}
}

/**
 * \brief This function clears the AST periodic prescalar counter to zero
 *
 * \param ast            Base address of the AST (i.e. &AVR32_AST).
 */
void ast_clear_prescalar(volatile avr32_ast_t *ast)
{
	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}

	/* Clear Counter on Alarm */
	ast->cr |= AVR32_AST_CR_PCLR_MASK;

	/* Wait until write is done */
	while (ast_is_busy(ast)) {
	}
}
