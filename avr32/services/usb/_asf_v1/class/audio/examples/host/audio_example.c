/**
 * \file
 *
 * \mainpage
 * \section title Main file of the USB AUDIO example.
 *
 * \section file File(s)
 * - \ref audio_example.c
 * - \ref host_audio_task.c
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#include <stddef.h>
#include <stdio.h>

#include <avr32/io.h>

#ifndef FREERTOS_USED
#if (defined __GNUC__)
#  include "nlao_cpu.h"
#  include "nlao_usart.h"
#endif
#else
#  include "usart.h"
#endif
#include "compiler.h"
#include "board.h"
#include "print_funcs.h"
#include "intc.h"
#include "gpio.h"
#include "pm.h"
#include "flashc.h"
#if UC3A3
#include "twim.h"
#else
#include "twi.h"
#endif
#include "usart.h"

#ifdef FREERTOS_USED
#  include "FreeRTOS.h"
#  include "task.h"
#endif
#include "conf_usb.h"
#include "usb_task.h"
#if USB_DEVICE_FEATURE == true
#  include "device_audio_task.h"
#  include "device_hid_task.h"
#endif
#if USB_HOST_FEATURE == true
#  include "host_audio_task.h"
#endif
#include "audio_example.h"
#include "controller.h"

#if(DEFAULT_DACS == AUDIO_MIXER_DAC_ABDAC)
#  include "tpa6130.h"
#  include "conf_tpa6130.h"
#elif(DEFAULT_DACS == AUDIO_MIXER_DAC_AIC23B)
#  include "tlv320aic23b.h"
#endif

#include "et024006dhu.h"
#include "avr32_logo.h"

#if (defined USB_RESYNC_METHOD) && (USB_RESYNC_METHOD == USB_RESYNC_METHOD_EXT_CLOCK_SYNTHESIZER)
#include "cs2200.h"
#endif


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

#if (defined USB_RESYNC_METHOD) && (USB_RESYNC_METHOD == USB_RESYNC_METHOD_EXT_CLOCK_SYNTHESIZER)
//! CPU frequency
uint32_t g_fcpu_hz=0;

//! HSB frequency
uint32_t g_fhsb_hz=0;

//! PBA frequency
uint32_t g_fpba_hz=0;

//! PBB frequency
uint32_t g_fpbb_hz=0;
#endif

#if (defined __GNUC__) && (defined __AVR32__)

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

/*! \brief Initializes the HSB bus matrix.
 */
static void init_hmatrix(void)
{
  // For the internal-flash HMATRIX slave, use last master as default.
  union
  {
    unsigned long                 scfg;
    avr32_hmatrix_scfg_t          SCFG;
  } u_avr32_hmatrix_scfg = {AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_FLASH]};
  u_avr32_hmatrix_scfg.SCFG.defmstr_type = AVR32_HMATRIX_DEFMSTR_TYPE_LAST_DEFAULT;
  AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_FLASH] = u_avr32_hmatrix_scfg.scfg;
}


/*! \brief Sets up generic clock for the audio codec.
 */
static void init_codec_gclk(void)
{
#if(DEFAULT_DACS == AUDIO_MIXER_DAC_ABDAC)
  // Configure the ABDAC generic clock
  // We do not activate it here since this is done by activating the
  // ABDAC in the driver.
  pm_gc_setup(&AVR32_PM, AVR32_PM_GCLK_ABDAC,
    AVR32_GC_USES_OSC, AVR32_GC_USES_OSC1, 0, 0);

#elif(DEFAULT_DACS == AUDIO_MIXER_DAC_AIC23B)
  int gc = 0;
  gpio_enable_module_pin(TLV320_PM_GCLK_PIN, TLV320_PM_GCLK_FUNCTION);

# if(AIC23B_MCLK_HZ == 11289600)
  pm_gc_setup(&AVR32_PM, gc, AVR32_GC_USES_OSC, AVR32_GC_USES_OSC1, 0, 0);
# elif(AIC23B_MCLK_HZ == 12000000)
  pm_gc_setup(&AVR32_PM, gc, AVR32_GC_USES_OSC, AVR32_GC_USES_OSC0, 0, 0);
# else
#   error Wrong Master clock configuration
# endif

  pm_gc_enable(&AVR32_PM, gc);
#endif
}

/*! \brief Initializes the USB clock.
 */
static void init_usb_clock(void)
{
    pm_configure_usb_clock();
}

/*! \brief Initializes the MCU system clocks.
 */
static void init_sys_clocks(void)
{
    // Switch to OSC0 to speed up the booting
    pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);

    // Start oscillator1
    pm_enable_osc1_crystal(&AVR32_PM, FOSC1);
    //
    pm_enable_clk1(&AVR32_PM, OSC1_STARTUP);

    // Set PLL0 (fed from OSC1 = 11.2896 MHz) to 112.896 MHz
    // We use OSC1 since we need a correct master clock for the SSC module to generate
    // the correct sample rate
    pm_pll_setup(&AVR32_PM, 0,  // pll.
      SYS_CLOCK_PLL_MUL-1,   // mul.
      1,   // div.
      1,   // osc.
      16); // lockcount.

    // Set PLL operating range and divider (fpll = fvco/2)
    // -> PLL0 output = 62.0928 MHz
    pm_pll_set_option(&AVR32_PM, 0, // pll.
      1,  // pll_freq.
      1,  // pll_div2.
      0); // pll_wbwdisable.

    // start PLL0 and wait for the lock
    pm_pll_enable(&AVR32_PM, 0);
    pm_wait_for_pll0_locked(&AVR32_PM);
    // Set all peripheral clocks torun at master clock rate
    pm_cksel(&AVR32_PM,
      0,   // pbadiv.
      0,   // pbasel.
      0,   // pbbdiv.
      0,   // pbbsel.
      0,   // hsbdiv.
      0);  // hsbsel.

    // Set one waitstate for the flash
    flashc_set_wait_state(1);

    // Switch to PLL0 as the master clock
    pm_switch_to_clock(&AVR32_PM, AVR32_PM_MCCTRL_MCSEL_PLL0);

#if (defined USB_RESYNC_METHOD) && (USB_RESYNC_METHOD == USB_RESYNC_METHOD_EXT_CLOCK_SYNTHESIZER)
    // Holds frequencies parameters
    g_fcpu_hz = g_fhsb_hz = g_fpba_hz = g_fpbb_hz = FMCK_HZ(11289600);
#endif

#if (defined __GNUC__) && (defined __AVR32__)
  // Give the used PBA clock frequency to Newlib, so it can work properly.
  set_cpu_hz(FPBA_HZ);
#endif
  init_usb_clock();
  init_codec_gclk();
}



#ifndef FREERTOS_USED

/*! \brief Initializes STDIO.
 */
static void init_stdio(void)
{
#if (defined __GNUC__) && (defined __AVR32__)

  static const gpio_map_t STDIO_USART_GPIO_MAP =
  {
    {STDIO_USART_RX_PIN, STDIO_USART_RX_FUNCTION},
    {STDIO_USART_TX_PIN, STDIO_USART_TX_FUNCTION}
  };

  // Initialize the USART used for STDIO.
  set_usart_base((void *)STDIO_USART);
  gpio_enable_module(STDIO_USART_GPIO_MAP,
                     sizeof(STDIO_USART_GPIO_MAP) / sizeof(STDIO_USART_GPIO_MAP[0]));
  usart_init(STDIO_USART_BAUDRATE);

#elif (defined __ICCAVR32__)

  static const gpio_map_t STDIO_USART_GPIO_MAP =
  {
    {STDIO_USART_RX_PIN, STDIO_USART_RX_FUNCTION},
    {STDIO_USART_TX_PIN, STDIO_USART_TX_FUNCTION}
  };

  static const usart_options_t STDIO_USART_OPTIONS =
  {
    .baudrate     = STDIO_USART_BAUDRATE,
    .charlength   = 8,
    .paritytype   = USART_NO_PARITY,
    .stopbits     = USART_1_STOPBIT,
    .channelmode  = USART_NORMAL_CHMODE
};

  // Initialize the USART used for STDIO.
  extern volatile avr32_usart_t *volatile stdio_usart_base;
  stdio_usart_base = STDIO_USART;
  gpio_enable_module(STDIO_USART_GPIO_MAP,
                     sizeof(STDIO_USART_GPIO_MAP) / sizeof(STDIO_USART_GPIO_MAP[0]));
  usart_init_rs232(STDIO_USART, &STDIO_USART_OPTIONS, FPBA_HZ);

#endif
}


  #if (defined __GNUC__)

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
  irq_initialize_vectors();
  cpu_irq_enable();

  init_stdio();

  // Don't-care value for GCC.
  return 1;
}

  #elif (defined __ICCAVR32__)

/*! \brief Low-level initialization routine called during startup, before the
 *         main function.
 */
int __low_level_init(void)
{
  // Enable exceptions.
  Enable_global_exception();

  // Initialize interrupt handling.
  irq_initialize_vectors();
  cpu_irq_enable();

  init_stdio();

  // Request initialization of data segments.
  return 1;
}

  #endif  // Compiler

#endif  // FREERTOS_USED


#if (defined USB_RESYNC_METHOD) && (USB_RESYNC_METHOD == USB_RESYNC_METHOD_EXT_CLOCK_SYNTHESIZER)
/*! \brief Initializes the two-wire interface using the internal RCOSC.
 */
static void init_twi_CS2200(uint32_t fpba_hz)
{
  volatile uint32_t i;

  static const gpio_map_t CS2200_TWI_GPIO_MAP =
  {
    {CS2200_TWI_SCL_PIN, CS2200_TWI_SCL_FUNCTION},
    {CS2200_TWI_SDA_PIN, CS2200_TWI_SDA_FUNCTION}
  };

  static twi_options_t CS2200_TWI_OPTIONS =
  {
    .speed  = CS2200_TWI_MASTER_SPEED,
    .chip   = CS2200_TWI_SLAVE_ADDRESS
  };
  CS2200_TWI_OPTIONS.pba_hz = fpba_hz;

  gpio_enable_module(CS2200_TWI_GPIO_MAP,
                     sizeof(CS2200_TWI_GPIO_MAP) / sizeof(CS2200_TWI_GPIO_MAP[0]));
  twi_master_init(CS2200_TWI, &CS2200_TWI_OPTIONS);
}
#endif

static void init_twi(uint32_t fpba_hz)
{
#if(DEFAULT_DACS == AUDIO_MIXER_DAC_ABDAC)
  const gpio_map_t TPA6130_TWI_GPIO_MAP =
  {
  {TPA6130_TWI_SCL_PIN, TPA6130_TWI_SCL_FUNCTION},
  {TPA6130_TWI_SDA_PIN, TPA6130_TWI_SDA_FUNCTION}
  };

  twi_options_t TPA6130_TWI_OPTIONS =
  {
    .speed = TPA6130_TWI_MASTER_SPEED,
    .chip = TPA6130_TWI_ADDRESS
  };
  TPA6130_TWI_OPTIONS.pba_hz = fpba_hz;

  // Assign I/Os to TWI.
  gpio_enable_module(TPA6130_TWI_GPIO_MAP,
    sizeof(TPA6130_TWI_GPIO_MAP) / sizeof(TPA6130_TWI_GPIO_MAP[0]));

  // Initialize as master.
  twi_master_init(TPA6130_TWI, &TPA6130_TWI_OPTIONS);

#elif(DEFAULT_DACS == AUDIO_MIXER_DAC_AIC23B)
  static const gpio_map_t AIC23B_TWI_GPIO_MAP =
  {
    {AIC23B_TWI_SCL_PIN, AIC23B_TWI_SCL_FUNCTION},
    {AIC23B_TWI_SDA_PIN, AIC23B_TWI_SDA_FUNCTION}
  };

  static twi_options_t AIC23B_TWI_OPTIONS =
  {
    .speed  = AIC23B_TWI_MASTER_SPEED,
    .chip   = AIC23B_TWI_ADDRESS
  };
  AIC23B_TWI_OPTIONS.pba_hz = fpba_hz;

  gpio_enable_module(AIC23B_TWI_GPIO_MAP,
                     sizeof(AIC23B_TWI_GPIO_MAP) / sizeof(AIC23B_TWI_GPIO_MAP[0]));
  twi_master_init(AIC23B_TWI, &AIC23B_TWI_OPTIONS);

#endif
}


/*! \brief Main function. Execution starts here.
 *
 * \retval 42 Fatal error.
 */
int main(void)
{
  init_hmatrix();

  // Configure standard I/O streams as unbuffered.
#if (defined __GNUC__) && (defined __AVR32__)
  setbuf(stdin, NULL);
#endif
  setbuf(stdout, NULL);

#if (defined USB_RESYNC_METHOD) && (USB_RESYNC_METHOD == USB_RESYNC_METHOD_EXT_CLOCK_SYNTHESIZER)
  // Initialize the TWI using the internal RCOSC
  init_twi_CS2200(AVR32_PM_RCOSC_FREQUENCY);

  // Initialize the CS2200 and produce a default 11.2896 MHz frequency
  cs2200_setup(11289600);
#endif

  // Initializes the MCU system clocks
  init_sys_clocks();

  // Initialize the TWI
  init_twi(FPBA_HZ);

  audio_mixer_enable_dacs(DEFAULT_DACS);
  audio_mixer_dacs_start(DEFAULT_DAC_SAMPLE_RATE_HZ,
                         DEFAULT_DAC_NUM_CHANNELS,
                         DEFAULT_DAC_BITS_PER_SAMPLE,
                         DEFAULT_DAC_SWAP_CHANNELS);

   // Initialize the display
   et024006_Init(  FCPU_HZ, FHSB_HZ);

   // Set Backlight
   gpio_set_gpio_pin(ET024006DHU_BL_PIN);

   // Clear the display
   et024006_DrawFilledRect(0, 0, ET024006_WIDTH, ET024006_HEIGHT, WHITE );

   // Display a logo.
   et024006_PutPixmap(avr32_logo, AVR32_LOGO_WIDTH, 0, 0
      ,(ET024006_WIDTH - AVR32_LOGO_WIDTH)/2
      ,(ET024006_HEIGHT - AVR32_LOGO_HEIGHT)/2, AVR32_LOGO_WIDTH, AVR32_LOGO_HEIGHT);

   et024006_PrintString(AUDIO_DEMO_STRING, (const unsigned char *)&FONT8x16, 30, 5, BLACK, -1);
   et024006_PrintString("Please plug the USB.", (const unsigned char *)&FONT8x8, 30, 30, BLACK, -1);

   // Initialize USB task
   usb_task_init();

   // Initialize Controller
   controller_init(FCPU_HZ, FHSB_HZ, FPBB_HZ, FPBA_HZ);

#if USB_DEVICE_FEATURE == true
   // Initialize device audio USB task
   device_audio_task_init();

   // Initialize the HID USB task
   device_hid_task_init();
#endif
#if USB_HOST_FEATURE == true
   // Initialize host audio USB task
   host_audio_task_init();
#endif

#ifdef FREERTOS_USED
   // Start OS scheduler
   vTaskStartScheduler();
   portDBG_TRACE("FreeRTOS returned.");
   return 42;
#else
   // No OS here. Need to call each task in round-robin mode.
   while (true)
   {
     usb_task();
  #if USB_DEVICE_FEATURE == true
     device_audio_task();
     device_hid_task();
  #endif
  #if USB_HOST_FEATURE == true
     host_audio_task();
  #endif
   }
#endif  // FREERTOS_USED
}
