/**
 * \file
 *
 * \brief Management of C42364A LCD Glass component.
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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
#include "slcdc.h"
#include "supc.h"
#include "c42364a.h"
#include "c42364a_font.h"
#include "conf_c42364a_slcdc.h"

/** ASCII definition for char space. */
#define   ASCII_SP                        0x20
/** ASCII char definition for char '0'. */
#define   ASCII_0                         0x30
/** ASCII char definition for char 'A'. */
#define   ASCII_BIG_A                     0x41
/** ASCII char definition for char 'a'. */
#define   ASCII_LITTLE_A                  0x61

/** Definition for maximum length to display as string. */
#if SAM4CP
#define MAX_STRING_LEN  6
#else
#define MAX_STRING_LEN  5
#endif

/** Definition of length of symbols */
#define NUM_SYMBOLS_D     4
#define SYMBOL_D_LEN      7
#if SAM4CP
#define NUM_SYMBOLS_A     6
#else
#define NUM_SYMBOLS_A     5
#endif
#define SYMBOL_A_LEN      14

/* Symbol pixel array definition */
const enum c42364a_symbol_pixel symbol_d0[SYMBOL_D_LEN] = C42364A_PATTERN_SYMBOL_D0;
const enum c42364a_symbol_pixel symbol_d1[SYMBOL_D_LEN] = C42364A_PATTERN_SYMBOL_D1;
const enum c42364a_symbol_pixel symbol_d2[SYMBOL_D_LEN] = C42364A_PATTERN_SYMBOL_D2;
const enum c42364a_symbol_pixel symbol_d3[SYMBOL_D_LEN] = C42364A_PATTERN_SYMBOL_D3;

const enum c42364a_symbol_pixel symbol_d[NUM_SYMBOLS_D][SYMBOL_D_LEN] = {
		C42364A_PATTERN_SYMBOL_D0,
		C42364A_PATTERN_SYMBOL_D1,
		C42364A_PATTERN_SYMBOL_D2,
		C42364A_PATTERN_SYMBOL_D3};
#if SAM4CP
const enum c42364a_symbol_pixel symbol_a1[SYMBOL_A_LEN] = C42364A_PATTERN_SYMBOL_A1;
#endif
const enum c42364a_symbol_pixel symbol_a2[SYMBOL_A_LEN] = C42364A_PATTERN_SYMBOL_A2;
const enum c42364a_symbol_pixel symbol_a3[SYMBOL_A_LEN] = C42364A_PATTERN_SYMBOL_A3;
const enum c42364a_symbol_pixel symbol_a4[SYMBOL_A_LEN] = C42364A_PATTERN_SYMBOL_A4;
const enum c42364a_symbol_pixel symbol_a5[SYMBOL_A_LEN] = C42364A_PATTERN_SYMBOL_A5;
const enum c42364a_symbol_pixel symbol_a6[SYMBOL_A_LEN] = C42364A_PATTERN_SYMBOL_A6;


const enum c42364a_symbol_pixel symbol_a[NUM_SYMBOLS_A][SYMBOL_A_LEN] = {
#if SAM4CP
		C42364A_PATTERN_SYMBOL_A1,
#endif
		C42364A_PATTERN_SYMBOL_A2,
		C42364A_PATTERN_SYMBOL_A3,
		C42364A_PATTERN_SYMBOL_A4,
		C42364A_PATTERN_SYMBOL_A5,
		C42364A_PATTERN_SYMBOL_A6};

/* Symbol plot value definition */
const uint8_t plot_number[10] = {C42364A_PLOT_NUMBER_0, C42364A_PLOT_NUMBER_1,
		C42364A_PLOT_NUMBER_2, C42364A_PLOT_NUMBER_3, C42364A_PLOT_NUMBER_4,
		C42364A_PLOT_NUMBER_5, C42364A_PLOT_NUMBER_6, C42364A_PLOT_NUMBER_7,
		C42364A_PLOT_NUMBER_8, C42364A_PLOT_NUMBER_9};
const uint32_t plot_letter[26] = {C42364A_PLOT_LETTER_A, C42364A_PLOT_LETTER_B,
		C42364A_PLOT_LETTER_C, C42364A_PLOT_LETTER_D, C42364A_PLOT_LETTER_E,
		C42364A_PLOT_LETTER_F, C42364A_PLOT_LETTER_G, C42364A_PLOT_LETTER_H,
		C42364A_PLOT_LETTER_I, C42364A_PLOT_LETTER_J, C42364A_PLOT_LETTER_K,
		C42364A_PLOT_LETTER_L, C42364A_PLOT_LETTER_M, C42364A_PLOT_LETTER_N,
		C42364A_PLOT_LETTER_O, C42364A_PLOT_LETTER_P, C42364A_PLOT_LETTER_Q,
		C42364A_PLOT_LETTER_R, C42364A_PLOT_LETTER_S, C42364A_PLOT_LETTER_T,
		C42364A_PLOT_LETTER_U, C42364A_PLOT_LETTER_V, C42364A_PLOT_LETTER_W,
		C42364A_PLOT_LETTER_X, C42364A_PLOT_LETTER_Y, C42364A_PLOT_LETTER_Z};
const uint32_t plot_letter_num[10] = {C42364A_PLOT_LETTER_0, C42364A_PLOT_LETTER_1,
		C42364A_PLOT_LETTER_2, C42364A_PLOT_LETTER_3, C42364A_PLOT_LETTER_4,
		C42364A_PLOT_LETTER_5, C42364A_PLOT_LETTER_6, C42364A_PLOT_LETTER_7,
		C42364A_PLOT_LETTER_8, C42364A_PLOT_LETTER_9};

/*Each bit mask of uint32 */
static uint32_t mask[32];

/**
 * \brief Initialize mask[32] value.
 */
static void init_mask(void)
{
	uint32_t tmp = 1;
	uint8_t index = 0;

	for(index = 0; index < 32; index++) {
		mask[index] = tmp;
		tmp <<= 1;
	}
}

/**
 * \brief Set pixel in LCD display memory.
 *
 * This function sets a pixel in LCD display memory. If a parameter
 * is out of range, then the function doesn't set any bit.
 *
 * \param  pix_com  Pixel coordinate - COMx - of the pixel.
 * \param  pix_seg  Pixel coordinate - SEGy - of the pixel.
 */
static void c42364a_slcdc_set_pixel(Slcdc *p_slcdc,
		uint8_t pix_com, uint8_t pix_seg)
{
	if (pix_seg < 32)
		slcdc_set_lsb_memory(p_slcdc, pix_com, mask[pix_seg]);
	else
		slcdc_set_msb_memory(p_slcdc, pix_com, mask[pix_seg & 0x1F]);
}

/**
 * \brief Clear pixel in LCD display memory.
 *
 * This function clears a pixel in LCD display memory. If a parameter
 * is out of range, then the function doesn't clear any bit.
 *
 * \param  pix_com  Pixel coordinate - COMx - of the pixel.
 * \param  pix_seg  Pixel coordinate - SEGy - of the pixel.
 */
static void c42364a_slcdc_clear_pixel(Slcdc *p_slcdc,
		uint8_t pix_com, uint8_t pix_seg)
{
	if (pix_seg < 32)
		slcdc_clear_lsb_memory(p_slcdc, pix_com, mask[pix_seg]);
	else
		slcdc_clear_msb_memory(p_slcdc, pix_com, mask[pix_seg & 0x1F]);
}

/**
 * \brief Turn on a symbol according to symbol plot definition.
 *
 * \param p_slcdc Pointer to an SLCDC instance.
 * \param p_symbol  Pointer to an symbol_pixel array.
 * \param number Symbol pixel array number.
 * \param plot   Symbol plot definition value
 */
static void c42364a_slcdc_display_symbol(Slcdc *p_slcdc,
		const enum c42364a_symbol_pixel *p_symbol, uint8_t number, uint32_t plot)
{
	while(number--) {
		if (plot & mask[number])
			c42364a_slcdc_set_pixel(p_slcdc, (*(p_symbol+number)) & 0x03,
					(*(p_symbol+number)) >> 2);
		else
			c42364a_slcdc_clear_pixel(p_slcdc, (*(p_symbol+number)) & 0x03,
					(*(p_symbol+number)) >> 2);
	}
}

/**
 * \brief Turn off a symbol.
 *
 * \param p_slcdc Pointer to an SLCDC instance.
 * \param p_symbol  Pointer to an symbol_pixel array.
 * \param number   Symbol pixel array number.
 */
static void c42364a_slcdc_efface_symbol(Slcdc *p_slcdc,
		const enum c42364a_symbol_pixel *p_symbol, uint8_t number)
{
	while(number --) {
		c42364a_slcdc_clear_pixel(p_slcdc, (*(p_symbol+number)) & 0x03,
				(*(p_symbol+number)) >> 2);
	}
}

/**
 * \brief Display a string in lcd glass alphanumeric field.
 *
 * This function will write the input string to the alphanumeric field of the
 * lcd glass.
 *
 * \param data Pointer to the input string(max length is 6 for SAM4CP
 * and 5 for SAM4C)
 */
static void c42364a_slcdc_display_alphanum_string(const uint8_t *data)
{
	for(uint8_t i=0; i<MAX_STRING_LEN; i++){
		if(data[i] == ASCII_SP) {
			c42364a_slcdc_efface_symbol(SLCDC, symbol_a[i], SYMBOL_A_LEN);
		} else if((ASCII_0 <= data[i]) && (data[i] < ASCII_BIG_A)) {
			c42364a_slcdc_display_symbol(SLCDC, symbol_a[i], SYMBOL_A_LEN,
								plot_letter_num[data[i] - ASCII_0]);
		} else if((ASCII_BIG_A <= data[i]) && (data[i] < ASCII_LITTLE_A)) {
			c42364a_slcdc_display_symbol(SLCDC, symbol_a[i], SYMBOL_A_LEN,
								plot_letter[data[i] - ASCII_BIG_A]);
		} else {
			c42364a_slcdc_display_symbol(SLCDC, symbol_a[i], SYMBOL_A_LEN,
								plot_letter[data[i] - ASCII_LITTLE_A]);
		}
	}
}

/**
 * \brief Display a string in lcd glass numeric field.
 *
 * This function will write the input string to the numeric field of the
 * lcd glass.
 *
 * \param data Pointer to the input string(max length is 4)
 */
static void c42364a_slcdc_display_num_string(const uint8_t *data)
{
	for(uint8_t i=0; i<4; i++){
		if(data[i] == ASCII_SP) {
			c42364a_slcdc_efface_symbol(SLCDC, symbol_d[i], SYMBOL_D_LEN);
		} else {
			c42364a_slcdc_display_symbol(SLCDC, symbol_d[i], SYMBOL_D_LEN,
								  plot_number[data[i] - ASCII_0]);
		}
	}
}

status_code_t c42364a_init(void)
{
	/* SLCDC configuration */
	struct slcdc_config slcdc_cfg;

	init_mask();

	/* Set LCD power mode: Internal supply */
	supc_set_slcd_power_mode(SUPC, CONF_C42364A_POWER_MODE);

	/* SLCDC Reset */
	slcdc_reset(SLCDC);

	/* SLCDC initialization */
	slcdc_cfg.buf_time = CONF_C42364A_BUF_TIME;
	slcdc_cfg.frame_rate= CONF_C42364A_FRAME_RATE;
	slcdc_cfg.disp_mode = CONF_C42364A_DISP_MODE;
	if(slcdc_init(SLCDC, &slcdc_cfg) != STATUS_OK) {
		return STATUS_ERR_BUSY;
	}

	/*LCD seg 17, 20~22, and 24 ~49 mapped on SEGx I/O pin */
	slcdc_set_segmap0(SLCDC, C42364A_SEGMAP_NUM_0);
	slcdc_set_segmap1(SLCDC, C42364A_SEGMAP_NUM_1);

	/* Enable SLCDC */
 	slcdc_enable(SLCDC);
	while(!slcdc_get_status(SLCDC));

	return STATUS_OK;
}

void c42364a_write_alphanum_packet(const uint8_t *data)
{
	c42364a_slcdc_display_alphanum_string(data);
}

void c42364a_write_num_packet(const uint8_t *data)
{
	c42364a_slcdc_display_num_string(data);
}

void c42364a_show_text(const uint8_t *data)
{
	c42364a_write_alphanum_packet(data);
}

void c42364a_clear_text(void)
{
	c42364a_write_alphanum_packet((const uint8_t*)"     ");
}

void c42364a_clear_icon(uint8_t icon_com, uint8_t icon_seg)
{
	c42364a_slcdc_clear_pixel(SLCDC, icon_com, icon_seg);
}

void c42364a_show_icon(uint8_t icon_com, uint8_t icon_seg)
{
	c42364a_slcdc_set_pixel(SLCDC, icon_com, icon_seg);
}

void c42364a_blink_icon_start(uint8_t icon_com, uint8_t icon_seg)
{
	/* SLCDC not support this feature */
	UNUSED(icon_com);
	UNUSED(icon_seg);
}

void c42364a_blink_icon_stop(uint8_t icon_com, uint8_t icon_seg)
{
	/* SLCDC not support this feature */
	UNUSED(icon_com);
	UNUSED(icon_seg);
}

void c42364a_blink_screen(void)
{
	slcdc_set_display_mode(SLCDC, SLCDC_DISPMODE_BLINKING);
	slcdc_set_blink_freq(SLCDC, CONF_C42364A_BLINK_FREQ, true);
}

void c42364a_blink_disable(void)
{
	slcdc_set_display_mode(SLCDC, SLCDC_DISPMODE_NORMAL);
}

void c42364a_set_contrast(int8_t contrast)
{
	Assert((0<=contrast) && (contrast < 16));
	uint8_t voltage[16] = {7, 6, 5, 4, 3, 2, 1, 0, 15, 14, 13, 12, 11, 10, 9, 8};

	supc_set_slcd_vol(SUPC, voltage[contrast]);
}

void c42364a_circular_animation_start(uint8_t csr_dir,
		uint8_t size, uint8_t data)
{
	/* SLCDC not support this feature */
	UNUSED(csr_dir);
	UNUSED(size);
	UNUSED(data);
}

void c42364a_circular_animation_stop(void)
{
	/* SLCDC not support this feature */
}

void c42364a_text_scrolling_start(const uint8_t *data, uint32_t length)
{
	/* SLCDC not support this feature */
	UNUSED(data);
	UNUSED(length);
}

void c42364a_text_scrolling_stop(void)
{
	/* SLCDC not support this feature */
}

void c42364a_show_all(void)
{
	slcdc_set_display_memory(SLCDC);
}

void c42364a_clear_all(void)
{
	slcdc_clear_display_memory(SLCDC);
}

void c42364a_show_numeric_dec(int32_t value)
{
	uint8_t lcd_num[5];

	Assert(value >= 0);
	Assert(value <= 9999);

	sprintf((char*)lcd_num, "%4d", (int)value);

	c42364a_write_num_packet((uint8_t const*)&lcd_num);
}

void c42364a_clear_numeric_dec(void)
{
	c42364a_write_num_packet((const uint8_t*)"    ");
}

void c42364a_show_battery(enum c42364a_battery_value val)
{
	c42364a_slcdc_clear_pixel(SLCDC, C42364A_ICON_BAT_LEVEL_1);
	c42364a_slcdc_clear_pixel(SLCDC, C42364A_ICON_BAT_LEVEL_2);
	c42364a_slcdc_clear_pixel(SLCDC, C42364A_ICON_BAT_LEVEL_3);
	c42364a_slcdc_set_pixel(SLCDC, C42364A_ICON_BAT);
	if (val > 2) {
		c42364a_slcdc_set_pixel(SLCDC, C42364A_ICON_BAT_LEVEL_3);
		c42364a_slcdc_set_pixel(SLCDC, C42364A_ICON_BAT_LEVEL_2);
		c42364a_slcdc_set_pixel(SLCDC, C42364A_ICON_BAT_LEVEL_1);
	}
	if (val > 1) {
		c42364a_slcdc_set_pixel(SLCDC, C42364A_ICON_BAT_LEVEL_2);
		c42364a_slcdc_set_pixel(SLCDC, C42364A_ICON_BAT_LEVEL_1);
	}
	if (val > 0) {
		c42364a_slcdc_set_pixel(SLCDC, C42364A_ICON_BAT_LEVEL_1);
	}
}
