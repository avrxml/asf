/* This source file is part of the ATMEL QTouch Library Release 4.4 */
/*****************************************************************************
 *
 * \file
 *
 * \brief  This file contains the BitBangSPI Configuration settings.
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
 ******************************************************************************/

#ifndef BITBANGSPI_MASTER_H_INCLUDED
#define BITBANGSPI_MASTER_H_INCLUDED

#if SAM3N
#define SCK_BB      14
#define MOSI_BB     13
#define MISO_BB     12
#define PIO_SS      31
#else
#define SCK_BB      15
#define MOSI_BB     16
#define MISO_BB     17
#endif

#define SPI_BB      A

#define DELAY50US() do{   \
    nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();  \
    nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();  \
    nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();  \
    nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();  \
    nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();  \
    nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();  \
    nop(); nop(); nop(); nop(); nop();  \
    nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();  \
    nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();  \
    nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();  \
    nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();  \
    nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();  \
    nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();  \
    nop(); nop(); nop(); nop(); nop();  \
    nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();  \
    nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();  \
    nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop();  \
    } while(0);

#define DELAY1US(...) DELAY50US()

void BitBangSPI_Master_Init(void);
void BitBangSPI_Send_Message(void);

#endif
