/**
* \file otau_parser.h
*
* \brief OTAU Parser interface
*
* Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries.
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
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES,
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

#ifndef OTAU_PARSER_H
#define OTAU_PARSER_H

#include "otau.h"
#include "miqueue.h"
/**
 * A UART state that expects a \ref SOF to be received as the next character.
 */
#define UART_RX_STATE_SOF               (1)

/**
 * A UART state that expects a \ref FCF to be received as the next character.
 */
#define UART_RX_STATE_FCF               (2)

/**
 * A UART state that expects the length to be received as the next character.
 */
#define UART_RX_STATE_LENGTH_1          (3)

/**
 * A UART state that expects the length to be received as the next character.
 */
#define UART_RX_STATE_LENGTH_2          (4)

/**
 * A UART state that expects the next byte as Sequence number to be received.
 */
#define UART_RX_STATE_SEQ               (5)

/**
 * A UART state that expects the next byte as Stack identifier to be received.
 */
#define UART_RX_STATE_STACK_ID          (6)

/**
 * A UART state that expects the next byte as Stack identifier to be received.
 */
#define UART_RX_STATE_DOMAIN            (7)

/**
 * A UART state that expects the next data character to be received.
 */
#define UART_RX_STATE_PAYLOAD           (8)

/**
 * A UART state that expects a FCS to be received as the next character.
 */
#define UART_RX_STATE_FCS               (9)


/** The start of transmission delimiter. */
#define SOF                             (0x2A)

#define SIO_RX_BUF_SIZE                 (156)
#define SIO_TX_BUF_SIZE                 SIO_RX_BUF_SIZE

#define STACK_ID						(0x00)

#define SIO_FRAME_TYPE_MASK             (0xC0)

#define SIO_FRAME_TYPE_ACK              (0 << 6)

#define SIO_FRAME_TYPE_SREQ             (1 << 6)

#define SIO_FRAME_TYPE_AREQ_ARSP        (2 << 6)

#define SIO_FRAME_TYPE_SRSP             (3 << 6)

#define SIO_FRAME_CTRL_OVERFLOW         (1 << 1)

#define SIO_FRAME_CTRL_INVALID_FCS      (1 << 0)

#define SIO_FRAME_CTRL_VALID_FCS        (0 << 0)


extern MiQueue_t dataToTool;
extern MiQueue_t dataFromTool;

void otauParserInit(void);
void serialDataHandler(void);
void send_pc_data(otau_domain_t domainId, uint8_t msgId, uint8_t *msg, uint16_t len);
void send_server_data(otau_domain_t domainId, addr_mode_t addr_mode, uint8_t *addr, uint8_t msgId, uint8_t *msg, uint16_t len);
#endif /* OTAU_PARSER_H */
