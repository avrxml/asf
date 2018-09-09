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

#define SYS_CLK_SRC                     MACSC_16MHz
#define RTC_CLK_SRC                                     MACSC_32KHz

#define ENABLE_SLEEP                            1
#define ENABLE_AUTO_TIMESTAMP                   0

#define COMPARE_MODE                            MACSC_RELATIVE_CMP

/*
 * Beacon Interval,BI = aBaseSuperframeDuration*(2^BO) (Symbols) .
 * For example, given aBaseSuperframeDuration of 960 symbols and macBeaconOrder
 * of 5,
 * the beacon interval will be 3840 symbols.
 * One symbol duration = 16 us for 2.4 GHz O-QPSK modulation scheme
 */
#define BEACON_INTERVAL                         62500

/*
 * Active Period duration(also called Superframe Duration),SD =
 * aBaseSuperframeDuration*(2^SO) (Symbols).
 * SO is always less than or equal to BO.
 * For example, given aBaseSuperframeDuration of 960 symbols and
 * macSuperframeOrder of 2,
 * the beacon interval will be 3840 symbols.
 * One symbol duration = 16 us for 2.4 GHz O-QPSK modulation scheme
 */
#define ACTIVE_PERIOD                           3840

/*
 * Wake-up a few symbol periods(3) earlier so that the next incoming beacon is
 * not missed.
 */
#define SLEEP_PERIOD                            (BEACON_INTERVAL - \
	ACTIVE_PERIOD - 3)

/*
 * These pins(output) can be probed to check the timing duration of the
 * functionalities of the MACSC. 
 */
enum macsc_cb_pins {
	CMP2_INT_CHK_PIN    = IOPORT_CREATE_PIN(PORTB,4),     /*!< cmp2 int toggle */
	CMP3_INT_CHK_PIN    = IOPORT_CREATE_PIN(PORTB,3),    /*!< cmp3 int toggle */	
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
