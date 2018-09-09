/*****************************************************************************
 *
 * \file
 *
 * \brief AT24cxx driver for AVR32 UC3.
 *
 * This file is the AT24cxx driver.
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


#ifndef _AT24CXX_H_
#define _AT24CXX_H_
#include <stdint.h>

/**
 * \defgroup group_avr32_components_memory_eeprom_at24cxx MEMORY - EEPROM AT24Cxx Series
 *
 * Driver to access (read and write) AT24Cxx series EEPROM through a TWI interface.
 *
 * \{
 */

/*! \brief This function will initialize the AT24CXX serial EEPROM.
 *  \note Must be called before any of the access functions.
 *
 *  \retval true AT24CXX device ready to use.
 *  \retval false Not able to initialize the AT24CXX device.
 */
void at24cxx_init(int32_t fcpu);

/*! \brief Write single byte to the serial EEPROM.
 *
 *  \param[in] byte_address Address of byte to write.
 *  \param[in] byte_value Value that will be written to the specified address.
 *
 *  \retval true Byte written successfully.
 *  \retval false Byte could not be written.
 */
void at24cxx_write_byte(uint16_t byte_address, uint8_t byte_value);


/*! \brief Write bytes continuously to the serial EEPROM.
 *
 *  \param[in] start_address Address of first byte in transaction.
 *  \param[in] length Number of bytes to write.
 *  \param[in] wr_buffer Pointer to array where the bytes to be written are stored.
 *
 *  \retval true Bytes written successfully.
 *  \retval false Bytes could not be written.
 */
void at24cxx_write_continuous(uint16_t start_address, uint16_t length, uint8_t const *wr_buffer) ;


/*! \brief Read single byte from serial EEPROM.
 *
 *  \param[in] byte_address Address of byte to read.
 *  \param[out] read_byte Pointer to memory where the read byte will be stored.
 *
 *  \retval true Byte read successfully.
 *  \retval false Byte could not be read.
 */
uint8_t at24cxx_read_byte(uint16_t byte_address) ;


/*! \brief Read bytes continuously from the serial EEPROM.
 *
 *  \param[in] start_address Address of first byte to read.
 *  \param[in] length Number of bytes to read.
 *  \param[out] rd_buffer Pointer to memory where the read bytes will be stored.
 *
 *  \retval true Bytes read successfully.
 *  \retval false Bytes could not be read.
 */
void at24cxx_read_continuous(uint16_t start_address, uint16_t length, uint8_t *rd_buffer);
//! @}

/**
 * \}
 */

#endif
