/**
 * \file
 *
 * \brief This file is used by the QDebug component to initialize, read
 * and write data over the USART SPI mode.
 * - Userguide:          QTouch Library User Guide - doc8207.pdf.
 * - Support: https://www.microchip.com/support/
 *
 * Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries.
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
