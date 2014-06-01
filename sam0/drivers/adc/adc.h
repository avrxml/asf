/**
 * \file
 *
 * \brief SAM D20/D21/R21 Peripheral Analog-to-Digital Converter Driver
 *
 * Copyright (C) 2012-2014 Atmel Corporation. All rights reserved.
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

#ifndef ADC_H_INCLUDED
#define ADC_H_INCLUDED

/**
 * \defgroup asfdoc_sam0_adc_group SAM D20/D21/R21 Analog to Digital Converter Driver (ADC)
 *
 * This driver for SAM D20/D21/R21 devices provides an interface for the configuration
 * and management of the device's Analog to Digital Converter functionality, for
 * the conversion of analog voltages into a corresponding digital form.
 * The following driver API modes are covered by this manual:
 * - Polled APIs
 * \if ADC_CALLBACK_MODE
 * - Callback APIs
 * \endif
 *
 * The following peripherals are used by this module:
 *
 *  - ADC (Analog to Digital Converter)
 *
 * The outline of this documentation is as follows:
 *  - \ref asfdoc_sam0_adc_prerequisites
 *  - \ref asfdoc_sam0_adc_module_overview
 *  - \ref asfdoc_sam0_adc_special_considerations
 *  - \ref asfdoc_sam0_adc_extra_info
 *  - \ref asfdoc_sam0_adc_examples
 *  - \ref asfdoc_sam0_adc_api_overview
 *
 *
 * \section asfdoc_sam0_adc_prerequisites Prerequisites
 *
 * There are no prerequisites for this module.
 *
 *
 * \section asfdoc_sam0_adc_module_overview Module Overview
 *
 * This driver provides an interface for the Analog-to-Digital conversion
 * functions on the device, to convert analog voltages to a corresponding
 * digital value. The ADC has up to 12-bit resolution, and is capable of
 * converting up to 500k samples per second (ksps).
 *
 * The ADC has a compare function for accurate monitoring of user defined
 * thresholds with minimum software intervention required.
 * The ADC may be configured for 8-, 10- or 12-bit result, reducing the
 * conversion time from 2.0μs for 12-bit to 1.4μs for 8-bit result. ADC
 * conversion results are provided left or right adjusted which eases
 * calculation when the result is represented as a signed integer.
 *
 * The input selection is flexible, and both single-ended and differential
 * measurements can be made. For differential measurements, an optional gain
 * stage is available to increase the dynamic range. In addition, several
 * internal signal inputs are available. The ADC can provide both signed and
 * unsigned results.
 *
 * The ADC measurements can either be started by application software or an
 * incoming event from another peripheral in the device, and both internal and
 * external reference voltages can be selected.
 *
 * \note Internal references will be enabled by the driver, but not disabled.
 *       Any reference not used by the application should be disabled by the application.
 *
 * A simplified block diagram of the ADC can be seen in
 * \ref asfdoc_sam0_adc_module_block_diagram "the figure below".
 *
 * \anchor asfdoc_sam0_adc_module_block_diagram
 * \dot
 * digraph overview {
 * splines = false;
 * rankdir=LR;
 *
 * mux1 [label="Positive input", shape=box];
 * mux2 [label="Negative input", shape=box];
 *
 *
 * mux3 [label="Reference", shape=box];
 *
 * adc [label="ADC", shape=polygon, sides=5, orientation=90, distortion=-0.6, style=filled, fillcolor=darkolivegreen1, height=1, width=1];
 * prescaler [label="PRESCALER", shape=box, style=filled, fillcolor=lightblue];
 *
 * mux1 -> adc;
 * mux2 -> adc;
 * mux3 -> adc:sw;
 * prescaler -> adc;
 *
 * postproc [label="Post processing", shape=box];
 * result [label="RESULT", shape=box, style=filled, fillcolor=lightblue];
 *
 * adc:e -> postproc:w;
 * postproc:e -> result:w;
 *
 * {rank=same; mux1 mux2}
 * {rank=same; prescaler adc}
 *
 * }
 * \enddot
 *
 *
 * \subsection asfdoc_sam0_adc_module_overview_prescaler Sample Clock Prescaler
 * The ADC features a prescaler which enables conversion at lower clock rates
 * than the input Generic Clock to the ADC module. This feature can be used to
 * lower the synchronization time of the digital interface to the ADC module
 * via a high speed Generic Clock frequency, while still allowing the ADC
 * sampling rate to be reduced.
 *
 * \subsection asfdoc_sam0_adc_module_overview_resolution ADC Resolution
 * The ADC supports full 8-bit, 10-bit or 12-bit resolution. Hardware
 * oversampling and decimation can be used to increase the
 * effective resolution at the expense of throughput. Using oversampling and
 * decimation mode the ADC resolution is increased from 12-bits to an effective
 * 13, 14, 15 or 16-bits. In these modes the conversion rate is reduced, as
 * a greater number of samples is used to achieve the increased resolution. The
 * available resolutions and effective conversion rate is listed in
 * \ref asfdoc_sam0_adc_module_conversion_rate "the table below".
 *
 * \anchor asfdoc_sam0_adc_module_conversion_rate
 * <table>
 *	<caption>Effective ADC conversion speed using oversampling</caption>
 *	<tr>
 *		<th>Resolution</th>
 *		<th>Effective conversion rate</th>
 *	</tr>
 *	<tr>
 *		<td>13-bits</td>
 *		<td>Conversion rate divided by 4</td>
 *	</tr>
 *	<tr>
 *		<td>14-bits</td>
 *		<td>Conversion rate divided by 16</td>
 *	</tr>
 *	<tr>
 *		<td>15-bits</td>
 *		<td>Conversion rate divided by 64</td>
 *	</tr>
 *	<tr>
 *		<td>16-bits</td>
 *		<td>Conversion rate divided by 256</td>
 *	</tr>
 * </table>
 *
 * \subsection asfdoc_sam0_adc_module_overview_conversion Conversion Modes
 * ADC conversions can be software triggered on demand by the user application,
 * if continuous sampling is not required. It is also possible to configure the
 * ADC in free-running mode, where new conversions are started as soon as the
 * previous conversion is completed, or configure the ADC to scan across a
 * number of input pins (see \ref asfdoc_sam0_adc_module_overview_pin_scan).
 *
 * \subsection asfdoc_sam0_adc_module_overview_diff_mode Differential and Single-Ended Conversion
 * The ADC has two conversion modes; differential and single-ended. When
 * measuring signals where the positive input pin is always at a higher voltage
 * than the negative input pin, the single-ended conversion mode should be used
 * in order to achieve a full 12-bit output resolution.
 *
 * If however the positive input pin voltage may drop below the negative input
 * pin the signed differential mode should be used.
 *
 * \subsection asfdoc_sam0_adc_module_overview_sample_time Sample Time
 * The sample time for each ADC conversion is configurable as a number of half
 * prescaled ADC clock cycles (depending on the prescaler value), allowing the
 * user application to achieve faster or slower sampling depending on the
 * source impedance of the ADC input channels. For applications with high
 * impedance inputs the sample time can be increased to give the ADC an adequate
 * time to sample and convert the input channel.
 *
 * The resulting sampling time is given by the following equation:
 * \f[
 * t_{SAMPLE} = (sample\_length+1) \times \frac{ADC_{CLK}} {2}
 * \f]
 *
 * \subsection asfdoc_sam0_adc_module_overview_averaging Averaging
 * The ADC can be configured to trade conversion speed for accuracy by averaging
 * multiple samples in hardware. This feature is suitable when operating in
 * noisy conditions.
 *
 * You can specify any number of samples to accumulate (up to 1024) and the
 * divide ratio to use (up to divide by 128). To modify these settings the
 * ADC_RESOLUTION_CUSTOM needs to be set as the resolution. When this is set
 * the number of samples to accumulate and the division ratio can be set by
 * the configuration struct members \ref adc_config.accumulate_samples and
 * \ref adc_config.divide_result When using this mode the ADC result register
 * will be set to be 16-bits wide to accommodate the larger result sizes
 * produced by the accumulator.
 *
 * The effective ADC conversion rate will be reduced by a factor of the number
 * of accumulated samples;
 * however the effective resolution will be increased according to
 * \ref asfdoc_sam0_adc_module_hw_av_resolution "the table below".
 *
 * \anchor asfdoc_sam0_adc_module_hw_av_resolution
 * <table>
 *   <caption>Effective ADC resolution from various hardware averaging modes</caption>
 *   <tr>
 *     <th>Number of Samples</tr>
 *     <th>Final Result</tr>
 *   </tr>
 *   <tr>
 *     <td>1</td>
 *     <td>12-bits</td>
 *   </tr>
 *   <tr>
 *      <td>2</td>
 *      <td>13-bits</td>
 *   </tr>
 *   <tr>
 *      <td>4</td>
 *      <td>14-bits</td>
 *   </tr>
 *   <tr>
 *      <td>8</td>
 *      <td>15-bits</td>
 *   </tr>
 *   <tr>
 *      <td>16</td>
 *      <td>16-bits</td>
 *   </tr>
 *   <tr>
 *      <td>32</td>
 *      <td>16-bits</td>
 *   </tr>
 *   <tr>
 *      <td>64</td>
 *      <td>16-bits</td>
 *   </tr>
 *   <tr>
 *      <td>128</td>
 *      <td>16-bits</td>
 *   </tr>
 *   <tr>
 *      <td>256</td>
 *      <td>16-bits</td>
 *   </tr>
 *   <tr>
 *      <td>512</td>
 *      <td>16-bits</td>
 *   </tr>
 *   <tr>
 *      <td>1024</td>
 *      <td>16-bits</td>
 *   </tr>
 * </table>
 *
 *
 * \subsection asfdoc_sam0_adc_module_overview_offset_corr Offset and Gain Correction
 * Inherent gain and offset errors affect the absolute accuracy of the ADC.
 *
 * The offset error is defined as the deviation of the ADC’s actual transfer
 * function from ideal straight line at zero input voltage.
 *
 * The gain error is defined as the deviation of the last output step's
 * midpoint from the ideal straight line, after compensating for offset error.
 *
 * The offset correction value is subtracted from the converted data before the
 * result is ready. The gain correction value is multiplied with the offset
 * corrected value.
 *
 * The equation for both offset and gain error compensation is shown below:
 * \f[
 * ADC_{RESULT} = (VALUE_{CONV} + CORR_{OFFSET}) \times CORR_{GAIN}
 * \f]
 *
 * When enabled, a given set of offset and gain correction values can be applied
 * to the sampled data in hardware, giving a corrected stream of sample data to
 * the user application at the cost of an increased sample latency.
 *
 * In single conversion, a latency of 13 ADC Generic Clock cycles is added for
 * the final sample result availability. As the correction time is always less
 * than the propagation delay, in free running mode this latency appears only
 * during the first conversion. After the first conversion is complete future
 * conversion results are available at the defined sampling rate.
 *
 * \subsection asfdoc_sam0_adc_module_overview_pin_scan Pin Scan
 * In pin scan mode, the first ADC conversion will begin from the configured
 * positive channel, plus the requested starting offset. When the first
 * conversion is completed, the next conversion will start at the next positive
 * input channel and so on, until all requested pins to scan have been sampled
 * and converted.
 *
 * Pin scanning gives a simple mechanism to sample a large number of physical
 * input channel samples, using a single physical ADC channel.
 *
 * \subsection asfdoc_sam0_adc_module_overview_window_monitor Window Monitor
 * The ADC module window monitor function can be used to automatically compare
 * the conversion result against a preconfigured pair of upper and lower
 * threshold values.
 *
 * The threshold values are evaluated differently, depending on whether
 * differential or single-ended mode is selected. In differential mode, the
 * upper and lower thresholds are evaluated as signed values for the comparison,
 * while in single-ended mode the comparisons are made as a set of unsigned
 * values.
 *
 * The significant bits of the lower window monitor threshold and upper window
 * monitor threshold values are user-configurable, and follow the overall ADC
 * sampling bit precision set when the ADC is configured by the user application.
 * For example, only the eight lower bits of the window threshold values will be
 * compares to the sampled data whilst the ADC is configured in 8-bit mode.
 * In addition, if using differential mode, the 8th bit will be considered as
 * the sign bit even if bit 9 is zero.
 *
 * \subsection asfdoc_sam0_adc_module_overview_events Events
 * Event generation and event actions are configurable in the ADC.
 *
 * The ADC has two actions that can be triggered upon event reception:
 * \li Start conversion
 * \li Flush pipeline and start conversion
 *
 * The ADC can generate two events:
 * \li Window monitor
 * \li Result ready
 *
 * If the event actions are enabled in the configuration, any incoming event
 * will trigger the action.
 *
 * If the window monitor event is enabled, an event will be generated
 * when the configured window condition is detected.
 *
 * If the result ready event is enabled, an event will be generated when a
 * conversion is completed.
 *
 * \note The connection of events between modules requires the use of the
 *       \ref asfdoc_sam0_events_group "SAM D20/D21/R21 Event System Driver (EVENTS)"
 *       to route output event of one module to the the input event of another.
 *       For more information on event routing, refer to the event driver
 *       documentation.
 *
 *
 * \section asfdoc_sam0_adc_special_considerations Special Considerations
 *
 * An integrated analog temperature sensor is available for use with the ADC.
 * The bandgap voltage, as well as the scaled IO and core voltages can also be
 * measured by the ADC. For internal ADC inputs, the internal source(s) may need
 * to be manually enabled by the user application before they can be measured.
 *
 *
 * \section asfdoc_sam0_adc_extra_info Extra Information
 *
 * For extra information see \ref asfdoc_sam0_adc_extra. This includes:
 *  - \ref asfdoc_sam0_adc_extra_acronyms
 *  - \ref asfdoc_sam0_adc_extra_dependencies
 *  - \ref asfdoc_sam0_adc_extra_errata
 *  - \ref asfdoc_sam0_adc_extra_history
 *
 *
 * \section asfdoc_sam0_adc_examples Examples
 *
 * For a list of examples related to this driver, see
 * \ref asfdoc_sam0_adc_exqsg.
 *
 *
 * \section asfdoc_sam0_adc_api_overview API Overview
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <compiler.h>
#include <system.h>

#if ADC_CALLBACK_MODE == true
#  include <system_interrupt.h>

#if !defined(__DOXYGEN__)
extern struct adc_module *_adc_instances[ADC_INST_NUM];
#endif

/** Forward definition of the device instance */
struct adc_module;

/** Type of the callback functions */
typedef void (*adc_callback_t)(const struct adc_module *const module);

/**
 * \brief ADC Callback enum
 *
 * Callback types for ADC callback driver
 *
 */
enum adc_callback {
	/** Callback for buffer received */
	ADC_CALLBACK_READ_BUFFER,
	/** Callback when window is hit */
	ADC_CALLBACK_WINDOW,
	/** Callback for error */
	ADC_CALLBACK_ERROR,
#  if !defined(__DOXYGEN__)
	/** Number of available callbacks. */
	ADC_CALLBACK_N,
#  endif
};

#endif

/**
 * \name Module status flags
 *
 * ADC status flags, returned by \ref adc_get_status() and cleared by
 * \ref adc_clear_status().
 *
 * @{
 */

/** ADC result ready */
#define ADC_STATUS_RESULT_READY  (1UL << 0)
/** Window monitor match */
#define ADC_STATUS_WINDOW        (1UL << 1)
/** ADC result overwritten before read */
#define ADC_STATUS_OVERRUN       (1UL << 2)

/** @} */

/**
 * \brief ADC reference voltage enum
 *
 * Enum for the possible reference voltages for the ADC.
 *
 */
enum adc_reference {
	/** 1.0V voltage reference */
	ADC_REFERENCE_INT1V   = ADC_REFCTRL_REFSEL_INT1V,
	/** 1/1.48 VCC reference */
	ADC_REFERENCE_INTVCC0 = ADC_REFCTRL_REFSEL_INTVCC0,
	/** 1/2 VCC (only for internal Vcc > 2.1v) */
	ADC_REFERENCE_INTVCC1 = ADC_REFCTRL_REFSEL_INTVCC1,
	/** External reference A */
	ADC_REFERENCE_AREFA   = ADC_REFCTRL_REFSEL_AREFA,
	/** External reference B */
	ADC_REFERENCE_AREFB   = ADC_REFCTRL_REFSEL_AREFB,
};

/**
 * \brief ADC clock prescaler enum
 *
 * Enum for the possible clock prescaler values for the ADC.
 *
 */
enum adc_clock_prescaler {
	/** ADC clock division factor 4 */
	ADC_CLOCK_PRESCALER_DIV4   = ADC_CTRLB_PRESCALER_DIV4,
	/** ADC clock division factor 8 */
	ADC_CLOCK_PRESCALER_DIV8   = ADC_CTRLB_PRESCALER_DIV8,
	/** ADC clock division factor 16 */
	ADC_CLOCK_PRESCALER_DIV16  = ADC_CTRLB_PRESCALER_DIV16,
	/** ADC clock division factor 32 */
	ADC_CLOCK_PRESCALER_DIV32  = ADC_CTRLB_PRESCALER_DIV32,
	/** ADC clock division factor 64 */
	ADC_CLOCK_PRESCALER_DIV64  = ADC_CTRLB_PRESCALER_DIV64,
	/** ADC clock division factor 128 */
	ADC_CLOCK_PRESCALER_DIV128 = ADC_CTRLB_PRESCALER_DIV128,
	/** ADC clock division factor 256 */
	ADC_CLOCK_PRESCALER_DIV256 = ADC_CTRLB_PRESCALER_DIV256,
	/** ADC clock division factor 512 */
	ADC_CLOCK_PRESCALER_DIV512 = ADC_CTRLB_PRESCALER_DIV512,
};

/**
 * \brief ADC resolution enum
 *
 * Enum for the possible resolution values for the ADC.
 *
 */
enum adc_resolution {
	/** ADC 12-bit resolution */
	ADC_RESOLUTION_12BIT = ADC_CTRLB_RESSEL_12BIT,
	/** ADC 16-bit resolution using oversampling and decimation */
	ADC_RESOLUTION_16BIT = ADC_CTRLB_RESSEL_16BIT,
	/** ADC 10-bit resolution */
	ADC_RESOLUTION_10BIT = ADC_CTRLB_RESSEL_10BIT,
	/** ADC 8-bit resolution */
	ADC_RESOLUTION_8BIT  = ADC_CTRLB_RESSEL_8BIT,
	/** ADC 13-bit resolution using oversampling and decimation */
	ADC_RESOLUTION_13BIT,
	/** ADC 14-bit resolution using oversampling and decimation */
	ADC_RESOLUTION_14BIT,
	/** ADC 15-bit resolution using oversampling and decimation */
	ADC_RESOLUTION_15BIT,
	/** ADC 16-bit result register for use with averaging. When using this mode
	  * the ADC result register will be set to 16-bit wide, and the number of
	  * samples to accumulate and the division factor is configured by the
	  * \ref adc_config.accumulate_samples and \ref adc_config.divide_result
	  * members in the configuration struct
	  */
	ADC_RESOLUTION_CUSTOM,
};

/**
 * \brief ADC window monitor mode enum
 *
 * Enum for the possible window monitor modes for the ADC.
 *
 */
enum adc_window_mode {
	/** No window mode */
	ADC_WINDOW_MODE_DISABLE          = ADC_WINCTRL_WINMODE_DISABLE,
	/** RESULT > WINLT */
	ADC_WINDOW_MODE_ABOVE_LOWER      = ADC_WINCTRL_WINMODE_MODE1,
	/** RESULT < WINUT */
	ADC_WINDOW_MODE_BELOW_UPPER      = ADC_WINCTRL_WINMODE_MODE2,
	/** WINLT < RESULT < WINUT */
	ADC_WINDOW_MODE_BETWEEN          = ADC_WINCTRL_WINMODE_MODE3,
	/** !(WINLT < RESULT < WINUT) */
	ADC_WINDOW_MODE_BETWEEN_INVERTED = ADC_WINCTRL_WINMODE_MODE4,
};

/**
 * \brief ADC gain factor selection enum
 *
 * Enum for the possible gain factor values for the ADC.
 *
 */
enum adc_gain_factor {
	/** 1x gain */
	ADC_GAIN_FACTOR_1X   = ADC_INPUTCTRL_GAIN_1X,
	/** 2x gain */
	ADC_GAIN_FACTOR_2X   = ADC_INPUTCTRL_GAIN_2X,
	/** 4x gain */
	ADC_GAIN_FACTOR_4X   = ADC_INPUTCTRL_GAIN_4X,
	/** 8x gain */
	ADC_GAIN_FACTOR_8X   = ADC_INPUTCTRL_GAIN_8X,
	/** 16x gain */
	ADC_GAIN_FACTOR_16X  = ADC_INPUTCTRL_GAIN_16X,
	/** 1/2x gain */
	ADC_GAIN_FACTOR_DIV2 = ADC_INPUTCTRL_GAIN_DIV2,
};

/**
 * \brief ADC event action enum
 *
 * Enum for the possible actions to take on an incoming event.
 *
 */
enum adc_event_action {
	/** Event action disabled */
	ADC_EVENT_ACTION_DISABLED         = 0,
	/** Flush ADC and start conversion */
	ADC_EVENT_ACTION_FLUSH_START_CONV = ADC_EVCTRL_SYNCEI,
	/** Start conversion */
	ADC_EVENT_ACTION_START_CONV       = ADC_EVCTRL_STARTEI,
};

/**
 * \brief ADC positive MUX input selection enum
 *
 * Enum for the possible positive MUX input selections for the ADC.
 *
 */
enum adc_positive_input {
	/** ADC0 pin */
	ADC_POSITIVE_INPUT_PIN0          = ADC_INPUTCTRL_MUXPOS_PIN0,
	/** ADC1 pin */
	ADC_POSITIVE_INPUT_PIN1          = ADC_INPUTCTRL_MUXPOS_PIN1,
	/** ADC2 pin */
	ADC_POSITIVE_INPUT_PIN2          = ADC_INPUTCTRL_MUXPOS_PIN2,
	/** ADC3 pin */
	ADC_POSITIVE_INPUT_PIN3          = ADC_INPUTCTRL_MUXPOS_PIN3,
	/** ADC4 pin */
	ADC_POSITIVE_INPUT_PIN4          = ADC_INPUTCTRL_MUXPOS_PIN4,
	/** ADC5 pin */
	ADC_POSITIVE_INPUT_PIN5          = ADC_INPUTCTRL_MUXPOS_PIN5,
	/** ADC6 pin */
	ADC_POSITIVE_INPUT_PIN6          = ADC_INPUTCTRL_MUXPOS_PIN6,
	/** ADC7 pin */
	ADC_POSITIVE_INPUT_PIN7          = ADC_INPUTCTRL_MUXPOS_PIN7,
	/** ADC8 pin */
	ADC_POSITIVE_INPUT_PIN8          = ADC_INPUTCTRL_MUXPOS_PIN8,
	/** ADC9 pin */
	ADC_POSITIVE_INPUT_PIN9          = ADC_INPUTCTRL_MUXPOS_PIN9,
	/** ADC10 pin */
	ADC_POSITIVE_INPUT_PIN10         = ADC_INPUTCTRL_MUXPOS_PIN10,
	/** ADC11 pin */
	ADC_POSITIVE_INPUT_PIN11         = ADC_INPUTCTRL_MUXPOS_PIN11,
	/** ADC12 pin */
	ADC_POSITIVE_INPUT_PIN12         = ADC_INPUTCTRL_MUXPOS_PIN12,
	/** ADC13 pin */
	ADC_POSITIVE_INPUT_PIN13         = ADC_INPUTCTRL_MUXPOS_PIN13,
	/** ADC14 pin */
	ADC_POSITIVE_INPUT_PIN14         = ADC_INPUTCTRL_MUXPOS_PIN14,
	/** ADC15 pin */
	ADC_POSITIVE_INPUT_PIN15         = ADC_INPUTCTRL_MUXPOS_PIN15,
	/** ADC16 pin */
	ADC_POSITIVE_INPUT_PIN16         = ADC_INPUTCTRL_MUXPOS_PIN16,
	/** ADC17 pin */
	ADC_POSITIVE_INPUT_PIN17         = ADC_INPUTCTRL_MUXPOS_PIN17,
	/** ADC18 pin */
	ADC_POSITIVE_INPUT_PIN18         = ADC_INPUTCTRL_MUXPOS_PIN18,
	/** ADC19 pin */
	ADC_POSITIVE_INPUT_PIN19         = ADC_INPUTCTRL_MUXPOS_PIN19,
	/** Temperature reference */
	ADC_POSITIVE_INPUT_TEMP          = ADC_INPUTCTRL_MUXPOS_TEMP,
	/** Bandgap voltage */
	ADC_POSITIVE_INPUT_BANDGAP       = ADC_INPUTCTRL_MUXPOS_BANDGAP,
	/** 1/4 scaled core supply */
	ADC_POSITIVE_INPUT_SCALEDCOREVCC = ADC_INPUTCTRL_MUXPOS_SCALEDCOREVCC,
	/** 1/4 scaled I/O supply */
	ADC_POSITIVE_INPUT_SCALEDIOVCC   = ADC_INPUTCTRL_MUXPOS_SCALEDIOVCC,
	/** DAC input */
	ADC_POSITIVE_INPUT_DAC           = ADC_INPUTCTRL_MUXPOS_DAC,
};

/**
 * \brief ADC negative MUX input selection enum
 *
 * Enum for the possible negative MUX input selections for the ADC.
 *
 */
enum adc_negative_input {
	/** ADC0 pin */
	ADC_NEGATIVE_INPUT_PIN0          = ADC_INPUTCTRL_MUXNEG_PIN0,
	/** ADC1 pin */
	ADC_NEGATIVE_INPUT_PIN1          = ADC_INPUTCTRL_MUXNEG_PIN1,
	/** ADC2 pin */
	ADC_NEGATIVE_INPUT_PIN2          = ADC_INPUTCTRL_MUXNEG_PIN2,
	/** ADC3 pin */
	ADC_NEGATIVE_INPUT_PIN3          = ADC_INPUTCTRL_MUXNEG_PIN3,
	/** ADC4 pin */
	ADC_NEGATIVE_INPUT_PIN4          = ADC_INPUTCTRL_MUXNEG_PIN4,
	/** ADC5 pin */
	ADC_NEGATIVE_INPUT_PIN5          = ADC_INPUTCTRL_MUXNEG_PIN5,
	/** ADC6 pin */
	ADC_NEGATIVE_INPUT_PIN6          = ADC_INPUTCTRL_MUXNEG_PIN6,
	/** ADC7 pin */
	ADC_NEGATIVE_INPUT_PIN7          = ADC_INPUTCTRL_MUXNEG_PIN7,
	/** Internal ground */
	ADC_NEGATIVE_INPUT_GND           = ADC_INPUTCTRL_MUXNEG_GND,
	/** I/O ground */
	ADC_NEGATIVE_INPUT_IOGND         = ADC_INPUTCTRL_MUXNEG_IOGND,
};

/**
 * \brief ADC number of accumulated samples enum
 *
 * Enum for the possible numbers of ADC samples to accumulate.
 * This setting is only used when the \ref ADC_RESOLUTION_CUSTOM
 * resolution setting is used.
 *
 */
enum adc_accumulate_samples {
	/** No averaging */
	ADC_ACCUMULATE_DISABLE      = ADC_AVGCTRL_SAMPLENUM_1,
	/** Average 2 samples */
	ADC_ACCUMULATE_SAMPLES_2    = ADC_AVGCTRL_SAMPLENUM_2,
	/** Average 4 samples */
	ADC_ACCUMULATE_SAMPLES_4    = ADC_AVGCTRL_SAMPLENUM_4,
	/** Average 8 samples */
	ADC_ACCUMULATE_SAMPLES_8    = ADC_AVGCTRL_SAMPLENUM_8,
	/** Average 16 samples */
	ADC_ACCUMULATE_SAMPLES_16   = ADC_AVGCTRL_SAMPLENUM_16,
	/** Average 32 samples */
	ADC_ACCUMULATE_SAMPLES_32   = ADC_AVGCTRL_SAMPLENUM_32,
	/** Average 64 samples */
	ADC_ACCUMULATE_SAMPLES_64   = ADC_AVGCTRL_SAMPLENUM_64,
	/** Average 128 samples */
	ADC_ACCUMULATE_SAMPLES_128  = ADC_AVGCTRL_SAMPLENUM_128,
	/** Average 265 samples */
	ADC_ACCUMULATE_SAMPLES_256  = ADC_AVGCTRL_SAMPLENUM_256,
	/** Average 512 samples */
	ADC_ACCUMULATE_SAMPLES_512  = ADC_AVGCTRL_SAMPLENUM_512,
	/** Average 1024 samples */
	ADC_ACCUMULATE_SAMPLES_1024 = ADC_AVGCTRL_SAMPLENUM_1024,
};

/**
 * \brief ADC possible dividers for the result register
 *
 * Enum for the possible division factors to use when accumulating
 * multiple samples. To keep the same resolution for the averaged
 * result and the actual input value the division factor must
 * be equal to the number of samples accumulated. This setting is only
 * used when the \ref ADC_RESOLUTION_CUSTOM resolution setting is used.
 */
enum adc_divide_result {
	/** Don't divide result register after accumulation */
	ADC_DIVIDE_RESULT_DISABLE = 0,
	/** Divide result register by 2 after accumulation */
	ADC_DIVIDE_RESULT_2       = 1,
	/** Divide result register by 4 after accumulation */
	ADC_DIVIDE_RESULT_4       = 2,
	/** Divide result register by 8 after accumulation */
	ADC_DIVIDE_RESULT_8       = 3,
	/** Divide result register by 16 after accumulation */
	ADC_DIVIDE_RESULT_16      = 4,
	/** Divide result register by 32 after accumulation */
	ADC_DIVIDE_RESULT_32      = 5,
	/** Divide result register by 64 after accumulation */
	ADC_DIVIDE_RESULT_64      = 6,
	/** Divide result register by 128 after accumulation */
	ADC_DIVIDE_RESULT_128     = 7,
};

#if ADC_CALLBACK_MODE == true
/**
 * Enum for the possible ADC interrupt flags
 */
enum adc_interrupt_flag {
	/** ADC result ready */
	ADC_INTERRUPT_RESULT_READY = ADC_INTFLAG_RESRDY,
	/** Window monitor match */
	ADC_INTERRUPT_WINDOW       = ADC_INTFLAG_WINMON,
	/** ADC result overwritten before read */
	ADC_INTERRUPT_OVERRUN      = ADC_INTFLAG_OVERRUN,
};
#endif

/**
 * \brief ADC oversampling and decimation enum
 *
 * Enum for the possible numbers of bits resolution can be increased by when
 * using oversampling and decimation.
 *
 */
enum adc_oversampling_and_decimation {
	/** Don't use oversampling and decimation mode */
	ADC_OVERSAMPLING_AND_DECIMATION_DISABLE = 0,
	/** 1 bit resolution increase */
	ADC_OVERSAMPLING_AND_DECIMATION_1BIT,
	/** 2 bits resolution increase */
	ADC_OVERSAMPLING_AND_DECIMATION_2BIT,
	/** 3 bits resolution increase */
	ADC_OVERSAMPLING_AND_DECIMATION_3BIT,
	/** 4 bits resolution increase */
	ADC_OVERSAMPLING_AND_DECIMATION_4BIT
};

/**
 * \brief Window monitor configuration structure
 *
 * Window monitor configuration structure.
 */
struct adc_window_config {
	/** Selected window mode */
	enum adc_window_mode window_mode;
	/** Lower window value */
	int32_t window_lower_value;
	/** Upper window value */
	int32_t window_upper_value;
};

/**
 * \brief ADC event enable/disable structure.
 *
 * Event flags for the ADC module. This is used to enable and
 * disable events via \ref adc_enable_events() and \ref adc_disable_events().
 */
struct adc_events {
	/** Enable event generation on conversion done */
	bool generate_event_on_conversion_done;
	/** Enable event generation on window monitor */
	bool generate_event_on_window_monitor;
};

/**
 * \brief Gain and offset correction configuration structure
 *
 * Gain and offset correction configuration structure.
 * Part of the \ref adc_config struct and will  be initialized by
 * \ref adc_get_config_defaults .
 */
struct adc_correction_config {
	/**
	 * Enables correction for gain and offset based on values of gain_correction and
	 * offset_correction if set to true.
	 */
	bool correction_enable;
	/**
	 * This value defines how the ADC conversion result is compensated for gain
	 * error before written to the result register. This is a fractional value,
	 * 1-bit integer plus an 11-bit fraction, therefore
	 * 1/2 <= gain_correction < 2. Valid \c gain_correction values ranges from
	 * \c 0b010000000000 to \c 0b111111111111.
	 */
	uint16_t gain_correction;
	/**
	 * This value defines how the ADC conversion result is compensated for
	 * offset error before written to the result register. This is a 12-bit
	 * value in two’s complement format.
	 */
	int16_t offset_correction;
};

/**
 * \brief Pin scan configuration structure
 *
 * Pin scan configuration structure. Part of the \ref adc_config struct and will
 * be initialized by \ref adc_get_config_defaults .
 */
struct adc_pin_scan_config {
	/**
	 * Offset (relative to selected positive input) of the first input pin to be
	 * used in pin scan mode.
	 */
	uint8_t offset_start_scan;
	/**
	 * Number of input pins to scan in pin scan mode. A value below 2 will
	 * disable pin scan mode.
	 */
	uint8_t inputs_to_scan;
};

/**
 * \brief ADC configuration structure
 *
 * Configuration structure for an ADC instance. This structure should be
 * initialized by the \ref adc_get_config_defaults()
 * function before being modified by the user application.
 */
struct adc_config {
	/** GCLK generator used to clock the peripheral */
	enum gclk_generator clock_source;
	/** Voltage reference */
	enum adc_reference reference;
	/** Clock prescaler */
	enum adc_clock_prescaler clock_prescaler;
	/** Result resolution */
	enum adc_resolution resolution;
	/** Gain factor */
	enum adc_gain_factor gain_factor;
	/** Positive MUX input */
	enum adc_positive_input positive_input;
	/** Negative MUX input */
	enum adc_negative_input negative_input;
	/** Number of ADC samples to accumulate when using the
	 *  \c ADC_RESOLUTION_CUSTOM mode
	 */
	enum adc_accumulate_samples accumulate_samples;
	/** Division ration when using the ADC_RESOLUTION_CUSTOM mode */
	enum adc_divide_result divide_result;
	/** Left adjusted result */
	bool left_adjust;
	/** Enables differential mode if true */
	bool differential_mode;
	/** Enables free running mode if true */
	bool freerunning;
	/** Enables ADC in standby sleep mode if true */
	bool run_in_standby;
	/**
	 * Enables reference buffer offset compensation if true.
	 * This will increase the accuracy of the gain stage, but decreases the input
	 * impedance; therefore the startup time of the reference must be increased.
	 */
	bool reference_compensation_enable;
	/**
	 * This value (0-63) control the ADC sampling time in number of half ADC
	 * prescaled clock cycles (depends of \c ADC_PRESCALER value), thus
	 * controlling the ADC input impedance. Sampling time is set according to
	 * the formula:
	 * Sample time = (sample_length+1) * (ADCclk / 2)
	 */
	uint8_t sample_length;
	/** Window monitor configuration structure */
	struct adc_window_config window;
	/** Gain and offset correction configuration structure */
	struct adc_correction_config correction;
	/** Event action to take on incoming event */
	enum adc_event_action event_action;
	/** Pin scan configuration structure */
	struct adc_pin_scan_config pin_scan;
};

/**
 * \brief ADC software device instance structure.
 *
 * ADC software instance structure, used to retain software state information
 * of an associated hardware module instance.
 *
 * \note The fields of this structure should not be altered by the user
 *       application; they are reserved for module-internal use only.
 */
struct adc_module {
#if !defined(__DOXYGEN__)
	/** Pointer to ADC hardware module */
	Adc *hw;
	/** Keep reference configuration so we know when enable is called */
	enum adc_reference reference;
#  if ADC_CALLBACK_MODE == true
	/** Array to store callback functions */
	adc_callback_t callback[ADC_CALLBACK_N];
	/** Pointer to buffer used for ADC results */
	volatile uint16_t *job_buffer;
	/** Remaining number of conversions in current job */
	volatile uint16_t remaining_conversions;
	/** Bit mask for callbacks registered */
	uint8_t registered_callback_mask;
	/** Bit mask for callbacks enabled */
	uint8_t enabled_callback_mask;
	/** Holds the status of the ongoing or last conversion job */
	volatile enum status_code job_status;
	/** If software triggering is needed */
	bool software_trigger;
#  endif
#endif
};

/**
 * \name Driver initialization and configuration
 * @{
 */
enum status_code adc_init(
		struct adc_module *const module_inst,
		Adc *hw,
		struct adc_config *config);

/**
 * \brief Initializes an ADC configuration structure to defaults
 *
 * Initializes a given ADC configuration struct to a set of known default
 * values. This function should be called on any new instance of the
 * configuration struct before being modified by the user application.
 *
 * The default configuration is as follows:
 *  \li GCLK generator 0 (GCLK main) clock source
 *  \li 1V from internal bandgap reference
 *  \li Div 4 clock prescaler
 *  \li 12 bit resolution
 *  \li Window monitor disabled
 *  \li No gain
 *  \li Positive input on ADC PIN 0
 *  \li Negative input on ADC PIN 1
 *  \li Averaging disabled
 *  \li Oversampling disabled
 *  \li Right adjust data
 *  \li Single-ended mode
 *  \li Free running disabled
 *  \li All events (input and generation) disabled
 *  \li Sleep operation disabled
 *  \li No reference compensation
 *  \li No gain/offset correction
 *  \li No added sampling time
 *  \li Pin scan mode disabled
 *
 * \param[out] config  Pointer to configuration struct to initialize to
 *                     default values
 */
static inline void adc_get_config_defaults(struct adc_config *const config)
{
	Assert(config);
	config->clock_source                  = GCLK_GENERATOR_0;
	config->reference                     = ADC_REFERENCE_INT1V;
	config->clock_prescaler               = ADC_CLOCK_PRESCALER_DIV4;
	config->resolution                    = ADC_RESOLUTION_12BIT;
	config->window.window_mode            = ADC_WINDOW_MODE_DISABLE;
	config->window.window_upper_value     = 0;
	config->window.window_lower_value     = 0;
	config->gain_factor                   = ADC_GAIN_FACTOR_1X;
#if SAMR21
	config->positive_input                = ADC_POSITIVE_INPUT_PIN6 ;
#else
	config->positive_input                = ADC_POSITIVE_INPUT_PIN0 ;
#endif
	config->negative_input                = ADC_NEGATIVE_INPUT_GND ;
	config->accumulate_samples            = ADC_ACCUMULATE_DISABLE;
	config->divide_result                 = ADC_DIVIDE_RESULT_DISABLE;
	config->left_adjust                   = false;
	config->differential_mode             = false;
	config->freerunning                   = false;
	config->event_action                  = ADC_EVENT_ACTION_DISABLED;
	config->run_in_standby                = false;
	config->reference_compensation_enable = false;
	config->correction.correction_enable  = false;
	config->correction.gain_correction    = ADC_GAINCORR_RESETVALUE;
	config->correction.offset_correction  = ADC_OFFSETCORR_RESETVALUE;
	config->sample_length                 = 0;
	config->pin_scan.offset_start_scan    = 0;
	config->pin_scan.inputs_to_scan       = 0;
}

/** @} */

/**
 * \name Status Management
 * @{
 */

/**
 * \brief Retrieves the current module status.
 *
 * Retrieves the status of the module, giving overall state information.
 *
 * \param[in] module_inst  Pointer to the ADC software instance struct
 *
 * \return Bitmask of \c ADC_STATUS_* flags
 *
 * \retval ADC_STATUS_RESULT_READY  ADC Result is ready to be read
 * \retval ADC_STATUS_WINDOW        ADC has detected a value inside the set
 *                                  window range
 * \retval ADC_STATUS_OVERRUN       ADC result has overrun
 */
static inline uint32_t adc_get_status(
		struct adc_module *const module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	Adc *const adc_module = module_inst->hw;

	uint32_t int_flags = adc_module->INTFLAG.reg;

	uint32_t status_flags = 0;

	/* Check for ADC Result Ready */
	if (int_flags & ADC_INTFLAG_RESRDY) {
		status_flags |= ADC_STATUS_RESULT_READY;
	}

	/* Check for ADC Window Match */
	if (int_flags & ADC_INTFLAG_WINMON) {
		status_flags |= ADC_STATUS_WINDOW;
	}

	/* Check for ADC Overrun */
	if (int_flags & ADC_INTFLAG_OVERRUN) {
		status_flags |= ADC_STATUS_OVERRUN;
	}

	return status_flags;
}

/**
 * \brief Clears a module status flag.
 *
 * Clears the given status flag of the module.
 *
 * \param[in] module_inst   Pointer to the ADC software instance struct
 * \param[in] status_flags  Bitmask of \c ADC_STATUS_* flags to clear
 */
static inline void adc_clear_status(
		struct adc_module *const module_inst,
		const uint32_t status_flags)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	Adc *const adc_module = module_inst->hw;

	uint32_t int_flags = 0;

	/* Check for ADC Result Ready */
	if (status_flags & ADC_STATUS_RESULT_READY) {
		int_flags |= ADC_INTFLAG_RESRDY;
	}

	/* Check for ADC Window Match */
	if (status_flags & ADC_STATUS_WINDOW) {
		int_flags |= ADC_INTFLAG_WINMON;
	}

	/* Check for ADC Overrun */
	if (status_flags & ADC_STATUS_OVERRUN) {
		int_flags |= ADC_INTFLAG_OVERRUN;
	}

	/* Clear interrupt flag */
	adc_module->INTFLAG.reg = int_flags;
}
/** @} */

/**
 * \name Enable, disable and reset ADC module, start conversion and read result
 * @{
 */

/**
 * \brief Determines if the hardware module(s) are currently synchronizing to the bus.
 *
 * Checks to see if the underlying hardware peripheral module(s) are currently
 * synchronizing across multiple clock domains to the hardware bus, This
 * function can be used to delay further operations on a module until such time
 * that it is ready, to prevent blocking delays for synchronization in the
 * user application.
 *
 * \param[in] module_inst  Pointer to the ADC software instance struct
 *
 * \return Synchronization status of the underlying hardware module(s).
 *
 * \retval true if the module synchronization is ongoing
 * \retval false if the module has completed synchronization
 */
static inline bool adc_is_syncing(
	struct adc_module *const module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);

	Adc *const adc_module = module_inst->hw;

	if (adc_module->STATUS.reg & ADC_STATUS_SYNCBUSY) {
		return true;
	}

	return false;
}

/**
 * \brief Enables the ADC module
 *
 * Enables an ADC module that has previously been configured. If any internal reference
 * is selected it will be enabled.
 *
 * \param[in] module_inst  Pointer to the ADC software instance struct
 */
static inline enum status_code adc_enable(
		struct adc_module *const module_inst)
{
	Assert(module_inst);
	Assert(module_inst->hw);

	Adc *const adc_module = module_inst->hw;

	while (adc_is_syncing(module_inst)) {
		/* Wait for synchronization */
	}

	/* Make sure bandgap is enabled if requested by the config */
	if (module_inst->reference == ADC_REFERENCE_INT1V) {
		system_voltage_reference_enable(SYSTEM_VOLTAGE_REFERENCE_BANDGAP);
	}

#if ADC_CALLBACK_MODE == true
	system_interrupt_enable(SYSTEM_INTERRUPT_MODULE_ADC);
#endif

	adc_module->CTRLA.reg |= ADC_CTRLA_ENABLE;
	return STATUS_OK;
}

/**
 * \brief Disables the ADC module
 *
 * Disables an ADC module that was previously enabled.
 *
 * \param[in] module_inst Pointer to the ADC software instance struct
 */
static inline enum status_code adc_disable(
		struct adc_module *const module_inst)
{
	Assert(module_inst);
	Assert(module_inst->hw);

	Adc *const adc_module = module_inst->hw;

#if ADC_CALLBACK_MODE == true
	system_interrupt_disable(SYSTEM_INTERRUPT_MODULE_ADC);
#endif

	while (adc_is_syncing(module_inst)) {
		/* Wait for synchronization */
	}

	adc_module->CTRLA.reg &= ~ADC_CTRLA_ENABLE;
	return STATUS_OK;
}

/**
 * \brief Resets the ADC module
 *
 * Resets an ADC module, clearing all module state and registers to their
 * default values.
 *
 * \param[in] module_inst  Pointer to the ADC software instance struct
 */
static inline enum status_code adc_reset(
		struct adc_module *const module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	Adc *const adc_module = module_inst->hw;

	/* Disable to make sure the pipeline is flushed before reset */
	adc_disable(module_inst);

	while (adc_is_syncing(module_inst)) {
		/* Wait for synchronization */
	}

	/* Software reset the module */
	adc_module->CTRLA.reg |= ADC_CTRLA_SWRST;
	return STATUS_OK;
}


/**
 * \brief Enables an ADC event input or output.
 *
 *  Enables one or more input or output events to or from the ADC module. See
 *  \ref adc_events "here" for a list of events this module supports.
 *
 *  \note Events cannot be altered while the module is enabled.
 *
 *  \param[in] module_inst  Software instance for the ADC peripheral
 *  \param[in] events       Struct containing flags of events to enable
 */
static inline void adc_enable_events(
		struct adc_module *const module_inst,
		struct adc_events *const events)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);
	Assert(events);

	Adc *const adc_module = module_inst->hw;

	uint32_t event_mask = 0;

	/* Configure Window Monitor event */
	if (events->generate_event_on_window_monitor) {
		event_mask |= ADC_EVCTRL_WINMONEO;
	}

	/* Configure Result Ready event */
	if (events->generate_event_on_conversion_done) {
		event_mask |= ADC_EVCTRL_RESRDYEO;
	}

	adc_module->EVCTRL.reg |= event_mask;
}

/**
 * \brief Disables an ADC event input or output.
 *
 *  Disables one or more input or output events to or from the ADC module. See
 *  \ref adc_events "here" for a list of events this module supports.
 *
 *  \note Events cannot be altered while the module is enabled.
 *
 *  \param[in] module_inst  Software instance for the ADC peripheral
 *  \param[in] events       Struct containing flags of events to disable
 */
static inline void adc_disable_events(
		struct adc_module *const module_inst,
		struct adc_events *const events)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);
	Assert(events);

	Adc *const adc_module = module_inst->hw;

	uint32_t event_mask = 0;

	/* Configure Window Monitor event */
	if (events->generate_event_on_window_monitor) {
		event_mask |= ADC_EVCTRL_WINMONEO;
	}

	/* Configure Result Ready event */
	if (events->generate_event_on_conversion_done) {
		event_mask |= ADC_EVCTRL_RESRDYEO;
	}

	adc_module->EVCTRL.reg &= ~event_mask;
}

/**
 * \brief Starts an ADC conversion
 *
 * Starts a new ADC conversion.
 *
 * \param[in] module_inst  Pointer to the ADC software instance struct
 */
static inline void adc_start_conversion(
		struct adc_module *const module_inst)
{
	Assert(module_inst);
	Assert(module_inst->hw);

	Adc *const adc_module = module_inst->hw;

	while (adc_is_syncing(module_inst)) {
		/* Wait for synchronization */
	}

	adc_module->SWTRIG.reg |= ADC_SWTRIG_START;
}

/**
 * \brief Reads the ADC result
 *
 * Reads the result from an ADC conversion that was previously started.
 *
 * \param[in]  module_inst  Pointer to the ADC software instance struct
 * \param[out] result       Pointer to store the result value in
 *
 * \return Status of the ADC read request.
 * \retval STATUS_OK           The result was retrieved successfully
 * \retval STATUS_BUSY         A conversion result was not ready
 * \retval STATUS_ERR_OVERFLOW The result register has been overwritten by the
 *                             ADC module before the result was read by the software
 */
static inline enum status_code adc_read(
		struct adc_module *const module_inst,
		uint16_t *result)
{
	Assert(module_inst);
	Assert(module_inst->hw);
	Assert(result);

	if (!(adc_get_status(module_inst) & ADC_STATUS_RESULT_READY)) {
		/* Result not ready */
		return STATUS_BUSY;
	}

	Adc *const adc_module = module_inst->hw;

	while (adc_is_syncing(module_inst)) {
		/* Wait for synchronization */
	}

	/* Get ADC result */
	*result = adc_module->RESULT.reg;

	/* Reset ready flag */
	adc_clear_status(module_inst, ADC_STATUS_RESULT_READY);

	if (adc_get_status(module_inst) & ADC_STATUS_OVERRUN) {
		adc_clear_status(module_inst, ADC_STATUS_OVERRUN);
		return STATUS_ERR_OVERFLOW;
	}

	return STATUS_OK;
}

/** @} */

/**
 * \name Runtime changes of ADC module
 * @{
 */

/**
 * \brief Flushes the ADC pipeline
 *
 * Flushes the pipeline and restart the ADC clock on the next peripheral clock
 * edge. All conversions in progress will be lost. When flush is complete, the
 * module will resume where it left off.
 *
 * \param[in] module_inst  Pointer to the ADC software instance struct
 */
static inline void adc_flush(
		struct adc_module *const module_inst)
{
	Assert(module_inst);
	Assert(module_inst->hw);

	Adc *const adc_module = module_inst->hw;

	while (adc_is_syncing(module_inst)) {
		/* Wait for synchronization */
	}

	adc_module->SWTRIG.reg |= ADC_SWTRIG_FLUSH;
}

/**
 * \brief Sets the ADC window mode
 *
 * Sets the ADC window mode to a given mode and value range.
 *
 * \param[in] module_inst         Pointer to the ADC software instance struct
 * \param[in] window_mode         Window monitor mode to set
 * \param[in] window_lower_value  Lower window monitor threshold value
 * \param[in] window_upper_value  Upper window monitor threshold value
  */
static inline void adc_set_window_mode(
		struct adc_module *const module_inst,
		const enum adc_window_mode window_mode,
		const int16_t window_lower_value,
		const int16_t window_upper_value)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	Adc *const adc_module = module_inst->hw;

	while (adc_is_syncing(module_inst)) {
		/* Wait for synchronization */
	}

	/* Set window mode */
	adc_module->WINCTRL.reg = window_mode << ADC_WINCTRL_WINMODE_Pos;

	while (adc_is_syncing(module_inst)) {
		/* Wait for synchronization */
	}

	/* Set lower window monitor threshold value */
	adc_module->WINLT.reg = window_lower_value << ADC_WINLT_WINLT_Pos;

	while (adc_is_syncing(module_inst)) {
		/* Wait for synchronization */
	}

	/* Set upper window monitor threshold value */
	adc_module->WINUT.reg = window_upper_value << ADC_WINUT_WINUT_Pos;
}


/**
 * \brief Sets ADC gain factor
 *
 * Sets the ADC gain factor to a specified gain setting.
 *
 * \param[in] module_inst  Pointer to the ADC software instance struct
 * \param[in] gain_factor  Gain factor value to set
 */
static inline void adc_set_gain(
		struct adc_module *const module_inst,
		const enum adc_gain_factor gain_factor)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	Adc *const adc_module = module_inst->hw;

	while (adc_is_syncing(module_inst)) {
		/* Wait for synchronization */
	}

	/* Set new gain factor */
	adc_module->INPUTCTRL.reg =
			(adc_module->INPUTCTRL.reg & ~ADC_INPUTCTRL_GAIN_Msk) |
			(gain_factor << ADC_INPUTCTRL_GAIN_Pos);
}

/**
 * \brief Sets the ADC pin scan mode
 *
 * Configures the pin scan mode of the ADC module. In pin scan mode, the first
 * conversion will start at the configured positive input + start_offset. When
 * a conversion is done, a conversion will start on the next input, until
 * \c inputs_to_scan number of conversions are made.
 *
 * \param[in] module_inst     Pointer to the ADC software instance struct
 * \param[in] inputs_to_scan  Number of input pins to perform a conversion on
 *                            (must be two or more)
 * \param[in] start_offset    Offset of first pin to scan (relative to
 *                            configured positive input)
 *
 * \return Status of the pin scan configuration set request.
 *
 * \retval STATUS_OK               Pin scan mode has been set successfully
 * \retval STATUS_ERR_INVALID_ARG  Number of input pins to scan or offset has
 *                                 an invalid value
 */
static inline enum status_code adc_set_pin_scan_mode(
		struct adc_module *const module_inst,
		uint8_t inputs_to_scan,
		const uint8_t start_offset)

{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	Adc *const adc_module = module_inst->hw;

	if (inputs_to_scan > 0) {
		/*
		* Number of input sources included is the value written to INPUTSCAN
		* plus 1.
		*/
		inputs_to_scan--;
	}

	if (inputs_to_scan > (ADC_INPUTCTRL_INPUTSCAN_Msk >> ADC_INPUTCTRL_INPUTSCAN_Pos) ||
			start_offset > (ADC_INPUTCTRL_INPUTOFFSET_Msk >> ADC_INPUTCTRL_INPUTOFFSET_Pos)) {
		/* Invalid number of input pins */
		return STATUS_ERR_INVALID_ARG;
	}

	while (adc_is_syncing(module_inst)) {
		/* Wait for synchronization */
	}

	/* Set pin scan mode */
	adc_module->INPUTCTRL.reg =
			(adc_module->INPUTCTRL.reg &
			~(ADC_INPUTCTRL_INPUTSCAN_Msk | ADC_INPUTCTRL_INPUTOFFSET_Msk)) |
			(start_offset   << ADC_INPUTCTRL_INPUTOFFSET_Pos) |
			(inputs_to_scan << ADC_INPUTCTRL_INPUTSCAN_Pos);

	return STATUS_OK;
}

/**
 * \brief Disables pin scan mode
 *
 * Disables pin scan mode. The next conversion will be made on only one pin
 * (the configured positive input pin).
 *
 * \param[in] module_inst  Pointer to the ADC software instance struct
 */
static inline void adc_disable_pin_scan_mode(
		struct adc_module *const module_inst)
{
	/* Disable pin scan mode */
	adc_set_pin_scan_mode(module_inst, 0, 0);
}


/**
 * \brief Sets positive ADC input pin
 *
 * Sets the positive ADC input pin selection.
 *
 * \param[in] module_inst     Pointer to the ADC software instance struct
 * \param[in] positive_input  Positive input pin
 */
static inline void adc_set_positive_input(
		struct adc_module *const module_inst,
		const enum adc_positive_input positive_input)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	Adc *const adc_module = module_inst->hw;

	while (adc_is_syncing(module_inst)) {
		/* Wait for synchronization */
	}

	/* Set positive input pin */
	adc_module->INPUTCTRL.reg =
			(adc_module->INPUTCTRL.reg & ~ADC_INPUTCTRL_MUXPOS_Msk) |
			(positive_input << ADC_INPUTCTRL_MUXPOS_Pos);
}


/**
 * \brief Sets negative ADC input pin for differential mode
 *
 * Sets the negative ADC input pin, when the ADC is configured in differential
 * mode.
 *
 * \param[in] module_inst     Pointer to the ADC software instance struct
 * \param[in] negative_input  Negative input pin
 */
static inline void adc_set_negative_input(
		struct adc_module *const module_inst,
		const enum adc_negative_input negative_input)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	Adc *const adc_module = module_inst->hw;

	while (adc_is_syncing(module_inst)) {
		/* Wait for synchronization */
	}

	/* Set negative input pin */
	adc_module->INPUTCTRL.reg =
			(adc_module->INPUTCTRL.reg & ~ADC_INPUTCTRL_MUXNEG_Msk) |
			(negative_input << ADC_INPUTCTRL_MUXNEG_Pos);
}

/** @} */

#if ADC_CALLBACK_MODE == true
/**
 * \name Enable and disable interrupts
 * @{
 */

/**
 * \brief Enable interrupt
 *
 * Enable the given interrupt request from the ADC module.
 *
 * \param[in] module_inst Pointer to the ADC software instance struct
 * \param[in] interrupt Interrupt to enable
 */
static inline void adc_enable_interrupt(struct adc_module *const module_inst,
		enum adc_interrupt_flag interrupt)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	Adc *const adc_module = module_inst->hw;
	/* Enable interrupt */
	adc_module->INTENSET.reg = interrupt;
}

/**
 * \brief Disable interrupt
 *
 * Disable the given interrupt request from the ADC module.
 *
 * \param[in] module_inst Pointer to the ADC software instance struct
 * \param[in] interrupt Interrupt to disable
 */
static inline void adc_disable_interrupt(struct adc_module *const module_inst,
		enum adc_interrupt_flag interrupt)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	Adc *const adc_module = module_inst->hw;
	/* Enable interrupt */
	adc_module->INTENCLR.reg = interrupt;
}

/** @} */
#endif /* ADC_CALLBACK_MODE == true */

#ifdef __cplusplus
}
#endif

/** @} */


/**
 * \page asfdoc_sam0_adc_extra Extra Information for ADC Driver
 *
 * \section asfdoc_sam0_adc_extra_acronyms Acronyms
 * Below is a table listing the acronyms used in this module, along with their
 * intended meanings.
 *
 * <table>
 *	<tr>
 *		<th>Acronym</th>
 *		<th>Description</th>
 *	</tr>
 *  <tr>
 *		<td>ADC</td>
 *		<td>Analog-to-Digital Converter</td>
 *	</tr>
 *  <tr>
 *		<td>DAC</td>
 *		<td>Digital-to-Analog Converter</td>
 *	</tr>
 *	<tr>
 *		<td>LSB</td>
 *		<td>Least Significant Bit</td>
 *	</tr>
 *	<tr>
 *		<td>MSB</td>
 *		<td>Most Significant Bit</td>
 *	</tr>
 *	<tr>
 *		<td>DMA</td>
 *		<td>Direct Memory Access</td>
 *	</tr>
 * </table>
 *
 *
 * \section asfdoc_sam0_adc_extra_dependencies Dependencies
 * This driver has the following dependencies:
 *
 *  - \ref asfdoc_sam0_system_pinmux_group "System Pin Multiplexer Driver"
 *
 *
 * \section asfdoc_sam0_adc_extra_errata Errata
 * There are no errata related to this driver.
 *
 *
 * \section asfdoc_sam0_adc_extra_history Module History
 * An overview of the module history is presented in the table below, with
 * details on the enhancements and fixes made to the module since its first
 * release. The current version of this corresponds to the newest version in
 * the table.
 *
 * <table>
 *	<tr>
 *		<th>Changelog</th>
 *	</tr>
 *	<tr>
 *		<td>Added support for SAMR21.</td>
 *	</tr>
 *	<tr>
 *		<td>Added support for SAMD21 and new DMA quick start guide.</td>
 *	</tr>
 *	<tr>
 *		<td>Added ADC calibration constant loading from the device signature
 *          row when the module is initialized.</td>
 *	</tr>
 *	<tr>
 *		<td>Initial Release</td>
 *	</tr>
 * </table>
 */

/**
 * \page asfdoc_sam0_adc_exqsg Examples for ADC Driver
 *
 * This is a list of the available Quick Start guides (QSGs) and example
 * applications for \ref asfdoc_sam0_adc_group. QSGs are simple examples with
 * step-by-step instructions to configure and use this driver in a selection of
 * use cases. Note that QSGs can be compiled as a standalone application or be
 * added to the user application.
 *
 *  - \subpage asfdoc_sam0_adc_basic_use_case
 * \if ADC_CALLBACK_MODE
 *  - \subpage asfdoc_sam0_adc_basic_use_case_callback
 * \endif
 *  - \subpage asfdoc_sam0_adc_dma_use_case
 *
 * \page asfdoc_sam0_adc_document_revision_history Document Revision History
 *
 * <table>
 *	<tr>
 *		<th>Doc. Rev.</td>
 *		<th>Date</td>
 *		<th>Comments</td>
 *	</tr>
 *	<tr>
 *		<td>D</td>
 *		<td>03/2014</td>
 *		<td>Added support for SAMR21.</td>
 *	</tr>
 *	<tr>
 *		<td>C</td>
 *		<td>01/2014</td>
 *		<td>Added support for SAMD21.</td>
 *	</tr>
 *	<tr>
 *		<td>B</td>
 *		<td>06/2013</td>
 *		<td>Added additional documentation on the event system. Corrected
 *          documentation typos.</td>
 *	</tr>
 *	<tr>
 *		<td>A</td>
 *		<td>06/2013</td>
 *		<td>Initial release</td>
 *	</tr>
 * </table>
 */

#endif /* ADC_H_INCLUDED */
