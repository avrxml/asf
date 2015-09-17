/**
 * \file
 *
 * \mainpage
 * \section title Main file of the USB HID example.
 *
 * \section file File(s)
 * - \ref hid_example.c
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


//_____  I N C L U D E S ___________________________________________________

#ifndef FREERTOS_USED
#if (defined __GNUC__)
#include "nlao_cpu.h"
#include "nlao_usart.h"
#endif
#else
#include <stdio.h>
#endif
#include "compiler.h"
#include "board.h"
#include "print_funcs.h"
#include "intc.h"
#if BOARD == EVK1105
#include "gpio.h"
#endif
#include "power_clocks_lib.h"
#ifdef FREERTOS_USED
#include "FreeRTOS.h"
#include "task.h"
#endif
#include "conf_usb.h"
#include "usb_task.h"
#if USB_DEVICE_FEATURE == true
#include "device_mouse_hid_task.h"
#endif
#if USB_HOST_FEATURE == true
//#include "host_keyboard_hid_task.h"
#include "host_mouse_hid_task.h"
#endif
#include "hid_example.h"


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

/*! \name System Clock Frequencies
 */
//! @{
static pcl_freq_param_t pcl_freq_param =
{
  .cpu_f        = FCPU_HZ,
  .pba_f        = FPBA_HZ,
  .osc0_f       = FOSC0,
  .osc0_startup = OSC0_STARTUP
};
//! @}


#ifndef FREERTOS_USED

  #if (defined __GNUC__)

/*! \brief Low-level initialization routine called during startup, before the
 *         main function.
 *
 * This version comes in replacement to the default one provided by the Newlib
 * add-ons library.
 * Newlib add-ons' _init_startup only calls init_exceptions, but Newlib add-ons'
 * exception and interrupt vectors are defined in the same section and Newlib
 * add-ons' interrupt vectors are not compatible with the interrupt management
 * of the INTC module.
 * More low-level initializations are besides added here.
 */
int _init_startup(void)
{
  // Import the Exception Vector Base Address.
  extern void _evba;

  // Load the Exception Vector Base Address in the corresponding system register.
  Set_system_register(AVR32_EVBA, (int)&_evba);

  // Enable exceptions.
  Enable_global_exception();

  // Initialize interrupt handling.
  INTC_init_interrupts();

  // Give the used CPU clock frequency to Newlib, so it can work properly.
  set_cpu_hz(pcl_freq_param.pba_f);

  // Initialize the USART used for the debug trace with the configured parameters.
  set_usart_base( ( void * ) DBG_USART );

  // Don't-care value for GCC.
  return 1;
}

  #elif (defined __ICCAVR32__)

/*! \brief Low-level initialization routine called during startup, before the
 *         main function.
 */
int __low_level_init(void)
{
  // Enable exceptions.
  Enable_global_exception();

  // Initialize interrupt handling.
  INTC_init_interrupts();

  // Initialize the USART used for the debug trace with the configured parameters.
  extern volatile avr32_usart_t *volatile stdio_usart_base;
  stdio_usart_base = DBG_USART;

  // Request initialization of data segments.
  return 1;
}

  #endif  // Compiler

#endif  // FREERTOS_USED


#if BOARD == EVK1105
/*! \brief Touch sensor ISR.
 *  This ISR is used to control the remote wakeup function.
 */
#if (defined __GNUC__)
__attribute__ ((__interrupt__))
#elif (defined __ICCAVR32__)
__interrupt
#endif
void touch_button_isr(void)
{
  // Unfreeze the clock (this has been done in the SUSPEND state
  Usb_unfreeze_clock();

  // Launch the remote wakeup
  Usb_initiate_remote_wake_up();

  //Disable the GPIO activity detection
  gpio_clear_pin_interrupt_flag(QT1081_TOUCH_SENSOR_ENTER);
  gpio_disable_pin_interrupt(QT1081_TOUCH_SENSOR_ENTER);
}
#endif


/*! \brief Main function. Execution starts here.
 *
 * \retval 42 Fatal error.
 */
int main(void)
{
  // Configure system clocks.
  if (pcl_configure_clocks(&pcl_freq_param) != PASS)
    return 42;

  // Initialize USB clock (on PLL1)
  pcl_configure_usb_clock();

  // Initialize usart comm
  init_dbg_rs232(pcl_freq_param.pba_f);

#if BOARD == EVK1105
  Disable_global_interrupt();
  /* Register interrupt handler to the interrupt controller
   * up, down buttons on PB22, PB23 -> GPIO_IRQ_6
   */
  INTC_register_interrupt(&touch_button_isr, AVR32_GPIO_IRQ_6, 0);
  /* all gpios between PB23 - PB31) */
  INTC_register_interrupt(&touch_button_isr, AVR32_GPIO_IRQ_7, 0);
  Enable_global_interrupt();
#endif

  // Initialize USB task
  usb_task_init();

#if USB_DEVICE_FEATURE == true
  // Initialize device mouse USB task
  device_mouse_hid_task_init();
#endif
#if USB_HOST_FEATURE == true
  //host_keyboard_hid_task_init();

  // Initialize host mouse USB task
  host_mouse_hid_task_init();
#endif

#ifdef FREERTOS_USED
  // Start OS scheduler
  vTaskStartScheduler();
  portDBG_TRACE("FreeRTOS returned.");
  return 42;
#else
  // No OS here. Need to call each task in round-robin mode.
  while (true)
  {
    usb_task();
  #if USB_DEVICE_FEATURE == true
    device_mouse_hid_task();
  #endif
  #if USB_HOST_FEATURE == true
    //host_keyboard_hid_task();
    host_mouse_hid_task();
  #endif
  }
#endif  // FREERTOS_USED
}
