/* This source file is part of the ATMEL QTouch Library Release 4.4 */
/*****************************************************************************
 *
 * \file
 *
 * \brief  This file contains the BitBangSPI Configuration settings.
 *
 * - Compiler:           IAR EWAVR and GNU GCC for AVR32
 * - Supported devices:  AT32UC3A0/A1 Series, AT32UC3B0/B1 Series 
 *                       AND AT32UC3C0/C1 Series 
 * - Userguide:          QTouch Library User Guide - doc8207.pdf.
 * - Support:            https://www.microchip.com/support/
 *
 * \author               Microchip Technology Inc: http://www.microchip.com
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
#include "parts.h"

#ifndef BITBANGSPI_MASTER_H_INCLUDED
#define BITBANGSPI_MASTER_H_INCLUDED

/*============================ PROTOTYPES ====================================*/
void BitBangSPI_Master_Init (void);
void BitBangSPI_Send_Message(void);

/*============================ MACROS ========================================*/
// The definitions below should not clash with the SNS/SNSK port pins


#define SS_BB       0
#define SCK_BB      3
#define MOSI_BB     1
#define MISO_BB     2

#if (UC3A0 || UC3A1 || UC3B)
#define SPI_BB_SS       B
#define SPI_BB_SCK      B
#define SPI_BB_MOSI     B
#define SPI_BB_MISO     B
#elif UC3C
#define SPI_BB_SS       C
#define SPI_BB_SCK      C
#define SPI_BB_MOSI     C
#define SPI_BB_MISO     C
#else
# error Unsupported chip type
#endif



#define DELAY_75US()  do{   \
  DELAYUS();  \
  DELAYUS();  \
    } while(0);

#define DELAY_1US() 	nop()

#if (defined __GNUC__)
#define nop() __asm__ __volatile__ ( "nop" )
#endif

#if (defined __ICCAVR32__)
#define nop() (__no_operation())
#endif

#define DELAYUS() do{   \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
        nop();  \
    } while(0);


#endif
/* EOF */
