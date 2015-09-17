/* This source file is part of the ATMEL QTouch Library Release 4.3.1 */
/**
 * \file
 *
 * \mainpage
 * QTouch Group A Example for STK600-RCUC3L0 board.
 *
 * This file (\ref main.c) contains the AT32UC3L QTouch Library API Example application
 * for QTouch Group A method using the Capacitive Touch (CAT) module on the
 * STK600-RCUC3L0 board.
 *
 * An External Sensor board can be connected to the STK600 GPIO Port pins for
 * testing.
 *
 * QT600 USB Bridge board can be used to transfer the QDebug data from STK600
 * board to the QTouch Studio.
 *
 * - Userguide:          QTouch Library User Guide - doc8207.pdf.
 * - Support: http://www.atmel.com/design-support/
 *
 *
 * Copyright (c) 2010-2015 Atmel Corporation. All rights reserved.
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


/*----------------------------------------------------------------------------
                            compiler information
----------------------------------------------------------------------------*/
#if !((defined(__GNUC__) && defined(__AVR32__)) || (defined(__ICCAVR32__) || defined(__AAVR32__)))
#error 'This compiler is not supported at the moment.'
#endif

/*----------------------------------------------------------------------------
                                include files
----------------------------------------------------------------------------*/
#include "compiler.h"
#include "board.h"

/**
 * Includes for Clock and Timer.
 */
#include "power_clocks_lib.h"
#include "gpio.h"
#include "tc.h"
#if defined(__GNUC__)
#include "intc.h"
#endif

/**
 * Includes for Touch Library.
 */
#include "touch_api_at32uc3l.h"

#if DEF_TOUCH_QDEBUG_ENABLE == 1
  /**
   * Includes for Touch Debug interface.
   */
#include "QDebug_at32uc3l.h"
#endif

/*----------------------------------------------------------------------------
                            manifest constants
----------------------------------------------------------------------------*/
/*! \name Example Timer configuration settings.
 */
//! @{

#define EXAMPLE_TC                       (&AVR32_TC0)	//!< timer number.
#define EXAMPLE_TC_IRQ_GROUP             (AVR32_TC0_IRQ_GROUP)	//!< timer irq group.
#define EXAMPLE_TC_IRQ                   (AVR32_TC0_IRQ0)	//!< timer irq.
#define EXAMPLE_TC_CHANNEL               (0u)	//!< timer channel.
#define EXAMPLE_RC_VALUE                 (6000u)	//!< set RC value for 1ms tick.

//! @}

/*! \name Example Clock configuration settings.
 * Note: The QDebug protocol USART clock settings are available in
 * qdebug/Serial.h.  When modifying the EXAMPLE_CPUCLK_HZ clock, care must be
 * taken to modify the TARGET_PBA_FREQ_HZ and TARGET_CPU_FREQ_HZ defines in
 * the Serial.h file for the QDebug protocol to work correctly.
 */
//! @{

#define EXAMPLE_TARGET_DFLL_FREQ_HZ       (96000000)	//!< DFLL target frequency, in Hz
#define EXAMPLE_TARGET_MCUCLK_FREQ_HZ     (48000000)	//!< MCU clock target frequency, in Hz
#define EXAMPLE_TARGET_PBACLK_FREQ_HZ     (48000000)	//!< PBA clock target frequency, in Hz
#define EXAMPLE_TARGET_PBBCLK_FREQ_HZ     (48000000)	//!< PBA clock target frequency, in Hz

//! @}

/*----------------------------------------------------------------------------
                                    macros
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
                            type definitions
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
                            Structure Declarations
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
                                global variables
----------------------------------------------------------------------------*/
/**
 * Touch Measurement period in milliseconds.  This variable determines how
 * often a new touch measurement must be done.
 *  Default value: Every 25 milliseconds.
 */
uint16_t measurement_period_ms = 25u;

/**
 * Flag set by timer ISR when it's time to measure touch.
 */
volatile uint8_t time_to_measure_touch = 0u;

/**
 * Current time, set by timer ISR.
 */
volatile uint16_t current_time_ms_touch = 0u;

/**
 * Flag set by touch_qta_measure_complete_callback() function when a fresh Touch
 * status is available.
 */
volatile uint8_t qta_measurement_done_touch = 0u;

/**
 * QTouch Group A measure data structure pointer.
 */
touch_measure_data_t *p_qta_measure_data = NULL;

/*----------------------------------------------------------------------------
                                extern variables
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
                                static variables
----------------------------------------------------------------------------*/
/*! \name Parameters to pcl_configure_clocks().
 */
//! @{

static scif_gclk_opt_t gc_dfllif_ref_opt = { SCIF_GCCTRL_SLOWCLOCK, 0, false };
static pcl_freq_param_t pcl_dfll_freq_param = {
  .main_clk_src = PCL_MC_DFLL0,
  .cpu_f = EXAMPLE_TARGET_MCUCLK_FREQ_HZ,
  .pba_f = EXAMPLE_TARGET_PBACLK_FREQ_HZ,
  .pbb_f = EXAMPLE_TARGET_PBBCLK_FREQ_HZ,
  .dfll_f = EXAMPLE_TARGET_DFLL_FREQ_HZ,
  .pextra_params = &gc_dfllif_ref_opt
};

//! @}

/**
 * QTouch Group A Data block provided as input to Touch library.
 */
static uint8_t qta_data_blk[PRIV_QTA_DATA_BLK_SIZE];

/**
 * QTouch Group A Configuration structure provided as input to Touch Library.
 *
 * Note: Use the touch_config_at32uc3l.h configuration header file to fill in
 * the elements of this structure.  DO NOT modify any of the input values
 * directly in this structure.
 */
static touch_qt_config_t qta_config = {
  QTA_NUM_CHANNELS,		/* QTouch Group A number of channels. */
  QTA_NUM_SENSORS,		/* QTouch Group A number of sensors. */
  QTA_NUM_ROTORS_SLIDERS,	/* QTouch Group A number of rotors and sliders. */
  /* QTOUCH GROUP A PIN CONFIGURATION INFO. */
  {
   QTA_SP_SELECTED,		/* QTouch Group A Sense pairs selected. */
   },
  /* QTOUCH GROUP A REGISTER CONFIGURATION INFO. */
  /* Refer CAT User Interface section in
     AT32UC3L datasheet. */
  /* TOUCH LIBRARY WILL SHIFT THE VALUE
     SPECIFIED BELOW TO THE REQUIRED
     REGISTER OFFSET */
  {
   QTA_CAT_CLK_DIV,		/* uint16_t div; TGxCFG0 Register. */
   QTA_CHLEN,			/* uint8_t  chlen; TGxCFG0 Register. */
   QTA_SELEN,			/* uint8_t  selen; TGxCFG0 Register. */
   QTA_DISHIFT,			/* uint8_t  dishift; TGxCFG1 Register. */
   QTA_ENABLE_EXTERNAL_SYNC,	/* uint8_t  sync; TGxCFG1 Register. */
   QTA_ENABLE_SPREAD_SPECTRUM,	/* uint8_t  spread; TGxCFG1 Register. */
   QTA_DILEN,			/* uint8_t  dilen; TGxCFG1 Register. */
   QTA_MAX_ACQ_COUNT,		/* uint16_t max; TGxCFG1 Register. */
   },
  /* QTOUCH GROUP A GLOBAL SENSOR CONFIGURATION INFO. */
  {
   QTA_DI,			/* uint8_t  di; Sensor detect integration (DI) limit. */
   QTA_NEG_DRIFT_RATE,		/* uint8_t  neg_drift_rate; Sensor negative drift rate. */
   QTA_POS_DRIFT_RATE,		/* uint8_t  pos_drift_rate; Sensor positive drift rate. */
   QTA_MAX_ON_DURATION,		/* uint8_t  max_on_duration; Sensor maximum on duration. */
   QTA_DRIFT_HOLD_TIME,		/* uint8_t  drift_hold_time; Sensor drift hold time. */
   QTA_POS_RECAL_DELAY,		/* uint8_t  pos_recal_delay; Sensor positive recalibration delay. */
   QTA_RECAL_THRESHOLD,		/* recal_threshold_t recal_threshold; Sensor recalibration threshold. */
   },
  qta_data_blk,			/* QTouch Group A data block index. */
  PRIV_QTA_DATA_BLK_SIZE,	/* QTouch Group A data block size. */
  QTA_FILTER_CALLBACK		/* QTouch Group A filter callback function pointer. */
};

/**
 * General configuration options common to QMatrix, QTouch A/B and Autonomous Touch
 * provided as input to Touch library.
 *
 * Note: Use the touch_config_at32uc3l.h configuration header file to fill in
 * the elements of this structure.  DO NOT modify any of the input values
 * directly in this structure.
 */
static touch_general_config_t general_config = {
  TOUCH_SYNC_PIN_OPTION,	/* Specify the sync pin option,
				   if included in the hardware design.
				   refer general_pin_options_t. */
  TOUCH_SPREAD_SPECTRUM_MAX_DEV,	/* uint8_t  maxdev; SSCFG Register. Max deviation of spread spectrum. */
  TOUCH_CSARES,			/* uint32_t csares; CSA Resistor control register. */
  TOUCH_CSBRES			/* uint32_t csbres;  CSA Resistor control register. */
};

/**
 * Touch Library input configuration structure.
 */
touch_config_t touch_config = {
  NULL,				/* Pointer to QMatrix configuration structure. */
  NULL,				/* Pointer to Autonomous Touch configuration structure. */
  &qta_config,			/* Pointer to QTouch Group A configuration structure. */
  NULL,				/* Pointer to QTouch Group B configuration structure. */
  &general_config		/* Pointer to General configuration structure. */
};

/*----------------------------------------------------------------------------
                                prototypes
----------------------------------------------------------------------------*/
/*! \brief Initialize host app, pins, watchdog, clock etc.
 */
static void init_system (void);

/*! \brief Initialize clock.
 */
static unsigned long init_clock (void);

/*! \brief Configure timer to fire ISR regularly
 */
static void init_timer (void);

/*! \brief configure keys, rotors and sliders.
 */
static touch_ret_t config_qt_grp_a_touch_sensors (void);

/*! \brief timer ISR
 */
#if defined(__AVR32__)
static void tc_irq (void);
#elif defined (__ICCAVR32__)
#pragma handler = EXAMPLE_TC_IRQ_GROUP, 1
__interrupt static void tc_irq (void);
#endif

/*! \brief QTouch Group A measure complete callback function example prototype.
 */
static void touch_qta_measure_complete_callback (touch_measure_data_t *
						 p_measure_data);

/*----------------------------------------------------------------------------
  static variables
----------------------------------------------------------------------------*/

/*! \brief Example application entry function.
 */
int
main (void)
{

/*  BEFORE USING THE EXAMPLE PROJECTS.

1. For support queries on,
     - QTouch Library usage
     - Capacitive Touch Sensor Tuning
     - Capacitive Touch Schematic design
     - Capacitive Touch Sensor design
   refer to http://www.atmel.com/design-support/

2. For more QTouch Library documentation,
   refer Atmel QTouch Library User Guide doc8207.pdf.

   For Capacitive Touch Sensor tuning guidelines,
   refer QTAN0062: QTouch and QMatrix Sensitivity Tuning for Keys, Sliders and Wheels.

   For Capacitive Touch Sensor design,
   refer doc10620.pdf: Touch Sensors Design Guide.

   http://www.atmel.com/dyn/products/app_notes.asp?family_id=697

3. The Example application uses a CPU, PBA and PBB clock of 48MHz.
   When using a different frequency setting, the following parameters must be
   changed accordingly to ensure proper QTouch operation.
   a. QTx_CAT_CLK_DIV.
   b. TOUCH_SPREAD_SPECTRUM_MAX_DEV, when Spread spectrum is enabled.
   c. PBA_HZ, when using qdebug/SPI_Master.c
   d. TARGET_PBA_FREQ_HZ and TARGET_CPU_FREQ_HZ, when using qdebug/SERIAL.c

4. STK600-QTouch Test setup pin information.
   The following table indicates the STK600 pin connections for the STK600-QTouch
   test setup.

   Important Note: The (csa1/csab1) and (csa2/csb2) Touch channel connections are
   multiplexed with the JTAG pins.  So, when using the JTAG debugging mode, these
   Touch channel connections MUST be removed. In the Flash mode, this will not
   cause any issues.

        ----------------------------------------------
	CAT CSA/CSB name - STK600 board Port-pin name
	----------------------------------------------
	ROTOR/WHEEL
	csa1 		 - 	pa1 (This pair is multiplexed with JTAG pins.
	csb1 		 - 	pa6  Remove Touch connections on these pins during JTAG debug mode.)

	csa2 		 - 	pa0 (This pair is multiplexed with JTAG pins.
	csb2 		 - 	pa7  Remove Touch connections on these pins during JTAG debug mode.)

	csa5 		 - 	pb2
	csb5 		 - 	pb4

	SLIDER
	csa9 		 - 	pd0
	csb9 		 - 	pd1

	csa7 		 - 	pa4
	csb7 		 - 	pa5

	csa8 		 - 	pc0
	csb8 		 - 	pc1

	KEY 1
	csa15 		 - 	pe4
	csb15 		 - 	pe1

	KEY 2
	csa16 		 - 	pe3
	csb16 		 - 	pe2

    ----------------------------------------------
	QT600 USB Bridge
	'TOUCH DATA' Header Pin name - STK600 board Port-pin name
	----------------------------------------------
	PA22 - 'TOUCH DATA' header pin 8 - clk 	 - pc6
	PA21 - 'TOUCH DATA' header pin 7 - miso	 - pc5
	PA20 - 'TOUCH DATA' header pin 6 - mosi  - pc4
	PA14 - 'TOUCH DATA' header pin 5 - nss 	 - pb6

5. When two or more acquisition methods are used, care must be taken such that a
   given port pin is not used by more than one method at the same time.  The following
   pin configuration options available in touch_config_at32uc3l.h must be carefully
   chosen to avoid any overlapping.
   a. QMatrix Pin Configuration Options.
   b. Autonomous QTouch Pin Configuration Options.
   c. QTouch Group A Pin Configuration Options.
   d. QTouch Group B Pin Configuration Options.
   e. Touch Sync Pin option.
*/

  touch_ret_t touch_ret = TOUCH_SUCCESS;
  touch_qt_dma_t qt_dma_ch;

  /* Initialize host clock, pins, watchdog, etc. */
  init_system ();

  /* Disable interrupts. */
  Disable_global_interrupt ();

  /* The INTC driver has to be used only for GNU GCC for AVR32. */
#if (defined __GNUC__)

  /* initialize interrupt vectors. */
  INTC_init_interrupts ();

  /* Register the Timer interrupt handler to the interrupt controller. */
  INTC_register_interrupt (&tc_irq, EXAMPLE_TC_IRQ, AVR32_INTC_INT0);

  /* Register the Touch Library CAT interrupt handler to the interrupt controller.
     Note: This interrupt registration is a MUST before using the Touch Library
     with the GCC compiler.

     For the case of IAR the registration of interrupt is automatically taken
     care by the compiler. The Touch Libary CAT interrupt level for the case
     of IAR is fixed to Interrupt level 3. */
  INTC_register_interrupt (&touch_acq_done_irq, AVR32_CAT_IRQ,
			   AVR32_INTC_INT3);

#endif

  /* Enable interrupts. */
  Enable_global_interrupt ();

  /* Configure timer to fire ISR regularly. */
  init_timer ();

  /* Initialize touch library and uc3l cat module for QTouch Group A operation. */
  touch_ret = touch_qt_sensors_init (TOUCH_QT_GRP_A, &touch_config);
  if (touch_ret != TOUCH_SUCCESS)
    {
      while (1u);		/* Check API Error return code. */
    }

#if DEF_TOUCH_QDEBUG_ENABLE == 1
  /* Initialize the debug interface. */
  QDebug_Init ();
#endif

  /* configure the touch library sensors. */
  touch_ret = config_qt_grp_a_touch_sensors ();
  if (touch_ret != TOUCH_SUCCESS)
    {
      while (1u);		/* Check API Error return code. */
    }

  /* Initialize touch sensing. */
  touch_ret = touch_qt_sensors_calibrate (TOUCH_QT_GRP_A);
  if (touch_ret != TOUCH_SUCCESS)
    {
      while (1u);		/* Check API Error return code. */
    }

  /* Provide the dma channel to be used by the CAT module.  For each
     acquisition cycle, any different dma channel from 0 to 11 can be provided.
     The touch library can handle a different dma channel for each call of the
     touch_qt_sensors_start_acquisition API. */
  qt_dma_ch = QTA_DMA_CHANNEL_0;


  /* Loop forever */
  for (;;)
    {
      /* Process touch library events. The touch_event_dispatcher API needs to
         be called as frequently as possible in order to have a good touch response. */
      touch_event_dispatcher ();

      if (time_to_measure_touch == 1u)
	{
	  /* Clear flag: it's time to measure touch */
	  time_to_measure_touch = 0u;

	  /* Start a touch sensors measurement process. */
	  touch_ret = touch_qt_sensors_start_acquisition (TOUCH_QT_GRP_A,
							  current_time_ms_touch,
							  qt_dma_ch,
							  NORMAL_ACQ_MODE,
							  touch_qta_measure_complete_callback);
	  if ((touch_ret != TOUCH_SUCCESS) &&
	      (touch_ret != TOUCH_ACQ_INCOMPLETE))
	    {
	      while (1);
	      /* Reaching this point can be due to -
	         1. The api has retured an error due to a invalid input parameter.
	         2. The api has been called during a invalid Touch Library state. */
	    }
	}


      /* Host application code goes here */


      if (qta_measurement_done_touch == 1u)
	{
	  /* Clear flag: QTouch Group A measurement complete. */
	  qta_measurement_done_touch = 0u;

#if DEF_TOUCH_QDEBUG_ENABLE == 1
	  /* QT600 two-way QDebug communication application Example. */
	  /* Process any commands received from QTouch Studio. */
	  QDebug_ProcessCommands ();

	  /* Send out the Touch debug information data each time when Touch
	     measurement process is completed . */
	  QDebug_SendData (p_qta_measure_data->acq_status);
#endif

	}
    }				/* Loop forever */

}

/*! \brief Example configuration for touch keys, rotors and sliders.
 *
 * \retval TOUCH_SUCCESS   Configuration setting successful.
 * \retval TOUCH_INVALID_xx   Invalid input configuration setting.
 */
touch_ret_t
config_qt_grp_a_touch_sensors (void)
{
  touch_ret_t touch_ret = TOUCH_SUCCESS;
  sensor_id_t sensor_id;

  touch_ret =
    touch_qt_sensor_config (TOUCH_QT_GRP_A, SENSOR_TYPE_ROTOR, (channel_t) 0u,
			    (channel_t) 2u, NO_AKS_GROUP, 20u,
			    (hysteresis_t) HYST_6_25, RES_8_BIT, &sensor_id);
  if (touch_ret != TOUCH_SUCCESS)
    {
      while (1);		/* Check API Error return code. */
    }

  touch_ret =
    touch_qt_sensor_config (TOUCH_QT_GRP_A, SENSOR_TYPE_SLIDER,
			    (channel_t) 3u, (channel_t) 5u, NO_AKS_GROUP, 20u,
			    (hysteresis_t) HYST_6_25, RES_8_BIT, &sensor_id);
  if (touch_ret != TOUCH_SUCCESS)
    {
      while (1);		/* Check API Error return code. */
    }


  /* configure touch key. */
  touch_ret =
    touch_qt_sensor_config (TOUCH_QT_GRP_A, SENSOR_TYPE_KEY, (channel_t) 6u,
			    (channel_t) 6u, NO_AKS_GROUP, 20u, HYST_6_25,
			    RES_1_BIT, &sensor_id);
  if (touch_ret != TOUCH_SUCCESS)
    {
      while (1);		/* Check API Error return code. */
    }

  /* configure touch key. */
  touch_ret =
    touch_qt_sensor_config (TOUCH_QT_GRP_A, SENSOR_TYPE_KEY, (channel_t) 7u,
			    (channel_t) 7u, NO_AKS_GROUP, 20u, HYST_6_25,
			    RES_1_BIT, &sensor_id);
  if (touch_ret != TOUCH_SUCCESS)
    {
      while (1);		/* Check API Error return code. */
    }

  return (touch_ret);
}


/*! \brief QTouch Group A measure complete callback function.
 *
 * This function is called by the library when the touch measurement
 * process for QTouch Group A sensors is completed.
 *
 * \param p_measure_data   Base address of touch_measure_data_t instance.
 * \note A touch_qta_measure_complete_callback() call signifies that fresh
 * values of touch status, rotor/slider position, measured signals,
 * references and Sensor data is available.
 * The QTouch Group A measured data can be accessed using the p_measure_data
 * pointer.
 */
void
touch_qta_measure_complete_callback (touch_measure_data_t * p_measure_data)
{
  /* Copy the measure data pointer. */
  p_qta_measure_data = p_measure_data;

  /* Dummy. To avoid compiler warning. */
  UNUSED(p_qta_measure_data);

  /* Set QTouch Group A measurement done flag to true. */
  qta_measurement_done_touch = 1u;
}

/*! \brief Initialize Clock.
 */
static unsigned long
init_clock (void)
{
  unsigned long ret_val = 0u;

  /*Configure the DFLL and switch the main clock source to the DFLL. */
  ret_val |= pcl_configure_clocks (&pcl_dfll_freq_param);

  return (ret_val);
}

/*! \brief Initialize host clock, pins, watchdog, etc.
 */
void
init_system (void)
{
  int32_t ret_val = 0u;

  /* 1. Configure and start the DFLL0 in open loop mode to generate a frequency of 96MHz.
     2. Set Flash Wait state.
     3. Configure CPU, PBA, PBB clock to 48MHz.
     4. Set up the GCLK_CAT for QMatrix operation.  */
  ret_val = init_clock ();
  if (ret_val != 0u)
    {
      while (1);		/* Clock configuration failed. */
    }

  /* Do any other system initialization. */

}

/*! \brief Configure timer ISR to fire regularly.
 */
void
init_timer (void)
{
  volatile avr32_tc_t *tc = EXAMPLE_TC;

  /* options for waveform generation. */
  static const tc_waveform_opt_t WAVEFORM_OPT = {
    .channel = EXAMPLE_TC_CHANNEL,	/* Channel selection. */

    .bswtrg = TC_EVT_EFFECT_NOOP,	/* Software trigger effect on TIOB. */
    .beevt = TC_EVT_EFFECT_NOOP,	/* External event effect on TIOB. */
    .bcpc = TC_EVT_EFFECT_NOOP,	/* RC compare effect on TIOB. */
    .bcpb = TC_EVT_EFFECT_NOOP,	/* RB compare effect on TIOB. */

    .aswtrg = TC_EVT_EFFECT_NOOP,	/* Software trigger effect on TIOA. */
    .aeevt = TC_EVT_EFFECT_NOOP,	/* External event effect on TIOA. */
    .acpc = TC_EVT_EFFECT_NOOP,	/* RC compare effect on TIOA: toggle. */
    .acpa = TC_EVT_EFFECT_NOOP,	/* RA compare effect on TIOA: toggle (other possibilities are none, set and clear). */

    .wavsel = TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER,	/* Waveform selection: Up mode with automatic trigger(reset) on RC compare. */
    .enetrg = 0u,		/* External event trigger enable. */
    .eevt = 0u,			/* External event selection. */
    .eevtedg = TC_SEL_NO_EDGE,	/* External event edge selection. */
    .cpcdis = 0u,		/* Counter disable when RC compare. */
    .cpcstop = 0u,		/* Counter clock stopped with RC compare. */

    .burst = 0u,		/* Burst signal selection. */
    .clki = 0u,			/* Clock inversion. */
    .tcclks = TC_CLOCK_SOURCE_TC3	/* Internal source clock 3, connected to fPBA / 8. */
  };

  static const tc_interrupt_t TC_INTERRUPT = {
    .etrgs = 0u,
    .ldrbs = 0u,
    .ldras = 0u,
    .cpcs = 1u,
    .cpbs = 0u,
    .cpas = 0u,
    .lovrs = 0u,
    .covfs = 0u
  };

  /* initialize the timer/counter. */
  tc_init_waveform (tc, &WAVEFORM_OPT);

  /* set the compare triggers. */
  tc_write_rc (tc, EXAMPLE_TC_CHANNEL, EXAMPLE_RC_VALUE);

  /* configure Timer interrupt. */
  tc_configure_interrupts (tc, EXAMPLE_TC_CHANNEL, &TC_INTERRUPT);

  /* start the timer/counter. */
  tc_start (tc, EXAMPLE_TC_CHANNEL);
}

/*! \brief Timer compare ISR.
 */
#if (defined __GNUC__)
__attribute__ ((__interrupt__))
#elif (defined __ICCAVR32__)
__interrupt
#endif
  static void
tc_irq (void)
{
  /* update the current time */
  current_time_ms_touch++;

  /* every 25th ms */
  if ((current_time_ms_touch % measurement_period_ms) == 0u)
    {
      /* set flag: it's time to measure touch */
      time_to_measure_touch = 1u;
    }

  /* clear the interrupt flag. This is a side effect of reading the TC SR. */
  tc_read_sr (EXAMPLE_TC, EXAMPLE_TC_CHANNEL);
}
