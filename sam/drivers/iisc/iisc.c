/**
 *
 * \file
 *
 * \brief IIS driver for SAM.
 *
 * This file defines a useful set of functions for the IIS on SAM devices.
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "iisc.h"
#include "sysclk.h"
#include "sleepmgr.h"

/**
 * \internal
 * \brief IIS callback function pointer array
 */
iis_callback_t iis_callback_pointer[_IIS_INTERRUPT_SOURCE_NUM];

struct iis_dev_inst *_iis_instance;

/**
 * \brief Enable the IIS module.
 *
 * \param dev_inst    Device structure pointer.
 *
 */
void iis_enable(struct iis_dev_inst *const dev_inst)
{
	UNUSED(dev_inst);
	sleepmgr_lock_mode(SLEEPMGR_ACTIVE);
}

/**
 * \brief Disable the IIS module.
 *
 * \param dev_inst    Device structure pointer.
 *
 */
void iis_disable(struct iis_dev_inst *const dev_inst)
{
	UNUSED(dev_inst);
	sleepmgr_unlock_mode(SLEEPMGR_ACTIVE);
}

/**
 * \internal Set configurations to module.
 *
 * \param  dev_inst Pointer to device instance structure.
 *
 */
static enum status_code _iis_set_config(struct iis_dev_inst *const dev_inst)
{
	/* Sanity check arguments */
	Assert(dev_inst);
	Assert(dev_inst->cfg);

	uint32_t mr = 0;
	mr = (dev_inst->cfg->master ? IISC_MR_MODE : 0) |
			(dev_inst->cfg->loopback ? IISC_MR_RXLOOP : 0) |
			((dev_inst->cfg->rx_dma == IIS_ONE_DMA_CHANNEL_FOR_ONE_CHANNEL) ?
				IISC_MR_RXDMA_MULTIPLE : 0) |
			((dev_inst->cfg->tx_dma == IIS_ONE_DMA_CHANNEL_FOR_ONE_CHANNEL) ?
				IISC_MR_TXDMA_MULTIPLE : 0) |
			((dev_inst->cfg->rx_channels == IIS_CHANNEL_MONO) ?
				IISC_MR_RXMONO : 0) |
			((dev_inst->cfg->tx_channels == IIS_CHANNEL_MONO) ?
				IISC_MR_TXMONO : 0) |
			IISC_MR_IMCKFS(dev_inst->cfg->fs_ratio) |
			IISC_MR_DATALENGTH(dev_inst->cfg->data_format);

	/* Set the master clock mode */
	if ((((uint32_t)dev_inst->cfg->fs_ratio + 1) * 16)
			> ((uint32_t)dev_inst->cfg->slot_length * 2)) {
		mr |= IISC_MR_IMCKMODE;
	}

	if ((dev_inst->cfg->slot_length == IIS_SLOT_LENGTH_24BIT) &&
			((dev_inst->cfg->data_format > IIS_DATE_32BIT) &&
			(dev_inst->cfg->data_format < IIS_DATE_16BIT))) {
		mr |= IISC_MR_IWS24_24;
	}

	dev_inst->hw_dev->IISC_MR = mr;

	return STATUS_OK;
}

/**
 * \brief Initialize and configure the IIS module.
 *
 * \param dev_inst    Device structure pointer.
 * \param iisc        Base address of the IISC instance.
 * \param cfg         Pointer to IIS configuration.
 *
 * \return status
 */
enum status_code iis_init(struct iis_dev_inst *const dev_inst, Iisc *iisc,
		struct iis_config *const cfg)
{
	/* Sanity check arguments */
	Assert(dev_inst);
	Assert(iisc);
	Assert(cfg);

	dev_inst->hw_dev = iisc;
	dev_inst->cfg = cfg;

	/* Enable APB clock for AESA */
	sysclk_enable_peripheral_clock(iisc);

	iis_reset(dev_inst);
	_iis_instance = dev_inst;

	return _iis_set_config(dev_inst);
}

/**
 * \brief Set callback for IIS
 *
 * \param dev_inst    Device structure pointer.
 * \param source      Interrupt source.
 * \param callback    Callback function pointer.
 * \param irq_level   Interrupt level.
 */
void iis_set_callback(struct iis_dev_inst *const dev_inst,
		iis_interrupt_source_t source, iis_callback_t callback,
		uint8_t irq_level)
{
	/* Sanity check arguments */
	Assert(dev_inst);

	iis_callback_pointer[source] = callback;
	NVIC_ClearPendingIRQ((IRQn_Type)IISC_IRQn);
	NVIC_SetPriority((IRQn_Type)IISC_IRQn, irq_level);
	NVIC_EnableIRQ((IRQn_Type)IISC_IRQn);
	iis_enable_interrupt(dev_inst, source);
}

/**
 * \brief Enable the specified interrput sources.
 *
 * \param dev_inst    Device structure pointer.
 * \param source      Interrupt source
 */
void iis_enable_interrupt(struct iis_dev_inst *const dev_inst,
		iis_interrupt_source_t source)
{
	/* Sanity check arguments */
	Assert(dev_inst);

	switch (source) {
	case IIS_INTERRUPT_RXRDY:
		dev_inst->hw_dev->IISC_IER = IISC_IER_RXRDY_ON;
		break;
	case IIS_INTERRUPT_RXOR:
		dev_inst->hw_dev->IISC_IER = IISC_IER_RXOR_ON;
		break;
	case IIS_INTERRUPT_TXRDY:
		dev_inst->hw_dev->IISC_IER = IISC_IER_TXRDY_ON;
		break;
	case IIS_INTERRUPT_TXUR:
		dev_inst->hw_dev->IISC_IER = IISC_IER_TXUR_ON;
		break;
	default:
		break;
	}
}

/**
 * \brief Disable the specified interrput sources.
 *
 * \param dev_inst    Device structure pointer.
 * \param source      Interrupt source
 */
void iis_disable_interrupt(struct iis_dev_inst *const dev_inst,
		iis_interrupt_source_t source)
{
	/* Sanity check arguments */
	Assert(dev_inst);

	switch (source) {
	case IIS_INTERRUPT_RXRDY:
		dev_inst->hw_dev->IISC_IDR = IISC_IDR_RXRDY_ON;
		break;
	case IIS_INTERRUPT_RXOR:
		dev_inst->hw_dev->IISC_IDR = IISC_IDR_RXOR_ON;
		break;
	case IIS_INTERRUPT_TXRDY:
		dev_inst->hw_dev->IISC_IDR = IISC_IDR_TXRDY_ON;
		break;
	case IIS_INTERRUPT_TXUR:
		dev_inst->hw_dev->IISC_IDR = IISC_IDR_TXUR_ON;
		break;
	default:
		break;
	}
}

/**
 * \brief Clear the IIS status value.
 *
 * \param dev_inst    Device structure pointer.
 * \param source      Interrupt source
 */
void iis_clear_status(struct iis_dev_inst *dev_inst,
		iis_interrupt_source_t source)
{
	/* Sanity check arguments */
	Assert(dev_inst);

	switch (source) {
	case IIS_INTERRUPT_RXOR:
		dev_inst->hw_dev->IISC_SCR = IISC_SCR_RXOR_CLEAR;
		break;
	case IIS_INTERRUPT_TXUR:
		dev_inst->hw_dev->IISC_SCR = IISC_SCR_TXUR_CLEAR;
		break;
	default:
		break;
	}
}

/**
 * \brief Write a single message of data.
 *
 * \param dev_inst  Device structure pointer.
 * \param data      The data to write
 *
 * \return status
 */
enum status_code iis_write(struct iis_dev_inst *const dev_inst, uint32_t data)
{
	/* Sanity check arguments */
	Assert(dev_inst);
	Assert(dev_inst->hw_dev);

	uint32_t timeout = IIS_RETRY_VALUE;

	while (!(dev_inst->hw_dev->IISC_SR & IISC_SR_TXRDY) && timeout) {
		--timeout;
	}

	if (timeout == 0) {
		return STATUS_ERR_TIMEOUT;
	}

	dev_inst->hw_dev->IISC_THR = data;

	return STATUS_OK;
}

/**
 * \brief Read a single message of data.
 *
 * \param dev_inst  Device structure pointer.
 * \param *data     Pointer for receive data
 *
 * \return status
 */
enum status_code iis_read(struct iis_dev_inst *const dev_inst, uint32_t *data)
{
	/* Sanity check arguments */
	Assert(dev_inst);
	Assert(dev_inst->hw_dev);

	uint32_t timeout = IIS_RETRY_VALUE;

	while (!(dev_inst->hw_dev->IISC_SR & IISC_SR_RXRDY) && timeout) {
		--timeout;
	}

	if (timeout == 0) {
		return STATUS_ERR_TIMEOUT;
	}

	*data = dev_inst->hw_dev->IISC_RHR;

	return STATUS_OK;
}

/**
 * \brief Internal interrupt handler for IIS.
 */
static void iis_interrupt_handler(void)
{
	uint32_t status = iis_get_status(_iis_instance);
	uint32_t mask = iis_get_interrupt_mask(_iis_instance);

	if ((status & IISC_SR_TXRDY_EMPTY) && (mask & IISC_IMR_TXRDY_ENABLED)) {
		iis_callback_pointer[IIS_INTERRUPT_TXRDY]();
	}

	if ((status & IISC_SR_TXUR_YES) && (mask & IISC_IMR_TXUR_ENABLED)) {
		iis_callback_pointer[IIS_INTERRUPT_TXUR]();
	}

	if ((status & IISC_SR_RXRDY_FULL) && (mask & IISC_IMR_RXRDY_ENABLED)) {
		iis_callback_pointer[IIS_INTERRUPT_RXRDY]();
	}

	if ((status & IISC_SR_RXOR_YES) && (mask & IISC_IMR_RXOR_ENABLED)) {
		iis_callback_pointer[IIS_INTERRUPT_RXOR]();
	}
}

/**
 * \brief Interrupt handler for IIS.
 */
void IISC_Handler(void)
{
	iis_interrupt_handler();
}
