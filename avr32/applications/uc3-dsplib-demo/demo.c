/**
 * \file
 *
 * \mainpage
 * \section title DSP demo.
 *
 * \section file File(s)
 * - \ref demo.c
 * - \ref dsp_process.c
 * - \ref dsp_sprintf.c
 * - \ref gui.c
 * - \ref controller.h
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "dsp_process.h"
#include "gui.h"
#include "controller.h"

#include "board.h"

#if BOARD == EVK1104
# include "twim.h"
#elif BOARD == EVK1105
# include "twi.h"
#endif

#include "pm.h"
#include "tpa6130.h"
#include "gpio.h"
#include "print_funcs.h"
#include "flashc.h"

#include "conf_demo.h"

#if BOARD == EVK1104
# define TEXT_IDLE     "FUNC1 - Frame/s\nFUNC2 - Volume\nWHEEL - Select box"
# define TEXT_SOURCE1  "Source1\nFUNC1 - Amplitude\nFUNC2 - Frequency\nFUNC3 - Zoom"
# define TEXT_SOURCE2  "Source2\nFUNC1 - Amplitude\nFUNC2 - Frequency\nFUNC3 - Zoom"
# define TEXT_INPUT    "Input signal\nFUNC1 - Time/FFT\nFUNC3 - Zoom"
# define TEXT_FILTER   "Filters\nFUNC 1 - 3:\n    Select filter"
# define TEXT_OUTPUT   "Output signal\nFUNC1 - Time/FFT\nFUNC3 - Zoom"
# define TEXT_WHEEL    "(Use the WHEEL)"
# define TEXT_FUNC1    "FUNC1"
# define TEXT_FUNC2    "FUNC2"
# define TEXT_FUNC3    "FUNC3"
#elif BOARD == EVK1105
# define TEXT_IDLE     "UP - Frame/s\nPLAY - Volume\nPREV/NEXT - Select box"
# define TEXT_SOURCE1  "Source1\nUP - Amplitude\nPLAY - Frequency\nDOWN - Zoom"
# define TEXT_SOURCE2  "Source2\nUP - Amplitude\nPLAY - Frequency\nDOWN - Zoom"
# define TEXT_INPUT    "Input signal\nUP - Time/FFT\nDOWN - Zoom"
# define TEXT_FILTER   "Filters\nUP - PLAY - DOWN:\n    Select filter"
# define TEXT_OUTPUT   "Output signal\nUP - Time/FFT\nDOWN - Zoom"
# define TEXT_WHEEL    "(Use the PREV/NEXT)"
# define TEXT_FUNC1    "UP"
# define TEXT_FUNC2    "PLAY"
# define TEXT_FUNC3    "DOWN"
#endif

#define TEXT_FUNC_NOT_IMPLEMENTED "Function not\nimplemented"

static void twi_init(void)
{
  const gpio_map_t TPA6130_TWI_GPIO_MAP =
  {
  {TPA6130_TWI_SCL_PIN, TPA6130_TWI_SCL_FUNCTION},
  {TPA6130_TWI_SDA_PIN, TPA6130_TWI_SDA_FUNCTION}
  };

  const twi_options_t TPA6130_TWI_OPTIONS =
  {
    .pba_hz = FPBA_HZ,
    .speed  = TPA6130_TWI_MASTER_SPEED,
    .chip   = TPA6130_TWI_ADDRESS
  };

  // Assign I/Os to TWI
  gpio_enable_module(TPA6130_TWI_GPIO_MAP,
    sizeof(TPA6130_TWI_GPIO_MAP) / sizeof(TPA6130_TWI_GPIO_MAP[0]));

  // Initialize as master.
  twi_master_init(TPA6130_TWI, &TPA6130_TWI_OPTIONS);

}

/*! \brief Initializes the MCU system clocks.
 */
void init_sys_clocks(void)
{
  // Switch to OSC0 to speed up the booting
  pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);

  // Start oscillator1
  pm_enable_osc1_crystal(&AVR32_PM, FOSC1);
  pm_enable_clk1(&AVR32_PM, OSC1_STARTUP);

  // Set PLL0 (fed from OSC0 = 12 MHz) to 132 MHz
  // We use OSC1 since we need a correct master clock for the SSC module to generate
  // the correct sample rate
  pm_pll_setup(&AVR32_PM, 0,  // pll.
    10,  // mul.
    1,   // div.
    0,   // osc.
    16); // lockcount.

  // Set PLL operating range and divider (fpll = fvco/2)
  // -> PLL0 output = 66 MHz
  pm_pll_set_option(&AVR32_PM, 0, // pll.
    1,  // pll_freq.
    1,  // pll_div2.
    0); // pll_wbwdisable.

  // start PLL0 and wait for the lock
  pm_pll_enable(&AVR32_PM, 0);
  pm_wait_for_pll0_locked(&AVR32_PM);
  // Set all peripheral clocks torun at master clock rate

  // Set one waitstate for the flash
  flashc_set_wait_state(1);

  // Switch to PLL0 as the master clock
  pm_switch_to_clock(&AVR32_PM, AVR32_PM_MCCTRL_MCSEL_PLL0);

  // Use 12MHz from OSC0 and generate 96 MHz
  pm_pll_setup(&AVR32_PM, 1,  // pll.
	  7,   // mul.
	  1,   // div.
	  0,   // osc.
	  16); // lockcount.

  pm_pll_set_option(&AVR32_PM, 1, // pll.
	  1,  // pll_freq: choose the range 80-180MHz.
	  1,  // pll_div2.
	  0); // pll_wbwdisable.

  // start PLL1 and wait forl lock
  pm_pll_enable(&AVR32_PM, 1);

  // Wait for PLL1 locked.
  pm_wait_for_pll1_locked(&AVR32_PM);

}

volatile bool input_fft_view = false;
volatile bool output_fft_view = false;
volatile bool zoom_view = false;
volatile int zoom_view_id;

enum state_master
{
  STATE_IDLE,
  STATE_SOURCE1,
  STATE_SOURCE2,
  STATE_FILTER,
  STATE_INPUT,
  STATE_OUTPUT
};

enum state_function
{
  STATE_FCT_IDLE,
  STATE_FCT_FUNCTION1,
  STATE_FCT_FUNCTION2,
  STATE_FCT_FUNCTION3,
  STATE_FCT_ZOOM
};

static void switch_state(enum state_master state);

static enum state_master state = STATE_IDLE;
static enum state_function state_fct = STATE_FCT_IDLE;
static bool new_state_fct = false;

static bool state_machine_source(int source_id, enum state_function *state)
{
  static dsp16_t volume;
  static unsigned int frequency;
  struct signal_source *source = NULL;

  if (source_id == 1)
    source = &signal1_generator;
  else if (source_id == 2)
    source = &signal2_generator;

  switch (*state)
  {
  case STATE_FCT_IDLE:
    if (source_id == 1)
    {
      if (new_state_fct)
      {
        gui_set_selection(GUI_SOURCE1_ID);
        gui_text_print(GUI_COMMENT_ID, TEXT_SOURCE1);
      }
      else
      {
        if (controller_wheel_right(2))
          switch_state(STATE_SOURCE2);
        return false;
      }
    }
    else if (source_id == 2)
    {
      if (new_state_fct)
      {
        gui_set_selection(GUI_SOURCE2_ID);
        gui_text_print(GUI_COMMENT_ID, TEXT_SOURCE2);
      }
      else
      {
        if (controller_wheel_left(2))
          switch_state(STATE_SOURCE1);
        else if (controller_wheel_right(2))
          switch_state(STATE_INPUT);
        return false;
      }
    }
    break;
  // Amplitude
  case STATE_FCT_FUNCTION1:
    volume = signal_source_get_volume(source);
    if (controller_wheel_right(1) && volume < DSP16_Q(1.))
    {
      if (volume < DSP16_Q(1.) - DSP16_Q(1./16))
        volume += DSP16_Q(1./16);
      else
        volume = DSP16_Q(1.);
      new_state_fct = true;
    }
    else if (controller_wheel_left(1))
    {
      if (volume > DSP16_Q(1./16))
        volume -= DSP16_Q(1./16);
      else
        volume = 0;
      new_state_fct = true;
    }
    if (new_state_fct)
    {
      signal_source_set_volume(source, volume);
      gui_text_printf(GUI_COMMENT_ID, "Source%i - " TEXT_FUNC1 "\nAmplitude %f\n\n\n\n" TEXT_WHEEL, source_id, volume);
    }
    break;
  // Frequency
  case STATE_FCT_FUNCTION2:
    frequency = signal_source_get_freq(source);
    if (controller_wheel_right(1) && frequency < 10000)
    {
      frequency *= 1.1;
      new_state_fct = true;
    }
    else if (controller_wheel_left(1) && frequency > 100)
    {
      frequency *= 0.9;
      new_state_fct = true;
    }
    if (new_state_fct)
    {
      signal_source_set_freq(source, frequency);
      gui_text_printf(GUI_COMMENT_ID, "Source%i - " TEXT_FUNC2 "\nFrequency %iHz\n\n\n\n" TEXT_WHEEL, source_id, frequency);
    }
    break;
  case STATE_FCT_FUNCTION3:
    break;
  // Zoom
  case STATE_FCT_ZOOM:
    if (new_state_fct)
    {
      zoom_view = true;
      if (source_id == 1)
        zoom_view_id = GUI_SOURCE1_ID;
      else if (source_id == 2)
        zoom_view_id = GUI_SOURCE2_ID;
      controller_reset();
    }
    break;
  }
  return true;
}

static bool state_machine_signal(enum state_master id, enum state_function *state)
{
  switch (*state)
  {
  case STATE_FCT_IDLE:
    if (id == STATE_INPUT)
    {
      if (new_state_fct)
      {
        gui_set_selection(GUI_INPUT_ID);
        gui_text_print(GUI_COMMENT_ID, TEXT_INPUT);
      }
      else
      {
        if (controller_wheel_right(2))
          switch_state(STATE_FILTER);
        else if (controller_wheel_left(2))
          switch_state(STATE_SOURCE2);
        return false;
      }
    }
    else if (id == STATE_OUTPUT)
    {
      if (new_state_fct)
      {
        gui_set_selection(GUI_OUTPUT_ID);
        gui_text_print(GUI_COMMENT_ID, TEXT_OUTPUT);
      }
      else
      {
        if (controller_wheel_left(2))
          switch_state(STATE_FILTER);
        return false;
      }
    }
    break;
  // FFT
  case STATE_FCT_FUNCTION1:
    if (new_state_fct)
    {
      if (id == STATE_INPUT)
      {
        input_fft_view = !input_fft_view;
        if (input_fft_view)
          gui_text_print(GUI_COMMENT_ID, "Input - " TEXT_FUNC1 "\nFrequency domain\n\n\n\n(Use " TEXT_FUNC1 ")");
        else
          gui_text_print(GUI_COMMENT_ID, "Input - " TEXT_FUNC1 "\nTemporal domain\n\n\n\n(Use " TEXT_FUNC1 ")");
      }
      else if (id == STATE_OUTPUT)
      {
        output_fft_view = !output_fft_view;
        if (output_fft_view)
          gui_text_print(GUI_COMMENT_ID, "Output - " TEXT_FUNC1 "\nFrequency domain\n\n\n\n(Use " TEXT_FUNC1 ")");
        else
          gui_text_print(GUI_COMMENT_ID, "Output - " TEXT_FUNC1 "\nTemporal domain\n\n\n\n(Use " TEXT_FUNC1 ")");
      }
    }
    break;
  case STATE_FCT_FUNCTION2:
  case STATE_FCT_FUNCTION3:
    if (new_state_fct)
      gui_text_print(GUI_COMMENT_ID, TEXT_FUNC_NOT_IMPLEMENTED);
    break;
  // Zoom
  case STATE_FCT_ZOOM:
    if (new_state_fct)
    {
      zoom_view = true;
      if (id == STATE_INPUT)
        zoom_view_id = GUI_INPUT_ID;
      else if (id == STATE_OUTPUT)
        zoom_view_id = GUI_OUTPUT_ID;
//      controller_reset();
    }
    break;
  }

  if (controller_wheel_right(0) || controller_wheel_left(0))
  {
    *state = STATE_FCT_IDLE;
    controller_reset();
  }

  return true;
}

static bool state_machine_filter(enum state_function *state)
{
  switch (*state)
  {
  case STATE_FCT_IDLE:
    if (new_state_fct)
    {
      gui_set_selection(GUI_FILTER_ID);
      gui_text_print(GUI_COMMENT_ID, TEXT_FILTER);
    }
    else
    {
      if (controller_wheel_right(2))
        switch_state(STATE_OUTPUT);
      else if (controller_wheel_left(2))
        switch_state(STATE_INPUT);
      return false;
    }
    break;
  // FFT
  case STATE_FCT_FUNCTION1:
    if (new_state_fct)
      filter_set_active(0);
    break;
  case STATE_FCT_FUNCTION2:
    if (new_state_fct)
      filter_set_active(1);
    break;
  case STATE_FCT_FUNCTION3:
    if (new_state_fct)
      filter_set_active(2);
    break;
  case STATE_FCT_ZOOM:
    break;
  }

  if (new_state_fct)
    gui_text_print(GUI_FILTER_ID, filter_active_get_description());

  if (controller_wheel_right(0) || controller_wheel_left(0))
  {
    *state = STATE_FCT_IDLE;
    controller_reset();
  }

  return true;
}

static bool state_machine_idle(enum state_function *state)
{
  uint32_t frame_rate_ms;
  S8 volume;
  const S8 volume_inc = (TPA6130_VOL_MAX - TPA6130_VOL_MIN) / 20;

  switch (*state)
  {
  case STATE_FCT_IDLE:
    if (new_state_fct)
    {
      gui_set_selection(GUI_NO_SEL);
      gui_text_print(GUI_COMMENT_ID, TEXT_IDLE);
    }
    else
    {
      if (controller_wheel_right(1) || controller_wheel_left(1))
        switch_state(STATE_SOURCE1);
      return false;
    }
    break;
  // Frame rate
  case STATE_FCT_FUNCTION1:
    frame_rate_ms = gui_get_update_fs();
    if (controller_wheel_left(1) && frame_rate_ms < 1000)
    {
      if (!frame_rate_ms)
        frame_rate_ms = 40;
      else
        frame_rate_ms = 1000 / ((1000 / frame_rate_ms) - 1);
      new_state_fct = true;
    }
    else if (controller_wheel_right(1))
    {
      if (frame_rate_ms <= 40)
        frame_rate_ms = 0;
      else
        frame_rate_ms = 1000 / ((1000 / frame_rate_ms) + 1);
      new_state_fct = true;
    }
    if (new_state_fct)
    {
      gui_change_update_fs(frame_rate_ms);
      if (frame_rate_ms)
      {
        gui_text_printf(GUI_COMMENT_ID, TEXT_FUNC1 "\n%i Frame(s)/s\n\n\n\n" TEXT_WHEEL, (1000 / frame_rate_ms));
      }
      else
        gui_text_print(GUI_COMMENT_ID, TEXT_FUNC1 "\nMAX Frame(s)/s\n\n\n\n" TEXT_WHEEL);
    }
    break;
  // Volume
  case STATE_FCT_FUNCTION2:
    volume = tpa6130_get_volume();
    if (volume < TPA6130_VOL_MIN)
      volume = TPA6130_VOL_MIN;
    if (controller_wheel_right(1))
    {
      if (volume < TPA6130_VOL_MAX - volume_inc)
        volume += volume_inc;
      else
        volume = TPA6130_VOL_MAX;
      new_state_fct = true;
    }
    else if (controller_wheel_left(1))
    {
      if (volume > TPA6130_VOL_MIN + volume_inc)
        volume -= volume_inc;
      else
        volume = TPA6130_VOL_MIN;
      new_state_fct = true;
    }
    if (new_state_fct)
    {
      tpa6130_set_volume(volume);
      gui_text_printf(GUI_COMMENT_ID, TEXT_FUNC2 "\nVolume %i%%\n\n\n\n" TEXT_WHEEL, ((int) volume * 100) / (TPA6130_VOL_MAX - TPA6130_VOL_MIN));
    }
    break;
  /*
extern U8 tpa6130_get_volume(void);
extern void tpa6130_set_volume(U8 volume);
  */
  case STATE_FCT_FUNCTION3:
    if (new_state_fct)
      gui_text_print(GUI_COMMENT_ID, TEXT_FUNC_NOT_IMPLEMENTED);
    else if (controller_wheel_right(1) || controller_wheel_left(1)) {
      switch_state(STATE_SOURCE1);
      return false;
    }
    break;
  case STATE_FCT_ZOOM:
    break;
  }
  return true;
}

static void switch_state(enum state_master new_state)
{
  state = new_state;
  state_fct = STATE_FCT_IDLE;
  new_state_fct = true;
}

static void state_machine_task(void)
{
  bool is_reset;


  // Set function state
  if (controller_key_fct3_pressed())
  {
    state_fct = STATE_FCT_ZOOM;
    new_state_fct = true;
  }
  else if (controller_key_fct1())
  {
    state_fct = STATE_FCT_FUNCTION1;
    new_state_fct = true;
  }
  else if (controller_key_fct2())
  {
    state_fct = STATE_FCT_FUNCTION2;
    new_state_fct = true;
  }
  else if (controller_key_fct3())
   {
    state_fct = STATE_FCT_FUNCTION3;
    new_state_fct = true;
  }

  // Clear Zoom state if on and a key is pressed
  if (zoom_view && !controller_key_fct3_pressed())
  {
    zoom_view = false;
    gui_clear_view();
    gui_text_print(GUI_FILTER_ID, filter_active_get_description());
    new_state_fct = true;
    state_fct = STATE_FCT_IDLE;
    controller_key_reset();
  }

  // Reset or back modes
  is_reset = controller_key_reset();
  if ((controller_key_back() && !zoom_view) || is_reset)
  {
    if (is_reset)
    {
      // SW resetof the application
      Reset_CPU();
      while(1);
    }
    if (state_fct == STATE_FCT_IDLE)
      state = STATE_IDLE;
    state_fct = STATE_FCT_IDLE;
    // Reset the controller (delete saved key states)
    controller_reset();
    new_state_fct = true;
  }

  switch (state)
  {
  case STATE_IDLE:
    if (!state_machine_idle(&state_fct))
      return;
    break;
  case STATE_SOURCE1:
    if (!state_machine_source(1, &state_fct))
      return;
    break;
  case STATE_SOURCE2:
    if (!state_machine_source(2, &state_fct))
      return;
    break;
  case STATE_INPUT:
    if (!state_machine_signal(STATE_INPUT, &state_fct))
      return;
    break;
  case STATE_FILTER:
    if (!state_machine_filter(&state_fct))
      return;
    break;
  case STATE_OUTPUT:
    if (!state_machine_signal(STATE_OUTPUT, &state_fct))
      return;
    break;
  }
  new_state_fct = false;
}

int main(void)
{
  init_sys_clocks();

  init_dbg_rs232(FPBA_HZ);

  print_dbg("AVR UC3 DSP DEMO\r\n");

  irq_initialize_vectors();

  // GUI, Controller and DSP process init
  gui_init(FCPU_HZ, FHSB_HZ, FPBA_HZ, FPBB_HZ);
  gui_text_print(GUI_COMMENT_ID, TEXT_IDLE);
  gui_text_print(GUI_FILTER_ID, filter_active_get_description());
  controller_init(FCPU_HZ, FHSB_HZ, FPBA_HZ, FPBB_HZ);
  twi_init();
  dsp_process_init(FCPU_HZ, FHSB_HZ, FPBA_HZ, FPBB_HZ);

  cpu_irq_enable();

  // Main loop
  while (1)
  {
    gui_task();
    controller_task();
    dsp_process_task();
    state_machine_task();
  }
}
