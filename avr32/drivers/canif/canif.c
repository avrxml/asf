/*****************************************************************************
 *
 * \file
 *
 * \brief CANIF driver for AVR32 UC3.
 *
 * This file contains basic functions for the AVR32 CAN, with support for all
 * modes, settings and clock speeds.
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
