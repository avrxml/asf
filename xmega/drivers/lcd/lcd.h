/**
 * \file
 *
 * \brief AVR XMEGA Liquid Crystal Display driver.
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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

#ifndef _LCD_H_
#define _LCD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "compiler.h"
#include "conf_board.h"
#include <sleepmgr.h>

/**
 * \defgroup lcd_group Liquid Crystal Display (LCD)
 *
 * See \ref lcd_quickstart.
 *
 * This is a driver for configuring, enabling/disabling and use of the on-chip
 * LCD controller.
 *
 * \section dependencies Dependencies
 *
 * The LCD module depends on the following modules:
 * - \ref sysclk_group for LCD clock control.
 * - \ref interrupt_group for enabling or disabling interrupts.
 * - \ref sleepmgr_group to unlock LCD controller
 * @{
 */


/**
 * \brief Interrupt event callback function type
 *
 * The interrupt handler can be configured to do a function callback,
 * the callback function must match the lcd_callback_t type.
 *
 */
typedef void (*lcd_callback_t) (void);


//! \name LCD addressing limits
//@{
//! Maximum number of common lines.
#define LCD_MAX_NR_OF_COM  4
//! Maximum number of segment lines.
#if XMEGA_B1
# define LCD_MAX_NBR_OF_SEG  40
#elif XMEGA_B3
# define LCD_MAX_NBR_OF_SEG  25
#else
# error Unsupported part family.
#endif

//@}

//! Low Power Waveform for the LCD
enum lcd_lp_waveform {
	//! Low Power Waveform disabled
	LCD_LP_WAVE_DISABLE_gc = (0x00 << LCD_LPWAV_bp),
	//! Low Power Waveform enabled
	LCD_LP_WAVE_ENABLE_gc  = (0x01 << LCD_LPWAV_bp)
};

//! LCD Interrupt level
enum lcd_int_level {
	//! Interrupt disabled
	LCD_INTLVL_OFF_gc   = (0x00 << LCD_FCINTLVL_gp),
	//! Low level
	LCD_INTLVL_LO_gc    = (0x01 << LCD_FCINTLVL_gp),
	//! Medium level
	LCD_INTLVL_MED_gc   = (0x02 << LCD_FCINTLVL_gp),
	//! High level
	LCD_INTLVL_HI_gc    = (0x03 << LCD_FCINTLVL_gp),
#ifdef CONFIG_XMEGA_128B1_REVA
	//! Extended interrupt mode, interrupt disabled
	LCD_X_INTLVL_OFF_gc = (1 << LCD_XIME0_bp) | (0x00 << LCD_FCINTLVL_gp),
	//! Extended interrupt mode, low level
	LCD_X_INTLVL_LO_gc  = (1 << LCD_XIME0_bp) | (0x01 << LCD_FCINTLVL_gp),
	//! Extended interrupt mode, medium level
	LCD_X_INTLVL_MED_gc = (1 << LCD_XIME0_bp) | (0x02 << LCD_FCINTLVL_gp),
	//! Extended interrupt mode, high level
	LCD_X_INTLVL_HI_gc  = (1 << LCD_XIME0_bp) | (0x03 << LCD_FCINTLVL_gp),
#endif
};


/**
 * \brief LCD clock initialization.
 *
 * The LCD power reduction bit is cleared.
 *
 * \note The RTC clock source used by the LCD module should be set up before
 *       calling this function. 
 */
static inline void lcd_clk_init(void)
{
	// Enable sys clock for LCD Module
	PR.PRGEN &= ~PR_LCD_bm;
}


/**
 * \brief LCD glass connection initialization.
 *
 * This function sets the LCD glass connection.
 *
 * The function enables COM & SEG swapping, LCD port mask and the using (or no)
 * of an External Resistor Bias Generation. This function also clears
 * all the display memory (=> all pixels "off") and lets the LCD disabled.
 *
 * \param  com_swp   COM swapping (false: no swapping, true: COM swapping).
 * \param  seg_swp   SEG swapping (false: no swapping, true: SEG swapping).
 * \param  port_mask Number of SEG used.
 * \param  x_bias    External bias (false: internal gen., true: external gen.).
 */
static inline void lcd_connection_init(bool com_swp, bool seg_swp,
		uint8_t port_mask, bool x_bias)
{
	LCD.CTRLA = (com_swp ? LCD_COMSWP_bm : 0)
			| (seg_swp ? LCD_SEGSWP_bm : 0)
			| (x_bias ? LCD_XBIAS_bm : 0) | LCD_CLRDT_bm;
	LCD.CTRLC = (port_mask << LCD_PMSK_gp) & LCD_PMSK_gm;
}


/**
 * \brief LCD timing initialization.
 *
 * This function enables the LCD timing.
 *
 * The function sets the prescaler, the divider and the duty cycle of the LCD.
 *
 * \param  lcd_pres    prescaler of the clock source.
 * \param  lcd_clkdiv  Divider of the prescaled clock source.
 * \param  lcd_lp_wave type of waveform, low power or default waveform.
 * \param  lcd_duty    duty cycle (operating mode) applied to the LCD.
 *
 * Timing setup examples with input clock rate 32.768 kHz, for different LCD
 * operating frequencies:
 * - 256 Hz:
 * \code
	LCD.CTRB = LCD_PRESC_16_gc | LCD_CLKDIV_DivBy1_gc | LCD_DUTY_1_4_gc
\endcode
 * - 128 Hz:
 * \code
	LCD.CTRB = LCD_PRESC_16_gc | LCD_CLKDIV_DivBy2_gc | LCD_DUTY_1_4_gc
\endcode
 * - 85.3 Hz:
 * \code
	LCD.CTRB = LCD_PRESC_16_gc | LCD_CLKDIV_DivBy3_gc | LCD_DUTY_1_4_gc
\endcode
 * - 64 Hz:
 * \code
	LCD.CTRB = LCD_PRESC_16_gc | LCD_CLKDIV_DivBy4_gc | LCD_DUTY_1_4_gc
\endcode
 * - 51.2 Hz:
 * \code
	LCD.CTRB = LCD_PRESC_16_gc | LCD_CLKDIV_DivBy5_gc | LCD_DUTY_1_4_gc
\endcode
 * - 42.7 Hz:
 * \code
	LCD.CTRB = LCD_PRESC_16_gc | LCD_CLKDIV_DivBy6_gc | LCD_DUTY_1_4_gc
\endcode
 * - 36.6 Hz:
 * \code
	LCD.CTRB = LCD_PRESC_16_gc | LCD_CLKDIV_DivBy7_gc | LCD_DUTY_1_4_gc
\endcode
 * - 32 Hz:
 * \code
	LCD.CTRB = LCD_PRESC_16_gc | LCD_CLKDIV_DivBy8_gc | LCD_DUTY_1_4_gc
\endcode
 */
static inline void lcd_timing_init(enum LCD_PRESC_enum lcd_pres,
		enum LCD_CLKDIV_enum lcd_clkdiv,
		enum lcd_lp_waveform lcd_lp_wave,
		enum LCD_DUTY_enum lcd_duty)
{
	LCD.CTRLB = ((uint8_t)lcd_pres) | ((uint8_t)lcd_clkdiv)
			| ((uint8_t)lcd_lp_wave) | ((uint8_t)lcd_duty);
}


/*! \brief LCD interrupt initialization.
 *
 *  This function sets the interrupt behaviour.
 *
 *  The function defines the interrupt level and sets the number of frames
 *  between 2 interrupts. Note that this function takes care of the waveform
 *  type (default or low power waveform) so 'lcd_timing_init()' function must
 *  be called before.
 *
 *  \param  int_level    LCD module interrupt level.
 *  \param  frame_count  Number of frames between 2 interrupts.
 *                       (2 <= frame_count <= 64 for low power waveform)
 *                       (1 <= frame_count <= 32 for default waveform)
 */
static inline void lcd_interrupt_init(enum lcd_int_level int_level,
		uint8_t frame_count)
{
	LCD.INTCTRL = (int_level << LCD_FCINTLVL0_bp);

#ifndef CONFIG_XMEGA_128B1_REVA  // XMEGA_B rev. higher than revA
	if (LCD.CTRLB & LCD_LPWAV_bm) {
		frame_count >>= 1;
	}
	if (frame_count != 0) {
		frame_count -= 1;
	}
	if (frame_count > 31) {
		frame_count = 31;
	}
	LCD.INTCTRL |= (frame_count << LCD_XIME0_bp);
#endif
}


/**
 * \brief LCD blinking initialization.
 *
 * This function sets the frequency of the hardware display blinking.
 *
 * The function defines the blinking rate. In the same time, the hardware
 * display blinking is disabled.
 *
 * \param  lcd_blink_rate  Hardware display blinking frequency.
 */
static inline void lcd_blinkrate_init(enum LCD_BLINKRATE_enum lcd_blink_rate)
{
	LCD.CTRLD = (uint8_t)((LCD.CTRLD & ~LCD_BLINKRATE_gm) | lcd_blink_rate);
}


/**
 * \brief Set the LCD contrast.
 *
 * This function finely sets the LCD contrast.
 *
 * Transfer function: VLCD = 3.0 V + (fcont[5:0] * 0.016 V)
 *
 * \param  fcont  -32 <= signed contrast value <= 31.
 */
static inline void lcd_set_contrast(int8_t fcont)
{
	if (fcont > ((int8_t)(LCD_FCONT_gm >> 1))) {
		LCD.CTRLF = LCD_FCONT_gm >> 1;
	} else if (fcont < ((int8_t)(((~LCD_FCONT_gm) >> 1) | 0x80))) {
		LCD.CTRLF = ((~LCD_FCONT_gm) >> 1) | 0x80;
	} else {
		LCD.CTRLF = fcont;
	}
}


/**
 * \brief Enable LCD.
 *
 * This function enables the LCD module.
 */
static inline void lcd_enable(void)
{
	sleepmgr_lock_mode(SLEEPMGR_PSAVE);
	LCD.CTRLA |= (1 << LCD_ENABLE_bp) | (1 << LCD_SEGON_bp);
}


/**
 * \brief Disable LCD.
 *
 * This function disables the LCD module.
 *
 */
static inline void lcd_disable(void)
{
	sleepmgr_unlock_mode(SLEEPMGR_PSAVE);
	LCD.CTRLA &= ~(1 << LCD_ENABLE_bp);
}


/**
 * \brief Set the blinking of pixels.
 *
 * This function sets the blinking of pixels in LCD module on SEG0 & SEG1.
 * Each bit position corresponds to one pixel.
 *
 * \param  pix_com  Pixel coordinate - COMx - of the pixel (icon).
 * \param  pix_seg  Pixel coordinate - SEGy - of the pixel (icon).
 *
 */
static inline void lcd_set_blink_pixel(uint8_t pix_com, uint8_t pix_seg)
{
	LCD.CTRLE |= 1 << ((LCD_BPS10_bp * pix_seg) + pix_com);
}

/**
 * \brief Clear the blinking of pixels.
 *
 * This function clears the blinking of pixels in LCD module on SEG0 & SEG1.
 * Each bit position corresponds to one pixel.
 *
 * \param  pix_com  Pixel coordinate - COMx - of the pixel (icon).
 * \param  pix_seg  Pixel coordinate - SEGy - of the pixel (icon).
 *
 */
static inline void lcd_clear_blink_pixel(uint8_t pix_com, uint8_t pix_seg)
{
	LCD.CTRLE &= ~(1 << ((LCD_BPS10_bp * pix_seg) + pix_com));
}

/**
 * \brief Clear the blinking of all pixels.
 *
 * This function clears the blinking of all pixels in LCD module on SEG0 & SEG1.
 *
 */
static inline void lcd_clear_blink_all_pixel(void)
{
	LCD.CTRLE = 0;
}

/**
 * \brief LCD blink enable.
 *
 * This function enables the blinking mode in LCD module.
 */
static inline void lcd_blink_enable(void)
{
	// Blinking "on"
	LCD.CTRLD |= 1 << LCD_BLINKEN_bp;
}


/**
 * \brief LCD blink disable.
 *
 * This function disables the blinking mode in LCD module.
 */
static inline void lcd_blink_disable(void)
{
	// Blinking "off"
	LCD.CTRLD &= ~LCD_BLINKEN_bm;
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
void lcd_set_frame_interrupt_callback(lcd_callback_t callback);


/**
 * \brief Send a sequence of ASCII characters to LCD device.
 *
 * This function enables/disables LCD pixels via the digit decoder.
 *
 * \param  lcd_tdg    Type of digit decoder.
 * \param  first_seg  First SEG where the first data will be writen.
 * \param  data       Data buffer.
 * \param  width      Number of data.
 * \param  dir        Direction (==0: Left->Right, !=0: Left<-Right).
 */
void lcd_write_packet(enum LCD_TDG_enum lcd_tdg, uint8_t first_seg,
		const uint8_t *data, size_t width, uint8_t dir);


/*! \brief Wait for n interrupt periods.
 *
 *  This function counts interrupt periods and returns when the count is
 *  reached. This function only runs if LCD interrupt is disabled.
 *
 *  \param  n_int  Number of interrupt periods.
 */
void lcd_wait_n_int(uint8_t n_int);


/**
 * \brief Set pixel (icon) in LCD display memory.
 *
 * This function sets a pixel in LCD (icon) display memory. If a parameter
 * is out of range, then the function doesn't set any bit.
 *
 * \param  pix_com  Pixel coordinate - COMx - of the pixel (icon).
 * \param  pix_seg  Pixel coordinate - SEGy - of the pixel (icon).
 */
void lcd_set_pixel(uint8_t pix_com, uint8_t pix_seg);


/**
 * \brief Clear pixel (icon) in LCD display memory.
 *
 * This function clears a pixel in LCD (icon) display memory. If a parameter
 * is out of range, then the function doesn't clear any bit.
 *
 * \param  pix_com  Pixel coordinate - COMx - of the pixel (icon).
 * \param  pix_seg  Pixel coordinate - SEGy - of the pixel (icon).
 */
void lcd_clear_pixel(uint8_t pix_com, uint8_t pix_seg);


/**
 * \brief Toggle pixel (icon) in LCD display memory.
 *
 * This function toggles a pixel in LCD (icon) display memory. If a parameter
 * is out of range, then the function doesn't toggle any bit.
 *
 * \param  pix_com  Pixel coordinate - COMx - of the pixel (icon).
 * \param  pix_seg  Pixel coordinate - SEGy - of the pixel (icon).
 */
void lcd_tgl_pixel(uint8_t pix_com, uint8_t pix_seg);


/**
 * \brief Get pixel value (icon) in LCD display memory.
 *
 * This function returns the pixel value in LCD (icon) display memory.
 *
 * \param  pix_com  Pixel coordinate - COMx - of the pixel (icon).
 * \param  pix_seg  Pixel coordinate - SEGy - of the pixel (icon).
 */
bool lcd_get_pixel(uint8_t pix_com, uint8_t pix_seg);


/**
 * @}
 */

#ifdef __cplusplus
}
#endif
/**
 * \page lcd_quickstart Quickstart guide for AVR XMEGA LCD driver
 *
 * This is the quickstart guide for the \ref lcd_group "AVR XMEGA LCD driver",
 * with step-by-step instructions on how to configure and use the driver in a
 * selection of use cases.
 *
 * The use cases contain several code fragments. The code fragments in the
 * steps for setup can be copied into a custom initialization function, while
 * the steps for usage can be copied into, e.g., the main application function.
 *
 * \section lcd_basic_use_case Basic use case
 * In the most basic use case, prior to light and clear segments,
 * the LCD module needs to be configured, meaning:
 * - set up a proper clock
 * - initialize connexion with the LCD target glass
 * - configure blink rate
 *
 * Once the LCD is initialized, the module can be used to:
 * - light on specific pixel in the glass
 * - blink the entire glass or specific pixel connected to SEG0, SEG1 lines
 * - display ASCII character on dedicated 7, 14 or 16 segments digits.
 *
 * \section lcd_basic_use_case_setup Setup steps
 * \subsection lcd_basic_use_case_prereq Prerequisites
 * The sysclk service (\ref sysclk_group) is required for RTC clock source setup. Make
 * sure this is a part of the main initialization code:
 * \code
	sysclk_init();
\endcode
 *
 * \subsection lcd_basic_use_case_setup_code Example code
 * The RTC clock source which is also used for LCD needs to be configured in
 * conf_clock.h :
 * \code
	#define CONFIG_RTC_SOURCE          SYSCLK_RTCSRC_ULP
\endcode
 * Add to application C-file:
 * \code
	   void lcd_init(void)
	   {
	        lcd_clk_init();

	        // Glass connection no COM swap, no SEG swap, use 40 seg line,
	        no external bias lcd_connection_init(false, false, 40, false);

	        // LCD waveform timing configuration:
	        // - Divide LCD source clock module (ULP) by 16
	        // - 4 divider of the prescaled clock source
	        // - Configure LCD module to generate low power mode
	        // - Duty cycle (operating mode) applied to the LCD.
	        lcd_timing_init(LCD_PRESC_16_gc, LCD_CLKDIV_DivBy4_gc,
	                        LCD_LP_WAVE_ENABLE_gc, 4);

	        // Configure blink rate
	        lcd_blinkrate_init(LCD_BLINKRATE_2Hz_gc);
	        lcd_enable();
	   }
\endcode
 *
 * \subsection lcd_basic_use_case_setup_flow Workflow
 * -# Make sure sysclk_init() is part of the main initalization code:
 *   - \code sysclk_init(); \endcode
 * -# Configure RTC clock source by adding this to conf_clock.h :
 *   - \code #define CONFIG_RTC_SOURCE          SYSCLK_RTCSRC_ULP \endcode
 *   - \note Using internal 32kHz ULP Oscillator allows the LCD module to
 *           refresh the glass while CPU core is halted and main oscillator
 *           stopped (power save mode).
 * -# Select LCD module Oscillator source clock.
 *   - \code lcd_clk_init(); \endcode
 * -# Define connection type between LCD module and LCD glass using
 * function:
 *   - \code lcd_connection_init(false, false, 40, false); \endcode
 *   - \note This allows to define if the COM or SEG line are swapped
 *   (PCB layout constraints)
 * , the number of SEG line used and if and external bias source is used.
 * -# Define the LCD waveform timing generation using function:
 *   - \code lcd_timing_init(LCD_PRESC_16_gc, LCD_CLKDIV_DivBy4_gc,
	                        LCD_LP_WAVE_ENABLE_gc, 4); \endcode
 *   - \note Enabling Low Power Waveform type is recommended to save
 *   power consumption.
 * refresh the glass while CPU core halted and main oscillator stopped
 * (power save mode).
 *   - \note The LCD duty to be used is specific to the glass type.
 * -# Configure blink rate frequency:
 *   - \code lcd_blinkrate_init(LCD_BLINKRATE_2Hz_gc); \endcode
 * -# Enable LCD module:
 *   - \code lcd_enable(); \endcode
 *
 * \section lcd_llllbasic_use_case_usage Usage steps
 * \subsection lcd_basic_use_case_usage_code Example code
 * Add to, e.g., main loop in application C-file:
 * \code
	    const uint8_t alpha_text[]="HELLO AVR";
	    const uint8_t num_text[]="1234";

	    lcd_set_pixel(1,0);

	    lcd_set_blink_pixel(2,0);

	    lcd_write_packet(LCD_TDG_14S_4C_gc, FIRST_14SEG_4C, alpha_text, \
	                    WIDTH_14SEG_4C, DIR_14SEG_4C);

	    lcd_write_packet(LCD_TDG_7S_4C_gc, FIRST_7SEG_4C, num_text, \
	                    WIDTH_7SEG_4C, DIR_7SEG_4C);

	    uint16_t result = adc_get_result(&ADCA, ADC_CH0);
\endcode
 *
 * \subsection lcd_basic_use_case_usage_flow Workflow
 * -# Declare some alphanumeric or numeric string to be displayed:
 *   - \code const uint8_t alpha_text[]="HELLO AVR";
	 const uint8_t num_text[]="1234"; \endcode
 * -# Turn on one individual pixel. A pixel is tupple of (COM,SEG) positions:
 *   - \code lcd_set_pixel(1,0); \endcode
 *   - \note The (1,0) correspond at ICON_USB on the specific glass
 *   c42048a used in ASF.
 * -# Blink one individual pixel:
 *   - \code lcd_set_blink_pixel(2,0); \endcode
 *   - \note The (2,0) correspond at ICON_AVR on the specific glass
 *   c42048a used in ASF.
 * -# Display alphanumeric string on 14 segment per digit line:
 *   - \code lcd_write_packet(LCD_TDG_14S_4C_gc, FIRST_14SEG_4C, alpha_text, \
	                    WIDTH_14SEG_4C, DIR_14SEG_4C); \endcode
 * -# Display numeric string on 7 segment per digit line:
 *   - \code lcd_write_packet(LCD_TDG_7S_4C_gc, FIRST_7SEG_4C, num_text, \
	                    WIDTH_7SEG_4C, DIR_7SEG_4C); \endcode
 * \section lcd_use_cases Advanced use cases
 * For more advanced use of the LCD driver, see the following use cases:
 * - \subpage lcd_use_case_1
 *
 */
/**
 * \page lcd_use_case_1  Defining LCD glass component
 * In this use case the LCD module will be used through a glass component
 * definition.
 * Defining an LCD glass component allows to define and use an LCD api that
 * matches the glass characteristics and features.
 *
 * The glass definition will contain:
 * - Glass Charateristics: Number of COM and SEG lines used,
 * digit characteristics (7, 14 or 16 segments per digit)...
 * - COM,SEG coordinates of each individual pixel on the glass
 * - Global initialization function that will initialize the LCD controller
 * to fit the LCD glass requirements
 * - Function to write alphanumeric or numeric string to the appropriate digit
 * type available on the glass.
 * - Any other functions type useful to access the specific glass feature.
 *
 * \note ASF contains c42048a.h and c42048a.c files that can be used as
 * reference to build other custom glass component file.
 *
 * \section lcd_use_case_1_setup Setup steps
 * \subsection lcd_use_case_1_setup_code Example code
 * See content of c42048a.h for LCD glass characteristics definitions.
 *
 * The c42048a.c file contains the following initialization function:
 * \code
	void c42048a_init(void)
	{
	lcd_clk_init();
	lcd_connection_init(COM_SWP, SEG_SWP, PORT_MASK, X_BIAS);
	lcd_timing_init(LCD_PRESC_16_gc, LCD_CLKDIV_DivBy4_gc,
	                 LCD_LP_WAVE_ENABLE_gc, LCD_DUTY);
	lcd_interrupt_init(LCD_INTLVL_OFF_gc, 16);
	lcd_blinkrate_init(LCD_BLINKRATE_2Hz_gc);
	lcd_enable();
	} \endcode
 *
 * Adding a call to this c42048a_init glass initialization will performs
 * LCD module initialization according to the glass specific characteristics.
 *
 * c42048a.c file also contains high level abstraction functions to easily
 * access specific glass fields like:
 * \code void c42048a_set_numeric_dec(uint16_t val)
	 {
	 lcd_write_packet(...);
	 }

	 void c42048a_set_text(const uint8_t *data)
	 {
	 lcd_write_packet(...);
	 }

	 void c42048a_set_pixel(uint8_t pix_com, uint8_t pix_seg)
	 {
	 lcd_set_pixel(pix_com,pix_seg);
	 } \endcode
 *
 * \section lcd_use_case_1_usage Usage steps
 * \subsection lcd_use_case_1_usage_code Example code
 * The following code illustrates how to initialize and use an LCD defined glass
 * thought its component abstraction layer:
 * \code
	c42048a_init();
	c42048a_set_numeric_dec(1245);
	c42048a_set_text((const uint8_t)"HELLO AVR");
	c42048a_set_pixel(2,0);
\endcode
 *
 * \subsection adc_use_case_1_usage_flow Workflow
 * -# Initialize the LCD glass component through its high level initilization
 * function. This will initialize the LCD module driver automatically
 *   - \code c42048a_init(); \endcode
 * -# Use the component layer function to update specific available fields on
 * the glass (numeric values, text fields or specific pixels...).
 *   - \code c42048a_set_numeric_dec(1245);
	c42048a_set_text((const uint8_t)"HELLO AVR");
	c42048a_set_pixel(2,0); \endcode
 */
#endif // _LCD_H_
