/**
 * \file
 *
 * \brief TWI driver for AVR.
 *
 * This file defines a useful set of functions for the TWI interface on AVR
 * devices.
 *
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
 * \defgroup group_xmega_drivers_twi_twim TWI Master
 *
 * \ingroup group_xmega_drivers_twi
 *
 * \{
 */

#ifdef __cplusplus
extern "C" {
#endif


#include <compiler.h>
#include <status_codes.h>

#include "conf_twim.h"
#include "twi_common.h"


/*! \brief Error Codes for the Module
 *
 * \deprecated
 * This definition is provided for compatibility with existing ASF example
 * applications.  This module uses the \ref status_code_t values that will
 * replace module-specific error codes in ASF drivers.
 */
#define TWI_SUCCESS (STATUS_OK)


/*! Baud register setting calculation. Formula described in datasheet. */
#define TWI_BAUD(F_SYS, F_TWI) ((F_SYS / (2 * F_TWI)) - 5)


/*! \brief Initialize the twi master module
 *
 * \param twi       Base address of the TWI (i.e. &TWIC).
 * \param *opt      Options for initializing the twi module
 *                  (see \ref twi_options_t)
 * \retval STATUS_OK        Transaction is successful
 * \retval ERR_INVALID_ARG  Invalid arguments in \c opt.
 */
status_code_t twi_master_init(TWI_t *twi, const twi_options_t *opt);

/*! \brief Perform a TWI master write or read transfer.
 *
 * This function is a TWI Master write or read transaction.
 *
 * \param twi       Base address of the TWI (i.e. &TWI_t).
 * \param package   Package information and data
 *                  (see \ref twi_package_t)
 * \param read      Selects the transfer direction
 *
 * \return  status_code_t
 *      - STATUS_OK if the transfer completes
 *      - ERR_BUSY to indicate an unavailable bus
 *      - ERR_IO_ERROR to indicate a bus transaction error
 *      - ERR_NO_MEMORY to indicate buffer errors
 *      - ERR_PROTOCOL to indicate an unexpected bus state
 *      - ERR_INVALID_ARG to indicate invalid arguments.
 */
status_code_t twi_master_transfer(TWI_t *twi, const twi_package_t *package,
		bool read);

/*! \brief Read multiple bytes from a TWI compatible slave device
 *
 * \param twi       Base address of the TWI (i.e. &TWI_t).
 * \param package   Package information and data
 *                  (see \ref twi_package_t)
 * \return STATUS_OK   If all bytes were read, error code otherwise
 */
static inline status_code_t twi_master_read(TWI_t *twi,
		const twi_package_t *package)
{
	return twi_master_transfer (twi, package, true);
}

/*! \brief Write multiple bytes to a TWI compatible slave device
 *
 * \param twi       Base address of the TWI (i.e. &TWI_t).
 * \param package   Package information and data
 *                  (see \ref twi_package_t)
 * \return STATUS_OK   If all bytes were written, error code otherwise
 */
static inline status_code_t twi_master_write(TWI_t *twi,
		const twi_package_t *package)
{
	return twi_master_transfer (twi, package, false);
}

/*! \brief Enable Master Mode of the TWI.
 *
 * \param twi       Base address of the TWI instance.
 */
static inline void twi_master_enable(TWI_t *twi)
{
  twi->MASTER.CTRLA |= TWI_MASTER_ENABLE_bm;
}

/*! \brief Disable Master Mode of the TWI.
 *
 * \param twi       Base address of the TWI instance.
 */
static inline void twi_master_disable(TWI_t *twi)
{
  twi->MASTER.CTRLA &= (~TWI_MASTER_ENABLE_bm);
}


#ifdef __cplusplus
}
#endif

/**
 * \}
 */

#endif // _TWIM_H_
