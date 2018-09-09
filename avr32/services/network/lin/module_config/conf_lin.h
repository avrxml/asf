/**************************************************************************
 *
 * \file
 *
 * \brief Usart Lin Driver configuration file.
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


#ifndef _CONF_LIN_H_
#define _CONF_LIN_H_

#include <avr32/io.h>
#include "board.h"

#include "usart.h"

/*! \name LIN Configuration
 */
// Lin Version Supported LIN_13 or LIN_20
#define USART_LIN_VERSION                   LIN_2x

#if BOARD == EVK1101

//! Usart Used for LIN Support
#define USART_LIN_NODE0_INSTANCE                        AVR32_USART0

//! IRQ for the usart
#define USART_LIN_NODE0_USART_IRQ                       AVR32_USART0_IRQ

//! IRQ level for the usart
#define USART_LIN_NODE0_USART_IRQ_LEVEL                 AVR32_INTC_INT0

//! \name USART0 Hardware Connections
//! @{
#define USART_LIN_NODE0_RX_PIN                    AVR32_USART0_RXD_0_1_PIN
#define USART_LIN_NODE0_RX_FUNCTION               AVR32_USART0_RXD_0_1_FUNCTION
#define USART_LIN_NODE0_TX_PIN                    AVR32_USART0_TXD_0_1_PIN
#define USART_LIN_NODE0_TX_FUNCTION               AVR32_USART0_TXD_0_1_FUNCTION
//! @}

//! PDCA Channel Used for Transmission
#define USART_LIN_NODE0_TX_PDCA_CHANNEL           0

//! PDCA Channel IRQ Group
#define USART_LIN_NODE0_TX_PDCA_IRQ_GROUP         AVR32_PDCA_IRQ_GROUP

//! PDCA Channel Used for Reception
#define USART_LIN_NODE0_RX_PDCA_CHANNEL           1

//! PDCA Channel IRQ Group
#define USART_LIN_NODE0_RX_PDCA_IRQ_GROUP         AVR32_PDCA_IRQ_GROUP

//! IRQ of PDCA Channel Used for Transmission
#define USART_LIN_NODE0_PDCA_TX_IRQ               AVR32_PDCA_IRQ_0

//! IRQ level of PDCA Channel Used for Transmission
#define USART_LIN_NODE0_PDCA_TX_IRQ_LEVEL         AVR32_INTC_INT0

//! IRQ of PDCA Channel Used for Reception
#define USART_LIN_NODE0_PDCA_RX_IRQ               AVR32_PDCA_IRQ_1

//! IRQ level of PDCA Channel Used for Reception
#define USART_LIN_NODE0_PDCA_RX_IRQ_LEVEL         AVR32_INTC_INT0

//! PDCA PID For IRQ of Usart selected in transmission
#define USART_LIN_NODE0_PDCA_PID_TX               AVR32_PDCA_PID_USART0_TX

//! PDCA PID For IRQ of Usart selected in reception
#define USART_LIN_NODE0_PDCA_PID_RX               AVR32_PDCA_PID_USART0_RX


#elif BOARD == UC3C_EK

//! Usart Used for LIN Support
#define USART_LIN_NODE0_INSTANCE                        AVR32_USART1

//! IRQ for the usart
#define USART_LIN_NODE0_USART_IRQ                       AVR32_USART1_IRQ

//! IRQ level for the usart
#define USART_LIN_NODE0_USART_IRQ_LEVEL                 AVR32_INTC_INT0

//! \name USART1 Hardware Connections
//! @{
#define USART_LIN_NODE0_RX_PIN                    AVR32_USART1_RXD_0_0_PIN
#define USART_LIN_NODE0_RX_FUNCTION               AVR32_USART1_RXD_0_0_FUNCTION
#define USART_LIN_NODE0_TX_PIN                    AVR32_USART1_TXD_0_0_PIN
#define USART_LIN_NODE0_TX_FUNCTION               AVR32_USART1_TXD_0_0_FUNCTION
//! @}

//! PDCA Channel Used for Transmission
#define USART_LIN_NODE0_TX_PDCA_CHANNEL           0

//! PDCA Channel IRQ Group
#define USART_LIN_NODE0_TX_PDCA_IRQ_GROUP         AVR32_PDCA_IRQ_GROUP0

//! PDCA Channel Used for Reception
#define USART_LIN_NODE0_RX_PDCA_CHANNEL           1

//! PDCA Channel IRQ Group
#define USART_LIN_NODE0_RX_PDCA_IRQ_GROUP         AVR32_PDCA_IRQ_GROUP0

//! IRQ of PDCA Channel Used for Transmission
#define USART_LIN_NODE0_PDCA_TX_IRQ               AVR32_PDCA_IRQ_0

//! IRQ level of PDCA Channel Used for Transmission
#define USART_LIN_NODE0_PDCA_TX_IRQ_LEVEL         AVR32_INTC_INT0

//! IRQ of PDCA Channel Used for Reception
#define USART_LIN_NODE0_PDCA_RX_IRQ               AVR32_PDCA_IRQ_1

//! IRQ level of PDCA Channel Used for Reception
#define USART_LIN_NODE0_PDCA_RX_IRQ_LEVEL         AVR32_INTC_INT0

//! PDCA PID For IRQ of Usart selected in transmission
#define USART_LIN_NODE0_PDCA_PID_TX               AVR32_PDCA_PID_USART1_TX

//! PDCA PID For IRQ of Usart selected in reception
#define USART_LIN_NODE0_PDCA_PID_RX               AVR32_PDCA_PID_USART1_RX


//! Usart Used for LIN Support
#define USART_LIN_NODE1_INSTANCE                        AVR32_USART0

//! IRQ for the usart
#define USART_LIN_NODE1_USART_IRQ                       AVR32_USART0_IRQ

//! IRQ level for the usart
#define USART_LIN_NODE1_USART_IRQ_LEVEL                 AVR32_INTC_INT0

//! \name USART0 Hardware Connections
//! @{
#define USART_LIN_NODE1_RX_PIN        	          AVR32_USART0_RXD_0_1_PIN
#define USART_LIN_NODE1_RX_FUNCTION   	          AVR32_USART0_RXD_0_1_FUNCTION
#define USART_LIN_NODE1_TX_PIN        	          AVR32_USART0_TXD_0_1_PIN
#define USART_LIN_NODE1_TX_FUNCTION   	          AVR32_USART0_TXD_0_1_FUNCTION
//! @}

//! PDCA Channel Used for Transmission
#define USART_LIN_NODE1_TX_PDCA_CHANNEL           2

//! PDCA Channel IRQ Group
#define USART_LIN_NODE1_TX_PDCA_IRQ_GROUP         AVR32_PDCA_IRQ_GROUP0

//! PDCA Channel Used for Reception
#define USART_LIN_NODE1_RX_PDCA_CHANNEL           3

//! PDCA Channel IRQ Group
#define USART_LIN_NODE1_RX_PDCA_IRQ_GROUP         AVR32_PDCA_IRQ_GROUP0

//! IRQ of PDCA Channel Used for Transmission
#define USART_LIN_NODE1_PDCA_TX_IRQ               AVR32_PDCA_IRQ_2

//! IRQ level of PDCA Channel Used for Transmission
#define USART_LIN_NODE1_PDCA_TX_IRQ_LEVEL         AVR32_INTC_INT0

//! IRQ of PDCA Channel Used for Reception
#define USART_LIN_NODE1_PDCA_RX_IRQ               AVR32_PDCA_IRQ_3

//! IRQ level of PDCA Channel Used for Reception
#define USART_LIN_NODE1_PDCA_RX_IRQ_LEVEL         AVR32_INTC_INT0

//! PDCA PID For IRQ of Usart selected in transmission
#define USART_LIN_NODE1_PDCA_PID_TX               AVR32_PDCA_PID_USART0_TX

//! PDCA PID For IRQ of Usart selected in reception
#define USART_LIN_NODE1_PDCA_PID_RX               AVR32_PDCA_PID_USART0_RX

#else

#warning 'Default Board Configuration. It should be customized for the final application.'

//! Usart Used for LIN Support
#define USART_LIN_NODE0_INSTANCE                        AVR32_USART0

//! IRQ for the usart
#define USART_LIN_NODE0_USART_IRQ                       AVR32_USART0_IRQ

//! IRQ level for the usart
#define USART_LIN_NODE0_USART_IRQ_LEVEL                 AVR32_INTC_INT0

//! \name USART0 Hardware Connections
//! @{
#define USART_LIN_NODE0_RX_PIN                    AVR32_USART0_RXD_0_1_PIN
#define USART_LIN_NODE0_RX_FUNCTION               AVR32_USART0_RXD_0_1_FUNCTION
#define USART_LIN_NODE0_TX_PIN                    AVR32_USART0_TXD_0_1_PIN
#define USART_LIN_NODE0_TX_FUNCTION               AVR32_USART0_TXD_0_1_FUNCTION
//! @}

//! PDCA Channel Used for Transmission
#define USART_LIN_NODE0_TX_PDCA_CHANNEL           0

//! PDCA Channel IRQ Group
#define USART_LIN_NODE0_TX_PDCA_IRQ_GROUP         AVR32_PDCA_IRQ_GROUP

//! PDCA Channel Used for Reception
#define USART_LIN_NODE0_RX_PDCA_CHANNEL           1

//! PDCA Channel IRQ Group
#define USART_LIN_NODE0_RX_PDCA_IRQ_GROUP         AVR32_PDCA_IRQ_GROUP

//! IRQ of PDCA Channel Used for Transmission
#define USART_LIN_NODE0_PDCA_TX_IRQ               AVR32_PDCA_IRQ_0

//! IRQ level of PDCA Channel Used for Transmission
#define USART_LIN_NODE0_PDCA_TX_IRQ_LEVEL         AVR32_INTC_INT0

//! IRQ of PDCA Channel Used for Reception
#define USART_LIN_NODE0_PDCA_RX_IRQ               AVR32_PDCA_IRQ_1

//! IRQ level of PDCA Channel Used for Reception
#define USART_LIN_NODE0_PDCA_RX_IRQ_LEVEL         AVR32_INTC_INT0

//! PDCA PID For IRQ of Usart selected in transmission
#define USART_LIN_NODE0_PDCA_PID_TX               AVR32_PDCA_PID_USART0_TX

//! PDCA PID For IRQ of Usart selected in reception
#define USART_LIN_NODE0_PDCA_PID_RX               AVR32_PDCA_PID_USART0_RX

#endif
#endif  // _CONF_USART_LIN_H_
