/**
 * \file
 *
 * \brief This file is used by the QDebug component to initialize, read
 * and write data over the USART SPI mode.
 * - Userguide:          QTouch Library User Guide - doc8207.pdf.
 * - Support: https://www.microchip.com/support/
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

#ifndef _SPI_MASTER_H_
#define _SPI_MASTER_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*============================ INCLUDES ======================================*/
#include <stdint.h>

/*============================ PROTOTYPES ====================================*/

/*! \brief Initialize the USART in SPI mode.
 */
void SPI_Master_Init(void);

/*! \brief Send and Read one byte using SPI Interface.
 * \param c: Data to send to slave.
 * \return uint8_t data: Data read from slave.
 * \note Called from SPI_Send_Message in this file.
 */
uint8_t SPI_Send_Byte(uint8_t c);

/*! \brief Send and Read one frame using SPI Interface..
 * \note Called from Send_Message in QDebugTransport.c
 */
void SPI_Send_Message(void);

#ifdef __cplusplus
}
#endif

#endif                          /* _SPI_MASTER_H_ */
