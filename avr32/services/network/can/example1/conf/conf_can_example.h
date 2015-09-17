/*****************************************************************************
 *
 * \file
 *
 * \brief CAN driver for AVR32 UC3.
 *
 * This file contains basic configuration for the AVR32 CAN driver.
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
 ******************************************************************************/
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
#define UART_USART                     (&AVR32_USART2)
#define UART_USART_RX_PIN              AVR32_USART2_RXD_0_1_PIN
#define UART_USART_RX_FUNCTION         AVR32_USART2_RXD_0_1_FUNCTION
#define UART_USART_TX_PIN              AVR32_USART2_TXD_0_1_PIN
#define UART_USART_TX_FUNCTION         AVR32_USART2_TXD_0_1_FUNCTION
#define UART_USART_BAUDRATE            57600
#define UART_USART_IRQ                 AVR32_USART2_IRQ
#define UART_USART_IRQ_LEVEL           AVR32_INTC_INT1
#define FPBA_HZ                         FOSC0

/*
 *	Mail Box Definition
 */
// -----------------------------------------------------------------
// CAN Message Definition: Tx Message
#if defined (__ICCAVR32__)
can_msg_t msg_tx_sot =
{
    0x130,                    // Identifier
    0x1ff,                    // Mask
    0x0102030405060708LL,     // Data
};
#else
can_msg_t msg_tx_sot =
{
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
                              &msg_tx_sot,	                	// Pointer on CAN Message
                              8,	                        	// Data length DLC
                              CAN_DATA_FRAME,                   // Request type : CAN_DATA_FRAME or CAN_REMOTE_FRAME
                              CAN_STATUS_NOT_COMPLETED          // Status: by default CAN_STATUS_NOT_COMPLETED
                          };

// -----------------------------------------------------------------
// CAN Message Definition: Rx Message
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

// MOB Message Definition: Tx Message
can_mob_t appli_rx_msg = {
                              CAN_MOB_NOT_ALLOCATED, 			// Handle: by default CAN_MOB_NOT_ALLOCATED
                              &msg_rx_listening,	   			// Pointer on CAN Message
                              8,		                		// Data length DLC
                              CAN_DATA_FRAME,        	        // Request type : CAN_DATA_FRAME or CAN_REMOTE_FRAME
                              CAN_STATUS_NOT_COMPLETED	        // Status: by default CAN_STATUS_NOT_COMPLETED
                         };

// -----------------------------------------------------------------
// CAN Message Definition: Rx Remote Message
#if defined (__ICCAVR32__)
can_msg_t msg_remote_rx =   {
				  0x110,                // Identifier
				  0x1ff,                // Mask
				  0x55AA00000000AA55    // Data Response
                            };
#else
can_msg_t msg_remote_rx =
{
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
can_mob_t appli_remote_rx_msg =  {
				    CAN_MOB_NOT_ALLOCATED,	    	// Handle: by default CAN_MOB_NOT_ALLOCATED
                                    &msg_remote_rx,	            	// Pointer on CAN Message
				    8,	                            // Data length DLC
				    CAN_REMOTE_FRAME,	            // Request type : CAN_DATA_FRAME or CAN_REMOTE_FRAME
				    CAN_STATUS_NOT_COMPLETED	    // Status: by default CAN_STATUS_NOT_COMPLETED
                                 };

// -----------------------------------------------------------------
const char CAN_Wakeup[] = "\n\r\
=== Switch into static Mode...Wait CAN communication... ===\n\r\
\n\r";

const char CAN_WakeupD[] = "\n\r\
  === OK: UC3C CAN Wake Up Detected  ===\n\r\
\n\r";

const char CAN_Success[] = "\n\r\
=== OK: UC3C CAN Communication Succeeded ===\n\r\
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
3 - Remote Reception Mode\r\n\
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
