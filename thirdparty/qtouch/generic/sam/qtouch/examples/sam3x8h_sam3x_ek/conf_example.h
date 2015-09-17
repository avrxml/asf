/**
 * \file
 *
 * \brief Example configuration.
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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

#ifndef CONF_EXAMPLE_H_INCLUDED
#define CONF_EXAMPLE_H_INCLUDED

/** Qtouch key number */
#define BOARD_KEY_NUM_5

/** The PIO numbers for silder is before key */
#define BOARD_SILDER_BEFOR_KEY

/** Qtouch valid key ID */
#define BOARD_VALID_KEY_ID    1

/** Qtouch up key ID */
#define BOARD_UP_KEY_ID    2

/** Qtouch down key ID */
#define BOARD_DOWN_KEY_ID    3

/** Qtouch left key ID */
#define BOARD_LEFT_KEY_ID    4

/** Qtouch right key ID */
#define BOARD_RIGHT_KEY_ID    5

/** Qtouch valid key channel */
#define BOARD_VALID_KEY_CHANNEL    CHANNEL_25

/** Qtouch up key channel */
#define BOARD_UP_KEY_CHANNEL    CHANNEL_26

/** Qtouch down key channel */
#define BOARD_DOWN_KEY_CHANNEL    CHANNEL_27

/** Qtouch left key channel */
#define BOARD_LEFT_KEY_CHANNEL    CHANNEL_28

/** Qtouch right key channel */
#define BOARD_RIGHT_KEY_CHANNEL    CHANNEL_29

/** Qtouch slider start channel */
#define BOARD_SLIDER_START_CHANNEL    CHANNEL_0

/** Qtouch slider end channel */
#define BOARD_SLIDER_END_CHANNEL    CHANNEL_2

#endif /* CONF_EXAMPLE_H_INCLUDED */
