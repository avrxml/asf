/**
 * \file *********************************************************************
 *
 * \brief AVR477QTouch buttons handling implementation.
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
 */

/*
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */
#ifdef AVR477_QTOUCH

#include "touch_api.h"
#include "Avr477QTouch.h"
#include "memorymap.h"

#include <asf.h>

#if defined(__ICCAVR__)
#include "ioavr.h"
#include "inavr.h"
#elif defined(__GNUC__)
#include "avr/io.h"
#include <avr/interrupt.h>
#endif

/* #include "memorymap.h" */

/******************************************************************************
*                   Definitions section
******************************************************************************/
#define QT_MEASUREMENT_PERIOD_MSEC   15u

/******************************************************************************
*                   Types section
******************************************************************************/
typedef struct _Button_t {
	bool pressed;
	BSP_ButtonEvent_t event;
	uint8_t interval;
	uint8_t threshold;
} Button_t;

/******************************************************************************
*                   Prototypes section
******************************************************************************/
/*  initialise host app, pins, watchdog, etc    */
static void init_system( void );

/*  configure timer ISR to fire regularly   */
static void init_timer_isr( void );

/*  Assign the parameters values to global configuration parameter structure
 *   */
static void qt_set_parameters( void );

/*  Configure the sensors */
static void config_sensors(void);

static void burst_len_config( void );

/* qt filter callback function */
static void qt_avr477_filter_cb(void);

/* Configure the buzzer */
static void buzzer_init(void);

/* LED Control */
static inline void led0_toggle(void);
static inline void led1_toggle(void);

uint8_t altFunc;
uint8_t touchlink_flag = false;

extern uint8_t qt_burst_lengths[QT_NUM_CHANNELS];

extern uint8_t qt_button;
extern uint8_t qt_event;

void touch_interrupt_call_back(void);

/******************************************************************************
*                   Local variables
******************************************************************************/
/* filter for channel signals */
static uint16_t filter[QT_NUM_CHANNELS][8];

/* Timer period in msec. */
uint16_t qt_measurement_period_msec = QT_MEASUREMENT_PERIOD_MSEC;

static volatile uint16_t status_flag = 0u;
static volatile uint16_t burst_flag = 0u;

/* flag set by timer ISR when it's time to measure touch */
static volatile uint8_t time_to_measure_touch = 0u;
/* current time, set by timer ISR */
volatile uint16_t current_time_ms_touch = 0u;

/******************************************************************************
*                   Extern variables
******************************************************************************/

/* This configuration data structure parameters if needs to be changed will be
 * changed in the qt_set_parameters function */
extern qt_touch_lib_config_data_t qt_config_data;

/* This configuration data structure parameters if needs to be changed will be
 * changed in the qt_set_parameters function */
extern qt_touch_lib_config_data_t qt_config_data;

#ifdef _QMATRIX_

/**
 * Info stored for each x line.
 *
 * For each X line, enter the port and pin it is on.
 * For instance, if X3 is on PB1, the 4th entry would be
 * FILL_OUT_X_LINE_INFO( 1,1 ), and PORT_X_1 is B
 */
#define FILL_OUT_X_LINE_INFO( port_num,	\
			x_bit ) {JOIN(burst_qm_, \
						 port_num), \
				 (TOUCH_DATA_T)(1u << x_bit), \
				 BURST_MASK_X_PORT_ ## port_num  }

/**
 * Info stored for each y line.
 *
 * For each Y line, enter the pin it is on.
 * For instance, if Y2 is on PA5 and PF5, the 3th entry would be
 *    FILL_OUT_Y_LINE_INFO( 5 )
 * NOTE: 1. The PORTs for YA and YB on which Y lines are going to
 *          be needs to be defined as PORT_YA and PORT_YB.
 *
 * Example: PORT_YA=A and PORT_YB=F in the case above.
 */
#define FILL_OUT_Y_LINE_INFO( bit ) { bit, (TOUCH_DATA_T)(1u << bit) }

/**
 * Info stored for each ya line.
 *
 * For each YA line, enter the pin it is on.
 * For instance, if Y2A is on PA5 , the 3th entry would be
 *    FILL_OUT_YA_LINE_INFO( 5 )
 * NOTE: 1. The PORTs for YA and YB on which Y lines are going to
 *          be needs to be defined as PORT_YA and PORT_YB.
 *
 * Example: PORT_YA=A in the case above.
 */
#define FILL_OUT_YA_LINE_INFO( bit ) { bit, (uint8_t)(1u << bit) }

/**
 * Info stored for each yb line.
 *
 * For each YB line, enter the pin it is on.
 * For instance, if Y2B is on PF3, the 3th entry would be
 *    FILL_OUT_YB_LINE_INFO( 3 )
 * NOTE: 1. The PORTs for YA and YB on which Y lines are going to
 *          be needs to be defined as PORT_YA and PORT_YB.
 *
 * Example: PORT_YB=F in the case above.
 */
#define FILL_OUT_YB_LINE_INFO( bit ) { bit, (uint8_t)(1u << bit) }

#ifndef MAKE_BUILD
/* ! Check necessary definitions. */
#if (NUM_X_PORTS == 1)
#if !defined(PORT_X_1) || !defined(PORT_YA) || !defined(PORT_YB) || \
	!defined(QT_DELAY_CYCLES) || !defined(PORT_SMP) || \
	!defined(SMP_BIT)
#error \
	Need to define PORT_X_1, PORT_YA,PORT_YB,PORT_SMP,SMP_BIT and QT_DELAY_CYCLES before including the header file.
#endif
#elif (NUM_X_PORTS == 2)
#if !defined(PORT_X_1) || !defined(PORT_X_2) || !defined(PORT_YA) || \
	!defined(PORT_YB) || !defined(QT_DELAY_CYCLES) || \
	!defined(PORT_SMP) || !defined(SMP_BIT)
#error \
	Need to define PORT_X_1,PORT_X_2, PORT_YA,PORT_YB,PORT_SMP,SMP_BIT and QT_DELAY_CYCLES before including the header file.
#endif
#elif (NUM_X_PORTS == 3)
#if !defined(PORT_X_1) || !defined(PORT_X_2) || !defined(PORT_X_3) || \
	!defined(PORT_YA) || !defined(PORT_YB) || \
	!defined(QT_DELAY_CYCLES) || !defined(PORT_SMP) || \
	!defined(SMP_BIT)
#error \
	Need to define PORT_X_1,PORT_X_2,PORT_X_3, PORT_YA,PORT_YB,PORT_SMP,SMP_BIT and QT_DELAY_CYCLES before including the header file.
#endif
#endif
#ifdef _ROTOR_SLIDER_
#if (QT_NUM_CHANNELS == 4u)
#if !(QT_MAX_NUM_ROTORS_SLIDERS == 1u)
#error QT_MAX_NUM_ROTORS_SLIDERS should be define as 1
#endif
#elif (QT_NUM_CHANNELS == 8u)
#if !(QT_MAX_NUM_ROTORS_SLIDERS == 2u)
#error QT_MAX_NUM_ROTORS_SLIDERS should be define as 2
#endif
#elif (QT_NUM_CHANNELS == 16u)
#if !((QT_MAX_NUM_ROTORS_SLIDERS == 2u) || (QT_MAX_NUM_ROTORS_SLIDERS == 4u))
#error \
	QT_MAX_NUM_ROTORS_SLIDERS should be define as 2 or 4 based on library selected
#endif
#elif (QT_NUM_CHANNELS == 24u)
#if !((QT_MAX_NUM_ROTORS_SLIDERS == 3u))
#error QT_MAX_NUM_ROTORS_SLIDERS should be define as 3
#endif
#elif (QT_NUM_CHANNELS == 32u)
#if !(QT_MAX_NUM_ROTORS_SLIDERS == 4u)
#error QT_MAX_NUM_ROTORS_SLIDERS should be define as 4 based on library selected
#endif
#elif (QT_NUM_CHANNELS == 64u)
#if !((QT_MAX_NUM_ROTORS_SLIDERS == 4u) || (QT_MAX_NUM_ROTORS_SLIDERS == 8u))
#error \
	QT_MAX_NUM_ROTORS_SLIDERS should be define as 4 or 8 based on library selected
#endif
#endif
#else
#if !(QT_MAX_NUM_ROTORS_SLIDERS == 0u)
#error \
	QT_MAX_NUM_ROTORS_SLIDERS should be define as 0 Or _ROTOR_SLIDER_ Macro Needs to be define
#endif
#endif

#endif /* ! MAKE_BUILD. */
#endif /* ! _QMATRIX_ */

#ifdef _QMATRIX_

extern y_line_info_t ya_line_info[NUM_Y_LINES];
extern y_line_info_t yb_line_info[NUM_Y_LINES];
extern x_line_info_t x_line_info[NUM_X_LINES];

/* Fill out the X-Line masks on the X- Port selected.
 * The order of X - Line numbering follows from the way the
 * X-Lines are filled as below.
 * Here, X0,X1,X2,X3,X4,X5,X6,X7 are on port-pin specified.
 * 1 - if to specify if X line is on PORT_X_1,pin on the same selected port.
 * 2 - if to specify if X line is on PORT_X_2,pin on the same selected port.
 * 3 - if to specify if X line is on PORT_X_3,pin on the same selected port.
 *
 * Note: 1. The Number entries should be based on NUM_X_LINES
 *          4 entries when NUM_X_LINES =4 and
 *          8 entries when NUM_X_LINES=8
 */

x_line_info_t x_line_info[NUM_X_LINES] = {
	FILL_OUT_X_LINE_INFO(1, 6),
	FILL_OUT_X_LINE_INFO(2, 4),
	FILL_OUT_X_LINE_INFO(2, 5),
	FILL_OUT_X_LINE_INFO(2, 6),
	FILL_OUT_X_LINE_INFO(2, 7),
};

/* Fill out the Y-Line masks on the Y- Line port selected
 * The order of Y - Line numbering follows from the way the
 * Y-Lines are filled as below
 * Here, Y0,Y1,Y2,Y3 on 0,1,2,3
 * Note: 1. The Number entries should be based on NUM_X_LINES
 *          2 entries when NUM_Y_LINES=2
 *          4 entries when NUM_Y_LINES=4
 *          8 entries when NUM_Y_LINES=8
 */
y_line_info_t ya_line_info[NUM_Y_LINES] = {
	FILL_OUT_YA_LINE_INFO(  0u ),
	FILL_OUT_YA_LINE_INFO(  1u ),
	FILL_OUT_YA_LINE_INFO(  2u ),
	FILL_OUT_YA_LINE_INFO(  3u ),
};
y_line_info_t yb_line_info[NUM_Y_LINES] = {
	FILL_OUT_YB_LINE_INFO(  0u ),
	FILL_OUT_YB_LINE_INFO(  1u ),
	FILL_OUT_YB_LINE_INFO(  2u ),
	FILL_OUT_YB_LINE_INFO(  3u ),
};
#endif /*_QMATRIX_*/

/******************************************************************************
*                   Implementations section
******************************************************************************/

/**************************************************************************//**
*  \brief Initialize QTouch.
******************************************************************************/
void BSP_InitQTouch(BSP_TouchEventHandler_t handler)
{
	/* initialise host app, pins, watchdog, etc */
	init_system();

	/* Reset touch sensing */
	qt_reset_sensing();

	/*Configure Burst Length*/
	burst_len_config();
	config_sensors();

	/* Initialise and set touch params */
	qt_init_sensing();
	qt_set_parameters();
	init_timer_isr();

	buzzer_init();

	/*  Address to pass address of user functions   */

	/*  This function is called after the library has made capacitive
	 * measurements,
	 *   but before it has processed them. The user can use this hook to
	 * apply filter
	 *   functions to the measured signal values.(Possibly to fix sensor
	 * layout faults)    */

	/* This function is also used to send signal values to simulate Accelero
	 * meter,
	 * Just for demo purpose */
	qt_filter_callback = qt_avr477_filter_cb;

	cpu_irq_enable();

	handler = handler;
}

uint32_t rotor_up;
uint32_t rotor_down;
uint32_t key_touch;
#ifdef QDEBUG
uint8_t wheel_pos1[512];
uint8_t wheel_pos2[512];
uint8_t wheel_pos3[512];
uint8_t wheel_pos4[512];
uint32_t wheel_index1;
uint32_t wheel_index2;
uint32_t wheel_index3;
uint32_t wheel_index4;
#endif

/**************************************************************************//**
*  \brief Touch task handler
******************************************************************************/
uint8_t Touch_measurement(uint8_t *button_event, uint8_t *button_state)
{
	uint8_t temp_status1;
	uint8_t temp_status2;

	uint8_t button = INVALID_KEY;
	uint8_t event = BUTTON_NO_EVENT;

	/* time_to_measure_touch = 1; //added for timing debug */

	/* After sending the data, do Touch Measurements */
	if ((time_to_measure_touch) || (burst_flag)) {
		time_to_measure_touch = 0u;

		/* Measure touch once */
		status_flag = qt_measure_sensors(current_time_ms_touch);
		burst_flag = status_flag & QTLIB_BURST_AGAIN;

		/* check whether the sensor status is changed from previous
		 * status */
		temp_status1 = memory_map.body.sensor_status[0] ^
				qt_measure_data.qt_touch_status.sensor_states[0];
		temp_status2 = memory_map.body.sensor_status[1] ^
				qt_measure_data.qt_touch_status.sensor_states[1];
		temp_status2 &= 0x03;

		/* If Sensor status is changed, do not sleep, but buzz  */
		if (temp_status1 | temp_status2) {
			event = BUTTON_PRESSED_EVENT;
			if (qt_measure_data.qt_touch_status.sensor_states[0] &
					0x01) {
				button = KEY_SW16;
			} else if (qt_measure_data.qt_touch_status.sensor_states
					[0] & 0x02) {
				button = KEY_SW09;
			} else if (qt_measure_data.qt_touch_status.sensor_states
					[0] & 0x04) {
				button = KEY_SW10;
			} else if (qt_measure_data.qt_touch_status.sensor_states
					[0] & 0x08) {
				button = KEY_SW17;
			} else if (qt_measure_data.qt_touch_status.sensor_states
					[0] & 0x10) {
				button = KEY_SW14;
			} else if (qt_measure_data.qt_touch_status.sensor_states
					[0] & 0x20) {
				button = KEY_SW11;
			} else if (qt_measure_data.qt_touch_status.sensor_states
					[0] & 0x40) {
				button = KEY_SW12;
			} else if (qt_measure_data.qt_touch_status.sensor_states
					[0] & 0x80) {
				button = KEY_SW15;
			} else {
				event = BUTTON_NO_EVENT;
			}

			if (qt_measure_data.qt_touch_status.sensor_states[1] &
					0x02) {
				button = KEY_SW13;
				event = BUTTON_PRESSED_EVENT;
#ifdef QDEBUG
				key_touch++;
#endif
			}

			if (qt_measure_data.qt_touch_status.sensor_states[1] &
					0x01) {
#ifdef QDEBUG
				wheel_pos1[wheel_index1++]
					= qt_measure_data.channel_signals
						[8];
				wheel_pos2[wheel_index2++]
					= qt_measure_data.channel_signals
						[9];
				wheel_pos3[wheel_index3++]
					= qt_measure_data.channel_signals
						[10];
				wheel_pos4[wheel_index4++]
					= qt_measure_data.channel_signals
						[11];
#endif
				if (qt_measure_data.qt_touch_status.
						rotor_slider_values[0] <
						memory_map.body.wheel_position)
				{
					button = KEY_ROT_CW;
					event = BUTTON_RELEASED_EVENT;
#ifdef QDEBUG
					rotor_up++;
#endif
				} else {
					button = KEY_ROT_CCW;
					event = BUTTON_RELEASED_EVENT;
#ifdef QDEBUG
					rotor_down++;
#endif
				}
			}

			/* Store back the current sensor status */
			temp_status1 = 0;
			temp_status2 = 0;
			memory_map.body.sensor_status[0]
				= qt_measure_data.qt_touch_status.
					sensor_states[0];
			memory_map.body.sensor_status[1]
				= qt_measure_data.qt_touch_status.
					sensor_states[1];
			memory_map.body.wheel_position
				= qt_measure_data.qt_touch_status.
					rotor_slider_values[0];

			if (event != BUTTON_NO_EVENT) {
				if (button == KEY_SW15) {
					led1_toggle();
				} else {
					led0_toggle();
				}

				*button_event = event;
				*button_state = button;
				return true;
			}

			/* Time-critical host application code goes here */
		}

		/* Non time-critical application code goes here */
	}

	return false;
}

/**
 * @brief This will fill the default threshold values in the configuration
 * data structure.But User can change the values of these parameters . This is
 * used to initialize the configuration data for processing.
 * @ingroup QTouch
 */

static void qt_set_parameters( void )
{
	qt_config_data.qt_di              = DEF_QT_DI;
	qt_config_data.qt_neg_drift_rate  = DEF_QT_NEG_DRIFT_RATE;
	qt_config_data.qt_pos_drift_rate  = DEF_QT_POS_DRIFT_RATE;
	qt_config_data.qt_max_on_duration = DEF_QT_MAX_ON_DURATION;
	qt_config_data.qt_drift_hold_time = DEF_QT_DRIFT_HOLD_TIME;
	qt_config_data.qt_recal_threshold = RECAL_100; /*DEF_QT_RECAL_THRESHOLD*/
	qt_config_data.qt_pos_recal_delay = 10u;   /*DEF_QT_POS_RECAL_DELAY*/
}

/**
 * @brief configure the individual channel burst length (Based on tuning)
 *
 */

#define DEFAULT_BURST_LENGTH    20u
#define LED_KEY_BURST_LENGTH_1  32u
#define LED_KEY_BURST_LENGTH_2  32u
#define ZERO_BURST_LENGTH       0u
#define ROTOR_BURST_LENGTH_8    40u
#define ROTOR_BURST_LENGTH_9    36u
#define ROTOR_BURST_LENGTH_10   25u
#define ROTOR_BURST_LENGTH_11   32u
static void burst_len_config( void )
{
	/* Used channels */
	qt_burst_lengths[0] = LED_KEY_BURST_LENGTH_1;
	qt_burst_lengths[1] = DEFAULT_BURST_LENGTH;
	qt_burst_lengths[2] = DEFAULT_BURST_LENGTH;
	qt_burst_lengths[3] = LED_KEY_BURST_LENGTH_2;

	qt_burst_lengths[8] = ROTOR_BURST_LENGTH_8;
	qt_burst_lengths[9] = ROTOR_BURST_LENGTH_9;
	qt_burst_lengths[10] = ROTOR_BURST_LENGTH_10;
	qt_burst_lengths[11] = ROTOR_BURST_LENGTH_11;
	qt_burst_lengths[12] = DEFAULT_BURST_LENGTH;

	qt_burst_lengths[16] = DEFAULT_BURST_LENGTH;
	qt_burst_lengths[17] = DEFAULT_BURST_LENGTH;
	qt_burst_lengths[18] = DEFAULT_BURST_LENGTH;
	qt_burst_lengths[19] = DEFAULT_BURST_LENGTH;

	/* Unused channels */
	qt_burst_lengths[4] = ZERO_BURST_LENGTH;
	qt_burst_lengths[5] = ZERO_BURST_LENGTH;
	qt_burst_lengths[6] = ZERO_BURST_LENGTH;
	qt_burst_lengths[7] = ZERO_BURST_LENGTH;
	qt_burst_lengths[13] = ZERO_BURST_LENGTH;
	qt_burst_lengths[14] = ZERO_BURST_LENGTH;
	qt_burst_lengths[15] = ZERO_BURST_LENGTH;
	qt_burst_lengths[20] = ZERO_BURST_LENGTH;
	qt_burst_lengths[21] = ZERO_BURST_LENGTH;
	qt_burst_lengths[22] = ZERO_BURST_LENGTH;
	qt_burst_lengths[23] = ZERO_BURST_LENGTH;
	qt_burst_lengths[24] = ZERO_BURST_LENGTH;
	qt_burst_lengths[25] = ZERO_BURST_LENGTH;
	qt_burst_lengths[26] = ZERO_BURST_LENGTH;
	qt_burst_lengths[27] = ZERO_BURST_LENGTH;
	qt_burst_lengths[28] = ZERO_BURST_LENGTH;
	qt_burst_lengths[29] = ZERO_BURST_LENGTH;
	qt_burst_lengths[30] = ZERO_BURST_LENGTH;
	qt_burst_lengths[31] = ZERO_BURST_LENGTH;
}

/**
 * @brief configure timer ISR to fire regularly
 * @ingroup QTouch
 *
 */
static void init_timer_isr( void )
{
	tc_enable(&TCCR0A);
	/*  set timer compare value (how often timer ISR will fire) */
	OCR0A = (TICKS_PER_MS * qt_measurement_period_msec);
	/*  enable timer ISR on compare A */
	TIMSK0 = 0x02u; /* Caution */
	/*  timer prescaler = system clock / 1024  */
	TCCR0B = 0x05u; /* Caution */
	/*  timer mode = CTC (count up to compare value, then reset)    */
	TCCR0A = 0x02u; /* Caution */
}

/**
 * @brief initialise host app, pins, watchdog, etc
 * @ingroup RApp_API
 *
 */
static void init_system( void )
{
	/* disable pull-ups */
	MCUCR |= (1u << PUD);

	/*Configure GPIO for LED Output and buzzer */
	DDRE  |=  (1 << DDRE3) | (1 << DDRE4) | (1 << DDRE6);
}

/**
 * @brief timer 0 compare ISR
 * @ingroup RApp_API
 *
 */
uint32_t timer_int;
ISR(TIMER0_COMPA_vect)
{
	touch_interrupt_call_back();
	timer_int++;
}

void touch_interrupt_call_back(void)
{
	/* set flag: it's time to measure touch */
	time_to_measure_touch = 1u;

	/* update the current time */
	current_time_ms_touch += qt_measurement_period_msec;
}

/**
 * @brief  Configure the sensors, assign the channel number, aks_group,
 * hysteresis.
 * @ingroup QTouch
 */
void config_sensors(void)
{
	qt_enable_key(CHANNEL_19, AKS_GROUP_1, 20u, HYST_6_25 ); /* Sensor 0 */
	qt_enable_key(CHANNEL_3, AKS_GROUP_1, 20u, HYST_6_25 ); /* Sensor 1 */
	qt_enable_key(CHANNEL_0, AKS_GROUP_1, 20u, HYST_6_25 ); /* Sensor 2 */
	qt_enable_key(CHANNEL_17, AKS_GROUP_1, 20u, HYST_6_25 ); /* Sensor 3 */
	qt_enable_key(CHANNEL_18, AKS_GROUP_1, 20u, HYST_6_25 ); /* Sensor 4 */
	qt_enable_key(CHANNEL_2, AKS_GROUP_1, 15u, HYST_6_25 ); /* Sensor 5 */
	qt_enable_key(CHANNEL_1, AKS_GROUP_1, 15u, HYST_6_25 ); /* Sensor 6 */
	qt_enable_key(CHANNEL_16, AKS_GROUP_1, 16u, HYST_6_25 ); /* Sensor 7 */
	qt_enable_rotor(CHANNEL_8, CHANNEL_11, AKS_GROUP_1, 45u, HYST_25,
			RES_8_BIT, 3u );                                               /*
	                                                                                *
	                                                                                *Sensor
	                                                                                *
	                                                                                *8
	                                                                                **/
	qt_enable_key(CHANNEL_12, AKS_GROUP_1, 20u, HYST_6_25 ); /* Sensor 9 */
}

/**
 * @brief This function is called every time the qt measuremement is done.
 * This is the pointer to callback function called before processing the signal
 * @ingroup QTouch
 *
 */
static void qt_avr477_filter_cb(void)
{
	uint8_t i;

	/* filter channel signals - averaging 6 samples */
	for (i = 0u; i < QT_NUM_CHANNELS; i++) {
		filter[i][0] = filter[i][1];
		filter[i][1] = filter[i][2];
		filter[i][2] = filter[i][3];
		filter[i][3] = filter[i][4];
		filter[i][4] = filter[i][5];
		filter[i][5] = qt_measure_data.channel_signals[i];
		qt_measure_data.channel_signals[i] = ((filter[i][0] +
				filter[i][1] +
				filter[i][2] +
				filter[i][3] +
				filter[i][4] +
				filter[i][5]) / 6u);
	}

	return;
}

/**
 * @brief Buzzer initialization
 * @ingroup RApp_API
 */
static void buzzer_init(void)
{
	DDRE |= (1 << DDRE6);
	PORTE &= ~(1 << PORTE6);
}

/**
 * @brief led0 Toggle
 * @ingroup RApp_API
 */
static inline void led0_toggle(void)
{
	PORTE ^= (1 << PE3);
}

/**
 * @brief Led1 Toggle
 * @ingroup RApp_API
 */
static inline void led1_toggle(void)
{
	PORTE ^= (1 << PE4);
}

#endif
uint8_t qt_button;
uint8_t qt_event;

/**************************************************************************//**
*  \brief Buttons handler
*
*  \param[in] event - event
*  \param[in] button - index
*  \param[in] data - data (AKA hold time)
******************************************************************************/
void appButtonsInd(BSP_TouchEvent_t event, uint8_t button, uint8_t data)
{
	qt_button = button;
	qt_event = event;
}

/*End of file  avr477QTouch.c */
