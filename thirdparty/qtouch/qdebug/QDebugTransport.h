/* This source file is part of the ATMEL QTouch Library Release 4.3.1 */
/*****************************************************************************
 *
 * \file
 *
 * \brief  This file contains the QDebug Transport API that is used by the
 * QDebug component.
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  AT32UC3A0/A1 Series, AT32UC3B0/B1 Series,
 *                       AT32UC3C0/C1 Series AND AT32UC3L0 series
 * - Userguide:          QTouch Library User Guide - doc8207.pdf.
 * - Support: https://www.microchip.com/support/
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


#ifndef DEBUGTRANSPORT_H_INCLUDED
#define DEBUGTRANSPORT_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif


/*============================ INCLUDES ======================================*/
#include <parts.h>
#if UC3L0
#include "touch_api_at32uc3l.h"
#elif SAM4L
# include "touch_api_sam4l.h"
#elif (SAMD20 || SAMD21)
# include "touch_api_SAMD.h"
#elif SAML22
# include "touch_api_ptc.h"
#else
#include "touch_api.h"
#endif
/*============================ MACROS ========================================*/

#ifdef _DEBUG_INTERFACE_

#define MESSAGE_START    0x1B

#ifndef DEF_TOUCH_QDEBUG_ENABLE_QM
#define DEF_TOUCH_QDEBUG_ENABLE_QM 0
#endif

#ifndef DEF_TOUCH_QDEBUG_ENABLE_QTA
#define DEF_TOUCH_QDEBUG_ENABLE_QTA 0
#endif

#ifndef DEF_TOUCH_QDEBUG_ENABLE_QTB
#define DEF_TOUCH_QDEBUG_ENABLE_QTB 0
#endif

#ifndef DEF_TOUCH_QDEBUG_ENABLE_AT
#define DEF_TOUCH_QDEBUG_ENABLE_AT 0
#endif

#ifndef DEF_TOUCH_QDEBUG_ENABLE
#define DEF_TOUCH_QDEBUG_ENABLE 0
#endif

#if DEF_TOUCH_QDEBUG_ENABLE_QM == 1
#define TX_BUFFER_SIZE (QM_NUM_CHANNELS*4)+10
#define RX_BUFFER_SIZE (QM_NUM_CHANNELS*4)+10
#elif (DEF_TOUCH_QDEBUG_ENABLE_QTA == 1)
#define TX_BUFFER_SIZE (QTA_NUM_CHANNELS*4)+10
#define RX_BUFFER_SIZE (QTA_NUM_CHANNELS*4)+10
#elif (DEF_TOUCH_QDEBUG_ENABLE_QTB == 1)
#define TX_BUFFER_SIZE (QTB_NUM_CHANNELS*4)+10
#define RX_BUFFER_SIZE (QTB_NUM_CHANNELS*4)+10
#elif DEF_TOUCH_QDEBUG_ENABLE_AT == 1
#define TX_BUFFER_SIZE (1*4)+10
#define RX_BUFFER_SIZE (1*4)+10
#elif (DEF_TOUCH_QDEBUG_ENABLE == 1)
#define TX_BUFFER_SIZE (QT_NUM_CHANNELS*4)+10
#define RX_BUFFER_SIZE (QT_NUM_CHANNELS*4)+10
#else
#endif

#if (!(UC3L0)) && (!(SAM4L)) && (!(SAMD20)) && (!(SAMD21))
#ifdef _ROTOR_SLIDER_
#define TX_BUFFER_SIZE (QT_NUM_CHANNELS*4)+10
#define RX_BUFFER_SIZE (QT_NUM_CHANNELS*4)+10
#else
#define TX_BUFFER_SIZE (QT_NUM_CHANNELS*3)+10
#define RX_BUFFER_SIZE (QT_NUM_CHANNELS*3)+10
#endif
#endif

#define STATE_IDLE	    0
#define STATE_LENGTH1	1
#define STATE_LENGTH2	2
#define STATE_DATA	    3

#define INT(a, b)	((a << 8) | b)

/*============================ GLOBAL VARIABLES ==============================*/
//! TX Buffer globals
  extern uint8_t TX_Buffer[TX_BUFFER_SIZE];
  extern uint16_t TX_index;

//! RX Buffer globals
  extern uint8_t RX_Buffer[RX_BUFFER_SIZE];
  extern uint16_t RX_index;

  extern uint8_t SequenceL;
  extern uint8_t SequenceH;

/*============================ PROTOTYPES ====================================*/

/*! \brief Initialize the send and receive buffers.
 * \note Called from QDebug_Init.
 */
  void Init_Buffers (void);

/*! \brief Puts one byte in the Transmit Buffer.
 * \param data: byte to be sent.
 * \note Called from QDebug_Init.
 */
  void PutChar (uint8_t data);

/*! \brief Puts two bytes in the Transmit Buffer.
 * \param data: 16bit data to be sent.
 * \note Big Endian. TX_index is post incremented.
 */
  void PutInt (uint16_t data);

/*! \brief Get one byte from the Receive Buffer.
 * \return uint8_t: byte received.
 * \note RX_index is post incremented.
 */
  uint8_t GetChar (void);

/*! \brief Send the content of the TX_Buffer to the USB Bridge using the
 * interface selected in QDebugSettings.h
 * \note Called from the transmit functions in QDebug.c.
 */
  void Send_Message (void);

/*! \brief Executes a master read transmission if TWI is selected as interface.
 * Checks if RX_Buffer has a valid frame
 * \return uint8_t: returns a true or false dependent on whether a valid frame is
 * available or not
 * \note Called from QDebug_ProcessCommands in QDebug.c.
 */
  uint8_t Receive_Message (void);

/*! \brief Handles the incoming bytes from the interface selected in
 * QDebugSettings.h and puts the bytes in the RX_Buffer data read by
 * the selected interface
 * \return uint8_t: returns a true if more data must be read,
 * returns a false if the frame is complete
 * \note Used by SPI and TWI receive handlers.
 */
  uint8_t RxHandler (uint8_t c);
#endif  /* _DEBUG_INTERFACE_ */

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
