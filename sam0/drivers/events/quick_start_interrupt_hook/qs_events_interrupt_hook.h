/**
 * \file
 *
 * \brief SAM D20/D21/R21 Event System Driver Quick Start
 *
 * Copyright (C) 2014 Atmel Corporation. All rights reserved.
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
 * \page asfdoc_sam0_events_interrupt_hook_use_case Quick Start Guide for EVENTS - interrupt hooks
 *
 * In this use case, the EVENT module is configured for:
 *  \li Synchronous event path with rising edge detection
 *  \li TC4 as event generator on the allocated event channel
 *  \li No event channel user attached
 *  \li An event interrupt hook is used to execute some code when an event is detected
 *
 * In this usecase TC4 is used as event generator, generating events on overflow.
 * No user attached, counting events on the channel. To able to execute
 * some code when an event is detected, an interrupt hook is used. The interrupt
 * hook will also count the number of events detected and toggle a led on the board
 * each time an event is detected.
 *
 * \note Because this example is showing how to setup an interrupt hook there is no
 *       user attached to the user.
 *
 * \section asfdoc_sam0_events_interrup_hook_setup Setup
 *
 * \subsection asfdoc_sam0_events_interrupt_hook_setup_prereq Prerequisites
 * There are no special setup requirements for this use-case.
 *
 * \subsection asfdoc_sam0_events_interrupt_hook_setup_code Code
 * Copy-paste the following setup code to your user application:
 * \snippet qs_events_interrupt_hook.c setup
 *
 * Add to user application initialization (typically the start of \c main()):
 * \snippet qs_events_interrupt_hook.c setup_init
 *
 * \subsection asfdoc_sam0_events_interrupt_hook_setup_flow Workflow
 * -# Create an event channel configuration structure instance which will contain the
 *    configuration for the event.
 *  \snippet qs_events_interrupt_hook.c setup_1
 *  \br
 *
 * -# Initialize the event channel configuration struct with safe default values.
 *    \note This shall always be performed before using the configuration
 *          struct to ensure that all members are initialized to known default
 *          values.
 *
 *  \snippet qs_events_interrupt_hook.c setup_2
 *  \br
 *
 * -# Adjust the configuration structure
 *      \li Use EXAMPLE_EVENT_GENRATOR as event generator
 *      \li Detect events on rising edge
 *      \li Use the synchronous event path
 *      \li Use GCLK Generator 0 as event channel clock source
 *
 *  \snippet qs_events_interrupt_hook.c setup_3
 *  \br
 *
 * -# Allocate and configure the channel using the configuration structure.
 *
 *  \snippet qs_events_interrupt_hook.c setup_4
 *  \br
 *
 * -# Make sure there is no user attached. To attach an user, change the value
 *    of EXAMPLE_EVENT_USER to the correct peripheral ID.
 *  \snippet qs_events_interrupt_hook.c setup_5
 *  \br
 *
 * -# Create config_tc and config_events configuration structure instances.
 *  \snippet qs_events_interrupt_hook.c setup_6
 *  \br
 *
 * -# Initialize the TC module configuration structure with safe default values.
 * \note This function shall always be called on new configuration structure instances
 *       to make sure that all structure members is initialized.
 *
 * \snippet qs_events_interrupt_hook.c setup_7
 *
 * -# Adjust the config_tc structure
 *	\li Set counter size to 8bit
 *	\li Set wave generation mode to normal frequency generation
 *	\li Use GCLK generator 1 to as tc module clock source
 *	\li Prescale the input clock with 64
 *
 * \snippet qs_events_interrupt_hook.c setup_8
 *
 * -# Initialize, configure and assosiate the tc_instance handle with the TC hardware pointed to by TC_MODULE
 * \snippet qs_events_interrupt_hook.c setup_9
 *
 * -# Adjust the config_events structure to enable event generation on overflow in the timer and then
 *    enable the event configuration
 * \snippet qs_events_interrupt_hook.c setup_10
 *
 * -# Enable the timer/counter module
 * \snippet qs_events_interrupt_hook.c setup_11
 *
 * -# Create a new interrupt hook and use the function event_counter as hook code
 * \snippet qs_events_interrupt_hook.c setup_12
 *
 * -# Add the newly created hook to the interrupt hook queue and enable the event detected interrupt
 * \snippet qs_events_interrupt_hook.c setup_13
 *
 * -# Example interrupt hook code. If the hook was triggered by a event detected interrupt on the
 *    event channel this code will toggle the led on the Xplained PRO board and increase the value
 *    of the event_count variable. The interrupt then acknowledged.
 *
 * \snippet qs_events_interrupt_hook.c setup_14
 *
 * \section asfdoc_sam0_events_interrupt_hook_main Use Case
 *
 * \subsection asfdoc_sam0_events_interrupt_hook_code_code Code
 * Copy-paste the following code to your user application:
 * \snippet qs_events_interrupt_hook.c main
 *
 * \subsection asfdoc_sam0_events_interrupt_hook_code_flow Workflow
 * -# Wait for the even channel to become ready.
 * \snippet qs_events_interrupt_hook.c main_1
 *
 * -# Start the timer/counter
 * \snippet qs_events_interrupt_hook.c main_2
 */
