/**
 * \file
 *
 * \brief Event Manager.
 *
 * Copyright (c) 2012-2015 Atmel Corporation. All rights reserved.
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
#include "event.h"

// Internal counter for the QTouch library
volatile uint32_t event_qtouch_sensors_idle_count = 0u;
// Push button PB0 event
volatile bool event_pbEvent = false;
// Boolean value to detect if the QTouch button is pressed or not
bool event_qTouchButtonPressed = false;

static void wdt_clear(void);

/**
 *  \brief Asynchronous timer (ASF) handler for the QTouch acquisition.
 *  and clear Watchdog counter - generates interrupt every 100ms
 */
static void ast_per_callback(void)
{
	touch_sensors_update_time();
	event_qtouch_sensors_idle_count++;
	ast_clear_interrupt_flag(AST, AST_INTERRUPT_PER);
	wdt_clear();
}

/**
 *  \brief External interrupt handler, used by PB0 push button
 */
static void eic5_callback(void)
{
	sysclk_enable_peripheral_clock(EIC);

	if(eic_line_interrupt_is_pending(EIC,GPIO_PUSH_BUTTON_EIC_LINE))
	{
		eic_line_clear_interrupt(EIC,GPIO_PUSH_BUTTON_EIC_LINE);
		event_pbEvent = true;
	}
	sysclk_disable_peripheral_clock(EIC);
}

/**
 *  \brief Set Control register function for WDT
 */
static void wdt_set_ctrl(uint32_t ctrl)
{
	WDT->WDT_CTRL = ctrl | WDT_CTRL_KEY((uint32_t)0x55);
	WDT->WDT_CTRL = ctrl | WDT_CTRL_KEY((uint32_t)0xAA);
}

/**
 *  \brief Clear WDT
 */
static void wdt_clear(void)
{
	while (!(WDT->WDT_SR & WDT_SR_CLEARED));
	WDT->WDT_CLR = WDT_CLR_WDTCLR | WDT_CLR_KEY((uint32_t)0x55);
	WDT->WDT_CLR = WDT_CLR_WDTCLR | WDT_CLR_KEY((uint32_t)0xAA);
}

/**
 *  \brief Enable WDT
 */
static void enable_wdt(void)
{
	wdt_set_ctrl(WDT_CTRL_EN | WDT_CTRL_CEN | WDT_CTRL_PSEL(17) );
}

/**
 * \brief Event Manager QTouch Init.
 */
void event_qtouch_init(void)
{
	struct ast_config ast_conf;
	// Enable Osc32
	osc_enable(OSC_ID_OSC32);
	while(!osc_is_ready(OSC_ID_OSC32));

	// Initialize AST Controller
	ast_enable(AST);

	ast_conf.mode = AST_COUNTER_MODE;
	ast_conf.osc_type = AST_OSC_32KHZ;
	ast_conf.psel = AST_PSEL_32KHZ_1HZ;
	ast_conf.counter = 0;
	ast_set_config(AST, &ast_conf);

	ast_clear_interrupt_flag(AST, AST_INTERRUPT_PER);
	ast_write_periodic0_value(AST, 9);

	ast_set_callback(AST, AST_INTERRUPT_PER, ast_per_callback,
		AST_PER_IRQn, 0);

	//CATB & Related clocks - for QTouch Sensors
	sysclk_enable_peripheral_clock(CATB);
	sysclk_enable_peripheral_clock(PDCA);

	// Initialize QTouch Library.
	touch_sensors_init();
}
/**
 * \brief Event Button Init.
 */
void event_button_init(void)
{
	// Structure holding the configuration parameters
	// of the EIC module.
	struct eic_line_config eic_line_conf;

	// Initialize EIC Controller
	sysclk_enable_peripheral_clock(EIC);

	// Enable level-triggered interrupt.
	eic_line_conf.eic_mode   = EIC_MODE_EDGE_TRIGGERED;
	// Interrupt will trigger on low-level.
	eic_line_conf.eic_level  = EIC_LEVEL_LOW_LEVEL;
	// Edge on falling edge
	eic_line_conf.eic_edge = EIC_EDGE_FALLING_EDGE;
	// Enable filter.
	eic_line_conf.eic_filter = EIC_FILTER_DISABLED;
	// For Wake Up mode, initialize in asynchronous mode
	eic_line_conf.eic_async  = EIC_ASYNCH_MODE;

	// Enable clock for EIC controller
	eic_enable(EIC);
	// Init the EIC controller with the options
	eic_line_set_config(EIC, GPIO_PUSH_BUTTON_EIC_LINE,
		&eic_line_conf);
	// Init the callback
	eic_line_set_callback(EIC, GPIO_PUSH_BUTTON_EIC_LINE, eic5_callback,
		EIC_5_IRQn, 1);
	// Enable the EIC line
	eic_line_enable(EIC, GPIO_PUSH_BUTTON_EIC_LINE);

	// EIC can wake the device from backup mode
	bpm_enable_wakeup_source(BPM, BPM_BKUP_WAKEUP_SRC_EIC
		| BPM_BKUP_WAKEUP_SRC_AST);
	// EIC can wake the device from backup mode
	bpm_enable_backup_pin(BPM, 1 << GPIO_PUSH_BUTTON_EIC_LINE);
	// Retain I/O lines after wakeup from backup
	bpm_disable_io_retention(BPM);
	bpm_enable_io_retention(BPM);
	bpm_enable_fast_wakeup(BPM);
	sysclk_disable_peripheral_clock(EIC);

	event_pbEvent = false;

	// Initialize WDT Controller
	sysclk_enable_peripheral_clock(WDT);
	enable_wdt();
}
/**
 * \brief Return QTouch Slider State.
 * \param event_qtouch_position returned slider position.
 */
bool event_qtouch_get_slider_state( uint8_t* event_qtouch_position )
{
	bool event_qtouch_slider_state = false;

	// Use Rotor/Slider Position.
	if (GET_QT_SENSOR_STATE(1) || GET_QT_SENSOR_STATE(2)
		|| GET_QT_SENSOR_STATE(3) ) {
		*event_qtouch_position = GET_QT_ROTOR_SLIDER_POSITION(0);
		*event_qtouch_position = 255 - *event_qtouch_position;
		event_qtouch_slider_state = true;
	}
	else {
		event_qtouch_slider_state = false;
	}

	return event_qtouch_slider_state;
}

/**
 * \brief Return QTouch Button State.
 */
bool event_qtouch_get_button_state(void)
{
	bool event_qtouch_button_state = false;

	// Use QTouch button status
	if (GET_QT_SENSOR_STATE(0)&&(event_qTouchButtonPressed==false)) {
		event_qTouchButtonPressed = true;
		event_qtouch_button_state = false;
	}
	else if (!GET_QT_SENSOR_STATE(0)&&(event_qTouchButtonPressed==true)) {
		event_qTouchButtonPressed = false;
		event_qtouch_button_state = true;
	}
	return event_qtouch_button_state;
}

/**
 * \brief Check if push button is pressed or not.
 */
bool event_is_push_button_pressed(void)
{
	bool event_button_state = false;
	event_button_state = event_pbEvent;
	if (event_pbEvent == true) {
		event_pbEvent = false;
	}
	return event_button_state;
}
