/**
 * \file
 *
 * \brief Touch button driver
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
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <compiler.h>
#include <tc.h>
#include <touch_api.h>
#include "touch.h"

/* Includes for QTouch Studio integration */
#ifdef _DEBUG_INTERFACE_
# include "QDebug.h"
# include "QDebugTransport.h"
#endif

/** Convenience macro for getting state of touch sensor*/
#define TOUCH_GET_SENSOR_STATE(SENSOR_NUMBER) ( \
	qt_measure_data.qt_touch_status.sensor_states[(SENSOR_NUMBER/8)] \
			& (1 << (SENSOR_NUMBER % 8)) \
)
/** Timer/counter for timing touch measurements */
#define TOUCH_TC           TCD0
/** Period in milliseconds between each touch measurement */
#define TOUCH_TC_PERIOD_MS 25

/** QTouch configuration struct */
extern qt_touch_lib_config_data_t qt_config_data;
/** QTouch measurement data struct */
extern qt_touch_lib_measure_data_t qt_measure_data;
/** QTouch measurement data pointer for debug purposes */
qt_touch_lib_measure_data_t *const pqt_measure_data = &qt_measure_data;
#ifdef QTOUCH_STUDIO_MASKS
extern TOUCH_DATA_T SNS_array[2][2];
extern TOUCH_DATA_T SNSK_array[2][2];
#endif

/** Flag set by timer period callback to trigger touch measurements */
static volatile bool time_to_measure_touch = false;
/** Current time, set by timer period callback */
static volatile uint16_t current_time_ms_touch = 0;

/**
 * \brief Pass default parameters to QTouch library
 */
static void touch_set_parameters(void)
{
	qt_config_data.qt_di              = DEF_QT_DI;
	qt_config_data.qt_neg_drift_rate  = DEF_QT_NEG_DRIFT_RATE;
	qt_config_data.qt_pos_drift_rate  = DEF_QT_POS_DRIFT_RATE;
	qt_config_data.qt_max_on_duration = DEF_QT_MAX_ON_DURATION;
	qt_config_data.qt_drift_hold_time = DEF_QT_DRIFT_HOLD_TIME;
	qt_config_data.qt_recal_threshold = DEF_QT_RECAL_THRESHOLD;
	qt_config_data.qt_pos_recal_delay = DEF_QT_POS_RECAL_DELAY;
}

/**
 * \brief Configure the touch sensor
 */
static void touch_config_sensor(void)
{
	 qt_enable_key(CHANNEL_0, NO_AKS_GROUP, 10, HYST_6_25);
}

/**
 * \brief Callback function for timer period interrupt
 */
static void touch_timer_period_handler(void)
{
	time_to_measure_touch = true;
	current_time_ms_touch += TOUCH_TC_PERIOD_MS;
}

/**
 * \brief Configure timer interrupts for touch measurements
 */
static void touch_init_timer_isr(void)
{
	tc_enable(&TOUCH_TC);
	tc_set_overflow_interrupt_callback(&TOUCH_TC, &touch_timer_period_handler);
	tc_set_resolution(&TOUCH_TC, (uint32_t)1000000);
	tc_write_period(&TOUCH_TC, (tc_get_resolution(&TOUCH_TC)
			* TOUCH_TC_PERIOD_MS) / 1000);
	tc_set_overflow_interrupt_level(&TOUCH_TC, TC_INT_LVL_LO);
}

/**
 * \brief Configure QTouch library and measurement setup
 */
void touch_init(void)
{

#ifdef QTOUCH_STUDIO_MASKS
	SNS_array[0][0]= 0x40;
	SNS_array[0][1]= 0x0;
	SNS_array[1][0]= 0x0;
	SNS_array[1][1]= 0x0;

	SNSK_array[0][0]= 0x80;
	SNSK_array[0][1]= 0x0;
	SNSK_array[1][0]= 0x0;
	SNSK_array[1][1]= 0x0;
#endif

	touch_config_sensor();
	qt_init_sensing();
	touch_set_parameters();
	touch_init_timer_isr();

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

/**
 * \brief Check if user is pressing the touch key
 *
 * \retval true if key is pressed.
 * \retval false if key is not pressed.
 */
bool touch_key_is_pressed(void)
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
		/* Time-critical host application code should be placed here since
		 * a full burst cycle may delay critical task in the main application
		 */

		} while (burst_flag);

#ifdef _DEBUG_INTERFACE_
		/* Process commands from PC */
		QDebug_ProcessCommands();
#endif
	}

	if (TOUCH_GET_SENSOR_STATE(0)) {
		return 0;
	} else {
		return 1;
	}
}
