/* This source file is part of the ATMEL QTouch Library Release 4.4 */
/******************************************************************************
 *
 * \file
 *
 * \brief  This file contains the QDebug public API that can be used to
 * transfer data from a Touch Device to QTouch Studio using the QT600
 * USB Bridge.
 *
 * - Compiler:           IAR EWAVR/EWAVR32 and GNU GCC for AVR/AVR32
 * - Supported devices:  ATXMEGA128A1, AT32UC3A0/A1 Series, AT32UC3B0/B1 Series, AT32UC3C0/C1 Series AND AT32UC3LO series
 * - Userguide:          QTouch Library User Guide - doc8207.pdf.
 * - Support:            https://www.microchip.com/support/
 *
 * \author               Microchip Technology Inc: http://www.microchip.com
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
#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

/*============================ INCLUDES ======================================*/
#include "touch_api.h"

/*======================== MANIFEST CONSTANTS ================================*/
/*! \name Subscription definitions.
 */
//! @{
#define SUBS_SIGN_ON     	       0
#define SUBS_GLOBAL_CONFIG	       1
#define SUBS_SENSOR_CONFIG	       2
#define SUBS_SIGNALS     	       3
#define SUBS_REF         	       4
#define SUBS_DELTA       	       5
#define SUBS_STATES      	       6
//! @}
/*! \name PC commands.
 */
//! @{
#define QT_CMD_DUMMY                   0x10
#define QT_CMD_SET_SUBS                0x11
#define QT_CMD_SET_GLOBAL_CONFIG       0x12
#define QT_CMD_SET_CH_CONFIG           0x13
#define QT_CMD_SET_QM_BURST_LENGTHS    0x14
#define QT_CMD_SET_USER_DATA           0x15
//! @}
/*! \name Touch MCU data packets.
 */
//! @{
#define QT_DUMMY	               0x20
#define QT_SIGN_ON	               0x21
#define QT_GLOBAL_CONFIG               0x22
#define QT_SENSOR_CONFIG               0x23
#define QT_SIGNALS	               0x24
#define QT_REFERENCES                  0x25
#define QT_DELTAS                      0x26
#define QT_STATES                      0x27
//! @}

/*============================ PROTOTYPES ====================================*/
/*! \name Public functions.
 */
//! @{
/*! \brief This API initializes QDebug interface, including the low level
 * hardware interface (SPI, TWI, USART etc).
 * \note Must be called before using any other QDebug API.
 */
void QDebug_Init (void);
/*! \brief Command handler for the data received from QTouch Studio
 * \note This function should be called in the main loop after
 * measure_sensors to process the data received from QTOuch Studio.
 */
void QDebug_ProcessCommands (void);
/*! \brief Send data to QTouch Studio based on the subscription.
 * \param  qt_lib_flags:Change flag from measure_sensors.
 */
void QDebug_SendData (uint16_t qt_lib_flags);
/*! \brief Set subscription values.
 * \note  This function can be used directly in main to set data subscription
 * if 1way SPI interface is used.
 */
void QDebug_SetSubscriptions (uint16_t once, uint16_t change,
			      uint16_t allways);
//! @}

/*! \name Private functions.
 */
//! @{
/*! \brief Extract the data packet from QTouch Studio and set global config.
 * \note  Should only be called from the command handler.
 */
void Set_Global_Config (void);
/*! \brief Extract the data packet from QTouch Studio and set channel config.
 * \note  Should only be called from the command handler.
 */
void Set_Channel_Config (void);
/*! \brief Set Data Subscription values.
 * \note  Should only be called from the command handler.
 */
void Set_Subscriptions (void);
/*! \brief Extract the data packet from QTouch Studio and set measurement period.
 * \note  Should only be called from the command handler.
 */
void Set_Measurement_Period (void);
/*! \brief Extracts user data from QTouch Studio to touch mcu memory.
 * \param pdata: data pointer.
 * \note  The data can be binary data.
 */
void Set_QT_User_Data (uint8_t *);
#ifdef _QMATRIX_
/*! \brief Extract the data packet from QTouch Studio and set QMatrix burst lengths.
 * \note  Should only be called from the command handler.
 */
void Set_QM_Burst_Lengths (void);
#endif
/*! \brief Transmits a dummy packet if no other subscriptions are set.
 */
void Transmit_Dummy (void);
/*! \brief Transmits the sign on packet to QTouch Studio.
 */
void Transmit_Sign_On (void);
/*! \brief Transmits the global config struct to QTouch Studio.
 */
void Transmit_Global_Config (void);
/*! \brief Transmits the channel config struct to QTouch Studio.
 */
void Transmit_Sensor_Config (void);
/*! \brief Transmits the measurement values for each channel to QTouch Studio.
 */
void Transmit_Signals (void);
/*! \brief Transmits the channel reference values to QTouch Studio.
 */
void Transmit_Ref (void);
/*! \brief Transmits the channel delta values to QTouch Studio.
 */
void Transmit_Delta (void);
/*! \brief Transmits the state values to QTouch Studio.
 */
void Transmit_State (void);
/*! \brief Transmits user data to QTouch Studio.
 * \param pdata: data pointer.
 * \param c: length of data in bytes.
 * \note The data will be binary data.
 */
void Transmit_QT_User_Data (uint8_t * pdata, uint16_t c);
/*! \brief Transmits the application execution timestamp values to QTouch Studio.
 * \note This value is a combination of current_time_ms_touch (high word) &
 * timer counter register (low word).
 */
void Transmit_Time_Stamps (void);
//! @}
#endif
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-EOF-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
