/**
 * \file
 *
 * \brief PPLC interface Configuration.
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

#ifndef CONF_PPLC_IF_H_INCLUDE
#define CONF_PPLC_IF_H_INCLUDE

#include "board.h"

/* Select the SPI module that PPLC is connected to */
#define PPLC_SPI_MODULE     SPI0

/* Chip select used by PPLC internal peripheral  */
#define PPLC_CS             0

/* Interruption pin used by PPLC internal peripheral */
#define PPLC_INT_GPIO   (PIO_PA12_IDX)
#define PPLC_INT_FLAGS (IOPORT_MODE_DEBOUNCE)
#define PPLC_INT_SENSE (IOPORT_SENSE_FALLING)

#define PPLC_INT       {PIO_PA12, PIOA, ID_PIOA, PIO_INPUT, \
			PIO_DEBOUNCE | PIO_IT_FALL_EDGE}
#define PPLC_INT_MASK  PIO_PA12
#define PPLC_INT_PIO   PIOA
#define PPLC_INT_ID    ID_PIOA
#define PPLC_INT_TYPE  PIO_INPUT
#define PPLC_INT_ATTR  (PIO_DEBOUNCE | PIO_IT_FALL_EDGE)
#define PPLC_INT_IRQn  PIOA_IRQn

/* Asynchronous PPLC Reset pin definition */
//#define PPLC_ARST_GPIO             (PIO_PB0_IDX)
//#define PPLC_ARST_ACTIVE_LEVEL    IOPORT_PIN_LEVEL_LOW
//#define PPLC_ARST_INACTIVE_LEVEL  IOPORT_PIN_LEVEL_HIGH

/* Wrapper macros to ensure common naming across all boards */
//#define PPLC_ARST       {PIO_PB0, PIOB, ID_PIOB, PIO_OUTPUT_1, PIO_DEFAULT}
//#define PPLC_ARST_MASK   PIO_PB0
//#define PPLC_ARST_PIO    PIOB
//#define PPLC_ARST_ID     ID_PIOB
//#define PPLC_ARST_TYPE   PIO_OUTPUT_1
//#define PPLC_ARST_ATTR   PIO_DEFAULT

/* Synchronous PPLC Reset pin definition */
//#define PPLC_SRST_GPIO             (PIO_PA25_IDX)
//#define PPLC_SRST_ACTIVE_LEVEL    IOPORT_PIN_LEVEL_LOW
//#define PPLC_SRST_INACTIVE_LEVEL  IOPORT_PIN_LEVEL_HIGH

/* Wrapper macros to ensure common naming across all boards */
//#define PPLC_SRST       {PIO_PA25, PIOA, ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT}
//#define PPLC_SRST_MASK   PIO_PA25
//#define PPLC_SRST_PIO    PIOA
//#define PPLC_SRST_ID     ID_PIOA
//#define PPLC_SRST_TYPE   PIO_OUTPUT_1
//#define PPLC_SRST_ATTR   PIO_DEFAULT

#endif  /* CONF_PPLC_IF_H_INCLUDE */
