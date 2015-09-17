/**
 * \file
 *
 * \brief Bitmap data
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
#include "compiler.h"
#include "gfx_mono.h"

// 8x10 pixel cloud icon
PROGMEM_DECLARE(gfx_mono_color_t, cloud_data[10]) = {
		0x60, 0x50, 0xa0, 0x98, 0x84, 0x82, 0x8c, 0x84, 0x4c, 0x30};

// 8x9 pixel indoor lighting icon
PROGMEM_DECLARE(gfx_mono_color_t, indoor_data[9]) = {
		0x10, 0xa, 0x2, 0x1a, 0x2, 0x1a, 0x2, 0xa, 0x10};

// 8x5 pixel moon icon
PROGMEM_DECLARE(gfx_mono_color_t, moon_data[5]) = {
		0x3c, 0x7e, 0xff, 0x81, 0x80};

// 8x10 pixel sun icon
PROGMEM_DECLARE(gfx_mono_color_t, sun_data[10]) = {
		0x8, 0x89, 0x42, 0x18, 0x3d, 0xbc, 0x18, 0x42, 0x91, 0x10};

struct gfx_mono_bitmap bitmap_cloud = { .height = 8, .width = 10,
		.type = GFX_MONO_BITMAP_PROGMEM, .data.progmem = cloud_data};

struct gfx_mono_bitmap bitmap_indoor = { .height = 8, .width = 9,
		.type = GFX_MONO_BITMAP_PROGMEM, .data.progmem = indoor_data};

struct gfx_mono_bitmap bitmap_moon = { .height = 8, .width = 5,
		.type = GFX_MONO_BITMAP_PROGMEM, .data.progmem = moon_data};

struct gfx_mono_bitmap bitmap_sun = { .height = 8, .width = 10,
		.type = GFX_MONO_BITMAP_PROGMEM, .data.progmem = sun_data};
