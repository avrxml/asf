/*****************************************************************************
 *
 * \file
 *
 * \brief Accelerometer example driver for AVR32 UC3.
 *
 * This file provides an example for the ADC + Accelerometer on AVR32 UC3 devices.
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
 *****************************************************************************/

/*! \mainpage
 * \section intro Introduction
 * This is the documentation for the data structures, functions, variables,
 * defines, enums, and typedefs for the lis3l06al accelerometer driver. \n This
 * documentation is also bundled with an example that covers various uses of this
 * accelerometer driver.
 *
 * <b>Operating mode:</b>
 * - Plug the USART1 of the EVK1101 to a PC serial port (see the section \ref configinfo
 * for details on the PC terminal settings)
 * - Flip the EVK1101 over to the left or right and see the axis values change
 * on your PC terminal with extra messages "left" or "right" and angles information
 * - Flip the EVK1101 forward or backward and see the axis values change on your
 * PC terminal with extra messages "up" or "down" and angles information
 * - Give a sudden acceleration in the movement of the board and you'll get a
 * special message on the USART1...
 *
 * \section files Main Files
 * - lis3l06al.c: accelerometer driver;
 * - lis3l06al.h: accelerometer driver header file;
 * - acc_example.c: accelerometer example application.
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC for AVR32 and for IAR Embedded Workbench
 * for Atmel AVR32. Other compilers may or may not work.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - EVK1101 evaluation kit;
 * - CPU clock: 12 MHz;
 * - USART1 connected to a PC serial port via a standard RS232 DB9 cable;
 * - PC terminal settings:
 *   - 57600 bps,
 *   - 8 data bits,
 *   - no parity bit,
 *   - 1 stop bit,
 *   - no flow control.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "stdio.h"
#include "board.h"
#include "print_funcs.h"
#include "gpio.h"
#include "pm.h"
#include "intc.h"
#include "lis3l06al.h"
#include "usart.h"

/*! \name Axis reference values for a set of predefined angles.
 */
//! @{
#define ACC_TRIG_X0 (int) ( ACC_ZERO_X - SIN15 * ACC_1G )
#define ACC_TRIG_X1 (int) ( ACC_ZERO_X + SIN15 * ACC_1G )
#define ACC_TRIG_Y0 (int) ( ACC_ZERO_Y - SIN15 * ACC_1G )
#define ACC_TRIG_Y1 (int) ( ACC_ZERO_Y + SIN15 * ACC_1G )
#define ACC_TRIG_Z_TOP (int) ( ACC_ZERO_Z - SIN75 * ACC_1G )
#define ACC_TRIG_Z_BOT (int) ( ACC_ZERO_Z + SIN60 * ACC_1G )
//! @}

/*! \brief Detect board orientation.
 */
void print_mouse()
{
  print_dbg("HEX Value for Channel x : 0x");
  print_dbg_hex(acc_get_m_x());
  if      ( is_acc_left()  ) { print_dbg(" < 0x") ; print_dbg_hex(ACC_TRIG_X0) ; print_dbg(" ... left") ; }
  else if ( is_acc_right() ) { print_dbg(" > 0x") ; print_dbg_hex(ACC_TRIG_X1) ; print_dbg(" ... right") ; }
  else                       { print_dbg(" = 0x") ; print_dbg_hex(ACC_ZERO_X) ; }
  print_dbg("\r\n");
  print_dbg("HEX Value for Channel y : 0x");
  print_dbg_hex(acc_get_m_y());
  if      ( is_acc_up()   ) { print_dbg(" < 0x") ; print_dbg_hex(ACC_TRIG_Y0) ; print_dbg(" ... up") ; }
  else if ( is_acc_down() ) { print_dbg(" > 0x") ; print_dbg_hex(ACC_TRIG_Y1) ; print_dbg(" ... down") ; }
  else                      { print_dbg(" = 0x") ; print_dbg_hex(ACC_ZERO_Y) ; }
  print_dbg("\r\n");
  print_dbg("HEX Value for Channel z : 0x");
  print_dbg_hex(acc_get_m_z());
  if ( is_acc_topdown() ) { print_dbg(" ... TOPDOWN") ; }
  if ( is_acc_meuh(1)   ) { print_dbg(" ... MEUH !!") ; }
  print_dbg("\r\n\r\n");
}


/*! \brief Detect 30 and 60 angles
 */
void print_angles()
{
  signed int res;
  static xyz_t angle ;

//  if ( ! is_acc_slow() ) { return }

  if(      0!=(res=is_acc_abs_angle_x(60)) ) angle.x = 60;
  else if( 0!=(res=is_acc_abs_angle_x(30)) ) angle.x = 30;
  else                                       angle.x = 0 ;

  if ( angle.x > 0 )
  {
     if(      res>0 ) print_dbg("LEFT ") ;
     else if( res<0 ) print_dbg("RIGHT ") ;

     print_dbg_ulong(angle.x) ;
     print_dbg("\r\n") ;
  }

  if(      0!=(res=is_acc_abs_angle_y(60)) ) angle.y = 60;
  else if( 0!=(res=is_acc_abs_angle_y(30)) ) angle.y = 30;
  else                                       angle.y = 0 ;

  if ( angle.y > 0 )
  {
     if(      res>0 ) print_dbg("DOWN ") ;
     else if( res<0 ) print_dbg("UP ") ;

     print_dbg_ulong(angle.y) ;
     print_dbg("\r\n") ;
  }
}



/*! \brief Display the tilt.
 * !! this is harder and certainly needs calibration + tuning !!
 */
void print_move()
{
  volatile U32 i;
  print_dbg("... T I L T !!!") ;
  print_dbg("\r\n");
  for ( i=0 ; i < 1000000 ; i++);
}



volatile char meuh_en = 0 ;
volatile char meuh_stop = 1 ;



#ifndef FREERTOS_USED

#  if __GNUC__

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

  // init debug serial line
  init_dbg_rs232(FOSC0);

  // Don't-care value for GCC.
  return 1;
}

#  elif __ICCAVR32__

/*! \brief Low-level initialization routine called during startup, before the
 *         main function.
 */
int __low_level_init(void)
{
  // Enable exceptions.
  Enable_global_exception();

  // Initialize interrupt handling.
  INTC_init_interrupts();

  // init debug serial line
  init_dbg_rs232(FOSC0);

  // Request initialization of data segments.
  return 1;
}
#  endif
#endif

/*! \brief This is an example demonstrating the accelerometer
 *         functionalities using the accelerometer driver.
 */
int main(void)
{
  volatile unsigned long i;

  // switch to oscillator 0
  pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);

  // Initialize the debug USART module.
  init_dbg_rs232(FOSC0);

  acc_init();

  // do a loop
  for (;;)
  {
    // slow down operations
    for ( i=0 ; i < 50000 ; i++);

    // display a header to user
    print_dbg("\x1B[2J\x1B[H\r\n\r\nAccelerometer Example\r\n\r\n");

    // Get accelerometer acquisition and process data
    acc_update();

    // test for fast or slow changes
    // depending on that, play with angles or moves
    if ( is_acc_slow() )
    {
      print_mouse() ;
      print_angles() ;
    }
    else
      print_move() ;

    // MEUH
    // only text here , needs to be combined with PWM
    // meuh_stop is the "end" signal from PWM
    meuh_en = is_acc_meuh( meuh_stop ) ;
  }
}
