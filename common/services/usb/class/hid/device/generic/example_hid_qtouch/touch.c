/**
 * \file
 *
 * \brief Functions for interfacing touch
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
#include <asf.h>
#include "touch.h"

#ifdef _DEBUG_INTERFACE_
// Include files for QTouch Studio integration 
#include "QDebug.h"
#include "QDebugTransport.h"
#endif

// TIMER_PERIOD computation
#define TIMER_PERIOD TICKS_PER_MS * qt_measurement_period_msec

/*
 * Stuff that is passed to the compiler
 * _QTOUCH_
 * SNS=F
 * SNSK=F
 * _SNS1_SNSK1_SAME_PORT_
 * QT_DELAY_CYCLES=1 (at 2MHz CPU freq this is 0.5us)
 * QT_NUM_CHANNELS=4
 * QTOUCH_STUDIO_MASKS=1
 * NUMBER_OF_PORTS=1
 */
#define GET_SENSOR_STATE(SENSOR_NUMBER) \
		qt_measure_data.qt_touch_status.sensor_states[(SENSOR_NUMBER/8)] \
		& (1 << (SENSOR_NUMBER % 8))


/**
 * \name The timing information for timer to fire periodically to measure touch
 */
#ifdef TICKS_PER_MS
#undef TICKS_PER_MS
//500u. 1.1 change as clock is running at 48MHz
#define TICKS_PER_MS                6000u
#endif

//! Timer period in msec.
uint16_t qt_measurement_period_msec = 25u;

/* 
 * This configuration data structure parameters if needs to be changed will be
 * changed in the qt_set_parameters function
 */
extern qt_touch_lib_config_data_t qt_config_data;
// touch output - measurement data
extern qt_touch_lib_measure_data_t qt_measure_data;
// Get sensor delta values
extern int16_t qt_get_sensor_delta( uint8_t sensor);
#ifdef QTOUCH_STUDIO_MASKS
extern TOUCH_DATA_T SNS_array[2][2];
extern TOUCH_DATA_T SNSK_array[2][2];
#endif
// Output can be observed in the watch window using this pointer
qt_touch_lib_measure_data_t *pqt_measure_data = &qt_measure_data;

// flag set by timer ISR when it's time to measure touch
static volatile bool time_to_measure_touch = false;

// current time, set by timer ISR
static volatile uint16_t current_time_ms_touch = 0;

// The ms timeout variable
unsigned int gMsTimeout;

/*! \brief qt_set_parameters function
 * This will fill the default threshold values in the configuration
 * data structure.But User can change the values of these parameters .
 * \note initialize configuration data for processing
 */
static void qt_set_parameters(void);

//! \brief Configures the sensors as keys and assigns the channel numbers
static void config_sensor(void);

//! \brief Configures timer ISR to fire regularly
static void init_timer_isr(void);


static void example_cca_interrupt_callback (void)
{
	if (gMsTimeout) {
		gMsTimeout--;
	}
	// set flag: it's time to measure touch
	time_to_measure_touch = 1u;
	// update the current time
	current_time_ms_touch += qt_measurement_period_msec;
}

static void qt_set_parameters(void)
{
	qt_config_data.qt_di              = DEF_QT_DI;
	qt_config_data.qt_neg_drift_rate  = DEF_QT_NEG_DRIFT_RATE;
	qt_config_data.qt_pos_drift_rate  = DEF_QT_POS_DRIFT_RATE;
	qt_config_data.qt_max_on_duration = DEF_QT_MAX_ON_DURATION;
	qt_config_data.qt_drift_hold_time = DEF_QT_DRIFT_HOLD_TIME;
	qt_config_data.qt_recal_threshold = DEF_QT_RECAL_THRESHOLD;
	qt_config_data.qt_pos_recal_delay = DEF_QT_POS_RECAL_DELAY;
}

static void config_sensor(void)
{
	/* 
	 * The sensor is wired up with SNS=PF6 and SNSK=PF7
	 * When using "pin reconfigurability" this will result in channel 0
	 * because it is the first and only channel that is used.
	 * For the standard qtouch library setup we would need to use
	 * channel 3 since we are using the last two pins on the port.
	 */
	qt_enable_key(CHANNEL_0, NO_AKS_GROUP, 10, HYST_6_25);
}

static void init_timer_isr(void)
{
	tc_enable (&TCC0);
	tc_write_period (&TCC0, TIMER_PERIOD);
	tc_write_clock_source (&TCC0, TC_CLKSEL_DIV8_gc);
	tc_set_cca_interrupt_level (&TCC0, PMIC_LVL_LOW);
	tc_set_cca_interrupt_callback (&TCC0, example_cca_interrupt_callback);
}

void touch_init(void)
{

#ifdef QTOUCH_STUDIO_MASKS
	SNS_array[0][0] = 0x40;
	SNS_array[0][1] = 0x0;
	SNS_array[1][0] = 0x0;
	SNS_array[1][1] = 0x0;

	SNSK_array[0][0] = 0x80;
	SNSK_array[0][1] = 0x0;
	SNSK_array[1][0] = 0x0;
	SNSK_array[1][1] = 0x0;
#endif

	init_timer_isr();
	config_sensor();
	qt_init_sensing();
	qt_set_parameters();

	/* 
	 * Set up callback function. This function is called after the library
	 * has made capacitive measurements, but before it has processed them.
	 * The user can use this hook to apply filter functions to the measured
	 * signal values.(Possibly to fix sensor layout faults)
	 */
	qt_filter_callback = 0;

#ifdef _DEBUG_INTERFACE_
	QDebug_Init();
#endif
}

bool check_touch_key_pressed(void)
{
	uint16_t status_flag;
	uint16_t burst_flag;

#ifdef _DEBUG_INTERFACE_
	// Process commands from PC
	QDebug_ProcessCommands();
#endif

	if (time_to_measure_touch) {
		time_to_measure_touch = false;

		do {
			status_flag = qt_measure_sensors( current_time_ms_touch );
			burst_flag = status_flag & QTLIB_BURST_AGAIN;

#ifdef _DEBUG_INTERFACE_
			QDebug_SendData(status_flag);
#endif
		/*
		 * Time-critical host application code should be placed here since
		 * a full burst cycle may delay critical task in the main application
		 */

		} while(burst_flag);

#ifdef _DEBUG_INTERFACE_
		// Process commands from PC
		QDebug_ProcessCommands();
#endif
	}
	if(GET_SENSOR_STATE(0)) {
		return 0;
	} else {
		return 1;
	}
}
