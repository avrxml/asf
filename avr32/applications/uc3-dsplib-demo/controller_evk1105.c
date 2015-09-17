/*****************************************************************************
 *
 * \file
 *
 * \brief Controller
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "controller.h"
#include "board.h"
#include "gpio.h"
#include "cycle_counter.h"
#include "qt1081.h"
#include "intc.h"

#define JOYSTICK_KEY_DEBOUNCE_MS           200

#define IS_JOYSTICK_KEY_UP()               (joystick_status & JOYSTICK_STATUS_UP)
#define IS_JOYSTICK_KEY_DOWN()             (joystick_status & JOYSTICK_STATUS_DOWN)
#define IS_JOYSTICK_KEY_RIGHT()            (joystick_status & JOYSTICK_STATUS_RIGHT)
#define IS_JOYSTICK_KEY_LEFT()             (joystick_status & JOYSTICK_STATUS_LEFT)
#define IS_JOYSTICK_KEY_PRESSED()          (joystick_status & JOYSTICK_STATUS_PRESSED)

#define IS_JOYSTICK_ONLY_KEY_UP()          ((joystick_status == JOYSTICK_STATUS_UP) & JOYSTICK_STATUS_PRESSED_MASK)
#define IS_JOYSTICK_ONLY_KEY_DOWN()        ((joystick_status == JOYSTICK_STATUS_DOWN) & JOYSTICK_STATUS_PRESSED_MASK)
#define IS_JOYSTICK_ONLY_KEY_RIGHT()       ((joystick_status == JOYSTICK_STATUS_RIGHT) & JOYSTICK_STATUS_PRESSED_MASK)
#define IS_JOYSTICK_ONLY_KEY_LEFT()        ((joystick_status == JOYSTICK_STATUS_LEFT) & JOYSTICK_STATUS_PRESSED_MASK)
#define IS_JOYSTICK_ONLY_KEY_PRESSED()     ((joystick_status == JOYSTICK_STATUS_PRESSED) & JOYSTICK_STATUS_PRESSED_MASK)

#define IS_JOYSTICK_RELEASED_KEY_UP()      (joystick_status & JOYSTICK_STATUS_RELEASED_UP)
#define IS_JOYSTICK_RELEASED_KEY_DOWN()    (joystick_status & JOYSTICK_STATUS_RELEASED_DOWN)
#define IS_JOYSTICK_RELEASED_KEY_RIGHT()   (joystick_status & JOYSTICK_STATUS_RELEASED_RIGHT)
#define IS_JOYSTICK_RELEASED_KEY_LEFT()    (joystick_status & JOYSTICK_STATUS_RELEASED_LEFT)
#define IS_JOYSTICK_RELEASED_KEY_PRESSED() (joystick_status & JOYSTICK_STATUS_RELEASED_PRESSED)

#define IS_JOYSTICK_ONLY_RELEASED_KEY_UP()      ((joystick_status & JOYSTICK_STATUS_RELEASED_UP) & JOYSTICK_STATUS_RELEASED_MASK)
#define IS_JOYSTICK_ONLY_RELEASED_KEY_DOWN()    ((joystick_status & JOYSTICK_STATUS_RELEASED_DOWN) & JOYSTICK_STATUS_RELEASED_MASK)
#define IS_JOYSTICK_ONLY_RELEASED_KEY_RIGHT()   ((joystick_status & JOYSTICK_STATUS_RELEASED_RIGHT) & JOYSTICK_STATUS_RELEASED_MASK)
#define IS_JOYSTICK_ONLY_RELEASED_KEY_LEFT()    ((joystick_status & JOYSTICK_STATUS_RELEASED_LEFT) & JOYSTICK_STATUS_RELEASED_MASK)
#define IS_JOYSTICK_ONLY_RELEASED_KEY_PRESSED() ((joystick_status & JOYSTICK_STATUS_RELEASED_PRESSED) & JOYSTICK_STATUS_RELEASED_MASK)

#define CLEAR_JOYSTICK_KEY_ALL()           (joystick_status = JOYSTICK_STATUS_IDLE)

#define CLEAR_JOYSTICK_KEY_UP()            (joystick_status &= ~JOYSTICK_STATUS_UP)
#define CLEAR_JOYSTICK_KEY_DOWN()          (joystick_status &= ~JOYSTICK_STATUS_DOWN)
#define CLEAR_JOYSTICK_KEY_RIGHT()         (joystick_status &= ~JOYSTICK_STATUS_RIGHT)
#define CLEAR_JOYSTICK_KEY_LEFT()          (joystick_status &= ~JOYSTICK_STATUS_LEFT)
#define CLEAR_JOYSTICK_KEY_PRESSED()       (joystick_status &= ~JOYSTICK_STATUS_PRESSED)

#define CLEAR_JOYSTICK_RELEASED_KEY_UP()            (joystick_status &= ~JOYSTICK_STATUS_RELEASED_UP)
#define CLEAR_JOYSTICK_RELEASED_KEY_DOWN()          (joystick_status &= ~JOYSTICK_STATUS_RELEASED_DOWN)
#define CLEAR_JOYSTICK_RELEASED_KEY_RIGHT()         (joystick_status &= ~JOYSTICK_STATUS_RELEASED_RIGHT)
#define CLEAR_JOYSTICK_RELEASED_KEY_LEFT()          (joystick_status &= ~JOYSTICK_STATUS_RELEASED_LEFT)
#define CLEAR_JOYSTICK_RELEASED_KEY_PRESSED()       (joystick_status &= ~JOYSTICK_STATUS_RELEASED_PRESSED)


static bool is_joystick_right(void);
static bool is_joystick_left(void);
static bool is_joystick_released_up(void);
static bool is_joystick_released_down(void);
static bool is_joystick_released_right(void);
static bool is_joystick_released_left(void);
static bool is_joystick_released_pressed(void);

static t_cpu_time joystick_key_sensibility_timer;
static U32 static_fcpu_hz;
static bool no_store = false;
static enum joystick_status_t
{
  JOYSTICK_STATUS_IDLE              = 0x00000000,
  JOYSTICK_STATUS_UP                = 0x00000001,
  JOYSTICK_STATUS_DOWN              = 0x00000002,
  JOYSTICK_STATUS_RIGHT             = 0x00000004,
  JOYSTICK_STATUS_LEFT              = 0x00000008,
  JOYSTICK_STATUS_PRESSED           = 0x00000010,
  JOYSTICK_STATUS_PRESSED_MASK      = 0x0000FFFF,
  JOYSTICK_STATUS_RELEASED_UP       = 0x00010000,
  JOYSTICK_STATUS_RELEASED_DOWN     = 0x00020000,
  JOYSTICK_STATUS_RELEASED_RIGHT    = 0x00040000,
  JOYSTICK_STATUS_RELEASED_LEFT     = 0x00080000,
  JOYSTICK_STATUS_RELEASED_PRESSED  = 0x00100000,
  JOYSTICK_STATUS_RELEASED_MASK     = 0xFFFF0000
} joystick_status = JOYSTICK_STATUS_IDLE;

static inline void manage_button_isr(int pin, enum joystick_status_t status_pressed, enum joystick_status_t status_released)
{
  if (gpio_get_pin_interrupt_flag(pin))
  {
    // Clear current pin status
    joystick_status &= ~(status_pressed | status_released);
    if (gpio_get_pin_value(pin))
      joystick_status |= status_pressed;
    else if (!no_store)
      joystick_status |= status_released;
    else
      no_store = false;
    gpio_clear_pin_interrupt_flag(pin);
  }
}

#if __GNUC__
__attribute__ ((__interrupt__))
#elif __ICCAVR32__
__interrupt
#endif
void touch_button_isr(void)
{
  manage_button_isr(QT1081_TOUCH_SENSOR_UP, JOYSTICK_STATUS_UP, JOYSTICK_STATUS_RELEASED_UP);
  manage_button_isr(QT1081_TOUCH_SENSOR_DOWN, JOYSTICK_STATUS_DOWN, JOYSTICK_STATUS_RELEASED_DOWN);
  manage_button_isr(QT1081_TOUCH_SENSOR_RIGHT, JOYSTICK_STATUS_RIGHT, JOYSTICK_STATUS_RELEASED_RIGHT);
  manage_button_isr(QT1081_TOUCH_SENSOR_LEFT, JOYSTICK_STATUS_LEFT, JOYSTICK_STATUS_RELEASED_LEFT);
  manage_button_isr(QT1081_TOUCH_SENSOR_ENTER, JOYSTICK_STATUS_PRESSED, JOYSTICK_STATUS_RELEASED_PRESSED);

  if (IS_JOYSTICK_KEY_UP())
    gpio_clr_gpio_pin(LED1_GPIO);
  else
    gpio_set_gpio_pin(LED1_GPIO);
  if (IS_JOYSTICK_KEY_PRESSED())
    gpio_clr_gpio_pin(LED2_GPIO);
  else
    gpio_set_gpio_pin(LED2_GPIO);
  if (IS_JOYSTICK_KEY_DOWN())
    gpio_clr_gpio_pin(LED3_GPIO);
  else
    gpio_set_gpio_pin(LED3_GPIO);
}

bool controller_key_pressed(void)
{
  if (IS_JOYSTICK_KEY_UP() ||
      IS_JOYSTICK_KEY_DOWN() ||
      IS_JOYSTICK_KEY_RIGHT() ||
      IS_JOYSTICK_KEY_LEFT() ||
      IS_JOYSTICK_KEY_PRESSED())
    return true;
  return false;
}

bool controller_key_released(void)
{
  if (IS_JOYSTICK_RELEASED_KEY_UP() ||
      IS_JOYSTICK_RELEASED_KEY_DOWN() ||
      IS_JOYSTICK_RELEASED_KEY_RIGHT() ||
      IS_JOYSTICK_RELEASED_KEY_LEFT() ||
      IS_JOYSTICK_RELEASED_KEY_PRESSED())
    return true;
  return false;
}

bool controller_wheel_pressed(void)
{
  return controller_key_pressed();
}

bool controller_key_back(void)
{
  static bool start = false;
  static t_cpu_time tempo;

  if (!IS_JOYSTICK_KEY_PRESSED())
  {
    gpio_set_gpio_pin(LED0_GPIO);
    start = false;
    return false;
  }

  if (!start)
  {
    cpu_set_timeout(cpu_ms_2_cy(1000, static_fcpu_hz), &tempo);
    start = true;
  }

  if (cpu_is_timeout(&tempo))
  {
    gpio_clr_gpio_pin(LED0_GPIO);
    no_store = true;
    start = false;
    return true;
  }

  gpio_set_gpio_pin(LED0_GPIO);
  return false;
}

bool controller_key_reset(void)
{
  return false;
}

bool controller_key_fct1(void)
{
  return is_joystick_released_up();
}

bool controller_key_fct2(void)
{
  return is_joystick_released_pressed();
}

bool controller_key_fct3(void)
{
  return is_joystick_released_down();
}

bool controller_key_fct1_pressed(void)
{
  return false;
}

bool controller_key_fct2_pressed(void)
{
  return false;
}

bool controller_key_fct3_pressed(void)
{
  return IS_JOYSTICK_KEY_DOWN();
}

bool controller_wheel_right(int wheel_inc)
{
  return is_joystick_right();
}

bool controller_wheel_left(int wheel_inc)
{
  return is_joystick_left();
}

void controller_reset(void)
{
  // CLEAR_JOYSTICK_KEY_ALL();
}

void controller_init(int cpu_hz, int hsb_hz, int pba_hz, int pbb_hz)
{
  Disable_global_interrupt();
  INTC_register_interrupt(&touch_button_isr, AVR32_GPIO_IRQ_6, 0);//AVR32_INTC_INT0);
  // Other buttons on PB[24..26] -> GPIO_IRQ_7 (PB23 - PB31)
  INTC_register_interrupt(&touch_button_isr, AVR32_GPIO_IRQ_7, 0);
  gpio_enable_pin_interrupt(QT1081_TOUCH_SENSOR_UP, GPIO_PIN_CHANGE);
  gpio_enable_pin_interrupt(QT1081_TOUCH_SENSOR_DOWN, GPIO_PIN_CHANGE);
  gpio_enable_pin_interrupt(QT1081_TOUCH_SENSOR_RIGHT, GPIO_PIN_CHANGE);
  gpio_enable_pin_interrupt(QT1081_TOUCH_SENSOR_LEFT, GPIO_PIN_CHANGE);
  gpio_enable_pin_interrupt(QT1081_TOUCH_SENSOR_ENTER, GPIO_PIN_CHANGE);
  // Enable interrupts globally.
  Enable_global_interrupt();
  static_fcpu_hz = cpu_hz;
  cpu_set_timeout(cpu_ms_2_cy(JOYSTICK_KEY_DEBOUNCE_MS, static_fcpu_hz), &joystick_key_sensibility_timer);
}

void controller_task(void)
{
}

static bool is_joystick_released_up(void)
{
  if (IS_JOYSTICK_ONLY_RELEASED_KEY_UP() && cpu_is_timeout(&joystick_key_sensibility_timer))
  {
    CLEAR_JOYSTICK_RELEASED_KEY_UP();
    cpu_set_timeout(cpu_ms_2_cy(JOYSTICK_KEY_DEBOUNCE_MS, static_fcpu_hz), &joystick_key_sensibility_timer);
    return true;
  }
  return false;
}

static bool is_joystick_released_down(void)
{
  if (IS_JOYSTICK_ONLY_RELEASED_KEY_DOWN() && cpu_is_timeout(&joystick_key_sensibility_timer))
  {
    CLEAR_JOYSTICK_RELEASED_KEY_DOWN();
    cpu_set_timeout(cpu_ms_2_cy(JOYSTICK_KEY_DEBOUNCE_MS, static_fcpu_hz), &joystick_key_sensibility_timer);
    return true;
  }
  return false;
}

static bool is_joystick_released_right(void)
{
  if (IS_JOYSTICK_ONLY_RELEASED_KEY_RIGHT() && cpu_is_timeout(&joystick_key_sensibility_timer))
  {
    CLEAR_JOYSTICK_RELEASED_KEY_RIGHT();
    cpu_set_timeout(cpu_ms_2_cy(JOYSTICK_KEY_DEBOUNCE_MS, static_fcpu_hz), &joystick_key_sensibility_timer);
    return true;
  }
  return false;
}

static bool is_joystick_released_left(void)
{
  if (IS_JOYSTICK_ONLY_RELEASED_KEY_LEFT() && cpu_is_timeout(&joystick_key_sensibility_timer))
  {
    CLEAR_JOYSTICK_RELEASED_KEY_LEFT();
    cpu_set_timeout(cpu_ms_2_cy(JOYSTICK_KEY_DEBOUNCE_MS, static_fcpu_hz), &joystick_key_sensibility_timer);
    return true;
  }
  return false;
}

static bool is_joystick_right(void)
{
  if (IS_JOYSTICK_KEY_RIGHT() && cpu_is_timeout(&joystick_key_sensibility_timer))
  {
    cpu_set_timeout(cpu_ms_2_cy(JOYSTICK_KEY_DEBOUNCE_MS, static_fcpu_hz), &joystick_key_sensibility_timer);
    return true;
  }
  return false;
}

static bool is_joystick_left(void)
{
  if (IS_JOYSTICK_KEY_LEFT() && cpu_is_timeout(&joystick_key_sensibility_timer))
  {
    cpu_set_timeout(cpu_ms_2_cy(JOYSTICK_KEY_DEBOUNCE_MS, static_fcpu_hz), &joystick_key_sensibility_timer);
    return true;
  }
  return false;
}

static bool is_joystick_released_pressed(void)
{
  if (IS_JOYSTICK_ONLY_RELEASED_KEY_PRESSED() && cpu_is_timeout(&joystick_key_sensibility_timer))
  {
    CLEAR_JOYSTICK_RELEASED_KEY_PRESSED();
    cpu_set_timeout(cpu_ms_2_cy(JOYSTICK_KEY_DEBOUNCE_MS, static_fcpu_hz), &joystick_key_sensibility_timer);
    return true;
  }
  return false;
}
