/**
 * \file tc_megarf.c
 *
 * \brief AVR MEGA TC Driver
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
#include <stdint.h>
#include "interrupt.h"
#include "compiler.h"
#include "parts.h"
#include "tc_megarf.h"
#include "sysclk.h"

#include "status_codes.h"

/* ! \internal Local storage of Timer Counter TCC1  interrupt callback function
**/
static tc_callback_t tc_tccr1_ovf_callback;
static tc_callback_t tc_tccr1_compa_callback;
static tc_callback_t tc_tccr1_compb_callback;
static tc_callback_t tc_tccr1_compc_callback;

/**
 * \internal
 * \brief Interrupt handler for Timer Counter  overflow
 */
ISR(TIMER1_OVF_vect)
{
	if (tc_tccr1_ovf_callback) {
		tc_tccr1_ovf_callback();
	}
}

/**
 * \brief Interrupt handler for Timer Counter  Compare Match in Channel A
 */
ISR(TIMER1_COMPA_vect)
{
	if (tc_tccr1_compa_callback) {
		tc_tccr1_compa_callback();
	}
}

/**
 * \brief Interrupt handler for Timer Counter  Compare Match in Channel B
 */
ISR(TIMER1_COMPB_vect)
{
	if (tc_tccr1_compb_callback) {
		tc_tccr1_compb_callback();
	}
}

/**
 * \brief Interrupt handler for Timer Counter  Compare Match in Channel C
 */
ISR(TIMER1_COMPC_vect)
{
	if (tc_tccr1_compc_callback) {
		tc_tccr1_compc_callback();
	}
}

/* !  Local storage of Timer Counter  TCC3 interrupt callback function */
static tc_callback_t tc_tccr3_ovf_callback;
static tc_callback_t tc_tccr3_compa_callback;
static tc_callback_t tc_tccr3_compb_callback;
static tc_callback_t tc_tccr3_compc_callback;

ISR(TIMER3_OVF_vect)
{
	if (tc_tccr3_ovf_callback) {
		tc_tccr3_ovf_callback();
	}
}

ISR(TIMER3_COMPA_vect)
{
	if (tc_tccr3_compa_callback) {
		tc_tccr3_compa_callback();
	}
}

ISR(TIMER3_COMPB_vect)
{
	if (tc_tccr3_compb_callback) {
		tc_tccr3_compb_callback();
	}
}

ISR(TIMER3_COMPC_vect)
{
	if (tc_tccr3_compc_callback) {
		tc_tccr3_compc_callback();
	}
}

/* !  Local storage of Timer Counter  TCC4 interrupt callback function */
static tc_callback_t tc_tccr4_ovf_callback;
static tc_callback_t tc_tccr4_compa_callback;
static tc_callback_t tc_tccr4_compb_callback;
static tc_callback_t tc_tccr4_compc_callback;

ISR(TIMER4_OVF_vect)
{
	if (tc_tccr4_ovf_callback) {
		tc_tccr4_ovf_callback();
	}
}

ISR(TIMER4_COMPA_vect)
{
	if (tc_tccr4_compa_callback) {
		tc_tccr4_compa_callback();
	}
}

ISR(TIMER4_COMPB_vect)
{
	if (tc_tccr4_compb_callback) {
		tc_tccr4_compb_callback();
	}
}

ISR(TIMER4_COMPC_vect)
{
	if (tc_tccr4_compc_callback) {
		tc_tccr4_compc_callback();
	}
}

/* !  Local storage of Timer Counter  TCC5 interrupt callback function */
static tc_callback_t tc_tccr5_ovf_callback;
static tc_callback_t tc_tccr5_compa_callback;
static tc_callback_t tc_tccr5_compb_callback;
static tc_callback_t tc_tccr5_compc_callback;

ISR(TIMER5_OVF_vect)
{
	if (tc_tccr5_ovf_callback) {
		tc_tccr5_ovf_callback();
	}
}

ISR(TIMER5_COMPA_vect)
{
	if (tc_tccr5_compa_callback) {
		tc_tccr5_compa_callback();
	}
}

ISR(TIMER5_COMPB_vect)
{
	if (tc_tccr5_compb_callback) {
		tc_tccr5_compb_callback();
	}
}

ISR(TIMER5_COMPC_vect)
{
	if (tc_tccr5_compc_callback) {
		tc_tccr5_compc_callback();
	}
}

void tc_enable(volatile void *tc)
{
	irqflags_t iflags = cpu_irq_save();

	sysclk_enable_peripheral_clock(tc);

	cpu_irq_restore(iflags);
}

void tc_disable(volatile void *tc)
{
	irqflags_t iflags = cpu_irq_save();

	sysclk_disable_peripheral_clock(tc);

	cpu_irq_restore(iflags);
}

void tc_set_overflow_interrupt_callback(volatile void *tc,
		tc_callback_t callback)
{
	if ((uintptr_t)tc == (uintptr_t)&TCCR1A) {
		tc_tccr1_ovf_callback = callback;
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR3A) {
		tc_tccr3_ovf_callback = callback;
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR4A) {
		tc_tccr4_ovf_callback = callback;
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR5A) {
		tc_tccr5_ovf_callback = callback;
	} else {}
}

void tc_set_compa_interrupt_callback(volatile void *tc, tc_callback_t callback)
{
	if ((uintptr_t)tc == (uintptr_t)&TCCR1A) {
		tc_tccr1_compa_callback = callback;
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR3A) {
		tc_tccr3_compa_callback = callback;
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR4A) {
		tc_tccr4_compa_callback = callback;
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR5A) {
		tc_tccr5_compa_callback = callback;
	} else {}
}

void tc_set_compb_interrupt_callback(volatile void *tc, tc_callback_t callback)
{
	if ((uintptr_t)tc == (uintptr_t)&TCCR1A) {
		tc_tccr1_compb_callback = callback;
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR3A) {
		tc_tccr3_compb_callback = callback;
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR4A) {
		tc_tccr4_compb_callback = callback;
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR5A) {
		tc_tccr5_compb_callback = callback;
	} else {}
}

void tc_set_compc_interrupt_callback(volatile void *tc, tc_callback_t callback)
{
	if ((uintptr_t)tc == (uintptr_t)&TCCR1A) {
		tc_tccr1_compc_callback = callback;
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR3A) {
		tc_tccr3_compc_callback = callback;
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR4A) {
		tc_tccr4_compc_callback = callback;
	} else if ((uintptr_t)tc == (uintptr_t)&TCCR5A) {
		tc_tccr5_compc_callback = callback;
	} else {}
}
