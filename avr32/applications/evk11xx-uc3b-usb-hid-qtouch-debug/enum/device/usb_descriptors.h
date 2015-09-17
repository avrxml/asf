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

#if USB_DEVICE_FEATURE == false
  #error usb_descriptors.h is #included although USB_DEVICE_FEATURE is disabled
#endif


#include "usb_standard_request.h"
#include "usb_task.h"


//_____ M A C R O S ________________________________________________________

#define Usb_unicode(c)                    (Usb_format_mcu_to_usb_data(16, (U16)(c)))
#define Usb_get_dev_desc_pointer()        (&(usb_dev_desc.bLength))
#define Usb_get_dev_desc_length()         (sizeof(usb_dev_desc))
#define Usb_get_conf_desc_pointer()       Usb_get_conf_desc_fs_pointer()
#define Usb_get_conf_desc_length()        Usb_get_conf_desc_fs_length()
#define Usb_get_conf_desc_hs_pointer()    (&(usb_conf_desc_hs.cfg.bLength))
#define Usb_get_conf_desc_hs_length()     (sizeof(usb_conf_desc_hs))
#define Usb_get_conf_desc_fs_pointer()    (&(usb_conf_desc_fs.cfg.bLength))
#define Usb_get_conf_desc_fs_length()     (sizeof(usb_conf_desc_fs))
#define Usb_get_qualifier_desc_pointer()  (&(usb_qualifier_desc.bLength))
#define Usb_get_qualifier_desc_length()   (sizeof(usb_qualifier_desc))


//_____ U S B    D E F I N E S _____________________________________________

            // USB Device descriptor
#define USB_SPECIFICATION     0x0200
#define DEVICE_CLASS          0                   //! Each configuration has its own class
#define DEVICE_SUB_CLASS      0                   //! Each configuration has its own subclass
#define DEVICE_PROTOCOL       0                   //! Each configuration has its own protocol
#define EP_CONTROL_LENGTH     64
#define VENDOR_ID             ATMEL_VID           //! Atmel vendor ID
#define PRODUCT_ID            HID_QTOUCH_DEBUG_PID
#define RELEASE_NUMBER        0x1000
#define MAN_INDEX             0x01
#define PROD_INDEX            0x02
#define SN_INDEX              0x03
#define NB_CONFIGURATION      1

            // CONFIGURATION
#define NB_INTERFACE       1     //! The number of interfaces for this configuration
#define CONF_NB            1     //! Number of this configuration
#define CONF_INDEX         0
#define CONF_ATTRIBUTES    USB_CONFIG_SELFPOWERED
#define MAX_POWER          50    // 100 mA

            // USB  HID Interface descriptor
#define INTERFACE_NB                0                  //! The number of this interface
#define ALTERNATE                   0                  //! The alt setting nb of this interface
#define NB_ENDPOINT                 2                  //! The number of endpoints this interface has
#define INTERFACE_CLASS             HID_CLASS          //! HID Class
#define INTERFACE_SUB_CLASS         NO_SUBCLASS        //! No Subclass
#define INTERFACE_PROTOCOL          NO_PROTOCOL        //! No Protocol
#define INTERFACE_INDEX             0

            // Mouse HID descriptor
#define HID_VERSION               0x0111  //! HID Class Specification release number
#define HID_COUNTRY_CODE            0x00    //! Hardware target country
#define HID_NUM_DESCRIPTORS         0x01    //! Number of HID class descriptors to follow

             // USB Endpoint 1 descriptor FS
#define ENDPOINT_NB_1       (EP_HID_GENERIC_IN | USB_ENDPOINT_IN)
#define EP_ATTRIBUTES_1     0x03          // BULK = 0x02, INTERUPT = 0x03
#define EP_IN_LENGTH        0x40          // 64Byte
#define EP_SIZE_1           EP_IN_LENGTH
#define EP_INTERVAL_1       1 //interrupt pooling from host

             // USB Endpoint 1 descriptor FS
#define ENDPOINT_NB_2       (EP_HID_GENERIC_OUT)
#define EP_ATTRIBUTES_2     0x03          // BULK = 0x02, INTERUPT = 0x03
#define EP_OUT_LENGTH       0x40          // 64Byte
#define EP_SIZE_2           EP_OUT_LENGTH
#define EP_INTERVAL_2       1 //interrupt pooling from host

#define SIZE_OF_REPORT        0x35
#define LENGTH_OF_REPORT_IN   0x40        // 64Byte
#define LENGTH_OF_REPORT_OUT  0x40        // 64Byte

#define DEVICE_STATUS         SELF_POWERED
#define INTERFACE_STATUS      0x00 // TBD

#define LANG_ID               0x00

#define USB_MN_LENGTH         5
#define USB_MANUFACTURER_NAME \
{\
  Usb_unicode('A'),\
  Usb_unicode('T'),\
  Usb_unicode('M'),\
  Usb_unicode('E'),\
  Usb_unicode('L') \
}

#define USB_PN_LENGTH         21
#define USB_PRODUCT_NAME \
{\
  Usb_unicode('A'),\
  Usb_unicode('V'),\
  Usb_unicode('R'),\
  Usb_unicode('3'),\
  Usb_unicode('2'),\
  Usb_unicode(' '),\
  Usb_unicode('U'),\
  Usb_unicode('C'),\
  Usb_unicode('3'),\
  Usb_unicode(' '),\
  Usb_unicode('H'),\
  Usb_unicode('I'),\
  Usb_unicode('D'),\
  Usb_unicode(' '),\
  Usb_unicode('G'),\
  Usb_unicode('E'),\
  Usb_unicode('N'),\
  Usb_unicode('E'),\
  Usb_unicode('R'),\
  Usb_unicode('I'),\
  Usb_unicode('C') \
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

#define LANGUAGE_ID           0x0409


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
  U8      bmRequestType;        //!< Characteristics of the request
  U8      bRequest;             //!< Specific request
  U16     wValue;               //!< Field that varies according to request
  U16     wIndex;               //!< Field that varies according to request
  U16     wLength;              //!< Number of bytes to transfer if Data
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
  U8      bLength;              //!< Size of this descriptor in bytes
  U8      bDescriptorType;      //!< DEVICE descriptor type
  U16     bscUSB;               //!< Binay Coded Decimal Spec. release
  U8      bDeviceClass;         //!< Class code assigned by the USB
  U8      bDeviceSubClass;      //!< Subclass code assigned by the USB
  U8      bDeviceProtocol;      //!< Protocol code assigned by the USB
  U8      bMaxPacketSize0;      //!< Max packet size for EP0
  U16     idVendor;             //!< Vendor ID. ATMEL = 0x03EB
  U16     idProduct;            //!< Product ID assigned by the manufacturer
  U16     bcdDevice;            //!< Device release number
  U8      iManufacturer;        //!< Index of manu. string descriptor
  U8      iProduct;             //!< Index of prod. string descriptor
  U8      iSerialNumber;        //!< Index of S.N.  string descriptor
  U8      bNumConfigurations;   //!< Number of possible configurations
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
  U8      bLength;              //!< Size of this descriptor in bytes
  U8      bDescriptorType;      //!< CONFIGURATION descriptor type
  U16     wTotalLength;         //!< Total length of data returned
  U8      bNumInterfaces;       //!< Number of interfaces for this conf.
  U8      bConfigurationValue;  //!< Value for SetConfiguration request
  U8      iConfiguration;       //!< Index of string descriptor
  U8      bmAttributes;         //!< Configuration characteristics
  U8      MaxPower;             //!< Maximum power consumption
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
  U8      bLength;              //!< Size of this descriptor in bytes
  U8      bDescriptorType;      //!< INTERFACE descriptor type
  U8      bInterfaceNumber;     //!< Number of interface
  U8      bAlternateSetting;    //!< Value to select alternate setting
  U8      bNumEndpoints;        //!< Number of EP except EP 0
  U8      bInterfaceClass;      //!< Class code assigned by the USB
  U8      bInterfaceSubClass;   //!< Subclass code assigned by the USB
  U8      bInterfaceProtocol;   //!< Protocol code assigned by the USB
  U8      iInterface;           //!< Index of string descriptor
}
#if (defined __ICCAVR32__)
#pragma pack()
#endif
S_usb_interface_descriptor;


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
  U8      bLength;              //!< Size of this descriptor in bytes
  U8      bDescriptorType;      //!< ENDPOINT descriptor type
  U8      bEndpointAddress;     //!< Address of the endpoint
  U8      bmAttributes;         //!< Endpoint's attributes
  U16     wMaxPacketSize;       //!< Maximum packet size for this EP
  U8      bInterval;            //!< Interval for polling EP in ms
}
#if (defined __ICCAVR32__)
#pragma pack()
#endif
S_usb_endpoint_descriptor;


//! USB Device Qualifier Descriptor
typedef
#if (defined __ICCAVR32__)
#pragma pack(1)
#endif
struct
#if (defined __GNUC__)
__attribute__((__packed__))
#endif
{
  U8      bLength;              //!< Size of this descriptor in bytes
  U8      bDescriptorType;      //!< Device Qualifier descriptor type
  U16     bscUSB;               //!< Binary Coded Decimal Spec. release
  U8      bDeviceClass;         //!< Class code assigned by the USB
  U8      bDeviceSubClass;      //!< Subclass code assigned by the USB
  U8      bDeviceProtocol;      //!< Protocol code assigned by the USB
  U8      bMaxPacketSize0;      //!< Max packet size for EP0
  U8      bNumConfigurations;   //!< Number of possible configurations
  U8      bReserved;            //!< Reserved for future use, must be zero
}
#if (defined __ICCAVR32__)
#pragma pack()
#endif
S_usb_device_qualifier_descriptor;


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
  U8      bLength;              //!< Size of this descriptor in bytes
  U8      bDescriptorType;      //!< STRING descriptor type
  U16     wlangid;              //!< Language id
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
  U8  bLength;                  //!< Size of this descriptor in bytes
  U8  bDescriptorType;          //!< STRING descriptor type
  U16 wstring[USB_MN_LENGTH];   //!< Unicode characters
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
  U8  bLength;                  //!< Size of this descriptor in bytes
  U8  bDescriptorType;          //!< STRING descriptor type
  U16 wstring[USB_PN_LENGTH];   //!< Unicode characters
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
  U8  bLength;                  //!< Size of this descriptor in bytes
  U8  bDescriptorType;          //!< STRING descriptor type
  U16 wstring[USB_SN_LENGTH];   //!< Unicode characters
}
#if (defined __ICCAVR32__)
#pragma pack()
#endif
S_usb_serial_number;


//_____ U S B   D E V I C E   H I D   D E S C R I P T O R ___________________

//! USB HID Descriptor
typedef
#if (defined __ICCAVR32__)
#pragma pack(1)
#endif
struct
#if (defined __GNUC__)
__attribute__((__packed__))
#endif
{
  U8      bLength;              //!< Size of this descriptor in bytes
  U8      bDescriptorType;      //!< HID descriptor type
  U16     bcdHID;               //!< HID Class Specification release number
  U8      bCountryCode;         //!< Hardware target country
  U8      bNumDescriptors;      //!< Number of HID class descriptors to follow
  U8      bRDescriptorType;     //!< Report descriptor type
  U16     wItemLength;          //!< Total length of Report descriptor
}
#if (defined __ICCAVR32__)
#pragma pack()
#endif
S_usb_hid_descriptor;


typedef
#if (defined __ICCAVR32__)
#pragma pack(1)
#endif
struct
#if (defined __GNUC__)
__attribute__((__packed__))
#endif
{
  S_usb_configuration_descriptor cfg;
  S_usb_interface_descriptor     ifc;
  S_usb_hid_descriptor           hid;
  S_usb_endpoint_descriptor      ep1;
  S_usb_endpoint_descriptor      ep2;
}
#if (defined __ICCAVR32__)
#pragma pack()
#endif
S_usb_user_configuration_descriptor;


#endif  // _USB_DESCRIPTORS_H_
