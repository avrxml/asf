/**
 * \file main.c
 *
 * \brief  Main of WSNDemo application
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

/**
 * \page license License
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

#ifndef WSNDEMO_H
#define WSNDEMO_H

# include "board.h"

void wsndemo_init(void);
void wsndemo_task(void);

#ifndef LED_COUNT
#define LED_COUNT 0
#endif

#if LED_COUNT > 2
#define LED_NETWORK       LED0_GPIO
#define LED_DATA          LED1_GPIO
#define LED_BLINK         LED2_GPIO
#define LED_IDENTIFY      LED0_GPIO
#elif LED_COUNT == 2
#define LED_NETWORK       LED0_GPIO
#define LED_DATA          LED1_GPIO
#define LED_BLINK         LED1_GPIO
#define LED_IDENTIFY      LED0_GPIO
#elif LED_COUNT == 1
#define LED_NETWORK       LED0_GPIO
#define LED_DATA          LED0_GPIO
#define LED_BLINK         LED0_GPIO
#define LED_IDENTIFY      LED0_GPIO
#endif

#ifdef LED0_ACTIVE_LEVEL
#define LED_NETWORK_GPIO       LED0_GPIO
#define LED_DATA_GPIO          LED0_GPIO
#define LED_BLINK_GPIO         LED0_GPIO
#define LED_IDENTIFY_GPIO      LED0_GPIO
#define LED_IDENTIFY_ACTIVE_LEVEL  LED0_ACTIVE_LEVEL
#define LED_IDENTIFY_INACTIVE_LEVEL  LED0_ACTIVE_LEVEL
#define LED_NETWORK_ACTIVE_LEVEL  LED0_INACTIVE_LEVEL
#define LED_NETWORK_INACTIVE_LEVEL  LED0_INACTIVE_LEVEL
#define LED_DATA_ACTIVE_LEVEL  LED0_ACTIVE_LEVEL
#define LED_DATA_INACTIVE_LEVEL  LED0_INACTIVE_LEVEL
#define LED_BLINK_ACTIVE_LEVEL  LED0_ACTIVE_LEVEL
#define LED_BLINK_INACTIVE_LEVEL  LED0_INACTIVE_LEVEL
#endif

#endif /* WSNDEMO_H */
