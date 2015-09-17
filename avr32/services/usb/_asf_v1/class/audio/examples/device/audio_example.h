/**************************************************************************
 *
 * \file
 *
 * \brief Header file of the USB AUDIO example.
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
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#ifndef _AUDIO_EXAMPLE_H_
#define _AUDIO_EXAMPLE_H_

#include "audio_mixer.h"
#include "conf_audio_mixer.h"

#include <avr32/io.h>
#include "board.h"


//_____ I N C L U D E S ____________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

#define AUDIO_DEMO_STRING "USB Audio demo v1.0.14"



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
