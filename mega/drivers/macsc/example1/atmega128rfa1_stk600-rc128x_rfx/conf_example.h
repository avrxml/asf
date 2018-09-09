/**
 * \file
 *
 * \brief MACSC Driver configuration
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
