/**
 * \file
 *
 * \brief This file controls the software Serial FIFO management.
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
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

#ifndef _SER_FIFO_H_
#define _SER_FIFO_H_

#include "compiler.h"

/**
 * \defgroup fifo_group serial First-In-First-Out Buffer (FIFO)
 *
 * See \ref fifo_quickstart.
 *
 * These functions manages FIFOs thanks to simple a API. The FIFO can
 * be 100% full thanks to a double-index range implementation. For example,
 * a FIFO of 4 elements can be implemented: the FIFO can really hold up to 4
 * elements. This is particularly well suited for any kind of application
 * needing a lot of small FIFO. The maximum fifo size is 128 items (uint8,
 * uint16 or uint32). Note that the driver, thanks to its conception, does
 * not use interrupt protection.
 *
 * @{
 */

//! Error codes used by SERIAL FIFO driver.
enum {
	SER_FIFO_OK = 0,          //!< Normal operation.
	SER_FIFO_ERROR_OVERFLOW,  //!< Attempt to push something in a FIFO that is full.
	SER_FIFO_ERROR_UNDERFLOW, //!< Attempt to pull something from a FIFO that is empty
	SER_FIFO_ERROR,           //!< Error condition during FIFO initialization
};

//! FIFO descriptor used by FIFO driver.
struct ser_fifo_desc {
	union
	{
		uint32_t *u32ptr; //!< Pointer to unsigned-32 bits location
		uint16_t *u16ptr; //!< Pointer to unsigned-16 bits location
		uint8_t  *u8ptr;  //!< Pointer to unsigned-8 bits location
	}  buffer;
	volatile uint16_t read_index;  //!< Read index
	volatile uint16_t write_index; //!< Write index
	uint16_t size;                 //!< Size of the FIFO (unit is in number of 'element')
	uint16_t mask;                 //!< Mask used to speed up FIFO operation (wrapping)
};

typedef struct ser_fifo_desc ser_fifo_desc_t;

/**
 *  \brief Initializes a new software FIFO for a certain 'size'.
 *
 *  \pre Both fifo descriptor and buffer must be allocated by the caller before.
 *
 *  \param ser_fifo_desc  Pointer on the FIFO descriptor.
 *  \param buffer     Pointer on the FIFO buffer.
 *  \param size       Size of the buffer (unit is in number of 'elements').
 *                    It must be a 2-power and <= to 128.
 *
 *  \return Status
 *    \retval FIFO_OK when no error occurred.
 *    \retval FIFO_ERROR when the size is not a 2-power.
 */
int ser_fifo_init(ser_fifo_desc_t *ser_fifo_desc, void *buffer, uint16_t size);

/**
 *  \brief Returns the number of elements in the FIFO.
 *
 *  \param ser_fifo_desc  The FIFO descriptor.
 *
 *  \return The number of used elements.
 */
static inline uint16_t ser_fifo_get_used_size(ser_fifo_desc_t *ser_fifo_desc)
{
        uint16_t read_index;
        read_index = ser_fifo_desc->read_index;
	return ((ser_fifo_desc->write_index - read_index) & ser_fifo_desc->mask);
}

/**
 *  \brief Returns the remaining free spaces of the FIFO (in number of elements).
 *
 *  \param ser_fifo_desc  The FIFO descriptor.
 *
 *  \return The number of free elements.
 */
static inline uint8_t ser_fifo_get_free_size(ser_fifo_desc_t *ser_fifo_desc)
{
	return ser_fifo_desc->size - ser_fifo_get_used_size(ser_fifo_desc);
}

/**
 *  \brief Tests if a FIFO is empty.
 *
 *  \param ser_fifo_desc  The FIFO descriptor.
 *
 *  \return Status
 *    \retval true when the FIFO is empty.
 *    \retval false when the FIFO is not empty.
 */
static inline bool ser_fifo_is_empty(ser_fifo_desc_t *ser_fifo_desc)
{
  uint16_t read_index;
  read_index = ser_fifo_desc->read_index;
  if(read_index == ser_fifo_desc->write_index)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/**
 *  \brief Tests if a FIFO is full.
 *
 *  \param ser_fifo_desc  The FIFO descriptor.
 *
 *  \return Status
 *    \retval true when the FIFO is full.
 *    \retval false when the FIFO is not full.
 */
static inline bool ser_fifo_is_full(ser_fifo_desc_t *ser_fifo_desc)
{
	return (ser_fifo_get_used_size(ser_fifo_desc) == ser_fifo_desc->size);
}

/**
 *  \brief Puts a new 8-bits element into the FIFO.
 *
 *  \param ser_fifo_desc  The FIFO descriptor.
 *  \param item       extracted element.
 */
static inline void ser_fifo_push_uint8_nocheck(ser_fifo_desc_t *ser_fifo_desc, uint32_t item)
{
	uint8_t write_index;

	write_index = ser_fifo_desc->write_index;
	ser_fifo_desc->buffer.u8ptr[write_index & (ser_fifo_desc->mask >> 1)] = item;
	write_index = (write_index + 1) & ser_fifo_desc->mask;

	// Must be the last thing to do.
	barrier();
	ser_fifo_desc->write_index = write_index;
}

/**
 *  \brief Puts a new 8-bits element into the FIFO and
 *         checks for a possible overflow.
 *
 *  \param ser_fifo_desc  The FIFO descriptor.
 *  \param item       extracted element.
 *
 *  \return Status
 *    \retval FIFO_OK when no error occurred.
 *    \retval FIFO_ERROR_UNDERFLOW when the FIFO was empty.
 */
static inline int ser_fifo_push_uint8(ser_fifo_desc_t *ser_fifo_desc, uint32_t item)
{
	uint16_t write_index;

	if (ser_fifo_is_full(ser_fifo_desc)) {
		return SER_FIFO_ERROR_OVERFLOW;
	}

	write_index = ser_fifo_desc->write_index;
	ser_fifo_desc->buffer.u8ptr[write_index & (ser_fifo_desc->mask >> 1)] = item;
	write_index = (write_index + 1) & ser_fifo_desc->mask;

	// Must be the last thing to do.
	barrier();
	ser_fifo_desc->write_index = write_index;

	return SER_FIFO_OK;
}

/**
 *  \brief Puts a new 16-bits element into the FIFO.
 *
 *  \param ser_fifo_desc  The FIFO descriptor.
 *  \param item       extracted element.
 */
static inline void ser_fifo_push_uint16_nocheck(ser_fifo_desc_t *ser_fifo_desc, uint32_t item)
{
	uint8_t write_index;

	write_index = ser_fifo_desc->write_index;
	ser_fifo_desc->buffer.u16ptr[write_index & (ser_fifo_desc->mask >> 1)] = item;
	write_index = (write_index + 1) & ser_fifo_desc->mask;

	// Must be the last thing to do.
	barrier();
	ser_fifo_desc->write_index = write_index;
}

/**
 *  \brief Puts a new 16-bits element into the FIFO and
 *         checks for a possible overflow.
 *
 *  \param ser_fifo_desc  The FIFO descriptor.
 *  \param item       extracted element.
 *
 *  \return Status
 *    \retval FIFO_OK when no error occurred.
 *    \retval FIFO_ERROR_UNDERFLOW when the FIFO was empty.
 */
static inline int ser_fifo_push_uint16(ser_fifo_desc_t *ser_fifo_desc, uint32_t item)
{
	uint8_t write_index;

	if (ser_fifo_is_full(ser_fifo_desc)) {
		return SER_FIFO_ERROR_OVERFLOW;
	}

	write_index = ser_fifo_desc->write_index;
	ser_fifo_desc->buffer.u16ptr[write_index & (ser_fifo_desc->mask >> 1)] = item;
	write_index = (write_index + 1) & ser_fifo_desc->mask;

	// Must be the last thing to do.
	barrier();
	ser_fifo_desc->write_index = write_index;

	return SER_FIFO_OK;
}

/**
 *  \brief Puts a new 32-bits element into the FIFO.
 *
 *  \param ser_fifo_desc  The FIFO descriptor.
 *  \param item       extracted element.
 */
static inline void ser_fifo_push_uint32_nocheck(ser_fifo_desc_t *ser_fifo_desc, uint32_t item)
{
	uint8_t write_index;

	write_index = ser_fifo_desc->write_index;
	ser_fifo_desc->buffer.u32ptr[write_index & (ser_fifo_desc->mask >> 1)] = item;
	write_index = (write_index + 1) & ser_fifo_desc->mask;

	// Must be the last thing to do.
	barrier();
	ser_fifo_desc->write_index = write_index;
}

/**
 *  \brief Puts a new 32-bits element into the FIFO and
 *         checks for a possible overflow.
 *
 *  \param ser_fifo_desc  The FIFO descriptor.
 *  \param item       extracted element.
 *
 *  \return Status
 *    \retval SER_FIFO_OK when no error occurred.
 *    \retval SER_FIFO_ERROR_UNDERFLOW when the FIFO was empty.
 */
static inline int ser_fifo_push_uint32(ser_fifo_desc_t *ser_fifo_desc, uint32_t item)
{
	uint8_t write_index;

	if (ser_fifo_is_full(ser_fifo_desc)) {
		return SER_FIFO_ERROR_OVERFLOW;
	}

	write_index = ser_fifo_desc->write_index;
	ser_fifo_desc->buffer.u32ptr[write_index & (ser_fifo_desc->mask >> 1)] = item;
	write_index = (write_index + 1) & ser_fifo_desc->mask;

	// Must be the last thing to do.
	barrier();
	ser_fifo_desc->write_index = write_index;

	return SER_FIFO_OK;
}

/**
 *  \brief Gets a 8-bits element from the FIFO.
 *
 *  \param ser_fifo_desc  The FIFO descriptor.
 *
 *  \return extracted element.
 */
static inline uint8_t ser_fifo_pull_uint8_nocheck(ser_fifo_desc_t *ser_fifo_desc)
{
	uint16_t read_index;
	uint8_t item;

	read_index = ser_fifo_desc->read_index;
	item = ser_fifo_desc->buffer.u8ptr[read_index & (ser_fifo_desc->mask >> 1)];
	read_index = (read_index + 1) & ser_fifo_desc->mask;

	// Must be the last thing to do.
	barrier();
	ser_fifo_desc->read_index = read_index;

	return item;
}

/**
 *  \brief Gets a 8-bits element from the FIFO and
 *         checks for a possible underflow.
 *
 *  \param ser_fifo_desc  The FIFO descriptor.
 *  \param item       extracted element.
 *
 *  \return Status
 *    \retval FIFO_OK when no error occurred.
 *    \retval FIFO_ERROR_UNDERFLOW when the FIFO was empty.
 */
static inline int ser_fifo_pull_uint8(ser_fifo_desc_t *ser_fifo_desc, uint8_t *item)
{
	uint16_t read_index;

	if (ser_fifo_is_empty(ser_fifo_desc)) {
		return SER_FIFO_ERROR_UNDERFLOW;
	}

	read_index = ser_fifo_desc->read_index;
	*item = ser_fifo_desc->buffer.u8ptr[read_index & (ser_fifo_desc->mask >> 1)];
	read_index = (read_index + 1) & ser_fifo_desc->mask;

	// Must be the last thing to do.
	barrier();
	ser_fifo_desc->read_index = read_index;

	return SER_FIFO_OK;
}

/**
 *  \brief Gets a 16-bits element from the FIFO.
 *
 *  \param ser_fifo_desc  The FIFO descriptor.
 *
 *  \return extracted element.
 */
static inline uint16_t ser_fifo_pull_uint16_nocheck(ser_fifo_desc_t *ser_fifo_desc)
{
	uint16_t read_index;
	uint16_t item;

	read_index = ser_fifo_desc->read_index;
	item = ser_fifo_desc->buffer.u16ptr[read_index & (ser_fifo_desc->mask >> 1)];
	read_index = (read_index + 1) & ser_fifo_desc->mask;

	// Must be the last thing to do.
	barrier();
	ser_fifo_desc->read_index = read_index;

	return item;
}

/**
 *  \brief Gets a 16-bits element from the FIFO and
 *         checks for a possible underflow.
 *
 *  \param ser_fifo_desc  The FIFO descriptor.
 *  \param item       extracted element.
 *
 *  \return Status
 *    \retval FIFO_OK when no error occurred.
 *    \retval FIFO_ERROR_UNDERFLOW when the FIFO was empty.
 */
static inline int ser_fifo_pull_uint16(ser_fifo_desc_t *ser_fifo_desc, uint16_t *item)
{
	uint16_t read_index;

	if (ser_fifo_is_empty(ser_fifo_desc)) {
		return SER_FIFO_ERROR_UNDERFLOW;
	}

	read_index = ser_fifo_desc->read_index;
	*item = ser_fifo_desc->buffer.u16ptr[read_index & (ser_fifo_desc->mask >> 1)];
	read_index = (read_index + 1) & ser_fifo_desc->mask;

	// Must be the last thing to do.
	barrier();
	ser_fifo_desc->read_index = read_index;

	return SER_FIFO_OK;
}

/**
 *  \brief Gets a 32-bits element from the FIFO
 *
 *  \param ser_fifo_desc  The FIFO descriptor.
 *
 *  \return extracted element.
 */
static inline uint32_t ser_fifo_pull_uint32_nocheck(ser_fifo_desc_t *ser_fifo_desc)
{
	uint16_t read_index;
	uint32_t item;

	read_index = ser_fifo_desc->read_index;
	item = ser_fifo_desc->buffer.u32ptr[read_index & (ser_fifo_desc->mask >> 1)];
	read_index = (read_index + 1) & ser_fifo_desc->mask;

	// Must be the last thing to do.
	barrier();
	ser_fifo_desc->read_index = read_index;

	return item;
}

/**
 *  \brief Gets a 32-bits element from the FIFO and
 *         checks for a possible underflow.
 *
 *  \param ser_fifo_desc  The FIFO descriptor.
 *  \param item       extracted element.
 *
 *  \return Status
 *    \retval FIFO_OK when no error occurred.
 *    \retval FIFO_ERROR_UNDERFLOW when the FIFO was empty.
 */
static inline int ser_fifo_pull_uint32(ser_fifo_desc_t *ser_fifo_desc, uint32_t *item)
{
	uint16_t read_index;

	if (ser_fifo_is_empty(ser_fifo_desc)) {
		return SER_FIFO_ERROR_UNDERFLOW;
	}

	read_index = ser_fifo_desc->read_index;
	*item = ser_fifo_desc->buffer.u32ptr[read_index & (ser_fifo_desc->mask >> 1)];
	read_index = (read_index + 1) & ser_fifo_desc->mask;

	// Must be the last thing to do.
	barrier();
	ser_fifo_desc->read_index = read_index;

	return SER_FIFO_OK;
}

/**
 *  \brief Gets a 32-bits element from the FIFO but does
 *         not remove it from the FIFO.
 *
 *  \param ser_fifo_desc  The FIFO descriptor.
 *
 *  \retval item      extracted element.
 */
static inline uint32_t ser_fifo_peek_uint32(ser_fifo_desc_t *ser_fifo_desc)
{
	return ser_fifo_desc->buffer.u32ptr[ser_fifo_desc->read_index & (ser_fifo_desc->mask >> 1)];
}

/**
 *  \brief Gets a 16-bits element from the FIFO but does
 *         not remove it from the FIFO.
 *
 *  \param ser_fifo_desc  The FIFO descriptor.
 *
 *  \retval item      extracted element.
 */
static inline uint16_t ser_fifo_peek_uint16(ser_fifo_desc_t *ser_fifo_desc)
{
	return ser_fifo_desc->buffer.u16ptr[ser_fifo_desc->read_index & (ser_fifo_desc->mask >> 1)];
}

/**
 *  \brief Gets a 8-bits element from the FIFO but does
 *         not remove it from the FIFO.
 *
 *  \param ser_fifo_desc  The FIFO descriptor.
 *
 *  \retval item      extracted element.
 */
static inline uint8_t ser_fifo_peek_uint8(ser_fifo_desc_t *ser_fifo_desc)
{
	return ser_fifo_desc->buffer.u8ptr[ser_fifo_desc->read_index & (ser_fifo_desc->mask >> 1)];
}

/**
 *  \brief Flushes a software FIFO.
 *
 *  \param ser_fifo_desc  The FIFO descriptor.
 */
static inline void ser_fifo_flush(ser_fifo_desc_t *ser_fifo_desc)
{
	// Fifo starts empty.
	ser_fifo_desc->read_index = ser_fifo_desc->write_index = 0;
}


#endif  // _SER_FIFO_H_
