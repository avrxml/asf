/**
 * \file sio2host.h
 *
 * \brief Event handling Serial I/O  Functionalities
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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
 */

#ifndef SIO2HOST_H
#define SIO2HOST_H

/**
 * \defgroup group_sio2host_usb SIO2HOST - USB
 * This module performs serial input/output functionalities via USB
 * @{
 */

/* === INCLUDES ============================================================ */
#include "compiler.h"
#include "conf_board.h"

#ifdef SENSOR_TERMINAL_BOARD

/*
 * USB transmit buffer size
 */
#define USB_TX_BUF_SIZE             (100)

/*
 * USB receive buffer size
 */
#define USB_RX_BUF_SIZE             (100)

/* Max bytes transmitted over USB at one instance;
 * Used for FTDI only; FTDI's tx buffer size is 384 bytes,
 * but an uint8_t variable is used. Therefore, the value is reduced to 255.
 */
#define MAX_BYTES_FOR_USB_TX    (255)

/* === PROTOTYPES ============================================================
**/

/**
 * \brief Initializes the Serial IO Module of the Host Device
 * \return STATUS_OK for successful initialization and FAILURE incase the IO is
 * not initialized
 */
void sio2host_init(void);

/**
 * \brief Transmits data via USB
 * \param data Pointer to the buffer where the data to be transmitted is present
 * \param length Number of bytes to be transmitted
 *
 * \return Number of bytes actually transmitted
 */

uint8_t sio2host_tx(uint8_t *data, uint8_t length);

/**
 * \brief Receives data from USB
 *
 * \param data pointer to the buffer where the received data is to be stored
 * \param max_length maximum length of data to be received
 *
 * \return actual number of bytes received
 */
uint8_t sio2host_rx(uint8_t *data, uint8_t max_length);

/**
 * \brief This function performs a non-blocking character receive functionality
 * \return '-1' if no data is recieved or returns the data if a character is
 * received
 */

int sio2host_getchar_nowait(void);

/**
 * \brief This function performs a blocking character receive functionality
 * \return returns the data which is received
 */
uint8_t sio2host_getchar(void);

/**
 * \brief Services data transmission or reception on USB
 *
 * This function polls for usb for completion of reception or transmission of
 * a byte on USB.
 */
void usb_handler(void);

#endif /* #ifdef SENSOR_TERMINAL_BOARD */
/* ! @} */
#endif /* SIO2HOST_H */
