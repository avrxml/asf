/*****************************************************************************
 *
 * \file
 *
 * \brief Configuration for the ET024006DHU display.
 *
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
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


 //! Configuration of the et024006dhu LCD display driver

#ifndef _CONF_ET024006DHU_H_
#define _CONF_ET024006DHU_H_

#include "compiler.h"
#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(ET024006_IFACE_MODE)      || \
    !defined(ET024006_CMD_ADDR)        || \
    !defined(ET024006_PARAM_ADDR)      || \
    !defined(ET024006DHU_TE_PIN)       || \
    !defined(ET024006DHU_BL_PIN)       || \
    !defined(ET024006DHU_BL_FUNCTION)  || \
    !defined(ET024006DHU_RESET_PIN)


#warning The ET024006DHU configuration does not exist in the board definition file. Using default settings.

#define ET024006_IFACE_MODE       ET024006_IFACE_MODE_EBI

/*! \brief address used to write to the index register
 * (or read device status
 */
#define ET024006_CMD_ADDR     ((volatile unsigned short int *) AVR32_EBI_CS0_ADDRESS)


/*! \brief address used to read/write from/to registers or RAM
 *
 * If the select signal connected to address line 21, 
 * this implies an address offset of 0x200000.
 */
#define ET024006_PARAM_ADDR   ((volatile unsigned short int *) (AVR32_EBI_CS0_ADDRESS + 0x00200000))

//! Tearing sync default configuration
#define ET024006DHU_TE_PIN				AVR32_PIN_PA00

//! Reset pin default configuration
#define ET024006DHU_RESET_PIN			AVR32_PIN_PA02

//! Backlight pin default configuration
#define ET024006DHU_BL_PIN				AVR32_PIN_PA01

// ! Backlight pin default function if PMW is used for managing the brightness
#define ET024006DHU_BL_FUNCTION			0


#endif

#ifdef __cplusplus
}
#endif

#endif // _CONF_ET024006DHU_H_
