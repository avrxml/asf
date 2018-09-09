/*******************************************************************************
*   $FILE:  QDebug.h
*   Microchip Technology Inc:  http://www.microchip.com \n
*   Support: https://www.microchip.com/support/
******************************************************************************/

/**
 *
 * Copyright (c) 2010-2018 Microchip Technology Inc. and its subsidiaries.
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
#include "touch_api.h"

/*============================ GLOBAL VARIABLES ==============================*/
// Touch API
extern qt_touch_lib_config_data_t qt_config_data;
extern qt_touch_lib_measure_data_t qt_measure_data;
extern int16_t qt_get_sensor_delta( uint8_t );

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
#ifdef _QMATRIX_
void Set_QM_Burst_Lengths(void);
#endif
void Set_QT_User_Data(uint8_t *);

void Transmit_Dummy(void);
void Transmit_Sign_On(void);
void Transmit_Global_Config(void);
void Transmit_Sensor_Config(void);
void Transmit_Signals(void);
void Transmit_Ref(void);
void Transmit_Delta(void);
void Transmit_State(void);
void Transmit_Time_Stamps(void);
void Transmit_QT_User_Data(uint8_t *pdata, uint16_t c);

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
#define QT_CMD_SET_QM_BURST_LENGTHS     0x14
#define QT_CMD_SET_USER_DATA            0x15

// Touch MCU data packets
#define QT_DUMMY	              0x20
#define QT_SIGN_ON	           0x21
#define QT_GLOBAL_CONFIG        0x22
#define QT_SENSOR_CONFIG        0x23
#define QT_SIGNALS	           0x24
#define QT_REFERENCES           0x25
#define QT_DELTAS               0x26
#define QT_STATES               0x27


#endif
/* EOF */
