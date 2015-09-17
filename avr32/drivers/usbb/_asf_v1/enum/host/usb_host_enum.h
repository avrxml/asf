/**************************************************************************
 *
 * \file
 *
 * \brief Processing of USB host enumeration requests.
 *
 * This file contains the USB control pipe management routines
 * corresponding to the standard enumeration process (refer to chapter 9 of
 * the USB specification).
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


#ifndef _USB_HOST_ENUM_H_
#define _USB_HOST_ENUM_H_


//_____ I N C L U D E S ____________________________________________________

#include "conf_usb.h"

#if USB_HOST_FEATURE == false
  #error usb_host_enum.h is #included although USB_HOST_FEATURE is disabled
#endif


#include "compiler.h"
#include "usb_drv.h"
#include "usb_task.h"


//! @defgroup host_enum USB host enumeration functions module
//! @{


//_____ M A C R O S ________________________________________________________


//_____ S T A N D A R D    D E F I N I T I O N S ___________________________

//! USB Setup Data
typedef struct
{
  U8    bmRequestType;        //!< Characteristics of the request
  U8    bRequest;             //!< Specific request
  U16   wValue;               //!< Field that varies according to request
  U16   wIndex;               //!< Field that varies according to request
  U16   wLength;              //!< Number of bytes to transfer if Data
  bool  incomplete_read;      //!< true: only one read
} S_usb_setup_data;

//! Interface
typedef struct
{
  U8  interface_nb;
  U8  altset_nb;
  U8  nb_ep;
  U8  uclass;
  U8  subclass;
  U8  protocol;
  U8  ep_pipe[MAX_EP_PER_INTERFACE];
} S_interface;

#define CONTROL_GOOD          0x00
#define CONTROL_DATA_TOGGLE   0x01
#define CONTROL_DATA_PID      0x02
#define CONTROL_PID           0x04
#define CONTROL_TIMEOUT       0x08
#define CONTROL_CRC16         0x10
#define CONTROL_STALL         0x20
#define CONTROL_NO_DEVICE     0x40

//! Offsets common to all descriptor types
#define OFFSET_DESCRIPTOR_LENGTH        0
#define OFFSET_FIELD_DESCRIPTOR_TYPE    1

//! Offsets in device descriptors
#define OFFSET_FIELD_MAXPACKETSIZE      7
#define OFFSET_FIELD_VID                8
#define OFFSET_FIELD_PID               10
#define OFFSET_FIELD_NB_CONFIGURATION  17

//! Offsets in configuration descriptors
#define OFFSET_FIELD_TOTAL_LENGTH       2
#define OFFSET_FIELD_NB_INTERFACE       4
#define OFFSET_FIELD_CONFIGURATION_NB   5
#define OFFSET_FIELD_BMATTRIBUTES       7
#define REMOTE_WAKEUP_BIT     5
#define REMOTE_WAKEUP_MASK    (1 << REMOTE_WAKEUP_BIT)
#define SELF_POWERED_BIT      6
#define SELF_POWERED_MASK     (1 << SELF_POWERED_BIT)
#define OFFSET_FIELD_MAXPOWER           8

//! Offsets in interface descriptors
#define OFFSET_FIELD_INTERFACE_NB       2
#define OFFSET_FIELD_ALT                3
#define OFFSET_FIELD_NB_OF_EP           4
#define OFFSET_FIELD_CLASS              5
#define OFFSET_FIELD_SUB_CLASS          6
#define OFFSET_FIELD_PROTOCOL           7

//! Offsets in endpoint descriptors
#define OFFSET_FIELD_EP_ADDR            2
#define OFFSET_FIELD_EP_TYPE            3
#define OFFSET_FIELD_EP_SIZE            4
#define OFFSET_FIELD_EP_INTERVAL        6

//! @brief Send a "clear endpoint feature" request
//! @param ep U8: Target endpoint
//! @return Status
#define host_clear_endpoint_feature(ep) \
(\
  usb_request.bmRequestType   = 0x02,\
  usb_request.bRequest        = CLEAR_FEATURE,\
  usb_request.wValue          = FEATURE_ENDPOINT_HALT << 8,\
  usb_request.wIndex          = (ep),\
  usb_request.wLength         = 0,\
  usb_request.incomplete_read = false,\
  host_transfer_control(data_stage)\
)

//! @brief Send a "get configuration" request
//! @return Status
#define host_get_configuration() \
(\
  usb_request.bmRequestType   = 0x80,\
  usb_request.bRequest        = GET_CONFIGURATION,\
  usb_request.wValue          = 0,\
  usb_request.wIndex          = 0,\
  usb_request.wLength         = 1,\
  usb_request.incomplete_read = false,\
  host_transfer_control(data_stage)\
)

//! @brief Send a "set configuration" request
//! @param cfg_nb U8: Configuration to activate
//! @return Status
#define host_set_configuration(cfg_nb) \
(\
  usb_request.bmRequestType   = 0x00,\
  usb_request.bRequest        = SET_CONFIGURATION,\
  usb_request.wValue          = (cfg_nb),\
  usb_request.wIndex          = 0,\
  usb_request.wLength         = 0,\
  usb_request.incomplete_read = false,\
  host_transfer_control(data_stage)\
)

//! @brief Send a "set interface" request to specify an alternate setting for an interface
//! @param interface_nb U8: Interface
//! @param alt_setting U8: Alternate setting
//! @return Status
#define host_set_interface(interface_nb, alt_setting) \
(\
  usb_request.bmRequestType   = 0x01,\
  usb_request.bRequest        = SET_INTERFACE,\
  usb_request.wValue          = (alt_setting),\
  usb_request.wIndex          = (interface_nb),\
  usb_request.wLength         = 0,\
  usb_request.incomplete_read = false,\
  host_transfer_control(data_stage)\
)

//! @brief Send an incomplete "get device descriptor" request
//! The descriptor received is stored in the data_stage array.
//! The received descriptors are limited to the length of the control pipe.
//! @return Status
#define host_get_device_descriptor_incomplete() \
(\
  usb_request.bmRequestType   = 0x80,\
  usb_request.bRequest        = GET_DESCRIPTOR,\
  usb_request.wValue          = DEVICE_DESCRIPTOR << 8,\
  usb_request.wIndex          = 0,\
  usb_request.wLength         = 64,\
  usb_request.incomplete_read = true,\
  host_transfer_control(data_stage)\
)

//! @brief Send a "get device descriptor" request
//! The descriptor received is stored in the data_stage array.
//! @return Status
#define host_get_device_descriptor() \
(\
  usb_request.bmRequestType   = 0x80,\
  usb_request.bRequest        = GET_DESCRIPTOR,\
  usb_request.wValue          = DEVICE_DESCRIPTOR << 8,\
  usb_request.wIndex          = 0,\
  usb_request.wLength         = 18,\
  usb_request.incomplete_read = false,\
  host_transfer_control(data_stage)\
)

//! @brief Send a "get device configuration descriptor" request
//! The configuration descriptor received is stored in the data_stage array.
//! @param cfg_ix U8: Index of the configuration descriptor to get
//! @return Status
#define host_get_configuration_descriptor(cfg_ix) \
(\
  usb_request.bmRequestType   = 0x80,\
  usb_request.bRequest        = GET_DESCRIPTOR,\
  usb_request.wValue          = CONFIGURATION_DESCRIPTOR << 8 | (cfg_ix),\
  usb_request.wIndex          = 0,\
  usb_request.wLength         = SIZEOF_DATA_STAGE,\
  usb_request.incomplete_read = false,\
  host_transfer_control(data_stage)\
)

//! @brief Send a "set address" request
//! @param addr U8: Address assigned to the device
//! @return Status
#define host_set_address(addr) \
(\
  usb_request.bmRequestType   = 0x00,\
  usb_request.bRequest        = SET_ADDRESS,\
  usb_request.wValue          = (addr),\
  usb_request.wIndex          = 0,\
  usb_request.wLength         = 0,\
  usb_request.incomplete_read = false,\
  host_transfer_control(data_stage)\
)

//! @brief Send a "set feature" "device remote wake-up"
//! @return Status
#define host_set_feature_remote_wakeup() \
(\
  usb_request.bmRequestType   = 0x00,\
  usb_request.bRequest        = SET_FEATURE,\
  usb_request.wValue          = 1,\
  usb_request.wIndex          = 1,\
  usb_request.wLength         = 0,\
  usb_request.incomplete_read = false,\
  host_transfer_control(data_stage)\
)

//! @brief Send the mass-storage specific request "get max LUN"
//! @return Status
#define host_ms_get_max_lun() \
(\
  usb_request.bmRequestType   = 0xA1,\
  usb_request.bRequest        = MS_GET_MAX_LUN,\
  usb_request.wValue          = 0,\
  usb_request.wIndex          = 0,\
  usb_request.wLength         = 1,\
  usb_request.incomplete_read = false,\
  host_transfer_control(data_stage)\
)

//! @brief VID of the connected device
//! @return U16: VID
#define Get_VID()                           (device_VID)

//! @brief PID of the connected device
//! @return U16: PID
#define Get_PID()                           (device_PID)

//! @brief true if the connected device supports remote wake-up
//! @return bool: Remote wake-up supported?
#define Is_device_supports_remote_wakeup()  (Tst_bits(bmattributes, REMOTE_WAKEUP_MASK))

//! @brief true if the connected device is self-powered
//! @return bool: Self-powered?
#define Is_device_self_powered()            (Tst_bits(bmattributes, SELF_POWERED_MASK))

//! @brief Maximal power consumption ot the connected device (unit is 2 mA)
//! @return U8: Maximal power
#define Get_maxpower()                      (maxpower)

//! @brief Number of supported interfaces in the connected device
//! @return U8: Number of supported interfaces
#define Get_nb_supported_interface()        (nb_interface_supported)

//! @brief Number of interfaces associated with a supported interface
//! @param s_interface U8: The supported interface number
//! @return U8: Number of interfaces
#define Get_interface_number(s_interface)   (interface_supported[(s_interface)].interface_nb)

//! @brief Number of alternate settings associated with a supported interface
//! @param s_interface U8: The supported interface number
//! @return U8: Number of alternate settings
#define Get_altset_nb(s_interface)          (interface_supported[(s_interface)].altset_nb)

//! @brief USB class associated with the supported interface
//! @param s_interface U8: The supported interface number
//! @return U8: Class
#define Get_class(s_interface)              (interface_supported[(s_interface)].uclass)

//! @brief USB subclass associated with the supported interface
//! @param s_interface U8: The supported interface number
//! @return U8: Subclass
#define Get_subclass(s_interface)           (interface_supported[(s_interface)].subclass)

//! @brief USB protocol associated with the supported interface
//! @param s_interface U8: The supported interface number
//! @return U8: protocol
#define Get_protocol(s_interface)           (interface_supported[(s_interface)].protocol)

//! @brief Number of endpoints associated with a supported interface
//! @param s_interface U8: The supported interface number
//! @return U8: Number of endpoints
#define Get_nb_ep(s_interface)              (interface_supported[(s_interface)].nb_ep)

//! @brief Endpoint pipe associated with the supported interface
//! @param s_interface U8: The supported interface number
//! @param n_ep U8: The endpoint number in this interface
//! @return U8: Endpoint pipe
#define Get_ep_pipe(s_interface, n_ep)      (interface_supported[(s_interface)].ep_pipe[(n_ep)])

//! @brief Endpoint number associated with the supported interface
//! @param s_interface U8: The supported interface number
//! @param n_ep U8: The endpoint number in this interface
//! @return U8: Endpoint number
#define Get_ep_nbr(s_interface, n_ep)       (Host_get_pipe_endpoint_number(Get_ep_pipe(s_interface, n_ep)))

//! @brief Endpoint type associated with the supported interface
//! @param s_interface U8: The supported interface number
//! @param n_ep U8: The endpoint number in this interface
//! @return U8: Endpoint type
#define Get_ep_type(s_interface, n_ep)      (Host_get_pipe_type(Get_ep_pipe(s_interface, n_ep)))

//! @brief true if the endpoint direction associated with the supported interface is IN
//! @param s_interface U8: The supported interface number
//! @param n_ep U8: The endpoint number in this interface
//! @return bool: Endpoint direction IN?
#define Is_ep_in(s_interface, n_ep)         (Host_get_pipe_token(Get_ep_pipe(s_interface, n_ep)) == TOKEN_IN)

//! @brief Extract token information from endpoint address
//! @param ep_addr U8: Endpoint address
//! @return TOKEN_IN/TOKEN_OUT: Pipe token
#define Get_pipe_token(ep_addr)             ((Get_desc_ep_dir(ep_addr)) ? TOKEN_IN : TOKEN_OUT)

extern bool host_check_VID_PID(void);
extern bool host_check_class(void);
extern Status_t host_transfer_control(void *data_pointer);

extern volatile U16 device_VID;
extern volatile U16 device_PID;
extern volatile U8 bmattributes;
extern volatile U8 maxpower;
extern volatile U8 nb_interface_supported;
extern volatile S_interface interface_supported[MAX_INTERFACE_SUPPORTED];


//! @}


#endif  // _USB_HOST_ENUM_H_
