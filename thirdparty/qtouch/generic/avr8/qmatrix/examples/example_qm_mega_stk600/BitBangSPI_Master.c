/*
 * \file
 *
 * \brief
 *
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
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
 */

/*============================ INCLUDES ======================================*/
#include "touch_config.h"

#ifdef _DEBUG_INTERFACE_
#include <avr/io.h>
#include "BitBangSPI_Master.h"
#include "QDebugTransport.h"

/*============================ IMPLEMENTATION ================================*/

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
    REG( DDR, SPI_BB_SS ) |=  ((1<<SS_BB));
    REG( DDR, SPI_BB_MOSI ) |=  (( 1 << MOSI_BB ));
    REG( DDR, SPI_BB_SCK ) |=  (( 1 << SCK_BB ));

    REG( DDR, SPI_BB_MISO ) &=  ~( 1 << MISO_BB );

    REG( PORT, SPI_BB_SS ) &=  ~((1<<SS_BB));
    REG( PORT, SPI_BB_MOSI ) &=  ~(( 1 << MOSI_BB ));
    REG( PORT, SPI_BB_SCK ) &=  ~(( 1 << SCK_BB ));

	REG( PORT, SPI_BB_MISO ) |=  ( 1 << MISO_BB );
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
            REG( PORT, SPI_BB_MOSI ) |=  ( 1 << MOSI_BB );
        else
            REG( PORT, SPI_BB_MOSI ) &=  ~( 1 << MOSI_BB );

        c <<= 1;

        /* half a clock cycle before leading/rising edge */
        DELAYUS(1);

        REG( PORT, SPI_BB_SCK ) |= ( 1 << SCK_BB );

        /* half a clock cycle before trailing/falling edge */
        DELAYUS(1);

        /* read MISO on trailing edge */
        c |= ((REG( PIN, SPI_BB_MISO ) >> MISO_BB) & 0x01);
        REG( PORT, SPI_BB_SCK ) &= ~( 1 << SCK_BB );
    }

    REG( PORT, SPI_BB_MOSI ) &=  ~( 1 << MOSI_BB );

    DELAYUS(75);

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
#endif /* _DEBUG_INTERFACE_ */
