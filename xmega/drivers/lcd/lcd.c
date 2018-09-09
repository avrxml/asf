/**
 * \file
 *
 * \brief AVR XMEGA Liquid Crystal Display driver.
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
