/**
 * \file
 *
 * \brief HMATRIX driver for SAM.
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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

#include "hmatrixb.h"
#include "sysclk.h"
#include "sleepmgr.h"

/**
 * \defgroup sam_drivers_hmatrix_group HSB BUS Matrix (HMATRIX)
 *
 * \par Purpose
 *
 * The HSB Bus Matrix implements a multi-layer bus structure that enables
 * parallel access paths between multiple High Speed Bus (HSB) masters and
 * slaves in a system, which increases the overall bandwidth.
 *
 * @{
 */

void hmatrix_init(void)
{
	sysclk_enable_peripheral_clock(HMATRIX);
}

void hmatrix_enable(void)
{
	sysclk_enable_peripheral_clock(HMATRIX);
	sleepmgr_lock_mode(SLEEPMGR_SLEEP_1);
}

void hmatrix_disable(void)
{
	sysclk_disable_peripheral_clock(HMATRIX);
	sleepmgr_unlock_mode(SLEEPMGR_SLEEP_1);
}

void hmatrix_master_ch_set_config(const enum hmatrix_master_id master_id,
		struct hmatrix_master_ch_conf *const config)
{
	Hmatrixb *p_hmatrixb = HMATRIX;
	p_hmatrixb->HMATRIXB_MCFG[master_id].HMATRIXB_MCFG
			= config->burst_type;
}

void hmatrix_slave_ch_set_config(const enum hmatrix_slave_id slave_id,
		struct hmatrix_slave_ch_conf *const config)
{
	Hmatrixb *p_hmatrixb = HMATRIX;
	p_hmatrixb->HMATRIXB_SCFG[slave_id].HMATRIXB_SCFG
			= config->arbitration_type |
			HMATRIXB_SCFG_FIXED_DEFMSTR(config->fixed_def_master_number) |
			config->def_master_type |
			HMATRIXB_SCFG_SLOT_CYCLE(config->slot_cycle);
}

void hmatrix_set_slave_priority(enum hmatrix_slave_id slave_id,
		hmatrix_prs *p_prio)
{
	Hmatrixb *p_hmatrixb = HMATRIX;

	p_hmatrixb->HMATRIXB_PRS[slave_id].HMATRIXB_PRAS = p_prio->HMATRIXB_PRAS;
	p_hmatrixb->HMATRIXB_PRS[slave_id].HMATRIXB_PRBS = p_prio->HMATRIXB_PRBS;
}

void hmatrix_get_slave_priority(enum hmatrix_slave_id slave_id,
		hmatrix_prs *p_prio)
{
	Hmatrixb *p_hmatrixb = HMATRIX;

	p_prio->HMATRIXB_PRAS = p_hmatrixb->HMATRIXB_PRS[slave_id].HMATRIXB_PRAS;
	p_prio->HMATRIXB_PRBS = p_hmatrixb->HMATRIXB_PRS[slave_id].HMATRIXB_PRBS;
}

/* @} */
