/**
 * \file *********************************************************************
 *
 * \brief AVR477 Board QTouch Implementation
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
 */

/*
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

#ifndef _AVR477QTOUCH_H
#define _AVR477QTOUCH_H

/******************************************************************************
*                  Includes section
******************************************************************************/

/**************************************************************************//**
*                   Types section
******************************************************************************/
typedef enum _BSP_TouchEvent_t {
	TOUCHKEY_NO_EVENT,
	TOUCHKEY_CHANGED_EVENT,
	TOUCHKEY_PRESSED_EVENT,
	TOUCHKEY_RELEASED_EVENT,
	TOUCHKEY_THRESHOLD_REACHED_EVENT,
	TOUCHKEY_LEVEL_CHANGED_EVENT,
} BSP_TouchEvent_t;

typedef enum _BSP_ButtonEvent_t {
	BUTTON_NO_EVENT,
	BUTTON_CHANGED_EVENT,
	BUTTON_PRESSED_EVENT,
	BUTTON_RELEASED_EVENT,
	BUTTON_THRESHOLD_REACHED_EVENT,
	BUTTON_LEVEL_CHANGED_EVENT,
} BSP_ButtonEvent_t;

/* Key-Button Mapping
 * These defines are basically a mapping between the keys on the AVR477 RF Touch
 * Board and the buttons on the Red Key Remote Control Board.
 *
 * For Example, in present implementation user wants to send 'ON' command
 * when key SW16 is touched on AVR477 board. Since in red key remote control
 * board,
 * BUTTON_LEFT_P is used for this function, the KEY_SW16 is mapped to
 * BUTTON_LEFT_P;
 *
 * This mapping needs to be changed, if user wants to change the intended
 * functionality when a key is touched.
 */

#define KEY_SW16        (0x00)
#define KEY_SW09        (0x01)
#define KEY_SW10        (0x02)
#define KEY_SW17        (0x03)
#define KEY_SW14        (0x04)
#define KEY_SW11        (0x05)
#define KEY_SW12        (0x06)
#define KEY_SW15        (0x07)
#define KEY_SW13        (0x08)
#define KEY_ROT_CW      (0x09)
#define KEY_ROT_CCW     (0x0A)

/* #define KEY_SW01        (0x0C) */
/* #define KEY_SW02        (0x0D) */
/* #define KEY_SW03        (0x0E) */
/* #define KEY_SW04        (0x0F) */
/* #define KEY_SW05        (0x10) */
/* #define KEY_SW06        (0x11) */
/* #define KEY_SW07        (0x12) */
/* #define KEY_SW08        (0x13) */
#define INVALID_KEY     (0x0B)

/** Type of buttons event handler */
typedef void (*BSP_TouchEventHandler_t)(BSP_TouchEvent_t event, uint8_t button,
		uint8_t data);

uint8_t Touch_measurement(uint8_t *button_event, uint8_t *button_state);
void BSP_InitQTouch(BSP_TouchEventHandler_t handler);
void appButtonsInd(BSP_TouchEvent_t event, uint8_t button, uint8_t data);

/**************************************************************************//**
*  \brief Set threshold value for a button
*  \param[in] button - button index
*  \param[in] threshold - value to set
******************************************************************************/
void BSP_SetButtonThreshold(uint8_t button, uint8_t threshold);

/**************************************************************************//**
*  \brief Determine if specified button is currently pressed.
*  \param[in] button - button index
*  \returns true - button is pressed
*  \returns false - button is released
******************************************************************************/
bool BSP_IsButtonPressed(uint8_t button);

/** Type of buttons event handler */
typedef void (*BSP_ButtonEventHandler_t)(BSP_ButtonEvent_t event,
		uint8_t button, uint8_t data);

#endif /* _AVR477QTOUCH_H */

/* eof bspDbg.h */
