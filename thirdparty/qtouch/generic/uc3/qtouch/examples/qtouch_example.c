/*****************************************************************************
 *
 * \file
 *
 * \brief Example to demonstrate the QTouch Capacitive Touch method on the 
 *     STK600 board for Atmel UC3 devices
 *
 * Copyright (C) 2011-2015 Atmel Corporation. All rights reserved.
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
 ******************************************************************************/
 
// Include Files 
#include "compiler.h"
#include "conf_example.h"

// Include ASF drivers 
#include "gpio.h"
#include "sysclk.h"
#include "tc.h"
#if defined(__GNUC__)
#  include "intc.h"
#endif
// Include Touch API
#include "touch_api.h"

//! \name QTOUCH LIBRARY CONFIGURATION
//!@{
/** 
 * \def _QTOUCH_
 * \brief This macro has to be defined in order to use QTouch libraries. 
 * Defined in the Symbols in Project Properties
 */
 /** 
 * \def QT_NUM_CHANNELS
 * \brief Number of QTouch Channels to be used
 * 8 or 16 or 32 - Defined in the Symbols in Project Properties
 */
 /** 
 * \def SNSK
 * \brief Port to be used for SNSK
 * Defined in the Symbols in Project Properties
 */
 /** 
 * \def SNS
 * \brief Port to be used for SNS
 * Defined in the Symbols in Project Properties
 */
 /** 
 * \def QT_DELAY_CYCLES
 * \brief Delay cycles that determine the capacitance charge transfer time.
 * 1 to 255 - Defined in the Symbols in Project Properties
 */
/** 
 * \def _ROTOR_SLIDER_
 * \brief Rotor/slider can be added to the design, if this macro is enabled.  
 * Defined in the Symbols in Project Properties
 */
 /** 
 * \def _DEBUG_INTERFACE_
 * \brief To enable debug interface with QTouch Studio
 * Defined in the Symbols in Project Properties
 */
 //!@}

#define PORTA (((volatile avr32_gpio_port_t *) &AVR32_GPIO) + 0)
#define PORTB (((volatile avr32_gpio_port_t *) &AVR32_GPIO) + 1)
#define PORTC (((volatile avr32_gpio_port_t *) &AVR32_GPIO) + 2)
#if (UC3A0 || UC3A1 || UC3B)
#define PORTX (((volatile avr32_gpio_port_t *) &AVR32_GPIO) + 3)
#elif UC3C
#define PORTD (((volatile avr32_gpio_port_t *) &AVR32_GPIO) + 3)
#else
#error Unsupported chip type
#endif



#ifdef _DEBUG_INTERFACE_
// Include files for QTouch Studio integration
#include "QDebug.h"
#include "QDebugTransport.h"
#endif


// FUNCTION PROTOTYPES
//! \brief Configures ISR to fire regularly
static void init_timer_isr( void );
//! \brief Initializes the necessary parameters for QTouch library 
static void qtouch_init(void);
/** 
 * \brief Initializes configuration data for processing.
 * This will fill the default threshold values in the configuration
 * data structure.But User can change the values of these parameters.
 * Values can be changed in "touch_api.h"
 */
static void qt_set_parameters( void );
//! \brief Configure the sensors
static void config_sensors(void);

//! \brief prototypes for rotor_slider configuration for various channels
#if defined(_ROTOR_SLIDER_)
//! Configure the sensors with rotors/sliders with keys
static void config_rotor_sliders(void);

#  if (QT_NUM_CHANNELS == 8u)
//! Configure the sensors for 8 channel Key Rotor/sliders
static void config_8ch_krs(void);
#  elif (QT_NUM_CHANNELS == 16u)
//! Configure the sensors for 16 channel Key Rotor/sliders
static void config_16ch_krs(void);
#  elif (QT_NUM_CHANNELS == 32u)
//!  Configure the sensors for 32 channel Key Rotor/sliders
static void config_32ch_krs(void);
#  endif
#else
//! Configure the sensors for  Keys configuration
static void config_keys(void);
#endif  // _ROTOR_SLIDER_


//! \name SENSOR STATE CONFIGURATIONS
//!@{
/**
 * \def GET_SENSOR_STATE(SENSOR_NUMBER)
 * \brief To get the sensor state that it is in detect or not
 * \param SENSOR_NUMBER for which the state to be detected 
 * \return Returns either 0 or 1
 * If the bit value is 0, it is not in detect
 * If the bit value is 1, it is in detect
 */
/**
 * \def GET_ROTOR_SLIDER_POSITION(ROTOR_SLIDER_NUMBER) 
 * \brief To get the rotor angle or slider position.
 * These values are valid only when the sensor state for 
 * corresponding rotor or slider shows in detect.
 * \param ROTOR_SLIDER_NUMBER for which the position to be known
 * \return Returns rotor angle or sensor position
 */
//!@}
#define GET_SENSOR_STATE(SENSOR_NUMBER) qt_measure_data.qt_touch_status.sensor_states[(SENSOR_NUMBER/8)] & (1 << (SENSOR_NUMBER % 8))

#define GET_ROTOR_SLIDER_POSITION(ROTOR_SLIDER_NUMBER) qt_measure_data.qt_touch_status.rotor_slider_values[ROTOR_SLIDER_NUMBER]


// EXTERN VARIABLES DECLARATION
/**
 * This configuration data structure parameters 
 * if needs to be changed will be changed in the 
 * qt_set_parameters function 
 */
extern qt_touch_lib_config_data_t qt_config_data;
//! Touch Measurement data
extern qt_touch_lib_measure_data_t qt_measure_data;
//! Get sensor delta values
extern int16_t qt_get_sensor_delta( uint8_t sensor);
//! Output can be observed in the watch window using this pointer
qt_touch_lib_measure_data_t *pqt_measure_data = &qt_measure_data;

extern uint16_t timestamp1_hword;
extern uint16_t timestamp1_lword;
extern uint16_t timestamp2_hword;
extern uint16_t timestamp2_lword;
extern uint16_t timestamp3_hword;
extern uint16_t timestamp3_lword;


//STATIC VARIABLES DECLARATION
// flag set by timer ISR when it's time to measure touch
static volatile uint8_t time_to_measure_touch = 0u;
// current time, set by timer ISR 
static volatile uint16_t current_time_ms_touch = 0u;

// GLOBAL VARIABLE DECLARATION
// Timer period in msec
uint16_t qt_measurement_period_msec = 25u;


/**
 * \brief TIMER ISR
 * \param tc_irq is the subroutine function name
 * \param GRP is the group under which the interrupt lies
 * \param LVL is the priority level assigned for the group
 */
ISR(tc_irq,GRP,LVL)
{
	// update the current time
	current_time_ms_touch++;

	// Every 25th ms
	if( (current_time_ms_touch % qt_measurement_period_msec) == 0u ) {
		// set flag: it's time to measure touch
		time_to_measure_touch = 1u;
	}

	// Clear the interrupt flag. This is a side effect of reading the TC SR.
	tc_read_sr(&AVR32_TC, TC_CHANNEL);
}


/**
 * \brief Main Application Code
 *  - Initialize the system clocks
 *  - Initialize the touch functions
 *  - Initialize the timer 
 *  - if debug interface is enabled, initialize the qdebug commmands
 *  - When touch status in detect, Set the led & send data to QTouch Studio
 */
int main (void)
{
	// status flags to indicate the re-burst for library
	uint16_t status_flag = 0u;
	uint16_t burst_flag = 0u;
	/**
	* \brief Initialize the system clock 
	* Clock settings are done in cof_clock.h
	* It sets the cpu to run at crystal frequency 
	* which uses OSC0 as souce
	*/
	sysclk_init(); 

	//! Initializes the functions necessary for QTouch
	qtouch_init();

	//! Configure the timer ISR to fire regularly for QTouch Acquisition
	init_timer_isr();


	//! Address to pass address of user functions 
	/** 
	* \brief This function is called after the library has made 
	* capacitive measurements, but before it has processed them. 
	* The user can use this hook to apply filter functions to the measured 
	* signal values.(Possibly to fix sensor layout faults) 
	*/
    qt_filter_callback = 0;

#ifdef _DEBUG_INTERFACE_
	// Initialize QDebug protocol
	QDebug_Init();
#endif

	// Enable global interrupts
	cpu_irq_enable();

#ifdef _DEBUG_INTERFACE_
	// Process commands from PC
	QDebug_ProcessCommands();
#endif

	// loop forever 
	for( ; ; ) {
		if( time_to_measure_touch ) {
			if( qt_measure_data.qt_touch_status.sensor_states[0]) {
				gpio_clr_gpio_pin(STATUS_LED);
			} else {
				gpio_set_gpio_pin(STATUS_LED);
			}

			// clear flag: it's time to measure touch
			time_to_measure_touch = 0u;

			do {
				// one time measure touch sensors
				status_flag = qt_measure_sensors( current_time_ms_touch );
				burst_flag = status_flag & QTLIB_BURST_AGAIN;

				/*Time critical host application code goes here*/

#ifdef _DEBUG_INTERFACE_
				// send debug data 
				QDebug_SendData(status_flag);
#endif

			} while (burst_flag) ;
#ifdef _DEBUG_INTERFACE_
			// Process commands from PC
			QDebug_ProcessCommands();
#endif
		}

	// Time Non-critical host application code goes here
	}  //end of loop forever
	
} // end of main


//! \brief  Initialize QTouch Configuration and sensors
static void qtouch_init(void){

	/** Configures sensors for rotor, slider or for button 
	 * based on the options given in the SYMBOLS
	 */
	config_sensors();

	// initialize touch sensing
	qt_init_sensing();

	// Set the parameters like recalibration threshold, Max_On_Duration etc
	qt_set_parameters( );
}


/** 
 * \brief Initialize configuration data for processing
 * This will fill the default threshold values in the configuration
 * data structure.But User can change the values of these parameters.
 * Values can be changed in "touch_api.h"
 */
static void qt_set_parameters( void )
{
	//! This can be modified by the user to different values
	qt_config_data.qt_di              = DEF_QT_DI;
	qt_config_data.qt_neg_drift_rate  = DEF_QT_NEG_DRIFT_RATE;
	qt_config_data.qt_pos_drift_rate  = DEF_QT_POS_DRIFT_RATE;
	qt_config_data.qt_max_on_duration = DEF_QT_MAX_ON_DURATION;
	qt_config_data.qt_drift_hold_time = DEF_QT_DRIFT_HOLD_TIME;
	qt_config_data.qt_recal_threshold = DEF_QT_RECAL_THRESHOLD;
	qt_config_data.qt_pos_recal_delay = DEF_QT_POS_RECAL_DELAY;
}


//! \brief Configure the QTouch sensors
static void config_sensors(void)
{
#if defined(_ROTOR_SLIDER_)
	// Configure for _ROTOR_SLIDER_
	config_rotor_sliders();
#else
	// Configure for only keys
	config_keys();
#endif 
}


#ifndef _ROTOR_SLIDER_
//! \brief  Configure the sensors as keys only.
static void config_keys(void)
{
	// enable sensors 0..3: keys on channels 0..3
	qt_enable_key( CHANNEL_0, AKS_GROUP_1, 10u, HYST_6_25 );
	qt_enable_key( CHANNEL_1, AKS_GROUP_1, 10u, HYST_6_25 );
	qt_enable_key( CHANNEL_2, AKS_GROUP_1, 10u, HYST_6_25 );
	qt_enable_key( CHANNEL_3, AKS_GROUP_1, 10u, HYST_6_25 );

#if(QT_NUM_CHANNELS >= 8u)
	// enable sensors 4..7: keys on channels 4..7
	qt_enable_key( CHANNEL_4, AKS_GROUP_1, 10u, HYST_6_25 );
	qt_enable_key( CHANNEL_5, AKS_GROUP_1, 10u, HYST_6_25 );
	qt_enable_key( CHANNEL_6, AKS_GROUP_1, 10u, HYST_6_25 );
	qt_enable_key( CHANNEL_7, AKS_GROUP_1, 10u, HYST_6_25 );
#endif
#if (QT_NUM_CHANNELS >= 16u)
	// enable sensors 8..15: keys on channels 8..15    
	qt_enable_key( CHANNEL_8, AKS_GROUP_1, 10u, HYST_6_25 );
	qt_enable_key( CHANNEL_9, AKS_GROUP_1, 10u, HYST_6_25 );
	qt_enable_key( CHANNEL_10, AKS_GROUP_1, 10u, HYST_6_25 );
	qt_enable_key( CHANNEL_11, AKS_GROUP_1, 10u, HYST_6_25 );
	qt_enable_key( CHANNEL_12, AKS_GROUP_1, 10u, HYST_6_25 );
	qt_enable_key( CHANNEL_13, AKS_GROUP_1, 10u, HYST_6_25 );
	qt_enable_key( CHANNEL_14, AKS_GROUP_1, 10u, HYST_6_25 );
	qt_enable_key( CHANNEL_15, AKS_GROUP_1, 10u, HYST_6_25 );
#endif
#if (QT_NUM_CHANNELS >= 32u)
	// enable sensors 16..31: keys on channels 16..31  
	qt_enable_key( CHANNEL_16, AKS_GROUP_1, 10u, HYST_6_25 );
	qt_enable_key( CHANNEL_17, AKS_GROUP_1, 10u, HYST_6_25 );
	qt_enable_key( CHANNEL_18, AKS_GROUP_1, 10u, HYST_6_25 );
	qt_enable_key( CHANNEL_19, AKS_GROUP_1, 10u, HYST_6_25 );
	qt_enable_key( CHANNEL_20, AKS_GROUP_1, 10u, HYST_6_25 );
	qt_enable_key( CHANNEL_21, AKS_GROUP_1, 10u, HYST_6_25 );
	qt_enable_key( CHANNEL_22, AKS_GROUP_1, 10u, HYST_6_25 );
	qt_enable_key( CHANNEL_23, AKS_GROUP_1, 10u, HYST_6_25 );
	qt_enable_key( CHANNEL_24, AKS_GROUP_1, 10u, HYST_6_25 );
	qt_enable_key( CHANNEL_25, AKS_GROUP_1, 10u, HYST_6_25 );
	qt_enable_key( CHANNEL_26, AKS_GROUP_1, 10u, HYST_6_25 );
	qt_enable_key( CHANNEL_27, AKS_GROUP_1, 10u, HYST_6_25 );
	qt_enable_key( CHANNEL_28, AKS_GROUP_1, 10u, HYST_6_25 );
	qt_enable_key( CHANNEL_29, AKS_GROUP_1, 10u, HYST_6_25 );
	qt_enable_key( CHANNEL_30, AKS_GROUP_1, 10u, HYST_6_25 );
	qt_enable_key( CHANNEL_31, AKS_GROUP_1, 10u, HYST_6_25 );
#endif 

}
#endif  //  _ROTOR_SLIDER_ 


#if defined(_ROTOR_SLIDER_)
//! \brief Configure the sensors for rotor,slider along with keys
static void config_rotor_sliders(void)
{
// Call this function if library is 8 channel library with KRS Configuration
#if (QT_NUM_CHANNELS == 8u)
	{
		config_8ch_krs();
	}
#endif
// Call this function if library is 16 channel library with KRS Configuration
#if (QT_NUM_CHANNELS == 16u)
	{
		config_16ch_krs();
	}
#endif
// Call this function if library is 32 channel library with KRS Configuration
#if (QT_NUM_CHANNELS == 32u)
	{
		config_32ch_krs();
	}
#endif
}

//! \brief Configure the Sensors as keys and Rotor/Sliders for 8 channels only
#if (QT_NUM_CHANNELS == 8u)
static void config_8ch_krs(void)
{
	// enable sensor 0: a key on channel 0
	qt_enable_key( CHANNEL_0, AKS_GROUP_2, 10u, HYST_6_25 );
	// enable sensor 1: a slider on channels 1..3  */
	qt_enable_slider( CHANNEL_1, CHANNEL_3, NO_AKS_GROUP, 16u, HYST_6_25, RES_8_BIT, 0u);
	// enable sensor 2: a key on channel 4
	qt_enable_key( CHANNEL_4, AKS_GROUP_2, 10u, HYST_6_25 );
	// enable sensor 3: a rotor on channels 5..7
	qt_enable_rotor( CHANNEL_5, CHANNEL_7, NO_AKS_GROUP, 16u, HYST_6_25, RES_8_BIT, 0u );
}
#endif  // QT_NUM_CHANNELS == 8u config_8ch_krs 

#if (QT_NUM_CHANNELS == 16u)
//! \brief Configure the Sensors as keys and Rotor/Sliders for 16 channels only
static void config_16ch_krs(void)
{
	// enable sensor 0: a keys on channel 0
	qt_enable_key( CHANNEL_0, AKS_GROUP_2, 10u, HYST_6_25 );
	// enable sensor 1: a slider on channels 1..3
	qt_enable_slider( CHANNEL_1, CHANNEL_3, NO_AKS_GROUP, 16u, HYST_6_25, RES_8_BIT, 0u  );
	// enable sensor 2: a key on channel 4
	qt_enable_key( CHANNEL_4, AKS_GROUP_2, 10u, HYST_6_25 );
	// enable sensor 3: a rotor on channels 5..7
	qt_enable_rotor( CHANNEL_5, CHANNEL_7, NO_AKS_GROUP, 16u, HYST_6_25, RES_8_BIT, 0u );
	// enable sensor 4: a keys on channel 8
	qt_enable_key( CHANNEL_8, AKS_GROUP_2, 10u, HYST_6_25 );
	// enable sensor 5: a slider on channels 9..11
	qt_enable_slider( CHANNEL_9, CHANNEL_11, NO_AKS_GROUP, 16u, HYST_6_25, RES_8_BIT, 0u  );
	// enable sensor 6: a keys on channel 12
	qt_enable_key( CHANNEL_12, AKS_GROUP_2, 10u, HYST_6_25 );
	// enable sensor 7: a slider on channels 13..15
	qt_enable_rotor( CHANNEL_13, CHANNEL_15, NO_AKS_GROUP, 16u, HYST_6_25, RES_8_BIT, 0u  );
}
#endif   // QT_NUM_CHANNELS == 16u config_16ch_krs

#if (QT_NUM_CHANNELS == 32u)
 //! \brief Configure the Sensors as keys and Rotor/Sliders for 32 channels only.
static void config_32ch_krs(void)
{
	// enable sensor 0: a keys on channel 0
	qt_enable_key( CHANNEL_0, AKS_GROUP_2, 10u, HYST_6_25 );
	// enable sensor 1: a slider on channels 1..3
	qt_enable_slider( CHANNEL_1, CHANNEL_3, NO_AKS_GROUP, 16u, HYST_6_25, RES_8_BIT, 0u  );
	// enable sensor 2: a key on channel 4
	qt_enable_key( CHANNEL_4, AKS_GROUP_2, 10u, HYST_6_25 );
	// enable sensor 3: a rotor on channels 5..7
	qt_enable_rotor( CHANNEL_5, CHANNEL_7, NO_AKS_GROUP, 16u, HYST_6_25, RES_8_BIT, 0u );
	// enable sensor 4: a keys on channel 8
	qt_enable_key( CHANNEL_8, AKS_GROUP_2, 10u, HYST_6_25 );
	// enable sensor 5: a slider on channels 9..11
	qt_enable_slider( CHANNEL_9, CHANNEL_11, NO_AKS_GROUP, 16u, HYST_6_25, RES_8_BIT, 0u  );
	// enable sensor 6: a keys on channel 12
	qt_enable_key( CHANNEL_12, AKS_GROUP_2, 10u, HYST_6_25 );
	// enable sensor 7: a slider on channels 13..15
	qt_enable_rotor( CHANNEL_13, CHANNEL_15, NO_AKS_GROUP, 16u, HYST_6_25, RES_8_BIT, 0u  );
	// enable sensor 0: a keys on channel 0
	qt_enable_key( CHANNEL_16, AKS_GROUP_2, 10u, HYST_6_25 );
	// enable sensor 1: a slider on channels 1..3
	qt_enable_slider( CHANNEL_17, CHANNEL_19, NO_AKS_GROUP, 16u, HYST_6_25, RES_8_BIT, 0u  );
	// enable sensor 2: a key on channel 4
	qt_enable_key( CHANNEL_20, AKS_GROUP_2, 10u, HYST_6_25 );
	// enable sensor 3: a rotor on channels 5..7
	qt_enable_rotor( CHANNEL_21, CHANNEL_23, NO_AKS_GROUP, 16u, HYST_6_25, RES_8_BIT, 0u );
	// enable sensor 4: a keys on channel 8
	qt_enable_key( CHANNEL_24, AKS_GROUP_2, 10u, HYST_6_25 );
	// enable sensor 5: a slider on channels 9..11
	qt_enable_slider( CHANNEL_25, CHANNEL_27, NO_AKS_GROUP, 16u, HYST_6_25, RES_8_BIT, 0u  );
	// enable sensor 6: a keys on channel 12
	qt_enable_key( CHANNEL_28, AKS_GROUP_2, 10u, HYST_6_25 );
	// enable sensor 7: a slider on channels 13..15
	qt_enable_rotor( CHANNEL_29, CHANNEL_31, NO_AKS_GROUP, 16u, HYST_6_25, RES_8_BIT, 0u  );
}
#endif  // QT_NUM_CHANNELS == 32u config_32ch_krs

#endif //ROTOR_SLIDER


/** 
 * \brief Function to initialize the Timer/Counter module in Waveform mode.
 * It fires the ISR at regular intervals to start QTouch Acquisition.
 */
void init_timer_isr( void )
{
	// Configure the timer isr to fire regularly
	volatile avr32_tc_t *tc = &AVR32_TC;
	// Waveform Mode Options for TC
	static const tc_waveform_opt_t WAVEFORM_OPT = {
		// Channel selection : channel 0.
		.channel  = TC_CHANNEL,

		// Software trigger effect on TIOB : none.
		.bswtrg   = TC_EVT_EFFECT_NOOP,
		// External event effect on TIOB : none.
		.beevt    = TC_EVT_EFFECT_NOOP,
		// RC compare effect on TIOB : none.
		.bcpc     = TC_EVT_EFFECT_NOOP,
		// RB compare effect on TIOB : none.
		.bcpb     = TC_EVT_EFFECT_NOOP,

		// Software trigger effect on TIOA : none.
		.aswtrg   = TC_EVT_EFFECT_NOOP,
		// External event effect on TIOA : none.
		.aeevt    = TC_EVT_EFFECT_NOOP,
		// RC compare effect on TIOA: None
		.acpc     = TC_EVT_EFFECT_NOOP,
		// RA compare effect on TIOA: none 
		.acpa     = TC_EVT_EFFECT_NOOP,

		// Waveform selection: Up mode with automatic trigger(reset) on RC compare.
		.wavsel   = TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER,
		// External event trigger enable : no effect.
		.enetrg   = false,
		// External event selection : .
		.eevt     = 0u,
		// External event edge selection : none.
		.eevtedg  = TC_SEL_NO_EDGE,
		// Counter disable when RC compare.
		.cpcdis   = false,
		// Counter clock stopped with RC compare.
		.cpcstop  = false,

		// Burst signal selection.
		.burst    = false,
		// Clock inversion.
		.clki     = false,
		// Internal source clock 3
		.tcclks   =   TC_CLOCK 
	};

	// Configure the interrupts 
	static const tc_interrupt_t TC_INTERRUPT = {
		.etrgs = 0u,
		.ldrbs = 0u,
		.ldras = 0u,
		// Interrupt is enabled for RC compare match
		.cpcs  = 1u,
		.cpbs  = 0u,
		.cpas  = 0u,
		.lovrs = 0u,
		.covfs = 0u
	};
	// Initialize the timer/counter waveform.
	tc_init_waveform(tc, &WAVEFORM_OPT);
	/*
	 * Set the compare triggers.
	 * We configure it to count every 1 milliseconds.
	 * We want: (1 / (fPBA / 8)) * RC = 1 ms, hence RC = (fPBA / 8) / 1000
	 * to get an interrupt every 10 ms.
	 */
	tc_write_rc(tc, TC_CHANNEL, (sysclk_get_pba_hz() / 8 / 1000));
	// Configure the interrupts for channel 0
	tc_configure_interrupts(tc, TC_CHANNEL, &TC_INTERRUPT);
	// Initialize interrupt vectors.
	irq_initialize_vectors();
	// Register the timer interrupt handler to the interrupt controller
	irq_register_handler(&tc_irq, TC_IRQ, AVR32_INTC_INT0);
	// Start the timer/counter.
	tc_start(tc, TC_CHANNEL);
}  // end of init_timer_isr


//Application Documentation
/** 
 * \mainpage
 * \section intro Introduction
 * This project demonstrates the usage of QTouch in Atmel UC3A, UC3B and UC3C 
 * devices. \n
 * Example Application Notes: \n 
 *  - To provide touch sensing capability in the project, appropriate 
 *    library file should be linked with the application which is available in
 *    the ATMEL QTOUCH LIBRARY.
 *
 *  - The touch_api.h header file is the public header file should be included
 *    in which touch sensors and channels can be defined.
 *    The application here is configured for 32 channels with rotor slider(can
 *    be enabled by adding (_ROTOR_SLIDER_) that means the sensors are
 *    configured for keys with one channel, rotor with 3 channels and a slider
 *    with 3 channels.
 *
 *  - The timer triggers an interrupt whenever RC compare match has occurred.
 *    The RC value is loaded in a way that the compare match occurs on
 *    25ms. So for each 25ms, the sensor detects for the touch and if it is in
 *    detect, the led is set.
 *
 *  - QTouch Studio is used to visualize the touch board virtually on the PC.
 *    To utilize this feature, _DBEUG_INTERFACE_ should be enabled. It uses 
 *    BitBangSPI for communicating to the PC, which is one way communication.
 *
 *  - When QTouch Library with channels less than 16(requires 32 GPIOs) is
 *    used, intraport bursting can be enabled by setting both SNS ans SNKS as
 *    the same port and adding _SNS_SNSK_SAME_PORT_ symbol in the project.
 *
 * \section files Main files
 * - qtouch_example.c     : QTouch for atmel UC3A,UC3B and UC3C devices.
 * - conf_example.h       : Example configuration
 * - conf_clock.h         : System Clock configuration
 * - touch_qt_config.h    : QTouch Configuration Parameters
 * - QDebugSettings.h     : QDebug interface settings
 * - touch_api.h          : API for QTouch and QMatrix on AVR microcontrollers
 *
 *
 * \section referenceinfo References
 *  - Refer Application Note: Atmel QTouch Library User Guide 
 *  - Refer Application Note: ASF Getting Started Guide
 *  - Refer Application Note: ASF Reference Manual
 *
 * \section compinfo Compiler Support
 * The example application supports:
 *  - IAR EWAVR32
 *  - GNU GCC for AVR32
 *
 * \section deviceinfo Device Support
 *  - AT32UC3A0/A1 Series
 *  - AT32UC3B0/B1 Series
 *  - AT32UC3C0/C1 Series
 *
 * \section testinfo Test Information
 * This example has been tested with the following :
 *  - AVR Studio 5.0
 *  - IAR32 Embedded Workbench
 *  - STK600 development board
 *  - 1.8V and 3.3V Supply mode
 *  - Tested with devices
 *     --AT32UC3A0512
 *     --AT32UC3B0256
 *     --AT32UC3C0512C
 *
 * \section setupinfo Setup Information
 *  - CPU Clock Source     : OSC0 
 *  - Timer Clock Source   : PBA clock/2
 *  - CPU Speed            : OSC0
 *  - PBA Speed            : OSC0
 *  - PBB Speed            : OSC0
 *  - Timer Trigger Period : Every 25ms
 *
 * \section setupinfo Hardware Setup Information
 *  For debug interface the following connection should be done.
 *  - UC3A0 and UC3B0 devices: \n
 *       __________________________________________          \n
 *      |  STK600         |   QT600 INTERFACE BOARD|         \n
 *       __________________________________________          \n
 *      |  PORTE.E0       |   TOUCHDATA.PIN 05     |  (SS)   \n
 *      |  PORTE.E1       |   TOUCHDATA.PIN 06     |  (MOSI) \n
 *      |  PORTE.E2       |   TOUCHDATA.PIN 07     |  (MISO) \n
 *      |  PORTE.E3       |   TOUCHDATA.PIN 08     |  (SCK)  \n
 *       __________________________________________          \n
 *  - UC3C0 devices: \n
 *       __________________________________________          \n
 *      |  STK600         |   QT600 INTERFACE BOARD|         \n
 *       __________________________________________          \n
 *      |  PORTJ.J0       |   TOUCHDATA.PIN 05     |  (SS)   \n
 *      |  PORTJ.J1       |   TOUCHDATA.PIN 06     |  (MOSI) \n
 *      |  PORTJ.J2       |   TOUCHDATA.PIN 07     |  (MISO) \n
 *      |  PORTJ.J3       |   TOUCHDATA.PIN 08     |  (SCK)  \n
 *       __________________________________________          \n
 *
 *  - The mcu should be programmed and the QTouch interface board should
 *      be connected to the STK600 as stated above. 
 *  - The Power supply to the QTouch interface board 
 *      should be given through USB.
 *  - Note: Make sure that the VTG pins should be short circuited (On the
 *      QTouch interface board). \n
 * After all the hardware setup is done, plug in and plug out the USB pin on
 * the touch interface board to get connected with the QTouch Studio.
 *
 * \section contactinfo Contact Information
 * For further information, visit : 
 * <A href="http://www.atmel.com/products/AVR32/"> Atmel AVR32 </A>.\n
 * Technical support & faq, visit : 
 * <A href="http://www.atmel.com/design-support/"> Support and FAQ </A>.
 *
 * \author 
 * Atmel Corporation : http://www.atmel.com \n
 */
        
 
 
 
 
 
 
 
 
 
 
