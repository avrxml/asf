/**
 * \file
 *
 * \brief BitBangSPI for Qtouch.
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
