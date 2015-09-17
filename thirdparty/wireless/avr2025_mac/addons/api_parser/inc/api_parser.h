/**
 * @file api_parser.h
 *
 * @brief This header defines protoypes and definitions for api parser
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

/** \page api MAC API Parser
 *    - \ref group_mac_api_parser
 */

/**
 * \section twopapp Two Processor Approach
 * The Two Processor approach basically consists of two devices Host and
 * NCP(Network Co-Processor).The MAC Stack runs on the NCP
 * and the MAC application runs on the Host.The api-parser module is used by the
 * host which will convert the mac api's to necessary command bytes stream (\ref
 * group_mac_api_encoder)
 * and    send it  to the NCP. The NCP board will contain the image of serial-if
 * application (MAC Core Stack) and this will receive  necessary command bytes
 * encode it  to api's  and
 * process it and send back command bytes as response which will be decoded  in
 * the host by the api-parser  module(\ref group_mac_api_decoder).
 */

/**
 * \defgroup group_mac_api_parser API Parser Module
 * This module is responsible for encoding and decoding the serial bytes to/from
 * NCP into MAC APIs to provide seamless working for application similar to
 * stack
 * running in same processor or on NCP.This
 * module is used for \ref twopapp .
 * @{
 */

#include "return_val.h"
#include "mac_api.h"
#include "ieee_const.h"
#include "mac_msg_const.h"

#define MAX_BUFFER_LENGTH    160

#define RX_BUFFER_LENGTH    MAX_BUFFER_LENGTH
#define TX_BUFFER_LENGTH    MAX_BUFFER_LENGTH

#define SOT_POS          0x00
#define LEN_POS          0x01
#define PROTOCOL_ID_POS  0x02
#define CMD_POS          0x03

#define PROTOCOL_ID  0X01

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

extern uint8_t *rcv_frame_ptr;
extern uint8_t rcv_buffer[];

void process_rf_serial_byte(uint8_t byte);
void handle_rx_frame(void);

/* ! @} */
/* EOF */
