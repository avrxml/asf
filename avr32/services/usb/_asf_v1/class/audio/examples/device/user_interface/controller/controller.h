/**************************************************************************
 *
 * \file
 *
 * \brief Controller interface
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

#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "compiler.h"

/*! \name Initialization
 */
//! \{
/*! \brief Initialization function of the controller interface.
 *
 * \param fcpu_hz CPU frequency.
 * \param fhsb_hz HSB frequency.
 * \param fpbb_hz PBB frequency.
 * \param fpba_hz PBA frequency.
 */
extern void controller_init(uint32_t fcpu_hz, uint32_t fhsb_hz, uint32_t fpbb_hz, uint32_t fpba_hz);
// \}

/*! \name is_joystick_up
 */
//! \{
/*! \brief check if up is pressed.
 *
 */
extern bool is_joystick_up(void);
// \}

/*! \name is_joystick_down
 */
//! \{
/*! \brief check if down is pressed.
 *
 */
extern bool is_joystick_down(void);
// \}

/*! \name is_joystick_right
 */
//! \{
/*! \brief check if right is pressed.
 *
 */
extern bool is_joystick_right(void);
// \}

/*! \name is_joystick_left
 */
//! \{
/*! \brief check if left is pressed.
 *
 */
bool is_joystick_left(void);
// \}

/*! \name is_joystick_pressed
 */
//! \{
/*! \brief check if pressed is pressed.
 *
 */
bool is_joystick_pressed(void);
// \}

/*! \name update_joystick_status
 */
//! \{
/*! \brief Update the internal controller status and test a specific event
 *
 */
void update_joystick_status(unsigned short new_status);
// \}

#endif // _CONTROLLER_H_
