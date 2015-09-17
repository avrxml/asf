/**************************************************************************
 *
 * \file
 *
 * \brief USB identifiers.
 *
 * This file contains the USB parameters that uniquely identify the USB
 * application through descriptor tables.
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


#if USB_DEVICE_FEATURE == true

#include "usb_drv.h"
#include "usb_descriptors.h"
#include "usb_standard_request.h"
#include "usb_specific_request.h"
#include "hid.h"
#include "audio_example.h"


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

// usb_user_device_descriptor
const S_usb_device_descriptor usb_dev_desc =
{
  sizeof(S_usb_device_descriptor),
  DEVICE_DESCRIPTOR,
  Usb_format_mcu_to_usb_data(16, USB_SPECIFICATION),
  DEVICE_CLASS,
  DEVICE_SUB_CLASS,
  DEVICE_PROTOCOL,
  EP_CONTROL_LENGTH,
  Usb_format_mcu_to_usb_data(16, VENDOR_ID),
  Usb_format_mcu_to_usb_data(16, PRODUCT_ID),
  Usb_format_mcu_to_usb_data(16, RELEASE_NUMBER),
  MAN_INDEX,
  PROD_INDEX,
  SN_INDEX,
  NB_CONFIGURATION
};


// usb_user_configuration_descriptor FS
const S_usb_user_configuration_descriptor usb_conf_desc =
{
 { sizeof(S_usb_configuration_descriptor)
 , CONFIGURATION_DESCRIPTOR
 , Usb_format_mcu_to_usb_data(16, sizeof(S_usb_user_configuration_descriptor))
 , NB_INTERFACE
 , CONF_NB
 , CONF_INDEX
 , CONF_ATTRIBUTES
 , MAX_POWER
 }
 ,

 //** The first interface is the Control Audio Interface
 // The AudioControl (AC) interface descriptors contain all relevant information to fully characterize the corresponding audio function.
 { sizeof(S_usb_interface_descriptor)
 , INTERFACE_DESCRIPTOR
 , AC_INTERFACE_NB
 , AC_ALTERNATE
 , AC_NB_ENDPOINT
 , AC_INTERFACE_CLASS
 , AC_INTERFACE_SUB_CLASS
 , AC_INTERFACE_PROTOCOL
 , AC_INTERFACE_INDEX
 }
 ,
 // Class-Specific AC Interface Descriptor
 { sizeof(S_usb_ac_interface_descriptor)
 ,DESCRIPTOR_AUDIO_INTERFACE
 ,DESCRIPTOR_SUBTYPE_AUDIO_AC_HEADER
 ,Usb_format_mcu_to_usb_data(16, AUDIO_CLASS_REVISION)
 ,Usb_format_mcu_to_usb_data(16, sizeof(S_usb_ac_interface_descriptor)\
                                   +sizeof(S_usb_in_ter_descriptor)
                                   +sizeof(S_usb_feature_unit_descriptor_speaker)
                                   +sizeof(S_usb_out_ter_descriptor)\
                                   +sizeof(S_usb_in_ter_descriptor)
                                   +sizeof(S_usb_feature_unit_descriptor_micro)
                                   +sizeof(S_usb_out_ter_descriptor))
 ,NB_OF_STREAMING_INTERFACE
 ,STD_AS_IN_INTERFACE_NB
 ,STD_AS_OUT_INTERFACE_NB
 }
 ,

 //* Descriptors for speaker
 // Class-Specific Input Terminal Descriptor
 { sizeof(S_usb_in_ter_descriptor)
 ,DESCRIPTOR_AUDIO_INTERFACE
 ,DESCRIPTOR_SUBTYPE_AUDIO_AC_INPUT_TERMINAL
 ,SPEAKER_INPUT_TERMINAL_ID
 ,Usb_format_mcu_to_usb_data(16, SPEAKER_INPUT_TERMINAL_TYPE)
 ,SPEAKER_INPUT_TERMINAL_ASSOCIATION
 ,SPEAKER_INPUT_TERMINAL_NB_CHANNELS
 ,SPEAKER_INPUT_TERMINAL_CHANNEL_CONF
 ,SPEAKER_INPUT_TERMINAL_CH_NAME_ID
 ,SPEAKER_INPUT_TERMINAL_NAME_ID
 }
 ,
 // Class-Specific Feature Unit Descriptor
 { sizeof(S_usb_feature_unit_descriptor_speaker)
 ,DESCRIPTOR_AUDIO_INTERFACE
 ,DESCRIPTOR_SUBTYPE_AUDIO_AC_FEATURE_UNIT
 ,SPEAKER_FEATURE_UNIT_ID
 ,SPEAKER_FEATURE_UNIT_SOURCE_ID
 ,0x01
 ,SPEAKER_FEATURE_UNIT_CTRL_CH_MASTER
 ,SPEAKER_FEATURE_UNIT_CTRL_CH_1
 }
 ,
 // Class-Specific Output Terminal Descriptor
 { sizeof(S_usb_out_ter_descriptor)
 ,DESCRIPTOR_AUDIO_INTERFACE
 ,DESCRIPTOR_SUBTYPE_AUDIO_AC_OUTPUT_TERMINAL
 ,SPEAKER_OUTPUT_TERMINAL_ID
 ,Usb_format_mcu_to_usb_data(16, SPEAKER_OUTPUT_TERMINAL_TYPE)
 ,SPEAKER_OUTPUT_TERMINAL_ASSOCIATION
 ,SPEAKER_OUTPUT_TERMINAL_SOURCE_ID
 ,SPEAKER_OUTPUT_TERMINAL_NAME_ID
 }
 ,
 //* Descriptors for micro
 // Class-Specific Input Terminal Descriptor
 { sizeof(S_usb_in_ter_descriptor)
 ,DESCRIPTOR_AUDIO_INTERFACE
 ,DESCRIPTOR_SUBTYPE_AUDIO_AC_INPUT_TERMINAL
 ,MICRO_INPUT_TERMINAL_ID
 ,Usb_format_mcu_to_usb_data(16, MICRO_INPUT_TERMINAL_TYPE)
 ,MICRO_INPUT_TERMINAL_ASSOCIATION
 ,MICRO_INPUT_TERMINAL_NB_CHANNELS
 ,MICRO_INPUT_TERMINAL_CHANNEL_CONF
 ,MICRO_INPUT_TERMINAL_CH_NAME_ID
 ,MICRO_INPUT_TERMINAL_NAME_ID
 }
 ,
 // Class-Specific Feature Unit Descriptor
 { sizeof(S_usb_feature_unit_descriptor_micro)
 ,DESCRIPTOR_AUDIO_INTERFACE
 ,DESCRIPTOR_SUBTYPE_AUDIO_AC_FEATURE_UNIT
 ,MICRO_FEATURE_UNIT_ID
 ,MICRO_FEATURE_UNIT_SOURCE_ID
 ,0x01
 ,MICRO_FEATURE_UNIT_CTRL_CH_MASTER
 ,MICRO_FEATURE_UNIT_CTRL_CH_1
 }
 ,
 // Class-Specific Output Terminal Descriptor
 { sizeof(S_usb_out_ter_descriptor)
 ,DESCRIPTOR_AUDIO_INTERFACE
 ,DESCRIPTOR_SUBTYPE_AUDIO_AC_OUTPUT_TERMINAL
 ,MICRO_OUTPUT_TERMINAL_ID
 ,Usb_format_mcu_to_usb_data(16, MICRO_OUTPUT_TERMINAL_TYPE)
 ,MICRO_OUTPUT_TERMINAL_ASSOCIATION
 ,MICRO_OUTPUT_TERMINAL_SOURCE_ID
 ,MICRO_OUTPUT_TERMINAL_NAME_ID
 }
 ,

 //** The second interface is the Audio Stream Interface for Speaker (with two alternate interface)
 // Standard AS Interface Descriptor (Alternate 0 without endpoint)
 { sizeof(S_usb_interface_descriptor)
 ,INTERFACE_DESCRIPTOR
 ,STD_AS_OUT_INTERFACE_NB
 ,ALT0_AS_OUT_INTERFACE_INDEX
 ,ALT0_AS_OUT_NB_ENDPOINT
 ,ALT0_AS_OUT_INTERFACE_CLASS
 ,ALT0_AS_OUT_INTERFACE_SUB_CLASS
 ,ALT0_AS_OUT_INTERFACE_PROTOCOL
 ,0x00
 }
 ,
 // Standard AS Interface Descriptor (Alternate 1 with 1 endpoint)
 { sizeof(S_usb_interface_descriptor)
 ,INTERFACE_DESCRIPTOR
 ,STD_AS_OUT_INTERFACE_NB
 ,ALT1_AS_OUT_INTERFACE_INDEX
 ,ALT1_AS_OUT_NB_ENDPOINT
 ,ALT1_AS_OUT_INTERFACE_CLASS
 ,ALT1_AS_OUT_INTERFACE_SUB_CLASS
 ,ALT1_AS_OUT_INTERFACE_PROTOCOL
 ,0x00
 }
 ,
 // Class-Specific AS Interface Descriptor
 { sizeof(S_usb_as_interface_descriptor)
 ,DESCRIPTOR_AUDIO_INTERFACE
 ,DESCRIPTOR_SUBTYPE_AUDIO_AS_GENERAL
 ,AS_OUT_TERMINAL_LINK
 ,AS_OUT_DELAY
 ,Usb_format_mcu_to_usb_data(16, AS_OUT_FORMAT_TAG)
 }
 ,
 { sizeof(S_usb_format_type_speaker)
 ,DESCRIPTOR_AUDIO_INTERFACE
 ,DESCRIPTOR_SUBTYPE_AUDIO_AS_FORMAT_TYPE
 ,OUT_FORMAT_TYPE
 ,OUT_FORMAT_NB_CHANNELS
 ,OUT_FORMAT_FRAME_SIZE
 ,OUT_FORMAT_BIT_RESOLUTION
 ,OUT_FORMAT_SAMPLE_FREQ_NB
#if (defined USB_RESYNC_METHOD)
  #if (USB_RESYNC_METHOD == USB_RESYNC_METHOD_SOFT_ADD_DEL_SAMPLES)
 ,Usb_format_mcu_to_usb_data(16, SPEAKER_FREQUENCY)
 ,OUT_FORMAT_MSBYTE_SAMPLE_FREQ
  #elif ((USB_RESYNC_METHOD == USB_RESYNC_METHOD_SOFT_ADAPTIF_SRC) || (USB_RESYNC_METHOD == USB_RESYNC_METHOD_EXT_CLOCK_SYNTHESIZER))
 ,Usb_format_mcu_to_usb_data(16, 48000)
 ,OUT_FORMAT_MSBYTE_SAMPLE_FREQ
 ,Usb_format_mcu_to_usb_data(16, 44100)
 ,OUT_FORMAT_MSBYTE_SAMPLE_FREQ
 ,Usb_format_mcu_to_usb_data(16, 32000)
 ,OUT_FORMAT_MSBYTE_SAMPLE_FREQ
  #else
  # error Unknown configuration
  #endif
#else
#error USB_RESYNC_METHOD in not known
#endif
 }
 ,
 { sizeof(S_usb_endpoint_audio_descriptor)
 , ENDPOINT_DESCRIPTOR
 , ENDPOINT_NB_OUT
 , EP_ATTRIBUTES_OUT
 , Usb_format_mcu_to_usb_data(16, EP_SIZE_OUT)
 , EP_INTERVAL_OUT
 , 0x00
 , 0x00
 }
 ,
 { sizeof(S_usb_endpoint_audio_specific)
 ,DESCRIPTOR_AUDIO_ENDPOINT
 ,DESCRIPTOR_SUBTYPE_AUDIO_ENDP_GENERAL
 ,AUDIO_EP_OUT_ATRIBUTES
 ,AUDIO_EP_OUT_DELAY_UNIT
 ,Usb_format_mcu_to_usb_data(16, AUDIO_EP_OUT_LOCK_DELAY)
 }
 ,

 //** The third interface is the Audio Stream Interface for Micro (with two alternate interface)
 // Standard AS Interface Descriptor (Alternate 0 without endpoint)
 { sizeof(S_usb_interface_descriptor)
 ,INTERFACE_DESCRIPTOR
 ,STD_AS_IN_INTERFACE_NB
 ,ALT0_AS_IN_INTERFACE_INDEX
 ,ALT0_AS_IN_NB_ENDPOINT
 ,ALT0_AS_IN_INTERFACE_CLASS
 ,ALT0_AS_IN_INTERFACE_SUB_CLASS
 ,ALT0_AS_IN_INTERFACE_PROTOCOL
 ,0x00
 }
 ,
 // Standard AS Interface Descriptor (Alternate 1 with 1 endpoint)
 { sizeof(S_usb_interface_descriptor)
 ,INTERFACE_DESCRIPTOR
 ,STD_AS_IN_INTERFACE_NB
 ,ALT1_AS_IN_INTERFACE_INDEX
 ,ALT1_AS_IN_NB_ENDPOINT
 ,ALT1_AS_IN_INTERFACE_CLASS
 ,ALT1_AS_IN_INTERFACE_SUB_CLASS
 ,ALT1_AS_IN_INTERFACE_PROTOCOL
 ,0x00
 }
 ,
 // Class-Specific AS Interface Descriptor
 { sizeof(S_usb_as_interface_descriptor)
 ,DESCRIPTOR_AUDIO_INTERFACE
 ,DESCRIPTOR_SUBTYPE_AUDIO_AS_GENERAL
 ,AS_IN_TERMINAL_LINK
 ,AS_IN_DELAY
 ,Usb_format_mcu_to_usb_data(16, AS_IN_FORMAT_TAG)
 }
 ,
 { sizeof(S_usb_format_type_microphone)
 ,DESCRIPTOR_AUDIO_INTERFACE
 ,DESCRIPTOR_SUBTYPE_AUDIO_AS_FORMAT_TYPE
 ,IN_FORMAT_TYPE
 ,IN_FORMAT_NB_CHANNELS
 ,IN_FORMAT_FRAME_SIZE
 ,IN_FORMAT_BIT_RESOLUTION
 ,IN_FORMAT_SAMPLE_FREQ_NB
 ,Usb_format_mcu_to_usb_data(16, IN_FORMAT_LSBYTE_SAMPLE_FREQ)
 ,IN_FORMAT_MSBYTE_SAMPLE_FREQ
 }
 ,
 { sizeof(S_usb_endpoint_audio_descriptor)
 , ENDPOINT_DESCRIPTOR
 , ENDPOINT_NB_IN
 , EP_ATTRIBUTES_IN
 , Usb_format_mcu_to_usb_data(16, EP_SIZE_IN)
 , EP_INTERVAL_IN
 , 0x00
 , 0x00
 }
 ,
 { sizeof(S_usb_endpoint_audio_specific)
 , DESCRIPTOR_AUDIO_ENDPOINT
 , DESCRIPTOR_SUBTYPE_AUDIO_ENDP_GENERAL
 , AUDIO_EP_IN_ATRIBUTES
 , AUDIO_EP_IN_DELAY_UNIT
 , Usb_format_mcu_to_usb_data(16, AUDIO_EP_IN_LOCK_DELAY)
 }

 // HID Keyboard interface
 ,
 { sizeof(S_usb_interface_descriptor)
 , INTERFACE_DESCRIPTOR
 , INTERFACE_NB_KBD
 , ALTERNATE_KBD
 , NB_ENDPOINT_KBD
 , INTERFACE_CLASS_KBD
 , INTERFACE_SUB_CLASS_KBD
 , INTERFACE_PROTOCOL_KBD
 , INTERFACE_INDEX_KBD
 }
 ,
 { sizeof(S_usb_hid_descriptor)
 , HID_DESCRIPTOR
 , HID_BDC
 , HID_COUNTRY_CODE_NOT_SUPPORTED
 , HID_CLASS_DESC_NB_DEFAULT
 , HID_REPORT_DESCRIPTOR
 , Usb_format_mcu_to_usb_data(16, sizeof(usb_hid_report_descriptor_kbd))
 }
 ,
 { sizeof(S_usb_endpoint_descriptor)
 , ENDPOINT_DESCRIPTOR
 , ENDPOINT_NB_KBD
 , EP_ATTRIBUTES_KBD
 , Usb_format_mcu_to_usb_data(16, (EP_SIZE_KBD))
 , EP_INTERVAL_KBD
 }
};


                                      // usb_user_language_id
const S_usb_language_id usb_user_language_id =
{
  sizeof(S_usb_language_id),
  STRING_DESCRIPTOR,
  Usb_format_mcu_to_usb_data(16, LANGUAGE_ID)
};


                                      // usb_user_manufacturer_string_descriptor
const S_usb_manufacturer_string_descriptor usb_user_manufacturer_string_descriptor =
{
  sizeof(S_usb_manufacturer_string_descriptor),
  STRING_DESCRIPTOR,
  USB_MANUFACTURER_NAME
};


                                      // usb_user_product_string_descriptor
const S_usb_product_string_descriptor usb_user_product_string_descriptor =
{
  sizeof(S_usb_product_string_descriptor),
  STRING_DESCRIPTOR,
  USB_PRODUCT_NAME
};


                                      // usb_user_serial_number
const S_usb_serial_number usb_user_serial_number =
{
  sizeof(S_usb_serial_number),
  STRING_DESCRIPTOR,
  USB_SERIAL_NUMBER
};


const uint8_t usb_hid_report_descriptor_kbd[USB_HID_REPORT_DESC_KBD] =
{
    0x05, 0x0c,                    // USAGE_PAGE (Consumer Devices)
    0x09, 0x01,                    // USAGE (Consumer Control)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x09, 0xE9,                    //   USAGE (Volume Up)
    0x09, 0xEA,                    //   USAGE (Volume Down)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x02,                    //   REPORT_COUNT (2)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs,Null)
    0x09, 0xCD,                    //   USAGE (Play/Pause)
    0x09, 0xB5,                    //   USAGE (Scan Next Track)
    0x09, 0xB6,                    //   USAGE (Scan Previous Track)
    0x09, 0xB3,                    //   USAGE (Fast Forward)
    0x09, 0xB4,                    //   USAGE (Rewind)
    0x95, 0x05,                    //   REPORT_COUNT (5)
    0x81, 0x06,                    //   INPUT (Data,Var,Abs,Null)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x81, 0x01,                    //   INPUT (Cnst,Ary,Abs)
    0xc0                           // END_COLLECTION
};


#endif  // USB_DEVICE_FEATURE == true
