/**
 * \file
 *
 * \mainpage
 * \section title Main file of the USB enumeration example.
 *
 * \section file File(s)
 * - \ref enum_example.c
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

#ifdef FREERTOS_USED
#include <stdio.h>
#endif
#include "compiler.h"
#include "board.h"
#include "print_funcs.h"
#include "intc.h"
#include "power_clocks_lib.h"
#ifdef FREERTOS_USED
#include "FreeRTOS.h"
#include "task.h"
#endif
#include "conf_usb.h"
#include "usb_task.h"
#if USB_DEVICE_FEATURE == true
#include "device_template_task.h"
#endif
#if USB_HOST_FEATURE == true
#include "host_template_task.h"
#endif


//_____ D E F I N I T I O N S ______________________________________________

/*! \brief Main function. Execution starts here.
 *
 * \retval 42 Fatal error.
 */
int main(void)
{
#ifndef FREERTOS_USED
  Enable_global_exception();
  INTC_init_interrupts();
#endif
  pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);
  init_dbg_rs232(FOSC0);
  pcl_configure_usb_clock();
  usb_task_init();
#if USB_DEVICE_FEATURE == true
  device_template_task_init();
#endif
#if USB_HOST_FEATURE == true
  host_template_task_init();
#endif

#ifdef FREERTOS_USED
  vTaskStartScheduler();
  portDBG_TRACE("FreeRTOS returned.");
  return 42;
#else
  while (true)
  {
    usb_task();
  #if USB_DEVICE_FEATURE == true
    device_template_task();
  #endif
  #if USB_HOST_FEATURE == true
    host_template_task();
  #endif
  }
#endif  // FREERTOS_USED
}
