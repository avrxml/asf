/**
 * \file
 *
 * \brief Analog comparator (AC) functions
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
#include <stdint.h>

#include "interrupt.h"
#include "compiler.h"
#include "parts.h"

#include "ac.h"
#include "sysclk.h"
#include "sleepmgr.h"
#include "status_codes.h"

#if defined(ACA) || defined(__DOXYGEN__)
//! \internal Local storage of analog comparator A interrupt callback function
static ac_callback_t    ac_aca_callback;

/**
 * \internal
 * \brief Local variable keeping track of number of users that has opened
 * (configuring, enabled) channels on analog comparator A.
 */
static uint8_t          ac_aca_opened = 0;

/**
 * \internal
 * \brief Interrupt handler for analog comparator A channel 0
 *
 * This function will handle interrupt on analog comparator A channel 0 and
 * call the analog comparator channel A callback function.
 */
ISR(ACA_AC0_vect)
{
	if (ac_aca_callback) {
		if (ACA.STATUS & AC_AC0STATE_bm) {
			ac_aca_callback(&ACA, 0, AC_STATUS_ABOVE);
		}
		else {
			ac_aca_callback(&ACA, 0, AC_STATUS_BELOW);
		}
	}
}

/**
 * \internal
 * \brief Interrupt handler for analog comparator A channel 1
 *
 * This function will handle interrupt on analog comparator A channel 1 and
 * call the analog comparator channel A callback function.
 */
ISR(ACA_AC1_vect)
{
	if (ac_aca_callback) {
		if (ACA.STATUS & AC_AC1STATE_bm) {
			ac_aca_callback(&ACA, 1, AC_STATUS_ABOVE);
		}
		else {
			ac_aca_callback(&ACA, 1, AC_STATUS_BELOW);
		}
	}
}

/**
 * \internal
 * \brief Interrupt handler for analog comparator A in window mode
 *
 * This function will handle interrupt on analog comparator A in window mode
 * and call the analog comparator channel A callback function.
 */
ISR(ACA_ACW_vect)
{
	if (ac_aca_callback) {
		ac_aca_callback(&ACA, 0, (enum ac_status_t)(ACA.STATUS &
					AC_WSTATE_gm));
	}
}
#endif

#if defined(ACB) || defined(__DOXYGEN__)
//! \internal Local storage of analog comparator B interrupt callback function
static ac_callback_t    ac_acb_callback;

/**
 * \internal
 * \brief Local variable keeping track of number of users that has opened
 * (configuring, enabled) channels on analog comparator A.
 */
static uint8_t          ac_acb_opened = 0;

/**
 * \internal
 * \brief Interrupt handler for analog comparator B channel 0
 *
 * This function will handle interrupt on analog comparator B channel 0 and
 * call the analog comparator channel B callback function.
 */
ISR(ACB_AC0_vect)
{
	if (ac_acb_callback) {
		if (ACB.STATUS & AC_AC0STATE_bm) {
			ac_acb_callback(&ACB, 0, AC_STATUS_ABOVE);
		}
		else {
			ac_acb_callback(&ACB, 0, AC_STATUS_BELOW);
		}
	}
}

/**
 * \internal
 * \brief Interrupt handler for analog comparator B channel 1
 *
 * This function will handle interrupt on analog comparator B channel 1 and
 * call the analog comparator channel B callback function.
 */
ISR(ACB_AC1_vect)
{
	if (ac_acb_callback) {
		if (ACB.STATUS & AC_AC1STATE_bm) {
			ac_acb_callback(&ACB, 1, AC_STATUS_ABOVE);
		}
		else {
			ac_acb_callback(&ACB, 1, AC_STATUS_BELOW);
		}
	}
}

/**
 * \internal
 * \brief Interrupt handler for analog comparator B in window mode
 *
 * This function will handle interrupt on analog comparator B in window mode
 * and call the analog comparator channel B callback function.
 */
ISR(ACB_ACW_vect)
{
	if (ac_acb_callback) {
		ac_acb_callback(&ACB, 0, (enum ac_status_t)(ACB.STATUS &
					AC_WSTATE_gm));
	}
}
#endif

/**
 * \brief Write analog comparator configuration to hardware
 *
 * This function will write a \ref ac_config struct to the selected analog
 * comparator hardware and channel.
 *
 * \param ac Pointer to the analog comparator (AC) base address
 * \param channel Number of analog comparator (AC) channel
 * \param config Pointer to a \ref ac_config variable
 */
void ac_write_config(AC_t *ac, uint8_t channel, struct ac_config *config)
{
	enum sysclk_port_id     sysclk_port;
	irqflags_t              iflags = cpu_irq_save();

#ifdef ACA
	if ((uintptr_t)ac == (uintptr_t)&ACA) {
		sysclk_port = SYSCLK_PORT_A;
		if (!ac_aca_opened) {
			sysclk_enable_module(sysclk_port, SYSCLK_AC);
		}
		ac_aca_opened++;
	} else
#endif
#ifdef ACB
	if ((uintptr_t)ac == (uintptr_t)&ACB) {
		sysclk_port = SYSCLK_PORT_B;
		if (!ac_acb_opened) {
			sysclk_enable_module(sysclk_port, SYSCLK_AC);
		}
		ac_acb_opened++;
	} else
#endif
	{
		cpu_irq_restore(iflags);
		return;
	}

	ac->CTRLB               = config->ctrlb;
	ac->WINCTRL             = config->winctrl;

	if (channel == 0) {
		ac->AC0MUXCTRL  = config->acmuxctrl;
		ac->AC0CTRL     = config->acctrl;
	} else {
		ac->AC1MUXCTRL  = config->acmuxctrl;
		ac->AC1CTRL     = config->acctrl;
	}

#ifdef ACA
	if (sysclk_port == SYSCLK_PORT_A) {
		ac_aca_opened--;
		if (!ac_aca_opened) {
			sysclk_disable_module(SYSCLK_PORT_A, SYSCLK_AC);
		}
	} else
#endif
#ifdef ACB
	if (sysclk_port == SYSCLK_PORT_B) {
		ac_acb_opened--;
		if (!ac_acb_opened) {
			sysclk_disable_module(SYSCLK_PORT_B, SYSCLK_AC);
		}
	} else
#endif
	{
		cpu_irq_restore(iflags);
		return;
	}

	cpu_irq_restore(iflags);
}

/**
 * \brief Read analog comparator configuration from hardware
 *
 * This function will read the hardware configuration and put it into the \a
 * config argument.
 *
 * \param ac Pointer to the analog comparator (AC) base address
 * \param channel Number of analog comparator (AC) channel
 * \param config Pointer to a \ref ac_config variable
 */
void ac_read_config(AC_t *ac, uint8_t channel, struct ac_config *config)
{
	irqflags_t iflags = cpu_irq_save();

	if (channel == 0) {
		config->acmuxctrl   = ac->AC0MUXCTRL;
		config->acctrl      = ac->AC0CTRL;
	} else {
		config->acmuxctrl   = ac->AC1MUXCTRL;
		config->acctrl      = ac->AC1CTRL;
	}

	config->winctrl = ac->WINCTRL;
	config->ctrlb   = ac->CTRLB;

	cpu_irq_restore(iflags);
}

/**
 * \brief Get analog compare channel status
 *
 * This function will get the current analog compare status from the given
 * channel.
 *
 * \param ac Pointer to the analog comparator (AC) base address
 * \param channel Number of analog comparator (AC) channel to get status from
 *
 * \retval Analog comparator status given by a \ref ac_status_t value
 */
enum ac_status_t ac_get_status(AC_t *ac, uint8_t channel)
{
	uint8_t winctrl = ac->WINCTRL;

	if (winctrl & AC_WEN_bm) {
		return (enum ac_status_t)(ac->STATUS & AC_WSTATE_gm);
	}
	else if (channel == 0) {
		if (ac->STATUS & AC_AC0STATE_bm) {
			return AC_STATUS_ABOVE;
		} else {
			return AC_STATUS_BELOW;
		}
	} else {
		if (ac->STATUS & AC_AC1STATE_bm) {
			return AC_STATUS_ABOVE;
		} else {
			return AC_STATUS_BELOW;
		}
	}
}

/**
 * \brief Enable an analog comparator channel
 *
 * \param ac Pointer to the analog comparator (AC) base address
 * \param channel Number of analog comparator (AC) channel to enable
 */
void ac_enable(AC_t *ac, uint8_t channel)
{
	irqflags_t              iflags = cpu_irq_save();

#ifdef ACA
	if ((uintptr_t)ac == (uintptr_t)&ACA) {
		if (!ac_aca_opened) {
			sysclk_enable_module(SYSCLK_PORT_A, SYSCLK_AC);
		}
		ac_aca_opened++;
	} else
#endif
#ifdef ACB
	if ((uintptr_t)ac == (uintptr_t)&ACB) {
		if (!ac_acb_opened) {
			sysclk_enable_module(SYSCLK_PORT_B, SYSCLK_AC);
		}
		ac_acb_opened++;
	} else
#endif
	{
		cpu_irq_restore(iflags);
		return;
	}

	sleepmgr_lock_mode(SLEEPMGR_IDLE);

	if (channel == 0) {
		ac->AC0CTRL |= AC_ENABLE_bm;
	} else {
		ac->AC1CTRL |= AC_ENABLE_bm;
	}

	cpu_irq_restore(iflags);
}

/**
 * \brief Disable an analog comparator channel
 *
 * \param ac Pointer to the analog comparator (AC) base address
 * \param channel Number of analog comparator (AC) channel to disable
 */
void ac_disable(AC_t *ac, uint8_t channel)
{
	irqflags_t              iflags = cpu_irq_save();

	if (channel == 0) {
		ac->AC0CTRL &= ~AC_ENABLE_bm;
	} else {
		ac->AC1CTRL &= ~AC_ENABLE_bm;
	}

	sleepmgr_unlock_mode(SLEEPMGR_IDLE);

#ifdef ACA
	if ((uintptr_t)ac == (uintptr_t)&ACA) {
		ac_aca_opened--;
		if (!ac_aca_opened) {
			sysclk_disable_module(SYSCLK_PORT_A, SYSCLK_AC);
		}
	} else
#endif
#ifdef ACB
	if ((uintptr_t)ac == (uintptr_t)&ACB) {
		ac_acb_opened--;
		if (!ac_acb_opened) {
			sysclk_disable_module(SYSCLK_PORT_B, SYSCLK_AC);
		}
	} else
#endif
	{
		cpu_irq_restore(iflags);
		return;
	}

	cpu_irq_restore(iflags);
}

/**
 * \brief Set interrupt callback function
 *
 * This function allows the caller to set and change the interrupt callback
 * function. Without setting a callback function the interrupt handler in the
 * driver will only clear the interrupt flags.
 *
 * \param ac Pointer to the analog comparator (AC) base address
 * \param callback Reference to a callback function
 */
void ac_set_interrupt_callback(AC_t *ac, ac_callback_t callback)
{
#ifdef ACA
	if (ac == &ACA) {
		ac_aca_callback = callback;
	}
#endif
#ifdef ACB
	if (ac == &ACB) {
		ac_acb_callback = callback;
	}
#endif
}
