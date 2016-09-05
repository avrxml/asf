/* This source file is part of the ATMEL QTouch Library 5.0.5 */

/*****************************************************************************
 *
 * \file
 *
 * \brief  This file contains the QDebug bitbang API that can be used to
 * transfer data from a Touch Device to QTouch Studio using the QT600
 * USB Bridge.
 *
 *
 * - Userguide:          QTouch Library Peripheral Touch Controller User Guide.
 * - Support email:      www.atmel.com/design-support/
 *
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
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

/*============================ PROTOTYPES ====================================*/
void BitBangSPI_Master_Init(void);
void BitBangSPI_Send_Message(void);

/*============================ MACROS ========================================*/

#define JOIN( x, y ) x ## y
#define JOIN1(x, y, z) x ## y ## z

#define REG( REGISTER, SIDE ) JOIN( REGISTER, SIDE )
#define CONCAT( A, B, C ) JOIN1( A, B, C )

/* The definitions below should not clash with the SNS/SNSK port pins */

/* Select QDebug Pins. */
#define SS_BB           QDEBUG_SPI_BB_SS_PIN
#define SCK_BB          QDEBUG_SPI_BB_SCK_PIN
#define MOSI_BB         QDEBUG_SPI_BB_MOSI_PIN
#define MISO_BB         QDEBUG_SPI_BB_MISO_PIN

/* Select QDebug Ports. */
#define SPI_BB_SS       QDEBUG_SPI_BB_SS_PORT
#define SPI_BB_SCK      QDEBUG_SPI_BB_SCK_PORT
#define SPI_BB_MOSI     QDEBUG_SPI_BB_MOSI_PORT
#define SPI_BB_MISO     QDEBUG_SPI_BB_MISO_PORT

#endif

/* EOF */
