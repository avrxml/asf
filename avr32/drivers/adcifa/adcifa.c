/*****************************************************************************
*
* \file
*
* \brief ADCIFA driver for AVR UC3.
*
* This file defines a useful set of functions for ADC on AVR UC3 devices.
*
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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
*****************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <avr32/io.h>
#include "compiler.h"
#include "adcifa.h"

static bool adcifa_seq1_configured = false;

/** \brief Start Calibration offset procedure.
 *  Calibration procedure algorithm. It returns offset value.
 *
 * \param adcifa       Base address of the ADCIFA
 * \param p_adcifa_opt Structure for the ADCIFA core configuration
 * \param pb_hz        Peripheral Bus frequency
 */
#define CALIBRATION_ADCIFA_NUMBER_OF_SEQUENCE              8

void adcifa_calibrate_offset(volatile avr32_adcifa_t *adcifa,
	adcifa_opt_t *p_adcifa_opt, uint32_t pb_hz){

	/* Sequencer Configuration */
	adcifa_sequencer_opt_t adcifa_sequence_opt;
	adcifa_sequencer_conversion_opt_t
	adcifa_sequence_conversion_opt[CALIBRATION_ADCIFA_NUMBER_OF_SEQUENCE];

	/* Configure the ADC */
	p_adcifa_opt->sample_and_hold_disable  = true;
	p_adcifa_opt->single_sequencer_mode    = true;
	p_adcifa_opt->free_running_mode_enable = false;
	p_adcifa_opt->sleep_mode_enable        = false;
	p_adcifa_opt->mux_settle_more_time     = false;

	/* Clear offset calibration value before starting calibration */
	p_adcifa_opt->offset_calibration_value = 0;

	/* Configure ADCIFA core */
	adcifa_configure(&AVR32_ADCIFA, p_adcifa_opt, pb_hz);

	/* ADCIFA sequencer 0 configuration structure*/
	adcifa_sequence_opt.convnb               =
		CALIBRATION_ADCIFA_NUMBER_OF_SEQUENCE;
	adcifa_sequence_opt.resolution           =
		ADCIFA_SRES_12B;
	adcifa_sequence_opt.trigger_selection    =
		ADCIFA_TRGSEL_SOFT;
	adcifa_sequence_opt.start_of_conversion  =
		ADCIFA_SOCB_ALLSEQ;
	adcifa_sequence_opt.sh_mode              =
		ADCIFA_SH_MODE_OVERSAMP;
	adcifa_sequence_opt.half_word_adjustment =
		ADCIFA_HWLA_NOADJ;
	adcifa_sequence_opt.software_acknowledge =
		ADCIFA_SA_NO_EOS_SOFTACK;

	/* ADCIFA conversions for sequencer 0*/
	for (uint8_t i=0; i<CALIBRATION_ADCIFA_NUMBER_OF_SEQUENCE; i++){
		adcifa_sequence_conversion_opt[i].channel_p = AVR32_ADCIFA_INP_GNDANA;
		adcifa_sequence_conversion_opt[i].channel_n = AVR32_ADCIFA_INN_GNDANA;
		adcifa_sequence_conversion_opt[i].gain = ADCIFA_SHG_1;
	}
	/* Configure ADCIFA sequencer 0 */
	adcifa_configure_sequencer(&AVR32_ADCIFA, 0, &adcifa_sequence_opt,
			adcifa_sequence_conversion_opt);

	/* Start ADCIFA sequencer 0 */
	adcifa_start_sequencer(&AVR32_ADCIFA, 0);

	/* Wait end of ADCIFA sequencer 0*/
	while(!ADCIFA_is_eos_sequencer_0());

	/* The last converted value is the offset value */
	p_adcifa_opt->offset_calibration_value =
		- ADCIFA_read_resx_sequencer_0(CALIBRATION_ADCIFA_NUMBER_OF_SEQUENCE-1);

}

/** \brief Get ADCIFA Calibration Data.
 *
 * Mandatory to call if factory calibration data are wanted to be used.
 * If not called, Calibration Data should be set by the application.
 *
 * \param adcifa       Base address of the ADCIFA
 * \param p_adcifa_opt Structure for the ADCIFA core configuration
 */
void adcifa_get_calibration_data(volatile avr32_adcifa_t *adcifa,
		adcifa_opt_t *p_adcifa_opt)
{
	/* Get Offset Calibration */
	int32_t adc_ocal
		= ((*(volatile signed int *)(AVR32_FLASHC_FACTORY_PAGE_ADDRESS
			+ AVR32_FLASHC_FROW_OCAL_WORD)) &
			AVR32_FLASHC_FROW_OCAL_MASK) >>
			AVR32_FLASHC_FROW_OCAL_OFFSET;

	/* Get Gain Calibration */
	int32_t adc_gcal
		= ((*(volatile signed int *)(AVR32_FLASHC_FACTORY_PAGE_ADDRESS
			+ AVR32_FLASHC_FROW_GCAL_WORD)) &
			AVR32_FLASHC_FROW_GCAL_MASK) >>
			AVR32_FLASHC_FROW_GCAL_OFFSET;

	/*  Get S/H Calibration */
	int32_t adc_gain0
		= ((*(volatile signed int *)(AVR32_FLASHC_FACTORY_PAGE_ADDRESS
			+ AVR32_FLASHC_FROW_GAIN0_WORD)) &
			AVR32_FLASHC_FROW_GAIN0_MASK) >>
			AVR32_FLASHC_FROW_GAIN0_OFFSET;

	int32_t adc_gain1
		= ((*(volatile signed int *)(AVR32_FLASHC_FACTORY_PAGE_ADDRESS
			+ AVR32_FLASHC_FROW_GAIN1_WORD)) &
			AVR32_FLASHC_FROW_GAIN1_MASK) >>
			AVR32_FLASHC_FROW_GAIN1_OFFSET;

	/* Get ADC Offset Calibration */
	p_adcifa_opt->offset_calibration_value = adc_ocal;

	/* Get ADC Gain Calibration */
	p_adcifa_opt->gain_calibration_value = adc_gcal;

	/* Get Sample & Hold Gain Calibration for Seq 0 */
	p_adcifa_opt->sh0_calibration_value = adc_gain0;

	/* Get Sample & Hold Gain Calibration for Seq 1 */
	p_adcifa_opt->sh1_calibration_value = adc_gain1;
}

/** \brief Configure ADCIFA. Mandatory to call.
 *
 * If not called, ADC channels will have side effects
 *
 * \param adcifa       Base address of the ADCIFA
 * \param p_adcifa_opt Structure for the ADCIFA core configuration
 * \param pb_hz        Peripheral Bus frequency
 *
 * \return ADCIFA_CONFIGURATION_REFUSED or ADCIFA_CONFIGURATION_ACCEPTED
 */
uint8_t adcifa_configure(volatile avr32_adcifa_t *adcifa,
		adcifa_opt_t *p_adcifa_opt,
		uint32_t pb_hz)
{
	/* Sanity Check */
	Assert(adcifa != NULL);

	/* Set the ADC configuration */
	AVR32_ADCIFA.cfg
		= (p_adcifa_opt->sleep_mode_enable <<
			AVR32_ADCIFA_CFG_SLEEP)
			| (p_adcifa_opt->single_sequencer_mode <<
			AVR32_ADCIFA_CFG_SSMQ)
			| (p_adcifa_opt->free_running_mode_enable <<
			AVR32_ADCIFA_CFG_FRM)
			| (p_adcifa_opt->reference_source <<
			AVR32_ADCIFA_CFG_RS)
			| (p_adcifa_opt->sample_and_hold_disable <<
			AVR32_ADCIFA_CFG_SHD)
			| (p_adcifa_opt->mux_settle_more_time <<
			AVR32_ADCIFA_CFG_MUXSET)
			| (((ADCIFA_START_UP_TIME *
			(p_adcifa_opt->frequency /
			1000)) / 32000) << AVR32_ADCIFA_CFG_SUT);

	/* Configure Clock  (rounded up) */
	adcifa->ckdiv
		= (((pb_hz /
			(2 * p_adcifa_opt->frequency)) - 1) <<
			AVR32_ADCIFA_CKDIV_CNT_OFFSET) &
			AVR32_ADCIFA_CKDIV_CNT_MASK;

	/* Set ADC Offset Calibration */
	ADCIFA_set_offset_calibration(p_adcifa_opt->offset_calibration_value);

	/* Set ADC Gain Calibration */
	ADCIFA_set_gain_calibration(p_adcifa_opt->gain_calibration_value);

	/* Set Sample & Hold Gain Calibration for seq 0 */
	ADCIFA_set_sh0_gain_calibration(p_adcifa_opt->sh0_calibration_value);

	/* Set Sample & Hold Gain Calibration for seq 1 */
	ADCIFA_set_sh1_gain_calibration(p_adcifa_opt->sh1_calibration_value);

	/* Enable ADCIFA */
	ADCIFA_enable();

	/* Wait Startup Time */
	while (1) {
		if (ADCIFA_is_startup_time()) {
			break;
		}
	}

	return ADCIFA_CONFIGURATION_ACCEPTED;
}

/** \brief Configure ADCIFA specific sequencer.
 *         - Sequence, Gain and Mux
 * \param adcifa                  Base address of the ADCIFA
 * \param sequencer               Sequencer index
 * \param p_adcifa_sequencer_opt  Structure for the sequencer configuration
 * \param p_adcifa_sequencer_conversion_opt  Pointer on a buffer for each
 *                                           conversion on a sequencer
 * \return ADCIFA_CONFIGURATION_REFUSED or ADCIFA_CONFIGURATION_ACCEPTED
 */
uint8_t adcifa_configure_sequencer(volatile avr32_adcifa_t *adcifa,
		uint8_t sequencer,
		adcifa_sequencer_opt_t *p_adcifa_sequencer_opt,
		adcifa_sequencer_conversion_opt_t *p_adcifa_sequencer_conversion_opt)
{
	uint8_t g[16] = {0};
	uint8_t mp[16] = {0};
	uint8_t mn[16] = {0};
	uint8_t i;

	/* Sanity Check */
	Assert( adcifa != NULL );

	/* Switch case with sequencer */
	switch (sequencer) {
	/* Sequencer 0 */
	case 0:
		/* Configure Sequencer 0 */
		ADCIFA_configure_sequencer_0(
				(p_adcifa_sequencer_opt->convnb - 1),
				(p_adcifa_sequencer_opt->resolution),
				(p_adcifa_sequencer_opt->trigger_selection),
				(p_adcifa_sequencer_opt->start_of_conversion),
#if (defined AVR32_ADCIFA_100_H_INCLUDED)
				(p_adcifa_sequencer_opt->oversampling),
#else
				(p_adcifa_sequencer_opt->sh_mode),
#endif
				(p_adcifa_sequencer_opt->half_word_adjustment),
				(p_adcifa_sequencer_opt->software_acknowledge));
		/* Configure Gain for Sequencer 0 */
		for (i = 0; i < p_adcifa_sequencer_opt->convnb; i++) {
			g[i] = p_adcifa_sequencer_conversion_opt[i].gain;
		}
		ADCIFA_configure_sh0gain(g[7], g[6], g[5], g[4], g[3], g[2],
				g[1], g[0]);

		/* Configure Mux for Sequencer 0 */
		for (i = 0; i < p_adcifa_sequencer_opt->convnb; i++) {
			mp[i] = p_adcifa_sequencer_conversion_opt[i].channel_p;
			mn[i] = p_adcifa_sequencer_conversion_opt[i].channel_n;
		}
		ADCIFA_configure_muxsel0p(mp[7], mp[6], mp[5], mp[4], mp[3],
				mp[2], mp[1], mp[0]);
		ADCIFA_configure_muxsel0n(mn[7], mn[6], mn[5], mn[4], mn[3],
				mn[2], mn[1], mn[0]);

		/*
		 * This adcifa_seq1_configured variable check if muxsel1p,muxsel1n 
		 * registers are already configured. This may happen if this function is
		 * called first with sequencer 1 as argument and then called with 
		 * sequencer 0 as argument in case dual mode is used.
		 */
		if(!adcifa_seq1_configured)
		{
			/*
			 * Configure gain for sequencer 0 (in single sequencer mode for 
			 * conversions 9 to 16)
			 */ 
			ADCIFA_configure_sh1gain(g[15], g[14], g[13], g[12], g[11], g[10], 
				g[9], g[8]);

			/*
			 * Configure Mux for sequencer 0 (in single sequencer mode for 
			 * conversions 9 to 16)
			 */
			ADCIFA_configure_muxsel1p(mp[15], mp[14], mp[13], mp[12], mp[11], 
				mp[10], mp[9], mp[8]);
			ADCIFA_configure_muxsel1n(mn[15], mn[14], mn[13], mn[12], mn[11], 
				mn[10], mn[9], mn[8]);

		}

		break;

	/* Sequencer 1 */
	case 1:
		/* Configure Sequencer 1 */
		ADCIFA_configure_sequencer_1(
				(p_adcifa_sequencer_opt->convnb - 1),
				(p_adcifa_sequencer_opt->resolution),
				(p_adcifa_sequencer_opt->trigger_selection),
				(p_adcifa_sequencer_opt->start_of_conversion),
#if (defined AVR32_ADCIFA_100_H_INCLUDED)
				(p_adcifa_sequencer_opt->oversampling),
#else
				(p_adcifa_sequencer_opt->sh_mode),
#endif
				(p_adcifa_sequencer_opt->half_word_adjustment),
				(p_adcifa_sequencer_opt->software_acknowledge));

		/* Configure Gain for Sequencer 1 */
		for (i = 0; i < p_adcifa_sequencer_opt->convnb; i++) {
			g[i] = p_adcifa_sequencer_conversion_opt[i].gain;
		}
		ADCIFA_configure_sh1gain(g[7], g[6], g[5], g[4], g[3], g[2],
				g[1], g[0]);

		/* Configure Mux for Sequencer 1 */
		for (i = 0; i < p_adcifa_sequencer_opt->convnb; i++) {
			mp[i] = p_adcifa_sequencer_conversion_opt[i].channel_p;
			mn[i] = p_adcifa_sequencer_conversion_opt[i].channel_n;
		}
		ADCIFA_configure_muxsel1p(mp[7], mp[6], mp[5], mp[4], mp[3],
				mp[2], mp[1], mp[0]);
		ADCIFA_configure_muxsel1n(mn[7], mn[6], mn[5], mn[4], mn[3],
				mn[2], mn[1], mn[0]);

		adcifa_seq1_configured = true;

		break;

	default:
		break;
	}
	return ADCIFA_CONFIGURATION_ACCEPTED;
}

/** \brief Configure ADCIFA specific window monitor.
 * \param adcifa                  Base address of the ADCIFA
 * \param sequencer               Sequencer index
 * \param adc_window_monitor_opt  Structure for the sequencer configuration
 */
void adcifa_configure_window_monitor( volatile avr32_adcifa_t *adcifa,
		uint8_t sequencer,
		adcifa_window_monitor_opt_t *adc_window_monitor_opt )
{
	switch (sequencer) {
		int temp;

	case 0:
		/* Configure sequencer 0 */
		temp = adcifa->imr & AVR32_ADCIFA_WM0_MASK;
		adcifa->idr = temp;         /* disable interrupt if it was
		                             * enabled */
		adcifa->wcfg0
			= (adc_window_monitor_opt->source_index <<
				AVR32_ADCIFA_WCFG0_SRC_OFFSET)
				| (adc_window_monitor_opt->high_threshold <<
				AVR32_ADCIFA_WCFG0_HT_OFFSET)
				| (adc_window_monitor_opt->low_threshold <<
				AVR32_ADCIFA_WCFG0_LT_OFFSET);
		/* Clear any pending interrupt before setting new mode */
		adcifa->scr = AVR32_ADCIFA_WM0_MASK;
		adcifa->wcfg0
			|= (adc_window_monitor_opt->mode <<
				AVR32_ADCIFA_WCFG0_WM_OFFSET);
		adcifa->ier = temp;         /* enable interrupt if it was
		                             * enabled */
		break;

	case 1:
		/* Configure sequencer 1 */
		temp = adcifa->imr & AVR32_ADCIFA_WM1_MASK;
		adcifa->idr = temp;         /* disable interrupt if it was
		                             * enabled */
		adcifa->wcfg1
			= (adc_window_monitor_opt->source_index <<
				AVR32_ADCIFA_WCFG1_SRC_OFFSET)
				| (adc_window_monitor_opt->high_threshold <<
				AVR32_ADCIFA_WCFG1_HT_OFFSET)
				| (adc_window_monitor_opt->low_threshold <<
				AVR32_ADCIFA_WCFG1_LT_OFFSET);
		/* Clear any pending interrupt before setting new mode */
		adcifa->scr = AVR32_ADCIFA_WM1_MASK;
		adcifa->wcfg1
			|= (adc_window_monitor_opt->mode <<
				AVR32_ADCIFA_WCFG1_WM_OFFSET);
		adcifa->ier = temp;         /* enable interrupt if it was
		                             * enabled */
		break;

	default:
		break;
	}
}

/** \brief Start analog to digital conversion for a specific sequencer
 *  \param adcifa     Base address of the ADCIFA
 *  \param sequencer  Sequencer index
 */
void adcifa_start_sequencer(volatile avr32_adcifa_t *adcifa, uint8_t sequencer)
{
	/* Sanity Check */
	Assert( adcifa != NULL );

	/* Switch Sequencer */
	switch (sequencer) {
	case ADCIFA_SEQ0:
		ADCIFA_softsoc_sequencer(AVR32_ADCIFA_CR_SOC0_MASK);
		break;

	case ADCIFA_SEQ1:
		ADCIFA_softsoc_sequencer(AVR32_ADCIFA_CR_SOC1_MASK);
		break;

	case ADCIFA_SEQ0_SEQ1:
		ADCIFA_softsoc_sequencer(
				AVR32_ADCIFA_CR_SOC0_MASK |
				AVR32_ADCIFA_CR_SOC1_MASK);
		break;

	default:
		break;
	}
}

/** \brief Get End of Conversion status bit
 *  \param adcifa     Base address of the ADCIFA
 *  \param sequencer  Sequencer index
 */
bool adcifa_check_eoc(volatile avr32_adcifa_t *adcifa, uint8_t sequencer)
{
	/* Sanity Check */
	Assert( adcifa != NULL );

	/* get SR register : EOC bit for channel */
	switch (sequencer) {
	case 0:
		return ((ADCIFA_is_eoc_sequencer_0()) ? true : false);

	case 1:
		return ((ADCIFA_is_eoc_sequencer_1()) ? true : false);

	default:
		break;
	}
	return false;
}

/** \brief Get End of Conversion status bit
 *  \param adcifa     Base address of the ADCIFA
 *  \param sequencer  Sequencer index
 */
bool adcifa_check_eos(volatile avr32_adcifa_t *adcifa, uint8_t sequencer)
{
	/* Sanity Check */
	Assert( adcifa != NULL );

	/* get SR register : EOS bit for channel */
	switch (sequencer) {
	case 0:
		return ((ADCIFA_is_eos_sequencer_0()) ? true : false);

	case 1:
		return ((ADCIFA_is_eos_sequencer_1()) ? true : false);

	default:
		break;
	}
	return false;
}

/** \brief Get channel values for a specific sequence
 *
 * \param adcifa                  Base address of the ADCIFA
 * \param sequencer               Sequencer index
 * \param p_adcifa_sequencer_opt  Structure for the sequencer configuration
 * \param adcifa_values           Pointer on the converter values
 * \return ADCIFA_STATUS_COMPLETED or ADCIFA_STATUS_NOT_COMPLETED
 */
uint8_t adcifa_get_values_from_sequencer(volatile avr32_adcifa_t *adcifa,
		uint8_t sequencer,
		adcifa_sequencer_opt_t *p_adcifa_sequencer_opt,
		int16_t *adcifa_values)
{
	uint8_t i;

	/* Sanity Check */
	Assert( adcifa != NULL );

	/* wait for end of sequence */
	if (adcifa_check_eos(adcifa, sequencer) != true) {
		return ADCIFA_STATUS_NOT_COMPLETED;
	}

	switch (sequencer) {
	case 0:
		/* Read values from Sequencer 0 */
		for (i = 0; i < p_adcifa_sequencer_opt->convnb; i++) {
			adcifa_values[i] = ADCIFA_read_resx_sequencer_0(i);
		}
		break;

	case 1:
		/* Read values from Sequencer 1 */
		for (i = 0; i < p_adcifa_sequencer_opt->convnb; i++) {
			adcifa_values[i] = ADCIFA_read_resx_sequencer_1(i);
		}
		break;

	default:
		break;
	}

	return ADCIFA_STATUS_COMPLETED;
}

/** \brief Start the ADCIFA timer
 *
 * \param adcifa                           Base address of the ADCIFA
 * \param timer_count                      Fs = Fadc / (timer_counter + 1)
 */
void adcifa_start_itimer(volatile avr32_adcifa_t *adcifa,
		uint32_t timer_count)
{
	adcifa->itimer = timer_count;
	adcifa->cr = AVR32_ADCIFA_CR_TSTART_MASK;
}

/** \brief Stop the ADCIFA timer
 *
 * \param adcifa                             Base address of the ADCIFA
 */
void adcifa_stop_itimer(volatile avr32_adcifa_t *adcifa)
{
	adcifa->cr = AVR32_ADCIFA_CR_TSTOP_MASK;
}

/** \brief Enable any ADCIFA interrupt
 *
 * \param adcifa                             Base address of the ADCIFA
 * \param interrupt_flags                    Interrupt mask
 */
void adcifa_enable_interrupt(volatile avr32_adcifa_t *adcifa,
		uint32_t interrupt_flags)
{
	adcifa->ier = interrupt_flags;   /* Interrupt Enable Register */
}

/** \brief Disable any ADCIFA interrupt
 *
 * \param adcifa                             Base address of the ADCIFA
 * \param interrupt_flags                    Interrupt mask
 */
void adcifa_disable_interrupt(volatile avr32_adcifa_t *adcifa,
		uint32_t interrupt_flags)
{
	adcifa->idr = interrupt_flags;   /* Interrupt Disable Register */
}

/** \brief Clear any ADCIFA interrupt
 *
 * \param *adcifa                             Base address of the ADCIFA
 * \param  interrupt_flags                    Interrupt mask
 */
void adcifa_clear_interrupt(volatile avr32_adcifa_t *adcifa,
		uint32_t interrupt_flags)
{
	adcifa->scr = interrupt_flags;   /* Status Clear Register */
}
