/*****************************************************************************
 *
 * \file
 *
 * \brief QT60168 driver example driver for AVR32 UC3.
 *
 * This file provides an example for the QT60168 driver.
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
 * defines, enums, and typedefs for the QT60168 software driver.\n It also comes
 * bundled with an example. Information on QT60168 device can be found here:
 * http://www.atmel.com/dyn/products/product_card.asp?part_id=4467
 * www.atmel.com/dyn/resources/prod_documents/qt60248_402.pdf
 *
 * \section files Main Files
 * - qt60168_example.c: the example
 * - qt60168.h: the components interface for the QT60168
 * - CONF/conf_qt60168.h : The configuration file
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC for AVR32 and for IAR Embedded Workbench
 * for Atmel AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Information
 * All AVR32 devices with a SPI module can be used.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - EVK1104 evaluation kits;
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


//_____  I N C L U D E S ___________________________________________________
#include "board.h"
#include "gpio.h"
#include "spi.h"
#include "pm.h"
#include "gpio.h"
#include "spi.h"
#include "print_funcs.h"
#include "usart.h"
#include "et024006dhu.h"

#include "qt60168.h"
#include "conf_qt60168.h"

#define DEMO_START_X                0
#define DEMO_START_Y               38
#define DEMO_SIZE_X    ET024006_WIDTH
#define DEMO_SIZE_Y               160

#define DEMO_WHEEL_RADIUS           60                                      // in pixels
#define DEMO_WHEEL_START_X          ET024006_WIDTH/2 - DEMO_WHEEL_SIZE_X/2  // in pixels
#define DEMO_WHEEL_START_Y          ET024006_HEIGHT/2 - DEMO_WHEEL_SIZE_Y/2 // in pixels
#define DEMO_WHEEL_SIZE_X           20                                      // in pixels
#define DEMO_WHEEL_SIZE_Y           20                                      // in pixels

#define SIN0  0
#define SIN30 0.5
#define SIN60 0.87
#define SIN90 1

#define COS0  1
#define COS30 0.87
#define COS60 0.5
#define COS90 0



/*! \brief Initializes QT60168 resources: GPIO and SPI
 */
static void qt60168_resources_init(void)
{
  static const gpio_map_t QT60168_SPI_GPIO_MAP =
  {
    {QT60168_SPI_SCK_PIN,          QT60168_SPI_SCK_FUNCTION         },  // SPI Clock.
    {QT60168_SPI_MISO_PIN,         QT60168_SPI_MISO_FUNCTION        },  // MISO.
    {QT60168_SPI_MOSI_PIN,         QT60168_SPI_MOSI_FUNCTION        },  // MOSI.
    {QT60168_SPI_NPCS0_PIN,        QT60168_SPI_NPCS0_FUNCTION}  // Chip Select NPCS.
  };

  // SPI options.
  spi_options_t spiOptions =
  {
    .reg          = QT60168_SPI_NCPS,
    .baudrate     = QT60168_SPI_MASTER_SPEED, // Defined in conf_qt60168.h.
    .bits         = QT60168_SPI_BITS,         // Defined in conf_qt60168.h.
    .spck_delay   = 0,
    .trans_delay  = 0,
    .stay_act     = 0,
    .spi_mode     = 3,
    .modfdis      = 1
  };

  // Assign I/Os to SPI.
  gpio_enable_module(QT60168_SPI_GPIO_MAP,
                     sizeof(QT60168_SPI_GPIO_MAP) / sizeof(QT60168_SPI_GPIO_MAP[0]));

  // Initialize as master.
  spi_initMaster(QT60168_SPI, &spiOptions);

  // Set selection mode: variable_ps, pcs_decode, delay.
  spi_selectionMode(QT60168_SPI, 0, 0, 0);

  // Enable SPI.
  spi_enable(QT60168_SPI);

  // Initialize QT60168 with SPI clock Osc0.
  spi_setupChipReg(QT60168_SPI, &spiOptions, FOSC0);
}


typedef enum
{
  DEMO_COLOR_ALL=0
, DEMO_COLOR_BLUE
, DEMO_COLOR_RED
, DEMO_COLOR_GREEN
, DEMO_COLOR_MAX
} demo_color_t;

typedef enum
{
  DEMO_DISPLAY_BOXES=0
, DEMO_DISPLAY_WHEEL
, DEMO_DISPLAY_MAX
} demo_display_t;

/*! \brief Main function
 */
int main(void)
{
  int i;
  bool idle=false; // Detect key transition (PRESSED -> RELEASED)
  U32 x_start;
  U32 y_start;
  U32 x_size;
  U32 y_size;
  U16 color;
  const U16 icon[QT60168_TOUCH_NUMBER_OF_SENSORS] = {0, 1*16, 2*16, 3*16, 4*16, 5*16, -1, -1, 6*16, 7*16, 8*16, 9*16, 10*16, 11*16, -1, -1};
  demo_color_t    demo_color=DEMO_COLOR_ALL;
  demo_display_t  demo_display=DEMO_DISPLAY_WHEEL;
  bool touch_states[QT60168_TOUCH_NUMBER_OF_SENSORS];

  // Switch the main clock to the external oscillator 0
  pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);

  // Initialize RS232 debug text output.
  init_dbg_rs232(FOSC0);

  // Initialize QT60168 resources: GPIO, SPI and QT60168.
  qt60168_resources_init();

  // Initialize QT60168 component.
  qt60168_init(FOSC0);

  // Initialize the LCD.
  et024006_Init(  FOSC0/*CPU*/, FOSC0/*HSB*/);

  // Clear the display i.e. make it black
  et024006_DrawFilledRect(0, 0, ET024006_WIDTH, ET024006_HEIGHT, BLACK );

  // Set the backlight.
  gpio_set_gpio_pin(ET024006DHU_BL_PIN);

  // Display welcome string.
  et024006_PrintString("QT60168 EXAMPLE", (const unsigned char *)&FONT8x8, 110,  5, WHITE, -1);
  et024006_PrintString("Press the QTouch sensors.", (const unsigned char *)&FONT6x8,  95, 20, WHITE, -1);
  et024006_PrintString("Color: All", (const unsigned char *)&FONT6x8,  10, 200, WHITE, -1);
  et024006_PrintString("Display sensors", (const unsigned char *)&FONT6x8,  120, 200, WHITE, -1);

  et024006_DrawLine(DEMO_START_X, DEMO_START_Y-1, DEMO_START_X+DEMO_SIZE_X, DEMO_START_Y-1, WHITE );
  et024006_DrawLine(DEMO_START_X, DEMO_START_Y+DEMO_SIZE_Y+1, DEMO_START_X+DEMO_SIZE_X, DEMO_START_Y+DEMO_SIZE_Y+1, WHITE );

  // Memorize the status for each key.
  for( i=0 ; i<QT60168_TOUCH_NUMBER_OF_SENSORS ; i++ )
    touch_states[i] = qt60168_is_key_pressed(i);

  // Set LED state in a known state.
  gpio_set_gpio_pin(LED0_GPIO);
  gpio_set_gpio_pin(LED1_GPIO);
  gpio_set_gpio_pin(LED2_GPIO);
  gpio_set_gpio_pin(LED3_GPIO);

  while(1)
  {
    for( i=0 ; i<QT60168_TOUCH_NUMBER_OF_SENSORS ; i++)
    {
      // Test Press event on sensors
      //
      if( !touch_states[i] && qt60168_is_key_pressed(i) )
      {
        touch_states[i] = true;

        if( i==QT60168_TOUCH_SENSOR_BUTTON_0 )
        {
          gpio_tgl_gpio_pin(LED0_GPIO);
          et024006_PrintString("B0", (const unsigned char *)&FONT6x8,  10, 215, WHITE, -1);
          demo_color=(demo_color+1) % DEMO_COLOR_MAX;

          // Erase previous line
          et024006_DrawFilledRect(10, 200, 80, 10, BLACK );
          switch( demo_color )
          {
          case DEMO_COLOR_BLUE:
            et024006_PrintString("Color: Blue", (const unsigned char *)&FONT6x8,  10, 200, WHITE, -1);
            break;
          case DEMO_COLOR_RED:
            et024006_PrintString("Color: Red", (const unsigned char *)&FONT6x8,  10, 200, WHITE, -1);
            break;
          case DEMO_COLOR_GREEN:
            et024006_PrintString("Color: Green", (const unsigned char *)&FONT6x8,  10, 200, WHITE, -1);
            break;
          default:
            et024006_PrintString("Color: All", (const unsigned char *)&FONT6x8,  10, 200, WHITE, -1);
            break;
          }
        }
        else if( i==QT60168_TOUCH_SENSOR_BUTTON_1 )
        {
          gpio_tgl_gpio_pin(LED1_GPIO);
          et024006_PrintString("B1", (const unsigned char *)&FONT6x8,  30, 215, WHITE, -1);
          demo_display=(demo_display+1) % DEMO_DISPLAY_MAX;

          // Erase previous line
          et024006_DrawFilledRect(120, 200, 160, 10, BLACK );
          switch( demo_display )
          {
          case DEMO_DISPLAY_WHEEL:
            et024006_PrintString("Display sensors", (const unsigned char *)&FONT6x8,  120, 200, WHITE, -1);
            break;
          case DEMO_DISPLAY_BOXES:
          default:
            et024006_PrintString("Display random boxes", (const unsigned char *)&FONT6x8,  120, 200, WHITE, -1);
            break;
          }
          // Erase display
          et024006_DrawFilledRect(DEMO_START_X, DEMO_START_Y, DEMO_SIZE_X, DEMO_SIZE_Y, BLACK );
        }
        else if( i==QT60168_TOUCH_SENSOR_BUTTON_2 )
        {
          gpio_tgl_gpio_pin(LED2_GPIO);
          et024006_PrintString("B2", (const unsigned char *)&FONT6x8,  50, 215, WHITE, -1);
        }

        else if( i==QT60168_TOUCH_SENSOR_BUTTON_3 )
        {
          gpio_tgl_gpio_pin(LED3_GPIO);
          et024006_PrintString("B3", (const unsigned char *)&FONT6x8,  70, 215, WHITE, -1);
        }
        else
        {
          // Press transition detected for the wheel
          idle = false;

          // Draw Wheel[i]
          et024006_DrawFilledRect(100 + icon[i], 215-2, 10, 10, WHITE );
        }
      }



      // Test Release event on sensors
      //
      if(touch_states[i] && !qt60168_is_key_pressed(i))
      {
        touch_states[i] = false;
        if( i==QT60168_TOUCH_SENSOR_BUTTON_0 )
        { // Erase "B0"
          et024006_DrawFilledRect(10, 215-2, 12, 12, BLACK );
        }
        else if( i==QT60168_TOUCH_SENSOR_BUTTON_1 )
        { // Erase "B1"
          et024006_DrawFilledRect(30, 215-2, 12, 12, BLACK );
        }
        else if( i==QT60168_TOUCH_SENSOR_BUTTON_2 )
        { // Erase "B2"
          et024006_DrawFilledRect(50, 215-2, 12, 12, BLACK );
        }
        else if( i==QT60168_TOUCH_SENSOR_BUTTON_3 )
        { // Erase "B3"
          et024006_DrawFilledRect(70, 215-2, 12, 12, BLACK );
        }
        else
        { // Erase Wheel[i]
          et024006_DrawFilledRect(100 + icon[i], 215-2, 10, 10, BLACK );
        }
      }
    } // for...



    if( demo_display==DEMO_DISPLAY_WHEEL )
    {
      if( touch_states[QT60168_TOUCH_SENSOR_BUTTON_0] ) color = WHITE;
      else                                              color = BLUE;
      et024006_DrawFilledRect(30, 50, DEMO_WHEEL_SIZE_X, DEMO_WHEEL_SIZE_Y, color );

      if( touch_states[QT60168_TOUCH_SENSOR_BUTTON_1] ) color = WHITE;
      else                                              color = BLUE;
      et024006_DrawFilledRect(30, 80, DEMO_WHEEL_SIZE_X, DEMO_WHEEL_SIZE_Y, color );

      if( touch_states[QT60168_TOUCH_SENSOR_BUTTON_2] ) color = WHITE;
      else                                              color = BLUE;
      et024006_DrawFilledRect(30, 110, DEMO_WHEEL_SIZE_X, DEMO_WHEEL_SIZE_Y, color );

      if( touch_states[QT60168_TOUCH_SENSOR_BUTTON_3] ) color = WHITE;
      else                                              color = BLUE;
      et024006_DrawFilledRect(30, 140, DEMO_WHEEL_SIZE_X, DEMO_WHEEL_SIZE_Y, color );

      if( touch_states[QT60168_TOUCH_SENSOR_WHEEL_0] ) color = WHITE;
      else                                             color = BLUE;
      et024006_DrawFilledRect(DEMO_WHEEL_START_X + DEMO_WHEEL_RADIUS*SIN0,
                              DEMO_WHEEL_START_Y - DEMO_WHEEL_RADIUS*COS0,
                              DEMO_WHEEL_SIZE_X, DEMO_WHEEL_SIZE_Y, color );

      if( touch_states[QT60168_TOUCH_SENSOR_WHEEL_1] ) color = WHITE;
      else                                             color = BLUE;
      et024006_DrawFilledRect(DEMO_WHEEL_START_X + DEMO_WHEEL_RADIUS*SIN30,
                              DEMO_WHEEL_START_Y - DEMO_WHEEL_RADIUS*COS30,
                              DEMO_WHEEL_SIZE_X, DEMO_WHEEL_SIZE_Y, color );

      if( touch_states[QT60168_TOUCH_SENSOR_WHEEL_2] ) color = WHITE;
      else                                             color = BLUE;
      et024006_DrawFilledRect(DEMO_WHEEL_START_X + DEMO_WHEEL_RADIUS*SIN60,
                              DEMO_WHEEL_START_Y - DEMO_WHEEL_RADIUS*COS60,
                              DEMO_WHEEL_SIZE_X, DEMO_WHEEL_SIZE_Y, color );

      if( touch_states[QT60168_TOUCH_SENSOR_WHEEL_3] ) color = WHITE;
      else                                             color = BLUE;
      et024006_DrawFilledRect(DEMO_WHEEL_START_X + DEMO_WHEEL_RADIUS*SIN90,
                              DEMO_WHEEL_START_Y - DEMO_WHEEL_RADIUS*COS90,
                              DEMO_WHEEL_SIZE_X, DEMO_WHEEL_SIZE_Y, color );

      if( touch_states[QT60168_TOUCH_SENSOR_WHEEL_4] ) color = WHITE;
      else                                             color = BLUE;
      et024006_DrawFilledRect(DEMO_WHEEL_START_X + DEMO_WHEEL_RADIUS*SIN60,
                              DEMO_WHEEL_START_Y + DEMO_WHEEL_RADIUS*COS60,
                              DEMO_WHEEL_SIZE_X, DEMO_WHEEL_SIZE_Y, color );

      if( touch_states[QT60168_TOUCH_SENSOR_WHEEL_5] ) color = WHITE;
      else                                             color = BLUE;
      et024006_DrawFilledRect(DEMO_WHEEL_START_X + DEMO_WHEEL_RADIUS*SIN30,
                              DEMO_WHEEL_START_Y + DEMO_WHEEL_RADIUS*COS30,
                              DEMO_WHEEL_SIZE_X, DEMO_WHEEL_SIZE_Y, color );

      if( touch_states[QT60168_TOUCH_SENSOR_WHEEL_6] ) color = WHITE;
      else                                             color = BLUE;
      et024006_DrawFilledRect(DEMO_WHEEL_START_X - DEMO_WHEEL_RADIUS*SIN0,
                              DEMO_WHEEL_START_Y + DEMO_WHEEL_RADIUS*COS0,
                              DEMO_WHEEL_SIZE_X, DEMO_WHEEL_SIZE_Y, color );

      if( touch_states[QT60168_TOUCH_SENSOR_WHEEL_7] ) color = WHITE;
      else                                             color = BLUE;
      et024006_DrawFilledRect(DEMO_WHEEL_START_X - DEMO_WHEEL_RADIUS*SIN30,
                              DEMO_WHEEL_START_Y + DEMO_WHEEL_RADIUS*COS30,
                              DEMO_WHEEL_SIZE_X, DEMO_WHEEL_SIZE_Y, color );

      if( touch_states[QT60168_TOUCH_SENSOR_WHEEL_8] ) color = WHITE;
      else                                             color = BLUE;
      et024006_DrawFilledRect(DEMO_WHEEL_START_X - DEMO_WHEEL_RADIUS*SIN60,
                              DEMO_WHEEL_START_Y + DEMO_WHEEL_RADIUS*COS60,
                              DEMO_WHEEL_SIZE_X, DEMO_WHEEL_SIZE_Y, color );

      if( touch_states[QT60168_TOUCH_SENSOR_WHEEL_9] ) color = WHITE;
      else                                             color = BLUE;
      et024006_DrawFilledRect(DEMO_WHEEL_START_X - DEMO_WHEEL_RADIUS*SIN90,
                              DEMO_WHEEL_START_Y + DEMO_WHEEL_RADIUS*COS90,
                              DEMO_WHEEL_SIZE_X, DEMO_WHEEL_SIZE_Y, color );

      if( touch_states[QT60168_TOUCH_SENSOR_WHEEL_10] ) color = WHITE;
      else                                              color = BLUE;
      et024006_DrawFilledRect(DEMO_WHEEL_START_X - DEMO_WHEEL_RADIUS*SIN60,
                              DEMO_WHEEL_START_Y - DEMO_WHEEL_RADIUS*COS60,
                              DEMO_WHEEL_SIZE_X, DEMO_WHEEL_SIZE_Y, color );

      if( touch_states[QT60168_TOUCH_SENSOR_WHEEL_11] ) color = WHITE;
      else                                              color = BLUE;
      et024006_DrawFilledRect(DEMO_WHEEL_START_X - DEMO_WHEEL_RADIUS*SIN30,
                              DEMO_WHEEL_START_Y - DEMO_WHEEL_RADIUS*COS30,
                              DEMO_WHEEL_SIZE_X, DEMO_WHEEL_SIZE_Y, color );
    }

    else if( !idle && ( demo_display==DEMO_DISPLAY_BOXES ) )
    { // Display a box randomly on the screen.
      idle = true;
      x_start = DEMO_START_X + rand()%DEMO_SIZE_X;
      y_start = DEMO_START_Y + rand()%DEMO_SIZE_Y;
      x_size  = rand()%(DEMO_START_X+DEMO_SIZE_X-x_start);
      y_size  = rand()%(DEMO_START_Y+DEMO_SIZE_Y-y_start);
      color   = rand()%0x10000;
      switch( demo_color )
      {
      case DEMO_COLOR_BLUE:
        color = color & BLUE;
        break;
      case DEMO_COLOR_RED:
        color = color & RED;
        break;
      case DEMO_COLOR_GREEN:
        color = color & GREEN;
        break;
      default:
        break;
      }

      et024006_DrawFilledRect(
        x_start
      , y_start
      , x_size
      , y_size
      , color );
    }
  } // while(1)...
}
