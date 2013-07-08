/**
 * \file
 *
 * \brief SAM D20 Digital-to-Analog Interrupt Driver
 *
 * Copyright (C) 2013 Atmel Corporation. All rights reserved.
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
#include "dac.h"
#include "dac_callback.h"

struct dac_module *_dac_instances[DAC_INST_NUM];

/**
 * \brief Registers an asynchronous callback function with the driver.
 *
 * Registers an asynchronous callback with the DAC driver, fired when a callback
 * condition occurs.
 *
 * \param[in,out] dac_module  Pointer to the DAC software instance struct
 * \param[in]     callback    Pointer to the callback function to register
 * \param[in]     type        Type of callback function to register
 *
 * \return Status of the registration operation.
 * \retval STATUS_OK                   The callback was registered successfully.
 * \retval STATUS_ERR_INVALID_ARG      If an invalid callback type was supplied.
 * \retval STATUS_ERR_UNSUPPORTED_DEV  If a callback that requires event driven
 *                                     mode was specified with a DAC instance
 *                                     configured in non-event mode.
 */
enum status_code dac_register_callback(
		struct dac_module *const dac_module,
		const dac_callback_t callback,
		const enum dac_callback type)
{
	/* Sanity check arguments */
	Assert(dac_module);
	Assert(callback);

	/* DAC interrupts require it to be driven by events to work, fail if in
	 * unbuffered (polled) mode */
	if (dac_module->start_on_event == false) {
		return STATUS_ERR_UNSUPPORTED_DEV;
	}

	if ((uint8_t)type < DAC_CALLBACK_N)
	{
		dac_module->callback[(uint8_t)type] = callback;
		return STATUS_OK;
	}

	return STATUS_ERR_INVALID_ARG;
}

/**
 * \brief Unregisters an asynchronous callback function with the driver.
 *
 * Unregisters an asynchronous callback with the DAC driver, removing it
 * from the internal callback registration table.
 *
 * \param[in,out] dac_module  Pointer to the DAC software instance struct
 * \param[in]     type        Type of callback function to unregister
 *
 * \return Status of the de-registration operation.
 * \retval STATUS_OK                   The callback was unregistered successfully.
 * \retval STATUS_ERR_INVALID_ARG      If an invalid callback type was supplied.
 * \retval STATUS_ERR_UNSUPPORTED_DEV  If a callback that requires event driven
 *                                     mode was specified with a DAC instance
 *                                     configured in non-event mode.
 */
enum status_code dac_unregister_callback(
		struct dac_module *const dac_module,
		const enum dac_callback type)
{
	/* Sanity check arguments */
	Assert(dac_module);

	/* DAC interrupts require it to be driven by events to work, fail if in
	 * unbuffered (polled) mode */
	if (dac_module->start_on_event == false) {
		return STATUS_ERR_UNSUPPORTED_DEV;
	}

	if ((uint8_t)type < DAC_CALLBACK_N)
	{
		dac_module->callback[(uint8_t)type] = NULL;
		return STATUS_OK;
	}

	return STATUS_ERR_INVALID_ARG;
}

/**
 * \brief Enables asynchronous callback generation for a given channel and type.
 *
 * Enables asynchronous callbacks for a given logical DAC channel and type. This
 * must be called before a DAC channel will generate callback events.
 *
 * \param[in,out] dac_module  Pointer to the DAC software instance struct
 * \param[in]     channel     Logical channel to enable callback generation for
 * \param[in]     type        Type of callback function callbacks to enable
 *
 * \return Status of the callback enable operation.
 * \retval STATUS_OK                   The callback was enabled successfully.
 * \retval STATUS_ERR_INVALID_ARG      If an invalid callback type was supplied.
 * \retval STATUS_ERR_UNSUPPORTED_DEV  If a callback that requires event driven
 *                                     mode was specified with a DAC instance
 *                                     configured in non-event mode.
 */
enum status_code dac_chan_enable_callback(
		struct dac_module *const dac_module,
		const uint32_t channel,
		const enum dac_callback type)
{
	/* Sanity check arguments */
	Assert(dac_module);
	Assert(dac_module->hw);

	UNUSED(channel);

	Dac *const dac_hw = dac_module->hw;

	/* DAC interrupts require it to be driven by events to work, fail if in
	 * unbuffered (polled) mode */
	if (dac_module->start_on_event == false) {
		return STATUS_ERR_UNSUPPORTED_DEV;
	}

	switch ((uint8_t)type)
	{
	case DAC_CALLBACK_DATA_UNDERRUN:
		dac_hw->INTENSET.reg = DAC_INTFLAG_UNDERRUN;
		return STATUS_OK;

	case DAC_CALLBACK_DATA_EMPTY:
		dac_hw->INTENSET.reg = DAC_INTFLAG_EMPTY;
		return STATUS_OK;
	}

	return STATUS_ERR_INVALID_ARG;
}

/**
 * \brief Disables asynchronous callback generation for a given channel and type.
 *
 * Disables asynchronous callbacks for a given logical DAC channel and type.
 *
 * \param[in,out] dac_module  Pointer to the DAC software instance struct
 * \param[in]     channel     Logical channel to disable callback generation for
 * \param[in]     type        Type of callback function callbacks to disable
 *
 * \return Status of the callback disable operation.
 * \retval STATUS_OK                   The callback was disabled successfully.
 * \retval STATUS_ERR_INVALID_ARG      If an invalid callback type was supplied.
 * \retval STATUS_ERR_UNSUPPORTED_DEV  If a callback that requires event driven
 *                                     mode was specified with a DAC instance
 *                                     configured in non-event mode.
 */
enum status_code dac_chan_disable_callback(
		struct dac_module *const dac_module,
		const uint32_t channel,
		const enum dac_callback type)
{
	/* Sanity check arguments */
	Assert(dac_module);
	Assert(dac_module->hw);

	UNUSED(channel);

	Dac *const dac_hw = dac_module->hw;

	/* DAC interrupts require it to be driven by events to work, fail if in
	 * unbuffered (polled) mode */
	if (dac_module->start_on_event == false) {
		return STATUS_ERR_UNSUPPORTED_DEV;
	}

	switch ((uint8_t)type)
	{
	case DAC_CALLBACK_DATA_UNDERRUN:
		dac_hw->INTENCLR.reg = DAC_INTFLAG_UNDERRUN;
		return STATUS_OK;

	case DAC_CALLBACK_DATA_EMPTY:
		dac_hw->INTENCLR.reg = DAC_INTFLAG_EMPTY;
		return STATUS_OK;
	}

	return STATUS_ERR_INVALID_ARG;
}

/** \internal
 *  Internal handler for DAC module interrupts.
 */
static void _dac_interrupt_handler(const uint8_t instance)
{
	struct dac_module *module = _dac_instances[instance];
	Dac *const dac_hw = module->hw;

	if (dac_hw->INTFLAG.reg & DAC_INTFLAG_UNDERRUN) {
		dac_hw->INTFLAG.reg |= DAC_INTFLAG_UNDERRUN;

		if (module->callback) {
			module->callback[DAC_CALLBACK_DATA_UNDERRUN](0);
		}
	}

	if (dac_hw->INTFLAG.reg & DAC_INTFLAG_EMPTY) {
		dac_hw->INTFLAG.reg |= DAC_INTFLAG_EMPTY;

		if (module->callback) {
			module->callback[DAC_CALLBACK_DATA_EMPTY](0);
		}
	}
}

/** Handler for the DAC hardware module interrupt. */
void DAC_Handler(void)
{
	_dac_interrupt_handler(0);
}
