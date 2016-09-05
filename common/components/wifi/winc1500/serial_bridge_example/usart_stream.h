/**
 *
 * \file
 *
 * \brief Serial Bridge.
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

#ifndef USART_STREAM_H_INCLUDED
#define USART_STREAM_H_INCLUDED

#include <inttypes.h>
#include <asf.h>

#define USART_BUFFER_MAX (2 * 1024)

/**
 * \brief Initialize USART module.
 *
 * \param baud_rate Baudrate of USART.
 */
void configure_usart(uint32_t baud_rate);

/**
 * \brief Clear USART buffer and flush USART module.
 */
void usart_stream_reset(void);

/**
 * \brief Write one byte of data.
 *
 * \param data Data to send.
 */
void usart_stream_write(uint8_t data);

/**
 * \brief Write multiple bytes of data.
 *
 * \param data Pointer to data to send.
 * \param size Size of data.
 */
void usart_stream_write_buffer(uint8_t *data, uint32_t size);

/**
 * \brief Read data from USART module.
 *
 * \param data Pointer to USART buffer.
 * \param data Pointer to USART buffer size.
 */
int usart_stream_read(uint8_t **data, uint32_t *size);

/**
 * \brief Move back USART buffer.
 *
 * \param offset Size of data to remove.
 */
void usart_stream_move(uint32_t offset);

#endif /* USART_STREAM_H_INCLUDED */
