/**
 * \file
 *
 * \brief SAM D20 External Interrupt Driver
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
#include "extint.h"
#include <system.h>
#include <system_interrupt.h>

/**
 * \internal
 * Internal driver device instance struct.
 */
struct _extint_module _extint_dev;

/**
 * \brief Resets and disables the External Interrupt driver.
 *
 * Resets and disables the External Interrupt driver, resetting all hardware
 * module registers to their power-on defaults.
 */
void extint_reset(void)
{
	Eic *const eics[EIC_INST_NUM] = EIC_INSTS;

	/* Disable all EIC modules before resetting them. */
	extint_disable();

	/* Reset all EIC hardware modules. */
	for (uint32_t i = 0; i < EIC_INST_NUM; i++) {
		eics[i]->CTRL.reg |= EIC_CTRL_SWRST;
	}

	while (extint_is_syncing()) {
		/* Wait for all hardware modules to complete synchronization */
	}
}

/**
 * \brief Enables the External Interrupt driver.
 *
 * Enables EIC modules ready for use. This function must be called before
 * attempting to use any NMI or standard external interrupt channel functions.
 */
void extint_enable(void)
{
	Eic *const eics[EIC_INST_NUM] = EIC_INSTS;

	/* Configure the generic clock for the module */
	struct system_gclk_chan_config gclk_chan_conf;
	system_gclk_chan_get_config_defaults(&gclk_chan_conf);
	gclk_chan_conf.source_generator = GCLK_GENERATOR_0;
	system_gclk_chan_set_config(EIC_GCLK_ID, &gclk_chan_conf);
	system_gclk_chan_enable(EIC_GCLK_ID);

	/* Enable all EIC hardware modules. */
	for (uint32_t i = 0; i < EIC_INST_NUM; i++) {
		eics[i]->CTRL.reg |= EIC_CTRL_ENABLE;
	}

	while (extint_is_syncing()) {
		/* Wait for all hardware modules to complete synchronization */
	}

#if EXTINT_CALLBACK_MODE == true
	/* Clear callback registration table */
	for (uint8_t j = 0; j < EXTINT_CALLBACKS_MAX; j++) {
		_extint_dev.callbacks[j] = NULL;
	}

	system_interrupt_enable(SYSTEM_INTERRUPT_MODULE_EIC);
#endif
}

/**
 * \brief Disables the External Interrupt driver.
 *
 * Disables EIC modules that were previously started via a call to
 * \ref extint_enable().
 */
void extint_disable(void)
{
	Eic *const eics[EIC_INST_NUM] = EIC_INSTS;

	/* Disable all EIC hardware modules. */
	for (uint32_t i = 0; i < EIC_INST_NUM; i++) {
		eics[i]->CTRL.reg &= ~EIC_CTRL_ENABLE;
	}

	while (extint_is_syncing()) {
		/* Wait for all hardware modules to complete synchronization */
	}
}

/**
 * \brief Writes an External Interrupt channel configuration to the hardware module.
 *
 * Writes out a given configuration of an External Interrupt channel
 * configuration to the hardware module. If the channel is already configured,
 * the new configuration will replace the existing one.
 *
 * \param[in] channel   External Interrupt channel to configure
 * \param[in] config    Configuration settings for the channel

 */
void extint_chan_set_config(
		const uint8_t channel,
		const struct extint_chan_conf *const config)
{
	/* Sanity check arguments */
	Assert(config);

	struct system_pinmux_config pinmux_config;
	system_pinmux_get_config_defaults(&pinmux_config);

	pinmux_config.mux_position = config->gpio_pin_mux;
	pinmux_config.direction    = SYSTEM_PINMUX_PIN_DIR_INPUT;
	pinmux_config.input_pull   = (enum system_pinmux_pin_pull)config->gpio_pin_pull;
	system_pinmux_pin_set_config(config->gpio_pin, &pinmux_config);

	/* Get a pointer to the module hardware instance */
	Eic *const EIC_module = _extint_get_eic_from_channel(channel);

	uint32_t config_pos = (4 * (channel % 8));
	uint32_t new_config;

	/* Determine the channel's new edge detection configuration */
	new_config = (config->detection_criteria << EIC_CONFIG_SENSE0_Pos);

	/* Enable the hardware signal filter if requested in the config */
	if (config->filter_input_signal) {
		new_config |= EIC_CONFIG_FILTEN0;
	}

	/* Clear the existing and set the new channel configuration */
	EIC_module->CONFIG[channel / 8].reg
		= (EIC_module->CONFIG[channel / 8].reg &
			~((EIC_CONFIG_SENSE0_Msk | EIC_CONFIG_FILTEN0) << config_pos)) |
			(new_config << config_pos);

	/* Set the channel's new wake up mode setting */
	if (config->wake_if_sleeping) {
		EIC_module->WAKEUP.reg |=  (1UL << channel);
	} else {
		EIC_module->WAKEUP.reg &= ~(1UL << channel);
	}
}

/**
 * \brief Writes an External Interrupt NMI channel configuration to the hardware module.
 *
 *  Writes out a given configuration of an External Interrupt NMI channel
 *  configuration to the hardware module. If the channel is already configured,
 *  the new configuration will replace the existing one.
 *
 *  \param[in] nmi_channel   External Interrupt NMI channel to configure
 *  \param[in] config        Configuration settings for the channel
 *
 * \returns Status code indicating the success or failure of the request.
 * \retval  STATUS_OK                   Configuration succeeded
 * \retval  STATUS_ERR_PIN_MUX_INVALID  An invalid pin mux value was supplied
 * \retval  STATUS_ERR_BAD_FORMAT       An invalid detection mode was requested
 */
enum status_code extint_nmi_set_config(
		const uint8_t nmi_channel,
		const struct extint_nmi_conf *const config)
{
	/* Sanity check arguments */
	Assert(config);

	if ((EIC_NMI_NO_DETECT_ALLOWED == 0) &&
			(config->detection_criteria == EXTINT_DETECT_NONE)) {
		return STATUS_ERR_BAD_FORMAT;
	}

	struct system_pinmux_config pinmux_config;
	system_pinmux_get_config_defaults(&pinmux_config);

	pinmux_config.mux_position = config->gpio_pin_mux;
	pinmux_config.direction    = SYSTEM_PINMUX_PIN_DIR_INPUT;
	pinmux_config.input_pull   = SYSTEM_PINMUX_PIN_PULL_UP;
	pinmux_config.input_pull   = (enum system_pinmux_pin_pull)config->gpio_pin_pull;
	system_pinmux_pin_set_config(config->gpio_pin, &pinmux_config);

	/* Get a pointer to the module hardware instance */
	Eic *const EIC_module = _extint_get_eic_from_channel(nmi_channel);

	uint32_t new_config;

	/* Determine the NMI's new edge detection configuration */
	new_config = (config->detection_criteria << EIC_NMICTRL_NMISENSE_Pos);

	/* Enable the hardware signal filter if requested in the config */
	if (config->filter_input_signal) {
		new_config |= EIC_NMICTRL_NMIFILTEN;
	}

	EIC_module->NMICTRL.reg = new_config;

	return STATUS_OK;
}

/**
 * \brief Enables an External Interrupt event output.
 *
 *  Enables one or more output events from the External Interrupt module. See
 *  \ref extint_events "here" for a list of events this module supports.
 *
 *  \note Events cannot be altered while the module is enabled.
 *
 *  \param[in] events    Struct containing flags of events to enable
 */
void extint_enable_events(
		struct extint_events *const events)
{
	/* Sanity check arguments */
	Assert(events);

	/* Array of available EICs. */
	Eic *const eics[EIC_INST_NUM] = EIC_INSTS;

	/* Update the event control register for each physical EIC instance */
	for (uint32_t i = 0; i < EIC_INST_NUM; i++) {
		uint32_t event_mask = 0;

		/* Create an enable mask for the current EIC module */
		for (uint32_t j = 0; j < 32; j++) {
			if (events->generate_event_on_detect[(32 * i) + j]) {
				event_mask |= (1UL << j);
			}
		}

		/* Enable the masked events */
		eics[i]->EVCTRL.reg |= event_mask;
	}
}

/**
 * \brief Disables an External Interrupt event output.
 *
 *  Disables one or more output events from the External Interrupt module. See
 *  \ref extint_events "here" for a list of events this module supports.
 *
 *  \note Events cannot be altered while the module is enabled.
 *
 *  \param[in] events    Struct containing flags of events to disable
 */
void extint_disable_events(
		struct extint_events *const events)
{
	/* Sanity check arguments */
	Assert(events);

	/* Array of available EICs. */
	Eic *const eics[EIC_INST_NUM] = EIC_INSTS;

	/* Update the event control register for each physical EIC instance */
	for (uint32_t i = 0; i < EIC_INST_NUM; i++) {
		uint32_t event_mask = 0;

		/* Create a disable mask for the current EIC module */
		for (uint32_t j = 0; j < 32; j++) {
			if (events->generate_event_on_detect[(32 * i) + j]) {
				event_mask |= (1UL << j);
			}
		}

		/* Disable the masked events */
		eics[i]->EVCTRL.reg &= ~event_mask;
	}
}
