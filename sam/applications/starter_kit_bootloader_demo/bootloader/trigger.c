/**
 * \file
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

#include "bootloader.h"

/**
 * Initialize triggers
 */
void trigger_init(void)
{
	dbg_print("trigger: init ...\r\n");
	/* Initialized in board_init */
	dbg_print("trigger: init done\r\n");
}

/**
 * Cleanup triggers
 */
void trigger_cleanup(void)
{
	dbg_print("trigger: cleanup ...\r\n");
	/* Nothing to do */
	dbg_print("trigger: cleanup done\r\n");
}

/**
 * Check if triggers asserted
 * \info The information loaded from memory to check
 */
enum trigger_modes trigger_poll(const struct regions_info *info)
{
#ifdef TRIGGER_USE_BUTTONS
#  ifdef TRIGGER_LOAD_BUTTON
	if (TRIGGER_LOAD_BUTTON_ACTIVE ==
		ioport_get_pin_level(TRIGGER_LOAD_BUTTON)) {
		return TRIGGER_LOAD;
	}
#  endif
#  ifdef TRIGGER_SWITCH_BUTTON
	if (TRIGGER_SWITCH_BUTTON_ACTIVE ==
		ioport_get_pin_level(TRIGGER_SWITCH_BUTTON)) {
		return TRIGGER_UPDATE;
	}
#  endif
#endif
#ifdef TRIGGER_USE_FLAG
	return (enum trigger_modes)info->trigger;
#else
	/* No trigger */
	return TRIGGER_BOOT;
#endif
}

