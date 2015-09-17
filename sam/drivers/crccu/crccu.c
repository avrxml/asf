/**
 * \file
 *
 * \brief Cyclic Redundancy Check Calculation Unit (CRCCU) driver for SAM.
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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

#include "crccu.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

/**
 * \defgroup sam_drivers_crccu_group Cyclic Redundancy Check Calculation Unit (CRCCU)
 *
 * Driver for the Cyclic Redundancy Check Calculation Unit. It provides access to the main 
 * features of the CRCCU controller.
 *
 * @{
 */

/**
 * \brief Configure CRCCU descriptor.
 *
 * \param p_crccu Pointer to a CRCCU instance.
 * \param ul_crc_dscr_addr CRCCU descriptor address.
 */
void crccu_configure_descriptor(Crccu *p_crccu, uint32_t ul_crc_dscr_addr)
{
	p_crccu->CRCCU_DSCR = ul_crc_dscr_addr;
}

/**
 * \brief Configure the CRCCU mode.
 *
 * \param p_crccu Pointer to a CRCCU instance. 
 * \param ul_mode CRC work mode.
 */
void crccu_configure_mode(Crccu *p_crccu, uint32_t ul_mode)
{
	p_crccu->CRCCU_MR = ul_mode;
}

/**
 * \brief Start CRCCU. 
 *
 * \note  To start the CRCCU, the user needs to set the CRC enable bit (ENABLE) 
 * in the CRCCU Mode Register (CRCCU_MR), and then configure it and finally set
 * the DMA enable bit (DMAEN) in the CRCCU DMA Enable Register (CRCCU_DMA_EN).
 *
 * \note The crccu_configure_descriptor() and crccu_configure_mode ()functions must 
 * be executed before calling this function.
 *
 * \param p_crccu Pointer to a CRCCU instance.
 */
void crccu_enable_dma(Crccu *p_crccu)
{
	/* Start CRC calculation */
	p_crccu->CRCCU_DMA_EN = CRCCU_DMA_EN_DMAEN;
}

/**
 * \brief Disable CRCCU. 
 *
 * \param p_crccu Pointer to a CRCCU instance.
 */
void crccu_disable_dma(Crccu *p_crccu)
{
	p_crccu->CRCCU_DMA_DIS = CRCCU_DMA_DIS_DMADIS;
}

/**
 * \brief Reset CRCCU.
 */
void crccu_reset(Crccu *p_crccu)
{
	p_crccu->CRCCU_CR = CRCCU_CR_RESET;
}

/**
 * \brief Check if DMA channel is enabled.
 *
 * \param p_crccu Pointer to a CRCCU instance.
 *
 * \return CRCCU DMA status.
 */
uint32_t crccu_get_dma_status(Crccu *p_crccu)
{
	return p_crccu->CRCCU_DMA_SR & CRCCU_DMA_SR_DMASR;
}

/**
 * \brief Enable CRCCU DMA transfer interrupt.
 *
 * \param p_crccu Pointer to a CRCCU instance.
 */
void crccu_enable_dma_interrupt(Crccu *p_crccu)
{
	p_crccu->CRCCU_DMA_IER = CRCCU_DMA_IER_DMAIER;
}

/**
 * \brief Disable CRCCU DMA transfer interrupt.
 *
 * \param p_crccu Pointer to a CRCCU instance.
 */
void crccu_disable_dma_interrupt(Crccu *p_crccu)
{
	p_crccu->CRCCU_DMA_IDR = CRCCU_DMA_IDR_DMAIDR;
}

/**
 * \brief Check if DMA buffer transfer has been terminated.
 *
 * \param p_crccu Pointer to a CRCCU instance.
 *
 * \return DMA interrupt status.
 */
uint32_t crccu_get_dma_interrupt_status(Crccu *p_crccu)
{
	return p_crccu->CRCCU_DMA_ISR & CRCCU_DMA_ISR_DMAISR;
}

/**
 * \brief Get DMA interrupt mask.
 *
 * \param p_crccu Pointer to a CRCCU instance.
 *
 * \return DMA interrupt mask.
 */
uint32_t crccu_get_dma_interrupt_mask(Crccu *p_crccu)
{
	return p_crccu->CRCCU_DMA_IMR & CRCCU_DMA_IMR_DMAIMR;
}

/**
 * \brief Read cyclic redundancy check value.
 *
 * \param p_crccu Pointer to a CRCCU instance.
 *
 * \return CRC value.
 */
uint32_t crccu_read_crc_value(Crccu *p_crccu)
{
	return p_crccu->CRCCU_SR & CRCCU_SR_CRC_Msk;
}

/**
 * \brief Enable cyclic redundancy check error interrupt.
 *
 * \param p_crccu Pointer to a CRCCU instance.
 */
void crccu_enable_error_interrupt(Crccu *p_crccu)
{
	p_crccu->CRCCU_IER = CRCCU_IER_ERRIER;
}

/**
 * \brief Disable cyclic redundancy check error interrupt.
 *
 * \param p_crccu Pointer to a CRCCU instance.
 */
void crccu_disable_error_interrupt(Crccu *p_crccu)
{
	p_crccu->CRCCU_IDR = CRCCU_IDR_ERRIDR;
}

/**
 * \brief Check if there is a CRC error.
 *
 * \param p_crccu Pointer to a CRCCU instance.
 *
 * \return Error interrupt status. 
 */
uint32_t crccu_get_error_interrupt_status(Crccu *p_crccu)
{
	return p_crccu->CRCCU_ISR & CRCCU_ISR_ERRISR;
}

/**
 * \brief Get check CRC error interrupt mask.
 *
 * \param p_crccu Pointer to a CRCCU instance.
 *
 * \return Error interrupt mask.
 */
uint32_t crccu_get_error_interrupt_mask(Crccu *p_crccu)
{
	return p_crccu->CRCCU_IMR & CRCCU_IMR_ERRIMR;
}

//@}

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond
