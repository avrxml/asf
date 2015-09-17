/**************************************************************************
 *
 * \file
 *
 * \brief Management of the USB device Audio task.
 *
 * This file manages the USB device Audio task.
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

#include <stdio.h>
#include "usart.h"     // Shall be included before FreeRTOS header files, since 'inline' is defined to ''; leading to
                       // link errors
#include "conf_usb.h"


#if USB_DEVICE_FEATURE == true

#include "board.h"
#ifdef FREERTOS_USED
#  include "FreeRTOS.h"
#  include "task.h"
#endif
#include "controller.h"
#include "usb_drv.h"
#include "usb_descriptors.h"
#include "usb_standard_request.h"
#include "device_audio_task.h"
#include "usb_stream_player.h"
#include "audio_example.h"
#include "et024006dhu.h"
#include "cycle_counter.h"
#include "debug.h"
#include "avr32_logo.h"
#if (BOARD!=EVK1105) || (DEFAULT_DACS!=AUDIO_MIXER_DAC_AIC23B)
# include "microphone_samples.h"
#endif


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________


//_____ D E C L A R A T I O N S ____________________________________________

#define TIMER_STARTUP    100
#define START_Y_DEMO_TEXT 33

#define FONT_HEIGHT   14
device_audio_task_data_t *device_audio_task_data = NULL;

static uint16_t  sof_cnt;
static uint32_t  mmi_state=0;

static void mmi_display( void );
static void mmi_activity_display( bool init, uint32_t fifo_cnt );

//!
//! Public : (bit) mute
//! mute is set to true when ACTIVE
//! mute is set to false otherwise
//!/
bool   mute;

//!
//! Public : (int16_t) volume
//! volume represents the current volume of audio stream
//!/
int16_t volume;

#if (BOARD!=EVK1105) || (DEFAULT_DACS!=AUDIO_MIXER_DAC_AIC23B)
static uint32_t  dat_sample_index; // Device Audio Task sample index.

#else

//!
//! Holds the pointers on a ping-pong audio buffer. Memory needs to be allocated.
struct microphone_st{
  uint16_t* buffer;
  uint16_t  size;
};

struct microphone_st microphone[2];

//!
//! Indicate if the microphone background interrupt is started or not.
static bool b_microphone_started=false;

//!
//! Control the microphone autoreload interruption. We need to sometimes
//! prevent the IRL from reloading automatically in the case that you want to
//! flush the entire codec. This may happen if you receive a SET_XXXXXX command
//! from the USB.
static bool b_microphone_pause=false;

//!
//! Holds the current microphone id to be sent to the USB
static uint8_t mic_buf_id = 0;

//!
//! Holds the packet id that will make a perfect 44.1 KHz for the microphone output on the USB.
//! The packets are sent each ms. The packet size is an integer number of 16-bits sample. So, we
//! can not 44.1 sample each ms. We will send most of the time 44 samples, than 45 samples.
static uint8_t mic_frame_id = 0;
void* sample_buffer1[2];
#endif


//!
//! @brief This function initializes the hardware/software resources
//! required for device Audio task.
//!
void device_audio_task_init(void)
{
  sof_cnt   =0;
#if (BOARD!=EVK1105)
  dat_sample_index =0;
#endif

#ifndef FREERTOS_USED
  #if USB_HOST_FEATURE == true
  // If both device and host features are enabled, check if device mode is engaged
  // (accessing the USB registers of a non-engaged mode, even with load operations,
  // may corrupt USB FIFO data).
  if (Is_usb_device())
  #endif  // USB_HOST_FEATURE == true
    Usb_enable_sof_interrupt();
#endif  // FREERTOS_USED

  player_init();

  if (!device_audio_task_data)
    device_audio_task_data = calloc(1, sizeof(*device_audio_task_data));

#if (BOARD==EVK1105) && (DEFAULT_DACS==AUDIO_MIXER_DAC_AIC23B)
  // Allocate memory for the microphone audio samples.
  // Receiving stereo samples from the ADC.
  // For a 48KHz, needs 196 bytes. Let's use a little more.
  microphone[0].buffer= (uint16_t*)malloc(200);
  microphone[0].size  = 0;
  microphone[1].buffer= (uint16_t*)malloc(200);
  microphone[1].size  = 0;
  b_microphone_started=false;
  b_microphone_pause=false;
#endif

  usb_stream_init(
    SPEAKER_FREQUENCY
  , 2
  , 16
  , false
  );

#ifdef FREERTOS_USED
  xTaskCreate(device_audio_task,
              configTSK_USB_DAUDIO_NAME,
              configTSK_USB_DAUDIO_STACK_SIZE,
              NULL,
              configTSK_USB_DAUDIO_PRIORITY,
              NULL);

#endif  // FREERTOS_USED
}


#if (defined __GNUC__)
  extern uint8_t *_data;
  extern uint8_t *_edata;
  extern uint8_t *__bss_start;
  extern uint8_t *end;
  extern uint8_t *_stack;
  extern uint8_t *_estack;
#endif

  t_cpu_time timer;


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
//! @brief This function displays the MMI interface and some static informations.
//!
static void mmi_display( void )
{
  #if (defined __GNUC__) || ((defined USB_RESYNC_METHOD) && (USB_RESYNC_METHOD == USB_RESYNC_METHOD_SOFT_ADD_DEL_SAMPLES))
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
              #if (defined USB_RESYNC_METHOD) && (USB_RESYNC_METHOD == USB_RESYNC_METHOD_EXT_CLOCK_SYNTHESIZER)
                 et024006_PrintString("32/44.1/48 KHz, HID, CS2200"     , (const unsigned char *)&FONT8x8, 50, START_Y_DEMO_TEXT+0*FONT_HEIGHT, BLUE, -1);
              #elif (defined USB_RESYNC_METHOD) && (USB_RESYNC_METHOD == USB_RESYNC_METHOD_SOFT_ADAPTIF_SRC)
                 et024006_PrintString("%32/44.1/48 KHz, HID, adaptive SRC", (const unsigned char *)&FONT8x8, 50, START_Y_DEMO_TEXT+0*FONT_HEIGHT, BLUE, -1);
              #elif (defined USB_RESYNC_METHOD) && (USB_RESYNC_METHOD == USB_RESYNC_METHOD_SOFT_ADD_DEL_SAMPLES)
                 sprintf( tmp, "%d.%d KHz, HID, Add/remove sample", SPEAKER_FREQUENCY/1000, SPEAKER_FREQUENCY%1000);
                 et024006_PrintString(tmp, (const unsigned char *)&FONT8x8, 50, START_Y_DEMO_TEXT+0*FONT_HEIGHT, BLUE, -1);
              #else
                 #error Unknown synchronization method.
              #endif
           #endif

           // Display bargraph window.
           mmi_activity_display(true, (uint32_t)NULL);

           #if (defined __GNUC__)
              sprintf( tmp, "RAM (DATA): %ld bytes", (uint32_t)&_edata-(uint32_t)&_data);
              et024006_PrintString(tmp, (const unsigned char *)&FONT8x8, 50, START_Y_DEMO_TEXT+10*FONT_HEIGHT, BLUE, -1);

              sprintf( tmp, "RAM (BSS): %ld bytes", (uint32_t)&end-(uint32_t)&__bss_start);
              et024006_PrintString(tmp, (const unsigned char *)&FONT8x8, 50, START_Y_DEMO_TEXT+11*FONT_HEIGHT, BLUE, -1);

              sprintf( tmp, "RAM (STACK): %ld bytes", (uint32_t)&_estack-(uint32_t)&_stack);
              et024006_PrintString(tmp, (const unsigned char *)&FONT8x8, 50, START_Y_DEMO_TEXT+12*FONT_HEIGHT, BLUE, -1);
           #endif
           #if (defined __GNUC__)
              et024006_PrintString("RAM (total used HEAP):       bytes", (const unsigned char *)&FONT8x8, 50, START_Y_DEMO_TEXT+13*FONT_HEIGHT, BLUE, WHITE);
              et024006_PrintString("RAM (curr used HEAP):        bytes", (const unsigned char *)&FONT8x8, 50, START_Y_DEMO_TEXT+14*FONT_HEIGHT, BLUE, WHITE);
           #elif (defined __ICCAVR32__)
              et024006_PrintString("RAM (free HEAP):             bytes", (const unsigned char *)&FONT8x8, 50, START_Y_DEMO_TEXT+14*FONT_HEIGHT, BLUE, WHITE);
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
  static char tmp[20];
  static int32_t  ppm;
  #define TIMER_MMI            1000   // Unit is in ms.
  static t_cpu_time mmi_timer;
  static uint32_t old_fcpu_hz = 0;
  static uint32_t mmi_activity_state=0;
  static uint32_t i;

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
     if( old_fcpu_hz==FCPU_HZ )
     {
        mmi_activity_state = 30;
        break;
     }
     else
        mmi_activity_state = 21;
     // No break here

  case 21:
     old_fcpu_hz=FCPU_HZ;
     sprintf( tmp, "%ld", (uint32_t)FCPU_HZ/1000000);
     et024006_PrintString(tmp, (const unsigned char *)&FONT8x8, 250+4, 70+4, BLUE, WHITE);
     mmi_activity_state = 30;
     break;

#if (defined __GNUC__)
  case 30:
     sprintf( tmp, "%ld", get_heap_total_used_size() );
     et024006_PrintString(tmp, (const unsigned char *)&FONT8x8, 230, START_Y_DEMO_TEXT+13*FONT_HEIGHT, BLUE, WHITE);
     mmi_activity_state = 31;
     break;

  case 31:
     sprintf( tmp, "%ld", get_heap_curr_used_size() );
     et024006_PrintString(tmp, (const unsigned char *)&FONT8x8, 230, START_Y_DEMO_TEXT+14*FONT_HEIGHT, BLUE, WHITE);
     // No break here
#elif (defined __ICCAVR32__)
  case 30:
     sprintf( tmp, "%ld", get_heap_free_size() );
     et024006_PrintString(tmp, (const unsigned char *)&FONT8x8, 230, START_Y_DEMO_TEXT+14*FONT_HEIGHT, BLUE, WHITE);
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
//! @brief Entry point of the device Audio task management
//!
#ifdef FREERTOS_USED
void device_audio_task(void *pvParameters)
#else
void device_audio_task(void)
#endif
{
#if( BOARD!=EVK1105) || (DEFAULT_DACS!=AUDIO_MIXER_DAC_AIC23B)
  uint32_t  i;
#endif
  uint32_t  fifo_cnt;
  int  stream_status;

#ifdef FREERTOS_USED
  portTickType xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  LED_On(LED0);

  while (true)
  {
    vTaskDelayUntil(&xLastWakeTime, configTSK_USB_DAUDIO_PERIOD);

    // First, check the device enumeration state
    if (!Is_device_enumerated()) { mmi_state=0; continue; }
#else
    // First, check the device enumeration state
    if (!Is_device_enumerated()) { mmi_state=0; return; }
#endif  // FREERTOS_USED

    mmi_display();

#if( BOARD!=EVK1105) || (DEFAULT_DACS!=AUDIO_MIXER_DAC_AIC23B)
    // Microphone emulation
    //
    if ( is_joystick_pressed() )
    {
       if (Is_usb_write_enabled(EP_AUDIO_IN))   // Endpoint buffer free ?
       {
          Usb_reset_endpoint_fifo_access(EP_AUDIO_IN);
          for( i=0 ; i<EP_SIZE_IN ; i++ )   // Fill endpoint with sample raw
          {
             if(mute==false)
             {
                uint8_t sample;
                sample = sample_sound[dat_sample_index++];
                LED_Set_Intensity(LED0, sample);
                Usb_write_endpoint_data(EP_AUDIO_IN, 8, sample);
                if (dat_sample_index >= SAMPLE_SOUND_LEN)
                {  dat_sample_index=0; }
             }
             else
             {
                LED_Set_Intensity(LED0, 0);
                Usb_write_endpoint_data(EP_AUDIO_IN, 8, 0x00);
             }
          }
          Usb_ack_in_ready_send(EP_AUDIO_IN);
       }
    }
#else
    // Handle input stream from microphone
    if( !b_microphone_started && (Is_usb_write_enabled(EP_AUDIO_IN)) && audio_mixer_dacs_input(NULL, 0) )
    {
      // Start ADC conversion. This will launch the IRL in background...
      mic_buf_id = 0;
      mic_frame_id=2;
      microphone[mic_buf_id].size=45;
      audio_mixer_dacs_input(microphone[0].buffer, 45 /*microphone[mic_buf_id].size*/);
      audio_mixer_dacs_input(microphone[1].buffer, 45 /*microphone[mic_buf_id].size*/);
      b_microphone_started=true;
    }
#endif



    // Handle incoming audio samples
    //
    if((Is_usb_out_received(EP_AUDIO_OUT) )
    /*&& (Is_usb_stall(EP_AUDIO_OUT)==false)*/)
    {
       stream_status = usb_stream_input(USB_STREAM_DEVICE, EP_AUDIO_OUT, &fifo_cnt);
       Usb_ack_out_received_free(EP_AUDIO_OUT);
       mmi_activity_display(false, fifo_cnt);
    }
#if USB_RESYNC_AUDIO_STREAM && (USB_RESYNC_AUDIO_STREAM==true)
    usb_stream_resync();
#endif

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

#if( BOARD==EVK1105) && (DEFAULT_DACS == AUDIO_MIXER_DAC_AIC23B)
void adc_underflow_cb(void)
{
  b_microphone_started=false;
}

void adc_sample_sent_cb(void)
{
  uint32_t i;
  uint8_t  old_mic_buf_id=mic_buf_id;

  if (!b_microphone_pause && Is_usb_write_enabled(EP_AUDIO_IN) && audio_mixer_dacs_input(NULL, 0) )  // Endpoint buffer free ?
  {
    mic_frame_id++;
    if( mic_frame_id>=10 )
    {
      mic_frame_id=0;
      microphone[mic_buf_id].size = 45;
    }
    else
      microphone[mic_buf_id].size = 44;

    Usb_reset_endpoint_fifo_access(EP_AUDIO_IN);
    if(!mute)
    {
      for(i = 0; i < microphone[old_mic_buf_id].size; i++)
      {
          Usb_write_endpoint_data(EP_AUDIO_IN, 8, LSB(microphone[old_mic_buf_id].buffer[2 * i]));
          Usb_write_endpoint_data(EP_AUDIO_IN, 8, MSB(microphone[old_mic_buf_id].buffer[2 * i]));
      }
    }
    else
    {
      for(i = 0; i < 2*microphone[old_mic_buf_id].size; i++)
      {
          Usb_write_endpoint_data(EP_AUDIO_IN, 8, 0);
      }
    }
    Usb_ack_in_ready_send(EP_AUDIO_IN);

    audio_mixer_dacs_input(microphone[mic_buf_id].buffer, 45/*microphone[mic_buf_id].size*/);
    // Let's switch to the next buffer.
    mic_buf_id^=1;
  }
  else
    b_microphone_started=false;
}

void device_audio_disable_microphone(void)
{
  b_microphone_pause=true;
}

void device_audio_enable_microphone(void)
{
  b_microphone_pause=false;
}
#endif
