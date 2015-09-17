/*****************************************************************************
 *
 * \file
 *
 * \brief LIN service for AVR32 UC3.
 *
 * This file contains basic functions for the AVR32 USART, with support for all
 * modes, settings and clock speeds.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <stdio.h>
#include <string.h>
#include "compiler.h"
#include "intc.h"
#include "pdca.h"
#include "gpio.h"
#include "lin.h"
//_____ D E F I N I T I O N S __________________________________________________

//_____ D E C L A R A T I O N S ________________________________________________
// Array of structure of type:'st_lin_message'
volatile st_lin_message   lin_descript_list_node0[NUMBER_OF_LIN_FRAMES_NODE0];
#ifdef USART_LIN_NODE1_INSTANCE
volatile st_lin_message   lin_descript_list_node1[NUMBER_OF_LIN_FRAMES_NODE1];
#endif

// Index in lin_descript_list[]
volatile U8  lin_handle_node0 = 0xFF;
#ifdef USART_LIN_NODE1_INSTANCE
volatile U8  lin_handle_node1 = 0xFF;
#endif

// Error counter
volatile U16  lin_error_number_node0 = 0;
#ifdef USART_LIN_NODE1_INSTANCE
volatile U16  lin_error_number_node1 = 0;
#endif

// Last error[n]: lin_handle | status - length = 4
volatile U16  lin_last_errors_node0[LIN_LAST_ERR_LENGHT];
#ifdef USART_LIN_NODE1_INSTANCE
volatile U16  lin_last_errors_node1[LIN_LAST_ERR_LENGHT];
#endif

// Tx Buffer message
U8 lin_tx_buffer_node0[256];
#ifdef USART_LIN_NODE1_INSTANCE
U8 lin_tx_buffer_node1[256];
#endif

// Rx Buffer message
volatile U8 lin_rx_buffer_node0[256];
#ifdef USART_LIN_NODE1_INSTANCE
volatile U8 lin_rx_buffer_node1[256];
#endif

//! Map of the LIN pins used
static const gpio_map_t USART_LIN_NODE0_GPIO_MAP =
{
  {USART_LIN_NODE0_RX_PIN, USART_LIN_NODE0_RX_FUNCTION},
  {USART_LIN_NODE0_TX_PIN, USART_LIN_NODE0_TX_FUNCTION}
};

#ifdef USART_LIN_NODE1_INSTANCE
static const gpio_map_t USART_LIN_NODE1_GPIO_MAP =
{
  {USART_LIN_NODE1_RX_PIN, USART_LIN_NODE1_RX_FUNCTION},
  {USART_LIN_NODE1_TX_PIN, USART_LIN_NODE1_TX_FUNCTION}
};
#endif

//! Instance of the USART IP used
volatile avr32_usart_t* usart_lin_node0 = &USART_LIN_NODE0_INSTANCE;
#ifdef USART_LIN_NODE1_INSTANCE
volatile avr32_usart_t* usart_lin_node1 = &USART_LIN_NODE1_INSTANCE;
#endif

//_____ F U N C T I O N S ______________________________________________________

static U8   lin_rx_response( U8 l_node, U8 l_handle, U8 l_len);
static U8   lin_tx_response( U8 l_node, U8 l_handle, U8 *l_data, U8 l_len) ;
static void lin_get_response(U8 l_node, U8 *l_data);
static U8   lin_tx_header_and_response(U8 l_node, U8 l_handle, U8 l_len);

/*! \brief LIN TX PDCA interrupt handler.
 */
#if defined (__GNUC__) && defined (__AVR32__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
#pragma handler = USART_LIN_NODE0_TX_PDCA_IRQ_GROUP, USART_LIN_NODE0_TX_PDCA_CHANNEL
__interrupt
#endif
static void lin_pdca_int_tx_handler_node0(void)
{
  U8 index = 0;
  U8 l_handle = 0xFF;

  // Check ID Value for the current message
  for(index = 0; index < NUMBER_OF_LIN_FRAMES_NODE0; index ++) {
      if(lin_descript_list_node0[index].l_id == usart_lin_get_id_char(usart_lin_node0)) {
          l_handle = index;
              break;
      }
  }
  // Check if the ID received is registered into the lin description list
  if (l_handle!=0xFF)
  {
    if (pdca_get_transfer_status(USART_LIN_NODE0_TX_PDCA_CHANNEL)&PDCA_TRANSFER_COMPLETE)
    {
        pdca_disable_interrupt_transfer_complete(USART_LIN_NODE0_TX_PDCA_CHANNEL);
        // Start of the associated task
        lin_descript_list_node0[l_handle].l_pt_function(lin_descript_list_node0[l_handle].l_pt_data);
        lin_handle_node0 = 0xFF;  // End of communication
    }
  }
}

/*! \brief USART LIN RX PDCA interrupt handler.
 */
#if defined (__GNUC__) && defined (__AVR32__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
#pragma handler = USART_LIN_NODE0_RX_PDCA_IRQ_GROUP, USART_LIN_NODE0_RX_PDCA_CHANNEL
__interrupt
#endif
static void lin_pdca_int_rx_handler_node0(void)
{
  U8 index = 0;
  U8 l_handle = 0xFF;
  // Check ID Value for the current message
  for(index = 0; index < NUMBER_OF_LIN_FRAMES_NODE0; index ++) {
      if(lin_descript_list_node0[index].l_id == usart_lin_get_id_char(usart_lin_node0)) {
          l_handle = index;
              break;
      }
  }
  // Check if the ID received is registered into the lin description list
  if (l_handle!=0xFF)
  {
    if (pdca_get_transfer_status(USART_LIN_NODE0_RX_PDCA_CHANNEL)&PDCA_TRANSFER_COMPLETE)
    {
        pdca_disable_interrupt_transfer_complete(USART_LIN_NODE0_RX_PDCA_CHANNEL);
        lin_get_response (0,lin_descript_list_node0[l_handle].l_pt_data);
        // Start of the associated task
        lin_descript_list_node0[l_handle].l_pt_function(lin_descript_list_node0[l_handle].l_pt_data);
        lin_handle_node0 = 0xFF;  // End of communication
    }
  }
}
/*! \brief USART LIN interrupt handler: manage ID reception and ERROR
 */
#if defined (__GNUC__) && defined (__AVR32__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
static void lin_int_handler_node0(void)
{
    U32 l_error;
    U8 index;
    U8 l_handle = 0xFF;
    // Check ID Value for the current message
    for(index = 0; index < NUMBER_OF_LIN_FRAMES_NODE0; index ++)
    {
        if(lin_descript_list_node0[index].l_id == usart_lin_get_id_char(usart_lin_node0))
        {
            l_handle = index;
                break;
        }
    }
    // Check Error Status
    l_error = usart_lin_get_error(usart_lin_node0);
    usart_reset_status(usart_lin_node0);
    if (l_error!=0)
    {
      if(l_handle != 0xFF)
      {
          lin_descript_list_node0[l_handle].l_status = l_error;
      }
      lin_last_errors_node0[(((U8)lin_error_number_node0) & (LIN_LAST_ERR_LENGHT-1))] \
                      = ((((U16)l_handle)<<8) | ((U16)l_error))            ;
      lin_error_number_node0++;
      lin_handle_node0 = 0xFF;  // End of communication
    }
    // Here the communication go on only in case no error is detected!!!
    else
    {
        usart_lin_set_node_action(usart_lin_node0,lin_descript_list_node0[l_handle].l_cmd);

        if(l_handle != 0xFF)
        {
#if USART_LIN_VERSION == LIN_2x
            // Configure Parity
            usart_lin_enable_parity(usart_lin_node0,true);
            // Configure Checksum
            usart_lin_enable_checksum(usart_lin_node0,true);
            // Configure Checksum Type
            usart_lin_set_checksum(usart_lin_node0,USART_LIN_ENHANCED_CHECKSUM);
            // Configure Frameslot
            usart_lin_enable_frameslot(usart_lin_node0,false);
            // Configure Frame Length
            usart_lin_set_data_length_lin2x(usart_lin_node0,lin_descript_list_node0[l_handle].l_dlc);
#elif USART_LIN_VERSION == LIN_1x
            // Configure Parity
            usart_lin_enable_parity(usart_lin_node0,true);
            // Configure Checksum
            usart_lin_enable_checksum(usart_lin_node0,true);
            // Configure Checksum Type
            usart_lin_set_checksum(usart_lin_node0,USART_LIN_CLASSIC_CHECKSUM);
            // Configure Frameslot
            usart_lin_enable_frameslot(usart_lin_node0,false);
            // Configure Frame Length
            usart_lin_set_data_length_lin1x(usart_lin_node0);
#endif

            // Diagnostic frames in LIN 2.0/2.1
            if((usart_lin_get_id_char(usart_lin_node0)>=60)) {
                usart_lin_set_checksum(usart_lin_node0,USART_LIN_CLASSIC_CHECKSUM);
                // Configure Frame Length
                usart_lin_set_data_length_lin1x(usart_lin_node0);
            }

            switch (lin_descript_list_node0[l_handle].l_cmd)
            {
                //------------
                case  PUBLISH:
                    lin_tx_response (      0,
                                           l_handle,
                                           lin_descript_list_node0[l_handle].l_pt_data,
                                           lin_descript_list_node0[l_handle].l_dlc) ;
                    break;
                //------------
                case SUBSCRIBE:
                    lin_rx_response (      0,
                                           l_handle,
                                           lin_descript_list_node0[l_handle].l_dlc) ;
                    break;
                //------------
                case IGNORE:
                default:
                    lin_handle_node0 = 0xFF;  // End of communication
                    break;
            }
        }
    }
}

#ifdef USART_LIN_NODE1_INSTANCE
/*! \brief LIN TX PDCA interrupt handler.
 */
#if defined (__GNUC__) && defined (__AVR32__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
#pragma handler = USART_LIN_NODE1_TX_PDCA_IRQ_GROUP, USART_LIN_NODE1_TX_PDCA_CHANNEL
__interrupt
#endif
static void lin_pdca_int_tx_handler_node1(void)
{
  U8 index = 0;
  U8 l_handle = 0xFF;
  // Check ID Value for the current message
  for(index = 0; index < NUMBER_OF_LIN_FRAMES_NODE1; index ++)
  {
      if(lin_descript_list_node1[index].l_id == usart_lin_get_id_char(usart_lin_node1))
      {
          l_handle = index;
                    break;
      }
  }

  // Check if the ID received is registered into the lin description list
  if (l_handle!=0xFF)
  {
    if (pdca_get_transfer_status(USART_LIN_NODE1_TX_PDCA_CHANNEL)&PDCA_TRANSFER_COMPLETE)
    {
        pdca_disable_interrupt_transfer_complete(USART_LIN_NODE1_TX_PDCA_CHANNEL);
        // Start of the associated task
        lin_descript_list_node1[l_handle].l_pt_function(lin_descript_list_node1[l_handle].l_pt_data);
        lin_handle_node1 = 0xFF;  // End of communication
    }
  }
}

/*! \brief USART LIN RX PDCA interrupt handler.
 */
#if defined (__GNUC__) && defined (__AVR32__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
#pragma handler = USART_LIN_NODE1_RX_PDCA_IRQ_GROUP, USART_LIN_NODE1_RX_PDCA_CHANNEL
__interrupt
#endif
static void lin_pdca_int_rx_handler_node1(void)
{
  U8 index = 0;
  U8 l_handle = 0xFF;
  // Check ID Value for the current message
  for(index = 0; index < NUMBER_OF_LIN_FRAMES_NODE1; index ++)
  {
      if(lin_descript_list_node1[index].l_id == usart_lin_get_id_char(usart_lin_node1))
      {
          l_handle = index;
                    break;
      }
  }

  // Check if the ID received is registered into the lin description list
  if (l_handle!=0xFF)
  {
    if (pdca_get_transfer_status(USART_LIN_NODE1_RX_PDCA_CHANNEL)&PDCA_TRANSFER_COMPLETE)
    {
        pdca_disable_interrupt_transfer_complete(USART_LIN_NODE1_RX_PDCA_CHANNEL);
        lin_get_response (1,lin_descript_list_node1[l_handle].l_pt_data);
        // Start of the associated task
        lin_descript_list_node1[l_handle].l_pt_function(lin_descript_list_node1[l_handle].l_pt_data);
        lin_handle_node1 = 0xFF;  // End of communication
    }
  }
}
/*! \brief USART LIN interrupt handler: manage ID reception and ERROR
 */
#if defined (__GNUC__) && defined (__AVR32__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
static void lin_int_handler_node1(void)
{
    U32 l_error;
    U8 index;
    U8 l_handle = 0xFF;
    // Check ID Value for the current message
    for(index = 0; index < NUMBER_OF_LIN_FRAMES_NODE1; index ++)
    {
        if(lin_descript_list_node1[index].l_id == usart_lin_get_id_char(usart_lin_node1))
        {
            l_handle = index;
              break;
        }
    }

    // Check Error Status
    l_error = usart_lin_get_error(usart_lin_node1);
    usart_reset_status(usart_lin_node1);
    if (l_error!=0)
    {
      if(l_handle != 0xFF)
      {
          lin_descript_list_node1[l_handle].l_status = l_error;
      }
      lin_last_errors_node1[(((U8)lin_error_number_node1) & (LIN_LAST_ERR_LENGHT-1))] \
                      = ((((U16)l_handle)<<8) | ((U16)l_error))            ;
      lin_error_number_node1++;
      lin_handle_node1 = 0xFF;  // End of communication
    }
    // Here the communication go on only in case no error is detected!!!
    else
    {
        usart_lin_set_node_action(usart_lin_node1,lin_descript_list_node1[l_handle].l_cmd);

        if(l_handle != 0xFF) {
#if USART_LIN_VERSION == LIN_2x
            // Configure Parity
            usart_lin_enable_parity(usart_lin_node1,true);
            // Configure Checksum
            usart_lin_enable_checksum(usart_lin_node1,true);
            // Configure Checksum Type
            usart_lin_set_checksum(usart_lin_node1,USART_LIN_ENHANCED_CHECKSUM);
            // Configure Frameslot
            usart_lin_enable_frameslot(usart_lin_node1,false);
            // Configure Frame Length
            usart_lin_set_data_length_lin2x(usart_lin_node1,lin_descript_list_node1[l_handle].l_dlc);
#elif USART_LIN_VERSION == LIN_1x
            // Configure Parity
            usart_lin_enable_parity(usart_lin_node1,true);
            // Configure Checksum
            usart_lin_enable_checksum(usart_lin_node1,true);
            // Configure Checksum Type
            usart_lin_set_checksum(usart_lin_node1,USART_LIN_CLASSIC_CHECKSUM);
            // Configure Frameslot
            usart_lin_enable_frameslot(usart_lin_node1,false);
            // Configure Frame Length
            usart_lin_set_data_length_lin1x(usart_lin_node1);
#endif

            // Diagnostic frames in LIN 2.0/2.1
            if((usart_lin_get_id_char(usart_lin_node1)>=60))
            {
                usart_lin_set_checksum(usart_lin_node1,USART_LIN_CLASSIC_CHECKSUM);
                // Configure Frame Length
                usart_lin_set_data_length_lin1x(usart_lin_node1);
            }

            switch (lin_descript_list_node1[l_handle].l_cmd)
            {
                //------------
                case  PUBLISH:
                    lin_tx_response (      1,
                                           l_handle,
                                           lin_descript_list_node1[l_handle].l_pt_data,
                                           lin_descript_list_node1[l_handle].l_dlc) ;
                    break;
                //------------
                case SUBSCRIBE:
                    lin_rx_response (      1,
                                           l_handle,
                                           lin_descript_list_node1[l_handle].l_dlc) ;
                    break;
                //------------
                case IGNORE:
                default:
                    lin_handle_node1 = 0xFF;  // End of communication
                    break;
            }
        }
    }
}
#endif
//------------------------------------------------------------------------------
/*! \name Initialization Functions
 */
//! @{

/*! \brief  This function initializes the LIN controller and, if needed, the LIN
 *          interrupts.
 *
 * \param master \c true for master, \c false for slave
 * \param l_node Node Value
 * \param b_rate Baudrate Value
 * \param pba_hz PBA Value
 * \return Status PASS / FAIL
 *
 */
U8 lin_init (bool master,
             U8 l_node,
             U16 b_rate,
             long pba_hz) {
if (l_node == 0)
{
    // Enable GPIO Alternate Functions
    gpio_enable_module(USART_LIN_NODE0_GPIO_MAP,
                     sizeof(USART_LIN_NODE0_GPIO_MAP) / sizeof(USART_LIN_NODE0_GPIO_MAP[0]));

    gpio_enable_pin_pull_up(USART_LIN_NODE0_RX_PIN);

    // USART options.
    if (master)
      usart_init_lin_master(usart_lin_node0,b_rate,pba_hz);
    else
      usart_init_lin_slave(usart_lin_node0,b_rate,pba_hz);

    Disable_global_interrupt();

    if (master==false)
    {
		        //! Enable Interrupt for Error flags and end ID Reception
			#if ( defined (AVR32_USART_400_H_INCLUDED) ||  \
                              defined (AVR32_USART_410_H_INCLUDED) ||  \
                              defined (AVR32_USART_420_H_INCLUDED) )
				usart_lin_node0->ier = AVR32_USART_IER_LINIR_MASK  |
							 AVR32_USART_IER_LINBE_MASK  |
							 AVR32_USART_IER_LINISFE_MASK |
							 AVR32_USART_IER_LINIPE_MASK |
							 AVR32_USART_IER_LINCE_MASK  |
							 AVR32_USART_IER_LINSNRE_MASK;
				//! Register Interrupt for LIN
				INTC_register_interrupt(&lin_int_handler_node0,
										USART_LIN_NODE0_USART_IRQ,
										USART_LIN_NODE0_USART_IRQ_LEVEL);
			#else

				usart_lin_node0->ier = AVR32_USART_IER_LINID_MASK  |
							 AVR32_USART_IER_LINBE_MASK  |
							 AVR32_USART_IER_LINISFE_MASK |
							 AVR32_USART_IER_LINIPE_MASK |
							 AVR32_USART_IER_LINCE_MASK  |
							 AVR32_USART_IER_LINSNRE_MASK;
				//! Register Interrupt for LIN
				INTC_register_interrupt(&lin_int_handler_node0,
										USART_LIN_NODE0_USART_IRQ,
										USART_LIN_NODE0_USART_IRQ_LEVEL);
			#endif
    }

    //! Register Interrupt for PDCA Transfer TX
    INTC_register_interrupt(&lin_pdca_int_tx_handler_node0,
                            USART_LIN_NODE0_PDCA_TX_IRQ,
                            USART_LIN_NODE0_PDCA_TX_IRQ_LEVEL);

    //! Register Interrupt for PDCA Transfer RX
    INTC_register_interrupt(&lin_pdca_int_rx_handler_node0,
                            USART_LIN_NODE0_PDCA_RX_IRQ,
                            USART_LIN_NODE0_PDCA_RX_IRQ_LEVEL);

}
#ifdef USART_LIN_NODE1_INSTANCE
else
{
    // Enable GPIO Alternate Functions
    gpio_enable_module(USART_LIN_NODE1_GPIO_MAP,
                     sizeof(USART_LIN_NODE1_GPIO_MAP) / sizeof(USART_LIN_NODE1_GPIO_MAP[0]));


    gpio_enable_pin_pull_up(USART_LIN_NODE1_RX_PIN);

    // USART options.
    if (master)
      usart_init_lin_master(usart_lin_node1,b_rate,pba_hz);
    else
      usart_init_lin_slave(usart_lin_node1,b_rate,pba_hz);

    Disable_global_interrupt();

    if (master==false)
    {
		        //! Enable Interrupt for Error flags and end ID Reception
			#if ( defined (AVR32_USART_400_H_INCLUDED) ||  \
                              defined (AVR32_USART_410_H_INCLUDED) ||  \
                              defined (AVR32_USART_420_H_INCLUDED) )
				usart_lin_node1->ier = AVR32_USART_IER_LINIR_MASK  |
							 AVR32_USART_IER_LINBE_MASK  |
							 AVR32_USART_IER_LINISFE_MASK |
							 AVR32_USART_IER_LINIPE_MASK |
							 AVR32_USART_IER_LINCE_MASK  |
							 AVR32_USART_IER_LINSNRE_MASK;
				//! Register Interrupt for LIN
				INTC_register_interrupt(&lin_int_handler_node1,
										USART_LIN_NODE1_USART_IRQ,
										USART_LIN_NODE1_USART_IRQ_LEVEL);
			#else
				usart_lin_node1->ier = AVR32_USART_IER_LINID_MASK  |
							 AVR32_USART_IER_LINBE_MASK  |
							 AVR32_USART_IER_LINISFE_MASK |
							 AVR32_USART_IER_LINIPE_MASK |
							 AVR32_USART_IER_LINCE_MASK  |
							 AVR32_USART_IER_LINSNRE_MASK;
				//! Register Interrupt for LIN
				INTC_register_interrupt(&lin_int_handler_node1,
										USART_LIN_NODE1_USART_IRQ,
										USART_LIN_NODE1_USART_IRQ_LEVEL);
			#endif
    }

    //! Register Interrupt for PDCA Transfer TX
    INTC_register_interrupt(&lin_pdca_int_tx_handler_node1,
                            USART_LIN_NODE1_PDCA_TX_IRQ,
                            USART_LIN_NODE1_PDCA_TX_IRQ_LEVEL);

    //! Register Interrupt for PDCA Transfer RX
    INTC_register_interrupt(&lin_pdca_int_rx_handler_node1,
                            USART_LIN_NODE1_PDCA_RX_IRQ,
                            USART_LIN_NODE1_PDCA_RX_IRQ_LEVEL);
}
#endif
    return PASS;
}

/*! \brief  This function commands the sending of the LIN header, MASTER task of MASTER
 *          node.
 *
 * \param l_node Node Value
 * \param l_id  LIN identifier value. In case of `LIN_1X', the coded length is transported into the LIN identifier.
 * \param l_len True length (not coded), number of data bytes transported in the response. This information is not used in `LIN_1X' because it is coded in `l_id'.
 *
 * \return Status PASS / FAIL
 *
 */
U8 lin_send_cmd (      U8 l_node,
                       U8 l_id,
                       U8 l_len
                      ){
      U8 index = 0;
      U8 l_handle = 0;
if (l_node == 0)
{
    // Clear error in case of previous communication
    usart_reset_status(usart_lin_node0);

    for(index = 0; index < NUMBER_OF_LIN_FRAMES_NODE0; index ++)
    {
        if(lin_descript_list_node0[index].l_id == l_id)
        {
            l_handle = index;
	          break;
        }
    }

    if(l_handle != 0xFF)
    {
        usart_lin_set_node_action(usart_lin_node0,lin_descript_list_node0[l_handle].l_cmd);
#if USART_LIN_VERSION == LIN_2x
        // Configure Parity
        usart_lin_enable_parity(usart_lin_node0,true);
        // Configure Checksum
        usart_lin_enable_checksum(usart_lin_node0,true);
        // Configure Checksum Type
        usart_lin_set_checksum(usart_lin_node0,USART_LIN_ENHANCED_CHECKSUM);
        // Configure Frameslot
        usart_lin_enable_frameslot(usart_lin_node0,false);
        // Configure Frame Length
        usart_lin_set_data_length_lin2x(usart_lin_node0,l_len);
#elif USART_LIN_VERSION == LIN_1x
        // Configure Parity
        usart_lin_enable_parity(usart_lin_node0,true);
        // Configure Checksum
        usart_lin_enable_checksum(usart_lin_node0,true);
        // Configure Checksum Type
        usart_lin_set_checksum(usart_lin_node0,USART_LIN_CLASSIC_CHECKSUM);
        // Configure Frameslot
        usart_lin_enable_frameslot(usart_lin_node0,false);
        // Configure Frame Length
        usart_lin_set_data_length_lin1x(usart_lin_node0);
#endif
        // Switch to Classic Checksum if diagnostic ID request
        if (lin_descript_list_node0[l_handle].l_id>=60)
        {
                usart_lin_set_checksum(usart_lin_node0,USART_LIN_CLASSIC_CHECKSUM);
                // Configure Frame Length
                usart_lin_set_data_length_lin1x(usart_lin_node0);
        }

        switch (lin_descript_list_node0[l_handle].l_cmd)
        {
            // In Publish, the USART Send the Header and the response
            case PUBLISH:
                  lin_tx_header_and_response(0,l_handle,l_len);
              break;
            // In Subscribe, the USART Receive the response
            case SUBSCRIBE:
            	  usart_lin_set_id_char(usart_lin_node0,l_id);
            	  lin_rx_response(0,l_handle,l_len);
              break;
            default:
              break;
        }
        return PASS;
    }
    else
    {
        return FAIL;
    }
}
#ifdef USART_LIN_NODE1_INSTANCE
else
{
    // Clear error in case of previous communication
    usart_reset_status(usart_lin_node1);

    for(index = 0; index < NUMBER_OF_LIN_FRAMES_NODE1; index ++) {
        if(lin_descript_list_node1[index].l_id == l_id) {
            l_handle = index;
	          break;
        }
    }

    if(l_handle != 0xFF) {

        usart_lin_set_node_action(usart_lin_node1,lin_descript_list_node1[l_handle].l_cmd);

#if USART_LIN_VERSION == LIN_2x
        // Configure Parity
        usart_lin_enable_parity(usart_lin_node1,true);
        // Configure Checksum
        usart_lin_enable_checksum(usart_lin_node1,true);
        // Configure Checksum Type
        usart_lin_set_checksum(usart_lin_node1,USART_LIN_ENHANCED_CHECKSUM);
        // Configure Frameslot
        usart_lin_enable_frameslot(usart_lin_node1,false);
        // Configure Frame Length
        usart_lin_set_data_length_lin2x(usart_lin_node1,l_len);
#elif USART_LIN_VERSION == LIN_1x
        // Configure Parity
        usart_lin_enable_parity(usart_lin_node1,true);
        // Configure Checksum
        usart_lin_enable_checksum(usart_lin_node1,true);
        // Configure Checksum Type
        usart_lin_set_checksum(usart_lin_node1,USART_LIN_CLASSIC_CHECKSUM);
        // Configure Frameslot
        usart_lin_enable_frameslot(usart_lin_node1,false);
        // Configure Frame Length
        usart_lin_set_data_length_lin1x(usart_lin_node1);
#endif
        // Switch to Classic Checksum if diagnostic ID request
        if (lin_descript_list_node1[l_handle].l_id>=60)
        {
                usart_lin_set_checksum(usart_lin_node1,USART_LIN_CLASSIC_CHECKSUM);
                // Configure Frame Length
                usart_lin_set_data_length_lin1x(usart_lin_node1);
        }

        switch (lin_descript_list_node0[l_handle].l_cmd)
        {
            // In Publish, the USART Send the Header and the response
            case PUBLISH:
                  lin_tx_header_and_response(1,l_handle,l_len);
              break;
            // In Subscribe, the USART Receive the response
            case SUBSCRIBE:
            	  usart_lin_set_id_char(usart_lin_node1,l_id);
            	  lin_rx_response(1,l_handle,l_len);
              break;
            default:
              break;
        }
        return PASS;
    }
    else
    {
        return FAIL;
    }
}
#endif
}

/*! \brief  This function commands the sending of a LIN header and response, MASTER task only
 *
 * \param l_node Node Value
 * \param l_handle  Handle on the descriptor list
 * \param l_len     Message length corresponding to the message pointed by the handle in the descriptor list
 *
 * \return Status PASS / FAIL
 *
 */
static U8 lin_tx_header_and_response( U8 l_node,
                                      U8 l_handle,
                                      U8 l_len
                                     )
{
if (l_node == 0)
{
        //! Enable Interrupt for Error flags and end ID Reception
        #if ( defined (AVR32_USART_400_H_INCLUDED) ||  \
              defined (AVR32_USART_410_H_INCLUDED) ||  \
              defined (AVR32_USART_420_H_INCLUDED) )
          // PDCA channel options
          pdca_channel_options_t USART_LIN_PDCA_OPTIONS =
          {
            .addr           = (void *)lin_tx_buffer_node0,   // memory address
            .pid            = USART_LIN_NODE0_PDCA_PID_TX,       // select peripheral - data are transmit on USART TX line.
            .size           = (l_len+1),           // transfer counter
            .r_addr         = NULL, // next memory address
            .r_size         = 0,	   // next transfer counter
            .transfer_size  = PDCA_TRANSFER_SIZE_BYTE // select size of the transfer
          };

          pdca_init_channel(USART_LIN_NODE0_TX_PDCA_CHANNEL, &USART_LIN_PDCA_OPTIONS);

          // Copy data of the data contained in the descriptor list into the tx buffer of the PDCA
          memcpy(&lin_tx_buffer_node0[1],lin_descript_list_node0[l_handle].l_pt_data,l_len);

          pdca_enable_interrupt_transfer_complete(USART_LIN_NODE0_TX_PDCA_CHANNEL);

          // Set the ID First
          lin_tx_buffer_node0[0] = lin_descript_list_node0[l_handle].l_id;

          // Start PDCA transfer ID + Data
          pdca_enable(USART_LIN_NODE0_TX_PDCA_CHANNEL);

          usart_lin_set_id_char(usart_lin_node0,lin_descript_list_node0[l_handle].l_id);
      #else
          // PDCA channel options
          pdca_channel_options_t USART_LIN_PDCA_OPTIONS =
          {
            .addr           = (void *)lin_tx_buffer_node0,   // memory address
            .pid            = USART_LIN_NODE0_PDCA_PID_TX,       // select peripheral - data are transmit on USART TX line.
            .size           = (l_len+1),           // transfer counter
            .r_addr         = NULL, // next memory address
            .r_size         = 0,	   // next transfer counter
            .transfer_size  = PDCA_TRANSFER_SIZE_BYTE // select size of the transfer
          };

          pdca_init_channel(USART_LIN_NODE0_TX_PDCA_CHANNEL, &USART_LIN_PDCA_OPTIONS);

          // Copy data of the data contained in the descriptor list into the tx buffer of the PDCA
          memcpy(&lin_tx_buffer_node0[1],lin_descript_list_node0[l_handle].l_pt_data,l_len);

          pdca_enable_interrupt_transfer_complete(USART_LIN_NODE0_TX_PDCA_CHANNEL);

          // Set the ID First
          lin_tx_buffer_node0[0] = lin_descript_list_node0[l_handle].l_id;

          // Start PDCA transfer ID + Data
          pdca_enable(USART_LIN_NODE0_TX_PDCA_CHANNEL);
      #endif
    return PASS;
}
#ifdef USART_LIN_NODE1_INSTANCE
else
{
      //! Enable Interrupt for Error flags and end ID Reception
      #if ( defined (AVR32_USART_400_H_INCLUDED) ||  \
            defined (AVR32_USART_410_H_INCLUDED) ||  \
            defined (AVR32_USART_420_H_INCLUDED) )
        // PDCA channel options
        pdca_channel_options_t USART_LIN_PDCA_OPTIONS =
        {
          .addr           = (void *)lin_tx_buffer_node1,   // memory address
          .pid            = USART_LIN_NODE1_PDCA_PID_TX,       // select peripheral - data are transmit on USART TX line.
          .size           = (l_len+1),           // transfer counter
          .r_addr         = NULL, // next memory address
          .r_size         = 0,	   // next transfer counter
          .transfer_size  = PDCA_TRANSFER_SIZE_BYTE // select size of the transfer
        };

        pdca_init_channel(USART_LIN_NODE1_TX_PDCA_CHANNEL, &USART_LIN_PDCA_OPTIONS);

        // Copy data of the data contained in the descriptor list into the tx buffer of the PDCA
        memcpy(&lin_tx_buffer_node1[1],lin_descript_list_node1[l_handle].l_pt_data,l_len);

        pdca_enable_interrupt_transfer_complete(USART_LIN_NODE1_TX_PDCA_CHANNEL);

        // Set the ID First
        lin_tx_buffer_node1[0] = lin_descript_list_node1[l_handle].l_id;

        // Start PDCA transfer ID + Data
        pdca_enable(USART_LIN_NODE1_TX_PDCA_CHANNEL);

        usart_lin_set_id_char(usart_lin_node1,lin_descript_list_node1[l_handle].l_id);
      #else
        // PDCA channel options
        pdca_channel_options_t USART_LIN_PDCA_OPTIONS =
        {
          .addr           = (void *)lin_tx_buffer_node1,   // memory address
          .pid            = USART_LIN_NODE1_PDCA_PID_TX,       // select peripheral - data are transmit on USART TX line.
          .size           = (l_len+1),           // transfer counter
          .r_addr         = NULL, // next memory address
          .r_size         = 0,	   // next transfer counter
          .transfer_size  = PDCA_TRANSFER_SIZE_BYTE // select size of the transfer
        };

        pdca_init_channel(USART_LIN_NODE1_TX_PDCA_CHANNEL, &USART_LIN_PDCA_OPTIONS);

        // Copy data of the data contained in the descriptor list into the tx buffer of the PDCA
        memcpy(&lin_tx_buffer_node1[1],lin_descript_list_node1[l_handle].l_pt_data,l_len);

        pdca_enable_interrupt_transfer_complete(USART_LIN_NODE1_TX_PDCA_CHANNEL);

        // Set the ID First
        lin_tx_buffer_node1[0] = lin_descript_list_node1[l_handle].l_id;

        // Start PDCA transfer ID + Data
        pdca_enable(USART_LIN_NODE1_TX_PDCA_CHANNEL);
      #endif
    return PASS;
}
#endif
}

/*! \brief  This function commands the sending of a LIN response, SLAVE task of MASTER or SLAVE node.
 *
 * \param l_node Node Value
 * \param l_handle  Handle on the descriptor list
 * \param l_data    Pointer on the data corresponding to the message pointed by the handle in the descriptor list
 * \param l_len     Message length corresponding to the message pointed by the handle in the descriptor list
 *
 * \return Status PASS / FAIL
 *
 */
static U8 lin_tx_response (          U8 l_node,
                                     U8 l_handle,
                                     U8 *l_data,
                                     U8 l_len
                                    ) {

if (l_node == 0)
{
      // PDCA channel options
      pdca_channel_options_t USART_LIN_PDCA_OPTIONS =
      {
        .addr = (void *)lin_tx_buffer_node0,   // memory address
        .pid = USART_LIN_NODE0_PDCA_PID_TX,  // select peripheral - data are transmit on USART TX line.
        .size = (l_len),                 // transfer counter
        .r_addr = NULL,                 // next memory address
        .r_size = 0,	                  // next transfer counter
        .transfer_size = PDCA_TRANSFER_SIZE_BYTE  // select size of the transfer
      };

      pdca_init_channel(USART_LIN_NODE0_TX_PDCA_CHANNEL, &USART_LIN_PDCA_OPTIONS);

      // Copy data of the data contained in the descriptor list into the tx buffer of the PDCA
      memcpy(&lin_tx_buffer_node0[0],lin_descript_list_node0[l_handle].l_pt_data,l_len+1);

       pdca_enable_interrupt_transfer_complete(USART_LIN_NODE0_TX_PDCA_CHANNEL);
      // Start PDCA transfer Data
      pdca_enable(USART_LIN_NODE0_TX_PDCA_CHANNEL);
    return 1;
}
#ifdef USART_LIN_NODE1_INSTANCE
else
{
    // PDCA channel options
    pdca_channel_options_t USART_LIN_PDCA_OPTIONS =
    {
      .addr = (void *)lin_tx_buffer_node1,   // memory address
      .pid = USART_LIN_NODE1_PDCA_PID_TX,  // select peripheral - data are transmit on USART TX line.
      .size = (l_len),                 // transfer counter
      .r_addr = NULL,                 // next memory address
      .r_size = 0,	                  // next transfer counter
      .transfer_size = PDCA_TRANSFER_SIZE_BYTE  // select size of the transfer
    };

    pdca_init_channel(USART_LIN_NODE1_TX_PDCA_CHANNEL, &USART_LIN_PDCA_OPTIONS);

    // Copy data of the data contained in the descriptor list into the tx buffer of the PDCA
    memcpy(&lin_tx_buffer_node1[0],lin_descript_list_node1[l_handle].l_pt_data,l_len+1);

     pdca_enable_interrupt_transfer_complete(USART_LIN_NODE1_TX_PDCA_CHANNEL);
    // Start PDCA transfer Data
    pdca_enable(USART_LIN_NODE1_TX_PDCA_CHANNEL);
    return 1;
}
#endif
}

/*! \brief  This function commands the reception of a LIN response, SLAVE task of MASTER or SLAVE node.
 *
 * \param l_node Node Value
 * \param l_handle  Handle on the descriptor list
 * \param l_len     Message length corresponding to the message pointed by the handle in the descriptor list
 *
 * \return Status PASS / FAIL
 *
 */
static U8 lin_rx_response ( U8 l_node,
                            U8 l_handle,
                            U8 l_len
                          ) {

if (l_node == 0)
{
    // PDCA channel options
    pdca_channel_options_t USART_LIN_PDCA_OPTIONS =
    {
      .addr = (void *)lin_rx_buffer_node0,   // memory address
      .pid = USART_LIN_NODE0_PDCA_PID_RX,  // select peripheral - data are transmit on USART TX line.
      .size = (l_len),                 // transfer counter
      .r_addr = NULL,                 // next memory address
      .r_size = 0,	                  // next transfer counter
      .transfer_size = PDCA_TRANSFER_SIZE_BYTE  // select size of the transfer
    };

    pdca_init_channel(USART_LIN_NODE0_RX_PDCA_CHANNEL, &USART_LIN_PDCA_OPTIONS);

     pdca_enable_interrupt_transfer_complete(USART_LIN_NODE0_RX_PDCA_CHANNEL);
    // Start PDCA transfer Data
    pdca_enable(USART_LIN_NODE0_RX_PDCA_CHANNEL);
    return 1;
}
#ifdef USART_LIN_NODE1_INSTANCE
else
{
    // PDCA channel options
    pdca_channel_options_t USART_LIN_PDCA_OPTIONS =
    {
      .addr = (void *)lin_rx_buffer_node1,   // memory address
      .pid = USART_LIN_NODE1_PDCA_PID_RX,  // select peripheral - data are transmit on USART TX line.
      .size = (l_len),                 // transfer counter
      .r_addr = NULL,                 // next memory address
      .r_size = 0,	                  // next transfer counter
      .transfer_size = PDCA_TRANSFER_SIZE_BYTE  // select size of the transfer
    };

    pdca_init_channel(USART_LIN_NODE1_RX_PDCA_CHANNEL, &USART_LIN_PDCA_OPTIONS);

     pdca_enable_interrupt_transfer_complete(USART_LIN_NODE1_RX_PDCA_CHANNEL);
    // Start PDCA transfer Data
    pdca_enable(USART_LIN_NODE1_RX_PDCA_CHANNEL);
    return 1;
}
#endif
}


/*! \brief  This function reads (empties) the reception data buffer when a LIN response
 *          had been received. This function is additional of the `lin_rx_response()'
 *          function.
 *
 * \param l_node Node Value
 * \param l_data    Pointer on the data corresponding to the message pointed by the handle in the descriptor list
 *
 * \return Status PASS / FAIL
 *
 */
static void lin_get_response (U8 l_node, U8 *l_data) {

U8 i, l_len;
if (l_node == 0)
{
    l_len = usart_lin_get_data_length(usart_lin_node0);
    for (i = 0; i < l_len; i++) {
        (*l_data++) = lin_rx_buffer_node0[i];
    }
}
#ifdef USART_LIN_NODE1_INSTANCE
else
{
    l_len = usart_lin_get_data_length(usart_lin_node1);
    for (i = 0; i < l_len; i++) {
        (*l_data++) = lin_rx_buffer_node1[i];
    }
}
#endif

}
