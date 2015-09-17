/**
 * \file touch_api_at32uc3l.h
 *
 * \brief  This file contains the AT32UC3L QTouch Library API interface for
 *         QMatrix, QTouch and Autonomous Touch method using the Capacitive
 *         Touch (CAT) module.
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

#ifndef TOUCH_API_AT32UC3L_H
#define TOUCH_API_AT32UC3L_H

/*----------------------------------------------------------------------------
                            compiler information
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
                                include files
----------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#if (defined __ICCAVR32__)
  #include <intrinsics.h>
#endif

#include "touch_config_at32uc3l.h"

/*----------------------------------------------------------------------------
                            manifest constants
----------------------------------------------------------------------------*/
/* No Touch activity. */
#define TOUCH_NO_ACTIVITY             (0x0000u)
/* At least one Touch channel is in detect. */
#define TOUCH_IN_DETECT               (0x0001u)
/* Change in Touch status of at least one Touch channel. */
#define TOUCH_STATUS_CHANGE           (0x0002u)
/* Change in Rotor or Slider position of at least one rotor or slider. */
#define TOUCH_ROTOR_SLIDER_POS_CHANGE (0x0004u)
/* Change in Reference value of at least one Touch channel. */
#define TOUCH_CHANNEL_REF_CHANGE      (0x0008u)
/* Indicates that reburst is required to resolve Filtering or Calibration state. */
#define TOUCH_BURST_AGAIN             (0x0100u)
/* Indicates that reburst is needed to resolve Calibration.  */
#define TOUCH_RESOLVE_CAL             (0x0200u)
/* Indicates that reburst is needed to resolve Filtering.  */
#define TOUCH_RESOLVE_FILTERIN        (0x0400u)
/* Indicates that reburst is needed to resolve Detect Integration. */
#define TOUCH_RESOLVE_DI              (0x0800u)
/* Indicates that reburst is needed to resolve Recalibration. */
#define TOUCH_RESOLVE_POS_RECAL       (0x1000u)

/* QTouch Group A. */
#define TOUCH_QT_GRP_A                (0u)
/* QTouch Group B. */
#define TOUCH_QT_GRP_B                (1u)

/*----------------------------------------------------------------------------
                             QMatrix macros.
----------------------------------------------------------------------------*/
#if DEF_TOUCH_QMATRIX == 1
/* QMatrix macros for quick error check. */
#if (QM_NUM_X_LINES == 0u)
  #error 'Number of X Lines cannot be 0.'
#endif

#if (QM_NUM_Y_LINES == 0u)
  #error 'Number of Y Lines cannot be 0.'
#endif

/* QMatrix Number of Channels. */
#define QM_NUM_CHANNELS           (QM_NUM_X_LINES * QM_NUM_Y_LINES)

#if (QM_NUM_SENSORS == 0u)
  #error 'Number of Sensors cannot be 0.'
#endif

#if (QM_NUM_SENSORS > QM_NUM_CHANNELS)
  #error 'Number of Sensors cannot exceed (Num X Lines * Num Y Lines).'
#endif

#if (QM_CONSEN > 5u)
  #error 'QM_CONSEN value cannot be greater than 5u .'
#endif

/* Size of QMatrix Sensor. */
#define PRIV_QM_SIZE_SENSOR       (8u)

/* Size of QMatrix Channel. */
#define PRIV_QM_SIZE_CHANNEL      (5u)

/* Size of QMatrix Rotor Slider. */
#define PRIV_QM_SIZE_RS           (11u)

/* Size of QMatrix Pad bytes. */
#define PRIV_QM_PAD_BYTE_SIZE     (36u)

/* QMatrix X Sense pairs mask. */
#define PRIV_QM_X_SENSE_PAIR_MASK ((((QM_X_PINS_SELECTED) & 0x00015555u) << 1u ) |    \
                                    ((QM_X_PINS_SELECTED) & 0x0002AAAAu))

/* QMatrix number of Sense pairs. */
#define PRIV_QM_NUM_X_SENSE_PAIRS ((((PRIV_QM_X_SENSE_PAIR_MASK) >> 1u)  & 0x00000001u) +  \
                                   (((PRIV_QM_X_SENSE_PAIR_MASK) >> 3u)  & 0x00000001u) +  \
                                   (((PRIV_QM_X_SENSE_PAIR_MASK) >> 5u)  & 0x00000001u) +  \
                                   (((PRIV_QM_X_SENSE_PAIR_MASK) >> 7u)  & 0x00000001u) +  \
                                   (((PRIV_QM_X_SENSE_PAIR_MASK) >> 9u)  & 0x00000001u) +  \
                                   (((PRIV_QM_X_SENSE_PAIR_MASK) >> 11u) & 0x00000001u) +  \
                                   (((PRIV_QM_X_SENSE_PAIR_MASK) >> 13u) & 0x00000001u) +  \
                                   (((PRIV_QM_X_SENSE_PAIR_MASK) >> 15u) & 0x00000001u) +  \
                                   (((PRIV_QM_X_SENSE_PAIR_MASK) >> 17u) & 0x00000001u))

/* QMatrix burst length write count. */
#define PRIV_QM_BURST_LENGTH_WRITE_COUNT  (((((QM_Y_PAIRS_SELECTED)  >> 4u) & 0x0Fu) != 0u) + 1u)

/* QMatrix Data block size. */
#define PRIV_QM_DATA_BLK_SIZE     ((PRIV_QM_SIZE_SENSOR          * QM_NUM_SENSORS)          + \
                                   (PRIV_QM_SIZE_CHANNEL         * QM_NUM_CHANNELS)         + \
                                   (PRIV_QM_SIZE_RS              * QM_NUM_ROTORS_SLIDERS)   + \
                                   (((PRIV_QM_NUM_X_SENSE_PAIRS) * 2u) *                      \
                                     (4u * PRIV_QM_BURST_LENGTH_WRITE_COUNT) * 5u)          + \
                                   ((QM_NUM_CHANNELS+ 7u) / 8u)  + (PRIV_QM_PAD_BYTE_SIZE))

/* Initialize QMatrix rotor slider table. */
#if QM_NUM_ROTORS_SLIDERS != 0u
  #define PRIV_QM_RS_TABLE_INIT   touch_qm_rs_table_init
#else
  #define PRIV_QM_RS_TABLE_INIT   NULL
#endif
#endif /* DEF_TOUCH_QMATRIX. */

/*----------------------------------------------------------------------------
                           QTouch Group A macros.
----------------------------------------------------------------------------*/
#if DEF_TOUCH_QTOUCH_GRP_A == 1

#if (QTA_NUM_SENSORS == 0u)
  #error 'Number of Sensors cannot be 0.'
#endif

/* QTouch Group A number of channels. */
#define QTA_NUM_CHANNELS          ((((QTA_SP_SELECTED) >> 0u)  & 0x00000001u) +  \
                                   (((QTA_SP_SELECTED) >> 1u)  & 0x00000001u) +  \
                                   (((QTA_SP_SELECTED) >> 2u)  & 0x00000001u) +  \
                                   (((QTA_SP_SELECTED) >> 3u)  & 0x00000001u) +  \
                                   (((QTA_SP_SELECTED) >> 4u)  & 0x00000001u) +  \
                                   (((QTA_SP_SELECTED) >> 5u)  & 0x00000001u) +  \
                                   (((QTA_SP_SELECTED) >> 6u)  & 0x00000001u) +  \
                                   (((QTA_SP_SELECTED) >> 7u)  & 0x00000001u) +  \
                                   (((QTA_SP_SELECTED) >> 8u)  & 0x00000001u) +  \
                                   (((QTA_SP_SELECTED) >> 9u)  & 0x00000001u) +  \
                                   (((QTA_SP_SELECTED) >> 10u) & 0x00000001u) +  \
                                   (((QTA_SP_SELECTED) >> 11u) & 0x00000001u) +  \
                                   (((QTA_SP_SELECTED) >> 12u) & 0x00000001u) +  \
                                   (((QTA_SP_SELECTED) >> 13u) & 0x00000001u) +  \
                                   (((QTA_SP_SELECTED) >> 14u) & 0x00000001u) +  \
                                   (((QTA_SP_SELECTED) >> 15u) & 0x00000001u) +  \
                                   (((QTA_SP_SELECTED) >> 16u) & 0x00000001u))

/* Size of QTouch Group A Sensor. */
#define PRIV_QTA_SIZE_SENSOR      (8u)

/* Size of QTouch Group A Channel. */
#define PRIV_QTA_SIZE_CHANNEL     (9u)

/* Size of QTouch Group A Rotor Slider. */
#define PRIV_QTA_SIZE_RS          (13u)

/* Size of QTouch Group A Pad bytes. */
#define PRIV_QTA_PAD_BYTE_SIZE    (36u)

/* QTouch Group A Data block size. */
#define PRIV_QTA_DATA_BLK_SIZE    ((PRIV_QTA_SIZE_SENSOR          * QTA_NUM_SENSORS)        + \
                                   (PRIV_QTA_SIZE_CHANNEL         * QTA_NUM_CHANNELS)       + \
                                   (PRIV_QTA_SIZE_RS              * QTA_NUM_ROTORS_SLIDERS) + \
                                   ((QTA_NUM_CHANNELS+ 7u) / 8u)  + (PRIV_QTA_PAD_BYTE_SIZE))
#endif /* DEF_TOUCH_QTOUCH_GRP_A. */

/*----------------------------------------------------------------------------
                           QTouch Group B macros.
----------------------------------------------------------------------------*/
#if DEF_TOUCH_QTOUCH_GRP_B == 1

#if (QTB_NUM_SENSORS == 0u)
  #error 'Number of Sensors cannot be 0.'
#endif

/* QTouch Group B number of channels. */
#define QTB_NUM_CHANNELS          ((((QTB_SP_SELECTED) >> 0u)  & 0x00000001u) +  \
                                   (((QTB_SP_SELECTED) >> 1u)  & 0x00000001u) +  \
                                   (((QTB_SP_SELECTED) >> 2u)  & 0x00000001u) +  \
                                   (((QTB_SP_SELECTED) >> 3u)  & 0x00000001u) +  \
                                   (((QTB_SP_SELECTED) >> 4u)  & 0x00000001u) +  \
                                   (((QTB_SP_SELECTED) >> 5u)  & 0x00000001u) +  \
                                   (((QTB_SP_SELECTED) >> 6u)  & 0x00000001u) +  \
                                   (((QTB_SP_SELECTED) >> 7u)  & 0x00000001u) +  \
                                   (((QTB_SP_SELECTED) >> 8u)  & 0x00000001u) +  \
                                   (((QTB_SP_SELECTED) >> 9u)  & 0x00000001u) +  \
                                   (((QTB_SP_SELECTED) >> 10u) & 0x00000001u) +  \
                                   (((QTB_SP_SELECTED) >> 11u) & 0x00000001u) +  \
                                   (((QTB_SP_SELECTED) >> 12u) & 0x00000001u) +  \
                                   (((QTB_SP_SELECTED) >> 13u) & 0x00000001u) +  \
                                   (((QTB_SP_SELECTED) >> 14u) & 0x00000001u) +  \
                                   (((QTB_SP_SELECTED) >> 15u) & 0x00000001u) +  \
                                   (((QTB_SP_SELECTED) >> 16u) & 0x00000001u))

/* Size of QTouch Group B Sensor. */
#define PRIV_QTB_SIZE_SENSOR      (8u)

/* Size of QTouch Group B Channel. */
#define PRIV_QTB_SIZE_CHANNEL     (9u)

/* Size of QTouch Group B Rotor Slider. */
#define PRIV_QTB_SIZE_RS          (13u)

/* Size of QTouch Group B Pad bytes. */
#define PRIV_QTB_PAD_BYTE_SIZE    (36u)

/* QTouch Group B Data block size. */
#define PRIV_QTB_DATA_BLK_SIZE    ((PRIV_QTB_SIZE_SENSOR          * QTB_NUM_SENSORS)        + \
                                   (PRIV_QTB_SIZE_CHANNEL         * QTB_NUM_CHANNELS)       + \
                                   (PRIV_QTB_SIZE_RS              * QTB_NUM_ROTORS_SLIDERS) + \
                                   ((QTB_NUM_CHANNELS+ 7u) / 8u)  + (PRIV_QTB_PAD_BYTE_SIZE))
#endif /* DEF_TOUCH_QTOUCH_GRP_B. */

/*----------------------------------------------------------------------------
                Common Macros for QTouch Group A and QTouch Group B .
----------------------------------------------------------------------------*/
/* Initialize rotor slider table. */
#if   ( (DEF_TOUCH_QTOUCH_GRP_A == 1  && DEF_TOUCH_QTOUCH_GRP_B == 1) && \
        (QTA_NUM_ROTORS_SLIDERS != 0u || QTB_NUM_ROTORS_SLIDERS != 0u) )
  #define PRIV_QT_RS_TABLE_INIT  touch_qt_rs_table_init
#elif ( (DEF_TOUCH_QTOUCH_GRP_A == 1 && DEF_TOUCH_QTOUCH_GRP_B == 0) &&  \
        (QTA_NUM_ROTORS_SLIDERS != 0u) )
  #define PRIV_QT_RS_TABLE_INIT  touch_qt_rs_table_init
#elif ( (DEF_TOUCH_QTOUCH_GRP_A == 0 && DEF_TOUCH_QTOUCH_GRP_B == 1) &&  \
        (QTB_NUM_ROTORS_SLIDERS != 0u) )
  #define PRIV_QT_RS_TABLE_INIT  touch_qt_rs_table_init
#else
  #define PRIV_QT_RS_TABLE_INIT  NULL
#endif  /* DEF_TOUCH_QTOUCH_GRP_A && DEF_TOUCH_QTOUCH_GRP_B. */

/*----------------------------------------------------------------------------
                            type definitions
----------------------------------------------------------------------------*/
/* Sensor start and end channel type of a Sensor. Channel number starts with value 0. */
typedef uint8_t  channel_t;

/* An unsigned 8-bit number setting a sensor detection threshold. */
typedef uint8_t  threshold_t;

/* Sensor number type. */
typedef uint8_t  sensor_id_t;

/* Current time type. */
typedef uint16_t touch_time_t;

/* QMatrix individual sensor burst length type. */
typedef uint8_t  touch_bl_t;

/* Touch sensor delta value type. */
typedef int16_t  touch_delta_t;

/* Status of Touch measurement. */
typedef uint16_t touch_acq_status_t;

/* QTouch Group type. Used to identify TOUCH_QT_GRP_A or TOUCH_QT_GRP_B. */
typedef uint8_t  touch_qt_grp_t;

/* QTouch Group A/ Group B DMA channel type. */
typedef uint8_t  touch_qt_dma_t;

/* Touch Library error codes. */
typedef enum tag_touch_ret_t
{
  TOUCH_SUCCESS,                                           /* Successful completion of operation. */
  TOUCH_ACQ_INCOMPLETE,                                    /* Touch Library is busy with pending previous Touch measurement. */
  TOUCH_INVALID_INPUT_PARAM,                               /* Invalid input parameter. */
  TOUCH_INVALID_LIB_STATE,                                 /* Operation not allowed in the current Touch Library state. */
  TOUCH_INVALID_QM_CONFIG_PARAM,                           /* Invalid QMatrix config input parameter. */
  TOUCH_INVALID_AT_CONFIG_PARAM,                           /* Invalid Autonomous Touch config input parameter. */
  TOUCH_INVALID_QT_CONFIG_PARAM,                           /* Invalid QTouch config input parameter. */
  TOUCH_INVALID_GENERAL_CONFIG_PARAM,                      /* Invalid General config input parameter. */
  TOUCH_INVALID_QM_NUM_X_LINES,                            /* Mismatch between number of X lines specified as QM_NUM_X_LINES
                                                                    and number of X lines enabled in QMatrix pin configuration
                                                                    touch_qm_pin_t x_lines. */
  TOUCH_INVALID_QM_NUM_Y_LINES,                            /* Mismatch between number of Y lines specified as QM_NUM_Y_LINES
                                                                    and number of Y lines enabled in QMatrix pin configuration
                                                                    touch_qm_pin_t y_yk_lines. */
  TOUCH_INVALID_QM_NUM_SENSORS,                            /* Number of Sensors specified is greater than
                                                                    (Number of X Lines * Number of Y Lines) */
  TOUCH_INVALID_MAXDEV_VALUE,                              /* Spread spectrum MAXDEV value should not exceed (2*DIV + 1). */
  TOUCH_INVALID_RECAL_THRESHOLD,                           /* Invalid Recalibration threshold input value. */
  TOUCH_INVALID_CHANNEL_NUM,                               /* Channel number parameter exceeded total number of channels configured. */
  TOUCH_INVALID_SENSOR_TYPE,                               /* Invalid sensor type. Sensor type can NOT be SENSOR_TYPE_UNASSIGNED */
  TOUCH_INVALID_SENSOR_ID,                                 /* Invalid Sensor number parameter. */
  TOUCH_INVALID_DMA_PARAM,                                 /* DMA Channel numbers are out of range. */
  TOUCH_FAILURE_ANALOG_COMP,                               /* Analog comparator configuration error.                                                                    Analog comparator output is not ready. */
  TOUCH_INVALID_RS_NUM                                     /* Number of Rotor/Sliders set as 0,
                                                                 when trying to configure a rotor/slider*/
} touch_ret_t;

/* Touch library state. */
typedef enum tag_touch_lib_state_t
{
  TOUCH_STATE_NULL,
  TOUCH_STATE_INIT,
  TOUCH_STATE_READY,
  TOUCH_STATE_CALIBRATE,
  TOUCH_STATE_BUSY
} touch_lib_state_t;

/* Touch library acquisition mode. */
typedef enum tag_touch_acq_mode_t
{
  RAW_ACQ_MODE,                                            /* When Raw acquisition mode is used, the measure_complete_callback
                                                              function is called immediately once a fresh value of Signals are
                                                              available.  In this mode, the Touch Library does not do any
                                                              processing on the Signals.  So, the References, Sensor states or
                                                              Rotor/Slider position values are not updated in this mode. */
  NORMAL_ACQ_MODE                                          /* When Normal acquisition mode is used, the measure_complete_callback
                                                              function is called only after the Touch Library completes processing
                                                              of the Signal values obtained. The References, Sensor states and
                                                              Rotor/Slider position values are updated in this mode. */
} touch_acq_mode_t;

/* Sensor types available. */
typedef enum tag_sensor_type_t
{
  SENSOR_TYPE_UNASSIGNED,
  SENSOR_TYPE_KEY,
  SENSOR_TYPE_ROTOR,
  SENSOR_TYPE_SLIDER,
  MAX_SENSOR_TYPE
} sensor_type_t;

/*
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
} aks_group_t;

/*
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
} hysteresis_t;

/*
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
} recal_threshold_t;

/*
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
} resolution_t;

/*
 * Autonomous Touch status change enum.
 */
typedef enum tag_at_status_change_t
{
  OUT_OF_TOUCH,                                            /* Autonomous Touch Status has changed to Out of Touch. */
  IN_TOUCH                                                 /* Autonomous Touch Status has changed to In Touch. */
} at_status_change_t;

/* QMatrix Sensor pair types available. */
typedef enum tag_x_pin_options_t
{
  X0,
  X1,
  X2,
  X3,
  X4,
  X5,
  X6,
  X7,
  X8,                                                      /* Note: X8 pin must NOT be used as X Line and it is recommended to be used as ACREFN pin for QMatrix. */
  X9,
  X10,
  X11,
  X12,
  X13,
  X14,
  X15,
  X16,
  X17
} x_pin_options_t;

/* QMatrix Y Pin options available. */
typedef enum tag_y_pin_options_t
{
  Y0_YK0,
  Y1_YK1,
  Y2_YK2,
  Y3_YK3,
  Y4_YK4,
  Y5_YK5,
  Y6_YK6,
  Y7_YK7
} y_pin_options_t;

/* Autonomous QTouch, QTouch Group A and QTouch Group B Pin options available. */
typedef enum tag_qt_pin_options_t
{
  SP0,
  SP1,
  SP2,
  SP3,
  SP4,
  SP5,
  SP6,
  SP7,
  SP8,
  SP9,
  SP10,
  SP11,
  SP12,
  SP13,
  SP14,
  SP15,
  SP16
} qt_pin_options_t;

/* Options for SMP, DIS, VDIV and SYNC pins. */
typedef enum tag_general_pin_options_t
{
  /* pin not included in the hardware design. */
  USE_NO_PIN     = 0u,
  /* smp pin options*/
  USE_PIN_PA12_AS_SMP,
  USE_PIN_PA13_AS_SMP,
  USE_PIN_PA14_AS_SMP,
  USE_PIN_PA17_AS_SMP,
  USE_PIN_PA21_AS_SMP,
  USE_PIN_PA22_AS_SMP,
  /* dis pin options*/
  USE_PIN_PA17_AS_DIS,
  /* vdiv pin options*/
  USE_PIN_PB11_AS_VDIV,
  /* sync pin options*/
  USE_PIN_PA15_AS_SYNC,
  USE_PIN_PA18_AS_SYNC,
  USE_PIN_PA19_AS_SYNC,
  USE_PIN_PB08_AS_SYNC,
  USE_PIN_PB12_AS_SYNC,
} general_pin_options_t;

/* Sensor structure for storing sensor related information. */
typedef struct tag_sensor_t
{
  uint8_t state;                                           /* sensor state (calibrating, on, ...) */
  uint8_t general_counter;                                 /* general purpose ctr:
                                                              used for calibration, drifting, etc */
  uint8_t ndil_counter;                                    /* Detect Integration ctr */
  uint8_t threshold;                                       /* sensor detection threshold */
  uint8_t type_aks_pos_hyst;                               /* bits 7..6: sensor type:
                                                              {00: key,01: rotor,10: slider,11: reserved}
                                                              bits 5..3: AKS group (0..7): 0 = no AKS group
                                                              bit 2    : positive recal flag
                                                              bits 1..0: hysteresis   */
  uint8_t from_channel;                                    /* sensor from channel
                                                              for keys: from channel = to channel
                                                              rotors  : Top channel
                                                              sliders : Left most channel
                                                              NB:only need to_channel for keys in rotors/sliders build */
  uint8_t to_channel;                                      /* FOR KEY's: this is unused,
                                                              rotors:  Bottom left channel
                                                              sliders: Middle  channel*/
  uint8_t index;                                           /* index into array of rotor/slider values */
} sensor_t;

/* Global sensor configuration info. */
typedef struct tag_touch_global_param_t
{
  uint8_t  di;                                             /* Sensor detect integration (DI) limit. */
  uint8_t  neg_drift_rate;                                 /* Sensor negative drift rate. */
  uint8_t  pos_drift_rate;                                 /* Sensor positive drift rate. */
  uint8_t  max_on_duration;                                /* Sensor maximum on duration. */
  uint8_t  drift_hold_time;                                /* Sensor drift hold time. */
  uint8_t  pos_recal_delay;                                /* Sensor Positive recalibration delay. */
  recal_threshold_t recal_threshold;                       /* Sensor recalibration threshold. */
} touch_global_param_t;

/* Touch Filter Callback data type. */
typedef struct tag_touch_filter_data_t
{
  uint8_t             num_channel_signals;                 /* Length of the measured signal values list. */
  uint16_t            *p_channel_signals;                  /* Pointer to measured signal values for each channel. */
} touch_filter_data_t;

/* Touch Measured data type. */
typedef struct tag_touch_measure_data_t
{
  touch_acq_status_t  acq_status;                          /* Status of Touch measurement. */

  uint8_t             num_channel_signals;                 /* Length of the measured signal values list. */
  uint16_t            *p_channel_signals;                  /* Pointer to measured signal values for each channel. */

  uint8_t             num_channel_references;              /* Length of the measured reference values list. */
  uint16_t            *p_channel_references;               /* Pointer to reference values for each channel. */

  uint8_t             num_sensor_states;                   /* Number of sensor state bytes. */
  uint8_t             *p_sensor_states;                    /* Pointer to Touch Status of each sensor. */

  uint8_t             num_rotor_slider_values;             /* Length of the Rotor and Slider position values list. */
  uint8_t             *p_rotor_slider_values;              /* Pointer to Rotor and Slider position values. */

  uint8_t             num_sensors;                         /* Length of the sensors data list. */
  sensor_t            *p_sensors;                          /* Pointer to Sensor data. */
} touch_measure_data_t;

/* QMatrix sensor configuration type. */
typedef struct tag_touch_qm_param_t
{
  aks_group_t   aks_group;                                 /* Sensor detection threshold. */
  threshold_t   detect_threshold;                          /* Sensor detection threshold. */
  hysteresis_t  detect_hysteresis;                         /* Sensor detection hysteresis. */
  resolution_t  position_resolution;                       /* Sensor position resolution. This is valid only for a Rotor or Slider. */
  uint8_t       position_hysteresis;                       /* Sensor position hysteresis. This is valid only for a Rotor or Slider.
                                                              Range of position_hysteresis value is from 0 to 7.*/
} touch_qm_param_t;

/* Autonomous Touch sensor param type. */
typedef struct tag_touch_at_param_t
{
  uint8_t         filter;                                  /* Autonomous Touch Filter Setting. Refer ATCFG2 register. */
  uint8_t         outsens;                                 /* Out-of-Touch Sensitivity. Refer ATCFG2 register. */
  uint8_t         sense;                                   /* Sensitivity. Refer ATCFG2 register. */
  uint8_t         pthr;                                    /* Positive Recalibration Threshold. Refer ATCFG3 register. */
  uint8_t         pdrift;                                  /* Positive Drift Compensation. Refer ATCFG3 register. */
  uint8_t         ndrift;                                  /* Negative Drift Compensation. Refer ATCFG3 register. */
} touch_at_param_t;

/* QTouch Group A/Group B sensor configuration type. */
typedef struct tag_touch_qt_param_t
{
  aks_group_t   aks_group;                                 /* Sensor detection threshold. */
  threshold_t   detect_threshold;                          /* Sensor detection threshold. */
  hysteresis_t  detect_hysteresis;                         /* Sensor detection hysteresis. */
  resolution_t  position_resolution;                       /* Sensor position resolution. This is valid only for a Rotor or Slider. */
} touch_qt_param_t;

/* Autonomous QTouch status type. */
typedef struct tag_touch_at_status_t
{
  at_status_change_t status_change;                        /* Autonomous QTouch Status change.*/
  uint16_t           base_count;                           /* The base count currently stored by the autonomous touch sensor. This is useful for autonomous touch debugging purposes. */
  uint16_t           current_count;                        /* The current count acquired by the autonomous touch sensor. This is useful for autonomous touch debugging purposes. */
}touch_at_status;

/* QMatrix dma type. */
typedef struct tag_touch_qm_dma_t
{
  uint8_t dma_ch1;                                         /* Dma channel 1 to be used by the Touch Library. */
  uint8_t dma_ch2;                                         /* Dma channel 2 to be used by the Touch Library. */
}touch_qm_dma_t;

/* QMatrix pin configuration type. */
typedef struct tag_touch_qm_pin_t
{
  uint32_t                     x_lines;                    /* X Lines selected. */
  uint8_t                      y_yk_lines;                 /* Y-Yk pairs selected. */
  general_pin_options_t        smp_dis_pin;                /* SMP/DIS pin selected. */
  general_pin_options_t        vdiv_pin;                   /* VDIV pin selected. */
}touch_qm_pin_t;

/* Autonomous Touch pin configuration type. */
typedef struct tag_touch_at_pin_t
{
  uint8_t        atsp;                                     /* Value n of AT Sense pair selected. Refer ATPINS Register.*/
}touch_at_pin_t;

/* QTouch Group A/Group B pin configuration type. */
typedef struct tag_touch_qt_pin_t
{
  uint32_t        sp;                                      /* Bit n indicates Sense Pair SP[n] is selected. Refer PINMODE Register.*/
}touch_qt_pin_t;

/* QMatrix register configuration type. */
typedef struct tag_touch_qm_reg_t
{
  uint16_t        div;                                     /* Clock Divider. */
  uint8_t         chlen;                                   /* Charge Length. */
  uint8_t         selen;                                   /* Settle Length. */
  uint8_t         dishift;                                 /* Discharge Shift. */
  uint8_t         sync;                                    /* Sync Pin enable. */
  uint8_t         spread;                                  /* Spread Spectrum Sensor Drive. */
  uint8_t         dilen;                                   /* Discharge Length. */
  uint16_t        max;                                     /* Maximum Count. */
  uint8_t         acctrl;                                  /* Analog Comparator Control. */
  uint8_t         consen;                                  /* Consensus Filter Length. */
  uint8_t         cxdilen;                                 /* Cx Capacitor Discharge Length. */
  uint16_t        synctim;                                 /* Sync Time Interval. */
  uint8_t         fsources;                                /* Force Discharge Current Sources. */
  uint8_t         glen;                                    /* Global Enable. */
  uint8_t         intvrefsel;                              /* Internal Voltage Reference Select. */
  uint8_t         intrefsel;                               /* Internal Reference Select. */
  uint8_t         trim;                                    /* Reference Current Trimming. */
  uint8_t         sources;                                 /* Enable Discharge Current Sources. */
  uint16_t        shival0;                                 /* Shift Offset Value 0. */
  uint16_t        shival1;                                 /* Shift Offset Value 1. */
  uint16_t        shival2;                                 /* Shift Offset Value 2. */
  uint16_t        shival3;                                 /* Shift Offset Value 3. */
  uint16_t        shival4;                                 /* Shift Offset Value 4. */
  uint16_t        shival5;                                 /* Shift Offset Value 5. */
  uint16_t        shival6;                                 /* Shift Offset Value 6. */
  uint16_t        shival7;                                 /* Shift Offset Value 7. */
}touch_qm_reg_t;

/* Autonomous Touch register configuration type. */
typedef struct tag_touch_at_reg_t
{
  uint16_t        div;                                     /* Clock Divider. */
  uint8_t         chlen;                                   /* Charge Length. */
  uint8_t         selen;                                   /* Settle Length. */
  uint8_t         dishift;                                 /* Discharge Shift. */
  uint8_t         sync;                                    /* Sync Pin enable. */
  uint8_t         spread;                                  /* Spread Spectrum Sensor Drive. */
  uint8_t         dilen;                                   /* Discharge Length. */
  uint16_t        max;                                     /* Maximum Count. */
  touch_at_param_t at_param;                               /* Sensor parameters. */
}touch_at_reg_t;

/* QTouch Group A/Group B register configuration type. */
typedef struct tag_touch_qt_reg_t
{
  uint16_t        div;                                     /* Clock Divider. */
  uint8_t         chlen;                                   /* Charge Length. */
  uint8_t         selen;                                   /* Settle Length. */
  uint8_t         dishift;                                 /* Discharge Shift. */
  uint8_t         sync;                                    /* Sync Pin enable. */
  uint8_t         spread;                                  /* Spread Spectrum Sensor Drive. */
  uint8_t         dilen;                                   /* Discharge Length. */
  uint16_t        max;                                     /* Maximum Count. */
}touch_qt_reg_t;

/* QMatrix configuration input. */
typedef struct tag_touch_qm_config_t
{
  uint8_t                  num_channels;                   /* QMatrix number of channels. */
  uint8_t                  num_sensors;                    /* QMatrix number of sensors. */
  uint8_t                  num_rotors_and_sliders;         /* QMatrix number of rotors and sliders. */
  uint8_t                  num_x_lines;                    /* QMatrix number of X Lines. */
  uint8_t                  num_y_lines;                    /* QMatrix number of Y Lines. */
  uint8_t                  num_x_sp;                       /* QMatrix number of X Sense pairs. */
  uint8_t                  bl_write_count;                 /* QMatrix burst length write count. */
  touch_qm_pin_t           pin;                            /* Pin configuration info. */
  touch_qm_reg_t           reg;                            /* QMatrix register configuration info. */
  touch_global_param_t     global_param;                   /* QMatrix global sensor configuration info. */
  uint8_t                  *p_data_blk;                    /* Pointer to QMatrix data block buffer. */
  uint16_t                 buffer_size;                    /* Size of data block buffer. */
  uint8_t                  *p_burst_length;                /* QMatrix burst length array pointer. */
  void (*filter_callback)( touch_filter_data_t *p_filter_data ); /* Filter Callback function for signal values. */
} touch_qm_config_t;

/* Autonomous Touch configuration input. */
typedef struct tag_touch_at_config_t
{
  touch_at_pin_t           pin;                            /* Pin configuration info. */
  touch_at_reg_t           reg;                            /* Autonomous Touch register configuration info. */
} touch_at_config_t;

/* QTouch Group A/Group B configuration input. */
typedef struct tag_touch_qt_config_t
{
  uint8_t                  num_channels;                   /* QTouch Grp A or Grp B number of channels. */
  uint8_t                  num_sensors;                    /* QTouch Grp A or Grp B number of sensors. */
  uint8_t                  num_rotors_and_sliders;         /* QTouch Grp A or Grp B number of rotors and sliders. */
  touch_qt_pin_t           pin;                            /* Pin configuration info. */
  touch_qt_reg_t           reg;                            /* QTouch Grp A or Grp B register configuration info. */
  touch_global_param_t     global_param;                   /* QTouch Grp A or Grp B global sensor configuration info. */
  uint8_t                  *p_data_blk;                    /* Pointer to QTouch Grp A or Grp B data block buffer. */
  uint16_t                 buffer_size;                    /* size of data block buffer */
  void (*filter_callback)( touch_filter_data_t *p_filter_data ); /* Filter Callback function for signal values. */
} touch_qt_config_t;

/* Touch General configuration input common to QMatrix, QTouch and Autonomous Touch. */
typedef struct tag_touch_general_config_t
{
  general_pin_options_t  sync_pin;
  uint8_t              maxdev;                             /* Max deviation of spread spectrum. */
  uint32_t             csares;                             /* CSA pin group resistor drive enable. */
  uint32_t             csbres;                             /* CSB pin group resistor drive enable. */
}touch_general_config_t;

/* Touch Input Configuration type. */
typedef struct tag_touch_config_t
{
  touch_qm_config_t          *p_qm_config;                 /* QMatrix configuration pointer. */
  touch_at_config_t          *p_at_config;                 /* Autonomous QTouch configuration pointer. */
  touch_qt_config_t          *p_qta_config;                /* QTouch Group A configuration pointer. */
  touch_qt_config_t          *p_qtb_config;                /* QTouch Group B configuration pointer. */
  touch_general_config_t     *p_general_config;            /* General configuration pointer. */
}touch_config_t;

/* Touch library information type. */
typedef struct tag_touch_info_t
{
  touch_lib_state_t tlib_state;                            /* Touch Library state specific to method. */
  uint8_t           num_channels_in_use;                   /* Number of channels in use, irrespective of the corresponding Sensor being disabled or enabled. */
  uint8_t           num_sensors_in_use;                    /* Number of sensors in use, irrespective of the Sensor being disabled or enabled. */
  uint8_t           num_rotors_sliders_in_use;             /* Number of rotor sliders in use, irrespective of the Rotor/Slider being disabled or enabled. */
  uint8_t           max_channels_per_rotor_slider;         /* Max possible number of channels per rotor or slider. */
  uint32_t          hw_version;                            /* Capacitive Touch (CAT) module version. */
  uint16_t          fw_version;                            /* QTouch Library version. */
} touch_info_t;

/*----------------------------------------------------------------------------
                            Structure Declarations
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
                                global variables
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
                                extern variables
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
                                static variables
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
                                prototypes
----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
                                QMatrix API
----------------------------------------------------------------------------*/


/*============================================================================
Name    :   touch_qm_sensors_init
------------------------------------------------------------------------------
Purpose :   This API is used to initialize the Touch Library with QMatrix
            pin, register and sensor configuration provided by the user.
Input   :   p_touch_config: Pointer to Touch configuration structure.
Output  :   n/a
Notes   :
============================================================================*/
/* touch_ret_t touch_qm_sensors_init( touch_config_t *p_touch_config); */

#define touch_qm_sensors_init( x ) touch_qm_sensors_init_with_rs_table( (x) , PRIV_QM_RS_TABLE_INIT )
touch_ret_t touch_qm_sensors_init_with_rs_table( touch_config_t *p_touch_config,
                                                 void (*rs_table_init)(void) );

/*============================================================================
Name    :   touch_qm_sensor_config
------------------------------------------------------------------------------
Purpose :   This API can be used to configure a sensor of type key, rotor or slider.
Input   :   sensor_type: can be of type key, rotor or slider.
            from_channel: the first channel in the slider sensor
            to_channel: the last channel in the slider sensor
            aks_group: which AKS group (if any) the sensor is in
            detect_threshold: the sensor detection threshold
            detect_hysteresis: the sensor detection hysteresis value
            position_resolution: the resolution of the reported position value
            position_hysteresis: the hysteresis of the reported position value
Output  :   p_sensor_id: The sensor id value of the configured sensor is updated
            by the Touch Library.
Notes   :   Range of position_hysteresis value is from 0 to 7.
============================================================================*/
touch_ret_t touch_qm_sensor_config( sensor_type_t sensor_type,
                                        channel_t from_channel,
                                        channel_t to_channel,
                                      aks_group_t aks_group,
                                      threshold_t detect_threshold,
                                     hysteresis_t detect_hysteresis,
                                     resolution_t position_resolution,
                                          uint8_t position_hysteresis,
                                      sensor_id_t *p_sensor_id );

/*============================================================================
Name    :   touch_qm_sensor_update_config
------------------------------------------------------------------------------
Purpose :   This API can be used to set the sensor configuration parameters.
Input   :   p_sensor_id: The sensor id for which configuration parameter
            information is being set.
            p_touch_sensor_param: The touch sensor parameter structure that will
            be used by the Touch Library to update.
Output  :   n/a
Notes   :
============================================================================*/
touch_ret_t touch_qm_sensor_update_config( sensor_id_t sensor_id,
                                      touch_qm_param_t *p_touch_sensor_param );

/*============================================================================
Name    :   touch_qm_sensor_get_config
------------------------------------------------------------------------------
Purpose :   This API can be used to read back the sensor configuration parameters.
Input   :   p_sensor_id: The sensor id for which configuration parameter
            information is being seeked.
Output  :   p_touch_sensor_param: The touch sensor parameter structure that will
            be updated by the Touch Library.
Notes   :
============================================================================*/
touch_ret_t touch_qm_sensor_get_config( sensor_id_t sensor_id,
                                   touch_qm_param_t *p_touch_sensor_param );

/*============================================================================
Name    :   touch_qm_channel_update_burstlen
------------------------------------------------------------------------------
Purpose :   This API can be used to update the burst length of a give input channel.
Input   :   channel_id: The channel id of the Touch sensor for which burst
            length is to be updated.
            qm_burst_length: The burst length value.
Output  :   n/a
Notes   :
============================================================================*/
touch_ret_t touch_qm_channel_update_burstlen( channel_t channel_id,
                                      touch_bl_t qm_burst_length);

/*============================================================================
Name    :   touch_qm_update_global_param
------------------------------------------------------------------------------
Purpose :   This API can be used to update the global parameter of QMatrix method.
Input   :   p_global_param: The pointer to QMatrix global sensor configuration.
Output  :   n/a
Notes   :
============================================================================*/
touch_ret_t touch_qm_update_global_param( touch_global_param_t *p_global_param );

/*============================================================================
Name    :   touch_qm_get_global_param
------------------------------------------------------------------------------
Purpose :   This API can be used to read back the global parameter of QMatrix method.
Input   :   p_global_param: The pointer to readback the QMatrix global sensor
            configuration.
Output  :   n/a
Notes   :
============================================================================*/
touch_ret_t touch_qm_get_global_param( touch_global_param_t *p_global_param );

/*============================================================================
Name    :   touch_qm_sensors_calibrate
------------------------------------------------------------------------------
Purpose :   This API is used to calibrate the sensors for the first time before
            starting a Touch measurement.  This API can also be used to force
            calibration of sensors when any of the Touch sensor parameters are
            changed during runtime.
Input   :   n/a
Output  :   n/a
Notes   :
============================================================================*/
touch_ret_t touch_qm_sensors_calibrate( void );

/*============================================================================
Name    :   touch_qm_sensors_start_acquisition
------------------------------------------------------------------------------
Purpose :   This API can be used to start a QMatrix Touch measurement.
Input   :   current_time_ms: Current time in millisecond.
            p_qm_dma: Pointer to the dma channels to be used for Touch
            measurement.
            qm_acq_mode: Specify whether Raw acquisition mode or Normal
            acquisition mode is be used.
Output  :   measure_complete_callback: This callback function is called by
            the Touch Library once the QMatrix Touch measurement is complete.
            This callback function provides pointers related to the measured
            touch data and touch status.
            p_measure_data: QMatrix sensor status and measured data pointer.
Notes   :
============================================================================*/
touch_ret_t touch_qm_sensors_start_acquisition( touch_time_t current_time_ms,
                                              touch_qm_dma_t *p_qm_dma,
                                            touch_acq_mode_t qm_acq_mode,
     void (*measure_complete_callback)( touch_measure_data_t *p_measure_data ));

/*============================================================================
Name    :   touch_qm_get_libinfo
------------------------------------------------------------------------------
Purpose :   This API can be used to get the Touch Library configuration
            information for QMatrix Touch acquisition method.
Input   :   n/a
Output  :   p_touch_info: Pointer to the Touch info data structure that will be
            updated by the Touch Library.
Notes   :
============================================================================*/
touch_ret_t touch_qm_get_libinfo( touch_info_t *p_touch_info);

/*============================================================================
Name    :   touch_qm_sensor_get_delta
------------------------------------------------------------------------------
Purpose :   This API can be used retrieve the delta value corresponding to
            a given sensor.
Input   :   sensor_id: The sensor id for which delta value is being seeked.
Output  :   p_delta: Pointer to the delta variable to be updated by the Touch
            Library.
Notes   :
============================================================================*/
touch_ret_t touch_qm_sensor_get_delta( sensor_id_t sensor_id, touch_delta_t *p_delta);


/*----------------------------------------------------------------------------
                          Autonomous QTouch API
----------------------------------------------------------------------------*/


/*============================================================================
Name    :   touch_at_sensor_init
------------------------------------------------------------------------------
Purpose :   This API is used to initialize the Touch Library with Autonomous
            Touch pin and register configuration provided by the user.
Input   :   p_touch_config: Pointer to Touch configuration structure.
Output  :   n/a
Notes   :
============================================================================*/
touch_ret_t touch_at_sensor_init( touch_config_t *p_touch_config );

/*============================================================================
Name    :   touch_at_sensor_update_config
------------------------------------------------------------------------------
Purpose :   This API is used to update the register configuration information
            of the Autonomous Touch sensor.
Input   :   p_at_param: Autonomous Touch sensor configuration parameter input
            to the Touch Library.
Output  :   n/a
Notes   :
============================================================================*/
touch_ret_t touch_at_sensor_update_config( touch_at_param_t *p_at_param );

/*============================================================================
Name    :   touch_at_sensor_get_config
------------------------------------------------------------------------------
Purpose :   This API is used to read back the register configuration information
            of the Autonomous Touch sensor.
Input   :   n/a
Output  :   p_at_param: Autonomous Touch sensor configuration parameter updated
            by the Touch Library.
Notes   :
============================================================================*/
touch_ret_t touch_at_sensor_get_config( touch_at_param_t *p_at_param );

/*============================================================================
Name    :   touch_at_sensor_enable
------------------------------------------------------------------------------
Purpose :   Enable the Autonomous Touch sensor to start Touch measurement.
Input   :   touch_at_status_change_interrupt_callback: Autonomous Touch Status
            Change Callback function.
Output  :   at_status: Autonomous Touch status change.
Notes   :
============================================================================*/
touch_ret_t touch_at_sensor_enable( void (*touch_at_status_change_interrupt_callback)
	                                   (touch_at_status *p_at_status) );


/*============================================================================
Name    :   touch_at_sensor_disable
------------------------------------------------------------------------------
Purpose :   Stop the Autonomous Touch sensor from doing Touch measurement.
Input   :   n/a
Output  :   n/a
Notes   :
============================================================================*/
touch_ret_t touch_at_sensor_disable( void);

/*============================================================================
Name    :   touch_at_get_libinfo
------------------------------------------------------------------------------
Purpose :   This API can be used to read back Autonomous Touch configuration
            and status information in addition to the Touch Library version.
Input   :   n/a
Output  :   n/a
Notes   :
============================================================================*/
touch_ret_t touch_at_get_libinfo( touch_info_t *p_touch_info);


/*----------------------------------------------------------------------------
                        QTouch Group A/Group B API
----------------------------------------------------------------------------*/


/*============================================================================
Name    :   touch_qt_sensors_init
------------------------------------------------------------------------------
Purpose :   This API is used to initialize the Touch Library with QTouch
            Group A or QTouch Group B pin, register and sensor configuration
            provided by the user.
Input   :   touch_qt_grp: Choose QTouch Group A or Group B.
            p_touch_config: Pointer to Touch configuration structure.
Output  :   n/a
Notes   :
============================================================================*/
/* touch_ret_t touch_qt_sensors_init( touch_qt_grp_t touch_qt_grp,
                                      touch_config_t *p_touch_config); */

#define touch_qt_sensors_init( x, y )  ((x) == TOUCH_QT_GRP_A)?              \
      touch_qt_sensors_init_with_rs_table( 0u, (y), PRIV_QT_RS_TABLE_INIT, touch_qta_table_init ): \
      touch_qt_sensors_init_with_rs_table( 1u, (y), PRIV_QT_RS_TABLE_INIT, touch_qtb_table_init );

touch_ret_t touch_qt_sensors_init_with_rs_table( touch_qt_grp_t touch_qt_grp,
                                                 touch_config_t *p_touch_config,
                                                 void (*rs_table_init)(void),
                                                 void (*touch_qt_table_init)(void));

/*============================================================================
Name    :   touch_qt_sensor_config
------------------------------------------------------------------------------
Purpose :   This API can be used to configure a sensor of type key, rotor or slider.
Input   :   touch_qt_grp: Choose QTouch Group A or Group B.
            sensor_type: can be of type key, rotor or slider.
            from_channel: the first channel in the slider sensor
            to_channel: the last channel in the slider sensor
            aks_group: which AKS group (if any) the sensor is in
            detect_threshold: the sensor detection threshold
            detect_hysteresis: the sensor detection hysteresis value
            position_resolution: the resolution of the reported position value
Output  :   p_sensor_id: The sensor id value of the configured sensor is updated
            by the Touch Library.
Notes   :   Range of position_hysteresis value is from 0 to 7.
============================================================================*/
touch_ret_t touch_qt_sensor_config( touch_qt_grp_t touch_qt_grp,
                                     sensor_type_t sensor_type,
                                         channel_t from_channel,
                                         channel_t to_channel,
                                       aks_group_t aks_group,
                                       threshold_t detect_threshold,
                                      hysteresis_t detect_hysteresis,
                                      resolution_t position_resolution,
                                       sensor_id_t *p_sensor_id );

/*============================================================================
Name    :   touch_qt_sensor_update_config
------------------------------------------------------------------------------
Purpose :   This API can be used to set the sensor configuration parameters.
Input   :   touch_qt_grp: Choose QTouch Group A or Group B.
            p_sensor_id: The sensor id for which configuration parameter
            information is being set.
            p_touch_sensor_param: The touch sensor parameter structure that will
            be used by the Touch Library to update.
Output  :   n/a
Notes   :
============================================================================*/
touch_ret_t touch_qt_sensor_update_config( touch_qt_grp_t touch_qt_grp,
                                              sensor_id_t sensor_id,
                                         touch_qt_param_t *p_touch_sensor_param );

/*============================================================================
Name    :   touch_qt_sensor_get_config
------------------------------------------------------------------------------
Purpose :   This API can be used to read back the sensor configuration parameters.
Input   :   touch_qt_grp: Choose QTouch Group A or Group B.
            p_sensor_id: The sensor id for which configuration parameter
            information is being seeked.
Output  :   p_touch_sensor_param: The touch sensor parameter structure that will
            be updated by the Touch Library.
Notes   :
============================================================================*/
touch_ret_t touch_qt_sensor_get_config( touch_qt_grp_t touch_qt_grp,
                                           sensor_id_t sensor_id,
                                      touch_qt_param_t *p_touch_sensor_param );

/*============================================================================
Name    :   touch_qt_update_global_param
------------------------------------------------------------------------------
Purpose :   This API can be used to update the global parameter of QTouch method.
Input   :   touch_qt_grp: Choose QTouch Group A or Group B.
            p_global_param: The pointer to QTouch Group A or QTouch Group B
            global sensor configuration.
Output  :   n/a
Notes   :
============================================================================*/
touch_ret_t touch_qt_update_global_param( touch_qt_grp_t touch_qt_grp,
                                    touch_global_param_t *p_global_param );

/*============================================================================
Name    :   touch_qt_get_global_param
------------------------------------------------------------------------------
Purpose :   This API can be used to read back the global parameter of QTouch method.
Input   :   touch_qt_grp: Choose QTouch Group A or Group B.
            p_global_param: The pointer to readback the QTouch Group A or QTouch
            Group B global sensor configuration.
Output  :   n/a
Notes   :
============================================================================*/
touch_ret_t touch_qt_get_global_param( touch_qt_grp_t touch_qt_grp,
                                 touch_global_param_t *p_global_param );

/*============================================================================
Name    :   touch_qt_sensors_calibrate
------------------------------------------------------------------------------
Purpose :   This API is used to calibrate the sensors for the first time before
            starting a Touch measurement.  This API can also be used to force
            calibration of sensors when any of the Touch sensor parameters are
            changed during runtime.
Input   :   touch_qt_grp: Choose QTouch Group A or Group B.
Output  :   n/a
Notes   :
============================================================================*/
touch_ret_t touch_qt_sensors_calibrate(  touch_qt_grp_t touch_qt_grp );

/*============================================================================
Name    :   touch_qt_sensors_start_acquisition
------------------------------------------------------------------------------
Purpose :   This API can be used to start a QTouch Group A or QTouch Group B
            Touch measurement.
Input   :   touch_qt_grp: Choose QTouch Group A or Group B.
            current_time_ms: Current time in millisecond.
            qt_dma_ch: The dma channel to be used for Touch measurement.
            qt_acq_mode: Specify whether Raw acquisition mode or Normal
            acquisition mode is be used.
Output  :   measure_complete_callback: This callback function is called by
            the Touch Library once the QMatrix Touch measurement is complete.
            This callback function provides pointers related to the measured
            touch data and touch status.
            p_measure_data: QTouch Group A or Group B sensor status and measured
            data pointer.
Notes   :
============================================================================*/
touch_ret_t touch_qt_sensors_start_acquisition( touch_qt_grp_t touch_qt_grp,
                                                  touch_time_t current_time_ms,
                                                touch_qt_dma_t qt_dma_ch,
                                              touch_acq_mode_t qt_acq_mode,
     void (*measure_complete_callback)( touch_measure_data_t *p_measure_data ));

/*============================================================================
Name    :   touch_qt_get_libinfo
------------------------------------------------------------------------------
Purpose :   This API can be used to get the Touch Library configuration
            information for QTouch Group A or Group B acquisition method.
Input   :   touch_qt_grp: Choose QTouch Group A or Group B.
Output  :   p_touch_info: Pointer to the Touch info data structure that will be
            updated by the Touch Library.
Notes   :
============================================================================*/
touch_ret_t touch_qt_get_libinfo( touch_qt_grp_t touch_qt_grp,
                                    touch_info_t *p_touch_info);

/*============================================================================
Name    :   touch_qt_sensor_get_delta
------------------------------------------------------------------------------
Purpose :   This API can be used retrieve the delta value corresponding to
            a given sensor.
Input   :   touch_qt_grp: Choose QTouch Group A or Group B.
            sensor_id: The sensor id for which delta value is being seeked.
Output  :   p_delta: Pointer to the delta variable to be updated by the Touch
            Library.
Notes   :
============================================================================*/
touch_ret_t touch_qt_sensor_get_delta(  touch_qt_grp_t touch_qt_grp,
                                           sensor_id_t sensor_id,
                                         touch_delta_t *p_delta);


/*============================================================================
Name    :   touch_qt_sensor_disable
------------------------------------------------------------------------------
Purpose :   This API can be used disable acquisition on a given Touch Sensor.
Input   :   touch_qt_grp: Choose QTouch Group A or Group B.
            sensor_id: The Sensor number for which acquisition is disabled.
Output  :   n/a
Notes   :   n/a
============================================================================*/
touch_ret_t touch_qt_sensor_disable( touch_qt_grp_t touch_qt_grp,
                                        sensor_id_t sensor_id );

/*============================================================================
Name    :   touch_qt_sensor_reenable
------------------------------------------------------------------------------
Purpose :   This API can be used re-enable acquisition on a disabled Touch Sensor.
Input   :   touch_qt_grp: Choose QTouch Group A or Group B.
            sensor_id: The Sensor number for which acquisition is disabled.
Output  :   n/a
Notes   :   The touch_qt_sensor_config() API enables acquisition on a given
            Sensor by default.  This API is only used to re-enable acquisition
            on a sensor that is disabled using the touch_qt_disable_sensor() API.
============================================================================*/
touch_ret_t touch_qt_sensor_reenable( touch_qt_grp_t touch_qt_grp,
                                         sensor_id_t sensor_id );


/*----------------------------------------------------------------------------
                                Common API
----------------------------------------------------------------------------*/


/*============================================================================
Name    :   touch_event_dispatcher
------------------------------------------------------------------------------
Purpose :   Touch Library Event Dispatcher.  Common to QTouch Group A, QTouch
            Group B or QMatrix Acquisition methods.
Input   :   n/a
Output  :   n/a
Notes   :   This API does not apply for Autonomous QTouch.
============================================================================*/
void touch_event_dispatcher( void );

/*============================================================================
Name    :   touch_deinit
------------------------------------------------------------------------------
Purpose :   This API can be used to deinitialize the touch library.  Once this
            API is called, both the QMatrix method and Autonomous Touch method
            acquisition is stopped and the CAT hardware module is disabled.
Input   :   n/a
Output  :   n/a
Notes   :   In an application if the user would like to call touch_qm_sensors_init()
            or the touch_at_sensor_init() API more than once, then the
            touch_deinit() API must be called before calling the touch_qm_sensors_init()
            or the touch_at_sensor_init() the second time.
            Also, in case any of the Touch API return an error, the the user
            must call the touch_deinit() API before calling the touch_qm_sensors_init()
            or the touch_at_sensor_init() API.
============================================================================*/
void touch_deinit( void );


/*----------------------------------------------------------------------------
                           Extern Functions
----------------------------------------------------------------------------*/


/*============================================================================
Name    :   touch_acq_done_irq
------------------------------------------------------------------------------
Purpose :   This Touch Library ISR must be registered to the Interrupt
            Controller module when using the GCC Compiler.
Input   :   n/a
Output  :   n/a
Notes   :
============================================================================*/
#if (defined __AVR32__)
__attribute__((__interrupt__))
extern void touch_acq_done_irq(void);
#endif

/*============================================================================
Name    :   touch_qm_rs_table_init
------------------------------------------------------------------------------
Purpose :   This is an extern function of the Touch Library.
            This function is NOT to be used by the user.
Input   :   n/a
Output  :   n/a
Notes   :
============================================================================*/
extern void touch_qm_rs_table_init(void);

/*============================================================================
Name    :   touch_qt_rs_table_init
------------------------------------------------------------------------------
Purpose :   This is an extern function of the Touch Library.
            This function is NOT to be used by the user.
Input   :   n/a
Output  :   n/a
Notes   :
============================================================================*/
extern void touch_qt_rs_table_init(void);

/*============================================================================
Name    :   touch_qta_table_init
------------------------------------------------------------------------------
Purpose :   This is an extern function of the Touch Library.
            This function is NOT to be used by the user.
Input   :   n/a
Output  :   n/a
Notes   :
============================================================================*/
extern void touch_qta_table_init(void);

/*============================================================================
Name    :   touch_qtb_table_init
------------------------------------------------------------------------------
Purpose :   This is an extern function of the Touch Library.
            This function is NOT to be used by the user.
Input   :   n/a
Output  :   n/a
Notes   :
============================================================================*/
extern void touch_qtb_table_init(void);
#endif /* TOUCH_API_AT32UC3L_H */
