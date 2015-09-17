/*****************************************************************************
*
* \file
*
* \brief DACIFB driver for AVR32 UC3.
*
* This file defines a useful set of functions for DACIFB on AVR32 devices.
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
#include "dacifb.h"
#include "delay.h"

/** \brief Get DACIFB Calibration Data.
 *
 * Mandatory to call if factory calibration data are wanted to be used.
 * If not called, Calibration Data should be set by the application.
 *
 * \param *dacifb       Base address of the DACIFB
 * \param *p_dacifb_opt Structure for the DACIFB core configuration
 * \param instance      DACIFB core instance 0 for DACIFB0 or 1 for DACIFB1
 */
void dacifb_get_calibration_data( volatile avr32_dacifb_t *dacifb,
		dacifb_opt_t *p_dacifb_opt,
		uint8_t instance)
{
	uint32_t *calibration_data_0
		= (uint32_t *)AVR32_FLASHC_CALIBRATION_FOURTH_WORD_ADDRESS;
	uint32_t *calibration_data_1
		= (uint32_t *)AVR32_FLASHC_CALIBRATION_FIVETH_WORD_ADDRESS;

	if (instance == 0) {
		p_dacifb_opt->offset_calibration_value
			= (*calibration_data_0 & 0x000001FF);
		p_dacifb_opt->gain_calibration_value
			= ((*calibration_data_0 >> 16) & 0x000000FF);
	} else {
		p_dacifb_opt->offset_calibration_value
			= ((*calibration_data_1) & 0x000001FF);
		p_dacifb_opt->gain_calibration_value
			= ((*calibration_data_1 >> 16) & 0x000000FF);
	}
}

/** \brief Configure DACIFB.
 *
 * Mandatory to call. If not called, DACIFB channels will have side effects.
 *
 * \param *dacifb        Base address of the DACIFB
 * \param *p_dacifb_opt  Structure for the DACIFB core configuration
 * \param pb_hz          Peripheral Bus frequency
 *
 * \return Boolean true if the module was configured, false otherwise
 */
bool dacifb_configure(volatile avr32_dacifb_t *dacifb,
		dacifb_opt_t *p_dacifb_opt,
		uint32_t pb_hz)
{
	uint32_t prescaler = 0;
	uint32_t counter = 0;

	Assert( dacifb != NULL );

	/* Calibration of DAC */
	dacifb->goc
		= (((p_dacifb_opt->offset_calibration_value <<
			AVR32_DACIFB_GOC_OCR_OFFSET) &
			AVR32_DACIFB_GOC_OCR_MASK) |
			((p_dacifb_opt->gain_calibration_value <<
			AVR32_DACIFB_GOC_GCR_OFFSET) &
			AVR32_DACIFB_GOC_GCR_MASK));

	/* Compute PRESC field */
	prescaler = 31 - clz(pb_hz / p_dacifb_opt->prescaler_clock_hz);

	/* Check PRESC value */
	if (prescaler > (1 << AVR32_DACIFB_TCR_PRESC_SIZE)) {
		return false;
	}

	/* Update prescaler_clock_hz value */
	p_dacifb_opt->prescaler_clock_hz = pb_hz / (1 << prescaler);

	/* Compute CHI field  ( minimum value of 2us) */
	counter = (p_dacifb_opt->prescaler_clock_hz / DACIFB_CHI_MIN_VALUE);

	/* Check CHI value */
	if (counter > (1 << AVR32_DACIFB_TCR_CHI_SIZE)) {
		return false;
	}

	/* Sampling Rate Frequency */
	dacifb->tcr
		|= (((prescaler <<
			AVR32_DACIFB_TCR_PRESC_OFFSET) &
			AVR32_DACIFB_TCR_PRESC_MASK) |
			((counter <<
			AVR32_DACIFB_TCR_CHI_OFFSET) &
			AVR32_DACIFB_TCR_CHI_MASK));

	/* Channel Selection */
	dacifb->cfr
		|= ((p_dacifb_opt->channel_selection <<
			AVR32_DACIFB_CFR_CHC_OFFSET) &
			AVR32_DACIFB_CFR_CHC_MASK);

	/* Reference Selection */
	dacifb->cfr
		|= ((p_dacifb_opt->reference <<
			AVR32_DACIFB_CFR_REF_OFFSET) &
			AVR32_DACIFB_CFR_REF_MASK);

	/* Dual Data */
	dacifb->cfr
		|= ((p_dacifb_opt->dual <<
			AVR32_DACIFB_CFR_DDA_OFFSET) &
			AVR32_DACIFB_CFR_DDA_MASK);

	/* Low Power Mode */
	dacifb->cfr
		|= ((p_dacifb_opt->low_power <<
			AVR32_DACIFB_CFR_LP_OFFSET) & AVR32_DACIFB_CFR_LP_MASK);

	/* Enable DAC */
	dacifb->cr |= AVR32_DACIFB_CR_EN_MASK;

	return true;
}

/** \brief Configure DACIFB specific channel.
 *
 * Sets channel Adjustment, Refresh_time and Trigger Mode settings.
 *
 * \param  *dacifb                Base address of the ADCIFA
 * \param  channel                DACIFB_CHANNEL_SELECTION_NONE /
 *                                DACIFB_CHANNEL_SELECTION_A /
 *                                DACIFB_CHANNEL_SELECTION_B /
 *                                DACIFB_CHANNEL_SELECTION_AB
 * \param  p_dacifb_channel_opt   Structure for the sequencer configuration
 * \param  prescaler_clock_hz     Prescaler Clock in Hertz (should be >
 *                                500000Hz)
 * \return Boolean true if the channel was configured, false otherwise
 */
bool dacifb_configure_channel( volatile avr32_dacifb_t *dacifb,
		uint8_t channel,
		dacifb_channel_opt_t *p_dacifb_channel_opt,
		uint32_t prescaler_clock_hz)
{
	uint32_t prescaler_min = 0;
	uint32_t prescaler_max = 0;
	uint32_t prescaler = 0;

	Assert( dacifb != NULL );

	/* Auto-refresh Mode */
	if (p_dacifb_channel_opt->auto_refresh_mode == true) {
		switch (channel) {
		case DACIFB_CHANNEL_SELECTION_A:
			dacifb->cr |= (AVR32_DACIFB_CR_ARAE_MASK);
			break;

		case DACIFB_CHANNEL_SELECTION_B:
			dacifb->cr |= (AVR32_DACIFB_CR_ARBE_MASK);
			break;

		case DACIFB_CHANNEL_SELECTION_AB:
			dacifb->cr |= (AVR32_DACIFB_CR_ARAE_MASK) |
					(AVR32_DACIFB_CR_ARBE_MASK);
			break;
		}
	}

	/* Compute CHRA/CHRB fields  ( min value of 25us) */
	prescaler_min = 31 - clz(prescaler_clock_hz / DACIFB_CHRx_MIN_VALUE);
	if (prescaler_min > 0) {
		prescaler_min--;
	}

	/* Check CHRA/CHRB fields */
	if ((prescaler_min > (1 << AVR32_DACIFB_TCR_CHRA_SIZE)) ||
			(prescaler_min > (1 << AVR32_DACIFB_TCR_CHRB_SIZE))) {
		return false;
	}

	/* Compute CHRA/CHRB fields  ( max value of 35us) */
	prescaler_max = 31 - clz(prescaler_clock_hz / DACIFB_CHRx_MAX_VALUE);
	if (prescaler_max > 0) {
		prescaler_max--;
	}

	/* Check CHRA/CHRB fields */
	if ((prescaler_max > (1 << AVR32_DACIFB_TCR_CHRA_SIZE)) ||
			(prescaler_max > (1 << AVR32_DACIFB_TCR_CHRB_SIZE))) {
		return false;
	}

	/* Find value for CHRA/CHRB in the range min/max values 25us/35us and
	 * stop if it is not possible. */
	prescaler = 0;
	while ((prescaler << 1) < prescaler_min) {
		prescaler++;
	}

	/* If value found is out of range, configuration refused */
	if ((prescaler << 1) > prescaler_max) {
		return false;
	}

	/* Refresh Time */
	switch (channel) {
	case DACIFB_CHANNEL_SELECTION_A:
		dacifb->tcr
			= ((prescaler <<
				AVR32_DACIFB_TCR_CHRA_OFFSET) &
				AVR32_DACIFB_TCR_CHRA_MASK);
		break;

	case DACIFB_CHANNEL_SELECTION_B:
		dacifb->tcr
			= ((prescaler <<
				AVR32_DACIFB_TCR_CHRB_OFFSET) &
				AVR32_DACIFB_TCR_CHRB_MASK);
		break;

	case DACIFB_CHANNEL_SELECTION_AB:
		dacifb->tcr
			= (((prescaler <<
				AVR32_DACIFB_TCR_CHRA_OFFSET) &
				AVR32_DACIFB_TCR_CHRA_MASK) |
				((prescaler <<
				AVR32_DACIFB_TCR_CHRB_OFFSET) &
				AVR32_DACIFB_TCR_CHRB_MASK));
		break;
	}

	/* Trigger Mode */
	switch (channel) {
	case DACIFB_CHANNEL_SELECTION_A:
		if (p_dacifb_channel_opt->trigger_mode !=
				DACIFB_TRIGGER_MODE_MANUAL) {
			dacifb->cfr |= (AVR32_DACIFB_CFR_AAE_MASK);
		}

		if (p_dacifb_channel_opt->trigger_mode ==
				DACIFB_TRIGGER_MODE_EVENT) {
			dacifb->ecr |= (AVR32_DACIFB_ECR_ESLA_MASK);
		} else if (p_dacifb_channel_opt->trigger_mode ==
				DACIFB_TRIGGER_MODE_TIMER) {
			dacifb->cr |= (AVR32_DACIFB_CR_TRAE_MASK);
		}

		break;

	case DACIFB_CHANNEL_SELECTION_B:
		if (p_dacifb_channel_opt->trigger_mode !=
				DACIFB_TRIGGER_MODE_MANUAL) {
			dacifb->cfr |= (AVR32_DACIFB_CFR_ABE_MASK);
		}

		if (p_dacifb_channel_opt->trigger_mode ==
				DACIFB_TRIGGER_MODE_EVENT) {
			dacifb->ecr |= (AVR32_DACIFB_ECR_ESLB_MASK);
		} else if (p_dacifb_channel_opt->trigger_mode ==
				DACIFB_TRIGGER_MODE_TIMER) {
			dacifb->cr |= (AVR32_DACIFB_CR_TRBE_MASK);
		}

		break;

	case DACIFB_CHANNEL_SELECTION_AB:
		if (p_dacifb_channel_opt->trigger_mode !=
				DACIFB_TRIGGER_MODE_MANUAL) {
			dacifb->cfr |= (AVR32_DACIFB_CFR_AAE_MASK) |
					(AVR32_DACIFB_CFR_ABE_MASK);
		}

		if (p_dacifb_channel_opt->trigger_mode ==
				DACIFB_TRIGGER_MODE_EVENT) {
			dacifb->ecr |= (AVR32_DACIFB_ECR_ESLA_MASK) |
					(AVR32_DACIFB_ECR_ESLB_MASK);
		} else if (p_dacifb_channel_opt->trigger_mode ==
				DACIFB_TRIGGER_MODE_TIMER) {
			dacifb->cr  |= (AVR32_DACIFB_CR_TRAE_MASK) |
					(AVR32_DACIFB_CR_TRBE_MASK);
		}

		break;
	}

	/* Left Adjust/ Shift Value / Data Rounding */
	switch (channel) {
	case DACIFB_CHANNEL_SELECTION_A:
		dacifb->drca
			= (((p_dacifb_channel_opt->left_adjustment <<
				AVR32_DACIFB_DRCA_DSD_OFFSET) &
				AVR32_DACIFB_DRCA_DSD_MASK) |
				((p_dacifb_channel_opt->data_shift <<
				AVR32_DACIFB_DRCA_DSV_OFFSET) &
				AVR32_DACIFB_DRCA_DSV_MASK) |
				((p_dacifb_channel_opt->data_round_enable <<
				AVR32_DACIFB_DRCA_DRN_OFFSET) &
				AVR32_DACIFB_DRCA_DRN_MASK));
		break;

	case DACIFB_CHANNEL_SELECTION_B:
		dacifb->drcb
			= (((p_dacifb_channel_opt->left_adjustment <<
				AVR32_DACIFB_DRCB_DSD_OFFSET) &
				AVR32_DACIFB_DRCB_DSD_MASK) |
				((p_dacifb_channel_opt->data_shift <<
				AVR32_DACIFB_DRCB_DSV_OFFSET) &
				AVR32_DACIFB_DRCB_DSV_MASK) |
				((p_dacifb_channel_opt->data_round_enable <<
				AVR32_DACIFB_DRCB_DRN_OFFSET) &
				AVR32_DACIFB_DRCB_DRN_MASK));
		break;

	case DACIFB_CHANNEL_SELECTION_AB:
		dacifb->drca
			= (((p_dacifb_channel_opt->left_adjustment <<
				AVR32_DACIFB_DRCA_DSD_OFFSET) &
				AVR32_DACIFB_DRCA_DSD_MASK) |
				((p_dacifb_channel_opt->data_shift <<
				AVR32_DACIFB_DRCA_DSV_OFFSET) &
				AVR32_DACIFB_DRCA_DSV_MASK) |
				((p_dacifb_channel_opt->data_round_enable <<
				AVR32_DACIFB_DRCA_DRN_OFFSET) &
				AVR32_DACIFB_DRCA_DRN_MASK));
		dacifb->drcb
			= (((p_dacifb_channel_opt->left_adjustment <<
				AVR32_DACIFB_DRCB_DSD_OFFSET) &
				AVR32_DACIFB_DRCB_DSD_MASK) |
				((p_dacifb_channel_opt->data_shift <<
				AVR32_DACIFB_DRCB_DSV_OFFSET) &
				AVR32_DACIFB_DRCB_DSV_MASK) |
				((p_dacifb_channel_opt->data_round_enable <<
				AVR32_DACIFB_DRCB_DRN_OFFSET) &
				AVR32_DACIFB_DRCB_DRN_MASK));
		break;
	}

	return true;
}

/** \brief Start analog to digital conversion
 * \param *dacifb   Base address of the DACIFB
 * \param  channel  DACIFB_CHANNEL_SELECTION_NONE /
 *                  DACIFB_CHANNEL_SELECTION_A / DACIFB_CHANNEL_SELECTION_B /
 *                  DACIFB_CHANNEL_SELECTION_AB
 * \param  cpu_hz   CPU Clock frequency
 */
void dacifb_start_channel(volatile avr32_dacifb_t *dacifb,
		uint8_t channel,
		uint32_t cpu_hz)
{
	Assert( dacifb != NULL );

	/* Wait 2us */
	delay_us(2);

	switch (channel) {
	case DACIFB_CHANNEL_SELECTION_A:
		dacifb->cr |= (AVR32_DACIFB_CR_AOE_MASK);
		break;

	case DACIFB_CHANNEL_SELECTION_B:
		dacifb->cr |= (AVR32_DACIFB_CR_BOE_MASK);
		break;

	case DACIFB_CHANNEL_SELECTION_AB:
		dacifb->cr
			|= (AVR32_DACIFB_CR_AOE_MASK |
				AVR32_DACIFB_CR_BOE_MASK);
		break;
	}
}

/** \brief Check channel conversion status
 *
 * \param *dacifb    Base address of the DACIFB
 * \param  channel   channel to check (0 to 1)
 * \return Boolean true if conversion not running,  false if conversion running.
 */
bool dacifb_check_eoc(volatile avr32_dacifb_t *dacifb,
		uint8_t channel)
{
	Assert( dacifb != NULL );

	/* Get SR register : EOC bit for channel */
	return (dacifb->sr & (channel << AVR32_DACIFB_SR_DEA_OFFSET));
}

/** \brief Set channel value
 *
 * \param *dacifb    Base address of the DACIFB
 * \param  channel   channel to handle (0 to 1)
 * \param  dual      Dual Mode Selection
 * \param  value     Value to be converted
 */
void dacifb_set_value(volatile avr32_dacifb_t *dacifb,
		uint8_t channel,
		bool dual,
		uint32_t value)
{
	Assert( dacifb != NULL );

	/* Set new conversion value */
	switch (channel) {
	case DACIFB_CHANNEL_SELECTION_A:
		dacifb->dr0 = value;
		break;

	case DACIFB_CHANNEL_SELECTION_B:
		dacifb->dr1 = value;
		break;

	case DACIFB_CHANNEL_SELECTION_AB:
		if (dual == true) {
			dacifb->dr0 = value;
		} else {
			dacifb->dr0
				= (value & 0xFFFF) | ((value & 0xFFFF) << 16);
		}

		break;
	}
}

/** \brief Reload Timer for Automatic Trigger on DAC
 *  \param *dacifb  Base address of the DACIFB
 *  \param channel  DACIFB_CHANNEL_SELECTION_NONE / DACIFB_CHANNEL_SELECTION_A /
 *                  DACIFB_CHANNEL_SELECTION_B / DACIFB_CHANNEL_SELECTION_AB
 * \param  timer_us Timer Value in Microseconds
 * \param  prescaler_clock_hz   Prescaler Clock in Hertz (should be > 500000Hz)
 */
void dacifb_reload_timer(volatile avr32_dacifb_t *dacifb,
		uint8_t channel,
		uint8_t timer_us,
		uint32_t prescaler_clock_hz)
{
	uint32_t timer;

	Assert( dacifb != NULL );

	timer = ((timer_us * prescaler_clock_hz) / 1000000);

	/* Reload Timer Value */
	switch (channel) {
	case DACIFB_CHANNEL_SELECTION_A:
		dacifb->tra = (AVR32_DACIFB_TRA_TRL_MASK) |
				((timer <<
				AVR32_DACIFB_TRA_TCD_OFFSET) &
				AVR32_DACIFB_TRA_TCD_MASK);
		break;

	case DACIFB_CHANNEL_SELECTION_B:
		dacifb->trb = (AVR32_DACIFB_TRB_TRL_MASK) |
				((timer <<
				AVR32_DACIFB_TRB_TCD_OFFSET) &
				AVR32_DACIFB_TRB_TCD_MASK);
		break;

	case DACIFB_CHANNEL_SELECTION_AB:
		dacifb->tra = (AVR32_DACIFB_TRA_TRL_MASK) |
				((timer <<
				AVR32_DACIFB_TRA_TCD_OFFSET) &
				AVR32_DACIFB_TRA_TCD_MASK);
		dacifb->trb = (AVR32_DACIFB_TRB_TRL_MASK) |
				((timer <<
				AVR32_DACIFB_TRB_TCD_OFFSET) &
				AVR32_DACIFB_TRB_TCD_MASK);
		break;
	}
}
