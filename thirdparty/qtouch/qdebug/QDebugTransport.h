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
 * - Support: http://www.atmel.com/design-support/
 *
 *
 * Copyright (c) 2010-2015 Atmel Corporation. All rights reserved.
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
