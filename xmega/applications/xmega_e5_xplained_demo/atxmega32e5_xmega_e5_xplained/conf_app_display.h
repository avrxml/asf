/**
 * \file
 *
 * \brief Display organization for this application.
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
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
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#ifndef CONF_APP_DISPLAY_H_INCLUDED
#define CONF_APP_DISPLAY_H_INCLUDED

#define DISPLAY_INTRO_ATMEL_DELAY           3000lu /* Unit ms */
#define DISPLAY_INTRO_MSG_EXIT              "SW1 to skip intro"
#define DISPLAY_INTRO_MSG_DELAY             7000lu /* Unit ms */
#define DISPLAY_INTRO_MSG_A                 "The demo gets light"
#define DISPLAY_INTRO_MSG_B                 "sensor and outputs"
#define DISPLAY_INTRO_MSG_C                 "them on DISP, USART"
#define DISPLAY_INTRO_HELP_DELAY            3000lu /* Unit ms */
#define DISPLAY_INTRO_HELP_A                "Quadrature encoder"
#define DISPLAY_INTRO_HELP_B                "changes sampling"

#define DISPLAY_CPU_LOAD_TEXT_POS_X         0
#define DISPLAY_CPU_LOAD_TEXT_POS_Y         0
#define DISPLAY_CPU_LOAD_TEXT               "CPU load"
#define DISPLAY_CPU_LOAD_PROBAR_START_POS_X DISPLAY_CPU_LOAD_TEXT_POS_X
#define DISPLAY_CPU_LOAD_PROBAR_START_POS_Y (DISPLAY_CPU_LOAD_TEXT_POS_Y + 9)
#define DISPLAY_CPU_LOAD_PROBAR_START_SIZE_X 1
#define DISPLAY_CPU_LOAD_PROBAR_START_SIZE_Y (DISPLAY_CPU_LOAD_PROBAR_SIZE_Y + 2)
#define DISPLAY_CPU_LOAD_PROBAR_STOP_POS_X  (DISPLAY_CPU_LOAD_TEXT_POS_X + DISPLAY_CPU_LOAD_PROBAR_MAX_SIZE_X +1)
#define DISPLAY_CPU_LOAD_PROBAR_STOP_POS_Y  DISPLAY_CPU_LOAD_PROBAR_START_POS_Y
#define DISPLAY_CPU_LOAD_PROBAR_STOP_SIZE_X 1
#define DISPLAY_CPU_LOAD_PROBAR_STOP_SIZE_Y (DISPLAY_CPU_LOAD_PROBAR_SIZE_Y + 2)
#define DISPLAY_CPU_LOAD_PROBAR_POS_X       (DISPLAY_CPU_LOAD_TEXT_POS_X + 1)
#define DISPLAY_CPU_LOAD_PROBAR_POS_Y       (DISPLAY_CPU_LOAD_TEXT_POS_Y + 11)
#define DISPLAY_CPU_LOAD_PROBAR_SIZE_Y      (4)
#define DISPLAY_CPU_LOAD_PROBAR_MAX_SIZE_X  (6 * 8)

#define DISPLAY_SAMPLING_TEXT_POS_X         ((128 / 2) + 3)
#define DISPLAY_SAMPLING_TEXT_POS_Y         0
#define DISPLAY_SAMPLING_TEXT               "Sampling\r\n%2u,%01us"
#define DISPLAY_SAMPLING_PROCIRCLE_POS_X    (DISPLAY_SAMPLING_TEXT_POS_X + 6 * 5 + 13)
#define DISPLAY_SAMPLING_PROCIRCLE_POS_Y    (DISPLAY_SAMPLING_TEXT_POS_Y + 9 + 5)
#define DISPLAY_SAMPLING_PROCIRCLE_RADIUS   (4)
#define DISPLAY_SAMPLING_PROCIRCLE_MAX_SIZE_X  (6 * 8)

#define DISPLAY_LIGHT_TEXT_POS_X            DISPLAY_SAMPLING_TEXT_POS_X
#define DISPLAY_LIGHT_TEXT_POS_Y            19
#define DISPLAY_LIGHT_TEXT                  "Light"
#define DISPLAY_LIGHT_PROBAR_START_POS_X    DISPLAY_LIGHT_TEXT_POS_X
#define DISPLAY_LIGHT_PROBAR_START_POS_Y    (DISPLAY_LIGHT_TEXT_POS_Y + 9)
#define DISPLAY_LIGHT_PROBAR_START_SIZE_X   1
#define DISPLAY_LIGHT_PROBAR_START_SIZE_Y   (DISPLAY_LIGHT_PROBAR_SIZE_Y + 2)
#define DISPLAY_LIGHT_PROBAR_STOP_POS_X     (DISPLAY_LIGHT_TEXT_POS_X + DISPLAY_LIGHT_PROBAR_MAX_SIZE_X +1)
#define DISPLAY_LIGHT_PROBAR_STOP_POS_Y     DISPLAY_LIGHT_PROBAR_START_POS_Y
#define DISPLAY_LIGHT_PROBAR_STOP_SIZE_X    1
#define DISPLAY_LIGHT_PROBAR_STOP_SIZE_Y    (DISPLAY_LIGHT_PROBAR_SIZE_Y + 2)
#define DISPLAY_LIGHT_PROBAR_POS_X          DISPLAY_LIGHT_TEXT_POS_X
#define DISPLAY_LIGHT_PROBAR_POS_Y          (DISPLAY_LIGHT_TEXT_POS_Y + 9)
#define DISPLAY_LIGHT_PROBAR_SIZE_Y         (4)
#define DISPLAY_LIGHT_PROBAR_MAX_SIZE_X     (6 * 6)

#endif /* CONF_APP_DISPLAY_H_INCLUDED */
