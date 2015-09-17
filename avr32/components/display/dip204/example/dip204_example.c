/*****************************************************************************
 *
 * \file
 *
 * \brief LCD DIP204 example driver for EVK1100 board.
 *
 * This file provides a useful example for the LCD DIP204 on SPI interface.
 * Press PB0 to see the full set of available chars on the LCD
 * Press PB1 to decrease the backlight power of the LCD
 * Press PB2 to increase the backlight power of the LCD
 * Use Joystick to see arrows displayed on the LCD
 * Press Joystick to return to the idle screen
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
 * This documents data structures, functions, variables, defines, enums, and
 * typedefs in the software. <BR>It also gives an example of the usage of the
 * DIP204 LCD on EVK1100. \n \n
 * <b>Example's operating mode:</b>
 * <ul>
 * <li>A default message is displayed on the 4 lines of the LCD
 * <li>Press PB0 to see the full set of available chars on the LCD
 * <li>Press PB1 to decrease the backlight power of the LCD
 * <li>Press PB2 to increase the backlight power of the LCD
 * <li>Use the joystick to see arrows displayed on the LCD
 * <li>Press the joystick to see a circle displayed on the LCD and to return to the
 *     idle screen (displaying the default message)
 *
 * </ul>
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32 and IAR Systems compiler
 * for AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Info
 * All AVR32UC devices with an SPI module can be used. This example has been tested
 * with the following setup:
 *- EVK1100 evaluation kit
 *
 * \section setupinfo Setup Information
 * CPU speed: <i> 12 MHz </i>
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#include "board.h"
#include "compiler.h"
#include "dip204.h"
#include "intc.h"
#include "gpio.h"
#include "pm.h"
#include "delay.h"
#include "spi.h"
#include "conf_clock.h"


/*! define the push button to see available char map on LCD */
#define GPIO_CHARSET            GPIO_PUSH_BUTTON_0

/*! define the push button to decrease back light power */
#define GPIO_BACKLIGHT_MINUS    GPIO_PUSH_BUTTON_1

/*! define the push button to increase back light power */
#define GPIO_BACKLIGHT_PLUS     GPIO_PUSH_BUTTON_2

/*! flag set when joystick display starts to signal main function to clear this display */
unsigned short display;

/*! current char displayed on the 4th line */
unsigned short current_char = 0;


/*!
 * \brief The Push Buttons interrupt handler.
 */
#if __GNUC__
__attribute__((__interrupt__))
#elif __ICCAVR32__
__interrupt
#endif
static void dip204_example_PB_int_handler(void)
{
unsigned short i;

  /* display all available chars */
  if (gpio_get_pin_interrupt_flag(GPIO_CHARSET))
  {
    /* go to first column of 4th line */
    dip204_set_cursor_position(1,4);
    /* display 20 chars of charmap */
    for (i = current_char; i < current_char + 0x10; i++)
    {
      dip204_write_data(i);
    }
    dip204_write_string("    ");
    /* mark position in charmap */
    current_char = i;
    /* reset marker */
    if (current_char >= 0xFF)
    {
      current_char = 0x10;
    }
    /* allow new interrupt : clear the IFR flag */
    gpio_clear_pin_interrupt_flag(GPIO_CHARSET);
  }
  /* increase backlight power */
  if (gpio_get_pin_interrupt_flag(GPIO_BACKLIGHT_PLUS))
  {
    dip204_set_backlight(backlight_power_increase);
    /* allow new interrupt : clear the IFR flag */
    gpio_clear_pin_interrupt_flag(GPIO_BACKLIGHT_PLUS);
  }
  /* decrease backlight power */
  if (gpio_get_pin_interrupt_flag(GPIO_BACKLIGHT_MINUS))
  {
    dip204_set_backlight(backlight_power_decrease);
    /* allow new interrupt : clear the IFR flag */
    gpio_clear_pin_interrupt_flag(GPIO_BACKLIGHT_MINUS);
  }
}


/*!
 * \brief The joystick interrupt handler.
 */
#if __GNUC__
__attribute__((__interrupt__))
#elif __ICCAVR32__
__interrupt
#endif
static void dip204_example_Joy_int_handler(void)
{
  if (gpio_get_pin_interrupt_flag(GPIO_JOYSTICK_UP))
  {
    dip204_set_cursor_position(19,1);
    dip204_write_data(0xDE);
    display = 1;
    /* allow new interrupt : clear the IFR flag */
    gpio_clear_pin_interrupt_flag(GPIO_JOYSTICK_UP);
  }
  if (gpio_get_pin_interrupt_flag(GPIO_JOYSTICK_DOWN))
  {
    dip204_set_cursor_position(19,3);
    dip204_write_data(0xE0);
    display = 1;
    /* allow new interrupt : clear the IFR flag */
    gpio_clear_pin_interrupt_flag(GPIO_JOYSTICK_DOWN);
  }
  if (gpio_get_pin_interrupt_flag(GPIO_JOYSTICK_LEFT))
  {
    dip204_set_cursor_position(18,2);
    dip204_write_data(0xE1);
    display = 1;
    /* allow new interrupt : clear the IFR flag */
    gpio_clear_pin_interrupt_flag(GPIO_JOYSTICK_LEFT);
  }
  if (gpio_get_pin_interrupt_flag(GPIO_JOYSTICK_RIGHT))
  {
    dip204_set_cursor_position(20,2);
    dip204_write_data(0xDF);
    display = 1;
    /* allow new interrupt : clear the IFR flag */
    gpio_clear_pin_interrupt_flag(GPIO_JOYSTICK_RIGHT);
  }
  if (gpio_get_pin_interrupt_flag(GPIO_JOYSTICK_PUSH))
  {
    dip204_set_cursor_position(19,2);
    dip204_write_data(0xBB);
    dip204_set_cursor_position(1,4);
    dip204_write_string("  AT32UC3A Series   ");
    display = 1;
    /* allow new interrupt : clear the IFR flag */
    gpio_clear_pin_interrupt_flag(GPIO_JOYSTICK_PUSH);
  }
}


/*!
 * \brief function to configure push button to generate IT upon rising edge
 */
void dip204_example_configure_push_buttons_IT(void)
{
  gpio_enable_pin_interrupt(GPIO_CHARSET , GPIO_RISING_EDGE);

  gpio_enable_pin_interrupt(GPIO_BACKLIGHT_PLUS , GPIO_RISING_EDGE);

  gpio_enable_pin_interrupt(GPIO_BACKLIGHT_MINUS , GPIO_RISING_EDGE);

  /* Disable all interrupts */
  Disable_global_interrupt();
  /* register PB0 handler on level 1 */
  INTC_register_interrupt( &dip204_example_PB_int_handler, AVR32_GPIO_IRQ_0 + (GPIO_BACKLIGHT_PLUS/8), AVR32_INTC_INT1);
  INTC_register_interrupt( &dip204_example_PB_int_handler, AVR32_GPIO_IRQ_0 + (GPIO_BACKLIGHT_MINUS/8), AVR32_INTC_INT1);
  INTC_register_interrupt( &dip204_example_PB_int_handler, AVR32_GPIO_IRQ_0 + (GPIO_CHARSET/8), AVR32_INTC_INT1);
  /* Enable all interrupts */
  Enable_global_interrupt();
}


/*!
 * \brief function to configure joystick to generate IT upon falling edge
 */
void dip204_example_configure_joystick_IT(void)
{
  gpio_enable_pin_interrupt(GPIO_JOYSTICK_UP , GPIO_FALLING_EDGE);
  gpio_enable_pin_interrupt(GPIO_JOYSTICK_DOWN , GPIO_FALLING_EDGE);
  gpio_enable_pin_interrupt(GPIO_JOYSTICK_RIGHT , GPIO_FALLING_EDGE);
  gpio_enable_pin_interrupt(GPIO_JOYSTICK_PUSH , GPIO_FALLING_EDGE);
  gpio_enable_pin_interrupt(GPIO_JOYSTICK_LEFT , GPIO_FALLING_EDGE);

  /* Disable all interrupts */
  Disable_global_interrupt();
  /* register PB0 handler on level 1 */
  INTC_register_interrupt( &dip204_example_Joy_int_handler, AVR32_GPIO_IRQ_0 + (GPIO_JOYSTICK_UP/8), AVR32_INTC_INT1);
  INTC_register_interrupt( &dip204_example_Joy_int_handler, AVR32_GPIO_IRQ_0 + (GPIO_JOYSTICK_DOWN/8), AVR32_INTC_INT1);
  INTC_register_interrupt( &dip204_example_Joy_int_handler, AVR32_GPIO_IRQ_0 + (GPIO_JOYSTICK_RIGHT/8), AVR32_INTC_INT1);
  INTC_register_interrupt( &dip204_example_Joy_int_handler, AVR32_GPIO_IRQ_0 + (GPIO_JOYSTICK_LEFT/8), AVR32_INTC_INT1);
  INTC_register_interrupt( &dip204_example_Joy_int_handler, AVR32_GPIO_IRQ_0 + (GPIO_JOYSTICK_PUSH/8), AVR32_INTC_INT1);
  /* Enable all interrupts */
  Enable_global_interrupt();
}

/*!
 * \brief main function : do init and loop (poll if configured so)
 */
int main(void)
{
  static const gpio_map_t DIP204_SPI_GPIO_MAP =
  {
    {DIP204_SPI_SCK_PIN,  DIP204_SPI_SCK_FUNCTION },  // SPI Clock.
    {DIP204_SPI_MISO_PIN, DIP204_SPI_MISO_FUNCTION},  // MISO.
    {DIP204_SPI_MOSI_PIN, DIP204_SPI_MOSI_FUNCTION},  // MOSI.
    {DIP204_SPI_NPCS_PIN, DIP204_SPI_NPCS_FUNCTION}   // Chip Select NPCS.
  };

  // Switch the CPU main clock to oscillator 0
  pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);

  // Disable all interrupts.
  Disable_global_interrupt();

  // init the interrupts
  INTC_init_interrupts();

  // Enable all interrupts.
  Enable_global_interrupt();

  // add the spi options driver structure for the LCD DIP204
  spi_options_t spiOptions =
  {
    .reg          = DIP204_SPI_NPCS,
    .baudrate     = 1000000,
    .bits         = 8,
    .spck_delay   = 0,
    .trans_delay  = 0,
    .stay_act     = 1,
    .spi_mode     = 0,
    .modfdis      = 1
  };

  // Assign I/Os to SPI
  gpio_enable_module(DIP204_SPI_GPIO_MAP,
                     sizeof(DIP204_SPI_GPIO_MAP) / sizeof(DIP204_SPI_GPIO_MAP[0]));

  // Initialize as master
  spi_initMaster(DIP204_SPI, &spiOptions);

  // Set selection mode: variable_ps, pcs_decode, delay
  spi_selectionMode(DIP204_SPI, 0, 0, 0);

  // Enable SPI
  spi_enable(DIP204_SPI);

  // setup chip registers
  spi_setupChipReg(DIP204_SPI, &spiOptions, FOSC0);

  // configure local push buttons
  dip204_example_configure_push_buttons_IT();

  // configure local joystick
  dip204_example_configure_joystick_IT();

  // initialize LCD
  dip204_init(backlight_PWM, true);

  // reset marker
  current_char = 0x10;

  // Display default message.
  dip204_set_cursor_position(8,1);
  dip204_write_string("ATMEL");
  dip204_set_cursor_position(7,2);
  dip204_write_string("EVK1100");
  dip204_set_cursor_position(6,3);
  dip204_write_string("AVR32 UC3");
  dip204_set_cursor_position(3,4);
  dip204_write_string("AT32UC3A Series");
  dip204_hide_cursor();

  /* do a loop */
  while (1)
  {
    if (display)
    {
      delay_ms(400);  // A delay so that it is humanly possible to see the
                      // character(s) before they are cleared.
      // Clear line 1 column 19
      dip204_set_cursor_position(19,1);
      dip204_write_string(" ");
      // Clear line 2 from column 18 to column 20.
      dip204_set_cursor_position(18,2);
      dip204_write_string("   "); // 3 spaces
      // Clear line 3 column 19
      dip204_set_cursor_position(19,3);
      dip204_write_string(" ");
      display = 0;
    }
  }
}
