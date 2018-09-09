/**
 * \file per_mode_common_utils.c
 *
 * \brief  Common utilities for both Initiator and Receptor in PER Measurement
 * mode - Performance Analyzer application for AT86RF215
 *
 * Copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
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
 */

/*
 * Copyright(c) 2015-2018, Microchip Technology Inc All rights reserved.
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
