/**
 * \file
 *
 * \mainpage
 * \section title Main file of the USB CDC example.
 *
 * \section file File(s)
 * - \ref cdc_example.c
 * - \ref device_cdc_task.c
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
#include "power_clocks_lib.h"
#include "cdc_example.h"
#ifdef FREERTOS_USED
#include "FreeRTOS.h"
#include "task.h"
#endif
#include "conf_usb.h"
#include "usb_task.h"
#if USB_DEVICE_FEATURE == true
#include "device_cdc_task.h"
#endif
#if USB_HOST_FEATURE == true
#include "host_cdc_task.h"
#endif
#include <stdio.h>


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

/*! \name System Clock Frequencies
 */
//! @{
pcl_freq_param_t pcl_freq_param =
{
  .cpu_f        = APPLI_CPU_SPEED,
  .pba_f        = APPLI_PBA_SPEED,
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

  // Initialize the USART used for the debug trace with the configured parameters.
  set_usart_base( ( void * ) DBG_USART );

  // Don't-care value for GCC.
  return 1;
}

  #elif __ICCAVR32__

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


/*! \brief Main function. Execution starts here.
 *
 * \retval 42 Fatal error.
 */
int main(void)
{
  // Configure system clocks.
  if (pcl_configure_clocks(&pcl_freq_param) != PASS)
    return 42;

  // Initialize usart comm
  init_dbg_rs232(pcl_freq_param.pba_f);

#ifndef FREERTOS_USED
# if (defined __GNUC__)
  // Give the used CPU clock frequency to Newlib, so it can work properly.
  set_cpu_hz(pcl_freq_param.pba_f);
# endif
#endif

  // Initialize USB clock.
  pcl_configure_usb_clock();

  // Initialize USB task
  usb_task_init();

  // Display a welcome banner on USART
  printf("                                                       ......       ......     \r\n");
  printf("       IIIIII  IIIII        IIII   IIIIIIIIIII      IIIIIIIIIII. .IIIIIIIIII.  \r\n");
  printf("      IIIIIII   IIIII      IIIII  IIIIIIIIIIIII     IIIIIIIIIIII..IIIIIIIIIII. \r\n");
  printf("     IIIIIIIII  IIIII     IIIII   IIIII   IIIII     I.      IIIII.:.     IIIII \r\n");
  printf("     IIII IIIII  IIIII    IIII   IIIII    IIIII            .IIII.        IIIII \r\n");
  printf("    IIIII  IIII   IIII   IIIII  IIIIIIIIIIIIII         IIIIIIII          IIII. \r\n");
  printf("    IIII   IIIII  IIIII IIIII   IIIIIIIIIIIII          IIIIIIII.       .IIII:  \r\n");
  printf("   IIIII    IIIII  IIIIIIIII   IIIIIIIIIII                 .IIIII     IIIII.   \r\n");
  printf("  IIIIIIIIIIIIIII   IIIIIIII   IIIII IIIII                  .IIII   .IIII:     \r\n");
  printf("  IIIIIIIIIIIIIIII  IIIIIII   IIIII   IIII         II:.    .IIIII .IIIII.      \r\n");
  printf(" IIIII        IIIII  IIIIII  IIIII    IIIII        IIIIIIIIIIIII.IIIIIIIIIIIIII\r\n");
  printf(" IIIII        IIIII  IIIII   IIIII    IIIII        :IIIIIIIIII.  IIIIIIIIIIIIII\r\n");
  printf("                      III                                                      \r\n");
  printf("                       II                                                      \r\n");

#if USB_DEVICE_FEATURE == true
  // Initialize device CDC USB task
  device_cdc_task_init();
#endif
#if USB_HOST_FEATURE == true
  // Initialize host CDC USB task
  host_cdc_task_init();
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
    device_cdc_task();
  #endif
  #if USB_HOST_FEATURE == true
    host_cdc_task();
  #endif
  }
#endif  // FREERTOS_USED
}
