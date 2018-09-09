/* This source file is part of the ATMEL QTouch Library 5.0.5 */

/*****************************************************************************
 *
 * \file
 *
 * \brief  This file contains the SAMD QTouch library sample user application.
 *
 *
 * - Userguide:          QTouch Library Peripheral Touch Controller User Guide.
 * - Support email:      www.atmel.com/design-support/
 *
 *
 * Copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
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


/**
 * \page asfdoc_saml22_tslcd Demo Example
 *
 * \section Purpose
 *
 * The application demonstrates how to use Touch SLCD.
 *
 * \section Requirements
 *
 * This package can be used with:
 * - SAML22 Xplained Pro B board.
 *
 * \section Description
 * There are 5 PTC buttons on the SLCD, when press the button:
 * - Button up, increase the digital number of selected.
 * - Button down, decrease the digital number of selected.
 * - Button left, shift the decimal point to left direction to select a digital number.
 * - Button right, shift the decimal point to right direction to select a digital number.
 * - Button enter, blink or not blink icons.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC.
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.microchip.com/">Microchip</A>.\n
 * Support and FAQ: https://www.microchip.com/support/
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

/**
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
#include <asf.h>

/**
 * RTC Interrupt timing definition
 */
#define TIME_PERIOD_1MSEC 1u

/**
 * Variables
 */
volatile uint16_t touch_time_counter = 0u;
struct rtc_module rtc_instance;

volatile uint8_t slcd_digital[5];
volatile uint8_t digital_index;

//! [slcd_data]
/* Character map 0-9, A-Z */
const uint32_t character_map[] = {
	0xE85C, 0x2008, 0xA314, 0xA30C, 0x6308, 0xC30C, 0xC31C, 0xA008, 0xE31C, 0xE30C, /* 0-9 */
	0xE318, 0xA68C, 0xC014, 0xA48C, 0xC314, 0xC310, 0xC21C, /* A-G */
	0x6318, 0x8484, 0x201C, 0x4930, 0x4014, 0x7818, 0x7038, /* H-N */
	0xE01C, 0xE310, 0xE03C, 0xE330, 0xC30C, 0x8480, /* O-T */
	0x601C, 0x4850, 0x6078, 0x1860, 0x1880, 0x8844  /* U-Z */
};

/* Digital map 0-9 */
const uint32_t digital_map[] = {
	0x77, 0x24, 0x5d, 0x6d, 0x2e, 0x6b, 0x7b, 0x25, 0x7f, 0x6f
};

/**
 * Prototypes
 */

/*! \brief Configure Port pins
 *
 */
void configure_port_pins(void);

/*! \brief Initialize timer
 *
 */
void timer_init( void );

/*! \brief RTC timer overflow callback
 *
 */
void rtc_overflow_callback(void);

/*! \brief Configure the RTC timer callback
 *
 */
void configure_rtc_callbacks(void);

/*! \brief Configure the RTC timer count after which interrupts comes
 *
 */
void configure_rtc_count(void);

/*! \brief Configure the SLCD
 *
 */
void slcd_configure(void);

/**
 * Port pin configuration / assignments
 */
void configure_port_pins(void)
{
	struct port_config config_port_pin;
	port_get_config_defaults(&config_port_pin);

	config_port_pin.direction = PORT_PIN_DIR_OUTPUT;

 }

/*! \brief Configure the RTC timer overflow callback
 *
 */

void rtc_overflow_callback(void)
{
	/* Do something on RTC overflow here */
	if (touch_time_counter == touch_time.measurement_period_ms) {
		touch_time.time_to_measure_touch = 1u;
		touch_time.current_time_ms = touch_time.current_time_ms +
				touch_time.measurement_period_ms;
		touch_time_counter = 0u;
	} else {
		touch_time_counter++;
	}
}

/*! \brief Configure the RTC timer callback
 *
 */
void configure_rtc_callbacks(void)
{
	/* register callback */
	rtc_count_register_callback(&rtc_instance,
			rtc_overflow_callback, RTC_COUNT_CALLBACK_OVERFLOW);
	/* Enable callback */
	rtc_count_enable_callback(&rtc_instance, RTC_COUNT_CALLBACK_OVERFLOW);
}

/*! \brief Configure the RTC timer count after which interrupts comes
 *
 */
void configure_rtc_count(void)
{
	struct rtc_count_config config_rtc_count;
	rtc_count_get_config_defaults(&config_rtc_count);

	config_rtc_count.prescaler           = RTC_MODE0_CTRLA_PRESCALER_DIV1;
	config_rtc_count.mode                = RTC_COUNT_MODE_16BIT;
#ifdef FEATURE_RTC_CONTINUOUSLY_UPDATED
	config_rtc_count.continuously_update = true;
#endif
	/* initialize rtc */
	rtc_count_init(&rtc_instance, RTC, &config_rtc_count);

	/* enable rtc */
	rtc_count_enable(&rtc_instance);
}

/*! \brief Initialize timer
 *
 */
void timer_init(void)
{
	/* Configure and enable RTC */
	configure_rtc_count();

	/* Configure and enable callback */
	configure_rtc_callbacks();

	/* Set Timer Period */
	rtc_count_set_period(&rtc_instance, TIME_PERIOD_1MSEC);
}

/*! \brief Initialize slcd
 *
 */
 void slcd_configure(void)
{
	struct slcd_config config;
	char string[] = {'T', 'S', 'L', 'C', 'D'};
	/* Turn on the back light. */
	port_pin_set_output_level(SLCD_BACLKLIGHT,true);
	
	slcd_get_config_defaults(&config);
	config.enable_bias_buffer = true;
	slcd_init(&config);
	slcd_set_contrast(0x9);
	slcd_enable();
	slcd_set_display_memory();
	delay_s(1);
	slcd_clear_display_memory();

	/* Icon display */
	for (uint32_t i = 3; i < 8; i++) {
		slcd_set_pixel(i, 0);
	}

	/* Character display*/
	slcd_character_map_set(SLCD_AUTOMATED_CHAR_START_FROM_BOTTOM_RIGHT, 1);
	for(uint32_t i = 0; i < 5; i++) {
		slcd_character_write_data(0, 8 + i*2, character_map[string[i]-'A'+10], 0xffff0003);
	}
	
	/* Digital display*/
	slcd_character_map_set(SLCD_AUTOMATED_CHAR_START_FROM_BOTTOM_RIGHT, 0);
	slcd_character_write_data(0, 1, digital_map[0], 0xffffff80);
	slcd_character_write_data(0, 4, digital_map[0], 0xffffff80);
	slcd_character_write_data(0, 5, digital_map[0], 0xffffff80);
	slcd_character_write_data(0, 6, digital_map[0], 0xffffff80);
	slcd_character_write_data(0, 7, digital_map[0], 0xffffff80);
	
	/* Blink Display */
	struct slcd_blink_config blink_config;
	slcd_set_pixel(0, 0);
	slcd_set_pixel(1, 0);
	slcd_set_pixel(2, 0);
	slcd_disable();	
	slcd_blink_get_config_defaults(&blink_config);
	blink_config.blink_all_seg = false;
	slcd_blink_set_config(&blink_config);
	slcd_set_blink_pixel(0,0);
	slcd_set_blink_pixel(1,0);
	slcd_set_blink_pixel(2,0);
	slcd_set_frame_counter(SLCD_FRAME_COUNTER_0,true,0x1f);
	slcd_enable_frame_counter(SLCD_FRAME_COUNTER_0);
	slcd_enable_blink();
	slcd_enable();
}
/*! \brief Main function
 *
 */
int main(void)
{
	uint8_t button_up;
	uint8_t button_down;
	uint8_t button_left;
	uint8_t button_right;
	uint8_t button_enter;
	bool    blink = true;
		
	/**
	 * Initialize and configure system and generic clocks.
	 * Use conf_clocks.h to configure system and generic clocks.
	 */
	system_init();
	delay_init();
	/**
	 * Initialize timer.
	 */
	timer_init();

	/**
	 * Initialize QTouch library and configure touch sensors.
	 */
	touch_sensors_init();
	
	/**
	 * Initialize SLCD.
	 */
	slcd_configure();
	/**
	 * Configure port pins
	 */
	configure_port_pins();

	while (1) {
		/**
		 * Start touch sensor measurement, if
		 *touch_time.time_to_measure_touch flag is set by timer.
		 */
		touch_sensors_measure();

		if ((p_mutlcap_measure_data->measurement_done_touch == 1u)) {
			p_mutlcap_measure_data->measurement_done_touch = 0u;
			
			button_up    = GET_MUTLCAP_SENSOR_STATE(0);
			button_down  = GET_MUTLCAP_SENSOR_STATE(1);
			button_left  = GET_MUTLCAP_SENSOR_STATE(2);
			button_right = GET_MUTLCAP_SENSOR_STATE(3);
			button_enter = GET_MUTLCAP_SENSOR_STATE(4); 
			if (button_up) {
				if (slcd_digital[digital_index] >= 9) {
					slcd_digital[digital_index] = 0;
				} else {
					slcd_digital[digital_index]++;
				}
			}
			
			if (button_down) {
				if (slcd_digital[digital_index] <= 0) {
					slcd_digital[digital_index] = 9;
				} else {
					slcd_digital[digital_index]--;
				}
			}
			
			if (button_left) {
				if (digital_index <= 0) {
					digital_index = 4;
				} else {
					digital_index--;
				}
			}
			
			if (button_right) {
				if (digital_index >= 4) {
					digital_index = 0;
					} else {
					digital_index++;
				}
			}
			
			if (button_enter) {
				if (blink) {
					blink = false;
					slcd_disable_blink();
				} else {
					blink = true;
					slcd_enable_blink();
				}
			}
			slcd_character_map_set(SLCD_AUTOMATED_CHAR_START_FROM_BOTTOM_RIGHT, 0);
			slcd_character_write_data(0, 1, digital_map[slcd_digital[0]], 0xffffff00);
			slcd_character_write_data(0, 4, digital_map[slcd_digital[1]], 0xffffff00);
			slcd_character_write_data(0, 5, digital_map[slcd_digital[2]], 0xffffff00);
			slcd_character_write_data(0, 6, digital_map[slcd_digital[3]], 0xffffff00);
			slcd_character_write_data(0, 7, digital_map[slcd_digital[4]], 0xffffff00);
			if (digital_index == 0) {
				slcd_character_write_data(0, 1, digital_map[slcd_digital[0]] | 0x80, 0xffffff00);
			} else {
				slcd_character_write_data(0, 3+digital_index, digital_map[slcd_digital[digital_index]] | 0x80, 0xffffff00);
			}
		}
			
	} /* while(1) */
} /* main */


