/*******************************************************************************
*   $FILE:  QDebug.c
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

/*============================ INCLUDES ======================================*/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include "touch_api.h"
#include "QDebug.h"
#include "QDebugTransport.h"
#include "QDebugSettings.h"

#if !(defined(__AVR32__) || defined(__ICCAVR32__) || defined(_TOUCH_ARM_))
#include "eeprom_access.h"
#endif

#if defined(QDEBUG_SPI)
#include "SPI_Master.h"
#elif defined(QDEBUG_TWI)
#include "TWI_Master.h"
#elif defined(QDEBUG_SPI_BB)
#include "BitBangSPI_Master.h"
#else
#warning "No Debug Interface is selected in QDebugSettings.h"
#endif

/*============================ GLOBAL VARIABLES ==============================*/
// These must somehow be updated from the library
uint8_t qgRefschanged;
uint8_t qgStateschanged;

uint16_t timestamp1_hword;
uint16_t timestamp1_lword;
uint16_t timestamp2_hword;
uint16_t timestamp2_lword;
uint16_t timestamp3_hword;
uint16_t timestamp3_lword;
extern uint8_t RX_Buffer[];

extern uint8_t num_sensors;
extern sensor_t sensors[];
extern uint16_t measurement_period_ms;
/* Eeprom info block */
#if !(defined(__AVR32__) || defined(__ICCAVR32__) || defined(_TOUCH_ARM_))
extern uint8_t eeprom_info_block[NUM_INFO_BYTES];
#endif

/*============================ LOCAL VARIABLES ===============================*/
// Subscriptions
static uint16_t  delivery = 0;
static uint16_t  qgSubsOnce = 0;
static uint16_t  qgSubsChange = 0;
static uint16_t  qgSubsAllways = 0;
static uint16_t  qgLibraryChanges = 0;

qt_lib_siginfo_t lib_siginfo;

/*============================ IMPLEMENTATION ================================*/

/*============================================================================
Name    :   QDebug_Init
------------------------------------------------------------------------------
Purpose :   Initialize Debug Interface
Input   :   n/a
Output  :   n/a
Notes   :	Must be called before the main loop
============================================================================*/
void QDebug_Init(void)
{
#if !(defined(__AVR32__) || defined(__ICCAVR32__) || defined(_TOUCH_ARM_))
    uint16_t eeprom_lib_version;
#endif
#if defined(QDEBUG_SPI)
    SPI_Master_Init();
#elif defined(QDEBUG_TWI)
    TWI_Master_Init();
#elif defined(QDEBUG_SPI_BB)
    BitBangSPI_Master_Init();
#endif
    qt_get_library_sig(&lib_siginfo);
    Init_Buffers();
    /* Eeprom settings sanity check */
#if !(defined(__AVR32__) || defined(__ICCAVR32__) || defined(_TOUCH_ARM_))
    read_info_from_eeprom();
    eeprom_lib_version = ((uint16_t)(eeprom_info_block[INFO_DATA_LIB_VERSION]) & 0x00FFu) + ((uint16_t)(eeprom_info_block[INFO_DATA_LIB_VERSION + 1u]) << 8u);
    if(eeprom_lib_version == lib_siginfo.library_version)
    {
        read_settings_from_eeprom();
    }
    else
    {
        eeprom_info_block[INFO_DATA_LIB_VERSION] = (lib_siginfo.library_version & 0x00FFu);
        eeprom_info_block[INFO_DATA_LIB_VERSION + 1u] = (lib_siginfo.library_version >> 8u);
    }
#endif
}

/*============================================================================
Name    :   QDebug_ProcessCommands
------------------------------------------------------------------------------
Purpose :   Command handler for the data received from QTouch Studio
Input   :   n/a
Output  :   n/a
Notes   :   This function should be called in the main loop after
measure_sensors to process the data received from QTouch Studio
============================================================================*/
void QDebug_ProcessCommands(void)
{
    uint8_t CommandID;

    /* Fill in the address for user data in memory */
    uint8_t *pdata = 0;
    uint8_t time_setting = 0;// time setting for future use

    if (Receive_Message() == 0)
    return;
    // Handle the commands
    CommandID = GetChar();
    switch (CommandID)
    {
    case QT_CMD_DUMMY:
        break;
    case QT_CMD_SET_SUBS:
        Set_Subscriptions();
        break;
    case QT_CMD_SET_GLOBAL_CONFIG:
        Set_Global_Config();
        Set_Measurement_Period();
        time_setting = GetChar(); // time setting for future use
        break;
    case QT_CMD_SET_CH_CONFIG:
        Set_Channel_Config();
        break;
    #ifdef _QMATRIX_
    case QT_CMD_SET_QM_BURST_LENGTHS:
        Set_QM_Burst_Lengths();
        break;
    #endif
    case QT_CMD_SET_USER_DATA:
        Set_QT_User_Data(pdata);
        break;
    }

    (void)time_setting;

    RX_Buffer[0]=0;
    RX_index=0;
}

/*============================================================================
Name    :   QDebug_SendData
------------------------------------------------------------------------------
Purpose :   Send data to QTouch Studio based on the subscription
Input   :   Change flag from measure_sensors
Output  :   n/a
Notes   :	This function should be called in the main loop after
measure_sensors to send the measured touch data
============================================================================*/
void QDebug_SendData(uint16_t qt_lib_flags)
{
    SequenceH = (SequenceH+1)&0x0F;
    /* Test if measure_sensors has reported change in key states or rotor/slider positions */
    if((qt_lib_flags & QTLIB_STATUS_CHANGE) ||
        (qt_lib_flags & QTLIB_ROTOR_SLIDER_POS_CHANGE) )
    {
        qgLibraryChanges |= (1<<SUBS_STATES);
    }
    /* Test if measure_sensors has reported change in at least one channel reference */
    if(qt_lib_flags & QTLIB_CHANNEL_REF_CHANGE)
    {
        qgLibraryChanges |= (1<<SUBS_REF);
    }
    delivery = qgSubsAllways | qgSubsOnce | (qgLibraryChanges & qgSubsChange);

    if (delivery & (1<<SUBS_SIGN_ON))
        Transmit_Sign_On();
    if (delivery & (1<<SUBS_GLOBAL_CONFIG))
        Transmit_Global_Config();
    if (delivery & (1<<SUBS_SENSOR_CONFIG))
        Transmit_Sensor_Config();
    if (delivery & (1<<SUBS_SIGNALS))
        Transmit_Signals();
    if (delivery & (1<<SUBS_REF))
        Transmit_Ref();
    if (delivery & (1<<SUBS_STATES))
        Transmit_State();
    if (delivery & (1<<SUBS_DELTA))
        Transmit_Delta();
    if (delivery == 0)
        Transmit_Dummy();
    // Reset masks until PC sends another request
    qgSubsOnce = 0;
    // Reset the changes we have just sent
    qgLibraryChanges &= ~delivery;
}

/*============================================================================
Name    :   QDebug_SetSubscriptions
------------------------------------------------------------------------------
Purpose :   Set subscription values.
Input   :   Values
Output  :   n/a
Notes   :	This function can be used directly in main to set data subscription
if 1way SPI interface is used
============================================================================*/
void QDebug_SetSubscriptions(uint16_t once, uint16_t change, uint16_t allways)
{
    qgSubsOnce = once;
    qgSubsChange = change;
    qgSubsAllways = allways;
}

/*============================================================================
Name    :   SetSubscriptions
------------------------------------------------------------------------------
Purpose :   Set Data Subscription values
Input   :   n/a
Output  :   n/a
Notes   :   Should only be called from the command handler
============================================================================*/
void Set_Subscriptions(void)
{
    GetChar();			// Bit 8-15 is reserved for future use
    uint8_t a = GetChar();
    GetChar();			// Bit 8-15 is reserved for future use
    uint8_t b = GetChar();
    GetChar();			// Bit 8-15 is reserved for future use
    uint8_t c = GetChar();
    QDebug_SetSubscriptions(a, b, c);
}

/*============================================================================
Name    :   Set_Global_Config
------------------------------------------------------------------------------
Purpose :   Extract the data packet from QTouch Studio and set global config
Input   :   n/a
Output  :   n/a
Notes   :	Should only be called from the command handler
============================================================================*/
void Set_Global_Config(void)
{
    qt_config_data.qt_recal_threshold = (recal_threshold_t)GetChar();
    qt_config_data.qt_di = GetChar();
    qt_config_data.qt_drift_hold_time = GetChar();
    qt_config_data.qt_max_on_duration = GetChar();
    qt_config_data.qt_neg_drift_rate = GetChar();
    qt_config_data.qt_pos_drift_rate = GetChar();
    qt_config_data.qt_pos_recal_delay = GetChar();

#if !(defined(__AVR32__) || defined(__ICCAVR32__) || defined(_TOUCH_ARM_))
    write_info_to_eeprom();
    write_settings_to_eeprom();
#endif
}

/*============================================================================
Name    :   Set_Channel_Config
------------------------------------------------------------------------------
Purpose :   Extract the data packet from QTouch Studio and set channel config
Input   :   n/a
Output  :   n/a
Notes   :	Should only be called from the command handler
============================================================================*/
void Set_Channel_Config(void)
{
   uint8_t sensor_number;
   uint8_t sensor_aks_hyst;

   sensor_number = GetChar();
   sensors[sensor_number].threshold = GetChar();
   sensor_aks_hyst = GetChar();

   /* Set only the AKS and Hysteresis fields (00111011) */
   sensors[sensor_number].type_aks_pos_hyst = \
      (sensors[sensor_number].type_aks_pos_hyst & (0xC4u)) | (sensor_aks_hyst & ~(0xC4u));
}

/*============================================================================
Name    :   Set_Measurement_Period
------------------------------------------------------------------------------
Purpose :   Extract the data packet from QTouch Studio and set measurement period
Input   :   n/a
Output  :   n/a
Notes   :	Should only be called from the command handler
============================================================================*/

void Set_Measurement_Period(void)
{
   measurement_period_ms = (GetChar() << 8u);
   measurement_period_ms |= GetChar();
}

#ifdef _QMATRIX_
/*============================================================================
Name    :   Set_QM_Burst_Lengths
------------------------------------------------------------------------------
Purpose :   Extract the data packet from QTouch Studio and set QMatrix burst lengths
Input   :   n/a
Output  :   n/a
Notes   :	Should only be called from the command handler
============================================================================*/

void Set_QM_Burst_Lengths(void)
{
    uint8_t c = 0;
    while( c < QT_NUM_CHANNELS )
    {
        qt_burst_lengths[c++] = GetChar();
    }
}
#endif

/*============================================================================
Name    :   Set_QT_User_Data
------------------------------------------------------------------------------
Purpose :   Extracts user data from QTouch Studio to touch mcu memory
Input   :   data pointer
Output  :   n/a
Notes   :	The data can be binary data
============================================================================*/

void Set_QT_User_Data(uint8_t *pdata)
{
    uint16_t c = RX_Buffer[1];
    while( c > 0 )
    {
        PutChar(*pdata++);
        c--;
    }
    Send_Message();
}

/*============================================================================
Name    :   Transmit_Dummy
------------------------------------------------------------------------------
Purpose :   Transmits a dummy packet if no other subscriptions are set
Input   :   n/a
Output  :   n/a
Notes   :
============================================================================*/
void Transmit_Dummy(void)
{
   PutChar(QT_DUMMY);
   Send_Message();
}

/*============================================================================
Name    :   Transmit_Sign_On
------------------------------------------------------------------------------
Purpose :   Transmits the sign on packet to QTouch Studio
Input   :   n/a
Output  :   n/a
Notes   :
============================================================================*/
void Transmit_Sign_On(void)
{
    PutChar(QT_SIGN_ON);
    PutInt(PROJECT_ID);
    PutChar(INTERFACE);
    PutChar(1);                         //PROTOCOL_TYPE
    PutChar(2);                         //PROTOCOL_VERSION
    PutChar(lib_siginfo.lib_sig_lword);	//LIB_TYPE
    PutInt(lib_siginfo.library_version);//LIB_VERSION
    PutInt(0);                          //LIB_VARIANT
    PutChar(QT_NUM_CHANNELS);
    PutInt(delivery);    // subscription info
    Send_Message();
}

/*============================================================================
Name    :   Transmit_Global_Config
------------------------------------------------------------------------------
Purpose :   Transmits the global config struct to QTouch Studio
Input   :   n/a
Output  :   n/a
Notes   :
============================================================================*/
void Transmit_Global_Config(void)
{
    PutChar(QT_GLOBAL_CONFIG);
    PutChar(qt_config_data.qt_recal_threshold);
    PutChar(qt_config_data.qt_di);
    PutChar(qt_config_data.qt_drift_hold_time);
    PutChar(qt_config_data.qt_max_on_duration);
    PutChar(qt_config_data.qt_neg_drift_rate);
    PutChar(qt_config_data.qt_pos_drift_rate);
    PutChar(qt_config_data.qt_pos_recal_delay);
    PutInt(measurement_period_ms);
    PutInt(TICKS_PER_MS);
    PutChar(0); // Time_Setting
    Send_Message();
}

/*============================================================================
Name    :   Transmit_Sensor_Config
------------------------------------------------------------------------------
Purpose :   Transmits the channel config struct to QTouch Studio
Input   :   n/a
Output  :   n/a
Notes   :
============================================================================*/
void Transmit_Sensor_Config(void)
{
   uint8_t c;
   PutChar(QT_SENSOR_CONFIG);
#ifdef _ROTOR_SLIDER_
   PutChar(1);		// 1 = KRS
   for(c=0; c<QT_NUM_CHANNELS; c++)
   {
      PutChar(sensors[c].threshold);
      PutChar(sensors[c].type_aks_pos_hyst);
      PutChar(sensors[c].from_channel);
      PutChar(sensors[c].to_channel);
   }
#else
   PutChar(0);		// 0 = K
   for(c=0; c<QT_NUM_CHANNELS; c++)
   {
      PutChar(sensors[c].threshold);
      PutChar(sensors[c].type_aks_pos_hyst);
      PutChar(sensors[c].from_channel);
   }
#endif
   Send_Message();
}

/*============================================================================
Name    :   Transmit_Signals
------------------------------------------------------------------------------
Purpose :   Transmits the measurement values for each channel to QTouch Studio
Input   :   n/a
Output  :   n/a
Notes   :
============================================================================*/
void Transmit_Signals(void)
{
   uint8_t c;
   PutChar(QT_SIGNALS);
   for(c=0; c<QT_NUM_CHANNELS; c++)
   {
      PutChar(qt_measure_data.channel_signals[c] >> 8);
      PutChar(qt_measure_data.channel_signals[c] & 0xFF);
   }
   Send_Message();
}

/*============================================================================
Name    :   Transmit_Ref
------------------------------------------------------------------------------
Purpose :   Transmits the channel reference values to QTouch Studio
Input   :   n/a
Output  :   n/a
Notes   :
============================================================================*/
void Transmit_Ref(void)
{
   uint8_t c;
   PutChar(QT_REFERENCES);
   for(c=0; c<QT_NUM_CHANNELS; c++)
   {
      PutChar(qt_measure_data.channel_references[c] >> 8);
      PutChar(qt_measure_data.channel_references[c] & 0xFF);
   }
   Send_Message();
}

/*============================================================================
Name    :   Transmit_Delta
------------------------------------------------------------------------------
Purpose :   Transmits the channel delta values to QTouch Studio
Input   :   n/a
Output  :   n/a
Notes   :	The value is equal to signal-reference
============================================================================*/
void Transmit_Delta(void)
{
   uint8_t c;
   uint16_t delta;
   PutChar(QT_DELTAS);
   for (c=0; c < num_sensors; c++)
   {
      delta=qt_get_sensor_delta(c);
      PutChar(delta >> 8);
      PutChar(delta & 0xFF);
   }
   Send_Message();
}

/*============================================================================
Name    :   Transmit_State
------------------------------------------------------------------------------
Purpose :   Transmits the state values to QTouch Studio
Input   :   n/a
Output  :   n/a
Notes   :	On/Off condition for each sensor
============================================================================*/
void Transmit_State(void)
{
   uint8_t c;
   PutChar(QT_STATES);
   PutChar(QT_NUM_CHANNELS);
   PutChar(QT_MAX_NUM_ROTORS_SLIDERS);
   for (c=0; c<QT_NUM_SENSOR_STATE_BYTES; c++)
   {
      PutChar(qt_measure_data.qt_touch_status.sensor_states[c]);
   }
   for (c=0; c<QT_MAX_NUM_ROTORS_SLIDERS; c++)
   {
      // 3.2 change - rotor/slider values are now 8-bit
      PutChar(qt_measure_data.qt_touch_status.rotor_slider_values[c]);
   }
   Send_Message();
}

#ifdef _QMATRIX_
/*============================================================================
Name    :   Transmit_Burst_Lengths
------------------------------------------------------------------------------
Purpose :   Transmits the QMatrix burst length values to QTouch Studio
Input   :   n/a
Output  :   n/a
Notes   :	This value is available for each channel
============================================================================*/
void Transmit_Burst_Lengths(void)
{
    uint8_t c = 0;
    while( c < QT_NUM_CHANNELS )
    {
        PutChar(qt_burst_lengths[c++]);
    }
    Send_Message();
}
#endif

/*============================================================================
Name    :   Transmit_Time_Stamps
------------------------------------------------------------------------------
Purpose :   Transmits the application execution timestamp values to QTouch Studio
Input   :   n/a
Output  :   n/a
Notes   :	This value is a combination of current_time_ms_touch (high word) &
            timer counter register (low word)
============================================================================*/
void Transmit_Time_Stamps(void)
{

    PutInt(timestamp1_hword);
    PutInt(timestamp1_lword);
    PutInt(timestamp2_hword);
    PutInt(timestamp2_lword);
    PutInt(timestamp3_hword);
    PutInt(timestamp3_lword);
    Send_Message();
}

/*============================================================================
Name    :   Transmit_QT_User_Data
------------------------------------------------------------------------------
Purpose :   Transmits user data to QTouch Studio
Input   :   data pointer, length of data in bytes
Output  :   n/a
Notes   :	The data will be binary data
============================================================================*/

void Transmit_QT_User_Data(uint8_t *pdata, uint16_t c)
{
    while( c > 0 )
    {
        PutChar(*pdata++);
        c--;
    }
    Send_Message();
}
