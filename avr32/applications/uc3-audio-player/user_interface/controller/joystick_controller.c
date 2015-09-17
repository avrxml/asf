/**************************************************************************
 *
 * \file
 *
 * \brief Joystick Controller
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

#include "board.h"
#include "gpio.h"
#include "intc.h"

#include "controller.h"
#include "qt1081.h"
#include "cycle_counter.h"

#include "conf_audio_player.h"

#define JOYSTICK_KEY_DEBOUNCE_MS           200

#define IS_JOYSTICK_KEY_UP()               (joystick_status & JOYSTICK_STATUS_UP)
#define IS_JOYSTICK_KEY_DOWN()             (joystick_status & JOYSTICK_STATUS_DOWN)
#define IS_JOYSTICK_KEY_RIGHT()            (joystick_status & JOYSTICK_STATUS_RIGHT)
#define IS_JOYSTICK_KEY_LEFT()             (joystick_status & JOYSTICK_STATUS_LEFT)
#define IS_JOYSTICK_KEY_PRESSED()          (joystick_status & JOYSTICK_STATUS_PRESSED)
#define IS_JOYSTICK_KEY_SLIDING_LEFT()     (joystick_status & JOYSTICK_STATUS_SLIDING_LEFT)
#define IS_JOYSTICK_KEY_SLIDE_LEFT()       (joystick_status & JOYSTICK_STATUS_SLIDE_LEFT)
#define IS_JOYSTICK_KEY_SLIDING_RIGHT()    (joystick_status & JOYSTICK_STATUS_SLIDING_RIGHT)
#define IS_JOYSTICK_KEY_SLIDE_RIGHT()      (joystick_status & JOYSTICK_STATUS_SLIDE_RIGHT)

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

#define CLEAR_JOYSTICK_KEY_ALL()           (joystick_status = JOYSTICK_STATUS_IDLE)

#define CLEAR_JOYSTICK_KEY_UP()            (joystick_status &= ~JOYSTICK_STATUS_UP)
#define CLEAR_JOYSTICK_KEY_DOWN()          (joystick_status &= ~JOYSTICK_STATUS_DOWN)
#define CLEAR_JOYSTICK_KEY_RIGHT()         (joystick_status &= ~JOYSTICK_STATUS_RIGHT)
#define CLEAR_JOYSTICK_KEY_LEFT()          (joystick_status &= ~JOYSTICK_STATUS_LEFT)
#define CLEAR_JOYSTICK_KEY_PRESSED()       (joystick_status &= ~JOYSTICK_STATUS_PRESSED)
#define CLEAR_JOYSTICK_KEY_SLIDING_LEFT()  (joystick_status &= ~JOYSTICK_STATUS_SLIDING_LEFT)
#define CLEAR_JOYSTICK_KEY_SLIDE_LEFT()    (joystick_status &= ~JOYSTICK_STATUS_SLIDE_LEFT)
#define CLEAR_JOYSTICK_KEY_SLIDING_RIGHT() (joystick_status &= ~JOYSTICK_STATUS_SLIDING_RIGHT)
#define CLEAR_JOYSTICK_KEY_SLIDE_RIGHT()   (joystick_status &= ~JOYSTICK_STATUS_SLIDE_RIGHT)

#define CLEAR_JOYSTICK_RELEASED_KEY_UP()            (joystick_status &= ~JOYSTICK_STATUS_RELEASED_UP)
#define CLEAR_JOYSTICK_RELEASED_KEY_DOWN()          (joystick_status &= ~JOYSTICK_STATUS_RELEASED_DOWN)
#define CLEAR_JOYSTICK_RELEASED_KEY_RIGHT()         (joystick_status &= ~JOYSTICK_STATUS_RELEASED_RIGHT)
#define CLEAR_JOYSTICK_RELEASED_KEY_LEFT()          (joystick_status &= ~JOYSTICK_STATUS_RELEASED_LEFT)
#define CLEAR_JOYSTICK_RELEASED_KEY_PRESSED()       (joystick_status &= ~JOYSTICK_STATUS_RELEASED_PRESSED)

static bool is_joystick_up(void);
static bool is_joystick_down(void);;
static bool is_joystick_released_up(void);
static bool is_joystick_released_down(void);
static bool is_joystick_released_right(void);
static bool is_joystick_released_left(void);
static bool is_joystick_released_pressed(void);

static t_cpu_time joystick_key_sensibility_timer;
static uint32_t static_fcpu_hz;
static bool fast_mode = false;
static enum joystick_status_t
{
  JOYSTICK_STATUS_IDLE              = 0x00000000,
  JOYSTICK_STATUS_UP                = 0x00000001,
  JOYSTICK_STATUS_DOWN              = 0x00000002,
  JOYSTICK_STATUS_RIGHT             = 0x00000004,
  JOYSTICK_STATUS_LEFT              = 0x00000008,
  JOYSTICK_STATUS_PRESSED           = 0x00000010,
  JOYSTICK_STATUS_SLIDING_LEFT      = 0x00000020,
  JOYSTICK_STATUS_SLIDE_LEFT        = 0x00000040,
  JOYSTICK_STATUS_SLIDING_RIGHT     = 0x00000080,
  JOYSTICK_STATUS_SLIDE_RIGHT       = 0x00000100,
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
    else
      joystick_status |= status_released;
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

  // Handle left slide
  if (IS_JOYSTICK_RELEASED_KEY_PRESSED() && IS_JOYSTICK_KEY_LEFT())
  {
    CLEAR_JOYSTICK_RELEASED_KEY_PRESSED();
    CLEAR_JOYSTICK_KEY_LEFT();
    joystick_status |= JOYSTICK_STATUS_SLIDING_LEFT;
  }
  if (IS_JOYSTICK_KEY_SLIDING_LEFT() && IS_JOYSTICK_RELEASED_KEY_LEFT())
  {
    CLEAR_JOYSTICK_KEY_SLIDING_LEFT();
    CLEAR_JOYSTICK_RELEASED_KEY_LEFT();
    joystick_status |= JOYSTICK_STATUS_SLIDE_LEFT;
  }
  // Handle right slide
  if (IS_JOYSTICK_RELEASED_KEY_PRESSED() && IS_JOYSTICK_KEY_RIGHT())
  {
    CLEAR_JOYSTICK_RELEASED_KEY_PRESSED();
    CLEAR_JOYSTICK_KEY_RIGHT();
    joystick_status |= JOYSTICK_STATUS_SLIDING_RIGHT;
  }
  if (IS_JOYSTICK_KEY_SLIDING_RIGHT() && IS_JOYSTICK_RELEASED_KEY_RIGHT())
  {
    CLEAR_JOYSTICK_KEY_SLIDING_RIGHT();
    CLEAR_JOYSTICK_RELEASED_KEY_RIGHT();
    joystick_status |= JOYSTICK_STATUS_SLIDE_RIGHT;
  }

  if (fast_mode && cpu_is_timeout(&joystick_key_sensibility_timer))
  {
    CLEAR_JOYSTICK_RELEASED_KEY_RIGHT();
    CLEAR_JOYSTICK_RELEASED_KEY_LEFT();
  }

  if (!IS_JOYSTICK_KEY_LEFT() && !IS_JOYSTICK_KEY_RIGHT())
    fast_mode = false;

  gpio_tgl_gpio_pin(LED0_GPIO);
}

void controller_init(uint32_t fcpu_hz, uint32_t fhsb_hz, uint32_t fpbb_hz, uint32_t fpba_hz)
{
  Disable_global_interrupt();
  INTC_register_interrupt(&touch_button_isr, AVR32_GPIO_IRQ_6, AVR32_INTC_INT0);
  // Other buttons on PB[24..26] -> GPIO_IRQ_7 (PB24 - PB31)
  INTC_register_interrupt(&touch_button_isr, AVR32_GPIO_IRQ_7, AVR32_INTC_INT0);
  gpio_enable_pin_interrupt(QT1081_TOUCH_SENSOR_UP, GPIO_PIN_CHANGE);
  gpio_enable_pin_interrupt(QT1081_TOUCH_SENSOR_DOWN, GPIO_PIN_CHANGE);
  gpio_enable_pin_interrupt(QT1081_TOUCH_SENSOR_RIGHT, GPIO_PIN_CHANGE);
  gpio_enable_pin_interrupt(QT1081_TOUCH_SENSOR_LEFT, GPIO_PIN_CHANGE);
  gpio_enable_pin_interrupt(QT1081_TOUCH_SENSOR_ENTER, GPIO_PIN_CHANGE);
  // Enable interrupts globally.
  Enable_global_interrupt();
  static_fcpu_hz = fcpu_hz;
  cpu_set_timeout(cpu_ms_2_cy(JOYSTICK_KEY_DEBOUNCE_MS, static_fcpu_hz), &joystick_key_sensibility_timer);
}

void controller_shutdown(void)
{
}

void controller_clear(void)
{
  joystick_status = JOYSTICK_STATUS_IDLE;
}

bool controller_navigation_cursor_previous(void)
{
  return is_joystick_up();
}

bool controller_navigation_cursor_next(void)
{
  return is_joystick_down();
}

bool controller_navigation_go_to_parent_directory(void)
{
  bool status;
  if (IS_JOYSTICK_KEY_PRESSED())
    return false;
  status = is_joystick_released_left();
  CLEAR_JOYSTICK_RELEASED_KEY_LEFT();
  return status;
}

bool controller_navigation_change_directory(void)
{
  bool status;
  if (IS_JOYSTICK_KEY_PRESSED() || IS_JOYSTICK_KEY_SLIDING_RIGHT())
    return false;
  status = is_joystick_released_right();
  CLEAR_JOYSTICK_RELEASED_KEY_RIGHT();
  return status;
}

bool controller_navigation_play(void)
{
  bool status;
  status = is_joystick_released_pressed();
  CLEAR_JOYSTICK_RELEASED_KEY_PRESSED();
  return status;
}

bool controller_switch_to_navigation_view(enum gui_update_view view)
{
  bool status = false;
  switch (view)
  {
  case GUI_UPDATE_VIEW_PLAYBACK:
    if (IS_JOYSTICK_KEY_SLIDE_LEFT())
    {
      status = true;
      CLEAR_JOYSTICK_KEY_ALL();
    }
    break;
  default:
    status = false;
  }
  return status;
}

bool controller_switch_to_playback_view(enum gui_update_view view)
{
  bool status = false;
  switch (view)
  {
  case GUI_UPDATE_VIEW_NAVIGATION:
    if (IS_JOYSTICK_KEY_SLIDE_RIGHT())
    {
      status = true;
      CLEAR_JOYSTICK_KEY_ALL();
    }
    break;
  case GUI_UPDATE_VIEW_CONFIG:
    if (IS_JOYSTICK_KEY_SLIDE_LEFT())
    {
      status = true;
      CLEAR_JOYSTICK_KEY_ALL();
    }
    break;
  default:
    break;
  }
  return status;
}

bool controller_switch_to_config_view(enum gui_update_view view)
{
  bool status = false;
  switch (view)
  {
  case GUI_UPDATE_VIEW_PLAYBACK:
    if (IS_JOYSTICK_KEY_SLIDE_RIGHT())
    {
      status = true;
      CLEAR_JOYSTICK_KEY_ALL();
    }
    break;
  default:
    break;
  }
  return status;
}

bool controller_playback_toggle_play_pause(void)
{
  bool status = false;
  if (IS_JOYSTICK_RELEASED_KEY_PRESSED())
    status = true;
  CLEAR_JOYSTICK_RELEASED_KEY_PRESSED();
  return status;
}

bool controller_playback_increase_volume(void)
{
  return is_joystick_up();
}

bool controller_playback_decrease_volume(void)
{
  return is_joystick_down();
}

bool controller_playback_previous_track(void)
{
  bool status = false;
  if (IS_JOYSTICK_KEY_PRESSED() || (fast_mode && cpu_is_timeout(&joystick_key_sensibility_timer)))
    return false;
  if (IS_JOYSTICK_RELEASED_KEY_LEFT())
    status = true;
  CLEAR_JOYSTICK_RELEASED_KEY_LEFT();
  return status;
}

bool controller_playback_next_track(void)
{
  bool status = false;
  if (IS_JOYSTICK_KEY_PRESSED() || (fast_mode && cpu_is_timeout(&joystick_key_sensibility_timer)))
    return false;
  if (IS_JOYSTICK_RELEASED_KEY_RIGHT())
    status = true;
  CLEAR_JOYSTICK_RELEASED_KEY_RIGHT();
  return status;
}

bool controller_playback_ffw(bool new_track)
{
  if (IS_JOYSTICK_KEY_RIGHT())
  {
    if (new_track)
      cpu_set_timeout(cpu_ms_2_cy(2000, static_fcpu_hz), &joystick_key_sensibility_timer);
    if (!fast_mode)
    {
      cpu_set_timeout(cpu_ms_2_cy(1000, static_fcpu_hz), &joystick_key_sensibility_timer);
      fast_mode = true;
    }
    else if (cpu_is_timeout(&joystick_key_sensibility_timer))
      return true;
    return false;
  }
  return false;
}

bool controller_playback_frw(bool new_track)
{
  if (IS_JOYSTICK_KEY_LEFT())
  {
    if (new_track)
      cpu_set_timeout(cpu_ms_2_cy(2000, static_fcpu_hz), &joystick_key_sensibility_timer);
    if (!fast_mode)
    {
      cpu_set_timeout(cpu_ms_2_cy(1000, static_fcpu_hz), &joystick_key_sensibility_timer);
      fast_mode = true;
    }
    else if (cpu_is_timeout(&joystick_key_sensibility_timer))
      return true;
    return false;
  }
  return false;
}

bool controller_config_change_mode(void)
{
  bool status;
  status = is_joystick_released_pressed();
  CLEAR_JOYSTICK_RELEASED_KEY_PRESSED();
  return status;
}

bool controller_config_next_option(void)
{
  bool status;
  if (IS_JOYSTICK_KEY_DOWN())
    return false;
  status = is_joystick_released_down();
  CLEAR_JOYSTICK_RELEASED_KEY_DOWN();
  return status;
}

bool controller_config_previous_option(void)
{
  bool status;
  if (IS_JOYSTICK_KEY_UP())
    return false;
  status = is_joystick_released_up();
  CLEAR_JOYSTICK_RELEASED_KEY_UP();
  return status;
}

static bool is_joystick_up(void)
{
  if (IS_JOYSTICK_KEY_UP() && cpu_is_timeout(&joystick_key_sensibility_timer))
  {
    cpu_set_timeout(cpu_ms_2_cy(JOYSTICK_KEY_DEBOUNCE_MS, static_fcpu_hz), &joystick_key_sensibility_timer);
    return true;
  }
  return false;
}

static bool is_joystick_released_up(void)
{
  if (IS_JOYSTICK_RELEASED_KEY_UP() && cpu_is_timeout(&joystick_key_sensibility_timer))
  {
    cpu_set_timeout(cpu_ms_2_cy(JOYSTICK_KEY_DEBOUNCE_MS, static_fcpu_hz), &joystick_key_sensibility_timer);
    return true;
  }
  return false;
}

static bool is_joystick_down(void)
{
  if (IS_JOYSTICK_KEY_DOWN() && cpu_is_timeout(&joystick_key_sensibility_timer))
  {
    cpu_set_timeout(cpu_ms_2_cy(JOYSTICK_KEY_DEBOUNCE_MS, static_fcpu_hz), &joystick_key_sensibility_timer);
    return true;
  }
  return false;
}

static bool is_joystick_released_down(void)
{
  if (IS_JOYSTICK_RELEASED_KEY_DOWN() && cpu_is_timeout(&joystick_key_sensibility_timer))
  {
    cpu_set_timeout(cpu_ms_2_cy(JOYSTICK_KEY_DEBOUNCE_MS, static_fcpu_hz), &joystick_key_sensibility_timer);
    return true;
  }
  return false;
}

static bool is_joystick_released_right(void)
{
  if (IS_JOYSTICK_RELEASED_KEY_RIGHT() && cpu_is_timeout(&joystick_key_sensibility_timer))
  {
    cpu_set_timeout(cpu_ms_2_cy(JOYSTICK_KEY_DEBOUNCE_MS, static_fcpu_hz), &joystick_key_sensibility_timer);
    return true;
  }
  return false;
}

static bool is_joystick_released_left(void)
{
  if (IS_JOYSTICK_RELEASED_KEY_LEFT() && cpu_is_timeout(&joystick_key_sensibility_timer))
  {
    cpu_set_timeout(cpu_ms_2_cy(JOYSTICK_KEY_DEBOUNCE_MS, static_fcpu_hz), &joystick_key_sensibility_timer);
    return true;
  }
  return false;
}

static bool is_joystick_released_pressed(void)
{
  if (IS_JOYSTICK_RELEASED_KEY_PRESSED() && cpu_is_timeout(&joystick_key_sensibility_timer))
  {
    cpu_set_timeout(cpu_ms_2_cy(JOYSTICK_KEY_DEBOUNCE_MS, static_fcpu_hz), &joystick_key_sensibility_timer);
    return true;
  }
  return false;
}
