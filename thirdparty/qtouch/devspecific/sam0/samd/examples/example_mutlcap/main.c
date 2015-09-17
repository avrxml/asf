/**
 * \file
 *
 * \brief  This file contains the SAMD QTouch library sample user application.
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

/**
 * \mainpage SAMD20/D21 QTouch Example
 *
 * \section Purpose
 *
 * This example demonstrates how to use QTouch library on SAMD20/D21 devices.
 *
 * \section Requirements
 *
 * This example can be used on SAMD20/D21 Xplained Pro and QT1 Xplained Pro.
 *
 * \section Description
 *
 * The program configures the necessary modules for using QTouch library. After
 * it started, users can turn on and off the LED by touching the button and slider,
 * change the color of RGB LED by touching wheel on QT1 Xplained Pro board.
 */

#include <asf.h>

/* Macros */

/**
 * \def GET_SENSOR_STATE(SENSOR_NUMBER)
 * \brief To get the sensor state that it is in detect or not
 * \param SENSOR_NUMBER for which the state to be detected
 * \return Returns either 0 or 1
 * If the bit value is 0, it is not in detect
 * If the bit value is 1, it is in detect
 * Alternatively, the individual sensor state can be directly accessed using
 * p_qm_measure_data->p_sensor_states[(SENSOR_NUMBER/8)] variable.
 */
 #define GET_MUTLCAP_SENSOR_STATE(SENSOR_NUMBER) p_mutlcap_measure_data-> \
	p_sensor_states[(SENSOR_NUMBER / \
	8)] & (1 << (SENSOR_NUMBER % 8))

/**
 * \def GET_ROTOR_SLIDER_POSITION(ROTOR_SLIDER_NUMBER)
 * \brief To get the rotor angle or slider position.
 * These values are valid only when the sensor state for
 * corresponding rotor or slider shows in detect.
 * \param ROTOR_SLIDER_NUMBER for which the position to be known
 * \return Returns rotor angle or sensor position
 */
#define GET_MUTLCAP_ROTOR_SLIDER_POSITION(ROTOR_SLIDER_NUMBER) \
	p_mutlcap_measure_data->p_rotor_slider_values[ \
		ROTOR_SLIDER_NUMBER]

/**
 * Discrete LED output pin definitions
 */
#ifdef LED_0_PIN
#undef LED_0_PIN
#define LED_0_PIN	PIN_PB00
#endif
#define LED_1_PIN       PIN_PB01
#define LED_2_PIN       PIN_PB02
#define LED_3_PIN       PIN_PB03
#define LED_4_PIN       PIN_PA05
#define LED_5_PIN       PIN_PA06
#define LED_6_PIN       PIN_PA04
#define LED_7_PIN       PIN_PA07
#if SAMD20
#define LED_8_PIN       PIN_PA22
#define LED_9_PIN       PIN_PA23
#elif SAMD21
#define LED_8_PIN       PIN_PB12
#define LED_9_PIN       PIN_PB13
#endif

/**
 * RGB LED output pin definitions
 */
#define LED_R_PIN       PIN_PB14
#define LED_G_PIN       PIN_PB15
#define LED_B_PIN       PIN_PA17

/**
 * RTC Interrupt timing definition
 */
#define TIME_PERIOD_1MSEC 33u
/**
 * Variables
 */
uint8_t rotor_state;
uint8_t rotor_position;
volatile uint8_t PWM_Count;
volatile uint16_t touch_time_counter = 0u;
struct rtc_module rtc_instance;

const uint8_t PWM_RGB_values[64][3] = {
	{20, 0, 0},
	{20, 0, 0},
	{19, 1, 0},
	{18, 2, 0},
	{17, 3, 0},
	{16, 4, 0},
	{15, 5, 0},
	{14, 6, 0},
	{13, 7, 0},
	{12, 8, 0},
	{11, 9, 0},
	{10, 10, 0},
	{9, 11, 0},
	{8, 12, 0},
	{7, 13, 0},
	{6, 14, 0},
	{5, 15, 0},
	{4, 16, 0},
	{3, 17, 0},
	{2, 18, 0},
	{1, 19, 0},
	{0, 20, 0},
	{0, 20, 0},
	{0, 19, 1},
	{0, 18, 2},
	{0, 17, 3},
	{0, 16, 4},
	{0, 15, 5},
	{0, 14, 6},
	{0, 13, 7},
	{0, 12, 8},
	{0, 11, 9},
	{0, 10, 10},
	{0, 9, 11},
	{0, 8, 12},
	{0, 7, 13},
	{0, 6, 14},
	{0, 5, 15},
	{0, 4, 16},
	{0, 3, 17},
	{0, 2, 18},
	{0, 1, 19},
	{0, 0, 20},
	{0, 0, 20},
	{1, 0, 19},
	{2, 0, 18},
	{3, 0, 17},
	{4, 0, 16},
	{5, 0, 15},
	{6, 0, 14},
	{7, 0, 13},
	{8, 0, 12},
	{9, 0, 11},
	{10, 0, 10},
	{11, 0, 9},
	{12, 0, 8},
	{13, 0, 7},
	{14, 0, 6},
	{15, 0, 5},
	{16, 0, 4},
	{17, 0, 3},
	{18, 0, 2},
	{19, 0, 1},
	{19, 0, 1}
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

/**
 * Port pin configuration / assignments
 */
void configure_port_pins(void)
{
	struct port_config config_port_pin;
	port_get_config_defaults(&config_port_pin);

	config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(LED_0_PIN, &config_port_pin);
	port_pin_set_config(LED_1_PIN, &config_port_pin);
	port_pin_set_config(LED_2_PIN, &config_port_pin);
	port_pin_set_config(LED_3_PIN, &config_port_pin);
	port_pin_set_config(LED_4_PIN, &config_port_pin);
	port_pin_set_config(LED_5_PIN, &config_port_pin);
	port_pin_set_config(LED_6_PIN, &config_port_pin);
	port_pin_set_config(LED_7_PIN, &config_port_pin);
	port_pin_set_config(LED_8_PIN, &config_port_pin);
	port_pin_set_config(LED_9_PIN, &config_port_pin);
	port_pin_set_config(LED_R_PIN, &config_port_pin);
	port_pin_set_config(LED_G_PIN, &config_port_pin);
	port_pin_set_config(LED_B_PIN, &config_port_pin);

}

/*! \brief Configure the RTC timer overflow callback
 *
 */

void rtc_overflow_callback(void)
{
	/* Do something on RTC overflow here */
	if(touch_time_counter == touch_time.measurement_period_ms)
	{
		touch_time.time_to_measure_touch = 1u;
		touch_time.current_time_ms = touch_time.current_time_ms +
		touch_time.measurement_period_ms;
		touch_time_counter = 0u;
	}
	else
	{
		touch_time_counter++;
	}


			// Update PWM counter
			if(PWM_Count < 20)
			{
				PWM_Count++;
			}
			else
			{
				PWM_Count = 0;
			}

			// If rotor is active
			if(rotor_state)
			{
				rotor_position = GET_MUTLCAP_ROTOR_SLIDER_POSITION(0);
				rotor_position = rotor_position >> 2u;
				if(PWM_Count < PWM_RGB_values[rotor_position][0])
				{
					port_pin_set_output_level(LED_R_PIN, 0);
				}
				else
				{
					port_pin_set_output_level(LED_R_PIN, 1);
				}

				if(PWM_Count < PWM_RGB_values[rotor_position][1])
				{
					port_pin_set_output_level(LED_G_PIN, 0);
				}
				else
				{
					port_pin_set_output_level(LED_G_PIN, 1);
				}

				if(PWM_Count < PWM_RGB_values[rotor_position][2])
				{
					port_pin_set_output_level(LED_B_PIN, 0);
				}
				else
				{
					port_pin_set_output_level(LED_B_PIN, 1);
				}
			}
			else
			{
				port_pin_set_output_level(LED_R_PIN, 1);
				port_pin_set_output_level(LED_G_PIN, 1);
				port_pin_set_output_level(LED_B_PIN, 1);
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
	rtc_count_enable_callback(&rtc_instance,RTC_COUNT_CALLBACK_OVERFLOW);
}

/*! \brief Configure the RTC timer count after which interrupts comes
 *
 */
void configure_rtc_count(void)
{
	struct rtc_count_config config_rtc_count;
	rtc_count_get_config_defaults(&config_rtc_count);

	config_rtc_count.prescaler           = RTC_COUNT_PRESCALER_DIV_1;
	config_rtc_count.mode                = RTC_COUNT_MODE_16BIT;
	config_rtc_count.continuously_update = true;
	/* initialize rtc */
	rtc_count_init(&rtc_instance,RTC,&config_rtc_count);

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

	rtc_count_set_period(&rtc_instance,TIME_PERIOD_1MSEC);
}

/*! \brief Main function
 *
 */
int main(void)
{
	uint8_t button1_state;
	uint8_t button2_state;
	uint8_t slider_state;
	uint8_t slider_position;

	/**
	 * Initialize and configure system and generic clocks.
	 * Use conf_clocks.h to configure system and generic clocks.
	 */
	system_init();

	/**
	 * Enable global interrupts.
	 */
	system_interrupt_enable_global();

	/**
	 * Initialize delay service.
	 */
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
	 * Configure port pins
	 */
	configure_port_pins();

	/**
	 * Turn off all extension board LEDs
	 */
	port_pin_set_output_level(LED_0_PIN, 1);
	port_pin_set_output_level(LED_1_PIN, 1);
	port_pin_set_output_level(LED_2_PIN, 1);
	port_pin_set_output_level(LED_3_PIN, 1);
	port_pin_set_output_level(LED_4_PIN, 1);
	port_pin_set_output_level(LED_5_PIN, 1);
	port_pin_set_output_level(LED_6_PIN, 1);
	port_pin_set_output_level(LED_7_PIN, 1);
	port_pin_set_output_level(LED_8_PIN, 1);
	port_pin_set_output_level(LED_9_PIN, 1);
	port_pin_set_output_level(LED_R_PIN, 1);
	port_pin_set_output_level(LED_G_PIN, 1);
	port_pin_set_output_level(LED_B_PIN, 1);

	PWM_Count = 0;
	/* Configure System Sleep mode to STANDBY. */
	system_set_sleepmode(SYSTEM_SLEEPMODE_STANDBY);
	while (1) {

		/**
		 * Goto STANDBY sleep mode, unless woken by timer or PTC interrupt.
		 */
		system_sleep();

		/**
		 * Start touch sensor measurement, if touch_time.time_to_measure_touch flag is set by timer.
		 */
		touch_sensors_measure();

		if ((p_mutlcap_measure_data->measurement_done_touch == 1u)) {

			p_mutlcap_measure_data->measurement_done_touch = 0u;

			/**
			 * Get touch sensor states
			 */
			button1_state = GET_MUTLCAP_SENSOR_STATE(0);
			button2_state = GET_MUTLCAP_SENSOR_STATE(1);
			rotor_state = GET_MUTLCAP_SENSOR_STATE(2);
			slider_state = GET_MUTLCAP_SENSOR_STATE(3);

			if (button1_state) {
				port_pin_set_output_level(LED_8_PIN, 0);
			} else {
				port_pin_set_output_level(LED_8_PIN, 1);
			}

			if (button2_state) {
				port_pin_set_output_level(LED_9_PIN, 0);
			} else {
				port_pin_set_output_level(LED_9_PIN, 1);
			}

			/**
			 * Clear all slider controlled LEDs
			 */
			port_pin_set_output_level(LED_0_PIN, 1);
			port_pin_set_output_level(LED_1_PIN, 1);
			port_pin_set_output_level(LED_2_PIN, 1);
			port_pin_set_output_level(LED_3_PIN, 1);
			port_pin_set_output_level(LED_4_PIN, 1);
			port_pin_set_output_level(LED_5_PIN, 1);
			port_pin_set_output_level(LED_6_PIN, 1);
			port_pin_set_output_level(LED_7_PIN, 1);

					/**
			 * If slider is activated
			 */
			if(slider_state)
			{
				/**
				 * Parse slider position
				 */
				slider_position = GET_MUTLCAP_ROTOR_SLIDER_POSITION(1);
				slider_position = slider_position >> 5u;
				switch(slider_position)
				{
					case 0:
						port_pin_set_output_level(LED_0_PIN, 0);
						break;
					case 1:
						port_pin_set_output_level(LED_0_PIN, 0);
						port_pin_set_output_level(LED_1_PIN, 0);
						break;
					case 2:
						port_pin_set_output_level(LED_0_PIN, 0);
						port_pin_set_output_level(LED_1_PIN, 0);
						port_pin_set_output_level(LED_2_PIN, 0);
						break;
					case 3:
						port_pin_set_output_level(LED_0_PIN, 0);
						port_pin_set_output_level(LED_1_PIN, 0);
						port_pin_set_output_level(LED_2_PIN, 0);
						port_pin_set_output_level(LED_3_PIN, 0);
						break;
					case 4:
						port_pin_set_output_level(LED_0_PIN, 0);
						port_pin_set_output_level(LED_1_PIN, 0);
						port_pin_set_output_level(LED_2_PIN, 0);
						port_pin_set_output_level(LED_3_PIN, 0);
						port_pin_set_output_level(LED_4_PIN, 0);
						break;
					case 5:
						port_pin_set_output_level(LED_0_PIN, 0);
						port_pin_set_output_level(LED_1_PIN, 0);
						port_pin_set_output_level(LED_2_PIN, 0);
						port_pin_set_output_level(LED_3_PIN, 0);
						port_pin_set_output_level(LED_4_PIN, 0);
						port_pin_set_output_level(LED_5_PIN, 0);
						break;
					case 6:
						port_pin_set_output_level(LED_0_PIN, 0);
						port_pin_set_output_level(LED_1_PIN, 0);
						port_pin_set_output_level(LED_2_PIN, 0);
						port_pin_set_output_level(LED_3_PIN, 0);
						port_pin_set_output_level(LED_4_PIN, 0);
						port_pin_set_output_level(LED_5_PIN, 0);
						port_pin_set_output_level(LED_6_PIN, 0);
						break;
					case 7:
						port_pin_set_output_level(LED_0_PIN, 0);
						port_pin_set_output_level(LED_1_PIN, 0);
						port_pin_set_output_level(LED_2_PIN, 0);
						port_pin_set_output_level(LED_3_PIN, 0);
						port_pin_set_output_level(LED_4_PIN, 0);
						port_pin_set_output_level(LED_5_PIN, 0);
						port_pin_set_output_level(LED_6_PIN, 0);
						port_pin_set_output_level(LED_7_PIN, 0);
						break;
					default:
						port_pin_set_output_level(LED_0_PIN, 1);
						port_pin_set_output_level(LED_1_PIN, 1);
						port_pin_set_output_level(LED_2_PIN, 1);
						port_pin_set_output_level(LED_3_PIN, 1);
						port_pin_set_output_level(LED_4_PIN, 1);
						port_pin_set_output_level(LED_5_PIN, 1);
						port_pin_set_output_level(LED_6_PIN, 1);
						port_pin_set_output_level(LED_7_PIN, 1);
						break;
				}
			}
		}//measurement done flag
	}//while(1)
}//main
