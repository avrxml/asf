/**
 * \file
 *
 * \brief Chip-specific example configuration
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
#ifndef CONF_EXAMPLE_H
#define CONF_EXAMPLE_H

#include <adc.h>
#include <board.h>

/* Refer to the example source for detailed documentation. */

//! \note This is pin 1 (PB0) on the A3BU Xplained J2 header
#define INPUT_PIN              ADCCH_POS_PIN0
//! \note This is LED0 on the A3BU Xplained.
#define LED1_PIN               LED0_GPIO
//! \note This is LED1 on the A3BU Xplained.
#define LED2_PIN               LED1_GPIO
//! \note This is push button 0 on the A3BU Xplained.
#define BUTTON_PIN             GPIO_PUSH_BUTTON_0
//! \note This gives 2^6 = 64 samples.
#define OVERSAMPLING_FACTOR    6

//! \note Size of the string output to the display
#define OUTPUT_STR_SIZE        32

#endif /* CONF_EXAMPLE_H */
