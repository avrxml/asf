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
 * Copyright (c) 2010-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


#ifndef _TWIM_H_
#define _TWIM_H_

/**
 * \defgroup group_avr32_drivers_twim TWI - Two-Wire Master Interface
 *
 * Driver for the TWIM (Two-Wire Master Interface).
 * This driver provides access to the main features of the TWIM controller.
 * The TWIM interconnects components on a unique two-wire bus.
 * The TWIM is programmable as a master with sequential or single-byte access.
 * Multiple master capability is supported.
 *
 * \{
 */

#include <avr32/io.h>
#include <stdint.h>
#include "compiler.h"
#include "status_codes.h"
#include "conf_twim.h"

//! @{
//! TWI Standard Mode
#define TWI_STD_MODE_SPEED      (100000 /* kbit/s */)
//! TWI Fast Mode
#define TWI_FAST_MODE_SPEED     (400000 /* kbit/s */)
//! @}

//! Status Clear Register Mask for No Acknowledgements
#define AVR32_TWIM_SCR_NAK_MASK (AVR32_TWIM_SCR_ANAK_MASK \
		| AVR32_TWIM_SCR_DNAK_MASK)
//! Status Register Mask for No Acknowledgements
#define AVR32_TWIM_SR_NAK_MASK  (AVR32_TWIM_SR_ANAK_MASK  \
		| AVR32_TWIM_SR_DNAK_MASK)
//! Interrupt Enable Register Mask for No Acknowledgements
#define AVR32_TWIM_IER_NAK_MASK (AVR32_TWIM_IER_ANAK_MASK \
		| AVR32_TWIM_IER_DNAK_MASK)
//! Frequently used Interrupt Enable Register Mask
#define AVR32_TWIM_IER_STD_MASK (AVR32_TWIM_IER_NAK_MASK  \
		| AVR32_TWIM_IER_ARBLST_MASK)
//! Frequently used Status Clear Register Mask
#define AVR32_TWIM_SR_STD_MASK  (AVR32_TWIM_SR_NAK_MASK  \
		| AVR32_TWIM_SR_ARBLST_MASK)

/**
 * \name TWI Driver Compatibility
 * Codes for UC3 devices using TWI modules can easily be ported
 * to UC3 devices with TWIM module
 * @{
 */
#define avr32_twi_t avr32_twim_t
#define twi_options_t twim_options_t
#define twi_package_t twim_package_t
#define twi_master_init twim_master_init
#define twi_probe twim_probe
//! @}

/**
 * \brief Status Codes for TWI Transfer
 * @{
 */
enum twim_transfer_status{
	TWI_SUCCESS = 0,            //!< \brief TWI Transaction Success
	TWI_INVALID_ARGUMENT = -1,  //!< \brief Invalid Argument Passed
	TWI_ARBITRATION_LOST = -2,  //!< \brief Bus Arbitration Lost
	TWI_NO_CHIP_FOUND = -3,     //!< \brief Slave Not Found
	TWI_RECEIVE_NACK = -4,      //!< \brief Data No Acknowledgement Received
	TWI_SEND_NACK = -5,         //!< \brief Data No Acknowledgement Send
	TWI_INVALID_CLOCK_DIV = -6  //!< \brief Invalid Clock Divider Value
};

typedef enum twim_transfer_status twim_transfer_status_t;
/* @} */

/**
 * \brief Input parameters when initializing the twim module mode
 */
typedef struct
{
	//! The PBA clock frequency.
	uint32_t pba_hz;
	//! The baudrate of the TWI bus.
	uint32_t speed;
	//! The desired address.
	uint32_t chip;
	//! SMBUS mode
	bool smbus;
}
twim_options_t;

/**
 * \brief Information concerning the data transmission
 */
typedef struct
{
	//! TWI chip address to communicate with.
	uint32_t chip;
	//! TWI address/commands to issue to the other chip (node).
	uint8_t addr[3];
	//! Length of the TWI data address segment (1-3 bytes).
	uint8_t addr_length;
	//! Where to find the data to be written.
	void *buffer;
	//! How many bytes do we want to write.
	uint32_t length;
	//! Whether to wait if bus is busy (false) or return immediately (true)
	bool no_wait;
}
twim_package_t;

/**
 * \brief Information concerning the data transmission
 */
typedef struct
{
	//! TWI chip address to communicate with.
	uint32_t chip;
	//! Where to find the data .
	void *buffer;
	//! How many bytes do we want to transfer.
	uint32_t length;
	//! Transfer direction
	bool read;
}
twim_transfer_t;

/**
 * \brief Enable Master Mode of the TWI.
 *
 * \param twim   Base address of the TWI instance.
 */
static inline void twim_master_enable (volatile avr32_twim_t *twim)
{
	twim->cr = AVR32_TWIM_CR_MEN_MASK;;
}

/**
 * \brief Disable Master Mode of the TWI.
 *
 * \param twim   Base address of the TWI instance.
 */
static inline void twim_master_disable (volatile avr32_twim_t *twim)
{
	twim->cr = AVR32_TWIM_CR_MDIS_MASK;;
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
extern status_code_t twim_master_init (volatile avr32_twim_t *twim,
		const twim_options_t *opt);

/**
 * \brief Set the twim bus speed in conjunction with the clock frequency
 *
 * \param twim              Base address of the TWIM (i.e. &AVR32_TWIM).
 * \param speed             The desired twim bus speed
 * \param pba_hz            The current running PBA clock frequency
 * \retval STATUS_OK        Transaction is successful
 * \retval ERR_INVALID_ARG  Invalid arg resulting in wrong CWGR Exponential
 */
extern status_code_t twim_set_speed (volatile avr32_twim_t *twim,
		uint32_t speed, uint32_t pba_hz);

/**
 * \brief Test if a chip answers for a given twi address
 *
 * \param twim            Base address of the TWIM (i.e. &AVR32_TWIM)
 * \param chip_addr       Address of the chip which is searched for
 * \retval STATUS_OK      Slave Found
 * \retval ERR_IO_ERROR   ANAK received or Bus Arbitration lost
 */
extern status_code_t twim_probe (volatile avr32_twim_t *twim,
		uint32_t chip_addr);

/**
 * \brief Read multiple bytes from a TWI compatible slave device
 *
 * \param twim            Base address of the TWIM (i.e. &AVR32_TWIM)
 * \param package         Package information and data
 *                        (see \ref twim_package_t)
 * \retval STATUS_OK      If all bytes were read successfully
 * \retval ERR_IO_ERROR   NACK received or Bus Arbitration lost
 */
extern status_code_t twim_read_packet (volatile avr32_twim_t *twim,
		const twim_package_t *package);

/**
 * \brief Write multiple bytes to a TWI compatible slave device
 *
 * \param twim            Base address of the TWIM (i.e. &AVR32_TWIM)
 * \param *package        Package information and data
 *                        (see \ref twim_package_t)
 * \retval STATUS_OK      If all bytes were send successfully
 * \retval ERR_IO_ERROR   NACK received or Bus Arbitration lost
 */
extern status_code_t twim_write_packet (volatile avr32_twim_t *twim,
		const twim_package_t *package);

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
extern status_code_t twim_read (volatile avr32_twim_t *twim, uint8_t *buffer,
		uint32_t nbytes, uint32_t saddr, bool tenbit);

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
extern status_code_t twim_write (volatile avr32_twim_t *twim,
		const uint8_t *buffer, uint32_t nbytes, uint32_t saddr,
		bool tenbit);

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
extern status_code_t twim_chained_transfer (volatile avr32_twim_t *twim,
		volatile twim_transfer_t *first,
		volatile twim_transfer_t *second,
		bool tenbit);

/**
 * \brief Compatibility with TWI Module driver for read operation
 *
 * \param twim         Base address of the TWIM (i.e. &AVR32_TWIM).
 * \param package      Package information and data
 *                     (see \ref twim_package_t)
 * \return STATUS_OK   If all bytes were read, error code otherwise
 */
static inline status_code_t twi_master_read (volatile avr32_twi_t *twim,
		const twi_package_t *package)
{
	return (twim_read_packet (twim, package));
}

/**
 * \brief Compatibility with TWI Module drivers for write operation
 *
 * \param twim         Base address of the TWIM (i.e. &AVR32_TWI).
 * \param *package     Package information and data
 *                     (see \ref twim_package_t)
 * \return STATUS_OK   if all bytes were written, error code otherwise
 */
static inline status_code_t twi_master_write (volatile avr32_twi_t *twim,
		const twi_package_t *package)
{
	return (twim_write_packet (twim, package));
}

/**
 * \brief Disable the TWI interrupts and clear its status register
 *
 * \param twim   Base address of the TWIM (i.e. &AVR32_TWI).
 */
extern void twim_disable_interrupt (volatile avr32_twim_t *twim);

/*!
 * \brief Information about the current status of the TWI Bus
 */
extern uint8_t twim_status ( void );

/**
 * \}
 */

#endif // _TWI_H_
