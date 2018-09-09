/**
 * \file
 *
 * \brief UC3-A3 Xplained Application Touch functions
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
#include "sysclk.h"
#include "tc.h"
#include "uc3-a3_xplained_demo.h"

#define _DEBUG_INTERFACE_
#include "touch_api.h"

#ifdef _DEBUG_INTERFACE_
# include "QDebug.h"
# include "QDebugTransport.h"
#endif

#define TOUCH_MEASUREMENT_TC             (&AVR32_TC1)
#define TOUCH_MEASUREMENT_TC_IRQ_GROUP   AVR32_TC1_IRQ_GROUP
#define TOUCH_MEASUREMENT_TC_IRQ         AVR32_TC1_IRQ0
#define TOUCH_MEASUREMENT_TC_CHANNEL     0
#define TOUCH_MEASUREMENT_PERIOD_MS      25

/** \brief Time measurement period in ms.
 *
 * Needs to be global since it is required for the QDebug module.
 */
uint16_t measurement_period_ms = TOUCH_MEASUREMENT_PERIOD_MS;

/** \brief Touch measurement flag.
 *
 * Indicates when a new touch measurement should be done.
 * This is set in the timer ISR so this must be volatile.
 */
static volatile bool time_to_measure_touch = true;

/** \brief Current time in ms.
 *
 * This is required for the touch library.
 * The time is updated in the timer ISR so this must be volatile.
 */
static volatile uint16_t current_time_ms = 0;

/**
 * \brief Touch measurement timer interrupt.
 *
 * Initiates periodically touch measurements.
 * Triggers every 1ms and will set the flag time_to_measure_touch every
 * TOUCH_MEASUREMENT_PERIOD_MS ms.
 */
ISR(tc_irq, TOUCH_MEASUREMENT_TC_IRQ_GROUP, AVR32_INTC_INT0)
{
	tc_read_sr(TOUCH_MEASUREMENT_TC, TOUCH_MEASUREMENT_TC_CHANNEL);
	current_time_ms++;

	if ((current_time_ms % measurement_period_ms) == 0) {
		time_to_measure_touch = true;
	}
}

/**
 * \brief Initializes the touch measurement timer.
 *
 * We need a timer that triggers approx. every millisecond.
 * The touch library seems to need this as time-base.
 */
static void init_timer(void)
{
	volatile avr32_tc_t *tc = TOUCH_MEASUREMENT_TC;

	const tc_waveform_opt_t waveform_options = {
		.channel  = TOUCH_MEASUREMENT_TC_CHANNEL,
		//! Software trigger effect on TIOB.
		.bswtrg   = TC_EVT_EFFECT_NOOP,
		//! External event effect on TIOB.
		.beevt    = TC_EVT_EFFECT_NOOP,
		//! RC compare effect on TIOB.
		.bcpc     = TC_EVT_EFFECT_NOOP,
		//! RB compare effect on TIOB.
		.bcpb     = TC_EVT_EFFECT_NOOP,
		//! Software trigger effect on TIOA.
		.aswtrg   = TC_EVT_EFFECT_NOOP,
		//! External event effect on TIOA.
		.aeevt    = TC_EVT_EFFECT_NOOP,
		//! RC compare effect on TIOA: toggle.
		.acpc     = TC_EVT_EFFECT_NOOP,
		//! RA compare effect on TIOA: toggle.
		.acpa     = TC_EVT_EFFECT_NOOP,
		//! Waveform selection
		.wavsel   = TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER,
		//! External event trigger enable.
		.enetrg   = 0,
		//! External event selection.
		.eevt     = 0,
		//! External event edge selection.
		.eevtedg  = TC_SEL_NO_EDGE,
		//! Counter disable when RC compare.
		.cpcdis   = 0,
		//! Counter clock stopped with RC compare.
		.cpcstop  = 0,
		//! Burst signal selection.
		.burst    = 0,
		//! Clock inversion selection.
		.clki     = 0,
		//! Internal source clock 3 (fPBA / 8).
		.tcclks   = TC_CLOCK_SOURCE_TC3
	};

	const tc_interrupt_t tc_interrupt = {
		.etrgs = 0,
		.ldrbs = 0,
		.ldras = 0,
		.cpcs  = 1,
		.cpbs  = 0,
		.cpas  = 0,
		.lovrs = 0,
		.covfs = 0,
	};

#if (defined __GNUC__)
	Disable_global_interrupt();
	INTC_register_interrupt(&tc_irq, TOUCH_MEASUREMENT_TC_IRQ,
			AVR32_INTC_INT0);
	Enable_global_interrupt();
#endif

	/* initialize the timer/counter. */
	sysclk_enable_peripheral_clock(&AVR32_TC1);
	tc_init_waveform(tc, &waveform_options);

	/* set the compare triggers. */
	tc_write_rc(tc, TOUCH_MEASUREMENT_TC_CHANNEL,
			(sysclk_get_pba_hz() / 8) / 1000);

	tc_configure_interrupts(tc, TOUCH_MEASUREMENT_TC_CHANNEL,
			&tc_interrupt);
	tc_start(tc, TOUCH_MEASUREMENT_TC_CHANNEL);
}

/**
 * \brief Initializes the touch library configuration.
 *
 * Sets the correct configuration for the QTouch library.
 */
static void qt_set_parameters(void)
{
	/* This will be modified by the user to different values. */
	qt_config_data.qt_di              = DEF_QT_DI;
	qt_config_data.qt_neg_drift_rate  = DEF_QT_NEG_DRIFT_RATE;
	qt_config_data.qt_pos_drift_rate  = DEF_QT_POS_DRIFT_RATE;
	qt_config_data.qt_max_on_duration = DEF_QT_MAX_ON_DURATION;
	qt_config_data.qt_drift_hold_time = DEF_QT_DRIFT_HOLD_TIME;
	qt_config_data.qt_recal_threshold = DEF_QT_RECAL_THRESHOLD;
	qt_config_data.qt_pos_recal_delay = DEF_QT_POS_RECAL_DELAY;

	/*
	 * This function is called after the library has made capacitive
	 * measurements, but before it has processed them. The user can use
	 * this hook to apply filter functions to the measured signal
	 * values. (Possibly to fix sensor layout faults).
	 */
	qt_filter_callback = NULL;
}

/**
 * \brief Initialize touch sensors
 *
 * The touch channels are connected on the GPIO controller 4 which is a part of
 * port X
 *
 * Touch Button:
 * GPIO
 * 98           SNSK1           CHANNEL1_SNS
 * 99           SNS1            CHANNEL1_SNSK
 *
 * Touch Slider:
 * 100          SNS0	        CHANNEL2_SNS
 * 101          SNSK0           CHANNEL2_SNSK
 * 102          SNS1            CHANNEL3_SNS
 * 103          SNSK1		CHANNEL3_SNSK
 * 104          SNS2            CHANNEL4_SNS
 * 105          SNSK2           CHANNEL4_SNSK
 */
void touch_init(void)
{
	/*
	 * Reset sensing is only needed when doing re-initialization during
	 * run-time
	 */
	// qt_reset_sensing();
	qt_enable_key(CHANNEL_1, NO_AKS_GROUP, 30u, HYST_12_5);

	/*
	 * Position hysteresis is not used in QTouch slider so this value will
	 * be ignored
	 */
	qt_enable_slider(CHANNEL_2, CHANNEL_4, NO_AKS_GROUP, 20u, HYST_12_5,
			RES_8_BIT, 0);

	qt_init_sensing();
	qt_set_parameters();

#ifdef _DEBUG_INTERFACE_
	sysclk_enable_peripheral_clock(&AVR32_SPI0);

	/* Initialize debug protocol */
	QDebug_Init();

	/* Process commands from PC */
	QDebug_ProcessCommands();
#endif

	init_timer();
}

/**
 * \brief Measure touch sensors and do appropriate action
 */
void touch_handler(void)
{
	uint16_t qt_measurement_status;

	if (time_to_measure_touch) {
		time_to_measure_touch = false;

		do {
			qt_measurement_status =
				qt_measure_sensors(current_time_ms);

#ifdef _DEBUG_INTERFACE_
			QDebug_SendData(qt_measurement_status);
#endif
		} while (qt_measurement_status & QTLIB_BURST_AGAIN);

#ifdef _DEBUG_INTERFACE_
		QDebug_ProcessCommands();
#endif

		if (qt_measure_data.qt_touch_status.sensor_states[0] & 0x01) {
			LED_On(LED1);
		}
		else {
			LED_Off(LED1);
		}

		if (qt_measure_data.qt_touch_status.sensor_states[0] & 0x02) {
			uint8_t slider_pos =
				qt_measure_data.qt_touch_status.rotor_slider_values[0];
			tc_write_rb(&AVR32_TC0, 0, (slider_pos + 1));

			LED_On(LED2);
		}
		else {
			LED_Off(LED2);
		}
	}
}
