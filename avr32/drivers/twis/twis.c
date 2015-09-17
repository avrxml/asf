/**
 * \file
 *
 * \brief TWIS driver for AVR32 UC3.
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <avr32/io.h>
#include "compiler.h"
#include "twis.h"
#include "status_codes.h"

//! Define all error conditions
#define AVR32_TWIS_SR_ERROR_MASK (AVR32_TWIS_SR_BUSERR_MASK     \
		| AVR32_TWIS_SR_SMBPECERR_MASK                  \
		| AVR32_TWIS_SR_SMBTOUT_MASK                    \
		| AVR32_TWIS_SR_ORUN_MASK                       \
		| AVR32_TWIS_SR_URUN_MASK)

//! Pointer to the TWIS instance physical address
static volatile avr32_twis_t *twis_inst_slave;

//! Pointer on TWI slave application routines
static twis_slave_fct_t twis_slave_fct;

/**
 * \internal
 * \brief TWIS interrupt handler
 */
ISR(twis_slave_interrupt_handler,CONF_TWIS_IRQ_GROUP,CONF_TWIS_IRQ_LEVEL)
{
	// Get status and interrupt mask register values
	unsigned long status  = twis_inst_slave->sr;
	unsigned long enabled = twis_inst_slave->imr;
	unsigned long pending = status & enabled;

	// An error has occurred, set only address match active and return
	if (status & AVR32_TWIS_SR_ERROR_MASK) {
		twis_inst_slave->idr = ~0UL;
		twis_inst_slave->scr = ~0UL;
		twis_inst_slave->ier = AVR32_TWIS_IER_SAM_MASK;

		twis_slave_fct.stop();

		return;
	}

	// Check if the slave address match flag is raised
	if (pending & AVR32_TWIS_IER_SAM_MASK) {
		// Ignore repeated start and transmission complete flags
		if (pending & (AVR32_TWIS_SR_REP_MASK | AVR32_TWIS_SR_TCOMP_MASK)) {
			twis_inst_slave->scr = (AVR32_TWIS_SCR_REP_MASK |
					AVR32_TWIS_SCR_TCOMP_MASK);
			twis_inst_slave->idr = (AVR32_TWIS_IDR_REP_MASK |
					AVR32_TWIS_IDR_TCOMP_MASK);
		}
		pending &= ~(AVR32_TWIS_SR_REP_MASK | AVR32_TWIS_SR_TCOMP_MASK);

		// Enable error handling
		twis_inst_slave->ier = AVR32_TWIS_SR_ERROR_MASK;

		// Check if the slave should be in receive or transmit mode
		if (status & AVR32_TWIS_SR_TRA_MASK) {
			// Transmit mode
			twis_inst_slave->scr = AVR32_TWIS_SR_BTF_MASK;
			twis_inst_slave->ier = AVR32_TWIS_IER_BTF_MASK
				| AVR32_TWIS_IER_TCOMP_MASK;
			twis_inst_slave->thr = twis_slave_fct.tx();
		} else {
			// Receive mode
			twis_inst_slave->ier = AVR32_TWIS_IER_RXRDY_MASK
				| AVR32_TWIS_IER_TCOMP_MASK
				| AVR32_TWIS_IER_REP_MASK;
		}
	}

	// Check if there is data ready to be read in the data receive register
	if (pending & AVR32_TWIS_IER_RXRDY_MASK) {
		// Call user specific receive function
		twis_slave_fct.rx(twis_inst_slave->rhr);
	}

	// Check if the transmit ready flag is raised
	if (pending & AVR32_TWIS_SR_BTF_MASK) {
		if (status & AVR32_TWIS_SR_NAK_MASK) {
			twis_inst_slave->idr = AVR32_TWIS_IDR_BTF_MASK;
			twis_inst_slave->scr = AVR32_TWIS_SCR_BTF_MASK;
			twis_inst_slave->ier = AVR32_TWIS_IER_TCOMP_MASK
				| AVR32_TWIS_IER_REP_MASK;
			// Clear the NAK
			twis_inst_slave->scr = AVR32_TWIS_SCR_NAK_MASK;
		} else {
			twis_inst_slave->thr = twis_slave_fct.tx();
		}
	}

	// Check if the transmission complete or repeated start flags raised
	if (pending & (AVR32_TWIS_SR_TCOMP_MASK | AVR32_TWIS_SR_REP_MASK)) {
		// Clear transmit complete and repeated start flags
		twis_inst_slave->scr = AVR32_TWIS_SCR_TCOMP_MASK
			| AVR32_TWIS_SCR_REP_MASK;
		// Disable transmission ready interrupt
		twis_inst_slave->idr = AVR32_TWIS_IDR_BTF_MASK
			| AVR32_TWIS_IDR_RXRDY_MASK
			| AVR32_TWIS_IDR_TCOMP_MASK
			| AVR32_TWIS_IDR_REP_MASK;
		// Enable slave address match interrupt
		twis_inst_slave->ier = AVR32_TWIS_IER_SAM_MASK;
		// Call user specific stop function
		twis_slave_fct.stop();
	}
	/* Never clear the NAK mask without first receiving the appropriate
	 * BTF bit too.
	 *
	 * L4U  Errata 38.1.3.2:
	 * Clearing the NAK bit before the BTF bit is set locks up the TWI bus
	 * When the TWIS is in transmit mode, clearing the NAK Received (NAK)
	 * bit of the Status Register (SR) before the end of the
	 * Acknowledge/Not Acknowledge cycle will cause the TWIS to
	 * attempt to continue transmitting data, thus locking up the bus.
	 *
	 * Fix/Workaround
	 * Clear SR.NAK only after the Byte Transfer Finished (BTF) bit of the same register has been
	 * set.
	 */
	twis_inst_slave->scr = pending & (~AVR32_TWIS_SCR_NAK_MASK);
	return;
}

/**
 * \brief Initialize the TWI Slave Module
 *
 * \param twis   Base address of the TWI (i.e. &AVR32_TWIS).
 * \param *opt  Options for initializing the TWI module
 *              (see \ref twis_options_t)
 * \param *slave_fct  Pointer on application functions
 */
status_code_t twis_slave_init(volatile avr32_twis_t *twis, const twis_options_t *opt,
		const twis_slave_fct_t *slave_fct)
{
	irqflags_t irq_save;

	irq_save = cpu_irq_save();

	twis->idr = ~0UL;
	twis->scr = ~0UL;

	// Reset the TWIS module
	twis->cr = AVR32_TWIS_CR_SWRST_MASK;

	cpu_irq_restore(irq_save);

	// Clear the TWIS status register
	twis->scr = ~0UL;;

	// Disable all interrupts
	cpu_irq_disable();

	// register Register twis_slave_interrupt_handler interrupt on level CONF_TWIS_IRQ_LEVEL
	irqflags_t flags = cpu_irq_save();
	irq_register_handler(twis_slave_interrupt_handler,
			CONF_TWIS_IRQ_LINE, CONF_TWIS_IRQ_LEVEL);
	cpu_irq_restore(flags);

	twis_inst_slave = twis;

	// Enable the TWI Slave Module and allow for clock stretching
	twis->cr = AVR32_TWIS_CR_SEN_MASK | AVR32_TWIS_CR_SMATCH_MASK
					| AVR32_TWIS_CR_STREN_MASK;

	/*
	 * Bug on TWIS up to version 120 included which incorrectly handles
	 * clock stretch on address match.
	 */
#if AVR32_TWIS_H_VERSION > 120
	twis->cr |= AVR32_TWIS_CR_SOAM_MASK;
#endif

	if (opt->smbus) {
		twis->cr |= AVR32_TWIS_CR_SMEN_MASK;
	}

	if (opt->tenbit) {
		twis->cr |= AVR32_TWIS_CR_TENBIT_MASK;
	}

	// Set slave address
	twis->cr |= (opt->chip << AVR32_TWIS_CR_ADR_OFFSET);

	// Calculate the wait time from clk falling edge to
	//let the slave control the bus
	uint8_t setup_time = (opt->pba_hz / opt->speed)/7;
	twis->tr = (setup_time << AVR32_TWIS_TR_SUDAT_OFFSET);

	// Get a pointer to user specific functions
	twis_slave_fct = *slave_fct;

	// Slave Address Match Interrupt Enable
	twis->ier = AVR32_TWIS_IER_SAM_MASK;

	return STATUS_OK;
}


/**
 * \brief Enable NACK transfer in Slave Receive Mode
 *
 * \param twis           Base address of the TWI (i.e. &AVR32_TWIS).
 * \param stop_callback  Set true to use Stop condition Callback function
 */
void twis_send_data_nack(volatile avr32_twis_t *twis, bool stop_callback) {
	// Send NACK
	twis->cr |= AVR32_TWIS_CR_ACK_MASK;
	// Disable all interrupts
	twis->idr = ~0UL;
	// Clear all status
	twis->scr = ~0UL;
	// Enable Slave Address Match Interrupt
	twis->ier = AVR32_TWIS_IER_SAM_MASK;
	// If callback is desired, go to the stop condition callback function
	if (stop_callback) {
		twis_slave_fct.stop();
	}
}


/**
 * \brief Enable ACK transfer in Slave Receiver Mode
 *
 * \param twis   Base address of the TWI (i.e. &AVR32_TWIS).
 */
void twis_send_data_ack(volatile avr32_twis_t *twis) {
	// Send ACK on data transfer
	twis->cr &= (~AVR32_TWIS_CR_ACK_MASK);
}
