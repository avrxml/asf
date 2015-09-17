/*****************************************************************************
 *
 * \file
 *
 * \brief Main file of the CS2200 example.
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



/*! \mainpage
 * \section intro Introduction
 * This is the documentation for the CS2200 Clock Synthesizer example.
 *
 * \section files Main Files
 * - cs2200.c , .h: the CS2200 driver
 * - main.c: the example
 * - conf_cs2200.h: the CS2200 configuration
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC for AVR32 and for IAR Embedded Workbench
 * for Atmel AVR32. Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/uc3">Atmel AVR UC3</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

//_____  I N C L U D E S ___________________________________________________

#include <stdio.h>
#include "compiler.h"
#include "preprocessor.h"
#include "board.h"
#include "print_funcs.h"
#include "flashc.h"
#include "gpio.h"
#include "intc.h"
#include "pm.h"
#include "twi_master.h"
#include "conf_cs2200.h"
#include "cycle_counter.h"
#include "cs2200.h"
#include "stdio_serial.h"
#include "conf_stdio_serial.h"
#include "conf_twi_master.h"


//_____ D E F I N I T I O N S ______________________________________________

#if __GNUC__ && __AVR32__

/*! \brief Replaces Newlib's implementation of the standard \c wcstombs function
 *         because of portability issues with \c wchar_t.
 *
 * \warning This function must be used with the `-fshort-wchar' code generation
 *          option.
 */
size_t wcstombs(char *s, const wchar_t *pwcs, size_t n)
{
  size_t count = 0;

  while (n--)
  {
    if ((*s++ = (char)*pwcs++) == '\0')
      break;
    count++;
  }

  return count;
}

#endif


/*! \brief Initializes MCU exceptions.
 */
static void init_exceptions(void)
{
#if __GNUC__ && __AVR32__
  // Import the Exception Vector Base Address.
  extern void _evba;

  // Load the Exception Vector Base Address in the corresponding system
  // register.
  Set_system_register(AVR32_EVBA, (int)&_evba);
#endif

  // Enable exceptions globally.
  Enable_global_exception();
}


/*! \brief Initializes the HSB bus matrix.
 */
static void init_hmatrix(void)
{
  union
  {
    unsigned long                 scfg;
    avr32_hmatrix_scfg_t          SCFG;
  } u_avr32_hmatrix_scfg;

  // For the internal-flash HMATRIX slave, use last master as default.
  u_avr32_hmatrix_scfg.scfg = AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_FLASH];
  u_avr32_hmatrix_scfg.SCFG.defmstr_type = AVR32_HMATRIX_DEFMSTR_TYPE_LAST_DEFAULT;
  AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_FLASH] = u_avr32_hmatrix_scfg.scfg;

  // For the internal-SRAM HMATRIX slave, use last master as default.
  u_avr32_hmatrix_scfg.scfg = AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_SRAM];
  u_avr32_hmatrix_scfg.SCFG.defmstr_type = AVR32_HMATRIX_DEFMSTR_TYPE_LAST_DEFAULT;
  AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_SRAM] = u_avr32_hmatrix_scfg.scfg;
}


  #if __GNUC__

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

  // Request initialization of data segments.
  return 1;
}

  #endif  // Compiler


/*! \brief Initializes the two-wire interface
 */
static void init_twi(uint32_t fpba_hz)
{
  volatile uint32_t i;

  static const gpio_map_t CS2200_TWI_GPIO_MAP =
  {
    {AVR32_TWI_SCL_0_0_PIN, AVR32_TWI_SCL_0_0_FUNCTION},
    {AVR32_TWI_SDA_0_0_PIN, AVR32_TWI_SDA_0_0_FUNCTION}
  };

  static twi_master_options_t CS2200_TWI_OPTIONS =
  {
    .speed  = CS2200_TWI_MASTER_SPEED,
    .chip   = CS2200_TWI_SLAVE_ADDRESS
  };
  CS2200_TWI_OPTIONS.pba_hz = fpba_hz;

  gpio_enable_module(CS2200_TWI_GPIO_MAP,
                     sizeof(CS2200_TWI_GPIO_MAP) / sizeof(CS2200_TWI_GPIO_MAP[0]));
  twi_master_setup(CS2200_TWI, &CS2200_TWI_OPTIONS);
}

/*! \brief Main function. Execution starts here.
 *
 * \retval 42 Fatal error.
 */
int main(void)
{
  uint32_t iter=0;
  uint32_t cs2200_out_freq=11289600;
  static bool b_sweep_up=true;
  static uint32_t freq_step=0;

  // USART options.
  static usart_serial_options_t USART_SERIAL_OPTIONS =
  {
    .baudrate     = USART_SERIAL_EXAMPLE_BAUDRATE,
    .charlength   = USART_SERIAL_CHAR_LENGTH,
    .paritytype   = USART_SERIAL_PARITY,
    .stopbits     = USART_SERIAL_STOP_BIT
  };

  // Initialize the TWI using the internal RCOSC
  init_twi(AVR32_PM_RCOSC_FREQUENCY);

  // Initialize the CS2200 and produce a default frequency.
  cs2200_setup(11289600, FOSC0);

  sysclk_init();

  // Initialize the board.
  // The board-specific conf_board.h file contains the configuration of the board
  // initialization.
  board_init();

  // Initialize the TWI
  init_twi(sysclk_get_pba_hz());

  // Initialize Serial Interface using Stdio Library
  stdio_serial_init(USART_SERIAL_EXAMPLE,&USART_SERIAL_OPTIONS);

  // Initialize the HMatrix.
  init_hmatrix();

  print_dbg("\r\nCS2200 Example\r\n");

  // Generate a 12.288 MHz frequency out of the CS2200.
  print_dbg("Output 12.288 MHz\r\n");
  cs2200_freq_clk_out(_32_BITS_RATIO(12288000, FOSC0));
  cpu_delay_ms( 10000, sysclk_get_cpu_hz());

  // Generate a 11.2896 MHz frequency out of the CS2200.
  print_dbg("Output 11.2896 MHz\r\n");
  cs2200_freq_clk_out(_32_BITS_RATIO(cs2200_out_freq, FOSC0));
  cpu_delay_ms( 10000, sysclk_get_cpu_hz());

  print_dbg("Sweep from 11.2896 MHz steps of 100 PPM\r\n");
  freq_step = PPM(cs2200_out_freq, 100);

  while(1)
  {
    uint32_t ratio;

    if(b_sweep_up)
    {
      if( iter<=10 )
      {
        print_dbg("Add 100 PPM\r\n");
        iter++;
        cs2200_out_freq += freq_step;
        ratio = _32_BITS_RATIO(cs2200_out_freq, FOSC0);
        cs2200_freq_clk_adjust((uint16_t)ratio);
        cpu_delay_ms( 1000, sysclk_get_cpu_hz());
        while( twi_is_busy() );
      }
      else
        b_sweep_up=false;
    }

    if(!b_sweep_up)
    {
      if( iter>0 )
      {
        print_dbg("Sub 100 PPM\r\n");
        iter--;
        cs2200_out_freq -= freq_step;
        ratio = _32_BITS_RATIO(cs2200_out_freq, FOSC0);
        cs2200_freq_clk_adjust((uint16_t)ratio);
        cpu_delay_ms( 1000, sysclk_get_cpu_hz());
        while( twi_is_busy() );
      }
      else
        b_sweep_up=true;
    }
  }
}
