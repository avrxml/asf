/**
 * \file sio2host.h
 *
 * \brief Event handling Serial I/O  Functionalities
 *
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
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
