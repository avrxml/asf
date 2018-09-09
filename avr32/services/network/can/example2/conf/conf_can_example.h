/*****************************************************************************
 *
 * \file
 *
 * \brief CAN driver for AVR32 UC3.
 *
 * This file contains basic functions for the AVR32 CAN, with support for all
 * modes, settings and clock speeds.
 *
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */



#ifndef _CONF_CAN_EXAMPLE_H
#define _CONF_CAN_EXAMPLE_H

//_____ I N C L U D E S ________________________________________________________
#include "avr32/io.h"
#include "compiler.h"
#include "can.h"
#include "board.h"

/*
 *
 */
#if BOARD == UC3C_EK
#  define UART_USART                     (&AVR32_USART2)
#  define UART_USART_RX_PIN              AVR32_USART2_RXD_0_1_PIN
#  define UART_USART_RX_FUNCTION         AVR32_USART2_RXD_0_1_FUNCTION
#  define UART_USART_TX_PIN              AVR32_USART2_TXD_0_1_PIN
#  define UART_USART_TX_FUNCTION         AVR32_USART2_TXD_0_1_FUNCTION
#  define UART_USART_BAUDRATE            57600
#  define UART_USART_IRQ                 AVR32_USART2_IRQ
#  define UART_USART_IRQ_LEVEL           AVR32_INTC_INT1
#  define FPBA_HZ                        FOSC0
#else
#  error 'Board not supported by the example!'
#endif
/*
 * Mail Box Definition
 */
// -----------------------------------------------------------------
// CAN Message Definition: Tx Message
#if defined (__ICCAVR32__)
can_msg_t msg_tx_sot = {
  0x130,                    // Identifier
  0x1ff,                    // Mask
  0x0102030405060708LL,     // Data
};
#else
can_msg_t msg_tx_sot = {
  {
    {
      .id = 0x130,                    // Identifier
      .id_mask  = 0x1ff,              // Mask
    },
  },
 .data.u64 = 0x0102030405060708LL,    // Data
};
#endif

// MOB Message Definition: Tx Message
can_mob_t appli_tx_msg = {
  CAN_MOB_NOT_ALLOCATED,            // Handle: by default CAN_MOB_NOT_ALLOCATED
  &msg_tx_sot,                      // Pointer on CAN Message
  8,                                // Data length DLC
  CAN_DATA_FRAME,                   // Request type : CAN_DATA_FRAME or CAN_REMOTE_FRAME
  CAN_STATUS_NOT_COMPLETED          // Status: by default CAN_STATUS_NOT_COMPLETED
};

// -----------------------------------------------------------------
// CAN Message Definition: Rx Message
#if defined (__ICCAVR32__)
can_msg_t msg_rx_listening = {
  0,                // Identifier
  0,                // Mask
  0x0LL,            // Data
};
#else
can_msg_t msg_rx_listening = {
  {
    {
      .id = 0,                      // Identifier
      .id_mask  = 0,                // Mask
    },
  },
 .data.u64 = 0x0LL,                 // Data
};
#endif

// MOB Message Definition: Tx Message
can_mob_t appli_rx_msg = {
  CAN_MOB_NOT_ALLOCATED,            // Handle: by default CAN_MOB_NOT_ALLOCATED
  &msg_rx_listening,                // Pointer on CAN Message
  8,                                // Data length DLC
  CAN_DATA_FRAME,                   // Request type : CAN_DATA_FRAME or CAN_REMOTE_FRAME
  CAN_STATUS_NOT_COMPLETED          // Status: by default CAN_STATUS_NOT_COMPLETED
};

// -----------------------------------------------------------------
// CAN Message Definition: Rx Remote Message
#if defined (__ICCAVR32__)
can_msg_t msg_remote_rx = {
  0x110,                // Identifier
  0x1ff,                // Mask
  0x55AA00000000AA55    // Data Response
};
#else
can_msg_t msg_remote_rx = {
  {
    {
      .id = 0x110,                      // Identifier
      .id_mask  = 0x1ff,                // Mask
    },
  },
 .data.u64 = 0x55AA00000000AA55,        // Data Response
};
#endif

// MOB Message Definition: Rx Remote Message
can_mob_t appli_remote_rx_msg = {
  CAN_MOB_NOT_ALLOCATED,          // Handle: by default CAN_MOB_NOT_ALLOCATED
  &msg_remote_rx,                 // Pointer on CAN Message
  8,                              // Data length DLC
  CAN_REMOTE_FRAME,               // Request type : CAN_DATA_FRAME or CAN_REMOTE_FRAME
  CAN_STATUS_NOT_COMPLETED        // Status: by default CAN_STATUS_NOT_COMPLETED
};

// -----------------------------------------------------------------
const char CAN_Success[] = "\r\n\
=== OK: UC3C CAN Communication Succeeded ===\r\n\
\r\n";

const char CAN_Error[] = "\r\n\
=== Error: UC3C CAN Communication Error ===\r\n\
\r\n";

const char CAN_WakeUpIRQ[] = "\r\n\
=== Error: UC3C CAN Wake Up IRQ Detected ===\r\n\
\r\n";

const char CAN_BusOFFIRQ[] = "\r\n\
=== Error: UC3C CAN Bus OFF IRQ Detected ===\r\n\
\r\n";

const char txt_logo_atmel[]="\r\n\
        _____________________________\r\n\
      /___   ________________________|\r\n\
     ___  | |  ________    ____   _\r\n\
    /   | | | |  _   _ \\  |  __| | |\r\n\
   / /| | | | | | | | | | | |__  | |\r\n\
  / / | | | | | | | | | | |  __| | |\r\n\
 / /_ | | | | | | | | | | | |__  | |__\r\n\
/____||_| |_| |_| |_| |_| |____| |___/\r\n\
,-----------------------------------,\r\n\
1 - Transmission Mode\r\n\
2 - Reception Mode\r\n\
3 - Remote Reception Mode\r\n\
\r\n\
\r\n";

const char Demo_Sent_Data[] = "\r\n\
-0- TxCAN @ 307: 0x133(Std.), L=8, 01-02-03-04-05-06-07-08; \r\n\
\r\n";

const char Data_Sent_Start_or_Not[] = "\
===Start demo now? Press y or Y - start\r\n\
===                    Press n or N - Not start\
\r\n";

#endif // _CONF_CAN_EXAMPLE_H
