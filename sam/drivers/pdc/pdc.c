/**
 * \file
 *
 * \brief Peripheral DMA Controller (PDC) driver for SAM.
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

#include "pdc.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

/**
 * \defgroup sam_drivers_pdc_group Peripheral DMA Controller (PDC)
 *
 * \par Purpose
 *
 * The Peripheral DMA Controller (PDC) transfers data between on-chip serial
 * peripherals and the on- and/or off-chip memories. The link between the PDC
 * and a serial peripheral is operated by the AHB to ABP bridge.
 *
 * @{
 */

/**
 * \brief Configure PDC for data transmit.
 *
 * \param p_pdc Pointer to a PDC instance.
 * \param p_packet Pointer to packet information for current buffer register
 * set, NULL to let them as is.
 * \param p_next_packet Pointer to packet information for next buffer register
 * set, NULL to let them as is.
 */
void pdc_tx_init(Pdc *p_pdc, pdc_packet_t *p_packet,
		pdc_packet_t *p_next_packet)
{
	if (p_packet) {
		p_pdc->PERIPH_TPR = p_packet->ul_addr;
		p_pdc->PERIPH_TCR = p_packet->ul_size;
	}
	if (p_next_packet) {
		p_pdc->PERIPH_TNPR = p_next_packet->ul_addr;
		p_pdc->PERIPH_TNCR = p_next_packet->ul_size;
	}
}

/**
 * \brief Configure PDC for data receive.
 *
 * \param p_pdc Pointer to a PDC instance.
 * \param p_packet Pointer to packet information for current buffer register
 * set, NULL to let them as is.
 * \param p_next_packet Pointer to packet information for next buffer register
 * set, NULL to let them as is.
 */
void pdc_rx_init(Pdc *p_pdc, pdc_packet_t *p_packet,
		pdc_packet_t *p_next_packet)
{
	if (p_packet) {
		p_pdc->PERIPH_RPR = p_packet->ul_addr;
		p_pdc->PERIPH_RCR = p_packet->ul_size;
	}
	if (p_next_packet) {
		p_pdc->PERIPH_RNPR = p_next_packet->ul_addr;
		p_pdc->PERIPH_RNCR = p_next_packet->ul_size;
	}
}

/**
 * \brief Clear PDC buffer receive counter.
 *
 * \param p_pdc Pointer to a PDC instance.
 */
void pdc_rx_clear_cnt(Pdc *p_pdc)
{
	p_pdc->PERIPH_RNCR = 0;
	p_pdc->PERIPH_RCR = 0;
}

/**
 * \brief Enable PDC transfers (TX and/or RX).
 *
 * \note It is forbidden to set both TXTEN and RXTEN for a half duplex
 * peripheral.
 *
 * \param p_pdc Pointer to a PDC instance.
 * \param ul_controls Transfer directions.
 * (bit PERIPH_PTCR_RXTEN and bit PERIPH_PTCR_TXTEN)
 */
void pdc_enable_transfer(Pdc *p_pdc, uint32_t ul_controls)
{
	p_pdc->PERIPH_PTCR =
			ul_controls & (PERIPH_PTCR_RXTEN | PERIPH_PTCR_TXTEN);
}

/**
 * \brief Disable PDC transfers (TX and/or RX)
 *
 * \param p_pdc Pointer to a PDC instance.
 * \param ul_controls Transfer directions.
 * (bit PERIPH_PTCR_TXTDIS, bit PERIPH_PTCR_TXTDIS)
 */
void pdc_disable_transfer(Pdc *p_pdc, uint32_t ul_controls)
{
	p_pdc->PERIPH_PTCR =
			ul_controls & (PERIPH_PTCR_RXTDIS | PERIPH_PTCR_TXTDIS);
}

/**
 * \brief Read PDC status.
 *
 * \param p_pdc Pointer to a PDC instance.
 *
 * \return PDC status register value.
 */
uint32_t pdc_read_status(Pdc *p_pdc)
{
	return p_pdc->PERIPH_PTSR;
}

/**
 * \brief Return Receive Pointer Register (RPR) value.
 *
 * \param p_pdc Pointer to a PDC instance.
 *
 * \return Receive Pointer Register value.
 */
uint32_t pdc_read_rx_ptr(Pdc *p_pdc)
{
	return p_pdc->PERIPH_RPR;
}

/**
 * \brief Return Receive Counter Register (RCR) value.
 *
 * \param p_pdc Pointer to a PDC instance.
 *
 * \return Receive Counter Register value.
 */
uint32_t pdc_read_rx_counter(Pdc *p_pdc)
{
	return p_pdc->PERIPH_RCR;
}

/**
 * \brief Return Transmit Pointer Register (TPR) value.
 *
 * \param p_pdc Pointer to a PDC instance.
 *
 * \return Transmit Pointer Register value.
 */
uint32_t pdc_read_tx_ptr(Pdc *p_pdc)
{
	return p_pdc->PERIPH_TPR;
}

/**
 * \brief Return Transmit Counter Register (TCR) value.
 *
 * \param p_pdc Pointer to a PDC instance.
 *
 * \return Transmit Counter Register value.
 */
uint32_t pdc_read_tx_counter(Pdc *p_pdc)
{
	return p_pdc->PERIPH_TCR;
}

/**
 * \brief Return Receive Next Pointer Register (RNPR) value.
 *
 * \param p_pdc Pointer to a PDC instance.
 *
 * \return Receive Next Pointer Register value.
 */
uint32_t pdc_read_rx_next_ptr(Pdc *p_pdc)
{
	return p_pdc->PERIPH_RNPR;
}

/**
 * \brief Return Receive Next Counter Register (RNCR) value.
 *
 * \param p_pdc Pointer to a PDC instance.
 *
 * \return Receive Next Counter Register value.
 */
uint32_t pdc_read_rx_next_counter(Pdc *p_pdc)
{
	return p_pdc->PERIPH_RNCR;
}

/**
 * \brief Return Transmit Next Pointer Register (TNPR) value.
 *
 * \param p_pdc Pointer to a PDC instance.
 *
 * \return Transmit Next Pointer Register value.
 */
uint32_t pdc_read_tx_next_ptr(Pdc *p_pdc)
{
	return p_pdc->PERIPH_TNPR;
}

/**
 * \brief Return Transmit Next Counter Register (TNCR) value.
 *
 * \param p_pdc Pointer to a PDC instance.
 *
 * \return Transmit Next Counter Register value.
 */
uint32_t pdc_read_tx_next_counter(Pdc *p_pdc)
{
	return p_pdc->PERIPH_TNCR;
}

//@}

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond
