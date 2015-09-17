/*******************************************************************************
*   $FILE:  qdebug_transport.c
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
#ifdef _DEBUG_INTERFACE_

/*----------------------------------------------------------------------------
include files
----------------------------------------------------------------------------*/
#include "touch_api.h"
#include "qdebug.h"
#include "qdebug_transport.h"
#include "qdebug_settings.h"
#if defined(QDEBUG_RF4CE)
      #include "vendor_data.h"
	  #include "app_config.h"
#else
   #warning "No Debug Interface is selected in QDebugSettings.h"
#endif

/*----------------------------------------------------------------------------
global variables
----------------------------------------------------------------------------*/

#ifdef _TOUCH_ARM_
#ifdef __ICCARM__          // IAR
#pragma data_alignment=16  // IAR
#define __attribute__(...) // IAR
#endif                     // IAR
__attribute__((aligned(16)))
#endif /* TOUCH_ARM */

uint8_t TX_Buffer[TX_BUFFER_SIZE];

uint8_t RX_Buffer[RX_BUFFER_SIZE];
uint16_t TX_index;
uint16_t RX_index;
uint8_t SequenceL=0;
uint8_t SequenceH=0;

/**
 * @brief Initialize the send and receive buffers
 * 
 * Called from QDebug_Init
 */
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

/**
 * @brief Puts one byte in the Transmit Buffer
 * @ingroup  QDebug-Remote 
 * TX_index is post incremented
 */
void PutChar(uint8_t data)
{
   TX_Buffer[TX_index++] = data;   
}

/**
 * @brief Puts two bytes in the Transmit Buffer
 * @ingroup  QDebug-Remote
 * Big Endian. TX_index is post incremented
 */
void PutInt(uint16_t data)
{
   TX_Buffer[TX_index++] = data >> 8;
   TX_Buffer[TX_index++] = data;
}

/**
 * @brief Get one byte from the Receive Buffer
 * @ingroup  QDebug-Remote 
 * RX_index is post incremented
 */
uint8_t GetChar(void)
{
   return RX_Buffer[RX_index++];
}

/**
 * @brief Send the content of the TX_Buffer to the USB Bridge using the
 * interface selected in QDebugSettings.h
 * @ingroup  QDebug-Remote
 * Called from the transmit functions in QDebug.c
 */
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
   #if defined (QDEBUG_RF4CE)
      //vendor_data_request(0, PROFILE_ID_VENDOR_DATA, NWKC_VENDOR_IDENTIFIER, TX_Buffer[2] + 1, &TX_Buffer[0], TX_OPTIONS);
      zrc_cmd_request(0, 0x0000, USER_CONTROL_PRESSED, TX_Buffer[2] + 1, &TX_Buffer[0], TX_OPTIONS);
   #endif
}

/**
 * @brief Handles the incomming bytes from the interface selected in
 *         QDebugSettings.h and puts the bytes in the RX_Buffer
 * @ingroup  QDebug-Remote
 * Used by SPI and TWI receive handlers
 *
 * @param c data read by the selected interface
 * @return returns a true if more data must be read
 *          returns a false if the frame is complete
 */
uint8_t RxHandler(uint8_t c)
{
   static uint16_t length = 0;
   static uint16_t received = 0;
   static uint8_t state = STATE_IDLE;
   uint8_t nextstate;
   uint8_t checksum;
   uint16_t i;
   
   if(c == MESSAGE_START)
     state = STATE_IDLE;

   switch (state)
   {
   case STATE_IDLE:
      if (c == MESSAGE_START)
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
      if ((length < 4) || (length > 255))
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
            RX_Buffer[0] = MESSAGE_START;
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
#endif
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-EOF-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
