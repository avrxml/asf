/**
 * \file
 *
 * \brief AVR UC3 LIN Driver Configuration File
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

#ifndef _CONF_LIN_H_
#define _CONF_LIN_H_

#include <avr32/io.h>
#include "board.h"

#include "usart.h"

/*! \name LIN Configuration
 */
// Lin Version Supported LIN_13 or LIN_20
#define USART_LIN_VERSION                         LIN_2x


// Usart Used for LIN Support
#define USART_LIN_NODE0_INSTANCE                  AVR32_USART1
// IRQ for the usart
#define USART_LIN_NODE0_USART_IRQ                 AVR32_USART1_IRQ
// IRQ level for the usart
#define USART_LIN_NODE0_USART_IRQ_LEVEL           AVR32_INTC_INT0
// Pins and Functions used for USART1
#define USART_LIN_NODE0_RX_PIN                    AVR32_USART1_RXD_0_0_PIN
#define USART_LIN_NODE0_RX_FUNCTION               AVR32_USART1_RXD_0_0_FUNCTION
#define USART_LIN_NODE0_TX_PIN                    AVR32_USART1_TXD_0_0_PIN
#define USART_LIN_NODE0_TX_FUNCTION               AVR32_USART1_TXD_0_0_FUNCTION

// PDCA Channel Used for Transmission
#define USART_LIN_NODE0_TX_PDCA_CHANNEL           0
//! PDCA Channel IRQ Group
#define USART_LIN_NODE0_TX_PDCA_IRQ_GROUP         AVR32_PDCA_IRQ_GROUP0
// PDCA Channel Used for Reception
#define USART_LIN_NODE0_RX_PDCA_CHANNEL           1
//! PDCA Channel IRQ Group
#define USART_LIN_NODE0_RX_PDCA_IRQ_GROUP         AVR32_PDCA_IRQ_GROUP0
//  IRQ of PDCA Channel Used for Transmission
#define USART_LIN_NODE0_PDCA_TX_IRQ               AVR32_PDCA_IRQ_0
//  IRQ level of PDCA Channel Used for Transmission
#define USART_LIN_NODE0_PDCA_TX_IRQ_LEVEL         AVR32_INTC_INT0
// IRQ of PDCA Channel Used for Reception
#define USART_LIN_NODE0_PDCA_RX_IRQ               AVR32_PDCA_IRQ_1
//  IRQ level of PDCA Channel Used for Reception
#define USART_LIN_NODE0_PDCA_RX_IRQ_LEVEL         AVR32_INTC_INT0
// PDCA PID For IRQ of Usart selected in transmission
#define USART_LIN_NODE0_PDCA_PID_TX               AVR32_PDCA_PID_USART1_TX
// PDCA PID For IRQ of Usart selected in reception
#define USART_LIN_NODE0_PDCA_PID_RX               AVR32_PDCA_PID_USART1_RX

// Usart Used for LIN Support
#define USART_LIN_NODE1_INSTANCE                  AVR32_USART0
// IRQ for the usart
#define USART_LIN_NODE1_USART_IRQ                 AVR32_USART0_IRQ
// IRQ level for the usart
#define USART_LIN_NODE1_USART_IRQ_LEVEL           AVR32_INTC_INT0
// Pins and Functions used for USART0
#define USART_LIN_NODE1_RX_PIN                    AVR32_USART0_RXD_0_1_PIN
#define USART_LIN_NODE1_RX_FUNCTION               AVR32_USART0_RXD_0_1_FUNCTION
#define USART_LIN_NODE1_TX_PIN                    AVR32_USART0_TXD_0_1_PIN
#define USART_LIN_NODE1_TX_FUNCTION               AVR32_USART0_TXD_0_1_FUNCTION
// PDCA Channel Used for Transmission
#define USART_LIN_NODE1_TX_PDCA_CHANNEL           2
//! PDCA Channel IRQ Group
#define USART_LIN_NODE1_TX_PDCA_IRQ_GROUP         AVR32_PDCA_IRQ_GROUP0
// PDCA Channel Used for Reception
#define USART_LIN_NODE1_RX_PDCA_CHANNEL           3
//! PDCA Channel IRQ Group
#define USART_LIN_NODE1_RX_PDCA_IRQ_GROUP         AVR32_PDCA_IRQ_GROUP0
//  IRQ of PDCA Channel Used for Transmission
#define USART_LIN_NODE1_PDCA_TX_IRQ               AVR32_PDCA_IRQ_2
//  IRQ level of PDCA Channel Used for Transmission
#define USART_LIN_NODE1_PDCA_TX_IRQ_LEVEL         AVR32_INTC_INT0
// IRQ of PDCA Channel Used for Reception
#define USART_LIN_NODE1_PDCA_RX_IRQ               AVR32_PDCA_IRQ_3
//  IRQ level of PDCA Channel Used for Reception
#define USART_LIN_NODE1_PDCA_RX_IRQ_LEVEL         AVR32_INTC_INT0
// PDCA PID For IRQ of Usart selected in transmission
#define USART_LIN_NODE1_PDCA_PID_TX               AVR32_PDCA_PID_USART0_TX
// PDCA PID For IRQ of Usart selected in reception
#define USART_LIN_NODE1_PDCA_PID_RX               AVR32_PDCA_PID_USART0_RX

#endif  // _CONF_USART_LIN_H_
