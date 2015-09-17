/**
 * \file
 *
 * \brief megaRF TWI master and slave driver.
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

#include "twi_megarf.h"

/** TWI Mode of operation **/
#define MASTER    0
#define SLAVE     1

/** Master Transfer Descriptor */
static struct {
	twi_package_t *pkg;             /* Bus message descriptor */
	int addr_count;                 /* Bus transfer address data counter */
	unsigned int data_count;        /* Bus transfer payload data counter */
	uint8_t state;                  /* Transfer state */
	status_code_t status;           /* Transfer status */
}master_transfer;

/** Slave Transfer Descriptor */
static struct {
	slave_data_buffer_t *data_buffer; /*  data buffer */
	unsigned int data_count;          /* Bus transfer payload data counter */
	uint8_t state;                    /* Transfer state */
	int status;                       /* Transfer status */
}slave_transfer;

static volatile bool twi_master_busy = false;
static volatile bool twi_mode = MASTER;

/**
 * \ingroup group_megarf_drivers_twi
 * \defgroup group_megarf_drivers_twim twi master Driver
 *
 * This driver is the implementation of TWI module of megaRF device as master.
 * Master initiates and terminates a data transmission. The transmission is
 * initiated when the Master issues a START condition on the bus, and it is 
 * terminated when the Master issues a STOP condition. This uses two
 * bi-directional bus lines(PD0 and PD1) one for clock (SCL) and one for 
 * data (SDA)
 *
 * \section megarf_twim_configuration Configuration
 * Configuration is done in the configuration file : conf_twi.h
 *
 * Configuration defines:
 * - \ref SLAVE_BUS_ADDR   : TWI slave bus address
 *
 * - \ref SLAVE_MEM_ADDR_LENGTH : TWI slave memory address length
 *
 * - \ref TWI_SPEED_HZ   : TWI data transfer rate
 *
 * \section megarf_twim_interface Interface
 * The twi module should be configured as master. The SCL period is
 * controlled by settings in the TWI Bit Rate Register (TWBR) and the
 * Prescaler bits in the TWI Status Register (TWSR). Enabling of TWI
 * clock source and interrupt is done by calling the function
 * \code twi_master_setup() \endcode
 *
 * \subsection megarf_twim_usage Usage
 * The include file is needed:
 * \code #include "twi_megarf.h" \endcode
 *
 * TWI transmission is to transmit a START condition. This is done by
 * writing a specific value into TWCR, instructing the TWI hardware to transmit
 * START condition and then transmit the data. Similarly in receive mode
 * This is done by calling:
 * \code twi_master_write() and twi_master_read() \endcode
 *
 * After initiating the transmission or reception the completion status will be
 * indicated to the application code through the following function
 * \code twi_master_get_status() and twi_master_read() \endcode
 * @{
 */

/**
 * \brief Reset TWI bus
 *
 * \param none.
 */
static void twi_master_bus_reset(void)
{
	master_transfer.state = TWI_IDLE;
	twi_master_busy      = false;
	twi_reset();
}

/**
 * \brief Notification that last byte was read from the TWI.
 *  Needs to send STOP condition
 *
 * \param data - contains byte that was read.
 */
static void twi_master_read_last_byte(uint8_t data)
{
	if (TWI_READ_DATA == master_transfer.state) {
		master_transfer.pkg->buffer[master_transfer.data_count++] = data;
		twi_send_stop();
		master_transfer.state = TWI_IDLE;
		master_transfer.status = STATUS_OK;
		twi_master_busy  = false;
	} else { /* abnormal */
		twi_master_bus_reset();
		master_transfer.status = ERR_PROTOCOL;
	}
}

/**
 * \brief Notification that byte was read by the TWI.
 *
 * \param data - contains byte that was read.
 */
static void twi_master_read_done(uint8_t data)
{
	if (TWI_READ_DATA == master_transfer.state) {
		master_transfer.pkg->buffer[master_transfer.data_count++] = data;
		if (master_transfer.data_count < (master_transfer.pkg->length - 1)) {
			twi_send_ack(true); /* send ACK */
		} else {
			twi_send_ack(false); /* send NACK */
		}
	} else { /* abnormal */
		twi_master_bus_reset();
		master_transfer.status = ERR_PROTOCOL;
	}
}

/**
 * \brief Notification that address byte was written to the TWI and need to send
 * ACK or NACK.
 *
 * \param none.
 */
static void twi_master_addr_ack(void)
{
	if (TWI_READ_DATA == master_transfer.state) {
		if (master_transfer.data_count == (master_transfer.pkg->length - 1)) {
			twi_send_ack(false); /* send nack */
		} else {
			twi_send_ack(true); /* send ack */
		}
	} else { /* abnormal */
		twi_master_bus_reset();
		master_transfer.status = ERR_PROTOCOL;
	}
}

/**
 * \brief Sending internal device address to twi bus.
 *
 * \param none.
 */
static void twi_master_internal_addr_write(void)
{
	uint8_t data;

	data = master_transfer.pkg->addr[master_transfer.addr_count];
	master_transfer.addr_count++;
	twi_write_byte(data);

	if (master_transfer.pkg->addr_length == master_transfer.addr_count) {
		if (TWI_WRITE_IADDR_WRITE_DATA == master_transfer.state) {
			master_transfer.state = TWI_WRITE_DATA;
		} else {
			master_transfer.state = TWI_READ_DATA;
		}
	}
}

/**
 * \brief Sending data to twi bus. If last byte then send stop condition.
 *
 * \param none.
 */
static void twi_master_data_write(void)
{
	if (master_transfer.data_count < master_transfer.pkg->length) {
		twi_write_byte(master_transfer.pkg->buffer[master_transfer.data_count++]);
	} else {
		twi_send_stop();
		master_transfer.state = TWI_IDLE;
		master_transfer.status = STATUS_OK;
		twi_master_busy = false;
	}
}

/**
 * \brief Notification that byte was written to the TWI.
 *
 * \param result - contains result of previous operation.
 */

static void twi_master_write_done(void)
{
	if (TWI_WRITE_DATA == master_transfer.state) {
		twi_master_data_write();
	} else if ((TWI_WRITE_IADDR_WRITE_DATA == master_transfer.state) ||
			(TWI_WRITE_IADDR_READ_DATA == master_transfer.state)) {
		twi_master_internal_addr_write();
	} else if (TWI_READ_DATA == master_transfer.state) {
		twi_send_start();
	} else { /* abnormal */
		twi_master_bus_reset();
		master_transfer.status = ERR_PROTOCOL;
	}
}

/**
 * \brief Notification about the start condition was sent.
 *
 * This function is a TWI Master start indication.
 *
 * \param none
 */
static void twi_master_start(void)
{
	uint8_t chip_add;

	if ((TWI_WRITE_IADDR_WRITE_DATA == master_transfer.state) || (TWI_WRITE_DATA ==
			master_transfer.state) || (TWI_WRITE_IADDR_READ_DATA ==
			master_transfer.state)) {
		chip_add = TWI_WRITE_ENABLE(master_transfer.pkg->chip);
		twi_write_byte(chip_add);
	} else if (TWI_READ_DATA == master_transfer.state) {
		chip_add = TWI_READ_ENABLE(master_transfer.pkg->chip);
		twi_write_byte(chip_add);
	} else { /* abnormal */
		twi_master_bus_reset();
		master_transfer.status = ERR_PROTOCOL;
	}
}

/**
 * \brief Perform a TWI master write transfer.
 *
 * This function is a TWI Master write transaction.
 *
 * \param package -  Package information and data
 *                  (see \ref twi_package_t)
 */
status_code_t twi_master_write(volatile void *twi,const twi_package_t *package)
{
	/* Do a sanity check on the arguments. */
	if (package == NULL) {
		return ERR_INVALID_ARG;
	}
	
	if (twi_master_busy == true) {
		return OPERATION_IN_PROGRESS;
	}

	/* Initiate a transaction when the bus is ready. */
	master_transfer.pkg         = (twi_package_t *)package;
	master_transfer.addr_count  = 0;
	master_transfer.data_count  = 0;
	twi_master_busy      = true;

	if (TWI_SLAVE_NO_INTERNAL_ADDRESS == master_transfer.pkg->addr_length) {
		master_transfer.state  = TWI_WRITE_DATA;
	} else {
		master_transfer.state  = TWI_WRITE_IADDR_WRITE_DATA;
	}

	twi_send_start();

	/* Wait for the transaction to complete */
	while(twi_master_busy){
		
	}
	
	return twi_master_get_status();
}

/**
 * \brief Reads the series of bytes from the TWI bus
 * \param package -  Package information and data
 *                  (see \ref twi_package_t)
 */
status_code_t twi_master_read(volatile void *twi,const twi_package_t *package)
{
	/* Do a sanity check on the arguments. */
	if ((package == NULL) || master_transfer.pkg->length == 0) {
		return ERR_INVALID_ARG;
	}

	if (true == twi_master_busy) {
		return OPERATION_IN_PROGRESS;
	}

	/* Initiate a transaction when the bus is ready. */
	/* twi = NULL;     megaRF has only one twi     */
	master_transfer.pkg         = (twi_package_t *)package;
	master_transfer.addr_count  = 0;
	master_transfer.data_count  = 0;
	twi_master_busy      = true;

	if (TWI_SLAVE_NO_INTERNAL_ADDRESS == master_transfer.pkg->addr_length) {
		master_transfer.state = TWI_READ_DATA;
	} else {
		master_transfer.state = TWI_WRITE_IADDR_READ_DATA;
	}

	twi_send_start();
    
	/* Wait for the transaction to complete */
	while(twi_master_busy);
	
	return twi_master_get_status();
}

/**
 * \brief returns the status of TWI bus
 *
 * \param none
 */
status_code_t twi_master_get_status(void)
{
	return master_transfer.status;
}

/**
 * \brief Inits TWI module as master
 *
 * This function is a TWI Master initialisation.
 *
 * \param opt twi setting options
 *                  (see \ref twi_master_options_t)
 */
status_code_t twi_master_init(volatile void *twi, twi_master_options_t *opt)
{
	cpu_irq_disable();
	
	TWCR = 0x00;
	/* ! prescaler */
	TWSR = TWI_PRESCALE_REG;
	/* ! Set bit rate */
	TWBR = opt->baud_reg;

	twi_interrupt_enable();
	
	cpu_irq_enable();
	
	twi_mode = MASTER;

	return STATUS_OK;
}

/** @}*/

/**
 * \ingroup group_megarf_drivers_twi
 * \defgroup group_megarf_drivers_twis twi slave Driver
 *
 * This driver is the implementation of TWI module of megaRF device as slave.
 * Master initiates and terminates a data transmission.In the Slave Transmitter
 * mode, a number of data bytes are transmitted to a Master. In the Slave Receiver
 * mode, a number of data bytes are received from a Master. This uses two 
 * bi-directional bus lines(PD0 and PD1), one to receive clock (SCL) from 
 * master and one for data (SDA)
 *
 * \section megarf_twis_configuration Configuration
 * Configuration is done in the configuration file : conf_twi.h
 *
 * Configuration defines:
 * - \ref SLAVE_BUS_ADDR   : TWI slave bus address to respond to master
 *
 * - \ref TWI_GCE : TWI slave general call enable. TWI will respond to the
 *                  general call address (0x00), otherwise it will ignore
 *                  the general call address
 *
 * \section megarf_twis_interface Interface
 * The twi module should be configured as slave. The slave address is set
 * in TWAR register. The upper 7 bits are the address to which the 2-wire Serial
 * Interface will respond when addressed by a Master. If the LSB is set(TWI_GCE), 
 * the TWI will respond to the general call address (0x00), otherwise it will
 * ignore the general call address.
 * Enabling of TWI as slave is done by calling the function:
 * \code twi_slave_start() \endcode
 *
 * \subsection megarf_twis_usage Usage
 * The include file needed is:
 * \code #include "twi_megarf.h" \endcode
 *
 * After initiating the twi as slave,the TWI waits until it is addressed by its
 * own slave address (or the general call address if enabled) followed by the
 * data direction bit. If the direction bit is (read), the TWI will operate in Slave
 * transmit mode, otherwise Slave receive mode is entered
 *
 * The status and state is indicated to the application code through the
 * following functions
 * \code twi_slave_status_get() and twi_slave_state_get() \endcode
 * @{
 */

/**
 * \brief Enable TWI as slave
 *
 * \param none.
 */
static inline void twi_slave_enable(void)
{
	TWCR = (1 << TWEN) |     /* Enable TWI-interface and release TWI pins */
			(1 << TWIE) | (1 << TWINT) | /* Keep interrupt enabled
	                                              * and clear the flag */
			(1 << TWEA) | (0 << TWSTA) | (0 << TWSTO) | /* Acknowledge on any
	                                                     * new requests */
			(0 << TWWC);
}

/**
 * \brief  writing an byte to TWI.
 *
 * \param data - contains the data to transmitted ro master.
 */
static inline void twi_slave_write_byte(uint8_t data)
{
	TWDR =  data;
	twi_slave_enable();
}

/**
 * \brief Sending data to twi bus.
 *
 * \param none.
 */
static void twi_slave_data_write(void)
{
	twi_slave_write_byte(slave_transfer.data_buffer->tx_buffer[slave_transfer.data_count
			++]);
}

/**
 * \brief Notification of Last byte is transmitted
 *
 * \param none.
 */
static void twi_slave_last_byte_write_done(void)
{
	slave_transfer.state = TWI_IDLE;
	slave_transfer.status = TWI_STATUS_TX_COMPLETE;

	twi_slave_enable();
}

/**
 * \brief Read received data
 *
 * \param data - Contains data read from twi bus to be written to data receive
 *buffer.
 */
static void twi_slave_data_read(uint8_t data)
{
	slave_transfer.data_buffer->rx_buffer[slave_transfer.data_count++] = data;
	twi_slave_enable();
}

/**
 * \brief Reset TWI bus
 *
 * \param none.
 */
static void twi_slave_bus_reset(void)
{
	slave_transfer.state = TWI_IDLE;
	twi_reset();
}

/**
 * \brief Get the status of TWI transceiver
 *
 * \return int - status information
 */
int twi_slave_status_get(void)
{
	return(slave_transfer.status);
}

/**
 * \brief Get the state of TWI transceiver
 *
 * \return int - state information
 */
int twi_slave_state_get(void)
{
	return(slave_transfer.state);
}

/**
 * \brief Initialize TWI as Slave
 *
 * \param TWI_ownAddress - contains Slave own Address
 */
status_code_t twi_slave_init(uint8_t twi_slave_ownadd)
{
	TWAR = twi_slave_ownadd;    /* Set own TWI slave address. Accept TWI
	                             * General Calls. */
	TWCR = (1 << TWEN) |        /* Enable TWI-interface and
	                             * release TWI pins. */
			(0 << TWIE) | (0 << TWINT) | /* Disable TWI Interupt. */
			(0 << TWEA) | (0 << TWSTA) | (0 << TWSTO) | /* Do not ACK on
	                                              * any requests yet. */
			(0 << TWWC);
			
	slave_transfer.state       = TWI_IDLE;
	slave_transfer.status      = TWI_STATUS_NO_STATE;			
			
	return STATUS_OK;		
}

/**
 * \brief Start the slave Transceiver
 *
 * \return status_code_t - status of twi slave
 */
status_code_t twi_slave_start(slave_data_buffer_t *package)
{
	if (TWI_IDLE != slave_transfer.state) {
		return OPERATION_IN_PROGRESS;
	}

	/* Initiate a transaction when the bus is ready. */
	slave_transfer.data_buffer     = (slave_data_buffer_t *)package;
	slave_transfer.data_count  = 0;

	twi_slave_status_reset();

	return STATUS_OK;
}

/**
 * \brief Resets the slave state and status to initial for next
 * transmission/reception
 */
void twi_slave_status_reset(void)
{
	slave_transfer.state       = TWI_IDLE;
	slave_transfer.status      = TWI_STATUS_NO_STATE;
	twi_mode                   = SLAVE;

	twi_slave_enable();
}

/** @}*/

/**
 * \internal
 *
 * \brief TWI Interrupt Handler
 */
static void twi_interrupt_handler(void)
{
	uint8_t status;
	status = TWI_TWSR_STATUS_MASK;

	switch (status) {
	case TWS_START:   /*A START condition has been transmitted.*/
	case TWS_RSTART:  /*A repeated START condition has been
	                                        *transmitted.*/
		twi_master_start();
		break;

	case TWS_MT_SLA_ACK:  /*SLA+W has been transmitted; ACK has
	                                        *been received.*/
	case TWS_MT_DATA_ACK:   /*Data byte has been transmitted; ACK has
	                                        *been received.*/
		twi_master_write_done();
		break;

	case TWS_BUSERROR:   /*Bus error due to illegal START or STOP
	                                        * condition.*/
	case TWS_MT_SLA_NACK:  /*SLA+W has been transmitted; NOT ACK has
	                                        *been received.*/
	case TWS_MT_DATA_NACK:  /*Data byte has been transmitted; NOT ACK
	                                        *has been received.*/
	case TWS_MR_SLA_NACK:  /*SLA+R has been transmitted; NOT ACK has
	                                       *been received.*/
		twi_master_bus_reset();
		master_transfer.status = ERR_IO_ERROR;
		break;

	case TWS_MR_SLA_ACK:  /*SLA+R has been transmitted; ACK has been
	                                       *received.*/
		twi_master_addr_ack();
		break;

	case TWS_MR_DATA_ACK:  /*Data byte has been received; ACK has been
	                                       *returned.*/
		twi_master_read_done(twi_read_byte());
		break;

	case TWS_MR_DATA_NACK:   /*Data byte has been received; NOT ACK has
	                                       *been returned.*/
		twi_master_read_last_byte(twi_read_byte());
		break;

	case TWS_M_ARB_LOST:  /*Arbitration lost in SLA+W or data bytes
	                       *(Transmitter); Arbitration lost in SLA+R or
	                       *NOT ACK bit (Receiver).*/
	    /* If arbitration lost indicate to application to decide either
		 * switch to Slave mode or wait until the bus is free and transmit
		 * a new START condition */
	    master_transfer.state = TWI_IDLE; 
		master_transfer.status = ERR_BUSY;
		twi_master_busy = false;
		break;

	case TWS_ST_SLA_ACK:        /* Own SLA+R has been received; ACK has been
		                     * returned */
	case TWS_ST_SLA_ACK_M_ARB_LOST:   /* ! Arbitration lost in SLA+R/W as Master;
	                                   *own SLA+R has been received;
	                                   *ACK has been returned */	
		slave_transfer.data_count   = 0; /* Set buffer pointer to first data
		                            * location */

	case TWS_ST_DATA_ACK:       /* Data byte in TWDR has been transmitted;
		                     * ACK has been received */
		twi_slave_data_write();
		slave_transfer.state = TWI_PROCESS;
		break;

	case TWS_ST_DATA_NACK:      /* Data byte in TWDR has been transmitted;
		                     * NACK has been received. */
		                    /* I.e. this could be the end of the
		                     * transmission. */
		twi_slave_last_byte_write_done();
		break;

	case TWS_SR_GEN_ACK:     /* General call address has been received;
		                     * ACK has been returned */
	case TWS_SR_GEN_ACK_M_ARB_LOST:  /* ! Arbitration lost in SLA+R/W as Master; 
	                                  * General call address has been received;
	                                  *  ACK has been returned */   	
	case TWS_SR_SLA_ACK:        /* Own SLA+W has been received ACK has been
		                     * returned */
	case TWS_SR_SLA_ACK_M_ARB_LOST: /* ! Arbitration lost in  SLA+R/W as Master;
	                                 * own SLA+W has been received; 
	                                 * ACK has been returned */	
		slave_transfer.data_count   = 0; /* Set buffer pointer to first data
		                            * location */
		twi_slave_enable();
		slave_transfer.state = TWI_PROCESS;

		break;

	case TWS_SR_SLA_DATA_ACK:   /* Previously addressed with own SLA+W; data
		                     * has been received; ACK has been returned */
	case TWS_SR_GEN_DATA_ACK:   /* Previously addressed with general call;
		                     * data has been received; ACK has been
		                     * returned */
		twi_slave_data_read(twi_read_byte());
		slave_transfer.state = TWI_PROCESS;
		break;

	case TWS_SR_STOP_RESTART:   /* A STOP condition or repeated START
		                     * condition has been received while still
		                     * addressed as Slave */
		/* Enter not addressed mode and listen to address match */
		slave_transfer.state = TWI_IDLE;
		slave_transfer.status = TWI_STATUS_RX_COMPLETE;
		twi_slave_enable();
		break;

	case TWS_SR_SLA_DATA_NACK:  /* Previously addressed with own SLA+W; data
		                     * has been received; NOT ACK has been
		                     * returned */
	case TWS_SR_GEN_DATA_NACK:  /* Previously addressed with general call;
		                     * data has been received; NOT ACK has been
		                     * returned */
	case TWS_ST_DATA_ACK_LAST_BYTE: /* Last data byte in TWDR has been
		                         * transmitted (TWEA = ; ACK has
		                         * been received */
		twi_slave_bus_reset();
		slave_transfer.status = TWI_STATUS_IO_ERROR;
		break;

	default:
	    if(twi_mode == MASTER)
		{
			master_transfer.state = TWI_IDLE;
			master_transfer.status = ERR_PROTOCOL;
			twi_master_busy = false;
		}
		else
		{
			slave_transfer.status = TWI_STATUS_PROTOCOL_ERROR;  /* Store TWI State as
		                            * errormessage, operation also
		                            * clears the Success bit */
		    slave_transfer.state = TWI_IDLE;
		    twi_slave_enable();		
		}
     	break;
	}
}

/**
 * \brief TWI interrupt Vector
 */
ISR(TWI_vect)
{
	twi_interrupt_handler();
}	
