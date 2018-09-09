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


#ifndef _UART_USB_LIB_H
#define _UART_USB_LIB_H


//_____ I N C L U D E S ____________________________________________________

//_____ M A C R O S ________________________________________________________

//_____ D E F I N I T I O N S ______________________________________________

//_____ D E C L A R A T I O N S ____________________________________________

//! @brief This function initializes the UART over USB emulation driver.
//!
void uart_usb_init(void);

//! @brief This function checks if a character has been received on the USB bus.
//!
//! @return bool (true if a byte is ready to be read)
//!
bool uart_usb_test_hit(void);

//! @brief This function reads one byte from the USB bus
//!
//! If one byte is present in the USB fifo, this byte is returned. If no data
//! is present in the USB fifo, this function waits for USB data.
//!
//! @return uint8_t byte received
//!
char uart_usb_getchar(void);

//! @brief This function checks if the USB emission buffer is ready to accept at
//! at least 1 byte
//!
//! @return Boolean. true if the firmware can write a new byte to transmit.
//!
bool uart_usb_tx_ready(void);

//! @brief This function fills the USB transmit buffer with the new data. This buffer
//! is sent if complete. To flush this buffer before waiting full, launch
//! the uart_usb_flush() function.
//!
//! @param data_to_send
//!
//! @return
//!
int  uart_usb_putchar(int data_to_send);

//! @brief This function sends the data stored in the USB transmit buffer.
//! This function does nothing if there is no data in the buffer.
//!
void uart_usb_flush (void);


#endif  // _UART_USB_LIB_H
