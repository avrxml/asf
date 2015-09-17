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
#define PIN_PUSHBUTTON_WAKEUP_PIO    PIOA
#define PIN_PUSHBUTTON_WAKEUP_MASK   PIO_PA2
#define PIN_PUSHBUTTON_WAKEUP_ID     ID_PIOA
#define PIN_PUSHBUTTON_WAKEUP_ATTR   PIO_DEFAULT
/** @} */

/** Wakeup pin for wait mode: SW0 */
#define WAKEUP_WAIT_INPUT_ID    (1u << 2)
/** Wakeup pin for backup mode: Touchscreen controller IRQ pin */
#define WAKEUP_BACKUP_INPUT_ID  (1u << 2)

/** Hint message for active mode */
#define STRING_ACTIVE \
		"Entering active mode:\n\r" \
		"- Press SW0 button to go out.\n\r"

/** Hint message for sleep mode */
#define STRING_SLEEP \
		"Enter into sleep mode.\n\r" \
		"- Press SW0 button to wake up.\n\r"

/** Hint message for wait mode */
#define STRING_WAIT \
		"Entering wait mode:\n\r" \
		"- Switching to 8MHz Fast RC oscillator and stop PLL.\n\r" \
		"- Press SW0 button to wake up.\n\r"

/** Hint message for backup mode */
#define STRING_BACKUP \
		"Enter into backup mode.\n\r" \
		"- Press SW0 button to wake up.\n\r"

#define CLOCK_LIST_MENU \
		"\n\rSelect one of the following clock configurations:\n\r" \
		"  1: 250KHz from Fast RC\n\r"                     \
		"  2: 500KHz from Fast RC\n\r"                     \
		"  3: 1MHz from Fast RC\n\r"                       \
		"  4: 2MHz from Fast RC\n\r"                       \
		"  5: 4MHz from Fast RC\n\r"                       \
		"  6: 8MHz from Fast RC\n\r"                       \
		"  7: 16MHz from Fast RC\n\r"                      \
		"  8: 24MHz from Fast RC\n\r"                      \
		"  a: 48MHz from PLL clock\n\r"                    \
		"  b: 96MHz from PLL clock\n\r"                    \

#define MIN_CLOCK_FAST_RC_ITEM '1'
#define MAX_CLOCK_FAST_RC_ITEM '8'
#define MIN_CLOCK_PLL_ITEM     'a'
#define MAX_CLOCK_PLL_ITEM     'b'

#define example_set_wakeup_from_backup_mode() \
	supc_set_wakeup_inputs(SUPC, WAKEUP_BACKUP_INPUT_ID, \
			0)

extern uint32_t g_fastrc_clock_list[][3];
extern uint32_t g_pll_clock_list[][4];

#define PLL_DEFAULT_MUL  1465
#define PLL_DEFAULT_DIV  1
#define MCK_DEFAULT_DIV  PMC_MCKR_PRES_CLK_1

void init_specific_board(void);

#endif /* LOW_POWER_BOARD_H_INCLUDED */
