/**************************************************************************
 *
 * \file
 *
 * \brief Management of the USB host audio task.
 *
 * This file manages the USB host audio task.
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
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


//_____  I N C L U D E S ___________________________________________________

#include "conf_usb.h"
#include "debug.h"

#if USB_HOST_FEATURE == true

#include "board.h"
#ifdef FREERTOS_USED
#  include "FreeRTOS.h"
#  include "task.h"
#endif

#include "usb_drv.h"
#include "usb_host_enum.h"
#include "usb_host_task.h"
#include "usb_audio.h"
#include "host_audio_task.h"
#include <stdio.h>
#include "usb_stream_player.h"
#include "et024006dhu.h"
#include "cycle_counter.h"
#include "avr32_logo.h"
#include "audio_example.h"

//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________




//_____ D E C L A R A T I O N S ____________________________________________

#define TIMER_STARTUP    100
#define START_Y_DEMO_TEXT 33

#define FONT_HEIGHT   14

#if (defined __GNUC__)
  extern uint8_t *_data;
  extern uint8_t *_edata;
  extern uint8_t *__bss_start;
  extern uint8_t *end;
  extern uint8_t *_stack;
  extern uint8_t *_estack;
  extern uint8_t *__heap_start__;
  extern uint8_t *__heap_end__;
#endif

  t_cpu_time timer;

static uint32_t  mmi_state=0;
host_audio_task_data_t *host_audio_task_data = NULL;

volatile bool audio_new_device_connected = false;
volatile bool audio_connected = false;

volatile uint8_t  audio_cpt_sof;
static   uint16_t sof_cnt;

static void mmi_display( void );
static void mmi_activity_display( bool init, uint32_t fifo_cnt );


uint8_t  pipe_audio_in;
uint8_t  interf_audio_stream;



//!
//! @brief This function displays a box on the LCD.
//!
static void display_box( uint32_t x, uint32_t y, uint32_t size_x, uint32_t size_y, uint16_t color, uint16_t edge_color )
{
  et024006_DrawFilledRect(x, y, size_x, size_y, color);
  et024006_DrawHorizLine(x, y, size_x, edge_color);
  et024006_DrawVertLine(x+size_x-1, y, size_y, edge_color);
  et024006_DrawHorizLine(x, y+size_y-1, size_x, edge_color);
  et024006_DrawVertLine(x, y, size_y, edge_color);
}


//!
//! @brief This function initializes the host audio task.
//!
void host_audio_task_init(void)
{
  sof_cnt = 0;

  player_init();

  if (!host_audio_task_data)
    host_audio_task_data = calloc(1, sizeof(*host_audio_task_data));



#ifdef FREERTOS_USED
  xTaskCreate(host_audio_task,
              configTSK_USB_HAUDIO_NAME,
              configTSK_USB_HAUDIO_STACK_SIZE,
              NULL,
              configTSK_USB_HAUDIO_PRIORITY,
              NULL);
#endif  // FREERTOS_USED
}


//!
//! @brief This function displays the MMI interface and some static informations.
//!
static void mmi_display( void )
{
  #if (defined __GNUC__)
    char tmp[64];
  #endif
  if( mmi_state!=11 )
  {
     if( mmi_state==0 )
     {
        cpu_set_timeout( cpu_ms_2_cy(TIMER_STARTUP, FCPU_HZ), &timer );
        mmi_state++;
     }
     else if( cpu_is_timeout(&timer) )
     {
        switch( mmi_state++ )
        {
        case 1:
           LED_On( LED0 );
           cpu_set_timeout( cpu_ms_2_cy(TIMER_STARTUP, FCPU_HZ), &timer );

           // Clear the display
           et024006_DrawFilledRect(0, 0, ET024006_WIDTH, ET024006_HEIGHT, WHITE );

           // Display a logo.
           et024006_PutPixmap(avr32_logo, AVR32_LOGO_WIDTH, 0, 0
              ,(ET024006_WIDTH - AVR32_LOGO_WIDTH)/2
              ,(ET024006_HEIGHT - AVR32_LOGO_HEIGHT)/2, AVR32_LOGO_WIDTH, AVR32_LOGO_HEIGHT);
           et024006_PrintString(AUDIO_DEMO_STRING              , (const unsigned char *)&FONT8x16, 30, 5, BLACK, -1);
           #if(defined USB_RESYNC_METHOD)
              #if (USB_RESYNC_METHOD == USB_RESYNC_METHOD_EXT_CLOCK_SYNTHESIZER)
                 et024006_PrintString("32/44.1/48 KHz, HID, CS2200"     , (const unsigned char *)&FONT8x8, 50, START_Y_DEMO_TEXT+0*FONT_HEIGHT, BLUE, -1);
              #elif (USB_RESYNC_METHOD == USB_RESYNC_METHOD_SOFT_ADAPTIF_SRC)
                 et024006_PrintString("44.1 KHz, HID, adaptive SRC", (const unsigned char *)&FONT8x8, 50, START_Y_DEMO_TEXT+0*FONT_HEIGHT, BLUE, -1);
              #elif (USB_RESYNC_METHOD == USB_RESYNC_METHOD_SOFT_ADD_DEL_SAMPLES)
                 et024006_PrintString("44.1 KHz, HID, Add/remove sample", (const unsigned char *)&FONT8x8, 50, START_Y_DEMO_TEXT+0*FONT_HEIGHT, BLUE, -1);
              #else
                 #error Unknown synchronization method.
              #endif
           #endif

           // Display bargraph window.
           mmi_activity_display(true, (uint32_t)NULL);

           #if (defined __GNUC__)
              sprintf( tmp, "DATA: %ld bytes", (uint32_t)&_edata-(uint32_t)&_data);
              et024006_PrintString(tmp, (const unsigned char *)&FONT8x8, 50, START_Y_DEMO_TEXT+9*FONT_HEIGHT, BLUE, -1);

              sprintf( tmp, "BSS: %ld bytes", (uint32_t)&end-(uint32_t)&__bss_start);
              et024006_PrintString(tmp, (const unsigned char *)&FONT8x8, 50, START_Y_DEMO_TEXT+10*FONT_HEIGHT, BLUE, -1);

              sprintf( tmp, "Stack: %ld bytes", (uint32_t)&_estack-(uint32_t)&_stack);
              et024006_PrintString(tmp, (const unsigned char *)&FONT8x8, 50, START_Y_DEMO_TEXT+11*FONT_HEIGHT, BLUE, -1);
           #endif
           #if (defined __GNUC__)
              sprintf( tmp, "Total HEAP: %ld bytes", (uint32_t)&__heap_end__-(uint32_t)&__heap_start__ );
              et024006_PrintString(tmp, (const unsigned char *)&FONT8x8, 50, START_Y_DEMO_TEXT+12*FONT_HEIGHT, BLUE, WHITE);
              et024006_PrintString("Max HEAP:          bytes", (const unsigned char *)&FONT8x8, 50, START_Y_DEMO_TEXT+13*FONT_HEIGHT, BLUE, WHITE);
              et024006_PrintString("Curr HEAP:         bytes", (const unsigned char *)&FONT8x8, 50, START_Y_DEMO_TEXT+14*FONT_HEIGHT, BLUE, WHITE);
           #elif (defined __ICCAVR32__)
              et024006_PrintString("free HEAP:         bytes", (const unsigned char *)&FONT8x8, 50, START_Y_DEMO_TEXT+13*FONT_HEIGHT, BLUE, WHITE);
           #endif
           break;

        case 2: cpu_set_timeout( cpu_ms_2_cy(TIMER_STARTUP, FCPU_HZ), &timer ); LED_On( LED1 ); break;
        case 3: cpu_set_timeout( cpu_ms_2_cy(TIMER_STARTUP, FCPU_HZ), &timer ); LED_On( LED2 ); break;
        case 4: cpu_set_timeout( cpu_ms_2_cy(TIMER_STARTUP, FCPU_HZ), &timer ); LED_On( LED3 ); break;
        case 5: cpu_set_timeout( cpu_ms_2_cy(TIMER_STARTUP, FCPU_HZ), &timer ); LED_Off( LED0 ); break;
        case 6: cpu_set_timeout( cpu_ms_2_cy(TIMER_STARTUP, FCPU_HZ), &timer ); LED_Off( LED1 ); break;
        case 7: cpu_set_timeout( cpu_ms_2_cy(TIMER_STARTUP, FCPU_HZ), &timer ); LED_Off( LED2 ); break;
        case 8:
           LED_Off( LED3 );
           cpu_stop_timeout(&timer);
           break;

        default:
           break;
        }
     }
  }
}


//!
//! @brief This function displays the a bargraph indicating the state of the audio buffers and
//! the PPM deviation between the incoming USB audio sampling frequency and the output DAC sampling frequency.
//!
static void mmi_activity_display( bool init, uint32_t fifo_cnt )
{
  static char tmp[100];
  static int32_t  ppm;
  #define TIMER_MMI            1000   // Unit is in ms.
  static t_cpu_time mmi_timer;
  static uint32_t mmi_activity_state=0;
  static uint32_t i;
  uint32_t val;

  if( init )
  {
     // Display PPM window
     et024006_PrintString("PPM", (const unsigned char *)&FONT8x8, 22, 70+4, BLUE, -1);
     display_box(50, 70, 40, 16, WHITE, BLACK);

     et024006_PrintString("HID", (const unsigned char *)&FONT8x8, 122, 70+4, BLUE, -1);
     display_box(150, 70, 40, 16, WHITE, BLACK);

     et024006_PrintString("CPU", (const unsigned char *)&FONT8x8, 222, 70+4, BLUE, -1);
     display_box(250, 70, 40, 16, WHITE, BLACK);

     et024006_PrintString("Buffers", (const unsigned char *)&FONT8x8, 0, 50+4, BLUE, -1);
     display_box(50, 50, 195, 16, WHITE, BLACK);
     cpu_set_timeout( cpu_ms_2_cy(TIMER_MMI, FCPU_HZ), &mmi_timer );
     goto mmi_end;
  }

  if( !cpu_is_timeout(&mmi_timer) )
     goto mmi_end;

  switch( mmi_activity_state )
  {
  case 0:
     i = 0;
     mmi_activity_state = 1;
     // no break here

  case 1:
     if( i>=USB_STREAM_BUFFER_NUMBER )
     {
        mmi_activity_state = 10;
        break;
     }
     if( i<fifo_cnt )
        et024006_DrawFilledRect(50+3 + i*(10+2), 50+3, 10, 10, BLUE_LEV(15) );
     else
        et024006_DrawFilledRect(50+3 + i*(10+2), 50+3, 10, 10, WHITE );
     i++;
     break;

  case 10:
     ppm = usb_stream_ppm_get();
     sprintf( tmp, "%4ld", ppm );
     et024006_PrintString(tmp, (const unsigned char *)&FONT8x8, 50+4, 70+4, BLUE, WHITE);
     mmi_activity_state = 20;
     break;

  case 20:
     sprintf( tmp, "%ld", (uint32_t)FCPU_HZ/1000000);
     et024006_PrintString(tmp, (const unsigned char *)&FONT8x8, 250+4, 70+4, BLUE, WHITE);
     mmi_activity_state = 30;
     break;

#if (defined __GNUC__)
  case 30:
     val = get_heap_total_used_size();
     if (val<65000) {
       // get_heap_total_used_size() sometimes returns exotic values...
       sprintf( tmp, "%05ld", val );
       et024006_PrintString(tmp, (const unsigned char *)&FONT8x8, 146, START_Y_DEMO_TEXT+13*FONT_HEIGHT, BLUE, WHITE);
     }
     mmi_activity_state = 31;
     break;

  case 31:
     val = get_heap_curr_used_size();
     if (val<65000) {
       // .. and it may be also the case for get_heap_curr_used_size
       sprintf( tmp, "%05ld", val );
       et024006_PrintString(tmp, (const unsigned char *)&FONT8x8, 146, START_Y_DEMO_TEXT+14*FONT_HEIGHT, BLUE, WHITE);
     }
     // No break here
#elif (defined __ICCAVR32__)
  case 30:
     sprintf( tmp, "%ld", get_heap_free_size() );
     et024006_PrintString(tmp, (const unsigned char *)&FONT8x8, 146, START_Y_DEMO_TEXT+13*FONT_HEIGHT, BLUE, WHITE);
     // No break here

#endif

  default:
     // Rearm timer
     cpu_set_timeout( cpu_ms_2_cy(TIMER_MMI, FCPU_HZ), &mmi_timer );
     mmi_activity_state = 0;
     break;
  }

mmi_end:
  return;
}


//!
//! @brief This function manages the host audio task.
//!
#ifdef FREERTOS_USED
void host_audio_task(void *pvParameters)
#else
void host_audio_task(void)
#endif
{
   uint32_t  i, j;
   uint32_t  fifo_cnt;
   int  stream_status;

#ifdef FREERTOS_USED
   portTickType xLastWakeTime;
   xLastWakeTime = xTaskGetTickCount();

   while (true)
   {
      vTaskDelayUntil(&xLastWakeTime, configTSK_USB_HAUDIO_PERIOD);

#endif  // FREERTOS_USED
   // First, check the host controller is in full operating mode with the
   // B-device attached and enumerated
   if (Is_host_ready())
   {
      // New device connection (executed only once after device connection)
      if (audio_new_device_connected)
      {
         audio_new_device_connected = false;

         // For all supported interfaces
         for (i = 0; i < Get_nb_supported_interface(); i++)
         {
            // Audio Streaming Interface
            // Select the audio streaming interface that has an IN PIPE
            if((Get_class(i)==AUDIO_CLASS)
            && (Get_subclass(i)==AUDIOSTREAMING_SUBCLASS)
            && (Get_nb_ep(i) != 0))
            {
               for (j=0 ; j<Get_nb_ep(i) ; j++)
               {
                  if (Is_ep_in(i,j))
                  {
                    // Log in device
                    audio_connected=true;

                    Host_enable_sof_interrupt();

                    // Select and enable ISOCHRONOUS pipe
                    PIPE_AUDIO_IN = Get_ep_pipe(i,j);
                    Host_enable_continuous_in_mode(PIPE_AUDIO_IN);
                    Host_unfreeze_pipe(PIPE_AUDIO_IN);

                    // Enable alternate streaming interface
                    host_audio_task_data->interf_audio_stream = Get_interface_number(i);                     // store interface number

                    // Nonzero-bandwidth alternate USB audio streaming interface
                    host_audio_set_alt_interface();

                    break;
                  }
               }
            }
         }

         //printf("Detecting Microphone settings:\r\n");
         //printf(". Sample freq      =%ld Hz.\r\n", host_audio_task_data->g_sample_freq[     host_audio_task_data->interf_audio_stream]);
         //printf(". Num of channels  =%d.\r\n", host_audio_task_data->g_n_channels[       host_audio_task_data->interf_audio_stream]);
         //printf(". Bits per sample  =%d.\r\n", host_audio_task_data->g_n_bits_per_sample[host_audio_task_data->interf_audio_stream]);

         // Unmuting all features unit and setting default volume to MAX
         //
         for( i=0 ; i<host_audio_task_data->g_cs_num_features_unit ; i++ )
         {
            for( j=0 ; j<host_audio_task_data->g_cs_feature[i].n_bmaControls ; j++ )
            {
               uint16_t max;
               uint8_t  bmaControls= host_audio_task_data->g_cs_feature[i].bmaControls[j];

               if( bmaControls==0 )
                  continue;

               if( bmaControls & 0x01)
               {
                  host_audio_set_cur_mute(host_audio_task_data->g_cs_feature[i].unit, false);  // Unmute channels
               }

               if( bmaControls & 0x02)
               {
                  max= host_audio_get_max(host_audio_task_data->g_cs_feature[i].unit, j);
                  host_audio_set_cur(host_audio_task_data->g_cs_feature[i].unit, j, max);
               }
            }
         }
         host_set_sampling_rate(Host_get_pipe_endpoint_number(PIPE_AUDIO_IN)|MSK_EP_DIR, host_audio_task_data->g_sample_freq[interf_audio_stream]);

         usb_stream_init(
           host_audio_task_data->g_sample_freq[      host_audio_task_data->interf_audio_stream]
         , host_audio_task_data->g_n_channels[       host_audio_task_data->interf_audio_stream]
         , host_audio_task_data->g_n_bits_per_sample[host_audio_task_data->interf_audio_stream]
         , false
         );
      }

      mmi_display();

      if( audio_connected )
      {
         if((Is_host_in_received(pipe_audio_in) )
         && (Is_host_stall(pipe_audio_in)==false))
         {
           stream_status = usb_stream_input(USB_STREAM_HOST, pipe_audio_in, &fifo_cnt);
/*            if( USB_STREAM_STATUS_OK == stream_status )
            {
               LED_Off( LED0 );
               LED_On(  LED1 );
               LED_Off( LED2 );
            }
            else if( USB_STREAM_STATUS_SPEED_UP == stream_status )
            {
               LED_On(  LED0 );
               LED_Off( LED1 );
               LED_Off( LED2 );
            }
            else if( USB_STREAM_STATUS_SLOW_DOWN == stream_status )
            {
               LED_Off( LED0 );
               LED_Off( LED1 );
               LED_On(  LED2 );
            }
            else if( USB_STREAM_STATUS_NOT_SYNCHRONIZED == stream_status )
            {
               LED_On( LED0 );
               LED_On( LED1 );
               LED_On( LED2 );
            }*/
            mmi_activity_display(false, fifo_cnt);
            Host_ack_in_received(pipe_audio_in);
            Host_free_in(pipe_audio_in);
         }

         if(Is_host_nak_received(pipe_audio_in))
         {
            Host_ack_nak_received(pipe_audio_in);
         }
      }
#if USB_RESYNC_AUDIO_STREAM && (USB_RESYNC_AUDIO_STREAM==true)
    usb_stream_resync();
#endif
   }
#ifdef FREERTOS_USED
  }
#endif
}


//!
//! @brief host_sof_action
//!
//! This function increments the sof_cnt counter each time
//! the USB Start-of-Frame interrupt subroutine is executed (1 ms).
//! Useful to manage time delays
//!
void host_sof_action(void)
{
  sof_cnt++;
}

void host_audio_set_alt_interface(void)
{
  host_set_interface(host_audio_task_data->interf_audio_stream, 1);   // enable streaming interface with "alternate 1" on Device
}

//!
//! @brief This function controls the Mute feature of a particular unit
//!
void host_audio_set_cur_mute(uint16_t unit, bool cs_mute)
{
   data_stage[0] = cs_mute;
   usb_request.bmRequestType   = 0x21;
   usb_request.bRequest        = BR_REQUEST_SET_CUR;
   usb_request.wValue          = CS_MUTE;
   usb_request.wIndex          = (unit)<<8;
   usb_request.wLength         = 1;
   usb_request.incomplete_read = false;
   host_transfer_control(data_stage);
}


//!
//! @brief This function set the 'current setting' feature of a particular unit
//!
void host_audio_set_cur(uint16_t unit, uint16_t channel_number, uint16_t cur)
{
   data_stage[0] = LSB(cur);
   data_stage[1] = MSB(cur);
   usb_request.bmRequestType   = 0x21;
   usb_request.bRequest        = BR_REQUEST_SET_CUR;
   usb_request.wValue          = CS_VOLUME | channel_number;
   usb_request.wIndex          = (unit)<<8;
   usb_request.wLength         = 2;
   usb_request.incomplete_read = false;
   host_transfer_control(data_stage);
}


//!
//! @brief This function returns the 'MAX setting' feature of a particular unit
//!
uint16_t host_audio_get_max(uint16_t unit, uint16_t channel_number)
{
   uint16_t max;
   usb_request.bmRequestType   = 0xA1;
   usb_request.bRequest        = BR_REQUEST_GET_MAX;
   usb_request.wValue          = CS_VOLUME | channel_number;
   usb_request.wIndex          = (unit)<<8;
   usb_request.wLength         = 2;
   usb_request.incomplete_read = false;
   host_transfer_control(data_stage);
   LSB(max)= data_stage[0];
   MSB(max)= data_stage[1];
   return max;
}


//!
//! @brief This function selects one of the sampling rate of the streaming interface.
//!
void host_set_sampling_rate(uint16_t endpoint, uint32_t sampling_rate)
{
   data_stage[0] = MSB3(sampling_rate);
   data_stage[1] = MSB2(sampling_rate);
   data_stage[2] = MSB1(sampling_rate);
   usb_request.bmRequestType   = 0x22;
   usb_request.bRequest        = 0x01;
   usb_request.wValue          = DEVICE_DESCRIPTOR << 8;
   usb_request.wIndex          = endpoint;
   usb_request.wLength         = 3;
   usb_request.incomplete_read = false;
   host_transfer_control(data_stage);
}


//!
//! @brief This function shuts the host audio task down.
//!
void host_audio_task_shutdown(void)
{
  free(host_audio_task_data);
  host_audio_task_data = NULL;

  player_shutdown();

}


#endif  // USB_HOST_FEATURE == true
