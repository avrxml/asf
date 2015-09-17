/*****************************************************************************
 *
 * \file
 *
 * \brief CANIF driver for AVR32 UC3.
 *
 * This file contains basic functions for the AVR32 CAN, with support for all
 * modes, settings and clock speeds.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#include "compiler.h"
#include "preprocessor.h"
#include "canif.h"

void canif_clear_all_mob(uint8_t ch,
		uint8_t nb_mob)
{
	uint8_t mob_number;

	for (mob_number = 0; mob_number < nb_mob; mob_number++) {
		CANIF_clr_mob(ch,mob_number)
	}
}

uint8_t canif_get_mob_free(uint8_t ch)
{
	uint8_t mob_number = CANIF_mob_get_mob_free(ch);

	if ((mob_number & 0x20) == 0x20)  {
		return (NO_MOB);
	}

	return mob_number;
}

uint8_t canif_get_mob_status(uint8_t ch,
		uint8_t mob)
{
	uint8_t mob_status;

	// Test if MOb ENABLE or DISABLE
	if (!(CANIF_mob_enable_status(ch, mob))) {
		return(MOB_DISABLE);
	}

	mob_status  = CANIF_mob_get_status(ch,mob);

	mob_status &= ((1<<AVR32_CANIF_MOBSR_DLCW_OFFSET) |
			(1<<AVR32_CANIF_MOBSR_TXOK_OFFSET) |
			(1<<AVR32_CANIF_MOBSR_RXOK_OFFSET));

	if ((mob_status == MOB_RX_COMPLETED) ||
			(mob_status==MOB_TX_COMPLETED) ||
			(mob_status==MOB_RX_COMPLETED_DLCW)) {
		return mob_status;
	}

	// Test if last mob selected is the mob interested
	if (CANIF_get_interrupt_lastmob_selected(ch) != mob) {
		return MOB_NOT_COMPLETED;
	}

	// If MOb is ENABLE & NOT_COMPLETED, test if CAN is in ERROR
	mob_status = CANIF_get_interrupt_error_status(ch);
	if (mob_status != 0) {
		return mob_status;
	}

	// If CANSTMOB = 0 then MOB_NOT_COMPLETED
	return MOB_NOT_COMPLETED;
}

uint8_t canif_fixed_baudrate(uint8_t ch)
{
  CANIF_conf_bt(ch);
  return 1;
}
