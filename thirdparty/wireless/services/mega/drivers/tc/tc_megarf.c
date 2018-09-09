/**
 * \file tc_megarf.c
 *
 * \brief AVR MEGA TC Driver
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
