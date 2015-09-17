/**
 * \file
 *
 * \brief Main functions for the Voltage meter demonstration.
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

/**
 * \mainpage XMEGA-B1 Xplained Voltage meter demonstration
 *
 * Four buttons available to calibrate the ADC and to display measurements.
 * Before to use ADC, offset and gain must be calibrated (only before the first
 * measurement).
 * ADC calibration values are stored in EEPROM.
 * Next measurements will use calibrated ADC.
 * If calibration values are out range, EEPROM write operation is not allowed by
 * software.
 *
 * Potentiometer voltage or external voltage (connected on Vin/GND connector,
 * top left) are displayed if CS2 or CS3 are pressed.
 *
 * CS0: Gain  calibration (need an external power supply + voltmeter)
 * Connect 7V to External input and adjust 0.6V with potentiometer
 * (check on PB1 the voltage) before to press CS0
 * Important: The calibration is stored when CS0 is released (to save EEPROM
 * write cycles)
 *
 * CS1: Offset calibration
 * Connect GND to external input and adjust 0V with potentiometer
 * before to press CS1
 * Important: The calibration is stored when CS0 is released (to save EEPROM
 * write cycles)
 *
 * CS2: Potentiometer voltage
 *
 * CS3: External voltage
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include "asf.h"
#include "main.h"
#include "ui.h"
#include <string.h>

//! LCD strings for on-board sensors
static uint8_t lcd_msg_gain_cal[LCD_TEXT_MEMORY] = "GAIN CALIBRATION     ";
static uint8_t lcd_msg_offset_cal[LCD_TEXT_MEMORY] = "OFFSET CALIBRATION   ";
static uint8_t lcd_msg_potentiometer[LCD_TEXT_MEMORY] = "POTENTIOMETER VOLTAGE    ";
static uint8_t lcd_msg_ext_voltage[LCD_TEXT_MEMORY] = "EXTERNAL VOLTAGE    ";


struct adc_ch_t {
	enum adcch_positive_input in;
	enum adc_reference ref;
};

//! Table of ADC mux values
static struct adc_ch_t adc_conv[]={
		{EXT_VIN_ADC_INPUT,ADC_REF_BANDGAP},
		{POTENTIOMETER_ADC_INPUT,ADC_REF_BANDGAP}
		};

//! ADC configurations
struct adc_config adc_conf;
static struct adc_channel_config adcch_conf;
static uint8_t adc_mux_index=0;

//! Sensors values
static uint16_t potentiometer,ext_voltage;

//! Sensor selector to be displayed on LCD
static enum sensor_display_t sensor_select;

//external voltage offset calibration
static eeprom_addr_t ext_offset_cal_addr = 0x00;
static uint16_t ext_offset_cal_val = ADC_OFFSET_EXT;

//potentiometer offset calibration
static eeprom_addr_t pot_offset_cal_addr = 0x02;
static uint16_t pot_offset_cal_val = ADC_OFFSET_POT;

//external voltage gain calibration
static eeprom_addr_t ext_gain_cal_addr = 0x04;
static uint16_t ext_gain_cal_val = ADC_GAIN_EXT;

//potentiometer gain calibration
static eeprom_addr_t pot_gain_cal_addr = 0x06;
static uint16_t pot_gain_cal_val = ADC_GAIN_POT;

//if button pressed, check button released to save data
uint8_t test_release_button_gain_cal = false;
uint8_t test_release_button_offset_cal  = false;

/**
 * \brief Callback function for ADC interrupts
 *
 * \param adc Pointer to ADC module.
 * \param channel ADC channel number.
 * \param result Conversion result from ADC channel.
 */
static void adc_handler(ADC_t *adc, uint8_t channel, adc_result_t result)
{
	switch (adc_conv[adc_mux_index].in) {
		case EXT_VIN_ADC_INPUT:
			ext_voltage=result;
			adc_mux_index++;

			//start new measurement
			adc_disable(&EXT_VIN_ADC_MODULE);
			adc_set_conversion_parameters(&adc_conf
					,ADC_SIGN_OFF
					,ADC_RES_12
					,adc_conv[adc_mux_index].ref);
			adc_write_configuration(
					&POTENTIOMETER_ADC_MODULE
					, &adc_conf);
			adc_enable(&POTENTIOMETER_ADC_MODULE);
			adcch_set_input(&adcch_conf
					,adc_conv[adc_mux_index].in
					,ADCCH_NEG_NONE,1);
			adcch_write_configuration(
					&POTENTIOMETER_ADC_MODULE
					, ADC_CH0, &adcch_conf);
			adc_start_conversion(&POTENTIOMETER_ADC_MODULE
					, ADC_CH0);
			break;
		case POTENTIOMETER_ADC_INPUT:
			potentiometer=result;
		break;

		default:
		break;
	}
}

/**
 * \brief Timer Counter Overflow interrupt callback function
 *
 * This function is called when an overflow interrupt has occurred on
 * TCC0.
 */
static void tcc0_ovf_interrupt_callback(void)
{
	adc_mux_index=0;
	adc_disable(&EXT_VIN_ADC_MODULE);
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF, ADC_RES_12
			,adc_conv[adc_mux_index].ref);
	adc_write_configuration(&EXT_VIN_ADC_MODULE, &adc_conf);
	adc_enable(&EXT_VIN_ADC_MODULE);
	adcch_set_input(&adcch_conf, adc_conv[adc_mux_index].in
			, ADCCH_NEG_NONE,1);
	adcch_write_configuration(&EXT_VIN_ADC_MODULE, ADC_CH0, &adcch_conf);
	adc_start_conversion(&EXT_VIN_ADC_MODULE, ADC_CH0);
}

/**
 * \brief LCD interrupt callback function
 *
 * This function is called when an interrupt has occurred on LCD.
 */
void lcd_interrupt_callback(void)
{
	uint32_t buf_temp;

	// Change text string on LCD when a button is pressed
	if (ui_pusb_button_0()) {
		ui_display_text(lcd_msg_gain_cal, sizeof(lcd_msg_gain_cal));
		c42048a_clear_pixel(ICON_DEGREE_C);
		c42048a_clear_pixel(ICON_MILLI_VOLT);
		test_release_button_gain_cal = true;
	}

	//to limit EEPROM write operations, CAL stored when button released
	if((ui_pusb_button_0() == false)
			&& (test_release_button_gain_cal == true)) {
		buf_temp = (((uint32_t)ext_voltage - ext_offset_cal_val)
				*8000/4095);
		buf_temp = buf_temp * ADC_GAIN_EXT;
		buf_temp = buf_temp / ext_gain_cal_val;

		//store gain value in Flash memory
		if(((buf_temp > EXT_GAIN_MIN_VAL)
				&& (buf_temp < EXT_GAIN_MAX_VAL))){
			ext_gain_cal_val = buf_temp;
			nvm_eeprom_write_byte(ext_gain_cal_addr + 1
				, (uint8_t)(buf_temp>>8));
			nvm_eeprom_write_byte(ext_gain_cal_addr,
				(uint8_t)(buf_temp));
		}

		buf_temp = (((uint32_t)potentiometer-pot_offset_cal_val)
			* 1000/4095);
		buf_temp = buf_temp * ADC_GAIN_POT;
		buf_temp = buf_temp / pot_gain_cal_val;

		//store gain value in Flash memory
		if(((buf_temp > POT_GAIN_MIN_VAL)
				&& (buf_temp < POT_GAIN_MAX_VAL))){
			pot_gain_cal_val = buf_temp;
			nvm_eeprom_write_byte(pot_gain_cal_addr +1,
				(uint8_t)(buf_temp>>8));
			nvm_eeprom_write_byte(pot_gain_cal_addr,
				(uint8_t)(buf_temp));
		}
		test_release_button_gain_cal = false;
	}

	if (ui_pusb_button_1()) {
		ui_display_text(lcd_msg_offset_cal, sizeof(lcd_msg_offset_cal));
		sensor_select=OFFSET_CALIBRATION;
		c42048a_clear_pixel(ICON_DEGREE_C);
		c42048a_clear_pixel(ICON_MILLI_VOLT);
		test_release_button_offset_cal = true;
	}

	//to limit EEPROM write operations, OFFSET stored when button released
	if((ui_pusb_button_1() == false)
			&& (test_release_button_offset_cal == true)) {
		//store offset value in Flash memory
		if(((ext_voltage > EXT_OFFSET_MIN_VAL)
				&& (ext_voltage < EXT_OFFSET_MAX_VAL))){
			ext_offset_cal_val = ext_voltage;
			nvm_eeprom_write_byte(ext_offset_cal_addr + 1
				,(uint8_t)(ext_voltage>>8));
			nvm_eeprom_write_byte(ext_offset_cal_addr
				, (uint8_t)(ext_voltage));
		}

		//store offset value in Flash memory
		if(((potentiometer > POT_OFFSET_MIN_VAL)
				&& (potentiometer < POT_OFFSET_MAX_VAL))){
			pot_offset_cal_val = potentiometer;
			nvm_eeprom_write_byte(pot_offset_cal_addr +1
				,(uint8_t)(potentiometer>>8));
			nvm_eeprom_write_byte(pot_offset_cal_addr
				, (uint8_t)(potentiometer));
		}

		test_release_button_offset_cal = false;
	}

	if (ui_pusb_button_2()) {
		ui_display_text(lcd_msg_potentiometer, sizeof(lcd_msg_potentiometer));
		sensor_select=POTENTIOMETER_SENSOR;
		c42048a_clear_pixel(ICON_DEGREE_C);
		c42048a_set_pixel(ICON_MILLI_VOLT);
	}

	if (ui_pusb_button_3()) {
		ui_display_text(lcd_msg_ext_voltage, sizeof(lcd_msg_ext_voltage));
		sensor_select=EXT_VOLTAGE_INPUT;
		c42048a_clear_pixel(ICON_DEGREE_C);
		c42048a_set_pixel(ICON_MILLI_VOLT);
	}

	// Performs LCD scrolling
	ui_scroll_text();

	switch (sensor_select) {
		case POTENTIOMETER_SENSOR:
			if(potentiometer<pot_offset_cal_val) {
				potentiometer=pot_offset_cal_val;
			}
			buf_temp = (((uint32_t)potentiometer-pot_offset_cal_val)
				* 1000/4095);
			buf_temp = buf_temp * ADC_GAIN_POT;
			buf_temp = buf_temp / pot_gain_cal_val;
			ui_display_sensor(buf_temp,UI_UNIT_MILIVOLT);
			break;

		case EXT_VOLTAGE_INPUT:
			if(ext_voltage<ext_offset_cal_val) {
				ext_voltage = ext_offset_cal_val;
			}
			buf_temp = (((uint32_t)ext_voltage
				- ext_offset_cal_val)*8000/4095);
			buf_temp = buf_temp * ADC_GAIN_EXT;
			buf_temp = buf_temp / ext_gain_cal_val;
			ui_display_sensor(buf_temp,UI_UNIT_MILIVOLT);
			break;

		default:
			break;
	}
}

/*! \brief Main function. Execution starts here.
 */
int main(void)
{
	uint16_t ext_cal_temp, pot_cal_temp;

	irq_initialize_vectors();
	cpu_irq_enable();

	// Initialize the sleep manager
	sleepmgr_init();
	sysclk_init();
	board_init();

	// Init LCD
	ui_init();
	lcd_interrupt_init(LCD_INTLVL_LO_gc, 12);
	lcd_blinkrate_init(LCD_BLINKRATE_4Hz_gc);

	ext_cal_temp = nvm_eeprom_read_byte(ext_offset_cal_addr);
	ext_cal_temp += (nvm_eeprom_read_byte(ext_offset_cal_addr + 1)<<8);
	if((ext_cal_temp > EXT_OFFSET_MIN_VAL)
			&& (ext_cal_temp < EXT_OFFSET_MAX_VAL)){
		 ext_offset_cal_val= ext_cal_temp;
	}

	pot_cal_temp = nvm_eeprom_read_byte(pot_offset_cal_addr);
	pot_cal_temp += (nvm_eeprom_read_byte(pot_offset_cal_addr + 1)<<8);
	if((pot_cal_temp > POT_OFFSET_MIN_VAL)
			&& (pot_cal_temp < POT_OFFSET_MAX_VAL)){
		pot_offset_cal_val= pot_cal_temp;
	}

	/* Configure the ADC module:
	 * - unsigned, 12-bit results
	 * - 200 kHz maximum clock rate
	 * - manual conversion triggering
	 * - callback function
	 */
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF, ADC_RES_12,
		ADC_REF_VCCDIV2);
	adc_set_clock_rate(&adc_conf, 200000UL);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_write_configuration(&EXT_VIN_ADC_MODULE, &adc_conf);
	adc_set_callback(&EXT_VIN_ADC_MODULE, &adc_handler);

	// Configure ADC channel:
	adcch_set_input(&adcch_conf, EXT_VIN_ADC_INPUT, ADCCH_NEG_NONE,1);
	adcch_set_interrupt_mode(&adcch_conf, ADCCH_MODE_COMPLETE);
	adcch_enable_interrupt(&adcch_conf);
	adcch_write_configuration(&EXT_VIN_ADC_MODULE, ADC_CH0, &adcch_conf);

	// Enable the ADC
	adc_enable(&EXT_VIN_ADC_MODULE);

	// Unmask clock for TCC0
	tc_enable(&TCC0);
	tc_set_overflow_interrupt_callback(&TCC0,
			tcc0_ovf_interrupt_callback);

	// Configure TC in normal mode, configure period
	tc_set_wgm(&TCC0, TC_WG_NORMAL);
	tc_write_period(&TCC0, TIMER_EXAMPLE_PERIOD);

	// Enable TC interrupts overflow
	tc_set_overflow_interrupt_level(&TCC0, PMIC_LVL_LOW);

	// Time selection
	tc_set_resolution(&TCC0, 50000);

	lcd_set_frame_interrupt_callback(lcd_interrupt_callback);

	// The main loop manages only the power mode
	while (true) {
		sleepmgr_enter_sleep();
	}
}
