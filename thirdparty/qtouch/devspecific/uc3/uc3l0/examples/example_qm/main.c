/* This source file is part of the ATMEL QTouch Library Release 4.3.1 */
/**
 * \file
 *
 * \mainpage
 * QMatrix Example for QT600 UC3L0 board.
 *
 * This file (\ref main.c) contains the AT32UC3L QTouch Library API Example application
 * for QMatrix method using the Capacitive Touch (CAT) module on the
 * QT600 UC3L board.
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
#if defined(__ICCAVR__)
#include <intrinsics.h>
#endif
#include "compiler.h"
#include "board.h"

/**
 * Includes for Clock and Timer.
 */
#include "power_clocks_lib.h"
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
 * Default value: Every 25 milliseconds.
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
 * Flag set by touch_qm_measure_complete_callback() function when a fresh Touch
 * status is available.
 */
volatile uint8_t qm_measurement_done_touch = 0u;

/**
 * QMatrix measured data pointer.
 */
touch_measure_data_t *p_qm_measure_data = NULL;

/**
 * QMatrix burst length.
 *
 * Specify the number of QMatrix burst pulses for each channel.
 * In order to disable bursting on a individual channel, a value of 1u has
 * to be specified.
 *
 * CAUTION: QMatrix Burst length setting recommendation.
 * The following are the three possible Scenario's of Burst length usage when
 * using the UC3L CAT module.
 *
 * Scenario 1: The burst length of all channels has the same count value.
 *             Additionally, any or all of the (X, Y) pairs can have a
 *             count value of 1 (when a channel is to be disabled, the count
 *             value is set as 1).
 *
 * Scenario 2: For a given X Line, the count value of all enabled Y Lines MUST
 *             be of the same value.  Additionally, for this X Line, any or all
 *             of these (X, Y) pairs can have a value of 1.
 *
 *             Example, when X0, X1, X2 and Y0, Y1, Y2 lines are used in a design, then,
 *             X0Y0, X0Y1 and X0Y2 can have count values (48, 48, 48) or (48, 48, 1) or (1, 1, 48) etc respectively, 48 is an example value here.
 *             X1Y0, X1Y1 and X1Y2 can have count values (64, 64, 64) or (64, 64, 1) or (1, 1, 64) etc respectively, 64 is an example value here.
 *             X2Y0, X2Y1 and X2Y2 can have count values (32, 32, 32) or (32, 32, 1) or (1, 1, 32) etc respectively, 32 is an example value here.
 *
 * Scenario 3: It is recommended to have the burst length settings as given in
 *             Scenario 1 or Scenario 2.  However, in a case wherein it is required
 *             to have different count values on different channels, 1k ohm
 *             resistive drive MUST be used on all the enabled Y lines by setting
 *             the corresponding bits in the CSARES register.
 */
uint8_t qm_burst_length[QM_NUM_CHANNELS] = {
  64u,				/* x0, y0 */
  64u,				/* x1, y0 */
  64u,				/* x2, y0 */
  64u,				/* x3, y0 */
  112u,				/* x4, y0 */
  112u,				/* x5, y0 */
  64u,				/* x8, y0 */
  64u,				/* x9, y0 */
  64u,				/* x0, y1 */
  64u,				/* x1, y1 */
  64u,				/* x2, y1 */
  64u,				/* x3, y1 */
  112u,				/* x4, y1 */
  112u,				/* x5, y1 */
  64u,				/* x8, y1 */
  64u,				/* x9, y1 */
  64u,				/* x0, y2 */
  64u,				/* x1, y2 */
  64u,				/* x2, y2 */
  64u,				/* x3, y2 */
  112u,				/* x4, y2 */
  112u,				/* x5, y2 */
  64u,				/* x8, y2 */
  64u,				/* x9, y2 */
  64u,				/* x0, y3 */
  64u,				/* x1, y3 */
  64u,				/* x2, y3 */
  64u,				/* x3, y3 */
  112u,				/* x4, y3 */
  112u,				/* x5, y3 */
  64u,				/* x8, y3 */
  64u,				/* x9, y3 */
  64u,				/* x0, y4 */
  64u,				/* x1, y4 */
  64u,				/* x2, y4 */
  64u,				/* x3, y4 */
  112u,				/* x4, y4 */
  112u,				/* x5, y4 */
  64u,				/* x8, y4 */
  64u,				/* x9, y4 */
  64u,				/* x0, y5 */
  64u,				/* x1, y5 */
  64u,				/* x2, y5 */
  64u,				/* x3, y5 */
  112u,				/* x4, y5 */
  112u,				/* x5, y5 */
  64u,				/* x8, y5 */
  64u,				/* x9, y5 */
  64u,				/* x0, y6 */
  64u,				/* x1, y6 */
  64u,				/* x2, y6 */
  64u,				/* x3, y6 */
  112u,				/* x4, y6 */
  112u,				/* x5, y6 */
  64u,				/* x8, y6 */
  64u,				/* x9, y6 */
  64u,				/* x0, y7 */
  64u,				/* x1, y7 */
  64u,				/* x2, y7 */
  64u,				/* x3, y7 */
  112u,				/* x4, y7 */
  112u,				/* x5, y7 */
  64u,				/* x8, y7 */
  64u				/* x9, y7 */
};

/*----------------------------------------------------------------------------
                                extern variables
----------------------------------------------------------------------------*/
/*! \brief Example signal filtering function.
 *
 * \param p_filter_data   Base address of the touch_filter_data_t instance.
 */
void qm_filter_callback (touch_filter_data_t * p_filter_data);

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
 * Filter for channel signals.
 */
static uint16_t qm_filter[QM_NUM_CHANNELS][4];

/**
 * QMatrix Data block provided as input to Touch library.
 */
static uint8_t qm_data_blk[PRIV_QM_DATA_BLK_SIZE];

/**
 * QMatrix Configuration structure provided as input to Touch Library.
 *
 * Note: Use the touch_config_at32uc3l.h configuration header file to fill in
 * the elements of this structure.  DO NOT modify any of the input values
 * directly in this structure.
 */
static touch_qm_config_t qm_config = {
  QM_NUM_CHANNELS,		/* QMatrix number of channels. */
  QM_NUM_SENSORS,		/* QMatrix number of sensors. */
  QM_NUM_ROTORS_SLIDERS,	/* QMatrix number of rotors and sliders. */
  QM_NUM_X_LINES,		/* QMatrix number of X Lines. */
  QM_NUM_Y_LINES,		/* QMatrix number of Y-Yk pairs. */
  PRIV_QM_NUM_X_SENSE_PAIRS,	/* QMatrix number of X sense pairs. */
  PRIV_QM_BURST_LENGTH_WRITE_COUNT,	/* QMatrix burst length write count. */
  /* QMATRIX PIN CONFIGURATION INFO. */
  {
   QM_X_PINS_SELECTED,		/* X pins selected. */
   QM_Y_PAIRS_SELECTED,		/* Y-Yk pairs selected. */
   QM_SMP_DIS_PIN_OPTION,	/* specify the smp or dis pin option,
				   if included in the hardware design.
				   refer general_pin_options_t. */
   QM_VDIV_PIN_OPTION		/* specify the vdiv pin option,
				   if included in the hardware design.
				   refer general_pin_options_t. */
   },
  /* QMATRIX REGISTER CONFIGURATION INFO. */
  /* Refer CAT User Interface section in
     AT32UC3L datasheet. */
  /* TOUCH LIBRARY WILL SHIFT THE VALUE
     SPECIFIED BELOW TO THE REQUIRED
     REGISTER OFFSET */
  {
   QM_CAT_CLK_DIV,		/* uint16_t div; MGCFG0 Register. */
   QM_CHLEN,			/* uint8_t  chlen; MGCFG0 Register. */
   QM_SELEN,			/* uint8_t  selen; MGCFG0 Register. */
   QM_DISHIFT,			/* uint8_t  dishift; MGCFG1 Register. */
   QM_ENABLE_EXTERNAL_SYNC,	/* uint8_t  sync; MGCFG1 Register. */
   QM_ENABLE_SPREAD_SPECTRUM,	/* uint8_t  spread; MGCFG1 Register. */
   QM_DILEN,			/* uint8_t  dilen; MGCFG1 Register. */
   QM_MAX_ACQ_COUNT,		/* uint16_t max; MGCFG1 Register. */
   1u,				/* uint8_t  acctrl; MGCFG2 Register. */
   QM_CONSEN,			/* uint8_t  consen; MGCFG2 Register. */
   QM_CXDILEN,			/* uint8_t  cxdilen; MGCFG2 Register. */
   QM_SYNC_TIM,			/* uint16_t synctim; MGCFG2 Register. */
   0u,				/* uint8_t  fsources; DICS Register. When SMP mode is used, this option is set to 0u by the Touch Library. */
   1u,				/* uint8_t  glen; DICS Register. When SMP mode is used, this option is set to 0u by the Touch Library.  */
   QM_INTVREFSEL,		/* uint8_t  intvrefsel; DICS Register. When SMP mode is used, this option is set to 0u by the Touch Library.  */
   QM_INTREFSEL,		/* uint8_t  intrefsel; DICS Register. When SMP mode is used, this option is set to 0u by the Touch Library.  */
   0u,				/* uint8_t  trim; DICS Register. When SMP mode is used, this option is set to 0u by the Touch Library.  */
   0xFFu,			/* uint8_t  sources; DICS Register. When SMP mode is used, this option is set to 0u by the Touch Library.  */
   0u,				/* uint16_t shival0; ACSHIx Register. */
   0u,				/* uint16_t shival1; ACSHIx Register. */
   0u,				/* uint16_t shival2; ACSHIx Register. */
   0u,				/* uint16_t shival3; ACSHIx Register. */
   0u,				/* uint16_t shival4; ACSHIx Register. */
   0u,				/* uint16_t shival5; ACSHIx Register. */
   0u,				/* uint16_t shival6; ACSHIx Register. */
   0u,				/* uint16_t shival7; ACSHIx Register. */
   },
  /* QMATRIX GLOBAL SENSOR CONFIGURATION INFO. */
  {
   QM_DI,			/* uint8_t  di; Sensor detect integration (DI) limit. */
   QM_NEG_DRIFT_RATE,		/* uint8_t  neg_drift_rate; Sensor negative drift rate. */
   QM_POS_DRIFT_RATE,		/* uint8_t  pos_drift_rate; Sensor positive drift rate. */
   QM_MAX_ON_DURATION,		/* uint8_t  max_on_duration; Sensor maximum on duration. */
   QM_DRIFT_HOLD_TIME,		/* uint8_t  drift_hold_time; Sensor drift hold time. */
   QM_POS_RECAL_DELAY,		/* uint8_t  pos_recal_delay; Sensor minimum positive recalibration delay. */
   QM_RECAL_THRESHOLD,		/* recal_threshold_t recal_threshold; Sensor recalibration threshold. */
   },
  qm_data_blk,			/* QMatrix data block index. */
  PRIV_QM_DATA_BLK_SIZE,	/* QMatrix data block size. */
  qm_burst_length,		/* QMatrix burst length array starting index. */
  QM_FILTER_CALLBACK,		/* QMatrix filter callback function pointer. */
};

/**
 * General configuration options common to QMatrix, QTouch and Autonomous Touch
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
  TOUCH_SPREAD_SPECTRUM_MAX_DEV,	/* uint8_t  maxdev; SSCFG Register. Max deviation of spread spectrum.
					   Note: MAXDEV should not exceed (2*DIV + 1). div - clock divider. */
  TOUCH_CSARES,			/* uint32_t csares; CSA Resistor control register. */
  TOUCH_CSBRES			/* uint32_t csbres;  CSA Resistor control register. */
};

/**
 * Touch Library input configuration structure.
 */
touch_config_t touch_config = {
  &qm_config,			/* Pointer to QMatrix configuration structure. */
  NULL,				/* Pointer to Autonomous Touch configuration structure. */
  NULL,				/* Pointer to QTouch Group A configuration structure. */
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

/*! \brief Configure timer to fire ISR regularly.
 */
static void init_timer (void);

/*! \brief Example configuration for touch keys, rotors and sliders.
 */
static touch_ret_t config_64ch_touch_keys_rotors_sliders (void);

/*! \brief Timer ISR.
 */
#if defined(__AVR32__)
static void tc_irq (void);
#elif defined (__ICCAVR32__)
#pragma handler = EXAMPLE_TC_IRQ_GROUP, 1
__interrupt static void tc_irq (void);
#endif

/*! \brief QMatrix measure complete callback function example prototype.
 */
static void touch_qm_measure_complete_callback (touch_measure_data_t *
						p_measure_data);

/*----------------------------------------------------------------------------
  static variables
----------------------------------------------------------------------------*/

/*! \brief Example application entry function.
 */
int
main (void)
{
/*
    BEFORE USING THE EXAMPLE PROJECTS.

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
   changed accordingly to ensure proper QMatrix operation.
   a. QM_GCLK_CAT_DIV.
   b. QM_CAT_CLK_DIV.
   c. TOUCH_SPREAD_SPECTRUM_MAX_DEV, when Spread spectrum is enabled.
   d. PBA_HZ, when using qdebug/SPI_Master.c
   e. TARGET_PBA_FREQ_HZ and TARGET_CPU_FREQ_HZ, when using qdebug/SERIAL.c

4. The VTarget voltage in the USB Bridge must be set to 3.3V.

5. The QTouch library uses PDCA channels 0 and 1.
   (QM_DMA_CHANNEL_0, QM_DMA_CHANNEL_1).

6. For QMatrix operation, the Analog comparators channels are used (using the
   ACIFB interface) depending on the Y Lines enabled.  For example, when
   Y lines Y2 and Y7 are enabled the Analog comparator channels 2 and 7
   are used by the CAT module for QMatrix operation.  The user can uses the rest
   of the Analog comparator channels in the main application. The QTouch Library
   enables the ACIFB using the Control register (if not already enabled by the main
   application) when the touch_qm_sensors_init API is called.

7. When two or more acquisition methods are used, care must be taken such that a
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
  touch_qm_dma_t qm_dma;

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

  /* Initialize touch library and uc3l cat module for QMatrix operation.
     Note: Set up the GCLK_CAT for proper QMatrix operation.  Refer init_system(). */
  touch_ret = touch_qm_sensors_init (&touch_config);
  if (touch_ret != TOUCH_SUCCESS)
    {
      while (1u);		/* Check API Error return code. */
    }

#if DEF_TOUCH_QDEBUG_ENABLE == 1
  /* Initialize the debug interface. */
  QDebug_Init ();
#endif

  /* Configure the touch library sensors. */
  touch_ret = config_64ch_touch_keys_rotors_sliders ();
  if (touch_ret != TOUCH_SUCCESS)
    {
      while (1u);		/* Check API Error return code. */
    }

  /* Initialize touch sensing. */
  touch_ret = touch_qm_sensors_calibrate ();
  if (touch_ret != TOUCH_SUCCESS)
    {
      while (1u);		/* Check API Error return code. */
    }

  /* Provide the dma channels to be used by the CAT module.  for each
     acquisition cycle, any different combination of dma channels from 0 to 11
     can be provided. The touch library can also handle a different combination
     of dma channels for each call of the touch_qm_sensors_start_acquisition API. */
  qm_dma.dma_ch1 = QM_DMA_CHANNEL_0;
  qm_dma.dma_ch2 = QM_DMA_CHANNEL_1;

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
	  touch_ret =
	    touch_qm_sensors_start_acquisition (current_time_ms_touch,
						&qm_dma, NORMAL_ACQ_MODE,
						touch_qm_measure_complete_callback);
	  if ((touch_ret != TOUCH_SUCCESS)
	      && (touch_ret != TOUCH_ACQ_INCOMPLETE))
	    {
	      while (1u);
	      /* Reaching this point can be due to -
	         1. The api has retured an error due to a invalid input parameter.
	         2. The api has been called during a invalid Touch Library state. */
	    }
	}

      /* Host application code goes here */


      if (qm_measurement_done_touch == 1u)
	{
	  /* Clear flag: qm measurement done */
	  qm_measurement_done_touch = 0u;

#if DEF_TOUCH_QDEBUG_ENABLE == 1
	  /* QT600 two-way QDebug communication application Example. */
	  /* Process any commands received from QTouch Studio. */
	  QDebug_ProcessCommands ();

	  /* Send out the Touch debug information data each time when Touch
	     measurement process is completed . */
	  QDebug_SendData (p_qm_measure_data->acq_status);
#endif

	}



    }
}

/*! \brief Example signal filtering function.
 *
 * Mean Filter function.
 * \param p_filter_data   Base address of the touch_filter_data_t instance.
 */
void
qm_filter_callback (touch_filter_data_t * p_filter_data)
{
  /* Shift previously stored channel signal data.
     Store new channel signal data.
     Set library channel signal data = mean of last 4 values. */
  uint8_t i;

  for (i = 0u; i < p_filter_data->num_channel_signals; i++)
    {
      qm_filter[i][0] = qm_filter[i][1];
      qm_filter[i][1] = qm_filter[i][2];
      qm_filter[i][2] = qm_filter[i][3];
      qm_filter[i][3] = p_filter_data->p_channel_signals[i];

      p_filter_data->p_channel_signals[i] = ((qm_filter[i][0] +
					      qm_filter[i][1] +
					      qm_filter[i][2] +
					      qm_filter[i][3]) / 4u);
    }
}

/*! \brief Example configuration for touch keys, rotors and sliders.
 *
 * \retval TOUCH_SUCCESS   Configuration setting successful.
 * \retval TOUCH_INVALID_xx   Invalid input configuration setting.
 */
touch_ret_t
config_64ch_touch_keys_rotors_sliders (void)
{
  touch_ret_t touch_ret = TOUCH_SUCCESS;
  sensor_id_t sensor_id;
  uint8_t i;

  /* Configure a 4 channel Rotor as Sensor 0. */
  touch_ret =
    touch_qm_sensor_config (SENSOR_TYPE_ROTOR, (channel_t) 0u, (channel_t) 3u,
			    AKS_GROUP_1, 40u, HYST_6_25, RES_8_BIT, 0u,
			    &sensor_id);
  if (touch_ret != TOUCH_SUCCESS)
    {
      while (1u);		/* Check API Error return code. */
    }

  /* Configure a 4 channel Slider as Sensor 1. */
  touch_ret =
    touch_qm_sensor_config (SENSOR_TYPE_SLIDER, (channel_t) 4u,
			    (channel_t) 7u, AKS_GROUP_1, 40u, HYST_6_25,
			    RES_8_BIT, 0u, &sensor_id);
  if (touch_ret != TOUCH_SUCCESS)
    {
      while (1u);		/* Check API Error return code. */
    }

  /* Configure 8 single channel keys as Sensor's 2-9. */
  for (i = 8u; i < 16u; i++)
    {
      touch_ret =
	touch_qm_sensor_config (SENSOR_TYPE_KEY, (channel_t) i, (channel_t) i,
				AKS_GROUP_1, 25u, HYST_6_25, RES_1_BIT, 0u,
				&sensor_id);
      if (touch_ret != TOUCH_SUCCESS)
	{
	  while (1u);		/* Check API Error return code. */
	}
    }

  /* Configure a 4 channel Rotor as Sensor 10. */
  touch_ret =
    touch_qm_sensor_config (SENSOR_TYPE_ROTOR, (channel_t) 16u,
			    (channel_t) 19u, AKS_GROUP_1, 40u, HYST_6_25,
			    RES_8_BIT, 0u, &sensor_id);
  if (touch_ret != TOUCH_SUCCESS)
    {
      while (1u);		/* Check API Error return code. */
    }

  /* Configure a 4 channel Slider as Sensor 11. */
  touch_ret =
    touch_qm_sensor_config (SENSOR_TYPE_SLIDER, (channel_t) 20u,
			    (channel_t) 23u, AKS_GROUP_1, 40u, HYST_6_25,
			    RES_8_BIT, 0u, &sensor_id);
  if (touch_ret != TOUCH_SUCCESS)
    {
      while (1u);		/* Check API Error return code. */
    }

  /* Configure 8 single channel keys as Sensor's 12-19. */
  for (i = 24u; i < 32u; i++)
    {
      touch_ret =
	touch_qm_sensor_config (SENSOR_TYPE_KEY, (channel_t) i, (channel_t) i,
				AKS_GROUP_1, 25u, HYST_6_25, RES_1_BIT, 0u,
				&sensor_id);
      if (touch_ret != TOUCH_SUCCESS)
	{
	  while (1u);		/* Check API Error return code. */
	}
    }

  /* Configure a 4 channel Rotor as Sensor 20. */
  touch_ret =
    touch_qm_sensor_config (SENSOR_TYPE_ROTOR, (channel_t) 32u,
			    (channel_t) 35u, AKS_GROUP_1, 40u, HYST_6_25,
			    RES_8_BIT, 0u, &sensor_id);
  if (touch_ret != TOUCH_SUCCESS)
    {
      while (1u);		/* Check API Error return code. */
    }

  /* Configure a 4 channel Slider as Sensor 21. */
  touch_ret =
    touch_qm_sensor_config (SENSOR_TYPE_SLIDER, (channel_t) 36u,
			    (channel_t) 39u, AKS_GROUP_1, 40u, HYST_6_25,
			    RES_8_BIT, 0u, &sensor_id);
  if (touch_ret != TOUCH_SUCCESS)
    {
      while (1u);		/* Check API Error return code. */
    }

  /* Configure 8 single channel keys as Sensor's 22-29. */
  for (i = 40; i < 48; i++)
    {
      touch_ret =
	touch_qm_sensor_config (SENSOR_TYPE_KEY, (channel_t) i, (channel_t) i,
				AKS_GROUP_1, 25u, HYST_6_25, RES_1_BIT, 0u,
				&sensor_id);
      if (touch_ret != TOUCH_SUCCESS)
	{
	  while (1u);		/* Check API Error return code. */
	}
    }

  /* Configure a 4 channel Rotor as Sensor 30. */
  touch_ret =
    touch_qm_sensor_config (SENSOR_TYPE_ROTOR, (channel_t) 48u,
			    (channel_t) 51u, AKS_GROUP_1, 40u, HYST_6_25,
			    RES_8_BIT, 0u, &sensor_id);
  if (touch_ret != TOUCH_SUCCESS)
    {
      while (1u);		/* Check API Error return code. */
    }

  /* Configure a 4 channel Slider as Sensor 31. */
  touch_ret =
    touch_qm_sensor_config (SENSOR_TYPE_SLIDER, (channel_t) 52u,
			    (channel_t) 55u, AKS_GROUP_1, 40u, HYST_6_25,
			    RES_8_BIT, 0u, &sensor_id);
  if (touch_ret != TOUCH_SUCCESS)
    {
      while (1u);		/* Check API Error return code. */
    }

  /* Configure 8 single channel keys as Sensor's 32-39. */
  for (i = 56u; i < 64u; i++)
    {
      touch_ret =
	touch_qm_sensor_config (SENSOR_TYPE_KEY, (channel_t) i, (channel_t) i,
				AKS_GROUP_1, 25u, HYST_6_25, RES_1_BIT, 0u,
				&sensor_id);
      if (touch_ret != TOUCH_SUCCESS)
	{
	  while (1u);		/* Check API Error return code. */
	}
    }
  return (touch_ret);
}

/*! \brief QMatrix measure complete callback function.
 *
 * This function is called by the library when the touch measurement
 * process for QMatrix Sensors is completed.
 *
 * \param p_measure_data   Base address of touch_measure_data_t instance.
 * \note A touch_qm_measure_complete_callback() call signifies that fresh
 * values of touch status, rotor/slider position, measured signals,
 * references and Sensor data is available.
 * The QMatrix measured data can be accessed using the p_measure_data
 * pointer.
 */
void
touch_qm_measure_complete_callback (touch_measure_data_t * p_measure_data)
{
  /* Copy the QMatrix measure data pointer. */
  p_qm_measure_data = p_measure_data;

  /* Set the QMatrix measurement done flag. */
  qm_measurement_done_touch = 1u;
}

/*! \brief Initialize Clock.
 */
static unsigned long
init_clock (void)
{
  unsigned long ret_val = 0u;

  /*Configure the DFLL and switch the main clock source to the DFLL. */
  ret_val |= pcl_configure_clocks (&pcl_dfll_freq_param);

#if DEF_TOUCH_QMATRIX == 1
  /* Set up the GCLK_CAT for proper QMatrix operation.  The GCLK_CAT is not
     required to be setup for Autonomous Touch and QTouch Group A/B operation.
     Note: for UC3L devices, the generic clock configurations are handled by the
     SCIF module. Setup gc to use the DFLL0 as source clock, divisor enabled, apply
     a division factor. */
  ret_val |=
    scif_gc_setup (AVR32_SCIF_GCLK_CAT, SCIF_GCCTRL_DFLL0, AVR32_GC_DIV_CLOCK,
		   QM_GCLK_CAT_DIV);

  /* Now enable the generic clock */
  ret_val |= scif_gc_enable (AVR32_SCIF_GCLK_CAT);
#endif

  return (ret_val);
}

/*! \brief Initialize host clock, pins, watchdog, etc.
 */
void
init_system (void)
{
  int32_t ret_val = 0u;

  /* 1. Configure and start the DFLL0 in closed loop mode to generate a frequency of 96MHz.
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
