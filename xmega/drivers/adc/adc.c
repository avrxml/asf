/**
 * \file
 *
 * \brief AVR XMEGA Analog to Digital Converter driver
 *
 * Copyright (c) 2010-2018 Microchip Technology Inc. and its subsidiaries.
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

#include <compiler.h>
#include <adc.h>

/**
 * \ingroup adc_module_group
 * @{
 */

/** \name ADC interrupt callback function */
/** @{ */
#ifdef ADCA

/**
 * \internal
 * \brief ADC A enable counter
 *
 * This is used to ensure that ADC A is not inadvertently disabled when its
 * module or channel configurations are updated.
 */
static uint8_t adca_enable_count;

#  ifdef CONFIG_ADC_CALLBACK_ENABLE

/**
 * \internal
 * \brief ADC A interrupt callback function pointer
 */
adc_callback_t adca_callback;

#  endif
#endif

#ifdef ADCB

/**
 * \internal
 * \brief ADC B enable counter
 *
 * This is used to ensure that ADC B is not inadvertently disabled when its
 * module or channel configurations are updated.
 */
static uint8_t adcb_enable_count;

#  ifdef CONFIG_ADC_CALLBACK_ENABLE

/**
 * \internal
 * \brief ADC B interrupt callback function pointer
 */
adc_callback_t adcb_callback;

#  endif
#endif

#if defined(CONFIG_ADC_CALLBACK_ENABLE) || defined(__DOXYGEN__)

/**
 * \brief Set ADC interrupt callback function
 *
 * Sets a new callback function for interrupts on the specified ADC.
 *
 * \param adc Pointer to ADC module.
 * \param callback Pointer to the callback function to set.
 */
void adc_set_callback(ADC_t *adc, adc_callback_t callback)
{
	irqflags_t flags;

	Assert(callback);

	flags = cpu_irq_save();

#ifdef ADCA
	if ((uintptr_t)adc == (uintptr_t)&ADCA) {
		adca_callback = callback;
	} else
#endif

#ifdef ADCB
	if ((uintptr_t)adc == (uintptr_t)&ADCB) {
		adcb_callback = callback;
	} else
#endif

	{
		Assert(0);
	}

	cpu_irq_restore(flags);
}

#endif /* CONFIG_ADC_CALLBACK_ENABLE */

/** @} */

/** \name Internal functions for driver */
/** @{ */

/**
 * \internal
 * \brief Enable peripheral clock for ADC
 *
 * Checks if the enable count for the ADC is zero, then increments it. If the
 * count was zero, the peripheral clock is enabled. Otherwise, it is already
 * enabled.
 *
 * \param adc Pointer to ADC module.
 */
void adc_enable_clock(ADC_t *adc);

void adc_enable_clock(ADC_t *adc)
{
#ifdef ADCA
	if ((uintptr_t)adc == (uintptr_t)(&ADCA)) {
		Assert(adca_enable_count < 0xff);
		if (!adca_enable_count++) {
			sysclk_enable_module(SYSCLK_PORT_A, SYSCLK_ADC);
		}
	} else
#endif

#ifdef ADCB
	if ((uintptr_t)adc == (uintptr_t)(&ADCB)) {
		Assert(adcb_enable_count < 0xff);
		if (!adcb_enable_count++) {
			sysclk_enable_module(SYSCLK_PORT_B, SYSCLK_ADC);
		}
	} else
#endif

	{
		Assert(0);
	}
}

/**
 * \internal
 * \brief Disable peripheral clock for ADC
 *
 * Decrements the enable count for the ADC, then disables its peripheral clock
 * if the count hit zero. If the count did not hit zero, it indicates the ADC is
 * enabled.
 *
 * \param adc Pointer to ADC module
 */
void adc_disable_clock(ADC_t *adc);

void adc_disable_clock(ADC_t *adc)
{
#ifdef ADCA
	if ((uintptr_t)adc == (uintptr_t)(&ADCA)) {
		Assert(adca_enable_count);
		if (!--adca_enable_count) {
			sysclk_disable_module(SYSCLK_PORT_A, SYSCLK_ADC);
		}
	} else
#endif

#ifdef ADCB
	if ((uintptr_t)adc == (uintptr_t)(&ADCB)) {
		Assert(adcb_enable_count);
		if (!--adcb_enable_count) {
			sysclk_disable_module(SYSCLK_PORT_B, SYSCLK_ADC);
		}
	} else
#endif

	{
		Assert(0);
	}
}

/** @} */

/** \name ADC module management */
/** @{ */

/**
 * \brief Enable ADC
 *
 * Enables the ADC and locks IDLE mode for the sleep manager.
 *
 * \param adc Pointer to ADC module
 *
 * \note To ensure accurate conversions, please wait for at least
 * the specified start-up time between enabling the ADC module, and starting
 * a conversion. For most XMEGA devices the start-up time is specified
 * to be a maximum of 24 ADC clock cycles. Please verify the start-up time for
 * the device in use.
 */
void adc_enable(ADC_t *adc)
{
	irqflags_t flags = cpu_irq_save();
	adc_enable_clock(adc);
	adc->CTRLA |= ADC_ENABLE_bm;
	cpu_irq_restore(flags);

	sleepmgr_lock_mode(SLEEPMGR_IDLE);
}

/**
 * \brief Disable ADC
 *
 * Disables the ADC and unlocks IDLE mode for the sleep manager.
 *
 * \param adc Pointer to ADC module
 */
void adc_disable(ADC_t *adc)
{
	irqflags_t flags = cpu_irq_save();
	adc->CTRLA &= ~ADC_ENABLE_bm;
	adc_disable_clock(adc);
	cpu_irq_restore(flags);

	sleepmgr_unlock_mode(SLEEPMGR_IDLE);
}

/**
 * \brief Check if the ADC is enabled
 *
 * \param adc Pointer to ADC module.
 *
 * \retval true if ADC is enabled.
 * \retval false if ADC is disabled.
 */
bool adc_is_enabled(ADC_t *adc)
{
	/* It is sufficient to return the state of the ADC enable counters
	 * since all driver functions that change the counts are protected
	 * against interrupts and only the enable/disable functions leave the
	 * counts incremented/decremented upon return.
	 */
#ifdef ADCA
	if ((uintptr_t)adc == (uintptr_t)&ADCA) {
		return adca_enable_count;
	} else
#endif

#ifdef ADCB
	if ((uintptr_t)adc == (uintptr_t)&ADCB) {
		return adcb_enable_count;
	} else
#endif

	{
		Assert(0);
		return false;
	}
}

/** @} */

/** @} */
