/**
 * \file
 *
 * \brief Unit test configuration
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
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
#ifndef CONF_TEST_H
#define CONF_TEST_H

/* Using USART on PORTE (PE2 & PE3) */
#define CONF_TEST_USART      &USARTE0
/* 38.4kbps */
#define CONF_TEST_BAUDRATE   38400
/* 8-bit character length */
#define CONF_TEST_CHARLENGTH USART_CHSIZE_8BIT_gc
/* No parity check */
#define CONF_TEST_PARITY     USART_PMODE_DISABLED_gc
/* No extra stopbit, i.e., use 1 */
#define CONF_TEST_STOPBITS   false

/* PORT used to connected Quadrature Encoder */
#define CONF_QDEC_PORT          &PORTA
/* Position of pin Phase 0 of Quadrature Encoder */
#define CONF_QDEC_PINS_BASE     1
/* Timer Counter used by QDec */
#define CONF_QDEC_TC            &TCC1
/* Timer Counter used by QDec for frequency computation */
#define CONF_QDEC_FREQ_TC       &TCC0
/* Timer Counter used by to simulate a quadrature encoder */
#define CONF_GENERATOR_QENC_TC  &TCE0

#endif
