/**************************************************************************
 *
 * \file
 *
 * \brief Header file of the USB AUDIO example.
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


#ifndef _AUDIO_EXAMPLE_H_
#define _AUDIO_EXAMPLE_H_

#include "audio_mixer.h"
#include "conf_audio_mixer.h"

#include <avr32/io.h>
#include "board.h"


//_____ I N C L U D E S ____________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

#define AUDIO_DEMO_STRING "USB Audio Host demo v1.0.1"



/*! \name Speaker sampling rate frequency.
 * Please choose between:
 * - 44100    In KHz. The OSC1 frequency must be 11289600 Hz.
 * - 48000    In KHz. The OSC1 frequency must be 12288000 Hz.
 */
#define SPEAKER_FREQUENCY  44100

#define USB_SPEAKER_VOL_MIN 0x0
#define USB_SPEAKER_VOL_MAX 0x1000
#define USB_SPEAKER_VOL_RES 0x50


#define MICRO_VOL_MIN      0x7000
#define MICRO_VOL_MAX      0x7000
#define MICRO_VOL_RES      0x1000

//! Default foreground color
#define APPLI_FG_COLOR             WHITE

//! Default background color
#define APPLI_BG_COLOR             -1

/*! \name USART Used for STDIO
 */
//! @{
#define STDIO_USART                   (&AVR32_USART0)
#define STDIO_USART_RX_PIN            AVR32_USART0_RXD_0_0_PIN
#define STDIO_USART_RX_FUNCTION       AVR32_USART0_RXD_0_0_FUNCTION
#define STDIO_USART_TX_PIN            AVR32_USART0_TXD_0_0_PIN
#define STDIO_USART_TX_FUNCTION       AVR32_USART0_TXD_0_0_FUNCTION
#define STDIO_USART_IRQ               AVR32_USART0_IRQ
#define STDIO_USART_BAUDRATE          57600
//! @}


//_____ M A C R O S ________________________________________________________


//_____ D E C L A R A T I O N S ____________________________________________


#endif  // _AUDIO_EXAMPLE_H_
