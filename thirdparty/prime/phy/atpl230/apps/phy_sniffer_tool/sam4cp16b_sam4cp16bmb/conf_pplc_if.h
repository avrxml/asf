/**
 * \file
 *
 * \brief PPLC interface Configuration.
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

#ifndef CONF_PPLC_IF_H_INCLUDE
#define CONF_PPLC_IF_H_INCLUDE

#include "board.h"

/* Select the SPI module that PPLC is connected to */
#define PPLC_SPI_MODULE     SPI0

/* Chip select used by PPLC internal peripheral  */
#define PPLC_CS             0

/* Interruption pin used by PPLC internal peripheral */
#define PPLC_INT_GPIO   (PIO_PB30_IDX)
#define PPLC_INT_FLAGS (IOPORT_MODE_DEBOUNCE)
#define PPLC_INT_SENSE (IOPORT_SENSE_FALLING)

#define PPLC_INT       {PIO_PB30, PIOB, ID_PIOB, PIO_INPUT, \
			PIO_DEBOUNCE | PIO_IT_FALL_EDGE}
#define PPLC_INT_MASK  PIO_PB30
#define PPLC_INT_PIO   PIOB
#define PPLC_INT_ID    ID_PIOB
#define PPLC_INT_TYPE  PIO_INPUT
#define PPLC_INT_ATTR  (PIO_DEBOUNCE | PIO_IT_FALL_EDGE)
#define PPLC_INT_IRQn  PIOB_IRQn

/* Asynchronous PPLC Reset pin definition */
#define PPLC_ARST_GPIO             (PIO_PC6_IDX)
#define PPLC_ARST_ACTIVE_LEVEL    IOPORT_PIN_LEVEL_LOW
#define PPLC_ARST_INACTIVE_LEVEL  IOPORT_PIN_LEVEL_HIGH

/* Wrapper macros to ensure common naming across all boards */
#define PPLC_ARST       {PIO_PC6, PIOC, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}
#define PPLC_ARST_MASK   PIO_PC6
#define PPLC_ARST_PIO    PIOC
#define PPLC_ARST_ID     ID_PIOC
#define PPLC_ARST_TYPE   PIO_OUTPUT_1
#define PPLC_ARST_ATTR   PIO_DEFAULT

/* Synchronous PPLC Reset pin definition */
#define PPLC_SRST_GPIO             (PIO_PC7_IDX)
#define PPLC_SRST_ACTIVE_LEVEL    IOPORT_PIN_LEVEL_LOW
#define PPLC_SRST_INACTIVE_LEVEL  IOPORT_PIN_LEVEL_HIGH

/* Wrapper macros to ensure common naming across all boards */
#define PPLC_SRST       {PIO_PC7, PIOC, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}
#define PPLC_SRST_MASK   PIO_PC7
#define PPLC_SRST_PIO    PIOC
#define PPLC_SRST_ID     ID_PIOC
#define PPLC_SRST_TYPE   PIO_OUTPUT_1
#define PPLC_SRST_ATTR   PIO_DEFAULT

#endif  /* CONF_PPLC_IF_H_INCLUDE */
