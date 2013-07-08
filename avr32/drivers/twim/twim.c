/*****************************************************************************
 *
 * \file
 *
 * \brief TWIM driver for AVR32 UC3.
 *
 * This file defines a useful set of functions for TWIM on AVR32 devices.
 *
 *****************************************************************************/
/**
 * Copyright (c) 2010-2012 Atmel Corporation. All rights reserved.
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

#include "twim.h"

//! \internal Pointer to the instance 1 of the TWIM registers for IT.
static volatile avr32_twim_t *twim_inst;
//! \internal Pointer to the applicative TWI transmit buffer.
static const volatile uint8_t  *twim_tx_data = NULL;
//! \internal Pointer to the applicative TWI receive buffer.
static volatile uint8_t *twim_rx_data = NULL;
//! \internal Status of the bus transfer
static volatile twim_transfer_status_t transfer_status;
//! \internal Remaining number of bytes to transmit.
static volatile uint32_t twim_tx_nb_bytes = 0;
//! \internal Remaining number of bytes to receive.
static volatile uint32_t twim_rx_nb_bytes = 0;
//! \internal IT mask.
static volatile uint32_t twim_it_mask;
//! \internal Pointer to twim package
static volatile const twim_package_t *twim_package;
//! \internal If Internal Address access is used
static volatile bool twim_next =false;

/**
 * \internal
 * \brief TWI interrupt handler.
 */
ISR(twim_master_interrupt_handler,CONF_TWIM_IRQ_GROUP,CONF_TWIM_IRQ_LEVEL)
{
	// get masked status register value
	uint32_t status = twim_inst->sr & twim_it_mask;
	// this is a NACK
	if (status & AVR32_TWIM_SR_STD_MASK) {
		//if we get a nak, clear the valid bit in cmdr,
		//otherwise the command will be resent.
		transfer_status =(status & AVR32_TWIM_IER_NAK_MASK) ?
							TWI_RECEIVE_NACK : TWI_ARBITRATION_LOST;
		twim_inst->CMDR.valid = 0;
		twim_inst->scr = ~0UL;
		twim_inst->idr = ~0UL;
		twim_next = false;
	}
	// this is a RXRDY
	else if (status & AVR32_TWIM_SR_RXRDY_MASK) {
		// get data from Receive Holding Register
		*twim_rx_data = twim_inst->rhr;
		twim_rx_data++;
		// decrease received bytes number
		twim_rx_nb_bytes--;
		// receive complete
		if (twim_rx_nb_bytes == 0) {
			// finish the receive operation
			twim_inst->idr = AVR32_TWIM_IDR_RXRDY_MASK;
			// set busy to false
			twim_next = false;
		}
	}
	// this is a TXRDY
	else if (status & AVR32_TWIM_SR_TXRDY_MASK) {
		// no more bytes to transmit
		if (twim_tx_nb_bytes == 0) {
			// finish the receive operation
			twim_inst->idr = AVR32_TWIM_IDR_TXRDY_MASK;
			// set busy to false
			twim_next = false;
		} else {
			// put the byte in the Transmit Holding Register
			twim_inst->thr = *twim_tx_data++;
			// decrease transmitted bytes number
			twim_tx_nb_bytes--;
			if (twim_tx_nb_bytes == 0) {
				// Check for next transfer
				if(twim_next) {
					twim_next = false;
					twim_tx_nb_bytes = twim_package->length;
					twim_tx_data = twim_package->buffer;
				}
			}
		}
	}
	return;
}

/**
 * \brief Set the twim bus speed in conjunction with the clock frequency
 *
 * \param twim              Base address of the TWIM (i.e. &AVR32_TWIM).
 * \param speed             The desired twim bus speed
 * \param pba_hz            The current running PBA clock frequency
 * \retval STATUS_OK        Transaction is successful
 * \retval ERR_INVALID_ARG  Invalid arg resulting in wrong CWGR Exponential
 */
status_code_t twim_set_speed (volatile avr32_twim_t *twim, uint32_t speed,
		uint32_t pba_hz)
{
	uint32_t f_prescaled;
	uint8_t cwgr_exp = 0;
	f_prescaled = (pba_hz / speed / 2);
	// f_prescaled must fit in 8 bits, cwgr_exp must fit in 3 bits
	while ((f_prescaled > 0xFF) && (cwgr_exp <= 0x7)) {
		// increase clock divider
		cwgr_exp++;
		// divide f_prescaled value
		f_prescaled /= 2;
	}
	if (cwgr_exp > 0x7) {
		return ERR_INVALID_ARG;
	}
	// set clock waveform generator register
	twim->cwgr = ((f_prescaled/2) << AVR32_TWIM_CWGR_LOW_OFFSET)
			| ((f_prescaled - f_prescaled/2) << AVR32_TWIM_CWGR_HIGH_OFFSET)
			| (cwgr_exp << AVR32_TWIM_CWGR_EXP_OFFSET)
			| (0     << AVR32_TWIM_CWGR_DATA_OFFSET)
			| (f_prescaled << AVR32_TWIM_CWGR_STASTO_OFFSET);
	return STATUS_OK;
}

/**
 * \brief Initialize the twi master module
 *
 * \param twim              Base address of the TWIM (i.e. &AVR32_TWIM)
 * \param *opt              Options for initializing the twim module
 *                          (see \ref twim_options_t)
 * \retval STATUS_OK        Transaction is successful
 * \retval ERR_INVALID_ARG  Invalid arg resulting in wrong CWGR Exponential
 * \retval ERR_IO_ERROR     NACK is received or Bus Arbitration lost
 */
status_code_t twim_master_init (volatile avr32_twim_t *twim,
		const twim_options_t *opt)
{
	bool global_interrupt_enabled = cpu_irq_is_enabled ();
	// Initialize bus transfer status
	transfer_status = TWI_SUCCESS;
	// Disable TWI interrupts
	if (global_interrupt_enabled) {
		cpu_irq_disable ();
	}
	twim->idr = ~0UL;
	// Enable master transfer
	twim->cr = AVR32_TWIM_CR_MEN_MASK;
	// Reset TWI
	twim->cr = AVR32_TWIM_CR_SWRST_MASK;
	if (global_interrupt_enabled) {
		cpu_irq_enable ();
	}
	// Clear SR
	twim->scr = ~0UL;

	// register Register twim_master_interrupt_handler interrupt on level CONF_TWIM_IRQ_LEVEL
	irqflags_t flags = cpu_irq_save();
	irq_register_handler(twim_master_interrupt_handler,
			CONF_TWIM_IRQ_LINE, CONF_TWIM_IRQ_LEVEL);
	cpu_irq_restore(flags);

	if (opt->smbus) {
		// Enable SMBUS Transfer
		twim->cr = AVR32_TWIM_CR_SMEN_MASK;
		twim->smbtr = (uint32_t) -1;
	}
	// Select the speed
	if (twim_set_speed (twim, opt->speed, opt->pba_hz) ==
			ERR_INVALID_ARG) {
		return ERR_INVALID_ARG;
	}
	// Probe the component
	twim_probe (twim, opt->chip);
	//Check for nack and arbitration
	if (transfer_status == TWI_RECEIVE_NACK
			|| transfer_status == TWI_ARBITRATION_LOST) {
		return ERR_IO_ERROR;
	}
	return STATUS_OK;
}

/**
 * \brief Disable the TWI interrupts and clear its status register
 *
 * \param twim         Base address of the TWIM (i.e. &AVR32_TWI).
 */
void twim_disable_interrupt (volatile avr32_twim_t *twim)
{
	bool global_interrupt_enabled = cpu_irq_is_enabled ();
	if (global_interrupt_enabled) {
		cpu_irq_disable ();
	}
	// Clear the interrupt flags
	twim->idr = ~0UL;
	// Clear the status flags
	twim->scr = ~0UL;
}

/**
 * \brief Test if a chip answers for a given twi address
 *
 * \param twim            Base address of the TWIM (i.e. &AVR32_TWIM)
 * \param chip_addr       Address of the chip which is searched for
 * \retval STATUS_OK      Slave Found
 * \retval ERR_IO_ERROR   ANAK received or Bus Arbitration lost
 */
status_code_t twim_probe (volatile avr32_twim_t *twim, uint32_t chip_addr)
{
	uint8_t data[1] = { 0 };
	return (twim_write (twim,data,0,chip_addr,0));
}

/**
 * \brief Read multiple bytes from a TWI compatible slave device
 *
 * \param twim            Base address of the TWIM (i.e. &AVR32_TWIM)
 * \param package         Package information and data
 *                        (see \ref twim_package_t)
 * \retval STATUS_OK      If all bytes were read successfully
 * \retval ERR_IO_ERROR   NACK received or Bus Arbitration lost
 */
status_code_t twim_read_packet (volatile avr32_twim_t *twim,
		const twim_package_t *package)
{
	// Disable master transfer
	twim->cr = AVR32_TWIM_CR_MDIS_MASK;
	// Set pointer to TWIM instance for IT
	twim_inst = twim;
	// Disable the TWIM interrupts
	twim_disable_interrupt (twim_inst);
	// get a pointer to applicative data
	twim_rx_data = package->buffer;
	// get a copy of nb bytes to read
	twim_rx_nb_bytes = package->length;
	// Set next write transfer to false
	twim_next = false;
	// Initialize bus transfer status
	transfer_status = TWI_SUCCESS;
	//check if internal address access is performed
	if (package->addr_length) {
		// Reset the TWIM module to clear the THR register
		twim_inst->cr = AVR32_TWIM_CR_MEN_MASK;
		twim_inst->cr = AVR32_TWIM_CR_SWRST_MASK;
		twim_inst->cr = AVR32_TWIM_CR_MDIS_MASK;
		// selection of first valid byte of the address
		twim_tx_data = package->addr;
		// set the number of bytes to transmit
		twim_tx_nb_bytes = package->addr_length;
		// mask NACK, TXRDY and RXRDY interrupts
		twim_it_mask = AVR32_TWIM_IER_STD_MASK |
				AVR32_TWIM_IER_TXRDY_MASK | AVR32_TWIM_IER_RXRDY_MASK;
		// Set the command register to initiate the transfer
		twim_inst->cmdr = (package->chip << AVR32_TWIM_CMDR_SADR_OFFSET)
				| (package->addr_length << AVR32_TWIM_CMDR_NBYTES_OFFSET)
				| (AVR32_TWIM_CMDR_VALID_MASK)
				| (AVR32_TWIM_CMDR_START_MASK)
				| (0 << AVR32_TWIM_CMDR_READ_OFFSET);
		// set the next command register to followup with the previous command
		twim_inst->ncmdr = ((package->chip) << AVR32_TWIM_CMDR_SADR_OFFSET)
				| (package->length << AVR32_TWIM_CMDR_NBYTES_OFFSET)
				| (AVR32_TWIM_CMDR_VALID_MASK)
				| (AVR32_TWIM_CMDR_START_MASK)
				| (AVR32_TWIM_CMDR_STOP_MASK)
				| (AVR32_TWIM_CMDR_READ_MASK);
	} else {
		twim_tx_nb_bytes = 0;
		// mask NACK and RXRDY interrupts
		twim_it_mask = AVR32_TWIM_IER_STD_MASK | AVR32_TWIM_IER_RXRDY_MASK;
		// Set the command register to initiate the transfer
		twim_inst->cmdr = (package->chip << AVR32_TWIM_CMDR_SADR_OFFSET)
				| (package->length << AVR32_TWIM_CMDR_NBYTES_OFFSET)
				| (AVR32_TWIM_CMDR_VALID_MASK)
				| (AVR32_TWIM_CMDR_START_MASK)
				| (AVR32_TWIM_CMDR_STOP_MASK)
				| (AVR32_TWIM_CMDR_READ_MASK);
	}
	// update IMR through IER
	twim_inst->ier = twim_it_mask;
	// Enable master transfer
	twim->cr = AVR32_TWIM_CR_MEN_MASK;
	// Enable all interrupts
	cpu_irq_enable ();
	// get data
	while (!(transfer_status) && !(twim_status ())) {
		cpu_relax();
	}
	// Disable master transfer
	twim->cr = AVR32_TWIM_CR_MDIS_MASK;
	if (transfer_status == TWI_RECEIVE_NACK
			|| transfer_status == TWI_ARBITRATION_LOST) {
		return ERR_IO_ERROR;
	}
	return STATUS_OK;
}

/**
 * \brief Read multiple bytes from a TWI compatible slave device
 *
 * \param twim            Base address of the TWIM (i.e. &AVR32_TWIM)
 * \param *buffer         Received data
 * \param nbytes          Number of bytes to be transmitted
 * \param saddr           Slave address
 * \param tenbit          Ten bit addressing
 * \retval STATUS_OK      If all bytes were read successfully
 * \retval ERR_IO_ERROR   NACK received or Bus Arbitration lost
 */
status_code_t twim_read (volatile avr32_twim_t *twim, uint8_t *buffer,
		uint32_t nbytes, uint32_t saddr, bool tenbit)
{
	// Reset the TWIM module to clear the THR register
	twim->cr = AVR32_TWIM_CR_MEN_MASK;
	twim->cr = AVR32_TWIM_CR_SWRST_MASK;
	twim->cr = AVR32_TWIM_CR_MDIS_MASK;
	// Set pointer to TWIM instance for IT
	twim_inst = twim;
	// Disable the TWIM interrupts
	twim_disable_interrupt (twim_inst);
	// get a pointer to applicative data
	twim_rx_data = buffer;
	// get a copy of nb bytes to read
	twim_rx_nb_bytes = nbytes;
	// Set next transfer to false
	twim_next = false;
	// Initialize bus transfer status
	transfer_status = TWI_SUCCESS;
	//tenbit need special handling
	if (tenbit)	{
		twim_inst->cmdr = (saddr << AVR32_TWIM_CMDR_SADR_OFFSET)
				| (0 << AVR32_TWIM_CMDR_NBYTES_OFFSET)
				| (AVR32_TWIM_CMDR_VALID_MASK)
				| (AVR32_TWIM_CMDR_START_MASK)
				| (0 << AVR32_TWIM_CMDR_STOP_OFFSET)
				| (AVR32_TWIM_CMDR_TENBIT_MASK)
				| (0 << AVR32_TWIM_CMDR_READ_OFFSET);

		twim_inst->ncmdr = (saddr << AVR32_TWIM_CMDR_SADR_OFFSET)
				| (nbytes << AVR32_TWIM_CMDR_NBYTES_OFFSET)
				| (AVR32_TWIM_CMDR_VALID_MASK)
				| (AVR32_TWIM_CMDR_START_MASK)
				| (AVR32_TWIM_CMDR_STOP_MASK)
				| (AVR32_TWIM_CMDR_TENBIT_MASK)
				| (AVR32_TWIM_CMDR_REPSAME_MASK)
				| (AVR32_TWIM_CMDR_READ_MASK);
	} else {
		twim_inst->cmdr = (saddr << AVR32_TWIM_CMDR_SADR_OFFSET)
				| (nbytes << AVR32_TWIM_CMDR_NBYTES_OFFSET)
				| (AVR32_TWIM_CMDR_VALID_MASK)
				| (AVR32_TWIM_CMDR_START_MASK)
				| (AVR32_TWIM_CMDR_STOP_MASK)
				| (0 << AVR32_TWIM_CMDR_TENBIT_OFFSET)
				| (AVR32_TWIM_CMDR_READ_MASK);
	}
	// mask NACK and RXRDY interrupts
	twim_it_mask = AVR32_TWIM_IER_STD_MASK | AVR32_TWIM_IER_RXRDY_MASK;
	// update IMR through IER
	twim_inst->ier = twim_it_mask;
	// Enable master transfer
	twim_inst->cr = AVR32_TWIM_CR_MEN_MASK;
	// Enable all interrupts
	cpu_irq_enable ();
	// get data
	while (!(transfer_status) && !(twim_status ())) {
		cpu_relax();
	}
	// Disable master transfer
	twim->cr = AVR32_TWIM_CR_MDIS_MASK;
	//Check for nack
	if (transfer_status == TWI_RECEIVE_NACK
			|| transfer_status == TWI_ARBITRATION_LOST) {
		return ERR_IO_ERROR;
	}
	return STATUS_OK;
}

/**
 * \brief Write multiple bytes to a TWI compatible slave device
 *
 * \param twim            Base address of the TWIM (i.e. &AVR32_TWIM)
 * \param *package        Package information and data
 *                        (see \ref twim_package_t)
 * \retval STATUS_OK      If all bytes were send successfully
 * \retval ERR_IO_ERROR   NACK received or Bus Arbitration lost
 */
status_code_t twim_write_packet (volatile avr32_twim_t *twim,
		const twim_package_t *package)
{
	// Reset the TWIM module to clear the THR register
	twim->cr = AVR32_TWIM_CR_MEN_MASK;
	twim->cr = AVR32_TWIM_CR_SWRST_MASK;
	twim->cr = AVR32_TWIM_CR_MDIS_MASK;
	// Set pointer to TWIM instance for IT
	twim_inst = twim;
	// Disable the TWIM interrupts
	twim_disable_interrupt (twim_inst);
	// Initialize bus transfer status
	transfer_status = TWI_SUCCESS;
	// mask NACK and TXRDY interrupts
	twim_it_mask = AVR32_TWIM_IER_STD_MASK | AVR32_TWIM_IER_TXRDY_MASK;
	// Set next transfer to false
	twim_next = false;
	//check if internal address access is performed
	if (package->addr_length) {
		// selection of first valid byte of the address
		twim_tx_data = package->addr;
		// set the number of bytes to transmit
		twim_tx_nb_bytes = package->addr_length;
		// set next transfer to true
		twim_next = true;
		// Set the number of bytes & address for next transfer
		twim_package = package;
	} else {
	// get a pointer to applicative data
	twim_tx_data = package->buffer;
	// get a copy of nb bytes to write
	twim_tx_nb_bytes = package->length;
	}
	// initiate the transfer to send the data
	twim->cmdr = (package->chip << AVR32_TWIM_CMDR_SADR_OFFSET)
			| ((package->length + package->addr_length)
					<< AVR32_TWIM_CMDR_NBYTES_OFFSET)
			| (AVR32_TWIM_CMDR_VALID_MASK)
			| (AVR32_TWIM_CMDR_START_MASK)
			| (AVR32_TWIM_CMDR_STOP_MASK)
			| (0 << AVR32_TWIM_CMDR_READ_OFFSET);
	// update IMR through IER
	twim_inst->ier = twim_it_mask;
	// Enable master transfer
	twim_inst->cr = AVR32_TWIM_CR_MEN_MASK;
	// Enable all interrupts
	cpu_irq_enable ();
	// send data
	while (!(transfer_status) && !(twim_status ())) {
		cpu_relax();
	}
#if AVR32_TWIM_H_VERSION > 101	//Removed in twim100 module due to IC bug
	// Disable master transfer
	twim->cr = AVR32_TWIM_CR_MDIS_MASK;
#endif
	// Check for nack
	if (transfer_status == TWI_RECEIVE_NACK
			|| transfer_status == TWI_ARBITRATION_LOST) {
		return ERR_IO_ERROR;
	}
	return STATUS_OK;
}

/**
 * \brief Write multiple bytes to a TWI compatible slave device
 *
 * \param twim            Base address of the TWIM (i.e. &AVR32_TWIM).
 * \param *buffer         Data to be transmitted
 * \param nbytes          Number of bytes to be transmitted
 * \param saddr           Slave address
 * \param tenbit          Ten bit addressing
 * \retval STATUS_OK      If all bytes were send successfully
 * \retval ERR_IO_ERROR   NACK received or Bus Arbitration lost
 */
status_code_t twim_write (volatile avr32_twim_t *twim, uint8_t const *buffer,
		uint32_t nbytes, uint32_t saddr, bool tenbit)
{
	// Reset the TWIM module to clear the THR register
	twim->cr = AVR32_TWIM_CR_MEN_MASK;
	twim->cr = AVR32_TWIM_CR_SWRST_MASK;
	twim->cr = AVR32_TWIM_CR_MDIS_MASK;
	// Set pointer to TWIM instance for IT
	twim_inst = twim;
	// Disable the TWIM interrupts
	twim_disable_interrupt (twim_inst);
	// get a pointer to applicative data
	twim_tx_data = buffer;
	// set the number of bytes to transmit
	twim_tx_nb_bytes = nbytes;
	// Set next transfer to false
	twim_next = false;
	// Initialize bus transfer status
	transfer_status = TWI_SUCCESS;
	// set the command to start the transfer
	twim_inst->cmdr = (saddr << AVR32_TWIM_CMDR_SADR_OFFSET)
			| (nbytes << AVR32_TWIM_CMDR_NBYTES_OFFSET)
			| (AVR32_TWIM_CMDR_VALID_MASK)
			| (AVR32_TWIM_CMDR_START_MASK)
			| (AVR32_TWIM_CMDR_STOP_MASK)
			| ((tenbit ? 1 : 0) << AVR32_TWIM_CMDR_TENBIT_OFFSET)
			| (0 << AVR32_TWIM_CMDR_READ_OFFSET);
	// mask NACK and TXRDY interrupts
	twim_it_mask = AVR32_TWIM_IER_NAK_MASK | AVR32_TWIM_IER_TXRDY_MASK;
	// update IMR through IER
	twim_inst->ier = twim_it_mask;
	// Enable master transfer
	twim_inst->cr = AVR32_TWIM_CR_MEN_MASK;
	// Enable all interrupts
	cpu_irq_enable ();
	// send data
	while (!(transfer_status) && !(twim_status ())) {
		cpu_relax();
	}
#if AVR32_TWIM_H_VERSION > 101	// Removed in twim100 module due to IC bug
	// Disable master transfer
	twim->cr = AVR32_TWIM_CR_MDIS_MASK;
#endif
	if (transfer_status == TWI_RECEIVE_NACK
			|| transfer_status == TWI_ARBITRATION_LOST) {
		return ERR_IO_ERROR;
	}
	return STATUS_OK;
}

/**
 * \brief Perform Chained transfers to a TWI compatible slave device
 *
 * \param twim            Base address of the TWIM (i.e. &AVR32_TWIM)
 * \param *first          Information regarding first transfer
 * \param *second         Information regarding second transfer
 *                        (see \ref twim_transfer_t)
 * \param tenbit          To enable tenbit addressing
 * \retval STATUS_OK      Chain transfer successful
 * \retval ERR_IO_ERROR   NACK received or Bus Arbitration lost
 */
status_code_t twim_chained_transfer (volatile avr32_twim_t *twim,
		volatile twim_transfer_t *first,
		volatile twim_transfer_t *second, bool tenbit)
{
	// Reset the TWIM module to clear the THR register
	twim->cr = AVR32_TWIM_CR_MEN_MASK;
	twim->cr = AVR32_TWIM_CR_SWRST_MASK;
	twim->cr = AVR32_TWIM_CR_MDIS_MASK;
	// Set pointer to TWIM instance for IT
	twim_inst = twim;
	// Disable the TWIM interrupts
	twim_disable_interrupt (twim_inst);
	// Set next transfer to false
	twim_next = false;
	transfer_status = TWI_SUCCESS;
	if (tenbit && first->read) {
		twim->cmdr = (first->chip << AVR32_TWIM_CMDR_SADR_OFFSET)
				| (0 << AVR32_TWIM_CMDR_NBYTES_OFFSET)
				| (AVR32_TWIM_CMDR_VALID_MASK)
				| (AVR32_TWIM_CMDR_START_MASK)
				| (0 << AVR32_TWIM_CMDR_STOP_OFFSET)
				| (AVR32_TWIM_CMDR_TENBIT_MASK)
				| (0 << AVR32_TWIM_CMDR_READ_OFFSET);

		twim->ncmdr = (first->chip << AVR32_TWIM_CMDR_SADR_OFFSET)
				| (first->length << AVR32_TWIM_CMDR_NBYTES_OFFSET)
				| (AVR32_TWIM_CMDR_VALID_MASK)
				| (AVR32_TWIM_CMDR_START_MASK)
				| (0 << AVR32_TWIM_CMDR_STOP_OFFSET)
				| (AVR32_TWIM_CMDR_TENBIT_MASK)
				| (AVR32_TWIM_CMDR_REPSAME_MASK)
				| (AVR32_TWIM_CMDR_READ_MASK);
		while (!(twim->sr & AVR32_TWIM_SR_CCOMP_MASK)) {
			cpu_relax();
		}
		twim->scr = AVR32_TWIM_SR_CCOMP_MASK;
	} else {
		twim->cmdr = (first->chip << AVR32_TWIM_CMDR_SADR_OFFSET)
				| (first->length << AVR32_TWIM_CMDR_NBYTES_OFFSET)
				| (AVR32_TWIM_CMDR_VALID_MASK)
				| (AVR32_TWIM_CMDR_START_MASK)
				| (0 << AVR32_TWIM_CMDR_STOP_OFFSET)
				| ((tenbit ? 1 : 0) << AVR32_TWIM_CMDR_TENBIT_OFFSET)
				| ((first->read ? 1 : 0) << AVR32_TWIM_CMDR_READ_OFFSET);
	}

	twim->ncmdr = (second->chip << AVR32_TWIM_CMDR_SADR_OFFSET)
			| (second->length << AVR32_TWIM_CMDR_NBYTES_OFFSET)
			| (AVR32_TWIM_CMDR_VALID_MASK)
			| (AVR32_TWIM_CMDR_START_MASK)
			| (AVR32_TWIM_CMDR_STOP_MASK)
			| ((tenbit ? 1 : 0) << AVR32_TWIM_CMDR_TENBIT_OFFSET)
			| (((tenbit && second->read) ? 1 : 0)
			<< AVR32_TWIM_CMDR_REPSAME_OFFSET)
			| ((second->read ? 1 : 0) << AVR32_TWIM_CMDR_READ_OFFSET);

	if (first->read) {
		// get a pointer to applicative data
		twim_rx_data = first->buffer;
		// get data
		while (!(twim->sr & AVR32_TWIM_SR_CCOMP_MASK)) {
			if (twim->sr & AVR32_TWIM_SR_RXRDY_MASK) {
				*twim_rx_data++ = twim->rhr;
			}
		}
		if (twim->sr & AVR32_TWIM_SR_RXRDY_MASK) {
			*twim_rx_data++ = twim->rhr;
		}
	} else {
		// get a pointer to applicative data
		twim_tx_data = first->buffer;
		twim_tx_nb_bytes = first->length;
		// send data
		while (!(twim->sr & AVR32_TWIM_SR_CCOMP_MASK)) {
			if ((twim_tx_nb_bytes > 0) &&
					(twim->sr & AVR32_TWIM_SR_TXRDY_MASK)) {
				twim->thr = *twim_tx_data++;
				twim_tx_nb_bytes--;
			}
		}
	}

	twim->scr = AVR32_TWIM_SR_CCOMP_MASK;

	if (second->read) {
		// get a pointer to applicative data
		twim_rx_data = second->buffer;
		// get data
		while (!(twim->sr & AVR32_TWIM_SR_IDLE_MASK)) {
			if (twim->sr & AVR32_TWIM_SR_RXRDY_MASK) {
				*twim_rx_data++ = twim->rhr;
			}
		}
	} else {
		// get a pointer to applicative data
		twim_tx_data = second->buffer;

		twim_tx_nb_bytes = second->length;
		// send data
		while (!(twim->sr & AVR32_TWIM_SR_IDLE_MASK)) {
			if ((twim_tx_nb_bytes > 0) &&
					(twim->sr & AVR32_TWIM_SR_TXRDY_MASK)) {
			twim->thr = *twim_tx_data++;
			twim_tx_nb_bytes--;
			}
		}
	}

	if (twim->sr & AVR32_TWIM_SR_ARBLST_MASK) {
		twim->scr = AVR32_TWIM_SCR_ARBLST_MASK;
		return ERR_IO_ERROR;
	}

	if (twim->sr & AVR32_TWIM_SR_NAK_MASK) {
		twim->cmdr = twim->cmdr ^ AVR32_TWIM_CMDR_VALID_MASK;
		twim->scr = AVR32_TWIM_SCR_NAK_MASK;
		return ERR_IO_ERROR;
	}
	return STATUS_OK;
}

/**
 * \brief Information about the current status of the TWI Bus
 */
uint8_t twim_status ( void )
{
	uint32_t status = twim_inst->sr;
	if ((status & AVR32_TWIM_SR_IDLE_MASK)
#if AVR32_TWIM_H_VERSION > 101 ||(status&AVR32_TWIM_SR_BUSFREE_MASK)
#endif
		) {
		return 1;
	} else {
		return 0;
	}
}
