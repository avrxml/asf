/**
 * \file qdebugtransport.h
 *
 * \brief API of the QDebug transport layer
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
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef DEBUGTRANSPORT_H_INCLUDED
#define DEBUGTRANSPORT_H_INCLUDED

/*============================ INCLUDES ======================================*/
#include "touch_api_at32uc3l.h"

/*============================ MACROS ========================================*/

#define MESSAGE_START	0x1B

#if DEF_TOUCH_QMATRIX == 1
  #define TX_BUFFER_SIZE (QM_NUM_CHANNELS*4)+10
  #define RX_BUFFER_SIZE (QM_NUM_CHANNELS*4)+10
#elif (DEF_TOUCH_QTOUCH_GRP_A == 1)
  #define TX_BUFFER_SIZE (QTA_NUM_CHANNELS*4)+10
  #define RX_BUFFER_SIZE (QTA_NUM_CHANNELS*4)+10
#elif (DEF_TOUCH_QTOUCH_GRP_A == 1)
  #define TX_BUFFER_SIZE (QTB_NUM_CHANNELS*4)+10
  #define RX_BUFFER_SIZE (QTB_NUM_CHANNELS*4)+10
#elif DEF_TOUCH_AUTONOMOUS_QTOUCH == 1
  #define TX_BUFFER_SIZE (1*4)+10
  #define RX_BUFFER_SIZE (1*4)+10
#endif

#define STATE_IDLE	0
#define STATE_LENGTH1	1
#define STATE_LENGTH2	2
#define STATE_DATA	3

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
