/**************************************************************************
 *
 * \file
 *
 * \brief Controller for QT60168 touch sensor.
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


#include "compiler.h"
#include "controller.h"
#include "board.h"
#include "gpio.h"
#include "rtc.h"
#include "spi.h"
#include "qt60168.h"
#include "conf_qt60168.h"
#include "cycle_counter.h"

#include <includes.h>
extern CPU_INT32U  BSP_INTC_IntReg (CPU_FNCT_VOID handler, CPU_INT32U irq, CPU_INT32U int_level);

#define CLEAR_PRESSED_STATE(x)  controller_state &= ~STATE_##x##_PRESSED
#define CLEAR_RELEASED_STATE(x) controller_state &= ~STATE_##x##_RELEASED
#define SET_PRESSED_STATE(x)    controller_state = (enum controller_state_t) ((controller_state & ~STATE_##x##_RELEASED) | STATE_##x##_PRESSED)
#define SET_RELEASED_STATE(x)   controller_state = (enum controller_state_t) ((controller_state & ~STATE_##x##_PRESSED) | STATE_##x##_RELEASED)

static enum
{
  TOUCH_IDLE    = 0,
  TOUCH_PRESS   = 1,
  TOUCH_RELEASE = 2
} touch_states[QT60168_TOUCH_NUMBER_OF_SENSORS];

enum controller_state_t
{
  STATE_IDLE                  = 0x0000,
  STATE_CS1_PRESSED           = 0x0001,
  STATE_CS2_PRESSED           = 0x0002,
  STATE_CS3_PRESSED           = 0x0004,
  STATE_CS4_PRESSED           = 0x0008,
  STATE_CS1_RELEASED          = 0x0010,
  STATE_CS2_RELEASED          = 0x0020,
  STATE_CS3_RELEASED          = 0x0040,
  STATE_CS4_RELEASED          = 0x0080,
  STATE_WHEEL_LEFT            = 0x0100,
  STATE_WHEEL_RIGHT           = 0x0200,
  STATE_WHEEL_LEFT_RELEASED   = 0x1000,
  STATE_WHEEL_RIGHT_RELEASED  = 0x2000,
  STATE_WHEEL_UP_RELEASED     = 0x4000,
  STATE_WHEEL_DOWN_RELEASED   = 0x8000
};

enum controller_state_t controller_state = STATE_IDLE;

static int wheel_step_counter = 0;

static int            Read_data   = 0;
static unsigned short Data;
static int            report_state= 0;
static t_cpu_time     cpu_time_clear_wheel;

bool        special_qt60168_get_reply60168_report_all_key(unsigned short *data);
static int  special_qt60168_get_reply(unsigned short *data);
static int  special_qt60168_send_cmd(unsigned char cmd);
static bool special_qt60168_report_all_key(unsigned short *data);
static void update_controller_state(void);
static void controller_keys_update(int all_keys);
void rtc_init_qt( void );

/** Interrupt *****************************************************************/


static void rtc_irq(void)
{
  static volatile int delay_count = 0;
  static unsigned short all_key = 0;
  static int update_delay = 0;

  delay_count++;
  update_delay++;

  if (update_delay > 10)
  {
    if (special_qt60168_report_all_key(&all_key) == true)
    {
      update_delay = 0;
      controller_keys_update(all_key);
    }
  }

  if (Read_data == 1)
  {
    Read_data = 2;
    delay_count = 0;
  }
  if ((delay_count > 1) && (Read_data == 2))
  {
    // We can read the DATA
    // Select QT60168
    spi_selectChip(QT60168_SPI,QT60168_SPI_NCPS);
    // Read Reply
    spi_read(QT60168_SPI, &Data);
    // Unselect QT60168
    spi_unselectChip(QT60168_SPI,QT60168_SPI_NCPS);
    Read_data = 0;
  }

  // clear the interrupt flag
  rtc_clear_interrupt(&AVR32_RTC);
}

inline static bool is_key_pressed(int all_keys, int key_num)
{
  return ((all_keys & (1 << key_num))?true:false);
}

static void controller_keys_update(int all_keys)
{
  int i;

  for (i=0; i<QT60168_TOUCH_NUMBER_OF_SENSORS; i++ )
  {
    // if (qt60168_is_key_pressed(i))
    if (is_key_pressed(all_keys, i))
      touch_states[i] = TOUCH_PRESS;
    else if (touch_states[i] == TOUCH_PRESS)
      touch_states[i] = TOUCH_RELEASE;
    else
      touch_states[i] = TOUCH_IDLE;
  }
  update_controller_state();
}

/** Initialization ************************************************************/
static void qt60168_resources_init(U32 fpba_hz)
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
    .baudrate     = 1000000, // Defined in conf_qt60168.h.
    .bits         = 8,         // Defined in conf_qt60168.h.
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
  spi_setupChipReg(QT60168_SPI, &spiOptions, 2*FOSC0);
}

void controller_init(U32 fcpu_hz, U32 fhsb_hz, U32 fpbb_hz, U32 fpba_hz) {
 qt60168_resources_init(fpba_hz);

  // Initialize QT60168 component.
  qt60168_init(fpba_hz);

  // Init timer to get key value.
  rtc_init_qt();

  // Invalidate the timeout already
  cpu_set_timeout(0, &cpu_time_clear_wheel);
}

void rtc_init_qt( void ) {
  // Init touch_states
  controller_clear();

  // Disable all interrupts
  Disable_global_interrupt();

  // Register the RTC interrupt handler to the interrupt controller.
  BSP_INTC_IntReg( &rtc_irq, AVR32_RTC_IRQ, AVR32_INTC_INT1);

  // Initialize the RTC
  rtc_init(&AVR32_RTC, RTC_OSC_32KHZ, 4);

  rtc_set_top_value(&AVR32_RTC, 1);
  // Enable the interrupts
  rtc_enable_interrupt(&AVR32_RTC);

  // Enable the RTC
  rtc_enable(&AVR32_RTC);

  // Enable global interrupts
  Enable_global_interrupt();

}

void controller_shutdown(void)
{
  // Disable all interrupts
  Disable_global_interrupt();
  // Disable the RTC
  rtc_disable(&AVR32_RTC);
  // Enable global interrupts
  Enable_global_interrupt();
}

/** "Report all key" status handling *******************************************/

static bool special_qt60168_report_all_key(unsigned short *data)
{
  static int AllKey=0;
  unsigned short dataread;

  switch(report_state) {
    case 0:
      AllKey=0;
      if(special_qt60168_send_cmd(QT60168_CMD_REPORT_ALL_KEYS)==0 ) {
        report_state=1;
      }
      break;
    case 1:
      if(special_qt60168_get_reply(&dataread)==0 ) {
        AllKey=dataread;
        report_state=2;
      }
      break;
    case 2:
      if(special_qt60168_get_reply(&dataread)==0 ) {
        AllKey|=dataread<<8;
        report_state=3;
      }
      break;
    case 3:
      if(special_qt60168_get_reply(&dataread)==0 ) {
        AllKey|=dataread<<16;
        report_state=4;
      }
      break;
    case 4:
      if(special_qt60168_get_reply(&dataread)==0 ) { // Trash CRC of QT60168_CMD_REPORT_ALL_KEYS
        // Update data
        *data = AllKey;
        report_state=0;
      }
      break;
    default:
      report_state=0;
  }
  if (report_state==0) {
     return true;
  }
  else {
     return false;
  }
}

static int special_qt60168_send_cmd(unsigned char cmd)
{
  static int state=0;
  switch(state) {
    case 0:
      // Select QT60168
      spi_selectChip(QT60168_SPI,QT60168_SPI_NCPS);
      // Write CMD
      spi_write(QT60168_SPI, cmd);
      // Unselect QT60168
      spi_unselectChip(QT60168_SPI,QT60168_SPI_NCPS);
      state = 1;
      Read_data=1;
    break;
    case 1:
      if(Read_data==0) {
        state=0;
      }
      break;
    default:
      state=0;
  }

  return state;
}

static int special_qt60168_get_reply(unsigned short *data)
{
    // Send NULL COMMAND
    if(special_qt60168_send_cmd(QT60168_CMD_NULL_COMMAND)==0) {
      *data = Data;
       return 0;
    }
    else {
      return 1;
    };
}

/** Set key status ************************************************************/

static inline void set_wheel_right()
{
  if (controller_state & STATE_WHEEL_LEFT)
  {
    controller_state &= ~STATE_WHEEL_LEFT;
    wheel_step_counter = 0;
  }
  controller_state |= STATE_WHEEL_RIGHT;
  wheel_step_counter++;
}

static inline void set_wheel_left()
{
  if (controller_state & STATE_WHEEL_RIGHT)
  {
    controller_state &= ~STATE_WHEEL_RIGHT;
    wheel_step_counter = 0;
  }
  controller_state |= STATE_WHEEL_LEFT;
  wheel_step_counter++;
}

static void update_controller_state(void)
{
  // Back key
  if (touch_states[QT60168_TOUCH_SENSOR_BUTTON_0] == TOUCH_PRESS)
    SET_PRESSED_STATE(CS1);
  else if (touch_states[QT60168_TOUCH_SENSOR_BUTTON_0] == TOUCH_RELEASE)
    SET_RELEASED_STATE(CS1);
  // Function 1 key
  if (touch_states[QT60168_TOUCH_SENSOR_BUTTON_1] == TOUCH_PRESS)
    SET_PRESSED_STATE(CS2);
  else if (touch_states[QT60168_TOUCH_SENSOR_BUTTON_1] == TOUCH_RELEASE)
    SET_RELEASED_STATE(CS2);
  // Function 2 key
  if (touch_states[QT60168_TOUCH_SENSOR_BUTTON_2] == TOUCH_PRESS)
    SET_PRESSED_STATE(CS3);
  else if (touch_states[QT60168_TOUCH_SENSOR_BUTTON_2] == TOUCH_RELEASE)
    SET_RELEASED_STATE(CS3);
  // Function 3 key
  if (touch_states[QT60168_TOUCH_SENSOR_BUTTON_3] == TOUCH_PRESS)
    SET_PRESSED_STATE(CS4);
  else if (touch_states[QT60168_TOUCH_SENSOR_BUTTON_3] == TOUCH_RELEASE)
    SET_RELEASED_STATE(CS4);

  // Wheel right
  if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_0] == TOUCH_RELEASE && touch_states[QT60168_TOUCH_SENSOR_WHEEL_1] == TOUCH_PRESS)
    set_wheel_right();
  else if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_1] == TOUCH_RELEASE && touch_states[QT60168_TOUCH_SENSOR_WHEEL_2] == TOUCH_PRESS)
    set_wheel_right();
  else if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_2] == TOUCH_RELEASE && touch_states[QT60168_TOUCH_SENSOR_WHEEL_3] == TOUCH_PRESS)
    set_wheel_right();
  else if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_3] == TOUCH_RELEASE && touch_states[QT60168_TOUCH_SENSOR_WHEEL_4] == TOUCH_PRESS)
    set_wheel_right();
  else if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_4] == TOUCH_RELEASE && touch_states[QT60168_TOUCH_SENSOR_WHEEL_5] == TOUCH_PRESS)
    set_wheel_right();
  else if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_5] == TOUCH_RELEASE && touch_states[QT60168_TOUCH_SENSOR_WHEEL_6] == TOUCH_PRESS)
    set_wheel_right();
  else if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_6] == TOUCH_RELEASE && touch_states[QT60168_TOUCH_SENSOR_WHEEL_7] == TOUCH_PRESS)
    set_wheel_right();
  else if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_7] == TOUCH_RELEASE && touch_states[QT60168_TOUCH_SENSOR_WHEEL_8] == TOUCH_PRESS)
    set_wheel_right();
  else if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_8] == TOUCH_RELEASE && touch_states[QT60168_TOUCH_SENSOR_WHEEL_9] == TOUCH_PRESS)
    set_wheel_right();
  else if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_9] == TOUCH_RELEASE && touch_states[QT60168_TOUCH_SENSOR_WHEEL_10] == TOUCH_PRESS)
    set_wheel_right();
  else if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_10] == TOUCH_RELEASE && touch_states[QT60168_TOUCH_SENSOR_WHEEL_11] == TOUCH_PRESS)
    set_wheel_right();
  else if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_11] == TOUCH_RELEASE && touch_states[QT60168_TOUCH_SENSOR_WHEEL_0] == TOUCH_PRESS)
    set_wheel_right();
  // Wheel left
  else if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_11] == TOUCH_PRESS && touch_states[QT60168_TOUCH_SENSOR_WHEEL_0] == TOUCH_RELEASE)
    set_wheel_left();
  else if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_10] == TOUCH_PRESS && touch_states[QT60168_TOUCH_SENSOR_WHEEL_11] == TOUCH_RELEASE)
    set_wheel_left();
  else if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_9] == TOUCH_PRESS && touch_states[QT60168_TOUCH_SENSOR_WHEEL_10] == TOUCH_RELEASE)
    set_wheel_left();
  else if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_8] == TOUCH_PRESS && touch_states[QT60168_TOUCH_SENSOR_WHEEL_9] == TOUCH_RELEASE)
    set_wheel_left();
  else if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_7] == TOUCH_PRESS && touch_states[QT60168_TOUCH_SENSOR_WHEEL_8] == TOUCH_RELEASE)
    set_wheel_left();
  else if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_6] == TOUCH_PRESS && touch_states[QT60168_TOUCH_SENSOR_WHEEL_7] == TOUCH_RELEASE)
    set_wheel_left();
  else if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_5] == TOUCH_PRESS && touch_states[QT60168_TOUCH_SENSOR_WHEEL_6] == TOUCH_RELEASE)
    set_wheel_left();
  else if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_4] == TOUCH_PRESS && touch_states[QT60168_TOUCH_SENSOR_WHEEL_5] == TOUCH_RELEASE)
    set_wheel_left();
  else if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_3] == TOUCH_PRESS && touch_states[QT60168_TOUCH_SENSOR_WHEEL_4] == TOUCH_RELEASE)
    set_wheel_left();
  else if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_2] == TOUCH_PRESS && touch_states[QT60168_TOUCH_SENSOR_WHEEL_3] == TOUCH_RELEASE)
    set_wheel_left();
  else if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_1] == TOUCH_PRESS && touch_states[QT60168_TOUCH_SENSOR_WHEEL_2] == TOUCH_RELEASE)
    set_wheel_left();
  else if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_0] == TOUCH_PRESS && touch_states[QT60168_TOUCH_SENSOR_WHEEL_1] == TOUCH_RELEASE)
    set_wheel_left();
  else if (!(controller_state & STATE_WHEEL_LEFT) && !(controller_state & STATE_WHEEL_RIGHT))
  {
    // Can be holding a key
    if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_9] == TOUCH_RELEASE)
      controller_state |= STATE_WHEEL_LEFT_RELEASED;
    if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_6] == TOUCH_RELEASE)
      controller_state |= STATE_WHEEL_DOWN_RELEASED;
    if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_3] == TOUCH_RELEASE)
      controller_state |= STATE_WHEEL_RIGHT_RELEASED;
    if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_0] == TOUCH_RELEASE)
      controller_state |= STATE_WHEEL_UP_RELEASED;
  }

  // Clear the wheel state after 500 ms
  if ((controller_state & STATE_WHEEL_LEFT || controller_state & STATE_WHEEL_RIGHT) &&
      wheel_step_counter)
    cpu_set_timeout(cpu_ms_2_cy(500, 48000000), &cpu_time_clear_wheel);
  // Clear the wheel state after 500 ms
  if ((controller_state & STATE_WHEEL_LEFT || controller_state & STATE_WHEEL_RIGHT) &&
      !wheel_step_counter && cpu_is_timeout(&cpu_time_clear_wheel))
  {
    controller_state &= ~STATE_WHEEL_LEFT;
    controller_state &= ~STATE_WHEEL_RIGHT;
  }
}

/******************************************************************************/

bool controller_key_pressed(void)
{
  if (controller_state == STATE_IDLE)
    return false;
  return true;
}

bool controller_key_released(void)
{
  if (controller_state & STATE_CS1_RELEASED ||
      controller_state & STATE_CS2_RELEASED ||
      controller_state & STATE_CS3_RELEASED ||
      controller_state & STATE_CS4_RELEASED)
    return true;
  return false;
}

bool controller_wheel_pressed(void)
{
  if (controller_state & STATE_WHEEL_LEFT ||
      controller_state & STATE_WHEEL_RIGHT)
    return true;
  return false;
}

bool controller_key_cs1(void)
{
  if (controller_state & STATE_CS1_RELEASED)
  {
    CLEAR_RELEASED_STATE(CS1);
    return true;
  }
  return false;
}

bool controller_key_cs2(void)
{
  if (controller_state & STATE_CS2_RELEASED)
  {
    CLEAR_RELEASED_STATE(CS2);
    return true;
  }
  return false;
}

bool controller_key_cs3(void)
{
  if (controller_state & STATE_CS3_RELEASED)
  {
    CLEAR_RELEASED_STATE(CS3);
    return true;
  }
  return false;
}

bool controller_key_cs4(void)
{
  if (controller_state & STATE_CS4_RELEASED)
  {
    CLEAR_RELEASED_STATE(CS4);
    return true;
  }
  return false;
}

bool controller_inquiry(void)
{
  return controller_key_cs1();
}

bool controller_select(void)
{
  return controller_key_cs2();
}

bool controller_connect(void)
{
  return controller_key_cs3();
}

bool controller_send(void)
{
  return controller_key_cs4();
}

void controller_clear(void)
{
  int i;

  for (i=0; i<QT60168_TOUCH_NUMBER_OF_SENSORS; i++)
    touch_states[i] = TOUCH_IDLE;
  controller_state = STATE_IDLE;
}

void controller_task(void)
{
}
