/*! \file ******************************************************************
 *
 * \brief file for Qtouch debug frame handling.
 *
 * These files contain two functions that are used to decode or encode the Qtouch debug message
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices with a USB module can be used.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://www.atmel.com/design-support/
 *
 ***************************************************************************/

/* Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an Atmel
 * AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include "qtouch_debug_frame.h"
#include <stdint.h> 
#include "pal.h"


/** @defgroup QDebug-Target QDebug Related API's(Target)
*
*/

/* === GLOBALS ============================================================= */
// RX USB frame and serial buffer
struct hid_frames hid_rx_frame[4];
uint8_t RF4CE_RX_Buffer[RX_BUFFER_SIZE];

unsigned int rx_index =0;

// TX USB frame and serial buffer
struct hid_frames hid_tx_frame[MAX_RX_PACKET_NUMBER];
unsigned int tx_index =0;

uint8_t data_to_transfer_qt_cont = 0;
/* === MACROS ============================================================== */

/* === IMPLEMENTATION ====================================================== */

/**
 *  @brief This function gets a message from QTouch Studio over USB and rebuilds
 *  it in a single message for RF4CE transfer to the Touch Remote.
  
 *  @param frame a frame received from USB
 *  @ingroup QDebug-Target
 */
int get_message(struct hid_frames frame,uint8_t *RF4CE_RX_Buffer)
{
    int index=0;
    int i=0;
    int j=0;
    /**
     * Error Handling
    */
    if((frame.protocol_id!=QDEBUG_PROTOCOL_ID)||
       (frame.packetNumber!=tx_index+1) ||
       (frame.packetNumber>frame.packetCount)) 
    {
        // Flush Message if QDEBUG_PROTOCOL_ID is not correct
        tx_index=0;
        return MESSAGE_ERROR;
    }
  
    /**
     * Store incoming packet in tx_frame_structure
     */
    hid_tx_frame[tx_index++]=frame;
  
    /**
     * if we have a full packet, this can be transmitted over air
     * to the AVR477 Remote
    */
    if(frame.packetCount==frame.packetNumber) {

        // rebuild and send message to Qtouch device
        for(i=0;i<frame.packetCount;i++) {
           for(j=0;j<PAYLOAD_SIZE;j++) {
               RF4CE_RX_Buffer[index++]=hid_tx_frame[i].payload[j];/* HID support*/
            }
        }
        if(index > RX_BUFFER_SIZE)
        return MESSAGE_ERROR;  
        
        tx_index=0;
        return MESSAGE_READY;
    }
    return MESSAGE_PENDING;
}

/**
 *  @brief This function transfers received RF4CE Touch data to QTouch Studio
 *  over USB HID
  
 *  @param message a pointer to the rx_buffer from RF4CE
 *  @param size of the message stored in the rx_buffer

 *  @ingroup QDebug-Target
 */
void send_message(char *message, unsigned int size)
{
    int index=0;
    int i=0;
    int j=0;
    int k=0;
    int num_packet=0;
  
    if(size>PAYLOAD_SIZE*MAX_TX_PACKET_NUMBER) 
        return;
  
    // Compute num Packet
    num_packet=(size/PAYLOAD_SIZE)+1;
 
    switch(message[4])
    {
        case QT_SIGN_ON:
           i=0;
           data_to_transfer_qt_cont = 1;
           break;
  
        case QT_GLOBAL_CONFIG:
            i=1;
            data_to_transfer_qt_cont = 1;
            break;
 
        case QT_SENSOR_CONFIG:
            i=2;
            data_to_transfer_qt_cont = 1;
            break;
  
        case QT_SIGNALS:
            i=0;
           data_to_transfer_qt_cont = 2;
            break;
    
        case QT_REFERENCES:
            i=0; 
            data_to_transfer_qt_cont = 3;
            break;
    
        case QT_DELTAS:
            i=0;
            data_to_transfer_qt_cont = 4;
            break;
    
        case QT_STATES:
            i=0;
            data_to_transfer_qt_cont = 5;
            break;
    
        default:
            i=15;
            break;
    }

    for(k = 0;k<num_packet;k++)
    {
        memset(&hid_rx_frame[i+k], 0, 64);
        hid_rx_frame[i+k].protocol_id=QDEBUG_PROTOCOL_ID;
        hid_rx_frame[i+k].spare_byte1=0x00;
        hid_rx_frame[i+k].spare_byte2=0x00;
        hid_rx_frame[i+k].packetCount=  num_packet; 
        hid_rx_frame[i+k].packetNumber=k+1;     
    
        for(j=0;j<PAYLOAD_SIZE&&index<size;j++) 
        {
            hid_rx_frame[i+k].payload[j]=message[index++]; 
        }
    }
}
/*EOF*/
