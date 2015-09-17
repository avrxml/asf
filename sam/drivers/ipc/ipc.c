/**
 * \file
 *
 * \brief Interprocessor Communication (IPC) driver for SAM.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "ipc.h"
#include <status_codes.h>
#include <sysclk.h>

#define IPC_MAX_INTERRUPT_SOURCES  32
#define NB_IPC  2

/**
 * Describes an IPC interrupt handler, including the IPC interrupt source and
 * the associated handler.
 */
struct ipc_interrupt_handler {
	enum ipc_interrupt_source mask;
	void (*handler) (Ipc *, enum ipc_interrupt_source);
};

static struct ipc_interrupt_handler ipc_handler[NB_IPC][IPC_MAX_INTERRUPT_SOURCES];

/**
 * \brief Process an interrupt request on the given IPC.
 *
 * \param p_ipc Pointer to an IPC instance.
 */
static void ipc_handler_process(Ipc *p_ipc)
{
	uint32_t status, i, ipc;
	(p_ipc == IPC0) ? (ipc = 0) : (ipc = 1);

	/* Read IPC interrupt status */
	status = ipc_get_status(p_ipc);
	status &= ipc_get_interrupt_mask(p_ipc);

	/* Check pending events */
	if (status != 0) {
		/* Find triggering source */
		i = 0;
		while ((status != 0) && (i < IPC_MAX_INTERRUPT_SOURCES)) {
			if ((status & ipc_handler[ipc][i].mask) != 0) {
				ipc_handler[ipc][i].handler(p_ipc, ipc_handler[ipc][i].mask);
				status &= ~(ipc_handler[ipc][i].mask);
			}
			i++;
		}
	}
}

/**
 * \brief Set an interrupt handler for the specified interrput source.
 *
 * \param p_ipc Pointer to an IPC instance.
 * \param source Interrupt source enumeration.
 * \param p_handler Interrupt handler function pointer.
 */
void ipc_set_handler(Ipc *p_ipc, enum ipc_interrupt_source source,
		void (*p_handler) (Ipc *, enum ipc_interrupt_source))
{
	struct ipc_interrupt_handler *pSource;
	uint32_t i = 0;

	while (!(source & (1 << i))) {
		i++;
	}
	/* Define new source */
	if (p_ipc == IPC0) {
		pSource = &(ipc_handler[0][i]);
	} else {
		pSource = &(ipc_handler[1][i]);
	}

	pSource->mask = source;
	pSource->handler = p_handler;
}

/**
 * \brief Enable the IPC peripheral.
 *
 * \param p_ipc Pointer to an IPC instance.
 */
void ipc_enable(Ipc *p_ipc)
{
	if (p_ipc == IPC0) {
		sysclk_enable_peripheral_clock(ID_IPC0);
	} else {
		sysclk_enable_peripheral_clock(ID_IPC1);
	}
}

/**
 * \brief Disable the IPC peripheral.
 *
 * \param p_ipc Pointer to an IPC instance.
 */
void ipc_disable(Ipc *p_ipc)
{
	if (p_ipc == IPC0) {
		sysclk_disable_peripheral_clock(ID_IPC0);
	} else {
		sysclk_disable_peripheral_clock(ID_IPC1);
	}
}

/**
 * \brief IPC0 interrupt handler.
 * Redefined IPC0 interrupt handler for NVIC interrupt table.
 */
void IPC0_Handler(void)
{
	ipc_handler_process(IPC0);
}

/**
 * \brief IPC1 interrupt handler.
 * Redefined IPC1 interrupt handler for NVIC interrupt table.
 */
void IPC1_Handler(void)
{
	ipc_handler_process(IPC1);
}
