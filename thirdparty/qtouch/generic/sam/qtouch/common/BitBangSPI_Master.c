/**
 * \file
 *
 * \brief BitBangSPI for Qtouch.
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

#ifdef _DEBUG_INTERFACE_
#include "compiler.h"
#include "BitBangSPI_Master.h"
#include "QDebugTransport.h"

#define JOIN1( A, B, C ) A ## B ## C
#define CONCAT( A, B, C ) JOIN1( A, B, C )
#define PORT REG_PIO

/**
 * \brief Initialize BitBangSPI Interface.
 */
void BitBangSPI_Master_Init(void)
{
#if SAM3N
	CONCAT(PORT, SPI_BB, _OER) =
			((1 << MOSI_BB) | (1 << SCK_BB) | (1 << PIO_SS));
	CONCAT(PORT, SPI_BB, _ODR) = (1 << MISO_BB);
	CONCAT(PORT, SPI_BB, _CODR) =
			((1 << MOSI_BB) | (1 << SCK_BB) | (1 << PIO_SS));
	CONCAT(PORT, SPI_BB, _SODR) = (1 << MISO_BB);
#else
	CONCAT(PORT, SPI_BB, _OER) = ((1 << MOSI_BB) | (1 << SCK_BB));
	CONCAT(PORT, SPI_BB, _ODR) = (1 << MISO_BB);
	CONCAT(PORT, SPI_BB, _CODR) = ((1 << MOSI_BB) | (1 << SCK_BB));
	CONCAT(PORT, SPI_BB, _SODR) = (1 << MISO_BB);
#endif
}

/**
 * \brief  Send and Read one byte using BitBangSPI Interface.
 *
 * \param c  Data to send to slave.
 *
 * \return Data read from slave.
 */
static uint8_t BitBangSPI_Send_Byte(uint8_t c)
{
	uint32_t bit;
	volatile uint32_t i;

	for (bit = 0; bit < 8; bit++) {
		/* Write MOSI on trailing edge of previous clock */
		if (c & 0x80) {
			CONCAT(PORT, SPI_BB, _SODR) = (1 << MOSI_BB);
		} else {
			CONCAT(PORT, SPI_BB, _CODR) = (1 << MOSI_BB);
		}

		c <<= 1;

		/* Half a clock cycle before leading/rising edge */
		DELAY1US();

		CONCAT(PORT, SPI_BB, _SODR) = (1 << SCK_BB);

		/* Half a clock cycle before trailing/falling edge */
		DELAY1US();

		/* Read MISO on trailing edge */
		c |= ((CONCAT(PORT, SPI_BB, _PDSR) >> MISO_BB) & 0x01);
		CONCAT(PORT, SPI_BB, _CODR) = (1 << SCK_BB);
	}

	CONCAT(PORT, SPI_BB, _CODR) = (1 << MOSI_BB);

	for (i = 0; i < 50; i++) {
		DELAY50US();
	}

	return c;
}

/**
 * \brief Send and Read one frame using BitBangSPI Interface.
 */
void BitBangSPI_Send_Message(void)
{
	uint32_t i;
	uint8_t frame_in_progress;

	/* Send our message upstream */
	for (i = 0; i <= TX_index; i++) {
		frame_in_progress =
				RxHandler(BitBangSPI_Send_Byte(TX_Buffer[i]));
	}

	/* Do we need to receive even more bytes? */
	while (frame_in_progress) {
		frame_in_progress = RxHandler(BitBangSPI_Send_Byte(0));
	}
}
#endif /*  _DEBUG_INTERFACE_ */
