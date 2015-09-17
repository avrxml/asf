/*****************************************************************************
 *
 * \file
 *
 * \brief A streaming IMA/DVI ADPCM encoder
 *
 * This file show a way to use the IMA/DVI ADPCM encoding functions with a full example.
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
 ******************************************************************************/

/*! \mainpage
 *
 * \section intro Introduction
 * This is a full example of a streaming IMA/DVI ADPCM encoder.\n
 * This example demonstrates the 32-bit AVR capabilities in real time computation.
 * It uses the ADC feature of the microcontroller to sample a sound which data are automatically written into a buffer
 * at a frequency given by the timer counter. During this process, the last data are readjusted and encoded in IMA/DVI ADPCM
 * and finally transmitted through the USART by another PDCA channel.\n
 * Through this page, you can access the \ref group_dsplib documentation.
 *
 * \section algo Conception
 * The code is based on two PDCA's interrupts.\n
 * The first PDCA channel is configured to work with the ADC. The sampling frequency is
 * fixed by the Timer Counter peripheral and its value is 8000 Hz. Each
 * time 252 data are received and stored into a circular buffer, as it has been configured for this
 * application, the sequencer will encode these data with the IMA/DVI ADPCM algorithm
 * in order to transfer them through the USART. Each data is first scaled to fit the 16-bit range
 * and received an offset that defines the 0 absolute of the recorded sound. Then, once encoded,
 * they are stored into another circular buffer to be sent thanks to the PDCA through the USART.\n
 * Here is a diagram summarizing the whole process:
 * \image html conception.jpg "Conception diagram"
 * All the process is sequenced by the two PDCA interrupts which make the system working as a real time system.
 *
 * \section interface Hardware Interface
 * Here is the hardware interface used to acquire and adapt the sound to be recorded.
 * \image html hardware.jpg "Hardware interface to acquire and adjust the sound"
 *
 * \section protocol Protocol
 *
 * Here is a description of the protocol used to communicate between the PC and the microcontroller.\n
 * This protocol is really simple and is only a single direction transfer (from the microcontroller to the PC).
 * The data are transmitted through 256-byte packets. The first 4 bytes of a packet correspond to the header of the packet.
 * They give information about the initial "predicted value" and "step index" used to encode the data
 * (those two values are used in the IMA/DVI ADPCM algorithm to encode and decode data). The last 252 bytes correspond
 * to the IMA/DVI ADPCM encoded data.\n
 *
 * \section deviceinfo Device Information
 * All 32-bit AVR devices with an USART, PWM and PDCA modules can be used.
 *
 * \section configinfo Configuration Information
 * To make this example works, you will need a PC and a serial cable
 * to connect the board to the PC.\n
 * This example has been tested with the following configuration:
 * - EVK1100 or EVK1101 evaluation kit;
 * - CPU clock: 12 MHz;
 * - USART1 connected to a PC serial port via a standard RS232 DB9 cable.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <avr32/io.h>

#include "compiler.h"
#include "board.h"

#include "dsp.h"

#include "pm.h"
#include "gpio.h"
#include "usart.h"
#include "intc.h"
#include "pwm.h"
#include "flashc.h"
#include "preprocessor.h"
#include "adc.h"
#include "tc.h"

#include "pdca.h"

/******** Configuration ************/
//! \brief PDCA channel for the ADC
#define PDCA_ADC_CHANNEL  0
//! \brief PDCA channel for the USART
#define PDCA_USART_CHANNEL  1
//! \brief ADC channel
#if BOARD == EVK1100
// ADC routed on the potentiometer
#  define ADC_CHANNEL  1
#elif BOARD == EVK1101
// ADC routed on the microphone on the EVK1102 extension board
#  define ADC_CHANNEL  7
#endif
//! \brief Timer Counter channel
#define TC_CHANNEL   0
/********************************/

#define STATE_SEND_USART              1
#define STATE_GET_SAMPLES             2

//! The size of the buffer
#define BUFFER_SIZE  (252*4)
//! Number of buffers
#define NB_BUFFERS                    2

//! The clock frequency
#define CP_PBA_SPEED                  12000000

#if BOARD == EVK1100
#  define EXAMPLE_USART               (&AVR32_USART1)
#  define EXAMPLE_USART_RX_PIN        AVR32_USART1_RXD_0_0_PIN
#  define EXAMPLE_USART_RX_FUNCTION   AVR32_USART1_RXD_0_0_FUNCTION
#  define EXAMPLE_USART_TX_PIN        AVR32_USART1_TXD_0_0_PIN
#  define EXAMPLE_USART_TX_FUNCTION   AVR32_USART1_TXD_0_0_FUNCTION
#  define AVR32_PDCA_PID_USART_TX     AVR32_PDCA_PID_USART1_TX
#elif BOARD == EVK1101
#  define EXAMPLE_USART               (&AVR32_USART1)
#  define EXAMPLE_USART_RX_PIN        AVR32_USART1_RXD_0_0_PIN
#  define EXAMPLE_USART_RX_FUNCTION   AVR32_USART1_RXD_0_0_FUNCTION
#  define EXAMPLE_USART_TX_PIN        AVR32_USART1_TXD_0_0_PIN
#  define EXAMPLE_USART_TX_FUNCTION   AVR32_USART1_TXD_0_0_FUNCTION
#  define AVR32_PDCA_PID_USART_TX     AVR32_PDCA_PID_USART1_TX
#endif



//! Transfer buffer
A_ALIGNED char tbuffer[BUFFER_SIZE/4 + 4];
//! The main buffer
A_ALIGNED char buffer[NB_BUFFERS][BUFFER_SIZE];
volatile enum
{
  BUFFER_STATE_FREE     = 0,
  BUFFER_STATE_BUSY     = 1,
  BUFFER_STATE_FILLED   = 2
} buffer_state[NB_BUFFERS];
char *pbuffer_w;
char *pbuffer_r;
int i_w;
int i_r;


//! \brief state machine variable
volatile int state;


/*! \brief Low-level initialization routine called during startup, before the
 *         \ref main function.
 */
#if __GNUC__ && __AVR32__
int _init_startup(void)
#elif __ICCAVR32__
int __low_level_init(void)
#endif
{
#if __GNUC__ && __AVR32__
  // Import the Exception Vector Base Address.
  extern void _evba;

  // Load the Exception Vector Base Address in the corresponding system
  // register.
  Set_system_register(AVR32_EVBA, (int)&_evba);
#endif

  // Enable exceptions globally.
  Enable_global_exception();
  return 1;
}

#if __GNUC__
__attribute__((__interrupt__))
#elif __ICCAVR32__
__interrupt
#endif
static void pdca_adc_interrupt_handler(void)
{
  pdca_disable_interrupt_transfer_complete(PDCA_ADC_CHANNEL);

  buffer_state[i_w] = BUFFER_STATE_FILLED;

  // Change pointer buffer
  i_w = ((i_w+1)%NB_BUFFERS);
  pbuffer_w = buffer[i_w];

  state |= STATE_GET_SAMPLES;
  state |= STATE_SEND_USART;
}


#if __GNUC__
__attribute__((__interrupt__))
#elif __ICCAVR32__
__interrupt
#endif
static void pdca_usart_interrupt_handler(void)
{
  pdca_disable_interrupt_transfer_complete(PDCA_USART_CHANNEL);

  buffer_state[i_r] = BUFFER_STATE_FREE;

  // Change pointer buffer
  i_r = ((i_r+1)%NB_BUFFERS);
  pbuffer_r = buffer[i_r];
}



//! To set the clock frequency to 48MHz
void sys_cpu_48MHz(void)
{
  // Switch to external Oscillator 0
  pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);

  // Set PLL0 @ 96 MHz from Osc0 => Fpll= Fosc*(7+1)/1
  pm_pll_setup(&AVR32_PM, 0,   // pll
                          7,   // mul
                          1,   // div
                          0,   // osc
                          16); // lockcount

  // PLL clock is lower than 160MHz: need to set pllopt.
  pm_pll_set_option(&AVR32_PM
    , 0 // pll
    , 0 // pll_freq
    , 1 // pll_div2 => 48MHz
    , 0 // pll_wbwdisable
  );

  AVR32_PM.pll[0] |= AVR32_PM_PLL0_PLLBPL_MASK;

  // Enable PLL1
  pm_pll_enable(&AVR32_PM, 0);

  // Wait for PLL0 locked
  pm_wait_for_pll0_locked(&AVR32_PM);
  pm_cksel(&AVR32_PM, TRUE, 1, TRUE, 0, TRUE, 0);
  pm_switch_to_clock(&AVR32_PM, AVR32_PM_MCCTRL_MCSEL_PLL0);
}

//! this function initializes the USART module at 115200 bauds
void init_usart()
{
  static const gpio_map_t USART_GPIO_MAP =
  {
    {EXAMPLE_USART_RX_PIN, EXAMPLE_USART_RX_FUNCTION},
    {EXAMPLE_USART_TX_PIN, EXAMPLE_USART_TX_FUNCTION}
  };

  // Configuration structure for the USART module
  static const usart_options_t USART_OPTIONS =
  {
    // Baudrate at 115200 bauds
    .baudrate     = 115200,
    // 8 bits per characters
    .charlength   = 8,
    // No parity
    .paritytype   = USART_NO_PARITY,
    // One stop bit
    .stopbits     = USART_1_STOPBIT,
    // Mode normal
    .channelmode  = USART_NORMAL_CHMODE
  };

  // Assign GPIO to USART.
  gpio_enable_module(USART_GPIO_MAP,
                     sizeof(USART_GPIO_MAP) / sizeof(USART_GPIO_MAP[0]));

  // Initialize USART in RS232 mode.
  usart_init_rs232(EXAMPLE_USART, &USART_OPTIONS, CP_PBA_SPEED);
}

void init_pdca_adc()
{
  pdca_channel_options_t pdca_options ={ /* pdca channel options */
    .addr = (void *) buffer[0],                            /* memory address.
                                                     We take here the address of the string ascii_anim.
                                                     This string is located in the file ascii_anim.h */
    .pid = AVR32_PDCA_PID_ADC_RX,                 /* select peripheral ID - data are transmit on USART0 TX line */
    .size = sizeof(buffer[0])/2,                    /* transfer counter: here the size of the string */
    .r_addr = NULL,                               /* next memory address after 1st transfer complete */
    .r_size = 0,                                  /* next transfer counter not used here */
    .transfer_size = PDCA_TRANSFER_SIZE_HALF_WORD /* select size of the transfer: 8,16,32 bits */
  };

  pdca_init_channel(PDCA_ADC_CHANNEL, &pdca_options);

  INTC_register_interrupt(&pdca_adc_interrupt_handler, ATPASTE2(AVR32_PDCA_IRQ_, PDCA_ADC_CHANNEL), AVR32_INTC_INT3);
}

void init_pdca_usart()
{
  pdca_channel_options_t pdca_options ={ /* pdca channel options */
    .addr = buffer[0],
    .pid = AVR32_PDCA_PID_USART_TX,           /* select peripheral ID - data are transmit on USART1 TX line */
    .size = 0,                                /* transfer counter: here the size of the string */
    .r_addr = NULL,                           /* next memory address after 1st transfer complete */
    .r_size = 0,                              /* next transfer counter not used here */
    .transfer_size = PDCA_TRANSFER_SIZE_BYTE  /* select size of the transfer: 8,16,32 bits */
  };

  pdca_init_channel(PDCA_USART_CHANNEL, &pdca_options);

  INTC_register_interrupt(&pdca_usart_interrupt_handler, ATPASTE2(AVR32_PDCA_IRQ_, PDCA_USART_CHANNEL), AVR32_INTC_INT0);
}

void init_adc()
{
  static const gpio_map_t ADC_GPIO_MAP =
  {
    {ATPASTE3(AVR32_ADC_AD_, ADC_CHANNEL, _PIN),  ATPASTE3(AVR32_ADC_AD_, ADC_CHANNEL, _FUNCTION)}
  };

  // enable GPIO pins for ADC module
  gpio_enable_module(ADC_GPIO_MAP,
                     sizeof(ADC_GPIO_MAP) / sizeof(ADC_GPIO_MAP[0]));

  // configure ADC
  adc_configure(&AVR32_ADC);

  AVR32_ADC.MR.trgen = 1;
  AVR32_ADC.MR.trgsel = TC_CHANNEL;

  // enable ADC
  adc_enable(&AVR32_ADC, ADC_CHANNEL);
}

void init_tc(int rate)
{
  volatile avr32_tc_t *tc = &AVR32_TC;

  // Options for waveform genration.
  static const tc_waveform_opt_t WAVEFORM_OPT =
  {
    .channel  = TC_CHANNEL,                        // Channel selection.

    .bswtrg   = TC_EVT_EFFECT_NOOP,                // Software trigger effect on TIOB.
    .beevt    = TC_EVT_EFFECT_NOOP,                // External event effect on TIOB.
    .bcpc     = TC_EVT_EFFECT_NOOP,                // RC compare effect on TIOB.
    .bcpb     = TC_EVT_EFFECT_NOOP,                // RB compare effect on TIOB.

    .aswtrg   = TC_EVT_EFFECT_NOOP,                // Software trigger effect on TIOA.
    .aeevt    = TC_EVT_EFFECT_NOOP,                // External event effect on TIOA.
    .acpc     = TC_EVT_EFFECT_TOGGLE,              // RC compare effect on TIOA: toggle.
    .acpa     = TC_EVT_EFFECT_NOOP,                // RA compare effect on TIOA: none.

    .wavsel   = TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER,// Waveform selection: Up mode without automatic trigger on RC compare.
    .enetrg   = FALSE,                             // External event trigger enable.
    .eevt     = 0,                                 // External event selection.
    .eevtedg  = TC_SEL_NO_EDGE,                    // External event edge selection.
    .cpcdis   = FALSE,                             // Counter disable when RC compare.
    .cpcstop  = FALSE,                             // Counter clock stopped with RC compare.

    .burst    = FALSE,                             // Burst signal selection.
    .clki     = FALSE,                             // Clock inversion.
    .tcclks   = TC_CLOCK_SOURCE_TC2                // Internal source clock 2 - connected to PBA/4
  };

  // Initialize the timer/counter.
  tc_init_waveform(tc, &WAVEFORM_OPT);         // Initialize the timer/counter waveform.

  // Set the compare triggers.
  // Remember TC counter is 16-bits, so counting second is not possible.
  // We configure it to count ms.
  tc_write_rc(tc, TC_CHANNEL, (CP_PBA_SPEED/4)/rate);  // Set RC value.

  // Start the timer/counter.
  tc_start(tc, TC_CHANNEL);
}

//! \brief The main function
int main(int argc, char *argv[])
{
  volatile int i;
  int moy, first;
  short step_index, predicted_value;

  i_w = 0;
  pbuffer_w = buffer[i_w];
  i_r = 0;
  pbuffer_r = buffer[i_r];
  state = 0;

  // Reset buffer states
  for(i=0; i<NB_BUFFERS; i++)
    buffer_state[i] = BUFFER_STATE_FREE;

  step_index = 0;
  predicted_value = 0;

  // Set the clock frequency to 48MHz
  flashc_set_wait_state(1);
  sys_cpu_48MHz();

  // Disable all interrupts.
  Disable_global_interrupt();

  // Initialize interrupt vectors.
  INTC_init_interrupts();

  // Initialize the USART
  init_usart();

  // Initialize the Timer Counter
  init_tc(8000);

  // Initialize the PDCA for the ADC
  init_pdca_adc();

  // Initialize the PDCA for the USART
  init_pdca_usart();

  // Initialize the ADC
  init_adc();
  adc_start(&AVR32_ADC);

  Enable_global_interrupt();

  //  Get samples
  state |= STATE_GET_SAMPLES;
  moy = 0;
  first = 1;
  while(1)
  {
    if ((state & STATE_GET_SAMPLES) && (buffer_state[i_w] == BUFFER_STATE_FREE))
    {
      // Set busy state
      buffer_state[i_w] = BUFFER_STATE_BUSY;

      // Add the address of memory buffer
      // BUFFER_SIZE/2 because the PDCA transfer is using half words data unit.
      pdca_reload_channel(PDCA_ADC_CHANNEL, pbuffer_w, BUFFER_SIZE/2);
      // Enable the transfer
      pdca_enable(PDCA_ADC_CHANNEL);
      // Enable pdca transfer interrupt when completed
      pdca_enable_interrupt_transfer_complete(PDCA_ADC_CHANNEL);

      // Reset the state
      state &= ~STATE_GET_SAMPLES;
    }

    if ((state & STATE_SEND_USART) && (buffer_state[i_r] == BUFFER_STATE_FILLED))
    {
      buffer_state[i_r] = BUFFER_STATE_FREE;

      tbuffer[0] = predicted_value & 0xFF;
      tbuffer[1] = (predicted_value >> 8) & 0xFF;
      tbuffer[2] = step_index & 0xFF;
      tbuffer[3] = 0x00;

      if (first)
      {
        // To adapt the sound and detect the reference
        moy = 0;
        for(i=0; i<(BUFFER_SIZE >> 1); i++)
          moy += ((unsigned short *) pbuffer_r)[i];

        moy /= (BUFFER_SIZE >> 1);

        first = 0;
      }

      for(i=0; i<(BUFFER_SIZE >> 1); i++)
      {
        ((short *) pbuffer_r)[i] -= (short) moy;
        // To convert the value from a 10-bit value to a 16-bit value
        ((short *) pbuffer_r)[i] <<= 6;
      }

      // Encode in IMA/DVI ADPCM
      dsp_adpcm_ima_encode(&tbuffer[4], (void *) pbuffer_r, BUFFER_SIZE >> 1, &step_index, &predicted_value);

      // Add the address of memory buffer
      pdca_reload_channel(PDCA_USART_CHANNEL, tbuffer, sizeof(tbuffer));
      // Enable the transfer
      pdca_enable(PDCA_USART_CHANNEL);
      // Enable pdca transfer interrupt when completed
      pdca_enable_interrupt_transfer_complete(PDCA_USART_CHANNEL);

      // Reset the state
      state &= ~STATE_SEND_USART;
    }
  }
}
