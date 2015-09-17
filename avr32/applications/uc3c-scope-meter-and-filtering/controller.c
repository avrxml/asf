/**
 * \file
 *
 * \brief Controller for AT42QT1060 Touch Sensor.
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
