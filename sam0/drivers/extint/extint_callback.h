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
#ifndef EXTINT_CALLBACK_H_INCLUDED
#define EXTINT_CALLBACK_H_INCLUDED

#include <compiler.h>
#include "extint.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup asfdoc_samd20_extint_group
 *
 * @{
 */

#if !defined(EXTINT_CALLBACKS_MAX) || defined(__DOXYGEN__)
#  warning  EXTINT_CALLBACKS_MAX is not defined, assuming a default value.

/** Configuration option, setting the maximum number of callbacks which can be
 *  registered with the driver. This option may be overridden in the module
 *  configuration header file \c conf_extint.h.
 */
#  define EXTINT_CALLBACKS_MAX 10
#endif

/** \name Callback configuration and initialization
 * @{
 */

/** Type definition for an EXTINT module callback function. */
typedef void (*extint_callback_t)(uint32_t channel);

/** Enum for the possible callback types for the EXTINT module. */
enum extint_callback_type
{
	/** Callback type for when an external interrupt detects the configured
	 *  channel criteria (i.e. edge or level detection)
	 */
	EXTINT_CALLBACK_TYPE_DETECT,
};

enum status_code extint_register_callback(
	const extint_callback_t callback,
	const enum extint_callback_type type);

enum status_code extint_unregister_callback(
	const extint_callback_t callback,
	const enum extint_callback_type type);

/** @} */

/** \name Callback enabling and disabling (channel)
 * @{
 */

enum status_code extint_chan_enable_callback(
	const uint32_t channel,
	const enum extint_callback_type type);

enum status_code extint_chan_disable_callback(
	const uint32_t channel,
	const enum extint_callback_type type);

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif
