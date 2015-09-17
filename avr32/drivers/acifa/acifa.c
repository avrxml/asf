/*****************************************************************************
*
* \file
*
* \brief ACIFA header for AVR UC3.
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

#include <avr32/io.h>
#include "compiler.h"
#include "acifa.h"

static bool acifa_is_aca_ready(volatile avr32_acifa_t *acifa)
{
	return (acifa->sr & AVR32_ACIFA_SR_ACARDY_MASK) != 0;
}

static bool acifa_is_acb_ready(volatile avr32_acifa_t *acifa)
{
	return (acifa->sr & AVR32_ACIFA_SR_ACBRDY_MASK) != 0;
}

static bool acifa_is_wf_ready(volatile avr32_acifa_t *acifa)
{
	return (acifa->sr & AVR32_ACIFA_SR_WFRDY_MASK) != 0;
}

/** \brief Configure ACIFA Normal Mode. Mandatory to call.
 *
 * \param acifa      Base address of the ACIFA
 * \param comp_sel   Comparator Selection
 * \param input_p    Input Positive Selection
 * \param input_n    Input Negative Selection
 * \param pb_hz      Peripheral Bus frequency
 */
void acifa_configure(volatile avr32_acifa_t *acifa,
		uint8_t comp_sel,
		uint8_t input_p,
		uint8_t input_n,
		uint32_t pb_hz)
{
	Assert( acifa != NULL );
	if (comp_sel == ACIFA_COMP_SELA) {
		acifa->confa
			= ((input_p <<
				AVR32_ACIFA_CONFA_INSELP_OFFSET) &
				AVR32_ACIFA_CONFA_INSELP_MASK) |
				((input_n <<
				AVR32_ACIFA_CONFA_INSELN_OFFSET) &
				AVR32_ACIFA_CONFA_INSELN_MASK);
	} else {
		acifa->confb
			= ((input_p <<
				AVR32_ACIFA_CONFB_INSELP_OFFSET) &
				AVR32_ACIFA_CONFB_INSELP_MASK) |
				((input_n <<
				AVR32_ACIFA_CONFB_INSELN_OFFSET) &
				AVR32_ACIFA_CONFB_INSELN_MASK);
	}

	/* Startup Time of up to 10us max. */
	acifa->sut = (pb_hz / 100000);
}

/** \brief Configure ACIFA Window Mode. Mandatory to call.
 *
 * \param acifa           Base address of the ACIFA
 * \param input_p         Input Positive Selection for window mode
 * \param input_n         Input Negative Selection for window mode
 * \param common_input    Common Input Selection for window mode
 * \param pb_hz           Peripheral Bus frequency
 */
void acifa_configure_window( volatile avr32_acifa_t *acifa,
		uint8_t input_p,
		uint8_t input_n,
		uint8_t common_input,
		uint32_t pb_hz)
{
	Assert( acifa != NULL );

	acifa->wconf
		|= ((common_input <<
			AVR32_ACIFA_WCONF_WINSEL_OFFSET) &
			AVR32_ACIFA_WCONF_WINSEL_MASK);
	acifa->confa
		= ((input_p <<
			AVR32_ACIFA_CONFA_INSELP_OFFSET) &
			AVR32_ACIFA_CONFA_INSELP_MASK);
	acifa->confb
		= ((input_n <<
			AVR32_ACIFA_CONFB_INSELN_OFFSET) &
			AVR32_ACIFA_CONFB_INSELN_MASK);

	/* Startup Time of up to 10us max. */
	acifa->sut = (pb_hz / 100000);
}

/** \brief Enable Output on comparator
 *  \param acifa  Base address of the ACIFA
 *  \param comp_sel      Comparator Selection
 */
void acifa_enable_output(volatile avr32_acifa_t *acifa,
		uint8_t comp_sel)
{
	if (comp_sel == ACIFA_COMP_SELA) {
		acifa->confa |= AVR32_ACIFA_CONFA_OEN_MASK;
	} else {
		acifa->confb |= AVR32_ACIFA_CONFB_OEN_MASK;
	}
}

/** \brief Configure Hysteresis on comparator
 *  \param *acifa        Base address of the ACIFA
 *  \param comp_sel      Comparator Selection
 *  \param level         Level On Hysteresis
 */
void acifa_configure_hysteresis(volatile avr32_acifa_t *acifa,
		uint8_t comp_sel,
		uint8_t level)
{
	Assert( acifa != NULL );

	/* Hysteresis Configuration */
	if (comp_sel == ACIFA_COMP_SELA) {
		acifa->confa
			|= ((level <<
				AVR32_ACIFA_CONFA_HS_OFFSET) &
				AVR32_ACIFA_CONFA_HS_MASK);
	} else {
		acifa->confb
			|= ((level <<
				AVR32_ACIFA_CONFB_HS_OFFSET) &
				AVR32_ACIFA_CONFB_HS_MASK);
	}
}

/** \brief Configure VCC Scale on comparator
 *  \param acifa         Base address of the ACIFA
 *  \param vcc_sel       VCC Scale Selection
 *  \param scale         Scale
 */
void acifa_configure_vccscale(volatile avr32_acifa_t *acifa,
		uint8_t vcc_sel,
		uint8_t scale)
{
	Assert( acifa != NULL );

	/* VCC Scale Configuration */
	if (vcc_sel == ACIFA_VCCSCALE_SELA) {
		acifa->scfa
			|= ((scale <<
				AVR32_ACIFA_SCFA_SCALEFACTOR_OFFSET) &
				AVR32_ACIFA_SCFA_SCALEFACTOR_MASK) |
				AVR32_ACIFA_SCFA_EN_MASK;
	} else {
		acifa->scfb
			|= ((scale <<
				AVR32_ACIFA_SCFB_SCALEFACTOR_OFFSET) &
				AVR32_ACIFA_SCFB_SCALEFACTOR_MASK) |
				AVR32_ACIFA_SCFB_EN_MASK;
	}
}

/** \brief Start comparator in Normal Mode
 * \param *acifa        Base address of the ACIFA
 * \param comp_sel      Comparator Selection
 */
void acifa_start(volatile avr32_acifa_t *acifa,
		uint8_t comp_sel)
{
	Assert( acifa != NULL );

	if (comp_sel == ACIFA_COMP_SELA) {
		acifa->en = (AVR32_ACIFA_EN_ACAEN_MASK) |
				(AVR32_ACIFA_EN_ACACPEN_MASK);

		while (!acifa_is_aca_ready(acifa)) {
			/* Wait for ACA */
		}
	} else if (comp_sel == ACIFA_COMP_SELB) {
		acifa->en = (AVR32_ACIFA_EN_ACBEN_MASK) |
				(AVR32_ACIFA_EN_ACBCPEN_MASK);

		while (!acifa_is_acb_ready(acifa)) {
			/* Wait for ACB */
		}
	} else {
		acifa->en = (AVR32_ACIFA_EN_ACAEN_MASK) |
				(AVR32_ACIFA_EN_ACACPEN_MASK) |
				(AVR32_ACIFA_EN_ACBEN_MASK) |
				(AVR32_ACIFA_EN_ACBCPEN_MASK);

		while (!acifa_is_aca_ready(acifa)) {
			/* Wait for ACA */
		}
		while (!acifa_is_acb_ready(acifa)) {
			/* Wait for ACB */
		}
	}
}

/** \brief Stops the comparators that are currently started.
 * \param *acifa        Base address of the ACIFA
 * \param comp_sel      Comparator Selection
 */
void acifa_stop(volatile avr32_acifa_t *acifa,
		uint8_t comp_sel)
{
	Assert( acifa != NULL );

	if (comp_sel == ACIFA_COMP_SELA) {
		acifa->dis = (AVR32_ACIFA_DIS_ACADIS_MASK) |
				(AVR32_ACIFA_DIS_ACACPDIS_MASK);
	} else if (comp_sel == ACIFA_COMP_SELB) {
		acifa->dis = (AVR32_ACIFA_DIS_ACBDIS_MASK) |
				(AVR32_ACIFA_DIS_ACBCPDIS_MASK);
	} else {
		acifa->dis =
				(AVR32_ACIFA_DIS_ACADIS_MASK) |
				(AVR32_ACIFA_DIS_ACACPDIS_MASK) |
				(AVR32_ACIFA_DIS_ACBDIS_MASK) |
				(AVR32_ACIFA_DIS_ACBCPDIS_MASK);
	}
}

/** \brief Start comparator in Window Mode
 * \param acifa        Base address of the ACIFA
 */
void acifa_start_window(volatile avr32_acifa_t *acifa)
{
	acifa->en = (AVR32_ACIFA_EN_WFEN_MASK);
	while (!acifa_is_wf_ready(acifa)) {
		/* Wait for AC window ready */
	}
}

/** \brief Return comparator A status
 *  \param acifa Base address of the ACIFA
 *  \retval true when ACA Input P is higher than Input N
 *  \retval false when ACA Input P is Lower than Input N
 */
bool acifa_is_aca_inp_higher(volatile avr32_acifa_t *acifa)
{
	return (acifa->sr & AVR32_ACIFA_SR_ACACS_MASK) != 0;
}

/** \brief Return comparator B status
 *  \param acifa Base address of the ACIFA
 *  \retval true when ACB Input P is higher than Input N
 *  \retval false when ACB Input P is Lower than Input N
 */
bool acifa_is_acb_inp_higher(volatile avr32_acifa_t *acifa)
{
	return (acifa->sr & AVR32_ACIFA_SR_ACBCS_MASK) != 0;
}

/** \brief Return Window status
 *  \param acifa Base address of the ACIFA
 *  \retval true if Common Input Signal is inside the window
 *  \retval false if Common Input Signal is outside the window
 */
bool acifa_is_ci_inside_window(volatile avr32_acifa_t *acifa)
{
	return (acifa->sr & AVR32_ACIFA_SR_WFCS_MASK) != 0;
}

/** \brief Clear Flags
 * \param acifa         Base address of the ACIFA
 * \param flag_mask     Flag Mask Value
 */
void acifa_clear_flags(volatile avr32_acifa_t *acifa, uint32_t flag_mask)
{
	acifa->scr = flag_mask;
}

/** \brief Enable AC Interrupt when Vinp>Vin
 * \param acifa         Base address of the ACIFA
 * \param comp_sel      Comparator Selection
 */
void acifa_enable_interrupt_inp_higher(volatile avr32_acifa_t *acifa,
		uint8_t comp_sel)
{
	Assert( acifa != NULL );

	if (comp_sel == ACIFA_COMP_SELA) {
		acifa->CONFA.is = 0;
	} else if (comp_sel == ACIFA_COMP_SELB) {
		acifa->CONFB.is = 0;
	}
}

/** \brief Enable AC Interrupt when Vinp<Vin
 * \param acifa         Base address of the ACIFA
 * \param comp_sel      Comparator Selection
 */
void acifa_enable_interrupt_inp_lower(volatile avr32_acifa_t *acifa,
		uint8_t comp_sel)
{
	Assert( acifa != NULL );

	if (comp_sel == ACIFA_COMP_SELA) {
		acifa->CONFA.is = 1;
	} else if (comp_sel == ACIFA_COMP_SELB) {
		acifa->CONFB.is = 1;
	}
}

/** \brief Enable AC Interrupt when a toggle of the comparator occurs
 * \param acifa        Base address of the ACIFA
 * \param comp_sel     Comparator Selection
 */
void acifa_enable_interrupt_toggle(volatile avr32_acifa_t *acifa,
		uint8_t comp_sel)
{
	Assert( acifa != NULL );

	if (comp_sel == ACIFA_COMP_SELA) {
		acifa->CONFA.is = 2;
	} else if (comp_sel == ACIFA_COMP_SELB) {
		acifa->CONFB.is = 2;
	}
}

/** \brief Enable Interrupt Flags
 * \param acifa        Base address of the ACIFA
 * \param flag_mask    Flag Mask Value
 */
void acifa_enable_interrupt(volatile avr32_acifa_t *acifa, uint32_t flag_mask)
{
	Assert( acifa != NULL );

	acifa->ier = flag_mask;
}
