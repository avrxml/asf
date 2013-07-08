/**
 * \file
 *
 * \brief SAM D20 Event System Driver
 *
 * Copyright (C) 2012-2013 Atmel Corporation. All rights reserved.
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
#ifndef EVENTS_H_INCLUDED
#define EVENTS_H_INCLUDED

/**
 * \defgroup asfdoc_samd20_events_group SAM D20 Event System Driver
 *
 * This driver for SAM D20 devices provides an interface for the configuration
 * and management of the device's peripheral event channels and users within
 * the device, including the enabling and disabling of peripheral source selection
 * and synchronization of clock domains between various modules.
 *
 * The following peripherals are used by this module:
 *
 *  - EVSYS (Event System Management)
 *
 * The outline of this documentation is as follows:
 *  - \ref asfdoc_samd20_events_prerequisites
 *  - \ref asfdoc_samd20_events_module_overview
 *  - \ref asfdoc_samd20_events_special_considerations
 *  - \ref asfdoc_samd20_events_extra_info
 *  - \ref asfdoc_samd20_events_examples
 *  - \ref asfdoc_samd20_events_api_overview
 *
 *
 * \section asfdoc_samd20_events_prerequisites Prerequisites
 *
 * There are no prerequisites for this module.
 *
 *
 * \section asfdoc_samd20_events_module_overview Module Overview
 *
 * Peripherals within the SAM D20 devices are capable of generating two types of
 * actions in response to given stimulus; they can set a register flag for later
 * intervention by the CPU (using interrupt or polling methods), or they can
 * generate event signals which can be internally routed directly to other
 * peripherals within the device. The use of events allows for direct actions
 * to be performed in one peripheral in response to a stimulus in another
 * without CPU intervention. This can lower the overall power consumption of the
 * system if the CPU is able to remain in sleep modes for longer periods, and
 * lowers the latency of the system response.
 *
 * The event system is comprised of a number of freely configurable Event
 * Channels, plus a number of fixed Event Users. Each Event Channel can be
 * configured to select the input peripheral that will generate the events on
 * the channel, as well as the synchronization path and edge detection mode.
 * The fixed-function Event Users, connected to peripherals within the device,
 * can then subscribe to an Event Channel in a one-to-many relationship in order
 * to receive events as they are generated. An overview of the event system
 * chain is shown in
 * \ref asfdoc_samd20_events_module_overview_fig "the figure below".
 *
 * \anchor asfdoc_samd20_events_module_overview_fig
 * \dot
 * digraph overview {
 *   rankdir=LR;
 *   node [label="Source\nPeripheral" shape=ellipse style=filled fillcolor=lightgray] src_peripheral;
 *   node [label="Event\nChannel a" shape=square style=""] event_gen0;
 *   node [label="Event\nUser x" shape=square style=""] event_user0;
 *   node [label="Event\nUser y" shape=square style=""] event_user1;
 *   node [label="Destination\nPeripheral" shape=ellipse style=filled fillcolor=lightgray] dst_peripheral0;
 *   node [label="Destination\nPeripheral" shape=ellipse style=filled fillcolor=lightgray] dst_peripheral1;
 *
 *   src_peripheral -> event_gen0;
 *   event_gen0     -> event_user0;
 *   event_gen0     -> event_user1;
 *   event_user0    -> dst_peripheral0;
 *   event_user1    -> dst_peripheral1;
 * }
 * \enddot
 *
 * There are many different events that can be routed in the device, which can
 * then trigger many different actions. For example, an Analog Comparator module
 * could be configured to generate an event when the input signal rises above
 * the compare threshold, which then triggers a Timer module to capture the
 * current count value for later use.
 *
 * \subsection asfdoc_samd20_events_module_overview_event_channels Event Channels
 * The Event module in each device consists of several channels, which can be
 * freely linked to an event generator (i.e. a peripheral within the device
 * that is capable of generating events). Each channel can be individually
 * configured to select the generator peripheral, signal path and edge detection
 * applied to the input event signal, before being passed to any event user(s).
 *
 * Event channels can support multiple users within the device in a standardized
 * manner; when an Event User is linked to an Event Channel, the channel will
 * automatically handshake with all attached users to ensure that all modules
 * correctly receive and acknowledge the event.
 *
 * \subsection asfdoc_samd20_events_module_overview_event_users Event Users
 * Event Users are able to subscribe to an Event Channel, once it has been
 * configured. Each Event User consists of a fixed connection to one of the
 * peripherals within the device (for example, an ADC module or Timer module)
 * and is capable of being connected to a single Event Channel.
 *
 * \subsection asfdoc_samd20_events_module_overview_edge_detection Edge Detection
 * For asynchronous events, edge detection on the event input is not possible,
 * and the event signal must be passed directly between the event generator and
 * event user. For synchronous and re-synchronous events, the input signal from
 * the event generator must pass through an edge detection unit, so that only
 * the rising, falling or both edges of the event signal triggers an action in
 * the event user.
 *
 * \subsection asfdoc_samd20_events_module_overview_path_selection Path Selection
 * The event system in the SAM0 devices supports three signal path types from
 * the event generator to event users: asynchronous, synchronous and
 * re-synchronous events.
 *
 * \subsubsection asfdoc_samd20_events_module_overview_path_selection_async Asynchronous Paths
 * Asynchronous event paths allow for an asynchronous connection between the
 * event generator and event user(s), when the source and destination
 * peripherals share the same \ref asfdoc_samd20_system_clock_group "Generic Clock"
 * channel. In this mode the event is propagated between the source and
 * destination directly to reduce the event latency, thus no edge detection is
 * possible. The asynchronous event chain is shown in
 * \ref asfdoc_samd20_events_module_async_path_fig "the figure below".
 *
 * \anchor asfdoc_samd20_events_module_async_path_fig
 * \dot
 * digraph overview {
 *   rankdir=LR;
 *   node [label="Source\nPeripheral" shape=ellipse style=filled fillcolor=lightgray] src_peripheral;
 *   node [label="<f0> EVSYS | <f1> Event\nChannel/User" fillcolor=white style="dashed" shape=record] events_chan;
 *   node [label="Destination\nPeripheral" shape=ellipse style=filled fillcolor=lightgray] dst_peripheral;
 *
 *   src_peripheral -> events_chan;
 *   events_chan    -> dst_peripheral;
 *
 * }
 * \enddot
 * \note Identically shaped borders in the diagram indicate a shared generic clock channel
 *
 * \subsubsection asfdoc_samd20_events_module_overview_path_selection_sync Synchronous Paths
 * Synchronous event paths can be used when the source and destination
 * peripherals, as well as the generic clock to the event system itself, use
 * different generic clock channels. This case introduces additional latency in
 * the event propagation due to the addition of a synchronizer and edge detector
 * on the input event signal, however this allows modules of different clocks to
 * communicate events to one-another. The synchronous event chain is shown in
 * \ref asfdoc_samd20_events_module_sync_path_fig "the figure below".
 *
 * \anchor asfdoc_samd20_events_module_sync_path_fig
 * \dot
 * digraph overview {
 *   rankdir=LR;
 *   node [label="Source\nPeripheral" shape=ellipse style="filled, dashed" fillcolor=lightgray] src_peripheral;
 *   node [label="<f0> EVSYS | <f1> Event\nChannel/User" fillcolor=white shape=record] events_chan;
 *   node [label="Destination\nPeripheral" shape=ellipse style="filled, dotted" fillcolor=lightgray] dst_peripheral;
 *
 *   src_peripheral -> events_chan;
 *   events_chan    -> dst_peripheral;
 *
 * }
 * \enddot
 * \note Identically shaped borders in the diagram indicate a shared generic clock channel
 *
 * \subsubsection asfdoc_samd20_events_module_overview_path_selection_resync Re-synchronous Paths
 * Re-synchronous event paths are a special form of synchronous events, where
 * the event users share the same generic clock channel as the event system
 * module itself, but the event generator does not. This reduces latency by
 * performing the synchronization across the event source and event user clock
 * domains once within the event channel itself, rather than in each event user.
 * The re-synchronous event chain is shown in
 * \ref asfdoc_samd20_events_module_resync_path_fig "the figure below".
 *
 * \anchor asfdoc_samd20_events_module_resync_path_fig
 * \dot
 * digraph overview {
 *   rankdir=LR;
 *   node [label="Source\nPeripheral" shape=ellipse style="filled, dashed" fillcolor=lightgray] src_peripheral;
 *   node [label="<f0> EVSYS | <f1> Event\nChannel/User" fillcolor=white shape=record] events_chan;
 *   node [label="Destination\nPeripheral" shape=ellipse style=filled fillcolor=lightgray] dst_peripheral;
 *
 *   src_peripheral -> events_chan;
 *   events_chan    -> dst_peripheral;
 *
 * }
 * \enddot
 * \note Identically shaped borders in the diagram indicate a shared generic clock channel
 *
 * \subsection asfdoc_samd20_events_module_overview_physical Physical Connection
 *
 * \ref asfdoc_samd20_events_module_int_connections_fig "The diagram below"
 * shows how this module is interconnected within the device.
 *
 * \anchor asfdoc_samd20_events_module_int_connections_fig
 * \dot
 * digraph overview {
 *   rankdir=LR;
 *   node [label="Source\nPeripherals" shape=ellipse style=filled fillcolor=lightgray] src_peripheral;
 *
 *   subgraph driver {
 *     node [label="<f0> EVSYS | <f1> Event Channels" fillcolor=white shape=record] events_chan;
 *     node [label="<f0> EVSYS | <f1> Event Users" fillcolor=white shape=record] events_user;
 *   }
 *
 *   node [label="Destination\nPeripherals" shape=ellipse style=filled fillcolor=lightgray] dst_peripheral;
 *
 *   src_peripheral -> events_chan:f1 [label="Source\nMUXs"];
 *   events_chan:f1 -> events_user:f1 [label="Channel\nMUXs"];
 *   events_user:f1 -> dst_peripheral;
 * }
 * \enddot
 *
 *
 * \section asfdoc_samd20_events_special_considerations Special Considerations
 *
 * There are no special considerations for this module.
 *
 *
 * \section asfdoc_samd20_events_extra_info Extra Information for EVENTS
 *
 * For extra information see \ref asfdoc_samd20_events_extra. This includes:
 *  - \ref asfdoc_samd20_events_extra_acronyms
 *  - \ref asfdoc_samd20_events_extra_dependencies
 *  - \ref asfdoc_samd20_events_extra_errata
 *  - \ref asfdoc_samd20_events_extra_history
 *
 *
 * \section asfdoc_samd20_events_examples Examples
 *
 * For a list of examples related to this driver, see
 * \ref asfdoc_samd20_events_exqsg.
 *
 *
 * \section asfdoc_samd20_events_api_overview API Overview
 * @{
 */

#include <compiler.h>
#include <system.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Event System channel selection
 *
 * Enum containing the possible event channel selections.
 */
enum events_channel {
	/** Event channel 0 */
	EVENT_CHANNEL_0 = 0,
	/** Event channel 1 */
	EVENT_CHANNEL_1 = 1,
	/** Event channel 2 */
	EVENT_CHANNEL_2 = 2,
	/** Event channel 3 */
	EVENT_CHANNEL_3 = 3,
	/** Event channel 4 */
	EVENT_CHANNEL_4 = 4,
	/** Event channel 5 */
	EVENT_CHANNEL_5 = 5,
	/** Event channel 6 */
	EVENT_CHANNEL_6 = 6,
	/** Event channel 7 */
	EVENT_CHANNEL_7 = 7,
};

/**
 * \brief Event System synchronous channel edge detection configurations.
 *
 * Enum containing the possible event channel edge detection configurations,
 * to select when the synchronous event triggers according to a particular
 * trigger edge.
 *
 * \note For asynchronous events, edge detection is not possible and selection
 *       of any value other than \ref EVENT_EDGE_NONE will have no effect. For
 *       synchronous events, a valid edge detection mode other than
 *       \ref EVENT_EDGE_NONE must be set for events to be generated.
 */
enum events_edge {
	/** Event channel disabled (or direct pass-through for asynchronous
	 *  events). */
	EVENT_EDGE_NONE    = 0,
	/** Event channel triggers on rising edges. */
	EVENT_EDGE_RISING  = 1,
	/** Event channel triggers on falling edges. */
	EVENT_EDGE_FALLING = 2,
	/** Event channel triggers on both edges. */
	EVENT_EDGE_BOTH    = 3,
};

/**
 * \brief Event System channel path type configurations.
 *
 * Enum containing the possible event channel paths, to select between
 * digital clock synchronization settings for each channel.
 */
enum events_path {
	/** Event is synchronized to the digital clock. */
	EVENT_PATH_SYNCHRONOUS   = 0,
	/** Event is re-synchronized between the source and destination digital
	 *  clock domains. */
	EVENT_PATH_RESYNCHRONOUS = 1,
	/** Event is asynchronous to the digital clock. */
	EVENT_PATH_ASYNCHRONOUS  = 2,
};

/**
 * \brief Event System Channel configuration structure.
 *
 * Configuration structure for an Event System channel. This structure
 * should be initialized by the \ref events_chan_get_config_defaults() function
 * before being modified by the user application.
 *
 * \note Selecting a GLCK will only make take effect when
 * \ref EVENT_PATH_SYNCHRONOUS and \ref EVENT_PATH_RESYNCHRONOUS paths are used.
 */
struct events_chan_config {
	/** Edge detection for synchronous event channels, from \ref events_edge. */
	enum events_edge edge_detection;
	/** Path of the event system, from \ref events_path. */
	enum events_path path;
	/** Event generator module that should be attached to the event channel,
	 *  an \c EVSYS_ID_GEN_* constant from the device header files. */
	uint8_t generator_id;
	/** GCLK generator used to clock the specific event channel */
	enum gclk_generator clock_source;
};

/**
 * \brief Event System user MUX configuration structure.
 *
 * Configuration structure for an Event System subscriber multiplexer
 * channel. This structure should be initialized by the
 * \ref events_user_get_config_defaults() function before being modified by the
 * user application.
 */
struct events_user_config {
	/** Event channel ID that should be attached to the user MUX. */
	enum events_channel event_channel_id;
};


/**
 * \name Configuration and initialization
 * @{
 */

void events_init(void);

/** @} */


/**
 * \name Configuration and initialization (Event Channel)
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
 * \return Synchronization status of the underlying hardware module(s).
 *
 * \retval true  if the module has completed synchronization
 * \retval false if the module synchronization is ongoing
 */
static inline bool events_is_syncing(void)
{
	/* No synchronization on this architecture */
	return false;
}

/**
 * \brief Initializes an Event System configuration structure to defaults.
 *
 * Initializes a given Event System channel configuration structure to a
 * set of known default values. This function should be called on all new
 * instances of these configuration structures before being modified by the
 * user application.
 *
 * The default configuration is as follows:
 *  \li Event channel uses asynchronous path between the source and destination
 *  \li Event channel is set not to use edge detection as the path is
 *      asynchronous and no intervention in the event system can take place
 *  \li Event channel is not connected to an Event Generator
 *  \li Event channel generic clock source is GLCK_GENERATOR_0
 *  \li Event channel generic clock does not run in standby mode
 *
 * \param[out] config    Configuration structure to initialize to default values
 */
static inline void events_chan_get_config_defaults(
		struct events_chan_config *const config)
{
	/* Sanity check arguments */
	Assert(config);

	/* Default configuration values */
	config->edge_detection = EVENT_EDGE_NONE;
	config->path           = EVENT_PATH_ASYNCHRONOUS;
	config->generator_id   = 0;
	config->clock_source   = GCLK_GENERATOR_0;
}

void events_chan_set_config(
		const enum events_channel event_channel,
		struct events_chan_config *const config);

/** @} */


/**
 * \name Configuration and initialization (Event User)
 * @{
 */

/**
 * \brief Initializes an Event System user MUX configuration structure to defaults.
 *
 * Initializes a given Event System user MUX configuration structure to a
 * set of known default values. This function should be called on all new
 * instances of these configuration structures before being modified by the
 * user application.
 *
 * The default configuration is as follows:
 *  \li User MUX input event is connected to source channel 0
 *
 * \param[out] config  Configuration structure to initialize to default values
 */
static inline void events_user_get_config_defaults(
		struct events_user_config *const config)
{
	/* Sanity check arguments */
	Assert(config);

	/* Default configuration values */
	config->event_channel_id = EVENT_CHANNEL_0;
}

void events_user_set_config(
		const uint8_t user,
		struct events_user_config *const config);

/** @} */


/**
 * \name Channel Control and Management
 * @{
 */

/**
 * \brief Retrieves the busy status of an Event channel.
 *
 * Reads the status of the requested Event channel, to determine if the
 * channel is currently busy.
 *
 * \pre The specified event channel must be configured and enabled.
 *
 * \param[in] event_channel  Event channel to check
 *
 * \return Status of the specified event channel.
 *
 * \retval true  If the channel is ready to be used
 * \retval false If the channel is currently busy
 */
static inline bool events_chan_is_ready(
		const enum events_channel event_channel)
{
	/* Get the channel number from the enum selector */
	uint8_t channel = (uint8_t)event_channel;

	/* Event channel busy/user busy flags are interleaved, 8 channels to a
	 * 16-bit word */
	uint8_t status_halfword = channel / 8;
	uint8_t status_bitindex = channel % 8;

	/* Make it a 16-bit array to be able to work on the upper and lower
	 * 16-bits */
	uint16_t *channel_status_ptr = (uint16_t*)(&EVSYS->CHSTATUS.reg);

	/* Determine if the specified channel is currently busy */
	if (channel_status_ptr[status_halfword] &
			EVSYS_CHSTATUS_CHBUSY0(1 << status_bitindex)) {
		return false;
	}

	return true;
}

/**
 * \brief Retrieves the channel status of the users subscribed to an Event channel.
 *
 * Reads the status of the requested Event channel users, to determine if the
 * users of the event channel are currently busy.
 *
 * \pre The specified event channel must be configured and enabled.
 *
 * \param[in] event_channel  Event channel to check
 *
 * \return Status of the specified event channel subscribers.
 *
 * \retval true  If all channel subscribers are ready
 * \retval false If one or more channel subscribers are currently busy
 */
static inline bool events_user_is_ready(
		const enum events_channel event_channel)
{
	/* Get the channel number from the enum selector */
	uint8_t channel = (uint8_t)event_channel;

	/* Event channel busy/user busy flags are interleaved, 8 channels to a
	 * 16-bit word */
	uint8_t status_halfword = channel / 8;
	uint8_t status_bitindex = channel % 8;

	/* Make it a 16-bit array to be able to work on the upper and lower
	 * 16-bits */
	uint16_t *channel_status_ptr = (uint16_t*)(&EVSYS->CHSTATUS.reg);

	/* Determine if the specified channel users(s) are currently ready */
	if (channel_status_ptr[status_halfword] &
			EVSYS_CHSTATUS_USRRDY0(1 << status_bitindex)) {
		return true;
	}

	return false;
}

/**
 * \brief Software triggers an event channel.
 *
 * Triggers an event channel via software, setting an event notification to
 * the channel subscriber module(s) of the channel.
 *
 * \pre The specified event channel must be configured and enabled.
 *
 * \param[in] event_channel  Event channel to trigger
 */
static inline void events_chan_software_trigger(
		const enum events_channel event_channel)
{
	/* Get the channel number from the enum selector */
	uint8_t channel = (uint8_t)event_channel;

	/* Trigger the appropriate event channel - must be performed as a single
	 * 8-bit write as mandated in the datasheet for the event system module */
	EVSYS->CHANNEL.reg = (channel << EVSYS_CHANNEL_CHANNEL_Pos) |
			EVSYS_CHANNEL_SWEVT;
}

/** @} */

#ifdef __cplusplus
}
#endif

/** @} */

/**
 * \page asfdoc_samd20_events_extra  Extra Information for EVENTS Driver
 *
 * \section asfdoc_samd20_events_extra_acronyms Acronyms
 * Below is a table listing the acronyms used in this module, along with their
 * intended meanings.
 *
 * <table>
 *	<tr>
 *		<th>Acronym</th>
 *		<th>Description</th>
 *	</tr>
 *	<tr>
 *		<td>CPU</td>
 *		<td>Central Processing Unit</td>
 *	</tr>
 *	<tr>
 *		<td>MUX</td>
 *		<td>Multiplexer</td>
 *	</tr>
 * </table>
 *
 *
 * \section asfdoc_samd20_events_extra_dependencies Dependencies
 * This driver has the following dependencies:
 *
 *  - \ref asfdoc_samd20_system_clock_group "System Clock Driver"
 *
 *
 * \section asfdoc_samd20_events_extra_errata Errata
 * There are no errata related to this driver.
 *
 *
 * \section asfdoc_samd20_events_extra_history Module History
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
 *		<td>Initial Release</td>
 *	</tr>
 * </table>
 */

/**
 * \page asfdoc_samd20_events_exqsg Examples for EVENTS Driver
 *
 * This is a list of the available Quick Start guides (QSGs) and example
 * applications for \ref asfdoc_samd20_events_group. QSGs are simple examples with
 * step-by-step instructions to configure and use this driver in a selection of
 * use cases. Note that QSGs can be compiled as a standalone application or be
 * added to the user application.
 *
 *  - \subpage asfdoc_samd20_events_basic_use_case
 *
 * \page asfdoc_samd20_events_document_revision_history Document Revision History
 *
 * <table>
 *	<tr>
 *		<th>Doc. Rev.</td>
 *		<th>Date</td>
 *		<th>Comments</td>
 *	</tr>
 *	<tr>
 *		<td>A</td>
 *		<td>06/2013</td>
 *		<td>Initial release</td>
 *	</tr>
 * </table>
 */

#endif /* EVENTS_H_INCLUDED */
