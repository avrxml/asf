/**************************************************************************
 *
 * \file
 *
 * \brief Management of the USB device generic HID QTouch debug task.
 *
 * This file manages the USB device generic HID QTouch debug task.
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


//_____  I N C L U D E S ___________________________________________________

#include "conf_usb.h"

#if USB_DEVICE_FEATURE == true

#include "board.h"
#include "usb_drv.h"
#include "gpio.h"
#include "pdca.h"
#include "intc.h"
#include "usart.h"
#include "usb_descriptors.h"
#include "usb_standard_request.h"
#include "cycle_counter.h"
#include "fifo.h"
#include "power_clocks_lib.h"

#include "qtouch_debug_frame.h"
#include "device_hid_qtouch_debug_task.h"

#include <string.h>


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________
#define RX_LED AVR32_PIN_PA20
#define TX_LED AVR32_PIN_PA21

#define USART               (&AVR32_USART1)
#define USART_BAUDRATE      115200
#define USART_RX_PIN        AVR32_USART1_RXD_0_0_PIN
#define USART_RX_FUNCTION   AVR32_USART1_RXD_0_0_FUNCTION
#define USART_TX_PIN        AVR32_USART1_TXD_0_0_PIN
#define USART_TX_FUNCTION   AVR32_USART1_TXD_0_0_FUNCTION
#define USART_IRQ           AVR32_USART1_IRQ


#define USART_RX_BUFFER     (EP_IN_LENGTH*2) // Unit is in characters.

#define AVR32_PDCA_PID_USART_TX       AVR32_PDCA_PID_USART1_TX
#define PDCA_CHANNEL_USART            0

//_____ D E C L A R A T I O N S ____________________________________________


static bool write_hid_message = false;
static int write_hid_message_index=0;

static struct hid_frames ack_message;

static bool send_nack = false;
static bool send_ack  = false;


static U16  sof_cnt;

static void hid_report_out  (void);
static void hid_report_in   (void);

struct hid_frames out_buffer; //sizeof must be EP_OUT_LENGTH

static int recieved_data=0; // Number of data received from USART


extern int jump_bootloader;
extern pcl_freq_param_t pcl_freq_param;

static fifo_desc_t   fifo_desc_usart;
static uint8_t       fifo_rx[USART_RX_BUFFER];

void usb_write_message(void* ptr_cram);


#if (defined __GNUC__) && (defined __AVR32__)
__attribute__((__interrupt__))
#elif (defined __ICCAVR32__)
__interrupt
#endif
static void int_handler_usart(void)
{
  if( USART->csr & AVR32_USART_RXRDY_MASK )
  {
    U8 i, status;
    i = (USART->rhr & AVR32_USART_RHR_RXCHR_MASK) >> AVR32_USART_RHR_RXCHR_OFFSET;
    status = fifo_push_uint8(&fifo_desc_usart, i);
    if( status==FIFO_ERROR_OVERFLOW )
    {
      // error
      gpio_clr_gpio_pin(RX_LED);
      gpio_clr_gpio_pin(TX_LED);
    }
  }
}

//! @brief This function initializes USART connected to main UC3 device
//! present on the kit.
//!
void init_rs232(unsigned long baudrate, long pba_hz)
{
  static const gpio_map_t USART_GPIO_MAP =
  {
    {USART_RX_PIN, USART_RX_FUNCTION},
    {USART_TX_PIN, USART_TX_FUNCTION}
  };

  // Options for debug USART.
  usart_options_t dbg_usart_options =
  {
    .baudrate = baudrate,
    .charlength = 8,
    .paritytype = USART_NO_PARITY,
    .stopbits = USART_1_STOPBIT,
    .channelmode = USART_NORMAL_CHMODE
  };

  // Setup GPIO for debug USART.
  gpio_enable_module(USART_GPIO_MAP,
                     sizeof(USART_GPIO_MAP) / sizeof(USART_GPIO_MAP[0]));

  // Initialize it in RS232 mode.
  usart_init_rs232(USART, &dbg_usart_options, pba_hz);
}


//!
//! @brief This function initializes the hardware/software resources
//! required for device generic HID task.
//!
void device_generic_hid_task_init(void)
{
  sof_cnt = 0;
  int i;

  // PDCA channel options
  static const pdca_channel_options_t PDCA_OPTIONS =
  {
    .addr   = (void *) serial_tx_frame   ,    // memory address
    .pid    = AVR32_PDCA_PID_USART_TX,        // select peripheral - data are transmit on USART TX line.
    .size   = 0 ,                             // transfer counter
    .r_addr = NULL,                           // next memory address
    .r_size = 0,                              // next transfer counter
    .transfer_size = PDCA_TRANSFER_SIZE_BYTE  // select size of the transfer
  };

  // Init USB HID Ack Message
  ack_message.protocol_id=PROTOCOL_ID;
  ack_message.spare_byte1=0x00;
  ack_message.spare_byte2=0x00;
  ack_message.packetCount=0;
  ack_message.packetNumber=0;
  ack_message.packetNumber=0;
  for(i=0;i<PAYLOAD_SIZE;i++) {
      ack_message.payload[i]=0x00;
   }

  // Initialize the PDCA channel with the pdca_options for the USART
  pdca_init_channel(PDCA_CHANNEL_USART, &PDCA_OPTIONS);

  // Initialize the FIFO module
  fifo_init( &fifo_desc_usart, &fifo_rx, USART_RX_BUFFER );

  // Register the USART interrupt handler to the interrupt controller.
  // Highest priority is required for the USART, since we do not want to loose
  // any characters.
  Disable_global_interrupt();
  INTC_register_interrupt(&int_handler_usart, USART_IRQ, AVR32_INTC_INT3);
  Enable_global_interrupt();

  // Clear all leds.
  gpio_set_gpio_pin(RX_LED);
  gpio_set_gpio_pin(TX_LED);

#ifndef FREERTOS_USED
  #if USB_HOST_FEATURE == true
  // If both device and host features are enabled, check if device mode is engaged
  // (accessing the USB registers of a non-engaged mode, even with load operations,
  // may corrupt USB FIFO data).
  if (Is_usb_device())
  #endif  // USB_HOST_FEATURE == true
    Usb_enable_sof_interrupt();
#endif  // FREERTOS_USED

}

//! @brief Entry point of the HID generic communication task
//! This function manages IN/OUT report management.
//!
void device_generic_hid_task(void)
{
   if(!Is_device_enumerated())          // Check USB HID is enumerated
      return;

   hid_check_vbus(); // Check if we vbus is on and
   hid_report_out(); // received data from USB
   hid_report_in();  // Send data to USB

   fill_tx_buffer();
}

//! @brief check if USB VBUS active.
//! If VBUS is not active, put the device in Stop mode until VBUS become active
//!
void hid_check_vbus(void)
{
  static bool b_startup=true;
  static U8   startup_state=0;

  if( Is_usb_vbus_low() )
    {
       U32 i;
       volatile avr32_pm_t *pm = &AVR32_PM;
       gpio_set_gpio_pin(RX_LED);
       gpio_set_gpio_pin(TX_LED);
       for( i=0 ; i<2 ; i++ )
       {
          gpio_tgl_gpio_pin(RX_LED);
          gpio_tgl_gpio_pin(TX_LED);
          cpu_delay_ms(100, pcl_freq_param.cpu_f);
       }

       pm->AWEN.usb_waken = 1;
       SLEEP(AVR32_PM_SMODE_STOP);
       pm->AWEN.usb_waken = 0;

       b_startup = true;
       startup_state = 0;

       for( i=0 ; i<2 ; i++ )
       {
          gpio_tgl_gpio_pin(RX_LED);
          gpio_tgl_gpio_pin(TX_LED);
          cpu_delay_ms(100, pcl_freq_param.cpu_f);
       }
    }

    if( b_startup )
    {
       if( startup_state==0 )
       {
         // Initialize usart comm
         init_rs232(USART_BAUDRATE, pcl_freq_param.pba_f);
         gpio_enable_pin_pull_up(USART_RX_PIN);
         USART->ier = AVR32_USART_IER_RXRDY_MASK;
         gpio_clr_gpio_pin(RX_LED);
       }
       else if( startup_state==1 ) gpio_clr_gpio_pin(TX_LED);
       else if( startup_state==2 ) gpio_set_gpio_pin(RX_LED);
       else if( startup_state==3 ) gpio_set_gpio_pin(TX_LED);
       else if( startup_state==4 ) gpio_clr_gpio_pin(RX_LED);
       else if( startup_state==5 ) gpio_clr_gpio_pin(TX_LED);
       else if( startup_state==6 ) gpio_set_gpio_pin(RX_LED);
       else if( startup_state==7 ) gpio_set_gpio_pin(TX_LED);
       else if( startup_state==8 ) b_startup=false;
       startup_state++;
       cpu_delay_ms(100, FOSC0);
    }
}

//! @brief Get data report from Host
//!
void hid_report_out(void)
{
   void* ptr_cram=& out_buffer;
   int none_read_byte;

   if(Is_usb_out_received(EP_HID_GENERIC_OUT))
   {
      // data received
      gpio_tgl_gpio_pin(RX_LED);
      Usb_reset_endpoint_fifo_access(EP_HID_GENERIC_OUT);
      none_read_byte = usb_read_ep_rxpacket(EP_HID_GENERIC_OUT, ptr_cram, EP_OUT_LENGTH, (void**)&ptr_cram);
      Usb_ack_out_received_free(EP_HID_GENERIC_OUT);
      if(none_read_byte==0) {
        // previous Message hasn't be read
        if((pdca_get_transfer_status(PDCA_CHANNEL_USART)&PDCA_TRANSFER_COMPLETE)==0) {
          // Flush Message and send Nack message
          send_nack = true;
        }
        else if(get_message(out_buffer)==MESSAGE_READY) {
           pdca_load_channel(PDCA_CHANNEL_USART, (void *)serial_tx_frame,((serial_tx_frame[1]<<8)| serial_tx_frame[2])+1 );
           pdca_enable(PDCA_CHANNEL_USART);
           // Store packet Count in send_ack, this will be use to send ack.
           send_ack = out_buffer.packetCount;
        }
      }
      // The Data has been read and stored in out_buffer
   }

   //** Check if we received DFU mode command from host
   if(jump_bootloader)
   {
      gpio_set_gpio_pin(LED0_GPIO);
      gpio_set_gpio_pin(LED1_GPIO);
      gpio_set_gpio_pin(LED2_GPIO);
      gpio_set_gpio_pin(LED3_GPIO);
      Usb_detach();                           // Detach actual generic HID application
      cpu_delay_ms(100, pcl_freq_param.cpu_f);// Wait some time before
      //start_boot();                         // Jumping to bootloader Not available yet
      SLEEP(AVR32_PM_SMODE_STOP);             // Stop mode instead of jump bootloader
   }
}


//! @brief Send data report to USB HID Host
//!
void hid_report_in(void)
{
  // No message to transmit yet
  void* ptr_cram;

  if(!Is_usb_write_enabled(EP_HID_GENERIC_IN))
      return;                                // Not ready to send report

  // Send report
  if(write_hid_message==true) {
    gpio_tgl_gpio_pin(TX_LED);

    ptr_cram = & (hid_rx_frame[write_hid_message_index++]);

    usb_write_message(ptr_cram);

    // If Message Fully send
    if(write_hid_message_index>=hid_rx_frame[0].packetNumber) {
       write_hid_message=false;
       recieved_data=0;
    }
  }
  // Send nack message if send_nack flag is true
  else if (send_nack == true) {
    ack_message.spare_byte2  = NACK_MESSAGE;
    ack_message.packetNumber = 0;
    ack_message.packetCount  = 0;
    usb_write_message(&ack_message);
    send_nack = false;

  }
  // Send ack message if send_ack flag is not equal to Zero
  // The send_ack value is the packet number of message to be acknowledged
  else if (send_ack != 0) {
    // Send Ack Message only if PDCA transfer is done
    if((pdca_get_transfer_status(PDCA_CHANNEL_USART)&PDCA_TRANSFER_COMPLETE)==0) {
      ack_message.spare_byte2  = ACK_MESSAGE;
      ack_message.packetNumber = send_ack;
      ack_message.packetCount  = send_ack;
      usb_write_message(&ack_message);
      send_ack = 0;
    }
  }
}

//! @brief Write message to USB HID Host
//!
void usb_write_message(void* ptr_cram)
{
  int data_to_transfer=EP_IN_LENGTH;
  while (data_to_transfer) {
    while (!Is_usb_in_ready(EP_HID_GENERIC_IN));
        Usb_reset_endpoint_fifo_access(EP_HID_GENERIC_IN);
        data_to_transfer = usb_write_ep_txpacket(EP_HID_GENERIC_IN, ptr_cram, data_to_transfer, (const void**)&ptr_cram);
        Usb_ack_in_ready_send(EP_HID_GENERIC_IN);
    }
}

//! @brief Send data report to USB HID Host
//!
void fill_tx_buffer(void)
{
  U8 c;
  int size;
  if(recieved_data>2) {
    // Do not forget to add MSG_Start byte in length
    size = (serial_rx_frame[1]<<8|serial_rx_frame[2])+1;
  }
  else {
    size=sizeof(serial_rx_frame);
  }

  while((fifo_get_used_size(&fifo_desc_usart)>0)&&(recieved_data<size)) {
    if(fifo_pull_uint8(&fifo_desc_usart, &c)==FIFO_OK) {
      serial_rx_frame[recieved_data++] = c;
    }
    else {
      gpio_clr_gpio_pin(RX_LED);
      gpio_clr_gpio_pin(TX_LED);
    }

    // Resynch until new char is ready
    if((recieved_data>=1)&&(serial_rx_frame[0])!=0x1B) {
      recieved_data=0;
      size=sizeof(serial_rx_frame);
    }
  }

  //can send message until previous is not fully sent
  if((recieved_data>=size)&&(write_hid_message==false)) {
    send_message(serial_rx_frame,size);
    write_hid_message=true;
    write_hid_message_index=0;
  }
}


//!
//! @brief usb_sof_action
//!
//! This function increments the sof_cnt counter each time
//! the USB Start-of-Frame interrupt subroutine is executed (1 ms).
//! Useful to manage time delays
//!
void usb_sof_action(void)
{
  sof_cnt++;
}


#endif  // USB_DEVICE_FEATURE == true
