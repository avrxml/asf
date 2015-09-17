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

#ifndef _USB_DESCRIPTORS_H_
#define _USB_DESCRIPTORS_H_

//_____ I N C L U D E S ____________________________________________________

#include "conf_usb.h"
#include "conf_audio_player.h"

#if USB_DEVICE_FEATURE == false
  #error usb_descriptors.h is #included although USB_DEVICE_FEATURE is disabled
#endif


#include "usb_standard_request.h"
#include "usb_task.h"
#include "usb_audio.h"


//_____ M A C R O S ________________________________________________________

#define Usb_unicode(c)                    (Usb_format_mcu_to_usb_data(16, (uint16_t)(c)))
#define Usb_get_dev_desc_pointer()        (&(usb_dev_desc.bLength))
#define Usb_get_dev_desc_length()         (sizeof (usb_dev_desc))
#define Usb_get_conf_desc_pointer()       (&(usb_conf_desc.cfg.bLength))
#define Usb_get_conf_desc_length()        (sizeof (usb_conf_desc))


//_____ U S B    D E F I N E S _____________________________________________

                  // USB Device descriptor
#define  USB_SPECIFICATION             0x0200
#define  DEVICE_CLASS                  0                   //! Each configuration has its own class
#define  DEVICE_SUB_CLASS              0                   //! Each configuration has its own subclass
#define  DEVICE_PROTOCOL               0                   //! Each configuration has its own protocol
#define  EP_CONTROL_LENGTH             64
#define  VENDOR_ID                     ATMEL_VID           //! Atmel vendor ID
#define  PRODUCT_ID                    AUDIO_SPEAKER_MICRO_EXAMPLE_PID
#define  RELEASE_NUMBER                0x1000
#define  MAN_INDEX                     0x01
#define  PROD_INDEX                    0x02
#define  SN_INDEX                      0x03
#define  NB_CONFIGURATION              1


               // USB mic configuration descriptor
#define  NB_INTERFACE                  4     //! The number of interfaces for this configuration: Audio Control + Audio Stream OUT (Speaker) + Audio Stream IN (Micro) interface
#define  CONF_NB                       1     //! Number of this configuration
#define  CONF_INDEX                    0
#define  CONF_ATTRIBUTES               USB_CONFIG_BUSPOWERED
#define  MAX_POWER                     50       // 100 mA

              // Standard Audio control (AC) interface descriptor
#define  AC_INTERFACE_NB               0
#define  AC_ALTERNATE                  0
#define  AC_NB_ENDPOINT                0        //! No endpoint for AC interface
#define  AC_INTERFACE_CLASS            AUDIO_CLASS   //! Audio Class
#define  AC_INTERFACE_SUB_CLASS        AUDIOCONTROL_SUBCLASS //! Audio_control sub class
#define  AC_INTERFACE_PROTOCOL         NO_PROTOCOL
#define  AC_INTERFACE_INDEX            0

              // USB Endpoint 1 for Audio stream Speaker
#define  ENDPOINT_NB_OUT               (EP_AUDIO_OUT | USB_ENDPOINT_OUT)
#define  EP_ATTRIBUTES_OUT             (TYPE_ISOCHRONOUS | ( ENDPOINT_TYPE_ADAPTIVE << ENDPOINT_TYPE_OFFSET))
#define  EP_SIZE_OUT                   (256)
#define  EP_INTERVAL_OUT               0x01     // One packet per frame

              // USB Endpoint 2 for Audio stream Microphone
#define  ENDPOINT_NB_IN                (EP_AUDIO_IN | USB_ENDPOINT_IN)
#define  EP_ATTRIBUTES_IN              TYPE_ISOCHRONOUS
#define  EP_SIZE_IN                    (128)
#define  EP_INTERVAL_IN                0x01     // One packet per frame


//** AC interface descriptor Audio specific
#define  NB_OF_STREAMING_INTERFACE     0x02

             //* AC interface descriptor for speaker
             // Input Terminal descriptor
#define  SPEAKER_INPUT_TERMINAL_ID           0x01
#define  SPEAKER_INPUT_TERMINAL_TYPE         AUDIO_TE_TYPE_USB_STREAMING      // USB Streaming
#define  SPEAKER_INPUT_TERMINAL_ASSOCIATION  0x00                             // No association
#define  SPEAKER_INPUT_TERMINAL_NB_CHANNELS  0x01                             // 1 channel for input terminal
#define  SPEAKER_INPUT_TERMINAL_CHANNEL_CONF AUDIO_CHANNEL_NO_POSITION        // Mono sets no position
#define  SPEAKER_INPUT_TERMINAL_CH_NAME_ID   0x00                             // No channel name
#define  SPEAKER_INPUT_TERMINAL_NAME_ID      0x00                             // No terminal name
             // Feature Unit Control descriptor
#define  SPEAKER_FEATURE_UNIT_ID             0x02
#define  SPEAKER_FEATURE_UNIT_SOURCE_ID      SPEAKER_INPUT_TERMINAL_ID
#define  SPEAKER_FEATURE_UNIT_CTRL_CH_MASTER (AUDIO_FU_CONTROL_CS_MUTE|AUDIO_FU_CONTROL_CS_VOLUME) // Mute + Volume control on master channel
#define  SPEAKER_FEATURE_UNIT_CTRL_CH_1      0x00                             // No control on channel 1
             // Output Terminal descriptor
#define  SPEAKER_OUTPUT_TERMINAL_ID          0x03
#define  SPEAKER_OUTPUT_TERMINAL_TYPE        AUDIO_TE_TYPE_OUTPUT_SPEAKER     // Output terminal is a speaker
#define  SPEAKER_OUTPUT_TERMINAL_ASSOCIATION 0x00                             // No association
#define  SPEAKER_OUTPUT_TERMINAL_SOURCE_ID   SPEAKER_FEATURE_UNIT_ID          // From Feature Unit Terminal
#define  SPEAKER_OUTPUT_TERMINAL_NAME_ID     0x00                             // No terminal name

             //* AC interface descriptor for micro
             // Input Terminal descriptor
#define  MICRO_INPUT_TERMINAL_ID             0x04
#define  MICRO_INPUT_TERMINAL_TYPE           AUDIO_TE_TYPE_INPUT_MICROPHONE   // Terminal is microphone
#define  MICRO_INPUT_TERMINAL_ASSOCIATION    0x00                             // No association
#define  MICRO_INPUT_TERMINAL_NB_CHANNELS    0x01                             // One channel for input terminal
#define  MICRO_INPUT_TERMINAL_CHANNEL_CONF   AUDIO_CHANNEL_NO_POSITION        // Mono sets no position
#define  MICRO_INPUT_TERMINAL_CH_NAME_ID     0x00                             // No channel name
#define  MICRO_INPUT_TERMINAL_NAME_ID        0x00                             // No terminal name
             // Feature Unit Control descriptor
#define  MICRO_FEATURE_UNIT_ID               0x05
#define  MICRO_FEATURE_UNIT_SOURCE_ID        MICRO_INPUT_TERMINAL_ID
#define  MICRO_FEATURE_UNIT_CTRL_CH_MASTER   (AUDIO_FU_CONTROL_CS_MUTE|AUDIO_FU_CONTROL_CS_VOLUME) // Mute + Volume control on master channel
#define  MICRO_FEATURE_UNIT_CTRL_CH_1        0x00                             // No control on channel 1
             // Output Terminal descriptor
#define  MICRO_OUTPUT_TERMINAL_ID            0x06
#define  MICRO_OUTPUT_TERMINAL_TYPE          AUDIO_TE_TYPE_USB_STREAMING      // USB Streaming
#define  MICRO_OUTPUT_TERMINAL_ASSOCIATION   0x00                             // No association
#define  MICRO_OUTPUT_TERMINAL_SOURCE_ID     MICRO_FEATURE_UNIT_ID            // From Feature Unit Terminal
#define  MICRO_OUTPUT_TERMINAL_NAME_ID       0x00                             // No terminal name


             // Audio Streaming (AS) interface descriptor for speaker
#define  STD_AS_OUT_INTERFACE_NB             0x01                             // Index of Std AS Interface
             //Alternate O Audio Streaming (AS) interface descriptor
#define  ALT0_AS_OUT_INTERFACE_INDEX         0x00                             // Index of Std AS interface Alt0
#define  ALT0_AS_OUT_NB_ENDPOINT             0x00                             // Nb of endpoints for alt0 interface
#define  ALT0_AS_OUT_INTERFACE_CLASS         AUDIO_CLASS                      // Audio class
#define  ALT0_AS_OUT_INTERFACE_SUB_CLASS     AUDIOSTREAMING_SUBCLASS          // Audio streaming sub class
#define  ALT0_AS_OUT_INTERFACE_PROTOCOL      NO_PROTOCOL                      // Unused
             //Alternate 1 Audio Streaming (AS) interface descriptor
#define  ALT1_AS_OUT_INTERFACE_INDEX         0x01                             // Index of Std AS interface Alt1
#define  ALT1_AS_OUT_NB_ENDPOINT             0x01                             // Nb of endpoints for alt1 interface
#define  ALT1_AS_OUT_INTERFACE_CLASS         AUDIO_CLASS                      // Audio class
#define  ALT1_AS_OUT_INTERFACE_SUB_CLASS     AUDIOSTREAMING_SUBCLASS          // Audio streaming sub class
#define  ALT1_AS_OUT_INTERFACE_PROTOCOL      NO_PROTOCOL                      // Unused
             //AS general Interface descriptor
#define  AS_OUT_TERMINAL_LINK                SPEAKER_INPUT_TERMINAL_ID        // Unit Id of the output terminal
#define  AS_OUT_DELAY                        0x01                             // Interface delay
#define  AS_OUT_FORMAT_TAG                   AUDIO_FORMAT_TI_PCM              // PCM Format
             // Format type
#define  OUT_FORMAT_TYPE                     AUDIO_FORMAT_TYPE_I              // Format Type I
#define  OUT_FORMAT_NB_CHANNELS              0x02                             // Two Channels
#define  OUT_FORMAT_FRAME_SIZE               0x02                             // Two bytes per audio sample
#define  OUT_FORMAT_BIT_RESOLUTION           0x10                             // 16 bits per sample

#if (defined USB_RESYNC_METHOD)
# if (USB_RESYNC_METHOD == USB_RESYNC_METHOD_SOFT_ADD_DEL_SAMPLES)
#   define  OUT_FORMAT_SAMPLE_FREQ_NB           0x01                          // One frequency supported
# elif ((USB_RESYNC_METHOD == USB_RESYNC_METHOD_SOFT_ADAPTIF_SRC) || (USB_RESYNC_METHOD == USB_RESYNC_METHOD_EXT_CLOCK_SYNTHESIZER) )
#   define  OUT_FORMAT_SAMPLE_FREQ_NB           0x03                          // Three frequencies supported
# else
#   error Unknown configuration
# endif
#else
# error USB_RESYNC_METHOD is not known
#endif

#define  OUT_FORMAT_MSBYTE_SAMPLE_FREQ       0x00                             // MsByte
             //Audio endpoint specific descriptor field
#define  AUDIO_EP_OUT_ATRIBUTES              0x01                             // Sampling freq control, no pitch, no padding
#define  AUDIO_EP_OUT_DELAY_UNIT             0x00                             // Unused
#define  AUDIO_EP_OUT_LOCK_DELAY             0x0000                           // Unused


             // Audio Streaming (AS) interface descriptor for micro
#define  STD_AS_IN_INTERFACE_NB              0x02                             // Index of Std AS Interface
             //Alternate O Audio Streaming (AS) interface descriptor
#define  ALT0_AS_IN_INTERFACE_INDEX          0x00                             // Index of Std AS interface Alt0
#define  ALT0_AS_IN_NB_ENDPOINT              0x00                             // Nb of endpoints for alt0 interface
#define  ALT0_AS_IN_INTERFACE_CLASS          AUDIO_CLASS                      // Audio class
#define  ALT0_AS_IN_INTERFACE_SUB_CLASS      AUDIOSTREAMING_SUBCLASS          // Audio streaming sub class
#define  ALT0_AS_IN_INTERFACE_PROTOCOL       NO_PROTOCOL                      // Unused
             //Alternate 1 Audio Streaming (AS) interface descriptor
#define  ALT1_AS_IN_INTERFACE_INDEX          0x01                             // Index of Std AS interface Alt1
#define  ALT1_AS_IN_NB_ENDPOINT              0x01                             // Nb of endpoints for alt1 interface
#define  ALT1_AS_IN_INTERFACE_CLASS          AUDIO_CLASS                      // Audio class
#define  ALT1_AS_IN_INTERFACE_SUB_CLASS      AUDIOSTREAMING_SUBCLASS          // Audio streaming sub class
#define  ALT1_AS_IN_INTERFACE_PROTOCOL       NO_PROTOCOL                      // Unused
             //AS general Interface descriptor
#define  AS_IN_TERMINAL_LINK                 MICRO_OUTPUT_TERMINAL_ID         // Unit Id of the output terminal
#define  AS_IN_DELAY                         0x01                             // Interface delay
#define  AS_IN_FORMAT_TAG                    AUDIO_FORMAT_TI_PCM              // PCM Format
             // Format type
#define  IN_FORMAT_TYPE                      AUDIO_FORMAT_TYPE_I              // Format Type I
#define  IN_FORMAT_NB_CHANNELS               0x01                             // One Channel
#define  IN_FORMAT_FRAME_SIZE                0x02                             // Two bytes per audio sample
#define  IN_FORMAT_BIT_RESOLUTION            0x10                             // 16 bits per sample
#define  IN_FORMAT_SAMPLE_FREQ_NB            0x01                             // One frequency supported
#if (BOARD!=EVK1105) || (DEFAULT_DACS!=AUDIO_MIXER_DAC_AIC23B)
# define  IN_FORMAT_LSBYTE_SAMPLE_FREQ        0x1F40                           // 0x001F40=8 kHz Sample frequency
#else
# define  IN_FORMAT_LSBYTE_SAMPLE_FREQ        0xAC44                           // 0x00AC44=44.1 kHz Sample frequency
#endif
#define  IN_FORMAT_MSBYTE_SAMPLE_FREQ        0x00                             // MsByte of 0x001F40
             //Audio endpoint specific descriptor field
#define  AUDIO_EP_IN_ATRIBUTES               0x00                             // No sampling freq, no pitch, no padding
#define  AUDIO_EP_IN_DELAY_UNIT              0x00                             // Unused
#define  AUDIO_EP_IN_LOCK_DELAY              0x0000                           // Unused

             // USB Interface descriptor Keyboard
#define INTERFACE_NB_KBD        3           // Interface's number
#define ALTERNATE_KBD           0
#define NB_ENDPOINT_KBD         1
#define INTERFACE_CLASS_KBD     HID_CLASS // HID Class
#define INTERFACE_SUB_CLASS_KBD NO_SUBCLASS
#define INTERFACE_PROTOCOL_KBD  KEYBOARD_PROTOCOL
#define INTERFACE_INDEX_KBD     0

            // USB Endpoint 1 descriptor FS
#define ENDPOINT_NB_KBD       (EP_KBD_IN | USB_ENDPOINT_IN)
#define EP_ATTRIBUTES_KBD     0x03              // BULK = 0x02, INTERUPT = 0x03
#define EP_IN_LENGTH_KBD      8
#define EP_SIZE_KBD           EP_IN_LENGTH_KBD
#define EP_INTERVAL_KBD       0x02              // Interrupt polling interval from host









#define  LANG_ID                             0x00
#define  DEVICE_STATUS                       BUS_POWERED
#define INTERFACE_STATUS      0x00 // TBD






#define  USB_MN_LENGTH         5
#define  USB_MANUFACTURER_NAME \
{\
  Usb_unicode('A'),\
  Usb_unicode('T'),\
  Usb_unicode('M'),\
  Usb_unicode('E'),\
  Usb_unicode('L') \
}

#define USB_PN_LENGTH         20
#define  USB_PRODUCT_NAME \
{\
  Usb_unicode('A')\
 ,Usb_unicode('V') \
 ,Usb_unicode('R') \
, Usb_unicode('3')\
, Usb_unicode('2')\
 ,Usb_unicode(' ') \
 ,Usb_unicode('U') \
, Usb_unicode('C')\
, Usb_unicode('3')\
 ,Usb_unicode(' ') \
 ,Usb_unicode('A') \
 ,Usb_unicode('U') \
 ,Usb_unicode('D') \
 ,Usb_unicode('I') \
 ,Usb_unicode('O') \
 ,Usb_unicode(' ') \
 ,Usb_unicode('D') \
 ,Usb_unicode('E') \
 ,Usb_unicode('M') \
 ,Usb_unicode('O') \
}

#define USB_SN_LENGTH         13
#define USB_SERIAL_NUMBER \
{\
  Usb_unicode('1'),\
  Usb_unicode('.'),\
  Usb_unicode('0'),\
  Usb_unicode('.'),\
  Usb_unicode('0'),\
  Usb_unicode('.'),\
  Usb_unicode('0'),\
  Usb_unicode('.'),\
  Usb_unicode('0'),\
  Usb_unicode('.'),\
  Usb_unicode('0'),\
  Usb_unicode('.'),\
  Usb_unicode('A') \
}
                           //Audio Channel Name
#define  AUDIO_CHANNEL_NAME \
{ Usb_unicode('A') \
, Usb_unicode('T') \
, Usb_unicode('M') \
, Usb_unicode('E') \
, Usb_unicode('L') \
}

#define  LANGUAGE_ID           0x0409


//! USB Request
typedef
#if (defined __ICCAVR32__)
#pragma pack(1)
#endif
struct
#if (defined __GNUC__)
__attribute__((__packed__))
#endif
{
   uint8_t    bmRequestType;       //!< Characteristics of the request
   uint8_t    bRequest;            //!< Specific request
   uint16_t   wValue;              //!< Field that varies according to request
   uint16_t   wIndex;              //!< Field that varies according to request
   uint16_t   wLength;             //!< Number of bytes to transfer if Data
}
#if (defined __ICCAVR32__)
#pragma pack()
#endif
S_UsbRequest;


//! USB Device Descriptor
typedef
#if (defined __ICCAVR32__)
#pragma pack(1)
#endif
struct
#if (defined __GNUC__)
__attribute__((__packed__))
#endif
{
   uint8_t    bLength;             //!< Size of this descriptor in bytes
   uint8_t    bDescriptorType;     //!< DEVICE descriptor type
   uint16_t   bscUSB;              //!< Binary Coded Decimal Spec. release
   uint8_t    bDeviceClass;        //!< Class code assigned by the USB
   uint8_t    bDeviceSubClass;     //!< Subclass code assigned by the USB
   uint8_t    bDeviceProtocol;     //!< Protocol code assigned by the USB
   uint8_t    bMaxPacketSize0;     //!< Max packet size for EP0
   uint16_t   idVendor;            //!< Vendor ID. ATMEL = 0x03EB
   uint16_t   idProduct;           //!< Product ID assigned by the manufacturer
   uint16_t   bcdDevice;           //!< Device release number
   uint8_t    iManufacturer;       //!< Index of manufacturer string descriptor
   uint8_t    iProduct;            //!< Index of product string descriptor
   uint8_t    iSerialNumber;       //!< Index of serial number string descriptor
   uint8_t    bNumConfigurations;  //!< Number of possible configurations
}
#if (defined __ICCAVR32__)
#pragma pack()
#endif
S_usb_device_descriptor;


//! USB Configuration Descriptor
typedef
#if (defined __ICCAVR32__)
#pragma pack(1)
#endif
struct
#if (defined __GNUC__)
__attribute__((__packed__))
#endif
{
   uint8_t    bLength;             //!< Size of this descriptor in bytes
   uint8_t    bDescriptorType;     //!< CONFIGURATION descriptor type
   uint16_t   wTotalLength;        //!< Total length of data returned
   uint8_t    bNumInterfaces;      //!< Number of interfaces for this conf.
   uint8_t    bConfigurationValue; //!< Value for SetConfiguration request
   uint8_t    iConfiguration;      //!< Index of string descriptor
   uint8_t    bmAttributes;        //!< Configuration characteristics
   uint8_t    MaxPower;            //!< Maximum power consumption
}
#if (defined __ICCAVR32__)
#pragma pack()
#endif
S_usb_configuration_descriptor;


//! USB Interface Descriptor
typedef
#if (defined __ICCAVR32__)
#pragma pack(1)
#endif
struct
#if (defined __GNUC__)
__attribute__((__packed__))
#endif
{
   uint8_t    bLength;             //!< Size of this descriptor in bytes
   uint8_t    bDescriptorType;     //!< INTERFACE descriptor type
   uint8_t    bInterfaceNumber;    //!< Number of interface
   uint8_t    bAlternateSetting;   //!< Value to select alternate setting
   uint8_t    bNumEndpoints;       //!< Number of EP except EP 0
   uint8_t    bInterfaceClass;     //!< Class code assigned by the USB
   uint8_t    bInterfaceSubClass;  //!< Subclass code assigned by the USB
   uint8_t    bInterfaceProtocol;  //!< Protocol code assigned by the USB
   uint8_t    iInterface;          //!< Index of string descriptor
}
#if (defined __ICCAVR32__)
#pragma pack()
#endif
S_usb_interface_descriptor;


            //! A U D I O Specific
            //! Class-Specific AC Interface Descriptor
typedef
#if (defined __ICCAVR32__)
#pragma pack(1)
#endif
struct
#if (defined __GNUC__)
__attribute__((__packed__))
#endif
{
   uint8_t    bLength;             //!< Size of this descriptor in bytes
   uint8_t    bDescriptorType;     //!< CS interface
   uint8_t    bDescritorSubtype;   //!< HEADER Subtype
   uint16_t   bcdADC;              //!< Revision of class spec
   uint16_t   wTotalLength;        //!< Total size of class specific descriptor
   uint8_t    bInCollection;       //!< Number of streaming interface
   uint8_t    baInterface1;        //!< Interface number of the first AudioStreaming or MIDIStreaming interface in the Collection.
   uint8_t    baInterface2;        //!< Interface number of the second AudioStreaming or MIDIStreaming interface in the Collection.
}
#if (defined __ICCAVR32__)
#pragma pack()
#endif
S_usb_ac_interface_descriptor;


            //! Class-Specific Input Terminal Descriptor
typedef
#if (defined __ICCAVR32__)
#pragma pack(1)
#endif
struct
#if (defined __GNUC__)
__attribute__((__packed__))
#endif
{
   uint8_t    bLenght;
   uint8_t    bDescriptorType;
   uint8_t    bDescriptorSubType;
   uint8_t    bTerminalID;
   uint16_t   wTerminalType;
   uint8_t    bAssocTerminal;
   uint8_t    bNrChannels;
   uint16_t   wChannelConfig;
   uint8_t    iChannelNames;
   uint8_t    iTerminal;
}
#if (defined __ICCAVR32__)
#pragma pack()
#endif
S_usb_in_ter_descriptor;


            //! Class-Specific Feature Unit Descriptor
typedef
#if (defined __ICCAVR32__)
#pragma pack(1)
#endif
struct
#if (defined __GNUC__)
__attribute__((__packed__))
#endif
{
   uint8_t    bLenght;
   uint8_t    bDescriptorType;
   uint8_t    bDescriptorSubType;
   uint8_t    bUnitID;
   uint8_t    bSourceID;
   uint8_t    bControSize;
   uint8_t    bmaControlMaster;
   uint8_t    bmaControlCh1;
}
#if (defined __ICCAVR32__)
#pragma pack()
#endif
S_usb_feature_unit_descriptor_speaker;

            //! Class-Specific Feature Unit Descriptor
typedef
#if (defined __ICCAVR32__)
#pragma pack(1)
#endif
struct
#if (defined __GNUC__)
__attribute__((__packed__))
#endif
{
   uint8_t    bLenght;
   uint8_t    bDescriptorType;
   uint8_t    bDescriptorSubType;
   uint8_t    bUnitID;
   uint8_t    bSourceID;
   uint8_t    bControSize;
   uint8_t    bmaControlMaster;
   uint8_t    bmaControlCh1;
}
#if (defined __ICCAVR32__)
#pragma pack()
#endif
S_usb_feature_unit_descriptor_micro;

            //! Class-Specific Output Terminal Descriptor
typedef
#if (defined __ICCAVR32__)
#pragma pack(1)
#endif
struct
#if (defined __GNUC__)
__attribute__((__packed__))
#endif
{
   uint8_t    bLenght;
   uint8_t    bDescriptorType;
   uint8_t    bDescriptorSubType;
   uint8_t    bTerminalID;
   uint16_t   wTerminalType;
   uint8_t    bAssocTerminal;
   uint8_t    bSourceID;
   uint8_t    iTerminal;
}
#if (defined __ICCAVR32__)
#pragma pack()
#endif
S_usb_out_ter_descriptor;

            //! Class-Specific AS Interface Descriptor
typedef
#if (defined __ICCAVR32__)
#pragma pack(1)
#endif
struct
#if (defined __GNUC__)
__attribute__((__packed__))
#endif
{
   uint8_t    bLenght;
   uint8_t    bDescriptorType;
   uint8_t    bDescriptorSubType;
   uint8_t    bTerminalLink;
   uint8_t    bDelay;
   uint16_t   wFormatTag;
}
#if (defined __ICCAVR32__)
#pragma pack()
#endif
S_usb_as_interface_descriptor;

            //! Audio Format Type descriptor
typedef
#if (defined __ICCAVR32__)
#pragma pack(1)
#endif
struct
#if (defined __GNUC__)
__attribute__((__packed__))
#endif
{
   uint8_t    bLenght;
   uint8_t    bDescriptorType;
   uint8_t    bDescriptorSubType;
   uint8_t    bFormatType;
   uint8_t    bNrChannels;
   uint8_t    bSubFrameSize;
   uint8_t    bBitResolution;
   uint8_t    bSampleFreqType;
#if (defined USB_RESYNC_METHOD)
  #if (USB_RESYNC_METHOD == USB_RESYNC_METHOD_SOFT_ADD_DEL_SAMPLES)
    uint16_t   wLsbyteiSamFreq;
    uint8_t    bMsbyteiSamFreq;
  #elif ((USB_RESYNC_METHOD == USB_RESYNC_METHOD_SOFT_ADAPTIF_SRC) || (USB_RESYNC_METHOD == USB_RESYNC_METHOD_EXT_CLOCK_SYNTHESIZER))
    uint16_t   wLsbyteiSamFreq_1;
    uint8_t    bMsbyteiSamFreq_1;
    uint16_t   wLsbyteiSamFreq_2;
    uint8_t    bMsbyteiSamFreq_2;
    uint16_t   wLsbyteiSamFreq_3;
    uint8_t    bMsbyteiSamFreq_3;
  #else
  # error Unknown configuration
  #endif
#else
#error USB_RESYNC_METHOD in not known
#endif
}
#if (defined __ICCAVR32__)
#pragma pack()
#endif
S_usb_format_type_speaker;


typedef
#if (defined __ICCAVR32__)
#pragma pack(1)
#endif
struct
#if (defined __GNUC__)
__attribute__((__packed__))
#endif
{
   uint8_t    bLenght;
   uint8_t    bDescriptorType;
   uint8_t    bDescriptorSubType;
   uint8_t    bFormatType;
   uint8_t    bNrChannels;
   uint8_t    bSubFrameSize;
   uint8_t    bBitResolution;
   uint8_t    bSampleFreqType;
   uint16_t   wLsbyteiSamFreq;
   uint8_t    bMsbyteiSamFreq;
}
#if (defined __ICCAVR32__)
#pragma pack()
#endif
S_usb_format_type_microphone;


            //! Endpoint AUDIO Specific descriptor
typedef
#if (defined __ICCAVR32__)
#pragma pack(1)
#endif
struct
#if (defined __GNUC__)
__attribute__((__packed__))
#endif
{
   uint8_t    bLenght;
   uint8_t    bDescriptorType;
   uint8_t    bDescriptorSubType;
   uint8_t    bmAttributes;
   uint8_t    bLockDelayUnits;
   uint16_t   wLockDelay;
}
#if (defined __ICCAVR32__)
#pragma pack()
#endif
S_usb_endpoint_audio_specific;


               //! Usb Audio Endpoint Descriptor
typedef
#if (defined __ICCAVR32__)
#pragma pack(1)
#endif
struct
#if (defined __GNUC__)
__attribute__((__packed__))
#endif
{
   uint8_t    bLength;             //!< Size of this descriptor in bytes
   uint8_t    bDescriptorType;     //!< ENDPOINT descriptor type
   uint8_t    bEndpointAddress;    //!< Address of the endpoint
   uint8_t    bmAttributes;        //!< Endpoint's attributes
   uint16_t   wMaxPacketSize;      //!< Maximum packet size for this EP
   uint8_t    bInterval;           //!< Interval for polling EP in ms
   uint8_t    bRefresh;
   uint8_t    bSynAddress;
}
#if (defined __ICCAVR32__)
#pragma pack()
#endif
S_usb_endpoint_audio_descriptor;


               //! USB Endpoint Descriptor
typedef
#if (defined __ICCAVR32__)
#pragma pack(1)
#endif
struct
#if (defined __GNUC__)
__attribute__((__packed__))
#endif
{
   uint8_t    bLength;             //!< Size of this descriptor in bytes
   uint8_t    bDescriptorType;     //!< ENDPOINT descriptor type
   uint8_t    bEndpointAddress;    //!< Address of the endpoint
   uint8_t    bmAttributes;        //!< Endpoint's attributes
   uint16_t   wMaxPacketSize;      //!< Maximum packet size for this EP
   uint8_t    bInterval;           //!< Interval for polling EP in ms
}
#if (defined __ICCAVR32__)
#pragma pack()
#endif
S_usb_endpoint_descriptor;


               //! USB Language Descriptor
typedef
#if (defined __ICCAVR32__)
#pragma pack(1)
#endif
struct
#if (defined __GNUC__)
__attribute__((__packed__))
#endif
{
   uint8_t    bLength;             //!< Size of this descriptor in bytes
   uint8_t    bDescriptorType;     //!< STRING descriptor type
   uint16_t   wlangid;             //!< Language id
}
#if (defined __ICCAVR32__)
#pragma pack()
#endif
S_usb_language_id;


//_____ U S B   M A N U F A C T U R E R   D E S C R I P T O R _______________

//! struct usb_st_manufacturer
typedef
#if (defined __ICCAVR32__)
#pragma pack(1)
#endif
struct
#if (defined __GNUC__)
__attribute__((__packed__))
#endif
{
   uint8_t    bLength;                //!< Size of this descriptor in bytes
   uint8_t    bDescriptorType;        //!< STRING descriptor type
   uint16_t   wstring[USB_MN_LENGTH]; //!< Unicode characters
}
#if (defined __ICCAVR32__)
#pragma pack()
#endif
S_usb_manufacturer_string_descriptor;


//_____ U S B   P R O D U C T   D E S C R I P T O R _________________________

//! struct usb_st_product
typedef
#if (defined __ICCAVR32__)
#pragma pack(1)
#endif
struct
#if (defined __GNUC__)
__attribute__((__packed__))
#endif
{
   uint8_t    bLength;                //!< Size of this descriptor in bytes
   uint8_t    bDescriptorType;        //!< STRING descriptor type
   uint16_t   wstring[USB_PN_LENGTH]; //!< Unicode characters
}
#if (defined __ICCAVR32__)
#pragma pack()
#endif
S_usb_product_string_descriptor;


//_____ U S B   S E R I A L   N U M B E R   D E S C R I P T O R _____________

//! struct usb_st_serial_number
typedef
#if (defined __ICCAVR32__)
#pragma pack(1)
#endif
struct
#if (defined __GNUC__)
__attribute__((__packed__))
#endif
{
   uint8_t  bLength;               //!< Size of this descriptor in bytes
   uint8_t  bDescriptorType;       //!< STRING descriptor type
   uint16_t wstring[USB_SN_LENGTH];//!< Unicode characters
}
#if (defined __ICCAVR32__)
#pragma pack()
#endif
S_usb_serial_number;


//_____ U S B   H I D   D E S C R I P T O R _____________________________________

typedef
#if (defined __ICCAVR32__)
#pragma pack(1)
#endif
struct
#if (defined __GNUC__)
__attribute__((__packed__))
#endif
{
  uint8_t  bLength;               /* Size of this descriptor in bytes */
  uint8_t  bDescriptorType;       /* HID descriptor type */
  uint16_t bscHID;                /* Binary Coded Decimal Spec. release */
  uint8_t  bCountryCode;          /* Hardware target country */
  uint8_t  bNumDescriptors;       /* Number of HID class descriptors to follow */
  uint8_t  bRDescriptorType;      /* Report descriptor type */
  uint16_t wDescriptorLength;     /* Total length of Report descriptor */
}
#if (defined __ICCAVR32__)
#pragma pack()
#endif
S_usb_hid_descriptor;


//_____ U S B   D E V I C E   A U D I O   D E S C R I P T O R ___________________

typedef
#if (defined __ICCAVR32__)
#pragma pack(1)
#endif
struct
#if (defined __GNUC__)
__attribute__((__packed__))
#endif
{
   // Audio stuff
   S_usb_configuration_descriptor         cfg;
   S_usb_interface_descriptor             ifc;
   S_usb_ac_interface_descriptor          audioac;
   S_usb_in_ter_descriptor                speaker_in_ter;
   S_usb_feature_unit_descriptor_speaker  speaker_fea_unit;
   S_usb_out_ter_descriptor               speaker_out_ter;
   S_usb_in_ter_descriptor                mic_in_ter;
   S_usb_feature_unit_descriptor_micro    mic_fea_unit;
   S_usb_out_ter_descriptor               mic_out_ter;


   S_usb_interface_descriptor             speaker_as_alt0;
   S_usb_interface_descriptor             speaker_as_alt1;
   S_usb_as_interface_descriptor          speaker_g_as;
   S_usb_format_type_speaker              speaker_format_type;
   S_usb_endpoint_audio_descriptor        speaker_ep1;
   S_usb_endpoint_audio_specific          speaker_ep1_s;
   S_usb_interface_descriptor             mic_as_alt0;
   S_usb_interface_descriptor             mic_as_alt1;
   S_usb_as_interface_descriptor          mic_g_as;
   S_usb_format_type_microphone           mic_format_type;
   S_usb_endpoint_audio_descriptor        mic_ep1;
   S_usb_endpoint_audio_specific          mic_ep1_s;

   // HID stuff
   S_usb_interface_descriptor             ifc_kbd;
   S_usb_hid_descriptor                   hid_kbd;
   S_usb_endpoint_descriptor              ep1_kbd;
}
#if (defined __ICCAVR32__)
#pragma pack()
#endif
S_usb_user_configuration_descriptor;


#endif  // _USB_DESCRIPTORS_H_
