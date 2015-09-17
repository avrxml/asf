/**
 * \file
 *
 * \brief Board specific spi as slave example configuration file
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

#ifndef _CONF_SPI_SLAVE_EXAMPLE_H
#define _CONF_SPI_SLAVE_EXAMPLE_H

/**
 * \defgroup spi_slave_example_pin_defs
 *  - <b> ATMEGA128RFA1-EK-STK600 (USART)  -- ATMEGA128RFA1-EK-STK600 (SPI) </b>
 *   - PB4      -- SS(PB0)
 *   - XCK(PE2) -- SCK(PB1)
 *   - RXD(PE0) -- MISO(PB3)
 *   - TXD(PE1) -- MOSI(PB2)
 */

/* SPI configured in slave mode */
#define SPI_SLAVE_EXAMPLE             &SPCR
#define SPI_SLAVE_EXAMPLE_LED_PIN     LED_YELLOW_GPIO

/* USARTA0 configured in SPI master mode */
#define USART_SPI_EXAMPLE                   &USARTA0
#define USART_SPI_DEVICE_EXAMPLE_ID         IOPORT_CREATE_PIN(PORTB, 4)
#define USART_SPI_EXAMPLE_BAUDRATE          1000000

#endif /* _CONF_SPI_SLAVE_EXAMPLE_H */
