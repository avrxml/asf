/**
 * \file
 *
 * \brief AVR XMEGA TC4/5 Driver
 *
 * Copyright (c) 2012-2015 Atmel Corporation. All rights reserved.
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

#include "tc45.h"

#if defined(TCC4) || defined(__DOXYGEN__)
/* ! \internal Local storage of Timer Counter tcc4 interrupt callback function */
static tc45_callback_t tc45_tcc4_ovf_callback;
static tc45_callback_t tc45_tcc4_err_callback;
static tc45_callback_t tc45_tcc4_cca_callback;
static tc45_callback_t tc45_tcc4_ccb_callback;
static tc45_callback_t tc45_tcc4_ccc_callback;
static tc45_callback_t tc45_tcc4_ccd_callback;

/**
 * \internal
 * \brief Interrupt handler for Timer Counter C4 overflow
 *
 * This function will handle interrupt on Timer Counter C4 overflow and
 * call the callback function.
 */
ISR(TCC4_OVF_vect)
{
	if (tc45_tcc4_ovf_callback) {
		tc45_tcc4_ovf_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter C4 error
 *
 * This function will handle interrupt on Timer Counter C4 error and
 * call the callback function.
 */
ISR(TCC4_ERR_vect)
{
	if (tc45_tcc4_err_callback) {
		tc45_tcc4_err_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter C4 Compare/CaptureA
 *
 * This function will handle interrupt on Timer Counter C4 Compare/CaptureA and
 * call the callback function.
 */
ISR(TCC4_CCA_vect)
{
	if (tc45_tcc4_cca_callback) {
		tc45_tcc4_cca_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter C4 Compare/CaptureB
 *
 * This function will handle interrupt on Timer Counter C4 Compare/CaptureB and
 * call the callback function.
 */
ISR(TCC4_CCB_vect)
{
	if (tc45_tcc4_ccb_callback) {
		tc45_tcc4_ccb_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter C4 Compare/CaptureC
 *
 * This function will handle interrupt on Timer Counter C4 Compare/CaptureC and
 * call the callback function.
 */
ISR(TCC4_CCC_vect)
{
	if (tc45_tcc4_ccc_callback) {
		tc45_tcc4_ccc_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter C4 Compare/CaptureD
 *
 * This function will handle interrupt on Timer Counter C4 Compare/CaptureD and
 * call the callback function.
 */
ISR(TCC4_CCD_vect)
{
	if (tc45_tcc4_ccd_callback) {
		tc45_tcc4_ccd_callback();
	}
}

#endif

#if defined(TCC5) || defined(__DOXYGEN__)
/* ! \internal Local storage of Timer Counter tcc5 interrupt callback function */
static tc45_callback_t tc45_tcc5_ovf_callback;
static tc45_callback_t tc45_tcc5_err_callback;
static tc45_callback_t tc45_tcc5_cca_callback;
static tc45_callback_t tc45_tcc5_ccb_callback;

/**
 * \internal
 * \brief Interrupt handler for Timer Counter C5 overflow
 *
 * This function will handle interrupt on Timer Counter C5 overflow and
 * call the callback function.
 */
ISR(TCC5_OVF_vect)
{
	if (tc45_tcc5_ovf_callback) {
		tc45_tcc5_ovf_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter C5 error
 *
 * This function will handle interrupt on Timer Counter C5 error and
 * call the callback function.
 */
ISR(TCC5_ERR_vect)
{
	if (tc45_tcc5_err_callback) {
		tc45_tcc5_err_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter C5 Compare/CaptureA
 *
 * This function will handle interrupt on Timer Counter C5 Compare/CaptureA and
 * call the callback function.
 */
ISR(TCC5_CCA_vect)
{
	if (tc45_tcc5_cca_callback) {
		tc45_tcc5_cca_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter C5 Compare/CaptureB
 *
 * This function will handle interrupt on Timer Counter C5 Compare/CaptureB and
 * call the callback function.
 */
ISR(TCC5_CCB_vect)
{
	if (tc45_tcc5_ccb_callback) {
		tc45_tcc5_ccb_callback();
	}
}

#endif

#if defined(TCD5) || defined(__DOXYGEN__)
/* ! \internal Local storage of Timer Counter tcd5 interrupt callback function */
static tc45_callback_t tc45_tcd5_ovf_callback;
static tc45_callback_t tc45_tcd5_err_callback;
static tc45_callback_t tc45_tcd5_cca_callback;
static tc45_callback_t tc45_tcd5_ccb_callback;

/**
 * \internal
 * \brief Interrupt handler for Timer Counter D5 overflow
 *
 * This function will handle interrupt on Timer Counter D5 overflow and
 * call the callback function.
 */
ISR(TCD5_OVF_vect)
{
	if (tc45_tcd5_ovf_callback) {
		tc45_tcd5_ovf_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter D5 error
 *
 * This function will handle interrupt on Timer Counter D5 error and
 * call the callback function.
 */
ISR(TCD5_ERR_vect)
{
	if (tc45_tcd5_err_callback) {
		tc45_tcd5_err_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter D5 Compare/CaptureA
 *
 * This function will handle interrupt on Timer Counter D5 Compare/CaptureA and
 * call the callback function.
 */
ISR(TCD5_CCA_vect)
{
	if (tc45_tcd5_cca_callback) {
		tc45_tcd5_cca_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter D5 Compare/CaptureB
 *
 * This function will handle interrupt on Timer Counter D5 Compare/CaptureB and
 * call the callback function.
 */
ISR(TCD5_CCB_vect)
{
	if (tc45_tcd5_ccb_callback) {
		tc45_tcd5_ccb_callback();
	}
}
#endif

/**
 * \brief Enable TC45
 *
 * Enables the TC45.
 *
 * \param tc Pointer to TC45 module
 *
 * \note
 * unmask TC45 clock (sysclk), but does not configure the TC45 clock source.
 */
void tc45_enable(volatile void *tc)
{
	irqflags_t iflags = cpu_irq_save();

#ifdef TCC4
	if ((uintptr_t)tc == (uintptr_t)&TCC4) {
		sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_TC4);
		sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_HIRES);
	} else
#endif
#ifdef TCC5
	if ((uintptr_t)tc == (uintptr_t)&TCC5) {
		sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_TC5);
		sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_HIRES);
	} else
#endif
#ifdef TCD5
	if ((uintptr_t)tc == (uintptr_t)&TCD5) {
		sysclk_enable_module(SYSCLK_PORT_D, SYSCLK_TC5);
		sysclk_enable_module(SYSCLK_PORT_D, SYSCLK_HIRES);
	} else
#endif

	{
		cpu_irq_restore(iflags);
		return;
	}
	sleepmgr_lock_mode(SLEEPMGR_IDLE);
	cpu_irq_restore(iflags);
}

/**
 * \brief Disable TC45
 *
 * Disables the TC45.
 *
 * \param tc Pointer to TC45 module
 *
 * \note
 * mask TC45 clock (sysclk).
 */
void tc45_disable(volatile void *tc)
{
	irqflags_t iflags = cpu_irq_save();

	sleepmgr_unlock_mode(SLEEPMGR_IDLE);

#ifdef TCC4
	if ((uintptr_t)tc == (uintptr_t)&TCC4) {
		sysclk_disable_module(SYSCLK_PORT_C, SYSCLK_TC4);
		sysclk_disable_module(SYSCLK_PORT_C, SYSCLK_HIRES);
	} else
#endif
#ifdef TCC5
	if ((uintptr_t)tc == (uintptr_t)&TCC5) {
		sysclk_disable_module(SYSCLK_PORT_C, SYSCLK_TC5);
		sysclk_disable_module(SYSCLK_PORT_C, SYSCLK_HIRES);
	} else
#endif
#ifdef TCD5
	if ((uintptr_t)tc == (uintptr_t)&TCD5) {
		sysclk_disable_module(SYSCLK_PORT_D, SYSCLK_TC4);
		sysclk_disable_module(SYSCLK_PORT_D, SYSCLK_HIRES);
	} else
#endif
	{
		cpu_irq_restore(iflags);
		return;
	}
	cpu_irq_restore(iflags);
}
/**
 * \brief Calls the interrupt function if Overflow 
 *
 * Timer Counter tc45 overflow interrupt callback function
 *
 * \param tc Pointer to TC45 module
 * \param callback  interrupt function which will be called 
 * 
 * \note
 * unmask TC45 clock (sysclk), but does not configure the TC45 clock source.
 */
void tc45_set_overflow_interrupt_callback(volatile void *tc,
		tc45_callback_t callback)
{
#ifdef TCC4
	if ((uintptr_t)tc == (uintptr_t)&TCC4) {
		tc45_tcc4_ovf_callback = callback;
	} else
#endif
#ifdef TCC5
	if ((uintptr_t)tc == (uintptr_t)&TCC5) {
		tc45_tcc5_ovf_callback = callback;
	} else
#endif
#ifdef TCD5
	if ((uintptr_t)tc == (uintptr_t)&TCD5) {
		tc45_tcd5_ovf_callback = callback;
	} else
#endif
	{}
}
/**
 * \brief Calls the interrupt function if Error
 *
 * Timer Counter tc45 Error interrupt callback function
 *
 * \param tc Pointer to TC45 module
 * \param callback  interrupt function which will be called 
 * 
 * \note
 * unmask TC45 clock (sysclk), but does not configure the TC45 clock source.
 */
void tc45_set_error_interrupt_callback(volatile void *tc,
		tc45_callback_t callback)
{
#ifdef TCC4
	if ((uintptr_t)tc == (uintptr_t)&TCC4) {
		tc45_tcc4_err_callback = callback;
	} else
#endif
#ifdef TCC5
	if ((uintptr_t)tc == (uintptr_t)&TCC5) {
		tc45_tcc5_err_callback = callback;
	} else
#endif
#ifdef TCD5
	if ((uintptr_t)tc == (uintptr_t)&TCD5) {
		tc45_tcd5_err_callback = callback;
	} else
#endif
	{}
}
/**
 * \brief Calls the interrupt function if Compare/Capture A 
 *
 * Timer Counter tc45 Compare/Capture A interrupt callback function
 *
 * \param tc Pointer to TC45 module
 * \param callback  interrupt function which will be called 
 * 
 * \note
 * unmask TC45 clock (sysclk), but does not configure the TC45 clock source.
 */
void tc45_set_cca_interrupt_callback(volatile void *tc,
		tc45_callback_t callback)
{
#ifdef TCC4
	if ((uintptr_t)tc == (uintptr_t)&TCC4) {
		tc45_tcc4_cca_callback = callback;
	} else
#endif
#ifdef TCC5
	if ((uintptr_t)tc == (uintptr_t)&TCC5) {
		tc45_tcc5_cca_callback = callback;
	} else
#endif
#ifdef TCD5
	if ((uintptr_t)tc == (uintptr_t)&TCD5) {
		tc45_tcd5_cca_callback = callback;
	} else
#endif
	{}
}
/**
 * \brief Calls the interrupt function if Compare/Capture B 
 *
 * Timer Counter tc45 Compare/Capture B interrupt callback function
 *
 * \param tc Pointer to TC45 module
 * \param callback  interrupt function which will be called 
 * 
 * \note
 * unmask TC45 clock (sysclk), but does not configure the TC45 clock source.
 */
void tc45_set_ccb_interrupt_callback(volatile void *tc,
		tc45_callback_t callback)
{
#ifdef TCC4
	if ((uintptr_t)tc == (uintptr_t)&TCC4) {
		tc45_tcc4_ccb_callback = callback;
	} else
#endif
#ifdef TCC5
	if ((uintptr_t)tc == (uintptr_t)&TCC5) {
		tc45_tcc5_ccb_callback = callback;
	} else
#endif
#ifdef TCD5
	if ((uintptr_t)tc == (uintptr_t)&TCD5) {
		tc45_tcd5_ccb_callback = callback;
	} else
#endif
	{}
}
/**
 * \brief Calls the interrupt function if Compare/Capture C 
 *
 * Timer Counter tc45 Compare/Capture C interrupt callback function
 *
 * \param tc Pointer to TC45 module
 * \param callback  interrupt function which will be called 
 * 
 * \note
 * unmask TC45 clock (sysclk), but does not configure the TC45 clock source.
 */
void tc45_set_ccc_interrupt_callback(volatile void *tc,
		tc45_callback_t callback)
{
#ifdef TCC4
	if ((uintptr_t)tc == (uintptr_t)&TCC4) {
		tc45_tcc4_ccc_callback = callback;
	} else
#endif
	{}
}
/**
 * \brief Calls the interrupt function if Compare/Capture D 
 *
 * Timer Counter tc45 Compare/Capture D interrupt callback function
 *
 * \param tc Pointer to TC45 module
 * \param callback  interrupt function which will be called 
 * 
 * \note
 * unmask TC45 clock (sysclk), but does not configure the TC45 clock source.
 */
void tc45_set_ccd_interrupt_callback(volatile void *tc,
		tc45_callback_t callback)
{
#ifdef TCC4
	if ((uintptr_t)tc == (uintptr_t)&TCC4) {
		tc45_tcc4_ccd_callback = callback;
	} else
#endif
	{}
}
