/**************************************************************************
 *
 * \file
 *
 * \brief Management of the USB host controller.
 *
 * This file manages the host controller, the host enumeration process and
 * the suspend/resume host requests.
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

#include "conf_usb.h"


#if USB_HOST_FEATURE == true

#include <avr32/io.h>
#include "preprocessor.h"
#ifdef FREERTOS_USED
#include "FreeRTOS.h"
#include "task.h"
#endif
#include "usb_drv.h"
#include "usb_task.h"
#include "usb_host_enum.h"
#include "usb_host_task.h"
#ifdef HOST_VBUS_LOW_TIMEOUT
# include "cycle_counter.h"
#endif

//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

static const char log_device_connected[]    = "Device connected\r\n";
static const char log_unsupported_device[]  = "Unsupported device\r\n";
static const char log_device_enumerated[]   = "Device enumerated\r\n";
static const char log_usb_suspended[]       = "USB suspended\r\n";
static const char log_usb_resumed[]         = "USB resumed\r\n";

#if USB_HOST_PIPE_INTERRUPT_TRANSFER == ENABLE
volatile bool g_sav_int_sof_enable;
volatile S_pipe_int it_pipe_str[MAX_PEP_NB];
#endif

//!
//! Public: uint8_t device_state
//! Its value represents the current state of the
//! device connected to the USB host controller
//! Value can be:
//! - DEVICE_ATTACHED
//! - DEVICE_POWERED
//! - DEVICE_SUSPENDED
//! - DEVICE_DEFAULT
//! - DEVICE_ADDRESSED
//! - DEVICE_CONFIGURED
//! - DEVICE_ERROR
//! - DEVICE_UNATTACHED
//! - DEVICE_READY
//! - DEVICE_WAIT_RESUME
//!
volatile uint8_t device_state;

//! For control requests management over control pipe
volatile S_usb_setup_data usb_request;

//!
//! Public: uint8_t data_stage[SIZEOF_DATA_STAGE]
//! Internal RAM buffer for USB data stage content
//! This buffer is required to setup host enumeration process
//! It contains the device descriptors received.
//! Depending on the device descriptors length, its size can be optimized
//! with the SIZEOF_DATA_STAGE define of conf_usb.h file
//!
uint8_t data_stage[SIZEOF_DATA_STAGE];

volatile uint8_t usb_host_device_status;

volatile bool request_resume;

//! As internal host Start-of-Frame counter
static uint16_t sof_cnt;

#ifdef FREERTOS_USED
//! Handle to the USB Host task
xTaskHandle usb_host_tsk = NULL;
#endif


//_____ D E C L A R A T I O N S ____________________________________________

//!
//! @brief This function initializes the USB host controller.
//!
//! This function enables the USB controller for host-mode operation.
//!
void usb_host_task_init(void)
{
  //! @todo Implement this on the silicon version
  //Pll_start_auto();
  //Wait_pll_ready();
  cpu_irq_disable();
  Usb_disable();
  (void)Is_usb_enabled();
  cpu_irq_enable();
  Usb_disable_otg_pad();
  Usb_enable_otg_pad();
  Usb_enable();
  Usb_unfreeze_clock();
  (void)Is_usb_clock_frozen();
#if USB_VBOF_ACTIVE_LEVEL == HIGH
  Usb_set_vbof_active_high();
#else // USB_VBOF_ACTIVE_LEVEL == LOW
  Usb_set_vbof_active_low();
#endif
  Usb_output_vbof_pin();
  Usb_disable_vbus_hw_control();  // Force VBus generation without time-out
  Host_enable_device_disconnection_interrupt();
  Host_enable_device_connection_interrupt();
#if USB_HOST_PIPE_INTERRUPT_TRANSFER == ENABLE
  reset_it_pipe_str();
#endif
  device_state = DEVICE_UNATTACHED;
  Host_clear_device_status();
  Host_ack_request_resume();
  sof_cnt = 0;

#ifdef FREERTOS_USED
  xTaskCreate(usb_host_task,
              configTSK_USB_HST_NAME,
              configTSK_USB_HST_STACK_SIZE,
              NULL,
              configTSK_USB_HST_PRIORITY,
              &usb_host_tsk);
#endif  // FREERTOS_USED
}


//!
//! @brief Entry point of the USB host management
//!
//! The aim is to manage the target device connection and enumeration.
//! Depending on device_state, the function performs the required operations
//! to get the device enumerated and configured.
//! Once the device is operational, device_state is DEVICE_READY.
//! This state should be tested by the host task before sending any
//! applicative request to the device.
//! This function is called from the usb_task function depending on the USB
//! operating mode (device or host) currently engaged.
//!
//! \image html usb_host_task.jpg "USB Host Task Overview"
//!
#ifdef FREERTOS_USED
void usb_host_task(void *pvParameters)
#else
void usb_host_task(void)
#endif
{
  #define DEVICE_DEFAULT_MAX_ERROR_COUNT  2
  static uint8_t device_default_error_count;

#ifdef HOST_VBUS_LOW_TIMEOUT
  extern t_cpu_time timer_vbus_low;
#endif
  static bool sav_int_sof_enable;
  uint8_t pipe;

#ifdef FREERTOS_USED
  portTickType xLastWakeTime;

  xLastWakeTime = xTaskGetTickCount();
  while (true)
  {
    vTaskDelayUntil(&xLastWakeTime, configTSK_USB_HST_PERIOD);

#endif  // FREERTOS_USED
    switch (device_state)
    {
#ifdef HOST_VBUS_LOW_TIMEOUT
    case DEVICE_VBUS_LOW:
      Usb_disable_vbus();
      if (cpu_is_timeout(&timer_vbus_low))
        usb_host_task_init();
      break;
#endif

    //------------------------------------------------------
    //   DEVICE_UNATTACHED state
    //
    //   - Default init state
    //   - Try to give device power supply
    //
    case DEVICE_UNATTACHED:
      device_default_error_count = 0;
      nb_interface_supported = 0;
      Host_clear_device_status();     // Reset device status
      Usb_clear_all_event();          // Clear all software events
      Host_disable_sof();
      host_disable_all_pipes();
      Usb_enable_vbus();              // Give at least device power supply!
      // If VBus OK, wait for device connection
      if (Is_usb_vbus_high())
        device_state = DEVICE_ATTACHED;
      break;

    //------------------------------------------------------
    //   DEVICE_ATTACHED state
    //
    //   - VBus is on
    //   - Try to detect device connection
    //
    case DEVICE_ATTACHED:
      if (Is_host_device_connection() || Is_usb_event(EVT_HOST_CONNECTION) )  // Device pull-up detected
      {
device_attached_retry:
        if( Is_usb_event(EVT_HOST_CONNECTION) ) {
          Usb_ack_event(EVT_HOST_CONNECTION);
        }
        Usb_ack_bconnection_error_interrupt();
        Usb_ack_vbus_error_interrupt();
        Host_ack_device_connection();

        Host_clear_device_status();   // Reset device status
        cpu_irq_disable();
        Host_disable_device_disconnection_interrupt();
        Host_send_reset();            // First USB reset
        (void)Is_host_sending_reset();
        cpu_irq_enable();
        Usb_ack_event(EVT_HOST_SOF);
        // Active wait for end of reset send
        while (Is_host_sending_reset())
        {
          // The USB macro does not signal the end of reset when a disconnection occurs
          if (Is_host_device_disconnection())
          {
            // Stop sending USB reset
            Host_stop_sending_reset();
          }
        }
        Host_ack_reset_sent();
        Host_enable_sof();            // Start SOF generation
        Host_enable_sof_interrupt();  // SOF will be detected under interrupt
        if (!Is_host_device_disconnection())
        {
          // Workaround for some buggy devices with powerless pull-up
          // usually low-speed where data line rises slowly and can be interpreted as disconnection
          for (sof_cnt = 0; sof_cnt < 0xFFFF; sof_cnt++)  // Basic time-out counter
          {
            // If we detect SOF, device is still alive and connected, just clear false disconnect flag
            if (Is_usb_event(EVT_HOST_SOF) && Is_host_device_disconnection())
            {
              Host_ack_device_connection();
              Host_ack_device_disconnection();
              break;
            }
          }
        }
        Host_enable_device_disconnection_interrupt();
        sof_cnt = 0;
        while (sof_cnt < 100)         // Wait 100 ms after USB reset
        {
          if (Is_usb_event(EVT_HOST_SOF)) Usb_ack_event(EVT_HOST_SOF), sof_cnt++; // Count SOFs
          if (Is_host_emergency_exit() || Is_usb_bconnection_error_interrupt()) goto device_attached_error;
        }
        device_state = DEVICE_POWERED;
        LOG_STR(log_device_connected);
        Host_device_connection_action();
        sof_cnt = 0;
      }
device_attached_error:
      // Device connection error, or VBus pb -> Retry the connection process from the beginning
      if (Is_usb_bconnection_error_interrupt() || Is_usb_vbus_error_interrupt() || Is_usb_vbus_low())
      {
        if (device_state != DEVICE_VBUS_LOW)
          device_state = DEVICE_UNATTACHED;
        Usb_ack_bconnection_error_interrupt();
        Usb_ack_vbus_error_interrupt();
        Host_disable_sof();
      }
      break;

    //------------------------------------------------------
    //   DEVICE_POWERED state
    //
    //   - Device connection (attach) has been detected,
    //   - Wait 100 ms and configure default control pipe
    //
    case DEVICE_POWERED:
      if (Is_usb_event(EVT_HOST_SOF))
      {
        Usb_ack_event(EVT_HOST_SOF);
        if (sof_cnt++ >= 100)         // Wait 100 ms
        {
          Host_enable_pipe(P_CONTROL);
          (void)Host_configure_pipe(P_CONTROL,
                                    0,
                                    EP_CONTROL,
                                    TYPE_CONTROL,
                                    TOKEN_SETUP,
                                    8,
                                    SINGLE_BANK);
          device_state = DEVICE_DEFAULT;
        }
      }
      break;

    //------------------------------------------------------
    //   DEVICE_DEFAULT state
    //
    //   - Get device descriptor
    //   - Reconfigure control pipe according to device control endpoint
    //   - Assign device address
    //
    case DEVICE_DEFAULT:
      // Get first device descriptor
      if (host_get_device_descriptor_incomplete() == CONTROL_GOOD)
      {
        sof_cnt = 0;
        while (sof_cnt < 20)          // Wait 20 ms before USB reset (special buggy devices...)
        {
          if (Is_usb_event(EVT_HOST_SOF)) Usb_ack_event(EVT_HOST_SOF), sof_cnt++;
          if (Is_host_emergency_exit() || Is_usb_bconnection_error_interrupt()) break;
        }
        cpu_irq_disable();
        Host_disable_device_disconnection_interrupt();
        Host_send_reset();            // First USB reset
        (void)Is_host_sending_reset();
        cpu_irq_enable();
        Usb_ack_event(EVT_HOST_SOF);
        // Active wait for end of reset send
        while (Is_host_sending_reset())
        {
          // The USB macro does not signal the end of reset when a disconnection occurs
          if (Is_host_device_disconnection())
          {
            // Stop sending USB reset
            Host_stop_sending_reset();
          }
        }
        Host_ack_reset_sent();
        if (!Is_host_device_disconnection())
        {
          // Workaround for some buggy devices with powerless pull-up
          // usually low-speed where data line rises slowly and can be interpreted as disconnection
          for (sof_cnt = 0; sof_cnt < 0xFFFF; sof_cnt++)  // Basic time-out counter
          {
            // If we detect SOF, device is still alive and connected, just clear false disconnect flag
            if (Is_usb_event(EVT_HOST_SOF) && Is_host_device_disconnection())
            {
              Host_ack_device_connection();
              Host_ack_device_disconnection();
              break;
            }
          }
        }
        Host_enable_device_disconnection_interrupt();
        sof_cnt = 0;
        while (sof_cnt < 200)         // Wait 200 ms after USB reset
        {
          if (Is_usb_event(EVT_HOST_SOF)) Usb_ack_event(EVT_HOST_SOF), sof_cnt++;
          if (Is_host_emergency_exit() || Is_usb_bconnection_error_interrupt()) break;
        }
        Host_disable_pipe(P_CONTROL);
        Host_unallocate_memory(P_CONTROL);
        Host_enable_pipe(P_CONTROL);
        // Reconfigure the control pipe according to the device control endpoint
        (void)Host_configure_pipe(P_CONTROL,
                                  0,
                                  EP_CONTROL,
                                  TYPE_CONTROL,
                                  TOKEN_SETUP,
                                  data_stage[OFFSET_FIELD_MAXPACKETSIZE],
                                  SINGLE_BANK);
        // Give an absolute device address
        if (host_set_address(DEVICE_ADDRESS) == CONTROL_GOOD)
        {
          for (pipe = 0; pipe < MAX_PEP_NB; pipe++)
            Host_configure_address(pipe, DEVICE_ADDRESS);
          device_state = DEVICE_ADDRESSED;
        }
        else if (device_state != DEVICE_VBUS_LOW)
          device_state = DEVICE_ERROR;
      }
      else
      {
        if (device_state != DEVICE_VBUS_LOW)
        {
          if (++device_default_error_count > DEVICE_DEFAULT_MAX_ERROR_COUNT)
          device_state = DEVICE_ERROR;
          else
          {
            Host_disable_sof();
            Host_disable_pipe(P_CONTROL);
            Host_unallocate_memory(P_CONTROL);
            device_state = DEVICE_ATTACHED;
            goto device_attached_retry;
          }
        }
        Usb_ack_bconnection_error_interrupt();
        Usb_ack_vbus_error_interrupt();
        Host_disable_sof();
      }
      break;

    //------------------------------------------------------
    //   DEVICE_ADDRESSED state
    //
    //   - Check if VID PID is in supported list
    //
    case DEVICE_ADDRESSED:
      if (host_get_device_descriptor() == CONTROL_GOOD)
      {
        // Detect if the device connected belongs to the supported devices table
        if (host_check_VID_PID())
        {
          Host_set_device_supported();
          Host_device_supported_action();
          device_state = DEVICE_CONFIGURED;
        }
        else
        {
#if HOST_STRICT_VID_PID_TABLE == ENABLE
          device_state = DEVICE_ERROR;
          LOG_STR(log_unsupported_device);
#else
          device_state = DEVICE_CONFIGURED;
#endif
          Host_device_not_supported_action();
        }
      }
      else if (device_state != DEVICE_VBUS_LOW)
        device_state = DEVICE_ERROR; // Can not get device descriptor
      break;

    //------------------------------------------------------
    //   DEVICE_CONFIGURED state
    //
    //   - Configure pipes for the supported interface
    //   - Send Set_configuration() request
    //   - Go to full operating mode (device ready)
    //
    case DEVICE_CONFIGURED:
      {
        uint8_t configuration_index = 0;

        if (host_get_configuration_descriptor(configuration_index) == CONTROL_GOOD)
        {
          if (host_check_class())       // Class support OK?
          {
#if HOST_AUTO_CFG_ENDPOINT == DISABLE
            User_configure_endpoint();  // User call here instead of autoconfig
            Host_set_configured();      // Assumes config is OK with user config
#endif
            if (Is_host_configured())
            {
              if (host_set_configuration(data_stage[OFFSET_FIELD_CONFIGURATION_NB]) == CONTROL_GOOD)  // Send Set_configuration
              {
                // Device and host are now fully configured
                // go to DEVICE_READY normal operation
                device_state = DEVICE_READY;
                // Monitor device disconnection under interrupt
                Host_enable_device_disconnection_interrupt();
                // If user host application requires SOF interrupt event
                // Keep SOF interrupt enabled, otherwise disable this interrupt
#if HOST_CONTINUOUS_SOF_INTERRUPT == DISABLE
                cpu_irq_disable();
                Host_disable_sof_interrupt();
                (void)Is_host_sof_interrupt_enabled();
                cpu_irq_enable();
#endif
                Host_new_device_connection_action();
                cpu_irq_enable();
                LOG_STR(log_device_enumerated);
              }
              else if (device_state != DEVICE_VBUS_LOW)
                device_state = DEVICE_ERROR; // Problem during Set_configuration request...
            }
          }
          else  // Device class not supported...
          {
            device_state = DEVICE_UNSUPPORTED;
            LOG_STR(log_unsupported_device);
            Host_device_class_not_supported_action();
          }
        }
        else if (device_state != DEVICE_VBUS_LOW)
          device_state = DEVICE_ERROR; // Can not get configuration descriptors...
      }
      break;

    //------------------------------------------------------
    //   DEVICE_READY state
    //
    //   - Full standard operating mode
    //   - Nothing to do...
    //
    case DEVICE_READY:                // Host full standard operating mode!
      break;

    //------------------------------------------------------
    //   DEVICE_UNSUPPORTED state
    //
    case DEVICE_UNSUPPORTED:
      break;

    //------------------------------------------------------
    //   DEVICE_ERROR state
    //
    //   - Error state
    //   - Do custom action call (probably go to default mode...)
    //
    case DEVICE_ERROR:                //! @todo
#if HOST_ERROR_RESTART == ENABLE
      device_state = DEVICE_UNATTACHED;
#endif
      Host_device_error_action();
      break;

    //------------------------------------------------------
    //   DEVICE_SUSPENDED state
    //
    //   - Host application request to suspend the device activity
    //   - State machine comes here thanks to Host_request_suspend()
    //
    case DEVICE_SUSPENDED:
      if (Is_device_supports_remote_wakeup()) // If the connected device supports remote wake-up
      {
        host_set_feature_remote_wakeup(); // Enable this feature...
      }
      LOG_STR(log_usb_suspended);
      sav_int_sof_enable = Is_host_sof_interrupt_enabled(); //Save current SOF interrupt enable state
      cpu_irq_disable();
      Host_disable_sof_interrupt();
      (void)Is_host_sof_interrupt_enabled();
      cpu_irq_enable();
      Host_ack_sof();
      Host_disable_sof();             // Stop SOF generation, this generates the suspend state
      Host_ack_hwup();
      Host_enable_hwup_interrupt();   // Enable host wake-up interrupt
                                      // (this is the unique USB interrupt able to wake up the CPU core from power-down mode)
      (void)Is_host_hwup_interrupt_enabled(); // Make sure host wake-up interrupt is enabled
      Usb_freeze_clock();
      //! @todo Implement this on the silicon version
      //Stop_pll();
      Host_suspend_action();          // Custom action here! (e.g. go to power-save mode...)
      device_state = DEVICE_WAIT_RESUME;  // Wait for device resume event
      break;

    //------------------------------------------------------
    //   DEVICE_WAIT_RESUME state
    //
    //   Wait in this state till:
    //   - the host receives an upstream resume from the device
    //   - or the host software request the device to resume
    //
    case DEVICE_WAIT_RESUME:
      if (Is_usb_event(EVT_HOST_HWUP) || Is_host_request_resume())  // Remote wake-up has been detected
                                                                    // or local resume request has been received
      {
        if (Is_host_request_resume())     // Not a remote wake-up, but a host application request
        {
          // CAUTION: HWUP can be cleared only when USB clock is active
          //! @todo Implement this on the silicon version
          //Pll_start_auto();               // First Restart the PLL for USB operation
          //Wait_pll_ready();               // Make sure PLL is locked
          Usb_unfreeze_clock();           // Enable clock on USB interface
          (void)Is_usb_clock_frozen();    // Make sure USB interface clock is enabled
          cpu_irq_disable();
          Host_disable_hwup_interrupt();  // Wake-up interrupt should be disabled as host is now awoken!
          (void)Is_host_hwup_interrupt_enabled();
          cpu_irq_enable();
          Host_ack_hwup();                // Clear HWUP interrupt flag
        }
        Host_enable_sof();
        Host_send_resume();               // Send downstream resume
        while (!Is_host_down_stream_resume());  // Wait for downstream resume sent
        Host_ack_remote_wakeup();         // Ack remote wake-up reception
        Host_ack_request_resume();        // Ack software request
        Host_ack_down_stream_resume();    // Ack downstream resume sent
        Usb_ack_event(EVT_HOST_HWUP);     // Ack software event
        if (sav_int_sof_enable) Host_enable_sof_interrupt();  // Restore SOF interrupt enable state before suspend
        device_state = DEVICE_READY;      // Come back to full operating mode
        LOG_STR(log_usb_resumed);
      }
      break;

    //------------------------------------------------------
    //   default state
    //
    //   - Default case: ERROR
    //   - Go to DEVICE_UNATTACHED state
    //
    default:
      device_state = DEVICE_UNATTACHED;
      break;
    }
#ifdef FREERTOS_USED
  }
#endif
}


//___ F U N C T I O N S   F O R   P O L L I N G   M A N A G E D   D A T A   F L O W S  ___

//!
//! @brief This function sends nb_data bytes pointed to by ptr_buf on the specified pipe.
//!
//! @note This function activates the host SOF interrupt to detect time-outs.
//! The initial enable state of this interrupt will be restored.
//!
//! @param pipe
//! @param nb_data
//! @param ptr_buf
//!
//! @return Status_t: Pipe status
//!
Status_t host_send_data(uint8_t pipe, uint16_t nb_data, const void *ptr_buf)
{
  Status_t status = PIPE_GOOD;      // Frame correctly sent by default
  bool sav_int_sof_enable;
  bool sav_glob_int_en;
  uint8_t nak_timeout;
#if NAK_TIMEOUT_ENABLE == ENABLE
  uint16_t cpt_nak;
#endif

  sav_int_sof_enable = Is_host_sof_interrupt_enabled(); // Save state of enable SOF interrupt
  Host_enable_sof_interrupt();
  Host_configure_pipe_token(pipe, TOKEN_OUT);
  Host_ack_out_ready(pipe);
  Host_unfreeze_pipe(pipe);
  while (nb_data)                   // While there is something to send...
  {
    // Prepare data to be sent
    Host_reset_pipe_fifo_access(pipe);
    nb_data = host_write_p_txpacket(pipe, ptr_buf, nb_data, &ptr_buf);
    private_sof_counter = 0;        // Reset the counter in SOF detection subroutine
#if NAK_TIMEOUT_ENABLE == ENABLE
    cpt_nak = 0;
#endif
    nak_timeout = 0;
    Host_ack_out_ready_send(pipe);
    while (!Is_host_out_ready(pipe))
    {
      if (Is_host_emergency_exit()) // Async disconnection or role change detected under interrupt
      {
        status = PIPE_DELAY_TIMEOUT;
        Host_reset_pipe(pipe);
        goto host_send_data_end;
      }
#if TIMEOUT_DELAY_ENABLE == ENABLE
      if (private_sof_counter >= 250) // Count 250 ms (250 SOF)
      {
        private_sof_counter = 0;
        if (nak_timeout++ >= TIMEOUT_DELAY) // Increment time-out and check for overflow
        {
          status = PIPE_DELAY_TIMEOUT;
          Host_reset_pipe(pipe);
          goto host_send_data_end;
        }
      }
#endif
      if (Is_host_pipe_error(pipe)) // Error management
      {
        status = Host_error_status(pipe);
        Host_ack_all_errors(pipe);
        goto host_send_data_end;
      }
      if (Is_host_stall(pipe))      // STALL management
      {
        status = PIPE_STALL;
        Host_ack_stall(pipe);
        goto host_send_data_end;
      }
#if NAK_TIMEOUT_ENABLE == ENABLE
      if (Is_host_nak_received(pipe)) // NAK received
      {
        Host_ack_nak_received(pipe);
        if (cpt_nak++ > NAK_SEND_TIMEOUT)
        {
          status = PIPE_NAK_TIMEOUT;
          Host_reset_pipe(pipe);
          goto host_send_data_end;
        }
      }
#endif
    }
    // Here OUT sent
    Host_ack_out_ready(pipe);
  }
  while (Host_nb_busy_bank(pipe));
host_send_data_end:
  Host_freeze_pipe(pipe);
  // Restore SOF interrupt enable state
  if (!sav_int_sof_enable)
  {
    if ((sav_glob_int_en = cpu_irq_is_enabled())) cpu_irq_disable();
    Host_disable_sof_interrupt();
    (void)Is_host_sof_interrupt_enabled();
    if (sav_glob_int_en) cpu_irq_enable();
  }

  // And return...
  return status;
}


//!
//! @brief This function receives nb_data bytes pointed to by ptr_buf on the specified pipe.
//!
//! *nb_data is updated with the final number of data bytes received.
//!
//! @note This function activates the host SOF interrupt to detect time-outs.
//! The initial enable state of this interrupt will be restored.
//!
//! @param pipe
//! @param nb_data
//! @param ptr_buf
//!
//! @return Status_t: Pipe status
//!
Status_t host_get_data(uint8_t pipe, uint16_t *nb_data, void *ptr_buf)
{
  Status_t status = PIPE_GOOD;      // Frame correctly received by default
  bool sav_int_sof_enable;
  bool sav_glob_int_en;
  uint8_t nak_timeout;
  uint16_t n, i;
#if NAK_TIMEOUT_ENABLE == ENABLE
  uint16_t cpt_nak;
#endif

  n = *nb_data;
  sav_int_sof_enable = Is_host_sof_interrupt_enabled();
  Host_enable_sof_interrupt();
  Host_enable_continuous_in_mode(pipe);
  Host_configure_pipe_token(pipe, TOKEN_IN);
  Host_ack_in_received(pipe);
  while (n)                         // While missing data...
  {
    Host_free_in(pipe);
    Host_unfreeze_pipe(pipe);
    private_sof_counter = 0;        // Reset the counter in SOF detection subroutine
    nak_timeout = 0;
#if NAK_TIMEOUT_ENABLE == ENABLE
    cpt_nak = 0;
#endif
    while (!Is_host_in_received(pipe))
    {
      if (Is_host_emergency_exit()) // Asynchronous disconnection or role exchange detected under interrupt
      {
        status = PIPE_DELAY_TIMEOUT;
        Host_reset_pipe(pipe);
        goto host_get_data_end;
      }
#if TIMEOUT_DELAY_ENABLE == ENABLE
      if (private_sof_counter >= 250) // Time-out management
      {
        private_sof_counter = 0;    // Done in host SOF interrupt
        if (nak_timeout++ >= TIMEOUT_DELAY) // Check for local time-out
        {
          status = PIPE_DELAY_TIMEOUT;
          Host_reset_pipe(pipe);
          goto host_get_data_end;
        }
      }
#endif
      if (Is_host_pipe_error(pipe)) // Error management
      {
        status = Host_error_status(pipe);
        Host_ack_all_errors(pipe);
        goto host_get_data_end;
      }
      if (Is_host_stall(pipe))      // STALL management
      {
        status = PIPE_STALL;
        Host_reset_pipe(pipe);
        Host_ack_stall(pipe);
        goto host_get_data_end;
      }
#if NAK_TIMEOUT_ENABLE == ENABLE
      if (Is_host_nak_received(pipe)) // NAK received
      {
        Host_ack_nak_received(pipe);
        if (cpt_nak++ > NAK_RECEIVE_TIMEOUT)
        {
          status = PIPE_NAK_TIMEOUT;
          Host_reset_pipe(pipe);
          goto host_get_data_end;
        }
      }
#endif
    }
    Host_freeze_pipe(pipe);
    Host_reset_pipe_fifo_access(pipe);
    i = Host_get_pipe_size(pipe) - Host_byte_count(pipe);
    if (!ptr_buf)
    {
      if (Host_byte_count(pipe) > n)  // More bytes received than expected
      {
        n = 0;
        //! @todo Error code management
      }
      else                            // Nb bytes received <= expected
      {
        n -= Host_byte_count(pipe);
        if (i)                          // Short packet
        {
          *nb_data -= n;
          n = 0;
        }
      }
    }
    else
    {
      n = host_read_p_rxpacket(pipe, ptr_buf, n, &ptr_buf);
      if (Host_byte_count(pipe))      // More bytes received than expected
      {
        //! @todo Error code management
      }
      else if (i)                     // Short packet with nb bytes received <= expected
      {
        *nb_data -= n;
        n = 0;
      }
    }
    Host_ack_in_received(pipe);

    // In low-speed mode, the USB IP may have not yet sent the ACK at this
    // point. The USB IP does not support a new start of transaction request
    // from the firmware if the ACK has not been sent. The only means of making
    // sure the ACK has been sent is to wait for the next Keep-Alive before
    // starting a new transaction.
    if (Is_usb_low_speed_mode())
    {
      Usb_ack_event(EVT_HOST_SOF);
      sav_int_sof_enable = Is_host_sof_interrupt_enabled();
      if ((sav_glob_int_en = cpu_irq_is_enabled())) cpu_irq_disable();
      Host_ack_sof();
      (void)Is_host_sof_interrupt_enabled();
      if (sav_glob_int_en) cpu_irq_enable();
      Host_enable_sof_interrupt();
      while (!Is_usb_event(EVT_HOST_SOF))         // Wait for next Keep-Alive
      {
        if (Is_host_emergency_exit())
        {
          status = PIPE_DELAY_TIMEOUT;
          Host_reset_pipe(pipe);
          goto host_get_data_end;
        }
      }
      if (!sav_int_sof_enable)                    // Restore SOF interrupt enable
      {
        if ((sav_glob_int_en = cpu_irq_is_enabled())) cpu_irq_disable();
        Host_disable_sof_interrupt();
        (void)Is_host_sof_interrupt_enabled();
        if (sav_glob_int_en) cpu_irq_enable();
      }
    }
  }
host_get_data_end:
  Host_freeze_pipe(pipe);
  // Restore SOF interrupt enable state
  if (!sav_int_sof_enable)
  {
    if ((sav_glob_int_en = cpu_irq_is_enabled())) cpu_irq_disable();
    Host_disable_sof_interrupt();
    (void)Is_host_sof_interrupt_enabled();
    if (sav_glob_int_en) cpu_irq_enable();
  }

  // And return...
  return status;
}


//___ F U N C T I O N S   F O R   I N T E R R U P T   M A N A G E D   D A T A   F L O W S  ___

#if USB_HOST_PIPE_INTERRUPT_TRANSFER == ENABLE

void reset_it_pipe_str(void)
{
  uint8_t i;

  for (i = 0; i < MAX_PEP_NB; i++)
  {
    it_pipe_str[i].enable = false;
    it_pipe_str[i].timeout = 0;
  }
}


bool is_any_interrupt_pipe_active(void)
{
  uint8_t i;

  for (i = 0; i < MAX_PEP_NB; i++)
  {
    if (it_pipe_str[i].enable) return true;
  }

  return false;
}


//!
//! @brief This function sends nb_data bytes pointed to by ptr_buf on the specified pipe.
//!
//! @param pipe
//! @param nb_data
//! @param ptr_buf
//! @param handler Call-back function pointer
//!
//! @return bool: Status
//!
bool host_send_data_interrupt(uint8_t pipe, uint16_t nb_data, const void *ptr_buf, Pipe_handler *handler)
{
  bool sav_glob_int_en;

  if (it_pipe_str[pipe].enable) return false;

  if (!is_any_interrupt_pipe_active())
  {
    g_sav_int_sof_enable = Is_host_sof_interrupt_enabled();
    Host_enable_sof_interrupt();
  }
  it_pipe_str[pipe].enable = true;
  it_pipe_str[pipe].nb_byte_to_process = nb_data;
  it_pipe_str[pipe].nb_byte_processed = 0;
  it_pipe_str[pipe].ptr_buf = (void *)ptr_buf;
  it_pipe_str[pipe].handler = handler;
  it_pipe_str[pipe].timeout = 0;
  it_pipe_str[pipe].nak_timeout = NAK_SEND_TIMEOUT;

  if ((sav_glob_int_en = cpu_irq_is_enabled())) cpu_irq_disable();
  Host_reset_pipe(pipe);
  (void)Is_host_resetting_pipe(pipe);
  if (sav_glob_int_en) cpu_irq_enable();
  Host_configure_pipe_token(pipe, TOKEN_OUT);
  Host_ack_out_ready(pipe);
  Host_unfreeze_pipe(pipe);
  // Prepare data to be sent
  Host_reset_pipe_fifo_access(pipe);
  it_pipe_str[pipe].nb_byte_on_going = nb_data - host_write_p_txpacket(pipe, ptr_buf, nb_data, NULL);
  private_sof_counter = 0;          // Reset the counter in SOF detection subroutine
  it_pipe_str[pipe].timeout = 0;    // Refresh time-out counter
  if ((sav_glob_int_en = cpu_irq_is_enabled())) cpu_irq_disable();
  Host_ack_out_ready(pipe);
  Host_ack_stall(pipe);
  Host_ack_nak_received(pipe);
  (void)Is_host_nak_received(pipe);
  if (sav_glob_int_en) cpu_irq_enable();

  Host_enable_stall_interrupt(pipe);
  Host_enable_pipe_error_interrupt(pipe);
  #if NAK_TIMEOUT_ENABLE == ENABLE
  Host_enable_nak_received_interrupt(pipe);
  #endif
  Host_enable_out_ready_interrupt(pipe);
  Host_enable_pipe_interrupt(pipe);

  Host_send_out(pipe);              // Send the USB frame

  return true;
}


//!
//! @brief This function receives nb_data bytes pointed to by ptr_buf on the specified pipe.
//!
//! *nb_data is updated with the final number of data bytes received.
//!
//! @param pipe
//! @param nb_data
//! @param ptr_buf
//! @param handler Call-back function pointer
//!
//! @return bool: Status
//!
bool host_get_data_interrupt(uint8_t pipe, uint16_t nb_data, void *ptr_buf, Pipe_handler *handler)
{
  bool sav_glob_int_en;

  if (it_pipe_str[pipe].enable) return false;

  if (!is_any_interrupt_pipe_active())
  {
    g_sav_int_sof_enable = Is_host_sof_interrupt_enabled();
    Host_enable_sof_interrupt();
  }
  it_pipe_str[pipe].enable = true;
  it_pipe_str[pipe].nb_byte_to_process = nb_data;
  it_pipe_str[pipe].nb_byte_processed = 0;
  it_pipe_str[pipe].ptr_buf = ptr_buf;
  it_pipe_str[pipe].handler = handler;
  it_pipe_str[pipe].timeout = 0;
  it_pipe_str[pipe].nak_timeout = NAK_RECEIVE_TIMEOUT;

  private_sof_counter = 0;          // Reset the counter in SOF detection subroutine
  if ((sav_glob_int_en = cpu_irq_is_enabled())) cpu_irq_disable();
  Host_reset_pipe(pipe);
  Host_ack_stall(pipe);
  Host_ack_nak_received(pipe);
  (void)Is_host_nak_received(pipe);
  if (sav_glob_int_en) cpu_irq_enable();

  Host_enable_stall_interrupt(pipe);
  #if NAK_TIMEOUT_ENABLE == ENABLE
  Host_enable_nak_received_interrupt(pipe);
  #endif
  Host_enable_pipe_error_interrupt(pipe);
  Host_enable_in_received_interrupt(pipe);
  Host_enable_pipe_interrupt(pipe);

  Host_enable_continuous_in_mode(pipe);
  Host_configure_pipe_token(pipe, TOKEN_IN);
  Host_ack_in_received(pipe);
  Host_unfreeze_pipe(pipe);

  return true;
}


//!
//! @brief USB pipe interrupt subroutine
//!
void usb_pipe_interrupt(uint8_t pipe)
{
  void *ptr_buf;
  uint16_t n, i;
  bool callback = false;

  // Detect which events generate an interrupt...

  if (Is_host_pipe_error(pipe))     // Error management
  {
    it_pipe_str[pipe].status = Host_error_status(pipe);
    it_pipe_str[pipe].enable = false;
    Host_reset_pipe(pipe);
    Host_ack_all_errors(pipe);
    callback = true;
    goto usb_pipe_interrupt_end;
  }

  if (Is_host_stall(pipe))          // STALL management
  {
    it_pipe_str[pipe].status = PIPE_STALL;
    it_pipe_str[pipe].enable = false;
    Host_reset_pipe(pipe);
    callback = true;
    goto usb_pipe_interrupt_end;
  }

  #if NAK_TIMEOUT_ENABLE == ENABLE
  if (Is_host_nak_received(pipe))   // NAK received
  {
    Host_ack_nak_received(pipe);
    // Check if NAK time-out error occurs (not for interrupt pipes)
    if (!--it_pipe_str[pipe].nak_timeout && Host_get_pipe_type(pipe) != TYPE_INTERRUPT)
    {
      it_pipe_str[pipe].status = PIPE_NAK_TIMEOUT;
      it_pipe_str[pipe].enable = false;
      Host_reset_pipe(pipe);
      callback = true;
      goto usb_pipe_interrupt_end;
    }
  }
  #endif

  if (Is_host_in_received(pipe))    // Pipe IN reception?
  {
    ptr_buf = (uint8_t *)it_pipe_str[pipe].ptr_buf + it_pipe_str[pipe].nb_byte_processed;  // Build pointer to data buffer
    n = it_pipe_str[pipe].nb_byte_to_process - it_pipe_str[pipe].nb_byte_processed; // Remaining data bytes
    Host_freeze_pipe(pipe);
    Host_reset_pipe_fifo_access(pipe);
    i = Host_get_pipe_size(pipe) - Host_byte_count(pipe);
    n = host_read_p_rxpacket(pipe, ptr_buf, n, NULL);
    it_pipe_str[pipe].nb_byte_processed = it_pipe_str[pipe].nb_byte_to_process - n;
    if (Host_byte_count(pipe))      // More bytes received than expected
    {
      //! @todo Error code management
    }
    else if (i)                     // Short packet with nb bytes received <= expected
    {
      n = 0;
    }
    Host_ack_in_received(pipe);
    if (n)                          // Still data to process
    {
      Host_free_in(pipe);
      Host_unfreeze_pipe(pipe);     // Request another IN transfer
      private_sof_counter = 0;      // Reset the counter in SOF detection subroutine
      it_pipe_str[pipe].timeout = 0;  // Reset time-out
      it_pipe_str[pipe].nak_timeout = NAK_RECEIVE_TIMEOUT;
    }
    else                            // End of transfer
    {
      it_pipe_str[pipe].enable = false;
      it_pipe_str[pipe].status = PIPE_GOOD;
      Host_reset_pipe(pipe);
      callback = true;
    }
  }

  if (Is_host_out_ready(pipe))      // Pipe OUT sent?
  {
    Host_ack_out_ready(pipe);
    it_pipe_str[pipe].nb_byte_processed += it_pipe_str[pipe].nb_byte_on_going;
    it_pipe_str[pipe].nb_byte_on_going = 0;
    ptr_buf = (uint8_t *)it_pipe_str[pipe].ptr_buf + it_pipe_str[pipe].nb_byte_processed;  // Build pointer to data buffer
    n = it_pipe_str[pipe].nb_byte_to_process - it_pipe_str[pipe].nb_byte_processed; // Remaining data bytes
    if (n)                          // Still data to process
    {
      Host_unfreeze_pipe(pipe);
      // Prepare data to be sent
      Host_reset_pipe_fifo_access(pipe);
      it_pipe_str[pipe].nb_byte_on_going = n - host_write_p_txpacket(pipe, ptr_buf, n, NULL);
      private_sof_counter = 0;      // Reset the counter in SOF detection subroutine
      it_pipe_str[pipe].timeout = 0;  // Refresh time-out counter
      it_pipe_str[pipe].nak_timeout = NAK_SEND_TIMEOUT;
      Host_send_out(pipe);          // Send the USB frame
    }
    else                            // End of transfer
    {
      it_pipe_str[pipe].enable = false; // Transfer end
      it_pipe_str[pipe].status = PIPE_GOOD; // Status OK
      Host_reset_pipe(pipe);
      callback = true;
    }
  }

usb_pipe_interrupt_end:
  if (!is_any_interrupt_pipe_active() && !g_sav_int_sof_enable) // If no more transfer is armed
  {
    Host_disable_sof_interrupt();
  }
  if (callback)                     // Any call-back function to perform?
  {
    it_pipe_str[pipe].handler(it_pipe_str[pipe].status, it_pipe_str[pipe].nb_byte_processed);
  }
}

#endif  // USB_HOST_PIPE_INTERRUPT_TRANSFER == ENABLE


#endif  // USB_HOST_FEATURE == true
