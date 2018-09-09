/* This source file is part of the ATMEL QTouch Library Release 4.4 */
/*****************************************************************************
 *
 * \file
 *
 * \brief  This file contains the BitBangSPI Configuration settings.
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
