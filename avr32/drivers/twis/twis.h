/*****************************************************************************
 *
 * \file
 *
 * \brief TWIS driver for AVR32 UC3.
 *
 * This file defines a useful set of functions for TWIS on AVR32 devices.
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

#ifndef _TWIS_H_
#define _TWIS_H_

/**
 * \defgroup group_avr32_drivers_twis TWI - Two-Wire Slave Interface
 *
 * Driver for the TWIS (Two-Wire Slave Interface).
 * This driver provides access to the main features of the TWIS controller.
 * The TWIS interconnects a TWI slave on a unique two-wire bus.
 * The TWIS is programmable as a slave with sequential or single-byte access.
 * Internal Address access and Ten bit addressing mode is supported.
 *
 * \{
 */
 
#include <avr32/io.h>
#include <stdint.h>
#include "compiler.h"
#include "status_codes.h"
#include "conf_twis.h"

/** 
 * \name TWI Driver Compatibility
 * Codes for UC3 devices using TWI modules can easily be ported
 * to UC3 devices with TWIM module
 * @{
 */
#define avr32_twi_t avr32_twis_t
#define twi_options_t twis_options_t
#define twi_package_t twis_package_t
#define twi_slave_fct_t twis_slave_fct_t
#define twi_slave_init twis_slave_init
//! @}

/*!
 * \brief Input parameters when initializing the twi module mode
 */
typedef struct
{
	//! The PBA clock frequency.
	uint32_t pba_hz;
	//! The baudrate of the TWI bus.
	uint32_t speed;
	//! The desired address.
	uint8_t chip;
	//! smbus mode
	bool smbus;
	//! tenbit mode
	bool tenbit;
}
twis_options_t;

/*!
 * \brief Information concerning the data transmission
 */
typedef struct
{
	//! TWI chip address to communicate with.
	uint8_t chip;
	//! TWI address/commands to issue to the other chip (node).
	uint32_t addr;
	//! Length of the TWI data address segment (1-3 bytes).
	uint8_t addr_length;
	//! Pointer to the transmit buffer
	void *buffer;
	//! The number of bytes in the buffer
	uint32_t length;
}
twis_package_t;

/*!
 * \brief Pointer on TWI slave user specific application routines
 */
typedef struct
{
	//! Routine to receive data from TWI master
	void (*rx) (uint8_t);
	//! Routine to transmit data to TWI master
	uint8_t (*tx) (void);
	//! Routine to signal a TWI STOP
	void (*stop) (void);
}
twis_slave_fct_t;

// Function Declarations

status_code_t twis_slave_init (volatile avr32_twis_t *twis,
		const twis_options_t *opt, const twis_slave_fct_t *slave_fct);

void twis_send_data_ack(volatile avr32_twis_t *twis);

void twis_send_data_nack(volatile avr32_twis_t *twis, bool stop_callback);

/**
 * \}
 */
#endif // _TWIS_H_
