/**
 * \file
 *
 * \brief Example configuration
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
