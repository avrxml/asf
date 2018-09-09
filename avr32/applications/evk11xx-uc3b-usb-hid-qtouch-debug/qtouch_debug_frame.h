/**
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
#ifndef _QTOUCH_HID_H_
#define _QTOUCH_HID_H_

#define PROTOCOL_ID            0x01
#define PAYLOAD_SIZE           60
#define MAX_RX_PACKET_NUMBER   15
#define MAX_TX_PACKET_NUMBER   15

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

extern struct hid_frames hid_rx_frame[MAX_TX_PACKET_NUMBER];
extern char serial_rx_frame[MAX_TX_PACKET_NUMBER*PAYLOAD_SIZE];

extern unsigned int rx_index;

extern struct hid_frames hid_tx_frame[MAX_RX_PACKET_NUMBER];
extern char serial_tx_frame[MAX_TX_PACKET_NUMBER*PAYLOAD_SIZE];
extern unsigned int tx_index;


int get_message(struct hid_frames frame);
void send_message(char *message, unsigned int size);

#endif //_QTOUCH_HID_H_
