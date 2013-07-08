/*****************************************************************************
 *
 * \file
 *
 * \brief Management of the generic host HID features.
 *
 * This file manages the generic host HID features.
 *
 * Copyright (c) 2009 Atmel Corporation. All rights reserved.
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
 ******************************************************************************/


#ifndef _HOST_HID_H_
#define _HOST_HID_H_


//_____ I N C L U D E S ________________________________________________________

#include "compiler.h"
#include "hid.h"


//_____ M A C R O S ____________________________________________________________


//_____ D E F I N I T I O N S __________________________________________________

//! HID report descriptor item.
typedef struct
{
  uint8_t type;
  uint8_t tag;
  bool long_format;
  union
  {
    hid_short_item_data_t short_data;
    struct
    {
      uint8_t size;
      const void *data;
    } long_data;
  };
} host_hid_item_t;


//_____ D E C L A R A T I O N S ________________________________________________

/*! \name Standard Requests Applied to HID
 */
//! @{

/*! \brief Gets a HID class descriptor.
 *
 * \param descriptor_type Type of the descriptor to get.
 * \param descriptor_index Index of the descriptor to get.
 * \param s_interface The supported interface number of which to get the
 *                    descriptor.
 *
 * \return Status.
 */
extern Status_t host_hid_get_descriptor(uint8_t descriptor_type, uint8_t descriptor_index, uint8_t s_interface);

/*! \brief Sets a HID class descriptor.
 *
 * \param descriptor_type Type of the descriptor to set.
 * \param descriptor_index Index of the descriptor to set.
 * \param s_interface The supported interface number of which to set the
 *                    descriptor.
 * \param length Length of the descriptor to set.
 *
 * \return Status.
 */
extern Status_t host_hid_set_descriptor(uint8_t descriptor_type, uint8_t descriptor_index, uint8_t s_interface, uint16_t length);

//! @}

/*! \name HID-Specific Requests
 */
//! @{

/*! \brief Gets a HID report.
 *
 * \param report_type Type of the report to get.
 * \param report_id ID of the report to get.
 * \param s_interface The supported interface number of which to get the report.
 *
 * \return Status.
 */
extern Status_t host_hid_get_report(uint8_t report_type, uint8_t report_id, uint8_t s_interface);

/*! \brief Sets a HID report.
 *
 * \param report_type Type of the report to set.
 * \param report_id ID of the report to set.
 * \param s_interface The supported interface number of which to set the report.
 * \param length Length of the report to set.
 *
 * \return Status.
 */
extern Status_t host_hid_set_report(uint8_t report_type, uint8_t report_id, uint8_t s_interface, uint16_t length);

/*! \brief Reads the current idle rate for a particular Input report.
 *
 * \param report_id ID of the report of which to get the idle rate.
 * \param s_interface The supported interface number of which to get the idle
 *                    rate.
 *
 * \return Idle duration with a 4-ms resolution, or
 *         \c HID_IDLE_DURATION_INDEFINITE.
 */
extern uint8_t host_hid_get_idle(uint8_t report_id, uint8_t s_interface);

/*! \brief Silences a particular report on the Interrupt In pipe until a new
 *         event occurs or the specified amount of time passes.
 *
 * \param duration_4_ms Idle duration with a 4-ms resolution, or
 *                      \c HID_IDLE_DURATION_INDEFINITE.
 * \param report_id ID of the report of which to set the idle rate (can be
 *                  \c HID_REPORT_ID_ALL).
 * \param s_interface The supported interface number of which to set the idle
 *                    rate.
 *
 * \return Status.
*/
extern Status_t host_hid_set_idle(uint8_t duration_4_ms, uint8_t report_id, uint8_t s_interface);

/*! \brief Reads which protocol is currently active (either the boot protocol or
 *         the report protocol).
 *
 * \param s_interface The supported interface number of which to get the
 *                    protocol.
 *
 * \return Protocol: \c HID_BOOT_PROTOCOL or \c HID_REPORT_PROTOCOL.
 *
 * \note This request is supported by devices in the Boot subclass.
 */
extern uint8_t host_hid_get_protocol(uint8_t s_interface);

/*! \brief Switches between the boot protocol and the report protocol (or vice
 *         versa).
 *
 * \param protocol Protocol: \c HID_BOOT_PROTOCOL or \c HID_REPORT_PROTOCOL.
 * \param s_interface The supported interface number of which to set the
 *                    protocol.
 *
 * \return Status.
 *
 * \note This request is supported by devices in the Boot subclass.
 *
 * \note When initialized, all devices default to the report protocol. However,
 *       the host should not make any assumptions about the device's state and
 *       should set the desired protocol whenever initializing a device.
 */
extern Status_t host_hid_set_protocol(uint8_t protocol, uint8_t s_interface);

//! @}

/*! \name HID Report Descriptor Parsing Functions
 */
//! @{

/*! \brief Gets the next HID report descriptor item.
 *
 * \param item Pointer to the item structure to fill.
 *
 * \return Status.
 *
 * \note \ref host_hid_get_descriptor must have been called before this function
 *       for \c HID_DESCRIPTOR and \c HID_REPORT_DESCRIPTOR.
 */
extern Status_bool_t host_hid_get_item(host_hid_item_t *item);

//! @}


#endif  // _HOST_HID_H_
