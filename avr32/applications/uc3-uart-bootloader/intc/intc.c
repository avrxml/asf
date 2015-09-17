/*****************************************************************************
 *
 * \file
 *
 * \brief INTC driver for AVR UC3.
 *
 * AVR32 Interrupt Controller driver module.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#include <avr32/io.h>
#include "compiler.h"
#include "intc.h"


//! Exception Vector Base Address.
extern void _evba;


void INTC_register_interrupt(__int_handler handler, unsigned int irq, unsigned int int_level)
{
  // Program the IPRX register of the IRQ group to set the interrupt priority
  // level and the interrupt vector offset that will be fetched by the core
  // interrupt system.
  AVR32_INTC.ipr[irq / AVR32_INTC_MAX_NUM_IRQS_PER_GRP] =
    ((((U32)handler - (U32)&_evba) << AVR32_INTC_IPR_AUTOVECTOR_OFFSET) & AVR32_INTC_IPR_AUTOVECTOR_MASK) |
    ((int_level << AVR32_INTC_IPR_INTLEVEL_OFFSET) & AVR32_INTC_IPR_INTLEVEL_MASK);
}


/*! \brief Default interrupt handler.
 *
 * \note Taken and adapted from Newlib.
 */
#if (defined __GNUC__)
__attribute__((__interrupt__))
#elif (defined __ICCAVR32__)
__interrupt
#endif
void _unhandled_interrupt(void);
#if (defined __GNUC__)
__attribute__((__interrupt__))
#elif (defined __ICCAVR32__)
__interrupt
#endif
void _unhandled_interrupt(void)
{
  // Catch unregistered interrupts.
  while (true);
}
