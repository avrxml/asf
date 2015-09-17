/*******************************************************************************
*   $FILE:  QDebug.c
*   Atmel Corporation:  http://www.atmel.com \n
*   Support: http://www.atmel.com/design-support/
******************************************************************************/

/*  License
 * Copyright (c) 2010-2015 Atmel Corporation. All rights reserved.
*
*   Redistribution and use in source and binary forms, with or without
*   modification, are permitted provided that the following conditions are met:
*
*   1. Redistributions of source code must retain the above copyright notice,
*   this list of conditions and the following disclaimer.
*
*   2. Redistributions in binary form must reproduce the above copyright notice,
*   this list of conditions and the following disclaimer in the documentation
*   and/or other materials provided with the distribution.
*
*   3. The name of ATMEL may not be used to endorse or promote products derived
*   from this software without specific prior written permission.
*
*   THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
*   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
*   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
*   SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
*   INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
*   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
*   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
*   THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifdef _DEBUG_INTERFACE_

/** @defgroup QDebug-Remote QDebug Related API's(Remote)
*
*/
/*----------------------------------------------------------------------------
include files
----------------------------------------------------------------------------*/
#include "touch_api.h"
#include "qdebug.h"
#include "qdebug_transport.h"
#include "qdebug_settings.h"
#include "pal.h"
#include "app_config.h"

/*----------------------------------------------------------------------------
global variables
----------------------------------------------------------------------------*/
uint8_t qgRefschanged;
uint8_t qgStateschanged;
uint16_t timestamp1_hword;
uint16_t timestamp1_lword;
uint16_t timestamp2_hword;
uint16_t timestamp2_lword;
uint16_t timestamp3_hword;
uint16_t timestamp3_lword;
qt_lib_siginfo_t lib_siginfo;

/*----------------------------------------------------------------------------
extern variables
----------------------------------------------------------------------------*/
extern uint8_t RX_Buffer[];
extern uint8_t num_sensors;
extern sensor_t sensors[];

/*----------------------------------------------------------------------------
static variables
----------------------------------------------------------------------------*/
uint16_t  delivery = 0;
uint16_t  qgSubsOnce = 0;
uint16_t  qgSubsChange = 0;
uint16_t  qgSubsAllways = 0;
static uint16_t  qgLibraryChanges = 0;

#ifdef _ENABLE_SLEEP_
uint8_t allow_to_sleep = 0;
#endif /* _ENABLE_SLEEP_ */

/**
 * @brief   Initialize Debug Interface
 * @ingroup  QDebug-Remote
 * Must be called before the main loop
 */
void QDebug_Init(void)
{
      qt_get_library_sig(&lib_siginfo);
      Init_Buffers();     
}

/**
 * @brief Command handler for the data received from QTouch Studio
 * @ingroup  QDebug-Remote
 * This function should be called in the main loop after measure_sensors
 * to process the data received from QTOuch Studio\
 */
void QDebug_ProcessCommands(void)
{
   uint8_t CommandID;

   /* Fill in the address for user data in memory */
   uint8_t *pdata = 0;

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
   RX_Buffer[0]=0;
   RX_index=0;	
}

/**
 * @brief Send data to QTouch Studio based on the subscription
 * @ingroup  QDebug-Remote
 *  This function should be called in the main loop after
 *  measure_sensors to send the measured touch data
 *
 * @param qt_lib_flags Change flag from measure_sensors
 */
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

   qgSubsOnce = 0;
   /* Reset the changes we have just sent */
   qgLibraryChanges &= ~delivery;	
}

/**
 * @brief Set subscription values.
 * @ingroup  QDebug-Remote
 * This function can be used directly in main to set data subscription
 * if 1way SPI interface is used
 * 
 * @param once 
 * @param change
 * @param always 
 */
void QDebug_SetSubscriptions(uint16_t once, uint16_t change, uint16_t allways)
{
   qgSubsOnce = once;
   qgSubsChange = change;
   qgSubsAllways = allways;

#ifdef _ENABLE_SLEEP_
   if(qgSubsAllways != 0x00)
   {   
   	   allow_to_sleep = 1;
   }
   else
   {
   	   allow_to_sleep = 0;
   }
#endif /* _ENABLE_SLEEP_ */  

   pal_ps_set(EE_QDEBUG_OFFSET, 2, &qgSubsOnce);
   pal_ps_set(EE_QDEBUG_OFFSET + 2, 2, &qgSubsChange);
   pal_ps_set(EE_QDEBUG_OFFSET + 4, 2, &qgSubsAllways);

#ifdef _ENABLE_SLEEP_   
   pal_ps_set(EE_QDEBUG_OFFSET + 6, 2, &allow_to_sleep);
#endif /* _ENABLE_SLEEP_ */
   
}

/**
 * @brief Set Data Subscription values
 * @ingroup  QDebug-Remote
 * Should only be called from the command handler
 */
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

/**
 * @brief Extract the data packet from QTouch Studio and set global config
 * @ingroup  QDebug-Remote
 * Should only be called from the command handler
 */
void Set_Global_Config(void)
{
   qt_config_data.qt_recal_threshold   = (recal_threshold_t)GetChar();
   qt_config_data.qt_di                = GetChar();
   qt_config_data.qt_drift_hold_time   = GetChar();
   qt_config_data.qt_max_on_duration   = GetChar();
   qt_config_data.qt_neg_drift_rate    = GetChar();
   qt_config_data.qt_pos_drift_rate    = GetChar();
   qt_config_data.qt_pos_recal_delay   = GetChar();
   qt_measurement_period_msec          = (GetChar() << 8u);
   qt_measurement_period_msec          |= GetChar();
}

/**
 * @brief Extract the data packet from QTouch Studio and set channel config
 * @ingroup  QDebug-Remote
 * Should only be called from the command handler
 */

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

#ifdef _QMATRIX_
/**
 * @brief Set QMatrix burst lengths 
 * @ingroup  QDebug-Remote
 * Should only be called from the command handler
 */
void Set_QM_Burst_Lengths(void)
{
   uint8_t c = 0;
   while( c < QT_NUM_CHANNELS )
   {
      qt_burst_lengths[c++] = GetChar();
   }
}
#endif

/**
 * @brief Extracts user data from QTouch Studio to touch mcu memory
 * @ingroup  QDebug-Remote
 * The data can be binary data
*/
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

/**
 * @brief Transmits the sign on packet to QTouch Studio
 * @ingroup  QDebug-Remote
 */
void Transmit_Sign_On(void)
{
   PutChar(QT_SIGN_ON);
   PutInt(PROJECT_ID);
   PutChar(0x01);                       // Simulated as TWI interface
   PutChar(1);                         //PROTOCOL_TYPE
   PutChar(2);                         //PROTOCOL_VERSION
   PutChar(lib_siginfo.lib_sig_lword);	//LIB_TYPE
   PutInt(lib_siginfo.library_version);//LIB_VERSION
   PutInt(0);                          //LIB_VARIANT
   PutChar(QT_NUM_CHANNELS);
   //PutInt(delivery);    // subscription info
   PutInt(7);
   Send_Message();
}

/**
 * @brief Transmits the global config struct to QTouch Studio
 * @ingroup  QDebug-Remote
 */
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
   PutInt(qt_measurement_period_msec);
   PutInt(TICKS_PER_MS);
   PutChar(0); // Time_Setting
   Send_Message();
}

/**
 * @brief Transmits the channel config struct to QTouch Studio
 * @ingroup  QDebug-Remote
 */
void Transmit_Sensor_Config(void)
{
    uint8_t c;
    PutChar(QT_SENSOR_CONFIG);
    
#ifdef _ROTOR_SLIDER_
    PutChar(1);		// 1 = KRS
    
    /* Value of c is used according the maximum channel number to be measured
       to avoid the over the air data transmission.
    */
    for(c = 0; c <= CHANNEL_19; c++) 
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

/* These functions (below) under the compiler switch '_GENERIC_QDEBUG_' can be 
   used by the user to send individual Qdebug data and user data. These functions
   are not called in the reference application. But one can call these if required. 
*/
#ifdef _GENERIC_QDEBUG_

/**
 * @brief Transmits a dummy packet if no other subscriptions are set
 * @ingroup  QDebug-Remote
 */
void Transmit_Dummy(void)
{
   PutChar(QT_DUMMY);
   Send_Message();
}

/**
 * @brief Transmits the measurement values for each channel to QTouch Studio
 * @ingroup  QDebug-Remote
 */
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

/**
 * @brief Transmits the channel reference values to QTouch Studio
 */
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

/**
 * @brief Transmits the channel delta values to QTouch Studio
 * @ingroup  QDebug-Remote
 * The value is equal to signal-reference
 */
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

/**
 *@brief Transmits the state values to QTouch Studio
 * @ingroup  QDebug-Remote
 * On/Off condition for each sensor
 */
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
/**
 * @brief Transmits the QMatrix burst length values to QTouch Studio
 * @ingroup  QDebug-Remote
 * This value is available for each channel
 */
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

/**
 * @brief Transmits the application execution timestamp values to QTouch Studio
 * @ingroup  QDebug-Remote
 * This value is a combination of current_time_ms_touch (high word) &
 * timer counter register (low word)
 */
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

/**
 * @brief Transmits user data to QTouch Studio
 * @ingroup  QDebug-Remote
 * The data will be binary data
 *
 * @param pdata data pointer
 * @param c     length of data in bytes 
 */
void Transmit_QT_User_Data(uint8_t *pdata, uint16_t c)
{
   while( c > 0 )
   {
      PutChar(*pdata++);
      c--;
   }
   Send_Message();
}

#endif /* _GENERIC_QDEBUG_ */

#endif
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-EOF-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
