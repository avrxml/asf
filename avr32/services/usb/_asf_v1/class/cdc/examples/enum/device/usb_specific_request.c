/**************************************************************************
 *
 * \file
 *
 * \brief Processing of USB device specific enumeration requests.
 *
 * This file contains the specific request decoding for enumeration process.
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


//_____ I N C L U D E S ____________________________________________________

#include "conf_usb.h"


#if USB_DEVICE_FEATURE == true

#include "usb_drv.h"
#include "usb_descriptors.h"
#include "usb_standard_request.h"
#include "usb_specific_request.h"
#include "cdc.h"
#include "gpio.h"
#include "usart.h"
#include "power_clocks_lib.h"


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________


//_____ P R I V A T E   D E C L A R A T I O N S ____________________________

S_line_coding   line_coding;

extern pcl_freq_param_t pcl_freq_param;
extern const    void *pbuffer;
extern          uint16_t   data_to_transfer;

//_____ D E C L A R A T I O N S ____________________________________________

//! @brief This function configures the endpoints of the device application.
//! This function is called when the set configuration request has been received.
//!
void usb_user_endpoint_init(uint8_t conf_nb)
{
#if (USB_HIGH_SPEED_SUPPORT==true)
   if( !Is_usb_full_speed_mode() )
   {
      (void)Usb_configure_endpoint(TX_EP,
                             EP_ATTRIBUTES_1,
                             DIRECTION_IN,
                             EP_SIZE_1_HS,
                             DOUBLE_BANK);

      (void)Usb_configure_endpoint(RX_EP,
                             EP_ATTRIBUTES_2,
                             DIRECTION_OUT,
                             EP_SIZE_2_HS,
                             DOUBLE_BANK);
      (void)Usb_configure_endpoint(INT_EP,
                             EP_ATTRIBUTES_3,
                             DIRECTION_IN,
                             EP_SIZE_3,
                             SINGLE_BANK);
      return;
   }
#endif
  (void)Usb_configure_endpoint(TX_EP,
                         EP_ATTRIBUTES_1,
                         DIRECTION_IN,
                         EP_SIZE_1_FS,
                         DOUBLE_BANK);
  (void)Usb_configure_endpoint(RX_EP,
                         EP_ATTRIBUTES_2,
                         DIRECTION_OUT,
                         EP_SIZE_2_FS,
                         DOUBLE_BANK);
  (void)Usb_configure_endpoint(INT_EP,
                         EP_ATTRIBUTES_3,
                         DIRECTION_IN,
                         EP_SIZE_3,
                         SINGLE_BANK);
}


//! This function is called by the standard USB read request function when
//! the USB request is not supported. This function returns true when the
//! request is processed. This function returns false if the request is not
//! supported. In this case, a STALL handshake will be automatically
//! sent by the standard USB read request function.
//!
bool usb_user_read_request(uint8_t type, uint8_t request)
{
  switch (request)
  {
    case GET_LINE_CODING:
      cdc_get_line_coding();
      return true;
      // No need to break here !

    case SET_LINE_CODING:
      cdc_set_line_coding();
      return true;
      // No need to break here !

    case SET_CONTROL_LINE_STATE:
      cdc_set_control_line_state();
      return true;
      // No need to break here !

    default:
      return false;
      // No need to break here !
  }
}


//! This function returns the size and the pointer on a user information
//! structure
//!
bool usb_user_get_descriptor(uint8_t type, uint8_t string)
{
  pbuffer = NULL;

  switch (type)
  {
  case STRING_DESCRIPTOR:
    switch (string)
    {
    case LANG_ID:
      data_to_transfer = sizeof(usb_user_language_id);
      pbuffer = &usb_user_language_id;
      break;

    case MAN_INDEX:
      data_to_transfer = sizeof(usb_user_manufacturer_string_descriptor);
      pbuffer = &usb_user_manufacturer_string_descriptor;
      break;

    case PROD_INDEX:
      data_to_transfer = sizeof(usb_user_product_string_descriptor);
      pbuffer = &usb_user_product_string_descriptor;
      break;

    case SN_INDEX:
      data_to_transfer = sizeof(usb_user_serial_number);
      pbuffer = &usb_user_serial_number;
      break;

    default:
      break;
    }
    break;

  default:
    break;
  }

  return pbuffer != NULL;
}

void cdc_get_line_coding(void)
{
   Usb_ack_setup_received_free();

   Usb_reset_endpoint_fifo_access(EP_CONTROL);
   Usb_write_endpoint_data(EP_CONTROL, 8, LSB0(line_coding.dwDTERate));
   Usb_write_endpoint_data(EP_CONTROL, 8, LSB1(line_coding.dwDTERate));
   Usb_write_endpoint_data(EP_CONTROL, 8, LSB2(line_coding.dwDTERate));
   Usb_write_endpoint_data(EP_CONTROL, 8, LSB3(line_coding.dwDTERate));
   Usb_write_endpoint_data(EP_CONTROL, 8, line_coding.bCharFormat);
   Usb_write_endpoint_data(EP_CONTROL, 8, line_coding.bParityType);
   Usb_write_endpoint_data(EP_CONTROL, 8, line_coding.bDataBits  );

   Usb_ack_control_in_ready_send();
   while (!Is_usb_control_in_ready());

   while(!Is_usb_control_out_received());
   Usb_ack_control_out_received_free();
}

void cdc_set_line_coding (void)
{
   Usb_ack_setup_received_free();

   while(!Is_usb_control_out_received());
   Usb_reset_endpoint_fifo_access(EP_CONTROL);

   LSB0(line_coding.dwDTERate) = Usb_read_endpoint_data(EP_CONTROL, 8);
   LSB1(line_coding.dwDTERate) = Usb_read_endpoint_data(EP_CONTROL, 8);
   LSB2(line_coding.dwDTERate) = Usb_read_endpoint_data(EP_CONTROL, 8);
   LSB3(line_coding.dwDTERate) = Usb_read_endpoint_data(EP_CONTROL, 8);
   line_coding.bCharFormat = Usb_read_endpoint_data(EP_CONTROL, 8);
   line_coding.bParityType = Usb_read_endpoint_data(EP_CONTROL, 8);
   line_coding.bDataBits = Usb_read_endpoint_data(EP_CONTROL, 8);
   Usb_ack_control_out_received_free();

   Usb_ack_control_in_ready_send();
   while (!Is_usb_control_in_ready());

   // Set the baudrate of the USART
   {
      static usart_options_t dbg_usart_options;
      uint32_t stopbits, parity;

      if     ( line_coding.bCharFormat==0 )   stopbits = USART_1_STOPBIT;
      else if( line_coding.bCharFormat==1 )   stopbits = USART_1_5_STOPBITS;
      else                                    stopbits = USART_2_STOPBITS;

      if     ( line_coding.bParityType==0 )   parity = USART_NO_PARITY;
      else if( line_coding.bParityType==1 )   parity = USART_ODD_PARITY;
      else if( line_coding.bParityType==2 )   parity = USART_EVEN_PARITY;
      else if( line_coding.bParityType==3 )   parity = USART_MARK_PARITY;
      else                                    parity = USART_SPACE_PARITY;

      // Options for debug USART.
      dbg_usart_options.baudrate    = line_coding.dwDTERate;
      dbg_usart_options.charlength  = line_coding.bDataBits;
      dbg_usart_options.paritytype  = parity;
      dbg_usart_options.stopbits    = stopbits;
      dbg_usart_options.channelmode = USART_NORMAL_CHMODE;

      // Initialize it in RS232 mode.
      usart_init_rs232(DBG_USART, &dbg_usart_options, pcl_freq_param.pba_f);

      // Enable Rx interrupts
      DBG_USART->ier = AVR32_USART_IER_RXRDY_MASK;
   }
}

void cdc_set_control_line_state (void)
{
   Usb_ack_setup_received_free();
   Usb_ack_control_in_ready_send();
   while (!Is_usb_control_in_ready());
}


#endif  // USB_DEVICE_FEATURE == true
