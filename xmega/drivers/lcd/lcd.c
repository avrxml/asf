/**
 * \file
 *
 * \brief AVR XMEGA Liquid Crystal Display driver.
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
#include "lcd.h"

/**
 * \ingroup lcd_group
 *
 * @{
 */

//! \internal Local storage of LCD interrupt callback function
static lcd_callback_t lcd_frame_callback = NULL;


/**
 * \internal
 * \brief Get pixel register from coordinates.
 *
 * Driver-internal function for getting the address of the register which
 * controls the state for the specified pixel coordinate pair.
 *
 * \param  pix_com  Pixel coordinate - COMx - of the pixel (icon).
 * \param  pix_seg  Pixel coordinate - SEGy - of the pixel (icon).
 *
 * \return Address of register controlling specified pixel.
 */
static inline register8_t *lcd_get_pixel_register(uint8_t pix_com,
		uint8_t pix_seg)
{
	register8_t *reg;
	reg = (register8_t*)((uint16_t)&LCD.DATA0)
			+ (pix_com * ((LCD_MAX_NBR_OF_SEG + 7) / 8))
			+ (pix_seg / 8);
	return reg;
}


/**
 * \brief Interrupt handler for LCD frame
 *
 * This function will handle interrupt on LCD frame and call the callback
 * function.
 */
ISR(LCD_INT_vect)
{
	if (lcd_frame_callback) {
		lcd_frame_callback();
	}
}

/**
 * \brief LCD frame interrupt callback function
 *
 * This function allows the caller to set and change the interrupt callback
 * function. Without setting a callback function the interrupt handler in the
 * driver will only clear the interrupt flags.
 *
 * \param callback Reference to a callback function
 */
void lcd_set_frame_interrupt_callback(lcd_callback_t callback)
{
	lcd_frame_callback = callback;
}


/**
 * \brief Send a sequence of ASCII characters to LCD device.
 *
 * This function enables LCD segments via the digit decoder.
 * The function will write the maximum number of byte passed as argument,
 * and will stop writing if a NULL character is found.
 *
 * \param  lcd_tdg    Type of digit decoder.
 * \param  first_seg  First SEG where the first data will be written.
 * \param  data       Data buffer.
 * \param  width      Maximum Number of data.
 * \param  dir        Direction (==0: Left->Right, !=0: Left<-Right).
 */
void lcd_write_packet(enum LCD_TDG_enum lcd_tdg, uint8_t first_seg,
		const uint8_t *data, size_t width, uint8_t dir)
{
	LCD.CTRLG = lcd_tdg | ((first_seg << LCD_STSEG_gp) & LCD_STSEG_gm);

#ifdef CONFIG_XMEGA_128B1_REVA
	if (dir) {
		data += (width-1);
	}
	while (width--) {
		if (*data == '\0') {
			break; // Stop on NULL char
		}
		LCD.CTRLH = *data;
		if (dir) {
			data--;
		} else {
			data++;
		}
	}
#else // XMEGA_B rev. higher than revA
	if (dir != 0) {
		dir = LCD_DEC_bm;
	}
	while (width--) {
		if (*data == '\0') {
			break; // Stop on NULL char
		}
		LCD.CTRLH = dir | (*data++);
	}
#endif
}


/*! \brief Wait for n interrupt periods.
 *
 *  This function counts interrupt periods and returns when the count is
 *  reached. This function only runs if LCD interrupt is disabled.
 *
 *  \param  n_int  Number of interrupt periods.
 */
void lcd_wait_n_int(uint8_t n_int)
{
#ifdef CONFIG_XMEGA_128B1_REVA
	// Only if XIME is set and if LCD interrupt is disabled
	if (LCD.INTCTRL == (LCD_XIME0_bm | LCD_INTLVL_OFF_gc ))
#else // XMEGA_B rev. higher than revA
	// Only if LCD interrupt is disabled
	if ((LCD.INTCTRL & LCD_FCINTLVL_gm) == LCD_INTLVL_OFF_gc)
#endif
	{
		LCD.INTFLAG = LCD_FCIF_bm;
		while (n_int) {
			if (LCD.INTFLAG) {
				n_int--;
				LCD.INTFLAG = LCD_FCIF_bm;
			}
		}
	}
}


/**
 * \brief Set pixel (icon) in LCD display memory.
 *
 * This function sets a pixel in LCD (icon) display memory. If a parameter
 * is out of range, then the function doesn't set any bit.
 *
 * \param  pix_com  Pixel coordinate - COMx - of the pixel (icon).
 * \param  pix_seg  Pixel coordinate - SEGy - of the pixel (icon).
 */
void lcd_set_pixel(uint8_t pix_com, uint8_t pix_seg)
{
	if ((pix_com < LCD_MAX_NR_OF_COM) && (pix_seg < LCD_MAX_NBR_OF_SEG)) {
		register8_t *pixreg = lcd_get_pixel_register(pix_com, pix_seg);

		*pixreg |= 1 << (pix_seg % 8);
	}
}


/**
 * \brief Clear pixel (icon) in LCD display memory.
 *
 * This function clears a pixel in LCD (icon) display memory. If a parameter
 * is out of range, then the function doesn't clear any bit.
 *
 * \param  pix_com  Pixel coordinate - COMx - of the pixel (icon).
 * \param  pix_seg  Pixel coordinate - SEGy - of the pixel (icon).
 */
void lcd_clear_pixel(uint8_t pix_com, uint8_t pix_seg)
{
	if ((pix_com < LCD_MAX_NR_OF_COM) && (pix_seg < LCD_MAX_NBR_OF_SEG)) {
		register8_t *pixreg = lcd_get_pixel_register(pix_com, pix_seg);

		*pixreg &= ~(1 << (pix_seg % 8));
	}
}


/**
 * \brief Toggle pixel (icon) in LCD display memory.
 *
 * This function toggles a pixel in LCD (icon) display memory. If a parameter
 * is out of range, then the function doesn't toggle any bit.
 *
 * \param  pix_com  Pixel coordinate - COMx - of the pixel (icon).
 * \param  pix_seg  Pixel coordinate - SEGy - of the pixel (icon).
 */
void lcd_tgl_pixel(uint8_t pix_com, uint8_t pix_seg)
{
	if ((pix_com < LCD_MAX_NR_OF_COM) && (pix_seg < LCD_MAX_NBR_OF_SEG)) {
		uint8_t pixels = *lcd_get_pixel_register(pix_com, pix_seg);

		if (pixels & (1 << (pix_seg % 8))) {
			lcd_clear_pixel(pix_com, pix_seg);
		} else {
			lcd_set_pixel(pix_com, pix_seg);
		}
	}
}


/**
 * \brief Get pixel value (icon) in LCD display memory.
 *
 * This function returns the pixel value in LCD (icon) display memory.
 *
 * \param  pix_com  Pixel coordinate - COMx - of the pixel (icon).
 * \param  pix_seg  Pixel coordinate - SEGy - of the pixel (icon).
 */
bool lcd_get_pixel(uint8_t pix_com, uint8_t pix_seg)
{
	uint8_t pixels = *lcd_get_pixel_register(pix_com, pix_seg);

	return  pixels & (1 << (pix_seg % 8));
}

/**
 * @}
 */
