/*****************************************************************************
*
* \file
*
* \brief Resistive Touch driver
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

/** \todo Handle PRR registers, especially shared access to ADC. */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <stdint.h>
#include <stdbool.h>
#include "board.h"
#include "compiler.h"
#include "assert.h"
#include "gpio.h"
#include "intc.h"
//#include "sleepmgr.h"
#include "rtouch.h"
#include "conf_rtouch.h"

#if defined(AVR32_ADCIFA)
# include "adcifa.h"
#else
# include "adc.h"
#endif

/** Driver state names. */
enum rtouch_state_enum {
	RTOUCH_DISABLED,    /**< Driver is disabled, no sampling will occur. */
	RTOUCH_NOT_TOUCHED, /**< Driver enabled, but panel not touched. */
	RTOUCH_READING_XL,  /**< Waiting for XL line sample. */
	RTOUCH_READING_XH,  /**< Waiting for XH line sample. */
	RTOUCH_READING_YL,  /**< Waiting for YL line sample. */
	RTOUCH_READING_YH,  /**< Waiting for YH line sample. */
	RTOUCH_PROCESSING,  /**< Waiting for sample processing. */
	RTOUCH_TOUCHED,     /**< Panel is currently touched. */
};

static struct {
	/** Current driver state. */
	enum rtouch_state_enum state;
	/** Last touch event. */
	rtouch_event_t last_event;

	/** Temporary raw sample values for X position. */
	int32_t rawX;
	/** Temporary raw sample values for Y position. */
	int32_t rawY;

	/** Current event handler, NULL if no handler defined. */
	rtouch_event_handler_t event_handler;

	/** Current calibration matrix. */
	rtouch_calibration_matrix_t *calibration_matrix;
} rtouch;

static const gpio_map_t rtouch_gpio_ymap = {
	{
		.pin      = RTOUCH_YL_PIN,
		.function = RTOUCH_YL_PIN_FUNCTION
	},
	{
		.pin       = RTOUCH_YH_PIN,
		.function = RTOUCH_YH_PIN_FUNCTION
	}
};

static const gpio_map_t rtouch_gpio_xmap = {
	{
		.pin      = RTOUCH_XL_PIN,
		.function = RTOUCH_XL_PIN_FUNCTION
	},
	{
		.pin      = RTOUCH_XH_PIN,
		.function = RTOUCH_XH_PIN_FUNCTION
	}
};

/* Detection interrupt handler prototype */
#if __GNUC__
__attribute__((__interrupt__))
#elif __ICCAVR32__
__interrupt
#endif
void rtouch_detect_int_handler(void);

/* ADC sampling interrupt handler prototype */
#if __GNUC__
__attribute__((__interrupt__))
#elif __ICCAVR32__
__interrupt
#endif
void rtouch_adc_int_handler(void);

/** Called from ADC ISR to compute coordinates from raw samples. */
static void rtouch_process_samples(void);

/** Called from \ref rtouch_process_samples or from timer module to start a new
 *  sample. */
static void rtouch_resample(void);

#ifdef AVR32_ADCIFA

/* ADCIFA IP registers address */
volatile avr32_adcifa_t *adcifa = &AVR32_ADCIFA;

/* ADC Configuration */
adcifa_opt_t adcifa_opt = {
	.frequency                = 1000000,
	.reference_source         = ADCIFA_ADCREF0,
	.sample_and_hold_disable  = false,
	.single_sequencer_mode    = false,
	.free_running_mode_enable = false,
	.sleep_mode_enable        = false,
	.mux_settle_more_time     = false
};

/* Sequencer Configuration */
adcifa_sequencer_opt_t adcifa_sequence_opt = {
	.convnb               = 1,
	.resolution           = ADCIFA_SRES_12B,
	.trigger_selection    = ADCIFA_TRGSEL_SOFT,
	.start_of_conversion  = ADCIFA_SOCB_ALLSEQ,
	.sh_mode              = ADCIFA_SH_MODE_OVERSAMP,
	.half_word_adjustment = ADCIFA_HWLA_NOADJ,
	.software_acknowledge = ADCIFA_SA_NO_EOS_SOFTACK
};

/* Conversions in the Sequencer Configuration */
adcifa_sequencer_conversion_opt_t adcifa_sequence_conversion_opt[1];

#endif

static void inline rtouch_enable_detect_int(void)
{
	/* Enable interrupt for Y lines */
	gpio_enable_pin_interrupt(rtouch_gpio_ymap[0].pin, GPIO_FALLING_EDGE);
}

static void inline rtouch_clear_detect_flag(void)
{
	gpio_clear_pin_interrupt_flag(rtouch_gpio_ymap[0].pin);
}

static void inline rtouch_disable_detect_int(void)
{
	/* Disable interrupt for Y lines. */
	/* Clear the pin interrupt, or otherwise it will trigger when
	 * enabled again */
	gpio_clear_pin_interrupt_flag(rtouch_gpio_ymap[0].pin);
	gpio_disable_pin_interrupt(rtouch_gpio_ymap[0].pin);
}

/** \brief Enable interrupts for the touch channels. */
static void inline rtouch_enable_adc_int(void)
{
#ifdef AVR32_ADCIFA
	volatile avr32_adcifa_t *adcifa = &RTOUCH_ADC;

	/* Enable interrupt for sequencer 0. */
	adcifa->ier = 0x1;
#else
	volatile avr32_adc_t *adc = &RTOUCH_ADC;

	adc->ier = RTOUCH_ADC_XL_CHANNEL | RTOUCH_ADC_YL_CHANNEL
			| RTOUCH_ADC_XH_CHANNEL | RTOUCH_ADC_YH_CHANNEL;
#endif
}

/** \brief Disable interrupts for the touch channels. */
static void inline rtouch_disable_adc_int(void)
{
#ifdef AVR32_ADCIFA
	volatile avr32_adcifa_t *adcifa = &RTOUCH_ADC;

	/* Enable interrupt for sequencer 0. */
	adcifa->idr = 0x1;
#else
	volatile avr32_adc_t *adc = &RTOUCH_ADC;

	adc->idr = RTOUCH_ADC_XL_CHANNEL | RTOUCH_ADC_YL_CHANNEL
			| RTOUCH_ADC_XH_CHANNEL | RTOUCH_ADC_YH_CHANNEL;
#endif
}

/** \brief Test if a touch is detected.
 *
 * Checks if one of the Y lines is pulled low due to a touch on the surface.
 * The Y lines must be pulled up and the X lines must be GND in order to work.
 *
 * \return \c true if a touch is detected and \c false if no touch is detected.
 */
static bool inline rtouch_is_detect(void)
{
	/* Check if one of the Y lines is pulled low */
	if (!gpio_get_pin_value(rtouch_gpio_ymap[0].pin) ||
			!gpio_get_pin_value(rtouch_gpio_ymap[1].pin)) {
		return true;
	} else {
		return false;
	}
}

/** \brief Drive X lines to ground. */
static void inline rtouch_ground_x_surface(void)
{
	/* To avoid a spike due to a Y-surface that is tristated and
	 * X-surface that was gradiented we need to set XH as input first.
	 * Otherwise the voltage on the signal line will rise to VDD. This is
	 * not an issue to the measurement but it helps to let the signal look
	 * nice and thus we perhaps improve the EMI a bit. */
	gpio_enable_gpio_pin(rtouch_gpio_xmap[1].pin);
	gpio_enable_gpio_pin(rtouch_gpio_xmap[0].pin);

	/* set output low */
	gpio_clr_gpio_pin(rtouch_gpio_xmap[1].pin);
	gpio_clr_gpio_pin(rtouch_gpio_xmap[0].pin);
}

/** \brief Drive Y lines to ground. */
static void inline rtouch_ground_y_surface(void)
{
	gpio_enable_gpio_pin(rtouch_gpio_ymap[1].pin);
	gpio_enable_gpio_pin(rtouch_gpio_ymap[0].pin);

	gpio_clr_gpio_pin(rtouch_gpio_ymap[0].pin);
	gpio_clr_gpio_pin(rtouch_gpio_ymap[1].pin);
}

/** \brief Enable pull-ups for Y lines.
 *
 * Used to be able to trigger an interrupt upon a touch.
 */
static void inline rtouch_pullup_y_surface(void)
{
	gpio_enable_gpio_pin(rtouch_gpio_ymap[1].pin);
	gpio_enable_gpio_pin(rtouch_gpio_ymap[0].pin);

	gpio_enable_pin_pull_up(rtouch_gpio_ymap[0].pin);
	gpio_enable_pin_pull_up(rtouch_gpio_ymap[1].pin);
}

/** \brief Drive voltage gradient on X surface (XL=GND, XH=VDD). */
static void inline rtouch_gradient_x_surface(void)
{
	gpio_enable_gpio_pin(rtouch_gpio_xmap[1].pin);
	gpio_enable_gpio_pin(rtouch_gpio_xmap[0].pin);

	gpio_disable_pin_pull_up(rtouch_gpio_xmap[0].pin);
	gpio_disable_pin_pull_up(rtouch_gpio_xmap[1].pin);

	gpio_clr_gpio_pin(rtouch_gpio_xmap[0].pin);
	gpio_set_gpio_pin(rtouch_gpio_xmap[1].pin);
}

/** \brief Drive voltage gradient on Y surface (YL=GND, YH=VDD). */
static void inline rtouch_gradient_y_surface(void)
{
	gpio_enable_gpio_pin(rtouch_gpio_ymap[1].pin);
	gpio_enable_gpio_pin(rtouch_gpio_ymap[0].pin);

	gpio_disable_pin_pull_up(rtouch_gpio_ymap[0].pin);
	gpio_disable_pin_pull_up(rtouch_gpio_ymap[1].pin);

	gpio_clr_gpio_pin(rtouch_gpio_ymap[0].pin);
	gpio_set_gpio_pin(rtouch_gpio_ymap[1].pin);
}

static void inline rtouch_tristate_x_surface(void)
{
	gpio_enable_gpio_pin(rtouch_gpio_xmap[1].pin);
	gpio_enable_gpio_pin(rtouch_gpio_xmap[0].pin);

	gpio_disable_pin_pull_up(rtouch_gpio_xmap[0].pin);
	gpio_disable_pin_pull_up(rtouch_gpio_xmap[1].pin);

	/* Enable ADC to control the pins */
	gpio_enable_module_pin(rtouch_gpio_xmap[0].pin,
			rtouch_gpio_xmap[0].function);
	gpio_enable_module_pin(rtouch_gpio_xmap[1].pin,
			rtouch_gpio_xmap[1].function);
}

static void inline rtouch_tristate_y_surface(void)
{
	gpio_enable_gpio_pin(rtouch_gpio_ymap[1].pin);
	gpio_enable_gpio_pin(rtouch_gpio_ymap[0].pin);

	gpio_disable_pin_pull_up(rtouch_gpio_ymap[0].pin);
	gpio_disable_pin_pull_up(rtouch_gpio_ymap[1].pin);

	/* Enable ADC to control the pins */
	gpio_enable_module_pin(rtouch_gpio_ymap[0].pin,
			rtouch_gpio_ymap[0].function);
	gpio_enable_module_pin(rtouch_gpio_ymap[1].pin,
			rtouch_gpio_ymap[1].function);

}

#ifdef AVR32_ADCIFA
static volatile int s_current_channel;
#endif

static void rtouch_start_read(uint32_t channel)
{
#ifdef AVR32_ADCIFA
	volatile avr32_adcifa_t *adcifa = &RTOUCH_ADC;

	s_current_channel = channel;

	if (channel < 8) {
		adcifa_sequence_conversion_opt[0].channel_p = channel;
		adcifa_sequence_conversion_opt[0].channel_n = AVR32_ADCIFA_INN_GNDANA;
		adcifa_sequence_conversion_opt[0].gain      = ADCIFA_SHG_1;
	} else {
		adcifa_sequence_conversion_opt[0].channel_p = AVR32_ADCIFA_INP_GNDANA;
		adcifa_sequence_conversion_opt[0].channel_n = channel;
		adcifa_sequence_conversion_opt[0].gain      = ADCIFA_SHG_1;
	}

	adcifa_configure_sequencer(adcifa, 0, &adcifa_sequence_opt,
			adcifa_sequence_conversion_opt);

	adcifa_start_sequencer(adcifa, 0);
#else
	volatile avr32_adc_t *adc = &RTOUCH_ADC;

	/* disable all touch channels */
	adc->chdr = RTOUCH_ADC_XL_CHANNEL | RTOUCH_ADC_YL_CHANNEL
			| RTOUCH_ADC_XH_CHANNEL | RTOUCH_ADC_YH_CHANNEL;

	/* enable current touch channel */
	adc->cher = channel;
	adc_start(adc);
#endif
}

/** \brief Return last converted value. */
static uint32_t inline rtouch_get_adc_value(void)
{
#ifdef AVR32_ADCIFA
	uint32_t value;

	if (s_current_channel < 8) {
		value = ADCIFA_read_resx_sequencer_0(0);
	} else {
		value = (~ADCIFA_read_resx_sequencer_0(0));
	}

	return value;

#else
	volatile avr32_adc_t *adc = &RTOUCH_ADC;

	return adc->lcdr;
#endif
}

static void rtouch_clear_adc_flag(void)
{

}

/** \brief  Touch screen low-level detect interrupt handler.
 *
 * This interrupt handler is called when a low level on the YL/YH lines is
 * detected, which means the panel is touched. The driver sets up and starts
 * a sample of the XL line, and defers further handling to the ADC interrupt.
 */
#if __GNUC__
__attribute__((__interrupt__))
#elif __ICCAVR32__
__interrupt
#endif
void rtouch_detect_int_handler(void)
{
	/* Change detection should only fire if previously untouched, or when */
	/* we need a resample while touched. */
	Assert((rtouch.state == RTOUCH_NOT_TOUCHED) ||
			(rtouch.state == RTOUCH_TOUCHED));

	/* Keep change detection disabled until panel is not touched anymore. */
	rtouch_disable_detect_int();

	/* Clear accumulators. */
	rtouch.rawX = 0;
	rtouch.rawY = 0;

	/* Change state and start ADC reading of Y position from XL line. */
	/* ADC interrupt will take over further handling. */
	rtouch.state = RTOUCH_READING_XL;
	rtouch_tristate_x_surface();
	rtouch_gradient_y_surface();

	rtouch_start_read(RTOUCH_ADC_XL_CHANNEL);
	/* SLEEPMGR_Lock( TOUCH_SAMPLE_SLEEP_MODE ); */
}

/** \brief Prepare the rtouch detect function.
 *
 * A touch can be detected by setting the Y lines  (or X lines) as inputs and
 * enabling the internal pull-ups for these pins. The other lines, X in this
 * case are connected to GND. By a touch the Y lines will be pulled low and an
 * interrupt can be triggered on a low level detect.
 *
 * This function does the registering of the interrupt handler for the touch
 * detect. Currently we use a GPIO interrupt, but here it is only possible to
 * trigger on edges but actually we should trigger on a low level. Anyway this
 * should work too.
 * Here we use the YL line as source input for the edge detection but it is
 * also possible to use YH.
 */
static void inline rtouch_prepare_detect(void)
{
	Disable_global_interrupt();
	INTC_register_interrupt(&rtouch_detect_int_handler,
			AVR32_GPIO_IRQ_0 + rtouch_gpio_ymap[0].pin / 8, 0);
	Enable_global_interrupt();
}

/** \brief  ADC conversion complete interrupt handler.
 *
 * This interrupt handler is called repeatedly as multiple samples are taken
 * from all touch panel lines in proper order. When all samples have been
 * taken, the ISR defers further handling to the TOUCH_ProcessSamples()
 * function.
 */
#if __GNUC__
__attribute__((__interrupt__))
#elif __ICCAVR32__
__interrupt
#endif
void rtouch_adc_int_handler(void)
{
	static uint8_t sample_count = 0;

#ifdef AVR32_ADCIFA
	volatile avr32_adcifa_t *adcifa = &RTOUCH_ADC;
	adcifa->scr = 0x1;
#endif

	switch (rtouch.state) {
	case RTOUCH_READING_XL:
		/* Store the raw sample value for later. */
		rtouch.rawY += rtouch_get_adc_value();

		/* Change state and start ADC reading of XH */
		/* when enough samples have been taken. */
		++sample_count;
		if (sample_count < RTOUCH_OVERSAMPLING) {
			rtouch_start_read(RTOUCH_ADC_XL_CHANNEL);
		} else {
			sample_count = 0;

			rtouch.state = RTOUCH_READING_XH;
			rtouch_start_read(RTOUCH_ADC_XH_CHANNEL);
		}

		break;

	case RTOUCH_READING_XH:
		/* Add XL and XH samples to form a raw Y position reading. */
		rtouch.rawY += rtouch_get_adc_value();

		/* Change state and start ADC reading of X position from YL line */
		/* when enough samples have been taken. */
		++sample_count;
		if (sample_count < RTOUCH_OVERSAMPLING) {
			rtouch_start_read(RTOUCH_ADC_XH_CHANNEL);
		} else {
			sample_count = 0;

			rtouch.state = RTOUCH_READING_YL;
			rtouch_tristate_y_surface();
			rtouch_gradient_x_surface();
			rtouch_start_read(RTOUCH_ADC_YL_CHANNEL);
		}

		break;

	case RTOUCH_READING_YL:
		/* Store the raw sample for later. */
		rtouch.rawX += rtouch_get_adc_value();

		/* Change state and start ADC reading of YH */
		/* when enough samples have been taken. */
		++sample_count;
		if (sample_count < RTOUCH_OVERSAMPLING) {
			rtouch_start_read(RTOUCH_ADC_YL_CHANNEL);
		} else {
			sample_count = 0;

			rtouch.state = RTOUCH_READING_YH;
			rtouch_start_read(RTOUCH_ADC_YH_CHANNEL);
		}

		break;

	case RTOUCH_READING_YH:
		/* Add YL and YH samples to form a raw X reading. */
		rtouch.rawX += rtouch_get_adc_value();

		/* Configure for detect again */
		/* when enough samples have been taken. */
		++sample_count;
		if (sample_count < RTOUCH_OVERSAMPLING) {
			rtouch_start_read(RTOUCH_ADC_YH_CHANNEL);
		} else {
			sample_count = 0;

			/* SLEEPMGR_Unlock( TOUCH_SAMPLE_SLEEP_MODE ); */

			/* prepare for touch detection */
			rtouch_ground_x_surface();
			rtouch_pullup_y_surface();

			rtouch.state = RTOUCH_PROCESSING;

			/* The processing of sample or lack of touch is */
			/* deferred to the bottom half of the driver. */
			rtouch_process_samples();
		}

		break;

	default:
		Assert(false); /* Always fail on illegal state. */
		break;
	}
}

/** \brief Prepare ADC for touch measuring.
 *
 * Register the interrupt handler, set sample, hold and startup time.
 */
static void inline rtouch_prepare_adc(void)
{
	Disable_global_interrupt();
	INTC_register_interrupt(&rtouch_adc_int_handler, RTOUCH_ADC_IRQ,
			RTOUCH_ADC_INT_LEVEL);
	Enable_global_interrupt();

#ifdef AVR32_ADCIFA
	volatile avr32_adcifa_t *adcifa = &RTOUCH_ADC;

	/* configure ADCIFA */
	adcifa_configure(adcifa, &adcifa_opt, FOSC0);
#else
	volatile avr32_adc_t *adc = &RTOUCH_ADC;

	adc_configure(adc);

	/* we need to lower the adc clock under 5MHz */
	/* adc_clock = adc_input_clock /((prescaler + 1)*2) */
	adc->mr |= 0x1 << AVR32_ADC_MR_PRESCAL_OFFSET;
#endif
}

/* \breif Initializes the resistive touch screen driver.
 *
 * This function initializes all peripherals required to detect a touch
 * and sample its position on a resistive touch panel. Call this function
 * before using any other features of this driver.
 *
 * The driver is disabled initially, so you must call TOUCH_Enable() before
 * any touch events will be detected.
 */
void rtouch_init(void)
{
	/* Start with no event handler. */
	rtouch.event_handler = NULL;

#ifdef AVR32_ADCIFA
	/* GPIO pin/adc-function map. */
	static const gpio_map_t ADCIFA_GPIO_MAP = {
		{AVR32_ADCREF0_PIN, AVR32_ADCREF0_FUNCTION},
		{AVR32_ADCREFP_PIN, AVR32_ADCREFP_FUNCTION},
		{AVR32_ADCREFN_PIN, AVR32_ADCREFN_FUNCTION}
	};
	gpio_enable_module(ADCIFA_GPIO_MAP, sizeof(ADCIFA_GPIO_MAP) /
			sizeof(ADCIFA_GPIO_MAP[0]));
#endif

	/* Prepare required peripherals. */
	rtouch_prepare_adc();
	rtouch_prepare_detect();

	/* We need to lock the sleep mode, and set a non-disable state for the
	 * driver, since TOUCH_Disable() will unlock the sleep mode, and also
	 * check that the driver is not disabled already. */
	/* SLEEPMGR_Lock( TOUCH_DETECT_SLEEP_MODE ); */

	rtouch.state = RTOUCH_NOT_TOUCHED;
	rtouch.last_event.type = RTOUCH_NO_EVENT;
	rtouch_disable();
}

/** \breif Disables the resistive touch screen driver.
 *
 * This function disables the driver, stopping all touch detection and
 * sampling. This will save power.
 */
void rtouch_disable(void)
{
	/* Do not try to disable if already disabled. That will mess up the
	 * sleep manager lock state. */
	if (rtouch.state == RTOUCH_DISABLED) {
		return;
	}

	/* Shutdown driver. */
	rtouch_disable_detect_int();
	rtouch_disable_adc_int();

	/* Continue shutdown. */
	rtouch.state = RTOUCH_DISABLED;

	/* SLEEPMGR_Unlock( TOUCH_DETECT_SLEEP_MODE ); */

	/* Ground touch panel to save power. */
	rtouch_ground_x_surface();
	rtouch_ground_y_surface();
}

/** \brief Enables touch detection of the driver once initialized.
 *
 * This function enables the driver, resuming touch detecting and sampling.
 */
void rtouch_enable(void)
{
	/* Do not try to enable if already enabled. That will mess up the
	 * sleep manager lock state. */
	if (rtouch.state != RTOUCH_DISABLED) {
		return;
	}

	/* Setup panel for touch detection. */
	rtouch_ground_x_surface();
	rtouch_pullup_y_surface();

	/* Initial state is untouched, but if touched, the change detect ISR
	 * will fire immediately. */
	rtouch.state = RTOUCH_NOT_TOUCHED;
	rtouch.last_event.type = RTOUCH_NO_EVENT;

	/* Start driver. */
	/* SLEEPMGR_Lock( TOUCH_DETECT_SLEEP_MODE ); */
	rtouch_clear_adc_flag();
	rtouch_enable_adc_int();

	rtouch_clear_detect_flag();
	rtouch_enable_detect_int();
}

/** \brief Retrieve the last touch event.
 *
 * This function will copy the last event information to a struct. The caller
 * is responsible for providing memory.
 *
 * \param  event  Pointer to the struct where event will be copied.
 */
void rtouch_get_event(rtouch_event_t *event)
{
	*event = rtouch.last_event;
}

/** \brief Determine if the screen is currently being touched.
 *
 * This function returns true if the touch panel is currently touched.
 *
 * \return \c true if panel touched.
 */
bool rtouch_is_touched(void)
{
	bool is_touched = (rtouch.last_event.type == RTOUCH_PRESS) ||
			(rtouch.last_event.type == RTOUCH_MOVE);

	return is_touched;
}

/** \brief Compute a raw coordinate to screen coordinate calibration matrix.
 *
 * This function computes a calibration matrix from a set of three calibration
 * points provided by the caller. Use the raw sample values from the event
 * struct when filling data into the calibration point struct. The calibration
 * matrix will be copied into the struct also provided by the user. Use the
 * \ref rtouch_SetCalibrationMatrix() function to assign a calibration matrix
 * to the driver.
 *
 * \param  points  Pointer to a calibration point set.
 * \param  matrix  Pointer to the struct where the matrix will be copied.
 */
void rtouch_compute_calibration_matrix(
		rtouch_calibration_points_t const *points,
		rtouch_calibration_matrix_t *matrix )
{
	/* Reference: http://www.embedded.com/story/OEG20020529S0046 */

	/* Local copies of touch readings. */
	int32_t Xr0 = points->point1.rawX;
	int32_t Yr0 = points->point1.rawY;
	int32_t Xr1 = points->point2.rawX;
	int32_t Yr1 = points->point2.rawY;
	int32_t Xr2 = points->point3.rawX;
	int32_t Yr2 = points->point3.rawY;

	/* Local copies of display coordinates. */
	int32_t Xp0 = points->point1.panelX;
	int32_t Yp0 = points->point1.panelY;
	int32_t Xp1 = points->point2.panelX;
	int32_t Yp1 = points->point2.panelY;
	int32_t Xp2 = points->point3.panelX;
	int32_t Yp2 = points->point3.panelY;

	/* Compute coefficients for X calibration. */
	matrix->A = ((Xp0 - Xp2) * (Yr1 - Yr2)) - ((Xp1 - Xp2) * (Yr0 - Yr2));
	matrix->B = ((Xr0 - Xr2) * (Xp1 - Xp2)) - ((Xp0 - Xp2) * (Xr1 - Xr2));
	matrix->C =
			Yr0 * ((Xr2 * Xp1) - (Xr1 * Xp2)) +
			Yr1 * ((Xr0 * Xp2) - (Xr2 * Xp0)) +
			Yr2 * ((Xr1 * Xp0) - (Xr0 * Xp1));

	/* Compute coefficients for X calibration. */
	matrix->D = ((Yp0 - Yp2) * (Yr1 - Yr2)) - ((Yp1 - Yp2) * (Yr0 - Yr2));
	matrix->E = ((Xr0 - Xr2) * (Yp1 - Yp2)) - ((Yp0 - Yp2) * (Xr1 - Xr2));
	matrix->F =
			Yr0 * ((Xr2 * Yp1) - (Xr1 * Yp2)) +
			Yr1 * ((Xr0 * Yp2) - (Xr2 * Yp0)) +
			Yr2 * ((Xr1 * Yp0) - (Xr0 * Yp1));

	/* Compute common denominator. */
	matrix->K = ((Xr0 - Xr2) * (Yr1 - Yr2)) - ((Xr1 - Xr2) * (Yr0 - Yr2));
}

/** \brief Set the touch driver calibration matrix.
 *
 * This function assigns a calibration matrix to the driver. The calibration
 * matrix must be retained in RAM while the driver is used.
 *
 * \param  source  Pointer to matrix data.
 */
void rtouch_set_calibration_matrix(rtouch_calibration_matrix_t const *source )
{
	rtouch.calibration_matrix = (rtouch_calibration_matrix_t *)source;
}

/** \brief Gets the currently set touch driver calibration matrix.
 *
 * This function retrieves the current calibration matrix from the driver.
 * Use this function to store a calibration matrix to e.g. EEPROM.
 *
 * \param  destination  Pointer to struct where matrix will be copied.
 */
void rtouch_get_calibration_matrix(rtouch_calibration_matrix_t *destination )
{
	*destination = *rtouch.calibration_matrix;
}

/** \brief Set the touch screen event handler callback function.
 *
 * This function sets a new touch event handler. The handler is called whenever
 * the position or state of the touch screen changes.
 *
 * \param  handler  Pointer to new event handler, or \c NULL to disable handler.
 */
void rtouch_set_event_handler(rtouch_event_handler_t handler)
{
	rtouch.event_handler = handler;
}

/** \brief Gets the currently set touch screen event handler callback function.
 *
 * This function returns the old event handler. Use this to store the old
 * handler while replacing it for a short while, e.g. for calibration.
 *
 * \return  Current event handler pointer.
 */
rtouch_event_handler_t rtouch_get_event_handler(void)
{
	return rtouch.event_handler;
}

/**
 * This function takes care of the final processing and calibration of the
 * raw panel sample values, or lack of touch. This function is called by the
 * ADC interrupt handler.
 *
 * This function will also automatically start a resample of the panel, either
 * immediately or delayed via the timer module, depending on the
 * TOUCH_USE_IMMEDIATE_RESAMPLE setting from "config_touch.h".
 */
static void rtouch_process_samples( void )
{
	/* Catch divide-by-zero, in case matrix is not initialized.
	 * The calibrated values will be unusable anyway, so setting tempK to 1
	 * is ok. */
	int32_t tempK = rtouch.calibration_matrix->K;
	if (tempK == 0) {
		tempK = 1;
	}

	/* Scale sample values down to 10 bit, to avoid matrix computing
	 * overflow. The extra shift compensates for XL/XH and YL/YH being added
	 * together. */
	rtouch.rawX >>= (RTOUCH_SAMPLESCALE + 1);
	rtouch.rawY >>= (RTOUCH_SAMPLESCALE + 1);

	/* Compute calibrated X position. */
	int32_t panelX
		= (rtouch.calibration_matrix->A * rtouch.rawX) +
			(rtouch.calibration_matrix->B * rtouch.rawY) +
			rtouch.calibration_matrix->C;
	panelX /= tempK;

	/* Compute calibrated Y position. */
	int32_t panelY
		= (rtouch.calibration_matrix->D * rtouch.rawX) +
			(rtouch.calibration_matrix->E * rtouch.rawY) +
			rtouch.calibration_matrix->F;
	panelY /= tempK;

	/* Finished processing, so state is now "touched". */
	rtouch.state = RTOUCH_TOUCHED;

	/* If both YL and YH are high, the Y surface is not in contact with the
	 * X surface anymore, so we change state. This will happen if the release
	 * action happened while we computed the panel coordinates. */
	rtouch_clear_detect_flag();
	if (rtouch_is_detect() == false) {
		rtouch.state = RTOUCH_NOT_TOUCHED;

		/* If the last event was "press" or "move", */
		/* we now have to send a "release" event. */
		if ((rtouch.last_event.type == RTOUCH_PRESS) ||
				(rtouch.last_event.type == RTOUCH_MOVE)) {
			rtouch.last_event.type = RTOUCH_RELEASE;

			/* Call event handler if registered. */
			if (rtouch.event_handler != NULL) {
				rtouch.event_handler( &rtouch.last_event );
			}
		}

		/* Enable detect ISR so that the next touch will be detected. */
		rtouch_enable_detect_int();
	} else {
		/* Any change since last time we sampled? Just touched? Perhaps
		 * only moved? */
		bool sendEvent = false;

		/* If it _was_ released or no event at all, it's now touched. */
		if ((rtouch.last_event.type == RTOUCH_NO_EVENT) ||
				(rtouch.last_event.type == RTOUCH_RELEASE)) {
			rtouch.last_event.type = RTOUCH_PRESS;
			sendEvent = true;
		}
		/* If this was not a new touch, was the "touch" moved then? */
		else if ((rtouch.last_event.panelX != panelX) ||
				(rtouch.last_event.panelY != panelY)) {
			rtouch.last_event.type = RTOUCH_MOVE;
			sendEvent = true;
		}

		/* Update coordinates and call event handler if something new
		 * happened. */
		if (sendEvent) {
			rtouch.last_event.rawX = rtouch.rawX;
			rtouch.last_event.rawY = rtouch.rawY;
			rtouch.last_event.panelX = panelX;
			rtouch.last_event.panelY = panelY;

			/* Call event handler if registered. */
			if (rtouch.event_handler != NULL) {
				rtouch.event_handler( &rtouch.last_event );
			}
		}

#if (TOUCH_USE_IMMEDIATE_RESAMPLE == 1)
		/* Now, start an immediate resample, to keep up to date */
		/* with touch position. */
		rtouch_resample();
#else
#  error "Delayed resample not implemented yet."
#endif
	}
}

/**
 * This function starts a new sampling of the touch panel. It will be
 * called by the driver repeatedly as long as the touch panel stays touched.
 */
static void rtouch_resample( void )
{
	/* Resampling should only occur when panel is touched. */
	Assert(rtouch.state == RTOUCH_TOUCHED );

	/* If both YL and YH are high, the Y surface is not in contact with
	 * the X surface anymore, so we change state. */
	rtouch_clear_detect_flag();
	if (rtouch_is_detect() == false) {
		/* Update state. */
		rtouch.state = RTOUCH_NOT_TOUCHED;
		rtouch.last_event.type = RTOUCH_RELEASE;

		/* Call event handler, if registered. */
		if (rtouch.event_handler != NULL) {
			rtouch.event_handler( &rtouch.last_event );
		}

		rtouch_enable_detect_int();
		return;
	}

	/* Clear accumulators. */
	rtouch.rawX = 0;
	rtouch.rawY = 0;

	/* Change state and start ADC reading of Y position from XL line.
	 * The ADC interrupt will take over further handling. */
	rtouch.state = RTOUCH_READING_XL;
	rtouch_gradient_y_surface();
	rtouch_tristate_x_surface();

	rtouch_start_read(RTOUCH_ADC_XL_CHANNEL);
}
