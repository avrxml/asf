/*****************************************************************************
*
* \file
*
* \brief ACIFA driver header for AVR32 UC3.
*
* This file defines a useful set of functions for ACIFA on AVR UC3 devices.
*
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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
*****************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef _ACIFA_H_
#define _ACIFA_H_

/**
 * \defgroup group_avr32_drivers_acifa ACIFA - Analog Comparator Interface
 *
 * Analog Comparator Interface is able to control two Analog Comparators (AC)
 * with identical behavior.
 *
 * @{
 */

#include <avr32/io.h>
#include "compiler.h"

/** \name Positive Inputs Comparator 0 Instance A used by the ACIFA
 * @{
 */
#define AVR32_ACIFA_AC0A_INSELP_AC0AP0                        0
#define AVR32_ACIFA_AC0A_INSELP_AC0AP1                        1
#define AVR32_ACIFA_AC0A_INSELP_DAC0_INT                      2
/** @} */

/** \name Negative Inputs Comparator 0 Instance A used by the ACIFA
 * @{
 */
#define AVR32_ACIFA_AC0A_INSELN_AC0AN0                        0
#define AVR32_ACIFA_AC0A_INSELN_AC0AN1                        1
#define AVR32_ACIFA_AC0A_INSELN_AC0BP0                        2
#define AVR32_ACIFA_AC0A_INSELN_AC0BP1                        3
#define AVR32_ACIFA_AC0A_INSELN_VDDANA_SCALED                 4
#define AVR32_ACIFA_AC0A_INSELN_DAC1_INT                      5
/** @} */

/** \name Positive Inputs Comparator 0 Instance B used by the ACIFA
 * @{
 */
#define AVR32_ACIFA_AC0B_INSELP_AC0AN0                        0
#define AVR32_ACIFA_AC0B_INSELP_AC0AN1                        1
#define AVR32_ACIFA_AC0B_INSELP_AC0BP0                        2
#define AVR32_ACIFA_AC0B_INSELP_AC0BP1                        3
#define AVR32_ACIFA_AC0B_INSELP_DAC0_INT                      4
#define AVR32_ACIFA_AC0B_INSELP_VDDANA_SCALED                 5
/** @} */

/** \name Negative Inputs Comparator 0 Instance B used by the ACIFA
 * @{
 */
#define AVR32_ACIFA_AC0B_INSELN_AC0BN0                        0
#define AVR32_ACIFA_AC0B_INSELN_AC0BN1                        1
#define AVR32_ACIFA_AC0B_INSELN_DAC1_INT                      2
/** @} */

/** \name Positive Inputs Comparator 1 Instance A used by the ACIFA
 * @{
 */
#define AVR32_ACIFA_AC1A_INSELP_AC1AP0                        0
#define AVR32_ACIFA_AC1A_INSELP_AC1AP1                        1
#define AVR32_ACIFA_AC1A_INSELP_DAC0_INT                      2
/** @} */

/** \name Negative Inputs Comparator 1 Instance A used by the ACIFA
 * @{
 */
#define AVR32_ACIFA_AC1A_INSELN_AC1AN0                        0
#define AVR32_ACIFA_AC1A_INSELN_AC1AN1                        1
#define AVR32_ACIFA_AC1A_INSELN_AC1BP0                        2
#define AVR32_ACIFA_AC1A_INSELN_AC1BP1                        3
#define AVR32_ACIFA_AC1A_INSELN_VDDANA_SCALED                 4
#define AVR32_ACIFA_AC1A_INSELN_DAC1_INT                      5
/** @} */

/** \name Positive Inputs Comparator 1 Instance B used by the ACIFA
 * @{
 */
#define AVR32_ACIFA_AC1B_INSELP_AC1AN0                        0
#define AVR32_ACIFA_AC1B_INSELP_AC1AN1                        1
#define AVR32_ACIFA_AC1B_INSELP_AC1BP0                        2
#define AVR32_ACIFA_AC1B_INSELP_AC1BP1                        3
#define AVR32_ACIFA_AC1B_INSELP_DAC0_INT                      4
#define AVR32_ACIFA_AC1B_INSELP_VDDANA_SCALED                 5
/** @} */

/** \name Negative Inputs Comparator 1 Instance B used by the ACIFA
 * @{
 */
#define AVR32_ACIFA_AC1B_INSELN_AC1BN0                        0
#define AVR32_ACIFA_AC1B_INSELN_AC1BN1                        1
#define AVR32_ACIFA_AC1B_INSELN_DAC1_INT                      2
/** @} */

/** \name Comparator Selection
 * @{
 */
#define ACIFA_COMP_SELA       1
#define ACIFA_COMP_SELB       2
/** @} */

/** \name VCCSCALE Selection
 * @{
 */
#define ACIFA_VCCSCALE_SELA   1
#define ACIFA_VCCSCALE_SELB   2
/** @} */

void acifa_configure(volatile avr32_acifa_t *acifa,
		uint8_t comp_sel,
		uint8_t input_p,
		uint8_t input_n,
		uint32_t pb_hz);
void acifa_configure_window( volatile avr32_acifa_t *acifa,
		uint8_t input_p,
		uint8_t input_n,
		uint8_t common_input,
		uint32_t pb_hz);
void acifa_start(volatile avr32_acifa_t *acifa,
		uint8_t comp_sel);
void acifa_stop(volatile avr32_acifa_t *acifa,
		uint8_t comp_sel);
void acifa_start_window(volatile avr32_acifa_t *acifa);
void acifa_enable_output(volatile avr32_acifa_t *acifa,
		uint8_t comp_sel);
void acifa_configure_hysteresis(volatile avr32_acifa_t *acifa,
		uint8_t comp_sel,
		uint8_t level);
void acifa_configure_vccscale(volatile avr32_acifa_t *acifa,
		uint8_t vccscale_sel,
		uint8_t scale);
bool acifa_is_aca_inp_higher(volatile avr32_acifa_t *acifa);
bool acifa_is_acb_inp_higher(volatile avr32_acifa_t *acifa);
bool acifa_is_ci_inside_window(volatile avr32_acifa_t *acifa);
void acifa_clear_flags(volatile avr32_acifa_t *acifa,
		uint32_t flag_mask);
void acifa_enable_interrupt_inp_higher(volatile avr32_acifa_t *acifa,
		uint8_t comp_sel);
void acifa_enable_interrupt_inp_lower(volatile avr32_acifa_t *acifa,
		uint8_t comp_sel);
void acifa_enable_interrupt_toggle(volatile avr32_acifa_t *acifa,
		uint8_t comp_sel);
void acifa_enable_interrupt(volatile avr32_acifa_t *acifa, uint32_t flag_mask);

/** @} */

#endif  /* _ACIFA_H_ */
