/*****************************************************************************
 *
 * \file
 *
 * \brief TWI driver for AVR32 UC3.
 *
 * This file defines a useful set of functions for TWI on AVR32 devices.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#include <avr32/io.h>
#include "compiler.h"
#include "twi.h"


//! Pointer to the instance of the TWI registers for IT.
static volatile avr32_twi_t *twi_inst;

//! Pointer to the applicative TWI transmit buffer.
static const unsigned char *volatile twi_tx_data = NULL;
//! Pointer to the applicative TWI receive buffer.
static volatile unsigned char *volatile twi_rx_data = NULL;

//! Remaining number of bytes to transmit.
static volatile int twi_tx_nb_bytes = 0;

//! Remaining number of bytes to receive.
static volatile int twi_rx_nb_bytes = 0;

//! Add NACK boolean.
static volatile bool twi_nack = false;

//! Signal end of transaction.
static volatile bool twi_busy = false;

//! IT mask.
static volatile unsigned long twi_it_mask;

#ifndef AVR32_TWI_180_H_INCLUDED

//! Pointer on TWI slave application routines
static twi_slave_fct_t twi_slave_fct;

#endif


#define CONF_TWI_IRQ_LINE          AVR32_TWI_IRQ
#define CONF_TWI_IRQ_GROUP         AVR32_TWI_IRQ_GROUP


/*! \brief TWI interrupt handler.
 */
ISR(twi_master_interrupt_handler, CONF_TWI_IRQ_GROUP, CONF_TWI_IRQ_LEVEL)
{
	// get masked status register value
	int status = twi_inst->sr & twi_it_mask;

	// this is a NACK
	if (status & AVR32_TWI_SR_NACK_MASK) {
		goto nack;
	}
	// this is a RXRDY
	else if (status & AVR32_TWI_SR_RXRDY_MASK) {
		// get data from Receive Holding Register
		*twi_rx_data = twi_inst->rhr;
		twi_rx_data++;
		// last byte to receive
		if (--twi_rx_nb_bytes == 1) {
			// set stop bit
			twi_inst->cr = AVR32_TWI_STOP_MASK;
		}
		// receive complete
		if (twi_rx_nb_bytes == 0) {
			// finish the receive operation
			goto complete;
		}
	}
	// this is a TXRDY
	else if (status & AVR32_TWI_SR_TXRDY_MASK) {
		// decrease transmitted bytes number
		twi_tx_nb_bytes--;
		// no more bytes to transmit
		if (twi_tx_nb_bytes <= 0) {
			// enable TXCOMP IT and unmask all others IT
			twi_it_mask = AVR32_TWI_IER_TXCOMP_MASK;
			twi_inst->idr = ~0UL;
			twi_inst->ier = twi_it_mask;
		} else {
			// put the byte in the Transmit Holding Register
			twi_inst->thr = *twi_tx_data++;
		}
	}
	// this is a TXCOMP
	else if (status & AVR32_TWI_SR_TXCOMP_MASK) {
		// finish the transmit operation
		goto complete;
	}

	return;

nack:
	twi_nack = true;

complete:
	// disable all interrupts
	twi_inst->idr = ~0UL;
	twi_inst->sr;
	twi_busy = false;

	return;
}


#ifndef AVR32_TWI_180_H_INCLUDED

/*! \brief TWI interrupt handler.
 */
ISR(twi_slave_interrupt_handler, AVR32_TWI_IRQ_GROUP, CONF_TWI_IRQ_LEVEL)
{
	// get masked status register value
	int status = twi_inst->sr;

	if( (twi_it_mask & AVR32_TWI_IER_EOSACC_MASK)
			&&  (status & AVR32_TWI_SR_EOSACC_MASK) ) {

		// Disable All interrupts
		twi_inst->idr = AVR32_TWI_IDR_TXRDY_MASK
				| AVR32_TWI_IDR_RXRDY_MASK
				| AVR32_TWI_IER_EOSACC_MASK;

		// Re-enable detection slave access
		twi_it_mask = AVR32_TWI_IER_SVACC_MASK;
		twi_inst->ier = twi_it_mask;

		// Signal EOF access
		twi_slave_fct.stop();

	} else if( (twi_it_mask & AVR32_TWI_IER_SVACC_MASK)
			&&  (status & AVR32_TWI_SR_SVACC_MASK ) ) {
		twi_inst->idr = AVR32_TWI_IDR_SVACC_MASK;
		// A slave is selected, then check direction
		if ( status & AVR32_TWI_SR_SVREAD_MASK ) {
			// enable flag to signal data transmission
			twi_it_mask = AVR32_TWI_IER_TXRDY_MASK;
			twi_inst->ier = twi_it_mask;
			// Transmit a data to master
			twi_inst->thr = twi_slave_fct.tx();
		} else {
			// enable flag to signal data reception
			twi_it_mask = AVR32_TWI_IER_RXRDY_MASK
					| AVR32_TWI_IER_EOSACC_MASK;
			twi_inst->ier = twi_it_mask;
		}

	// this is a RXRDY
	} else if( (twi_it_mask & AVR32_TWI_IER_RXRDY_MASK)
			&&  (status & AVR32_TWI_SR_RXRDY_MASK ) ) {
		// Get data from Receive Holding Register
		twi_slave_fct.rx( twi_inst->rhr );

	// this is a TXRDY
	} else if( (twi_it_mask & AVR32_TWI_IER_TXRDY_MASK)
			&&  (status & AVR32_TWI_SR_TXRDY_MASK ) ) {
		// Byte transmitted
		if ( status & AVR32_TWI_SR_NACK_MASK ) {
			// Last Byte
			// Clear flag NACK
			twi_inst->rhr;
			// Re-enable IT select slave
			twi_it_mask = AVR32_TWI_IER_EOSACC_MASK;
			twi_inst->ier = twi_it_mask;
		} else {
			// Transmit a data to master
			twi_inst->thr = twi_slave_fct.tx();
		}
	}
	return;
}

#endif


/*! \brief Set the twi bus speed in conjunction with the clock frequency
 *
 * \param twi    Base address of the TWI (i.e. &AVR32_TWI).
 * \param speed  The desired twi bus speed
 * \param pba_hz The current running PBA clock frequency
 * \return TWI_SUCCESS
 */
static int twi_set_speed(volatile avr32_twi_t *twi, unsigned int speed,
		unsigned long pba_hz)
{
	unsigned int ckdiv = 0;
	unsigned int c_lh_div;

	c_lh_div = pba_hz / (speed * 2) - 4;

	// cldiv must fit in 8 bits, ckdiv must fit in 3 bits
	while ((c_lh_div > 0xFF) && (ckdiv < 0x7)) {
		// increase clock divider
		ckdiv++;

		// divide cldiv value
		c_lh_div /= 2;
	}

	// set clock waveform generator register
	twi->cwgr = ((c_lh_div << AVR32_TWI_CWGR_CLDIV_OFFSET) |
			(c_lh_div << AVR32_TWI_CWGR_CHDIV_OFFSET) |
			(ckdiv << AVR32_TWI_CWGR_CKDIV_OFFSET));

	return TWI_SUCCESS;
}


int twi_master_init(volatile avr32_twi_t *twi, const twi_options_t *opt)
{
	irqflags_t flags = sysreg_read(AVR32_SR);
	int status = TWI_SUCCESS;

	// Set pointer to TWIM instance for IT
	twi_inst = twi;

	// Disable TWI interrupts
	cpu_irq_disable();
	twi->idr = ~0UL;
	twi->sr;

	// Reset TWI
	twi->cr = AVR32_TWI_CR_SWRST_MASK;
	cpu_irq_restore(flags);

	// Dummy read in SR
	twi->sr;

	// register Register twim_master_interrupt_handler interrupt
	// on level CONF_TWI_IRQ_LEVEL
	flags = cpu_irq_save();
	irq_register_handler(&twi_master_interrupt_handler, CONF_TWI_IRQ_LINE,
			CONF_TWI_IRQ_LEVEL);
	cpu_irq_restore(flags);

	// Select the speed
	twi_set_speed(twi, opt->speed, opt->pba_hz);

	// Probe the component
	//status = twi_probe(twi, opt->chip);

	return status;
}


#ifndef AVR32_TWI_180_H_INCLUDED

int twi_slave_init(volatile avr32_twi_t *twi, const twi_options_t *opt,
		const twi_slave_fct_t *slave_fct)
{
	irqflags_t flags = sysreg_read(AVR32_SR);

	// Set pointer to TWIM instance for IT
	twi_inst = twi;

	// Disable TWI interrupts
	cpu_irq_disable();
	twi->idr = ~0UL;
	twi->sr;

	// Reset TWI
	twi->cr = AVR32_TWI_CR_SWRST_MASK;
	cpu_irq_restore(flags);

	// Dummy read in SR
	twi->sr;

	// register Register twim_master_interrupt_handler interrupt
	// on level CONF_TWI_IRQ_LEVEL
	flags = cpu_irq_save();
	irq_register_handler(&twi_slave_interrupt_handler, CONF_TWI_IRQ_LINE,
			CONF_TWI_IRQ_LEVEL);
	cpu_irq_restore(flags);

	// Set slave address
	twi->smr = (opt->chip << AVR32_TWI_SMR_SADR_OFFSET);

	// Disable master transfer
	twi->cr = AVR32_TWI_CR_MSDIS_MASK;

	// Enable slave
	twi->cr = AVR32_TWI_CR_SVEN_MASK;

	// get a pointer to applicative routines
	twi_slave_fct = *slave_fct;

	// Slave Access Interrupt Enable
	twi_it_mask = AVR32_TWI_IER_SVACC_MASK;
	twi->ier = twi_it_mask;

	// Everything went ok
	return TWI_SUCCESS;
}

#endif


void twi_disable_interrupt(volatile avr32_twi_t *twi)
{
	irqflags_t flags = cpu_irq_save();
	twi->idr = ~0UL;
	twi->sr;
	cpu_irq_restore(flags);
}


int twi_probe(volatile avr32_twi_t *twi, char chip_addr)
{
	twi_package_t package;
	char data[1] = {0};

	// data to send
	package.buffer = data;
	// chip address
	package.chip = chip_addr;
	// frame length
	package.length = 1;
	// address length
	package.addr_length = 0;
	// internal chip address
	package.addr[0] = 0;
	// perform a master write access
	return (twi_master_write(twi, &package));
}

/**
 * \internal
 * \brief Construct the TWI module address register field
 *
 * The TWI module address register is sent out MSB first. And the size controls
 * which byte is the MSB to start with.
 *
 * Please see the device datasheet for details on this.
 */
static uint32_t twi_mk_addr(const uint8_t *addr, int len)
{
	uint32_t val;

	if (len == 0)
		return 0;

	val = addr[0];
	if (len > 1) {
		val <<= 8;
		val |= addr[1];
	}
	if (len > 2) {
		val <<= 8;
		val |= addr[2];
	}
	return val;
}

int twi_master_read(volatile avr32_twi_t *twi, const twi_package_t *package)
{
	// check argument
	if (package->length == 0) {
		return TWI_INVALID_ARGUMENT;
	}

	while (twi_is_busy()) {
		cpu_relax();
	};

	twi_nack = false;
	twi_busy = true;

	// set read mode, slave address and 3 internal address byte length
	twi->mmr = (package->chip << AVR32_TWI_MMR_DADR_OFFSET) |
			((package->addr_length << AVR32_TWI_MMR_IADRSZ_OFFSET) & AVR32_TWI_MMR_IADRSZ_MASK) |
			(1 << AVR32_TWI_MMR_MREAD_OFFSET);

	// Set pointer to TWIM instance for IT
	twi_inst = twi;

	// set internal address for remote chip
	twi->iadr = twi_mk_addr(package->addr, package->addr_length);

	// get a pointer to applicative data
	twi_rx_data = package->buffer;

	// get a copy of nb bytes to read
	twi_rx_nb_bytes = package->length;

	// Enable master transfer
	twi->cr =  AVR32_TWI_CR_MSEN_MASK;

	// Send start condition
	twi->cr = AVR32_TWI_START_MASK;

	// only one byte to receive
	if (twi_rx_nb_bytes == 1) {
		// set stop bit
		twi->cr = AVR32_TWI_STOP_MASK;
	}

	// mask NACK and RXRDY interrupts
	twi_it_mask = AVR32_TWI_IER_NACK_MASK | AVR32_TWI_IER_RXRDY_MASK;

	// update IMR through IER
	twi->ier = twi_it_mask;

	// get data
	while (twi_is_busy()) {
		cpu_relax();
	}

	// Disable master transfer
	twi->cr =  AVR32_TWI_CR_MSDIS_MASK;

	if (twi_nack) {
		return TWI_RECEIVE_NACK;
	}

	return TWI_SUCCESS;
}


int twi_master_write(volatile avr32_twi_t *twi, const twi_package_t *package)
{
	// No data to send
	if (package->length == 0) {
		return TWI_INVALID_ARGUMENT;
	}

	while (twi_is_busy()) {
		cpu_relax();
	};

	twi_nack = false;
	twi_busy = true;

	// Enable master transfer, disable slave
	twi->cr =   AVR32_TWI_CR_MSEN_MASK
#ifndef AVR32_TWI_180_H_INCLUDED
			| AVR32_TWI_CR_SVDIS_MASK
#endif
			;

	// set write mode, slave address and 3 internal address byte length
	twi->mmr = (0 << AVR32_TWI_MMR_MREAD_OFFSET) |
			(package->chip << AVR32_TWI_MMR_DADR_OFFSET) |
			((package->addr_length << AVR32_TWI_MMR_IADRSZ_OFFSET) & AVR32_TWI_MMR_IADRSZ_MASK);

	// Set pointer to TWI instance for IT
	twi_inst = twi;

	// set internal address for remote chip
	twi->iadr = twi_mk_addr(package->addr, package->addr_length);

	// get a pointer to applicative data
	twi_tx_data = package->buffer;

	// get a copy of nb bytes to write
	twi_tx_nb_bytes = package->length;

	// put the first byte in the Transmit Holding Register
	twi->thr = *twi_tx_data++;

	// mask NACK and TXRDY interrupts
	twi_it_mask = AVR32_TWI_IER_NACK_MASK | AVR32_TWI_IER_TXRDY_MASK;

	// update IMR through IER
	twi->ier = twi_it_mask;

	// send data
	while (twi_is_busy()) {
		cpu_relax();
	}

	// Disable master transfer
	twi->cr =  AVR32_TWI_CR_MSDIS_MASK;

	if (twi_nack) {
		return TWI_RECEIVE_NACK;
	}

	return TWI_SUCCESS;
}


//! This function is not blocking.
int twi_master_write_ex(volatile avr32_twi_t *twi, const twi_package_t *package)
{
	int status = TWI_SUCCESS;

	if (twi_nack) {
		status = TWI_RECEIVE_NACK;  // Previous transaction returns a NACK

	} else if (twi_tx_nb_bytes) {
		return TWI_BUSY;          // Still transmitting...
	}

	// No data to send
	if (package->length == 0) {
		return TWI_INVALID_ARGUMENT;
	}

	twi_nack = false;

	// Enable master transfer, disable slave
	twi->cr =   AVR32_TWI_CR_MSEN_MASK
#ifndef AVR32_TWI_180_H_INCLUDED
			| AVR32_TWI_CR_SVDIS_MASK
#endif
			;

	// set write mode, slave address and 3 internal address byte length
	twi->mmr = (0 << AVR32_TWI_MMR_MREAD_OFFSET) |
			(package->chip << AVR32_TWI_MMR_DADR_OFFSET) |
			((package->addr_length << AVR32_TWI_MMR_IADRSZ_OFFSET) & AVR32_TWI_MMR_IADRSZ_MASK);

	// Set pointer to TWIM instance for IT
	twi_inst = twi;

	// set internal address for remote chip
	twi->iadr = twi_mk_addr(package->addr, package->addr_length);

	// get a pointer to applicative data
	twi_tx_data = package->buffer;

	// get a copy of nb bytes to write
	twi_tx_nb_bytes = package->length;

	// put the first byte in the Transmit Holding Register
	twi->thr = *twi_tx_data++;

	// mask NACK and TXRDY interrupts
	twi_it_mask = AVR32_TWI_IER_NACK_MASK | AVR32_TWI_IER_TXRDY_MASK;

	// update IMR through IER
	twi->ier = twi_it_mask;

	return status;
}


bool twi_is_busy(void)
{
	if (twi_busy) {
		return true;          // Still receiving/transmitting...
	} else {
		return false;
	}
}
