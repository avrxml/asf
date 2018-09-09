/*****************************************************************************
 *
 * \file
 *
 * \brief Controller
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include "controller.h"
#include "board.h"
#include "gpio.h"
#include "spi.h"
#include "qt60168.h"
#include "conf_qt60168.h"
#include "cycle_counter.h"

#define CLEAR_PRESSED_STATE(x)  controller_state &= ~STATE_##x##_PRESSED
#define CLEAR_RELEASED_STATE(x) controller_state &= ~STATE_##x##_RELEASED
#define SET_PRESSED_STATE(x)    controller_state = (controller_state & ~STATE_##x##_RELEASED) | STATE_##x##_PRESSED
#define SET_RELEASED_STATE(x)   controller_state = (controller_state & ~STATE_##x##_PRESSED) | STATE_##x##_RELEASED

static enum
{
  TOUCH_IDLE    = 0,
  TOUCH_PRESS   = 1,
  TOUCH_RELEASE = 2
} touch_states[QT60168_TOUCH_NUMBER_OF_SENSORS];

static enum
{
  STATE_IDLE              = 0x0000,
  STATE_BACK_PRESSED      = 0x0001,
  STATE_FCT1_PRESSED      = 0x0002,
  STATE_FCT2_PRESSED      = 0x0004,
  STATE_FCT3_PRESSED      = 0x0008,
  STATE_BACK_RELEASED     = 0x0010,
  STATE_FCT1_RELEASED     = 0x0020,
  STATE_FCT2_RELEASED     = 0x0040,
  STATE_FCT3_RELEASED     = 0x0080,
  STATE_WHEEL_LEFT        = 0x0100,
  STATE_WHEEL_RIGHT       = 0x0200,
  STATE_BACK_PRESSING     = 0x0400,
  STATE_BACK_LONG_PRESS   = 0x0800
} controller_state = STATE_IDLE;

static int wheel_step_counter = 0;
static t_cpu_time long_press_timer;
static int controller_cpu_hz;

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
  // Long pressing for BACK key handler
  if (touch_states[QT60168_TOUCH_SENSOR_BUTTON_0] == TOUCH_RELEASE)
    controller_state &= ~STATE_BACK_PRESSING;
  if (touch_states[QT60168_TOUCH_SENSOR_BUTTON_0] == TOUCH_PRESS)
  {
    if (!(controller_state & STATE_BACK_PRESSING))
    {
      controller_state |= STATE_BACK_PRESSING;
      cpu_set_timeout(cpu_ms_2_cy(CONTROLLER_LONG_PRESS_TIME_MS, controller_cpu_hz), &long_press_timer);
    }
    if (cpu_is_timeout(&long_press_timer))
      controller_state |= STATE_BACK_LONG_PRESS;
  }

  // Back key
  if (touch_states[QT60168_TOUCH_SENSOR_BUTTON_0] == TOUCH_PRESS)
    SET_PRESSED_STATE(BACK);
  else if (touch_states[QT60168_TOUCH_SENSOR_BUTTON_0] == TOUCH_RELEASE)
    SET_RELEASED_STATE(BACK);
  // Function 1 key
  if (touch_states[QT60168_TOUCH_SENSOR_BUTTON_1] == TOUCH_PRESS)
    SET_PRESSED_STATE(FCT1);
  else if (touch_states[QT60168_TOUCH_SENSOR_BUTTON_1] == TOUCH_RELEASE)
    SET_RELEASED_STATE(FCT1);
  // Function 2 key
  if (touch_states[QT60168_TOUCH_SENSOR_BUTTON_2] == TOUCH_PRESS)
    SET_PRESSED_STATE(FCT2);
  else if (touch_states[QT60168_TOUCH_SENSOR_BUTTON_2] == TOUCH_RELEASE)
    SET_RELEASED_STATE(FCT2);
  // Function 3 key
  if (touch_states[QT60168_TOUCH_SENSOR_BUTTON_3] == TOUCH_PRESS)
    SET_PRESSED_STATE(FCT3);
  else if (touch_states[QT60168_TOUCH_SENSOR_BUTTON_3] == TOUCH_RELEASE)
    SET_RELEASED_STATE(FCT3);

  // Wheel right
  if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_0] == TOUCH_RELEASE && touch_states[QT60168_TOUCH_SENSOR_WHEEL_1] == TOUCH_PRESS)
    set_wheel_right();
  if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_1] == TOUCH_RELEASE && touch_states[QT60168_TOUCH_SENSOR_WHEEL_2] == TOUCH_PRESS)
    set_wheel_right();
  if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_2] == TOUCH_RELEASE && touch_states[QT60168_TOUCH_SENSOR_WHEEL_3] == TOUCH_PRESS)
    set_wheel_right();
  if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_3] == TOUCH_RELEASE && touch_states[QT60168_TOUCH_SENSOR_WHEEL_4] == TOUCH_PRESS)
    set_wheel_right();
  if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_4] == TOUCH_RELEASE && touch_states[QT60168_TOUCH_SENSOR_WHEEL_5] == TOUCH_PRESS)
    set_wheel_right();
  if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_5] == TOUCH_RELEASE && touch_states[QT60168_TOUCH_SENSOR_WHEEL_6] == TOUCH_PRESS)
    set_wheel_right();
  if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_6] == TOUCH_RELEASE && touch_states[QT60168_TOUCH_SENSOR_WHEEL_7] == TOUCH_PRESS)
    set_wheel_right();
  if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_7] == TOUCH_RELEASE && touch_states[QT60168_TOUCH_SENSOR_WHEEL_8] == TOUCH_PRESS)
    set_wheel_right();
  if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_8] == TOUCH_RELEASE && touch_states[QT60168_TOUCH_SENSOR_WHEEL_9] == TOUCH_PRESS)
    set_wheel_right();
  if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_9] == TOUCH_RELEASE && touch_states[QT60168_TOUCH_SENSOR_WHEEL_10] == TOUCH_PRESS)
    set_wheel_right();
  if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_10] == TOUCH_RELEASE && touch_states[QT60168_TOUCH_SENSOR_WHEEL_11] == TOUCH_PRESS)
    set_wheel_right();
  if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_11] == TOUCH_RELEASE && touch_states[QT60168_TOUCH_SENSOR_WHEEL_0] == TOUCH_PRESS)
    set_wheel_right();
  // Wheel left
  if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_11] == TOUCH_PRESS && touch_states[QT60168_TOUCH_SENSOR_WHEEL_0] == TOUCH_RELEASE)
    set_wheel_left();
  if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_10] == TOUCH_PRESS && touch_states[QT60168_TOUCH_SENSOR_WHEEL_11] == TOUCH_RELEASE)
    set_wheel_left();
  if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_9] == TOUCH_PRESS && touch_states[QT60168_TOUCH_SENSOR_WHEEL_10] == TOUCH_RELEASE)
    set_wheel_left();
  if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_8] == TOUCH_PRESS && touch_states[QT60168_TOUCH_SENSOR_WHEEL_9] == TOUCH_RELEASE)
    set_wheel_left();
  if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_7] == TOUCH_PRESS && touch_states[QT60168_TOUCH_SENSOR_WHEEL_8] == TOUCH_RELEASE)
    set_wheel_left();
  if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_6] == TOUCH_PRESS && touch_states[QT60168_TOUCH_SENSOR_WHEEL_7] == TOUCH_RELEASE)
    set_wheel_left();
  if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_5] == TOUCH_PRESS && touch_states[QT60168_TOUCH_SENSOR_WHEEL_6] == TOUCH_RELEASE)
    set_wheel_left();
  if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_4] == TOUCH_PRESS && touch_states[QT60168_TOUCH_SENSOR_WHEEL_5] == TOUCH_RELEASE)
    set_wheel_left();
  if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_3] == TOUCH_PRESS && touch_states[QT60168_TOUCH_SENSOR_WHEEL_4] == TOUCH_RELEASE)
    set_wheel_left();
  if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_2] == TOUCH_PRESS && touch_states[QT60168_TOUCH_SENSOR_WHEEL_3] == TOUCH_RELEASE)
    set_wheel_left();
  if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_1] == TOUCH_PRESS && touch_states[QT60168_TOUCH_SENSOR_WHEEL_2] == TOUCH_RELEASE)
    set_wheel_left();
  if (touch_states[QT60168_TOUCH_SENSOR_WHEEL_0] == TOUCH_PRESS && touch_states[QT60168_TOUCH_SENSOR_WHEEL_1] == TOUCH_RELEASE)
    set_wheel_left();
}

/*! \brief Initializes QT60168 resources: GPIO and SPI
 */
static void qt60168_resources_init(int cpu_hz)
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
  spi_setupChipReg(QT60168_SPI, &spiOptions, cpu_hz);
}

bool controller_key_pressed(void)
{
  if (controller_state == STATE_IDLE)
    return false;
  return true;
}

bool controller_key_released(void)
{
  if (controller_state & STATE_BACK_RELEASED ||
      controller_state & STATE_FCT1_RELEASED ||
      controller_state & STATE_FCT2_RELEASED ||
      controller_state & STATE_FCT3_RELEASED)
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

bool controller_key_back(void)
{
  if (controller_state & STATE_BACK_RELEASED)
  {
    CLEAR_RELEASED_STATE(BACK);
    return true;
  }
  return false;
}

bool controller_key_reset(void)
{
  if (controller_state & STATE_BACK_LONG_PRESS)
  {
    controller_state &= ~STATE_BACK_LONG_PRESS;
    return true;
  }
  return false;
}

bool controller_key_fct1(void)
{
  if (controller_state & STATE_FCT1_RELEASED)
  {
    CLEAR_RELEASED_STATE(FCT1);
    return true;
  }
  return false;
}

bool controller_key_fct2(void)
{
  if (controller_state & STATE_FCT2_RELEASED)
  {
    CLEAR_RELEASED_STATE(FCT2);
    return true;
  }
  return false;
}

bool controller_key_fct3(void)
{
  if (controller_state & STATE_FCT3_RELEASED)
  {
    CLEAR_RELEASED_STATE(FCT3);
    return true;
  }
  return false;
}

bool controller_key_fct1_pressed(void)
{
  if (controller_state & STATE_FCT1_PRESSED)
    return true;
  return false;
}

bool controller_key_fct2_pressed(void)
{
  if (controller_state & STATE_FCT2_PRESSED)
    return true;
  return false;
}

bool controller_key_fct3_pressed(void)
{
  if (controller_state & STATE_FCT3_PRESSED)
    return true;
  return false;
}

bool controller_wheel_right(int wheel_inc)
{
  if (wheel_step_counter >= wheel_inc && controller_state & STATE_WHEEL_RIGHT)
  {
    wheel_step_counter -= wheel_inc;
    return true;
  }
  return false;
}

bool controller_wheel_left(int wheel_inc)
{
  if (wheel_step_counter >= wheel_inc && controller_state & STATE_WHEEL_LEFT)
  {
    wheel_step_counter -= wheel_inc;
    return true;
  }
  return false;
}

void controller_reset(void)
{
  controller_state = STATE_IDLE;
  wheel_step_counter = 0;
}

void controller_init(int cpu_hz, int hsb_hz, int pba_hz, int pbb_hz)
{
  int i;
  // Initialize QT60168 resources: GPIO, SPI and QT60168.
  qt60168_resources_init(cpu_hz);
  // Initialize QT60168 component.
  qt60168_init(cpu_hz);
  // Read the touch states
  for (i=0; i<QT60168_TOUCH_NUMBER_OF_SENSORS; i++ )
    touch_states[i] = TOUCH_IDLE;
  controller_cpu_hz = cpu_hz;
}

void controller_task(void)
{
  int i;

  for (i=0; i<QT60168_TOUCH_NUMBER_OF_SENSORS; i++ )
  {
    if (qt60168_is_key_pressed(i))
      touch_states[i] = TOUCH_PRESS;
    else if (touch_states[i] == TOUCH_PRESS)
      touch_states[i] = TOUCH_RELEASE;
    else
      touch_states[i] = TOUCH_IDLE;
  }
  update_controller_state();
}
