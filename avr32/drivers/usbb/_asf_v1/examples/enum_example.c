/**
 * \file
 *
 * \mainpage
 * \section title Main file of the USB enumeration example.
 *
 * \section file File(s)
 * - \ref enum_example.c
 *
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
