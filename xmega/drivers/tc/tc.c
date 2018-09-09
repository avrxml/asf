/**
 * \file
 *
 * \brief AVR XMEGA TC Driver
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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include <stdint.h>

#include "interrupt.h"
#include "compiler.h"
#include "parts.h"

#include "tc.h"
#include "sysclk.h"
#include "sleepmgr.h"
#include "status_codes.h"

#if defined(TCC0) || defined(__DOXYGEN__)
//! \internal Local storage of Timer Counter TCC0 interrupt callback function
static tc_callback_t tc_tcc0_ovf_callback;
static tc_callback_t tc_tcc0_err_callback;
static tc_callback_t tc_tcc0_cca_callback;
static tc_callback_t tc_tcc0_ccb_callback;
static tc_callback_t tc_tcc0_ccc_callback;
static tc_callback_t tc_tcc0_ccd_callback;


/**
 * \internal
 * \brief Interrupt handler for Timer Counter C0 overflow
 *
 * This function will handle interrupt on Timer Counter CO overflow and
 * call the callback function.
 */
ISR(TCC0_OVF_vect)
{
	if (tc_tcc0_ovf_callback) {
		tc_tcc0_ovf_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter C0 error
 *
 * This function will handle interrupt on Timer Counter CO error and
 * call the callback function.
 */
ISR(TCC0_ERR_vect)
{
	if (tc_tcc0_err_callback) {
		tc_tcc0_err_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter C0 Compare/CaptureA
 *
 * This function will handle interrupt on Timer Counter CO Compare/CaptureA and
 * call the callback function.
 */
ISR(TCC0_CCA_vect)
{
	if (tc_tcc0_cca_callback) {
		tc_tcc0_cca_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter C0 Compare/CaptureB
 *
 * This function will handle interrupt on Timer Counter CO Compare/CaptureB and
 * call the callback function.
 */
ISR(TCC0_CCB_vect)
{
	if (tc_tcc0_ccb_callback) {
		tc_tcc0_ccb_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter C0 Compare/CaptureC
 *
 * This function will handle interrupt on Timer Counter CO Compare/CaptureC and
 * call the callback function.
 */
ISR(TCC0_CCC_vect)
{
	if (tc_tcc0_ccc_callback) {
		tc_tcc0_ccc_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter C0 Compare/CaptureD
 *
 * This function will handle interrupt on Timer Counter CO Compare/CaptureD and
 * call the callback function.
 */
ISR(TCC0_CCD_vect)
{
	if (tc_tcc0_ccd_callback) {
		tc_tcc0_ccd_callback();
	}
}

#endif

#if defined(TCC1) || defined(__DOXYGEN__)
//! \internal Local storage of Timer Counter TCC1 interrupt callback function
static tc_callback_t tc_tcc1_ovf_callback;
static tc_callback_t tc_tcc1_err_callback;
static tc_callback_t tc_tcc1_cca_callback;
static tc_callback_t tc_tcc1_ccb_callback;

/**
 * \internal
 * \brief Interrupt handler for Timer Counter C1 overflow
 *
 * This function will handle interrupt on Timer Counter C1 overflow and
 * call the callback function.
 */
ISR(TCC1_OVF_vect)
{
	if (tc_tcc1_ovf_callback) {
		tc_tcc1_ovf_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter C1 error
 *
 * This function will handle interrupt on Timer Counter C1 error and
 * call the callback function.
 */
ISR(TCC1_ERR_vect)
{
	if (tc_tcc1_err_callback) {
		tc_tcc1_err_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter C1 Compare/CaptureA
 *
 * This function will handle interrupt on Timer Counter C1 Compare/CaptureA and
 * call the callback function.
 */
ISR(TCC1_CCA_vect)
{
	if (tc_tcc1_cca_callback) {
		tc_tcc1_cca_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter C1 Compare/CaptureB
 *
 * This function will handle interrupt on Timer Counter C1 Compare/CaptureB and
 * call the callback function.
 */
ISR(TCC1_CCB_vect)
{
	if (tc_tcc1_ccb_callback) {
		tc_tcc1_ccb_callback();
	}
}

#endif

#if defined(TCD0) || defined(__DOXYGEN__)
//! \internal Local storage of Timer Counter TCD0 interrupt callback function
static tc_callback_t tc_tcd0_ovf_callback;
static tc_callback_t tc_tcd0_err_callback;
static tc_callback_t tc_tcd0_cca_callback;
static tc_callback_t tc_tcd0_ccb_callback;
static tc_callback_t tc_tcd0_ccc_callback;
static tc_callback_t tc_tcd0_ccd_callback;


/**
 * \internal
 * \brief Interrupt handler for Timer Counter D0 overflow
 *
 * This function will handle interrupt on Timer Counter D0 overflow and
 * call the callback function.
 */
ISR(TCD0_OVF_vect)
{
	if (tc_tcd0_ovf_callback) {
		tc_tcd0_ovf_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter D0 error
 *
 * This function will handle interrupt on Timer Counter D0 error and
 * call the callback function.
 */
ISR(TCD0_ERR_vect)
{
	if (tc_tcd0_err_callback) {
		tc_tcd0_err_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter D0 Compare/CaptureA
 *
 * This function will handle interrupt on Timer Counter D0 Compare/CaptureA and
 * call the callback function.
 */
ISR(TCD0_CCA_vect)
{
	if (tc_tcd0_cca_callback) {
		tc_tcd0_cca_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter D0 Compare/CaptureB
 *
 * This function will handle interrupt on Timer Counter D0 Compare/CaptureB and
 * call the callback function.
 */
ISR(TCD0_CCB_vect)
{
	if (tc_tcd0_ccb_callback) {
		tc_tcd0_ccb_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter D0 Compare/CaptureC
 *
 * This function will handle interrupt on Timer Counter D0 Compare/CaptureC and
 * call the callback function.
 */
ISR(TCD0_CCC_vect)
{
	if (tc_tcd0_ccc_callback) {
		tc_tcd0_ccc_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter D0 Compare/CaptureD
 *
 * This function will handle interrupt on Timer Counter D0 Compare/CaptureD and
 * call the callback function.
 */
ISR(TCD0_CCD_vect)
{
	if (tc_tcd0_ccd_callback) {
		tc_tcd0_ccd_callback();
	}
}

#endif

#if defined(TCD1) || defined(__DOXYGEN__)
//! \internal Local storage of Timer Counter TCD1 interrupt callback function
static tc_callback_t tc_tcd1_ovf_callback;
static tc_callback_t tc_tcd1_err_callback;
static tc_callback_t tc_tcd1_cca_callback;
static tc_callback_t tc_tcd1_ccb_callback;

/**
 * \internal
 * \brief Interrupt handler for Timer Counter D1 overflow
 *
 * This function will handle interrupt on Timer Counter D1 overflow and
 * call the callback function.
 */
ISR(TCD1_OVF_vect)
{
	if (tc_tcd1_ovf_callback) {
		tc_tcd1_ovf_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter D1 error
 *
 * This function will handle interrupt on Timer Counter D1 error and
 * call the callback function.
 */
ISR(TCD1_ERR_vect)
{
	if (tc_tcd1_err_callback) {
		tc_tcd1_err_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter D1 Compare/CaptureA
 *
 * This function will handle interrupt on Timer Counter D1 Compare/CaptureA and
 * call the callback function.
 */
ISR(TCD1_CCA_vect)
{
	if (tc_tcd1_cca_callback) {
		tc_tcd1_cca_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter D1 Compare/CaptureB
 *
 * This function will handle interrupt on Timer Counter D1 Compare/CaptureB and
 * call the callback function.
 */
ISR(TCD1_CCB_vect)
{
	if (tc_tcd1_ccb_callback) {
		tc_tcd1_ccb_callback();
	}
}

#endif


#if defined(TCE0) || defined(__DOXYGEN__)
//! \internal Local storage of Timer Counter TCE0 interrupt callback function
static tc_callback_t tc_tce0_ovf_callback;
static tc_callback_t tc_tce0_err_callback;
static tc_callback_t tc_tce0_cca_callback;
static tc_callback_t tc_tce0_ccb_callback;
static tc_callback_t tc_tce0_ccc_callback;
static tc_callback_t tc_tce0_ccd_callback;


/**
 * \internal
 * \brief Interrupt handler for Timer Counter E0 overflow
 *
 * This function will handle interrupt on Timer Counter E0 overflow and
 * call the callback function.
 */
ISR(TCE0_OVF_vect)
{
	if (tc_tce0_ovf_callback) {
		tc_tce0_ovf_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter E0 error
 *
 * This function will handle interrupt on Timer Counter E0 error and
 * call the callback function.
 */
ISR(TCE0_ERR_vect)
{
	if (tc_tce0_err_callback) {
		tc_tce0_err_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter E0 Compare/CaptureA
 *
 * This function will handle interrupt on Timer Counter E0 Compare/CaptureA and
 * call the callback function.
 */
ISR(TCE0_CCA_vect)
{
	if (tc_tce0_cca_callback) {
		tc_tce0_cca_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter E0 Compare/CaptureB
 *
 * This function will handle interrupt on Timer Counter E0 Compare/CaptureB and
 * call the callback function.
 */
ISR(TCE0_CCB_vect)
{
	if (tc_tce0_ccb_callback) {
		tc_tce0_ccb_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter E0 Compare/CaptureC
 *
 * This function will handle interrupt on Timer Counter E0 Compare/CaptureC and
 * call the callback function.
 */
ISR(TCE0_CCC_vect)
{
	if (tc_tce0_ccc_callback) {
		tc_tce0_ccc_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter E0 Compare/CaptureD
 *
 * This function will handle interrupt on Timer Counter E0 Compare/CaptureD and
 * call the callback function.
 */
ISR(TCE0_CCD_vect)
{
	if (tc_tce0_ccd_callback) {
		tc_tce0_ccd_callback();
	}
}

#endif

#if defined(TCE1) || defined(__DOXYGEN__)
//! \internal Local storage of Timer Counter TCE1 interrupt callback function
static tc_callback_t tc_tce1_ovf_callback;
static tc_callback_t tc_tce1_err_callback;
static tc_callback_t tc_tce1_cca_callback;
static tc_callback_t tc_tce1_ccb_callback;

/**
 * \internal
 * \brief Interrupt handler for Timer Counter E1 overflow
 *
 * This function will handle interrupt on Timer Counter E1 overflow and
 * call the callback function.
 */
ISR(TCE1_OVF_vect)
{
	if (tc_tce1_ovf_callback) {
		tc_tce1_ovf_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter E1 error
 *
 * This function will handle interrupt on Timer Counter E1 error and
 * call the callback function.
 */
ISR(TCE1_ERR_vect)
{
	if (tc_tce1_err_callback) {
		tc_tce1_err_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter E1 Compare/CaptureA
 *
 * This function will handle interrupt on Timer Counter E1 Compare/CaptureA and
 * call the callback function.
 */
ISR(TCE1_CCA_vect)
{
	if (tc_tce1_cca_callback) {
		tc_tce1_cca_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter E1 Compare/CaptureB
 *
 * This function will handle interrupt on Timer Counter E1 Compare/CaptureB and
 * call the callback function.
 */
ISR(TCE1_CCB_vect)
{
	if (tc_tce1_ccb_callback) {
		tc_tce1_ccb_callback();
	}
}

#endif

#if defined(TCF0) || defined(__DOXYGEN__)
//! \internal Local storage of Timer Counter TCF0 interrupt callback function
static tc_callback_t tc_tcf0_ovf_callback;
static tc_callback_t tc_tcf0_err_callback;
static tc_callback_t tc_tcf0_cca_callback;
static tc_callback_t tc_tcf0_ccb_callback;
static tc_callback_t tc_tcf0_ccc_callback;
static tc_callback_t tc_tcf0_ccd_callback;


/**
 * \internal
 * \brief Interrupt handler for Timer Counter E0 overflow
 *
 * This function will handle interrupt on Timer Counter F0 overflow and
 * call the callback function.
 */
ISR(TCF0_OVF_vect)
{
	if (tc_tcf0_ovf_callback) {
		tc_tcf0_ovf_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter F0 error
 *
 * This function will handle interrupt on Timer Counter F0 error and
 * call the callback function.
 */
ISR(TCF0_ERR_vect)
{
	if (tc_tcf0_err_callback) {
		tc_tcf0_err_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter F0 Compare/CaptureA
 *
 * This function will handle interrupt on Timer Counter F0 Compare/CaptureA and
 * call the callback function.
 */
ISR(TCF0_CCA_vect)
{
	if (tc_tcf0_cca_callback) {
		tc_tcf0_cca_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter F0 Compare/CaptureB
 *
 * This function will handle interrupt on Timer Counter F0 Compare/CaptureB and
 * call the callback function.
 */
ISR(TCF0_CCB_vect)
{
	if (tc_tcf0_ccb_callback) {
		tc_tcf0_ccb_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter F0 Compare/CaptureC
 *
 * This function will handle interrupt on Timer Counter F0 Compare/CaptureC and
 * call the callback function.
 */
ISR(TCF0_CCC_vect)
{
	if (tc_tcf0_ccc_callback) {
		tc_tcf0_ccc_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter F0 Compare/CaptureD
 *
 * This function will handle interrupt on Timer Counter F0 Compare/CaptureD and
 * call the callback function.
 */
ISR(TCF0_CCD_vect)
{
	if (tc_tcf0_ccd_callback) {
		tc_tcf0_ccd_callback();
	}
}

#endif

#if defined(TCF1) || defined(__DOXYGEN__)
//! \internal Local storage of Timer Counter TCF1 interrupt callback function
static tc_callback_t tc_tcf1_ovf_callback;
static tc_callback_t tc_tcf1_err_callback;
static tc_callback_t tc_tcf1_cca_callback;
static tc_callback_t tc_tcf1_ccb_callback;

/**
 * \internal
 * \brief Interrupt handler for Timer Counter F1 overflow
 *
 * This function will handle interrupt on Timer Counter F1 overflow and
 * call the callback function.
 */
ISR(TCF1_OVF_vect)
{
	if (tc_tcf1_ovf_callback) {
		tc_tcf1_ovf_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter F1 error
 *
 * This function will handle interrupt on Timer Counter F1 error and
 * call the callback function.
 */
ISR(TCF1_ERR_vect)
{
	if (tc_tcf1_err_callback) {
		tc_tcf1_err_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter F1 Compare/CaptureA
 *
 * This function will handle interrupt on Timer Counter F1 Compare/CaptureA and
 * call the callback function.
 */
ISR(TCF1_CCA_vect)
{
	if (tc_tcf1_cca_callback) {
		tc_tcf1_cca_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter F1 Compare/CaptureB
 *
 * This function will handle interrupt on Timer Counter F1 Compare/CaptureB and
 * call the callback function.
 */
ISR(TCF1_CCB_vect)
{
	if (tc_tcf1_ccb_callback) {
		tc_tcf1_ccb_callback();
	}
}

#endif

/**
 * \brief Enable TC
 *
 * Enables the TC.
 *
 * \param tc Pointer to TC module
 *
 * \note
 * unmask TC clock (sysclk), but does not configure the TC clock source.
 */
void tc_enable(volatile void *tc)
{
	irqflags_t iflags = cpu_irq_save();

#ifdef TCC0
	if ((uintptr_t) tc == (uintptr_t) & TCC0) {
		sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_TC0);
		sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_HIRES);
	} else
#endif
#ifdef TCC1
	if ((uintptr_t) tc == (uintptr_t) & TCC1) {
		sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_TC1);
		sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_HIRES);
	} else
#endif
#ifdef TCD0
	if ((uintptr_t) tc == (uintptr_t) & TCD0) {
		sysclk_enable_module(SYSCLK_PORT_D, SYSCLK_TC0);
		sysclk_enable_module(SYSCLK_PORT_D, SYSCLK_HIRES);
	} else
#endif
#ifdef TCD1
	if ((uintptr_t) tc == (uintptr_t) & TCD1) {
		sysclk_enable_module(SYSCLK_PORT_D, SYSCLK_TC1);
		sysclk_enable_module(SYSCLK_PORT_D, SYSCLK_HIRES);
	} else
#endif
#ifdef TCE0
	if ((uintptr_t) tc == (uintptr_t) & TCE0) {
		sysclk_enable_module(SYSCLK_PORT_E, SYSCLK_TC0);
		sysclk_enable_module(SYSCLK_PORT_E, SYSCLK_HIRES);
	} else
#endif
#ifdef TCE1
	if ((uintptr_t) tc == (uintptr_t) & TCE1) {
		sysclk_enable_module(SYSCLK_PORT_E, SYSCLK_TC1);
		sysclk_enable_module(SYSCLK_PORT_E, SYSCLK_HIRES);
	} else
#endif
#ifdef TCF0
	if ((uintptr_t) tc == (uintptr_t) & TCF0) {
		sysclk_enable_module(SYSCLK_PORT_F, SYSCLK_TC0);
		sysclk_enable_module(SYSCLK_PORT_F, SYSCLK_HIRES);
	} else
#endif
#ifdef TCF1
	if ((uintptr_t) tc == (uintptr_t) & TCF1) {
		sysclk_enable_module(SYSCLK_PORT_F, SYSCLK_TC1);
		sysclk_enable_module(SYSCLK_PORT_F, SYSCLK_HIRES);
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
 * \brief Disable TC
 *
 * Disables the TC.
 *
 * \param tc Pointer to TC module
 *
 * \note
 * mask TC clock (sysclk).
 */
void tc_disable(volatile void *tc)
{
	irqflags_t iflags = cpu_irq_save();

	sleepmgr_unlock_mode(SLEEPMGR_IDLE);

#ifdef TCC0
	if ((uintptr_t) tc == (uintptr_t) & TCC0) {
		sysclk_disable_module(SYSCLK_PORT_C, SYSCLK_TC0);
		sysclk_disable_module(SYSCLK_PORT_C, SYSCLK_HIRES);
	} else
#endif
#ifdef TCC1
	if ((uintptr_t) tc == (uintptr_t) & TCC1) {
		sysclk_disable_module(SYSCLK_PORT_C, SYSCLK_TC1);
		sysclk_disable_module(SYSCLK_PORT_C, SYSCLK_HIRES);
	} else
#endif
#ifdef TCD0
	if ((uintptr_t) tc == (uintptr_t) & TCD0) {
		sysclk_disable_module(SYSCLK_PORT_D, SYSCLK_TC0);
		sysclk_disable_module(SYSCLK_PORT_D, SYSCLK_HIRES);
	} else
#endif
#ifdef TCD1
	if ((uintptr_t) tc == (uintptr_t) & TCD1) {
		sysclk_disable_module(SYSCLK_PORT_D, SYSCLK_TC1);
		sysclk_disable_module(SYSCLK_PORT_D, SYSCLK_HIRES);
	} else
#endif
#ifdef TCE0
	if ((uintptr_t) tc == (uintptr_t) & TCE0) {
		sysclk_disable_module(SYSCLK_PORT_E, SYSCLK_TC0);
		sysclk_disable_module(SYSCLK_PORT_E, SYSCLK_HIRES);
	} else
#endif
#ifdef TCE1
	if ((uintptr_t) tc == (uintptr_t) & TCE1) {
		sysclk_disable_module(SYSCLK_PORT_E, SYSCLK_TC1);
		sysclk_disable_module(SYSCLK_PORT_E, SYSCLK_HIRES);
	} else
#endif
#ifdef TCF0
	if ((uintptr_t) tc == (uintptr_t) & TCF0) {
		sysclk_disable_module(SYSCLK_PORT_F, SYSCLK_TC0);
		sysclk_disable_module(SYSCLK_PORT_F, SYSCLK_HIRES);
	} else
#endif
#ifdef TCF1
	if ((uintptr_t) tc == (uintptr_t) & TCF1) {
		sysclk_disable_module(SYSCLK_PORT_F, SYSCLK_TC1);
		sysclk_disable_module(SYSCLK_PORT_F, SYSCLK_HIRES);
	} else
#endif
	{
		cpu_irq_restore(iflags);
		return;
	}
	cpu_irq_restore(iflags);
}

void tc_set_overflow_interrupt_callback(volatile void *tc,
		tc_callback_t callback)
{
#ifdef TCC0
	if ((uintptr_t) tc == (uintptr_t) & TCC0) {
		tc_tcc0_ovf_callback = callback;
	} else
#endif
#ifdef TCC1
	if ((uintptr_t) tc == (uintptr_t) & TCC1) {
		tc_tcc1_ovf_callback = callback;
	} else
#endif
#ifdef TCD0
	if ((uintptr_t) tc == (uintptr_t) & TCD0) {
		tc_tcd0_ovf_callback = callback;
	} else
#endif
#ifdef TCD1
	if ((uintptr_t) tc == (uintptr_t) & TCD1) {
		tc_tcd1_ovf_callback = callback;
	} else
#endif
#ifdef TCE0
	if ((uintptr_t) tc == (uintptr_t) & TCE0) {
		tc_tce0_ovf_callback = callback;
	} else
#endif
#ifdef TCE1
	if ((uintptr_t) tc == (uintptr_t) & TCE1) {
		tc_tce1_ovf_callback = callback;
	} else
#endif
#ifdef TCF0
	if ((uintptr_t) tc == (uintptr_t) & TCF0) {
		tc_tcf0_ovf_callback = callback;
	} else
#endif
#ifdef TCF1
	if ((uintptr_t) tc == (uintptr_t) & TCF1) {
		tc_tcf1_ovf_callback = callback;
	} else
#endif
	{}
}

void tc_set_error_interrupt_callback(volatile void *tc, tc_callback_t callback)
{
#ifdef TCC0
	if ((uintptr_t) tc == (uintptr_t) & TCC0) {
		tc_tcc0_err_callback = callback;
	} else
#endif
#ifdef TCC1
	if ((uintptr_t) tc == (uintptr_t) & TCC1) {
		tc_tcc1_err_callback = callback;
	} else
#endif
#ifdef TCD0
	if ((uintptr_t) tc == (uintptr_t) & TCD0) {
		tc_tcd0_err_callback = callback;
	} else
#endif
#ifdef TCD1
	if ((uintptr_t) tc == (uintptr_t) & TCD1) {
		tc_tcd1_err_callback = callback;
	} else
#endif
#ifdef TCE0
	if ((uintptr_t) tc == (uintptr_t) & TCE0) {
		tc_tce0_err_callback = callback;
	} else
#endif
#ifdef TCE1
	if ((uintptr_t) tc == (uintptr_t) & TCE1) {
		tc_tce1_err_callback = callback;
	} else
#endif
#ifdef TCF0
	if ((uintptr_t) tc == (uintptr_t) & TCF0) {
		tc_tcf0_err_callback = callback;
	} else
#endif
#ifdef TCF1
	if ((uintptr_t) tc == (uintptr_t) & TCF1) {
		tc_tcf1_err_callback = callback;
	} else
#endif
	{}
}

void tc_set_cca_interrupt_callback(volatile void *tc, tc_callback_t callback)
{
#ifdef TCC0
	if ((uintptr_t) tc == (uintptr_t) & TCC0) {
		tc_tcc0_cca_callback = callback;
	} else
#endif
#ifdef TCC1
	if ((uintptr_t) tc == (uintptr_t) & TCC1) {
		tc_tcc1_cca_callback = callback;
	} else
#endif
#ifdef TCD0
	if ((uintptr_t) tc == (uintptr_t) & TCD0) {
		tc_tcd0_cca_callback = callback;
	} else
#endif
#ifdef TCD1
	if ((uintptr_t) tc == (uintptr_t) & TCD1) {
		tc_tcd1_cca_callback = callback;
	} else
#endif
#ifdef TCE0
	if ((uintptr_t) tc == (uintptr_t) & TCE0) {
		tc_tce0_cca_callback = callback;
	} else
#endif
#ifdef TCE1
	if ((uintptr_t) tc == (uintptr_t) & TCE1) {
		tc_tce1_cca_callback = callback;
	} else
#endif
#ifdef TCF0
	if ((uintptr_t) tc == (uintptr_t) & TCF0) {
		tc_tcf0_cca_callback = callback;
	} else
#endif
#ifdef TCF1
	if ((uintptr_t) tc == (uintptr_t) & TCF1) {
		tc_tcf1_cca_callback = callback;
	}  else
#endif
	{}
}

void tc_set_ccb_interrupt_callback(volatile void *tc, tc_callback_t callback)
{
#ifdef TCC0
	if ((uintptr_t) tc == (uintptr_t) & TCC0) {
		tc_tcc0_ccb_callback = callback;
	} else
#endif
#ifdef TCC1
	if ((uintptr_t) tc == (uintptr_t) & TCC1) {
		tc_tcc1_ccb_callback = callback;
	} else
#endif
#ifdef TCD0
	if ((uintptr_t) tc == (uintptr_t) & TCD0) {
		tc_tcd0_ccb_callback = callback;
	} else
#endif
#ifdef TCD1
	if ((uintptr_t) tc == (uintptr_t) & TCD1) {
		tc_tcd1_ccb_callback = callback;
	} else
#endif
#ifdef TCE0
	if ((uintptr_t) tc == (uintptr_t) & TCE0) {
		tc_tce0_ccb_callback = callback;
	} else
#endif
#ifdef TCE1
	if ((uintptr_t) tc == (uintptr_t) & TCE1) {
		tc_tce1_ccb_callback = callback;
	} else
#endif
#ifdef TCF0
	if ((uintptr_t) tc == (uintptr_t) & TCF0) {
		tc_tcf0_ccb_callback = callback;
	} else
#endif
#ifdef TCF1
	if ((uintptr_t) tc == (uintptr_t) & TCF1) {
		tc_tcf1_ccb_callback = callback;
	} else
#endif
	{}
}

void tc_set_ccc_interrupt_callback(volatile void *tc, tc_callback_t callback)
{
#ifdef TCC0
	if ((uintptr_t) tc == (uintptr_t) & TCC0) {
		tc_tcc0_ccc_callback = callback;
	} else
#endif

#ifdef TCD0
	if ((uintptr_t) tc == (uintptr_t) & TCD0) {
		tc_tcd0_ccc_callback = callback;
	} else
#endif

#ifdef TCE0
	if ((uintptr_t) tc == (uintptr_t) & TCE0) {
		tc_tce0_ccc_callback = callback;
	} else
#endif

#ifdef TCF0
	if ((uintptr_t) tc == (uintptr_t) & TCF0) {
		tc_tcf0_ccc_callback = callback;
	}  else
#endif
	{}

}


void tc_set_ccd_interrupt_callback(volatile void *tc, tc_callback_t callback)
{
#ifdef TCC0
	if ((uintptr_t) tc == (uintptr_t) & TCC0) {
		tc_tcc0_ccd_callback = callback;
	} else
#endif

#ifdef TCD0
	if ((uintptr_t) tc == (uintptr_t) & TCD0) {
		tc_tcd0_ccd_callback = callback;
	} else
#endif

#ifdef TCE0
	if ((uintptr_t) tc == (uintptr_t) & TCE0) {
		tc_tce0_ccd_callback = callback;
	} else
#endif

#ifdef TCF0
	if ((uintptr_t) tc == (uintptr_t) & TCF0) {
		tc_tcf0_ccd_callback = callback;
	} else
#endif
	{}
}
