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
#include "extint_callback.h"

/**
 * \internal
 * Internal driver device instance struct, declared in the main module driver.
 */
extern struct _extint_module _extint_dev;

/**
 * \brief Registers an asynchronous callback function with the driver.
 *
 * Registers an asynchronous callback with the EXTINT driver, fired when a
 * channel detects the configured channel detection criteria
 * (e.g. edge or level). Callbacks are fired once for each detected channel.
 *
 * \note NMI channel callbacks cannot be registered via this function; the
 *       device's NMI interrupt should be hooked directly in the user
 *       application and the NMI flags manually cleared via
 *       \ref extint_nmi_clear_detected().
 *
 * \param[in] callback  Pointer to the callback function to register
 * \param[in] type      Type of callback function to register
 *
 * \return Status of the registration operation.
 * \retval STATUS_OK               The callback was registered successfully.
 * \retval STATUS_ERR_INVALID_ARG  If an invalid callback type was supplied.
 * \retval STATUS_ERR_NO_MEMORY    No free entries were found in the registration
 *                                 table.
 */
enum status_code extint_register_callback(
	const extint_callback_t callback,
	const enum extint_callback_type type)
{
	/* Sanity check arguments */
	Assert(callback);

	if (type != EXTINT_CALLBACK_TYPE_DETECT) {
		Assert(false);
		return STATUS_ERR_INVALID_ARG;
	}

	for (uint8_t i = 0; i < EXTINT_CALLBACKS_MAX; i++) {
		if (_extint_dev.callbacks[i] == NULL) {
			_extint_dev.callbacks[i] = callback;
			return STATUS_OK;
		}
	}

	Assert(false);
	return STATUS_ERR_NO_MEMORY;
}

/**
 * \brief Unregisters an asynchronous callback function with the driver.
 *
 * Unregisters an asynchronous callback with the EXTINT driver, removing it
 * from the internal callback registration table.
 *
 * \param[in] callback  Pointer to the callback function to unregister
 * \param[in] type      Type of callback function to unregister
 *
 * \return Status of the de-registration operation.
 * \retval STATUS_OK               The callback was Unregistered successfully.
 * \retval STATUS_ERR_INVALID_ARG  If an invalid callback type was supplied.
 * \retval STATUS_ERR_BAD_ADDRESS  No matching entry was found in the
 *                                 registration table.
 */
enum status_code extint_unregister_callback(
	const extint_callback_t callback,
	const enum extint_callback_type type)
{
	/* Sanity check arguments */
	Assert(callback);

	if (type != EXTINT_CALLBACK_TYPE_DETECT) {
		Assert(false);
		return STATUS_ERR_INVALID_ARG;
	}

	for (uint8_t i = 0; i < EXTINT_CALLBACKS_MAX; i++) {
		if (_extint_dev.callbacks[i] == callback) {
			_extint_dev.callbacks[i] = NULL;
			return STATUS_OK;
		}
	}

	return STATUS_ERR_BAD_ADDRESS;
}

/**
 * \brief Enables asynchronous callback generation for a given channel and type.
 *
 * Enables asynchronous callbacks for a given logical external interrupt channel
 * and type. This must be called before an external interrupt channel will
 * generate callback events.
 *
 * \param[in] channel  Logical channel to enable callback generation for
 * \param[in] type     Type of callback function callbacks to enable
 *
 * \return Status of the callback enable operation.
 * \retval STATUS_OK               The callback was enabled successfully.
 * \retval STATUS_ERR_INVALID_ARG  If an invalid callback type was supplied.
 */
enum status_code extint_chan_enable_callback(
	const uint32_t channel,
	const enum extint_callback_type type)
{
	if (type == EXTINT_CALLBACK_TYPE_DETECT) {
		Eic *const eic = _extint_get_eic_from_channel(channel);

		eic->INTENSET.reg = (1UL << channel);
	}
	else {
		Assert(false);
		return STATUS_ERR_INVALID_ARG;
	}

	return STATUS_OK;
}

/**
 * \brief Disables asynchronous callback generation for a given channel and type.
 *
 * Disables asynchronous callbacks for a given logical external interrupt
 * channel and type.
 *
 * \param[in] channel  Logical channel to disable callback generation for
 * \param[in] type     Type of callback function callbacks to disable
 *
 * \return Status of the callback disable operation.
 * \retval STATUS_OK               The callback was disabled successfully.
 * \retval STATUS_ERR_INVALID_ARG  If an invalid callback type was supplied.
 */
enum status_code extint_chan_disable_callback(
	const uint32_t channel,
	const enum extint_callback_type type)
{
	if (type == EXTINT_CALLBACK_TYPE_DETECT) {
		Eic *const eic = _extint_get_eic_from_channel(channel);

		eic->INTENCLR.reg = (1UL << channel);
	}
	else {
		Assert(false);
		return STATUS_ERR_INVALID_ARG;
	}

	return STATUS_OK;
}

/** Handler for the EXTINT hardware module interrupt. */
void EIC_Handler(void)
{
	/* Find any triggered channels, run associated callback handlers */
	for (uint32_t i = 0; i < (32 * EIC_INST_NUM); i++) {
		if (extint_chan_is_detected(i)) {
			/* Find any associated callback entries in the callback table */
			for (uint8_t j = 0; j < EXTINT_CALLBACKS_MAX; j++) {
				if (_extint_dev.callbacks[j] != NULL) {
					/* Run the registered callback */
					_extint_dev.callbacks[j](i);
				}
			}

			extint_chan_clear_detected(i);
		}
	}
}
