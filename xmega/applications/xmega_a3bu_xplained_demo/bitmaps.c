/**
 * \file
 *
 * \brief Bitmap data
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
