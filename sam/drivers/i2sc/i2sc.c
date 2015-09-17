/**
 *
 * \file
 *
 * \brief I2S driver for SAM.
 *
 * This file defines a useful set of functions for the I2S on SAM devices.
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

#include "i2sc.h"
#include "sysclk.h"
#include "sleepmgr.h"

#if defined(ID_I2SC1)
#define I2SC_NUM  2
#elif defined(ID_I2SC0)
#define I2SC_NUM  1
#else
#error There is no I2SC module on this device.
#endif

/**
 * \internal
 * \brief I2S callback function pointer array
 */
i2s_callback_t i2s_callback_pointer[I2SC_NUM][_I2S_INTERRUPT_SOURCE_NUM];

/**
 * \brief Enable the I2S module.
 *
 * \param dev_inst    Device structure pointer.
 *
 */
void i2s_enable(struct i2s_dev_inst *const dev_inst)
{
	UNUSED(dev_inst);
	sleepmgr_lock_mode(SLEEPMGR_ACTIVE);
}

/**
 * \brief Disable the I2S module.
 *
 * \param dev_inst    Device structure pointer.
 *
 */
void i2s_disable(struct i2s_dev_inst *const dev_inst)
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
static enum status_code _i2s_set_config(struct i2s_dev_inst *const dev_inst)
{
	/* Sanity check arguments */
	Assert(dev_inst);
	Assert(dev_inst->cfg);

	uint32_t mr = 0;
	mr = (dev_inst->cfg->master_mode? I2SC_MR_MODE : 0) |
			(dev_inst->cfg->loopback ? I2SC_MR_RXLOOP : 0) |
			((dev_inst->cfg->rx_dma == I2S_ONE_DMA_CHANNEL_FOR_ONE_CHANNEL) ?
				I2SC_MR_RXDMA : 0) |
			((dev_inst->cfg->tx_dma == I2S_ONE_DMA_CHANNEL_FOR_ONE_CHANNEL) ?
				I2SC_MR_TXDMA : 0) |
			((dev_inst->cfg->rx_channels == I2S_CHANNEL_MONO) ?
				I2SC_MR_RXMONO : 0) |
			((dev_inst->cfg->tx_channels == I2S_CHANNEL_MONO) ?
				I2SC_MR_TXMONO : 0) |
			(dev_inst->cfg->fs_ratio << I2SC_MR_IMCKFS_Pos) |
			(dev_inst->cfg->data_format << I2SC_MR_DATALENGTH_Pos);

	/* Set the master clock mode */
	if (dev_inst->cfg->master_clock_enable) {
		mr |= I2SC_MR_IMCKMODE;
	}

	if (dev_inst->cfg->slot_length_24) {
		mr |= I2SC_MR_IWS;
	}

	if (dev_inst->cfg->transmit_mode_underrun) {
		mr |= I2SC_MR_TXSAME;
	}

	dev_inst->hw_dev->I2SC_MR = mr;

	return STATUS_OK;
}

/**
 * \brief Initialize and configure the I2S module.
 *
 * \param dev_inst    Device structure pointer.
 * \param i2sc        Base address of the I2SC instance.
 * \param cfg         Pointer to I2S configuration.
 *
 * \return status
 */
enum status_code i2s_init(struct i2s_dev_inst *const dev_inst, I2sc *i2sc,
		struct i2s_config *const cfg)
{
	/* Sanity check arguments */
	Assert(dev_inst);
	Assert(i2sc);
	Assert(cfg);

	dev_inst->hw_dev = i2sc;
	dev_inst->cfg = cfg;

	/* Enable PMC clock for I2SC */
#ifdef ID_I2SC1
	 if (dev_inst->hw_dev == I2SC1) {
		 sysclk_enable_peripheral_clock(ID_I2SC1);
	} else
#endif
#ifdef ID_I2SC0
	if (dev_inst->hw_dev == I2SC0) {
		sysclk_enable_peripheral_clock(ID_I2SC0);
	} else
#endif
	{
		Assert(false);
	}

	i2s_reset(dev_inst);

	return _i2s_set_config(dev_inst);
}

/**
 * \brief Set callback for I2S
 *
 * \param dev_inst    Device structure pointer.
 * \param source      Interrupt source.
 * \param callback    Callback function pointer.
 * \param irq_level   Interrupt level.
 */
void i2s_set_callback(struct i2s_dev_inst *const dev_inst,
		i2s_interrupt_source_t source, i2s_callback_t callback,
		uint8_t irq_level)
{
	uint32_t irq_line = 0;
	uint32_t row_num = 0;

	/* Sanity check arguments */
	Assert(dev_inst);

#if defined(ID_I2SC1)
	if (dev_inst->hw_dev == I2SC1) {
		irq_line = I2SC1_IRQn;
		row_num = 1;
	}
#endif

#if defined(ID_I2SC0)
	if (dev_inst->hw_dev == I2SC0) {
		irq_line = I2SC0_IRQn;
		row_num = 0;
	}
#endif

	i2s_callback_pointer[row_num][source] = callback;
	NVIC_ClearPendingIRQ((IRQn_Type)irq_line);
	NVIC_SetPriority((IRQn_Type)irq_line, irq_level);
	NVIC_EnableIRQ((IRQn_Type)irq_line);
	i2s_enable_interrupt(dev_inst, source);
}

/**
 * \brief Enable the specified interrput sources.
 *
 * \param dev_inst    Device structure pointer.
 * \param source      Interrupt source
 */
void i2s_enable_interrupt(struct i2s_dev_inst *const dev_inst,
		i2s_interrupt_source_t source)
{
	/* Sanity check arguments */
	Assert(dev_inst);

	switch (source) {
	case I2S_INTERRUPT_RXRDY:
		dev_inst->hw_dev->I2SC_IER = I2SC_IER_RXRDY;
		break;
	case I2S_INTERRUPT_RXOR:
		dev_inst->hw_dev->I2SC_IER = I2SC_IER_RXOR;
		break;
	case I2S_INTERRUPT_TXRDY:
		dev_inst->hw_dev->I2SC_IER = I2SC_IER_TXRDY;
		break;
	case I2S_INTERRUPT_TXUR:
		dev_inst->hw_dev->I2SC_IER = I2SC_IER_TXUR;
		break;
	case I2S_INTERRUPT_ENDRX:
		dev_inst->hw_dev->I2SC_IER = I2SC_IER_ENDRX;
		break;
	case I2S_INTERRUPT_ENDTX:
		dev_inst->hw_dev->I2SC_IER = I2SC_IER_ENDTX;
		break;
	case I2S_INTERRUPT_RXBUFF:
		dev_inst->hw_dev->I2SC_IER = I2SC_IER_RXFULL;
		break;
	case I2S_INTERRUPT_TXBUFE:
		dev_inst->hw_dev->I2SC_IER = I2SC_IER_TXEMPTY;
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
void i2s_disable_interrupt(struct i2s_dev_inst *const dev_inst,
		i2s_interrupt_source_t source)
{
	/* Sanity check arguments */
	Assert(dev_inst);

	switch (source) {
	case I2S_INTERRUPT_RXRDY:
		dev_inst->hw_dev->I2SC_IDR = I2SC_IDR_RXRDY;
		break;
	case I2S_INTERRUPT_RXOR:
		dev_inst->hw_dev->I2SC_IDR = I2SC_IDR_RXOR;
		break;
	case I2S_INTERRUPT_TXRDY:
		dev_inst->hw_dev->I2SC_IDR = I2SC_IDR_TXRDY;
		break;
	case I2S_INTERRUPT_TXUR:
		dev_inst->hw_dev->I2SC_IDR = I2SC_IDR_TXUR;
		break;
	case I2S_INTERRUPT_ENDRX:
		dev_inst->hw_dev->I2SC_IDR = I2SC_IDR_ENDRX;
		break;
	case I2S_INTERRUPT_ENDTX:
		dev_inst->hw_dev->I2SC_IDR = I2SC_IDR_ENDTX;
		break;
	case I2S_INTERRUPT_RXBUFF:
		dev_inst->hw_dev->I2SC_IDR = I2SC_IDR_RXFULL;
		break;
	case I2S_INTERRUPT_TXBUFE:
		dev_inst->hw_dev->I2SC_IDR = I2SC_IDR_TXEMPTY;
		break;
	default:
		break;
	}
}

/**
 * \brief Clear the I2S status value.
 *
 * \param dev_inst    Device structure pointer.
 * \param source      Interrupt source
 */
void i2s_clear_status(struct i2s_dev_inst *dev_inst,
		i2s_interrupt_source_t source)
{
	/* Sanity check arguments */
	Assert(dev_inst);

	switch (source) {
	case I2S_INTERRUPT_RXOR:
		dev_inst->hw_dev->I2SC_SCR = I2SC_SCR_RXOR;
		break;
	case I2S_INTERRUPT_TXUR:
		dev_inst->hw_dev->I2SC_SCR = I2SC_SCR_TXUR;
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
enum status_code i2s_write(struct i2s_dev_inst *const dev_inst, uint32_t data)
{
	/* Sanity check arguments */
	Assert(dev_inst);
	Assert(dev_inst->hw_dev);

	uint32_t timeout = I2S_RETRY_VALUE;

	while (!(dev_inst->hw_dev->I2SC_SR & I2SC_SR_TXRDY) && timeout) {
		--timeout;
	}

	if (timeout == 0) {
		return STATUS_ERR_TIMEOUT;
	}

	dev_inst->hw_dev->I2SC_THR = data;

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
enum status_code i2s_read(struct i2s_dev_inst *const dev_inst, uint32_t *data)
{
	/* Sanity check arguments */
	Assert(dev_inst);
	Assert(dev_inst->hw_dev);

	uint32_t timeout = I2S_RETRY_VALUE;

	while (!(dev_inst->hw_dev->I2SC_SR & I2SC_SR_RXRDY) && timeout) {
		--timeout;
	}

	if (timeout == 0) {
		return STATUS_ERR_TIMEOUT;
	}

	*data = dev_inst->hw_dev->I2SC_RHR;

	return STATUS_OK;
}

/**
 * \brief Internal interrupt handler for I2S.
 */
static void i2s_interrupt_handler(I2sc *i2sc)
{
	uint32_t row_num;
	struct i2s_dev_inst i2s_instance;

	i2s_instance.hw_dev = i2sc;

	uint32_t status = i2s_get_status(&i2s_instance);
	uint32_t mask = i2s_get_interrupt_mask(&i2s_instance);

#if defined(ID_I2SC1)
	if (i2sc == I2SC1) {
		row_num = 1;
	}
#endif
	
#if defined(ID_I2SC0)
	if (i2sc == I2SC0) {
		row_num = 0;
	}
#endif

	if ((status & I2SC_SR_RXRDY) && (mask & I2SC_IMR_RXRDY)) {
		i2s_callback_pointer[row_num][I2S_INTERRUPT_RXRDY]();
	}

	if ((status & I2SC_SR_RXOR) && (mask & I2SC_IMR_RXOR)) {
		i2s_callback_pointer[row_num][I2S_INTERRUPT_RXOR]();
	}

	if ((status & I2SC_SR_TXRDY) && (mask & I2SC_IMR_TXRDY)) {
		i2s_callback_pointer[row_num][I2S_INTERRUPT_TXRDY]();
	}

	if ((status & I2SC_SR_TXUR) && (mask & I2SC_IMR_TXUR)) {
		i2s_callback_pointer[row_num][I2S_INTERRUPT_TXUR]();
	}

	if ((status & I2SC_SR_RXRDY) && (mask & I2SC_IMR_ENDRX)) {
		i2s_callback_pointer[row_num][I2S_INTERRUPT_ENDRX]();
	}

	if ((status & I2SC_SR_RXOR) && (mask & I2SC_IMR_ENDTX)) {
		i2s_callback_pointer[row_num][I2S_INTERRUPT_ENDTX]();
	}

	if ((status & I2SC_SR_TXRDY) && (mask & I2SC_IMR_RXFULL)) {
		i2s_callback_pointer[row_num][I2S_INTERRUPT_RXBUFF]();
	}

	if ((status & I2SC_SR_TXUR) && (mask & I2SC_IMR_TXEMPTY)) {
		i2s_callback_pointer[row_num][I2S_INTERRUPT_TXBUFE]();
	}

}

#if defined(ID_I2SC0)
/**
 * \brief Interrupt handler for I2S.
 */
void I2SC0_Handler(void)
{
	i2s_interrupt_handler(I2SC0);
}
#endif

#if defined(ID_I2SC1)
/**
 * \brief Interrupt handler for I2S.
 */
void I2SC1_Handler(void)
{
	i2s_interrupt_handler(I2SC1);
}
#endif

/**
 * \brief Get I2SC PDC base address.
 *
 * \param dev_inst  Device structure pointer.
 *
 * \return I2SC PDC registers base for PDC driver to access.
 */
Pdc *i2s_get_pdc_base(struct i2s_dev_inst *const dev_inst)
{
	Pdc *p_pdc_base = NULL;
#ifdef PDC_I2SC1
	 if (dev_inst->hw_dev == I2SC1) {
		p_pdc_base = PDC_I2SC1;
	} else
#endif
#ifdef PDC_I2SC0
	if (dev_inst->hw_dev == I2SC0) {
		p_pdc_base = PDC_I2SC0;
	} else
#endif
	{
		Assert(false);
	}

	return p_pdc_base;
}

