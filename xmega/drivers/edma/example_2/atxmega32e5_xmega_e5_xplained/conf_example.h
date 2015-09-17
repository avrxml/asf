/**
 * \file
 *
 * \brief Example configuration
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

/**
 * \ingroup edma_group
 * \defgroup edma_driver_example_configuration EDMA Driver Example 2
 *           Configuration
 *
 * @{
 */

/** \brief EDMA channel configuration mode */
#define EDMA_CHMODE      EDMA_CHMODE_PER0123_gc
/** \brief EDMA channel priority mode */
#define EDMA_PRIMODE     EDMA_PRIMODE_CH0123_gc
/** \brief EDMA double buffer mode */
#define EDMA_DBUFMODE    EDMA_DBUFMODE_BUF0123_gc

/** \brief EDMA channel for Tx USART */
#define EDMA_TX_CHANNEL     EDMA_CH_3
/** \brief EDMA channel for match on USART */
#define EDMA_MATCH_CHANNEL  EDMA_CH_1
/** \brief EDMA channel for Rx USART */
#define EDMA_RX_CHANNEL     EDMA_CH_0

/** \brief Size of EDMA source buffer (from ' ' to '~')*/
#define EDMA_SRC_BUFFER_SIZE    ((size_t)('~' - (' ' - 1)))
/** \brief Size of EDMA destination buffer (expected from 'A' to 'Z') */
#define EDMA_DEST_BUFFER_SIZE   ((size_t)('Z' - ('A' - 1)))
/** \brief Pattern (or character) to match */
#define EDMA_MATCH_PATTERN      '@'

/**  \brief USART to use */
#define USART_SERIAL                &USARTD0
/**  \brief USART baudrate */
#define USART_SERIAL_BAUDRATE       38400
/**  \brief USART character length */
#define USART_SERIAL_CHAR_LENGTH    USART_CHSIZE_8BIT_gc
/**  \brief USART parity */
#define USART_SERIAL_PARITY         USART_PMODE_DISABLED_gc
/**  \brief USART stop bit length */
#define USART_SERIAL_STOP_BIT       false
/**  \brief USART start of frame detection */
#define USART_SERIAL_SOF_DETECTION  false
/**  \brief USART one-wire mode */
#define USART_SERIAL_ONE_WIRE       true
/**  \brief USART/XCL peripheral counter length */
#define USART_SERIAL_PEC_LENGTH     NULL
/**  \brief USART/XCL peripheral counter action */
#define USART_SERIAL_PEC_ACTION     USART_PECACT_OFF_gc
/**  \brief USART/XCL encoding type */
#define USART_SERIAL_ENCODING_TYPE  USART_DECTYPE_DATA_gc
/**  \brief USART/XCL encoding stream */
#define USART_SERIAL_ENCODING_STREAM  USART_LUTACT_OFF_gc

#endif

/** @} */

/* CONF_EXAMPLE_H */
