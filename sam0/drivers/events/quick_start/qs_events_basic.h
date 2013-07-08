/**
 * \file
 *
 * \brief SAM D20 Event System Driver Quick Start
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

/**
 * \page asfdoc_samd20_events_basic_use_case Quick Start Guide for EVENTS - Basic
 *
 * In this use case, the EVENT module is configured for:
 *  \li One generator attached to event channel 0
 *  \li Synchronous event path with rising edge detection on the input
 *  \li One user attached to the configured event channel
 *
 * This use case configures an event channel within the device, attaching it to
 * a peripheral's event generator, and attaching a second peripheral's event
 * user to the configured channel. The event channel is then software triggered.
 *
 * \section asfdoc_samd20_events_basic_use_case_setup Setup
 *
 * \subsection asfdoc_samd20_events_basic_use_casesetup_prereq Prerequisites
 * There are no special setup requirements for this use-case.
 *
 * \subsection asfdoc_samd20_events_basic_use_casesetup_code Code
 * Copy-paste the following setup code to your user application:
 * \snippet qs_events_basic.c setup
 *
 * Add to user application initialization (typically the start of \c main()):
 * \snippet qs_events_basic.c setup_init
 *
 * \subsection asfdoc_samd20_events_basic_use_casesetup_flow Workflow
 * -# Create an event channel configuration struct, which can be filled out to
 *    adjust the configuration of a single event channel.
 *  \snippet qs_events_basic.c setup_1
 * -# Initialize the event channel configuration struct with the module's
 *    default values.
 *    \note This should always be performed before using the configuration
 *          struct to ensure that all values are initialized to known default
 *          settings.
 *
 *  \snippet qs_events_basic.c setup_2
 * -# Adjust the configuration struct to request that the channel be attached
 *    to the specified event generator, that rising edges of the event signal
 *    be detected on the channel and that the synchronous event path be used.
 *  \snippet qs_events_basic.c setup_3
 * -# Configure the channel using the configuration structure.
 *    \note The existing configuration struct may be re-used, as long as any
 *          values that have been altered from the default settings are taken
 *          into account by the user application.
 *
 *  \snippet qs_events_basic.c setup_4
 *
 * -# Create an event user configuration struct, which can be filled out to
 *    adjust the configuration of a single event user.
 *  \snippet qs_events_basic.c setup_5
 * -# Initialize the event user configuration struct with the module's default
 *    values.
 *    \note This should always be performed before using the configuration
 *          struct to ensure that all values are initialized to known default
 *          settings.
 *
 *  \snippet qs_events_basic.c setup_6
 * -# Adjust the configuration struct to request that the previously configured
 *    event channel be used as the event source for the user.
 *  \snippet qs_events_basic.c setup_7
 * -# Configure the event user using the configuration structure.
 *    \note The existing configuration struct may be re-used, as long as any
 *          values that have been altered from the default settings are taken
 *          into account by the user application.
 *
 *  \snippet qs_events_basic.c setup_8
 *
 * \section asfdoc_samd20_events_basic_use_case_main Use Case
 *
 * \subsection asfdoc_samd20_events_basic_use_casecode_code Code
 * Copy-paste the following code to your user application:
 * \snippet qs_events_basic.c main
 *
 * \subsection events_basic_use_case_code_flow Workflow
 * -# Wait for the even channel to become ready to accept a new event trigger.
 *  \snippet qs_events_basic.c main_1
 * -# Perform a software event trigger on the configured event channel.
 *  \snippet qs_events_basic.c main_2
 */
