/**
 * \file
 *
 * \brief Analog Comparator Controller (ACC) driver for SAM.
 *
 * Copyright (c) 2011 - 2013 Atmel Corporation. All rights reserved.
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

#include "acc.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

/**
 * \defgroup sam_drivers_acc_group Analog Comparator Controller (ACC)
 *
 * \par Purpose
 *
 * This driver offers API to access SAM ACC registers.
 *
 * \section dependencies Dependencies
 * This driver does not depend on other modules.
 *
 * @{
 */

#define ACC_MR_INV_Pos			12	/* ACC invert output (register offset) */

#define ACC_ACR_HYST_0mv_max	0x00	/* HYSTeresis levels: please refer to Electrical Characteristics */
#define ACC_ACR_HYST_50mv_max	0x01
#define ACC_ACR_HYST_90mv_max	0x11

#define ACC_WPMR_WPKEY_VALUE ACC_WPMR_WPKEY((uint32_t) 0x414343)

/**
 * \brief Initialize the ACC controller.
 *
 * \param p_acc Pointer to an ACC instance.
 * \param ul_select_plus Input connected to inp, 0~7.
 * \param ul_select_minus Input connected to inm, 0~7. 
 * \param ul_edge_type CF flag triggering mode, use pattern defined in the device header file.
 * \param ul_invert Invert comparator output.
 */
void acc_init(Acc *p_acc, uint32_t ul_select_plus, uint32_t ul_select_minus,
		uint32_t ul_edge_type, uint32_t ul_invert)
{
	/* Reset the controller. */
	p_acc->ACC_CR |= ACC_CR_SWRST;

	/* Write to the MR register. */
	p_acc->ACC_MR = (((ul_select_plus << ACC_MR_SELPLUS_Pos) & ACC_MR_SELPLUS_Msk) | 
			((ul_select_minus << ACC_MR_SELMINUS_Pos) & ACC_MR_SELMINUS_Msk) | 
			((ul_edge_type << ACC_MR_EDGETYP_Pos) & ACC_MR_EDGETYP_Msk) | 
			((ul_invert << ACC_MR_INV_Pos) & ACC_MR_INV)) | ACC_MR_ACEN_EN;

	/* Set hysteresis and current option. */
	p_acc->ACC_ACR = (ACC_ACR_ISEL_HISP |
			ACC_ACR_HYST(ACC_ACR_HYST_50mv_max));

	/* Automatic Output Masking Period. */
	while (p_acc->ACC_ISR & (uint32_t) ACC_ISR_MASK);
}

/**
 * \brief Enable the ACC.
 *
 * \param p_acc Pointer to an ACC instance.
 */
void acc_enable(Acc *p_acc)
{
	p_acc->ACC_MR |= ACC_MR_ACEN_EN;
}

/**
 * \brief Disable the ACC.
 *
 * \param p_acc Pointer to an ACC instance.
 */
void acc_disable(Acc *p_acc)
{
	p_acc->ACC_MR &= ~ACC_MR_ACEN_EN;
}

/**
 * \brief Reset the ACC.
 *
 * \param p_acc Pointer to an ACC instance.
 */
void acc_reset(Acc *p_acc)
{
	p_acc->ACC_CR = ACC_CR_SWRST;
}

/**
 * \brief Set the ACC input source.
 *
 * \param p_acc Pointer to an ACC instance.
 * \param ul_select_minus Selection for minus comparator input.
 * \param ul_select_plus Selection for plus comparator input.
 */
void acc_set_input(Acc *p_acc, uint32_t ul_select_minus,
		uint32_t ul_select_plus)
{
	p_acc->ACC_MR &= ~(ACC_MR_SELMINUS_Msk | ACC_MR_SELPLUS_Msk);
	p_acc->ACC_MR |= ul_select_plus | ul_select_minus;
}

/**
 * \brief Set the ACC output.
 *
 * \param p_acc Pointer to an ACC instance.
 * \param ul_invert Invert comparator output, 0 for disable, 1 for enable.
 * \param ul_fault_enable Fault enable, 0 for disable, 1 for enable.
 * \param ul_fault_source Selection of fault source, 0 for CF, 1 for output.
 */
void acc_set_output(Acc *p_acc, uint32_t ul_invert,
		uint32_t ul_fault_enable, uint32_t ul_fault_source)
{
	p_acc->ACC_MR &= ~(ACC_MR_INV_EN | ACC_MR_FE_EN | ACC_MR_SELFS_OUTPUT);

	p_acc->ACC_MR |= ul_invert | ul_fault_source | ul_fault_enable;
}

/**
 * \brief Get the comparison result.
 *
 * \param p_acc Pointer to an ACC instance.
 *
 * \return Result of the comparison, 0 for inn>inp, 1 for inp>inn.
 */
uint32_t acc_get_comparison_result(Acc *p_acc)
{
	uint32_t ul_temp = p_acc->ACC_MR;
	uint32_t ul_status = p_acc->ACC_ISR;

	if ((ul_temp & ACC_MR_INV_EN) == ACC_MR_INV_EN) {
		if (ul_status & ACC_ISR_SCO) {
			return 0;	/* inn>inp */
		} else {
			return 1;	/* inp>inn */
		}
	} else {
		if (ul_status & ACC_ISR_SCO) {
			return 1;	/* inp>inn */
		} else {
			return 0;	/* inn>inp */
		}
	}
}

/**
 * \brief Enable interrupt.
 *
 * \param p_acc Pointer to an ACC instance.
 */
void acc_enable_interrupt(Acc *p_acc)
{
	p_acc->ACC_IER = ACC_IER_CE;
}

/**
 * \brief Disable interrupt.
 *
 * \param p_acc Pointer to an ACC instance.
 */
void acc_disable_interrupt(Acc *p_acc)
{
	p_acc->ACC_IDR = ACC_IDR_CE;
}

/**
 * \brief Get the interrupt status.
 *
 * \param p_acc Pointer to an ACC instance.
 *
 * \return Status of interrupt.
 */
uint32_t acc_get_interrupt_status(Acc *p_acc)
{
	return p_acc->ACC_ISR;
}

/**
 * \brief Set write protect of ACC registers.
 *
 * \param p_acc Pointer to an ACC instance.
 *
 * \param ul_enable 1 to enable, 0 to disable.
 */
void acc_set_writeprotect(Acc *p_acc, uint32_t ul_enable)
{
	if (ul_enable)
		p_acc->ACC_WPMR = ACC_WPMR_WPKEY_VALUE | ACC_WPMR_WPEN;
	else
		p_acc->ACC_WPMR = ACC_WPMR_WPKEY_VALUE;
}

/** 
 * \brief Return write protect status.
 *
 * \retval 0 No write protection error.
 * \retval 1 Write protection error.
 */
uint32_t acc_get_writeprotect_status(Acc *p_acc)
{
	return p_acc->ACC_WPSR & ACC_WPSR_WPROTERR;
}

//@}

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond
