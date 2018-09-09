/*****************************************************************************
*
* \file
*
* \brief DACIFB header for AVR32 UC3.
*
* This file defines a useful set of functions for DACIFB on AVR32 devices.
*
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
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
*****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef _DACIFB_H_
#define _DACIFB_H_

/**
 * \defgroup group_avr32_drivers_dacifb DACIFB - DAC Interface B
 *
 * Driver for the Digital to Analog Converter (DAC) Interface B. It provides
 * functions for configuring and using the DAC and its channels.
 *
 * @{
 */

#include <avr32/io.h>
#include "compiler.h"

#if UC3C
#  define AVR32_FLASHC_CALIBRATION_FOURTH_WORD_ADDRESS    0x8080020C
#  define AVR32_FLASHC_CALIBRATION_FIVETH_WORD_ADDRESS    0x80800214
#endif

/** \name Channel Selection used by the DAC */
/** @{ */
#define  DACIFB_CHANNEL_SELECTION_NONE     0x0
#define  DACIFB_CHANNEL_SELECTION_A        0x1
#define  DACIFB_CHANNEL_SELECTION_B        0x2
#define  DACIFB_CHANNEL_SELECTION_AB       0x3
/** @} */

/** \name Reference used by the DAC */
/** @{ */
#define  DACIFB_REFERENCE_EXT              0x0
#define  DACIFB_REFERENCE_VDDANA           0x1
/** @} */

/** \name Trigger Mode used by the DAC */
/** @{ */
#define  DACIFB_TRIGGER_MODE_MANUAL        0x0
#define  DACIFB_TRIGGER_MODE_TIMER         0x1
#define  DACIFB_TRIGGER_MODE_EVENT         0x2
/** @} */

/** CHI Min Value of 2us */
#define  DACIFB_CHI_MIN_VALUE              500000

/** CHRx Min Value of 25us */
#define  DACIFB_CHRx_MIN_VALUE             40000

/** CHRx Max Value of 35us */
#define  DACIFB_CHRx_MAX_VALUE             33333

/** Parameters for the DACIFB. */
typedef struct {
	/** Reference for DAC Conversion */
	bool reference;

	/** Channel Selection */
	uint8_t channel_selection;

	/** Gain Calibration Value */
	uint16_t gain_calibration_value;

	/** Offset Calibration Value */
	uint16_t offset_calibration_value;

	/** Low Power Mode */
	bool low_power;

	/** Dual Mode */
	bool dual;

	/** Prescaler Clock in Hertz (should be > 500000Hz) */
	uint32_t prescaler_clock_hz;
} dacifb_opt_t;

/** Parameters for the configuration of the channel. */
typedef struct {
	/** Auto Refresh Mode. */
	bool auto_refresh_mode;

	/** Trigger Mode: Manual/Timer/Event */
	uint8_t trigger_mode;

	/** Left or Right Adjustment. */
	bool left_adjustment;

	/** Data Shift Value. */
	uint8_t data_shift;

	/** Data Round. */
	bool data_round_enable;
} dacifb_channel_opt_t;

void dacifb_get_calibration_data(volatile avr32_dacifb_t *dacifb,
		dacifb_opt_t *p_dacifb_opt,
		uint8_t instance);

bool dacifb_configure(volatile avr32_dacifb_t *dacifb,
		dacifb_opt_t *p_dacifb_opt,
		uint32_t pb_hz);

bool dacifb_configure_channel(volatile avr32_dacifb_t *dacifb,
		uint8_t channel,
		dacifb_channel_opt_t *p_dacifb_channel_opt,
		uint32_t prescaler_clock_hz);

void dacifb_start_channel(volatile avr32_dacifb_t *dacifb,
		uint8_t channel,
		uint32_t cpu_hz);

bool dacifb_check_eoc(volatile avr32_dacifb_t *dacifb,
		uint8_t channel);

void dacifb_set_value(volatile avr32_dacifb_t *dacifb,
		uint8_t channel,
		bool dual,
		uint32_t value);

void dacifb_reload_timer(volatile avr32_dacifb_t *dacifb,
		uint8_t channel,
		uint8_t timer_us,
		uint32_t prescaler_clock_hz);

/** @} */

#endif  /* _DACIFB_H_ */
