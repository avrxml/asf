/**
 * \file
 *
 * \brief AVR CAN Task Configuration File
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
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
#define STDIO_USART                     (&AVR32_USART2)
#define STDIO_USART_RX_PIN              AVR32_USART2_RXD_0_1_PIN
#define STDIO_USART_RX_FUNCTION         AVR32_USART2_RXD_0_1_FUNCTION
#define STDIO_USART_TX_PIN              AVR32_USART2_TXD_0_1_PIN
#define STDIO_USART_TX_FUNCTION         AVR32_USART2_TXD_0_1_FUNCTION
#define STDIO_USART_BAUDRATE            57600
#define STDIO_USART_IRQ                 AVR32_USART2_IRQ
#define STDIO_USART_IRQ_LEVEL           AVR32_INTC_INT1
/*
 *
 */
#if defined (__ICCAVR32__)
can_msg_t msg_tx_sot =
{
    0x133,                    // Identifier
    0x1ff,                    // Mask
    0x0102030405060708LL,     // Data
};
#else
can_msg_t msg_tx_sot =
{
  {
    {
      .id = 0x133,                    // Identifier
      .id_mask  = 0x1ff,              // Mask
    },
  },
 .data.u64 = 0x0102030405060708LL,    // Data
};
#endif

can_mob_t pCANMOB_message0[10] = {
                                    {
                                      CAN_MOB_NOT_ALLOCATED,
                                      &msg_tx_sot,
                                      8,
                                      CAN_DATA_FRAME,
                                      CAN_STATUS_NOT_COMPLETED
                                    },
                               };

#if defined (__ICCAVR32__)
can_msg_t msg_rx_listening =
{
     0,                // Identifier
     0,                // Mask
     0x0LL,            // Data
};
#else
can_msg_t msg_rx_listening =
{
  {
    {
      .id = 0,                      // Identifier
      .id_mask  = 0,                // Mask
    },
  },
 .data.u64 = 0x0LL,                 // Data
};
#endif

can_mob_t pCANMOB_message2[1] = {
                                  {CAN_MOB_NOT_ALLOCATED,&msg_rx_listening,0,CAN_DATA_FRAME,CAN_STATUS_NOT_COMPLETED}
                                 };

const char CAN_Success[] = "\n\r\
=== OK: UC3C CAN Communication Success ===\n\r\
\n\r";

const char CAN_Error[] = "\n\r\
=== Error: UC3C CAN Communication Error ===\n\r\
\n\r";

const char CAN_WakeUpIRQ[] = "\n\r\
=== Error: UC3C CAN Wake Up IRQ Detected ===\n\r\
\n\r";

const char CAN_BusOFFIRQ[] = "\n\r\
=== Error: UC3C CAN Bus OFF IRQ Detected ===\n\r\
\n\r";

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
\r\n\
\n\r";

const char Demo_Sent_Data[] = "\n\r\
-0- TxCAN @ 307: 0x133(Std.), L=8, 01-02-03-04-05-06-07-08; \n\r\
\n\r";

const char Data_Sent_Start_or_Not[] = "\
===Start demo now? Press y or Y - start\n\r\
===                    Press n or N - Not start\
\n\r";

#endif // _CONF_CAN_EXAMPLE_H
