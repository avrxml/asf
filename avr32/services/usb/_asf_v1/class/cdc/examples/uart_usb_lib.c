/**************************************************************************
 *
 * \file
 *
 * \brief This file controls the UART USB functions.
 *
 * These functions allow to use en USB endpoint as we would do using an UART.
 * This is particularly well suited for USB CDC class.
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
