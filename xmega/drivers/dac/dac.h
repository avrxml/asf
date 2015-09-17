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
#ifndef DAC_H
#define DAC_H

#include <compiler.h>
#include <nvm.h>
#include <sysclk.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup dac_group Digital to Analog Converter (DAC)
 *
 * See \ref dac_quickstart.
 * 
 * This is a driver for versions 1 and 2, introduced with the XMEGA AU family,
 * of the AVR XMEGA DAC. It provides functions for enabling, disabling and
 * configuring the DAC and its channels.
 *
 * Note that functions for configuring refresh interval and conversion timing
 * are not present for version 2 of the DAC, since it does not need those
 * settings. The symbols \ref XMEGA_DAC_VERSION_1 and \ref XMEGA_DAC_VERSION_2
 * can be used to tell, at compile time, which DAC the driver is compiled for.
 *
 * The API functions and definitions can be divided in two groups:
 * - module management: direct access for enabling and disabling the DAC,
 * setting and getting channel values, waiting for conversion completion, etc.
 * - module configuration: create/change configurations and write/read them
 * to/from a DAC.
 *
 * The API makes use of a structure that contains the configuration. This
 * structure must be set up before the configuration is written to a DAC module.
 *
 * After the DAC has been configured, it must be enabled before any conversions
 * can be done or any of its channel values be changed.
 *
 * \pre The functions for creating/changing configurations are not protected
 * against interrupts. The functions that read from or write to the DAC's
 * registers are protected.
 *
 * \section dependencies Dependencies
 * This driver depends on the following modules:
 * - \ref sysclk_group for peripheral clock control.
 * - \ref interrupt_group for disabling interrupts during critical code
 * sections.
 * - \ref sleepmgr_group for setting allowed sleep mode.
 *
 * \note If both DAC channels are active, and one of them is run at maximum
 * conversion rate, automatic refresh can fail since the triggered conversions
 * have higher priority than the refresh.
 *
 * @{
 */

#if XMEGA_AU || XMEGA_B || XMEGA_E
#  define XMEGA_DAC_VERSION_2 true
#else
#  define XMEGA_DAC_VERSION_1 true
#endif

//! DAC configuration
struct dac_config {
	uint8_t ctrla;
	uint8_t ctrlb;
	uint8_t ctrlc;
	uint8_t evctrl;
#if XMEGA_DAC_VERSION_1
	uint8_t timctrl;
#endif
};

//! DAC reference settings
enum dac_reference {
	//! 1 V from internal bandgap reference.
	DAC_REF_BANDGAP = DAC_REFSEL_INT1V_gc,
	//! Analog VCC as reference.
	DAC_REF_AVCC    = DAC_REFSEL_AVCC_gc,
	//! External reference on AREF pin on port A.
	DAC_REF_AREFA   = DAC_REFSEL_AREFA_gc,
	//! External reference on AREF pin on port B.
	DAC_REF_AREFB   = DAC_REFSEL_AREFB_gc,
};

//! DAC value adjust settings
enum dac_adjust {
	DAC_ADJ_RIGHT,                     //!< Right-adjusted value.
	DAC_ADJ_LEFT  = DAC_LEFTADJ_bm,    //!< Left-adjusted value.
};

//! \name DAC channel masks
//@{

#define DAC_CH0    (1 << 0)                //!< DAC channel 0.
#define DAC_CH1    (1 << 1)                //!< DAC channel 1.

//@}

//! \name Calibration data addresses
//@{
#if XMEGA_DAC_VERSION_1

#  if defined(DACA)
//! DAC A, offset calibration.
#    define DACAOFFCAL     offsetof(NVM_PROD_SIGNATURES_t, DACAOFFCAL)
//! DAC A, gain calibration.
#    define DACAGAINCAL    offsetof(NVM_PROD_SIGNATURES_t, DACAGAINCAL)
#  endif

#  if defined(DACB)
//! DAC B, offset calibration.
#    define DACBOFFCAL     offsetof(NVM_PROD_SIGNATURES_t, DACBOFFCAL)
//! DAC B, gain calibration.
#    define DACBGAINCAL    offsetof(NVM_PROD_SIGNATURES_t, DACBGAINCAL)
#  endif

#elif XMEGA_DAC_VERSION_2

//! DAC A Channel 0, offset calibration.
#  define DACA0OFFCAL     offsetof(NVM_PROD_SIGNATURES_t, DACA0OFFCAL)
//! DAC A Channel 0, gain calibration.
#  define DACA0GAINCAL    offsetof(NVM_PROD_SIGNATURES_t, DACA0GAINCAL)
//! DAC A Channel 1, offset calibration.
#  define DACA1OFFCAL     offsetof(NVM_PROD_SIGNATURES_t, DACA1OFFCAL)
//! DAC A Channel 1, gain calibration.
#  define DACA1GAINCAL    offsetof(NVM_PROD_SIGNATURES_t, DACA1GAINCAL)

//! DAC B Channel 0, offset calibration.
#  define DACB0OFFCAL     offsetof(NVM_PROD_SIGNATURES_t, DACB0OFFCAL)
//! DAC B Channel 0, gain calibration.
#  define DACB0GAINCAL    offsetof(NVM_PROD_SIGNATURES_t, DACB0GAINCAL)
//! DAC B Channel 1, offset calibration.
#  define DACB1OFFCAL     offsetof(NVM_PROD_SIGNATURES_t, DACB1OFFCAL)
//! DAC B Channel 1, gain calibration.
#  define DACB1GAINCAL    offsetof(NVM_PROD_SIGNATURES_t, DACB1GAINCAL)

#endif
//@}

#if XMEGA_DAC_VERSION_1

//! DAC calibration data
enum dac_calibration_data {
#  if defined(DACA)
	DAC_CAL_DACA_OFFSET = DACAOFFCAL,    //!< DAC A offset calibration data.
	DAC_CAL_DACA_GAIN   = DACAGAINCAL,   //!< DAC A gain calibration data.
#  endif
#  if defined(DACB)
	DAC_CAL_DACB_OFFSET = DACBOFFCAL,    //!< DAC B offset calibration data.
	DAC_CAL_DACB_GAIN   = DACBGAINCAL,   //!< DAC B gain calibration data.
#  endif
};

#elif XMEGA_DAC_VERSION_2

//! DAC calibration data
enum dac_calibration_data {
#  if defined(DACA)
	DAC_CAL_DACA0_OFFSET = DACA0OFFCAL,    //!< DAC A Channel 0 offset calibration data.
	DAC_CAL_DACA0_GAIN   = DACA0GAINCAL,   //!< DAC A Channel 0 gain calibration data.
	DAC_CAL_DACA1_OFFSET = DACA1OFFCAL,    //!< DAC A Channel 1 offset calibration data.
	DAC_CAL_DACA1_GAIN   = DACA1GAINCAL,   //!< DAC A Channel 1 gain calibration data.
#  endif
#  if defined(DACB)
	DAC_CAL_DACB0_OFFSET = DACB0OFFCAL,    //!< DAC B Channel 0 offset calibration data.
	DAC_CAL_DACB0_GAIN   = DACB0GAINCAL,   //!< DAC B Channel 0 gain calibration data.
	DAC_CAL_DACB1_OFFSET = DACB1OFFCAL,    //!< DAC B Channel 1 offset calibration data.
	DAC_CAL_DACB1_GAIN   = DACB1GAINCAL,   //!< DAC B Channel 1 gain calibration data.
#  endif
};

#endif

//! \name DAC module and channel management
//@{

void dac_enable(DAC_t *dac);
void dac_disable(DAC_t *dac);
bool dac_is_enabled(DAC_t *dac);

/**
 * \brief Get calibration data
 *
 * Returns the specified calibration data from the production signature row.
 *
 * \param cal Identifier for calibration data to get.
 *
 * \return Byte with specified calibration data.
 */
__always_inline static uint8_t dac_get_calibration_data(
		enum dac_calibration_data cal)
{
	return nvm_read_production_signature_row(cal);
}

/**
 * \brief Set value of DAC channel
 *
 * \param dac Pointer to DAC module.
 * \param ch_mask Mask of DAC channel for which to set the value:
 * \arg \c DAC_CHn , where \c n specifies the channel.
 * \param val 12-bit value to set. (Must be adjusted as configured in the
 * conversion parameters with \ref dac_set_conversion_parameters.)
 *
 * \note This function has no effect if the DAC is not enabled.
 */
__always_inline static void dac_set_channel_value(DAC_t *dac, uint8_t ch_mask,
		uint16_t val)
{
	irqflags_t flags;
	uint16_t   *dac_ch;
	uint8_t    ch;

	Assert(ch_mask);
	ch = ilog2(ch_mask);
	Assert(ch <= 1);
	dac_ch = (uint16_t *)&dac->CH0DATA + ch;

	flags = cpu_irq_save();
	*dac_ch = val;
	cpu_irq_restore(flags);
}

/**
 * \brief Get value of DAC channel
 *
 * \param dac Pointer to DAC module.
 * \param ch_mask Mask of DAC channel from which to retrieve the value:
 * \arg \c DAC_CHn , where \c n specifies the channel.
 *
 * \return Current 12-bit value of the DAC channel.
 */
__always_inline static uint16_t dac_get_channel_value(DAC_t *dac,
		uint8_t ch_mask)
{
	irqflags_t flags;
	uint16_t   *dac_ch;
	uint16_t   val;
	uint8_t    ch;

	Assert(ch_mask);
	ch = ilog2(ch_mask);
	Assert(ch <= 1);
	dac_ch = (uint16_t *)&dac->CH0DATA + ch;

	flags = cpu_irq_save();
	val = *dac_ch;
	cpu_irq_restore(flags);

	return val;
}

/**
 * \brief Check whether DAC channel(s) are ready for data
 *
 * Checks if the data registers for all of the masked DAC channels are ready for
 * a new value. This indicates that a conversion with the current value has been
 * completed.
 *
 * \param dac Pointer to DAC module.
 * \param ch_mask Mask of DAC channel(s) to check whether are ready:
 * \arg \c DAC_CHn , where \c n specifies the channel. (These can be OR'ed
 * together.)
 *
 * \retval true if DAC channels are ready.
 * \retval false if a DAC channel is not ready.
 */
__always_inline static bool dac_channel_is_ready(DAC_t *dac, uint8_t ch_mask)
{
	Assert(dac);

	return (dac->STATUS & ch_mask) == ch_mask;
}

/**
 * \brief Wait for DAC channel(s) to get ready for data
 *
 * Busy-waits for all of the masked DAC channels' data registers to become ready
 * for new values.
 *
 * \param dac Pointer to DAC module.
 * \param ch_mask Mask of DAC channel(s) to wait for data ready:
 * \arg \c DAC_CHn , where \c n specifies the channel. (These can be OR'ed
 * together.)
 */
__always_inline static void dac_wait_for_channel_ready(DAC_t *dac,
		uint8_t ch_mask)
{
	Assert(dac);

	do { } while (!dac_channel_is_ready(dac, ch_mask));
}

//@}

//! \name DAC module and channel configuration
//@{

void dac_write_configuration(DAC_t *dac, struct dac_config *conf);
void dac_read_configuration(DAC_t *dac, struct dac_config *conf);

/**
 * \brief Set active DAC channel(s) and output(s)
 *
 * Enables the masked DAC channels and configures their output. Either of the
 * channels may be configured for internal output, but not both. Implicitly,
 * channels that are not masked for internal output are configured for output
 * on their respective pins.
 *
 * \param conf Pointer to DAC configuration.
 * \param ch_mask Mask of DAC channel(s) to enable:
 * \arg \c DAC_CHn , where \c n specifies the channel. (These can be OR'ed
 * together.)
 * \param int_out_ch_mask Mask of DAC channel to connect to internal output:
 * \arg \c 0 to connect all channels to output pin.
 * \arg \c DAC_CHn , where \c n specifies the channel.
 *
 * \note If both channels of a version 1 DAC are enabled, the refresh rate must
 * also be configured with \ref dac_set_refresh_interval().
 */
__always_inline static void dac_set_active_channel(struct dac_config *conf,
		uint8_t ch_mask, uint8_t int_out_ch_mask)
{
	uint8_t setting;

	Assert(conf);
	Assert(ch_mask);
	Assert(ilog2(ch_mask) <= 2);
	Assert((ch_mask & int_out_ch_mask) == int_out_ch_mask);
	Assert(int_out_ch_mask != (DAC_CH0 | DAC_CH1));

	// Enable internal and external output channels.
	conf->ctrla = ((ch_mask ^ int_out_ch_mask) << DAC_CH0EN_bp) |
			(int_out_ch_mask ? DAC_IDOEN_bm : 0);

	// Enable the specified number of DAC channels.
	if ((ch_mask == DAC_CH0) || (ch_mask == DAC_CH1)) {
		setting = DAC_CHSEL_SINGLE_gc;
	} else {
		setting = DAC_CHSEL_DUAL_gc;
	}
	conf->ctrlb &= ~DAC_CHSEL_gm;
	conf->ctrlb |= setting;
}

// Timing configuration is only needed for version 1 of DAC
#if XMEGA_DAC_VERSION_1 || defined(__DOXYGEN__)

/**
 * \brief Set automatic refresh timing interval
 *
 * Sets or disables the automatic refresh timing interval for dual-channel mode
 * of the DAC. If it is disabled, the DAC channels must be manually refreshed by
 * updating the channel values and triggering new conversions.
 *
 * This interval is rounded downward to the closest setting.
 *
 * \param conf Pointer to DAC configuration.
 * \param us Microseconds between channel refresh:
 * \arg \c 1 to \c 30 to enable.
 * \arg \c 0 to disable.
 *
 * \note The refresh interval should not exceed 30 us.
 */
__always_inline static void dac_set_refresh_interval(struct dac_config *conf,
		uint8_t us)
{
	uint32_t cycles;
	uint8_t  cycles_log2;
	uint8_t  setting;

	Assert(conf);
	Assert(us <= 30);

	// Enabling automatic refresh?
	if (us) {
		// Compute the corresponding number of peripheral clock cycles.
		cycles = sysclk_get_per_hz();
		cycles *= us;
		cycles /= 1000000;

		// Get the integer number of bits for cycle count.
		cycles_log2 = cycles ? ilog2(cycles) : 0;

		// Handle out-of-bounds settings.
		if (cycles_log2 < 4) {
			setting = DAC_REFRESH_16CLK_gc;
		} else if (cycles_log2 > 16) {
			setting = DAC_REFRESH_65536CLK_gc;
		// Handle within-bounds settings.
		} else {
			setting = (cycles_log2 - 4) << DAC_REFRESH_gp;
		}
	// Disabling automatic refresh.
	} else {
		setting = DAC_REFRESH_OFF_gc;
	}
	conf->timctrl &= ~DAC_REFRESH_gm;
	conf->timctrl |= setting;
}

/**
 * \brief Set conversion timing interval
 *
 * Sets the timing interval from a conversion is completed until a new one can
 * be started. This settings depends on whether single- or dual-channel mode is
 * used, and so the channels should be configured before this function is
 * called to ensure the proper configuration.
 *
 * This interval is rounded downward to the closest setting.
 *
 * \param conf Pointer to DAC configuration.
 * \param us Microseconds between conversions:
 * \arg \c 1 to \c 127 for single-channel.
 * \arg \c 2 to \c 127 for dual-channel.
 *
 * \note The conversion interval should not exceed 127.
 */
__always_inline static void dac_set_conversion_interval(struct dac_config *conf,
		uint8_t us)
{
	uint32_t cycles;
	uint8_t  cycles_log2;
	uint8_t  setting;

	Assert(us);
	Assert(us <= 127);

	// Compute the corresponding number of peripheral clock cycles.
	cycles = sysclk_get_per_hz();
	cycles *= us;
	cycles /= 1000000;

	// Compensate for 50% cycle addition for dual-channel mode.
	if ((conf->ctrlb & DAC_CHSEL_gm) == DAC_CHSEL_DUAL_gc) {
		cycles *= 2;
		cycles /= 3;
	}

	// Get the integer number of bits for cycle count.
	cycles_log2 = cycles ? ilog2(cycles) : 0;

	if (cycles_log2 < 7) {
		setting = cycles_log2 << DAC_CONINTVAL_gp;
	} else {
		setting = DAC_CONINTVAL_128CLK_gc;
	}
	conf->timctrl &= ~DAC_CONINTVAL_gm;
	conf->timctrl |= setting;
}

#endif // XMEGA_DAC_VERSION_1

/**
 * \brief Set DA conversion parameters
 *
 * Sets up the voltage reference and channel value adjust, i.e., whether the
 * 12-bit value is shifted to the most significant (left) or least significant
 * (right) bits in the channels' 16-bit data registers.
 *
 * \param conf Pointer to DAC configuration.
 * \param ref DAC voltage reference setting.
 * \param adj DAC value adjust setting.
 */
__always_inline static void dac_set_conversion_parameters(
		struct dac_config *conf, enum dac_reference ref,
		enum dac_adjust adj)
{
	conf->ctrlc = (uint8_t)ref | (uint8_t)adj;
}

/**
 * \brief Set DA conversions triggers
 *
 * Sets up event-triggering of conversions on the masked channels, and manual
 * triggering on the others. Conversions are manually triggered by setting new
 * channel values, while events only trigger new conversions if a new channel
 * value was set since the last conversion.
 *
 * \param conf Pointer to DAC configuration.
 * \param ev_ch Event channel to use.
 * \param ch_mask Mask of DAC channel(s) to use event-triggering on:
 * \arg \c DAC_CHn , where \c n specifies the channel. (These can be OR'ed
 * together.)
 * \arg \c 0 to disable.
 *
 * \note The specified event channel is common to all DAC channels on which
 * event-triggering of conversions is configured.
 */
__always_inline static void dac_set_conversion_trigger(struct dac_config *conf,
		uint8_t ch_mask, uint8_t ev_ch)
{
	Assert(ev_ch <= 7);

	conf->ctrlb &= ~(DAC_CH1TRIG_bm | DAC_CH0TRIG_bm);
	conf->ctrlb |= (ch_mask << DAC_CH0TRIG_bp);
	conf->evctrl = ev_ch << DAC_EVSEL_gp;
}

//@}

//! @}

/**
 * \page dac_quickstart Quick start guide for XMEGA DAC
 *
 * This is the quick start guide for the \ref dac_group, with
 * step-by-step instructions on how to configure and use the driver in a
 * selection of use cases.
 *
 * The use cases contain several code fragments. The code fragments in the
 * steps for setup can be copied into a custom initialization function, while
 * the steps for usage can be copied into, e.g., the main application function.
 *
 * \section dac_use_cases DAC use cases
 * - \ref dac_basic_use_case
 * - \subpage dac_use_case_1
 *
 * \section dac_basic_use_case Basic use case - DA conversion on one channel
 * In this use case, the DAC is configured for:
 * - Using DAC on port B, channel 0
 * - 1V from bandgap as reference, left adjusted channel value
 * - one active DAC channel, no internal output
 * - conversions triggered by event channel 0
 * - 1 us conversion intervals
 *
 * This use case sets up the DAC to perform DA conversions on one channel,
 * with output to a pin, every time it receives events in event channel 0.
 * The output pin may, e.g., be connected to an oscilloscope or an amplifier
 * and speaker.
 *
 * \section dac_basic_use_case_setup Setup steps
 *
 * \subsection dac_basic_use_case_setup_prereq Prerequisites
 * -# \ref sysclk_group "System Clock Management"
 * -# \ref tc_group "Timer/Counter Driver"
 *
 * \subsection dac_basic_use_case_setup_code Example code
 * The following configuration must be added to the project (typically to a
 * conf_dac.h file, but it can also be added to your main application file):
 * \code
	#define SPEAKER_DAC              DACB
	#define SPEAKER_DAC_CHANNEL      DAC_CH0
	#define RATE_OF_CONVERSION       22050
	#define NR_OF_SAMPLES            32
\endcode
 *
 * A static const sample buffer with one period of a sine wave must be added:
 * \code static const uint16_t sine[NR_OF_SAMPLES] = {
	    32768, 35325, 37784, 40050, 42036, 43666, 44877, 45623,
	    45875, 45623, 44877, 43666, 42036, 40050, 37784, 35325,
	    32768, 30211, 27752, 25486, 23500, 21870, 20659, 19913,
	    19661, 19913, 20659, 21870, 23500, 25486, 27752, 30211,
	};
\endcode
 *
 * Add to application initialization:
 * \code
	 static void evsys_init(void)
	 {
	     sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_EVSYS);
	     EVSYS.CH3MUX = EVSYS_CHMUX_TCC0_OVF_gc;
	 }

	 void tc_init(void)
	 {
	     tc_enable(&TCC0);
	     tc_set_wgm(&TCC0, TC_WG_NORMAL);
	     tc_write_period(&TCC0, (sysclk_get_per_hz() / RATE_OF_CONVERSION) - 1);
	     tc_write_clock_source(&TCC0, TC_CLKSEL_DIV1_gc);	
	 }

	 void dac_init(void)
	 {
	     struct dac_config conf;

	     dac_read_configuration(&SPEAKER_DAC, &conf);
	     dac_set_conversion_parameters(&conf, DAC_REF_BANDGAP, DAC_ADJ_LEFT);
	     dac_set_active_channel(&conf, SPEAKER_DAC_CHANNEL, 0);
	     dac_set_conversion_trigger(&conf, SPEAKER_DAC_CHANNEL, 3);
	     #ifdef XMEGA_DAC_VERSION_1
	     dac_set_conversion_interval(&conf, 1);
	     #endif
	     dac_write_configuration(&SPEAKER_DAC, &conf);
	     dac_enable(&SPEAKER_DAC);
	 }
\endcode
 *
 * Add to \c main():
 * \code
	sysclk_init();
	evsys_init();
	tc_init();
	dac_init();
\endcode
 *
 * \subsection dac_basic_use_case_setup_flow Workflow
 * -# Create a function \c evsys_init() to intialize the event system clocks and
 *    to link the conversion timer to the correct event channel:
 *     - \code
	static void evsys_init(void)
	{
	    // ...
	}
\endcode
 * -# Use the sysclk service to enable the clock to the event system:
 *     - \code sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_EVSYS); \endcode
 * -# Connect the TCC0 overflow event to event channel 3:
 *     - \code EVSYS.CH3MUX = EVSYS_CHMUX_TCC0_OVF_gc; \endcode
 *    \note If the DAC trigger timer is changed from TCC0, the \c EVSYS_CHMUX_*
 *          mask here will also need to be altered.
 * -# Create a function \c tc_init() to intialize the DAC trigger timer:
 *     - \code
	static void tc_init(void)
	{
	    // ...
	}
\endcode
 * -# Enable the clock to the DAC trigger timer:
 *     - \code tc_enable(&TCC0); \endcode
 * -# Configure the DAC trigger timer in normal Waveform Generation mode:
 *     - \code tc_set_wgm(&TCC0, TC_WG_NORMAL); \endcode
 * -# Configure the DAC trigger timer period to overflow at at the specified
 *    rate of conversion in Hz:
 *     - \code tc_write_period(&TCC0, (sysclk_get_per_hz() / RATE_OF_CONVERSION) - 1); \endcode
 * -# Configure the DAC trigger timer clock source to use the peripheral bus
 *    frequency:
 *     - \code tc_write_clock_source(&TCC0, TC_CLKSEL_DIV1_gc); \endcode
 * -# Create a function \c dac_init() to intialize the DAC:
 *     - \code
	static void dac_init(void)
	{
	    // ...
	}
\endcode
 * -# Config struct for the DAC:
 *   - \code struct dac_config conf; \endcode
 * -# Initialize the dac configuration by reading back the configuration from the peripheral:
 *   - \code dac_read_configuration(&SPEAKER_DAC, &conf); \endcode
 * -# Create and set configuration:
 *   - \code
	dac_set_conversion_parameters(&conf, DAC_REF_BANDGAP, DAC_ADJ_LEFT);
	dac_set_active_channel(&conf, SPEAKER_DAC_CHANNEL, 0);
	dac_set_conversion_trigger(&conf, SPEAKER_DAC_CHANNEL, 3);
	#ifdef XMEGA_DAC_VERSION_1
	dac_set_conversion_interval(&conf, 1);
	#endif
	dac_write_configuration(&SPEAKER_DAC, &conf);
\endcode
 *    \note If the DAC trigger event channel is changed from channel 3, the
 *          parameter to dac_set_conversion_trigger() here will also need to be
 *          altered.
 * -# Enable DAC:
 *   - \code dac_enable(&SPEAKER_DAC); \endcode
 * -# Initialize the clock system, event system, DAC trigger timer, and the DAC:
 *     - \code
	sysclk_init();
	evsys_init();
	tc_init();
	dac_init();
\endcode
 *
 * \section dac_basic_use_case_usage Usage steps
 *
 * \subsection dac_basic_use_case_usage_code Example code
 * Add to application C-file:
 * \code
	uint8_t i = 0;
	while (1) {
	    dac_wait_for_channel_ready(&SPEAKER_DAC, SPEAKER_DAC_CHANNEL);
	    dac_set_channel_value(&SPEAKER_DAC, SPEAKER_DAC_CHANNEL, sine[i]);
	    i++;
	    i %= NR_OF_SAMPLES;
	}
\endcode
 *
 * \subsection dac_basic_use_case_usage_flow Workflow
 * -# Create a local variable to hold the current sine wave lookup table index:
 *   - \code uint8_t i = 0; \endcode
 * -# Wait for DAC channel to be ready for new data:
 *   - \code
	dac_wait_for_channel_ready(&SPEAKER_DAC, SPEAKER_DAC_CHANNEL);
\endcode
 * -# Set the current sine wave table element as new channel value:
 *   - \code
	dac_set_channel_value(&SPEAKER_DAC, SPEAKER_DAC_CHANNEL, sine[i]);
\endcode
 * \note Conversion is triggered by the timer/counter
 * -# Get next index in sine wave array to convert, wrapping around at NR_OF_SAMPLES:
 *   - \code
	i++;
	i %= NR_OF_SAMPLES;
\endcode
 */

/**
 * \page dac_use_case_1 DA conversions on two channels
 *
 * In this use case, the DAC module is configured for:
 * - AVCC as reference, right adjusted channel value
 * - Both DAC channels active, no internal output
 * - Manually triggered conversions on both channels
 * - 2 us conversion intervals
 * - 10 us refresh intervals
 *
 * This use case configures the DAC to perform DA conversions on both channels,
 * with output to their respective pins every time it receives an event on event
 * channel 0.
 * \section dac_use_case_1_setup Setup steps
 *
 * \subsection dac_use_case_1_setup_prereq Prerequisites
 * -# \ref sysclk_group "System Clock Management"
 * -# \ref tc_group "Timer/Counter Driver"
 *
 * \subsection dac_use_case_1_setup_code Example code
 * -# The following configuration must be added to the project (typically to a
 * conf_dac.h file, but it can also be added to your main application file.):
 * \code
	#define RATE_OF_CONVERSION       500
	#define OUTPUT_DAC               DACA
\endcode
 *
 * -# Add to application initialization:
 * \code
	 void tc_init(void)
	 {
	     tc_enable(&TCC0);
	     tc_set_wgm(&TCC0, TC_WG_NORMAL);
	     tc_write_period(&TCC0, (sysclk_get_per_hz() / RATE_OF_CONVERSION) - 1);
	     tc_write_clock_source(&TCC0, TC_CLKSEL_DIV1_gc);	
	 }

	 void dac_init(void)
	 {
	     struct dac_config conf;

	     dac_read_configuration(&OUTPUT_DAC, &conf);
	     dac_set_conversion_parameters(&conf, DAC_REF_AVCC, DAC_ADJ_RIGHT);
	     dac_set_active_channel(&conf, DAC_CH0 | DAC_CH1, 0);
	     #ifdef XMEGA_DAC_VERSION_1
	     dac_set_conversion_interval(&conf, 10);
	     dac_set_refresh_interval(&conf, 20);
	     #endif
	     dac_write_configuration(&OUTPUT_DAC, &conf);
	     dac_enable(&OUTPUT_DAC);
	 }
\endcode
 *
 * Add to \c main():
 * \code
	sysclk_init();
	tc_init();
	dac_init();
\endcode
 *
 * \subsection dac_use_case_1_setup_flow Workflow
 * -# Create a function \c tc_init() to intialize the DAC trigger timer:
 *     - \code
	static void tc_init(void)
	{
	    // ...
	}
\endcode
 * -# Enable the clock to the DAC trigger timer:
 *     - \code tc_enable(&TCC0); \endcode
 * -# Configure the DAC trigger timer in normal Waveform Generation mode:
 *     - \code tc_set_wgm(&TCC0, TC_WG_NORMAL); \endcode
 * -# Configure the DAC trigger timer period to overflow at at the specified
 *    rate of conversion in Hz:
 *     - \code tc_write_period(&TCC0, (sysclk_get_per_hz() / RATE_OF_CONVERSION) - 1); \endcode
 * -# Configure the DAC trigger timer clock source to use the peripheral bus
 *    frequency:
 *     - \code tc_write_clock_source(&TCC0, TC_CLKSEL_DIV1_gc); \endcode
 * -# Create a function \c dac_init() to intialize the DAC:
 *     - \code
	static void dac_init(void)
	{
	    // ...
	}
\endcode
 * -# Config struct for the DAC:
 *   - \code struct dac_config conf; \endcode
 * -# Initialize system clock:
 *   - \code sysclk_init(); \endcode
 * -# Initialize the dac configuration by reading back the configuration from the peripheral:
 *   - \code dac_read_configuration(&SPEAKER_DAC, &conf); \endcode
 * -# Create and set configuration:
 *   - \code
	dac_read_configuration(&OUTPUT_DAC, &conf);
	dac_set_conversion_parameters(&conf, DAC_REF_AVCC, DAC_ADJ_RIGHT);
	dac_set_active_channel(&conf, DAC_CH0 | DAC_CH1, 0);
	#ifdef XMEGA_DAC_VERSION_1
	dac_set_conversion_interval(&conf, 10);
	dac_set_refresh_interval(&conf, 20);
	#endif
	dac_write_configuration(&OUTPUT_DAC, &conf);
	dac_enable(&OUTPUT_DAC);
\endcode
 * -# Enable DAC:
 *   - \code dac_enable(&SPEAKER_DAC); \endcode
 * -# Initialize the clock system, event system, DAC trigger timer, and the DAC:
 *     - \code
	sysclk_init();
	tc_init();
	dac_init();
\endcode
 *
 * \section dac_use_case_1_usage Usage steps
 *
 * \subsection dac_use_case_1_usage_code Example code
 * Add to, e.g., main loop in application C-file:
 * \code
	 while (!(tc_is_overflow(&TCC0))) {};
	 tc_clear_overflow(&TCC0);

	 dac_set_channel_value(&OUTPUT_DAC, DAC_CH0, 410);
	 dac_set_channel_value(&OUTPUT_DAC, DAC_CH1, 3686);

	 while (!(tc_is_overflow(&TCC0))) {};
	 tc_clear_overflow(&TCC0);

	 dac_set_channel_value(&OUTPUT_DAC, DAC_CH0, 3686);
	 dac_set_channel_value(&OUTPUT_DAC, DAC_CH1, 410);
\endcode
 *
 * \subsection dac_use_case_1_usage_flow Workflow
 * -# Wait for channels to get ready for new values, by waiting for the sample
 *    timer to overflow and then clearing the flag:
 *   - \code
	while (!(tc_is_overflow(&TCC0))) {};
	tc_clear_overflow(&TCC0);
\endcode
 * -# Set the value of channel 0 to 10% and the other to 90% of maximum:
 *   - \code
	dac_set_channel_value(&OUTPUT_DAC, DAC_CH0, 410);
	dac_set_channel_value(&OUTPUT_DAC, DAC_CH1, 3686);
\endcode
 * -# Wait for channels to get ready for new values, by waiting for the sample
 *    timer to overflow and then clearing the flag:
 *   - \code
	while (!(tc_is_overflow(&TCC0))) {};
	tc_clear_overflow(&TCC0);
\endcode
 * -# Set the value of channel 0 to 90% and the other to 10% of maximum:
 *   - \code
	dac_set_channel_value(&OUTPUT_DAC, DAC_CH0, 3686);
	dac_set_channel_value(&OUTPUT_DAC, DAC_CH1, 410);
\endcode
 * \note Conversions are triggered by the timer/counter
 */

#ifdef __cplusplus
}
#endif

#endif /* DAC_H */
