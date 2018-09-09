/**
 * \file
 *
 * \brief QTouch Sensor interface
 *
 * Copyright (c) 2010-2018 Microchip Technology Inc. and its subsidiaries.
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

#include <asf.h>
#include "touch_api.h"

#ifdef _DEBUG_INTERFACE_
/* Include files for QTouch Studio integration */
#  include "QDebug.h"
#  include "QDebugTransport.h"
#endif

#define GET_SENSOR_STATE(SENSOR_NUMBER) qt_measure_data.qt_touch_status. \
	sensor_states[(SENSOR_NUMBER / 8)] & (1 << (SENSOR_NUMBER % 8))

/** Timer period in msec. */
uint16_t qt_measurement_period_msec = 25;

/** This configuration data structure parameters if needs to be changed will be
 *  changed in the qt_set_parameters function */
extern qt_touch_lib_config_data_t qt_config_data;

/** Touch output - measurement data */
extern qt_touch_lib_measure_data_t qt_measure_data;

/** Get sensor delta values */
extern int16_t qt_get_sensor_delta( uint8_t sensor);

#ifdef QTOUCH_STUDIO_MASKS
extern TOUCH_DATA_T SNS_array[2][2];
extern TOUCH_DATA_T SNSK_array[2][2];
#endif

/** Output can be observed in the watch window using this pointer */
qt_touch_lib_measure_data_t *pqt_measure_data = &qt_measure_data;

/** Flag set by timer ISR when it's time to measure touch */
static volatile bool time_to_measure_touch = false;

/** Current time, set by timer ISR */
static volatile uint16_t current_time_ms_touch = 0;

/** \brief Passes default parameters from config file to touch library
 */
static void qt_set_parameters( void )
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
	/* The sensor is wired up with SNS=PF6 and SNSK=PF7
	 * When using "pin configurability" this will result in channel 0
	 * because it is the first and only channel that is used.
	 * For the standard qtouch library setup we would need to use
	 * channel 3 since we are using the last two pins on the port.
	 */
	qt_enable_key(CHANNEL_0, NO_AKS_GROUP, 10, HYST_6_25);
}

static void touch_timer_callback()
{
	time_to_measure_touch = true;
	current_time_ms_touch += qt_measurement_period_msec;
}

static void init_timer_isr( void )
{
	tc_enable(&TCD0);

	/* We divide the peripheral 2MHz clock by 2 to get 1MHz*/
	tc_write_clock_source(&TCD0, TC_CLKSEL_DIV2_gc);

	/* Set Compare A interrupt to low level */
	tc_set_cca_interrupt_level(&TCD0, TC_INT_LVL_LO);

	/* 1000 counts is 1ms at 1MHz input clock */
	tc_write_period (&TCD0, 1000 * qt_measurement_period_msec);

	/* Handling callback */
	tc_set_cca_interrupt_callback(&TCD0, touch_timer_callback);

	/* Enable CCA */
	tc_enable_cc_channels(&TCD0, TC_CCAEN);
}

/** \brief Configures touch library and measurement setup
 *
 * @pre Interrupts must be enabled in order to trigger regular measurements
 * The touch configuration requires a fixed CPU speed to work properly in this
 * case we need 2 MHz.
 */
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

	config_sensor();
	qt_init_sensing();
	qt_set_parameters();
	init_timer_isr();

	/* Set up callback function. This function is called after the library
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
	/* Process commands from PC */
	QDebug_ProcessCommands();
#endif

	if (time_to_measure_touch) {
		time_to_measure_touch = false;

		do {
			status_flag = qt_measure_sensors(current_time_ms_touch);
			burst_flag = status_flag & QTLIB_BURST_AGAIN;

#ifdef _DEBUG_INTERFACE_
			QDebug_SendData(status_flag);
#endif

			/* Time-critical host application code should be placed
			 * here since
			 * a full burst cycle may delay critical task in the
			 *main application
			 */
		} while (burst_flag);

#ifdef _DEBUG_INTERFACE_
		/* Process commands from PC */
		QDebug_ProcessCommands();
#endif
	}

	if (GET_SENSOR_STATE(0)) {
		return 0;
	} else {
		return 1;
	}
}
