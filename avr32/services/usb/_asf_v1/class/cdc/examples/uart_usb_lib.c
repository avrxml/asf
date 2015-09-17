/**************************************************************************
 *
 * \file
 *
 * \brief This file controls the UART USB functions.
 *
 * These functions allow to use en USB endpoint as we would do using an UART.
 * This is particularly well suited for USB CDC class.
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
#include "compiler.h"
#include "conf_usb.h"
#include "usb_drv.h"
#include "uart_usb_lib.h"

//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

//_____ D E C L A R A T I O N S ____________________________________________
bool b_tx_new;
bool b_rx_new;

void uart_usb_init(void)
{
   b_tx_new = true;
   b_rx_new = true;
}

// Functions that manage characters input through USB
//

bool uart_usb_test_hit(void)
{
   if( Is_usb_out_received(RX_EP) )
   {
      if( 0!=Usb_byte_count(RX_EP) )
      {
         if( b_rx_new )
         {
            Usb_reset_endpoint_fifo_access(RX_EP);
            b_rx_new = false;
         }
         return true;
      }
      Usb_ack_out_received_free(RX_EP);
      b_rx_new = true;
   }
   return false;
}


char uart_usb_getchar(void)
{
  register char data_rx;

  while( !uart_usb_test_hit() );
  data_rx=Usb_read_endpoint_data(RX_EP, 8);
  if( 0==Usb_byte_count(RX_EP) ) {
      Usb_ack_out_received_free(RX_EP);
      b_rx_new = true;
  }
  return data_rx;
}


// Functions that manage characters output through USB
//
bool uart_usb_tx_ready(void)
{
  if( Is_usb_write_enabled(TX_EP) )
  {
      if( b_tx_new )
      {
         Usb_reset_endpoint_fifo_access(TX_EP);
         b_tx_new = false;
      }
      return true;
  }
  return false;
}


int uart_usb_putchar(int data_to_send)
{
  while( !uart_usb_tx_ready() ); // Wait Endpoint ready
  Usb_write_endpoint_data(TX_EP, 8, data_to_send);
  if( !Is_usb_write_enabled(TX_EP) ) // If Endpoint full -> flush
  {
     uart_usb_flush();
  }
  return data_to_send;
}


void uart_usb_flush (void)
{
  bool zlp=false;

  if(!Is_usb_write_enabled(TX_EP))              // Endpoint full, need ZLP
     zlp=true;

  Usb_ack_in_ready_send(TX_EP);
  b_tx_new = true;
  if( zlp==true )
  {
     while( !Is_usb_write_enabled(TX_EP) );     // Wait Endpoint ready...
     Usb_ack_in_ready_send(TX_EP);              // ...and Send ZLP
  }
}
