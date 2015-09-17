/**
 * \file
 *
 * \brief Application Controller for AT42QT1060 Touch Management.
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
#include "intc.h"
#include "controller.h"
#include "at42qt1060.h"
#include "conf_at42qt1060.h"
#include "delay.h"

#define JOYSTICK_KEY_DEBOUNCE_MS    200

// Local definition of touch detect function
void touch_detect_callback(void);

volatile bool touch_detect = false;
static uint32_t static_fcpu_hz;
t_cpu_time joystick_key_sensibility_timer;
struct at42qt1060_data {
	uint8_t detect_status;
	uint16_t key_signal[6];
	uint8_t key_threshold[6];
	uint16_t key_ref_value[6];
};

/*! \brief Callback function for a detect event of the touch sensor device.
 */
void touch_detect_callback(void)
{
	touch_detect = true;
}

struct at42qt1060_data touch_data;
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
}

void controller_init(uint32_t fcpu_hz, uint32_t fhsb_hz, uint32_t fpbb_hz,
		uint32_t fpba_hz)
{
	// wait until the device settles its CHG line
	delay_ms(230);
	at42qt1060_init(fcpu_hz);
	at42qt1060_register_int(&touch_detect_callback);
	//static_fcpu_hz = fcpu_hz;
	cpu_set_timeout(cpu_ms_2_cy(JOYSTICK_KEY_DEBOUNCE_MS, static_fcpu_hz),
		&joystick_key_sensibility_timer);
}

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
