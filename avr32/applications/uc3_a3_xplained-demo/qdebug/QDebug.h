/*******************************************************************************
*   $FILE:  QDebug.h
*   Atmel Corporation:  http://www.atmel.com \n
*   Support: http://www.atmel.com/design-support/
******************************************************************************/

/**
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
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
