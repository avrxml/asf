/**
 * \file
 *
 * \brief SAM D20 Serial Peripheral Interface Driver
 *
 * Copyright (C) 2012-2013 Atmel Corporation. All rights reserved.
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
#include "spi.h"

/**
 * \brief Resets the SPI module
 *
 * This function will reset the SPI module to its power on default values and
 * disable it.
 *
 * \param[in,out] module Pointer to the software instance struct
 */
void spi_reset(
		struct spi_module *const module)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(module->hw);

	SercomSpi *const spi_module = &(module->hw->SPI);

	/* Disable the module */
	spi_disable(module);

	while (spi_is_syncing(module)) {
		/* Wait until the synchronization is complete */
	}

	/* Software reset the module */
	spi_module->CTRLA.reg |= SERCOM_SPI_CTRLA_SWRST;
}

/**
 * \internal Clears the Transmit Complete interrupt flag.
 *
 * \param[in]  module  Pointer to the software instance struct
 */
static void _spi_clear_tx_complete_flag(
		struct spi_module *const module)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(module->hw);

	SercomSpi *const spi_module = &(module->hw->SPI);

	/* Clear interrupt flag */
	spi_module->INTFLAG.reg = SPI_INTERRUPT_FLAG_TX_COMPLETE;
}

/**
 * \internal Writes an SPI SERCOM configuration to the hardware module.
 *
 * This function will write out a given configuration to the hardware module.
 * Can only be done when the module is disabled.
 *
 * \param[in]  module  Pointer to the software instance struct
 * \param[in]  config  Pointer to the configuration struct
 *
 * \return The status of the configuration
 * \retval STATUS_ERR_INVALID_ARG  If invalid argument(s) were provided.
 * \retval STATUS_OK               If the configuration was written
 */
static enum status_code _spi_set_config(
		struct spi_module *const module,
		const struct spi_config *const config)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(config);
	Assert(module->hw);

	SercomSpi *const spi_module = &(module->hw->SPI);
	Sercom *const sercom_module = module->hw;

	struct system_pinmux_config pin_conf;
	system_pinmux_get_config_defaults(&pin_conf);

	uint32_t pad0 = config->pinmux_pad0;
	uint32_t pad1 = config->pinmux_pad1;
	uint32_t pad2 = config->pinmux_pad2;
	uint32_t pad3 = config->pinmux_pad3;

	/* SERCOM PAD0 */
	if (pad0 == PINMUX_DEFAULT) {
		pad0 = _sercom_get_default_pad(sercom_module, 0);
	}
	pin_conf.mux_position = pad0 & 0xFFFF;
	system_pinmux_pin_set_config(pad0 >> 16, &pin_conf);

	/* SERCOM PAD1 */
	if (pad1 == PINMUX_DEFAULT) {
		pad1 = _sercom_get_default_pad(sercom_module, 1);
	}
	if (pad1 != PINMUX_UNUSED) {
		pin_conf.mux_position = pad1 & 0xFFFF;
		system_pinmux_pin_set_config(pad1 >> 16, &pin_conf);
	}

	/* SERCOM PAD2 */
	if (pad2 == PINMUX_DEFAULT) {
		pad2 = _sercom_get_default_pad(sercom_module, 2);
	}
	if (pad2 != PINMUX_UNUSED) {
		pin_conf.mux_position = pad2 & 0xFFFF;
		system_pinmux_pin_set_config(pad2 >> 16, &pin_conf);
	}

	/* SERCOM PAD3 */
	if (pad3 == PINMUX_DEFAULT) {
		pad3 = _sercom_get_default_pad(sercom_module, 3);
	}
	if (pad3 != PINMUX_UNUSED) {
		pin_conf.mux_position = pad3 & 0xFFFF;
		system_pinmux_pin_set_config(pad3 >> 16, &pin_conf);
	}

	module->mode             = config->mode;
	module->character_size   = config->character_size;
	module->receiver_enabled = config->receiver_enable;

	/* Value to write to BAUD register */
	uint16_t baud = 0;
	/* Value to write to CTRLA register */
	uint32_t ctrla = 0;
	/* Value to write to CTRLB register */
	uint32_t ctrlb = 0;

	/* Find baud value and write it */
	if (config->mode == SPI_MODE_MASTER) {
		/* Find frequency of the internal SERCOMi_GCLK_ID_CORE */
		uint32_t sercom_index = _sercom_get_sercom_inst_index(module->hw);
		uint32_t gclk_index   = sercom_index + SERCOM0_GCLK_ID_CORE;
		uint32_t internal_clock = system_gclk_chan_get_hz(gclk_index);

		/* Get baud value, based on baudrate and the internal clock frequency */
		enum status_code error_code = _sercom_get_sync_baud_val(
				config->master.baudrate,
				internal_clock, &baud);

		if (error_code != STATUS_OK) {
			/* Baud rate calculation error, return status code */
			return STATUS_ERR_INVALID_ARG;
		}

		spi_module->BAUD.reg = (uint8_t)baud;
	}

	if (config->mode == SPI_MODE_SLAVE) {
		/* Set frame format */
		ctrla = config->slave.frame_format;

		/* Set address mode */
		ctrlb = config->slave.address_mode;

		/* Set address and address mask*/
		spi_module->ADDR.reg |=
				(config->slave.address      << SERCOM_SPI_ADDR_ADDR_Pos) |
				(config->slave.address_mask << SERCOM_SPI_ADDR_ADDRMASK_Pos);

		if (config->slave.preload_enable) {
			/* Enable pre-loading of shift register */
			ctrlb |= SERCOM_SPI_CTRLB_PLOADEN;
		}
	}

	/* Set data order */
	ctrla |= config->data_order;

	/* Set clock polarity and clock phase */
	ctrla |= config->transfer_mode;

	/* Set mux setting */
	ctrla |= config->mux_setting;

	/* Set SPI character size */
	ctrlb |= config->character_size;

	if (config->run_in_standby) {
		/* Enable in sleep mode */
		ctrla |= SERCOM_SPI_CTRLA_RUNSTDBY;
	}

	if (config->receiver_enable) {
		/* Enable receiver */
		ctrlb |= SERCOM_SPI_CTRLB_RXEN;
	}

	/* Write CTRLA register */
	spi_module->CTRLA.reg |= ctrla;

	/* Write CTRLB register */
	spi_module->CTRLB.reg |= ctrlb;

	return STATUS_OK;
}

#if SPI_CALLBACK_MODE == false
/**
 * \internal Checks an SPI config against current set config
 *
 * This function will check that the config does not alter the
 * configuration of the module. If the new config changes any
 * setting, the initialization will be discarded.
 *
 * \param[in]  module  Pointer to the software instance struct
 * \param[in]  config  Pointer to the configuration struct
 *
 * \return The status of the configuration
 * \retval STATUS_ERR_INVALID_ARG  If invalid argument(s) were provided.
 * \retval STATUS_ERR_DENIED       If configuration was different from previous
 * \retval STATUS_OK               If the configuration was written
 */
static enum status_code _spi_check_config(
		struct spi_module *const module,
		const struct spi_config *const config)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(config);
	Assert(module->hw);

	SercomSpi *const spi_module = &(module->hw->SPI);
	Sercom *const sercom_module = module->hw;

	uint32_t pad0 = config->pinmux_pad0;
	uint32_t pad1 = config->pinmux_pad1;
	uint32_t pad2 = config->pinmux_pad2;
	uint32_t pad3 = config->pinmux_pad3;

	/* SERCOM PAD0 */
	if (pad0 == PINMUX_DEFAULT) {
		pad0 = _sercom_get_default_pad(sercom_module, 0);
	}
	if (pad0 != PINMUX_UNUSED) {
		if ((pad0 & 0xFFFF) != system_pinmux_pin_get_mux_position(pad0 >> 16)) {
			module->hw = NULL;
			return STATUS_ERR_DENIED;
		}
	}

	/* SERCOM PAD1 */
	if (pad1 == PINMUX_DEFAULT) {
		pad1 = _sercom_get_default_pad(sercom_module, 1);
	}
	if (pad1 != PINMUX_UNUSED) {
		if ((pad1 & 0xFFFF) != system_pinmux_pin_get_mux_position(pad1 >> 16)) {
			module->hw = NULL;
			return STATUS_ERR_DENIED;
		}
	}

	/* SERCOM PAD2 */
	if (pad2 == PINMUX_DEFAULT) {
		pad2 = _sercom_get_default_pad(sercom_module, 2);
	}
	if (pad2 != PINMUX_UNUSED) {
		if ((pad2 & 0xFFFF) != system_pinmux_pin_get_mux_position(pad2 >> 16)) {
			module->hw = NULL;
			return STATUS_ERR_DENIED;
		}
	}

	/* SERCOM PAD3 */
	if (pad3 == PINMUX_DEFAULT) {
		pad3 = _sercom_get_default_pad(sercom_module, 3);
	}
	if (pad3 != PINMUX_UNUSED) {
		if ((pad3 & 0xFFFF) != system_pinmux_pin_get_mux_position(pad3 >> 16)) {
			module->hw = NULL;
			return STATUS_ERR_DENIED;
		}
	}

	/* Value to read BAUD register */
	uint16_t baud;

	/* Value to read CTRLA, CTRLB and ADDR register */
	uint32_t ctrla = 0;
	uint32_t ctrlb = 0;
	uint32_t addr = 0;

	uint32_t external_clock = system_gclk_chan_get_hz(SERCOM_GCLK_ID);

	/* Find baud value and compare it */
	if (config->mode == SPI_MODE_MASTER) {
		enum status_code error_code = _sercom_get_sync_baud_val(
				config->master.baudrate,
				external_clock, &baud);

		if (error_code != STATUS_OK) {
			/* Baud rate calculation error, return status code */
			return STATUS_ERR_INVALID_ARG;
		}

		if (spi_module->BAUD.reg !=  (uint8_t)baud) {
			return STATUS_ERR_DENIED;
		}

		ctrla |= SERCOM_SPI_CTRLA_MODE_SPI_MASTER;
	} else {
		/* Set frame format */
		ctrla |= config->slave.frame_format;

		/* Set address mode */
		ctrlb |= config->slave.address_mode;

		/* Set address and address mask*/
		addr |= (config->slave.address      << SERCOM_SPI_ADDR_ADDR_Pos) |
				(config->slave.address_mask << SERCOM_SPI_ADDR_ADDRMASK_Pos);
		if (spi_module->CTRLA.reg != addr) {
			return STATUS_ERR_DENIED;
		}

		if (config->slave.preload_enable) {
			/* Enable pre-loading of shift register */
			ctrlb |= SERCOM_SPI_CTRLB_PLOADEN;
		}

		ctrla |= SERCOM_SPI_CTRLA_MODE_SPI_SLAVE;
	}

	/* Set data order */
	ctrla |= config->data_order;

	/* Set clock polarity and clock phase */
	ctrla |= config->transfer_mode;

	/* Set mux setting */
	ctrla |= config->mux_setting;

	/* Set SPI character size */
	ctrlb |= config->character_size;

	if (config->run_in_standby) {
		/* Enable in sleep mode */
		ctrla |= SERCOM_SPI_CTRLA_RUNSTDBY;
	}

	if (config->receiver_enable) {
		/* Enable receiver */
		ctrlb |= SERCOM_SPI_CTRLB_RXEN;
	}

	ctrla |= SERCOM_SPI_CTRLA_ENABLE;

	/* Check that same config is set */
	if (spi_module->CTRLA.reg == ctrla &&
			spi_module->CTRLB.reg == ctrlb) {
		module->mode           = config->mode;
		module->character_size = config->character_size;
		return STATUS_OK;
	}

	/* Not same config, wipe module pointer and return */
	module->hw = NULL;

	return STATUS_ERR_DENIED;
}
#endif

/**
 * \brief Initializes the SERCOM SPI module
 *
 * This function will initialize the SERCOM SPI module, based on the values
 * of the config struct.
 *
 * \param[out]  module  Pointer to the software instance struct
 * \param[in]   hw      Pointer to hardware instance
 * \param[in]   config  Pointer to the config struct
 *
 * \return Status of the initialization
 * \retval STATUS_OK               Module initiated correctly.
 * \retval STATUS_ERR_DENIED       If module is enabled.
 * \retval STATUS_BUSY             If module is busy resetting.
 * \retval STATUS_ERR_INVALID_ARG  If invalid argument(s) were provided.
 */
enum status_code spi_init(
		struct spi_module *const module,
		Sercom *const hw,
		const struct spi_config *const config)
{

	/* Sanity check arguments */
	Assert(module);
	Assert(hw);
	Assert(config);

	/* Initialize device instance */
	module->hw = hw;

	SercomSpi *const spi_module = &(module->hw->SPI);

	/* Check if module is enabled. */
	if (spi_module->CTRLA.reg & SERCOM_SPI_CTRLA_ENABLE) {
#  if SPI_CALLBACK_MODE == false
		/* Check if config is valid */
		return _spi_check_config(module, config);
#  else
		return STATUS_ERR_DENIED;
#  endif
	}

	/* Check if reset is in progress. */
	if (spi_module->CTRLA.reg & SERCOM_SPI_CTRLA_SWRST){
		return STATUS_BUSY;
	}

	uint32_t sercom_index = _sercom_get_sercom_inst_index(module->hw);
	uint32_t pm_index     = sercom_index + PM_APBCMASK_SERCOM0_Pos;
	uint32_t gclk_index   = sercom_index + SERCOM0_GCLK_ID_CORE;

	/* Turn on module in PM */
	system_apb_clock_set_mask(SYSTEM_CLOCK_APB_APBC, 1 << pm_index);

	/* Set up the GCLK for the module */
	struct system_gclk_chan_config gclk_chan_conf;
	system_gclk_chan_get_config_defaults(&gclk_chan_conf);
	gclk_chan_conf.source_generator = config->generator_source;
	system_gclk_chan_set_config(gclk_index, &gclk_chan_conf);
	system_gclk_chan_enable(gclk_index);
	sercom_set_gclk_generator(config->generator_source, false);

	if (config->mode == SPI_MODE_MASTER) {
		/* Set the SERCOM in SPI master mode */
		spi_module->CTRLA.reg |= SERCOM_SPI_CTRLA_MODE_SPI_MASTER;
	}
	else {
		/* Set the SERCOM in SPI slave mode */
		spi_module->CTRLA.reg |= SERCOM_SPI_CTRLA_MODE_SPI_SLAVE;
	}

#if SPI_CALLBACK_MODE == true
	/* Temporary variables */
	uint8_t i;
	uint8_t instance_index;

	/* Initialize parameters */
	for (i = 0; i < SPI_CALLBACK_N; i++) {
		module->callback[i]        = NULL;
	}
	module->tx_buffer_ptr              = NULL;
	module->rx_buffer_ptr              = NULL;
	module->remaining_tx_buffer_length = 0x0000;
	module->remaining_rx_buffer_length = 0x0000;
	module->registered_callback        = 0x00;
	module->enabled_callback           = 0x00;
	module->status                     = STATUS_OK;
	module->dir                        = SPI_DIRECTION_IDLE;
	/*
	 * Set interrupt handler and register SPI software module struct in
	 * look-up table
	 */
	instance_index = _sercom_get_sercom_inst_index(module->hw);
	_sercom_set_handler(instance_index, _spi_interrupt_handler);
	_sercom_instances[instance_index] = module;
#endif

	/* Write configuration to module and return status code */
	return _spi_set_config(module, config);
}

/**
 * \brief Reads buffer of \c length SPI characters
 *
 * This function will read a buffer of data from an SPI peripheral by sending
 * dummy SPI character if in master mode, or by waiting for data in slave mode.
 *
 * \note If address matching is enabled for the slave, the first character
 *       received and placed in the buffer will be the address.
 *
 * \param[in]  module   Pointer to the software instance struct
 * \param[out] rx_data  Data buffer for received data
 * \param[in]  length   Length of data to receive
 * \param[in]  dummy    8- or 9-bit dummy byte to shift out in master mode
 *
 * \return Status of the read operation
 * \retval STATUS_OK              If the read was completed
 * \retval STATUS_ABORTED          If transaction was ended by master before
 *                                 entire buffer was transferred
 * \retval STATUS_ERR_INVALID_ARG If invalid argument(s) were provided.
 * \retval STATUS_ERR_TIMEOUT     If the operation was not completed within the
 *                                timeout in slave mode.
 * \retval STATUS_ERR_DENIED      If the receiver is not enabled
 * \retval STATUS_ERR_OVERFLOW    If the data is overflown
 */
enum status_code spi_read_buffer_wait(
		struct spi_module *const module,
		uint8_t *rx_data,
		uint16_t length,
		uint16_t dummy)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(module->hw);

#  if SPI_CALLBACK_MODE == true
	if (module->status == STATUS_BUSY) {
		/* Check if the SPI module is busy with a job */
		return STATUS_BUSY;
	}
#  endif

	/* Sanity check arguments */
	if (length == 0) {
		return STATUS_ERR_INVALID_ARG;
	}

	if (!(module->receiver_enabled)) {
		return STATUS_ERR_DENIED;
	}

	if ((module->mode == SPI_MODE_SLAVE) && (spi_is_write_complete(module))) {
		/* Clear TX complete flag */
		_spi_clear_tx_complete_flag(module);
	}

	uint16_t rx_pos = 0;

	while (length--) {
		if (module->mode == SPI_MODE_MASTER) {
			/* Wait until the module is ready to write a character */
			while (!spi_is_ready_to_write(module)) {
			}

			/* Send dummy SPI character to read in master mode */
			spi_write(module, dummy);
		}

		/* Start timeout period for slave */
		if (module->mode == SPI_MODE_SLAVE) {
			for (uint32_t i = 0; i <= SPI_TIMEOUT; i++) {
				if (spi_is_ready_to_read(module)) {
					break;
				}
			}
			/* Check if master has ended the transaction */
			if (spi_is_write_complete(module)) {
				_spi_clear_tx_complete_flag(module);
				return STATUS_ABORTED;
			}

			if (!spi_is_ready_to_read(module)) {
				/* Not ready to read data within timeout period */
				return STATUS_ERR_TIMEOUT;
			}
		}

		/* Wait until the module is ready to read a character */
		while (!spi_is_ready_to_read(module)) {
		}

		uint16_t received_data = 0;
		enum status_code retval = spi_read(module, &received_data);

		if (retval != STATUS_OK) {
			/* Overflow, abort */
			return retval;
		}

		/* Read value will be at least 8-bits long */
		rx_data[rx_pos++] = received_data;

		/* If 9-bit data, write next received byte to the buffer */
		if (module->character_size == SPI_CHARACTER_SIZE_9BIT) {
			rx_data[rx_pos++] = (received_data >> 8);
		}
	}

	return STATUS_OK;
}

 /**
 * \brief Selects slave device
 *
 * This function will drive the slave select pin of the selected device low or
 * high depending on the select boolean.
 * If slave address recognition is enabled, the address will be sent to the
 * slave when selecting it.
 *
 * \param[in] module  Pointer to the software module struct
 * \param[in] slave   Pointer to the attached slave
 * \param[in] select  Boolean stating if the slave should be selected or
 *                    deselected
 *
 * \return Status of the operation
 * \retval STATUS_OK                   If the slave device was selected
 * \retval STATUS_ERR_UNSUPPORTED_DEV  If the SPI module is operating in slave
 *                                     mode
 * \retval STATUS_BUSY                 If the SPI module is not ready to write
 *                                     the slave address
 */
enum status_code spi_select_slave(
		struct spi_module *const module,
		struct spi_slave_inst *const slave,
		const bool select)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(module->hw);
	Assert(slave);

	/* Check that the SPI module is operating in master mode */
	if (module->mode != SPI_MODE_MASTER) {
		return STATUS_ERR_UNSUPPORTED_DEV;
	}

	if (select) {
		/* Drive Slave Select low */
		port_pin_set_output_level(slave->ss_pin, false);

		/* Check if address recognition is enabled */
		if (slave->address_enabled) {
			/* Check if the module is ready to write the address */
			if (!spi_is_ready_to_write(module)) {
				/* Not ready, do not select slave and return */
				port_pin_set_output_level(slave->ss_pin, true);
				return STATUS_BUSY;
			}

			/* Write address to slave */
			spi_write(module, slave->address);

			if (!(module->receiver_enabled)) {
				/* Flush contents of shift register shifted back from slave */
				while (!spi_is_ready_to_read(module)) {
				}
				uint16_t flush = 0;
				spi_read(module, &flush);
			}
		}
	} else {
		/* Drive Slave Select high */
		port_pin_set_output_level(slave->ss_pin, true);
	}

	return STATUS_OK;
}

/**
 * \brief Sends a buffer of \c length SPI characters
 *
 * This function will send a buffer of SPI characters via the SPI
 * and discard any data that is received. To both send and receive a buffer of
 * data, use the \ref spi_transceive_buffer_wait function.
 *
 * Note that this function does not handle the _SS (slave select) pin(s) in
 * master mode; this must be handled by the user application.
 *
 * \param[in] module   Pointer to the software instance struct
 * \param[in] tx_data  Pointer to the buffer to transmit
 * \param[in] length   Number of SPI characters to transfer
 *
 * \return Status of the write operation
 * \retval STATUS_OK               If the write was completed
 * \retval STATUS_ABORTED          If transaction was ended by master before
 *                                 entire buffer was transferred
 * \retval STATUS_ERR_INVALID_ARG  If invalid argument(s) were provided
 * \retval STATUS_ERR_TIMEOUT      If the operation was not completed within the
 *                                 timeout in slave mode
 */
enum status_code spi_write_buffer_wait(
		struct spi_module *const module,
		const uint8_t *tx_data,
		uint16_t length)
{
	/* Sanity check arguments */
	Assert(module);

#  if SPI_CALLBACK_MODE == true
	if (module->status == STATUS_BUSY) {
		/* Check if the SPI module is busy with a job */
		return STATUS_BUSY;
	}
#  endif

	if (length == 0) {
		return STATUS_ERR_INVALID_ARG;
	}

	if ((module->mode == SPI_MODE_SLAVE) && (spi_is_write_complete(module))) {
		/* Clear TX complete flag */
		_spi_clear_tx_complete_flag(module);
	}

	uint16_t tx_pos = 0;

	/* Write block */
	while (length--) {
		/* Start timeout period for slave */
		if (module->mode == SPI_MODE_SLAVE) {
			for (uint32_t i = 0; i <= SPI_TIMEOUT; i++) {
				if (spi_is_ready_to_write(module)) {
					break;
				}
			}
			/* Check if master has ended the transaction */
			if (spi_is_write_complete(module)) {
				_spi_clear_tx_complete_flag(module);
				return STATUS_ABORTED;
			}

			if (!spi_is_ready_to_write(module)) {
				/* Not ready to write data within timeout period */
				return STATUS_ERR_TIMEOUT;
			}
		}

		/* Wait until the module is ready to write a character */
		while (!spi_is_ready_to_write(module)) {
		}

		/* Write value will be at least 8-bits long */
		uint16_t data_to_send = tx_data[tx_pos++];

		/* If 9-bit data, get next byte to send from the buffer */
		if (module->character_size == SPI_CHARACTER_SIZE_9BIT) {
			data_to_send |= (tx_data[tx_pos++] << 8);
		}

		/* Write the data to send */
		spi_write(module, data_to_send);

		if (module->receiver_enabled) {

			/* Start timeout period for slave */
			if (module->mode == SPI_MODE_SLAVE) {
				for (uint32_t i = 0; i <= SPI_TIMEOUT; i++) {
					if (spi_is_ready_to_read(module)) {
						break;
					}
				}
			/* Check if master has ended the transaction */
			if (spi_is_write_complete(module)) {
				_spi_clear_tx_complete_flag(module);
				return STATUS_ABORTED;
			}

				if (!spi_is_ready_to_read(module)) {
					/* Not ready to read data within timeout period */
					return STATUS_ERR_TIMEOUT;
				}
			}

			while (!spi_is_ready_to_read(module)) {
			}

			/* Flush read buffer */
			uint16_t flush;
			spi_read(module, &flush);
		}
	}

	if (module->mode == SPI_MODE_MASTER) {
		/* Wait for last byte to be transferred */
		while (!spi_is_write_complete(module)) {
		}
	}

	return STATUS_OK;
}

/**
 * \brief Sends and receives a buffer of \c length SPI characters
 *
 * This function will send and receive a buffer of data via the SPI.
 *
 * In master mode the SPI characters will be sent immediately and the
 * received SPI character will  be read as soon as the shifting of the SPI
 * character is complete.
 *
 * In slave mode this function will place the data to be sent into the transmit
 * buffer. It will then block until an SPI master has shifted the complete
 * buffer and the received data is available.
 *
 * \param[in]  module   Pointer to the software instance struct
 * \param[in]  tx_data  Pointer to the buffer to transmit
 * \param[out] rx_data  Pointer to the buffer where received data will be stored
 * \param[in]  length   Number of SPI characters to transfer
 *
 * \return Status of the operation
 * \retval STATUS_OK               If the operation was completed
 * \retval STATUS_ERR_INVALID_ARG  If invalid argument(s) were provided.
 * \retval STATUS_ERR_TIMEOUT      If the operation was not completed within the
 *                                 timeout in slave mode.
 * \retval STATUS_ERR_DENIED       If the receiver is not enabled
 * \retval STATUS_ERR_OVERFLOW     If the data is overflown
 */
enum status_code spi_transceive_buffer_wait(
		struct spi_module *const module,
		uint8_t *tx_data,
		uint8_t *rx_data,
		uint16_t length)
{
	/* Sanity check arguments */
	Assert(module);

#  if SPI_CALLBACK_MODE == true
	if (module->status == STATUS_BUSY) {
		/* Check if the SPI module is busy with a job */
		return STATUS_BUSY;
	}
#  endif

	/* Sanity check arguments */
	if (length == 0) {
		return STATUS_ERR_INVALID_ARG;
	}
	
	if (!(module->receiver_enabled)) {
		return STATUS_ERR_DENIED;
	}

	if ((module->mode == SPI_MODE_SLAVE) && (spi_is_write_complete(module))) {
		/* Clear TX complete flag */
		_spi_clear_tx_complete_flag(module);
	}

	uint16_t tx_pos = 0;
	uint16_t rx_pos = 0;

	/* Send and receive buffer */
	while (length--) {
		/* Start timeout period for slave */
		if (module->mode == SPI_MODE_SLAVE) {
			for (uint32_t i = 0; i <= SPI_TIMEOUT; i++) {
				if (spi_is_ready_to_write(module)) {
					break;
				}
			}
			/* Check if master has ended the transaction */
			if (spi_is_write_complete(module)) {
				_spi_clear_tx_complete_flag(module);
				return STATUS_ABORTED;
			}

			if (!spi_is_ready_to_write(module)) {
				/* Not ready to write data within timeout period */
				return STATUS_ERR_TIMEOUT;
			}
		}

		/* Wait until the module is ready to write a character */
		while (!spi_is_ready_to_write(module)) {
		}

		/* Write value will be at least 8-bits long */
		uint16_t data_to_send = tx_data[tx_pos++];

		/* If 9-bit data, get next byte to send from the buffer */
		if (module->character_size == SPI_CHARACTER_SIZE_9BIT) {
			data_to_send |= (tx_data[tx_pos++] << 8);
		}

		/* Write the data to send */
		spi_write(module, data_to_send);

		/* Start timeout period for slave */
		if (module->mode == SPI_MODE_SLAVE) {
			for (uint32_t i = 0; i <= SPI_TIMEOUT; i++) {
				if (spi_is_ready_to_read(module)) {
					break;
				}
			}
			/* Check if master has ended the transaction */
			if (spi_is_write_complete(module)) {
				_spi_clear_tx_complete_flag(module);
				return STATUS_ABORTED;
			}

			if (!spi_is_ready_to_read(module)) {
				/* Not ready to read data within timeout period */
				return STATUS_ERR_TIMEOUT;
			}
		}

		/* Wait until the module is ready to read a character */
		while (!spi_is_ready_to_read(module)) {
		}

		enum status_code retval;
		uint16_t received_data = 0;

		retval = spi_read(module, &received_data);

		if (retval != STATUS_OK) {
			/* Overflow, abort */
			return retval;
		}

		/* Read value will be at least 8-bits long */
		rx_data[rx_pos++] = received_data;

		/* If 9-bit data, write next received byte to the buffer */
		if (module->character_size == SPI_CHARACTER_SIZE_9BIT) {
			rx_data[rx_pos++] = (received_data >> 8);
		}
	}

	if (module->mode == SPI_MODE_MASTER) {
		/* Wait for last byte to be transferred */
		while (!spi_is_write_complete(module)) {
		}
	}

	return STATUS_OK;
}
