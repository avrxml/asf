/**************************************************************************
 *
 * \file
 *
 * \brief file for QTouch debug frame.
 *
 * These files contain two functions that are used to decode or encode the QTouch debug message
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
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


#include "qtouch_debug_frame.h"

// RX USB frame and serial buffer
struct hid_frames hid_rx_frame[MAX_TX_PACKET_NUMBER];
char serial_rx_frame[MAX_TX_PACKET_NUMBER*PAYLOAD_SIZE];
unsigned int rx_index =0;

// TX USB frame and serial buffer
struct hid_frames hid_tx_frame[MAX_RX_PACKET_NUMBER];
char serial_tx_frame[MAX_TX_PACKET_NUMBER*PAYLOAD_SIZE];
unsigned int tx_index =0;

//! @brief This function get a messages from USB and rebuild it in a single
//   message for USART transfer to the main UC3 device present on the kit.
//  /param frame a frame received from USB
//!
int get_message(struct hid_frames frame)
{
  int index=0;
  int i=0;
  int j=0;
  // Error Handling
  if((frame.protocol_id!=PROTOCOL_ID)||
     (frame.packetNumber!=tx_index+1) ||
     (frame.packetNumber>frame.packetCount)) {
    // Flush Message if PROTOCOL_ID is not correct
    tx_index=0;
    return MESSAGE_ERROR;
  }

  // Store incoming packet in tx_frame_structure
  hid_tx_frame[tx_index++]=frame;

  // if we have a full packet, this can be transmitted the QTouch device
  if(frame.packetCount==frame.packetNumber) {

    // rebuild and send message to QTouch device
    for(i=0;i<frame.packetCount;i++) {
       for(j=0;j<PAYLOAD_SIZE;j++) {
            serial_tx_frame[index++]=hid_tx_frame[i].payload[j];
       }
    }
    tx_index=0;
    return MESSAGE_READY;
  }
  return MESSAGE_PENDING;
}

//! @brief This function split message received from USART to USB frame messages.
//   Then the HID_rx_frame is used to store the split message.
//  /param message a pointer to the rx_buffer
//  /param size of the message stored in the rx_buffer
//!
void send_message(char *message, unsigned int size)
{
  int index=0;
  int i=0;
  int j=0;
  int num_packet;

  if(size>PAYLOAD_SIZE*MAX_TX_PACKET_NUMBER)
     return;

  // Compute num Packet
  num_packet=(size/PAYLOAD_SIZE)+1;
  for(i = 0;i<num_packet;i++) {
    hid_rx_frame[i].protocol_id=PROTOCOL_ID;
    hid_rx_frame[i].spare_byte1=0x00;
    hid_rx_frame[i].spare_byte2=0x00;
    hid_rx_frame[i].packetCount=num_packet;
    hid_rx_frame[i].packetNumber=i+1;
    for(j=0;j<PAYLOAD_SIZE&&index<size;j++) {
      hid_rx_frame[i].payload[j]=message[index++];
    }
  }
}
