/**
 * \file main.c
 *
 * \brief  Main of WSNDemo application
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 */

/**
 * \page license License
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
