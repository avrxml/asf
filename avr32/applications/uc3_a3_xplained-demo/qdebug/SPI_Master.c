/*****************************************************************************
 *
 * \file
 *
 * \brief USART driver for AVR32 UC3.
 *
 * This file contains basic functions for the AVR32 USART, with support for all
 * modes, settings and clock speeds.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#include "compiler.h"
#include "SPI_Master.h"
#include "QDebugTransport.h"
#include "usart.h"
#include "gpio.h"
#include "spi.h"

#define PBA_HZ                            12000000UL                /* PBA Clock Frequency */
#define TOUCH_DEBUG_SPI                 &AVR32_SPI0
#define EXAMPLE_USART_SPI_SCK_PIN         8
#define EXAMPLE_USART_SPI_SCK_FUNCTION    0
#define EXAMPLE_USART_SPI_MISO_PIN        11
#define EXAMPLE_USART_SPI_MISO_FUNCTION   0
#define EXAMPLE_USART_SPI_MOSI_PIN        10
#define EXAMPLE_USART_SPI_MOSI_FUNCTION   0
#define EXAMPLE_USART_SPI_NSS_PIN         7
#define EXAMPLE_USART_SPI_NSS_FUNCTION    0

// USART SPI GPIO options.
static  gpio_map_t USART_SPI_GPIO_MAP =
{
	{EXAMPLE_USART_SPI_SCK_PIN,  EXAMPLE_USART_SPI_SCK_FUNCTION },
	{EXAMPLE_USART_SPI_MISO_PIN, EXAMPLE_USART_SPI_MISO_FUNCTION},
	{EXAMPLE_USART_SPI_MOSI_PIN, EXAMPLE_USART_SPI_MOSI_FUNCTION},
	{EXAMPLE_USART_SPI_NSS_PIN,  EXAMPLE_USART_SPI_NSS_FUNCTION }
};


//! Option structure for SPI channels.
static spi_options_t TOUCH_DEBUG_SPI_OPTIONS =
{
  //! The SPI channel to set up.
  .reg = 3,
  .baudrate = 48000, //! Preferred baudrate for the SPI.
  .bits = 8,//! Number of bits in each character (8 to 16).

  //! Delay before first clock pulse after selecting slave (in PBA clock periods).
  .spck_delay = 0,

  //! Delay between each transfer/character (in PBA clock periods).
  .trans_delay = 0,

  //! Sets this chip to stay active after last transfer to it.
  .stay_act = 1,

  //! Which SPI mode to use when transmitting.
  .spi_mode = 0,

  //! Disables the mode fault detection.
  //! With this bit cleared, the SPI master mode will disable itself if another
  //! master tries to address it.
  .modfdis = 1,
};

void SPI_Master_Init()
{
  // Assign GPIO to SPI.
  gpio_enable_module(USART_SPI_GPIO_MAP, sizeof(USART_SPI_GPIO_MAP) / sizeof(USART_SPI_GPIO_MAP[0]));

  // Initialize USART in SPI Master Mode.
  spi_initMaster(TOUCH_DEBUG_SPI, &TOUCH_DEBUG_SPI_OPTIONS);
  spi_selectionMode(TOUCH_DEBUG_SPI, 0, 0, 0);
  spi_setupChipReg(TOUCH_DEBUG_SPI, &TOUCH_DEBUG_SPI_OPTIONS, PBA_HZ);
  spi_enable(TOUCH_DEBUG_SPI);
  spi_selectChip(TOUCH_DEBUG_SPI, 3);
}

/*============================================================================
Name    :   SPI_Send_Byte
------------------------------------------------------------------------------
Purpose :   Send and Read one byte using SPI Interface
Input   :   Data to send to slave
Output  :   Data read from slave
Notes   :	Called from SPI_Send_Message in this file
============================================================================*/
uint8_t SPI_Send_Byte(uint8_t c)
{
    uint16_t data;

    if(spi_write(TOUCH_DEBUG_SPI, c) == SPI_OK)
    {
        spi_read(TOUCH_DEBUG_SPI, &data);
    }
    else
    {
      return (uint8_t) SPI_ERROR;
    }

    return (uint8_t) data;
}

/*============================================================================
Name    :   SPI_Send_Message
------------------------------------------------------------------------------
Purpose :   Send and Read one frame using SPI Interface
Input   :   n/a
Output  :   n/a
Notes   :	Called from Send_Message in QDebugTransport.c
============================================================================*/
void SPI_Send_Message(void)
{
    unsigned int i;
    uint8_t FrameInProgress = 0;

   // Send our message upstream
    for (i=0; i <= TX_index; i++)
    {
		FrameInProgress = RxHandler(SPI_Send_Byte(TX_Buffer[i]));
    }

  // Do we need to receive even more bytes?
  while (FrameInProgress)
	FrameInProgress = RxHandler(SPI_Send_Byte(0));
}

#if 0

// USART SPI options.
static  usart_spi_options_t USART_SPI_OPTIONS =
{
	.baudrate     = 48000,
	.charlength   = 8,
	.spimode      = 0,
	.channelmode  = USART_NORMAL_CHMODE
};

void SPI_Master_Init()
{
  // Assign GPIO to SPI.
  gpio_enable_module(USART_SPI_GPIO_MAP, sizeof(USART_SPI_GPIO_MAP) / sizeof(USART_SPI_GPIO_MAP[0]));

  // Initialize USART in SPI Master Mode.
  usart_init_spi_master(EXAMPLE_USART_SPI, &USART_SPI_OPTIONS, PBA_HZ);
  spi_initMaster(TOUCH_DEBUG_SPI, const spi_options_t *options);
}

/*============================================================================
Name    :   SPI_Send_Byte
------------------------------------------------------------------------------
Purpose :   Send and Read one byte using SPI Interface
Input   :   Data to send to slave
Output  :   Data read from slave
Notes   :	Called from SPI_Send_Message in this file
============================================================================*/
uint8_t SPI_Send_Byte(uint8_t c)
{
    uint8_t data;

    if(usart_write_char(&AVR32_USART2,c) == USART_SUCCESS)
    {
        data = usart_getchar(&AVR32_USART2);
    }
    else
    {
      return (uint8_t)USART_FAILURE;
    }

    return data;
}

/*============================================================================
Name    :   SPI_Send_Message
------------------------------------------------------------------------------
Purpose :   Send and Read one frame using SPI Interface
Input   :   n/a
Output  :   n/a
Notes   :	Called from Send_Message in QDebugTransport.c
============================================================================*/
void SPI_Send_Message(void)
{
    unsigned int i;
    uint8_t FrameInProgress;

   // Send our message upstream
    for (i=0; i <= TX_index; i++)
    {
		FrameInProgress = RxHandler(SPI_Send_Byte(TX_Buffer[i]));
    }

  // Do we need to receive even more bytes?
  while (FrameInProgress)
	FrameInProgress = RxHandler(SPI_Send_Byte(0));
}
#endif
