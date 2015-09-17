/**
 * \file
 *
 * \brief UC3-L0 Xplained demo touch handler
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
#include "compiler.h"
#include "board.h"
#include "gpio.h"
#include "sysclk.h"
#include "scif_uc3l.h"
#include "tc.h"
#include "pwma.h"
#include "print_funcs.h"
#include "touch_config_at32uc3l.h"
#include "touch_api_at32uc3l.h"
#include "uc3-l0_xplained_demo.h"

#if DEF_TOUCH_QDEBUG_ENABLE
# include "QDebug_at32uc3l.h"
#endif

#define TOUCH_MEASUREMENT_TC             (&AVR32_TC0)
#define TOUCH_MEASUREMENT_TC_IRQ_GROUP   AVR32_TC0_IRQ_GROUP
#define TOUCH_MEASUREMENT_TC_IRQ         AVR32_TC0_IRQ0
#define TOUCH_MEASUREMENT_TC_CHANNEL     0
#define TOUCH_MEASUREMENT_PERIOD_MS      25

static void touch_qm_measurement_complete_callback(touch_measure_data_t
		*measured_data);
static void touch_at_status_change_callback(touch_at_status *at_status);
static void init_timer(void);
static void handle_touch_error(touch_ret_t touch_error);

/**
 * \brief Time measurement period in ms.
 *
 * Needs to be global since it is required for the QDebug module.
 */
uint16_t measurement_period_ms = TOUCH_MEASUREMENT_PERIOD_MS;

/**
 * \brief Pointer to the measured data.
 *
 * Needs to be global since it is required for the QDebug module.
 */
touch_measure_data_t *p_qm_measure_data = NULL;

/**
 * \brief Touch measurement complete flag.
 *
 * This flag is set by the touch library callback function once
 * a touch measurement is completed.
 */
bool p_qm_measurement_done = false;

/**
 * \brief Touch measurement flag.
 *
 * Indicates when a new touch measurement should be done.
 * This is set in the timer ISR so this must be volatile.
 */
static volatile bool time_to_measure_touch = false;

/**
 * \brief Current time in ms.
 *
 * This is required for the touch library.
 * The time is updated in the timer ISR so this must be volatile.
 */
static volatile uint16_t current_time_ms = 0;

/**
 * \brief Touch detection flag for autonomous touch.
 *
 * This flag will be set in the touch library callback function for
 * the autonomous touch when a touch change was detected.
 * This must be volatile since the callback will be called from an ISR.
 */
static volatile bool autonomous_qtouch_in_touch = false;

/**
 * \brief Currently selected RGB LED to manipulate.
 *
 * This enum indicates the currently selected RGB, whose brightness will
 * be altered by a touch of the board's slider control.
 */
static led_pwm_channel_t rgb_led = PWM_CHANNEL_RED;

/**
 * The below data is required for the QTouch library. Do not alter these here
 * since they are usually configured via the touch_config_at32uc3l.h
 */
//! @{

/** \brief QMatrix burst length.
 *
 * Specify the number of QMatrix burst pulses for each channel. In order to
 * disable bursting on a individual channel, a value of 1u has to be specified.
 *
 * \warning QMatrix Burst length setting recommendation.
 * When the burst length value corresponding to (X0,Y1),(X0,Y2)...(Xn,Yn) are
 * different, 1k ohm resistive drive MUST be used on all the enabled Y lines by
 * setting the corresponding bits in the CSARES register.
 *
 * When the burst length value corresponding to (X0,Y1),(X0,Y2)...(Xn,Yn) are
 * same or set to 0x01(disabled), enabling the 1k ohm resistive drive on Y
 * lines is optional.
 *
 * \note A value of 0u will be same as specifying a burst length value of 255u
 * pulses. Range: 1u to 255u.
 *
 * \note Once the Touch Library has been initialized for QMatrix method using
 * the touch_qm_sensors_init API, a new qm_burst_length[x] value of a Touch
 * channel MUST be updated only using the touch_qm_channel_update_burstlen API.
 * It is recommended to have qm_burst_length array as global variable as the
 * Touch Library updates this array when the touch_qm_channel_update_burstlen
 * API is called.
 */
uint8_t qm_burst_length[QM_NUM_CHANNELS] = {
	245, // X0, Y3
	140, // X1, Y3
	140, // X2, Y3
	255, // X3, Y3
};

/** \brief QMatrix Data block provided as input to the QTouch library. */
static uint8_t qm_data_blk[PRIV_QM_DATA_BLK_SIZE];

/** \brief QMatrix configuration input. */
static touch_qm_config_t qm_config = {
	.num_channels = QM_NUM_CHANNELS,
	.num_sensors = QM_NUM_SENSORS,
	.num_rotors_and_sliders = QM_NUM_ROTORS_SLIDERS,
	.num_x_lines = QM_NUM_X_LINES,
	.num_y_lines = QM_NUM_Y_LINES,
	.num_x_sp = PRIV_QM_NUM_X_SENSE_PAIRS,
	.bl_write_count = PRIV_QM_BURST_LENGTH_WRITE_COUNT,
	.pin = {
		.x_lines = QM_X_PINS_SELECTED,
		.y_yk_lines = QM_Y_PAIRS_SELECTED,
		.smp_dis_pin = QM_SMP_DIS_PIN_OPTION,
		.vdiv_pin = QM_VDIV_PIN_OPTION,
	},
	.reg = {
		.div = QM_CAT_CLK_DIV,
		.chlen = QM_CHLEN,
		.selen = QM_SELEN,
		.dishift = QM_DISHIFT,
		.sync = QM_ENABLE_EXTERNAL_SYNC,
		.spread = QM_ENABLE_SPREAD_SPECTRUM,
		.dilen = QM_DILEN,
		.max = QM_MAX_ACQ_COUNT,
		.acctrl = 1u,
		.consen = QM_CONSEN,
		.cxdilen = QM_CXDILEN,
		.synctim = QM_SYNC_TIM,
		.fsources = 0u,
		.glen = 1u,
		.intvrefsel = QM_INTVREFSEL,
		.intrefsel = QM_INTREFSEL,
		.trim = 0u,
		.sources = 0xFF,
		.shival0 = 0,
		.shival1 = 0,
		.shival2 = 0,
		.shival3 = 0,
		.shival4 = 0,
		.shival5 = 0,
		.shival6 = 0,
		.shival7 = 0,
	},
	.global_param = {
		.di = QM_DI,
		.neg_drift_rate = QM_NEG_DRIFT_RATE,
		.pos_drift_rate = QM_POS_DRIFT_RATE,
		.max_on_duration = QM_MAX_ON_DURATION,
		.drift_hold_time = QM_DRIFT_HOLD_TIME,
		.pos_recal_delay = QM_POS_RECAL_DELAY,
		.recal_threshold = QM_RECAL_THRESHOLD,
	},
	.p_data_blk = qm_data_blk,
	.buffer_size = PRIV_QM_DATA_BLK_SIZE,
	.p_burst_length = qm_burst_length,
	.filter_callback = QM_FILTER_CALLBACK,
};

/** \brief Autonomous Touch configuration input. */
static touch_at_config_t at_config = {
	.reg = {
		.div = AT_CAT_CLK_DIV,
		.chlen = AT_CHLEN,
		.selen = AT_SELEN,
		.dishift = AT_DISHIFT,
		.sync = AT_ENABLE_EXTERNAL_SYNC,
		.spread = AT_ENABLE_SPREAD_SPECTRUM,
		.dilen = AT_DILEN,
		.max = AT_MAX_ACQ_COUNT,
		.at_param = {
			  .filter = AT_FILTER,
			  .outsens = AT_OUTSENS,
			  .sense = AT_SENSE,
			  .pthr = AT_PTHR,
			  .pdrift = AT_PDRIFT,
			  .ndrift = AT_NDRIFT,
		},
	},
	.pin = {
		.atsp = AT_SP_SELECTED,
	},
};

/** \brief General touch configuration values. */
static touch_general_config_t touch_common_config = {
	.sync_pin = TOUCH_SYNC_PIN_OPTION,
	.maxdev = TOUCH_SPREAD_SPECTRUM_MAX_DEV,
	.csares = TOUCH_CSARES,
	.csbres = TOUCH_CSBRES,
};

/** \brief Main touch library configuration options. */
static touch_config_t touch_config = {
	.p_qm_config = &qm_config,
	.p_at_config = &at_config,
	.p_qta_config = NULL,
	.p_qtb_config = NULL,
	.p_general_config = &touch_common_config,
};

/** \brief Touch DMA configuration options. */
static touch_qm_dma_t qm_dma = {
	.dma_ch1 = QM_DMA_CHANNEL_0,
	.dma_ch2 = QM_DMA_CHANNEL_1,
};
//! @}

/**
 * \brief QMatrix measurement complete callback.
 *
 * This function will be called from the touch library each time a measurement
 * is completed.
 *
 * \param measured_data Pointer to the measured data.
 */
static void touch_qm_measurement_complete_callback(touch_measure_data_t
		*measured_data)
{
	p_qm_measure_data = measured_data;
	p_qm_measurement_done = true;
}

/**
 * \brief Callback for autonomous touch status changes.
 *
 * \param at_status Pointer to autonomous touch status data.
 */
static void touch_at_status_change_callback(touch_at_status *at_status)
{
	if (at_status->status_change == IN_TOUCH) {
		// QButton currently being touched
		autonomous_qtouch_in_touch = true;
		LED_On(LED1);

		switch (rgb_led) {
		case PWM_CHANNEL_RED:
			rgb_led = PWM_CHANNEL_BLUE;
			break;

		case PWM_CHANNEL_BLUE:
			rgb_led = PWM_CHANNEL_GREEN;
			break;

		case PWM_CHANNEL_GREEN:
			rgb_led = PWM_CHANNEL_RED;
			break;

		default:
			break;
		}
	}
	else {
		// QButton currently not being touched
		autonomous_qtouch_in_touch = false;
		LED_Off(LED1);
	}
}

/**
 * \brief Touch measurement timer interrupt.
 *
 * Initiates periodically touch measurements.
 * Triggers every 1ms and will set the flag time_to_measure_touch every
 * TOUCH_MEASUREMENT_PERIOD_MS.
 */
ISR(tc_irq, TOUCH_MEASUREMENT_TC_IRQ_GROUP, 1)
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
 * The touch library needs this as time-base.
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
		//! RC compare effect on TIOA.
		.acpc     = TC_EVT_EFFECT_NOOP,
		//! RA compare effect on TIOA.
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
		.burst    = 0u,
		//! Clock inversion selection.
		.clki     = 0u,
		//! Internal source clock 3 (fPBA / 8).
		.tcclks   = TC_CLOCK_SOURCE_TC3,
	};

	const tc_interrupt_t tc_interrupt =
	{
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
	tc_init_waveform(tc, &waveform_options);

	// set the compare triggers - 1ms interval (using timer prescaler of 8).
	tc_write_rc(tc, TOUCH_MEASUREMENT_TC_CHANNEL,
			((sysclk_get_pba_hz() / 8) / 1000));

	tc_configure_interrupts(tc, TOUCH_MEASUREMENT_TC_CHANNEL,
			&tc_interrupt);
	tc_start(tc, TOUCH_MEASUREMENT_TC_CHANNEL);
}

/**
 * \brief handles errors during touch acquisition in the touch library.
 *
 * Write debug messages to the debug USART to indicate the problem that occurred.
 */
void handle_touch_error(touch_ret_t touch_error)
{
	switch (touch_error) {
	case TOUCH_ACQ_INCOMPLETE:
		print_dbg("ACQ_INCOMPLETE\r\n");
		break;

	case TOUCH_INVALID_INPUT_PARAM:
		print_dbg("INVALID_INPUT_PARAM\r\n");
		break;

	case TOUCH_INVALID_LIB_STATE:
		print_dbg("INVALID_LIB_STATE\r\n");
		break;

	case TOUCH_INVALID_QM_CONFIG_PARAM:
		print_dbg("INVALID_QM_CONFIG_PARAM\r\n");
		break;

	case TOUCH_INVALID_AT_CONFIG_PARAM:
		print_dbg("INVALID_AT_CONFIG_PARAM\r\n");
		break;

	default:
		print_dbg("Undefined error: ");
		print_dbg_hex(touch_error);
		break;
	}
}

/**
 * \brief Initializes the touch library ready for acquisition of touch data.
 *
 * Sets up all required hardware and initializes the touch library.
 */
void init_touch(void)
{
	touch_ret_t ret;
	sensor_id_t sensor_id;

	sysclk_enable_hsb_module(SYSCLK_EVENT);

	sysclk_enable_peripheral_clock(&AVR32_TC0);
	sysclk_enable_peripheral_clock(&AVR32_CAT);
	sysclk_enable_peripheral_clock(&AVR32_ACIFB);
	sysclk_enable_peripheral_clock(&AVR32_PDCA);


#if defined(__GNUC__)
	Disable_global_interrupt();
	INTC_register_interrupt(&touch_acq_done_irq, AVR32_CAT_IRQ,
			AVR32_INTC_INT0);
	Enable_global_interrupt();
#endif

	ret = touch_at_sensor_init(&touch_config);
	if (ret != TOUCH_SUCCESS) {
		handle_touch_error(ret);
	}

	scif_gc_setup(AVR32_SCIF_GCLK_CAT, SCIF_GCCTRL_DFLL0,
			AVR32_GC_DIV_CLOCK, QM_GCLK_CAT_DIV);
	scif_gc_enable(AVR32_SCIF_GCLK_CAT);

	init_timer();

	ret = touch_qm_sensors_init(&touch_config);
	if (ret != TOUCH_SUCCESS) {
		handle_touch_error(ret);
	}

	// Either we use the four sensors as slider or as keys
#if defined(USE_QTOUCH_KEYS)
	ret = touch_qm_sensor_config(SENSOR_TYPE_KEY, 0, 0, NO_AKS_GROUP, 25,
			HYST_6_25, RES_1_BIT, 0, &sensor_id);
	if (ret != TOUCH_SUCCESS) {
		handle_touch_error(ret);
	}

	ret = touch_qm_sensor_config(SENSOR_TYPE_KEY, 1, 1, NO_AKS_GROUP, 25,
			HYST_6_25, RES_1_BIT, 0, &sensor_id);
	if (ret != TOUCH_SUCCESS) {
		handle_touch_error(ret);
	}

	ret = touch_qm_sensor_config(SENSOR_TYPE_KEY, 2, 2, NO_AKS_GROUP, 25,
			HYST_6_25, RES_1_BIT, 0, &sensor_id);
	if (ret != TOUCH_SUCCESS) {
		handle_touch_error(ret);
	}

	ret = touch_qm_sensor_config(SENSOR_TYPE_KEY, 3, 3, NO_AKS_GROUP, 25,
			HYST_6_25, RES_1_BIT, 0, &sensor_id);
	if (ret != TOUCH_SUCCESS) {
		handle_touch_error(ret);
	}
#else
	ret = touch_qm_sensor_config(SENSOR_TYPE_SLIDER, 0, 3, NO_AKS_GROUP, 15,
			HYST_12_5, RES_8_BIT, 0, &sensor_id);
	if (ret != TOUCH_SUCCESS) {
		handle_touch_error(ret);
	}
#endif

	ret = touch_qm_sensors_calibrate();
	if (ret != TOUCH_SUCCESS) {
		handle_touch_error(ret);
	}

#if DEF_TOUCH_QDEBUG_ENABLE == 1
	QDebug_Init();
#endif

	ret = touch_at_sensor_enable(touch_at_status_change_callback);
	if (ret != TOUCH_SUCCESS) {
		handle_touch_error(ret);
	}
}

void touch_handler(void)
{
	/*
	 * According to the documentation, this function should be called as
	 * frequently as possible. This function will process any touch library
	 * events.
	 */
	touch_event_dispatcher();

	if (time_to_measure_touch) {
		time_to_measure_touch = false;

		touch_ret_t ret =
			touch_qm_sensors_start_acquisition(current_time_ms,
					&qm_dma, NORMAL_ACQ_MODE,
					touch_qm_measurement_complete_callback);
		if (ret != TOUCH_SUCCESS) {
			handle_touch_error(ret);
		}
	}

	if (p_qm_measurement_done) {
#if defined(USE_QTOUCH_KEYS)
		uint8_t current_button_value = 0;
#endif
		p_qm_measurement_done = false;

#if DEF_TOUCH_QDEBUG_ENABLE == 1
		/*
		 * Process any incoming commands e.g. from AVR Studio via
		 * serial link
		 */
		QDebug_ProcessCommands();

		/* Sends the touch debug data to QTouch Studio */
		QDebug_SendData(p_qm_measure_data->acq_status);
#endif
		// Either we use the four sensors as slider or as keys
#if defined(USE_QTOUCH_KEYS)
		if (p_qm_measure_data->p_sensor_states[0] & 0x01) {
			current_button_value = (0xFF / 4) * 1;
		}

		if (p_qm_measure_data->p_sensor_states[0] & 0x02) {
			current_button_value = (0xFF / 4) * 2;
		}

		if (p_qm_measure_data->p_sensor_states[0] & 0x04) {
			current_button_value = (0xFF / 4) * 3;
		}

		if (p_qm_measure_data->p_sensor_states[0] & 0x08) {
			current_button_value = (0xFF / 4) * 4;
		}

		if (current_button_value) {
			pwma_set_channels_value(&AVR32_PWMA, rgb_led,
					current_button_value);
		}
#else
		/*
		 * Adjust selected RGB LED's brightness according to the slider
		 * position
		 */
		if (p_qm_measure_data->p_sensor_states[0] & 0x01) {
			uint8_t current_slider_value =
				p_qm_measure_data->p_rotor_slider_values[0];

			pwma_set_channels_value(&AVR32_PWMA, rgb_led,
					current_slider_value);
		}
#endif
	}
}
