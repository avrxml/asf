/**
 * \file
 *
 * \brief Bitmap data
 *
 * Copyright (C) 2011-2015 Atmel Corporation. All rights reserved.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>

/* 16x8 pixel cooking pot on plate */
PROGMEM_DECLARE(gfx_mono_color_t, pot_on_data[16]) = {
	0x40, 0xc0, 0xc0, 0xe0, 0xe0, 0xc0, 0xc0, 0x40,
	0x70, 0x5f, 0x98, 0x9f, 0x9f, 0x9f, 0x5f, 0x70
};

/* 16x8 pixel cooking pot off plate */
PROGMEM_DECLARE(gfx_mono_color_t, pot_off_data[16]) = {
	0x02, 0xfe, 0xc6, 0xff, 0xff, 0xfe, 0xfe, 0x02,
	0x70, 0x50, 0x88, 0x88, 0x88, 0x88, 0x50, 0x70
};

/** Meta bitmap container for pot on heater */
struct gfx_mono_bitmap bitmap_pot_on = {
	.height = 16,
	.width = 8,
	.type = GFX_MONO_BITMAP_PROGMEM,
	.data.progmem = pot_on_data
};

/** Meta bitmap container for pot off heater */
struct gfx_mono_bitmap bitmap_pot_off = {
	.height = 16,
	.width = 8,
	.type = GFX_MONO_BITMAP_PROGMEM,
	.data.progmem = pot_off_data
};
