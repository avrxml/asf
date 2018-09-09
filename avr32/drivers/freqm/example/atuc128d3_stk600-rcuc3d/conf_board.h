/**
 * \file
 *
 * \brief AVR UC3 Frequency Meter (FREQM) driver
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
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef _CONF_BOARD_H_
#define _CONF_BOARD_H_

/**
 * \ingroup freqm_group
 * \defgroup usart_setting USART Settings
 *
 * @{
 */

// For the OSC32, connect STK600.PORTB.PB37 to STK600.AUX.32KHz

# define EXAMPLE_USART               (&AVR32_USART1)
// For the RX pin, connect STK600.PORTD.PD0 to STK600.RS232 SPARE.RXD
# define EXAMPLE_USART_RX_PIN        AVR32_USART1_RXD_0_0_PIN
# define EXAMPLE_USART_RX_FUNCTION   AVR32_USART1_RXD_0_0_FUNCTION
// For the TX pin, connect STK600.PORTC.PC7 to STK600.RS232 SPARE.TXD
# define EXAMPLE_USART_TX_PIN        AVR32_USART1_TXD_0_0_PIN
# define EXAMPLE_USART_TX_FUNCTION   AVR32_USART1_TXD_0_0_FUNCTION
# define FPBA                        FOSC0

//! @}

// Osc0 frequency
#define BOARD_OSC0_HZ                12000000
// 2048 cycles (18 ms)
#define BOARD_OSC0_STARTUP_US        18000
// Crystal connected to XIN/XOUT
#define BOARD_OSC0_IS_XTAL           true

#endif // _CONF_BOARD_H_

