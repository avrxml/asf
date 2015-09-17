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


//_____ I N C L U D E S ____________________________________________________

#include "conf_usb.h"

#if USB_HOST_FEATURE == true

#include "compiler.h"
#include "usb_drv.h"
#include "usb_host_enum.h"
#include "usb_host_task.h"
#include "usb_task.h"

//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________


//_____ P R I V A T E   D E C L A R A T I O N S ____________________________

//! Table of registered devices (see conf_usb.h for table contents)
static const uint16_t registered_VID_PID[] = VID_PID_TABLE;
#define REG_VID_PID_CNT   (sizeof(registered_VID_PID) / sizeof(registered_VID_PID[0]))

//! Table of registered classes (see conf_usb.h for table contents)
static const uint8_t  registered_class[]   = CLASS_SUBCLASS_PROTOCOL;
#define REG_CLASS_CNT     (sizeof(registered_class) / sizeof(registered_class[0]))


//_____ D E C L A R A T I O N S ____________________________________________

//! VID of connected device
volatile uint16_t device_VID;

//! PID of connected device
volatile uint16_t device_PID;

//! bmAttributes byte of connected device
volatile uint8_t bmattributes;

//! maxpower byte of connected device (unit is 2 mA)
volatile uint8_t maxpower;

//! Number of interfaces the host is able to support in the connected device
volatile uint8_t nb_interface_supported = 0;

//! Supported interfaces
volatile S_interface interface_supported[MAX_INTERFACE_SUPPORTED];

extern uint32_t  host_get_timeout( void );

//! This function checks if the VID and the PID are supported
//! (if the VID & PID belong to the VID_PID table).
//!
//! @return bool: Status
//!
bool host_check_VID_PID(void)
{
  uint8_t c, d;

  // Rebuild VID & PID from data stage
  device_VID = usb_format_usb_to_mcu_data(16, *(uint16_t *)(data_stage + OFFSET_FIELD_VID));
  device_PID = usb_format_usb_to_mcu_data(16, *(uint16_t *)(data_stage + OFFSET_FIELD_PID));

  // Look for received VID & PID in supported table
  for (c = 0; c < REG_VID_PID_CNT; )
  {
    if (registered_VID_PID[c] == device_VID)  // VID is correct
    {
      for (c += 2, d = c + registered_VID_PID[c - 1]; c < d; c++)
      {
        if (registered_VID_PID[c] == device_PID) return true; // PID is correct
      }
    }
    else c += 2 + registered_VID_PID[c + 1];
  }

  return false;
}


//! This function checks if the device class is supported.
//! The function looks in all interfaces declared in the received descriptors if
//! one of them matches an entry of the CLASS/SUB_CLASS/PROTOCOL table.
//! If HOST_AUTO_CFG_ENDPOINT is enabled, a pipe is configured for each endpoint
//! of supported interfaces.
//!
//! @return bool: Status
//!
bool host_check_class(void)
{
  uint8_t *descriptor, *conf_end;
  uint8_t device_class, device_subclass, device_protocol;
  uint8_t c;
#if HOST_AUTO_CFG_ENDPOINT == ENABLE
  uint8_t nb_endpoint_to_configure = 0;
  uint8_t ep_index = 0;
  uint8_t physical_pipe = P_1;   // P_1 because physical pipe 0 is reserved for control
  uint16_t ep_size;

  // By default, the host is configured when returning
  Host_set_configured();
#endif

  // First, assume no interface is supported
  nb_interface_supported = 0;

  // Check if configuration descriptor
  if (data_stage[OFFSET_FIELD_DESCRIPTOR_TYPE] != CONFIGURATION_DESCRIPTOR) return false;

  bmattributes = data_stage[OFFSET_FIELD_BMATTRIBUTES];
  maxpower     = data_stage[OFFSET_FIELD_MAXPOWER];

  conf_end = data_stage +
             min(usb_format_usb_to_mcu_data(16, *(uint16_t *)(data_stage + OFFSET_FIELD_TOTAL_LENGTH)),
                 SIZEOF_DATA_STAGE - OFFSET_FIELD_PROTOCOL);

  // Look in all interfaces declared in the configuration
  for (descriptor = data_stage + data_stage[OFFSET_DESCRIPTOR_LENGTH]; descriptor < conf_end;
       descriptor += descriptor[OFFSET_DESCRIPTOR_LENGTH])
  {
    // Find next interface descriptor
    switch (descriptor[OFFSET_FIELD_DESCRIPTOR_TYPE])
    {
    case INTERFACE_DESCRIPTOR:
      // Check the number of supported interfaces does not exceed the maximum
      if (nb_interface_supported >= MAX_INTERFACE_SUPPORTED) return true;

#if HOST_AUTO_CFG_ENDPOINT == ENABLE
      // If there are still endpoints to configure although a new interface descriptor has been found
      if (nb_endpoint_to_configure)
      {
        // Mark the host as not configured
        Host_clear_configured();

        // Reset the number of endpoints to configure
        nb_endpoint_to_configure = 0;
      }
#endif

      // Found an interface descriptor
      // Get characteristics of this interface
      device_class    = descriptor[OFFSET_FIELD_CLASS];
      device_subclass = descriptor[OFFSET_FIELD_SUB_CLASS];
      device_protocol = descriptor[OFFSET_FIELD_PROTOCOL];

      // Look in registered class table for match
      for (c = 0; c < REG_CLASS_CNT; c += 3)
      {
        if (registered_class[c]     == device_class    &&   // Class is correct
            registered_class[c + 1] == device_subclass &&   // Subclass is correct
            registered_class[c + 2] == device_protocol)     // Protocol is correct
        {
          // Store this interface as supported interface
          // Memorize its interface nb
          interface_supported[nb_interface_supported].interface_nb = descriptor[OFFSET_FIELD_INTERFACE_NB];
          //          its alternate setting
          interface_supported[nb_interface_supported].altset_nb    = descriptor[OFFSET_FIELD_ALT];
          //          its USB class
          interface_supported[nb_interface_supported].uclass        = device_class;
          //          its USB subclass
          interface_supported[nb_interface_supported].subclass     = device_subclass;
          //          its USB protocol
          interface_supported[nb_interface_supported].protocol     = device_protocol;
          //          the number of endpoints associated with this interface
#if HOST_AUTO_CFG_ENDPOINT == ENABLE
          ep_index = 0;
          nb_endpoint_to_configure =
#endif
          interface_supported[nb_interface_supported].nb_ep        = min(descriptor[OFFSET_FIELD_NB_OF_EP], MAX_EP_PER_INTERFACE);

          // Update the number of supported interfaces
          nb_interface_supported++;

          // Class/subclass/protocol is registered, so look for next interface descriptor
          break;
        }
      }
      break;

#if HOST_AUTO_CFG_ENDPOINT == ENABLE
    case ENDPOINT_DESCRIPTOR:
      // If there are still endpoints to configure while there are free pipes
      if (physical_pipe < MAX_PEP_NB && nb_endpoint_to_configure)
      {
        nb_endpoint_to_configure--;

        // Reconfigure the new physical pipe to get rid of any previous configuration
  #if USB_HOST_PIPE_INTERRUPT_TRANSFER == ENABLE
        cpu_irq_disable();
  #endif
        Host_disable_pipe(physical_pipe);
  #if USB_HOST_PIPE_INTERRUPT_TRANSFER == ENABLE
        (void)Is_host_pipe_enabled(physical_pipe);
        cpu_irq_enable();
  #endif
        Host_unallocate_memory(physical_pipe);
        Host_enable_pipe(physical_pipe);

        // Fix HW, set freq at 0 in case of no interrupt endpoint
        if( TYPE_INTERRUPT != descriptor[OFFSET_FIELD_EP_TYPE] ) descriptor[OFFSET_FIELD_EP_INTERVAL] = 0;

        ep_size = descriptor[OFFSET_FIELD_EP_SIZE] |
                  descriptor[OFFSET_FIELD_EP_SIZE + 1] << 8;
#if BOARD != EVK1104
        if (ep_size <= 64)
        {
#endif
          // Build the pipe configuration according to the endpoint descriptor fields received
          (void)Host_configure_pipe(
                  physical_pipe,                                      // Pipe nb in USB interface
                  descriptor[OFFSET_FIELD_EP_INTERVAL],               // Interrupt period (for interrupt pipe)
                  Get_desc_ep_nbr(descriptor[OFFSET_FIELD_EP_ADDR]),  // Pipe endpoint number
                  descriptor[OFFSET_FIELD_EP_TYPE],                   // Pipe type (isochronous/bulk/interrupt)
                  Get_pipe_token(descriptor[OFFSET_FIELD_EP_ADDR]),   // Pipe token (IN/OUT)
                  ep_size,                                            // Pipe size
                  (descriptor[OFFSET_FIELD_EP_TYPE] == TYPE_BULK) ? SINGLE_BANK : DOUBLE_BANK // Number of banks to allocate for pipe
                );

          // Update endpoint pipe table in supported interface structure
          interface_supported[nb_interface_supported - 1].ep_pipe[ep_index++] = physical_pipe++;
        }
#if BOARD != EVK1104
        else
        {
          // Build the pipe configuration according to the endpoint descriptor fields received
          (void)Host_configure_pipe(
                  MAX_PEP_NB - 1,                                     // Pipe nb in USB interface
                  descriptor[OFFSET_FIELD_EP_INTERVAL],               // Interrupt period (for interrupt pipe)
                  Get_desc_ep_nbr(descriptor[OFFSET_FIELD_EP_ADDR]),  // Pipe endpoint number
                  descriptor[OFFSET_FIELD_EP_TYPE],                   // Pipe type (isochronous/bulk/interrupt)
                  Get_pipe_token(descriptor[OFFSET_FIELD_EP_ADDR]),   // Pipe token (IN/OUT)
                  ep_size,                                            // Pipe size
                  (descriptor[OFFSET_FIELD_EP_TYPE] == TYPE_BULK) ? SINGLE_BANK : DOUBLE_BANK // Number of banks to allocate for pipe
                );

          // Update endpoint pipe table in supported interface structure
          interface_supported[nb_interface_supported - 1].ep_pipe[ep_index++] = MAX_PEP_NB - 1;
        }
      }
#endif
      break;
#endif
    }

    // Call user callback to look more deeply into the configuration descriptor
    Host_user_check_class_action(descriptor);
  }

#if HOST_AUTO_CFG_ENDPOINT == ENABLE
  // If there are still endpoints to configure although all descriptors have been parsed
  if (nb_endpoint_to_configure)
  {
    // Mark the host as not configured
    Host_clear_configured();
  }
#endif

  return (nb_interface_supported > 0);
}


//! This function is the generic control pipe management function.
//! This function is used to send and receive control requests over control pipe.
//!
//! @todo Fix all time-out errors and disconnections in active wait loop.
//!
//! @param data_pointer void *: Pointer to data to transfer
//!
//! @return Status_t: Status
//!
//! @note This function uses the usb_request global structure. Hence, this
//! structure should be filled before calling this function.
//!
Status_t host_transfer_control(void *data_pointer)
{
  int status = CONTROL_GOOD;
  bool sav_int_sof_enable;
  bool sav_glob_int_en;
  uint16_t data_length;
  uint8_t c;

  Usb_ack_event(EVT_HOST_SOF);
  sav_int_sof_enable = Is_host_sof_interrupt_enabled();
  Host_enable_sof_interrupt();                // SOF software detection is in interrupt subroutine
  while (!Is_usb_event(EVT_HOST_SOF))         // Wait 1 SOF
  {
#if defined(Host_wait_action)
    Host_wait_action();
#endif
    if (Is_host_emergency_exit())
    {
      Host_freeze_pipe(P_CONTROL);
      Host_reset_pipe(P_CONTROL);
      status = CONTROL_TIMEOUT;
      goto host_transfer_control_end;
    }
  }
  Host_configure_pipe_token(P_CONTROL, TOKEN_SETUP);
  Host_ack_setup_ready();
  Host_unfreeze_pipe(P_CONTROL);

  // Build and send the setup request fields
  Host_reset_pipe_fifo_access(P_CONTROL);
  Host_write_pipe_data(P_CONTROL, 8, usb_request.bmRequestType);
  Host_write_pipe_data(P_CONTROL, 8, usb_request.bRequest);
  Host_write_pipe_data(P_CONTROL, 16, usb_format_mcu_to_usb_data(16, usb_request.wValue));
  Host_write_pipe_data(P_CONTROL, 16, usb_format_mcu_to_usb_data(16, usb_request.wIndex));
  Host_write_pipe_data(P_CONTROL, 16, usb_format_mcu_to_usb_data(16, usb_request.wLength));
  Host_send_setup();

  while (!Is_host_setup_ready())  // Wait for SETUP ack
  {
#if defined(Host_wait_action)
    Host_wait_action();
#endif
    if (Is_host_emergency_exit())
    {
      Host_freeze_pipe(P_CONTROL);
      Host_reset_pipe(P_CONTROL);
      status = CONTROL_TIMEOUT;
      goto host_transfer_control_end;
    }
    if (Is_host_pipe_error(P_CONTROL))  // Any error?
    {
      c = Host_error_status(P_CONTROL);
      Host_ack_all_errors(P_CONTROL);
      status = c;   // Send error status
      goto host_transfer_control_end;
    }
  }

  // Setup token sent; now send IN or OUT token
  // Before just wait 1 SOF
  Usb_ack_event(EVT_HOST_SOF);
  Host_freeze_pipe(P_CONTROL);
  data_length = usb_request.wLength;
  while (!Is_usb_event(EVT_HOST_SOF))         // Wait 1 SOF
  {
#if defined(Host_wait_action)
    Host_wait_action();
#endif
    if (Is_host_emergency_exit())
    {
      Host_freeze_pipe(P_CONTROL);
      Host_reset_pipe(P_CONTROL);
      status = CONTROL_TIMEOUT;
      goto host_transfer_control_end;
    }
  }

  // IN request management ---------------------------------------------
  if (usb_request.bmRequestType & 0x80)       // Data stage IN (bmRequestType.D7 == 1)
  {
    Host_disable_continuous_in_mode(P_CONTROL);
    Host_configure_pipe_token(P_CONTROL, TOKEN_IN);
    Host_ack_control_in_received_free();
    while (data_length)
    {
      Host_unfreeze_pipe(P_CONTROL);
      private_sof_counter = 0;        // Reset the counter in SOF detection subroutine
      while (!Is_host_control_in_received())
      {
#if defined(Host_wait_action)
        Host_wait_action();
#endif
        if (Is_host_emergency_exit())
        {
          Host_freeze_pipe(P_CONTROL);
          Host_reset_pipe(P_CONTROL);
          status = CONTROL_TIMEOUT;
          goto host_transfer_control_end;
        }
        if (Is_host_pipe_error(P_CONTROL))  // Any error?
        {
          c = Host_error_status(P_CONTROL);
          Host_ack_all_errors(P_CONTROL);
          status = c;   // Send error status
          goto host_transfer_control_end;
        }
        if (Is_host_stall(P_CONTROL))
        {
          Host_ack_stall(P_CONTROL);
          status = CONTROL_STALL;
          goto host_transfer_control_end;
        }
#if TIMEOUT_DELAY_ENABLE == ENABLE
      if (1000 < host_get_timeout()) // Count 1s
      {
          Host_freeze_pipe(P_CONTROL);
          Host_reset_pipe(P_CONTROL);
          status = CONTROL_TIMEOUT;
          goto host_transfer_control_end;
      }
#endif
      }
      Host_reset_pipe_fifo_access(P_CONTROL);
      c = Host_get_pipe_size(P_CONTROL) - Host_byte_count(P_CONTROL);
      data_length = host_read_p_rxpacket(P_CONTROL, data_pointer, data_length, &data_pointer);
      if (usb_request.incomplete_read || c) data_length = 0;
      Host_freeze_pipe(P_CONTROL);
      Host_ack_control_in_received_free();

      // In low-speed mode, the USB IP may have not yet sent the ACK at this
      // point. The USB IP does not support a new start of transaction request
      // from the firmware if the ACK has not been sent. The only means of
      // making sure the ACK has been sent is to wait for the next Keep-Alive
      // before starting a new transaction.
      if (Is_usb_low_speed_mode())
      {
        Usb_ack_event(EVT_HOST_SOF);
        if ((sav_glob_int_en = cpu_irq_is_enabled())) cpu_irq_disable();
        Host_ack_sof();
        (void)Is_host_sof_interrupt_enabled();
        if (sav_glob_int_en) cpu_irq_enable();
        while (!Is_usb_event(EVT_HOST_SOF))         // Wait for next Keep-Alive
        {
          if (Is_host_emergency_exit())
          {
            Host_freeze_pipe(P_CONTROL);
            Host_reset_pipe(P_CONTROL);
            status = CONTROL_TIMEOUT;
            goto host_transfer_control_end;
        }
        }
      }
    }                                 // End of IN data stage

    Host_configure_pipe_token(P_CONTROL, TOKEN_OUT);
    Host_ack_control_out_ready_send();
    Host_unfreeze_pipe(P_CONTROL);
#if TIMEOUT_DELAY_ENABLE == ENABLE
    private_sof_counter = 0;        // Reset the counter in SOF detection subroutine
#endif
    while (!Is_host_control_out_ready())
    {
#if defined(Host_wait_action)
      Host_wait_action();
#endif
      if (Is_host_emergency_exit())
      {
        Host_freeze_pipe(P_CONTROL);
        Host_reset_pipe(P_CONTROL);
        status = CONTROL_TIMEOUT;
        goto host_transfer_control_end;
      }
      if (Is_host_pipe_error(P_CONTROL))  // Any error?
      {
        c = Host_error_status(P_CONTROL);
        Host_ack_all_errors(P_CONTROL);
        status = c;   // Send error status
        goto host_transfer_control_end;
      }
      if (Is_host_stall(P_CONTROL))
      {
        Host_ack_stall(P_CONTROL);
        status = CONTROL_STALL;
        goto host_transfer_control_end;
      }
#if TIMEOUT_DELAY_ENABLE == ENABLE
      if (2000 < host_get_timeout()) // Count 2s
      {
          Host_freeze_pipe(P_CONTROL);
          Host_reset_pipe(P_CONTROL);
          status = CONTROL_TIMEOUT;
          goto host_transfer_control_end;
      }
#endif
    }
    Host_ack_control_out_ready();
  }

  // OUT request management --------------------------------------------
  else                                        // Data stage OUT (bmRequestType.D7 == 0)
  {
    Host_configure_pipe_token(P_CONTROL, TOKEN_OUT);
    Host_ack_control_out_ready();
    while (data_length)
    {
      Host_unfreeze_pipe(P_CONTROL);
      Host_reset_pipe_fifo_access(P_CONTROL);
      data_length = host_write_p_txpacket(P_CONTROL, data_pointer, data_length, (const void **)&data_pointer);
      Host_send_control_out();
      while (!Is_host_control_out_ready())
      {
#if defined(Host_wait_action)
        Host_wait_action();
#endif
        if (Is_host_emergency_exit())
        {
          Host_freeze_pipe(P_CONTROL);
          Host_reset_pipe(P_CONTROL);
          status = CONTROL_TIMEOUT;
          goto host_transfer_control_end;
        }
        if (Is_host_pipe_error(P_CONTROL))  // Any error?
        {
          c = Host_error_status(P_CONTROL);
          Host_ack_all_errors(P_CONTROL);
          status = c;   // Send error status
          goto host_transfer_control_end;
        }
        if (Is_host_stall(P_CONTROL))
        {
          Host_ack_stall(P_CONTROL);
          status = CONTROL_STALL;
          goto host_transfer_control_end;
        }
      }
      Host_ack_control_out_ready();
    }                                 // End of OUT data stage

    Host_freeze_pipe(P_CONTROL);
    Host_configure_pipe_token(P_CONTROL, TOKEN_IN);
    Host_ack_control_in_received_free();
    Host_unfreeze_pipe(P_CONTROL);
#if TIMEOUT_DELAY_ENABLE == ENABLE
    private_sof_counter = 0;        // Reset the counter in SOF detection subroutine
#endif
    while (!Is_host_control_in_received())
    {
#if defined(Host_wait_action)
      Host_wait_action();
#endif
      if (Is_host_emergency_exit())
      {
        Host_freeze_pipe(P_CONTROL);
        Host_reset_pipe(P_CONTROL);
        status = CONTROL_TIMEOUT;
        goto host_transfer_control_end;
      }
      if (Is_host_pipe_error(P_CONTROL))  // Any error?
      {
        c = Host_error_status(P_CONTROL);
        Host_ack_all_errors(P_CONTROL);
        status = c;   // Send error status
        goto host_transfer_control_end;
      }
      if (Is_host_stall(P_CONTROL))
      {
        Host_ack_stall(P_CONTROL);
        status = CONTROL_STALL;
        goto host_transfer_control_end;
      }
#if TIMEOUT_DELAY_ENABLE == ENABLE
      if (2000 < host_get_timeout()) // Count 2s
      {
          Host_freeze_pipe(P_CONTROL);
          Host_reset_pipe(P_CONTROL);
          status = CONTROL_TIMEOUT;
          goto host_transfer_control_end;
      }
#endif
    }
    Host_ack_control_in_received();
    Host_freeze_pipe(P_CONTROL);
    Host_free_control_in();
  }

host_transfer_control_end:
  if (!sav_int_sof_enable)                    // Restore SOF interrupt enable
  {
    if ((sav_glob_int_en = cpu_irq_is_enabled())) cpu_irq_disable();
    Host_disable_sof_interrupt();
    (void)Is_host_sof_interrupt_enabled();
    if (sav_glob_int_en) cpu_irq_enable();
  }

  return status;
}


#endif  // USB_HOST_FEATURE == true
