/**
 * \file QDebug.h
 *
 * \brief API of the QDebug module
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

/*============================ INCLUDES ======================================*/
#include "touch_api_at32uc3l.h"

/*======================== MANIFEST CONSTANTS ================================*/
// Subscription definitions
#define SUBS_SIGN_ON     	0
#define SUBS_GLOBAL_CONFIG	1
#define SUBS_SENSOR_CONFIG	2
#define SUBS_SIGNALS     	3
#define SUBS_REF         	4
#define SUBS_DELTA       	5
#define SUBS_STATES      	6

// PC commands
#define QT_CMD_DUMMY                    0x10
#define QT_CMD_SET_SUBS                 0x11
#define QT_CMD_SET_GLOBAL_CONFIG        0x12
#define QT_CMD_SET_CH_CONFIG            0x13
#define QT_CMD_SET_MEASUREMENT_PERIOD   0x14
#define QT_CMD_SET_QM_BURST_LENGTHS     0x15

// Touch MCU data packets
#define	QT_DUMMY			0x20
#define QT_SIGN_ON			0x21
#define QT_GLOBAL_CONFIG		0x22
#define QT_SENSOR_CONFIG		0x23
#define	QT_SIGNALS			0x24
#define QT_REFERENCES			0x25
#define	QT_DELTAS			0x26
#define QT_STATES			0x27

/*============================ MACROS ========================================*/
#if DEF_TOUCH_QMATRIX == 1
  #define QDEBUG_SENSOR_STATES_PTR         p_qm_measure_data->p_sensor_states
  #define QDEBUG_SENSOR_PTR      	   p_qm_measure_data->p_sensors
  #define QDEBUG_SIGNALS_PTR               p_qm_measure_data->p_channel_signals
  #define QDEBUG_REFERENCES_PTR            p_qm_measure_data->p_channel_references
  #define QDEBUG_SENSOR_RS_VALUES          p_qm_measure_data->p_rotor_slider_values
  #define QDEBUG_BURST_LEN_PTR             (&qm_burst_length[0])
  #define QDEBUG_LIBINFO                   qm_libinfo
  #define QDEBUG_NUM_SENSOR_STATE_BYTES    ((QM_NUM_CHANNELS + 7u) / 8u )
  #define QDEBUG_GET_DELTA_FUNC            touch_qm_sensor_get_delta
  #define QDEBUG_GET_LIBINFO_FUNC          touch_qm_get_libinfo
  #define QDEBUG_GET_GLOBAL_PARAM_FUNC     touch_qm_get_global_param
  #define QDEBUG_UPDATE_GLOBAL_PARAM_FUNC  touch_qm_update_global_param
  #define QDEBUG_GET_SENSOR_CONFIG_FUNC    touch_qm_sensor_get_config
  #define QDEBUG_UPDATE_SENSOR_CONFIG_FUNC touch_qm_sensor_update_config
  #define QDEBUG_NUM_SENSORS               QM_NUM_SENSORS
  #define QDEBUG_NUM_CHANNELS              QM_NUM_CHANNELS
  #define QDEBUG_NUM_ROTORS_SLIDERS        QM_NUM_ROTORS_SLIDERS
#endif

#if DEF_TOUCH_AUTONOMOUS_QTOUCH == 1
  #define QDEBUG_SIGNALS_PTR               ((volatile uint16_t *)(0xFFFF6872u))
  #define QDEBUG_REFERENCES_PTR            ((volatile uint16_t *)(0xFFFF686Eu))
  #define QDEBUG_LIBINFO                   at_libinfo
  #define QDEBUG_NUM_SENSOR_STATE_BYTES    (1u)
  #define QDEBUG_GET_LIBINFO_FUNC          touch_at_get_libinfo
  #define QDEBUG_GET_GLOBAL_PARAM_FUNC     touch_at_sensor_get_config
  #define QDEBUG_UPDATE_GLOBAL_PARAM_FUNC  touch_at_sensor_update_config
  #define QDEBUG_GET_SENSOR_CONFIG_FUNC    touch_at_sensor_get_config
  #define QDEBUG_UPDATE_SENSOR_CONFIG_FUNC touch_at_sensor_update_config
  #define QDEBUG_NUM_SENSORS               (1u)
  #define QDEBUG_NUM_CHANNELS              (1u)
  #define QDEBUG_NUM_ROTORS_SLIDERS        (0u)
#endif

#if DEF_TOUCH_QTOUCH_GRP_A == 1
  #define QDEBUG_SENSOR_STATES_PTR               p_qta_measure_data->p_sensor_states
  #define QDEBUG_SENSOR_PTR      	         p_qta_measure_data->p_sensors
  #define QDEBUG_SIGNALS_PTR                     p_qta_measure_data->p_channel_signals
  #define QDEBUG_REFERENCES_PTR                  p_qta_measure_data->p_channel_references
  #define QDEBUG_SENSOR_RS_VALUES                p_qta_measure_data->p_rotor_slider_values
  #define QDEBUG_LIBINFO                         qta_libinfo
  #define QDEBUG_NUM_SENSOR_STATE_BYTES          ((QTA_NUM_CHANNELS + 7u) / 8u )
  #define QDEBUG_GET_DELTA_FUNC(x,y)             touch_qt_sensor_get_delta(TOUCH_QT_GRP_A,x,y)
  #define QDEBUG_GET_LIBINFO_FUNC(x)             touch_qt_get_libinfo(TOUCH_QT_GRP_A,x)
  #define QDEBUG_GET_GLOBAL_PARAM_FUNC(x)        touch_qt_get_global_param(TOUCH_QT_GRP_A,x)
  #define QDEBUG_UPDATE_GLOBAL_PARAM_FUNC(x)     touch_qt_update_global_param(TOUCH_QT_GRP_A,x)
  #define QDEBUG_GET_SENSOR_CONFIG_FUNC(x,y)     touch_qt_sensor_get_config(TOUCH_QT_GRP_A,x,y)
  #define QDEBUG_UPDATE_SENSOR_CONFIG_FUNC(x,y)  touch_qt_sensor_update_config(TOUCH_QT_GRP_A,x,y)
  #define QDEBUG_NUM_SENSORS                     QTA_NUM_SENSORS
  #define QDEBUG_NUM_CHANNELS                    QTA_NUM_CHANNELS
  #define QDEBUG_NUM_ROTORS_SLIDERS              QTA_NUM_ROTORS_SLIDERS
#endif

#if DEF_TOUCH_QTOUCH_GRP_B == 1
  #define QDEBUG_SENSOR_STATES_PTR               p_qtb_measure_data->p_sensor_states
  #define QDEBUG_SENSOR_PTR      	         p_qtb_measure_data->p_sensors
  #define QDEBUG_SIGNALS_PTR                     p_qtb_measure_data->p_channel_signals
  #define QDEBUG_REFERENCES_PTR                  p_qtb_measure_data->p_channel_references
  #define QDEBUG_SENSOR_RS_VALUES                p_qtb_measure_data->p_rotor_slider_values
  #define QDEBUG_LIBINFO                         qtb_libinfo
  #define QDEBUG_NUM_SENSOR_STATE_BYTES          ((QTB_NUM_CHANNELS + 7u) / 8u )
  #define QDEBUG_GET_DELTA_FUNC(x,y)             touch_qt_sensor_get_delta(TOUCH_QT_GRP_B,x,y)
  #define QDEBUG_GET_LIBINFO_FUNC(x)             touch_qt_get_libinfo(TOUCH_QT_GRP_B,x)
  #define QDEBUG_GET_GLOBAL_PARAM_FUNC(x)        touch_qt_get_global_param(TOUCH_QT_GRP_B,x)
  #define QDEBUG_UPDATE_GLOBAL_PARAM_FUNC(x)     touch_qt_update_global_param(TOUCH_QT_GRP_B,x)
  #define QDEBUG_GET_SENSOR_CONFIG_FUNC(x,y)     touch_qt_sensor_get_config(TOUCH_QT_GRP_B,x,y)
  #define QDEBUG_UPDATE_SENSOR_CONFIG_FUNC(x,y)  touch_qt_sensor_update_config(TOUCH_QT_GRP_B,x,y)
  #define QDEBUG_NUM_SENSORS                     QTB_NUM_SENSORS
  #define QDEBUG_NUM_CHANNELS                    QTB_NUM_CHANNELS
  #define QDEBUG_NUM_ROTORS_SLIDERS              QTB_NUM_ROTORS_SLIDERS
#endif

/*======================== EXTERN VARIABLES ==================================*/
/* QMatrix measured data pointer. */
extern touch_measure_data_t *p_qm_measure_data;
extern touch_measure_data_t *p_qta_measure_data;
extern touch_measure_data_t *p_qtb_measure_data;

#if (DEF_TOUCH_QMATRIX == 1)
  extern const uint8_t qm_burst_length[QM_NUM_CHANNELS];
#endif

/*============================ PROTOTYPES ====================================*/
// Public functions
void QDebug_Init(void);
void QDebug_ProcessCommands(void);
void QDebug_SendData(uint16_t  qt_lib_flags);
void QDebug_SetSubscriptions(uint16_t once, uint16_t change, uint16_t allways);

// Private functions
void Set_Global_Config(void);
void Set_Channel_Config(void);
void Set_Subscriptions(void);
void Set_Measurement_Period(void);
void Set_QM_Burst_Lengths(void);

void Transmit_Dummy(void);
void Transmit_Sign_On(void);
void Transmit_Global_Config(void);
void Transmit_Sensor_Config(void);
void Transmit_Signals(void);
void Transmit_Ref(void);
void Transmit_Delta(void);
void Transmit_State(void);

#endif
/* EOF */
