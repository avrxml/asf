/* This source file is part of the ATMEL QTouch Library Release 4.4 */
/*****************************************************************************
 *
 * \file
 *
 * \brief  This file is used by the QDebug component to initialize, read
 * and write data over the USART SPI mode.
 *
 * - Userguide:          QTouch Library User Guide - doc8207.pdf.
 * - Support: https://www.microchip.com/support/
 *
 *
 * Copyright (c) 2010-2018 Microchip Technology Inc. and its subsidiaries.
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
