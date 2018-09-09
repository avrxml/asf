/*****************************************************************************
 *
 * \file
 *
 * \brief EIC driver for AVR32 UC3.
 *
 * AVR32 External Interrupt Controller driver module.
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


#include <avr32/io.h>
#include "compiler.h"
#include "preprocessor.h"
#include "eic.h"



void eic_init(volatile avr32_eic_t *eic, const eic_options_t *opt, uint32_t nb_lines)
{
	int i;
	for (i = 0; i < nb_lines; i++)
	{
		// Set up mode level
		eic->mode = (opt[i].eic_mode == 1)
			? (eic->mode | (1 << opt[i].eic_line))
			: (eic->mode & ~(1 << opt[i].eic_line));
		// Set up edge type
		eic->edge = (opt[i].eic_edge == 1)
			? (eic->edge | (1 << opt[i].eic_line))
			: (eic->edge & ~(1 << opt[i].eic_line));
		// Set up level
		eic->level = (opt[i].eic_level == 1)
			? (eic->level | (1 << opt[i].eic_line))
			: (eic->level & ~(1 << opt[i].eic_line));
		// Set up if filter is used
		eic->filter = (opt[i].eic_filter == 1)
			? (eic->filter | (1 << opt[i].eic_line))
			: (eic->filter & ~(1 << opt[i].eic_line));
		// Set up which mode is used : asynchronous mode/ synchronous mode
		eic->async = (opt[i].eic_async == 1)
			? (eic->async | (1 << opt[i].eic_line))
			: (eic->async & ~(1 << opt[i].eic_line));
	}
}

void eic_enable_lines(volatile avr32_eic_t *eic, uint32_t mask_lines)
{
	eic->en = mask_lines;
}

void eic_enable_line(volatile avr32_eic_t *eic, uint32_t line_number)
{
	// Enable line line_number
	eic->en = 1 << line_number;
}

void eic_disable_lines(volatile avr32_eic_t *eic, uint32_t mask_lines)
{
	eic->dis = mask_lines;
}

void eic_disable_line(volatile avr32_eic_t *eic, uint32_t line_number)
{
	// Disable line line_number
	eic->dis = 1 << line_number;
}

bool eic_is_line_enabled(volatile avr32_eic_t *eic, uint32_t line_number)
{
	return (eic->ctrl & (1 << line_number)) != 0;
}

void eic_enable_interrupt_lines(volatile avr32_eic_t *eic, uint32_t mask_lines)
{
	eic->ier = mask_lines;
}

void eic_enable_interrupt_line(volatile avr32_eic_t *eic, uint32_t line_number)
{
	// Enable line line_number
	eic->ier = 1 << line_number;
}

void eic_disable_interrupt_lines(volatile avr32_eic_t *eic, uint32_t mask_lines)
{
	irqflags_t flags;
	flags = cpu_irq_save();
	eic->idr = mask_lines;
	eic->imr;
	cpu_irq_restore(flags);
}

void eic_disable_interrupt_line(volatile avr32_eic_t *eic, uint32_t line_number)
{
	irqflags_t flags;
	flags = cpu_irq_save();
	eic->idr = 1 << line_number;
	eic->imr;
	cpu_irq_restore(flags);
}

bool eic_is_interrupt_line_enabled(volatile avr32_eic_t *eic, uint32_t line_number)
{
	return (eic->imr & (1 << line_number)) != 0;
}

void eic_clear_interrupt_lines(volatile avr32_eic_t *eic, uint32_t mask_lines)
{
	irqflags_t flags;
	flags = cpu_irq_save();
	eic->icr = mask_lines;
	eic->isr;
	cpu_irq_restore(flags);
}

void eic_clear_interrupt_line(volatile avr32_eic_t *eic, uint32_t line_number)
{
	irqflags_t flags;
	flags = cpu_irq_save();
	eic->icr = 1 << line_number;
	eic->isr;
	cpu_irq_restore(flags);
}

bool eic_is_interrupt_line_pending(volatile avr32_eic_t *eic, uint32_t line_number)
{
	return (eic->isr & (1 << line_number)) != 0;
}

#if !defined(AVR32_EIC_301_H_INCLUDED) && !defined(AVR32_EIC_302_H_INCLUDED)
void eic_enable_interrupt_scan(volatile avr32_eic_t *eic, uint32_t presc)
{
	// Enable SCAN function with PRESC value
	eic->scan |= (presc << AVR32_EIC_SCAN_PRESC_OFFSET) | (1 << AVR32_EIC_SCAN_EN_OFFSET);
}

void eic_disable_interrupt_scan(volatile avr32_eic_t *eic)
{
	// Disable SCAN function
	eic->scan = 0 << AVR32_EIC_SCAN_EN_OFFSET;
}

uint32_t eic_get_interrupt_pad_scan(volatile avr32_eic_t *eic)
{
	// Return pad number that causes interrupt
	return(eic->scan>>AVR32_EIC_SCAN_PIN_OFFSET);
}
#endif
