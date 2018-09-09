/*****************************************************************************
 *
 * \file
 *
 * \brief  LIN service for AVR32 UC3.
 *
 * This file contains basic functions for the AVR32 USART, with support for all
 * modes, settings and clock speeds.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


#ifndef _LIN_H_
#define _LIN_H_

/**
 * \defgroup group_avr32_services_network_lin LIN software stack
 *
 * LIN software stack for 32-bit AVR that supports initializing the LIN bus and to transmit/receive  messages
 *
 * \{
 */

#include <avr32/io.h>
#include "compiler.h"
#include "usart.h"

#include "conf_lin.h"
//_____ I N C L U D E S ________________________________________________________

//_____ D E F I N I T I O N S __________________________________________________

/*! \name LIN Versions
 */
//! @{
#define LIN_2x                        0      //!< Lin 2.x Version.
#define LIN_1x                        1      //!< Lin 1.x Version.
//! @}

#ifndef NUMBER_OF_LIN_FRAMES_NODE0
#define NUMBER_OF_LIN_FRAMES_NODE0    8
#endif // NUMBER_OF_LIN_FRAMES_NODE0

#ifdef USART_LIN_NODE1_INSTANCE
  #ifndef NUMBER_OF_LIN_FRAMES_NODE1
  #define NUMBER_OF_LIN_FRAMES_NODE1    8
  #endif // NUMBER_OF_LIN_FRAMES_NODE1
#endif

/*! @brief This enumeration is used to select an action for a specific message
 *        declared in st_lin_message structure.
 *        Note: XXX_STAND_BY keeps the direction (PUBLISH or SUBSCRIBE) for a
 *             message that will be IGNORE. When the 'stand_by' will be
 *             finished, PUBLISH_STAND_BY would become PUBLISH and
 *             SUBSCRIBE_STAND_BY would become SUBSCRIBE. An IGNORE message
 *             could become either PUBLISH or SUBSCRIBE.
 */
typedef enum {
  PUBLISH,            //!< The node sends the response
  SUBSCRIBE,          //!< The node receives the response
  IGNORE              //!< The node is not concerned by the response, it doesn't send or receive
} enum_lin_cmd;

/*! @brief This structure allows to handle a LIN message and, at the end of the
 *        received/transmitted message, allows to call a function associated
 *        to this message, the update or the capture of "signals".
 */
typedef  struct {
    //! LIN message ID (in case of LIN 1.3 - without lenght)
    U8             l_id;

    //! Length of the LIN message, it is the number of data bytes
    //! of the LIN response
    U8             l_dlc;

    //! Select an action for a specific message
    enum_lin_cmd   l_cmd;

    //! Status of the last communication:
    //! - == 0x00: RXOK or TXOK
    //! - != 0x00: LINERR, content of LIN Error Register 'LINERR'
    U8             l_status;

    //! Pointer on the data buffer, the data buffer where the
    //! bytes of the LIN response are stored
    U8*            l_pt_data;

    //! Pointer on the function which handles the data. This
    //! has as parameter 'l_pt_data'. This function could update
    //! the received 'signals' and could capture and store signals
    //! for the next transmission.
    void         (*l_pt_function)(U8*);
} st_lin_message;

/*! @brief Error report definitions
 */
#define  LIN_OVERLOAD_INT     ((U16)0x00FF)  //!< More than ONE interrupt in the same time !
#ifndef  LIN_LAST_ERR_LENGHT
#define  LIN_LAST_ERR_LENGHT   0x04          //!< Default length of the 'lin_last_error[]' array
#endif

//_____ M A C R O S ____________________________________________________________

//_____ D E C L A R A T I O N S ________________________________________________

/*! @brief Array of structure of type:'st_lin_message'. Default: 8 elements.
 */
extern  volatile st_lin_message   lin_descript_list_node0[NUMBER_OF_LIN_FRAMES_NODE0];
#ifdef USART_LIN_NODE1_INSTANCE
extern  volatile st_lin_message   lin_descript_list_node1[NUMBER_OF_LIN_FRAMES_NODE1];
#endif

/*!  @brief Index in lin_descript_list[], 'lin_handle' is set after processing
 *        IDOK and verified once RXOK or TXOK rises.
 */
extern  volatile U8  lin_handle_node0;
#ifdef USART_LIN_NODE1_INSTANCE
extern  volatile U8  lin_handle_node1;
#endif

/*! @brief Counter of LIN error. If an error clearly linked to a message
 *        (i.e. time-out) the status of this message is written with the
 *        error (LINERR).
 */
extern  volatile U16  lin_error_number_node0;
#ifdef USART_LIN_NODE1_INSTANCE
extern  volatile U16  lin_error_number_node1;
#endif

/*! @brief Last error FIFO: lin_handle | status. Default: 4 elements.
 */
extern  volatile U16  lin_last_errors_node0[];
#ifdef USART_LIN_NODE1_INSTANCE
extern  volatile U16  lin_last_errors_node1[];
#endif

/*! \brief  This function initializes the LIN controller and, if needed, the LIN
 *          interrupts.
 *
 * \param master \c true for master, \c false for slave
 * \param b_rate Baudrate Value
 * \param l_node Node Value
 * \param pba_hz PBA Value
 *
 * \return Status PASS / FAIL
 *
 */
extern  U8 lin_init (bool master, U8 l_node, U16 b_rate, long pba_hz);

/*! \brief  This function commands the sending of the LIN header, MASTER task of MASTER
 *          node.
 *
 * \param l_id  LIN identifier value. In case of `LIN_1X', the coded length is transported into the LIN identifier.
 * \param l_len True length (not coded), number of data bytes transported in the response. This information is not used in `LIN_1X' because it is coded in `l_id'.
 * \param l_node Node Value
 * \return Status PASS / FAIL
 *
 */
extern U8 lin_send_cmd (U8 l_node, U8 l_id, U8 l_len);

/**
 * \}
 */

#endif  // _LIN_H_
