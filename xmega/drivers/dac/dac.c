/**
 * \file
 *
 * \brief AVR XMEGA Digital to Analog Converter driver
 *
 * Copyright (c) 2010-2015 Atmel Corporation. All rights reserved.
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
#include <compiler.h>
#include <dac.h>
#include <sleepmgr.h>

/**
 * \ingroup dac_group
 *
 * @{
 */

#if defined(DACA)
/**
 * \internal
 * \brief DAC A enable count
 *
 * This is used to ensure that DAC A has a peripheral clock signal when it is
 * enabled, even if its configuration is changed.
 */
static uint8_t daca_enable_count;
#endif /* DACA */

#if defined(DACB)
/**
 * \internal
 * \brief DAC B enable count
 *
 * This is used to ensure that DAC B has a peripheral clock signal when it is
 * enabled, even if its configuration is changed.
 */
static uint8_t dacb_enable_count;
#endif /* DACB */

/**
 * \internal
 * \brief Enable peripheral clock for DAC
 *
 * Enables the peripheral clock for the specified DAC, unless it is already
 * enabled.
 *
 * \param dac Pointer to DAC module.
 */
static void dac_enable_clock(DAC_t *dac)
{
#if defined(DACA)
	if ((uintptr_t)dac == (uintptr_t)&DACA) {
		if (!daca_enable_count++) {
			sysclk_enable_module(SYSCLK_PORT_A, SYSCLK_DAC);
		}
	} else
#endif

#if defined(DACB)
	if ((uintptr_t)dac == (uintptr_t)&DACB) {
		if (!dacb_enable_count++) {
			sysclk_enable_module(SYSCLK_PORT_B, SYSCLK_DAC);
		}
	} else
#endif

	{
		Assert(0);
		return;
	}
}

/**
 * \internal
 * \brief Disable peripheral clock for DAC
 *
 * Disables the peripheral clock for the specified DAC, unless the enable count
 * indicates that it should be left enabled.
 *
 * \param dac Pointer to DAC module.
 */
static void dac_disable_clock(DAC_t *dac)
{
#if defined(DACA)
	if ((uintptr_t)dac == (uintptr_t)&DACA) {
		if (!--daca_enable_count) {
			sysclk_disable_module(SYSCLK_PORT_A, SYSCLK_DAC);
		}
	} else
#endif

#if defined(DACB)
	if ((uintptr_t)dac == (uintptr_t)&DACB) {
		if (!--dacb_enable_count) {
			sysclk_disable_module(SYSCLK_PORT_B, SYSCLK_DAC);
		}
	} else
#endif

	{
		Assert(0);
		return;
	}
}

/**
 * \brief Enable DAC
 *
 * Enables the DAC for conversions, and locks the sleep mode needed for
 * the DAC to operate.
 *
 * \param dac Pointer to DAC module.
 */
void dac_enable(DAC_t *dac)
{
	irqflags_t flags;

	flags = cpu_irq_save();
	sleepmgr_lock_mode(SLEEPMGR_IDLE);
	dac_enable_clock(dac);
	dac->CTRLA |= DAC_ENABLE_bm;
	cpu_irq_restore(flags);
}

/**
 * \brief Disable DAC
 *
 * Disables the DAC, stopping all conversions.
 *
 * \param dac Pointer to DAC module.
 */
void dac_disable(DAC_t *dac)
{
	irqflags_t flags;

	flags = cpu_irq_save();
	dac->CTRLA &= ~DAC_ENABLE_bm;
	dac_disable_clock(dac);
	sleepmgr_unlock_mode(SLEEPMGR_IDLE);
	cpu_irq_restore(flags);
}

/**
 * \brief Check if DAC is enabled
 *
 * \param dac Pointer to DAC module.
 */
bool dac_is_enabled(DAC_t *dac)
{
	/* It is sufficient to return the state of the DAC enable counters
	 * since all driver functions that change the counts are protected
	 * against interrupts and only the enable/disable functions leave the
	 * counts incremented/decremented upon return.
	 */
#if defined(DACA)
	if ((uintptr_t)dac == (uintptr_t)&DACA) {
		return daca_enable_count;
	} else
#endif

#if defined(DACB)
	if ((uintptr_t)dac == (uintptr_t)&DACB) {
		return dacb_enable_count;
	} else
#endif

	{
		Assert(0);
		return false;
	}
}

/**
 * \brief Write configuration to DAC
 *
 * Disables the DAC before writing the specified configuration and factory
 * calibration values to it. If the DAC was enabled upon entry of the function,
 * it is enabled upon function return.
 *
 * \param dac Pointer to DAC module.
 * \param conf Pointer to DAC configuration.
 */
void dac_write_configuration(DAC_t *dac, struct dac_config *conf)
{
	irqflags_t flags;
	uint8_t    enable;
	uint8_t    gaincal0;
	uint8_t    offsetcal0;
	/* avoid Cppcheck Warning */
	UNUSED(gaincal0);
	UNUSED(offsetcal0);
#if XMEGA_DAC_VERSION_2
	uint8_t    gaincal1;
	uint8_t    offsetcal1;
#endif

#if defined(DACA)
	if ((uintptr_t)dac == (uintptr_t)&DACA) {
#  if XMEGA_DAC_VERSION_1
		gaincal0 = dac_get_calibration_data(DAC_CAL_DACA_GAIN);
		offsetcal0 = dac_get_calibration_data(DAC_CAL_DACA_OFFSET);
#  elif XMEGA_DAC_VERSION_2
		gaincal0 = dac_get_calibration_data(DAC_CAL_DACA0_GAIN);
		offsetcal0 = dac_get_calibration_data(DAC_CAL_DACA0_OFFSET);
		gaincal1 = dac_get_calibration_data(DAC_CAL_DACA1_GAIN);
		offsetcal1 = dac_get_calibration_data(DAC_CAL_DACA1_OFFSET);
#  endif
	} else
#endif

#if defined(DACB)
	if ((uintptr_t)dac == (uintptr_t)&DACB) {
#  if XMEGA_DAC_VERSION_1
		gaincal0 = dac_get_calibration_data(DAC_CAL_DACB_GAIN);
		offsetcal0 = dac_get_calibration_data(DAC_CAL_DACB_OFFSET);
#  elif XMEGA_DAC_VERSION_2
		gaincal0 = dac_get_calibration_data(DAC_CAL_DACB0_GAIN);
		offsetcal0 = dac_get_calibration_data(DAC_CAL_DACB0_OFFSET);
		gaincal1 = dac_get_calibration_data(DAC_CAL_DACB1_GAIN);
		offsetcal1 = dac_get_calibration_data(DAC_CAL_DACB1_OFFSET);
#  endif
	} else
#endif

	{
		Assert(0);
		return;
	}

	flags = cpu_irq_save();
	dac_enable_clock(dac);

	// Temporarily disable the DAC, remember if it was enabled.
	enable = dac->CTRLA & DAC_ENABLE_bm;
	dac->CTRLA = 0;

	dac->CTRLB = conf->ctrlb;
	dac->CTRLC = conf->ctrlc;
	dac->EVCTRL = conf->evctrl;

#if XMEGA_DAC_VERSION_1
	dac->TIMCTRL = conf->timctrl;

	dac->GAINCAL = gaincal0;
	dac->OFFSETCAL = offsetcal0;
#elif XMEGA_DAC_VERSION_2
	dac->CH0GAINCAL = gaincal0;
	dac->CH0OFFSETCAL = offsetcal0;
	dac->CH1GAINCAL = gaincal1;
	dac->CH1OFFSETCAL = offsetcal1;
#endif

	dac->CTRLA = conf->ctrla | enable;

	dac_disable_clock(dac);
	cpu_irq_restore(flags);
}

/**
 * \brief Read configuration from DAC
 *
 * Reads out the current configuration of the DAC to the specified buffer.
 *
 * \param dac Pointer to DAC module.
 * \param conf Pointer to DAC configuration.
 */
void dac_read_configuration(DAC_t *dac, struct dac_config *conf)
{
	irqflags_t flags;

	flags = cpu_irq_save();
	dac_enable_clock(dac);

	conf->ctrla = dac->CTRLA & ~DAC_ENABLE_bm;
	conf->ctrlb = dac->CTRLB;
	conf->ctrlc = dac->CTRLC;
	conf->evctrl = dac->EVCTRL;

#if XMEGA_DAC_VERSION_1
	conf->timctrl = dac->TIMCTRL;
#endif

	dac_disable_clock(dac);
	cpu_irq_restore(flags);
}

//! @}
