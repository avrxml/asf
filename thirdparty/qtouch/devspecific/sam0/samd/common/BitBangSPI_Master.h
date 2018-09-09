/* This source file is part of the ATMEL QTouch Library 5.0.1 */

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
 * - Support: https://www.microchip.com/support/
 *
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
