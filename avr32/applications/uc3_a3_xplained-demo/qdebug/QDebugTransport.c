/*******************************************************************************
*   $FILE:  QDebugTransport.c
*   Atmel Corporation:  http://www.atmel.com \n
*   Support: http://www.atmel.com/design-support/
******************************************************************************/

/**
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
 */

/*============================ INCLUDES ======================================*/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include "touch_api.h"
#include "QDebug.h"
#include "QDebugTransport.h"
#include "QDebugSettings.h"

#if defined(QDEBUG_SPI)
#include "SPI_Master.h"
#elif defined(QDEBUG_TWI)
#include "TWI_Master.h"
#elif defined(QDEBUG_SPI_BB)
#include "BitBangSPI_Master.h"
#else
#warning "No Debug Interface is selected in QDebugSettings.h"
#endif

/*============================ GLOBAL VARIABLES ==============================*/

uint8_t TX_Buffer[TX_BUFFER_SIZE];
uint16_t TX_index;

uint8_t RX_Buffer[RX_BUFFER_SIZE];
uint16_t RX_index;

uint8_t SequenceL=0;
uint8_t SequenceH=0;

/*============================ IMPLEMENTATION ================================*/

/*============================================================================
Name    :   Init_Buffers
------------------------------------------------------------------------------
Purpose :   Initialize the send and receive buffers
Input   :   n/a
Output  :   n/a
Notes   :	Called from QDebug_Init
============================================================================*/
void Init_Buffers(void)
{
#ifdef __DEBUG__
   int i;

   for (i = 0; i < sizeof(TX_Buffer); i++)
   {
      RX_Buffer[i] = 0;
      RX_Buffer[i] = 0;
   }
#endif

   TX_Buffer[0] = MESSAGE_START;
   TX_index = 4; // Start to add data after MSG_START, MSG_SIZEH, MSG_SIZEL, and Sequence number
   RX_index=0;
}

/*============================================================================
Name    :   PutChar
------------------------------------------------------------------------------
Purpose :   Puts one byte in the Transmit Buffer
Input   :   n/a
Output  :   n/a
Notes   :	TX_index is post incremented
============================================================================*/
void PutChar(uint8_t data)
{
   TX_Buffer[TX_index++] = data;
}

/*============================================================================
Name    :   PutInt
------------------------------------------------------------------------------
Purpose :   Puts two bytes in the Transmit Buffer
Input   :   n/a
Output  :   n/a
Notes   :	Big Endian. TX_index is post incremented
============================================================================*/
void PutInt(uint16_t data)
{
   TX_Buffer[TX_index++] = data >> 8;
   TX_Buffer[TX_index++] = data;
}

/*============================================================================
Name    :   GetChar
------------------------------------------------------------------------------
Purpose :   Get one byte from the Receive Buffer
Input   :   n/a
Output  :   n/a
Notes   :	RX_index is post incremented
============================================================================*/
uint8_t GetChar(void)
{
   return RX_Buffer[RX_index++];
}

/*============================================================================
Name    :   Send_Message
------------------------------------------------------------------------------
Purpose :   Send the content of the TX_Buffer to the USB Bridge using the
interface selected in QDebugSettings.h
Input   :   n/a
Output  :   n/a
Notes   :	Called from the transmit functions in QDebug.c
============================================================================*/
void Send_Message(void)
{
   uint16_t i;
   uint8_t checksum = 0;

   SequenceL= (SequenceL+1)& 0x0F;	// inc and wrap

   // Store length field
   TX_Buffer[1] = (uint8_t)(TX_index >> 8);
   TX_Buffer[2] = (uint8_t)(TX_index & 0xFF);

   // Store Sequence Number
   TX_Buffer[3] = (SequenceH<<4)+SequenceL;

   // Calculate checksum
   for (i=1; i < TX_index; i++)
      checksum ^= TX_Buffer[i];
   TX_Buffer[TX_index] = checksum;

   // Send data using selected interface
#if defined(QDEBUG_SPI)
   SPI_Send_Message();
#elif defined(QDEBUG_TWI)
   TWI_Send_Message();
#elif defined(QDEBUG_SPI_BB)
   BitBangSPI_Send_Message();
#endif

   // Ready for next message
   TX_index = 4;
}

/*============================================================================
Name    :   Receive_Message
------------------------------------------------------------------------------
Purpose :   Executes a master read transmission if TWI is selected as interface
Checks if RX_Buffer has a valid frame
Input   :   n/a
Output  :   returns a true or false dependant on whether a valid frame is
available or not
Notes   :	Called from QDebug_ProcessCommands in QDebug.c
============================================================================*/
uint8_t Receive_Message(void)
{
#if defined(QDEBUG_TWI)
   static uint8_t count=0;

   count++;
   if (count == 10)
   {
      count=0;
      TWI_Retrieve_Message();
   }
   else
   {
      return 0;
   }
#endif

   // Check for token
   if (RX_Buffer[0] != 0x1B)
      return 0;

   // Yes, we have received a valid frame!
   RX_index = 4;	// Next GetChar() will get the command id
   return 1;
}

/*============================================================================
Name    :   RxHandler
------------------------------------------------------------------------------
Purpose :   Handles the incoming bytes from the interface selected in
QDebugSettings.h and puts the bytes in the RX_Buffer
Input   :   data read by the selected interface
Output  :   returns a true if more data must be read
returns a false if the frame is complete
Notes   :	Used by SPI and TWI receive handlers
============================================================================*/
uint8_t RxHandler(uint8_t c)
{
   static uint16_t length = 0;
   static uint16_t received = 0;
   static uint8_t state = STATE_IDLE;
   uint8_t nextstate;
   uint8_t checksum;
   uint16_t i;

   switch (state)
   {
   case STATE_IDLE:
      if (c == 0x1B)
      {
         nextstate = STATE_LENGTH1;
      }
      else
         nextstate = STATE_IDLE;
      break;

   case STATE_LENGTH1:
      RX_Buffer[1] = c;
      length = c << 8;
      nextstate = STATE_LENGTH2;
      break;

   case STATE_LENGTH2:
      RX_Buffer[2] = c;
      length |= c;
      if ((length < 4) || (length > 270))
      {
         // Illegal length, discard it
         nextstate = STATE_IDLE;
      }
      else
      {
         received = 3;
         nextstate = STATE_DATA;
      }
      break;

   case STATE_DATA:
      RX_Buffer[received] = c;
      if (received == length)
      {
         // This is the CRC byte
         checksum = 0;
         for (i = 1; i <= length; i++)
            checksum ^= RX_Buffer[i];

         if (checksum == 0)
         {
            // Valid frame
            RX_Buffer[0] = 0x1B;
         }
         nextstate = STATE_IDLE;
         break;
      }
      nextstate = STATE_DATA;
      received++;;
      break;

   default:
      nextstate = STATE_IDLE;
   }
   state = nextstate;
   return state;
}
