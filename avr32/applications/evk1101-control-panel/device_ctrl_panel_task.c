/**************************************************************************
 *
 * \file
 *
 * \brief Management of the USB device Full Custom task.
 *
 * This file manages the USB device Full Custom task.
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


//_____  I N C L U D E S ___________________________________________________

#include <string.h>
#include <stdio.h>
#include "conf_usb.h"
#include "compiler.h"


#if USB_DEVICE_FEATURE == true

#include "board.h"
#include "usb_drv.h"
#include "gpio.h"
#include "usb_descriptors.h"
#include "usb_standard_request.h"
#include "device_ctrl_panel_task.h"
#include "adc.h"
#include "cpled.h"
#include "s_accelerometer.h"
#include "s_joystick.h"
#include "s_light.h"
#include "s_pushb.h"
#include "s_temperature.h"
#ifdef FREERTOS_USED
#include "FreeRTOS.h"
#include "task.h"
#endif


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

#define STR_SET_LEDM1      "set_actuator_value actuator=ledm1 state="
#define STR_SET_LEDM2      "set_actuator_value actuator=ledm2 state="
#define STR_SET_LEDM3      "set_actuator_value actuator=ledm3 state="
#define STR_SET_LEDM4      "set_actuator_value actuator=ledm4 state="

//_____ D E C L A R A T I O N S ____________________________________________
static U16  sof_cnt;

#define RXBUF_SIZE  64
#define TXBUF_SIZE  40
char rxbuf[RXBUF_SIZE];
char txbuf[TXBUF_SIZE];

bool b_report_pending=false;





//!
//! @brief This function initializes the hardware/software resources
//! required for the USB Full custom task.
//!
void device_full_custom_task_init(void)
{
  sof_cnt = 0;
#ifndef FREERTOS_USED
  #if USB_HOST_FEATURE == true
  // If both device and host features are enabled, check if device mode is engaged
  // (accessing the USB registers of a non-engaged mode, even with load operations,
  // may corrupt USB FIFO data).
  if (Is_usb_device())
  #endif  // USB_HOST_FEATURE == true
    Usb_enable_sof_interrupt();
#endif  // FREERTOS_USED

  // Initialize Leds state
  LED_Off( LED_MONO0_GREEN );
  LED_Off( LED_MONO1_GREEN );
  LED_Off( LED_MONO2_GREEN );
  LED_Off( LED_MONO3_GREEN );

  // Initialize accelerometer sensor
  accelerometer_init();

#ifdef FREERTOS_USED
  xTaskCreate(device_full_custom_task,
              configTSK_USB_DFC_NAME,
              configTSK_USB_DFC_STACK_SIZE,
              NULL,
              configTSK_USB_DFC_PRIORITY,
              NULL);
#endif  // FREERTOS_USED
}



static U32 build_answer( char* output, const char * log)
{
   return sprintf( output, "%s|00/00/00 00:00:00|", log);
}



//!
//! @brief Entry point of the device full custom task management
//!
#ifdef FREERTOS_USED
void device_full_custom_task(void *pvParameters)
#else
void device_full_custom_task(void)
#endif
{
  U32  time=0;
  bool startup=true;

#ifdef FREERTOS_USED
  portTickType xLastWakeTime;

  xLastWakeTime = xTaskGetTickCount();
  while (true)
  {
    vTaskDelayUntil(&xLastWakeTime, configTSK_USB_DFC_PERIOD);

    if( startup )
    {
       time+=configTSK_USB_DFC_PERIOD;
       #define STARTUP_LED_DELAY  100
       if     ( time== 1*STARTUP_LED_DELAY ) LED_On( LED_MONO0_GREEN );
       else if( time== 2*STARTUP_LED_DELAY ) LED_On( LED_MONO1_GREEN );
       else if( time== 3*STARTUP_LED_DELAY ) LED_On( LED_MONO2_GREEN );
       else if( time== 4*STARTUP_LED_DELAY ) LED_On( LED_MONO3_GREEN );
       else if( time== 5*STARTUP_LED_DELAY ) LED_Off( LED_MONO0_GREEN );
       else if( time== 6*STARTUP_LED_DELAY ) LED_Off( LED_MONO1_GREEN );
       else if( time== 7*STARTUP_LED_DELAY ) LED_Off( LED_MONO2_GREEN );
       else if( time== 8*STARTUP_LED_DELAY ) LED_Off( LED_MONO3_GREEN );
       else if( time== 9*STARTUP_LED_DELAY ) startup=false;
    }

    // First, check the device enumeration state
    if (!Is_device_enumerated()) continue;
#else
    // First, check the device enumeration state
    if (!Is_device_enumerated()) return;
#endif  // FREERTOS_USED

    if(Is_usb_out_received(EP_FC_OUT))
    {
       U32 nchar;
       Usb_reset_endpoint_fifo_access(EP_FC_OUT);
       memset(rxbuf, 0, RXBUF_SIZE);
       usb_read_ep_rxpacket(EP_FC_OUT, &rxbuf, RXBUF_SIZE, NULL);
       Usb_ack_out_received_free(EP_FC_OUT);
       //printf("Received %s\n\r", rxbuf);
       if( !strcmp((const char*)rxbuf, "get_sensor_value sensor=temp") )
       {  // Temperature sensor
          nchar=build_answer(txbuf, "temp");
          b_temperature_get_value( txbuf+nchar );
          b_report_pending=true;
       }

       else if( !strcmp((const char*)rxbuf, "get_sensor_value sensor=js") )
       {  // Joystick
          nchar=build_answer(txbuf, "js");
          b_joystick_get_value( txbuf+nchar );
          b_report_pending=true;
       }

       else if( !strcmp((const char*)rxbuf, "get_sensor_value sensor=pb1") )
       {  // Push button 1
          nchar=build_answer(txbuf, "pb1");
          b_pushb1_get_value( txbuf+nchar );
          b_report_pending=true;
       }

       else if( !strcmp((const char*)rxbuf, "get_sensor_value sensor=pb2") )
       {  // Push button 2
          nchar=build_answer(txbuf, "pb2");
          b_pushb2_get_value( txbuf+nchar );
          b_report_pending=true;
       }

#if BOARD == EVK1100
       else if( !strcmp((const char*)rxbuf, "get_sensor_value sensor=pb3") )
       {  // Push button 3
          nchar=build_answer(txbuf, "pb3");
          sprintf( txbuf+nchar, "RELEASE\r\n");
          b_report_pending=true;
       }
#endif

       else if( !strcmp((const char*)rxbuf, "get_sensor_value sensor=light") )
       {  // light
          U32 value;
          nchar=build_answer(txbuf, "light");
          b_light_get_value( txbuf+nchar, &value );
          e_ledm_refresh_intensity( value );
          b_report_pending=true;
       }


       else if( !strcmp((const char*)rxbuf, "get_actuator_value actuator=ledm1") )
       {  // led1
          nchar=build_answer(txbuf, "ledm1");
          b_ledm1_get_value( txbuf+nchar );
          b_report_pending=true;
       }
       else if( !strcmp((const char*)rxbuf, "get_actuator_value actuator=ledm2") )
       {  // led2
          nchar=build_answer(txbuf, "ledm2");
          b_ledm2_get_value( txbuf+nchar );
          b_report_pending=true;
       }
       else if( !strcmp((const char*)rxbuf, "get_actuator_value actuator=ledm3") )
       {  // led3
          nchar=build_answer(txbuf, "ledm3");
          b_ledm3_get_value( txbuf+nchar );
          b_report_pending=true;
       }
       else if( !strcmp((const char*)rxbuf, "get_actuator_value actuator=ledm4") )
       {  // led4
          nchar=build_answer(txbuf, "ledm4");
          b_ledm4_get_value( txbuf+nchar );
          b_report_pending=true;
       }


       else if( !strncmp((const char*)rxbuf, STR_SET_LEDM1, strlen(STR_SET_LEDM1)) )
       {  // led1
          nchar=build_answer(txbuf, "ledm1");
          e_ledm1_set_value(rxbuf+strlen(STR_SET_LEDM1), txbuf+nchar);
          b_report_pending=true;
       }
       else if( !strncmp((const char*)rxbuf, STR_SET_LEDM2, strlen(STR_SET_LEDM2)) )
       {  // led2
          nchar=build_answer(txbuf, "ledm2");
          e_ledm2_set_value(rxbuf+strlen(STR_SET_LEDM2), txbuf+nchar);
          b_report_pending=true;
       }
       else if( !strncmp((const char*)rxbuf, STR_SET_LEDM3, strlen(STR_SET_LEDM3)) )
       {  // led3
          nchar=build_answer(txbuf, "ledm3");
          e_ledm3_set_value(rxbuf+strlen(STR_SET_LEDM2), txbuf+nchar);
          b_report_pending=true;
       }
       else if( !strncmp((const char*)rxbuf, STR_SET_LEDM4, strlen(STR_SET_LEDM4)) )
       {  // led4
          nchar=build_answer(txbuf, "ledm4");
          e_ledm4_set_value(rxbuf+strlen(STR_SET_LEDM2), txbuf+nchar);
          b_report_pending=true;
       }


       else if( !strcmp((const char*)rxbuf, "get_sensor_value sensor=accx") )
       {  // accelerometer
          nchar=build_answer(txbuf, "accx");
          accelerometer_measure(0, txbuf+nchar);
          b_report_pending=true;
       }
       else if( !strcmp((const char*)rxbuf, "get_sensor_value sensor=accy") )
       {  // accelerometer
          nchar=build_answer(txbuf, "accy");
          accelerometer_measure(1, txbuf+nchar);
          b_report_pending=true;
       }
    }

    if( b_report_pending && Is_usb_in_ready(EP_FC_IN) )
    {
       U8  data_to_transfer;
       char* ptr_cram=txbuf;

       //printf( "Sending %s", txbuf);
       #if 0
       Usb_reset_endpoint_fifo_access(EP_FC_IN);
       usb_write_ep_txpacket(EP_FC_IN, &txbuf, TXBUF_SIZE, NULL);
       Usb_ack_in_ready_send(EP_FC_IN);
       #endif

       data_to_transfer = strlen(txbuf);
       while (data_to_transfer)
       {
         while (!Is_usb_in_ready(EP_FC_IN));

         Usb_reset_endpoint_fifo_access(EP_FC_IN);
         data_to_transfer = usb_write_ep_txpacket(EP_FC_IN, ptr_cram, data_to_transfer, (const void**)&ptr_cram);
         Usb_ack_in_ready_send(EP_FC_IN);
       }
       b_report_pending=false;
    }
#ifdef FREERTOS_USED
  }
#endif
}


//!
//! @brief usb_sof_action
//!
//! This function increments the sof_cnt counter each time
//! the USB Start-of-Frame interrupt subroutine is executed (1 ms).
//! Useful to manage time delays
//!
void usb_sof_action(void)
{
  sof_cnt++;
}


#endif  // USB_DEVICE_FEATURE == true
