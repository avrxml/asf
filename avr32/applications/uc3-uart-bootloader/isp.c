/**************************************************************************
 *
 * \file
 *
 * \brief Main file of the UART DFU ISP.
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


/*! \mainpage
 * \section intro Introduction
 * This is the documentation for the data structures, functions, variables,
 * defines, enums, and typedefs for the UART DFU bootloader.\n
 * Refer to http://www.atmel.com/dyn/resources/prod_documents/docxxxx.pdf
 *
 * \section files Main Files
 * - isp.c: the main file of the UART DFU ISP;
 * - uart_dfu.c: Management of the UART device firmware upgrade;
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC for AVR32 and for IAR Embedded Workbench
 * for Atmel AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Information
 * All AVR UC3 devices with a UART module.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


//_____  I N C L U D E S ___________________________________________________

#include "compiler.h"
#include "board.h"
#include "power_clocks_lib.h"
#include "cycle_counter.h"
#include "gpio.h"
#include "conf_isp.h"
#include "conf_clocks.h"
#include "isp.h"
#include "uart_dfu.h"


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

#if UC3A3
 #define TC               (&AVR32_TC0)
#else
  #define TC              (&AVR32_TC)
#endif
#define TC_32KHZ_CHANNEL  0



#if defined(__GNUC__)
volatile static const char build_info[] = "GCC "__VERSION__" ATMEL_AVR_UC3_UART_BOOTLOADER_1.0.2";
#elif defined(__ICCAVR32__)
volatile static const char build_info[] = "IAR "ASTRINGZ(__VER__)" ATMEL_AVR_UC3_UART_BOOTLOADER_1.0.2";
#endif



void wait_10_ms(void)
{
  Set_system_register(AVR32_COUNT, 0);
#if (defined AVR32_PM_RCOSC_FREQUENCY)
  while ((U32)Get_system_register(AVR32_COUNT) <
      (AVR32_PM_RCOSC_FREQUENCY * 10 + 999) / 1000);
#elif (defined AVR32_SCIF_RCOSC_FREQUENCY)
  while ((U32)Get_system_register(AVR32_COUNT) <
      (AVR32_SCIF_RCOSC_FREQUENCY * 10 + 999) / 1000);
#else
  #error Unknow RCOSC frequency value
#endif
}


/*!
 *  Start the generation of system clocks
 */
#if UC3L
extern int conf_clocks_uc3l(void);
#endif
void sys_clk_gen_start(void)
{
#if UC3L
#ifdef DEBUG_ON
  if(conf_clocks_uc3l())
  {
    gpio_clr_gpio_pin(LED0_GPIO); gpio_clr_gpio_pin(LED1_GPIO);
    gpio_clr_gpio_pin(LED2_GPIO); gpio_clr_gpio_pin(LED3_GPIO);
    while(1); // ERROR
  }
#else
  conf_clocks_uc3l();
#endif
#elif UC3A || UC3B || UC3C
#ifdef DEBUG_ON
  if(conf_clocks_uc3_extosc0())
  {
    gpio_clr_gpio_pin(LED0_GPIO); gpio_clr_gpio_pin(LED1_GPIO);
    gpio_clr_gpio_pin(LED2_GPIO); gpio_clr_gpio_pin(LED3_GPIO);
    while(1); // ERROR
  }
#else
  conf_clocks_uc3_extosc0();
#endif
#endif
}


/*!
 *  Stop the generation of system clocks and switch to RCOsc
 */
#if UC3L
extern void stop_clocks_uc3l(void);
#endif
void sys_clk_gen_stop(void)
{
#if UC3L
  stop_clocks_uc3l();
#elif UC3A || UC3B || UC3C
  stop_clocks_uc3_extosc0();
#endif
}

/*! \brief Set-up a generic clock at 12MHz with the DFLL as a source, output the
 * generic clock to a pin.
 *
 */
#if 0
#ifdef DEBUG_ON
#define EXAMPLE_GCLK_ID             AVR32_SCIF_GCLK_DFLL0_SSG // Do not use the AVR32_SCIF_GCLK_DFLL0_REF gc.
#define EXAMPLE_GCLK_PIN            AVR32_SCIF_GCLK_1_0_PIN   // Mapped on STK600.PORTA.PA6; connector PA6 on AT32UC3L-EK
#define EXAMPLE_GCLK_FUNCTION       AVR32_SCIF_GCLK_1_0_FUNCTION
#define EXAMPLE_GCLK_FREQ_HZ        TARGET_CPU_FREQ_HZ
static void local_start_gc()
{
  // Setup gc on DFLL; the target frequency is 12MHz => divide the DFLL frequency
  // by 2 (== EXAMPLE_FDFLL_HZ / EXAMPLE_GCLK_FREQ_HZ).
  scif_gc_setup(EXAMPLE_GCLK_ID, SCIF_GCCTRL_DFLL0, AVR32_GC_DIV_CLOCK,
                TARGET_DFLL_FREQ_HZ/EXAMPLE_GCLK_FREQ_HZ);

  // Now enable the generic clock
  scif_gc_enable(EXAMPLE_GCLK_ID);

  /* Assign a GPIO to generic clock output */
  gpio_enable_module_pin(EXAMPLE_GCLK_PIN, EXAMPLE_GCLK_FUNCTION);
  // Note that gclk1 is GPIO pin 6 pa06 on AT32UC3L064 pin 10 on QFP48.
}
#endif
#endif

int main(void)
{
  req_header_t  req_header;
  U8            checksum;


  // Dummy access to force the link of this variable into the binary.
  *build_info;

  // Clocks configuration
  sys_clk_gen_start();
#if 0
#ifdef DEBUG_ON
  local_start_gc();
#endif
#endif

  // Uart init
  uart_dfu_start();

  // Main loop
  while(1)
  {
    // Get the command from the external programmer
    checksum = uart_get_dfu_command(&req_header);

    // Execute the command
    uart_execute_dfu_command(&req_header, &checksum);
  }
}
