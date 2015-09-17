/* This source file is part of the ATMEL QTouch Library Release 4.4 */
/*****************************************************************************
 *
 * \file
 *
 * \brief  This file is used by the QDebug component to initialize, read
 * and write data over the USART SPI mode.
 *
 * - Userguide:          QTouch Library User Guide - doc8207.pdf.
 * - Support: http://www.atmel.com/design-support/
 *
 *
 * Copyright (c) 2010-2015 Atmel Corporation. All rights reserved.
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
/*============================ INCLUDES ======================================*/
#ifdef _DEBUG_INTERFACE_
#include <avr/io.h>
#include "BitBangSPI_Master.h"
#include "QDebugTransport.h"

/*============================ IMPLEMENTATION ================================*/
#define JOIN1( A, B, C ) A ## B ## C
#define CONCAT( A, B, C ) JOIN1( A, B, C )

/*! \brief BitBangSPI_Master_Init to Initialize BitBangSPI Interface
 * 
 * \note Called from QDebug_Init in QDebug.c
 */
void
BitBangSPI_Master_Init (void)
{
  CONCAT (PORT, SPI_BB_SS, _DIRSET) = ((1 << SS_BB));
  CONCAT (PORT, SPI_BB_MOSI, _DIRSET) = ((1 << MOSI_BB));
  CONCAT (PORT, SPI_BB_SCK, _DIRSET) = ((1 << SCK_BB));

  CONCAT (PORT, SPI_BB_MISO, _DIRCLR) = (1 << MISO_BB);

  CONCAT (PORT, SPI_BB_SS, _OUTCLR) = ((1 << SS_BB));
  CONCAT (PORT, SPI_BB_MOSI, _OUTCLR) = ((1 << MOSI_BB));
  CONCAT (PORT, SPI_BB_SCK, _OUTCLR) = ((1 << SCK_BB));

  CONCAT (PORT, SPI_BB_MISO, _OUTSET) = (1 << MISO_BB);
}

/*! \brief BitBangSPI_Send_Byte Send and Read 
 *  one byte using BitBangSPI Interface
 * \param input is Data to send to slave
 * 		  output is Data read from slave
 * \note Called from BitBangSPI_Send_Message in this file
 */
uint8_t
BitBangSPI_Send_Byte (uint8_t c)
{
  unsigned bit;
  for (bit = 0; bit < 8; bit++)
    {
      /* write MOSI on trailing edge of previous clock */
      if (c & 0x80)
	CONCAT (PORT, SPI_BB_MOSI, _OUTSET) = (1 << MOSI_BB);
      else
	CONCAT (PORT, SPI_BB_MOSI, _OUTCLR) = (1 << MOSI_BB);

      c <<= 1;

      /* half a clock cycle before leading/rising edge */
      DELAYUS (1);

      CONCAT (PORT, SPI_BB_SCK, _OUTSET) = (1 << SCK_BB);

      /* half a clock cycle before trailing/falling edge */
      DELAYUS (1);

      /* read MISO on trailing edge */
      c |= ((CONCAT (PORT, SPI_BB_MISO, _IN) >> MISO_BB) & 0x01);
      CONCAT (PORT, SPI_BB_SCK, _OUTCLR) = (1 << SCK_BB);
    }

  CONCAT (PORT, SPI_BB_MOSI, _OUTCLR) = (1 << MOSI_BB);

  DELAYUS (50);

  return c;
}

/*! \brief BitBangSPI_Send_Message to Send and Read one frame 
 *  using BitBangSPI Interface
 * \note Called from Send_Message in QDebugTransport.c
 */

void
BitBangSPI_Send_Message (void)
{
  unsigned int i;
  uint8_t FrameInProgress;

  // Send our message upstream        
  for (i = 0; i <= TX_index; i++)
    {
      FrameInProgress = RxHandler (BitBangSPI_Send_Byte (TX_Buffer[i]));
    }

  // Do we need to receive even more bytes?
  while (FrameInProgress)
    FrameInProgress = RxHandler (BitBangSPI_Send_Byte (0));
}
#endif /*  _DEBUG_INTERFACE_ */
