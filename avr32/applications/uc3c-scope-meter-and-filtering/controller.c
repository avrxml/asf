/**
 * \file
 *
 * \brief Controller for AT42QT1060 Touch Sensor.
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
#include "board.h"
#include "gpio.h"
#include "twim.h"
#include "controller.h"
#include "at42qt1060.h"
#include "cycle_counter.h"
#include "sysclk.h"
#include "conf_demo.h"
#include "conf_at42qt1060.h"

volatile bool touch_detect = false;
t_cpu_time joystick_key_sensibility_timer;
struct at42qt1060_data {
	uint8_t detect_status;
	uint16_t key_signal[6];
	uint8_t key_threshold[6];
	uint16_t key_ref_value[6];
};

volatile bool zoom_view = false;
volatile int32_t zoom_view_id;
volatile uint8_t signal_selected_display=1;

//! Local definition for call detect callback
void touch_detect_callback(void);
//! Local definition for controller Key 1: Right Button (Input+Noise Signal)
bool controller_key_fct1(void);
//! Local definition for controller Key 2: Left Button (Input Signal)
bool controller_key_fct2(void);
//! Local definition for controller Key 3: Up Button (FPU Signal)
bool controller_key_fct3(void);
//! Local definition for controller Key 4: Down Button (FP Signal)
bool controller_key_fct4(void);
//! Local definition for controller Key 5: Middle Button (Noise Signal)
bool controller_key_fct5(void);

/*! \brief TWI Initialization for QTouch Controller
 */
static void controller_twi_init(void)
{
	const gpio_map_t AT42QT1060_TWI_GPIO_MAP = {
		{AT42QT1060_TWI_SCL_PIN, AT42QT1060_TWI_SCL_FUNCTION},
		{AT42QT1060_TWI_SDA_PIN, AT42QT1060_TWI_SDA_FUNCTION}
	};

	const twi_options_t AT42QT1060_TWI_OPTIONS = {
		.pba_hz = sysclk_get_pba_hz(),
		.speed  = AT42QT1060_TWI_MASTER_SPEED,
		.chip   = AT42QT1060_TWI_ADDRESS,
		.smbus  = false,
	};

	// Assign I/Os to TWI.
	gpio_enable_module(AT42QT1060_TWI_GPIO_MAP,
	sizeof(AT42QT1060_TWI_GPIO_MAP) / sizeof(AT42QT1060_TWI_GPIO_MAP[0]));
	// Initialize as master.
	twi_master_init(AT42QT1060_TWI, &AT42QT1060_TWI_OPTIONS);
}

/*! \brief Callback function for a detect event of the touch sensor device.
 */
void touch_detect_callback(void)
{
	touch_detect = true;
}

/*! \brief Controller Init:
 *          - Initialize TWI,
 *          - Initialize QTouch Sensor Controller
 */
void controller_task_init(void)
{
	controller_twi_init();
	// wait until the device settles its CHG line
	cpu_delay_ms(230, sysclk_get_cpu_hz());
	at42qt1060_init(sysclk_get_pba_hz());
	at42qt1060_register_int(&touch_detect_callback);
	cpu_set_timeout(cpu_ms_2_cy(CONTROLLER_DEBOUNCE_MS, sysclk_get_cpu_hz()),
		&joystick_key_sensibility_timer);
}

struct at42qt1060_data touch_data;
// FCT3/ Up
bool controller_key_fct3(void)
{
	if (touch_data.detect_status & (1 << 0)) {
		return true;
	}
	return false;
}

// FCT4/ Down
bool controller_key_fct4(void)
{
	if (touch_data.detect_status & (1 << 1)) {
		return true;
	}
	return false;
}

// FCT1/ Right button
bool controller_key_fct1(void)
{
	if (touch_data.detect_status & (1 << 2)) {
		return true;
	}
	return false;
}

// FCT2/ Left
bool controller_key_fct2(void)
{
	if (touch_data.detect_status & (1 << 3)) {
		return true;
	}
	return false;
}

// FCT5/ Middle
bool controller_key_fct5(void)
{
	if (touch_data.detect_status & (1 << 4)) {
		return true;
	}
	return false;
}

void controller_task(void)
{
	// if a touch is detected we read the status
	if (touch_detect) {
		touch_data.detect_status =
			at42qt1060_read_reg(AT42QT1060_DETECTION_STATUS);
		// need to read input port status too to reset CHG line
		at42qt1060_read_reg(AT42QT1060_INPUT_PORT_STATUS);
		touch_detect = false;
	}
	// Left Button (Select Input Signal)
	if (controller_key_fct2()) {
		signal_selected_display = 1;
	}
	// Middle Button (Select Noise Signal)
	else if (controller_key_fct5()) {
		signal_selected_display = 2;
	}
	// Down Button (Select FP Signal)
	else if (controller_key_fct4()) {
		signal_selected_display = 4;
	}
	// Right Button (Select Input+Noise Signal)
	else if (controller_key_fct1()) {
		signal_selected_display = 3;
	}
	// Up Button (Select FPU Signal)
	else if (controller_key_fct3()) {
		signal_selected_display = 5;
	}
}
