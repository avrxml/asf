/**
 * \file
 *
 * \brief PICOUART driver for SAM.
 *
 * Copyright (C) 2012-2015 Atmel Corporation. All rights reserved.
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

#include "picouart.h"
#include "sysclk.h"
#include "sleepmgr.h"

/* define the source mode */
#define SOURCE_NONE 0
#define SOURCE_START_BIT_MODE 1
#define SOURCE_FULL_FRAME_MODE 2
#define SOURCE_CHARACTER_MATCH_MODE 3

void picouart_get_config_defaults(struct picouart_config *config)
{
	config->action = PICOUART_ACTION_WAKEUP_ON_FULLFRAME;
	config->match = 0;
}

status_code_t picouart_init(struct picouart_dev_inst *const dev_inst, 
		Picouart *hw_dev, struct picouart_config *config)
{
	dev_inst->dev_ptr = hw_dev;

	return picouart_set_config(dev_inst, config);
}

status_code_t picouart_set_config(struct picouart_dev_inst *const dev_inst,
		struct picouart_config *config)
{
	if(PM->PM_PBDMASK & (1 << SYSCLK_PICOUART)) {
		dev_inst->dev_ptr->PICOUART_CFG = config->action
					| PICOUART_CFG_MATCH(config->match);
	} else {
		sysclk_enable_peripheral_clock(dev_inst->dev_ptr);
		dev_inst->dev_ptr->PICOUART_CFG = config->action
					| PICOUART_CFG_MATCH(config->match);
		sysclk_disable_peripheral_clock(dev_inst->dev_ptr);
	}
	return STATUS_OK;
}

void picouart_enable(struct picouart_dev_inst *const dev_inst)
{
	sysclk_enable_peripheral_clock(PICOUART);
	sleepmgr_lock_mode(SLEEPMGR_BACKUP);
	dev_inst->dev_ptr->PICOUART_CR = PICOUART_CR_EN;
}

void picouart_disable(struct picouart_dev_inst *const dev_inst)
{
	dev_inst->dev_ptr->PICOUART_CR = PICOUART_CR_DIS;
	sysclk_disable_peripheral_clock(PICOUART);
	sleepmgr_unlock_mode(SLEEPMGR_BACKUP);
}

bool picouart_is_enabled(struct picouart_dev_inst *const dev_inst)
{
	return (dev_inst->dev_ptr->PICOUART_SR & PICOUART_SR_EN);
}

bool picouart_is_data_ready(struct picouart_dev_inst *const dev_inst)
{
	return (dev_inst->dev_ptr->PICOUART_SR & PICOUART_SR_DRDY);
}

status_code_t picouart_read(struct picouart_dev_inst *const dev_inst, 
	uint8_t *result)
{
	*result = dev_inst->dev_ptr->PICOUART_RHR;

	return STATUS_OK;
}

