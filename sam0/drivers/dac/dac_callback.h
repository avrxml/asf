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
#ifndef DAC_CALLBACK_H_INCLUDED
#define DAC_CALLBACK_H_INCLUDED

#include <compiler.h>
#include "dac.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup asfdoc_samd20_dac_group
 *
 * @{
 */

#if !defined(__DOXYGEN__)
extern struct dac_module *_dac_instances[DAC_INST_NUM];
#endif

/** \name Callback configuration and initialization
 * @{
 */

/** Type definition for a DAC module callback function. */
typedef void (*dac_callback_t)(uint8_t channel);

/** Enum for the possible callback types for the DAC module. */
enum dac_callback
{
	/** Callback type for when a DAC channel data empty condition occurs
	 *  (requires event triggered mode). */
	DAC_CALLBACK_DATA_EMPTY,
	/** Callback type for when a DAC channel data under-run condition occurs
	 *  (requires event triggered mode). */
	DAC_CALLBACK_DATA_UNDERRUN,
#if !defined(__DOXYGEN__)
	DAC_CALLBACK_N,
#endif
};

enum status_code dac_register_callback(
		struct dac_module *const module,
		const dac_callback_t callback,
		const enum dac_callback type);

enum status_code dac_unregister_callback(
		struct dac_module *const module,
		const enum dac_callback type);

/** @} */

/** \name Callback enabling and disabling (Channel)
 * @{
 */

enum status_code dac_chan_enable_callback(
		struct dac_module *const module,
		const uint32_t channel,
		const enum dac_callback type);

enum status_code dac_chan_disable_callback(
		struct dac_module *const module,
		const uint32_t channel,
		const enum dac_callback type);

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif
