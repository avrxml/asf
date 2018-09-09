/**
 * \file
 *
 * \brief Management of C42048A LCD Glass component in a reduced form..
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
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include "compiler.h"
#include "c42048a.h"
#include "lcd.h"

void c42048a_init(void)
{
	// LCD_initialization
	lcd_clk_init();
	lcd_connection_init(COM_SWP, SEG_SWP, PORT_MASK, X_BIAS);
	lcd_timing_init(LCD_PRESC_16_gc, LCD_CLKDIV_DivBy4_gc,
			LCD_LP_WAVE_ENABLE_gc, LCD_DUTY);
	lcd_interrupt_init(LCD_INTLVL_OFF_gc, 16);
	lcd_blinkrate_init(LCD_BLINKRATE_2Hz_gc);
	lcd_enable();
}

void c42048a_bar_graph(uint8_t val)
{
	lcd_clear_pixel(ICON_LEVEL_1);
	lcd_clear_pixel(ICON_LEVEL_2);
	lcd_clear_pixel(ICON_LEVEL_3);
	lcd_clear_pixel(ICON_LEVEL_4);
	lcd_clear_pixel(ICON_LEVEL_5);
	lcd_clear_pixel(ICON_LEVEL_6);
	lcd_clear_pixel(ICON_LEVEL_7);
	lcd_clear_pixel(ICON_LEVEL_8);
	lcd_set_pixel(ICON_LEVELBAR);
	if (val>224){
		lcd_set_pixel(ICON_LEVEL_8);
	}
	if (val>192){
		lcd_set_pixel(ICON_LEVEL_7);
	}
	if (val>160){
		lcd_set_pixel(ICON_LEVEL_6);
	}
	if (val>128) {
		lcd_set_pixel(ICON_LEVEL_5);
	}
	if (val>96){
		lcd_set_pixel(ICON_LEVEL_4);
	}
	if (val>64){
		lcd_set_pixel(ICON_LEVEL_3);
	}
	if (val>32){
		lcd_set_pixel(ICON_LEVEL_2);
	}
	if (val>0){
		lcd_set_pixel(ICON_LEVEL_1);
	}
}

/**
 * \brief Conversion of an hexa value in ASCII code
 *
 */
static uint8_t conv_ascii(uint8_t temp)
{
	if(temp<= 0x09) {
		temp = temp + 0x30;
	}
	if((temp >= 0x0A)&&(temp<= 0x0F)) {
		temp = temp + 0x37;
	}
	return(temp);
}

void c42048a_set_numeric_dec(uint16_t val)
{
	uint8_t lcd_num[5];
	uint16_t tmp;

	// Compute each digit char value
	tmp = val / 10000;
	if(tmp) {
		lcd_num[0] = conv_ascii(tmp);
	} else
	lcd_num[0] = ' ';

	val = val - tmp*10000;
	tmp = val / 1000;
	if(tmp) {
		lcd_num[1] = conv_ascii(tmp);
	} else
	lcd_num[1] = ' ';

	val = val - tmp*1000;
	tmp = val / 100;
	if(tmp || lcd_num[1]!=' ') {
		lcd_num[2] = conv_ascii(tmp);
	} else lcd_num[2] = ' ';
	val = val - tmp*100;

	tmp = val / 10;
	if(tmp  || lcd_num[2]!=' ' || lcd_num[1]!=' ') {
		lcd_num[3] = conv_ascii(tmp);
	} else lcd_num[3] = ' ';
	val = val - tmp*10;
	lcd_num[4] = conv_ascii(val);

#ifdef CONFIG_XMEGA_128B1_REVA
	//! Save pixels that characters generator may overwrite
	bool p1=lcd_get_pixel(ICON_DEGREE_C);
	bool p2=lcd_get_pixel(ICON_DEGREE_F);
	bool p3=lcd_get_pixel(ICON_VOLT);
	bool p4=lcd_get_pixel(ICON_MILLI_VOLT);
#endif
	//! Write to character generator
	c42048a_write_num_packet((uint8_t const*)&lcd_num);
#ifdef CONFIG_XMEGA_128B1_REVA
	//! Restore overwritten pixels
	if(p1) lcd_set_pixel(ICON_DEGREE_C);
	if(p2) lcd_set_pixel(ICON_DEGREE_F);
	if(p3) lcd_set_pixel(ICON_VOLT);
	if(p4) lcd_set_pixel(ICON_MILLI_VOLT);
#endif
}

#ifdef CONFIG_XMEGA_128B1_REVA
void c42048a_set_text(const uint8_t *data) {
	bool bar1,bar2,bar3,bar4,bar5,bar6,bar7,bar8,am,pm;

	//! Save pixels that characters generator may overwrite
	bar1=lcd_get_pixel(ICON_LEVEL_1);
	bar2=lcd_get_pixel(ICON_LEVEL_2);
	bar3=lcd_get_pixel(ICON_LEVEL_3);
	bar4=lcd_get_pixel(ICON_LEVEL_4);
	bar5=lcd_get_pixel(ICON_LEVEL_5);
	bar6=lcd_get_pixel(ICON_LEVEL_6);
	bar7=lcd_get_pixel(ICON_LEVEL_7);
	bar8=lcd_get_pixel(ICON_LEVEL_8);
	am=lcd_get_pixel(ICON_AM);
	pm=lcd_get_pixel(ICON_PM);
	//! Write to character generator
	c42048a_write_alpha_packet(data);
	//! Restore overwritten pixels
	if(bar1) lcd_set_pixel(ICON_LEVEL_1);
	if(bar2) lcd_set_pixel(ICON_LEVEL_2);
	if(bar3) lcd_set_pixel(ICON_LEVEL_3);
	if(bar4) lcd_set_pixel(ICON_LEVEL_4);
	if(bar5) lcd_set_pixel(ICON_LEVEL_5);
	if(bar6) lcd_set_pixel(ICON_LEVEL_6);
	if(bar7) lcd_set_pixel(ICON_LEVEL_7);
	if(bar8) lcd_set_pixel(ICON_LEVEL_8);
	if(am) lcd_set_pixel(ICON_AM);
	if(pm) lcd_set_pixel(ICON_PM);
}
#endif
