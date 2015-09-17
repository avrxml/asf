/*****************************************************************************
 *
 * \file
 *
 * \brief A streaming IMA/DVI ADPCM player
 *
 * This file show a way to use the IMA/DVI ADPCM decoding functions with a full example.
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
 * This is a full example of a streaming IMA/DVI ADPCM decoder.\n
 * It uses the USART to receive data blocks (compressed in IMA/DVI ADPCM format) and
 * two mixed PWM to play through a 16 bits precision sample sound.\n
 * Through this page, you can access the \ref group_dsplib documentation.
 *
 * \section algo Conception
 * The code is based on two interrupts, the PWM interrupt and the PDCA interrupt.\n
 * At the beginning, the program will send a 'S' character on the USART line to tell
 * the host that it is ready to start the data acquisition. The host will send first initialization
 * parameters like the size of a block and the bit rate of the sound. Once the application
 * acknowledge those data, the data block acquisition can start.\n
 * At the beginning of the process, the application fills all its buffers before playing
 * any data. Then, it can start playing a data block on the PWMs. Each time a data block is
 * read, it will check if the next one is available, if not it will wait until it is completely filled.
 * Equally, each time a data block is filled, it will check if the next data block as already been
 * played or not. If not it will wait, else it will start the next data block acquisition process.\n
 *
 * You will find in section "Protocol" the whole protocol description of the data block transfer.
 *
 * \section interface Hardware Interface
 * Here is the hardware interface used to mixed the two PWM, to make them act as a 16 bits DAC.
 * \image html pwm_to_dac_mixer.jpg "PWM to DAC mixer"
 *
 * \section protocol Protocol
 *
 * Here is a description of the protocol used to communicate between the PC and the microcontroller.\n
 * To initialize the transfer, the target have to send the character 'S' trough the USART,
 * then the host send a block of 8 bytes described as follow:
 * \image html protocol_start.gif "Initialization block"
 * This block will fix the length of the following blocks used to transfer the IMA/DVI ADPCM data
 * (this length is called "block_size") and will also specify the sample rate of the sound.\n
 * Then the host wait until a byte of value 1 is sent on the line.
 * Each time it receives this character, it will send to the target a block of (block_size + 4) bytes of IMA/DVI ADPCM data
 * containing the sound samples. The first 4 bytes of this block contain the "Predicted Value" and the "Step Index"
 * values used in the IMA/DVI ADPCM algorithm to compute the following data. The following data are the sound samples,
 * coded in IMA/DVI ADPCM on 4 bits per sample.
 * \image html protocol_block.gif "Block format"
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

#include "pdca.h"

//! The size of the buffer
#define BUFFER_SIZE  2048

//! The clock frequency
#define CP_PBA_SPEED              48000000
#define DAC_PWM_FREQUENCY         (CP_PBA_SPEED >> 8)















//! The main buffer
A_ALIGNED char buffer[BUFFER_SIZE];

//! A pointer on the current writing block
char *pbuffer_w;
//! A pointer on the current reading block
char *pbuffer_r;

//! A flag to tell if we need to get a new block from the USART
volatile int get_new_block = 0;
//! A flag to tell if we need to play a new sample with the DAC
volatile int get_sample = 0;

//! The offset of the current reading buffer
int cur_i_read_buffer = 0;
//! The offset of the current writing buffer
int cur_i_buffer = 0;
//! The size of a block
int block_size;
//! The number of buffers
int nb_buffers;
//! Step index for the adpcm decoder
A_ALIGNED short step_index;
//! Predicted value for the adpcm decoder
A_ALIGNED short predicted_value;
//! A flag to tell if the buffer is currently filled or not
int fill_activ = 0;

//! In the example we will use the pdca channel 0. There is 17 possible pdca channel in AT32UC3A0512.
int pdca_channel_usart = 0;

int dac_pwm_period, dac_pwm_periods_per_sample;






/*! \name USART Settings
 */
//! @{
#if BOARD == EVK1100
#  define EXAMPLE_USART               (&AVR32_USART1)
#  define EXAMPLE_USART_RX_PIN        AVR32_USART1_RXD_0_0_PIN
#  define EXAMPLE_USART_RX_FUNCTION   AVR32_USART1_RXD_0_0_FUNCTION
#  define EXAMPLE_USART_TX_PIN        AVR32_USART1_TXD_0_0_PIN
#  define EXAMPLE_USART_TX_FUNCTION   AVR32_USART1_TXD_0_0_FUNCTION
#  define DAC_PWM_CHANNEL_LSB         2
#  define DAC_PWM_CHANNEL_LSB_PIN
#  define DAC_PWM_CHANNEL_MSB         3
#  define DAC_PWM_CHANNEL_MSB_PIN
#elif BOARD == EVK1101
#  define EXAMPLE_USART               (&AVR32_USART1)
#  define EXAMPLE_USART_RX_PIN        AVR32_USART1_RXD_0_0_PIN
#  define EXAMPLE_USART_RX_FUNCTION   AVR32_USART1_RXD_0_0_FUNCTION
#  define EXAMPLE_USART_TX_PIN        AVR32_USART1_TXD_0_0_PIN
#  define EXAMPLE_USART_TX_FUNCTION   AVR32_USART1_TXD_0_0_FUNCTION
#  define DAC_PWM_CHANNEL_LSB       6
#  define DAC_PWM_CHANNEL_LSB_PIN   _0
#  define DAC_PWM_CHANNEL_MSB       4
#  define DAC_PWM_CHANNEL_MSB_PIN   _1
#endif
//! @}

/*! \brief This function is used by the debugging module. It permits to print a string through the USART.
 * \param str The input string to print.
 * \pre It as to be used after having initializing the USART module.
 */
void print_fct(char *str)
{
  usart_write_line(EXAMPLE_USART, str);
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
  usart_init_rs232(EXAMPLE_USART, &USART_OPTIONS, FOSC0*2);
}













//! Read the next block
void set_new_dac_buffer(char *_next_buffer)
{
  pbuffer_r = _next_buffer;

  predicted_value = *((short *) &pbuffer_r[4]);
  step_index = *((short *) &pbuffer_r[6]);

  cur_i_read_buffer = 8;
}


/*!
 * \brief The PWM interrupt handler.
 */
#if __GNUC__
__attribute__((__interrupt__))
#elif __ICCAVR32__
__interrupt
#endif
static void dac_pwm_int_handler(void)
{
  AVR32_PWM.isr;

  if (!dac_pwm_period--)
    get_sample = 1;
}


//! Initialize the PWM to make it works as a DAC
void dac_pwm_init()
{
  static const gpio_map_t DAC_PWM_GPIO_MAP =
  {
    {ATPASTE4(AVR32_PWM_, DAC_PWM_CHANNEL_LSB, DAC_PWM_CHANNEL_LSB_PIN, _PIN),
     ATPASTE4(AVR32_PWM_, DAC_PWM_CHANNEL_LSB, DAC_PWM_CHANNEL_LSB_PIN, _FUNCTION)},
    {ATPASTE4(AVR32_PWM_, DAC_PWM_CHANNEL_MSB, DAC_PWM_CHANNEL_MSB_PIN, _PIN),
     ATPASTE4(AVR32_PWM_, DAC_PWM_CHANNEL_MSB, DAC_PWM_CHANNEL_MSB_PIN, _FUNCTION)}
  };

  INTC_register_interrupt(&dac_pwm_int_handler, AVR32_PWM_IRQ, AVR32_INTC_INT3);

  gpio_enable_module(DAC_PWM_GPIO_MAP, sizeof(DAC_PWM_GPIO_MAP) / sizeof(DAC_PWM_GPIO_MAP[0]));

  AVR32_PWM.channel[DAC_PWM_CHANNEL_LSB].cmr = ((AVR32_PWM_CPRE_MCK << AVR32_PWM_CPRE_OFFSET) |
                                                AVR32_PWM_CPOL_MASK) &
                                               ~(AVR32_PWM_CALG_MASK |
                                                 AVR32_PWM_CPD_MASK);
  AVR32_PWM.channel[DAC_PWM_CHANNEL_LSB].cprd = 0x000000FF;
  AVR32_PWM.channel[DAC_PWM_CHANNEL_LSB].cdty = 0x00;

  AVR32_PWM.channel[DAC_PWM_CHANNEL_MSB].cmr = ((AVR32_PWM_CPRE_MCK << AVR32_PWM_CPRE_OFFSET) |
                                                AVR32_PWM_CPOL_MASK) &
                                               ~(AVR32_PWM_CALG_MASK |
                                                 AVR32_PWM_CPD_MASK);
  AVR32_PWM.channel[DAC_PWM_CHANNEL_MSB].cprd = 0x000000FF;
  AVR32_PWM.channel[DAC_PWM_CHANNEL_MSB].cdty = 0x80;

  AVR32_PWM.ena = (1 << DAC_PWM_CHANNEL_LSB) | (1 << DAC_PWM_CHANNEL_MSB);
  AVR32_PWM.ier = (1 << DAC_PWM_CHANNEL_LSB) | (1 << DAC_PWM_CHANNEL_MSB);
}



#if __GNUC__
__attribute__((__interrupt__))
#elif __ICCAVR32__
__interrupt
#endif
static void pdca_interrupt_handler(void)
{
  pdca_disable_interrupt_transfer_complete(pdca_channel_usart);

  *((unsigned int *) pbuffer_w) = 1;
  // Point on the next buffer
  pbuffer_w += block_size;
  if (pbuffer_w + block_size >= buffer + BUFFER_SIZE)
    pbuffer_w = buffer;

  // If the next buffer is invalid, make an acquisition of the new data
  if (!*((unsigned int *) pbuffer_w))
  {
    // If the buffer is invalidate, fill it with new data;
    get_new_block = 1;
    fill_activ = 1;
  }
  else
  {
    fill_activ = 0;
  }
}

void init_pdca()
{
  pdca_channel_options_t pdca_options ={ /* pdca channel options */
    .addr  = buffer,
   /* memory address. We take here the address of the string ascii_anim. This string is located in the file ascii_anim.h */
    .pid = AVR32_PDCA_PID_USART1_RX,  /* select peripheral ID - data are transmit on USART1 TX line */
    .size = 0,  /* transfer counter: here the size of the string */
    .r_addr = NULL,           /* next memory address after 1st transfer complete */
    .r_size = 0,       /* next transfer counter not used here */
    .transfer_size = PDCA_TRANSFER_SIZE_BYTE  /* select size of the transfer: 8,16,32 bits */
  };

  /* start pdca transfer on USART*/
  pdca_init_channel(pdca_channel_usart, &pdca_options);

  INTC_register_interrupt(&pdca_interrupt_handler, AVR32_PDCA_IRQ_0, AVR32_INTC_INT3);
}



//! To set the clock frequency to 48MHz
void sys_cpu_48MHz(void)
{
  // Switch to external Oscillator 0
  pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);

  // Set PLL1 @ 96 MHz from Osc0
  pm_pll_setup(&AVR32_PM, 0,   // pll
                   15,   // mul
                   1,   // div
                   0,   // osc
                   16); // lockcount

  // PLL clock is lower than 160MHz: need to set pllopt.
  pm_pll_set_option(&AVR32_PM
    , 0 // pll
    , 0 // pll_freq
    , 1 // pll_div2
    , 0 // pll_wbwdisable
  );

  // Enable PLL1
  pm_pll_enable(&AVR32_PM, 0);

  // Wait for PLL0 locked
  pm_wait_for_pll0_locked(&AVR32_PM);
  pm_cksel(&AVR32_PM, TRUE, 1, TRUE, 0, TRUE, 0);
  pm_switch_to_clock(&AVR32_PM, AVR32_PM_MCCTRL_MCSEL_PLL0);
}


//! The main function
int main(int argc, char *argv[])
{
  char c;
  int sample_rate;
  int i, even;
  int sound_ampl;
  volatile unsigned int *pbuffer;

  // Set the clock frequency to 48MHz
  sys_cpu_48MHz();
  flashc_set_wait_state(1);

  // Disable all interrupts.
  Disable_global_interrupt();

  // Initialize interrupt vectors.
  INTC_init_interrupts();

  // Initialize the USART
  init_usart();

  // Initialize the PDCA
  init_pdca();

  // To make the buffer writable
  *((unsigned int *) buffer) = 0;
  pbuffer_w = buffer;
  block_size = 12;
  cur_i_buffer = 8;

  *((unsigned int *) (buffer + block_size)) = 1;

  /*! \brief Enable pdca transfer interrupt when completed*/
  pdca_enable_interrupt_transfer_complete(pdca_channel_usart);
  /*! \brief add the address of memory buffer*/
  pdca_reload_channel(pdca_channel_usart, pbuffer_w + 4, 8);
  /* enable the transfer */
  pdca_enable(pdca_channel_usart);
  // Enable all interrupts.
  Enable_global_interrupt();

  // Start transfer to read the data block size
  usart_putchar(EXAMPLE_USART, 'S');

  // Wait until 2 bytes are received
  // Those 2 bytes corresponds to the length of the data block
  pbuffer = (volatile unsigned int *) buffer;
  while(!*pbuffer);

  // + 4 is for the length of the predicted and step index elements
  // read the length of one data block
  block_size = *((int *) &buffer[4]) + 8;
  // read the sample rate of the sound
  sample_rate = *((int *) &buffer[8]);

  nb_buffers = BUFFER_SIZE/block_size;

  pbuffer = (unsigned int *) buffer;
  // Set all buffers to invalidate state
  for(i=0; i<nb_buffers; i++)
  {
    *pbuffer = 0;
    pbuffer += block_size;
  }

  // Launch data acquisition
  pbuffer_w = buffer;
  cur_i_buffer = 4;
  pbuffer = (volatile unsigned int *) buffer;

  for(i=0; i<nb_buffers; i++)
  {
    /*! \brief add the address of memory buffer*/
    pdca_reload_channel(pdca_channel_usart, pbuffer_w + 4, block_size-4);
    /* enable the transfer */
    pdca_enable(pdca_channel_usart);
    /*! \brief Enable pdca transfer interrupt when completed*/
    pdca_enable_interrupt_transfer_complete(pdca_channel_usart);

    usart_putchar(EXAMPLE_USART, 1);
    while(!*pbuffer);
    pbuffer = (volatile unsigned int *) ((unsigned char *) pbuffer + block_size);
  }

  cur_i_read_buffer = 4;

  dac_pwm_period = 0;
  dac_pwm_periods_per_sample = (DAC_PWM_FREQUENCY + (sample_rate >> 1))/(sample_rate << 1);
  get_sample = 0;
  get_new_block = 0;
  fill_activ = 0;

  set_new_dac_buffer(buffer);

  // Initialize the PWM
  dac_pwm_init();

  even = 0;
  while(1)
  {
    if (get_sample)
    {
        dac_pwm_period = dac_pwm_periods_per_sample - 1;

        if (cur_i_read_buffer < block_size)
        {
          if (!even)
            c = pbuffer_r[cur_i_read_buffer] & 0xF;
          else
            c = (pbuffer_r[cur_i_read_buffer++] >> 4) & 0xF;
          even = !even;
          sound_ampl = ((U16) dsp_adpcm_ima_decode_nibble(c, &step_index, &predicted_value)) + 0x8000;
          AVR32_PWM.channel[DAC_PWM_CHANNEL_MSB].cupd = (sound_ampl >> 8) & 0xFF;
          AVR32_PWM.channel[DAC_PWM_CHANNEL_LSB].cupd = sound_ampl & 0xFF;
        }
      else
      {
          // Point on the next buffer to be read
          pbuffer = (unsigned int *) ((unsigned char *) pbuffer_r + block_size);
          if (((unsigned char *) pbuffer) + block_size >= ((unsigned char *) buffer) + BUFFER_SIZE)
          pbuffer = (unsigned int *) buffer;

          // If next buffer is ready for reading
          if (*((unsigned int *) pbuffer))
          {
            // Invalid the previously read buffer
            *((unsigned int *) pbuffer_r) = 0;
            set_new_dac_buffer((char *) pbuffer);
          }

          // If auto-fill is not active and the next buffer is invalidate, then launch a data block acquisition
          if (!fill_activ && !*((unsigned int *) pbuffer_w))
          {
            // If the buffer is invalidate, fill it with new data;
            get_new_block = 1;
              fill_activ = 1;
          }
      }

      get_sample = 0;
    }

    // Get the next block of data
    if (get_new_block)
    {

      /*! \brief add the address of memory buffer*/
      pdca_reload_channel(pdca_channel_usart, pbuffer_w + 4, block_size-4);
      /* enable the transfer */
      pdca_enable(pdca_channel_usart);
      /*! \brief Enable pdca transfer interrupt when completed*/
      pdca_enable_interrupt_transfer_complete(pdca_channel_usart);
      usart_putchar(EXAMPLE_USART, 1);

      get_new_block = 0;
    }
  }
}
