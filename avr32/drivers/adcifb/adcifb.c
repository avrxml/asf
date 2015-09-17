/**
 * \file
 *
 * \brief ADCIFB Software driver interface for AVR32 UC3.
 *
 * Copyright (C) 2011-2015 Atmel Corporation. All rights reserved.
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

#include <avr32/io.h>
#include "compiler.h"
#include "adcifb.h"

/**
 * \brief Initialize the ADCIFB module (enable and configure).
 *        Mandatory to call.
 *
 * \param *adcifb       Base address of the ADCIFB module
 * \param p_adcifb_opt  Structure for the ADCIFB module configuration
 *
 * \note To avoid an infinite loop, this function checks ADCIFB READY status
 *       bit but no more than ADCIFB_POLL_TIMEOUT times.
 *
 * \return Status.
 * \retval 0   ADCIFB successfully enabled and configured
 * \retval <0  ADCIFB initialization failed.
 */
int32_t adcifb_configure(volatile avr32_adcifb_t *adcifb,
		const adcifb_opt_t *p_adcifb_opt)
{
	uint32_t prescal_tempo;

	/* Enable the ADCIFB module. */
	adcifb->cr = AVR32_ADCIFB_CR_EN_MASK;

	/* Compute the PRESCAL field according to the ratio
	 * Fclk_adcifb/Fclk_adc. */
	/* PRESCAL = ((Fclk_adcifb/Fclk_adc)/2)-1 */
	prescal_tempo = (p_adcifb_opt->ratio_clkadcifb_clkadc >> 1) - 1;

	/* Set the ADC configuration */
	adcifb->acr
		= (p_adcifb_opt->sleep_mode_enable <<
			AVR32_ADCIFB_ACR_SLEEP_OFFSET)
			| (p_adcifb_opt->resolution <<
			AVR32_ADCIFB_ACR_RES_OFFSET)
			| (prescal_tempo << AVR32_ADCIFB_ACR_PRESCAL_OFFSET)
			| (p_adcifb_opt->startup <<
			AVR32_ADCIFB_ACR_STARTUP_OFFSET)
			| (p_adcifb_opt->shtim <<
			AVR32_ADCIFB_ACR_SHTIM_OFFSET);

	/* Wait Startup Time, with a timeout. */
	return(adcifb_sr_statushigh_wait(adcifb, AVR32_ADCIFB_SR_READY_MASK));
}

/**
 * \brief Disable ADCIFB.
 *
 * \param *adcifb  Base address of the ADCIFB module
 * \return Status.
 * \retval 0   ADCIFB successfully disabled
 * \retval <0  ADCIFB configuration failed.
 */
int32_t adcifb_disable(volatile avr32_adcifb_t *adcifb)
{
	uint32_t timeout = ADCIFB_POLL_TIMEOUT;
	/* Wait till the ADCIFB module is idle */
	while (adcifb_is_busy(adcifb)) {
		if (--timeout == 0) {
			return -1;
		}
	}
	/* Enable the ADCIFB module. */
	adcifb->cr = AVR32_ADCIFB_CR_DIS_MASK;
	return PASS;
}

/**
 * \brief Configure the ADCIFB trigger mode.
 *
 * \param *adcifb Base address of the ADCIFB module
 * \param trgmod Trigger mode, one of AVR32_ADCIFB_TRGR_TRGMOD_* defines
 *               (cf adcifb_xxx.h part header file)
 * \param trgper Defines the Trigger period where
 *                Trigger Period = trgper *Tclk_adc
 *               (effective only if trigger_mode==AVR32_ADCIFB_TRGMOD_PT)
 *
 * \return Status.
 * \retval 0   ADCIFB trigger mode successfully configured
 * \retval <0  ADCIFB trigger mode configuration failed.
 */
int32_t adcifb_configure_trigger(volatile avr32_adcifb_t *adcifb,
		uint8_t trgmod, uint32_t trgper)
{
	/*
	 * If the trigger mode is not set to "Periodic Trigger",
	 * clear the trgper field.
	 */
	if (AVR32_ADCIFB_TRGMOD_PT != trgmod) {
		trgper = 0;
	}

	/* Configure the trigger mode */
	adcifb->trgr
		= ((trgmod <<
			AVR32_ADCIFB_TRGMOD_OFFSET) & AVR32_ADCIFB_TRGMOD_MASK)
			| ((trgper <<
			AVR32_ADCIFB_TRGPER_OFFSET) & AVR32_ADCIFB_TRGPER_MASK);
	return PASS;
}

/**
 * \brief Enable Pen Detect mode in ADCIFB
 *
 * \param *adcifb          Base address of the ADCIFB module
 * \param pen_dbc_period   Pen Detect Debouncing Period
 */
void adcifb_enable_pen_detect_mode(volatile avr32_adcifb_t *adcifb,
		uint8_t pen_dbc_period)
{
	/* set the required pen detect debouncing period */
	adcifb_set_pen_debouncing_period(adcifb, pen_dbc_period);
	/* Enable Pen Detect Mode */
	adcifb->mr  |= AVR32_ADCIFB_MR_PENDET_MASK;
}

/**
 * \brief Enable Touch Screen mode in ADCIFB
 *
 * \param *adcifb                   Base address of the ADCIFB module
 * \param touch_screen_pin_offset   The Touch Screen Pin Offset field is used
 *                                  to indicate which AD pins are connected to
 *                                  the resistive touch screen film edges.
 * \param analog_pin_output_enable  Enable Analog Pin Output in ADCIFB
 * \warning If the selected I/O voltage configuration is incompatible with the
 *          ADC cell voltage specification, analog_pin_output_enable must stay
 *          cleared to avoid damaging the ADC. In this case the ADP pins must
 *          be used to drive VDD instead. If the I/O and ADC voltages are
 *          compatible, the AD pins can be used directly by writing a one to
 *          this bit. In this case the ADP pins can be ignored.
 */
void adcifb_enable_touch_screen_mode(volatile avr32_adcifb_t *adcifb,
		uint8_t touch_screen_pin_offset, bool analog_pin_output_enable)
{
	/* set the required Touch Screen Pin Offset */
	adcifb_set_touch_screen_pin_offset(adcifb, touch_screen_pin_offset);
	/* Touch Screen ADC mode enable */
	adcifb->mr  |= AVR32_ADCIFB_MR_TSAMOD_MASK;
	if (analog_pin_output_enable) {
		adcifb_enable_analog_pin_output(adcifb);
	}
}

/**
 * \brief Get Last Converted data
 *
 * \param *adcifb   Base address of the ADCIFB module
 *
 * \return The value acquired (uint32_t)
 * \warning The return value contains the converted data in the bits[0-11];
 *          bits[16-23] contain the channel id that was last converted.
 */
uint32_t adcifb_get_last_data(volatile avr32_adcifb_t *adcifb)
{
	return (adcifb->lcdr);
}

/**
 * \brief Wait for a status high in the ADCIFB SR status register.
 *
 * \param *adcifb     Base address of the ADCIFB module
 * \param statusMask  Mask field of the status to poll
 *
 * \return Status.
 * \retval 0   Status is high.
 * \retval <0  ADCIFB_POLL_TIMEOUT Timeout expired before the status was high
 */
int32_t adcifb_sr_statushigh_wait(volatile avr32_adcifb_t *adcifb,
		uint32_t statusMask)
{
	uint32_t timeout = ADCIFB_POLL_TIMEOUT;

	while (!(adcifb->sr & statusMask)) {
		if (--timeout == 0) {
			return -1;
		}
	}
	return PASS;
}

/**
 * \brief Check if the ADCIFB is ready to perform a conversion sequence.
 *
 * \param *adcifb Base address of the ADCIFB module
 *
 * \return bool   true if ADCIFB is ready to perform conversion sequence.
 *                false if ADCIFB is not ready to perform conversion sequence.
 */
bool adcifb_is_ready(volatile avr32_adcifb_t *adcifb)
{
	return ((adcifb->sr & AVR32_ADCIFB_SR_READY_MASK)
	       >> AVR32_ADCIFB_SR_READY_OFFSET);
}

/**
 * \brief Check if the ADCIFB is busy.
 *
 * \param *adcifb Base address of the ADCIFB module
 *
 * \return bool   true if ADCIFB is busy.
 *                false if ADCIFB is idle.
 */
bool adcifb_is_busy(volatile avr32_adcifb_t *adcifb)
{
	return ((adcifb->sr & AVR32_ADCIFB_SR_BUSY_MASK)
	       >> AVR32_ADCIFB_SR_BUSY_OFFSET);
}

/**
 * \brief Check if one or more data has been converted since the start of
 *        conversion and is available
 *
 * \param *adcifb Base address of the ADCIFB module
 *
 * \return bool   true if one or more data has been converted since the start
 *                of conversion and is available false if no data has been
 *                converted since the start of conversion
 */
bool adcifb_is_drdy(volatile avr32_adcifb_t *adcifb)
{
	return ((adcifb->sr & AVR32_ADCIFB_SR_DRDY_MASK)
	       >> AVR32_ADCIFB_SR_DRDY_OFFSET);
}

/**
 * \brief Check if one or more Overrun Error has occurred since the start of
 *        conversion.
 *
 * \param *adcifb Base address of the ADCIFB module
 *
 * \return bool   true if an overrun has occurred
 *                false if no overrun occurred
 */
bool adcifb_is_ovre(volatile avr32_adcifb_t *adcifb)
{
	return ((adcifb->sr & AVR32_ADCIFB_SR_OVRE_MASK)
	       >> AVR32_ADCIFB_SR_OVRE_OFFSET);
}
