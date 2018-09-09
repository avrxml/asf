/**
 * \file
 *
 * \brief Board-specific example configuration
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

#ifndef _CONF_EXAMPLE_H
#define _CONF_EXAMPLE_H

//! \note This is pin 0 on port A.
#define GAMEMODE_SENSE_PIN     IOPORT_CREATE_PIN(PORTA, 0)
//! \note This is pin 1 on port A.
#define GAMEMODE_GROUND_PIN    IOPORT_CREATE_PIN(PORTA, 1)
//! \note This is pin 2 on port A.
#define SOUNDMODE_SENSE_PIN    IOPORT_CREATE_PIN(PORTA, 2)
//! \note This is pin 3 on port A.
#define SOUNDMODE_GROUND_PIN   IOPORT_CREATE_PIN(PORTA, 3)

//! \note This is TC0 on port C.
#define SOUND_TIMER_MODULE     TCC0
//! \note This is overflow on TC0 on port C.
#define SOUND_TIMER_EVENT      EVSYS_CHMUX_TCC0_OVF_gc
//! \note This is TC1 on port C.
#define DEBOUNCE_TIMER_MODULE  TCC1
//! \note This is TC0 on port D.
#define TIMEOUT_TIMER_MODULE   TCD0

//! \name I/O port and pin config for board buttons
//@{
#define BUTTON_IOPORT_1       PORTD
#define BUTTON_BITMASK_1      0x3f
#define BUTTON_IOPORT_2       PORTR
#define BUTTON_BITMASK_2      0x03
#define BUTTON_BITPOSITION_2  6
//@}

static inline uint8_t get_board_button_mask(void)
{
	uint8_t mask;

	mask = ~BUTTON_IOPORT_1.IN & BUTTON_BITMASK_1;
	mask |= (~BUTTON_IOPORT_2.IN & BUTTON_BITMASK_2)
			<< BUTTON_BITPOSITION_2;

	return mask;
}

//! \name I/O port config for all board LEDs
//@{
#define LED_IOPORT            PORTE
//@}

static inline void write_board_led_mask(uint8_t mask)
{
	LED_IOPORT.OUT = ~mask;
}

#endif  // _CONF_EXAMPLE_H
