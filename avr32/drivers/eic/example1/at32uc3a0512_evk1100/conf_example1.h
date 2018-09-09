/**
 * \file
 *
 * \brief AVR UC3 External Interrupt Controller(EIC) driver
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
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef _CONF_BOARD_H_
#define _CONF_BOARD_H_


/*! \name External Interrupt pin Mappings
 */
//! @{

/*
 * With the UC3A datasheet, in section "Peripheral Multiplexing on I/O lines" in
 * the table "GPIO Controller Function Multiplexing", we see that the NMI is
 * mapped on GPIO 20 (i.e. PA20). Using the EVK1100 schematics, we see that the
 * joystick push event is connected to PA20. Thus, a push on the joystick will
 * generate an NMI. The External Interrupt number 8 is the NMI.
 * Note: AVR32_EIC_EXTINT_8_PIN & AVR32_EIC_EXTINT_8_FUNCTION defines are found
 * in the uc3axxxx.h part-specific header file.
 */
#  define EXT_INT_EXAMPLE_PIN_LINE               AVR32_EIC_EXTINT_8_PIN
#  define EXT_INT_EXAMPLE_FUNCTION_LINE          AVR32_EIC_EXTINT_8_FUNCTION
#  define EXT_INT_EXAMPLE_LINE                   EXT_NMI

//! @}

#endif // _CONF_BOARD_H_

