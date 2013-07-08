/*****************************************************************************
 *
 * \file
 *
 * \brief PDCA driver for AVR32 UC3.
 *
 * This file defines a useful set of functions for the PDCA interface on AVR32
 * devices.
 *
 * Copyright (c) 2009-2012 Atmel Corporation. All rights reserved.
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
 ******************************************************************************/

#include "compiler.h"
#include "pdca.h"

volatile avr32_pdca_channel_t *pdca_get_handler(uint8_t pdca_ch_number)
{
	/* get the correct channel pointer */
	volatile avr32_pdca_channel_t *pdca_channel
		= &AVR32_PDCA.channel[pdca_ch_number];

	if (pdca_ch_number >= AVR32_PDCA_CHANNEL_LENGTH) {
		return (volatile avr32_pdca_channel_t *)PDCA_INVALID_ARGUMENT;
	}

	return pdca_channel;
}

uint32_t pdca_init_channel(uint8_t pdca_ch_number,
		const pdca_channel_options_t *opt)
{
	/* get the correct channel pointer */
	volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(
			pdca_ch_number);

	pdca_disable_interrupt_transfer_complete(pdca_ch_number); 
	pdca_disable_interrupt_reload_counter_zero(pdca_ch_number);
	
	irqflags_t flags = cpu_irq_save();

	pdca_channel->mar = (uint32_t)opt->addr;
	pdca_channel->tcr = opt->size;
	pdca_channel->psr = opt->pid;
	pdca_channel->marr = (uint32_t)opt->r_addr;
	pdca_channel->tcrr = opt->r_size;
	pdca_channel->mr =
#if (AVR32_PDCA_H_VERSION >= 120)
			opt->etrig << AVR32_PDCA_ETRIG_OFFSET |
#endif
			opt->transfer_size << AVR32_PDCA_SIZE_OFFSET;
	pdca_channel->cr = AVR32_PDCA_ECLR_MASK;
	pdca_channel->isr;
	
	cpu_irq_restore(flags);

	return PDCA_SUCCESS;
}

bool pdca_get_channel_status(uint8_t pdca_ch_number)
{
	/* get the correct channel pointer */
	volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(
			pdca_ch_number);

	return (pdca_channel->sr & AVR32_PDCA_TEN_MASK) != 0;
}

void pdca_disable(uint8_t pdca_ch_number)
{
	/* get the correct channel pointer */
	volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(
			pdca_ch_number);

	/* Disable transfer */
	pdca_channel->cr = AVR32_PDCA_TDIS_MASK;
}

void pdca_enable(uint8_t pdca_ch_number)
{
	/* get the correct channel pointer */
	volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(
			pdca_ch_number);

	/* Enable transfer */
	pdca_channel->cr = AVR32_PDCA_TEN_MASK;
}

uint32_t pdca_get_load_size(uint8_t pdca_ch_number)
{
	/* get the correct channel pointer */
	volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(
			pdca_ch_number);

	return pdca_channel->tcr;
}

void pdca_load_channel(uint8_t pdca_ch_number, volatile void *addr,
		uint32_t size)
{
	/* get the correct channel pointer */
	volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(
			pdca_ch_number);

	irqflags_t flags = cpu_irq_save();

	pdca_channel->mar = (uint32_t)addr;
	pdca_channel->tcr = size;
	pdca_channel->cr = AVR32_PDCA_ECLR_MASK;
	pdca_channel->isr;

	cpu_irq_restore(flags);
}

uint32_t pdca_get_reload_size(uint8_t pdca_ch_number)
{
	/* get the correct channel pointer */
	volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(
			pdca_ch_number);

	return pdca_channel->tcrr;
}

void pdca_reload_channel(uint8_t pdca_ch_number, volatile void *addr,
		uint32_t size)
{
	/* get the correct channel pointer */
	volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(
			pdca_ch_number);

	irqflags_t flags = cpu_irq_save();

	/* set up next memory address */
	pdca_channel->marr = (uint32_t)addr;
	/* set up next memory size */
	pdca_channel->tcrr = size;
	pdca_channel->cr = AVR32_PDCA_ECLR_MASK;
	pdca_channel->isr;

	cpu_irq_restore(flags);
}

void pdca_set_peripheral_select(uint8_t pdca_ch_number, uint32_t pid)
{
	/* get the correct channel pointer */
	volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(
			pdca_ch_number);

	pdca_channel->psr = pid;
}

void pdca_set_transfer_size(uint8_t pdca_ch_number,
		uint32_t transfer_size)
{
	/* get the correct channel pointer */
	volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(
			pdca_ch_number);

	pdca_channel->mr = (pdca_channel->mr & ~AVR32_PDCA_SIZE_MASK) |
			transfer_size << AVR32_PDCA_SIZE_OFFSET;
}

#if (AVR32_PDCA_H_VERSION >= 120)

void pdca_disable_event_trigger(uint8_t pdca_ch_number)
{
	/* get the correct channel pointer */
	volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(
			pdca_ch_number);

	pdca_channel->mr &= ~AVR32_PDCA_ETRIG_MASK;
}

void pdca_enable_event_trigger(uint8_t pdca_ch_number)
{
	/* get the correct channel pointer */
	volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(
			pdca_ch_number);

	pdca_channel->mr |= AVR32_PDCA_ETRIG_MASK;
}

#endif

void pdca_disable_interrupt_transfer_error(uint8_t pdca_ch_number)
{
	/* get the correct channel pointer */
	volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(
			pdca_ch_number);

	irqflags_t flags = cpu_irq_save();

	pdca_channel->idr = AVR32_PDCA_TERR_MASK;
	pdca_channel->isr;

	cpu_irq_restore(flags);
}

void pdca_enable_interrupt_transfer_error(uint8_t pdca_ch_number)
{
	/* get the correct channel pointer */
	volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(
			pdca_ch_number);

	pdca_channel->ier = AVR32_PDCA_TERR_MASK;
}

void pdca_disable_interrupt_transfer_complete(uint8_t pdca_ch_number)
{
	/* get the correct channel pointer */
	volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(
			pdca_ch_number);

	irqflags_t flags = cpu_irq_save();

	pdca_channel->idr = AVR32_PDCA_TRC_MASK;
	pdca_channel->isr;

	cpu_irq_restore(flags);
}

void pdca_enable_interrupt_transfer_complete(uint8_t pdca_ch_number)
{
	/* get the correct channel pointer */
	volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(
			pdca_ch_number);

	pdca_channel->ier = AVR32_PDCA_TRC_MASK;
}

void pdca_disable_interrupt_reload_counter_zero(uint8_t pdca_ch_number)
{
	/* get the correct channel pointer */
	volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(
			pdca_ch_number);

	irqflags_t flags = cpu_irq_save();

	pdca_channel->idr = AVR32_PDCA_RCZ_MASK;
	pdca_channel->isr;

	cpu_irq_restore(flags);
}

void pdca_enable_interrupt_reload_counter_zero(uint8_t pdca_ch_number)
{
	/* get the correct channel pointer */
	volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(
			pdca_ch_number);

	pdca_channel->ier = AVR32_PDCA_RCZ_MASK;
}

uint32_t pdca_get_transfer_status(uint8_t pdca_ch_number)
{
	/* get the correct channel pointer */
	volatile avr32_pdca_channel_t *pdca_channel = pdca_get_handler(
			pdca_ch_number);

	return pdca_channel->isr;
}
