/* This source file is part of the ATMEL QTouch Library Release 5.1 */

/**
 * \file
 *
 * \brief This file contains the SAM4L QTouch Libary API interface for
 *         QMatrix, QTouch and Autonomous Touch method using the Capacitive
 *         Touch (CATB) module.
 * - Userguide:          QTouch Library User Guide - doc8207.pdf.
 * - Support: http://www.atmel.com/design-support/
 *
 * Copyright (c) 2012-2015 Atmel Corporation. All rights reserved.
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

#ifndef TOUCH_API_SAM4L_H
#define TOUCH_API_SAM4L_H

/**
 * \defgroup group_thirdparty_qtouch_devspecific_sam4_sam4l QTouch Library for SAM4L
 * 
 * This is the QTouch Library for SAM4L devices that consists in
 * one api header file(touch_api_sam4l.h) and one pre-compiled
 * library(libsam4l-qt-gnu.a).
 *
 * All projects using this library must also provide the required header
 * file(touch_config_sam4l.h) used to configure the library.
 *
 * For more information on the Qtouch Library, check www.atmel.com/qtouchlib.
 *
 * \{
 */

#ifdef __cplusplus
extern "C"
{
#endif

/*----------------------------------------------------------------------------
                            compiler information
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
                                include files
----------------------------------------------------------------------------*/
// From module:
#include <exceptions.h>
// From module: Common SAM compiler driver
#include <compiler.h>
#include <status_codes.h>
// From module: Part identification macros
#include <parts.h>

#include "touch_config_sam4l.h"

/*----------------------------------------------------------------------------
                            manifest constants
----------------------------------------------------------------------------*/
/*! \name QTouch Library Acquisition Status bitfields.
 */
//! @{

#define TOUCH_NO_ACTIVITY             (0x0000u)	//!< No Touch activity.
#define TOUCH_IN_DETECT               (0x0001u)	//!< Atleast one Touch channel is in detect.
#define TOUCH_STATUS_CHANGE           (0x0002u)	//!< Change in Touch status of atleast one Touch channel.
#define TOUCH_ROTOR_SLIDER_POS_CHANGE (0x0004u)	//!< Change in Rotor or Slider position of atleast one rotor or slider.
#define TOUCH_CHANNEL_REF_CHANGE      (0x0008u)	//!< Change in Reference value of atleast one Touch channel.
#define TOUCH_BURST_AGAIN             (0x0100u)	//!< Indicates that reburst is required to resolve Filtering or Calibration state.
#define TOUCH_RESOLVE_CAL             (0x0200u)	//!< Indicates that reburst is needed to resolve Calibration.
#define TOUCH_RESOLVE_FILTERIN        (0x0400u)	//!< Indicates that reburst is needed to resolve Filtering.
#define TOUCH_RESOLVE_DI              (0x0800u)	//!< Indicates that reburst is needed to resolve Detect Integration.
#define TOUCH_RESOLVE_POS_RECAL       (0x1000u)	//!< Indicates that reburst is needed to resolve Recalibration.

//! @}

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
#define GET_QT_SENSOR_STATE(SENSOR_NUMBER) p_qt_measure_data->p_sensor_states[(SENSOR_NUMBER/8)] & (1 << (SENSOR_NUMBER % 8))

#define GET_QT_ROTOR_SLIDER_POSITION(ROTOR_SLIDER_NUMBER)p_qt_measure_data->p_rotor_slider_values[ROTOR_SLIDER_NUMBER]


/*----------------------------------------------------------------------------
                             macros
----------------------------------------------------------------------------*/
#if (QT_NUM_SENSORS == 0u)
#error 'Number of Sensors cannot be 0.'
#endif

/**
  * QTouch number of channels.
  */
#define QT_NUM_CHANNELS          (QT_NUM_SENSOR_PINS_SELECTED)

/**
  * Size of QTouch Sensor.
  */
#define PRIV_QT_SIZE_SENSOR      (8u)

/**
  * Size of QTouch Channel.
  */
#define PRIV_QT_SIZE_CHANNEL     (25u)

/**
  * Size of QTouch Rotor Slider.
  */
#define PRIV_QT_SIZE_RS          (13u)

/**
  * Size of QTouch Pad bytes.
  */
#define PRIV_QT_PAD_BYTE_SIZE    (36u)

/**
  * QTouch Data block size.
  */
#define PRIV_QT_DATA_BLK_SIZE    ((PRIV_QT_SIZE_SENSOR          * QT_NUM_SENSORS)        + \
                                   (PRIV_QT_SIZE_CHANNEL         * QT_NUM_CHANNELS)       + \
                                   (PRIV_QT_SIZE_RS              * QT_NUM_ROTORS_SLIDERS) + \
                                   ((QT_NUM_CHANNELS+ 7u) / 8u)  + (PRIV_QT_PAD_BYTE_SIZE))

/**
  * Initialize rotor slider table.
  */
#if   (QT_NUM_ROTORS_SLIDERS != 0u)
#define PRIV_QT_RS_TABLE_INIT  touch_qt_rs_table_init
#else
#define PRIV_QT_RS_TABLE_INIT  NULL
#endif /* QT_NUM_ROTORS_SLIDERS*/

/*----------------------------------------------------------------------------
                            type definitions
----------------------------------------------------------------------------*/
//! Sensor start and end channel type of a Sensor. Channel number starts with value 0.
  typedef uint8_t channel_t;

//! An unsigned 8-bit number setting a sensor detection threshold.
  typedef uint8_t threshold_t;

//! Sensor number type.
  typedef uint8_t sensor_id_t;

//! Current time type.
  typedef uint16_t touch_time_t;

//! QMatrix individual sensor burst length type.
  typedef uint8_t touch_bl_t;

//! Touch sensor delta value type.
  typedef int16_t touch_delta_t;

//! Status of Touch measurement.
  typedef uint16_t touch_acq_status_t;

//! Touch Library error codes.
  typedef enum tag_touch_ret_t
  {
    //! Successful completion of operation.
    TOUCH_SUCCESS,
    //! Touch Library is busy with pending previous Touch measurement.
    TOUCH_ACQ_INCOMPLETE,
    //! Invalid input parameter.
    TOUCH_INVALID_INPUT_PARAM,
    //! Operation not allowed in the current Touch Library state.
    TOUCH_INVALID_LIB_STATE,
    //! Invalid QTouch config input parameter.
    TOUCH_INVALID_QT_CONFIG_PARAM,
    //! Invalid Recalibration threshold input value.
    TOUCH_INVALID_RECAL_THRESHOLD,
    //! Channel number parameter exceeded total number of channels configured.
    TOUCH_INVALID_CHANNEL_NUM,
    //! Invalid sensor type. Sensor type can NOT be SENSOR_TYPE_UNASSIGNED.
    TOUCH_INVALID_SENSOR_TYPE,
    //! Invalid Sensor number parameter.
    TOUCH_INVALID_SENSOR_ID,
    //! DMA Channel numbers are out of range.
    TOUCH_INVALID_DMA_PARAM,
    //! Number of Rotor/Sliders set as 0,
    //! when trying to configure a rotor/slider.
    TOUCH_INVALID_RS_NUM,
    //! CATB clock not initialized.
    TOUCH_CATB_CLOCK_ERROR
  }
  touch_ret_t;

//! Touch library state.
  typedef enum tag_touch_lib_state_t
  {
    TOUCH_STATE_NULL,
    TOUCH_STATE_INIT,
    TOUCH_STATE_READY,
    TOUCH_STATE_CALIBRATE,
    TOUCH_STATE_BUSY
  }
  touch_lib_state_t;

//! Touch library acquisition mode.
  typedef enum tag_touch_acq_mode_t
  {
    //! When Raw acquisition mode is used, the measure_complete_callback
    //! function is called immediately once a fresh value of Signals are
    //! available.  In this mode, the Touch Library does not do any
    //! processing on the Signals.  So, the References, Sensor states or
    //! Rotor/Slider position values are not updated in this mode.
    RAW_ACQ_MODE,
    //! When Nomal acquisition mode is used, the measure_complete_callback
    //! function is called only after the Touch Library completes processing
    //! of the Signal values obtained. The References, Sensor states and
    //! Rotor/Slider position values are updated in this mode.
    NORMAL_ACQ_MODE
  }
  touch_acq_mode_t;

//! Sensor types available.
  typedef enum tag_sensor_type_t
  {
    SENSOR_TYPE_UNASSIGNED,
    SENSOR_TYPE_KEY,
    SENSOR_TYPE_ROTOR,
    SENSOR_TYPE_SLIDER,
    MAX_SENSOR_TYPE
  }
  sensor_type_t;

/**
 * Which AKS group, if any, a sensor is in.
 * NO_AKS_GROUP = sensor is not in an AKS group, and cannot be suppressed.
 * AKS_GROUP_x  = sensor is in AKS group x.
 */
  typedef enum tag_aks_group_t
  {
    NO_AKS_GROUP,
    AKS_GROUP_1,
    AKS_GROUP_2,
    AKS_GROUP_3,
    AKS_GROUP_4,
    AKS_GROUP_5,
    AKS_GROUP_6,
    AKS_GROUP_7,
    MAX_AKS_GROUP
  }
  aks_group_t;

/**
 * A sensor detection hysteresis value.  This is expressed as a percentage of
 * the sensor detection threshold.
 * HYST_x = hysteresis value is x% of detection threshold value (rounded down).
 * NB: a minimum value of 2 is used.
 *
 * Example: if detection threshold = 20, then:
 *    HYST_50   = 10 (50% of 20)
 *    HYST_25   = 5  (25% of 20)
 *    HYST_12_5 = 2  (12.5% of 20)
 *    HYST_6_25 = 2  (6.25% of 20 = 1, but value is hardlimited to 2)
 */
  typedef enum tag_hysteresis_t
  {
    HYST_50,
    HYST_25,
    HYST_12_5,
    HYST_6_25,
    MAX_HYST
  }
  hysteresis_t;

/**
 * A sensor recalibration threshold.  This is expressed as a percentage of the
 * sensor detection threshold.
 * RECAL_x = recalibration threshold is x% of detection threshold value
 *           (rounded down).
 * NB: a minimum value of 4 is used.
 *
 * Example: if detection threshold = 40, then:
 *     RECAL_100  = 40 (100% of 40)
 *     RECAL_50   = 20 (50% of 40)
 *     RECAL_25   = 10 (25% of 40)
 *     RECAL_12_5 = 5  (12.5% of 40)
 *     RECAL_6_25 = 4  (6.25% of 40 = 2, but value is hardlimited to 4)
 */
  typedef enum tag_recal_threshold_t
  {
    RECAL_100,
    RECAL_50,
    RECAL_25,
    RECAL_12_5,
    RECAL_6_25,
    MAX_RECAL
  }
  recal_threshold_t;

/**
 * For rotors and sliders, the resolution of the reported angle or position.
 * RES_x_BIT = rotor/slider reports x-bit values.
 *
 * Example: if slider resolution is RES_7_BIT, then reported positions are in
 * the range 0..127.
 */
  typedef enum tag_resolution_t
  {
    RES_1_BIT,
    RES_2_BIT,
    RES_3_BIT,
    RES_4_BIT,
    RES_5_BIT,
    RES_6_BIT,
    RES_7_BIT,
    RES_8_BIT,
    MAX_RES
  }
  resolution_t;

//! Autonomous Touch status change enum.
  typedef enum tag_at_status_change_t
  {
    //! Autonomous Touch Status has changed to Out of Touch.
    OUT_OF_TOUCH,
    //! Autonomous Touch Status has changed to In Touch.
    IN_TOUCH
  }
  at_status_change_t;

//! QTouch Pin options available.
  typedef enum tag_qt_sensor_pin_options_t
  {
    SP_PA04,   /*SENSE0 on PA04*/
    SP_PA27,   /*SENSE0 on PA27*/
    SP_PB13,   /*SENSE0 on PB13*/
    SP_PA05,   /*SENSE1 on PA05*/
    SP_PA28,   /*SENSE1 on PA28*/
    SP_PB14,   /*SENSE1 on PB14*/
    SP_PA06,   /*SENSE2 on PA06*/
    SP_PA29,   /*SENSE2 on PA29*/
    SP_PB15,   /*SENSE2 on PB15*/
    SP_PA07,   /*SENSE3 on PA07*/
    SP_PA30,   /*SENSE3 on PA30*/
    SP_PC00,   /*SENSE3 on PC00*/
    SP_PA08,   /*SENSE4 on PA08*/
    SP_PC01,   /*SENSE4 on PC01*/
    SP_PA09,   /*SENSE5 on PA09*/
    SP_PC02,   /*SENSE5 on PC02*/
    SP_PA10,   /*SENSE6 on PA10*/
    SP_PC03,   /*SENSE6 on PC03*/
    SP_PA11,   /*SENSE7 on PA11*/
    SP_PC04,   /*SENSE7 on PC04*/
    SP_PA13,   /*SENSE8 on PA13*/
    SP_PC06,   /*SENSE8 on PC06*/
    SP_PA14,   /*SENSE9 on PA14*/
    SP_PC07,   /*SENSE9 on PC07*/
    SP_PA15,   /*SENSE10 on PA15*/
    SP_PC08,   /*SENSE10 on PC08*/
    SP_PA16,   /*SENSE11 on PA16*/
    SP_PC09,   /*SENSE11 on PC09*/
    SP_PA17,   /*SENSE12 on PA17*/
    SP_PC10,   /*SENSE12 on PC10*/
    SP_PA18,   /*SENSE13 on PA18*/
    SP_PC11,   /*SENSE13 on PC11*/
    SP_PA19,   /*SENSE14 on PA19*/
    SP_PC12,   /*SENSE14 on PC12*/
    SP_PA20,   /*SENSE15 on PA20*/
    SP_PC13,   /*SENSE15 on PC13*/
    SP_PA21,   /*SENSE16 on PA21*/
    SP_PC15,   /*SENSE16 on PC15*/
    SP_PA22,   /*SENSE17 on PA22*/
    SP_PC16,   /*SENSE17 on PC16*/
    SP_PA24,   /*SENSE18 on PA24*/
    SP_PC17,   /*SENSE18 on PC17*/
    SP_PA25,   /*SENSE19 on PA25*/
    SP_PC18,   /*SENSE19 on PC18*/
    SP_PA26,   /*SENSE20 on PA26*/
    SP_PC19,   /*SENSE20 on PC19*/
    SP_PB00,   /*SENSE21 on PB00*/
    SP_PC20,   /*SENSE21 on PC20*/
    SP_PB01,   /*SENSE22 on PB01*/
    SP_PC21,   /*SENSE22 on PC21*/
    SP_PB02,   /*SENSE23 on PB02*/
    SP_PC22,   /*SENSE23 on PC22*/
    SP_PB04,   /*SENSE24 on PB04*/
    SP_PC24,   /*SENSE24 on PC24*/
    SP_PB05,   /*SENSE25 on PB05*/
    SP_PC25,   /*SENSE25 on PC25*/
    SP_PB06,   /*SENSE26 on PB06*/
    SP_PC26,   /*SENSE26 on PC26*/
    SP_PB07,   /*SENSE27 on PB07*/
    SP_PC27,   /*SENSE27 on PC27*/
    SP_PB08,   /*SENSE28 on PB08*/
    SP_PC28,   /*SENSE28 on PC28*/
    SP_PB09,   /*SENSE29 on PB09*/
    SP_PC29,   /*SENSE29 on PC29*/
    SP_PB10,   /*SENSE30 on PB10*/
    SP_PC30,   /*SENSE30 on PC30*/
    SP_PB11,   /*SENSE31 on PB11*/
    SP_PC31    /*SENSE31 on PC31*/
  }
  qt_sensor_pin_options_t;

//! QTouch Pin options available.
  typedef enum tag_qt_discharge_pin_options_t
  {
    DIS_PA02,     /*DIS on PA02*/
    DIS_PA12,     /*DIS on PA12*/
    DIS_PA23,     /*DIS on PA23*/
    DIS_PA31,     /*DIS on PA31*/
    DIS_PB03,     /*DIS on PB03*/
    DIS_PB12,     /*DIS on PB12*/
    DIS_PC05,     /*DIS on PC05*/
    DIS_PC14,     /*DIS on PC14*/
    DIS_PC23,     /*DIS on PC23*/
  }
  qt_discharge_pin_options_t;

//! QTouch Library Timing info.
  typedef struct tag_touch_qt_time_t
  {
	//! Touch Measurement period in milliseconds.  This variable determines how
	//! often a new touch measurement must be done.
	//! Default value: Every 25 milliseconds.
    uint16_t measurement_period_ms;
    //! Current time, set by timer ISR.
    volatile uint16_t current_time_ms;
    //! Flag set by timer ISR when it's time to measure touch.
    volatile uint8_t time_to_measure_touch;
	//! Flag set by touch_qt_measure_complete_callback() function when a fresh Touch
	//! status is available.
    volatile uint8_t measurement_done_touch;
  }
  touch_qt_time_t;

//! Sensor structure for storing sensor related information.
  typedef struct tag_sensor_t
  {
    //! sensor state (calibrating, on, ...).
    uint8_t state;
    //! general purpose ctr: used for calibration, drifting, etc.
    uint8_t general_counter;
    //! Detect Integration ctr.
    uint8_t ndil_counter;
    //! sensor detection threshold.
    uint8_t threshold;
    //! bits 7..6: sensor type:
    //! {00: key,01: rotor,10: slider,11: reserved}
    //! bits 5..3: AKS group (0..7): 0 = no AKS group
    //! bit 2    : positive recal flag
    //! bits 1..0: hysteresis
    uint8_t type_aks_pos_hyst;
    //! sensor from channel
    //! for keys: from channel = to channel
    //! rotors  : Top channel
    //! sliders : Left most channel
    //! NB:only need to_channel for keys in rotors/sliders build
    uint8_t from_channel;
    //! FOR KEY's: this is unused,
    //! rotors:  Bottom left channel
    //! sliders: Middle  channel
    uint8_t to_channel;
    //! index into array of rotor/slider values.
    uint8_t index;
  }
  sensor_t;

//! Global sensor configuration info.
  typedef struct tag_touch_global_param_t
  {
    //! Sensor detect integration (DI) limit.
    uint8_t di;
    //! Sensor negative drift rate.
    uint8_t neg_drift_rate;
    //! Sensor positive drift rate.
    uint8_t pos_drift_rate;
    //! Sensor maximum on duration.
    uint8_t max_on_duration;
    //! Sensor drift hold time.
    uint8_t drift_hold_time;
    //! Sensor Positive recalibration delay.
    uint8_t pos_recal_delay;
    //! Sensor recalibration threshold.
    recal_threshold_t recal_threshold;
  }
  touch_global_param_t;

//! Touch Filter Callback data type.
  typedef struct tag_touch_filter_data_t
  {
    //! Length of the measured signal values list.
    uint8_t num_channel_signals;
    //! Pointer to measured signal values for each channel.
    uint16_t *p_channel_signals;
  }
  touch_filter_data_t;

//! Touch Measured data type.
  typedef struct tag_touch_measure_data_t
  {
    //! Status of Touch measurement.
    touch_acq_status_t acq_status;
    //! Length of the measured signal values list.
    uint8_t num_channel_signals;
    //! Pointer to measured signal values for each channel.
    uint16_t *p_channel_signals;
    //! Length of the measured reference values list.
    uint8_t num_channel_references;
    //! Pointer to reference values for each channel.
    uint16_t *p_channel_references;
    //! Number of sensor state bytes.
    uint8_t num_sensor_states;
    //! Pointer to Touch Status of each sensor.
    uint8_t *p_sensor_states;
    //! Length of the Rotor and Slider position values list.
    uint8_t num_rotor_slider_values;
    //! Pointer to Rotor and Slider position values.
    uint8_t *p_rotor_slider_values;
    //! Length of the sensors data list.
    uint8_t num_sensors;
    //! Pointer to Sensor data.
    sensor_t *p_sensors;
  }
  touch_measure_data_t;

//! QTouch dma type.
  typedef struct tag_touch_qt_dma_t
  {
    //! Dma channel 1 to be used by the Touch Library.
    uint8_t dma_ch1;
    //! Dma channel 2 to be used by the Touch Library.
    uint8_t dma_ch2;
  }
  touch_qt_dma_t;

//! QTouch sensor configuration type.
  typedef struct tag_touch_qt_param_t
  {
    //! Sensor detection threshold.
    aks_group_t aks_group;
    //! Sensor detection threshold.
    threshold_t detect_threshold;
    //! Sensor detection hysteresis.
    hysteresis_t detect_hysteresis;
    //! Sensor position resolution. This is valid only for a Rotor or Slider.
    resolution_t position_resolution;
  }
  touch_qt_param_t;

//! Autonomous QTouch status type.
  typedef struct tag_touch_at_status_t
  {
    //! Autonomous QTouch Status change.
    at_status_change_t status_change;
  }
  touch_at_status;

//! QTouch pin configuration type.
  typedef struct tag_touch_qt_pin_t
  {
    uint8_t *p_sp; //! Pointer to Sensor Pins Info array.
    uint8_t num_sp;
    uint8_t dis;
  }
  touch_qt_pin_t;

//! Autonomous Touch pin configuration type.
  typedef struct tag_touch_at_pin_t
  {
    //! AT Sense Pin selected.
    uint8_t atsp;
    //! AT Discharge Pin selected.
    uint8_t dis;
  }
  touch_at_pin_t;

//! QTouch register configuration type.
  typedef struct tag_touch_qt_reg_t
  {
    //! Clock Divider.
    uint8_t  cksel;
    //! Settle Length.
    uint8_t  charget;
    //! Repeat measurements.
    uint8_t  repeat;
    //! Spread Spectrum mode of operation.
    uint8_t  spread;
    //! Discharge time.
    uint32_t discharget;
  }
  touch_qt_reg_t;

//! Autonomous Touch register configuration type.
  typedef struct tag_touch_at_reg_t
  {
    uint8_t  repeat;
    uint8_t  spread;
    uint8_t  threshold;
    uint8_t  di;
    uint8_t  charget;
    uint8_t  cksel;
    uint8_t  esamples;
    uint16_t tidle;
    uint16_t tlevel;
    uint32_t discharget;
  }
  touch_at_reg_t;

//! QTouch configuration input.
  typedef struct tag_touch_qt_config_t
  {
    //! QTouch Grp A or Grp B number of channels.
    uint8_t num_channels;
    //! QTouch Grp A or Grp B number of sensors.
    uint8_t num_sensors;
    //! QTouch Grp A or Grp B number of rotors and sliders.
    uint8_t num_rotors_and_sliders;
    //! Pin configuration info.
    touch_qt_pin_t pin;
    //! QTouch Grp A or Grp B register configuration info.
    touch_qt_reg_t reg;
    //! QTouch Grp A or Grp B global sensor configuration info.
    touch_global_param_t global_param;
    //! Pointer to QTouch Grp A or Grp B data block buffer.
    uint8_t *p_data_blk;
    //! size of data block buffer
    uint16_t buffer_size;
    //! Filter Callback function for signal values.
    void (*filter_callback) (touch_filter_data_t * p_filter_data);
  }
  touch_qt_config_t;

//! Autonomous Touch configuration input.
  typedef struct tag_touch_at_config_t
  {
    //! Pin configuration info.
    touch_at_pin_t pin;
    //! Autonomous Touch register configuration info.
    touch_at_reg_t reg;
    //! Autonomous Touch Status Change Callback function.
    void (*touch_at_status_change_interrupt_callback)
      ( touch_at_status *p_at_status );
  }
  touch_at_config_t;

//! Touch Input Configuration type.
  typedef struct tag_touch_config_t
  {
    //! QTouch configuration pointer.
    touch_qt_config_t *p_qt_config;
    //! Autonomous QTouch configuration pointer.
    touch_at_config_t *p_at_config;
  }
  touch_config_t;

//! Touch library information type.
  typedef struct tag_touch_info_t
  {
    //! Touch Library state specific to method.
    touch_lib_state_t tlib_state;
    //! Number of channels in use, irrespective of the corresponding Sensor being disabled or enabled.
    uint8_t num_channels_in_use;
    //! Number of sensors in use, irrespective of the Sensor being disabled or enabled.
    uint8_t num_sensors_in_use;
    //! Number of rotor sliders in use, irrespective of the Rotor/Slider being disabled or enabled.
    uint8_t num_rotors_sliders_in_use;
    //! Max possible number of channels per rotor or slider.
    uint8_t max_channels_per_rotor_slider;
    //! Capacitive Touch (CAT) module version.
    uint32_t hw_version;
    //! QTouch Library version.
    uint16_t fw_version;
  }
  touch_info_t;

/*----------------------------------------------------------------------------
                            Structure Declarations
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
                                global variables
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
                                extern variables
----------------------------------------------------------------------------*/
//! QTouch Library Timing info.
extern touch_qt_time_t touch_qt_time;

//! QTouch measured data pointer.
extern touch_measure_data_t *p_qt_measure_data;

/*----------------------------------------------------------------------------
                                static variables
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
                                prototypes
----------------------------------------------------------------------------*/
/*! \name QTouch Helper API.
 */
//! @{

/*! \brief This API is used to initialize and configure the Touch Library.
 * \return touch_ret_t: QTouch Library Error status.
 */
  touch_ret_t touch_sensors_init (void);

/*! \brief This API is used to perform a Single Capacitance measurement
 * using the QTouch Library.
 * \return touch_ret_t: QTouch Library Error status.
 */
  touch_ret_t touch_sensors_measure (void);

/*! \brief This API is used to deinitialize capacitive touch acquisition
 * on QTouch Sensors.
 */
  void touch_sensors_deinit (void);

/*! \brief This API is used to update Timing info for QTouch Library.
 * Typical usage of this API is inside a periodic timer ISR.
 * \return touch_ret_t: QTouch Library Error status.
 */
  void touch_sensors_update_time (void);

/*! \brief This API is used to enable Autonomous QTouch Sensor that can
 * be used to Wakeup the CPU from Deep-Sleep modes.
 * \return touch_ret_t: QTouch Library Error status.
 */
  touch_ret_t touch_autonomous_sensor_enable (void);

/*! \brief This API is used to disable Autonomous QTouch Sensor.
 * \return touch_ret_t: QTouch Library Error status.
 */
  void touch_autonomous_sensor_disable (void);

//! @}

/*----------------------------------------------------------------------------
                                QTouch API
----------------------------------------------------------------------------*/
/*! \name QTouch API.
 */
//! @{

/**
  * touch_ret_t touch_qt_sensors_init( touch_config_t *p_touch_config);
  */
#define touch_qt_sensors_init( y )  \
      touch_qt_sensors_init_with_rs_table( (y), PRIV_QT_RS_TABLE_INIT );

/*! \brief This API is used to initialize the Touch Library with QTouch
 * pin, register and sensor configuration provided by the user.
 *
 * \param  p_touch_config: Pointer to Touch configuration structure.
 * \return touch_ret_t: QTouch Library Error status.
 */
  touch_ret_t touch_qt_sensors_init_with_rs_table (touch_config_t *p_touch_config,
						   void (*rs_table_init) (void));

/*! \brief This API can be used to configure a sensor of type key, rotor or slider.
 *
 * \param  sensor_type: can be of type key, rotor or slider.
 * \param  from_channel: the first channel in the slider sensor.
 * \param  to_channel: the last channel in the slider sensor.
 * \param  aks_group: which AKS group (if any) the sensor is in.
 * \param  detect_threshold: the sensor detection threshold.
 * \param  detect_hysteresis: the sensor detection hysteresis value.
 * \param  position_resolution: the resolution of the reported position value.
 * \param  p_sensor_id: The sensor id value of the configured sensor is updated
 * by the Touch Library.
 * \note   Range of position_hysteresis value is from 0 to 7.
 * \return touch_ret_t: QTouch Library Error status.
 */
  touch_ret_t touch_qt_sensor_config (
				      sensor_type_t sensor_type,
				      channel_t from_channel,
				      channel_t to_channel,
				      aks_group_t aks_group,
				      threshold_t detect_threshold,
				      hysteresis_t detect_hysteresis,
				      resolution_t position_resolution,
				      sensor_id_t * p_sensor_id);



/*! \brief This API is used to calibrate the sensors for the first time before
 * starting a Touch measurement.  This API can also be used to force
 * calibration of sensors when any of the Touch sensor parameters are
 * changed during runtime.
 *
 * \return touch_ret_t: QTouch Library Error status.
 */
  touch_ret_t touch_qt_sensors_calibrate ( void );

/*! \brief This API can be used to start a QTouch measurement.
 *
 * \param  current_time_ms: Current time in millisecond.
 * \param  qt_dma_ch: The dma channel to be used for Touch measurement.
 * \param  qt_acq_mode: Specify whether Raw acquisition mode or Normal
 * acquisition mode is be used.
 * \param  measure_complete_callback: This callback function is called by
 * the Touch Library once the QMatrix Touch measurement is complete.
 * This callback function provides pointers related to the measured
 * touch data and touch status.
 * p_measure_data: QTouch sensor status and measured data pointer.
 * \return touch_ret_t: QTouch Library Error status.
 */
  touch_ret_t touch_qt_sensors_start_acquisition (
						  touch_time_t
						  current_time_ms,
						  touch_qt_dma_t qt_dma_ch,
						  touch_acq_mode_t
						  qt_acq_mode,
						  void
						  (*measure_complete_callback)
                                                    (void));

/*! \brief This API can be used to get the Touch Library configuration
 * information for QTouch acquisition method.
 *
 * \param  p_touch_info: Pointer to the Touch info data structure that will be
 * updated by the Touch Library.
 * \return touch_ret_t: QTouch Library Error status.
 */
  touch_ret_t touch_qt_get_libinfo (touch_info_t * p_touch_info);

/*! \brief This API can be used retrieve the delta value corresponding to
 * a given sensor.
 *
 * \param  sensor_id: The sensor id for which delta value is being seeked.
 * \param  p_delta: Pointer to the delta variable to be updated by the Touch
 * Library.
 * \return touch_ret_t: QTouch Library Error status.
 */
  touch_ret_t touch_qt_sensor_get_delta (
					 sensor_id_t sensor_id,
					 touch_delta_t * p_delta);



/*! \brief Touch Library Event Dispatcher.
 *
 * \note   This API does not apply for Autonomous QTouch.
 */
  void touch_event_dispatcher (void);

/*! \brief This API can be used to deinitialize the touch library.  Once this
 * API is called, capacitive touch acquisition is stopped and the CATB hardware
 * module is disabled. Before calling this API, use touch_qt_get_libinfo(&touch_info)
 * and ensure that the library state is TOUCH_STATE_READY.
 * if (touch_info.tlib_state == TOUCH_STATE_READY)
 * { touch_sensors_deinit();}
 *
 * \return touch_ret_t: QTouch Library Error status.
 */
  void touch_qt_deinit (void);

//! @}

/*----------------------------------------------------------------------------
                          Autonomous QTouch API
----------------------------------------------------------------------------*/
/*! \name Autonomous QTouch API.
 */
//! @{

/*! \brief This API is used to initialize the Touch Library with Autonomous
 * Touch pin and register configuration provided by the user.
 *
 * \param  p_touch_config: Pointer to Touch configuration structure.
 * \return touch_ret_t: QTouch Library Error status.
 */
  touch_ret_t touch_at_sensor_enable (touch_config_t * p_touch_config);

/*! \brief Stop the Autonomous Touch sensor from doing Touch measurement.
 *
 * \return touch_ret_t: QTouch Library Error status.
 */
  touch_ret_t touch_at_sensor_disable (void);

/*! \brief This API can be used to read back Autonomous Touch configuration
 * and status information in addition to the Touch Library version.
 *
 * \return touch_ret_t: QTouch Library Error status.
 */
  touch_ret_t touch_at_get_libinfo (touch_info_t * p_touch_info);

//! @}

/*----------------------------------------------------------------------------
                           Extern Functions
----------------------------------------------------------------------------*/
/*! \name QTouch Library internal use functions.
 */
//! @{

/*! \brief This is an extern function of the Touch Library.
 * This function is NOT to be used by the user.
 */
extern void touch_qt_rs_table_init (void);

//! @}

#ifdef __cplusplus
}
#endif

/**
 * \}
 */
#endif    /* TOUCH_API_SAM4L_H */
