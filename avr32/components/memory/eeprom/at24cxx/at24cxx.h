/*****************************************************************************
 *
 * \file
 *
 * \brief AT24cxx driver for AVR32 UC3.
 *
 * This file is the AT24cxx driver.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
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
