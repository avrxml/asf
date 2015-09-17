/*****************************************************************************
 *
 * \file
 *
 * \brief _ET024006DHU_H_ Header file for ET024006DHU TFT display driver.
 *
 * \note The values defined in this file are device-specific. See the device
 *       datasheet for further information.
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef _ET024006DHU_H_
#define _ET024006DHU_H_

/**
 * \defgroup group_avr32_components_display_et024006dhu DISPLAY - LCD Display - ET024006DHU
 *
 * The ET024006DHU is a RGB LCD display from EDT and it offers a resolution of 240x320 pixels.
 * The driver uses the display in a 16-bit mode however each pixel has a resolution of 18 bits
 * (6 bits for each color). In 16-bit mode red and blue have 5-bit while green has 6-bit resolution.
 * The display offers a parallel and a serial interface currently only the parallel mode is implemented in the driver.
 *
 * \{
 */

/*============================ INCLUDES ======================================*/
#include <stdint.h>
#include <stdbool.h>
//#include "tft_driver_config.h"



/*============================ DEFINITIONS ===================================*/
#define ET024006_WIDTH  320
#define ET024006_HEIGHT 240

#define ET024006_IFACE_MODE_EBI   0
#define ET024006_IFACE_MODE_SPI   1

#define BLACK 0x0000
#define BLUE  0x001F
#define RED   0xF800
#define GREEN 0x07E0
#define WHITE 0xFFFF

#define BLUE_LEV( level)  (   (level)&BLUE )                                      // level is in [0; 31]
#define GREEN_LEV(level)  ( (((level)*2)<<5)&GREEN )                              // level is in [0; 31]
#define RED_LEV(  level)  (  ((level)<<(5+6))&RED )                               // level is in [0; 31]
#define GRAY_LEV( level)  ( BLUE_LEV(level) | GREEN_LEV(level) | RED_LEV(level) ) // level is in [0; 31]

/*============================ TYPES =========================================*/

//! Datatype color information. Use this for portability between displays.
//! bits: 15 14 13 12 11 10 9  8  7  6  5  4  3  2  1  0
//!       R  R  R  R  R  G  G  G  G  G  G  B  B  B  B  B
typedef uint16_t et024006_color_t;



/*============================ PROTOTYPES ====================================*/
#ifdef __cplusplus
extern "C" {
#endif

extern const unsigned char FONT6x8[97][8];
extern const unsigned char FONT8x8[97][8];
extern const unsigned char FONT8x16[97][16];

void et024006_PrintConsole(char *lcd_string, uint16_t fcolor, int bcolor);

void et024006_PrintString(char *lcd_string, const unsigned char *font_style,
                          uint16_t x,
                          uint16_t y,
                          uint16_t fcolor,
                          int bcolor);

void et024006_DuplicatePixel( et024006_color_t color, uint32_t count );


void et024006_Init( unsigned long cpu_hz, unsigned long hsb_hz );

/*! \brief Tests if a valid display is attached.
 *  For EBI connection it tests also the data lines.
 *  @returns 0 for a successful test. -1 if test failed.
 */
int et024006_TestDisplay( void);

/*! \brief Configures a scroll area on the display.
 *
 *  @param tfa Top fixed area in rows. This is an area that is unaffected
 *  from the scrolling and that starts at the top of the display area.
 *  @param vsa Scroll area in rows. This is the actual scroll area.
 *  @param bfa Bottom fixed area in rows. This is a section at the bottom
 *  of the display area that will not be used for scrolling.
 *
 */
void et024006_ScrollAreaSetup(uint16_t tfa, uint16_t vsa, uint16_t bfa);

/*! \brief Scroll the scroll area down/up some lines.
 *  @param lines Number of lines the scroll are should be scrolled.
 *  A positive lines value means lines down. A negative value means lines
 *  up.
 */
void et024006_Scroll(int16_t lines);

/*! \brief Enables the scrolling feature.
 */
void et024006_ScrollEnable(void);

/*! \brief Disables the scrolling.
 */
void et024006_ScrollDisable(void);

et024006_color_t et024006_Color( uint8_t r, uint8_t g, uint8_t b );

void et024006_DrawPixel( uint16_t x, uint16_t y, et024006_color_t color );

void et024006_DrawQuickPixel( uint16_t x, uint16_t y, et024006_color_t color );

et024006_color_t et024006_GetPixel( uint16_t x, uint16_t y );

void et024006_DrawHorizLine( uint16_t x, uint16_t y, uint16_t length, et024006_color_t color );

void et024006_DrawVertLine( uint16_t x, uint16_t y, uint16_t length, et024006_color_t color );

void et024006_DrawLine( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, et024006_color_t color );

void et024006_DrawFilledRect( uint16_t x, uint16_t y, uint16_t width, uint16_t height, et024006_color_t color );

void et024006_SetLimits( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2 );

void et024006_SetQuickLimits( uint16_t x, uint16_t y );
void et024006_SetQuickLimits2( uint16_t x, uint16_t y );

void et024006_CopyPixelsToScreen( et024006_color_t const * pixels, uint32_t count );
void et024006_CopyPixelsFromScreen( et024006_color_t * pixels, uint32_t count );

/*!
 * This function draws a filled circle, or parts of it.
 * The radius is the distance from the center to the arc itself, which
 * means that the total width or height of a circle will be (radius*2+1).
 * The quadrantMask parameter is a bitmask that decides which quadrants of the
 * circle to draw. Use the QUADRANT, HALF and WHOLE constants from the
 * header file and OR them together if required.
 * Radius equal to zero gives a single pixel.
 *
 * \param  x             X of center.
 * \param  y             Y of center.
 * \param  radius        Circle radius.
 * \param  color         Circle color.
 * \param  quadrantMask  Bitmask deciding which quadrants to draw.
 */
void et024006_DrawFilledCircle(
	uint16_t x, uint16_t y,
	uint16_t radius,
	uint16_t color,
	uint8_t quadrantMask );

/*!
 * Compute the actual width of the bit map.
 * The actual width of a bit map is aligned on 8 since 1 bit corresponds to 1
 * pixel.
 */
#define ET024006_BITMAP_WIDTH(width)   Align_up(width/8, 8)

/*!
 * This function print a buffer filled with bits which corresponds to a pixel on
 * the screen. A 1 will be displayed with the foreground_color color, while a 0
 * with a background_color.
 *
 * \param bitmap The buffer which contains the bits.
 * \param map_width The width of the map.
 * \param x The horizontal offset where to place the map on the screen.
 * \param y The vertical offset where to place the map on the screen.
 * \param width The width of the area to print on the screen.
 * \param height The height of the area to print on the screen.
 * \param foreground_color The color on the screen of a bit which value equals
 *                         to 1 on the map.
 * \param background_color The color on the screen of a bit which value equals
 *                         to 0 on the map.
 */
void et024006_PutBitmap(
  void *bitmap,
  uint16_t map_width,
  uint16_t x,
  uint16_t y,
  uint16_t width,
  uint16_t height,
  et024006_color_t foreground_color,
  et024006_color_t background_color
);

void et024006_PutPixmap(
  et024006_color_t const * pixmap,
  uint16_t map_width,
  uint16_t map_x,
  uint16_t map_y,
  uint16_t x,
  uint16_t y,
  uint16_t width,
  uint16_t height
);

void et024006_GetPixmap(
  et024006_color_t * pixmap,
  uint16_t map_width,
  uint16_t map_x,
  uint16_t map_y,
  uint16_t x,
  uint16_t y,
  uint16_t width,
  uint16_t height
);
void et024006_DrawBitmap( const uint16_t data[], uint16_t columnOffset, uint16_t rowOffset );

/*! \brief Make display enter in an idle mode
 */
void et024006_enter_idle(void);

/*! \brief Make display leave the idle mode
 */
void et024006_leave_idle(void);
#ifdef __cplusplus
} /* extern "C" */
#endif

/**
 * \}
 */

#endif // _ET024006DHU_H_
/* EOF */
