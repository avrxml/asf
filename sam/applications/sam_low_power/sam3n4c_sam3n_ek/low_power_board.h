/**
 * \file
 *
 * \brief Board specific definition for low power example.
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
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef LOW_POWER_BOARD_H_INCLUDED
#define LOW_POWER_BOARD_H_INCLUDED

/**
 * Push button definitions for sleep mode and active mode
 * @{
 */
#define PIN_PUSHBUTTON_WAKEUP_PIO    PIN_PUSHBUTTON_1_PIO
#define PIN_PUSHBUTTON_WAKEUP_MASK   PIN_PUSHBUTTON_1_MASK
#define PIN_PUSHBUTTON_WAKEUP_ID     PIN_PUSHBUTTON_1_ID
#define PIN_PUSHBUTTON_WAKEUP_ATTR   PIN_PUSHBUTTON_1_ATTR
/** @} */

/** Wakeup pin for wait mode: USRPB1 */
#define WAKEUP_WAIT_INPUT_ID    (1u << 14)

/** Wakeup pin for backup mode: USRPB1 */
#define WAKEUP_BACKUP_INPUT_ID  (1u << 14)

/** Hint message for active mode */
#define STRING_ACTIVE \
		"Enter into active mode.\n\r" \
		"- Press USR-LEFT button to go out.\n\r"

/** Hint message for sleep mode */
#define STRING_SLEEP \
		"Enter into sleep mode.\n\r" \
		"- Press USR-LEFT button to wake up.\n\r"

/** Hint message for wait mode */
#define STRING_WAIT \
		"Enter into wait mode.\n\r" \
		"- Switch to 4MHz Fast RC oscillator, PLL stopped.\n\r" \
		"- Press USR-LEFT button to wake up.\n\r"

/** Hint message for backup mode */
#define STRING_BACKUP \
		"Enter into backup mode.\n\r" \
		"- Press USR-LEFT button to wake up.\n\r"

#define CLOCK_LIST_MENU \
		"\n\rSelect one of the following clock configurations:\n\r" \
		"  1: 125KHz from Fast RC\n\r"                     \
		"  2: 250KHz from Fast RC\n\r"                     \
		"  3: 500KHz from Fast RC\n\r"                     \
		"  4: 1MHz from Fast RC\n\r"                       \
		"  5: 2MHz from Fast RC\n\r"                       \
		"  6: 4MHz from Fast RC\n\r"                       \
		"  7: 8MHz from Fast RC\n\r"                       \
		"  8: 12MHz from Fast RC\n\r"                      \
		"  a: 24MHz from PLL clock\n\r"                    \
		"  b: 32MHz from PLL clock\n\r"                    \
		"  c: 48MHz from PLL clock\n\r"

#define MIN_CLOCK_FAST_RC_ITEM '1'
#define MAX_CLOCK_FAST_RC_ITEM '8'
#define MIN_CLOCK_PLL_ITEM     'a'
#define MAX_CLOCK_PLL_ITEM     'd'

extern uint32_t g_fastrc_clock_list[][3];
extern uint32_t g_pll_clock_list[][4];

void init_specific_board(void);

#endif /* LOW_POWER_BOARD_H_INCLUDED */
