/**
 * \file
 *
 * \brief TWIM driver for SAM.
 *
 * This file defines a useful set of functions for the TWIM on SAM4L devices.
 *
 * Copyright (c) 2012 Atmel Corporation. All rights reserved.
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
#include "sleepmgr.h"
#include "sysclk.h"

#if defined(ID_TWIM3)
#define NUM_TWIM_CH  4
#elif defined(ID_TWIM2)
#define NUM_TWIM_CH  3
#elif defined(ID_TWIM1)
#define NUM_TWIM_CH  2
#elif defined(ID_TWIM0)
#define NUM_TWIM_CH  1
#else
#error There is no TWIM module on this device.
#endif

/**
 * \internal
 * \brief TWIM callback function pointer array
 */
twim_callback_t twim_callback_pointer[NUM_TWIM_CH];

/** \internal Pointer to the applicative TWI transmit buffer. */
static const volatile uint8_t  *twim_tx_data[NUM_TWIM_CH] = {NULL};
/** \internal Pointer to the applicative TWI receive buffer. */
static volatile uint8_t *twim_rx_data[NUM_TWIM_CH] = {NULL};
/** \internal Status of the bus transfer */
static volatile twim_transfer_status_t transfer_status[NUM_TWIM_CH];
/** \internal Remaining number of bytes to transmit. */
static volatile uint32_t twim_tx_nb_bytes[NUM_TWIM_CH] = {0};
/** \internal Remaining number of bytes to receive. */
static volatile uint32_t twim_rx_nb_bytes[NUM_TWIM_CH] = {0};
/** \internal IT mask. */
static volatile uint32_t twim_it_mask[NUM_TWIM_CH];
/** \internal Pointer to twim package */
static volatile const struct twim_package *twim_package[NUM_TWIM_CH];
/** \internal If Internal Address access is used */
static volatile bool twim_next[NUM_TWIM_CH] = {false};


/**
 * \brief Get TWIM channel number.
 *
 * \param twim  Base address of the TWIM
 *
 * \return      TWIM channel number
 */
static uint32_t find_twim_channel_num(Twim *twim)
{
#if defined(ID_TWIM3)
	if (twim == TWIM3) {
		return 3;
	}
#endif
#if defined(ID_TWIM2)
	if (twim == TWIM2) {
		return 2;
	}
#endif
#if defined(ID_TWIM1)
	if (twim == TWIM1) {
		return 1;
	}
#endif
#if defined(ID_TWIM0)
	if (twim == TWIM0) {
		return 0;
	}
#endif

	return 0;
}

/**
 * \brief TWIM default callback function
 */
void twim_default_callback(Twim *twim)
{
	/* Get masked status register value */
	uint32_t twim_ch = find_twim_channel_num(twim);
	uint32_t status = twim->TWIM_SR;
	status &= twim_it_mask[twim_ch];
	/* this is a NACK */
	if (status & TWIM_SR_STD_MASK) {
		/*
		 * If we get a nak, clear the valid bit in cmdr,
		 * otherwise the command will be resent.
		 */
		transfer_status[twim_ch] = (status & TWIM_IER_ANAK) ?
				TWI_RECEIVE_NACK : TWI_ARBITRATION_LOST;
		twim->TWIM_CMDR &= ~TWIM_CMDR_VALID;
		twim->TWIM_SCR = ~0UL;
		twim->TWIM_IDR = ~0UL;
		twim_next[twim_ch] = false;
	}
	/* This is a RXRDY */
	else if (status & TWIM_SR_RXRDY) {
		/* Get data from Receive Holding Register */
		*twim_rx_data[twim_ch] = twim->TWIM_RHR;
		twim_rx_data[twim_ch]++;
		/* Decrease received bytes number */
		twim_rx_nb_bytes[twim_ch]--;
		/* Receive complete */
		if (twim_rx_nb_bytes[twim_ch] == 0) {
			/* Finish the receive operation */
			twim->TWIM_IDR = TWIM_IDR_RXRDY;
			/* Set busy to false */
			twim_next[twim_ch] = false;
#if TWIM_LOW_POWER_ENABLE
			twim->TWIM_SCR = TWIM_SCR_CCOMP;
			twim->TWIM_IER = TWIM_IER_CCOMP;
			twim_it_mask[twim_ch] |= TWIM_IER_CCOMP;
#endif
		}
	}
	/* This is a TXRDY */
	else if (status & TWIM_SR_TXRDY) {
		/* No more bytes to transmit */
		if (twim_tx_nb_bytes[twim_ch] == 0) {
			/* Finish the receive operation */
			twim->TWIM_IDR = TWIM_IDR_TXRDY;
			/* Set busy to false */
			twim_next[twim_ch] = false;
#if TWIM_LOW_POWER_ENABLE
			twim->TWIM_SCR = TWIM_SCR_CCOMP;
			twim->TWIM_IER = TWIM_IER_CCOMP;
			twim_it_mask[twim_ch] |= TWIM_IER_CCOMP;
#endif
		} else {
			/* Put the byte in the Transmit Holding Register */
			twim->TWIM_THR = *twim_tx_data[twim_ch]++;
			/* Decrease transmitted bytes number */
			twim_tx_nb_bytes[twim_ch]--;
			if (twim_tx_nb_bytes[twim_ch] == 0) {
				/* Check for next transfer */
				if (twim_next[twim_ch]) {
					twim_next[twim_ch] = false;
					twim_tx_nb_bytes[twim_ch] = twim_package[twim_ch]->length;
					twim_tx_data[twim_ch] = twim_package[twim_ch]->buffer;
				}
			}
		}
	}
#if TWIM_LOW_POWER_ENABLE
	else if (status & TWIM_SR_CCOMP) {
		twim->TWIM_SCR = TWIM_SCR_CCOMP;
		twim->TWIM_IDR = TWIM_IDR_CCOMP;
	}
#endif
}


/**
 * \brief Set the TWI bus speed in conjunction with the clock frequency
 *
 * \param twim              Base address of the TWIM
 * \param speed             The desired twim bus speed
 * \param clk               The current running system clock frequency
 * \param cycles            Clock cycles for data setup count
 *
 * \retval STATUS_OK        Transaction is successful
 * \retval ERR_INVALID_ARG  Invalid arg resulting in wrong CWGR Exponential
 */
status_code_t twim_set_speed(Twim *twim, uint32_t speed, uint32_t clk,
	uint8_t cycles)
{
	uint32_t f_prescaled;
	uint8_t cwgr_exp = 0;
	f_prescaled = (clk / speed / 2);

	/* f_prescaled must fit in 8 bits, cwgr_exp must fit in 3 bits */
	while ((f_prescaled > 0xFF) && (cwgr_exp <= 0x7)) {
		/* increase clock divider */
		cwgr_exp++;
		/* divide f_prescaled value */
		f_prescaled /= 2;
	}

	if (cwgr_exp > 0x7) {
		return ERR_INVALID_ARG;
	}

	/* set clock waveform generator register */
	twim->TWIM_CWGR = TWIM_CWGR_LOW(f_prescaled / 2)
			| TWIM_CWGR_HIGH(f_prescaled - f_prescaled / 2)
			| TWIM_CWGR_EXP(cwgr_exp)
			| TWIM_CWGR_DATA(cycles)
			| TWIM_CWGR_STASTO(f_prescaled);

	return STATUS_OK;
}

/**
 * \brief Set the TWI bus speed in conjunction with the clock frequency in high speed mode
 *
 * \param twim              Base address of the TWIM
 * \param speed             The desired twim bus speed
 * \param clk               The current running system clock frequency
 * \param cycles            Clock cycles for data setup count
 *
 * \retval STATUS_OK        Transaction is successful
 * \retval ERR_INVALID_ARG  Invalid arg resulting in wrong CWGR Exponential
 */
status_code_t twim_set_hsmode_speed(Twim *twim, uint32_t speed, uint32_t clk,
	uint8_t cycles)
{
	uint32_t f_prescaled;
	uint8_t cwgr_exp = 0;
	f_prescaled = (clk / speed / 2);

	/* f_prescaled must fit in 8 bits, cwgr_exp must fit in 3 bits */
	while ((f_prescaled > 0xFF) && (cwgr_exp <= 0x7)) {
		/* increase clock divider */
		cwgr_exp++;
		/* divide f_prescaled value */
		f_prescaled /= 2;
	}

	if (cwgr_exp > 0x7) {
		return ERR_INVALID_ARG;
	}

	/* set clock waveform generator register */
	twim->TWIM_HSCWGR = TWIM_HSCWGR_LOW(f_prescaled / 2)
			| TWIM_HSCWGR_HIGH(f_prescaled - f_prescaled / 2)
			| TWIM_HSCWGR_EXP(cwgr_exp)
			| TWIM_HSCWGR_DATA(cycles)
			| TWIM_HSCWGR_STASTO(f_prescaled);

	return STATUS_OK;
}


/**
 * \brief Initialize the TWIM module
 *
 * \param twim         Base address of the TWIM
 * \param config       Options for initializing the TWIM module
 *
 * \retval STATUS_OK        Transaction is successful
 * \retval ERR_INVALID_ARG  Invalid arg resulting in wrong CWGR Exponential
 */
status_code_t twim_set_config(Twim *twim, struct twim_config *config)
{
	sysclk_enable_peripheral_clock(twim);
	/* Enable master transfer */
	twim->TWIM_CR = TWIM_CR_MEN;
	/* Reset TWI */
	twim->TWIM_CR = TWIM_CR_SWRST;
	/* Clear SR */
	twim->TWIM_SCR = ~0UL;

	if (config->smbus) {
		/* Enable SMBUS Transfer */
		twim->TWIM_CR = TWIM_CR_SMEN;
		twim->TWIM_SMBTR = (uint32_t) -1;
	}

	/* Select the speed */
	if (config->speed) {
		if (twim_set_speed(twim, config->speed, config->twim_clk,
				config->data_setup_cycles) != STATUS_OK) {
			return ERR_INVALID_ARG;
		}
	}
	if (config->hsmode_speed) {
		if (twim_set_hsmode_speed(twim, config->hsmode_speed, config->twim_clk,
				config->hsmode_data_setup_cycles) != STATUS_OK) {
			return ERR_INVALID_ARG;
		}
	}

	/* Set clock and data slew rate */
	twim->TWIM_SRR = ((config->speed < TWI_FAST_MODE_PLUS_SPEED) ?
			TWIM_SRR_FILTER(2) : TWIM_SRR_FILTER(3))
			| TWIM_SRR_CLSLEW(config->clock_slew_limit)
			| TWIM_SRR_CLDRIVEL(config->clock_drive_strength_low)
			| TWIM_SRR_DASLEW(config->data_slew_limit)
			| TWIM_SRR_DADRIVEL(config->data_drive_strength_low);
	twim->TWIM_HSSRR = TWIM_HSSRR_FILTER(1)
			| TWIM_HSSRR_CLSLEW(config->hs_clock_slew_limit)
			| TWIM_HSSRR_CLDRIVEL(config->hs_clock_drive_strength_low)
			| TWIM_HSSRR_CLDRIVEH(config->hs_clock_drive_strength_high)
			| TWIM_HSSRR_DASLEW(config->hs_data_slew_limit)
			| TWIM_HSSRR_DADRIVEL(config->hs_data_drive_strength_low);

#if TWIM_LOW_POWER_ENABLE
	sleepmgr_init();
#endif

	return STATUS_OK;
}

/**
 * \brief Enable the TWIM interrupts
 *
 * \param twim              Base address of the TWIM
 * \param interrupt_source  The TWI interrupt to be enabled
 */
void twim_enable_interrupt(Twim *twim, uint32_t interrupt_source)
{
	/* Set the interrupt flags */
	twim->TWIM_IER = interrupt_source;
}

/**
 * \brief Disable the TWIM interrupts and clear their status
 *
 * \param twim              Base address of the TWIM.
 * \param interrupt_source  The TWI interrupt to be disabled
 */
void twim_disable_interrupt(Twim *twim, uint32_t interrupt_source)
{
	/* Clear the interrupt flags */
	twim->TWIM_IDR = interrupt_source;
	/* Clear the status flags */
	twim->TWIM_SCR = interrupt_source;
}

/**
 * \brief Get the TWIM interrupt mask
 *
 * \param twim   Base address of the TWIM
 *
 * \retval TWIM interrupt mask
 */
uint32_t twim_get_interrupt_mask(Twim *twim)
{
	return twim->TWIM_IMR;
}

/**
 * \brief Test if a chip answers for a given TWI address
 *
 * \param twim            Base address of the TWIM
 * \param chip_addr       Address of the chip which is searched for
 *
 * \retval STATUS_OK      Slave Found
 * \retval ERR_IO_ERROR   ANAK received or Bus Arbitration lost
 */
status_code_t twim_probe(Twim *twim, uint32_t chip_addr)
{
	uint8_t data[1] = { 0 };
	struct twim_package packet;
	packet.chip = chip_addr;
	packet.addr_length = 0;
	packet.buffer = (void *) data;
	packet.length = 1;

	return (twi_master_read(twim, &packet));
}

/**
 * \brief Read multiple bytes from a TWI compatible slave device
 *
 * \param twim            Base address of the TWIM
 * \param package         Package information and data (see \ref twim_package_t)
 *
 * \retval STATUS_OK      If all bytes were read successfully
 * \retval ERR_IO_ERROR   NACK received or Bus Arbitration lost
 */
status_code_t twi_master_read(Twim *twim, struct twim_package *package)
{
	uint32_t twim_ch = find_twim_channel_num(twim);

	/* Disable master transfer */
	twim->TWIM_CR = TWIM_CR_MDIS;

	/* Clear the interrupt flags */
	twim->TWIM_IDR = ~0UL;
	/* Clear the status flags */
	twim->TWIM_SCR = ~0UL;
	/* Get a pointer to applicative data */
	twim_rx_data[twim_ch] = package->buffer;
	/* Get a copy of nb bytes to read */
	twim_rx_nb_bytes[twim_ch] = package->length;
	/* Set next write transfer to false */
	twim_next[twim_ch] = false;
	/* Initialize bus transfer status */
	transfer_status[twim_ch] = TWI_SUCCESS;

	/* Reset the TWIM module to clear the THR register */
	twim->TWIM_CR = TWIM_CR_MEN;
	twim->TWIM_CR = TWIM_CR_SWRST;
	twim->TWIM_CR = TWIM_CR_MDIS;

	if (package->ten_bit) {
		twim->TWIM_CMDR = (package->high_speed ? (TWIM_CMDR_HS |
					TWIM_CMDR_HSMCODE(package->high_speed_code)) : 0)
					| TWIM_CMDR_SADR(package->chip)
					| TWIM_CMDR_VALID
					| TWIM_CMDR_START
					| TWIM_CMDR_TENBIT;

		if (package->addr_length) {
			/* Selection of first valid byte of the address */
			twim_tx_data[twim_ch] = package->addr;
			/* Set the number of bytes to transmit */
			twim_tx_nb_bytes[twim_ch] = package->addr_length;
			/* Mask NACK, TXRDY and RXRDY interrupts */
			twim_it_mask[twim_ch] = TWIM_IER_STD_MASK | TWIM_IER_TXRDY
					| TWIM_IER_RXRDY;
			twim->TWIM_NCMDR = TWIM_CMDR_SADR(package->chip)
					| TWIM_CMDR_NBYTES(package->addr_length)
					| TWIM_NCMDR_VALID
					| TWIM_NCMDR_START
					| TWIM_NCMDR_STOP
					| TWIM_NCMDR_TENBIT
					| TWIM_NCMDR_REPSAME;
			/* Update IMR through IER */
			twim->TWIM_IER = twim_it_mask[twim_ch];
			/* Enable master transfer */
			twim->TWIM_CR = TWIM_CR_MEN;
			/* Get data */
#if TWIM_LOW_POWER_ENABLE
			sleepmgr_lock_mode(SLEEPMGR_SLEEP_1);
			while (!(transfer_status[twim_ch])
					&& !(twim_get_status(twim) & TWIM_SR_IDLE)) {
				sleepmgr_enter_sleep();
			}
			sleepmgr_unlock_mode(SLEEPMGR_SLEEP_1);
#else
			while (!(transfer_status[twim_ch])
					&& !(twim_get_status(twim) & TWIM_SR_IDLE)) {
			}
#endif
			/* Disable master transfer */
			twim->TWIM_CR = TWIM_CR_MDIS;
			/* Check for nack */
			if (transfer_status[twim_ch] == TWI_RECEIVE_NACK
					|| transfer_status[twim_ch] == TWI_ARBITRATION_LOST) {
				return ERR_IO_ERROR;
			}

			twim_tx_nb_bytes[twim_ch] = 0;
			/* Mask NACK and RXRDY interrupts */
			twim_it_mask[twim_ch] = TWIM_IER_STD_MASK | TWIM_IER_RXRDY;
			/* Set the command register to initiate the transfer */
			twim->TWIM_CMDR = (package->high_speed ? (TWIM_CMDR_HS |
					TWIM_CMDR_HSMCODE(package->high_speed_code)) : 0)
					| TWIM_CMDR_SADR(package->chip)
					| TWIM_CMDR_NBYTES(package->length)
					| TWIM_CMDR_VALID
					| TWIM_CMDR_START
					| TWIM_CMDR_STOP
					| TWIM_CMDR_TENBIT
					| TWIM_CMDR_REPSAME
					| TWIM_CMDR_READ;
		} else {
			twim_tx_nb_bytes[twim_ch] = 0;
			/* Mask NACK and RXRDY interrupts */
			twim_it_mask[twim_ch] = TWIM_IER_STD_MASK | TWIM_IER_RXRDY;
			twim->TWIM_NCMDR = TWIM_NCMDR_SADR(package->chip)
					| TWIM_NCMDR_NBYTES(package->length)
					| TWIM_NCMDR_VALID
					| TWIM_NCMDR_START
					| TWIM_NCMDR_STOP
					| TWIM_NCMDR_TENBIT
					| TWIM_NCMDR_REPSAME
					| TWIM_NCMDR_READ;
		}

	} else {
		/* Check if internal address access is performed */
		if (package->addr_length) {
			/* Selection of first valid byte of the address */
			twim_tx_data[twim_ch] = package->addr;
			/* Set the number of bytes to transmit */
			twim_tx_nb_bytes[twim_ch] = package->addr_length;
			/* Mask NACK, TXRDY and RXRDY interrupts */
			twim_it_mask[twim_ch] = TWIM_IER_STD_MASK | TWIM_IER_TXRDY
					| TWIM_IER_RXRDY;
			/* Set the command register to initiate the transfer */
			twim->TWIM_CMDR = (package->high_speed ? (TWIM_CMDR_HS |
					TWIM_CMDR_HSMCODE(package->high_speed_code)) : 0)
					| TWIM_CMDR_SADR(package->chip)
					| TWIM_CMDR_NBYTES(package->addr_length)
					| TWIM_CMDR_VALID
					| TWIM_CMDR_START
					| 0;
			/* Set the command register to initiate the transfer */
			twim->TWIM_NCMDR = (package->high_speed ? (TWIM_NCMDR_HS |
					TWIM_NCMDR_HSMCODE(package->high_speed_code)) : 0)
					| TWIM_NCMDR_SADR(package->chip)
					| TWIM_NCMDR_NBYTES(package->length)
					| TWIM_NCMDR_VALID
					| TWIM_NCMDR_START
					| TWIM_NCMDR_STOP
					| TWIM_NCMDR_READ;
		} else {
			twim_tx_nb_bytes[twim_ch] = 0;
			/* Mask NACK and RXRDY interrupts */
			twim_it_mask[twim_ch] = TWIM_IER_STD_MASK | TWIM_IER_RXRDY;
			/* Set the command register to initiate the transfer */
			twim->TWIM_CMDR = (package->high_speed ? (TWIM_CMDR_HS |
					TWIM_CMDR_HSMCODE(package->high_speed_code)) : 0)
					| TWIM_CMDR_SADR(package->chip)
					| TWIM_CMDR_NBYTES(package->length)
					| TWIM_CMDR_VALID
					| TWIM_CMDR_START
					| TWIM_CMDR_STOP
					| TWIM_CMDR_READ;
		}
	}

	/* Update IMR through IER */
	twim->TWIM_IER = twim_it_mask[twim_ch];
	/* Enable master transfer */
	twim->TWIM_CR = TWIM_CR_MEN;
	/* Get data */
#if TWIM_LOW_POWER_ENABLE
	sleepmgr_lock_mode(SLEEPMGR_SLEEP_1);
	while (!(transfer_status[twim_ch])
			&& !(twim_get_status(twim) & TWIM_SR_IDLE)) {
		sleepmgr_enter_sleep();
	}
	sleepmgr_unlock_mode(SLEEPMGR_SLEEP_1);
#else
	while (!(transfer_status[twim_ch])
			&& !(twim_get_status(twim) & TWIM_SR_IDLE)) {
	}
#endif
	/* Disable master transfer */
	twim->TWIM_CR = TWIM_CR_MDIS;
	if (transfer_status[twim_ch] == TWI_RECEIVE_NACK
			|| transfer_status[twim_ch] == TWI_ARBITRATION_LOST) {
		return ERR_IO_ERROR;
	}
	return STATUS_OK;
}

/**
 * \brief Write multiple bytes to a TWI compatible slave device
 *
 * \param twim            Base address of the TWIM
 * \param *package        Package information and data (see \ref twim_package_t)
 *
 * \retval STATUS_OK      If all bytes were send successfully
 * \retval ERR_IO_ERROR   NACK received or Bus Arbitration lost
 */
status_code_t twi_master_write(Twim *twim, struct twim_package *package)
{
	uint32_t twim_ch = find_twim_channel_num(twim);

	/* Reset the TWIM module to clear the THR register */
	twim->TWIM_CR = TWIM_CR_MEN;
	twim->TWIM_CR = TWIM_CR_SWRST;
	twim->TWIM_CR = TWIM_CR_MDIS;
	/* Clear the interrupt flags */
	twim->TWIM_IDR = ~0UL;
	/* Clear the status flags */
	twim->TWIM_SCR = ~0UL;
	/* Initialize bus transfer status */
	transfer_status[twim_ch] = TWI_SUCCESS;
	/* Mask NACK and TXRDY interrupts */
	twim_it_mask[twim_ch] = TWIM_IER_STD_MASK | TWIM_IER_TXRDY;
	/* Set next transfer to false */
	twim_next[twim_ch] = false;

	/* Check if internal address access is performed */
	if (package->addr_length) {
		/* Selection of first valid byte of the address */
		twim_tx_data[twim_ch] = package->addr;
		/* Set the number of bytes to transmit */
		twim_tx_nb_bytes[twim_ch] = package->addr_length;
		/* Set next transfer to true */
		twim_next[twim_ch] = true;
		/* Set the number of bytes & address for next transfer */
		twim_package[twim_ch] = package;
	} else {
		/* Get a pointer to applicative data */
		twim_tx_data[twim_ch] = package->buffer;
		/* Get a copy of nb bytes to write */
		twim_tx_nb_bytes[twim_ch] = package->length;
	}

	/* Initiate the transfer to send the data */
	twim->TWIM_CMDR = (package->high_speed ? (TWIM_CMDR_HS |
			TWIM_CMDR_HSMCODE(package->high_speed_code)) : 0)
			| TWIM_CMDR_SADR(package->chip)
			| TWIM_CMDR_NBYTES(package->length + package->addr_length)
			| TWIM_CMDR_VALID
			| TWIM_CMDR_START
			| TWIM_CMDR_STOP
			| (package->ten_bit ? TWIM_CMDR_TENBIT : 0);

	/* Update IMR through IER */
	twim->TWIM_IER = twim_it_mask[twim_ch];
	/* Enable master transfer */
	twim->TWIM_CR = TWIM_CR_MEN;
	/* Send data */
#if TWIM_LOW_POWER_ENABLE
	sleepmgr_lock_mode(SLEEPMGR_SLEEP_1);
	while (!(transfer_status[twim_ch])
			&& !(twim_get_status(twim) & TWIM_SR_IDLE)) {
		sleepmgr_enter_sleep();
	}
	sleepmgr_unlock_mode(SLEEPMGR_SLEEP_1);
#else
	while (!(transfer_status[twim_ch])
			&& !(twim_get_status(twim) & TWIM_SR_IDLE)) {
	}
#endif

	/* Disable master transfer */
	twim->TWIM_CR = TWIM_CR_MDIS;
	/* Check for nack */
	if (transfer_status[twim_ch] == TWI_RECEIVE_NACK
			|| transfer_status[twim_ch] == TWI_ARBITRATION_LOST) {
		return ERR_IO_ERROR;
	}
	return STATUS_OK;
}

/**
 * \brief Information about the current status of the TWIM
 *
 * \param twim   Base address of the TWIM
 */
uint32_t twim_get_status(Twim *twim)
{
	return twim->TWIM_SR;
}

/**
 * \brief Clear the current status of the TWIM
 *
 * \param twim   Base address of the TWIM
 * \param clear_status  The TWIM status to be clear
 */
void twim_clear_status(Twim *twim, uint32_t clear_status)
{
	twim->TWIM_SCR = clear_status;
}

/**
 * \brief Set callback for TWIM
 *
 * \param twim       Base address of the TWIM
 * \param source     TWIM interrupt source
 * \param callback   Callback function pointer
 * \param irq_line   Interrupt line
 * \param irq_level  Interrupt level
 */
void twim_set_callback(Twim *twim, uint32_t interrupt_source,
	twim_callback_t callback, uint8_t irq_level)
{
	uint32_t twim_ch = find_twim_channel_num(twim);
	uint32_t irq_line = 0;

#if defined(ID_TWIM3)
	if (twim == TWIM3) {
		irq_line = TWIM3_IRQn;
	}
#endif
#if defined(ID_TWIM2)
	if (twim == TWIM2) {
		irq_line = TWIM2_IRQn;
	}
#endif
#if defined(ID_TWIM1)
	if (twim == TWIM1) {
		irq_line = TWIM1_IRQn;
	}
#endif
#if defined(ID_TWIM0)
	if (twim == TWIM0) {
		irq_line = TWIM0_IRQn;
	}
#endif
	twim_callback_pointer[twim_ch] = callback;
	NVIC_ClearPendingIRQ((IRQn_Type)irq_line);
	NVIC_SetPriority((IRQn_Type)irq_line, irq_level);
	NVIC_EnableIRQ((IRQn_Type)irq_line);
	twim_enable_interrupt(twim, interrupt_source);
}

#if defined(ID_TWIM0)
void TWIM0_Handler(void)
{
	twim_callback_pointer[0](TWIM0);
}
#endif

#if defined(ID_TWIM1)
void TWIM1_Handler(void)
{
	twim_callback_pointer[1](TWIM1);
}
#endif

#if defined(ID_TWIM2)
void TWIM2_Handler(void)
{
	twim_callback_pointer[2](TWIM2);
}
#endif

#if defined(ID_TWIM3)
void TWIM3_Handler(void)
{
	twim_callback_pointer[3](TWIM3);
}
#endif
