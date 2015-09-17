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

#include "conf_usb.h"


#if USB_HOST_FEATURE == true

#include <stdio.h>
#include "conf_usb.h"
#include "usb_host_enum.h"
#include "usb_host_user.h"
#include "usb_audio.h"
#include "host_audio_task.h"


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________


//_____ P R I V A T E   D E C L A R A T I O N S ____________________________

uint8_t   state=0;

uint8_t n_interface=0;

//_____ D E C L A R A T I O N S ____________________________________________

//! Initialization
//!
void host_user_check_class_init(void)
{
   state=0;
   n_interface=0;
}


//! This function is called by the standard USB host_check_class() function when
//! the Host is analyzing the configuration descriptor previously sent by a USB Device.
//! The function will be called for each descriptor found in the configuration descriptor.
//!
bool host_user_check_class(const uint8_t* descriptor)
{
   uint32_t num;
   const uint8_t* p_bmaControls;
   uint8_t  n_sample_freq=0;
   uint8_t  i;

   if (!n_interface)
   {
      free(host_audio_task_data);
      host_audio_task_data = NULL;
   }

   switch (descriptor[OFFSET_FIELD_DESCRIPTOR_TYPE])
   {
   case INTERFACE_DESCRIPTOR:
      if (n_interface < MAX_INTERFACE_SUPPORTED)
         n_interface++;
      break;

   case CS_INTERFACE:
      if (!host_audio_task_data)
         host_audio_task_data = calloc(1, sizeof(*host_audio_task_data));;

      switch (descriptor[OFFSET_FIELD_DESCRIPTOR_SUBTYPE])
      {
      case HEADER_SUB_TYPE:
         // This variable allow to detect AUDIO CONTROL sub class to AUDIO STREAMING
         // sub class transition.
         state++;
         break;

      case FEATURE_UNIT_SUB_TYPE:
         if( state!=1 )
            break;

         // Still in AUDIO CONTROL sub class
         // Analysis of the Features Units

         // Ensure no out-of-limit access in array.
         num = (descriptor[0]-7)>MAX_BMA_CONTROLS ? MAX_BMA_CONTROLS : descriptor[0]-7;

         host_audio_task_data->g_cs_feature[host_audio_task_data->g_cs_num_features_unit].n_bmaControls = num;
         host_audio_task_data->g_cs_feature[host_audio_task_data->g_cs_num_features_unit].unit          = descriptor[3];

         p_bmaControls = &descriptor[6]; // Point on beginning of bmaControls infos
         for( i=0 ; i<num ; i++,p_bmaControls++ )
         {
            host_audio_task_data->g_cs_feature[host_audio_task_data->g_cs_num_features_unit].bmaControls[i]=p_bmaControls[0];
         }
         host_audio_task_data->g_cs_num_features_unit++;

      case FORMAT_SUB_TYPE:
         if( state<2 )
            break;

         // We are in AUDIO STREAMING sub class
         // Analysis of the sampling frequencies

         const uint8_t* p_sampling_freq= &descriptor[8];
               uint8_t  id_interface=Get_interface_number(n_interface-1);

         host_audio_task_data->g_n_channels[id_interface]        = descriptor[4];
         host_audio_task_data->g_n_bits_per_sample[id_interface] = descriptor[6];
         n_sample_freq                     = descriptor[7];
         host_audio_task_data->g_sample_freq[id_interface]=0;
         for( i=0 ; i<n_sample_freq ; i++, p_sampling_freq+=3 )
         {
            // Keep the max declared sampling frequency. An other policy would be to keep the
            // highest sampling frequency which is compatible with the CPU and PBA frequency
            host_audio_task_data->g_sample_freq[id_interface]= max(
               host_audio_task_data->g_sample_freq[id_interface]
            ,  p_sampling_freq[0] + (p_sampling_freq[1]<<8) + (p_sampling_freq[2]<<16));
         }

         break;
      }
      break;

   default:
      break;
   }
   return true;
}

#endif  // USB_HOST_FEATURE == true
