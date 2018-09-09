/* This source file is part of the ATMEL QTouch Library Release 5.1 */

/**
 * \file
 *
 * \brief  This file contains the BitBangSPI public API that can be used to
 * transfer data from a Touch Device to QTouch Studio using the QT600
 * USB Bridge.
 * - Userguide:          QTouch Library User Guide - doc8207.pdf.
 * - Support: https://www.microchip.com/support/
 *
 * Copyright (c) 2012-2018 Microchip Technology Inc. and its subsidiaries.
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
/*============================ INCLUDES ======================================*/
#include "asf.h"


#include "BitBangSPI_Master.h"
#include "QDebugSettings.h"
#include "QDebugTransport.h"

//! compile file only when QDebug is enabled.
#if DEF_TOUCH_QDEBUG_ENABLE == 1

#define PORTA (&(*((volatile Gpio*)GPIO)).GPIO_PORT[0])
#define PORTB (&(*((volatile Gpio*)GPIO)).GPIO_PORT[1])
#define PORTC (&(*((volatile Gpio*)GPIO)).GPIO_PORT[2])

/*============================ IMPLEMENTATION ================================*/
static uint8_t BitBangSPI_Send_Byte(uint8_t c);

/*============================================================================
Name    :   BitBangSPI_Master_Init
------------------------------------------------------------------------------
Purpose :   Initialize BitBangSPI Interface
Input   :   n/a
Output  :   n/a
Notes   :	Called from QDebug_Init in QDebug.c
============================================================================*/
void BitBangSPI_Master_Init (void)
{   
    REG( PORT, SPI_BB_SS )->ODERS =  ((1<<SS_BB) );
    REG( PORT, SPI_BB_MOSI )->ODERS =  (( 1 << MOSI_BB ) );
    REG( PORT, SPI_BB_SCK )->ODERS =  ( ( 1 << SCK_BB ));

    REG( PORT, SPI_BB_MISO )->ODERC =  ( 1 << MISO_BB );
#if defined(REV_GPIO) || (REV_GPIO == 0x215)
    // Always enable the Schmitt trigger for input pins.
    REG( PORT, SPI_BB_MISO )->STERS =  ( 1 << MISO_BB );
#endif
    
    REG( PORT, SPI_BB_SS )->OVRC =  ((1<<SS_BB) );
    REG( PORT, SPI_BB_MOSI )->OVRC =  (( 1 << MOSI_BB ) );
    REG( PORT, SPI_BB_SCK )->OVRC =  ( ( 1 << SCK_BB ));

    REG( PORT, SPI_BB_MISO )->OVRS =  ( 1 << MISO_BB );
    

}

/*============================================================================
Name    :   BitBangSPI_Send_Byte
------------------------------------------------------------------------------
Purpose :   Send and Read one byte using BitBangSPI Interface
Input   :   Data to send to slave
Output  :   Data read from slave
Notes   :	Called from BitBangSPI_Send_Message in this file
============================================================================*/
uint8_t BitBangSPI_Send_Byte(uint8_t c)
{
    unsigned bit;
    
    for (bit = 0; bit < 8; bit++) {
        /* write MOSI on trailing edge of previous clock */
        if (c & 0x80)
            REG( PORT, SPI_BB_MOSI )->OVRS =  ( 1 << MOSI_BB );
        else
            REG( PORT, SPI_BB_MOSI )->OVRC =  ( 1 << MOSI_BB );

        c <<= 1;

        /* half a clock cycle before leading/rising edge */
        cpu_delay_us(3u, F_CPU); /* SHOULD BE ~3uS. */

        REG( PORT, SPI_BB_SCK )->OVRS = ( 1 << SCK_BB );

        /* half a clock cycle before trailing/falling edge */
         cpu_delay_us(3u, F_CPU); /* SHOULD BE ~3uS. */

        /* read MISO on trailing edge */
        c |= ((REG( PORT, SPI_BB_MISO )->PVR >> MISO_BB) & 0x01);
        REG( PORT, SPI_BB_SCK )->OVRC = ( 1 << SCK_BB );
    }

    REG( PORT, SPI_BB_MOSI )->OVRC =  ( 1 << MOSI_BB );

    cpu_delay_us(18u, F_CPU);/* SHOULD BE ~18uS. */


    return c;
}


/*============================================================================
Name    :   BitBangSPI_Send_Message
------------------------------------------------------------------------------
Purpose :   Send and Read one frame using BitBangSPI Interface
Input   :   n/a
Output  :   n/a
Notes   :	Called from Send_Message in QDebugTransport.c
============================================================================*/
void BitBangSPI_Send_Message(void)
{
    unsigned int i;
    uint8_t FrameInProgress;

    // Send our message upstream
    for (i=0; i <= TX_index; i++)
    {
        FrameInProgress = RxHandler(BitBangSPI_Send_Byte(TX_Buffer[i]));
    }

    // Do we need to receive even more bytes?
    while (FrameInProgress)
        FrameInProgress = RxHandler(BitBangSPI_Send_Byte(0));

}

#endif  //DEF_TOUCH_QDEBUG_ENABLE == 1
