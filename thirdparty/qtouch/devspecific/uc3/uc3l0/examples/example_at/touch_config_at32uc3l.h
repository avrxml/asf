/* This source file is part of the ATMEL QTouch Library Release 4.3.1 */

/*****************************************************************************
 *
 * \file
 *
 * \brief  This file contains the AT32UC3L Touch Library pin, register and
 * sensors configuration options for QMatrix, Autonomous QTouch and QTouch
 * Group A/B acquisition methods using the Capacitive Touch (CAT) module.
 *
 * When only QMatrix method of operation is used, the following configuration
 * options are valid.
 * 1. Touch common configuration options.
 * 2. QMatrix Sensor Configuration options.
 * 3. QMatrix Pin Configuration Options.
 * 4. QMatrix Clock and Register Configuration Options.
 * 5. QMatrix DMA Channel Options.
 * 6. QMatrix Global acquisition parameters.
 * 7. QMatrix Callback functions.
 *
 * When only Autonomous QTouch method of operation is used, the following
 * configuration options are valid.
 * 1. Touch common configuration options.
 * 2. Autonomous QTouch Pin Configuration Options.
 * 3. Autonomous QTouch Clock and Register Configuration Options.
 *
 * When only QTouch Group A method of operation is used, the following
 * configuration options are valid.
 * 1. Touch common configuration options.
 * 2. QTouch Group A Sensor Configuration options.
 * 3. QTouch Group A Pin Configuration Options.
 * 4. QTouch Group A Clock and Register Configuration Options.
 * 5. QTouch Group A DMA Channel Options.
 * 6. QTouch Group A Global acquisition parameters.
 * 7. QTouch Group A Callback functions.
 *
 * When only QTouch Group B method of operation is used, the following
 * configuration options are valid.
 * 1. Touch common configuration options.
 * 2. QTouch Group B Sensor Configuration options.
 * 3. QTouch Group B Pin Configuration Options.
 * 4. QTouch Group B Clock and Register Configuration Options.
 * 5. QTouch Group B DMA Channel Options.
 * 6. QTouch Group B Global acquisition parameters.
 * 7. QTouch Group B Callback functions.
 *
 * When a combination of QMatrix, Autonomous QTouch method, QTouch Group A and
 * QTouch Group B methods are used, then a combination of their individual
 * configuration options are valid.
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

#ifndef TOUCH_CONFIG_AT32UC3L_H
#define TOUCH_CONFIG_AT32UC3L_H

#ifdef __cplusplus
extern "C"
{
#endif

/*----------------------------------------------------------------------------
                           compiler information
  ----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
                               include files
*  ----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
                  acquisition method manifest constants
  ----------------------------------------------------------------------------*/

/*! \name Acquisition method manifest constants.
 * \brief The following constants can be used to select the desired Touch
 * acquisition methods to be used with the UC3L QTouch Library.  Choose QMatrix,
 * Autonomous QTouch, QTouch Group A and QTouch Group B methods by setting the
 * corresponding macro to 1.  It is possible for the QTouch Library to support
 * one or more acquisition methods at the same time.  For example, in order to
 * use QMatrix and Autonomous QTouch acquisition methods at the same time, both
 * DEF_TOUCH_QMATRIX and DEF_TOUCH_AUTONOMOUS_QTOUCH macros must be set to 1.
 */
/*! @{ */

/*! When 1, QMatrix method acquisition is used. */
/*! When 0, QMatrix method acquisition is not used. */
#define DEF_TOUCH_QMATRIX               (0)

/*! When 1, Autonomous QTouch method acquisition is used. */
/*! When 0, Autonomous QTouch method acquisition is not used. */
#define DEF_TOUCH_AUTONOMOUS_QTOUCH     (1)

/*! When 1, QTouch Group A method acquisition is used. */
/*! When 0, QTouch Group A method acquisition is not used. */
#define DEF_TOUCH_QTOUCH_GRP_A          (0)

/*! When 1, QTouch Group B method acquisition is used. */
/*! When 0, QTouch Group B method acquisition is not used. */
#define DEF_TOUCH_QTOUCH_GRP_B          (0)

/**
 * Enable/Disable QDebug for touch debug information communication with
 * QTouch Studio PC Software.
 * The QTouch Library supports Autonomous QTouch, QMatrix, QTouch Group A/B
 * modes at the sametime.  However, when using QDebug, it is only possible to
 * send the Touch debug information related to any one mode, at a given time.
 * Only one among the DEF_TOUCH_QDEBUG_ENABLE_xx manifest constants below
 * should be set to 1.
 *
 *  When 1, QDebug debug data communication to QTouch Studio is enabled.
 *  When 0, QDebug debug data communication to QTouch Studio is disabled.
 */
#define DEF_TOUCH_QDEBUG_ENABLE_QM      (0)

#if BOARD == STK600_RCUC3L0
#define DEF_TOUCH_QDEBUG_ENABLE_AT      (1)     /*! Demonstrate QTouch Studio
                                                 * communication. */
#elif BOARD == UC3L_EK
#define DEF_TOUCH_QDEBUG_ENABLE_AT      (0)     /*! Demonstrate Sleep mode. */
#elif BOARD == UC3_L0_XPLAINED
#define DEF_TOUCH_QDEBUG_ENABLE_AT      (0)     /*! Demonstrate Sleep mode. */
#else
#error\
	Board not supported. Autonomous QTouch configuration options may require changes to support User Board.
#endif

#define DEF_TOUCH_QDEBUG_ENABLE_QTA     (0)
#define DEF_TOUCH_QDEBUG_ENABLE_QTB     (0)
#define DEF_TOUCH_QDEBUG_ENABLE         (DEF_TOUCH_QDEBUG_ENABLE_QM  ||\
	DEF_TOUCH_QDEBUG_ENABLE_AT  ||\
	DEF_TOUCH_QDEBUG_ENABLE_QTA ||\
	DEF_TOUCH_QDEBUG_ENABLE_QTB)
/*! @} */

/*----------------------------------------------------------------------------
                                   macros
  ----------------------------------------------------------------------------*/
/*! Helper macro to include a selected pin. */
#define SET_PIN(x)                      (1u << (x))

/*----------------------------------------------------------------------------
                           QMatrix notes.
  ----------------------------------------------------------------------------*/
/* QMatrix method is selected. */
#if DEF_TOUCH_QMATRIX == 1

/**
 * QMatrix Clock dependency note.
 * The Example application uses a PBA clock of 48MHz.
 * When the UC3L PBA Clock is changed to a different frequency, the following
 * parameters must be changed accordingly to ensure proper QMatrix operation.
 * 1. QM_GCLK_CAT_DIV.
 * 2. QM_CAT_CLK_DIV.
 * 3. TOUCH_SPREAD_SPECTRUM_MAX_DEV when Spread spectrum is enabled.
 *
 * QMatrix resource requirement note.
 * Caution:
 * 1. The QMatrix method acquisition using the CAT module uses the Analog
 * comparators, controlled by the ACIFB.  When QMatrix method acquisition
 * is enabled, the Analog comparators should not be used by the user
 * application at any given time.
 * 2. The QMatrix method acquisition using the CAT module requires two
 * Peripheral DMA channels that must be provided by the application.
 */

/*----------------------------------------------------------------------------
                    QMatrix Sensor Configuration options.
  ----------------------------------------------------------------------------*/

/*! \name QMatrix Sensor Configuration options.
 */
/*! @{ */

/**
 * QMatrix number of X Lines.
 * Specify the number of QMatrix X Lines to be used by the Touch Library.
 * Refer CAT Matrix Connections Figure 28-2 in the datasheet.
 * Range: 1u to 17u.
 */
#define QM_NUM_X_LINES                  (8u)

/**
 * QMatrix number of Y Lines.
 * Specify the number of QMatrix Y-Yk pair to be used by the Touch Library.
 * Refer CAT Matrix Connections Figure 28-2 in the datasheet.
 * Range: 1u to 8u.
 */
#define QM_NUM_Y_LINES                  (8u)

/**
 * QMatrix number of Sensors.
 * Specify the number of QMatrix touch sensors to be used by the Touch Library.
 * A sensor can be a key, rotor or slider.
 * Example configuration: If the configuration has 6 keys (a key is formed
 * using one channel), one rotor (a QMatrix rotor is formed using 3 to 8
 * channels) and one slider (a QMatrix slider is formed using 3 to 8 channels),
 * then the number of sensors is 6 key + 1 rotor + 1 slider = 8 sensors.
 * Range: 1u to (number of X Lines * number of Y_Yk pairs).
 */
#define QM_NUM_SENSORS                  (40u)

/**
 * QMatrix number of Rotors and Sliders.
 * Specify the total number of QMatrix Rotors and Sliders to be used by the
 * Touch Library.  The number of Rotors and Sliders mentioned here is part of
 * the Total number of sensors specified in the QM_NUM_SENSORS macro.
 * Note: When no rotors or slider are required, specify a value of 0u.
 */
#define QM_NUM_ROTORS_SLIDERS           (8u)

/*! @} */

/*----------------------------------------------------------------------------
                   QMatrix Pin Configuration Options.
  ----------------------------------------------------------------------------*/

/*! \name QMatrix Pin Configuration Options.
 */
/*! @{ */

/**
 * QMatrix X Pins selected.
 * Specify the X lines to be used. Choose from X0 to X17.
 * Setting Bit'n' indicates that a pin Xn is selected.  Use the SET_PIN()
 * macro to select the required X pin.
 * Refer x_pin_options_t enum in touch_api_at32uc3l.h file.
 */
#define QM_X_PINS_SELECTED              (SET_PIN(X0)      | SET_PIN(X1) | \
                                         SET_PIN(X2)      | SET_PIN(X3) | \
                                         SET_PIN(X4)      | SET_PIN(X5) | \
                                         SET_PIN(X7)      | SET_PIN(X9))

/**
 * QMatrix Y-Yk Sense pairs selected.
 * Specify the Y pairs to be used. Choose from Y0_YK0 to Y7_YK7.
 * Setting Bit'n' indicates that a pair Yn_YKn is selected.  Use the SET_PIN()
 * macro to select the required Yn_YKn pair.
 * Refer y_pin_options_t enum in touch_api_at32uc3l.h file.
 */
#define QM_Y_PAIRS_SELECTED             (SET_PIN(Y0_YK0) | SET_PIN(Y1_YK1) | \
                                         SET_PIN(Y2_YK2) | SET_PIN(Y3_YK3) | \
                                         SET_PIN(Y4_YK4) | SET_PIN(Y5_YK5) | \
                                         SET_PIN(Y6_YK6) | SET_PIN(Y7_YK7))

/**
 * QMatrix SMP or DIS pin option.
 * Specify the smp or dis pin option, if included in the hardware design.
 * The QMatrix hardware can use a SMP pin for the discharge of the capacitors.
 * In this arrangement, a logic-level (SMP) pin is connected to the capacitors
 * through external high value resistors (typically between 100 kohm and
 * 1 Megohm). When this arrangement is used, one of the SMP pin option must be
 * chosen from the list available in the general_pin_options_t enum.
 *
 * Alternatively, the UC3L CAT module provides an option of using the internal
 * current sources for the discharge of capacitors.  By choosing this hardware
 * arrangement, the external resistors to be used with the SMP method can be
 * avoided.  The discharge of capacitors in this method can be done in two ways.
 * If the INTREFSEL macro is set to 1, then a internal resistor shall be used
 * to discharge the current.  If the INTREFSEL macro is set to 0, then a
 * external resistor shall be used to discharge the current.
 * When the INTREFSEL=0 external discharge mode arrangement is used, one of the
 * DIS pin option must be chosen from the list available in the
 * general_pin_options_t enum.
 * When the INTREFSEL=1 internal discharge mode arrangement is used, the
 * USE_NO_PIN option must be specified for the QM_SMP_DIS_PIN_OPTION.
 *
 * Refer general_pin_options_t enum in touch_api_at32uc3l.h file.
 * Also, refer macros QM_INTREFSEL and QM_INTVREFSEL.
 * Also, refer CAT Matrix Connections Figure 28-2 in the datasheet.
 */
#define QM_SMP_DIS_PIN_OPTION           (USE_NO_PIN)

/**
 * QMatrix VDIV Pin option.
 * Specify the vdiv pin option, if included in the hardware design.
 * The VDIV pin provides an option to make ACREFN a small positive voltage
 * in the case of any analog comparator offset issues.  The VDIV pin is driven
 * when the analog comparators are in use, and this signal can be used along
 * with a voltage divider arrangement to create a small positive offset on the
 * ACREFN pin.
 *
 * Refer general_pin_options_t enum in touch_api_at32uc3l.h file.
 * Also, refer CAT Matrix Connections Figure 28-2 in the datasheet.
 */
#define QM_VDIV_PIN_OPTION              (USE_NO_PIN)

/*! @} */

/*----------------------------------------------------------------------------
              QMatrix Clock and Register Configuration Options.
  ----------------------------------------------------------------------------*/

/*! \name QMatrix Clock and Register Configuration Options.
 */
/*! @{ */

/**
 * QMatrix Generic clock (GCLK_CAT) divider for CAT.
 * For proper QMatrix operation, the frequency of GCLK_CAT <= (PBA Clock/4).
 * Range: 0u to 65535u.
 * Note: The range specified is valid when this macro is used in conjunction
 * with scif_gc_setup() API.  Refer clock.c file.
 * Note: This option is common for QMatrix, QTouch and Autonomous Touch methods
 * of acquisition.
 */
#define QM_GCLK_CAT_DIV                 (8u)

/**
 * QMatrix CAT burst timing clock.
 * The prescaler value is used to ensure that the CAT module clock (CLK_CAT) is
 * divided to around 4 MHz to produce the burst timing clock.  The prescaler
 * uses the following formula to generate the burst timing clock:
 * Burst timing clock = CLK_CAT / (2(DIV+1))
 * Range: 0u to 255u.
 */
#define QM_CAT_CLK_DIV                  (5u)

/**
 * QMatrix Charge Length.
 * For QMatrix sensors, specifies how many burst prescaler clock cycles should
 * be used for transferring charge to the sense capacitor.
 * Units: Burst timing clock
 * Range: 0u to 255u.
 */
#define QM_CHLEN                        (3u)

/**
 * QMatrix Settle Length.
 * For QMatrix sensors, specifies how many burst prescaler clock cycles should
 * be used for settling after charge transfer.
 * Units: Burst timing clock
 * Range: 0u to 255u.
 */
#define QM_SELEN                        (3u)

/**
 * QMatrix Cx Capacitor Discharge Length.
 * For QMatrix sensors, specifies how many burst prescaler clock cycles the CAT
 * should use to discharge the Cx capacitor at the end of each burst cycle.
 * Units: Burst timing clock
 * Range: 0u to 255u.
 */
#define QM_CXDILEN                      (3u)

/**
 * QMatrix Discharge Length.
 * For QMatrix sensors, specifies how many burst prescaler clock cycles the CAT
 * should use to discharge the capacitors at the beginning of a burst sequence.
 * Units: Burst timing clock
 * Range: 0u to 255u.
 */
#define QM_DILEN                        (255u)

/**
 * QMatrix Discharge Shift.
 * For QMatrix sensors, specifies how many bits the DILEN field should be
 * shifted before using it to determine the discharge time.
 * Range: 0u to 3u.
 */
#define QM_DISHIFT                      (0u)

/**
 * QMatrix Maximum Count.
 * For QMatrix sensors, specifies how many counts (signal value) the maximum
 * acquisition should be.
 * Range: 0u to 65535u.
 */
#define QM_MAX_ACQ_COUNT                (3000u)

/**
 * QMatrix Consensus Filter Length.
 * For QMatrix sensors, specifies that discharge will be terminated when CONSEN
 * out of the most recent 5 comparator samples are positive. For example, a
 * value of 3 in the CONSEN field will terminate discharge when 3 out of the
 * most recent 5 comparator samples are positive. When CONSEN has the default
 * value of 0, discharge will be terminated immediately when the comparator
 * output goes positive.
 * Range: 0u to 5u.
 */
#define QM_CONSEN                       (5u)

/**
 * QMatrix Internal Reference Select.
 * This option is valid only when using the internal current sources mode of
 * discharging the capacitors.
 * 0u: The reference current flows through an external resistor on the DIS pin.
 * 1u: The reference current flows through the internal reference resistor.
 */
#define QM_INTREFSEL                    (1u)

/**
 * QMatrix Internal Voltage Reference select.
 * This option is valid only when using the internal current sources mode of
 * discharging the capacitors.
 * 0u: The voltage for the reference resistor is generated from the internal
 * band gap circuit.
 * 1u: The voltage for the reference resistor is VDDIO/2.
 */
#define QM_INTVREFSEL                   (1u)

/**
 * QMatrix Spread Spectrum Sensor Drive for QMatrix.
 * 0u: For QMatrix sensors, specifies that spread spectrum sensor drive shall
 * not be used.
 * 1u: For QMatrix sensors, specifies that spread spectrum sensor drive shall be
 *used.
 */
#define QM_ENABLE_SPREAD_SPECTRUM       (1u)

/**
 * QMatrix External synchronization to reduce 50 or 60 Hz mains interference.
 * 0u: For QMatrix sensors, specifies that external synchronization is disabled.
 * 1u: For QMatrix sensors, specifies that external synchronization mode is
 * enabled using the TOUCH_SYNC_PIN_OPTION pin option provided.
 * Refer TOUCH_SYNC_PIN_OPTION option.
 */
#define QM_ENABLE_EXTERNAL_SYNC         (0u)

/**
 * QMatrix Sync Time Interval.
 * When non-zero, determines the number of prescaled clock cycles between the
 * start of the acquisition on each X line for QMatrix acquisition.
 * This option is valid only in the case when QM_ENABLE_EXTERNAL_SYNC is
 * enabled.
 * Units: Burst timing clock
 * Range: 0u to 4095u.
 */
#define QM_SYNC_TIM                     (0u)

/*! @} */

/*----------------------------------------------------------------------------
                   QMatrix DMA Channel Options.
  ----------------------------------------------------------------------------*/

/*! \name QMatrix DMA Channel Options.
 */
/*! @{ */

/**
 * QMatrix DMA Channel 0.
 * Specify the DMA Channel option 0 to be used by the CAT module for transfer
 * of Acquisition count from CAT register to memory.
 * A different combination of DMA Channel option 0 and DMA Channel option 1 can
 * be provided for each touch_qm_sensors_start_acquisition API call.
 * Range: 0u to 11u.
 * Note: DMA Channel option 0 and DMA Channel option 1 cannot be the same.
 */
#define QM_DMA_CHANNEL_0                (0u)

/**
 * QMatrix DMA Channel 1.
 * Specify the DMA Channel option 1 to be used by the CAT module for transfer
 * of burst length values from memory to CAT register.
 * A different combination of DMA Channel option 0 and DMA Channel option 1 can
 * be provided for each touch_qm_sensors_start_acquisition API call.
 * Range: 0u to 11u.
 * Note: DMA Channel option 0 and DMA Channel option 1 cannot be the same.
 */
#define QM_DMA_CHANNEL_1                (1u)

/*! @} */

/*----------------------------------------------------------------------------
                   QMatrix Global acquisition parameters.
  ----------------------------------------------------------------------------*/

/*! \name QMatrix Global acquisition parameters.
 * Refer the Touch Library User guide for more information on these parameters.
 */
/*! @{ */

/**
 * QMatrix Sensor detect integration (DI) limit.
 * Range: 0u to 255u.
 */
#define QM_DI                           (4u)

/**
 * QMatrix Sensor negative drift rate.
 * Units: 200ms
 * Default value: 20u = 4 seconds.
 * Range: 1u to 127u.
 */
#define QM_NEG_DRIFT_RATE               (20u)

/**
 * QMatrix Sensor positive drift rate.
 * Units: 200ms
 * Default value: 5u = 1 second.
 * Range: 1u to 127u.
 */
#define QM_POS_DRIFT_RATE               (5u)

/**
 * QMatrix Sensor maximum on duration.
 * Units: 200ms (Example: a value 5u indicated Max ON duration of 1 second.)
 * Default value: 0 (No maximum ON time limit).
 * Range: 0u to 255u.
 */
#define QM_MAX_ON_DURATION              (0u)

/**
 * QMatrix Sensor drift hold time.
 * Units: 200ms
 * Default value: 20 (hold off drifting for 4 seconds after leaving detect).
 * Range: 1u to 255u.
 */
#define QM_DRIFT_HOLD_TIME              (20u)

/**
 * QMatrix Positive Recalibration delay.
 * Default value: 10.
 * Range: 1u to 255u.
 */
#define QM_POS_RECAL_DELAY              (10u)

/** QMatrix Sensor recalibration threshold.
 * Default: RECAL_50 (recalibration threshold = 50% of detection threshold).
 * Range: refer recal_threshold_t enum in touch_api_at32uc3l.h.
 */
#define QM_RECAL_THRESHOLD              (RECAL_50)

/*! @} */

/*----------------------------------------------------------------------------
*                   QMatrix Callback functions.
*  ----------------------------------------------------------------------------*/

/*! \name QMatrix Callback functions.
 */
/*! @{ */

/**
 * QMatrix Filter callback function.
 * A filter callback (when not NULL) is called by the Touch Library each time
 * a new set of Signal values are available.
 * An Example filter callback function prototype.
 * void qm_filter_callback( touch_filter_data_t *p_filter_data );
 */
#define QM_FILTER_CALLBACK              (qm_filter_callback)

/*! @} */

#endif                          /* end of #if DEF_TOUCH_QMATRIX. */

/*----------------------------------------------------------------------------
                   Autonomous QTouch notes.
  ----------------------------------------------------------------------------*/
/* Autonomous QTouch method is selected. */
#if DEF_TOUCH_AUTONOMOUS_QTOUCH == 1

/**
 * Autonomous QTouch Clock dependency note:
 * The Example application uses a PBA clock of 48MHz.
 * When the UC3L PBA Clock is changed to a different frequency, the following
 * parameters must be changed accordingly to ensure proper QMatrix operation.
 * 1. AT_CAT_CLK_DIV.
 * 2. TOUCH_SPREAD_SPECTRUM_MAX_DEV when Spread spectrum is enabled.
 *
 * Autonomous QTouch Sensor Tuning Guide:
 * When we start to tune the Proximity sensor, the values of AT_OUTSENS and
 * AT_SENSE must be kept large. For example, AT_OUTSENS = 40u, AT_SENSE=80u.
 * This will ensure that we do NOT hit the
 * touch_at_status_change_interrupt_callback(), while we tune.  
 * With this setting, the Autonomous Touch Base Count Register
 * (at memory location 0xFFFF686Cu) and Autonomous Touch Current Count Register
 * (at memory location 0xFFFF6870u) should be watched using the IDE Memory
 * window.
 * In `no Touch' situation, the two memory addresses should read similar values.
 * Expect these values to be at least few tens. When the sensor is touched,
 * the Current count register should decrease with respect to the base count
 * register. In order to increase the count, one could try increasing the
 * AT_CHLEN and AT_SELEN. Once we see this behavior, we can then tune the
 * AT_OUTSENS and AT_SENSE as desired, and this should result in proper 
 * interrupts.
 * Also, please ensure that you are using Rev D UC3L chip and not Rev B.
 * With the RevB has few know issue on the Autonomous Touch.
 * A smaller difference between the Autonomous Touch Base count and Autonomous
 * Touch Current count register can be chosen as the OUTSENS value.
 * A larger difference between the Autonomous Touch Base count and Autonomous
 * Touch Current count register can be chosen as the SENSE value.
 */

/*----------------------------------------------------------------------------
               Autonomous QTouch Pin Configuration Options.
  ----------------------------------------------------------------------------*/

/**
 * Autonomous QTouch Sense pair selected.
 * Specify the Sense pair to be used for Autonomous QTouch.  Choose any ONE
 * Sense Pair from SP0 to SP16.  Specify the Sense pair SPn directly.
 * Do NOT use the SET_PIN() macro when specifying the Sense pair.
 * Refer qt_pin_options_t enum in touch_api_at32uc3l.h file.
 */

#if (BOARD == STK600_RCUC3L0) || (BOARD == UC3_L0_XPLAINED)
#define AT_SP_SELECTED       (SP15)
#elif BOARD == UC3L_EK
#define AT_SP_SELECTED       (SP3)
#else
#error\
	Board not supported. Autonomous QTouch configuration options may require changes to support User Board.
#endif

/*----------------------------------------------------------------------------
             Autonomous QTouch Clock and Register Configuration Options.
  ----------------------------------------------------------------------------*/

/*! \name Autonomous QTouch Clock and Register Configuration Options.
 */
/*! @{ */

/**
 * Autonomous QTouch burst timing clock Divider.
 * The prescaler value is used to ensure that the CAT module clock (CLK_CAT) is
 * divided to around 1 MHz to produce the burst timing clock.  The prescaler
 * uses the following formula to generate the burst timing clock:
 * Burst timing clock = CLK_CAT / (2(DIV+1)).
 * Range: 0u to 65535u.
 */
#define AT_CAT_CLK_DIV                  (23u)

/**
 * Autonomous QTouch Charge Length
 * For Autonomous QTouch sensor, specifies how many burst prescaler clock cycles
 * should be used for transferring charge to the sense capacitor.
 * Units: Burst timing clock
 * Range: 0u to 255u.
 */
#define AT_CHLEN                        (2u)

/**
 * Autonomous QTouch Settle Length.
 * For Autonomous QTouch sensor, specifies how many burst prescaler clock cycles
 * should be used for settling after charge transfer.
 * Units: Burst timing clock
 * Range: 0u to 255u.
 */
#define AT_SELEN                        (1u)

/**
 * Autonomous QTouch Discharge Length.
 * For Autonomous QTouch sensor, specifies how many burst prescaler clock cycles
 * the CAT should use to discharge the capacitors before charging them.
 * Units: Burst timing clock
 * Range: 0u to 255u.
 */
#define AT_DILEN                        (255u)

/**
 * Autonomous QTouch Discharge Shift.
 * For Autonomous QTouch sensor, specifies how many bits the DILEN field should
 * be shifted before using it to determine the discharge time.
 * Range: 0u to 3u.
 */
#define AT_DISHIFT                      (0u)

/**
 * Autonomous QTouch Maximum Count.
 * For Autonomous QTouch sensor, specifies how many counts (signal value) the
 * maximum acquisition should be.
 * Range: 0u to 65535u.
 */
#define AT_MAX_ACQ_COUNT                (3000u)

/**
 * Autonomous QTouch Spread Spectrum Sensor Drive.
 * 0u: For Autonomous QTouch sensor, specifies that spread spectrum sensor drive
 * shall not be used.
 * 1u: For Autonomous QTouch sensor, specifies that spread spectrum sensor drive
 * shall be used.
 */
#define AT_ENABLE_SPREAD_SPECTRUM       (1u)

/**
 * Autonomous QTouch External synchronization to reduce 50 or 60 Hz mains
 * interference.
 * 0u: For Autonomous QTouch sensor, specifies that external synchronization is
 * disabled.
 * 1u: For Autonomous QTouch sensor, specifies that external synchronization
 * mode is enabled using the TOUCH_SYNC_PIN_OPTION pin option provided.
 * Refer TOUCH_SYNC_PIN_OPTION option.
 */
#define AT_ENABLE_EXTERNAL_SYNC         (0u)

/**
 * Autonomous Touch Filter Setting.
 * For the autonomous QTouch sensor, specifies how many positive detects in a
 * row the CAT needs to have on the autonomous QTouch sensor before reporting
 * it as a touch.
 * Note: A FILTER value of 0 is not allowed and will result in undefined
 * behavior.
 * Range: 1u to 15u.
 */
#define AT_FILTER                       (2u)

/**
 * Autonomous Touch Out-of-Touch Sensitivity.
 * For the autonomous QTouch sensor, specifies how sensitive the out-of-touch
 * detector should be.
 * When the sensor is not touched, the Autonomous Touch Current count register
 * is same as the Autonomous Touch Base count register. When the sensor is
 * touched the Autonomous Touch Current count register decreases.
 * When using the Autonomous QTouch in proximity mode, the Autonomous Touch Base
 * count register decreases as we move towards proximity of the sensor.
 * The OUTSENS value can be arrived at by watching the CAT Autonomous Touch Base
 * Count Register(at memory location 0xFFFF686Cu) and Autonomous Touch Current
 * Count Register(at memory location 0xFFFF6870u) during a sensor
 * touch/proximity and not in touch/proximity.
 * A smaller difference between the Autonomous Touch Base count and Autonomous
 * Touch Current count register can be chosen as the OUTSENS value.
 * Range: 0u to 255u.
 */
#define AT_OUTSENS                      (8u)

/**
 * Autonomous Touch Sensitivity.
 * For the autonomous QTouch sensor, specifies how sensitive the touch detector
 * should be. When the sensor is not touched, the Autonomous Touch Current
 * count register is same as the Autonomous Touch Base count register. When the
 * sensor is touched the Autonomous Touch Current count register decreases.
 * When using the Autonomous QTouch in proximity mode, the Autonomous Touch Base
 * count register decreases as we move towards proximity of the sensor.
 * The SENSE value can be arrived at by watching the CAT Autonomous Touch Base
 * Count Register(at memory location 0xFFFF686Cu) and Autonomous Touch Current
 * Count Register(at memory location 0xFFFF6870u) during a sensor
 * touch/proximity and not in touch/proximity.
 * A larger difference between the Autonomous Touch Base count and Autonomous
 * Touch Current count register can be chosen as the SENSE value.
 * Range: 1u to 255u.
 */
#define AT_SENSE                        (15u)

/**
 * Autonomous Touch Positive Recalibration Threshold.
 * For the autonomous QTouch sensor, specifies how far a sensor's signal must
 * move in a positive direction from the reference in order to cause a
 * recalibration.
 * Range: 0u to 255u.
 */
#define AT_PTHR                         (5u)

/**
 * Autonomous Touch Positive Drift Compensation.
 * For the autonomous QTouch sensor, specifies how often a positive drift
 * compensation should be performed. When this field is zero, positive drift
 * compensation will never be performed. When this field is non-zero, the
 * positive drift compensation time interval is given by the following formula:
 * Tpdrift = PDRIFT * 65536 * (sample clock period).
 * Range: 0u to 255u.
 */
#define AT_PDRIFT                       (60u)

/**
 * Autonomous Touch Negative Drift Compensation.
 * For the autonomous QTouch sensor, specifies how often a negative drift
 * compensation should be performed. When this field is zero, negative drift
 * compensation will never be performed. When this field is non-zero, the
 * negative drift compensation time interval is given by the following formula:
 * Tndrift = NDRIFT * 65536 * (sample clock period)
 * Range: 0u to 255u.
 */
#define AT_NDRIFT                       (60u)

/*! @} */

#endif                          /* end of #if DEF_TOUCH_AUTONOMOUS_QTOUCH. */

/*----------------------------------------------------------------------------
                     QTouch Group A notes.
  ----------------------------------------------------------------------------*/
/* QTouch Group A method is selected. */
#if DEF_TOUCH_QTOUCH_GRP_A == 1

/**
 * QTouch Group A Clock dependency note:
 * The Example application uses a PBA clock of 48MHz.
 * When the UC3L PBA Clock is changed to a different frequency, the following
 * parameters must be changed accordingly to ensure proper QTouch Group A
 * operation.
 * 1. QTA_CAT_CLK_DIV.
 * 2. TOUCH_SPREAD_SPECTRUM_MAX_DEV when Spread spectrum is enabled.
 *
 * QTouch Group A resource requirement note.
 * Caution:
 * 1. The QTouch Group A method acquisition using the CAT module requires one
 *    Peripheral DMA channel that must be provided by the application.
 */

/*----------------------------------------------------------------------------
                    QTouch Group A Sensor Configuration options.
  ----------------------------------------------------------------------------*/

/*! \name QTouch Group A Sensor Configuration options.
 */
/*! @{ */

/**
 * QTouch Group A number of Sensors.
 * Specify the number of QTouch Group A touch sensors to be used by the Touch
 * Library.
 * A sensor is either a key, rotor or slider.
 * Example configuration: If the configuration has 6 keys (a key is formed
 * using one channel), one rotor (a QTouch Group A rotor is formed using 3
 * channels) and one slider (a QTouch Group A slider is formed using
 * 3 channels), then the number of sensors is 6 key + 1 rotor + 1 slider =
 * 8 sensors.
 * Range: 1u to (number of Sense pairs specified in QTA_SP_SELECTED).
 */
#define QTA_NUM_SENSORS                 (4u)

/**
 * QTouch Group A number of Rotors and Sliders.
 * Specify the total number of QTouch Group A Rotors and Sliders to be used by
 * the Touch Library.  The number of Rotors and Sliders mentioned here is part
 * of the Total number of sensors specified in the QTA_NUM_SENSORS macro.  When
 * no rotors or slider are required, specify a value of 0u.
 */
#define QTA_NUM_ROTORS_SLIDERS          (2u)

/*! @} */

/*----------------------------------------------------------------------------
               QTouch Group A Pin Configuration Options.
  ----------------------------------------------------------------------------*/

/*! \name QTouch Group A Pin Configuration Options.
 */
/*! @{ */

/**
 * QTouch Group A Sense pair's selected.
 * Specify the Sense pair's to be used for QTouch Group A. Choose from SP0 to
 * SP16.
 * Setting Bit'n' indicates that Sense Pair SPn is selected.  Use the SET_PIN()
 * macro to select the required Sense pair.
 * Refer qt_pin_options_t enum in touch_api_at32uc3l.h file.
 */
#define QTA_SP_SELECTED                 (SET_PIN(SP1)  | SET_PIN(SP2)  | \
                                         SET_PIN(SP5)  | SET_PIN(SP7)  | \
                                         SET_PIN(SP8)  | SET_PIN(SP9)  | \
                                         SET_PIN(SP15) | SET_PIN(SP16) )

/*! @} */

/*----------------------------------------------------------------------------
          QTouch Group A Clock and Register Configuration Options.
  ----------------------------------------------------------------------------*/

/*! \name QTouch Group A Clock and Register Configuration Options.
 */
/*! @{ */

/**
 * QTouch Group A CAT burst timing clock.
 * The prescaler value is used to ensure that the CAT module clock (CLK_CAT) is
 * divided to around 1 MHz to produce the burst timing clock.  The prescaler
 * uses the following formula to generate the burst timing clock:
 * Burst timing clock = CLK_CAT / (2(DIV+1))
 * Range: 0u to 255u.
 */
#define QTA_CAT_CLK_DIV                 (23u)

/**
 * QTouch Group A Charge Length.
 * For QTouch Group A sensors, specifies how many burst prescaler clock cycles
 * should be used for transferring charge to the sense capacitor.
 * Units: Burst timing clock
 * Range: 0u to 255u.
 */
#define QTA_CHLEN                       (3u)

/**
 * QTouch Group A Settle Length.
 * For QTouch Group A sensors, specifies how many burst prescaler clock cycles
 * should be used for settling after charge transfer.
 * Units: Burst timing clock
 * Range: 0u to 255u.
 */
#define QTA_SELEN                       (1u)

/**
 * QTouch Group A Discharge Length.
 * For QTouch Group A sensors, specifies how many clock cycles the CAT should
 * use to discharge the capacitors before charging them.
 * Units: Burst timing clock
 * Range: 0u to 255u.
 */
#define QTA_DILEN                       (255u)

/**
 * QTouch Group A Discharge Shift.
 * For QTouch Group A sensors, specifies how many bits the DILEN field should be
 * shifted before using it to determine the discharge time.
 * Range: 0u to 3u.
 */
#define QTA_DISHIFT                     (0u)

/**
 * QTouch Group A Maximum Count.
 * For QTouch Group A sensors, specifies how many counts (signal value) the
 * maximum acquisition should be.
 * Range: 0u to 65535u.
 */
#define QTA_MAX_ACQ_COUNT               (3000u)

/**
 * QTouch Group A Spread Spectrum Sensor Drive.
 * 0u: For QTouch Group A sensors, specifies that spread spectrum sensor drive
 * shall not be used.
 * 1u: For QTouch Group A sensors, specifies that spread spectrum sensor drive
 * shall be used.
 */
#define QTA_ENABLE_SPREAD_SPECTRUM      (1u)

/**
 * QTouch Group A External synchronization to reduce 50 or 60 Hz mains
 * interference.
 * 0u: For QTouch Group A sensors, specifies that external synchronization is
 * disabled.
 * 1u: For QTouch Group A sensors, specifies that external synchronization mode
 * is enabled using the TOUCH_SYNC_PIN_OPTION pin option provided.
 * Refer TOUCH_SYNC_PIN_OPTION option.
 */
#define QTA_ENABLE_EXTERNAL_SYNC        (0u)

/*! @} */

/*----------------------------------------------------------------------------
                 QTouch Group A DMA Channel Options.
  ----------------------------------------------------------------------------*/

/*! \name QTouch Group A DMA Channel Options.
 */
/*! @{ */

/**
 * QTouch Group A DMA Channel 0.
 * Specify the DMA Channel option 0 to be used by the CAT module for transfer
 * of Acquisition count from CAT register to memory.
 * Range: 0u to 11u.
 */
#define QTA_DMA_CHANNEL_0               (0u)

/*! @} */

/*----------------------------------------------------------------------------
              QTouch Group A Global acquisition parameters.
  ----------------------------------------------------------------------------*/

/*! \name QTouch Group A Global acquisition parameters.
 * Refer the Touch Library User guide for more information on these parameters.
 */
/*! @{ */

/**
 * QTouch Group A Sensor detect integration (DI) limit.
 * Range: 0u to 255u.
 */
#define QTA_DI                          (4u)

/**
 * QTouch Group A Sensor negative drift rate.
 * Units: 200ms
 * Default value: 20u = 4 seconds.
 * Range: 1u to 127u.
 */
#define QTA_NEG_DRIFT_RATE              (20u)

/**
 * QTouch Group A Sensor positive drift rate.
 * Units: 200ms
 * Default value: 5u = 1 second.
 * Range: 1u to 127u.
 */
#define QTA_POS_DRIFT_RATE              (5u)

/**
 * QTouch Group A Sensor maximum on duration.
 * Units: 200ms (Example: a value 5u indicated Max ON duration of 1 second.)
 * Default value: 0 (No maximum ON time limit).
 * Range: 0u to 255u.
 */
#define QTA_MAX_ON_DURATION             (0u)

/**
 * QTouch Group A Sensor drift hold time.
 * Units: 200ms
 * Default value: 20 (hold off drifting for 4 seconds after leaving detect).
 * Range: 1u to 255u.
 */
#define QTA_DRIFT_HOLD_TIME             (20u)

/**
 * QTouch Group A Positive Recalibration delay.
 * Default value: 10.
 * Range: 1u to 255u.
 */
#define QTA_POS_RECAL_DELAY             (10u)

/**
 * QTouch Group A Sensor recalibration threshold.
 * Default: RECAL_50 (recalibration threshold = 50% of detection threshold).
 * Range: refer recal_threshold_t enum in touch_api_at32uc3l.h.
 */
#define QTA_RECAL_THRESHOLD             (RECAL_50)

/*! @} */

/*----------------------------------------------------------------------------
                   QTouch Group A Callback functions.
  ----------------------------------------------------------------------------*/

/*! \name  QTouch Group A Callback functions.
 */
/*! @{ */

/**
 * QTouch Group A Filter callback function.
 * A filter callback (when not NULL) is called by the Touch Library each time
 * a new set of Signal values are available.
 * An Example filter callback function prototype.
 * void qt_grp_a_filter_callback( touch_filter_data_t *p_filter_data );
 */
#define QTA_FILTER_CALLBACK             (NULL)

/*! @} */

#endif                          /* end of #if DEF_TOUCH_QTOUCH_GRP_A. */

/*----------------------------------------------------------------------------
                     QTouch Group B notes.
  ----------------------------------------------------------------------------*/
/* QTouch Group B method is selected. */
#if DEF_TOUCH_QTOUCH_GRP_B == 1

/**
 * QTouch Group B Clock dependency note:
 * The Example application uses a PBA clock of 48MHz.
 * When the UC3L PBA Clock is changed to a different frequency, the following
 * parameters must be changed accordingly to ensure proper QTouch Group B
 * operation.
 * 1. QTB_CAT_CLK_DIV.
 * 2. TOUCH_SPREAD_SPECTRUM_MAX_DEV when Spread spectrum is enabled.
 *
 * QTouch Group B resource requirement note.
 * Caution:
 * 1. The QTouch Group B method acquisition using the CAT module requires one
 *    Peripheral DMA channel that must be provided by the application.
 */

/*----------------------------------------------------------------------------
                    QTouch Group B Sensor Configuration options.
  ----------------------------------------------------------------------------*/

/*! \name QTouch Group B Sensor Configuration options.
 */
/*! @{ */

/**
 * QTouch Group B number of Sensors.
 * Specify the number of QTouch Group B touch sensors to be used by the Touch
 * Library.
 * A sensor is either a key, rotor or slider.
 * Example configuration: If the configuration has 6 keys (a key is formed
 * using one channel), one rotor (a QTouch Group B rotor is formed using 3
 * channels) and one slider (a QTouch Group B slider is formed using 3
 * channels), then the number of sensors is 6 key + 1 rotor + 1 slider = 
 * 8 sensors.
 * Range: 1u to (number of Sense pairs specified in QTB_SP_SELECTED).
 */
#define QTB_NUM_SENSORS                 (4u)

/**
 * QTouch Group B number of Rotors and Sliders.
 * Specify the total number of QTouch Group B Rotors and Sliders to be used by
 * the Touch Library.  The number of Rotors and Sliders mentioned here is part
 * of the Total number of sensors specified in the QTB_NUM_SENSORS macro.  When
 * no rotors or slider are required, specify a value of 0u.
 */
#define QTB_NUM_ROTORS_SLIDERS          (2u)

/*! @} */

/*----------------------------------------------------------------------------
               QTouch Group B Pin Configuration Options.
  ----------------------------------------------------------------------------*/

/*! \name QTouch Group B Pin Configuration Options.
 */
/*! @{ */

/**
 * QTouch Group B Sense pair's selected.
 * Specify the Sense pair's to be used for QTouch Group B. Choose from SP0 to
 * SP16.
 * Setting Bit'n' indicates that Sense Pair SPn is selected.  Use the SET_PIN()
 * macro to select the required Sense pair.
 * Refer qt_pin_options_t enum in touch_api_at32uc3l.h file.
 */
#define QTB_SP_SELECTED                 (SET_PIN(SP1)  | SET_PIN(SP2)  | \
                                         SET_PIN(SP5)  | SET_PIN(SP7)  | \
                                         SET_PIN(SP8)  | SET_PIN(SP9)  | \
                                         SET_PIN(SP15) | SET_PIN(SP16) )

/*! @} */

/*----------------------------------------------------------------------------
          QTouch Group B Clock and Register Configuration Options.
  ----------------------------------------------------------------------------*/

/*! \name QTouch Group B Clock and Register Configuration Options.
 */
/*! @{ */

/**
 * QTouch Group B CAT burst timing clock.
 * The prescaler value is used to ensure that the CAT module clock (CLK_CAT) is
 * divided to around 1 MHz to produce the burst timing clock.  The prescaler
 * uses the following formula to generate the burst timing clock:
 * Burst timing clock = CLK_CAT / (2(DIV+1))
 * Range: 0u to 255u.
 */
#define QTB_CAT_CLK_DIV                 (23u)

/**
 * QTouch Group B Charge Length.
 * For QTouch Group B sensors, specifies how many burst prescaler clock cycles
 * should be used for transferring charge to the sense capacitor.
 * Units: Burst timing clock
 * Range: 0u to 255u.
 */
#define QTB_CHLEN                       (3u)

/**
 * QTouch Group B Settle Length.
 * For QTouch Group B sensors, specifies how many burst prescaler clock cycles
 * should be used for settling after charge transfer.
 * Units: Burst timing clock
 * Range: 0u to 255u.
 */
#define QTB_SELEN                       (1u)

/**
 * QTouch Group B Discharge Length.
 * For QTouch Group B sensors, specifies how many clock cycles the CAT should
 * use to discharge the capacitors before charging them.
 * Units: Burst timing clock
 * Range: 0u to 255u.
 */
#define QTB_DILEN                       (255u)

/**
 * QTouch Group B Discharge Shift.
 * For QTouch Group B sensors, specifies how many bits the DILEN field should be
 * shifted before using it to determine the discharge time.
 * Range: 0u to 3u.
 */
#define QTB_DISHIFT                     (0u)

/**
 * QTouch Group B Maximum Count.
 * For QTouch Group B sensors, specifies how many counts (signal value) the
 * maximum acquisition should be.
 * Range: 0u to 65535u.
 */
#define QTB_MAX_ACQ_COUNT               (3000u)

/**
 * QTouch Group B Spread Spectrum Sensor Drive.
 * 0u: For QTouch Group B sensors, specifies that spread spectrum sensor drive
 * shall not be used.
 * 1u: For QTouch Group B sensors, specifies that spread spectrum sensor drive
 * shall be used.
 */
#define QTB_ENABLE_SPREAD_SPECTRUM      (1u)

/**
 * QTouch Group B External synchronization to reduce 50 or 60 Hz mains
 * interference.
 * 0u: For QTouch Group B sensors, specifies that external synchronization is
 * disabled.
 * 1u: For QTouch Group B sensors, specifies that external synchronization mode
 * is enabled using the TOUCH_SYNC_PIN_OPTION pin option provided.
 * Refer TOUCH_SYNC_PIN_OPTION option.
 */
#define QTB_ENABLE_EXTERNAL_SYNC        (0u)

/*! @} */

/*----------------------------------------------------------------------------
*                 QTouch Group B DMA Channel Options.
*  ----------------------------------------------------------------------------*/

/*! \name QTouch Group B DMA Channel Options.
 */
/*! @{ */

/**
 * QTouch Group B DMA Channel 0.
 * Specify the DMA Channel option 0 to be used by the CAT module for transfer
 * of Acquisition count from CAT register to memory.
 * Range: 0u to 11u.
 */
#define QTB_DMA_CHANNEL_0               (0u)

/*! @} */

/*----------------------------------------------------------------------------
              QTouch Group B Global acquisition parameters.
  ----------------------------------------------------------------------------*/

/*! \name QTouch Group B Global acquisition parameters.
 * Refer the Touch Library User guide for more information on these parameters.
 */
/*! @{ */

/**
 * QTouch Group B Sensor detect integration (DI) limit.
 * Range: 0u to 255u.
 */
#define QTB_DI                          (4u)

/**
 * QTouch Group B Sensor negative drift rate.
 * Units: 200ms
 * Default value: 20u = 4 seconds.
 * Range: 1u to 127u.
 */
#define QTB_NEG_DRIFT_RATE              (20u)

/**
 * QTouch Group B Sensor positive drift rate.
 * Units: 200ms
 * Default value: 5u = 1 second.
 * Range: 1u to 127u.
 */
#define QTB_POS_DRIFT_RATE              (5u)

/**
 * QTouch Group B Sensor maximum on duration.
 * Units: 200ms (Example: a value 5u indicated Max ON duration of 1 second.)
 * Default value: 0 (No maximum ON time limit).
 * Range: 0u to 255u.
 */
#define QTB_MAX_ON_DURATION             (0u)

/**
 * QTouch Group B Sensor drift hold time.
 * Units: 200ms
 * Default value: 20 (hold off drifting for 4 seconds after leaving detect).
 * Range: 1u to 255u.
 */
#define QTB_DRIFT_HOLD_TIME             (20u)

/**
 * QTouch Group B Positive Recalibration delay.
 * Default value: 10.
 * Range: 1u to 255u.
 */
#define QTB_POS_RECAL_DELAY             (10u)

/**
 * QTouch Group B Sensor recalibration threshold.
 * Default: RECAL_50 (recalibration threshold = 50% of detection threshold).
 * Range: refer recal_threshold_t enum in touch_api_at32uc3l.h.
 */
#define QTB_RECAL_THRESHOLD             (RECAL_50)

/*! @} */

/*----------------------------------------------------------------------------
                   QTouch Group B Callback functions.
  ----------------------------------------------------------------------------*/

/*! \name QTouch Group B Callback functions.
 */
/*! @{ */

/**
 * QTouch Group B Filter callback function.
 * A filter callback (when not NULL) is called by the Touch Library each time
 * a new set of Signal values are available.
 * An Example filter callback function prototype.
 * void qt_grp_b_filter_callback( touch_filter_data_t *p_filter_data );
 */
#define QTB_FILTER_CALLBACK             (NULL)

/*! @} */

#endif                          /* end of DEF_TOUCH_QTOUCH_GRP_B. */

/*----------------------------------------------------------------------------
                    Touch common configuration options.
  ----------------------------------------------------------------------------*/

/*! \name Touch common configuration options.
 */
/*! @{ */

/**
 * Touch Sync Pin option.
 * Specify the sync pin option, if included in the hardware design.
 * To prevent interference from the 50 or 60 Hz mains line the CAT can trigger
 * acquisition on the SYNC signal. The SYNC signal should be derived from the
 * mains line. The acquisition will trigger on a falling edge of this signal.
 * Refer general_pin_options_t enum in touch_api_at32uc3l.h file.
 */
#define TOUCH_SYNC_PIN_OPTION           (USE_NO_PIN)

/**
 * Touch Maximum Deviation.
 * When spread spectrum burst is enabled using the QM_ENABLE_SPREAD_SPECTRUM
 * option, MAX_DEV indicates the maximum number of prescaled clock cycles the
 * burst pulse will be extended or shortened.
 * Range: When only QMatrix is used, 0u to ((2u * QM_CAT_CLK_DIV) + 1u).
 * Range: When only Autonomous QTouch is used, 0u to ((2u * AT_CAT_CLK_DIV) +
 * 1u).
 * Range: When only QTouch Group A is used, 0u to ((2u * QTA_CAT_CLK_DIV) + 1u).
 * Range: When only QTouch Group B is used, 0u to ((2u * QTB_CAT_CLK_DIV) + 1u).
 * Range: When a combination of QMatrix, Autonomous QTouch, QTouch Group A or
 *        QTouch Group B is used,
 *        0u to ((2u * least of(QM_CAT_CLK_DIV, AT_CAT_CLK_DIV,
 *                              QTA_CAT_CLK_DIV, QTB_CAT_CLK_DIV )) + 1u)
 */
#define TOUCH_SPREAD_SPECTRUM_MAX_DEV   (4u)

/**
 * Touch Resistive Drive Enable for CSA lines.
 * This option can be used to enable 1kOhm resistive drive capability, if
 * available on a specific CSA pin.  For the UC3L, the following CSA[n] pin
 * have the resistive drive capability - CSARES: 0x0001AEEEu
 * When bit n is 0, CSA[n] has the same drive properties as normal I/O pads.
 * When bit n is 1, CSA[n] has a nominal output resistance of 1kOhm during the
 * burst phase.
 * Refer Table 28-2. Pin Selection Guide.
 */
#define TOUCH_CSARES                    (0x00000000)

/**
 * Touch Resistive Drive Enable for CSB lines.
 * This option can be used to enable 1kOhm resistive drive capability, if
 * available on a specific CSA pin.  For the UC3L, the following CSB[n] pin
 * have the resistive drive capability - CSBRES: 0x0000BEECu
 * When bit n is 0, CSB[n] has the same drive properties as normal I/O pads.
 * When bit n is 1, CSB[n] has a nominal output resistance of 1kOhm during the
 * burst phase.
 * Refer Table 28-2. Pin Selection Guide.
 */
#define TOUCH_CSBRES                    (0x00000000)

/*! @} */

#ifdef __cplusplus
}
#endif

#endif                          /* TOUCH_CONFIG_AT32UC3L_H */
/* EOF */
