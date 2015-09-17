/**
 * \file
 *
 * \brief This file is used by the QDebug component to initialize, read
 * and write data over the USART SPI mode.
 * - Userguide:          QTouch Library User Guide - doc8207.pdf.
 * - Support: http://www.atmel.com/design-support/
 *
 * Copyright (C) 2011-2015 Atmel Corporation. All rights reserved.
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

#include "compiler.h"
#include "usart.h"
#include "gpio.h"

#include "SPI_Master.h"

#include "QDebugSettings.h"
#include "QDebugTransport.h"

/*! compile file only when QDebug is enabled. */
#if DEF_TOUCH_QDEBUG_ENABLE == 1

/*! USART SPI GPIO options. */
static gpio_map_t USART_SPI_GPIO_MAP = {
	{QDEBUG_SPI_SCK_PIN, QDEBUG_SPI_SCK_FUNCTION},
	{QDEBUG_SPI_MISO_PIN, QDEBUG_SPI_MISO_FUNCTION},
	{QDEBUG_SPI_MOSI_PIN, QDEBUG_SPI_MOSI_FUNCTION},
	{QDEBUG_SPI_NSS_PIN, QDEBUG_SPI_NSS_FUNCTION}
};

/*! USART SPI options. */
static usart_spi_options_t USART_SPI_OPTIONS = {
	.baudrate = QDEBUG_SPI_BAUD_RATE,
	.charlength = 8,
	.spimode = 0,
	.channelmode = USART_NORMAL_CHMODE
};

/*! \brief Initialize the USART in SPI mode.
 */
void SPI_Master_Init()
{
	/* Assign GPIO to SPI. */
	gpio_enable_module(USART_SPI_GPIO_MAP,
			sizeof(USART_SPI_GPIO_MAP) /
			sizeof(USART_SPI_GPIO_MAP[0]));

	/* Initialize USART in SPI Master Mode. */
	usart_init_spi_master(QDEBUG_SPI_USART, &USART_SPI_OPTIONS,
			QDEBUG_PBA_FREQ_HZ);
}

/*! \brief Send and Read one byte using SPI Interface.
 * \param c: Data to send to slave.
 * \return uint8_t data: Data read from slave.
 * \note Called from SPI_Send_Message in this file.
 */
uint8_t SPI_Send_Byte(uint8_t c)
{
	uint8_t data;

	if (usart_write_char(&AVR32_USART2, c) == USART_SUCCESS) {
		data = usart_getchar(&AVR32_USART2);
	} else {
		return (uint8_t)USART_FAILURE;
	}

	return data;
}

/*! \brief Send and Read one frame using SPI Interface..
 * \note Called from Send_Message in QDebugTransport.c
 */
void SPI_Send_Message(void)
{
	unsigned int i;
	uint8_t FrameInProgress;

	/* Disable interrupts. */
	cpu_irq_disable();

	/* Send our message upstream */
	for (i = 0; i <= TX_index; i++) {
		FrameInProgress = RxHandler(SPI_Send_Byte(TX_Buffer[i]));
	}

	/* Do we need to receive even more bytes? */
	while (FrameInProgress) {
		FrameInProgress = RxHandler(SPI_Send_Byte(0));
	}

	/* Enable interrupts. */
	cpu_irq_enable();
}

#endif  /* DEF_TOUCH_QDEBUG_ENABLE == 1 */
