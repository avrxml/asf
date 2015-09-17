/**
 * \file
 *
 * \brief Unit test configuration
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
#ifndef CONF_TEST_H
#define CONF_TEST_H

/* ! \note USART0 on PORTD routed through board controller to USB (REMAP) */
#define CONF_TEST_USART      &USARTD0
/* ! \note Enable USARTD0 on PD6(Rx) and PD7(Tx) */
#define CONF_BOARD_ENABLE_USARTD0_REMAP
/* ! \note 38400 baud */
#define CONF_TEST_BAUDRATE   38400
/* ! \note 8-bit character length */
#define CONF_TEST_CHARLENGTH USART_CHSIZE_8BIT_gc
/* ! \note No parity check */
#define CONF_TEST_PARITY     USART_PMODE_DISABLED_gc
/* ! \note No extra stop bit, i.e., use 1 */
#define CONF_TEST_STOPBITS   false

/* ! \note Size of memory blocks to copy */
#define MEMORY_BLOCK_SIZE    300

/* ! \note Size of triggered/interrupt test destination data block */
#define DEST_BLOCK_TC45_SIZE   16

/* ! \note Timers used in EDMA fixed read/trigger/callback test */
#define TIMER45 TCC4

/* ! \note Period used in EDMA fixed read/trigger/callback test */
#define TIMER45_PERIOD 8192

/* ! \note Timer resolution */
#define TIMER45_RESOLUTION 32768

#endif
