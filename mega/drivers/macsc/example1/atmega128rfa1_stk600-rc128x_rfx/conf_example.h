/**
 * \file
 *
 * \brief MACSC Driver configuration
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
#ifndef CONF_EXAMPLE_H
#define CONF_EXAMPLE_H

#include "ioport.h"

#define SYS_CLK_SRC                        MACSC_16MHz
#define RTC_CLK_SRC                                        MACSC_32KHz

#define ENABLE_SLEEP                               0
#define ENABLE_AUTO_TIMESTAMP              0
#define ENABLE_BACKOFF_SLOT_CNTR           1

#define COMPARE_MODE                               MACSC_ABSOLUTE_CMP

/* We request a tick of 1Hz */
#define CONFIG_MACSC_TIMEOUT_TICK_HZ   62500

/*
 * These pins(output) can be probed to check the timing duration of the
 * functionalities of the MACSC. 
 */
enum macsc_cb_pins {
	OVF_INT_CHK_PIN     = IOPORT_CREATE_PIN(PORTB,0),       /*!< overflow interrupt toggle  */
	CMP1_INT_CHK_PIN    = IOPORT_CREATE_PIN(PORTB,1),      /*!< cmp1 int toggle */
	CMP2_INT_CHK_PIN    = IOPORT_CREATE_PIN(PORTB,2),     /*!< cmp2 int toggle */
	CMP3_INT_CHK_PIN    = IOPORT_CREATE_PIN(PORTB,3),    /*!< cmp3 int toggle */
	BACKOFF_INT_CHK_PIN = IOPORT_CREATE_PIN(PORTB,4),    /*!< back-off int toggle */
};

/*
 * For USARTA1 connect a two wire cable for PORTD2 to connect with RXD
 * For USARTA1 connect a two wire cable for PORTD3 to connect with TXD
 */
#define USART_SERIAL_PORT            &USARTA1

#define USART_SERIAL_BAUDRATE   9600
#define USART_SERIAL_CHAR_LENGTH        USART_CHSIZE_8BIT_gc
#define USART_SERIAL_PARITY             USART_PMODE_DISABLED_gc
#define USART_SERIAL_STOP_BIT           false

#endif /* CONF_EXAMPLE_H */
