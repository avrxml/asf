/*****************************************************************************
 *
 * \file
 *
 * \brief PDCA driver for AVR32 UC3.
 *
 * This file defines a useful set of functions for the PDCA interface on AVR32
 * devices.
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
