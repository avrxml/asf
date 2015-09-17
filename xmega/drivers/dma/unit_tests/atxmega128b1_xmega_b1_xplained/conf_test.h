/**
 * \file
 *
 * \brief Unit test configuration
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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

//! \note Using USART on PORTE (PE2 & PE3)
#define CONF_TEST_USART      &USARTE0
//! \note 38.4kbps
#define CONF_TEST_BAUDRATE   38400
//! \note 8-bit character length
#define CONF_TEST_CHARLENGTH USART_CHSIZE_8BIT_gc
//! \note No parity check
#define CONF_TEST_PARITY     USART_PMODE_DISABLED_gc
//! \note No extra stop bit, i.e., use 1
#define CONF_TEST_STOPBITS   false

//! \note Size of memory blocks to copy
#define MEMORY_BLOCK_SIZE    512

//! \note Size of triggered/interrupt test destination data block
#define DEST_BLOCK_TC_SIZE   16

//! \note Number of repeats for double buffer test
#define DOUBLE_BUFFER_REPEATS 16

//! \note Timers used in DMA fixed read/trigger/callback test
#define TIMER TCC0

//! \note Period used in DMA fixed read/trigger/callback test
#define TIMER_PERIOD 4096

//! \note Timer resolution
#define TIMER_RESOLUTION 8192

//! \note DMA channels available
enum dma_channels {
	DMA_CHANNEL_0,
	DMA_CHANNEL_1,
	DMA_CHANNEL_2,
	DMA_CHANNEL_3
};

#endif
