/**
 *
 * \file
 *
 * \brief CMCC software driver for SAM.
 *
 * This file defines a useful set of functions for the CMCC on SAM devices.
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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

#include "cmcc.h"

/**
 * \brief Initializes a CMCC configuration structure to defaults.
 *
 *  Initializes a given CMCC configuration structure to a set of
 *  known default values. This function should be called on all new
 *  instances of these configuration structures before being modified by the
 *  user application.
 *
 *  The default configuration is as follows:
 *  - the monitor counter is activated.
 *  - Data hit counter mode.
 *
 *  \param cfg    Configuration structure to initialize to default values.
 */
void cmcc_get_config_defaults(struct cmcc_config *const cfg)
{
	/* Sanity check arguments */
	Assert(cfg);

	/* Default configuration values */
	cfg->cmcc_monitor_enable = true;
	cfg->cmcc_mcfg_mode = CMCC_DHIT_COUNT_MODE;
#if SAMG55
	cfg->cmcc_cfg_cache_size = CMCC_PROG_CSIZE_1KB;
#endif
}

/**
 * \brief Initialize the CMCC module.
 *
 * \param p_cmcc Pointer to an CMCC instance.
 * \param cfg Pointer to CMCC configuration.
 *
 * \retval true if the initialization was successful.
 * \retval false if initialization failed.
 */
bool cmcc_init(Cmcc *const p_cmcc, struct cmcc_config *const cfg)
{
	/* Sanity check arguments */
	Assert(cfg);

	/* Initialize the Cache Controller Monitor if it is enabled */
	if (true == cfg->cmcc_monitor_enable) {
		cmcc_set_config(p_cmcc, cfg);
		cmcc_enable_monitor(p_cmcc);
	}

	return true;
}

/**
 * \brief Enable Cache Controller.
 *
 * \param p_cmcc Pointer to an CMCC instance.
 *
 */
void cmcc_enable(Cmcc *const p_cmcc)
{
	if (CMCC_SR_CSTS != cmcc_get_status(p_cmcc)) {
		p_cmcc->CMCC_CTRL |= CMCC_CTRL_CEN;
	}
}

/**
 * \brief Cache Controller Invalidate by Line Operation.
 *
 * \param p_cmcc Pointer to an CMCC instance.
 *
 */
void cmcc_invalidate_line(Cmcc *const p_cmcc, uint32_t cmcc_way,
		uint32_t cmcc_index)
{
	cmcc_disable(p_cmcc);
	while (cmcc_get_status(p_cmcc) & CMCC_SR_CSTS) {
	}
	p_cmcc->CMCC_MAINT1 = (cmcc_way << CMCC_MAINT1_WAY_Pos) |
			(cmcc_index << CMCC_MAINT1_INDEX_Pos);
	cmcc_enable(p_cmcc);
}
