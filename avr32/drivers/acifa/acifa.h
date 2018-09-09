/*****************************************************************************
*
* \file
*
* \brief ACIFA driver header for AVR32 UC3.
*
* This file defines a useful set of functions for ACIFA on AVR UC3 devices.
*
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
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
*****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
