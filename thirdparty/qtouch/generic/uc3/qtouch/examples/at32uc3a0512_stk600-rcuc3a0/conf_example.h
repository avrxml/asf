/*****************************************************************************
 *
 * \file
 *
 * \brief qtouch_example.c file configurations for UC3A0 devices.
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
 


#ifndef CONF_EXAMPLE_H_
#define CONF_EXAMPLE_H_

/* MACROS FOR TIMER */ 
#define TC_CHANNEL       0						// Channel 0 is assigned
#define TC_IRQ           AVR32_TC_IRQ0
#define TC_CLOCK         TC_CLOCK_SOURCE_TC3	// Clock source 3->timer clock source = PBA clock/8

/* MACROS FOR INTERRUPT COMTROLLER */
#if defined(__GNUC__)
#define AVR32_INTC_INT0  0 
#endif
#define GRP              14						// timer interrupt lies under group0
#define LVL              0						// Interrupt priority level

#define STATUS_LED       AVR32_PIN_PB08			// On STK600 PORTF.F0 to any of the LEDs




#endif /* CONF_EXAMPLE_H_ */
