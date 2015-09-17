/**
 * \file
 *
 * \brief SAM4L PARC driver(callback) for SAM.
 *
 * Copyright (C) 2013-2015 Atmel Corporation. All rights reserved.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef PARC_CALLBACK_H_INCLUDED
#define PARC_CALLBACK_H_INCLUDED

#include <parc.h>

/** \addtogroup asfdoc_sam_drivers_parc_group
 * @{
 */

/// @cond
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

enum status_code parc_register_callback(struct parc_module *const module,
		parc_callback_t const callback_func,
		enum parc_callback_type callback_type);

enum status_code parc_unregister_callback(struct parc_module *const module,
		enum parc_callback_type callback_type);
		
/**
 * \brief Enable a PARC interrupt callback function.
 *
 * \note The callback function will be called from the PARC interrupt handler
 * when the callback's corresponding interrupt is asserted.
 *
 * \param[in,out] module_inst Driver structure pointer
 * \param[in]  type           \ref parc_callback_type "Interrupt callback" to enable
 *
 * \return The status of the interrupt callback enable operation.
 * \retval STATUS_OK Interrupt callback enabled successfully
 */
static inline enum status_code parc_enable_callback(
	struct parc_module *const module_inst,
	enum parc_callback_type type)
{
	/* Sanity check arguments */
	Assert(module_inst);

	/* Enable callback */
	module_inst->enabled_callback_mask |= (1 << type);

	return STATUS_OK;
}

/**
 * \brief Disable a PARC interrupt callback function.
 *
 * \param[in,out] module_inst Driver structure pointer
 * \param[in]  type           \ref parc_callback_type "Interrupt callback" to disable
 *
 * \return The status of the interrupt callback disable operation.
 * \retval STATUS_OK Interrupt callback disabled successfully
 */
static inline enum status_code parc_disable_callback(
	struct parc_module *const module_inst,
	enum parc_callback_type type)
{
	/* Sanity check arguments */
	Assert(module_inst);

	/* Disable callback */
	module_inst->enabled_callback_mask &= ~(1 << type);

	return STATUS_OK;
}

/// @cond
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond

/** @} */

#endif  /* PARC_CALLBACK_H_INCLUDED */
