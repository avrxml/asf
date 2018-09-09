/**
 * \file
 *
 * \brief  ATMEGA256RFR2 RCB board LEDs support package.
 *
 * This file contains definitions and services related to the LED features of
 * the ATMEGA256RFR2 XPLAINED PRO board.
 *
 * To use this board, define BOARD=ATMEGA256RFR2_XPLAINED_PRO.
 *
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
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

#ifndef _LED_H_
#define _LED_H_


typedef enum led_id_tag
{
    LED_0,
    LED_1,
    LED_2,
    LED_3,
    LED_4,
    LED_5
} SHORTENUM led_id_t;
/**
 * \brief LED action
 */
typedef enum led_action_tag
{
    /** Switch LED on. */
    LED_ON,
    /** Switch LED off. */
    LED_OFF,
    /** Toggle LED. */
    LED_TOGGLE
} SHORTENUM led_action_t;


/*
 * Bit numbers address where LEDs are mapped to.
 */
#define LED_BIT_0                       (0)
#define LED_BIT_1                       (1)

/*
 * LED bit mask
 */
#define LED_BIT_MASK                    ((1 << LED_BIT_0) | (1 << LED_BIT_1))
#define LED_PORT                        (PORTB)
#define LED_PORT_DIR                    (DDRB)

/*! \brief Turns off the specified LEDs.
 *
 * \param led_gpio LED to turn off (LEDx_GPIO).
 *
 * \note The pins of the specified LEDs are set to GPIO output mode.
 */
#define LED_Off(led_gpio)     led_ctrl(led_gpio,LED_OFF)

                                  

/*! \brief Turns on the specified LEDs.
 *
 * \param led_gpio LED to turn on (LEDx_GPIO).
 *
 * \note The pins of the specified LEDs are set to GPIO output mode.
 */
#define LED_On(led_gpio)      led_ctrl(led_gpio,LED_ON)

/*! \brief Toggles the specified LEDs.
 *
 * \param led_gpio LED to toggle (LEDx_GPIO).
 *
 * \note The pins of the specified LEDs are set to GPIO output mode.
 */
#define LED_Toggle(led_gpio)  led_ctrl(led_gpio,LED_TOGGLE)

#endif /* _LED_H_ */
