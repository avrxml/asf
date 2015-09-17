/**************************************************************************
 *
 * \file
 *
 * \brief Processing of USB device specific enumeration requests.
 *
 * This file contains the specific request decoding for enumeration process.
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


//_____ I N C L U D E S ____________________________________________________

#include "conf_audio_mixer.h"
#include "conf_audio_player.h"
#include "conf_usb.h"


#if USB_DEVICE_FEATURE == true

#include "usb_drv.h"
#include "usb_descriptors.h"
#include "usb_standard_request.h"
#include "usb_specific_request.h"
#include "usart.h"
#include "audio_example.h"
#include "usb_stream_player.h"
#include "device_audio_task.h"
#include "hid.h"

#define TASKS_SCHEDULE()    device_audio_task()

//_____ M A C R O S ________________________________________________________

#define TARGET_UNDEFINED 0
#define TARGET_SPEAKER   1
#define TARGET_MICRO     2

typedef struct {
  uint8_t target;   // audio target (speaker or micro)
  uint8_t cs;       // Command selector
} st_audio_cmd;

//! Mute control of speaker
//! true when ACTIVE, false otherwise
//!
bool b_speaker_mute;

//! Represents the current volume of audio stream speaker
int16_t s16_speaker_volume = (SPEAKER_VOL_MIN + SPEAKER_VOL_MAX)/2;

//! Represents a copy of the USB volume. The USB volume has a range [USB_SPEAKER_VOL_MIN; USB_SPEAKER_VOL_MAX]
//! and must be converted to the volume of the DAC used.
//! We keep back a copy of the last USB volume since Windows ask for the current volume and a double conversion
//! USB -> appli -> USB may lead to imprecision.
int16_t s16_usb_speaker_volume = (USB_SPEAKER_VOL_MIN + USB_SPEAKER_VOL_MAX)/2;

//! Mute control of micro
//! true when ACTIVE, false otherwise
//!
bool b_micro_mute;

//! Represents the current volume of audio stream micro
//! No effect in this demo
//!
int16_t s16_micro_volume;


//_____ D E F I N I T I O N S ______________________________________________

st_audio_cmd check_audio_control_request(uint16_t wValue);


//_____ P R I V A T E   D E C L A R A T I O N S ____________________________

extern const    void *pbuffer;
extern uint16_t  data_to_transfer;

//_____ D E C L A R A T I O N S ____________________________________________

static void hid_get_descriptor(uint8_t size_of_report, const uint8_t* p_usb_hid_report);
static void usb_hid_set_report_ouput(void);
static void usb_hid_set_idle (uint8_t u8_report_id, uint8_t u8_duration );
static void usb_hid_get_idle (uint8_t u8_report_id);
static int16_t  volume_usb_to_appli(int16_t usb_volume);



//! @brief This function configures the endpoints of the device application.
//! This function is called when the set configuration request has been received.
//!
void usb_user_endpoint_init(uint8_t conf_nb)
{

    (void)Usb_configure_endpoint( EP_KBD_IN,
                                 TYPE_INTERRUPT,
                                 DIRECTION_IN,
                                 EP_SIZE_KBD,
                                 SINGLE_BANK);

    (void)Usb_configure_endpoint( EP_AUDIO_OUT,
                                 EP_ATTRIBUTES_OUT,
                                 DIRECTION_OUT,
                                 EP_SIZE_OUT,
                                 DOUBLE_BANK);

    (void)Usb_configure_endpoint( EP_AUDIO_IN,
                                 EP_ATTRIBUTES_IN,
                                 DIRECTION_IN,
                                 EP_SIZE_IN,
                                 DOUBLE_BANK);
}


//! This function is called by the standard USB read request function when
//! the USB request is not supported. This function returns true when the
//! request is processed. This function returns false if the request is not
//! supported. In this case, a STALL handshake will be automatically
//! sent by the standard USB read request function.
//!
bool usb_user_read_request(uint8_t bmRequestType, uint8_t bmRequest)
{
   st_audio_cmd cmd;

   uint16_t wValue;
   LSB(wValue)=Usb_read_endpoint_data(EP_CONTROL, 8);
   MSB(wValue)=Usb_read_endpoint_data(EP_CONTROL, 8);

   // HID Keyboard stuff
   if( USB_SETUP_GET_STAND_INTERFACE == bmRequestType )
   {
      switch( bmRequest )
      {
         case SETUP_GET_DESCRIPTOR:
         switch( MSB(wValue) ) // Descriptor ID
         {
            case HID_DESCRIPTOR:
            hid_get_descriptor(
               sizeof(usb_conf_desc.hid_kbd)
            ,  (const uint8_t*)&usb_conf_desc.hid_kbd);
            return true;
            // no break here

            case HID_REPORT_DESCRIPTOR:
            hid_get_descriptor(
               sizeof(usb_hid_report_descriptor_kbd)
            ,  usb_hid_report_descriptor_kbd);
            return true;

            case HID_PHYSICAL_DESCRIPTOR:
            // TODO
            break;
         }
         break;
      }
   }

   //** Specific request from Class Audio
   //* AudioControl Requests
   if( USB_SETUP_SET_CLASS_INTER == bmRequestType )
   {
      switch( bmRequest )
      {
         // HID Stuff

         case HID_SET_REPORT:
         // The MSB wValue field specifies the Report Type
         // The LSB wValue field specifies the Report ID
         switch (MSB(wValue))
         {
            case HID_REPORT_INPUT:
            // TODO
            break;

            case HID_REPORT_OUTPUT:
            usb_hid_set_report_ouput();
            return true;
            // no break here

            case HID_REPORT_FEATURE:
            break;
         }
         break;

         case HID_SET_IDLE:
         usb_hid_set_idle(LSB(wValue), MSB(wValue));
         return true;

         case HID_SET_PROTOCOL:
         // TODO if you use a sub class HID_SUB_CLASS_BOOT
         break;



         // Audio stuff

         case SETUP_AUDIO_SET_CUR:
         cmd = check_audio_control_request(wValue);
         switch( cmd.target )
         {
            case TARGET_SPEAKER:
            switch( cmd.cs )
            {
               case AUDIO_FU_CONTROL_CS_MUTE:
               audio_speaker_set_mute();
               return true;
               //break;
               case AUDIO_FU_CONTROL_CS_VOLUME:
               audio_speaker_set_volume();
               return true;
               //break;
            }
            break;
            case TARGET_MICRO:
            switch( cmd.cs )
            {
               case AUDIO_FU_CONTROL_CS_MUTE:
               audio_micro_set_mute();
               return true;
               //break;
               case AUDIO_FU_CONTROL_CS_VOLUME:
               audio_micro_set_volume();
               return true;
               //break;
            }
            break;
         }
         break;

         case SETUP_AUDIO_SET_MIN:
         case SETUP_AUDIO_SET_MAX:
         case SETUP_AUDIO_SET_RES:
         case SETUP_AUDIO_SET_MEM:
         // TODO
         break;
      }
   }

   if( USB_SETUP_GET_CLASS_INTER == bmRequestType )
   {
      // HID stuff
      switch( bmRequest )
      {
         case HID_GET_REPORT:
         // TODO
         break;
         case HID_GET_IDLE:
         usb_hid_get_idle(LSB(wValue));
         return true;
         case HID_GET_PROTOCOL:
         // TODO if you use a sub class HID_SUB_CLASS_BOOT
         break;
      }



      // Audio stuff

      switch( bmRequest )
      {
         case SETUP_AUDIO_GET_CUR:
         cmd = check_audio_control_request(wValue);
         switch( cmd.target )
         {
            case TARGET_SPEAKER:
            switch( cmd.cs )
            {
               case AUDIO_FU_CONTROL_CS_MUTE:
               audio_speaker_get_mute();
               return true;
               //break;
               case AUDIO_FU_CONTROL_CS_VOLUME:
               audio_speaker_get_vol_cur();
               return true;
               //break;
            }
            break;
            case TARGET_MICRO:
            switch( cmd.cs )
            {
               case AUDIO_FU_CONTROL_CS_MUTE:
               audio_micro_get_mute();
               return true;
               //break;
               case AUDIO_FU_CONTROL_CS_VOLUME:
               audio_micro_get_vol_cur();
               return true;
               //break;
            }
            break;
         }
         break;

         case SETUP_AUDIO_GET_MIN:
         cmd = check_audio_control_request(wValue);
         switch( cmd.target )
         {
            case TARGET_SPEAKER:
            switch( cmd.cs )
            {
               case AUDIO_FU_CONTROL_CS_MUTE:
               audio_speaker_get_mute();
               return true;
               //break;
               case AUDIO_FU_CONTROL_CS_VOLUME:
               audio_speaker_get_vol_min();
               return true;
               //break;
            }
            break;
            case TARGET_MICRO:
            switch( cmd.cs )
            {
               case AUDIO_FU_CONTROL_CS_MUTE:
               audio_micro_get_mute();
               return true;
               //break;
               case AUDIO_FU_CONTROL_CS_VOLUME:
               audio_micro_get_vol_min();
               return true;
               //break;
            }
            break;
         }
         break;

         case SETUP_AUDIO_GET_MAX:
         cmd = check_audio_control_request(wValue);
         switch( cmd.target )
         {
            case TARGET_SPEAKER:
            switch( cmd.cs )
            {
               case AUDIO_FU_CONTROL_CS_MUTE:
               audio_speaker_get_mute();
               return true;
               //break;
               case AUDIO_FU_CONTROL_CS_VOLUME:
               audio_speaker_get_vol_max();
               return true;
               //break;
            }
            break;
            case TARGET_MICRO:
            switch( cmd.cs )
            {
               case AUDIO_FU_CONTROL_CS_MUTE:
               audio_micro_get_mute();
               return true;
               //break;
               case AUDIO_FU_CONTROL_CS_VOLUME:
               audio_micro_get_vol_max();
               return true;
               //break;
            }
            break;
         }
         break;

         case SETUP_AUDIO_GET_RES:
         cmd = check_audio_control_request(wValue);
         switch( cmd.target )
         {
            case TARGET_SPEAKER:
            switch( cmd.cs )
            {
               case AUDIO_FU_CONTROL_CS_MUTE:
               audio_speaker_get_mute();
               return true;
               //break;
               case AUDIO_FU_CONTROL_CS_VOLUME:
               audio_speaker_get_vol_res();
               return true;
               //break;
            }
            break;
            case TARGET_MICRO:
            switch( cmd.cs )
            {
               case AUDIO_FU_CONTROL_CS_MUTE:
               audio_micro_get_mute();
               return true;
               //break;
               case AUDIO_FU_CONTROL_CS_VOLUME:
               audio_micro_get_vol_res();
               return true;
               //break;
            }
            break;
         }
         break;

         case SETUP_AUDIO_GET_MEM:
         case SETUP_AUDIO_GET_STAT:
         // TODO
         break;
      }
   }

   //* AudioStreaming Requests
   if( USB_SETUP_SET_CLASS_ENDPOINT == bmRequestType )
   {
      switch( bmRequest )
      {
         case SETUP_AUDIO_SAMPLING_FREQ_CONTROL:
           audio_speaker_set_sample_freq();
           return true;
           //break;
         case SETUP_AUDIO_SET_MEM:
         case SETUP_AUDIO_SET_MIN:
         case SETUP_AUDIO_SET_MAX:
         case SETUP_AUDIO_SET_RES:
         // TODO
         break;
      }
   }
   if( USB_SETUP_GET_CLASS_ENDPOINT == bmRequestType )
   {
      switch( bmRequest )
      {
         case SETUP_AUDIO_GET_CUR:
         case SETUP_AUDIO_GET_MIN:
         case SETUP_AUDIO_GET_MAX:
         case SETUP_AUDIO_GET_RES:
         case SETUP_AUDIO_GET_MEM:
         case SETUP_AUDIO_GET_STAT:
         // TODO
         break;
      }
   }
   return false;  // No supported bmRequest
}

//! This function returns the size and the pointer on a user information
//! structure
//!
bool usb_user_get_descriptor(uint8_t type, uint8_t string)
{
  pbuffer = NULL;

   switch(type)
   {
   case STRING_DESCRIPTOR:
      switch (string)
      {
         case LANG_ID:
         data_to_transfer = sizeof (usb_user_language_id);
         pbuffer = &usb_user_language_id;
         break;

         case MAN_INDEX:
         data_to_transfer = sizeof (usb_user_manufacturer_string_descriptor);
         pbuffer = &usb_user_manufacturer_string_descriptor;
         break;

         case PROD_INDEX:
         data_to_transfer = sizeof (usb_user_product_string_descriptor);
         pbuffer = &usb_user_product_string_descriptor;
         break;

         case SN_INDEX:
         data_to_transfer = sizeof (usb_user_serial_number);
         pbuffer = &usb_user_serial_number;
         break;

         default:
         break;
      }
      break;

  default:
    break;
   }

  return pbuffer != NULL;
}



//! Checks if the audio control request is a Feature Unit Control request for the good feature unit and channel
//!
//! @return the target audio and the Control Selector from request
//!
st_audio_cmd check_audio_control_request(uint16_t wValue)
{
   st_audio_cmd cmd;
   uint16_t wIndex;
   uint16_t length;

   LSB(wIndex)=Usb_read_endpoint_data(EP_CONTROL, 8);
   MSB(wIndex)=Usb_read_endpoint_data(EP_CONTROL, 8);
   LSB(length)=Usb_read_endpoint_data(EP_CONTROL, 8);
   MSB(length)=Usb_read_endpoint_data(EP_CONTROL, 8);
   Usb_ack_setup_received_free();

   cmd.target = TARGET_UNDEFINED;
   cmd.cs     = AUDIO_FU_CONTROL_CS_UNDEFINED;

   // Note: The wValue field interpretation is qualified by the value in the wIndex field.
   // Check target of the command
   if( LSB(wIndex) == AC_INTERFACE_NB)
   {
      // The request is for the Feature Unit Control then
      // MSB(wIndex) = Feature Unit ID, LSB(wValue) = Channel Number (CN), MSB(wValue) = Control Selector (CS)
      cmd.cs = MSB(wValue);

      if( (MSB(wIndex) == SPEAKER_FEATURE_UNIT_ID)
      &&  (LSB(wValue) == 0) )
      {
         // for Master Channel of speaker
         cmd.target = TARGET_SPEAKER;

      }
      if( (MSB(wIndex) == MICRO_FEATURE_UNIT_ID)
      &&  (LSB(wValue) == 0) )
      {
         // for Master Channel of micro
         cmd.target = TARGET_MICRO;
      }
   }
   return cmd;
}

//******* Speaker control
void audio_speaker_set_mute(void)
{
   while(!Is_usb_control_out_received())
   {
     TASKS_SCHEDULE();
   }
   Usb_reset_endpoint_fifo_access(EP_CONTROL);
   b_speaker_mute=Usb_read_endpoint_data(EP_CONTROL, 8);
   audio_mixer_mute_audio(b_speaker_mute);

   if( b_speaker_mute )
   {
     audio_mixer_dacs_set_volume_direct((uint8_t)SPEAKER_VOL_MIN);
   }
   else
   {
     audio_mixer_dacs_set_volume_direct(s16_speaker_volume);
   }

   Usb_ack_control_out_received_free();
   Usb_ack_control_in_ready_send();  // send a ZLP
   while (!Is_usb_control_in_ready())
   {
     TASKS_SCHEDULE();
   }
}
void audio_speaker_get_mute(void)
{
   Usb_reset_endpoint_fifo_access(EP_CONTROL);
   Usb_write_endpoint_data(EP_CONTROL, 8, b_speaker_mute);
   Usb_ack_control_in_ready_send();
   while(!Is_usb_control_out_received()) // send a ZLP
   {
     TASKS_SCHEDULE();
   }
   Usb_ack_control_out_received_free();
}
void audio_speaker_set_sample_freq(void)
{
   uint32_t sample_freq=0;
   Usb_ack_setup_received_free();
   while(!Is_usb_control_out_received())
   {
     TASKS_SCHEDULE();
   }
   Usb_reset_endpoint_fifo_access(EP_CONTROL);
   LSB0(sample_freq)=Usb_read_endpoint_data(EP_CONTROL, 8);
   LSB1(sample_freq)=Usb_read_endpoint_data(EP_CONTROL, 8);
   LSB2(sample_freq)=Usb_read_endpoint_data(EP_CONTROL, 8);
   Usb_ack_control_out_received_free();
   Usb_ack_control_in_ready_send();  // send a ZLP
   while (!Is_usb_control_in_ready())
   {
     TASKS_SCHEDULE();
   }

#if (defined BOARD) && (BOARD==EVK1105) && (defined DEFAULT_DACS) && (DEFAULT_DACS==AUDIO_MIXER_DAC_AIC23B)
   // Disable microphone callback interrupt in order for the flush to not be blocked.
   // TODO: audio_mixer_dacs_flush_direct flushes Rx and Tw part of the DAC.
   //       Should we separate them? Here, we want to flash the output only.
   device_audio_disable_microphone();
#endif
   audio_mixer_dacs_flush_direct(false);
   usb_stream_init(
     sample_freq
   , 2
   , 16
   , false
   );
#if (defined BOARD) && (BOARD==EVK1105) && (defined DEFAULT_DACS) && (DEFAULT_DACS==AUDIO_MIXER_DAC_AIC23B)
   // Enable microphone call back interrupts.
   device_audio_enable_microphone();
#endif
}
void audio_speaker_set_volume(void)
{
   while(!Is_usb_control_out_received())
   {
     TASKS_SCHEDULE();
   }
   Usb_reset_endpoint_fifo_access(EP_CONTROL);
   LSB(s16_usb_speaker_volume)=Usb_read_endpoint_data(EP_CONTROL, 8);
   MSB(s16_usb_speaker_volume)=Usb_read_endpoint_data(EP_CONTROL, 8);

   // Convert USB range volume into the volume range of the application.
   s16_speaker_volume = volume_usb_to_appli(s16_usb_speaker_volume) ;

   Usb_ack_control_out_received_free();
   Usb_ack_control_in_ready_send();  // send a ZLP

   // Ensures good limits of the parameter
   s16_speaker_volume = min( s16_speaker_volume, SPEAKER_VOL_MAX);
   s16_speaker_volume = max( s16_speaker_volume, SPEAKER_VOL_MIN);
   audio_mixer_dacs_set_volume_direct(s16_speaker_volume);
   while (!Is_usb_control_in_ready())
   {
     TASKS_SCHEDULE();
   }
}
void audio_speaker_get_vol_cur(void)
{
   Usb_reset_endpoint_fifo_access(EP_CONTROL);
   Usb_write_endpoint_data(EP_CONTROL, 16, Usb_format_mcu_to_usb_data(16, s16_usb_speaker_volume));
   Usb_ack_control_in_ready_send();
   while(!Is_usb_control_out_received()) // send a ZLP
   {
     TASKS_SCHEDULE();
   }
   Usb_ack_control_out_received_free();
}
void audio_speaker_get_vol_min(void)
{
   Usb_reset_endpoint_fifo_access(EP_CONTROL);
   Usb_write_endpoint_data(EP_CONTROL, 16, Usb_format_mcu_to_usb_data(16, USB_SPEAKER_VOL_MIN));
   Usb_ack_control_in_ready_send();
   while(!Is_usb_control_out_received()) // send a ZLP
   {
     TASKS_SCHEDULE();
   }
   Usb_ack_control_out_received_free();
}
void audio_speaker_get_vol_max(void)
{
   Usb_reset_endpoint_fifo_access(EP_CONTROL);
   Usb_write_endpoint_data(EP_CONTROL, 16, Usb_format_mcu_to_usb_data(16, USB_SPEAKER_VOL_MAX));
   Usb_ack_control_in_ready_send();
   while(!Is_usb_control_out_received()) // send a ZLP
   {
     TASKS_SCHEDULE();
   }
   Usb_ack_control_out_received_free();
}
void audio_speaker_get_vol_res(void)
{
   Usb_reset_endpoint_fifo_access(EP_CONTROL);
   Usb_write_endpoint_data(EP_CONTROL, 16, Usb_format_mcu_to_usb_data(16, USB_SPEAKER_VOL_RES));
   Usb_ack_control_in_ready_send();
   while(!Is_usb_control_out_received()) // send a ZLP
   {
     TASKS_SCHEDULE();
   }
   Usb_ack_control_out_received_free();
}


//******* Micro control
void audio_micro_set_mute(void)
{
   while(!Is_usb_control_out_received())
   {
     TASKS_SCHEDULE();
   }
   Usb_reset_endpoint_fifo_access(EP_CONTROL);
   b_micro_mute=Usb_read_endpoint_data(EP_CONTROL, 8);
   Usb_ack_control_out_received_free();
   Usb_ack_control_in_ready_send();  // send a ZLP
   while (!Is_usb_control_in_ready())
   {
     TASKS_SCHEDULE();
   }
}
void audio_micro_get_mute(void)
{
   Usb_reset_endpoint_fifo_access(EP_CONTROL);
   Usb_write_endpoint_data(EP_CONTROL, 8, b_micro_mute);
   Usb_ack_control_in_ready_send();
   while(!Is_usb_control_out_received()) // send a ZLP
   {
     TASKS_SCHEDULE();
   }
   Usb_ack_control_out_received_free();
}
void audio_micro_set_volume(void)
{
   while(!Is_usb_control_out_received())
   {
     TASKS_SCHEDULE();
   }
   Usb_reset_endpoint_fifo_access(EP_CONTROL);
   LSB(s16_micro_volume)=Usb_read_endpoint_data(EP_CONTROL, 8);
   MSB(s16_micro_volume)=Usb_read_endpoint_data(EP_CONTROL, 8);
   Usb_ack_control_out_received_free();
   Usb_ack_control_in_ready_send();  // send a ZLP
   while (!Is_usb_control_in_ready())
   {
     TASKS_SCHEDULE();
   }
}
void audio_micro_get_vol_cur(void)
{
   Usb_reset_endpoint_fifo_access(EP_CONTROL);
   Usb_write_endpoint_data(EP_CONTROL, 8, LSB(s16_micro_volume));
   Usb_write_endpoint_data(EP_CONTROL, 8, MSB(s16_micro_volume));
   Usb_ack_control_in_ready_send();
   while(!Is_usb_control_out_received()) // send a ZLP
   {
     TASKS_SCHEDULE();
   }
   Usb_ack_control_out_received_free();
}
void audio_micro_get_vol_min(void)
{
   Usb_reset_endpoint_fifo_access(EP_CONTROL);
   Usb_write_endpoint_data(EP_CONTROL, 16, Usb_format_mcu_to_usb_data(16, MICRO_VOL_MIN));
   Usb_ack_control_in_ready_send();
   while(!Is_usb_control_out_received()) // send a ZLP
   {
     TASKS_SCHEDULE();
   }
   Usb_ack_control_out_received_free();
}
void audio_micro_get_vol_max(void)
{
   Usb_reset_endpoint_fifo_access(EP_CONTROL);
   Usb_write_endpoint_data(EP_CONTROL, 16, Usb_format_mcu_to_usb_data(16, MICRO_VOL_MAX));
   Usb_ack_control_in_ready_send();
   while(!Is_usb_control_out_received()) // send a ZLP
   {
     TASKS_SCHEDULE();
   }
   Usb_ack_control_out_received_free();
}
void audio_micro_get_vol_res(void)
{
   Usb_reset_endpoint_fifo_access(EP_CONTROL);
   Usb_write_endpoint_data(EP_CONTROL, 16, Usb_format_mcu_to_usb_data(16, MICRO_VOL_RES));
   Usb_ack_control_in_ready_send();
   while(!Is_usb_control_out_received()) // send a ZLP
   {
     TASKS_SCHEDULE();
   }
   Usb_ack_control_out_received_free();
}


//! @brief This function manages hit set report request.
//!
static void usb_hid_set_report_ouput (void)
{
   //Usb_ack_receive_setup();
   Usb_ack_setup_received_free();
   //Usb_send_control_in();
   Usb_ack_control_in_ready_send();

   //while(!Is_usb_receive_out());
   while (!Is_usb_control_out_received());
   //Usb_ack_receive_out();
   Usb_ack_control_out_received_free();
   //Usb_send_control_in();
   Usb_ack_control_in_ready_send();
}


//! @brief This function manages hid set idle request.
//!
//! @param Duration     When the upper byte of wValue is 0 (zero), the duration is indefinite else from 0.004 to 1.020 seconds
//! @param Report ID    0 the idle rate applies to all input reports, else only applies to the Report ID
//!
uint8_t g_u8_report_rate=0;
static void usb_hid_set_idle (uint8_t u8_report_id, uint8_t u8_duration )
{
   uint16_t wInterface;

   // Get interface number to put in idle mode
   wInterface=usb_format_usb_to_mcu_data(16, Usb_read_endpoint_data(EP_CONTROL, 16));
   Usb_ack_setup_received_free();

   if( wInterface == INTERFACE_NB_KBD )
     g_u8_report_rate = u8_duration;

   Usb_ack_control_in_ready_send();
   while (!Is_usb_control_in_ready());
}


//! @brief This function manages hid get idle request.
//!
//! @param u8_report_id    0 the idle rate applies to all input reports, else only applies to the Report ID
//!
static void usb_hid_get_idle (uint8_t u8_report_id)
{
   uint16_t wLength;
   uint16_t wInterface;

   // Get interface number to put in idle mode
   wInterface=usb_format_usb_to_mcu_data(16, Usb_read_endpoint_data(EP_CONTROL, 16));
   wLength   =usb_format_usb_to_mcu_data(16, Usb_read_endpoint_data(EP_CONTROL, 16));
   Usb_ack_setup_received_free();

   if( (wLength != 0) && (wInterface == INTERFACE_NB_KBD) )
   {
      Usb_reset_endpoint_fifo_access(EP_CONTROL);
      Usb_write_endpoint_data(EP_CONTROL, 8, g_u8_report_rate);
      Usb_ack_control_in_ready_send();
   }

   while (!Is_usb_control_out_received());
   Usb_ack_control_out_received_free();
}


//! This function manages the HID Get_Descriptor request.
//!
static void hid_get_descriptor(uint8_t size_of_report, const uint8_t* p_usb_hid_report)
{
  bool  zlp;
  uint16_t   wIndex;
  uint16_t   wLength;

  zlp = false;                                              /* no zero length packet */

  data_to_transfer = size_of_report;
  pbuffer          = p_usb_hid_report;

  wIndex = Usb_read_endpoint_data(EP_CONTROL, 16);
  wIndex = usb_format_usb_to_mcu_data(16, wIndex);
  wLength = Usb_read_endpoint_data(EP_CONTROL, 16);
  wLength = usb_format_usb_to_mcu_data(16, wLength);
  Usb_ack_setup_received_free();                          //!< clear the setup received flag

  if (wLength > data_to_transfer)
  {
    zlp = !(data_to_transfer % EP_CONTROL_LENGTH);  //!< zero length packet condition
  }
  else
  {
    data_to_transfer = wLength; //!< send only requested number of data bytes
  }

  Usb_ack_nak_out(EP_CONTROL);

  while (data_to_transfer && (!Is_usb_nak_out(EP_CONTROL)))
  {
    while( !Is_usb_control_in_ready() && !Is_usb_nak_out(EP_CONTROL) );

    if( Is_usb_nak_out(EP_CONTROL) )
       break;    // don't clear the flag now, it will be cleared after

    Usb_reset_endpoint_fifo_access(EP_CONTROL);
    data_to_transfer = usb_write_ep_txpacket(EP_CONTROL, pbuffer,
                                             data_to_transfer, &pbuffer);
    if( Is_usb_nak_out(EP_CONTROL) )
       break;
    else
       Usb_ack_control_in_ready_send();  //!< Send data until necessary
  }

  if ( zlp && (!Is_usb_nak_out(EP_CONTROL)) )
  {
    while (!Is_usb_control_in_ready());
    Usb_ack_control_in_ready_send();
  }

  while (!(Is_usb_nak_out(EP_CONTROL)));
  Usb_ack_nak_out(EP_CONTROL);
  while (!Is_usb_control_out_received());
  Usb_ack_control_out_received_free();
}

//! This function converts the volume from the USB world to the application world.
//! [USB_SPEAKER_VOL_MIN; USB_SPEAKER_VOL_MAX] => [SPEAKER_VOL_MIN; SPEAKER_VOL_MAX]
//!
static int16_t  volume_usb_to_appli(int16_t usb_volume)
{
  int16_t volume;
  if( usb_volume==-32768 )
  {
    volume=SPEAKER_VOL_MIN;
  }
  else
  {
    volume= (((int32_t)usb_volume-USB_SPEAKER_VOL_MIN)*(SPEAKER_VOL_MAX-SPEAKER_VOL_MIN)/(USB_SPEAKER_VOL_MAX-USB_SPEAKER_VOL_MIN))+SPEAKER_VOL_MIN;
  }

  return volume;
}

#endif  // USB_DEVICE_FEATURE == true
