/**
 * \file
 *
 * \brief QTouch Example for QT600-ATXMEGA128A1-QT16 board.
 *
 * Copyright (C) 2011-2015 Atmel Corporation. All rights reserved.
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 * Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */
/*----------------------------------------------------------------------------
Example File History
----------------------------------------------------------------------------*/
/* QT600 Example projet version 1.2
* ---------------------------------
* To work with QT library version 4.0
*/

/* QT600 Example projet version 1.1
* ---------------------------------
*/

/**
 * \mainpage ASF Qtouch example on QT600-ATXMEGA128A1-QT16
 *
 * \section intro Introduction
 * This example shows how to implement the Qtouch library and Qtouch Debug
 * on QT600-ATXMEGA128A1-QT16 board.
 *
 * \section QtouchDebug QTouch Debug
 * The QT600 Bridge must be used to connect QT600-ATXMEGA128A1-QT16 board with
 * the QTouch Analyser tool.
 *
 * \section QT600 QT600 Setting
 *
 * Device Fuse Settings
 * The default fuse settings for the device will be correct for the
 * QT600 Example Project. However it is adviceable to verify
 * the fuse settings before trying to run the application.
 *
 * WDWP:       8 cycles (8ms @ 3.3V)
 * WDP:        8 cycles (8ms @ 3.3V)
 * BOOTRST:	Application Reset
 * BODACT:     BOD Disabled
 * BODPD:      BOD Disabled
 * JTAGEN:     Programmed
 *
 * Fusebyte0: 0xFF
 * Fusebyte1: 0x00
 * Fusebyte2: 0xFF
 * Fusebyte4: 0xFE
 * Fusebyte5: 0xFF
 */

/*----------------------------------------------------------------------------
compiler information
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
include files
----------------------------------------------------------------------------*/
#include <avr/io.h>
#include <avr/interrupt.h>
#define __delay_cycles(n)     __builtin_avr_delay_cycles(n)
#define __enable_interrupt()  sei()

/*  now include touch api.h with the localization defined above */
#include "touch_api.h"

#include "QDebug.h"
#include "QDebugTransport.h"
#include "sysclk.h"
#include "pmic.h"
#include "tc.h"
/*! \name TIMER_PERIOD conmputation
 */
//! @{
#define TIMER_PERIOD TICKS_PER_MS * qt_measurement_period_msec
//! @}
/*----------------------------------------------------------------------------
manifest constants
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
type definitions
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
prototypes
----------------------------------------------------------------------------*/
/*  initialise host app, pins, watchdog, etc    */
static void init_system (void);
/*  configure timer ISR to fire regularly   */
static void init_timer_isr (void);
/*  Assign the parameters values to global configuration parameter structure    */
static void qt_set_parameters (void);

/*----------------------------------------------------------------------------
Structure Declarations
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
macros
----------------------------------------------------------------------------*/
/*! \name The timing information for timer to fire periodically to measure touch
 */
//! @{
#ifdef TICKS_PER_MS
#undef TICKS_PER_MS
#define TICKS_PER_MS                1000u	//500u. 1.1 change as clock is running at 8MHz
#endif

#define TIMER_COUNTER_L TCC0.CNTL
#define TIMER_COUNTER_H TCC0.CNTH
//! @}
#define GET_SENSOR_STATE(SENSOR_NUMBER) qt_measure_data.qt_touch_status.sensor_states[(SENSOR_NUMBER/8)] & (1 << (SENSOR_NUMBER % 8))
#define GET_ROTOR_SLIDER_POSITION(ROTOR_SLIDER_NUMBER) qt_measure_data.qt_touch_status.rotor_slider_values[ROTOR_SLIDER_NUMBER]
/*----------------------------------------------------------------------------
global variables
----------------------------------------------------------------------------*/
unsigned int gMsTimeout;	// The ms timeout variable,
// only access with NOT_TIMEDOUT and SET_TIMEOUT
 /**
 * Touch Measurement period in milliseconds.  This variable determines how
 * often a new touch measurement must be done.
 *  Default value: Every 25 milliseconds.
 */
uint16_t qt_measurement_period_msec = 25u;

#ifdef _DEBUG_INTERFACE_
extern uint16_t timestamp1_hword;
extern uint16_t timestamp1_lword;
extern uint16_t timestamp2_hword;
extern uint16_t timestamp2_lword;
extern uint16_t timestamp3_hword;
extern uint16_t timestamp3_lword;
#endif
/*----------------------------------------------------------------------------
extern variables
----------------------------------------------------------------------------*/
/* This configuration data structure parameters if needs to be changed will be
changed in the qt_set_parameters function */
extern qt_touch_lib_config_data_t qt_config_data;
/* touch output - measurement data */
extern qt_touch_lib_measure_data_t qt_measure_data;
/* Get sensor delta values */
extern int16_t qt_get_sensor_delta (uint8_t sensor);
/* Output can be observed in the watch window using this pointer */
qt_touch_lib_measure_data_t *pqt_measure_data = &qt_measure_data;

/*----------------------------------------------------------------------------
static variables
----------------------------------------------------------------------------*/
/**
 * Flag set by timer ISR when it's time to measure touch.
 */
static volatile uint8_t time_to_measure_touch = 0u;
/**
 * Current time, set by timer ISR.
 */
volatile uint16_t current_time_ms_touch = 0u;
static void
example_cca_interrupt_callback (void)
{
  if (gMsTimeout)
    {
      gMsTimeout--;
    }
  /*  set flag: it's time to measure touch    */
  time_to_measure_touch = 1u;
  /*  update the current time  */
  current_time_ms_touch += qt_measurement_period_msec;
}

/*! \brief code main entry point
 */
int
main (void)
{
  /*status flags to indicate the re-burst for library */
  uint16_t status_flag = 0u;
  uint16_t burst_flag = 0u;


#ifdef _DEBUG_INTERFACE_
  timestamp1_hword = current_time_ms_touch;
  timestamp1_lword = (uint16_t) TIMER_COUNTER_L;
  timestamp1_lword |= (uint16_t) (TIMER_COUNTER_H << 8);
#endif
  /* initialise host app, pins, watchdog, etc */

  pmic_init ();
  sysclk_init ();
  init_timer_isr ();
  /* Config sensors  */
  qt_enable_key (CHANNEL_0, NO_AKS_GROUP, 10u, HYST_6_25);
  qt_enable_key (CHANNEL_1, NO_AKS_GROUP, 10u, HYST_6_25);
  qt_enable_key (CHANNEL_2, NO_AKS_GROUP, 10u, HYST_6_25);
  qt_enable_key (CHANNEL_3, NO_AKS_GROUP, 10u, HYST_6_25);
  qt_enable_key (CHANNEL_4, NO_AKS_GROUP, 10u, HYST_6_25);
  qt_enable_key (CHANNEL_5, NO_AKS_GROUP, 10u, HYST_6_25);
  qt_enable_key (CHANNEL_6, NO_AKS_GROUP, 10u, HYST_6_25);
  qt_enable_key (CHANNEL_7, NO_AKS_GROUP, 10u, HYST_6_25);
  qt_enable_key (CHANNEL_8, NO_AKS_GROUP, 10u, HYST_6_25);
  qt_enable_key (CHANNEL_9, NO_AKS_GROUP, 10u, HYST_6_25);
  qt_enable_slider (CHANNEL_10, CHANNEL_12, NO_AKS_GROUP, 10u, HYST_6_25,
		    RES_8_BIT, 0u);
  qt_enable_rotor (CHANNEL_13, CHANNEL_15, NO_AKS_GROUP, 6u, HYST_50,
		   RES_8_BIT, 0u);

  /* Initialise and set touch params */
  qt_init_sensing ();
  qt_set_parameters ();
  cpu_irq_enable ();
  tc_set_cca_interrupt_callback (&TCC0, example_cca_interrupt_callback);
  /*  Address to pass address of user functions   */
  /*  This function is called after the library has made capacitive measurements,
     *   but before it has processed them. The user can use this hook to apply filter
     *   functions to the measured signal values.(Possibly to fix sensor layout faults)    */
  qt_filter_callback = 0;
#ifdef _DEBUG_INTERFACE_
  /* Initialize debug protocol */
  QDebug_Init ();
#endif
  __enable_interrupt ();
#ifdef _DEBUG_INTERFACE_
  /* Process commands from PC */
  QDebug_ProcessCommands ();
#endif
  /* loop forever */
  for (;;)
    {
      if (time_to_measure_touch)
	{
	  time_to_measure_touch = 0u;
	  do
	    {
#ifdef _DEBUG_INTERFACE_
	      timestamp2_hword = current_time_ms_touch;
	      timestamp2_lword = (uint16_t) TIMER_COUNTER_L;
	      timestamp2_lword |= (uint16_t) (TIMER_COUNTER_H << 8);
#endif
	      /* Measure touch once */
	      status_flag = qt_measure_sensors (current_time_ms_touch);
#ifdef _DEBUG_INTERFACE_
	      timestamp3_hword = current_time_ms_touch;
	      timestamp3_lword = (uint16_t) TIMER_COUNTER_L;
	      timestamp3_lword |= (uint16_t) (TIMER_COUNTER_H << 8);
#endif
	      burst_flag = status_flag & QTLIB_BURST_AGAIN;
#ifdef _DEBUG_INTERFACE_
	      /* send debug data */
	      QDebug_SendData (status_flag);
#endif
	      /*Time critical host application code goes here */

	    }
	  while (burst_flag);
#ifdef _DEBUG_INTERFACE_
	  /* Process commands from PC */
	  QDebug_ProcessCommands ();
#endif
	}

      /* Non time-critical application code goes here */
    }
}

/*! \brief qt_set_parameters function
 *
 * This will fill the default threshold values in the configuration
 * data structure.But User can change the values of these parameters .
 * \note initialize configuration data for processing
 */
static void
qt_set_parameters (void)
{
  qt_config_data.qt_di = DEF_QT_DI;
  qt_config_data.qt_neg_drift_rate = DEF_QT_NEG_DRIFT_RATE;
  qt_config_data.qt_pos_drift_rate = DEF_QT_POS_DRIFT_RATE;
  qt_config_data.qt_max_on_duration = DEF_QT_MAX_ON_DURATION;
  qt_config_data.qt_drift_hold_time = DEF_QT_DRIFT_HOLD_TIME;
  qt_config_data.qt_recal_threshold = DEF_QT_RECAL_THRESHOLD;
  qt_config_data.qt_pos_recal_delay = DEF_QT_POS_RECAL_DELAY;
}

/*! \brief Configure timer ISR to fire regularly.
 */
static void
init_timer_isr (void)
{
  tc_enable (&TCC0);
  tc_write_period (&TCC0, TIMER_PERIOD);
  tc_write_clock_source (&TCC0, TC_CLKSEL_DIV8_gc);
  tc_set_cca_interrupt_level (&TCC0, PMIC_LVL_LOW);
}
