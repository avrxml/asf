/**
 * \file tc_megarf.h
 *
 * \brief AVR MEGA Timer Counter (TC) driver
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
#ifndef _TC_H_
#define _TC_H_

/* Timer prescalars */
typedef enum TC_CLKSEL_enum {
	TC_CLKSEL_OFF_gc = (0x00),      /* /< Timer Off */
	TC_CLKSEL_DIV1_gc = (0x01),     /* /< System Clock */
	TC_CLKSEL_DIV8_gc = (0x02),     /* /< System Clock / 8 */
	TC_CLKSEL_DIV64_gc = (0x03),    /* /< System Clock / 64 */
	TC_CLKSEL_DIV256_gc = (0x04),   /* /< System Clock / 256 */
	TC_CLKSEL_DIV1024_gc = (0x05),  /* /< System Clock / 1024 */
	TC_EXT_CLK_FALLING_gc = (0x06), /* /< External Clock source on falling
	                                 * edge */
	TC_EXT_CLK_RISING_gc = (0x07)   /* /< External Clock source on rising
	                                 * edge */
} TC_CLKSEL_t;

/* Timer Modes of operation */
typedef enum TC_MODE_enum {
	NORMAL     = 0X00,
	PWM_Mode1  = 0X01,
	PWM_Mode2  = 0X02,
	PWM_Mode3   = 0X03,
	CTC_Mode1   = 0X04,
	PWM_Mode4   = 0X05,
	PWM_Mode5   = 0X06,
	PWM_Mode6   = 0X07,
	PWM_Mode7   = 0X08,
	PWM_Mode8   = 0X09,
	PWM_Mode9   = 0X0A,
	PWM_Mode10   = 0X0B,
	CTC_Mode2   = 0X0C,     /* 0X0D is reserved */
	PWM_Mode11   = 0X0E,
	PWM_Mode12   = 0X0F
} TC_MODE_t;
#define TCCRB_OFFSET 1

#define TCNT_OFFSET  4

/* ! Timer Counter Capture Compare Channel index */
enum tc_cc_channel_t {
	/* ! Channel A */
	TC_COMPA = 0X08,
	/* ! Channel B */
	TC_COMPB = 0X0A,
	/* ! Channel C */
	TC_COMPC = 0X0C,
};

/**
 * \brief Interrupt event callback function type
 *
 * The interrupt handler can be configured to do a function callback,
 * the callback function must match the tc_callback_t type.
 *
 */
typedef void (*tc_callback_t)(void);

/**
 * @brief Enable Timer
 * @param tc Timer Address
 */
void tc_enable(volatile void *tc);

/**
 * @brief Disable Timer
 * @param tc Timer Address
 */
void tc_disable(volatile void *tc);

/**
 * @brief Register a Overflow interrupt callback
 * @param tc Timer Address
 * @param callback Callback called once Overflow interrupt occurs
 */
void tc_set_overflow_interrupt_callback(volatile void *tc,
		tc_callback_t callback);

/**
 * @brief Register a Compare interrupt callback
 * @param tc Timer Address
 * @param callback Callback called once Compare interrupt occurs in channel A
 */
void tc_set_compa_interrupt_callback(volatile void *tc, tc_callback_t callback);

/**
 * @brief Register a Compare interrupt callback
 * @param tc Timer Address
 * @param callback Callback called once Compare interrupt occurs in channel B
 */
void tc_set_compb_interrupt_callback(volatile void *tc, tc_callback_t callback);

/**
 * @brief Register a Compare interrupt callback
 * @param tc Timer Address
 * @param callback Callback called once Compare interrupt occurs in channel C
 */
void tc_set_compc_interrupt_callback(volatile void *tc, tc_callback_t callback);

/**
 * @brief Select a source for a timer
 * @param tc Timer Address
 * @param TC_CLKSEL_enum Select a source from enum type
 */
static inline void tc_write_clock_source(volatile void *tc,
		TC_CLKSEL_t TC_CLKSEL_enum)
{
	if ((uintptr_t)tc == (uintptr_t)&TCCR1A) {
		TCCR1B |=  TC_CLKSEL_enum;
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR3A) {
		TCCR3B |=  TC_CLKSEL_enum;
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR4A) {
		TCCR4B |=  TC_CLKSEL_enum;
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR5A) {
		TCCR5B |=  TC_CLKSEL_enum;
	} else {}
}

/**
 * @brief Enable Overflow Interrupt
 * @param tc Timer Address
 */
static inline void tc_enable_ovf_int(volatile void *tc)
{
	if ((uintptr_t)tc == (uintptr_t)&TCCR1A) {
		TIMSK1 |= (1 << TOIE1);
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR3A) {
		TIMSK3 |= (1 << TOIE3);
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR4A) {
		TIMSK4 |= (1 << TOIE4);
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR5A) {
		TIMSK5 |= (1 << TOIE5);
	} else {}
}

/**
 * @brief Enable Compare Interrupt in channel A
 * @param tc Timer Address
 */
static inline void tc_enable_compa_int(volatile void *tc)
{
	if ((uintptr_t)tc == (uintptr_t)&TCCR1A) {
		TIMSK1 |= (1 << OCIE1A);
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR3A) {
		TIMSK3 |= (1 << OCIE3A);
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR4A) {
		TIMSK4 |= (1 << OCIE4A);
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR5A) {
		TIMSK5 |= (1 << OCIE5A);
	} else {}
}

/**
 * @brief Enable Compare Interrupt in channel B
 * @param tc Timer Address
 */
static inline void tc_enable_compb_int(volatile void *tc)
{
	if ((uintptr_t)tc == (uintptr_t)&TCCR1A) {
		TIMSK1 |= (1 << OCIE1B);
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR3A) {
		TIMSK3 |= (1 << OCIE3B);
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR4A) {
		TIMSK4 |= (1 << OCIE4B);
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR5A) {
		TIMSK5 |= (1 << OCIE5B);
	} else {}
}

/**
 * @brief Enable Compare Interrupt in channel C
 * @param tc Timer Address
 */
static inline void tc_enable_compc_int(volatile void *tc)
{
	if ((uintptr_t)tc == (uintptr_t)&TCCR1A) {
		TIMSK1 |= (1 << OCIE1C);
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR3A) {
		TIMSK3 |= (1 << OCIE3C);
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR4A) {
		TIMSK4 |= (1 << OCIE4C);
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR5A) {
		TIMSK5 |= (1 << OCIE5C);
	} else {}
}

/**
 * @brief Disable Overflow Interrupt
 * @param tc Timer Address
 */
static inline void tc_disable_ovf_int(volatile void *tc)
{
	if ((uintptr_t)tc == (uintptr_t)&TCCR1A) {
		TIMSK1 &= ~(1 << TOIE1);
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR3A) {
		TIMSK3 &= ~(1 << TOIE3);
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR4A) {
		TIMSK4 &= ~(1 << TOIE4);
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR5A) {
		TIMSK5 &= ~(1 << TOIE5);
	} else {}
}

/**
 * @brief Disable Compare Interrupt in channel A
 * @param tc Timer Address
 */
static inline void tc_disable_compa_int(volatile void *tc)
{
	if ((uintptr_t)tc == (uintptr_t)&TCCR1A) {
		TIMSK1 &= ~(1 << OCIE1A);
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR3A) {
		TIMSK3 &= ~(1 << OCIE3A);
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR4A) {
		TIMSK4 &= ~(1 << OCIE4A);
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR5A) {
		TIMSK5 &= ~(1 << OCIE5A);
	} else {}
}

/**
 * @brief Disable Compare Interrupt in channel B
 * @param tc Timer Address
 */
static inline void tc_disable_compb_int(volatile void *tc)
{
	if ((uintptr_t)tc == (uintptr_t)&TCCR1A) {
		TIMSK1 &= ~(1 << OCIE1B);
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR3A) {
		TIMSK3 &= ~(1 << OCIE3B);
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR4A) {
		TIMSK4 &= ~(1 << OCIE4B);
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR5A) {
		TIMSK5 &= ~(1 << OCIE5B);
	} else {}
}

/**
 * @brief Disable Compare Interrupt in channel C
 * @param tc Timer Address
 */
static inline void tc_disable_compc_int(volatile void *tc)
{
	if ((uintptr_t)tc == (uintptr_t)&TCCR1A) {
		TIMSK1 &= ~(1 << OCIE1C);
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR3A) {
		TIMSK3 &= ~(1 << OCIE3C);
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR4A) {
		TIMSK4 &= ~(1 << OCIE4C);
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR5A) {
		TIMSK5 &= ~(1 << OCIE5C);
	} else {}
}

/**
 * @brief Write Compare register with timer Value
 * @param tc Timer Address
 * @param channel_index Compare Channel to be used
 * @param value Compare value to be written
 */
static inline void tc_write_cc(volatile void *tc,
		enum tc_cc_channel_t channel_index, uint16_t value)
{
	uint8_t *reg = (uint8_t *)tc;
	*(reg + channel_index + 1) |=  (value >> 8);
	*(reg + channel_index) |=  value;
}

/**
 * @brief Checks whether a timer has overflowed
 * @param tc Timer Address
 */
static inline bool tc_is_overflow(volatile void *tc)
{
	if ((uintptr_t)tc == (uintptr_t)&TCCR1A) {
		return (TIFR1 & (1 << TOV1));
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR3A) {
		return (TIFR3 & (1 << TOV3));
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR4A) {
		return (TIFR4 & (1 << TOV4));
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR5A) {
		return (TIFR5 & (1 << TOV5));
	} else {
		return false;
	}
}

/**
 * @brief Clears Overflow Flag
 * @param tc Timer Address
 */
static inline void clear_ovf_flag(volatile void *tc)
{
	if ((uintptr_t)tc == (uintptr_t)&TCCR1A) {
		TIFR1 |= (1 << TOV1);
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR3A) {
		(TIFR3 |= (1 << TOV3));
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR4A) {
		(TIFR4 |= (1 << TOV4));
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR5A) {
		(TIFR5 |= (1 << TOV5));
	} else {}
}

/**
 * @brief Checks whether a Compare Match has occured in Channel A
 * @param tc Timer Address
 */
static inline bool tc_is_compa_match(volatile void *tc)
{
	if ((uintptr_t)tc == (uintptr_t)&TCCR1A) {
		return (TIFR1 & (1 << OCF1A));
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR3A) {
		return (TIFR3 & (1 << OCF3A));
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR4A) {
		return (TIFR4 & (1 << OCF4A));
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR5A) {
		return (TIFR5 & (1 << OCF5A));
	} else {
		return false;
	}
}

/**
 * @brief Clears Compare Match  Flag in channel A
 * @param tc Timer Address
 */
static inline void clear_compa_flag(volatile void *tc)
{
	if ((uintptr_t)tc == (uintptr_t)&TCCR1A) {
		TIFR1 |= (1 << OCF1A);
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR3A) {
		(TIFR3 |= (1 << OCF1A));
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR4A) {
		(TIFR4 |= (1 << OCF1A));
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR5A) {
		(TIFR5 |= (1 << OCF1A));
	} else {}
}

/**
 * @brief Checks whether a Compare Match has occured in Channel B
 * @param tc Timer Address
 */
static inline bool tc_is_compb_match(volatile void *tc)
{
	if ((uintptr_t)tc == (uintptr_t)&TCCR1A) {
		return (TIFR1 & (1 << OCF1B));
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR3A) {
		return (TIFR3 & (1 << OCF3B));
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR4A) {
		return (TIFR4 & (1 << OCF4B));
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR5A) {
		return (TIFR5 & (1 << OCF5B));
	} else {
		return false;
	}
}

/**
 * @brief Clears Compare Match  Flag in channel B
 * @param tc Timer Address
 */
static inline void clear_compb_flag(volatile void *tc)
{
	if ((uintptr_t)tc == (uintptr_t)&TCCR1A) {
		TIFR1 |= (1 << OCF1B);
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR3A) {
		(TIFR3 |= (1 << OCF1B));
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR4A) {
		(TIFR4 |= (1 << OCF1B));
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR5A) {
		(TIFR5 |= (1 << OCF1B));
	} else {}
}

/**
 * @brief Checks whether a Compare Match has occured in Channel C
 * @param tc Timer Address
 */
static inline bool tc_is_compc_match(volatile void *tc)
{
	if ((uintptr_t)tc == (uintptr_t)&TCCR1A) {
		return (TIFR1 & (1 << OCF1C));
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR3A) {
		return (TIFR3 & (1 << OCF3C));
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR4A) {
		return (TIFR4 & (1 << OCF4C));
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR5A) {
		return (TIFR5 & (1 << OCF5C));
	} else {
		return false;
	}
}

/**
 * @brief Clears Compare Match  Flag in channel C
 * @param tc Timer Address
 */
static inline void clear_compc_flag(volatile void *tc)
{
	if ((uintptr_t)tc == (uintptr_t)&TCCR1A) {
		TIFR1 |= (1 << OCF1C);
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR3A) {
		(TIFR3 |= (1 << OCF1C));
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR4A) {
		(TIFR4 |= (1 << OCF1C));
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR5A) {
		(TIFR5 |= (1 << OCF1C));
	} else {}
}

/**
 * @brief Writes a count value to the Timer Counter Register
 * @param tc Timer Address
 * @param value count to be written in the register
 */
static inline void tc_write_count(volatile void *tc, uint16_t value)
{
	if ((uintptr_t)tc == (uintptr_t)&TCCR1A) {
		TCNT1 = value;
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR3A) {
		TCNT3 = value;
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR4A) {
		TCNT4 = value;
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR5A) {
		TCNT5 = value;
	} else {}
}

/**
 * @brief Reads the  count value in the Timer Counter Register
 * @param tc Timer Address
 * @return  count  in the register
 */
static inline uint16_t tc_read_count(volatile void *tc)
{
	if ((uintptr_t)tc == (uintptr_t)&TCCR1A) {
		return TCNT1;
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR3A) {
		return TCNT3;
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR4A) {
		return TCNT4;
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR5A) {
		return TCNT5;
	}

	return 0;
}

/**
 * @brief Sets a timer in a particular mode of operation
 * @param tc Timer Address
 * @param mode Enum value of the selected mode
 */
static inline void tc_set_mode(volatile void *tc, TC_MODE_t mode)
{
	uint8_t *reg = (uint8_t *)tc;
	if (mode == NORMAL) {
		*(reg) |=  ((0 << WGM10) || ((0 << WGM11)));
	} else if (mode == CTC_Mode1) {
		*(reg + TCCRB_OFFSET) |= (1 << WGM12);
	}
}

#endif /* _TC_H_ */
