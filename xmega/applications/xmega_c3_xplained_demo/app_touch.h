/**
 * \file
 *
 * \brief Declaration of functions for touch interfacing
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

#ifndef _APP_TOUCH_H_
#define _APP_TOUCH_H_

/**
 * \ingroup demo_xmegac3_xplained_group
 * \defgroup touch_task_group Touch task
 *
 * The QTouch is managed by a library, and the QTouch sensor measurement must
 * be called each 25ms by the main task. In this demo application a TC counter
 * is used to schedule it. The implementation is available in app_touch.c/.h
 * files.
 *
 * @{
 */

/*! \brief Configures touch library and measurement setup
 *
 * \note Interrupts must be enabled in order to trigger regular measurements
 * The touch configuration requires a fixed CPU speed to work properly in this
 * case we need 24 MHz.
 */
void app_touch_init(void);

/*! \brief Configures touch library and measurement setup
 *
 * \note Interrupts must be enabled in order to trigger regular measurements
 * The touch configuration requires a fixed CPU speed to work properly in this
 * case we need 24 MHz.
 */
void app_touch_task(void);

/* ! \brief Checks whether the QTouch key is pressed */
bool app_touch_check_key_pressed(uint8_t button_num);

/* ! @} */

#endif /* _APP_TOUCH_H_ */
