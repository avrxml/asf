/**
 * @file serial_interface.h
 *
 * @brief This header defines prototypes and definitions for serial interface
 * functionalities
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

/*
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

#include <string.h>

/**
 * \defgroup group_mac_serial_if Serial Interface Module
 * This module is responsible for encoding and decoding the serial bytes to/from
 * Host into corresponding MAC APIs .
 * @{
 */

/**
 * A UART state that expects a \ref SOT to be received as the next character.
 */
#define UART_RX_STATE_SOT               (1)

/**
 * A UART state that expects the length to be received as the next character.
 */
#define UART_RX_STATE_LENGTH            (2)

/**
 * A UART state that expects the next data character to be received.
 */
#define UART_RX_STATE_DATA              (3)

/**
 * A UART state that expects a \ref EOT to be received as the next character.
 */
#define UART_RX_STATE_EOT               (4)

/* UART communication framing */

/** The start of transmission delimiter. */
#define SOT                             (1)

/** The end of transmission delimiter. */
#define EOT                             (4)

/* Protocol Identifier of the MAC */
#define MAC_PID                         (1)

/* MAC Protocol Identifier Length */
#define MAC_PID_LEN                     (1)

#define SIO_BUF_COUNT                   (5)

#define SIO_RX_BUF_SIZE                 (156)
#define SIO_TX_BUF_SIZE                 SIO_RX_BUF_SIZE

/**
 * \brief This function does the initialization of the Serial handler state
 * Machine.
 */
void serial_interface_init(void);

/**
 * @brief Function to handle the state machine serial data exchange.
 */
void serial_data_handler(void);

/* @} */
/* EOF */
