/**
 * \file per_mode_common_utils.c
 *
 * \brief  Common utilities for both Initiator and Receptor in PER Measurement
 * mode - Performance Analyzer application for AT86RF215
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
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
 */

/*
 * Copyright(c) 2015, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* === INCLUDES ============================================================ */
#include <stdio.h>
#include "tal.h"
#include "tal_internal.h"
#include "tal_helper_2.h"
#include "ieee_const.h"
#include "app_per_mode.h"
#include "app_init.h"
#include "perf_api.h"

/* === TYPES =============================================================== */

/* === GLOBALS============================================================== */

/* === IMPLEMENTATION======================================================= */

/*
 * \brief This function is called rest the application equivalent to soft reset
 *
 */
void app_reset(trx_id_t trx)
{
	/* app reset - this is to make the node to restart application as boot
	 * up */
	init_after_disconnect(trx);

	/* INIT was a success - so change to WAIT_FOR_EVENT state */
	set_main_state(trx, WAIT_FOR_EVENT, NULL);
}

int8_t scale_reg_value_to_ed(uint8_t reg_val)
{
	return ((((MAX_ED_VAL -
	       MIN_ED_VAL) *
	       (reg_val -
	       MIN_ED_REG_VAL)) /
	       (MAX_ED_REG_VAL - MIN_ED_REG_VAL)) + MIN_ED_VAL);
}

uint8_t scale_ed_to_reg_val(int8_t ed_val)
{
	return ((((MAX_ED_REG_VAL -
	       MIN_ED_REG_VAL) *
	       (ed_val -
	       MIN_ED_VAL)) /
	       (MAX_ED_VAL - MIN_ED_VAL)) + MIN_ED_REG_VAL);
}

/**
 * \brief The reverse_float is used for reversing a float variable for
 * supporting BIG ENDIAN systems
 * \param float_val Float variable to be reversed
 */
float reverse_float( const float float_val )
{
	float retuVal;
	char *floatToConvert = (char *)&float_val;
	char *returnFloat = (char *)&retuVal;

	returnFloat[0] = floatToConvert[0];
	returnFloat[1] = floatToConvert[1];
	returnFloat[2] = floatToConvert[2];
	returnFloat[3] = floatToConvert[3];

	return retuVal;
}

/* EOF */
