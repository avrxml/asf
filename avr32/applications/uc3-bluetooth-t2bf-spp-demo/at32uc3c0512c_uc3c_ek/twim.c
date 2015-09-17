/*****************************************************************************
 *
 * \file
 *
 * \brief TWIM driver for AVR32 UC3.
 *
 * This file defines a useful set of functions for TWIM on AVR32 devices.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <avr32/io.h>
#include "compiler.h"
#include "intc.h"
#include "twim.h"

#include <includes.h>
extern CPU_INT32U  BSP_INTC_IntReg (CPU_FNCT_VOID handler, CPU_INT32U irq, CPU_INT32U int_level);


//! Pointer to the instance 1 of the TWIM registers for IT.
static volatile avr32_twim_t *twim_inst;

//! Add NACK boolean.
static volatile bool twim_nack = false;

//! Pointer to the applicative TWI transmit buffer.
static const unsigned char * volatile twim_tx_data = NULL;
//! Pointer to the applicative TWI receive buffer.
static volatile unsigned char * volatile twim_rx_data = NULL;

//! Remaining number of bytes to transmit.
static volatile int twim_tx_nb_bytes = 0;
//! Remaining number of bytes to receive.
static volatile int twim_rx_nb_bytes = 0;

//! IT mask.
static volatile unsigned long twim_it_mask;

/*! \brief TWI interrupt handler.
 */
static void twi_master_interrupt_handler(void)
{
  // get masked status register value
    int status = twim_inst->sr & twim_it_mask;
    // this is a NACK
    if (status & AVR32_TWIM_SR_ANAK_MASK)
    {
      //if we get a nak, clear the valid bit in cmdr, otherwise the command will be resent.
      twim_inst->cmdr = twim_inst->cmdr ^ AVR32_TWIM_CMDR_VALID_MASK;
      twim_inst->scr = AVR32_TWIM_SCR_ANAK_MASK;
      goto nack;
    }
    // this is a RXRDY
    else if (status & AVR32_TWIM_SR_RXRDY_MASK)
    {
      // get data from Receive Holding Register
      *twim_rx_data = twim_inst->rhr;
      twim_rx_data++;
      // last byte to receive
      twim_rx_nb_bytes--;
      // receive complete
      if (twim_rx_nb_bytes==0)
      {
        // finish the receive operation
        twim_inst->idr = AVR32_TWIM_IDR_RXRDY_MASK;
      }
    }
    // this is a TXRDY
    else if (status & AVR32_TWIM_SR_TXRDY_MASK)
    {
      // no more bytes to transmit
      if (twim_tx_nb_bytes == 0)
      {
        twim_inst->idr = AVR32_TWIM_IDR_TXRDY_MASK;
      }
      else
      {
        // put the byte in the Transmit Holding Register
        twim_inst->thr = *twim_tx_data++;
        // decrease transmitted bytes number
        twim_tx_nb_bytes--;
      }

    }
    return;

  nack:
    twim_nack = true;

  return;
}



/*! \brief Set the twi bus speed in conjunction with the clock frequency
 *
 * \param twi    Base address of the TWI (i.e. &AVR32_TWI).
 * \param speed  The desired twi bus speed
 * \param pba_hz The current running PBA clock frequency
 * \return TWI_SUCCESS
 */
int twi_set_speed(volatile avr32_twim_t *twi, unsigned int speed,
        unsigned long pba_hz) {
    unsigned int cldiv;
    unsigned int ckdiv = 0;

    cldiv = (pba_hz / speed / 2);

    // cldiv must fit in 8 bits, ckdiv must fit in 3 bits
    while ((cldiv > 0xFF) && (ckdiv <= 0x7)) {
        // increase clock divider
        ckdiv++;
        // divide cldiv value
        cldiv /= 2;
    }
    if (ckdiv > 0x7)
        return TWI_INVALID_CLOCK_DIV;
    // set clock waveform generator register
    twi->cwgr = ((cldiv/2) << AVR32_TWIM_CWGR_LOW_OFFSET)
              | ((cldiv - cldiv/2) << AVR32_TWIM_CWGR_HIGH_OFFSET)
              | (ckdiv << AVR32_TWIM_CWGR_EXP_OFFSET)
              | (0     << AVR32_TWIM_CWGR_DATA_OFFSET)
              | (cldiv << AVR32_TWIM_CWGR_STASTO_OFFSET);

    return TWI_SUCCESS;
}

int twi_master_init(volatile avr32_twim_t *twi, const twi_options_t *opt) {

  bool global_interrupt_enabled = Is_global_interrupt_enabled();
  int status = TWI_SUCCESS;

  // Disable TWI interrupts
  if (global_interrupt_enabled) Disable_global_interrupt();
  twi->idr = ~0UL;

  // Reset TWI
  twi->cr = AVR32_TWIM_CR_SWRST_MASK;
  if (global_interrupt_enabled) Enable_global_interrupt();

  // Clear SR
  twi->scr = ~0UL;

  // Disable all interrupts
  Disable_global_interrupt();

  // Register TWI handler on level 2
  BSP_INTC_IntReg( &twi_master_interrupt_handler, AVR32_TWIM0_IRQ, AVR32_INTC_INT1);

  // Enable all interrupts
  Enable_global_interrupt();

  twi->cr = AVR32_TWIM_CR_MEN_MASK;

  if (opt->smbus) {
    twi->cr = AVR32_TWIM_CR_SMEN_MASK;
    twi->smbtr = (unsigned long) -1;
  }

  // Select the speed
  if (twi_set_speed(twi, opt->speed, opt->pba_hz) == TWI_INVALID_CLOCK_DIV)
     return TWI_INVALID_CLOCK_DIV;

  // Probe the component
  //status = twi_probe(twi, opt->chip);

  return status;
}

void twim_disable_interrupt(volatile avr32_twim_t *twi)
{
  bool global_interrupt_enabled = Is_global_interrupt_enabled();

  if (global_interrupt_enabled) Disable_global_interrupt();
  twi->idr = ~0UL;
  twi->scr = ~0UL;
}

int twi_probe(volatile avr32_twim_t *twi, char chip_addr) {
    twi_package_t package;
    char data[1] = { 0 };

    // data to send
    package.buffer = data;
    // chip address
    package.chip = chip_addr;
    // frame length
    package.length = 0;
    // address length
    package.addr_length = 0;
    // internal chip address
    package.addr = 0;
    // perform a master write access
    return (twim_write_packet(twi, &package));
}

int twim_read_packet(volatile avr32_twim_t *twi, const twi_package_t *package) {

    twim_disable_interrupt(twi);

    twim_nack = false;

    // get a pointer to applicative data
    twim_rx_data = package->buffer;

    // get a copy of nb bytes to read
    twim_rx_nb_bytes = package->length;

    if (package->addr_length) {
        twim_tx_data = (unsigned char *) (&(package->addr));
        // selection of first valid byte of the address
        twim_tx_data += (4 - package->addr_length);

        twim_tx_nb_bytes = package->addr_length;

        twi->cmdr = (package->chip        << AVR32_TWIM_CMDR_SADR_OFFSET)
                  | (package->addr_length << AVR32_TWIM_CMDR_NBYTES_OFFSET)
                  | (1                    << AVR32_TWIM_CMDR_VALID_OFFSET)
                  | (1                    << AVR32_TWIM_CMDR_START_OFFSET)
                  | (0                    << AVR32_TWIM_CMDR_READ_OFFSET);

        twi->ncmdr = ((package->chip) << AVR32_TWIM_CMDR_SADR_OFFSET)
                   | (package->length << AVR32_TWIM_CMDR_NBYTES_OFFSET)
                   | (1               << AVR32_TWIM_CMDR_VALID_OFFSET)
                   | (1               << AVR32_TWIM_CMDR_START_OFFSET)
                   | (1               << AVR32_TWIM_CMDR_STOP_OFFSET)
                   | (1               << AVR32_TWIM_CMDR_READ_OFFSET);


    } else {
        twim_tx_nb_bytes = 0;
        twi->cmdr = (package->chip   << AVR32_TWIM_CMDR_SADR_OFFSET)
                  | (package->length<< AVR32_TWIM_CMDR_NBYTES_OFFSET)
                  | (1               << AVR32_TWIM_CMDR_VALID_OFFSET)
                  | (1               << AVR32_TWIM_CMDR_START_OFFSET)
                  | (1               << AVR32_TWIM_CMDR_STOP_OFFSET)
                  | (1               << AVR32_TWIM_CMDR_READ_OFFSET);
    }

    // mask NACK and RXRDY interrupts
    twim_it_mask =  AVR32_TWIM_IER_ANAK_MASK | AVR32_TWIM_IER_TXRDY_MASK | AVR32_TWIM_IER_RXRDY_MASK;

    // update IMR through IER
    twi->ier = twim_it_mask;

    // Set pointer to TWIM instance for IT
    twim_inst = twi;

     // Enable master transfer
    twi->cr =  AVR32_TWIM_CR_MEN_MASK;

    // Enable all interrupts
    Enable_global_interrupt();

    // get data
    while (!twim_nack && !(twi->sr & AVR32_TWIM_SR_IDLE_MASK));

    // Disable master transfer
    twi->cr =  AVR32_TWIM_CR_MDIS_MASK;


    if( twim_nack )
    {
      return TWI_RECEIVE_NACK;
    }

    return TWI_SUCCESS;
}

int twim_read(volatile avr32_twim_t *twi, unsigned char *buffer, int nbytes,
        int saddr, bool tenbit) {

    twim_disable_interrupt(twi);

    twim_nack = false;

    // get a pointer to applicative data
    twim_rx_data = buffer;

    //tenbit need special handling
    if (tenbit) {
        twi->cmdr = (saddr << AVR32_TWIM_CMDR_SADR_OFFSET)
                  | (0    << AVR32_TWIM_CMDR_NBYTES_OFFSET)
                  | (1 << AVR32_TWIM_CMDR_VALID_OFFSET)
                  | (1 << AVR32_TWIM_CMDR_START_OFFSET)
                  | (0 << AVR32_TWIM_CMDR_STOP_OFFSET)
                  | (1 << AVR32_TWIM_CMDR_TENBIT_OFFSET)
                  | (0 << AVR32_TWIM_CMDR_READ_OFFSET);

        twi->ncmdr = (saddr << AVR32_TWIM_CMDR_SADR_OFFSET)
                   | (nbytes << AVR32_TWIM_CMDR_NBYTES_OFFSET)
                   | (1      << AVR32_TWIM_CMDR_VALID_OFFSET)
                   | (1      << AVR32_TWIM_CMDR_START_OFFSET)
                   | (1      << AVR32_TWIM_CMDR_STOP_OFFSET)
                   | (1      << AVR32_TWIM_CMDR_TENBIT_OFFSET)
                   | (1      << AVR32_TWIM_CMDR_REPSAME_OFFSET)
                   | (1      << AVR32_TWIM_CMDR_READ_OFFSET);

    } else {
        twi->cmdr = (saddr  << AVR32_TWIM_CMDR_SADR_OFFSET)
                  | (nbytes << AVR32_TWIM_CMDR_NBYTES_OFFSET)
                  | (1      << AVR32_TWIM_CMDR_VALID_OFFSET)
                  | (1      << AVR32_TWIM_CMDR_START_OFFSET)
                  | (1      << AVR32_TWIM_CMDR_STOP_OFFSET)
                  | (0      << AVR32_TWIM_CMDR_TENBIT_OFFSET)
                  | (1      << AVR32_TWIM_CMDR_READ_OFFSET);

    }
    // get data

    // mask NACK and RXRDY interrupts
    twim_it_mask =  AVR32_TWIM_IER_ANAK_MASK | AVR32_TWIM_IER_TXRDY_MASK | AVR32_TWIM_IER_RXRDY_MASK;

    // update IMR through IER
    twi->ier = twim_it_mask;

    // Set pointer to TWIM instance for IT
    twim_inst = twi;

     // Enable master transfer
    twi->cr =  AVR32_TWIM_CR_MEN_MASK;

    // Enable all interrupts
    Enable_global_interrupt();

    // get data
    while (!twim_nack && !(twi->sr & AVR32_TWIM_SR_IDLE_MASK));

    // Disable master transfer
    twi->cr =  AVR32_TWIM_CR_MDIS_MASK;


    if( twim_nack )
    {
      return TWI_RECEIVE_NACK;
    }

  return TWI_SUCCESS;

}

int twi_master_read(volatile avr32_twim_t *twi, const twi_package_t *package) {
    unsigned int addr = Swap32(package->addr);

    // Set Register address if needed
    if (package->addr_length) {
#if AVR32_TWIM_H_VERSION > 101
        while(twim_write(twi, (unsigned char*)&addr, package->addr_length, package->chip,0)!=TWI_SUCCESS);
#else
        twim_write(twi, (unsigned char *)&addr, package->addr_length, package->chip,0);
#endif
    }
    return twim_read(twi, package->buffer, package->length,package->chip, 0);
}

int twim_write_packet(volatile avr32_twim_t *twi, const twi_package_t *package) {

    if (package->addr_length) {
        twim_tx_data = (unsigned char *) (&(package->addr));
        // selection of first valid byte of the address
        twim_tx_data += (4 - package->addr_length);

        twim_tx_nb_bytes = package->addr_length;

        twi->cmdr = (package->chip << AVR32_TWIM_CMDR_SADR_OFFSET)
                  | (package->addr_length << AVR32_TWIM_CMDR_NBYTES_OFFSET)
                  | (1 << AVR32_TWIM_CMDR_VALID_OFFSET)
                  | (1 << AVR32_TWIM_CMDR_START_OFFSET)
                  | (0 << AVR32_TWIM_CMDR_READ_OFFSET);

        // send data
        while (!(twi->sr & AVR32_TWIM_SR_IDLE_MASK) && twim_tx_nb_bytes) {
            if (twi->sr & AVR32_TWIM_SR_TXRDY_MASK)
                twi->thr = *twim_tx_data++;
        };
    }

    twi->cmdr = (package->chip << AVR32_TWIM_CMDR_SADR_OFFSET)
              | (package->length << AVR32_TWIM_CMDR_NBYTES_OFFSET)
              | (1 << AVR32_TWIM_CMDR_VALID_OFFSET)
              | (1 << AVR32_TWIM_CMDR_START_OFFSET)
              | (1 << AVR32_TWIM_CMDR_STOP_OFFSET)
              | (0 << AVR32_TWIM_CMDR_READ_OFFSET);

    // get a pointer to applicative data
    twim_tx_data = package->buffer;

    // get a copy of nb bytes to write
    twim_tx_nb_bytes = package->length;

    // send data
    while (!(twi->sr & AVR32_TWIM_SR_IDLE_MASK)) {
        if ((twim_tx_nb_bytes > 0) && (twi->sr & AVR32_TWIM_SR_TXRDY_MASK)) {
            twi->thr = *twim_tx_data++;
            twim_tx_nb_bytes--;
        }
    };

    if (twi->sr & AVR32_TWIM_SR_ANAK_MASK) {
        twi->cmdr = twi->cmdr ^ AVR32_TWIM_CMDR_VALID_MASK;
        twi->scr = AVR32_TWIM_SCR_ANAK_MASK;
        return TWI_RECEIVE_NACK;
    }

    return TWI_SUCCESS;
}

int twim_write(volatile avr32_twim_t *twi, unsigned const char *buffer,
        int nbytes, int saddr, bool tenbit) {

    twim_disable_interrupt(twi);

    twim_nack = false;

    // get a pointer to applicative data
    twim_tx_data = buffer;

    twim_tx_nb_bytes = nbytes;

    twi->cmdr = (saddr << AVR32_TWIM_CMDR_SADR_OFFSET)
              | (nbytes << AVR32_TWIM_CMDR_NBYTES_OFFSET)
              | (1 << AVR32_TWIM_CMDR_VALID_OFFSET)
              | (1 << AVR32_TWIM_CMDR_START_OFFSET)
              | (1 << AVR32_TWIM_CMDR_STOP_OFFSET)
              | ((tenbit ? 1 : 0) << AVR32_TWIM_CMDR_TENBIT_OFFSET)
              | (0 << AVR32_TWIM_CMDR_READ_OFFSET);


     // mask NACK and TXRDY interrupts
     twim_it_mask = AVR32_TWIM_IER_ANAK_MASK | AVR32_TWIM_IER_TXRDY_MASK;

     // update IMR through IER
     twi->ier = twim_it_mask;

     // Set pointer to TWIM instance for IT
     twim_inst = twi;

     // Enable master transfer
     twi->cr =  AVR32_TWIM_CR_MEN_MASK;

#if AVR32_TWIM_H_VERSION > 101
     // put the byte in the Transmit Holding Register
     twim_inst->thr = *twim_tx_data++;
     // decrease transmitted bytes number
     twim_tx_nb_bytes--;
#endif

     // Enable all interrupts
     Enable_global_interrupt();

     // wait until Nack or IDLE in SR
     while (!twim_nack && !(twi->sr & AVR32_TWIM_SR_IDLE_MASK));

     // Disable master transfer
     twi->cr =  AVR32_TWIM_CR_MDIS_MASK;

#if AVR32_TWIM_H_VERSION > 101
     if( twim_nack ) {
       return TWI_RECEIVE_NACK;
     }
#else
     if( twi->sr & AVR32_TWIM_SR_ANAK_MASK ) {
       twi->scr = AVR32_TWIM_SCR_ANAK_MASK;
       return TWI_RECEIVE_NACK;
     }
#endif

     return TWI_SUCCESS;
}

int twim_chained_transfer(volatile avr32_twim_t *twi,
        volatile twim_transfer_t *first, volatile twim_transfer_t *second,
        bool tenbit) {

    twi->scr = AVR32_TWIM_SR_CCOMP_MASK;
    if (tenbit && first->read) {
        twi->cmdr = (first->chip << AVR32_TWIM_CMDR_SADR_OFFSET)
                  | (0           << AVR32_TWIM_CMDR_NBYTES_OFFSET)
                  | (1           << AVR32_TWIM_CMDR_VALID_OFFSET)
                  | (1           << AVR32_TWIM_CMDR_START_OFFSET)
                  | (0           << AVR32_TWIM_CMDR_STOP_OFFSET)
                  | (1           << AVR32_TWIM_CMDR_TENBIT_OFFSET)
                  | (0           << AVR32_TWIM_CMDR_READ_OFFSET);

        twi->ncmdr = (first->chip   << AVR32_TWIM_CMDR_SADR_OFFSET)
                   | (first->length << AVR32_TWIM_CMDR_NBYTES_OFFSET)
                   | (1             << AVR32_TWIM_CMDR_VALID_OFFSET)
                   | (1             << AVR32_TWIM_CMDR_START_OFFSET)
                   | (0             << AVR32_TWIM_CMDR_STOP_OFFSET)
                   | (1             << AVR32_TWIM_CMDR_TENBIT_OFFSET)
                   | (1             << AVR32_TWIM_CMDR_REPSAME_OFFSET)
                   | (1             << AVR32_TWIM_CMDR_READ_OFFSET);

        while (!(twi->sr & AVR32_TWIM_SR_CCOMP_MASK)) {

        }
        twi->scr = AVR32_TWIM_SR_CCOMP_MASK;

    } else {
        twi->cmdr = (first->chip           << AVR32_TWIM_CMDR_SADR_OFFSET)
                  | (first->length         << AVR32_TWIM_CMDR_NBYTES_OFFSET)
                  | (1                     << AVR32_TWIM_CMDR_VALID_OFFSET)
                  | (1                     << AVR32_TWIM_CMDR_START_OFFSET)
                  | (0                     << AVR32_TWIM_CMDR_STOP_OFFSET)
                  | ((tenbit ? 1 : 0)      << AVR32_TWIM_CMDR_TENBIT_OFFSET)
                  | ((first->read ? 1 : 0) << AVR32_TWIM_CMDR_READ_OFFSET);
    }

    twi->ncmdr = (second->chip                       << AVR32_TWIM_CMDR_SADR_OFFSET)
               | (second->length                     << AVR32_TWIM_CMDR_NBYTES_OFFSET)
               | (1                                  << AVR32_TWIM_CMDR_VALID_OFFSET)
               | (1                                  << AVR32_TWIM_CMDR_START_OFFSET)
               | (1                                  << AVR32_TWIM_CMDR_STOP_OFFSET)
               | ((tenbit ? 1 : 0)                   << AVR32_TWIM_CMDR_TENBIT_OFFSET)
               | (((tenbit && second->read) ? 1 : 0) << AVR32_TWIM_CMDR_REPSAME_OFFSET)
               | ((second->read ? 1 : 0)             << AVR32_TWIM_CMDR_READ_OFFSET);

    if (first->read) {
        // get a pointer to applicative data
        twim_rx_data = first->buffer;

        // get data
        while (!(twi->sr & AVR32_TWIM_SR_CCOMP_MASK)) {
            if (twi->sr & AVR32_TWIM_SR_RXRDY_MASK)
                *twim_rx_data++ = twi->rhr;
        }
        if (twi->sr & AVR32_TWIM_SR_RXRDY_MASK)
            *twim_rx_data++ = twi->rhr;

    } else {
        // get a pointer to applicative data
        twim_tx_data = first->buffer;

        twim_tx_nb_bytes = first->length;
        // send data
        while (!(twi->sr & AVR32_TWIM_SR_CCOMP_MASK)) {
            if ((twim_tx_nb_bytes > 0) && (twi->sr & AVR32_TWIM_SR_TXRDY_MASK)) {
                twi->thr = *twim_tx_data++;
                twim_tx_nb_bytes--;
            }
        }
    }

    twi->scr = AVR32_TWIM_SR_CCOMP_MASK;

    if (second->read) {
        // get a pointer to applicative data
        twim_rx_data = second->buffer;

        // get data
        while (!(twi->sr & AVR32_TWIM_SR_IDLE_MASK)) {
            if (twi->sr & AVR32_TWIM_SR_RXRDY_MASK)
                *twim_rx_data++ = twi->rhr;
        }

    } else {
        // get a pointer to applicative data
        twim_tx_data = second->buffer;

        twim_tx_nb_bytes = second->length;
        // send data
        while (!(twi->sr & AVR32_TWIM_SR_IDLE_MASK)) {
            if ((twim_tx_nb_bytes > 0) && (twi->sr & AVR32_TWIM_SR_TXRDY_MASK)) {
                twi->thr = *twim_tx_data++;
                twim_tx_nb_bytes--;
            }
        }
    }

    if (twi->sr & AVR32_TWIM_SR_ARBLST_MASK) {
        twi->scr = AVR32_TWIM_SCR_ARBLST_MASK;
        return TWI_ARBITRATION_LOST;
    }

    if (twi->sr & AVR32_TWIM_SR_ANAK_MASK) {
        twi->cmdr = twi->cmdr ^ AVR32_TWIM_CMDR_VALID_MASK;
        twi->scr = AVR32_TWIM_SCR_ANAK_MASK;
        return TWI_RECEIVE_NACK;
    }

    return TWI_SUCCESS;
}
