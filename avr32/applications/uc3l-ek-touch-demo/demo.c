/**
 * \file demo.c
 *
 * \brief  This file contains the demo main functions.
 *
 * Copyright (c) 2010-2015 Atmel Corporation. All rights reserved.
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "board.h"

#include "gpio.h"
#include "scif_uc3l.h"
#include "pwma.h"

#include "delay.h"

#include "touch_api_at32uc3l.h"

/*! \name PWMA settings.
 */
//! @{
//! The PWMA frequency is 470588 so that the TOP value is maximum(i.e 0xff)
#define PWMA_OUTPUT_FREQUENCY  470588
#define PWMA_GCLK_FREQUENCY    120000000
//! Define the channel mask for using Interlinked multi mode method as the channel is limited to 4
#define PWMA_CHANNELS_MASK     ((LED0_PWM<<0)|(LED1_PWM<<8)|(LED2_PWM<<16)|(LED3_PWM<<24))

//! Minimum number of duty cycles for a PWM channel.
#define PWMA_MIN_DUTY_CYCLES  0

//! Maximum number of duty cycles for a PWM channel.
#define PWMA_MAX_DUTY_CYCLES  0xFF

#define PWMA_DUTY_CYCLES_STEP 0xF // 0xF // 0x11 // 0x19

#define PWMA_DUTY_CYCLE_INIT_VAL  0

#define PWMA_DUTY_CYCLE_INCREASE  (+1)
#define PWMA_DUTY_CYCLE_DECREASE  (-1)
//! @}

enum
{
  IDX_LED0,
  IDX_LED1,
  IDX_LED2,
  IDX_LED3
};

enum
{
  DEMO_STATE_IDLE,
  DEMO_STATE_WHEEL_PROCESS,
  DEMO_STATE_BTNS_PROCESS
};

enum
{
  BTNS_STATE_MACHINE_IDLE,
  BTNS_STATE_MACHINE_PLAY,
  BTNS_STATE_MACHINE_UP_DOWN,
  BTNS_STATE_MACHINE_RIGHT_LEFT,
};

enum
{
  DIRECTION_STABLE = 0,
  DIRECTION_CLOCKWISE = PWMA_DUTY_CYCLE_DECREASE,
  DIRECTION_ANTICLOCKWISE = PWMA_DUTY_CYCLE_INCREASE
};

enum
{
  INTENSITY_INCREASE,
  INTENSITY_DECREASE
};

enum
{
  SPEED_INCREASE,
  SPEED_DECREASE,
  SPEED_NO_CHANGE
};

enum
{
  LIGHT_SWITCH_UP,
  LIGHT_SWITCH_DOWN,
  LIGHT_SWITCH_RELEASE
};

#define WHEEL_SENSORS_MASK        (0x1u)
#define BTN_SENSOR_DOWN_MASK      (0x2u)
#define BTN_SENSOR_UP_MASK        (0x4u)
#define BTN_SENSOR_LEFT_MASK      (0x8u)
#define BTN_SENSOR_PLAYPAUSE_MASK (0x10u)
#define BTN_SENSOR_RIGHT_MASK     (0x20u)

uint16_t duty_cycles_per_led[LED_COUNT] =
{
  PWMA_DUTY_CYCLE_INIT_VAL,
  PWMA_DUTY_CYCLE_INIT_VAL,
  PWMA_DUTY_CYCLE_INIT_VAL,
  PWMA_DUTY_CYCLE_INIT_VAL,
};

// Currently selected LED
static int current_led = IDX_LED0;  // LED0

static unsigned int demo_state = DEMO_STATE_IDLE;

static unsigned int btns_state_machine = BTNS_STATE_MACHINE_IDLE;

static int  leds_intensity_effect = INTENSITY_INCREASE;

extern touch_measure_data_t *p_qm_measure_data;
extern volatile uint8_t qm_measurement_done_touch;

void demo_init_pwma(void);
void demo_init_pwma(void)
{
  // Enable the pins driving the LEDs with the PWMA function (functionality E).
  gpio_enable_module_pin(LED0_GPIO, LED0_PWM_FUNCTION);
  gpio_enable_module_pin(LED1_GPIO, LED1_PWM_FUNCTION);
  gpio_enable_module_pin(LED2_GPIO, LED2_PWM_FUNCTION);
  gpio_enable_module_pin(LED3_GPIO, LED3_PWM_FUNCTION);

  //#
  //# Setup and enable the "PWMA and GCLK3 pin" generic clock to 120MHz
  //#
  // Start the RC120M clock
  scif_start_rc120M();
  // Setup the generic clock at 120MHz
  scif_gc_setup(  AVR32_PM_GCLK_PWMA, // The generic clock to setup
                  SCIF_GCCTRL_RC120M, // The input clock source to use for the generic clock
                  false,              // Disable the generic clock divisor
                  0);                 // divfactor = DFLL0freq/gclkfreq
  // Enable the generic clock
  scif_gc_enable(AVR32_PM_GCLK_PWMA);

  //#
  //# Enable all the PWMAs to output to all LEDs. LEDs are default turned on by
  //# setting PWM duty cycles to 0.
  //#
  pwma_config_enable(&AVR32_PWMA,PWMA_OUTPUT_FREQUENCY,PWMA_GCLK_FREQUENCY,0);
  pwma_set_channels_value(&AVR32_PWMA,LED_PWMA_CHANNELS_MASK,PWMA_DUTY_CYCLE_INIT_VAL);
}


// Prologue of the demo: all 4 LEDs blink n(=repeat) times simultaneously with
// PWM fading effect.
void demo_automatic_ledshow_play(int repeat);
void demo_automatic_ledshow_play(int repeat)
{
  int current_duty_cycle;
  int leds_intensity_direction = PWMA_DUTY_CYCLE_INCREASE;
  int i;


  do
  {
    // Get the current duty cycle (at that point, all LEDs have the same duty cycle value).
    current_duty_cycle = duty_cycles_per_led[current_led];
    // Change the duty cycle
    current_duty_cycle += leds_intensity_direction*PWMA_DUTY_CYCLES_STEP;
    // Constrain the duty cycle to be in [PWMA_MIN_DUTY_CYCLES,PWMA_MAX_DUTY_CYCLES]
    current_duty_cycle = max(current_duty_cycle, PWMA_MIN_DUTY_CYCLES);
    current_duty_cycle = min(current_duty_cycle, PWMA_MAX_DUTY_CYCLES);
    // Update the duty cycle of all LEDs.
    for(i=0; i<LED_COUNT;i++)
      duty_cycles_per_led[i] = current_duty_cycle;
    pwma_set_channels_value(&AVR32_PWMA, LED_PWMA_CHANNELS_MASK, current_duty_cycle);
    // Change the direction of the LEDs intensity change.
    if((current_duty_cycle == PWMA_MAX_DUTY_CYCLES) || (current_duty_cycle == PWMA_MIN_DUTY_CYCLES))
    {
      leds_intensity_direction *= -1;
      repeat--;
    }
    // End of this led show
    if(repeat == 0)
      break;
    // Make the changes progressive to a human eye by slowing down the rates of
    // the duty cycle changes.
    delay_ms(15);
  }while(1);
}


// Clear all LEDs and reset current LED to LED0
static void reset_leds(void)
{
  int i;
  // Set all LEDs duty cycles to PWMA_MAX_DUTY_CYCLES => clear all LEDs
  for(i=0; i<LED_COUNT; ++i)
    duty_cycles_per_led[i] = PWMA_MAX_DUTY_CYCLES;
  pwma_set_channels_value(&AVR32_PWMA, LED_PWMA_CHANNELS_MASK, PWMA_MAX_DUTY_CYCLES);
  // Set focus on LED0
  current_led = IDX_LED0;
}


// Turn all LEDs on.
static void set_all_leds(void)
{
  int i;
  // Set all LEDs duty cycles to PWMA_MIN_DUTY_CYCLES => set all LEDs
  for(i=0; i<LED_COUNT; ++i)
    duty_cycles_per_led[i] = PWMA_MIN_DUTY_CYCLES;
  pwma_set_channels_value(&AVR32_PWMA, LED_PWMA_CHANNELS_MASK, PWMA_MIN_DUTY_CYCLES);
}


// Return a direction based on a previous and current position.
static int compute_direction(unsigned char previous_position_angle, unsigned char position_angle)
{
  unsigned char abs_angle_diff;
  signed int    angle_diff;
  int           direction = DIRECTION_STABLE;


  angle_diff = (char)position_angle - (char)previous_position_angle;
  abs_angle_diff = abs(position_angle - previous_position_angle);
  if(angle_diff > 0)
  {
    direction = DIRECTION_CLOCKWISE;
    if(abs_angle_diff>64*2) // SHOULD NOT HAPPEN: the angles should be less than 128 apart.
      direction = DIRECTION_STABLE; // DIRECTION_ANTICLOCKWISE;
  }
  else if(angle_diff < 0)
  {
    direction = DIRECTION_ANTICLOCKWISE;
    if(abs_angle_diff>64*2) // SHOULD NOT HAPPEN: the angles should be less than 128 apart.
      direction = DIRECTION_STABLE; // DIRECTION_CLOCKWISE;
  }
  // else angle_diff == 0 => we'll use the default init value of the direction variable.
  return(direction);
}

// The wheel engages the GPIO PWM to create a 256 level "bar" on the 4 LEDs.
// Rotating clockwise will gradually light up the bottom led in 64 steps, then
// the LED above it will begin to glow, and so on until all 4 LEDs are fully lit.
// Turning another clockwise round will have no effect. Turning anticlockwise one
// round will make it gradually decrease the 256 level counter, thus causing the
// bar to fade downwards until you hit 0.
static void process_wheel(void)
{
  unsigned char         position_angle = p_qm_measure_data->p_rotor_slider_values[0];
  static unsigned char  previous_position_angle = 0;
  static int            previous_leds_intensity_direction = DIRECTION_STABLE;
  int                   leds_intensity_direction;
  static bool           first_call = true;


  // Special case for the first call.
  if(first_call == true)
  { // Just record the current angle; we'll determine the direction upon next call.
    first_call = false;
    previous_position_angle = position_angle;
    return;
  }
  // Compute the direction based on previous angle.
  leds_intensity_direction = compute_direction(previous_position_angle, position_angle);
  // Update for next call.
  previous_position_angle = position_angle;

  if(leds_intensity_direction == DIRECTION_STABLE)
    return;
  else if(leds_intensity_direction == DIRECTION_CLOCKWISE)
  {   // Increase the intensity of the LEDs
    // Since we update also the nearby LEDs of the current LED, when there is a
    // change of direction we must insure consistency.
    if((previous_leds_intensity_direction != DIRECTION_CLOCKWISE)&&(current_led != IDX_LED0))
      current_led--;

    // Update the duty cycle of the current LED
    while(1)
    {   // Loop for safety: we always want to check the current duty cycle of a LED
        // before updating it.
      if(duty_cycles_per_led[current_led] == PWMA_MIN_DUTY_CYCLES) // Max intensity
      { // The current LED has already reached its max intensity
        if(current_led == (LED_COUNT-1)) // All LEDs are at max intensity
          return;
        else
          current_led++;  // Switch focus to the next LED
      }
      else
        break;
    }
    duty_cycles_per_led[current_led] -= PWMA_DUTY_CYCLES_STEP;
    // If the current LED is over half of its max intensity, start to increase
    // the next LED intensity. Do this for all LEDs except the last.
    if(current_led != (LED_COUNT-1)) // The current LED is not the last
    {
      if(duty_cycles_per_led[current_led] <= (PWMA_MAX_DUTY_CYCLES - PWMA_MIN_DUTY_CYCLES)>>1)
      {
        if(duty_cycles_per_led[current_led+1] != PWMA_MIN_DUTY_CYCLES)
          duty_cycles_per_led[current_led+1] -= PWMA_DUTY_CYCLES_STEP;
      }
    }
  }
  else  // ANTI-CLOCKWISE direction
  {   // Decrease the intensity of the LEDs
    // Since we update also the nearby LEDs of the current LED, when there is a
    // change of direction we must insure consistency.
    if((previous_leds_intensity_direction != DIRECTION_ANTICLOCKWISE)&&(current_led != (LED_COUNT-1)))
      current_led++;

    // Update the duty cycle of the current LED
    while(1)
    {   // Loop for safety: we always want to check the current duty cycle of a LED
        // before updating it.
      if(duty_cycles_per_led[current_led] == PWMA_MAX_DUTY_CYCLES) // Min intensity
      { // The current LED has already reached its min intensity
        if(current_led == 0) // All LEDs are at min intensity
          return;
        else
          current_led--;  // Switch focus to the previous LED
      }
      else
        break;
    }
    duty_cycles_per_led[current_led] += PWMA_DUTY_CYCLES_STEP;
    // If the current LED is below half of its max intensity, start to decrease
    // the previous LED intensity. Do this for all LEDs except the first.
    if(current_led != 0) // The current LED is not the first
    {
      if(duty_cycles_per_led[current_led] >= (PWMA_MAX_DUTY_CYCLES - PWMA_MIN_DUTY_CYCLES)>>1)
      {
        if(duty_cycles_per_led[current_led-1] != PWMA_MAX_DUTY_CYCLES)
          duty_cycles_per_led[current_led-1] += PWMA_DUTY_CYCLES_STEP;
      }
    }
  }
  // Update the intensity of all LEDs
  pwma_set_multiple_values(&AVR32_PWMA, PWMA_CHANNELS_MASK, (uint16_t*)duty_cycles_per_led);
}


// - Pushing Play/pause starts an automatic running lights sequence on all 4 LEDs.
// The PWM is used to create fading effect. Use the Up and Down arrows to increase
// or decrease the speed of the automatic running lights sequence.
#define AUTO_RUNNING_LIGHTS_MIN_TIMER     (1)
#define AUTO_RUNNING_LIGHTS_DEFAULT_TIMER 25
#define AUTO_RUNNING_LIGHTS_MAX_TIMER     (1000)
#define AUTO_RUNNING_LIGHTS_SPEED_STEPS   (1)
static void play_auto_running_lights(int speed_changes)
{
  static int  timer = 1;
  static int  timer_init = AUTO_RUNNING_LIGHTS_DEFAULT_TIMER;
  static int  speed_increase_count = 0;
  static int  speed_decrease_count = 0;


  // Change the speed of the sequence.
  if(speed_changes == SPEED_INCREASE)
  {   // shorten the timer
    speed_increase_count++;
    if(speed_increase_count == 15) // Limit the frequency of the change of speed.
    {
      speed_increase_count = 0;
      timer_init -= AUTO_RUNNING_LIGHTS_SPEED_STEPS;
      timer_init = max(timer_init, AUTO_RUNNING_LIGHTS_MIN_TIMER);
    }
  }
  else if(speed_changes == SPEED_DECREASE)
  {   // Make the timer longer
    speed_decrease_count++;
    if(speed_decrease_count == 15) // Limit the frequency of the change of speed.
    {
      speed_decrease_count = 0;
      timer_init += AUTO_RUNNING_LIGHTS_SPEED_STEPS;
      timer_init = min(timer_init, AUTO_RUNNING_LIGHTS_MAX_TIMER);
    }
  }

  // Slow the running lights down
  --timer;
  if(timer)
    return;
  else
    timer = timer_init;

  if(leds_intensity_effect == INTENSITY_INCREASE)
  {   // Increase the intensity of the LEDs
    // Update the duty cycle of the current LED
    if(duty_cycles_per_led[current_led] == PWMA_MIN_DUTY_CYCLES) // Max intensity
    { // The current LED has already reached its max intensity
      if(current_led == (LED_COUNT-1)) // All LEDs are at max intensity
      {
        current_led = IDX_LED0;
        leds_intensity_effect = INTENSITY_DECREASE;
      }
      else
        current_led++;  // Switch focus to the next LED
    }
    if(leds_intensity_effect == INTENSITY_INCREASE)
    {
      duty_cycles_per_led[current_led] -= PWMA_DUTY_CYCLES_STEP;
      // If the current LED is over half of its max intensity, start to increase
      // the next LED intensity. Do this for all LEDs except the last.
      if(current_led != (LED_COUNT-1)) // The current LED is not the last
      {
        if(duty_cycles_per_led[current_led] <= (PWMA_MAX_DUTY_CYCLES - PWMA_MIN_DUTY_CYCLES)>>1)
        {
          duty_cycles_per_led[current_led+1] -= PWMA_DUTY_CYCLES_STEP;
        }
      }
    }
  }
  if(leds_intensity_effect == INTENSITY_DECREASE)
  {   // Decrease the intensity of the LEDs
    // Update the duty cycle of the current LED
    if(duty_cycles_per_led[current_led] == PWMA_MAX_DUTY_CYCLES) // Min intensity
    { // The current LED has already reached its min intensity
      if(current_led == (LED_COUNT-1)) // All LEDs are at min intensity
      {
        current_led = IDX_LED0;
        leds_intensity_effect = INTENSITY_INCREASE;
      }
      else
        current_led++;  // Switch focus to the next LED
    }
    if(leds_intensity_effect == INTENSITY_DECREASE)
    {
      duty_cycles_per_led[current_led] += PWMA_DUTY_CYCLES_STEP;
      // If the current LED is below half of its max intensity, start to decrease
      // the next LED intensity. Do this for all LEDs except the last.
      if(current_led != (LED_COUNT-1)) // The current LED is not the last
      {
        if(duty_cycles_per_led[current_led] >= (PWMA_MAX_DUTY_CYCLES - PWMA_MIN_DUTY_CYCLES)>>1)
        {
          duty_cycles_per_led[current_led+1] += PWMA_DUTY_CYCLES_STEP;
        }
      }
    }
  }
  // Update the intensity of all LEDs
  pwma_set_multiple_values(&AVR32_PWMA, PWMA_CHANNELS_MASK, (uint16_t*)duty_cycles_per_led);
}


// - The Right arrow engages the GPIO PWM to create a 256 level "bar" on the 4 LEDs.
// Keep the right arrow pushed to gradually light up the bottom LED in 64 steps,
// then the LED above it will begin to glow, and so on until all 4 LEDs are fully
// lit.
// Keep the Left arrow pushed to gradually decrease the 256 level counter, thus
// causing the bar to fade downwards until you hit 0.
static void play_level_bar(int intensity_direction)
{
  static int  timer = 1;
  int         i;


  // Slow the update of the LEDs intensity down
  --timer;
  if(timer)
    return;
  else
    timer = 2;

  if(intensity_direction == INTENSITY_INCREASE)
  {   // Increase the intensity of the LEDs
    // Update the duty cycle of the current LED
    while(1)
    {   // We do a loop for safety because:
        //  - this function may be called without a pre-reset of the LEDs
        //  - we always want to check a LED's current intensity before updating it.
      if(duty_cycles_per_led[current_led] == PWMA_MIN_DUTY_CYCLES) // Max intensity
      { // The current LED has already reached its max intensity
        if(current_led == (LED_COUNT-1))
        {   // Last LED
          // Check if all LEDs are at max intensity.
          for(i=0; i<LED_COUNT; i++)
          {
            if(duty_cycles_per_led[i] != PWMA_MIN_DUTY_CYCLES)
            {
              current_led = i;
              break;
            }
          }
          if(i == LED_COUNT)
            return; // All LEDs are at max intensity
        }
        else
          current_led++;  // Switch focus to the next LED
      }
      else
        break;
    }
    duty_cycles_per_led[current_led] -= PWMA_DUTY_CYCLES_STEP;
    // If the current LED is over half of its max intensity, start to increase
    // the next LED intensity. Do this for all LEDs except the last.
    if(current_led != (LED_COUNT-1)) // The current LED is not the last
    {
      if(duty_cycles_per_led[current_led] <= (PWMA_MAX_DUTY_CYCLES - PWMA_MIN_DUTY_CYCLES)>>1)
      {
        if(duty_cycles_per_led[current_led+1] != PWMA_MIN_DUTY_CYCLES)
          duty_cycles_per_led[current_led+1] -= PWMA_DUTY_CYCLES_STEP;
      }
    }
  }
  else
  {   // Decrease the intensity of the LEDs
    // Identify the upper most LED that has a non-zero intensity.
    for(i = (LED_COUNT-1); i >= 0; i--)
    {
      if(duty_cycles_per_led[i] != PWMA_MAX_DUTY_CYCLES)
      {
        current_led = i;
        break;
      }
    }
    if(i<0)
    {   // All LEDs already are at the zero-intensity point: nothing to do.
      return;
    }
    // Update the duty cycle of the current LED
    duty_cycles_per_led[current_led] += PWMA_DUTY_CYCLES_STEP;
    // If the current LED is below half of its max intensity, start to decrease
    // the previous LED intensity. Do this for all LEDs except the first.
    if(current_led != 0) // The current LED is not the first
    {
      if(duty_cycles_per_led[current_led] >= (PWMA_MAX_DUTY_CYCLES - PWMA_MIN_DUTY_CYCLES)>>1)
      {
        if(duty_cycles_per_led[current_led-1] != PWMA_MAX_DUTY_CYCLES)
          duty_cycles_per_led[current_led-1] += PWMA_DUTY_CYCLES_STEP;
      }
    }
  }
  // Update the intensity of all LEDs
  pwma_set_multiple_values(&AVR32_PWMA, PWMA_CHANNELS_MASK,(uint16_t*)duty_cycles_per_led);
}


// - The Up and Down arrows are used to turn the 4 LEDs fully on (quick push on up),
// off (quick push on down), and gradually fading on and off (keep up or down pushed
// for 0.5 seconds, use 2.5 seconds to go from rail to rail, like a dimming light
// switch).
#define DIM_LIGHT_SUDDEN_ON_LIMIT     (32UL)
#define DIM_LIGHT_SUDDEN_OFF_LIMIT    (32UL)
static void emulate_light_switch(int switch_event)
{
  static unsigned int switch_event_up_count = 0;
  static unsigned int switch_event_down_count = 0;
  static unsigned int switch_event_release_count = 0;
  static int          dim_speed_timer = 1;
  int                 current_duty_cycle;
  int                 i;


  if(switch_event == LIGHT_SWITCH_UP)
  {
    switch_event_release_count = 0;
    // If previous event was LIGHT_SWITCH_DOWN and the press was short: turn all
    // LEDs off
    if((switch_event_down_count != 0) &&
       (switch_event_down_count < DIM_LIGHT_SUDDEN_OFF_LIMIT))
    {
      reset_leds();
    }
    switch_event_down_count = 0;
    switch_event_up_count++;
    if(switch_event_up_count < DIM_LIGHT_SUDDEN_ON_LIMIT)
      return;
    // else start dimming all LEDs up.
  }
  else if(switch_event == LIGHT_SWITCH_DOWN)
  {
    switch_event_release_count = 0;
    // If previous event was LIGHT_SWITCH_UP and the press was short: turn all
    // LEDs on
    if((switch_event_up_count != 0) &&
       (switch_event_up_count < DIM_LIGHT_SUDDEN_ON_LIMIT))
    {
      set_all_leds();
    }
    switch_event_up_count = 0;
    switch_event_down_count++;
    if(switch_event_down_count < DIM_LIGHT_SUDDEN_OFF_LIMIT)
      return;
    // else start dimming all LEDs down.
  }
  else
  {
    switch_event_release_count++;
    // If previous event was LIGHT_SWITCH_DOWN and the press was short: turn all
    // LEDs off
    if((switch_event_down_count != 0) &&
       (switch_event_down_count < DIM_LIGHT_SUDDEN_OFF_LIMIT))
    {
      reset_leds();
    }
    switch_event_down_count = 0;
    // If previous event was LIGHT_SWITCH_UP and the press was short: turn all
    // LEDs on
    if((switch_event_up_count != 0) &&
       (switch_event_up_count < DIM_LIGHT_SUDDEN_ON_LIMIT))
    {
      set_all_leds();
    }
    switch_event_up_count = 0;
    return;
  }

  // Slow the update of the LEDs intensity down
  --dim_speed_timer;
  if(dim_speed_timer)
    return;
  else
    dim_speed_timer = 5;

  // Get the current duty cycle (at that point, all LEDs have the same duty cycle value).
  current_duty_cycle = duty_cycles_per_led[current_led];
  if(switch_event == LIGHT_SWITCH_UP)
  {   // Fade all LEDs on
    // Change the duty cycle
    if(current_duty_cycle == PWMA_MIN_DUTY_CYCLES)
      return; // We've already reached the max intensity
    else
      current_duty_cycle -= PWMA_DUTY_CYCLES_STEP;
  }
  else  // LIGHT_SWITCH_DOWN
  {   // Fade all LEDs off
    // Change the duty cycle
    if(current_duty_cycle == PWMA_MAX_DUTY_CYCLES)
      return; // We've already reached the min intensity
    else
      current_duty_cycle += PWMA_DUTY_CYCLES_STEP;
  }
  // Update the duty cycle of all LEDs.
  for(i=0; i<LED_COUNT;i++)
    duty_cycles_per_led[i] = current_duty_cycle;
  pwma_set_channels_value(&AVR32_PWMA, LED_PWMA_CHANNELS_MASK, current_duty_cycle);
}


// - Pushing Play/pause starts an automatic running lights sequence on all 4 LEDs.
// The PWM is used to create fading effect. Use the Up and Down arrows to increase
// or decrease the speed of the automatic running lights sequence.
//
// - The Right arrow engages the GPIO PWM to create a 256 level "bar" on the 4 LEDs.
// Keep the right arrow pushed to gradually light up the bottom LED in 64 steps,
// then the LED above it will begin to glow, and so on until all 4 LEDs are fully
// lit.
// Keep the Left arrow pushed to gradually decrease the 256 level counter, thus
// causing the bar to fade downwards until you hit 0.
//
// - The Up and Down arrows are used to turn the 4 LEDs fully on (quick push on up),
// off (quick push on down), and gradually fading on and off (keep up or down pushed
// for 0.5 seconds, use 2.5 seconds to go from rail to rail, like a dimming light
// switch).
// While the automatic running lights sequence is active, the Up and Down arrows
// are used to increase or decrease the speed of that sequence.
static void process_buttons(touch_acq_status_t touch_acq_status)
{
  uint8_t               btn_dwn_state = p_qm_measure_data->p_sensor_states[0]&BTN_SENSOR_DOWN_MASK;
  uint8_t               btn_up_state = p_qm_measure_data->p_sensor_states[0]&BTN_SENSOR_UP_MASK;
  uint8_t               btn_left_state = p_qm_measure_data->p_sensor_states[0]&BTN_SENSOR_LEFT_MASK;
  uint8_t               btn_playpause_state = p_qm_measure_data->p_sensor_states[0]&BTN_SENSOR_PLAYPAUSE_MASK;
  uint8_t               btn_right_state = p_qm_measure_data->p_sensor_states[0]&BTN_SENSOR_RIGHT_MASK;
  uint8_t               btns_sensors_state = btn_dwn_state | btn_up_state | \
                                             btn_left_state | btn_playpause_state | \
                                             btn_right_state;


  if(btns_sensors_state)
  {   // Switch to the appropriate buttons state.
    if(btn_playpause_state)
    {   // Pushing Play/pause starts an automatic running lights sequence on all 4 LEDs.
      reset_leds();
      btns_state_machine = BTNS_STATE_MACHINE_PLAY;
      leds_intensity_effect = INTENSITY_INCREASE;
      play_auto_running_lights(SPEED_NO_CHANGE);
    }
    else if(btn_right_state)
    {   // The Right arrow engages the GPIO PWM to create a 256 level "bar" on the 4 LEDs.
      btns_state_machine = BTNS_STATE_MACHINE_RIGHT_LEFT;
      play_level_bar(INTENSITY_INCREASE);
    }
    else if(btn_left_state)
    {   // Keep the Left arrow pushed to gradually decrease the 256 level counter
      btns_state_machine = BTNS_STATE_MACHINE_RIGHT_LEFT;
      play_level_bar(INTENSITY_DECREASE);
    }
    else if(btn_up_state)
    {
      if(btns_state_machine == BTNS_STATE_MACHINE_PLAY)
      {
        // Increase the speed of the automatic running lights sequence.
        play_auto_running_lights(SPEED_INCREASE);
      }
      else
      {   // Up & Down arrow: emulate a Dimming Light Switch
        if(btns_state_machine != BTNS_STATE_MACHINE_UP_DOWN)
        {
          reset_leds();
          btns_state_machine = BTNS_STATE_MACHINE_UP_DOWN;
        }
        emulate_light_switch(LIGHT_SWITCH_UP);
      }
    }
    else if(btn_dwn_state)
    {
      if(btns_state_machine == BTNS_STATE_MACHINE_PLAY)
      {
        // Decrease the speed of the automatic running lights sequence.
        play_auto_running_lights(SPEED_DECREASE);
      }
      else
      {   // Up & Down arrow: emulate a Dimming Light Switch
        if(btns_state_machine != BTNS_STATE_MACHINE_UP_DOWN)
        {
          reset_leds();
          btns_state_machine = BTNS_STATE_MACHINE_UP_DOWN;
        }
        emulate_light_switch(LIGHT_SWITCH_DOWN);
      }
    }
  }
  else  // Continue with the current buttons state.
  {
    if(btns_state_machine == BTNS_STATE_MACHINE_PLAY)
    {
      play_auto_running_lights(SPEED_NO_CHANGE);
    }
    else if(btns_state_machine == BTNS_STATE_MACHINE_UP_DOWN)
    {
      if(qm_measurement_done_touch == true)
        // We are processing new touch measurements data: no buttons are pressed
        // currently => call the functionalities that need to be aware of that.
        emulate_light_switch(LIGHT_SWITCH_RELEASE);
    }
  }
}

/*
 * \brief main entry point of the touch demo: process the QTouch lib events.
 */
void process_qtouchlib_data(void);
void process_qtouchlib_data(void)
{
  touch_acq_status_t    touch_acq_status = (p_qm_measure_data == NULL ? 0 : p_qm_measure_data->acq_status);
  uint8_t               wheel_sensors_states = (p_qm_measure_data == NULL ? 0 : p_qm_measure_data->p_sensor_states[0]&WHEEL_SENSORS_MASK);
  uint8_t               btn_dwn_state = (p_qm_measure_data == NULL ? 0 : p_qm_measure_data->p_sensor_states[0]&BTN_SENSOR_DOWN_MASK);
  uint8_t               btn_up_state = (p_qm_measure_data == NULL ? 0 : p_qm_measure_data->p_sensor_states[0]&BTN_SENSOR_UP_MASK);
  uint8_t               btn_left_state = (p_qm_measure_data == NULL ? 0 : p_qm_measure_data->p_sensor_states[0]&BTN_SENSOR_LEFT_MASK);
  uint8_t               btn_playpause_state = (p_qm_measure_data == NULL ? 0 : p_qm_measure_data->p_sensor_states[0]&BTN_SENSOR_PLAYPAUSE_MASK);
  uint8_t               btn_right_state = (p_qm_measure_data == NULL ? 0 : p_qm_measure_data->p_sensor_states[0]&BTN_SENSOR_RIGHT_MASK);
  uint8_t               btns_sensors_state = btn_dwn_state | btn_up_state | \
                                             btn_left_state | btn_playpause_state | \
                                             btn_right_state;


  while(1)
  {
    if(demo_state == DEMO_STATE_IDLE)
    {   // Current State == "Idle": no touch events
      // If there is a touch event, change the state depending on the
      // detected touch event. We'll process the event in the next loop.
      // If the touch event is from the wheel sensor, go to the state "Wheel
      // Process", else if the touch event is on one of the buttons sensors, go
      // to the state "Buttons Process".
      if((touch_acq_status & TOUCH_STATUS_CHANGE) ||
         (touch_acq_status & TOUCH_ROTOR_SLIDER_POS_CHANGE))
      {
        if(wheel_sensors_states)
          demo_state = DEMO_STATE_WHEEL_PROCESS;
        else if(btns_sensors_state)
          demo_state = DEMO_STATE_BTNS_PROCESS;
        else  // Never reached
          break;
        reset_leds();
        continue;
      }
      else
        break;
    }
    else if(demo_state == DEMO_STATE_BTNS_PROCESS)
    {   // Current State == "Buttons Process": process the buttons sensors touch
        // events.
      if((touch_acq_status & TOUCH_STATUS_CHANGE) ||
         (touch_acq_status & TOUCH_ROTOR_SLIDER_POS_CHANGE))
      {
        if(wheel_sensors_states)
        {
          // The wheel was touched during the Buttons state: reset the LEDs and
          // switch to the Wheel process.
          reset_leds();
          demo_state = DEMO_STATE_WHEEL_PROCESS;
          continue;
        }
        else
        {   // Buttons process
          process_buttons(touch_acq_status);
          break;
        }
      }
      else  // No touch activity
      {   // Execute the current buttons process
        process_buttons(touch_acq_status);
        break;
      }
    }
    else if(demo_state == DEMO_STATE_WHEEL_PROCESS)
    {   // Current State == "Wheel Process": process the wheel sensor touch
        // events.
      if((touch_acq_status & TOUCH_STATUS_CHANGE) ||
         (touch_acq_status & TOUCH_ROTOR_SLIDER_POS_CHANGE))
      {
        if(wheel_sensors_states)
        {   // Wheel process
          process_wheel();
          break;
        }
        else if(btns_sensors_state)
        {
          // A button was touched during the Wheel state: switch to the Buttons process.
          btns_state_machine = BTNS_STATE_MACHINE_IDLE; // Reset to the default state
          demo_state = DEMO_STATE_BTNS_PROCESS;
          continue;
        }
        else
          break;
      }
      else  // No touch activity
        break;
    }
    else // Impossible because there are no other states than those already parsed.
      break;
  }
}
