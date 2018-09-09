/**
 * \file
 *
 * \brief Unit test configuration
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
#ifndef CONF_TEST_H
#define CONF_TEST_H

//! \note Using USART on PORTC (PC2 & PC3)
#define CONF_TEST_USART      &USARTC0
//! \note 38400 baud
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
