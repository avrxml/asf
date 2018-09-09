/**
 * \file
 *
 * \brief Bitmap data
 *
 * Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries.
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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
