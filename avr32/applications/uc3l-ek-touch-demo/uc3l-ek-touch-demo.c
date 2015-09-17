/**
 * \file uc3l-ek-touch-demo.c
 *
 * \brief  This file contains the AT32UC3L-EK demonstration application that
 * interfaces with QTouchStudio and controls the LEDs with the touch sensors.
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

/**
 * \mainpage
 *
 * \section intro Introduction
 * This application is the default AT32UC3L-EK demonstration application that
 * interfaces with QTouchStudio and controls the LEDs with the touch sensors. It
 * is using the Atmel QTouch Libraries for the AVR UC3 L0 series.
 *
 * \section files Main files:
 * - uc3l-ek-touch-demo.c: main file with
 *   - setup the clocks,
 *   - setup a timer and its interrupt handler,
 *   - setup the QTouch lib,
 *   - setup the QDebug interface,
 *   - setup and calibrate the QTouch lib sensors,
 *   - setup the demo
 *   - main loop.
 * - demo.c: demo state-machine.
 * - touch_api_at32uc3l.h: the AT32UC3L QTouch Library API interface for
 *   QMatrix, QTouch and Autonomous Touch method using the Capacitive Touch
 *   (CAT) module.
 * - touch_config_at32uc3l.h: the AT32UC3L Touch Library pin, register and
 *   sensors configuration options for QMatrix, Autonomous QTouch and
 *   QTouch Group A/B method acquisition using the Capacitive Touch
 *   (CAT) module. The configuration is tailored for the AT32UC3L-EK.
 * - QDebugSettings.h: QTouch Debug Interface configuration for AT32UC3L-EK
 *
 * \section deviceinfo Device Info
 * This application is targeted for the AT32UC3L-EK board only.
 *
 * \section exampledescription Description of the example
 * - Prologue of the demo: all 4 LEDs blink 3 times simultaneously with PWM
 * fading effect.
 *
 * - The wheel engages the GPIO PWM to create a 256-level "bar" on the 4 LEDs.
 * Rotating clockwise will gradually light up the bottom led in 64 steps, then
 * the LED above it will begin to glow, and so on until all 4 LEDs are fully lit.
 * Turning another clockwise round will have no effect. Turning anticlockwise one
 * round will make it gradually decrease the 256-level counter, thus causing the
 * bar to fade downwards until you hit 0.
 *
 * - Pushing Play/pause starts an automatic running lights sequence on all 4 LEDs.
 * The PWM is used to create fading effect.
 * Use the Up and Down arrows to increase or decrease the speed of the automatic
 * running lights sequence.
 *
 * - The Up and Down arrows are used to turn the 4 LEDs fully on (quick push on up),
 * off (quick push on down), and gradually fading on and off (keep up or down pushed
 * for 0.5 seconds, use 2.5 seconds to go from rail to rail, like a dimming light
 * switch).
 * While the automatic running lights sequence is active, the Up and Down arrows
 * are used to increase or decrease the speed of that sequence.
 *
 * - The Right arrow engages the GPIO PWM to create a 256-level "bar" on the 4 LEDs.
 * Keep the right arrow pushed to gradually light up the bottom LED in 64 steps,
 * then the LED above it will begin to glow, and so on until all 4 LEDs are fully
 * lit.
 * Keep the Left arrow pushed to gradually decrease the 256-level counter, thus
 * causing the bar to fade downwards until you hit 0.
 *
 * - Interface with AVR QTouchStudio: when the USB mode is in QDebug mode (USB
 * HID class that communicates with QTouch Studio), all touch events are
 * reported to QTouch Studio. Open QTouchStudio: a drawing of the UC3L-EK should
 *  appear. Click on the "start reading" button. All touch events on the board
 * appear on the UC3L-EK drawing on QTouch Studio (a green color is a match).
 * \note The AT32UC3L-EK supports two USB modes: the CDC Virtual Com Port and
 * the QDebug mode. Change of the USB mode is done upon power-up of the board by
 * pressing the USB mode button.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for AVR.
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/">Atmel</A>.\n
 */

/*----------------------------------------------------------------------------
                            compiler information
----------------------------------------------------------------------------*/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#if !((defined(__GNUC__) && defined(__AVR32__)) || (defined(__ICCAVR32__) || defined(__AAVR32__)))
#error 'This compiler is not supported at the moment.'
#endif

/*----------------------------------------------------------------------------
                                include files
----------------------------------------------------------------------------*/
#if defined(__ICCAVR__)
#include <intrinsics.h>
#endif
#include "board.h"
#include "conf_clock.h"

#include "tc.h"
#if defined(__GNUC__)
#include "intc.h"
#endif
#include "gpio.h"
#include "delay.h"

#include "power_clocks_lib.h"

/* Includes for Touch Library. */
#include "touch_api_at32uc3l.h"

#if DEF_TOUCH_QDEBUG_ENABLE == 1
  /* Includes for Touch Debug interface. */
  #include "qdebug.h"
#endif

/* Includes for the demo */
#include "demo.h"

/*----------------------------------------------------------------------------
                            manifest constants
----------------------------------------------------------------------------*/
/* Example Timer configuration settings. */
#define EXAMPLE_TC                       (&AVR32_TC0)
#define EXAMPLE_TC_IRQ_GROUP             (AVR32_TC0_IRQ_GROUP)
#define EXAMPLE_TC_IRQ                   (AVR32_TC0_IRQ0)
#define EXAMPLE_TC_CHANNEL               (0u)
#define EXAMPLE_RC_VALUE                 (6000u)                                /* configure timer for 1ms tick. */
#define EXAMPLE_MEASUREMENT_PERIOD_MS    (25u)                                  /* The timing information for timer to fire periodically to measure touch. */

#define EXAMPLE_DFLL_COARSE_FDFLL96      (150u)
#define EXAMPLE_DFLL_FINE_FDFLL96        (65u)

//! Number of automatic LEDs intensity changes done at the beginning of the demo.
#define DEMO_INIT_NB_AUTOMATIC_CHANGES  5


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

/* Touch Measurement period in milliseconds.  This variable determines how
   often a new touch measurement must be done.
   Default value: Every 25 milliseconds. */
uint16_t measurement_period_ms = EXAMPLE_MEASUREMENT_PERIOD_MS;

/* Flag set by timer ISR when it's time to measure touch */
volatile uint8_t time_to_measure_touch = 0u;

/* Current time, set by timer ISR */
volatile uint16_t current_time_ms_touch = 0u;

/* Flag set by touch_qm_measure_complete_callback() function when a fresh Touch
   status is available. */
volatile uint8_t qm_measurement_done_touch = 0u;

/* QMatrix measured data pointer. */
touch_measure_data_t *p_qm_measure_data = NULL;

/*! \name Parameters to pcl_configure_clocks().
 */
//! @{
static scif_gclk_opt_t gc_dfllif_ref_opt = { SCIF_GCCTRL_SLOWCLOCK, 0, false};
static pcl_freq_param_t pcl_dfll_freq_param = {
	.main_clk_src = PCL_MC_DFLL0,
	.cpu_f        = EXAMPLE_TARGET_MCUCLK_FREQ_HZ,
	.pba_f        = EXAMPLE_TARGET_PBACLK_FREQ_HZ,
	.pbb_f        = EXAMPLE_TARGET_PBACLK_FREQ_HZ,
	.dfll_f       = EXAMPLE_TARGET_DFLL_FREQ_HZ,
	.pextra_params = &gc_dfllif_ref_opt
};
//! @}

/*
 * QMatrix burst length.
 *
 * Specify the number of QMatrix burst pulses for each channel.
 * In order to disable bursting on a individual channel, a value of 1u has
 * to be specified.
 *
 * QMatrix burst length setting recommendation.
 * For a given X Line, the burst length value of ALL enabled Y Lines MUST be the
 * same or set to 0x01(disabled).  For example, the burst length value corresponding
 * to (X0,Y1),(X0,Y2)...(X0,Yn) must be the same.  In case of a scenario, wherein
 * it is required to have a different a burst length, then the following option
 * can be tried out - Enable the 1k ohm drive resistors on all the enabled Y lines
 * by setting the corresponding bit in the CSARES register.
 *
 * Note: A value of 0u will be same as specifying a burst length value of 255u pulses.
 * Range: 1u to 255u.
 *
 * Important Note: Once the Touch Library has been initialized for QMatrix method
 * using the touch_qm_sensors_init API, a new qm_burst_length[x] value of a Touch channel
 * MUST be updated only using the touch_qm_channel_update_burstlen API.  It is
 * recommended to have qm_burst_length array as global variable as the Touch Library
 * updates this array when the touch_qm_channel_update_burstlen API is called.
 */
const uint8_t qm_burst_length[QM_NUM_CHANNELS] =
{
  128, /* x6 , y2 */   // Rotor
  128, /* x7, y2 */    // Rotor
  128, /* x10, y2 */   // Rotor
  128, /* x11, y2 */   // Rotor
  128, /* x14, y2 */   // Rotor
  128, /* x15 , y2 */  // Rotor
  128, /* x6, y7 */    // Touch Key down (v)
  128, /* x7, y7 */    // Touch Key up (^)
  128, /* x10, y7 */   // Touch Key left (<)
    1, /* x11, y7 */   // No connections
  128, /* x14, y7 */   // Touch Key play/pause (>/||)
  128  /* x15, y7 */   // Touch Key right (>)
};

/*----------------------------------------------------------------------------
                                extern variables
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
                                static variables
----------------------------------------------------------------------------*/
/* QMatrix Data block provided as input to Touch library. */
static uint8_t qm_data_blk[PRIV_QM_DATA_BLK_SIZE];

/*
 * QMatrix Configuration structure provided as input to Touch Library.
 *
 * Note: Use the touch_config_at32uc3l.h configuration header file to fill in
 * the elements of this structure.  DO NOT modify any of the input values
 * directly in this structure.
 */
static touch_qm_config_t qm_config =
{
  QM_NUM_CHANNELS,                                                              /* QMatrix number of channels. */
  QM_NUM_SENSORS,                                                               /* QMatrix number of sensors. */
  QM_NUM_ROTORS_SLIDERS,                                                        /* QMatrix number of rotors and sliders. */
  QM_NUM_X_LINES,                                                               /* QMatrix number of X Lines. */
  QM_NUM_Y_LINES,                                                               /* QMatrix number of Y-Yk pairs. */
  PRIV_QM_NUM_X_SENSE_PAIRS,                                                    /* QMatrix number of X sense pairs. */
  PRIV_QM_BURST_LENGTH_WRITE_COUNT,                                             /* QMatrix burst length write count. */
  /* QMATRIX PIN CONFIGURATION INFO. */
  {
    QM_X_PINS_SELECTED,                                                         /* X pins selected. */
    QM_Y_PAIRS_SELECTED,                                                        /* Y-Yk pairs selected. */
    QM_SMP_DIS_PIN_OPTION,                                                      /* specify the smp or dis pin option,
                                                                                   if included in the hardware design.
                                                                                   refer general_pin_options_t. */
    QM_VDIV_PIN_OPTION                                                          /* specify the vdiv pin option,
                                                                                   if included in the hardware design.
                                                                                   refer general_pin_options_t. */
  },
  /* QMATRIX REGISTER CONFIGURATION INFO.*/
                                                                                /* Refer CAT User Interface section in
                                                                                   AT32UC3L datasheet. */
                                                                                /* TOUCH LIBRARY WILL SHIFT THE VALUE
                                                                                   SPECIFIED BELOW TO THE REQUIRED
                                                                                   REGISTER OFFSET */
  {
    QM_CAT_CLK_DIV,                                                             /* uint16_t div; MGCFG0 Register. */
    QM_CHLEN,                                                                   /* uint8_t  chlen; MGCFG0 Register. */
    QM_SELEN,                                                                   /* uint8_t  selen; MGCFG0 Register. */
    QM_DISHIFT,                                                                 /* uint8_t  dishift; MGCFG1 Register. */
    QM_ENABLE_EXTERNAL_SYNC,                                                    /* uint8_t  sync; MGCFG1 Register. */
    QM_ENABLE_SPREAD_SPECTRUM,                                                  /* uint8_t  spread; MGCFG1 Register. */
    QM_DILEN,                                                                   /* uint8_t  dilen; MGCFG1 Register. */
    QM_MAX_ACQ_COUNT,                                                           /* uint16_t max; MGCFG1 Register. */
    1u,                                                                         /* uint8_t  acctrl; MGCFG2 Register. */
    QM_CONSEN,                                                                  /* uint8_t  consen; MGCFG2 Register. */
    QM_CXDILEN,                                                                 /* uint8_t  cxdilen; MGCFG2 Register. */
    QM_SYNC_TIM,                                                                /* uint16_t synctim; MGCFG2 Register. */
    0u,                                                                         /* uint8_t  fsources; DICS Register. When SMP mode is used, this option is set to 0u by the Touch Library. */
    1u,                                                                         /* uint8_t  glen; DICS Register. When SMP mode is used, this option is set to 0u by the Touch Library.  */
    QM_INTVREFSEL,                                                              /* uint8_t  intvrefsel; DICS Register. When SMP mode is used, this option is set to 0u by the Touch Library.  */
    QM_INTREFSEL,                                                               /* uint8_t  intrefsel; DICS Register. When SMP mode is used, this option is set to 0u by the Touch Library.  */
    0u,                                                                         /* uint8_t  trim; DICS Register. When SMP mode is used, this option is set to 0u by the Touch Library.  */
    0xFFu,                                                                      /* uint8_t  sources; DICS Register. When SMP mode is used, this option is set to 0u by the Touch Library.  */
    0u,                                                                         /* uint16_t shival0; ACSHIx Register.*/
    0u,                                                                         /* uint16_t shival1; ACSHIx Register. */
    0u,                                                                         /* uint16_t shival2; ACSHIx Register. */
    0u,                                                                         /* uint16_t shival3; ACSHIx Register. */
    0u,                                                                         /* uint16_t shival4; ACSHIx Register. */
    0u,                                                                         /* uint16_t shival5; ACSHIx Register. */
    0u,                                                                         /* uint16_t shival6; ACSHIx Register. */
    0u,                                                                         /* uint16_t shival7; ACSHIx Register. */
  },
  /* QMATRIX GLOBAL SENSOR CONFIGURATION INFO. */
  {
    QM_DI,                                                                      /* uint8_t  di; Sensor detect integration (DI) limit. */
    QM_NEG_DRIFT_RATE,                                                          /* uint8_t  neg_drift_rate; Sensor negative drift rate. */
    QM_POS_DRIFT_RATE,                                                          /* uint8_t  pos_drift_rate; Sensor positive drift rate. */
    QM_MAX_ON_DURATION,                                                         /* uint8_t  max_on_duration; Sensor maximum on duration. */
    QM_DRIFT_HOLD_TIME,                                                         /* uint8_t  drift_hold_time; Sensor drift hold time. */
    QM_POS_RECAL_DELAY,                                                         /* uint8_t  pos_recal_delay; Sensor positive recalibration delay. */
    QM_RECAL_THRESHOLD,                                                         /* recal_threshold_t recal_threshold; Sensor recalibration threshold. */
  },
  qm_data_blk,                                                                  /* QMatrix data block index. */
  PRIV_QM_DATA_BLK_SIZE,                                                        /* QMatrix data block size. */
  (uint8_t *)qm_burst_length,                                                   /* QMatrix burst length array starting index. */
  QM_FILTER_CALLBACK                                                            /* QMatrix filter callback function pointer. */
};

/*
 * General configuration options common to QMatrix, QTouch and Autonomous Touch
 * provided as input to Touch library.
 *
 * Note: Use the touch_config_at32uc3l.h configuration header file to fill in
 * the elements of this structure.  DO NOT modify any of the input values
 * directly in this structure.
 */
static touch_general_config_t general_config =
{
  TOUCH_SYNC_PIN_OPTION,                                                        /* Specify the sync pin option,
                                                                                   if included in the hardware design.
                                                                                   refer general_pin_options_t. */
  TOUCH_SPREAD_SPECTRUM_MAX_DEV,                                                /* uint8_t  maxdev; SSCFG Register. Max deviation of spread spectrum. */
  TOUCH_CSARES,                                                                 /* uint32_t csares; CSA Resistor control register. */
  TOUCH_CSBRES                                                                  /* uint32_t csbres;  CSA Resistor control register. */
};

/* Touch Library input configuration structure. */
touch_config_t touch_config =
{
  &qm_config,                                                                   /* Pointer to QMatrix configuration structure. */
  NULL,                                                                         /* Pointer to Autonomous Touch configuration structure. */
  NULL,                                                                         /* Pointer to QTouch Group A configuration structure. */
  NULL,                                                                         /* Pointer to QTouch Group B configuration structure. */
  &general_config                                                               /* Pointer to General configuration structure. */
};

/*----------------------------------------------------------------------------
                                prototypes
----------------------------------------------------------------------------*/
/* Initialize host app, pins, watchdog, clock etc */
static void init_system( void );

/* Configure timer to fire ISR regularly */
static void init_timer( void );

/* configure touch library for keys, rotors and sliders. */
static touch_ret_t config_uc3lek_touch_sensors( void );

/* timer ISR */
#if defined(__AVR32__)
  __attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
  #pragma handler = EXAMPLE_TC_IRQ_GROUP, 1
  __interrupt
#endif
static void tc_irq(void);

/* QMatrix measure complete callback function example prototype. */
static void touch_qm_measure_complete_callback ( touch_measure_data_t *p_measure_data );

/*============================================================================
Name    :   main
------------------------------------------------------------------------------
Purpose :   main code entry point.
Input   :   n/a
Output  :   n/a
Notes   :
============================================================================*/
int main( void )
{

/*  BEFORE USING THE EXAMPLE PROJECTS.

1. The Example application uses a CPU, PBA and PBB clock of 48MHz.
   When using a different frequency setting, the following parameters must be
   changed accordingly to ensure proper QMatrix operation.
   a. QM_GCLK_CAT_DIV.
   b. QM_CAT_CLK_DIV.
   c. TOUCH_SPREAD_SPECTRUM_MAX_DEV, when Spread spectrum is enabled.
   d. PBA_HZ, when using QDebug/SPI_Master.c
   e. TARGET_PBA_FREQ_HZ and TARGET_CPU_FREQ_HZ, when using QDebug/SERIAL.c

2. In the UC3L-Evaluation kit (Rev2), the R42 and R54 (both 470KOhm) resistors
   MUST be replaced to 910KOhms.

3. The QTouch library uses PDCA channels 0 and 1.
   (QM_DMA_CHANNEL_0, QM_DMA_CHANNEL_1).
   Similarly, the QDebug/SERIAL.c uses PDCA channels 2 and 3.
   (PDCA_CHANNEL_RX_USART, PDCA_CHANNEL_TX_USART)

4. For QMatrix operation, the Analog comparators channels are used (using the
   ACIFB interface) depending on the Y Lines enabled.  For example, when
   Y lines Y2 and Y7 are enabled the Analog comparator channels 2 and 7
   are used by the CAT module for QMatrix operation.  The user can uses the rest
   of the Analog comparator channels in the main application. The QTouch Library
   enables the ACIFB using the Control register (if not already enabled by the main
   application) when the touch_qm_sensors_init API is called.

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
  touch_qm_dma_t qm_dma;

  /* Initialize host clock, pins, watchdog, etc. */
  init_system();

  /* Disable interrupts. */
  Disable_global_interrupt();

  /* The INTC driver has to be used only for GNU GCC for AVR32. */
#if (defined __GNUC__)

  /* initialize interrupt vectors. */
  INTC_init_interrupts();

  /* Register the Timer interrupt handler to the interrupt controller. */
  INTC_register_interrupt(&tc_irq, EXAMPLE_TC_IRQ, AVR32_INTC_INT1);

  /* Register the Touch Library CAT interrupt handler to the interrupt controller.
     Note: This interrupt registration is a MUST before using the Touch Library
     with the GCC compiler.

     For the case of IAR the registration of interrupt is automatically taken
     care by the compiler. The Touch Library CAT interrupt level for the case
     of IAR is fixed to Interrupt level 3. */
  INTC_register_interrupt(&touch_acq_done_irq, AVR32_CAT_IRQ, AVR32_INTC_INT3);

#endif

  /* Enable interrupts. */
  Enable_global_interrupt();

  /* Configure timer to fire ISR regularly. */
  init_timer();

  /* Initialize touch library and uc3l cat module for QMatrix operation.
     Note: Set up the GCLK_CAT for proper QMatrix operation.  Refer init_system(). */
  touch_ret = touch_qm_sensors_init( &touch_config );
  if(touch_ret != TOUCH_SUCCESS)
  {
    while(1u); /* Check API Error return code. */
  }

#if DEF_TOUCH_QDEBUG_ENABLE == 1
  /* Initialize the debug interface. */
  QDebug_Init();
#endif

  /* configure the touch library sensors. */
  touch_ret = config_uc3lek_touch_sensors();
  if(touch_ret != TOUCH_SUCCESS)
  {
    while(1u); /* Check API Error return code. */
  }

  /* Initialize touch sensing. */
  touch_ret = touch_qm_sensors_calibrate();
  if(touch_ret != TOUCH_SUCCESS)
  {
    while(1u); /* Check API Error return code. */
  }

  /* Provide the dma channels to be used by the CAT module.  For each
     acquisition cycle, any different combination of dma channels from 0 to 11
     can be provided. The touch library can also handle a different combination
     of dma channels for each call of the touch_qm_sensors_start_acquisition API. */
  qm_dma.dma_ch1 = QM_DMA_CHANNEL_0;
  qm_dma.dma_ch2 = QM_DMA_CHANNEL_1;

  // Initialize the PWMA module
  demo_init_pwma();

  // At the start of the demo, automatically change several times the PWMA duty
  // cycle (i.e. the intensity) of all LEDs.
  demo_automatic_ledshow_play(DEMO_INIT_NB_AUTOMATIC_CHANGES);

  /* Loop forever */
  for( ; ; )
  {
    /* Process touch library events. The touch_event_dispatcher API needs to
       be called as frequently as possible in order to have a good touch response. */
    touch_event_dispatcher();

    if( time_to_measure_touch == 1u )
    {
      /* Clear flag: it's time to measure touch */
      time_to_measure_touch = 0u;

      /* Start a touch sensors measurement process. */
      touch_ret = touch_qm_sensors_start_acquisition( current_time_ms_touch,
                                                      &qm_dma,
                                                      NORMAL_ACQ_MODE,
                                                      touch_qm_measure_complete_callback);
      if( (touch_ret != TOUCH_SUCCESS)       &&
          (touch_ret != TOUCH_ACQ_INCOMPLETE) )
      {
        gpio_clr_gpio_pin(LED0_GPIO);  // LED0
        gpio_clr_gpio_pin(LED1_GPIO);  // LED1
        gpio_clr_gpio_pin(LED2_GPIO);  // LED2
        gpio_clr_gpio_pin(LED3_GPIO);  // LED3
        do{
            delay_ms(50);
            gpio_tgl_gpio_pin(LED0_GPIO); gpio_tgl_gpio_pin(LED1_GPIO);
            gpio_tgl_gpio_pin(LED2_GPIO); gpio_tgl_gpio_pin(LED3_GPIO);
        }while(1);
       /* Reaching this point can be due to -
          1. The api has returned an error due to a invalid input parameter.
          2. The api has been called during a invalid Touch Library state. */
      }
    }


    /* Host application code goes here */


    /* Led demo application. */
    if(qm_measurement_done_touch == 1u)
    {
#if DEF_TOUCH_QDEBUG_ENABLE == 1
      /* UC3L_EK two-way QDebug communication application Example. */
      /* Process any commands received from QTouch Studio. */
      QDebug_ProcessCommands();

      /* Send out the Touch debug information data each time when Touch */
      /* measurement process is completed . */
      QDebug_SendData(p_qm_measure_data->acq_status);
#endif
      // New touch data measurement are available.
      process_qtouchlib_data();
      /* Clear flag: QMatrix measurement complete. */
      qm_measurement_done_touch = 0u;

      // Once the latest touch data measurements have been processed, clear them.
      // Here we clear only the measurements that are used by the application.
      p_qm_measure_data->p_sensor_states[0] = 0;
      p_qm_measure_data->acq_status = TOUCH_NO_ACTIVITY;
      p_qm_measure_data->p_rotor_slider_values[0] = 0;
    }
    else
      process_qtouchlib_data();
    // Note: we cannot go deeper than the IDLE sleep mode because the QMatrix lib
    // uses the PDMA.
    SLEEP(AVR32_PM_SMODE_IDLE);
  }
}

/*============================================================================
  Name : config_uc3lek_touch_sensors
------------------------------------------------------------------------------
Purpose: Example configuration for 64 touch keys.
Input  : n/a
Output : n/a
Notes  :
============================================================================*/
static touch_ret_t config_uc3lek_touch_sensors( void )
{
  touch_ret_t touch_ret = TOUCH_SUCCESS;
  sensor_id_t sensor_id;

  /* configure touch rotor. */
  touch_ret = touch_qm_sensor_config( SENSOR_TYPE_ROTOR, (channel_t)0u, (channel_t)5u,
                                       NO_AKS_GROUP, 25u, (hysteresis_t)HYST_6_25, RES_8_BIT, 0u,
                                       &sensor_id );
  if(touch_ret != TOUCH_SUCCESS)
  {
    while(1); /* Check API Error return code. */
  }

  /* configure touch key. */
  touch_ret = touch_qm_sensor_config( SENSOR_TYPE_KEY, (channel_t)6u, (channel_t)6u,
                                       NO_AKS_GROUP, 25u, HYST_6_25, RES_1_BIT, 0u,
                                       &sensor_id );
  if(touch_ret != TOUCH_SUCCESS)
  {
    while(1); /* Check API Error return code. */
  }

  /* configure touch key. */
  touch_ret = touch_qm_sensor_config( SENSOR_TYPE_KEY, (channel_t)7u, (channel_t)7u,
                                       NO_AKS_GROUP, 25u, HYST_6_25, RES_1_BIT, 0u,
                                       &sensor_id );
  if(touch_ret != TOUCH_SUCCESS)
  {
    while(1); /* Check API Error return code. */
  }

  /* configure touch key. */
  touch_ret = touch_qm_sensor_config( SENSOR_TYPE_KEY, (channel_t)8u, (channel_t)8u,
                                       NO_AKS_GROUP, 25u, HYST_6_25, RES_1_BIT, 0u,
                                       &sensor_id );
  if(touch_ret != TOUCH_SUCCESS)
  {
    while(1); /* Check API Error return code. */
  }

  /* configure touch key. */
  touch_ret = touch_qm_sensor_config( SENSOR_TYPE_KEY, (channel_t)10u, (channel_t)10u,
                                       NO_AKS_GROUP, 25u, HYST_6_25, RES_1_BIT, 0u,
                                       &sensor_id );
  if(touch_ret != TOUCH_SUCCESS)
  {
    while(1); /* Check API Error return code. */
  }

  /* configure touch key. */
  touch_ret = touch_qm_sensor_config( SENSOR_TYPE_KEY, (channel_t)11u, (channel_t)11u,
                                       NO_AKS_GROUP, 25u, HYST_6_25, RES_1_BIT, 0u,
                                       &sensor_id );
  if(touch_ret != TOUCH_SUCCESS)
  {
    while(1); /* Check API Error return code. */
  }

  return ( touch_ret );
}

/*============================================================================
Name    :   touch_qm_measure_complete_callback
------------------------------------------------------------------------------
Purpose :   QMatrix measure complete callback function.
            This function is called by the library when the touch measurement
            process is completed.
Input   :   n/a
Output  :
Notes   :   A touch_qm_measure_complete_callback() call signifies that fresh
            values of touch status, rotor/slider position, measured signals,
            references and Sensor data is available.

            The QMatrix  measured data can be accessed using the p_measure_data
            pointer.
============================================================================*/
static void touch_qm_measure_complete_callback ( touch_measure_data_t *p_measure_data )
{
  /* Copy the QMatrix measure data pointer. */
  p_qm_measure_data = p_measure_data;

  /* Set the QMatrix measurement done flag. */
  qm_measurement_done_touch = 1u;
}

static unsigned long init_clock( unsigned long cpuclk_hz )
{
  unsigned long ret_val = 0u;

  // Program the DFLL and switch the main clock source to the DFLL.
  pcl_configure_clocks(&pcl_dfll_freq_param);

#if DEF_TOUCH_QMATRIX == 1
  /* Set up the GCLK_CAT for proper QMatrix operation.  The GCLK_CAT is not
     required to be setup for Autonomous Touch and QTouch Group A/B operation.
     Note: for UC3L devices, the generic clock configurations are handled by the
     SCIF module. Setup gc to use the DFLL0 as source clock, divisor enabled, apply
     a division factor. */
  ret_val |= scif_gc_setup(AVR32_SCIF_GCLK_CAT, SCIF_GCCTRL_DFLL0, AVR32_GC_DIV_CLOCK, QM_GCLK_CAT_DIV);

  /* Now enable the generic clock */
  ret_val |= scif_gc_enable(AVR32_SCIF_GCLK_CAT);
#endif

  return (ret_val);
}

/*============================================================================
  Name : init_system
------------------------------------------------------------------------------
Purpose: initialize host clock, pins, watchdog, etc.
Input  : n/a
Output : n/a
Notes  :
============================================================================*/
static void init_system( void )
{
  int32_t ret_val = 0u;

  /* 1. Configure and start the DFLL0 in closed-loop mode to generate a frequency of 96MHz.
     2. Set Flash Wait state.
     3. Configure CPU, PBA, PBB clock to 48MHz.
     4. Set up the GCLK_CAT for QMatrix operation.  */
  ret_val = init_clock( EXAMPLE_CPUCLK_HZ );
  if(ret_val != 0u)
  {
    while(1); /* Clock configuration failed. */
  }


}

/*============================================================================
  Name : init_timer
------------------------------------------------------------------------------
Purpose: configure timer ISR to fire regularly.
Input  : n/a
Output : n/a
Notes  :
============================================================================*/
static void init_timer( void )
{
  volatile avr32_tc_t *tc = EXAMPLE_TC;

  /* options for waveform generation. */
  static const tc_waveform_opt_t WAVEFORM_OPT =
  {
    .channel  = EXAMPLE_TC_CHANNEL,                                             /* Channel selection. */

    .bswtrg   = TC_EVT_EFFECT_NOOP,                                             /* Software trigger effect on TIOB. */
    .beevt    = TC_EVT_EFFECT_NOOP,                                             /* External event effect on TIOB. */
    .bcpc     = TC_EVT_EFFECT_NOOP,                                             /* RC compare effect on TIOB. */
    .bcpb     = TC_EVT_EFFECT_NOOP,                                             /* RB compare effect on TIOB. */

    .aswtrg   = TC_EVT_EFFECT_NOOP,                                             /* Software trigger effect on TIOA. */
    .aeevt    = TC_EVT_EFFECT_NOOP,                                             /* External event effect on TIOA. */
    .acpc     = TC_EVT_EFFECT_NOOP,                                             /* RC compare effect on TIOA: toggle. */
    .acpa     = TC_EVT_EFFECT_NOOP,                                             /* RA compare effect on TIOA: toggle (other possibilities are none, set and clear). */

    .wavsel   = TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER,                             /* Waveform selection: Up mode with automatic trigger(reset) on RC compare. */
    .enetrg   = 0u,                                                             /* External event trigger enable. */
    .eevt     = 0u,                                                             /* External event selection. */
    .eevtedg  = TC_SEL_NO_EDGE,                                                 /* External event edge selection. */
    .cpcdis   = 0u,                                                             /* Counter disable when RC compare. */
    .cpcstop  = 0u,                                                             /* Counter clock stopped with RC compare. */

    .burst    = 0u,                                                             /* Burst signal selection. */
    .clki     = 0u,                                                             /* Clock inversion. */
    .tcclks   = TC_CLOCK_SOURCE_TC3                                             /* Internal source clock 3, connected to fPBA / 8. */
  };

  static const tc_interrupt_t TC_INTERRUPT =
  {
    .etrgs = 0u,
    .ldrbs = 0u,
    .ldras = 0u,
    .cpcs  = 1u,
    .cpbs  = 0u,
    .cpas  = 0u,
    .lovrs = 0u,
    .covfs = 0u
  };

  /* initialize the timer/counter. */
  tc_init_waveform(tc, &WAVEFORM_OPT);

  /* set the compare triggers. */
  tc_write_rc(tc, EXAMPLE_TC_CHANNEL, EXAMPLE_RC_VALUE );

  /* configure Timer interrupt. */
  tc_configure_interrupts(tc, EXAMPLE_TC_CHANNEL, &TC_INTERRUPT);

  /* start the timer/counter. */
  tc_start(tc, EXAMPLE_TC_CHANNEL);
}

/*============================================================================
  Name : timer_isr
------------------------------------------------------------------------------
Purpose: timer compare ISR.
Input  : n/a
Output : n/a
Notes  :
============================================================================*/
#if (defined __GNUC__)
  __attribute__((__interrupt__))
#elif (defined __ICCAVR32__)
  __interrupt
#endif
static void tc_irq(void)
{
  /* update the current time */
  current_time_ms_touch++;

  /* every 25th ms*/
  if( (current_time_ms_touch % measurement_period_ms) == 0u )
  {
    /* set flag: it's time to measure touch */
    time_to_measure_touch = 1u;
  }

  /* clear the interrupt flag. This is a side effect of reading the TC SR. */
  tc_read_sr(EXAMPLE_TC, EXAMPLE_TC_CHANNEL);
}
