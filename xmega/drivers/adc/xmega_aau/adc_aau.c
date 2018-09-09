/**
 * \file
 *
 * \brief AVR XMEGA A/AU specific ADC driver implementation
 *
 * Copyright (c) 2012-2018 Microchip Technology Inc. and its subsidiaries.
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

#include "../adc.h"

/**
 * \ingroup adc_module_group
 * @{
 */

/** \name Internal functions for driver */
/** @{ */
extern void adc_enable_clock(ADC_t *adc);
extern void adc_disable_clock(ADC_t *adc);

/** @} */

/** \name ADC interrupt callback function */
/** @{ */

#ifdef ADCA
#  ifdef CONFIG_ADC_CALLBACK_ENABLE

extern adc_callback_t adca_callback;

/**
 * \internal
 * \brief ISR for channel 0 on ADC A
 *
 * Calls the callback function that has been set for the ADC when the channel's
 * interrupt flag is set, if its interrupt has been enabled.
 */
ISR(ADCA_CH0_vect)
{
	adca_callback(&ADCA, ADC_CH0, adc_get_result(&ADCA, ADC_CH0));
}

/**
 * \internal
 * \brief ISR for channel 1 on ADC A
 *
 * Calls the callback function that has been set for the ADC when the channel's
 * interrupt flag is set, if its interrupt has been enabled.
 */
ISR(ADCA_CH1_vect)
{
	adca_callback(&ADCA, ADC_CH1, adc_get_result(&ADCA, ADC_CH1));
}

/**
 * \internal
 * \brief ISR for channel 2 on ADC A
 *
 * Calls the callback function that has been set for the ADC when the channel's
 * interrupt flag is set, if its interrupt has been enabled.
 */
ISR(ADCA_CH2_vect)
{
	adca_callback(&ADCA, ADC_CH2, adc_get_result(&ADCA, ADC_CH2));
}

/**
 * \internal
 * \brief ISR for channel 3 on ADC A
 *
 * Calls the callback function that has been set for the ADC when the channel's
 * interrupt flag is set, if its interrupt has been enabled.
 */
ISR(ADCA_CH3_vect)
{
	adca_callback(&ADCA, ADC_CH3, adc_get_result(&ADCA, ADC_CH3));
}

#  endif
#endif

#ifdef ADCB
#  ifdef CONFIG_ADC_CALLBACK_ENABLE

extern adc_callback_t adcb_callback;

/**
 * \internal
 * \brief ISR for channel 0 on ADC B
 *
 * Calls the callback function that has been set for the ADC when the channel's
 * interrupt flag is set, if its interrupt has been enabled.
 */
ISR(ADCB_CH0_vect)
{
	adcb_callback(&ADCB, ADC_CH0, adc_get_result(&ADCB, ADC_CH0));
}

/**
 * \internal
 * \brief ISR for channel 1 on ADC B
 *
 * Calls the callback function that has been set for the ADC when the channel's
 * interrupt flag is set, if its interrupt has been enabled.
 */
ISR(ADCB_CH1_vect)
{
	adcb_callback(&ADCB, ADC_CH1, adc_get_result(&ADCB, ADC_CH1));
}

/**
 * \internal
 * \brief ISR for channel 2 on ADC B
 *
 * Calls the callback function that has been set for the ADC when the channel's
 * interrupt flag is set, if its interrupt has been enabled.
 */
ISR(ADCB_CH2_vect)
{
	adcb_callback(&ADCB, ADC_CH2, adc_get_result(&ADCB, ADC_CH2));
}

/**
 * \internal
 * \brief ISR for channel 3 on ADC B
 *
 * Calls the callback function that has been set for the ADC when the channel's
 * interrupt flag is set, if its interrupt has been enabled.
 */
ISR(ADCB_CH3_vect)
{
	adcb_callback(&ADCB, ADC_CH3, adc_get_result(&ADCB, ADC_CH3));
}

#  endif
#endif

/** @} */

/** \name ADC module configuration */
/** @{ */

/**
 * \brief Write configuration to ADC module
 *
 * Disables the ADC and flushes its pipeline before writing the specified
 * configuration and factory calibration value to it. If the ADC was enabled
 * upon entry of the function, it is enabled upon function return.
 *
 * \param adc Pointer to ADC module.
 * \param conf Pointer to ADC module configuration.
 */
void adc_write_configuration(ADC_t *adc, const struct adc_config *conf)
{
	uint16_t cal;
	uint8_t enable;
	irqflags_t flags;

#ifdef ADCA
	if ((uintptr_t)adc == (uintptr_t)&ADCA) {
		cal = adc_get_calibration_data(ADC_CAL_ADCA);
	} else
#endif

#ifdef ADCB
	if ((uintptr_t)adc == (uintptr_t)&ADCB) {
		cal = adc_get_calibration_data(ADC_CAL_ADCB);
	} else
#endif

	{
		Assert(0);
		return;
	}

	flags = cpu_irq_save();
	adc_enable_clock(adc);
	enable = adc->CTRLA & ADC_ENABLE_bm;

	adc->CTRLA = ADC_FLUSH_bm;
	adc->CAL = cal;
	adc->CMP = conf->cmp;
	adc->REFCTRL = conf->refctrl;
	adc->PRESCALER = conf->prescaler;
	adc->EVCTRL = conf->evctrl;
	adc->CTRLB = conf->ctrlb;

	adc->CTRLA = enable | conf->ctrla;

	adc_disable_clock(adc);

	cpu_irq_restore(flags);
}

/**
 * \brief Read configuration from ADC module
 *
 * Reads out the current configuration of the ADC module to the specified
 * buffer.
 *
 * \param adc Pointer to ADC module.
 * \param conf Pointer to ADC module configuration.
 */
void adc_read_configuration(ADC_t *adc, struct adc_config *conf)
{
	irqflags_t flags = cpu_irq_save();

	adc_enable_clock(adc);

	conf->ctrla = adc->CTRLA & ADC_DMASEL_gm;

	conf->cmp = adc->CMP;
	conf->refctrl = adc->REFCTRL;
	conf->prescaler = adc->PRESCALER;
	conf->evctrl = adc->EVCTRL;
	conf->ctrlb = adc->CTRLB;

	adc_disable_clock(adc);

	cpu_irq_restore(flags);
}

/** @} */

/** @} */

/**
 * \ingroup adc_channel_group
 * @{
 */

/** \name ADC channel configuration */
/** @{ */

/**
 * \brief Write configuration to ADC channel
 *
 * Writes the specified configuration to the ADC channel.
 *
 * \param adc Pointer to ADC module.
 * \param ch_mask Mask of ADC channel(s):
 * \arg \c ADC_CHn , where \c n specifies the channel. (Only a single channel
 * can be given in mask)
 * \param ch_conf Pointer to ADC channel configuration.
 *
 * \note The specified ADC's callback function must be set before this function
 * is called if callbacks are enabled and interrupts are enabled in the
 * channel configuration.
 */
void adcch_write_configuration(ADC_t *adc, uint8_t ch_mask,
		const struct adc_channel_config *ch_conf)
{
	ADC_CH_t *adc_ch;
	irqflags_t flags;

	adc_ch = adc_get_channel(adc, ch_mask);

	flags = cpu_irq_save();

#if defined(CONFIG_ADC_CALLBACK_ENABLE) && defined(_ASSERT_ENABLE_)
	if ((adc_ch->INTCTRL & ADC_CH_INTLVL_gm) != ADC_CH_INTLVL_OFF_gc) {
#  ifdef ADCA
		if ((uintptr_t)adc == (uintptr_t)&ADCA) {
			Assert(adca_callback);
		} else
#  endif

#  ifdef ADCB
		if ((uintptr_t)adc == (uintptr_t)&ADCB) {
			Assert(adcb_callback);
		} else
#  endif

		{
			Assert(0);
			return;
		}
	}
#endif

	adc_enable_clock(adc);
	adc_ch->CTRL = ch_conf->ctrl;
	adc_ch->INTCTRL = ch_conf->intctrl;
	adc_ch->MUXCTRL = ch_conf->muxctrl;
	if (ch_mask & ADC_CH0) {
		/* USB devices has channel scan available on ADC channel 0 */
		adc_ch->SCAN = ch_conf->scan;
	}
	adc_disable_clock(adc);

	cpu_irq_restore(flags);
}

/**
 * \brief Read configuration from ADC channel
 *
 * Reads out the current configuration from the ADC channel to the specified
 * buffer.
 *
 * \param adc Pointer to ADC module.
 * \param ch_mask Mask of ADC channel(s):
 * \arg \c ADC_CHn , where \c n specifies the channel. (Only a single channel
 * can be given in mask)
 * \param ch_conf Pointer to ADC channel configuration.
 */
void adcch_read_configuration(ADC_t *adc, uint8_t ch_mask,
		struct adc_channel_config *ch_conf)
{
	ADC_CH_t *adc_ch;
	irqflags_t flags;

	adc_ch = adc_get_channel(adc, ch_mask);

	flags = cpu_irq_save();

	adc_enable_clock(adc);
	ch_conf->ctrl = adc_ch->CTRL;
	ch_conf->intctrl = adc_ch->INTCTRL;
	ch_conf->muxctrl = adc_ch->MUXCTRL;
	if (ch_mask & ADC_CH0) {
		/* USB devices has channel scan available on ADC channel 0 */
		ch_conf->scan = adc_ch->SCAN;
	}
	adc_disable_clock(adc);

	cpu_irq_restore(flags);
}

/** @} */

/** @} */
