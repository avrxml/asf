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
#ifndef _QTOUCH_HID_H_
#define _QTOUCH_HID_H_


#include <stdint.h> 

#define QDEBUG_PROTOCOL_ID     0x01
#define RX_BUFFER_SIZE         300
#define PAYLOAD_SIZE           60
#define MAX_RX_PACKET_NUMBER   15
#define MAX_TX_PACKET_NUMBER   15

#define END_OF_HID_BUFFER_PAYLOAD     0x3B

//! @ingroup QDebug-Target
//! List of the command IDs used in QDebug.
//! @{
#define QT_SIGN_ON                   0x21
#define QT_GLOBAL_CONFIG               0x22
#define QT_SENSOR_CONFIG               0x23
#define QT_SIGNALS                   0x24
#define QT_REFERENCES                  0x25
#define QT_DELTAS                      0x26
#define QT_STATES                      0x27

#define QT_MESSAGE_START              0x1B
#define QT_CHANNEL_DATA_START         0x05
#define QT_PAYLOAD_LOCATION           0x09 //after 32 channel data,this applies only for QT_SIGNALS and QT_REFERENCES
#define QT_NUM_CHANNELS               32
//! @}

// Ack byte definition
#define NOMRAL_MESSAGE 0X00
#define ACK_MESSAGE    0x40
#define NACK_MESSAGE   0x80
#define SPARE_MESSAGE  0xC0

#define MESSAGE_ERROR   -1
#define MESSAGE_PENDING  1
#define MESSAGE_READY    2

typedef struct hid_frames {
  unsigned int protocol_id            : 8;
  unsigned int spare_byte1            : 8;
  unsigned int spare_byte2            : 8;
  unsigned int packetCount            : 4;
  unsigned int packetNumber           : 4;
  char         payload[PAYLOAD_SIZE];
} hid_frames ;


extern struct hid_frames hid_rx_frame[4];

extern uint8_t RF4CE_RX_Buffer[RX_BUFFER_SIZE];
extern unsigned int rx_index;

extern struct hid_frames hid_tx_frame[MAX_RX_PACKET_NUMBER];
extern unsigned int tx_index;

extern uint8_t data_to_transfer_qt_cont;

int get_message(struct hid_frames frame,uint8_t*);
void send_message(char *message, unsigned int size);

#endif //_QTOUCH_HID_H_
