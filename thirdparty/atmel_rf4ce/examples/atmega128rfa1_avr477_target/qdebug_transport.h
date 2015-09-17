/******************************************************************************* 
*   $FILE:  qdebug_transport.h
*   Atmel Corporation:  http://www.atmel.com \n
*   Support: http://www.atmel.com/design-support/
******************************************************************************/

/*  License
 * Copyright (c) 2010-2015 Atmel Corporation. All rights reserved.
*   
*   Redistribution and use in source and binary forms, with or without
*   modification, are permitted provided that the following conditions are met:
*   
*   1. Redistributions of source code must retain the above copyright notice,
*   this list of conditions and the following disclaimer.
*   
*   2. Redistributions in binary form must reproduce the above copyright notice,
*   this list of conditions and the following disclaimer in the documentation
*   and/or other materials provided with the distribution.
*   
*   3. The name of ATMEL may not be used to endorse or promote products derived
*   from this software without specific prior written permission.
*   
*   THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
*   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
*   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
*   SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
*   INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
*   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
*   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
*   THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef DEBUGTRANSPORT_H_INCLUDED
#define DEBUGTRANSPORT_H_INCLUDED

/*============================ INCLUDES ======================================*/
//#include "touch_api.h"
/*----------------------------------------------------------------------------
                            nested include files
----------------------------------------------------------------------------*/
#if defined (__GNUC__) || defined (__ICCARM__) ||defined (__CC_ARM) || defined (__ICCAVR__)
    #include <stdint.h>
#endif

#if !defined (__CC_ARM)
    #include <stdbool.h>
#endif
#include <stdlib.h>

#ifdef __ICCAVR__
    #include <intrinsics.h>
#endif /* (__ICCAVR__) */

/*============================ MACROS ========================================*/
#define _ROTOR_SLIDER_
#define QT_NUM_CHANNELS     25

#define MESSAGE_START	0x1B

#ifdef _ROTOR_SLIDER_
#define TX_BUFFER_SIZE (QT_NUM_CHANNELS*4)+10
#define RX_BUFFER_SIZE (QT_NUM_CHANNELS*4)+10
#else
#define TX_BUFFER_SIZE (QT_NUM_CHANNELS*3)+10
#define RX_BUFFER_SIZE (QT_NUM_CHANNELS*3)+10
#endif

#define STATE_IDLE		0
#define STATE_LENGTH1	1
#define STATE_LENGTH2	2
#define STATE_DATA		3

#define INT(a, b)	((a << 8) | b)

/*============================ GLOBAL VARIABLES ==============================*/
// TX Buffer globals
extern uint8_t TX_Buffer[TX_BUFFER_SIZE];
extern uint16_t TX_index;

// RX Buffer globals
extern uint8_t RX_Buffer[RX_BUFFER_SIZE];
extern uint16_t RX_index;

extern uint8_t SequenceL;
extern uint8_t SequenceH;

extern unsigned int gMsTimeout;  // The ms timeout variable,
// only access with NOT_TIMEDOUT and SET_TIMEOUT

extern uint8_t RF4CE_RX_Buffer[RX_BUFFER_SIZE];


/*============================ PROTOTYPES ====================================*/

void Init_Buffers(void);
void Set_datamode(unsigned char mode);


// Send functions
void PutChar(uint8_t data);
void PutInt(uint16_t data);
uint8_t GetChar(void);
void Send_Message(void);

// Receive functions
uint8_t Receive_Message(void);
uint8_t RxHandler(uint8_t c);



#endif
/* EOF */
