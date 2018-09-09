/**************************************************************************
 *
 * \file
 *
 * \brief Management of the UART device firmware upgrade.
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


#ifndef _UART_DFU_H_
#define _UART_DFU_H_


//_____ I N C L U D E S ____________________________________________________

#include "conf_uart.h"
#include "compiler.h"


//_____ M A C R O S ________________________________________________________

// DFU status
#define STATUS_OK                     0x00
#define STATUS_errTARGET              0x01
#define STATUS_errFILE                0x02
#define STATUS_errWRITE               0x03
#define STATUS_errERASE               0x04
#define STATUS_errCHECK_ERASED        0x05
#define STATUS_errPROG                0x06
#define STATUS_errVERIFY              0x07
#define STATUS_errADDRESS             0x08
#define STATUS_errNOTDONE             0x09
#define STATUS_errFIRMWARE            0x0A
#define STATUS_errVENDOR              0x0B
#define STATUS_errUSBR                0x0C
#define STATUS_errPOR                 0x0D
#define STATUS_errUNKNOWN             0x0E
#define STATUS_errSTALLEDPKT          0x0F

// DFU state
#define STATE_appIDLE                 0x00
#define STATE_appDETACH               0x01
#define STATE_dfuIDLE                 0x02
#define STATE_dfuDNLOAD_SYNC          0x03
#define STATE_dfuDNBUSY               0x04
#define STATE_dfuDNLOAD_IDLE          0x05
#define STATE_dfuMANIFEST_SYNC        0x06
#define STATE_dfuMANIFEST             0x07
#define STATE_dfuMANIFEST_WAIT_RESET  0x08
#define STATE_dfuUPLOAD_IDLE          0x09
#define STATE_dfuERROR                0x0A


//_____ D E C L A R A T I O N S ____________________________________________

typedef union
{
  U32 long_address;
  struct
  {
    unsigned int page         : 16;
    unsigned int page_offset  : 16;
  };
} address_t;

/*! \brief Structure used to describe a frame header
 */
typedef struct
{
  U8 type;              // Type of the frame
  U8 length;            // Length of the data of the frame
  address_t address;    // Memory address concerned
} req_header_t;

extern U8 uart_dfu_status;
extern U8 uart_dfu_state;

/*! \brief Configure the USART, initialize the DFU and setup/calculate the autobaud.
 */
void uart_dfu_start(void);

/*! \brief Get the DFU command from the external programmer tool.
 */
U8 uart_get_dfu_command(req_header_t *req_header);

/*! \brief Execute the DFU command required by the external programmer tool.
 */
void uart_execute_dfu_command(req_header_t *req_header, U8 *checksum);

#endif  // _UART_DFU_H_
