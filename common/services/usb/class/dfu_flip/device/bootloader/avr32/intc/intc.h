/**
 * \file
 *
 * \brief Optimized INTC driver for bootloader on AVR UC3.
 *
 * Copyright (c) 2011 Atmel Corporation. All rights reserved.
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

#ifndef _INTC_H_
#define _INTC_H_

#include "compiler.h"


//! Maximal number of interrupt request lines per group.
#define AVR32_INTC_MAX_NUM_IRQS_PER_GRP             32

//! Number of interrupt priority levels.
#define AVR32_INTC_NUM_INT_LEVELS   (1 << AVR32_INTC_IPR_INTLEVEL_SIZE)


#ifdef __AVR32_ABI_COMPILER__   // Automatically defined when compiling for AVR32, not when assembling.

/*! \brief Registers an interrupt handler.
 *
 * \param handler   Interrupt handler to register.
 * \param irq       IRQ of the interrupt handler to register.
 * \param int_level Interrupt priority level to assign to the group of this IRQ.
 *
 * \warning The interrupt handler must manage the `rete' instruction, what can
 *          be done thanks to pure assembly, inline assembly or the
 *          `__attribute__((__interrupt__))' C function attribute.
 *
 * \warning If several interrupt handlers of a same group are registered, all
 *          interrupts of this group will fetch the last interrupt handler
 *          registered for this group with the last registered interrupt
 *          priority level.
 *
 * \note Taken and adapted from Newlib.
 */
extern void INTC_register_interrupt(__int_handler handler, unsigned int irq,
		unsigned int int_level);

#endif // __AVR32_ABI_COMPILER__


#endif // _INTC_H_
