/**
 * \file
 *
 * \brief PARC driver for SAM.
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

#include "parc.h"
#include "sysclk.h"

/**
 * \brief Initializes the PARC
 *
 * Initializes the PARC device struct and the hardware module based on the
 * given configuration struct values.
 *
 * \param[out] module_inst Pointer to the PARC software instance struct
 * \param[in]  hw          Pointer to the PARC module instance
 * \param[in]  config      Pointer to the configuration struct
 *
 * \return Status of the initialization procedure
 * \retval STATUS_OK                The initialization was successful
 * \retval STATUS_ERR_INVALID_ARG   Invalid argument(s) were provided
 */
enum status_code parc_init(
	struct parc_module *const module_inst,
	Parc *const hw,
	struct parc_config *const config)
{
	/* Turn on the clock for PARC*/
	sysclk_enable_peripheral_clock(hw);

	/* Associate the software module instance with the hardware module */
	module_inst->hw = hw;

#if PARC_CALLBACK_MODE == true
	for (uint8_t i = 0; i < PARC_CALLBACK_N; i++) {
		module_inst->callback[i] = NULL;
	}

	module_inst->registered_callback_mask = 0;
	module_inst->enabled_callback_mask = 0;

	parc_module_instance = module_inst;
#endif
	return parc_set_config(module_inst, config);
}

/**
 * \Writes an PARC configuration to the hardware module
 *
 * Writes a given PARC module configuration to the hardware module.
 *
 * \param[out] module_inst  Pointer to the PARC software instance struct
 * \param[in]  config       Pointer to configuration struct
 *
 * \return Status of the configuration procedure
 * \retval STATUS_OK               The configuration was successful
 */
enum status_code parc_set_config(
	struct parc_module *const module_inst,
	struct parc_config *config)
{
	Assert(module_inst);
	Assert(module_inst->hw);
	uint32_t tmp_cfg = 0;

	Parc *const parc_module_hw = module_inst->hw;

	tmp_cfg |= PARC_CFG_SMODE(config->smode);
	tmp_cfg |= PARC_CFG_DSIZE(config->dsize);

	if (config->capture_mode == PARC_BOTH_CAPTURE) {
		tmp_cfg &= ~(PARC_CFG_HALF);
	} else if (config->capture_mode == PARC_ODD_CAPTURE) {
		tmp_cfg |= (PARC_CFG_HALF | PARC_CFG_ODD);
	} else if (config->capture_mode == PARC_EVEN_CAPTURE) {
		tmp_cfg |= PARC_CFG_HALF;
		tmp_cfg &= ~(PARC_CFG_ODD);
	}

	if (config->sampling_edge == PARC_FALLING_EDGE) {
		tmp_cfg |= PARC_CFG_EDGE;
	} else if (config->sampling_edge == PARC_RISING_EDGE) {
		tmp_cfg &= ~(PARC_CFG_EDGE);
	}

	parc_module_hw->PARC_CFG = tmp_cfg;

	return STATUS_OK;
}
