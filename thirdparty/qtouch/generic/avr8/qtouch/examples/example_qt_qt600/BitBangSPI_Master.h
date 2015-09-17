/* This source file is part of the ATMEL QTouch Library Release 4.4 */
/*! \file *********************************************************************
 *
 * \brief  This file contains the BitBangSPI Configuration settings.
 *
 * - Compiler:           IAR EWAVR and GNU GCC for AVR8
 * - Supported devices:  ATXMEGA128A1
 * - Userguide:          QTouch Library User Guide - doc8207.pdf.
 * - Support:            http://www.atmel.com/design-support/
 *
 * \author               Atmel Corporation: http://www.atmel.com
 *
 ******************************************************************************/
 /* Copyright (c) 2010-2015 Atmel Corporation. All rights reserved.
    *
    * Redistribution and use in source and binary forms, with or without
    * modification, are permitted provided that the following conditions are met:
    *
    * 1. Redistributions of source code must retain the above copyright notice, this
    * list of conditions and the following disclaimer.
    *
    * 2. Redistributions in binary form must reproduce the above copyright notice,
    * this list of conditions and the following disclaimer in the documentation
    * and/or other materials provided with the distribution.
    *
    * 3. The name of Atmel may not be used to endorse or promote products derived
    * from this software without specific prior written permission.
    *
    * 4. This software may only be redistributed and used in connection with an Atmel
    * AVR product.
    *
    * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
    * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
    * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
    * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
    * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
    *
  */
#ifndef BITBANGSPI_MASTER_H_INCLUDED
#define BITBANGSPI_MASTER_H_INCLUDED

/*============================ PROTOTYPES ====================================*/
void BitBangSPI_Master_Init (void);
void BitBangSPI_Send_Message (void);

/*============================ MACROS ========================================*/
/*! \name Port and Pin Selection
 * \brief Port and Pin selection for interfacing 
 * device to QT600 Bridge
 */
//! @{
#define SS_BB       4
#define SCK_BB      7
#define MOSI_BB     5
#define MISO_BB     6

#define SPI_BB_SS      		C
#define SPI_BB_SCK      	C
#define SPI_BB_MOSI      	C
#define SPI_BB_MISO      	C
//! @}

#define CPU_SPEED   8		// MHz

#define DELAYUS(DELAY) __builtin_avr_delay_cycles((DELAY)*CPU_SPEED);



#endif
/* EOF */
