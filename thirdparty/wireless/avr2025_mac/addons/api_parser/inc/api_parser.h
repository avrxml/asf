/**
 * @file api_parser.h
 *
 * @brief This header defines protoypes and definitions for api parser
 * functionalities
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

/*
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
