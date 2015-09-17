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

#define Usb_encode(c)                     (Usb_format_mcu_to_usb_data(16, (U16)(c)))
#define Usb_get_dev_desc_pointer()        (&(usb_dev_desc.bLength))
#define Usb_get_dev_desc_length()         (sizeof(usb_dev_desc))
#define Usb_get_conf_desc_pointer()       (&(usb_conf_desc.cfg.bLength))
#define Usb_get_conf_desc_length()        (sizeof(usb_conf_desc))


//_____ U S B    D E F I N E S _____________________________________________

            // USB Device descriptor
#define USB_SPECIFICATION     0x0200
#define DEVICE_CLASS          0                   //! Each configuration has its own class
#define DEVICE_SUB_CLASS      0                   //! Each configuration has its own subclass
#define DEVICE_PROTOCOL       0                   //! Each configuration has its own protocol
#define EP_CONTROL_LENGTH     64
#define VENDOR_ID             ATMEL_VID           //! Atmel vendor ID
#define PRODUCT_ID            EVK1100_CTRL_PANEL_DEMO_PID
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

            // USB Interface descriptor gen
#define INTERFACE_NB             0              //! The number of this interface
#define ALTERNATE                0              //! The alt setting nb of this interface
#define NB_ENDPOINT              2              //! The number of endpoints this interface has
#define INTERFACE_CLASS          MS_CLASS       //! Mass-Storage Class
#define INTERFACE_SUB_CLASS      SCSI_SUBCLASS  //! SCSI Transparent Command Set Subclass
#define INTERFACE_PROTOCOL       BULK_PROTOCOL  //! Bulk-Only Protocol
#define INTERFACE_INDEX          0

            // USB Endpoint 1 descriptor FS
#define ENDPOINT_NB_1           (EP_MS_IN | MSK_EP_DIR)
#define EP_ATTRIBUTES_1         TYPE_BULK
#define EP_IN_LENGTH_1          64
#define EP_SIZE_1               EP_IN_LENGTH_1
#define EP_INTERVAL_1           0x00              // Interrupt polling interval from host

            // USB Endpoint 2 descriptor FS
#define ENDPOINT_NB_2           EP_MS_OUT
#define EP_ATTRIBUTES_2         TYPE_BULK
#define EP_OUT_LENGTH_2         64
#define EP_SIZE_2               EP_OUT_LENGTH_2
#define EP_INTERVAL_2           0x00              // Interrupt polling interval from host

#define DEVICE_STATUS         SELF_POWERED
#define INTERFACE_STATUS      0x00 // TBD

#define LANG_ID               0x00

#define USB_MN_LENGTH         5
#define USB_MANUFACTURER_NAME \
{\
  Usb_encode('A'),\
  Usb_encode('T'),\
  Usb_encode('M'),\
  Usb_encode('E'),\
  Usb_encode('L') \
}

#define USB_PN_LENGTH         23
#define USB_PRODUCT_NAME \
{\
  Usb_encode('U'),\
  Usb_encode('C'),\
  Usb_encode('3'),\
  Usb_encode('A'),\
  Usb_encode(' '),\
  Usb_encode('C'),\
  Usb_encode('O'),\
  Usb_encode('N'),\
  Usb_encode('T'),\
  Usb_encode('R'),\
  Usb_encode('O'),\
  Usb_encode('L'),\
  Usb_encode(' '),\
  Usb_encode('P'),\
  Usb_encode('A'),\
  Usb_encode('N'),\
  Usb_encode('E'),\
  Usb_encode('L'),\
  Usb_encode(' '),\
  Usb_encode('D'),\
  Usb_encode('E'),\
  Usb_encode('M'),\
  Usb_encode('O') \
}

#define USB_SN_LENGTH         0x0D
#define USB_SERIAL_NUMBER \
{\
  Usb_encode('1'),\
  Usb_encode('.'),\
  Usb_encode('0'),\
  Usb_encode('.'),\
  Usb_encode('0'),\
  Usb_encode('.'),\
  Usb_encode('0'),\
  Usb_encode('.'),\
  Usb_encode('0'),\
  Usb_encode('.'),\
  Usb_encode('0'),\
  Usb_encode('.'),\
  Usb_encode('A') \
}

#define LANGUAGE_ID           0x0409


//! USB Request
typedef
#ifdef __ICCAVR32__
#pragma pack(1)
#endif
struct
#ifdef __GNUC__
__attribute__((__packed__))
#endif
{
  U8      bmRequestType;        //!< Characteristics of the request
  U8      bRequest;             //!< Specific request
  U16     wValue;               //!< Field that varies according to request
  U16     wIndex;               //!< Field that varies according to request
  U16     wLength;              //!< Number of bytes to transfer if Data
}
#ifdef __ICCAVR32__
#pragma pack()
#endif
S_UsbRequest;


//! USB Device Descriptor
typedef
#ifdef __ICCAVR32__
#pragma pack(1)
#endif
struct
#ifdef __GNUC__
__attribute__((__packed__))
#endif
{
  U8      bLength;              //!< Size of this descriptor in bytes
  U8      bDescriptorType;      //!< DEVICE descriptor type
  U16     bscUSB;               //!< Binary Coded Decimal Spec. release
  U8      bDeviceClass;         //!< Class code assigned by the USB
  U8      bDeviceSubClass;      //!< Subclass code assigned by the USB
  U8      bDeviceProtocol;      //!< Protocol code assigned by the USB
  U8      bMaxPacketSize0;      //!< Max packet size for EP0
  U16     idVendor;             //!< Vendor ID. ATMEL = 0x03EB
  U16     idProduct;            //!< Product ID assigned by the manufacturer
  U16     bcdDevice;            //!< Device release number
  U8      iManufacturer;        //!< Index of manufacturer string descriptor
  U8      iProduct;             //!< Index of product string descriptor
  U8      iSerialNumber;        //!< Index of serial number string descriptor
  U8      bNumConfigurations;   //!< Number of possible configurations
}
#ifdef __ICCAVR32__
#pragma pack()
#endif
S_usb_device_descriptor;


//! USB Configuration Descriptor
typedef
#ifdef __ICCAVR32__
#pragma pack(1)
#endif
struct
#ifdef __GNUC__
__attribute__((__packed__))
#endif
{
  U8      bLength;              //!< Size of this descriptor in bytes
  U8      bDescriptorType;      //!< CONFIGURATION descriptor type
  U16     wTotalLength;         //!< Total length of data returned
  U8      bNumInterfaces;       //!< Number of interfaces for this conf.
  U8      bConfigurationValue;  //!< Value for SetConfiguration resquest
  U8      iConfiguration;       //!< Index of string descriptor
  U8      bmAttributes;         //!< Configuration characteristics
  U8      MaxPower;             //!< Maximum power consumption
}
#ifdef __ICCAVR32__
#pragma pack()
#endif
S_usb_configuration_descriptor;


//! USB Interface Descriptor
typedef
#ifdef __ICCAVR32__
#pragma pack(1)
#endif
struct
#ifdef __GNUC__
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
#ifdef __ICCAVR32__
#pragma pack()
#endif
S_usb_interface_descriptor;


//! USB Endpoint Descriptor
typedef
#ifdef __ICCAVR32__
#pragma pack(1)
#endif
struct
#ifdef __GNUC__
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
#ifdef __ICCAVR32__
#pragma pack()
#endif
S_usb_endpoint_descriptor;


//! USB Device Qualifier Descriptor
typedef
#ifdef __ICCAVR32__
#pragma pack(1)
#endif
struct
#ifdef __GNUC__
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
#ifdef __ICCAVR32__
#pragma pack()
#endif
S_usb_device_qualifier_descriptor;


//! USB Language Descriptor
typedef
#ifdef __ICCAVR32__
#pragma pack(1)
#endif
struct
#ifdef __GNUC__
__attribute__((__packed__))
#endif
{
  U8      bLength;              //!< Size of this descriptor in bytes
  U8      bDescriptorType;      //!< STRING descriptor type
  U16     wlangid;              //!< Language id
}
#ifdef __ICCAVR32__
#pragma pack()
#endif
S_usb_language_id;


//_____ U S B   M A N U F A C T U R E R   D E S C R I P T O R _______________

//! struct usb_st_manufacturer
typedef
#ifdef __ICCAVR32__
#pragma pack(1)
#endif
struct
#ifdef __GNUC__
__attribute__((__packed__))
#endif
{
  U8  bLength;                  //!< Size of this descriptor in bytes
  U8  bDescriptorType;          //!< STRING descriptor type
  U16 wstring[USB_MN_LENGTH];   //!< Unicode characters
}
#ifdef __ICCAVR32__
#pragma pack()
#endif
S_usb_manufacturer_string_descriptor;


//_____ U S B   P R O D U C T   D E S C R I P T O R _________________________

//! struct usb_st_product
typedef
#ifdef __ICCAVR32__
#pragma pack(1)
#endif
struct
#ifdef __GNUC__
__attribute__((__packed__))
#endif
{
  U8  bLength;                  //!< Size of this descriptor in bytes
  U8  bDescriptorType;          //!< STRING descriptor type
  U16 wstring[USB_PN_LENGTH];   //!< Unicode characters
}
#ifdef __ICCAVR32__
#pragma pack()
#endif
S_usb_product_string_descriptor;


//_____ U S B   S E R I A L   N U M B E R   D E S C R I P T O R _____________

//! struct usb_st_serial_number
typedef
#ifdef __ICCAVR32__
#pragma pack(1)
#endif
struct
#ifdef __GNUC__
__attribute__((__packed__))
#endif
{
  U8  bLength;                  //!< Size of this descriptor in bytes
  U8  bDescriptorType;          //!< STRING descriptor type
  U16 wstring[USB_SN_LENGTH];   //!< Unicode characters
}
#ifdef __ICCAVR32__
#pragma pack()
#endif
S_usb_serial_number;


//_____ U S B   D E V I C E   M A S S - S T O R A G E   D E S C R I P T O R _

typedef
#ifdef __ICCAVR32__
#pragma pack(1)
#endif
struct
#ifdef __GNUC__
__attribute__((__packed__))
#endif
{
  S_usb_configuration_descriptor cfg;
  S_usb_interface_descriptor     ifc;
  S_usb_endpoint_descriptor      ep1;
  S_usb_endpoint_descriptor      ep2;
}
#ifdef __ICCAVR32__
#pragma pack()
#endif
S_usb_user_configuration_descriptor;


#endif  // _USB_DESCRIPTORS_H_
