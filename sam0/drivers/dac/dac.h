/**
 * \file
 *
 * \brief SAM D20/D21 Peripheral Digital-to-Analog Converter Driver
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
#ifndef DAC_H_INCLUDED
#define DAC_H_INCLUDED

/**
 * \defgroup asfdoc_sam0_dac_group SAM D20/D21 Digital-to-Analog Driver (DAC)
 *
 * This driver for SAM D20/D21 devices provides an interface for the conversion of
 * digital values to analog voltage. The following driver API modes are covered
 * by this manual:
 *
 *  - Polled APIs
 * \if DAC_CALLBACK_MODE
 *  - Callback APIs
 * \endif
 *
 * The following peripherals are used by this module:
 *
 *  - DAC (Digital to Analog Converter)
 *
 * The outline of this documentation is as follows:
 *  - \ref asfdoc_sam0_dac_prerequisites
 *  - \ref asfdoc_sam0_dac_module_overview
 *  - \ref asfdoc_sam0_dac_special_considerations
 *  - \ref asfdoc_sam0_dac_extra_info
 *  - \ref asfdoc_sam0_dac_examples
 *  - \ref asfdoc_sam0_dac_api_overview
 *
 *
 * \section asfdoc_sam0_dac_prerequisites Prerequisites
 *
 * There are no prerequisites for this module.
 *
 *
 * \section asfdoc_sam0_dac_module_overview Module Overview
 *
 * The Digital-to-Analog converter converts a digital value to analog voltage.
 * The SAM D20/D21 DAC module has one channel with 10-bit resolution, and is capable
 * of converting up to 350k samples per second (ksps).
 *
 * A common use of DAC is to generate audio signals by connecting the DAC
 * output to a speaker, or to generate a reference voltage; either for an
 * external circuit or an internal peripheral such as the Analog Comparator.
 *
 * After being set up, the DAC will convert new digital values written to the
 * conversion data register (DATA) to an analog value either on the VOUT pin of
 * the device, or internally for use as an input to the AC, ADC and other analog
 * modules.
 *
 * Writing the DATA register will start a new conversion. It is also possible
 * to trigger the conversion from the event system.
 *
 * A simplified block diagram of the DAC can be seen in
 * \ref asfdoc_sam0_dac_module_block_diagram "the figure below".
 *
 * \anchor asfdoc_sam0_dac_module_block_diagram
 * \image html dac_block_diagram.gif "DAC Block Diagram"
 *
 * \subsection asfdoc_sam0_dac_conversion_range Conversion Range
 * The conversion range is between GND and the selected voltage reference.
 * Available voltage references are:
 * \li AVCC voltage reference
 * \li Internal 1V reference (INT1V)
 * \li External voltage reference (AREF)
 *
 * \note Internal references will be enabled by the driver, but not disabled.
 * Any reference not used by the application should be disabled by the application.
 *
 * The output voltage from a DAC channel is given as:
 * \f[
 *    V_{OUT} = \frac{DATA}{0x3FF} \times VREF
 * \f]
 *
 * \subsection asfdoc_sam0_dac_conversion Conversion
 * The digital value written to the conversion data register (DATA) will be
 * converted to an analog value.
 * Writing the DATA register will start a new conversion.
 * It is also possible to write the conversion data to the DATABUF register,
 * the writing of the DATA register can then be triggered from the event
 * system, which will load the value from DATABUF to DATA.
 *
 * \subsection asfdoc_sam0_dac_analog_output Analog Output
 * The analog output value can be output to either the VOUT pin or internally,
 * but not both at the same time.
 *
 * \subsubsection asfdoc_sam0_dac_analog_output_external External Output
 * The output buffer must be enabled in order to drive the DAC output to the
 * VOUT pin. Due to the output buffer, the DAC has high drive strength, and is
 * capable of driving both resistive and capacitive loads, as well as loads
 * which combine both.
 *
 * \subsubsection asfdoc_sam0_dac_analog_output_internal Internal Output
 * The analog value can be internally available for use as input to the
 * AC or ADC modules.
 *
 * \subsection asfdoc_sam0_dac_events Events
 * Events generation and event actions are configurable in the DAC.
 * The DAC has one event line input and one event output: <i>Start Conversion</i>
 * and <i>Data Buffer Empty</i>.
 *
 * If the Start Conversion input event is enabled in the module configuration,
 * an incoming event will load data from the data buffer to the data register
 * and start a new conversion. This method synchronizes conversions with
 * external events (such as those from a timer module) and ensures regular and
 * fixed conversion intervals.
 *
 * If the Data Buffer Empty output event is enabled in the module configuration,
 * events will be generated when the DAC data buffer register becomes empty and
 * new data can be loaded to the buffer.
 *
 * \note The connection of events between modules requires the use of the
 *       \ref asfdoc_sam0_events_group "SAM D20/D21 Event System Driver (EVENTS)"
 *       to route output event of one module to the the input event of another.
 *       For more information on event routing, refer to the event driver
 *       documentation.
 *
 * \subsection asfdoc_sam0_dac_data_adjust Left and Right Adjusted Values
 * The 10-bit input value to the DAC is contained in a 16-bit register. This
 * can be configured to be either left or right adjusted. In
 * \ref asfdoc_sam0_dac_module_adj_modes "the figure below" both options are
 * shown, and the position of the most (MSB) and the least (LSB) significant bits
 * are indicated. The unused bits should always be written to zero.
 *
 * \anchor asfdoc_sam0_dac_module_adj_modes
 * \dot
 * digraph {
 *   subgraph cluster_right {
 *      msbl [label="MSB", shape=none, group="msbl"];
 *      lsbl [label="LSB", shape=none];
 *      node [shape=none];
 *      color="white";
 *      reg_left [label=<
 *        <table cellspacing="0" cellpadding="2" width="100%">
 *          <tr>
 *            <td port="msb">15</td>
 *            <td>14</td>
 *            <td>13</td>
 *            <td>12</td>
 *            <td>11</td>
 *            <td>10</td>
 *            <td>9</td>
 *            <td>8</td>
 *            <td>7</td>
 *            <td port="lsb">6</td>
 *            <td>5</td>
 *            <td>4</td>
 *            <td>3</td>
 *            <td>2</td>
 *            <td>1</td>
 *            <td>0</td>
 *          </tr>
 *          <tr>
 *            <td COLSPAN="10"> DATA[9:0] </td>
 *            <td BGCOLOR="lightgray"> </td>
 *            <td BGCOLOR="lightgray"> </td>
 *            <td BGCOLOR="lightgray"> </td>
 *            <td BGCOLOR="lightgray"> </td>
 *            <td BGCOLOR="lightgray"> </td>
 *            <td BGCOLOR="lightgray"> </td>
 *          </tr>
 *        </table>
 *      >];
 *      msbl -> reg_left:msb:n;
 *      lsbl -> reg_left:lsb;
 *      label ="Left adjusted.\n";
 *   }
 *   subgraph cluster_left {
 *      rankdir=TB;
 *      msb [label="MSB", shape=none];
 *      lsb [label="LSB", shape=none];
 *      color="white";
 *      node [shape=none];
 *      reg_right [label=<
 *        <table cellspacing="0" cellpadding="2">
 *          <tr>
 *            <td>15</td>
 *            <td>14</td>
 *            <td>13</td>
 *            <td>12</td>
 *            <td>11</td>
 *            <td>10</td>
 *            <td port="msb">9</td>
 *            <td>8</td>
 *            <td>7</td>
 *            <td>6</td>
 *            <td>5</td>
 *            <td>4</td>
 *            <td>3</td>
 *            <td>2</td>
 *            <td>1</td>
 *            <td port="lsb">0</td>
 *          </tr>
 *          <tr>
 *            <td BGCOLOR="lightgray"></td>
 *            <td BGCOLOR="lightgray"></td>
 *            <td BGCOLOR="lightgray"></td>
 *            <td BGCOLOR="lightgray"></td>
 *            <td BGCOLOR="lightgray"></td>
 *            <td BGCOLOR="lightgray"></td>
 *            <td COLSPAN="10"> DATA[9:0] </td>
 *          </tr>
 *        </table>
 *      >];
 *      msb -> reg_right:msb;
 *      lsb -> reg_right:lsb:n;
 *      label = "Right adjusted.\n";
 *      graph [shape=none];
 *  }
 * }
 * \enddot
 *
 * \subsection asfdoc_sam0_dac_clk_sources Clock Sources
 * The clock for the DAC interface (CLK_DAC) is generated by the Power Manager.
 * This clock is turned on by default, and can be enabled and disabled in the
 * Power Manager.
 *
 * Additionally, an asynchronous clock source (GCLK_DAC) is required.
 * These clocks are normally disabled by default. The selected clock source
 * must be enabled in the Power Manager before it can be used by the DAC.
 * The DAC core operates asynchronously from the user interface and
 * peripheral bus. As a consequence, the DAC needs two clock cycles of both
 * CLK_DAC and GCLK_DAC to synchronize the values written to some of the
 * control and data registers.
 * The oscillator source for the GCLK_DAC clock is selected in the System
 * Control Interface (SCIF).
 *
 * \section asfdoc_sam0_dac_special_considerations Special Considerations
 *
 * \subsection asfdoc_sam0_dac_special_considerations_output_buffer Output Driver
 * The DAC can only do conversions in Active or Idle modes. However, if the
 * output buffer is enabled it will draw current even if the system is in
 * sleep mode. Therefore, always make sure that the output buffer is not
 * enabled when it is not needed, to ensure minimum power consumption.
 *
 * \subsection asfdoc_sam0_dac_special_considerations_conversion_time Conversion Time
 * DAC conversion time is approximately 2.85us. The user must ensure that new
 * data is not written to the DAC before the last conversion is complete.
 * Conversions should be triggered by a periodic event from a Timer/Counter or
 * another peripheral.
 *
 *
 * \section asfdoc_sam0_dac_extra_info Extra Information
 *
 * For extra information see \ref asfdoc_sam0_dac_extra. This includes:
 *  - \ref asfdoc_sam0_dac_extra_acronyms
 *  - \ref asfdoc_sam0_dac_extra_dependencies
 *  - \ref asfdoc_sam0_dac_extra_errata
 *  - \ref asfdoc_sam0_dac_extra_history
 *
 *
 * \section asfdoc_sam0_dac_examples Examples
 *
 * For a list of examples related to this driver, see
 * \ref asfdoc_sam0_dac_exqsg.
 *
 *
 * \section asfdoc_sam0_dac_api_overview API Overview
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <compiler.h>
#include <clock.h>
#include <gclk.h>

/**
 * Define DAC features set according to different device family
 * @{
 */
#if (SAMD21)
#  define FEATURE_DAC_DATABUF_WRITE_PROTECTION
#endif
/*@}*/

#ifndef DAC_TIMEOUT
#  define DAC_TIMEOUT 0xFFFF
#endif

#if DAC_CALLBACK_MODE == true
#  include <system_interrupt.h>

/** Forward definition of the device instance */
struct dac_module;

#if !defined(__DOXYGEN__)
extern struct dac_module *_dac_instances[DAC_INST_NUM];
#endif

/** Type definition for a DAC module callback function. */
typedef void (*dac_callback_t)(uint8_t channel);

/** Enum for the possible callback types for the DAC module. */
enum dac_callback {
	/** Callback type for when a DAC channel data empty condition occurs
	 *  (requires event triggered mode). */
	DAC_CALLBACK_DATA_EMPTY,

	/** Callback type for when a DAC channel data under-run condition occurs
	 *  (requires event triggered mode). */
	DAC_CALLBACK_DATA_UNDERRUN,

	/** Callback type for when a DAC channel write buffer job complete.
	 *  (requires event triggered mode). */
	DAC_CALLBACK_TRANSFER_COMPLETE,
#if !defined(__DOXYGEN__)
	DAC_CALLBACK_N,
#endif
};

#endif

/**
 * \name DAC status flags
 *
 * DAC status flags, returned by \ref dac_get_status() and cleared by
 * \ref dac_clear_status().
 * @{
 */

/** Data Buffer Empty Channel 0 - Set when data is transferred from DATABUF
 *  to DATA by a start conversion event and DATABUF is ready for new data.
 */
#define DAC_STATUS_CHANNEL_0_EMPTY     (1UL << 0)

/** Under-run Channel 0 - Set when a start conversion event occurs when
 *  DATABUF is empty.
 */
#define DAC_STATUS_CHANNEL_0_UNDERRUN  (1UL << 1)

/** @} */

/**
 * \brief DAC reference voltage enum.
 *
 * Enum for the possible reference voltages for the DAC.
 */
enum dac_reference {
	/** 1V from the internal band-gap reference.*/
	DAC_REFERENCE_INT1V = DAC_CTRLB_REFSEL(0),
	/** Analog VCC as reference. */
	DAC_REFERENCE_AVCC  = DAC_CTRLB_REFSEL(1),
	/** External reference on AREF. */
	DAC_REFERENCE_AREF  = DAC_CTRLB_REFSEL(2),
};

/**
 * \brief DAC output selection enum.
 *
 * Enum for the DAC output selection.
 */
enum dac_output {
	/** DAC output to VOUT pin */
	DAC_OUTPUT_EXTERNAL = DAC_CTRLB_EOEN,
	/** DAC output as internal reference */
	DAC_OUTPUT_INTERNAL = DAC_CTRLB_IOEN,
	/** No output */
	DAC_OUTPUT_NONE     = 0,
};

/**
 * \brief DAC channel selection enum.
 *
 * Enum for the DAC channel selection.
 */
enum dac_channel {
	/** DAC output channel 0. */
	DAC_CHANNEL_0,
};

/**
 * \brief DAC software device instance structure.
 *
 * DAC software instance structure, used to retain software state information
 * of an associated hardware module instance.
 *
 * \note The fields of this structure should not be altered by the user
 *       application; they are reserved for module-internal use only.
 */
struct dac_module {
#if !defined(__DOXYGEN__)
	/** DAC hardware module */
	Dac *hw;
	/** DAC output selection */
	enum dac_output output;
	/** Reference selection */
	enum dac_reference reference;
	/** DAC event selection */
	bool start_on_event;
#  if DAC_CALLBACK_MODE == true
	/** Pointer to buffer used for ADC results */
	volatile uint16_t *job_buffer;
	/** Remaining number of conversions in current job */
	volatile uint16_t remaining_conversions;
	/** Transferred number of conversions in current job */
	volatile uint16_t transferred_conversions;
	/** DAC callback enable */
	bool callback_enable[DAC_CALLBACK_N];
	/** DAC registered callback functions */
	dac_callback_t callback[DAC_CALLBACK_N];
	/** Holds the status of the ongoing or last conversion job */
	volatile enum status_code job_status;
#  endif
#endif
};

/**
 * \brief DAC configuration structure.
 *
 * Configuration structure for a DAC instance. This structure should be
 * initialized by the \ref dac_get_config_defaults()
 * function before being modified by the user application.
 */
struct dac_config {
	/** Reference voltage */
	enum dac_reference reference;
	/** Select DAC output */
	enum dac_output output;
	/** Left adjusted data */
	bool left_adjust;
	/** GCLK generator used to clock the peripheral */
	enum gclk_generator clock_source;
#ifdef FEATURE_DAC_DATABUF_WRITE_PROTECTION
	/** Bypass DATABUF write protection */
	bool databuf_protection_bypass;
#endif
	/** Voltage pump disable */
	bool voltage_pump_disable;
	/**
	 * The DAC behaves as in normal mode when the chip enters STANDBY sleep
	 * mode
	 */
	bool run_in_standby;
};

/**
 * \brief DAC event enable/disable structure.
 *
 * Event flags for the DAC module. This is used to enable and
 * disable events via \ref dac_enable_events() and \ref dac_disable_events().
 */
struct dac_events {
	/** Start a new DAC conversion */
	bool on_event_start_conversion;
	/** Enable event generation on data buffer empty */
	bool generate_event_on_buffer_empty;
};

/**
 * \brief DAC channel configuration structure
 *
 * Configuration for a DAC channel. This structure should be initialized by the
 * \ref dac_chan_get_config_defaults() function before being modified by the
 * user application.
 */
struct dac_chan_config {
#if !defined(__DOXYGEN__)
	/** Dummy value to ensure the struct has at least one member */
	uint8_t _dummy;
#endif
};

/**
 * \name Configuration and Initialization
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
 * \param[in] dev_inst  Pointer to the DAC software instance struct
 *
 * \return Synchronization status of the underlying hardware module(s).
 *
 * \retval true if the module has completed synchronization
 * \retval false if the module synchronization is ongoing
 */
static inline bool dac_is_syncing(
		struct dac_module *const dev_inst)
{
	/* Sanity check arguments */
	Assert(dev_inst);

	Dac *const dac_module = dev_inst->hw;

	if (dac_module->STATUS.reg & DAC_STATUS_SYNCBUSY) {
		return true;
	}

	return false;
}

/**
 * \brief Initializes a DAC configuration structure to defaults.
 *
 *  Initializes a given DAC configuration structure to a set of
 *  known default values. This function should be called on any new
 *  instance of the configuration structures before being modified by the
 *  user application.
 *
 *  The default configuration is as follows:
 *   \li 1V from internal bandgap reference
 *   \li Drive the DAC output to the VOUT pin
 *   \li Right adjust data
 *   \li GCLK generator 0 (GCLK main) clock source
 *   \li The output buffer is disabled when the chip enters STANDBY sleep
 *       mode
 *
 * \param[out] config  Configuration structure to initialize to default values
 */
static inline void dac_get_config_defaults(
		struct dac_config *const config)
{
	/* Sanity check arguments */
	Assert(config);

	/* Default configuration values */
	config->reference      = DAC_REFERENCE_INT1V;
	config->output         = DAC_OUTPUT_EXTERNAL;
	config->left_adjust    = false;
#ifdef FEATURE_DAC_DATABUF_WRITE_PROTECTION
	config->databuf_protection_bypass = false;
#endif
	config->voltage_pump_disable = false;
	config->clock_source   = GCLK_GENERATOR_0;
	config->run_in_standby = false;
}

enum status_code dac_init(
		struct dac_module *const dev_inst,
		Dac *const module,
		struct dac_config *const config);

void dac_reset(
		struct dac_module *const dev_inst);

void dac_enable(
		struct dac_module *const dev_inst);

void dac_disable(
		struct dac_module *const dev_inst);

/**
 * \brief Enables a DAC event input or output.
 *
 *  Enables one or more input or output events to or from the DAC module. See
 *  \ref dac_events "here" for a list of events this module supports.
 *
 *  \note Events cannot be altered while the module is enabled.
 *
 *  \param[in] module_inst  Software instance for the DAC peripheral
 *  \param[in] events       Struct containing flags of events to enable
 */
static inline void dac_enable_events(
		struct dac_module *const module_inst,
		struct dac_events *const events)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);
	Assert(events);

	Dac *const dac_module = module_inst->hw;

	uint32_t event_mask = 0;

	/* Configure Buffer Empty event */
	if (events->generate_event_on_buffer_empty) {
		event_mask |= DAC_EVCTRL_EMPTYEO;
	}

	/* Configure Conversion Start event */
	if (events->on_event_start_conversion) {
		event_mask |= DAC_EVCTRL_STARTEI;
		module_inst->start_on_event = true;
	}

	dac_module->EVCTRL.reg |= event_mask;
}

/**
 * \brief Disables a DAC event input or output.
 *
 *  Disables one or more input or output events to or from the DAC module. See
 *  \ref dac_events "here" for a list of events this module supports.
 *
 *  \note Events cannot be altered while the module is enabled.
 *
 *  \param[in] module_inst  Software instance for the DAC peripheral
 *  \param[in] events       Struct containing flags of events to disable
 */
static inline void dac_disable_events(
		struct dac_module *const module_inst,
		struct dac_events *const events)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);
	Assert(events);

	Dac *const dac_module = module_inst->hw;

	uint32_t event_mask = 0;

	/* Configure Buffer Empty event */
	if (events->generate_event_on_buffer_empty) {
		event_mask |= DAC_EVCTRL_EMPTYEO;
	}

	/* Configure Conversion Start event */
	if (events->on_event_start_conversion) {
		event_mask |= DAC_EVCTRL_STARTEI;
		module_inst->start_on_event = false;
	}

	dac_module->EVCTRL.reg &= ~event_mask;
}

/** @} */

/**
 * \name Configuration and Initialization (Channel)
 * @{
 */

/**
 * \brief Initializes a DAC channel configuration structure to defaults.
 *
 * Initializes a given DAC channel configuration structure to a set of
 * known default values. This function should be called on any new
 * instance of the configuration structures before being modified by the
 * user application.
 *
 * The default configuration is as follows:
 *  \li Start Conversion Event Input enabled
 *  \li Start Data Buffer Empty Event Output disabled
 *
 * \param[out] config  Configuration structure to initialize to default values
 */
static inline void dac_chan_get_config_defaults(
		struct dac_chan_config *const config)
{
	/* Sanity check arguments */
	Assert(config);
}

void dac_chan_set_config(
		struct dac_module *const dev_inst,
		const enum dac_channel channel,
		struct dac_chan_config *const config);

void dac_chan_enable(
		struct dac_module *const dev_inst,
		enum dac_channel channel);

void dac_chan_disable(
		struct dac_module *const dev_inst,
		enum dac_channel channel);

void dac_chan_enable_output_buffer(
		struct dac_module *const dev_inst,
		const enum dac_channel channel);

void dac_chan_disable_output_buffer(
		struct dac_module *const dev_inst,
		const enum dac_channel channel);

/** @} */

/**
 * \name Channel Data Management
 * @{
 */

enum status_code dac_chan_write(
		struct dac_module *const dev_inst,
		enum dac_channel channel,
		const uint16_t data);

enum status_code dac_chan_write_buffer_wait(
		struct dac_module *const module_inst,
		enum dac_channel channel,
		uint16_t *buffer,
		uint32_t length);

/** @} */

/**
 * \name Status Management
 * @{
 */
uint32_t dac_get_status(
		struct dac_module *const module_inst);
void dac_clear_status(
		struct dac_module *const module_inst,
		uint32_t status_flags);

/** @} */

#ifdef __cplusplus
}
#endif

/** @} */

/**
 * \page asfdoc_sam0_dac_extra Extra Information for DAC Driver
 *
 * \section asfdoc_sam0_dac_extra_acronyms Acronyms
 * The table below presents the acronyms used in this module:
 *
 * <table>
 *	<tr>
 *		<th>Acronym</th>
 *		<th>Description</th>
 *	</tr>
 *	<tr>
 *		<td>ADC</td>
 *		<td>Analog-to-Digital Converter</td>
 *	</tr>
 *	<tr>
 *		<td>AC</td>
 *		<td>Analog Comparator</td>
 *	</tr>
 *	<tr>
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
 * \section asfdoc_sam0_dac_extra_dependencies Dependencies
 * This driver has the following dependencies:
 *
 *  - \ref asfdoc_sam0_system_pinmux_group "System Pin Multiplexer Driver"
 *
 *
 * \section asfdoc_sam0_dac_extra_errata Errata
 * There are no errata related to this driver.
 *
 *
 * \section asfdoc_sam0_dac_extra_history Module History
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
 *		<td>Added new configuration parameters \c databuf_protection_bypass,
 *		\c voltage_pump_disable. Added new callback functions 
 *		\c dac_chan_write_buffer_wait,
 *		\c dac_chan_write_buffer_job, \c dac_chan_write_job,
 *		\c dac_get_job_status, \c dac_abort_job and new callback type
 *		\c DAC_CALLBACK_TRANSFER_COMPLETE for DAC conversion job</td>
 *	</tr>
 *	<tr>
 *		<td>Initial Release</td>
 *	</tr>
 * </table>
 */

/**
 * \page asfdoc_sam0_dac_exqsg Examples for DAC Driver
 *
 * This is a list of the available Quick Start guides (QSGs) and example
 * applications for \ref asfdoc_sam0_dac_group. QSGs are simple examples with
 * step-by-step instructions to configure and use this driver in a selection of
 * use cases. Note that QSGs can be compiled as a standalone application or be
 * added to the user application.
 *
 *  - \subpage asfdoc_sam0_dac_basic_use_case
 * \if DAC_CALLBACK_MODE
 *  - \subpage asfdoc_sam0_dac_basic_use_case_callback
 * \endif
 * \if DAC_DMA_USE_MODE_SELECTION  
 *	- \subpage asfdoc_sam0_adc_dma_use_case_dac_in_dma	
 *	\else  
 *  - \subpage asfdoc_sam0_adc_dma_use_case  
 * \endif  
 *
 * \if DAC_DMA_USE_MODE_SELECTION 
 * \page asfdoc_sam0_adc_dma_use_case_dac_in_dma Quick Start Guide for Using DMA with ADC/DAC  
 * For	this examples, see	
 * \ref asfdoc_sam0_adc_dma_use_case  
 * \endif
 *
 * \page asfdoc_sam0_dac_document_revision_history Document Revision History
 *
 * <table>
 *	<tr>
 *		<th>Doc. Rev.</td>
 *		<th>Date</td>
 *		<th>Comments</td>
 *	</tr>
 *	<tr>
 *		<td>C</td>
 *		<td>01/2014</td>
 *		<td>Add SAMD21 support.</td>
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

#endif /* DAC_H_INCLUDED */
