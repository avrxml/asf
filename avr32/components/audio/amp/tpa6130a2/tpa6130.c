/*****************************************************************************
 *
 * \file
 *
 * \brief TPA6130 amplifier driver.
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

#include "pdca.h"
#include "abdac.h"
#include "gpio.h"
#include "intc.h"
#include "board.h"
#include "tpa6130.h"
#include "conf_tpa6130.h"
#include "audio.h"
//#include "print_funcs.h"

#if ( UC3A0 || UC3A1 ) 
# include "twi.h"
#else
# include "twim.h"
#endif

/* Register map
 * Use these definitions with the register write/read functions
 */
#define TPA6130_CONTROL              0x1
#define TPA6130_VOLUME_AND_MUTE      0x2
#define TPA6130_OUTPUT_IMPEDANCE     0x3
#define TPA6130_I2C_ADDRESS_VERSION  0x4

/* Default register values after a reset */
#define TPA6130_CONTROL_DEFAULT             0x00
//#define TPA6130_VOLUME_AND_MUTE_DEFAULT   0xC0
#define TPA6130_VOLUME_AND_MUTE_DEFAULT     0x0F
#define TPA6130_OUTPUT_IMPEDANCE_DEFAULT    0x00
#define TPA6130_I2C_ADDRESS_VERSION_DEFAULT 0x02
/* Register bitfield macros */
// Control register
#define HP_EN_L          0x80
#define HP_EN_R          0x40
#define STEREO_HP        0x00
#define DUAL_MONO_HP     0x10
#define BRIDGE_TIED_LOAD 0x20
#define SW_SHUTDOWN      0x01
#define THERMAL          0x02
// Volume and mute register
#define MUTE_L           0x80
#define MUTE_R           0x40
// Output impedance register
#define HIZ_L            0x80
#define HIZ_R            0x40
// I2C address version register
#define VERSION          0x02

/* Maximum volume that can be set on the amplifier
 * 0x3F = 63 = 4dB, minimum is 0x0 = -59dB.
 */
#define TPA6130_MAX_VOLUME  0x3F

/* Useful macros for the twi write/read functions*/
// read from hardware
#define TWI_READ_HW 0
// read from shadow registers
#define TWI_READ_SR 1

/* Pin configurations. These are described in the configuration file
 * for the amplifier. A lot more combinations are possible but we
 * concentrate here on the most used cases like simple stereo with
 * positive and negative ABDAC signals (all ABDAC pins used,
 * TPA6130_MODE_STEREO + TPA6130_SIG_DIFF)*/
#if(TPA6130_MODE == TPA6130_MODE_STEREO)
#if(TPA6130_SIG == TPA6130_SIG_DIFF)
static const gpio_map_t TPA6130_ABDAC_GPIO_MAP =
{
  {TPA6130_DATA0_PIN, TPA6130_DATA0_FUNCTION},
  {TPA6130_DATA1_PIN, TPA6130_DATA1_FUNCTION},
  {TPA6130_DATAN0_PIN, TPA6130_DATAN0_FUNCTION},
  {TPA6130_DATAN1_PIN, TPA6130_DATAN1_FUNCTION}
};
#elif(TPA6130_SIG == TPA6130_SIG_POS)
static const gpio_map_t TPA6130_ABDAC_GPIO_MAP =
{
  {TPA6130_DATA0_PIN, TPA6130_DATA0_FUNCTION},
  {TPA6130_DATA1_PIN, TPA6130_DATA1_FUNCTION}
};
#else
#error No valid TPA6130_SIG is defined
#endif
#elif(TPA6130_MODE == TPA6130_MODE_MONO)
#if(TPA6130_SIG == TPA6130_SIG_DIFF)
static const gpio_map_t TPA6130_ABDAC_GPIO_MAP =
{
  {TPA6130_DATA0_PIN, TPA6130_DATA0_FUNCTION},
  {TPA6130_DATAN0_PIN, TPA6130_DATAN0_FUNCTION}
};
#elif(TPA6130_SIG == TPA6130_SIG_POS)
static const gpio_map_t TPA6130_ABDAC_GPIO_MAP =
{
  {TPA6130_DATA0_PIN, TPA6130_DATA0_FUNCTION}
};
#else
#error No valid TPA6130_SIG is defined
#endif
/*
#elif(TPA6130_MODE == TPA6130_MODE_MONO_R)
#if(TPA6130_SIG == TPA6130_SIG_DIFF)
static const gpio_map_t TPA6130_ABDAC_GPIO_MAP =
{
  {TPA6130_DATA1_PIN, TPA6130_DATA1_FUNCTION}
  {TPA6130_DATAN1_PIN, TPA6130_DATAN1_FUNCTION}
}
#elif(TPA6130_SIG == TPA6130_SIG_POS)
static const gpio_map_t TPA6130_ABDAC_GPIO_MAP =
{
  {TPA6130_DATA1_PIN, TPA6130_DATA1_FUNCTION}
};
#else
#error No valid TPA6130_SIG is defined
#endif
*/
#else
  #error No output mode defined in configuration setup
#endif


/* Internal driver parameters */
static struct
{
  uint8_t num_channels;
  void (*callback)(uint32_t arg);
  uint32_t callback_opt;
} tpa6130_output_param =
{
  .num_channels = 0,
  .callback       = NULL,
  .callback_opt   = 0,
};

/*! \brief Driver internal shadow register set.
 */
static struct
{
  uint8_t control;
  uint8_t volume_and_mute;
  uint8_t output_impedance;
  uint8_t i2c_address_version;
} tpa6130_shadow_regs =
{
  .control             = TPA6130_CONTROL_DEFAULT,
  .volume_and_mute     = TPA6130_VOLUME_AND_MUTE_DEFAULT,
  .output_impedance    = TPA6130_OUTPUT_IMPEDANCE_DEFAULT,
  .i2c_address_version = TPA6130_I2C_ADDRESS_VERSION_DEFAULT,
};

ISR(tpa6130_abdac_tx_pdca_int_handler, TPA6130_ABDAC_PDCA_IRQ_GROUP, TPA6130_ABDAC_PDCA_INT_LEVEL)
{
  if (pdca_get_transfer_status(TPA6130_ABDAC_PDCA_CHANNEL) & PDCA_TRANSFER_COMPLETE)
  {
    pdca_disable_interrupt_transfer_complete(TPA6130_ABDAC_PDCA_CHANNEL);
    if (tpa6130_output_param.callback_opt & AUDIO_DAC_OUT_OF_SAMPLE_CB)
      tpa6130_output_param.callback(AUDIO_DAC_OUT_OF_SAMPLE_CB);
  }

  if (pdca_get_transfer_status(TPA6130_ABDAC_PDCA_CHANNEL) & PDCA_TRANSFER_COUNTER_RELOAD_IS_ZERO)
  {
    pdca_disable_interrupt_reload_counter_zero(TPA6130_ABDAC_PDCA_CHANNEL);
    if (tpa6130_output_param.callback_opt & AUDIO_DAC_RELOAD_CB)
      tpa6130_output_param.callback(AUDIO_DAC_RELOAD_CB);
  }
}

/*! \brief Writes data to a register.
 *  \param reg Register index. Use the defines in this file.
 *  \param data Register data. Macros from this file can be used
 *  to ease writing to the bitfields.
 */
static void tpa6130_write_data(uint8_t reg, uint8_t data)
{
  uint16_t message = (reg << 8) | data;
  int twi_status;

  twi_package_t twi_package =
  {
    .chip = TPA6130_TWI_ADDRESS,
    .addr_length = 0,//AVR32_TWI_MMR_IADRSZ_NO_ADDR,
    .buffer = &message,
    .length = sizeof(message)
  };

  do
  {
     twi_status=twi_master_write(TPA6130_TWI, &twi_package);
  }
  while( twi_status != TWI_SUCCESS );

  /* Save write value to shadow registers */
  *(((uint8_t *) &tpa6130_shadow_regs) + reg - 1) = data;
}

/*! \brief Reads data from a register.
 *  The shadow parameter is used to specify if the data should be read
 *  from a driver internal register shadowing or directly from the device.
 *  \param reg Register index.
 *  \param shadow Read from device (shadow=false) or from shadowed register
 *  (shadow=true).
 */
static uint8_t tpa6130_read_data(uint8_t reg, bool shadow)
{
  uint8_t data;
  /*If we want to read from the shadowed registers */
  if(shadow)
  {
    data = *((uint8_t *) &tpa6130_shadow_regs + reg - 1);
  }
  else
  {
    twi_package_t twi_package =
    {
      .chip = TPA6130_TWI_ADDRESS,
      .addr_length = 1,//AVR32_TWI_MMR_IADRSZ_ONE_BYTE,
      .addr[0] = reg,
      .buffer = &data,
      .length = sizeof(data)
    };
    twi_master_read(TPA6130_TWI, &twi_package);
  }
  //print_dbg("Read reg ");
  //print_dbg_ulong(reg);
  //print_dbg(" = 0x");
  //print_dbg_hex(data);
  //print_dbg("\n");

  return data;
}

/*! \brief Probes and initializes the amplifier.
 *  Probes the TWI bus for the amplifier by using the slave address
 *  specified in the configuration (TPA6130_TWI_ADDRESS).
 *  If the device responds with an ACK the version register is read out
 *  and compared to the valid versions (TPA6130_VERSION).
 *  Last step is to set volume to 0, unmute and set the configuration
 *  specified in the conf_tpa6130.h file (stereo, mono ..).
 *
 *  \returns A positive value upon success and a negative value upon failure.
 */
int8_t tpa6130_init(void)
{
  /* Check if the device responds on the TWI bus*/
  if(twi_probe(TPA6130_TWI, TPA6130_TWI_ADDRESS) != TWI_SUCCESS)
  return TWI_NO_CHIP_FOUND;
  /* If the device has no valid version we can not use it */
  if(tpa6130_read_data(TPA6130_I2C_ADDRESS_VERSION, TWI_READ_HW)!= VERSION)
  {
    return -8;
  }
  /* un-mute the output channels, the volume is still 0 and
   * should be increased by an application (fade-in/fade-out) */
  tpa6130_write_data(TPA6130_VOLUME_AND_MUTE, tpa6130_shadow_regs.volume_and_mute);
  /* set stereo/mono mode and enable both amplifiers (left/right) */
  tpa6130_write_data(TPA6130_CONTROL,(TPA6130_MODE << 4) | HP_EN_L | HP_EN_R);

  return TWI_SUCCESS;
}

/*! \brief Shuts down the amplifier and sets it into low power mode.
 *  This is the software low power mode described in the datasheet.
 */
void tpa6130_shutdown(void)
{
  uint8_t data;
  data = tpa6130_read_data(TPA6130_CONTROL, TWI_READ_HW);
  tpa6130_write_data(TPA6130_CONTROL, data | SW_SHUTDOWN);
}
/*! \brief Powers up the amplifier from low power mode.
 */
void tpa6130_powerup(void)
{
  uint8_t data;
  data = tpa6130_read_data(TPA6130_CONTROL, TWI_READ_HW);
  tpa6130_write_data(TPA6130_CONTROL, data & (~SW_SHUTDOWN));
}
/*! \brief Sets the volume of the amplifier.
 *  Valid values are between 0 (min -59dB) and 63 (max 4dB) although
 *  the function takes care of any values higher than that by setting
 *  it to max.
 *  A volume of 0 will mute both channels. Any other value will unmute
 *  them.
 */
void tpa6130_set_volume(int8_t volume)
{
  int8_t new_volume = volume;

  if(volume > TPA6130_VOL_MAX)
  {
    new_volume = TPA6130_VOL_MAX;
  }
  else if(volume <= TPA6130_VOL_MIN )
  {
    // MUTE Left and Right;
    new_volume = MUTE_L|MUTE_R;
  }

  tpa6130_write_data(TPA6130_VOLUME_AND_MUTE, new_volume );
}

/*! \brief Gets the current volume settings.
 *  \returns Current volume settings. Value is between 0 (-59dB) and
 *  63 (4dB).
 */
int8_t tpa6130_get_volume(void)
{
  return tpa6130_read_data(TPA6130_VOLUME_AND_MUTE, TWI_READ_SR);
}

/*! \brief Starts the ABDAC.
 *  \param sample_rate_hz Sample rate for the ABDAC.
 *  \param num_channels number of channel
 *  \param bits_per_sample number of bit per sample
 *  \param swap_channels true -> swap the channels
 *  \param callback Provide a functions that handles callback.
 *  \param callback_opt Callback flag
 *  \param pba_hz Speed of the peripheral bus A.
 */
void tpa6130_dac_start(uint32_t sample_rate_hz,
                       uint8_t num_channels,
                       uint8_t bits_per_sample,
                       bool swap_channels,
                       void (*callback)(uint32_t arg),
                       uint32_t callback_opt,
                       uint32_t pba_hz)
{
  /* stop ABDAC if running*/
  tpa6130_dac_stop();

  /* configure used pins for ABDAC */
  gpio_enable_module(TPA6130_ABDAC_GPIO_MAP,
    sizeof(TPA6130_ABDAC_GPIO_MAP) /
    sizeof(TPA6130_ABDAC_GPIO_MAP[0]));

  /* configure and start PDC and ABDAC*/
  tpa6130_dac_setup(sample_rate_hz,
    num_channels,
    bits_per_sample,
    swap_channels,
    callback,
    callback_opt,
    pba_hz);

  /* Register a interrupt service routine for the ABDAC channel of
   * the PDCA
   */
  irq_register_handler(tpa6130_abdac_tx_pdca_int_handler, TPA6130_ABDAC_PDCA_IRQ, 1);

  tpa6130_powerup();

}

/*! \brief Sets the DACs up with new settings.
 *
 * \note The DACs must have been started beforehand.
 */
void tpa6130_dac_setup(uint32_t sample_rate_hz,
                       uint8_t num_channels,
                       uint8_t bits_per_sample,
                       bool swap_channels,
                       void (*callback)(uint32_t arg),
                       uint32_t callback_opt,
                       uint32_t pba_hz)
{
  // save input parameters to local driver data
  tpa6130_output_param.num_channels = num_channels;
  tpa6130_output_param.callback     = callback;
  tpa6130_output_param.callback_opt = callback_opt;

  /* Probe for amplifier and initialize it */
  tpa6130_init();

#if defined(TPA6130_DAC_CLOCK_SET_CALLBACK)
  TPA6130_DAC_CLOCK_SET_CALLBACK(sample_rate_hz);
#else
  /* ABDAC configuration
   * The ABDAC needs the input frequency of its generic clock (bus_hz)
   * Here we use the configuration value from the conf_tpa6130.h file
   * (TPA6130_ABDAC_GCLK_INPUT_HZ).
   * The sample rate specifies the desired sample rate for the ABDAC.
   * The generic clock input must be greater than 256*sample_rate_hz
   * or the setup of the ABDAC will fail silently here.
   * TODO we could add asserts here to detect wrong settings during
   * compile time.
   */
  if(!abdac_set_dac_sample_rate(sample_rate_hz)) {
    // if it is not possible to set correctly the sample rate
    // Use default set function
    abdac_set_dac_hz(TPA6130_ABDAC, TPA6130_ABDAC_GCLK_INPUT_HZ,sample_rate_hz);
  }
#endif

  if(swap_channels)
  {
    abdac_swap_channels(TPA6130_ABDAC);
  }
  abdac_enable(TPA6130_ABDAC);

  /* PDCA setup */
  /*FIXME we use only word as transfer size for now.
   * half-word transfer size will only write to channel0
   * of the ABDAC, this can be used to implement mono */
  pdca_channel_options_t tpa6130_abdac_pdca_options =
  {
    .addr   = NULL,
    .size   = 0,
    .r_addr   = 0,
    .r_size   = 0,
    .pid    = TPA6130_ABDAC_PDCA_PID,
    .transfer_size  = PDCA_TRANSFER_SIZE_WORD
  };

  /* Initialize the PCDA for the ABDAC
   * The channel number can be set in the configuration file
   * with the define TPA6130_ABDAC_PDCA_CHANNEL.
   */
  pdca_init_channel(TPA6130_ABDAC_PDCA_CHANNEL,
    &tpa6130_abdac_pdca_options);
  /* Enable the PDCA channel. Since we did not provide any data
   * yet the channel is in idle mode */
  pdca_enable(TPA6130_ABDAC_PDCA_CHANNEL);

}

/*! \brief Outputs a sample buffer to the DACs.
 * The input requires a sample buffer that consists of words (32-bit)
 * which contain two (16-bit) samples, one for each channel.
 *
 * \note The DACs must have been started beforehand.
 */
bool tpa6130_dac_output(void *sample_buffer, size_t sample_length)
{
  //int global_interrupt_enabled;

  /*Wait until the PDCA loads the reload value to its transfer
   * counter register(TCRR=0). Then we are ready to set up a new
   * transfer */
  if(!(pdca_get_transfer_status(TPA6130_ABDAC_PDCA_CHANNEL) &
    PDCA_TRANSFER_COUNTER_RELOAD_IS_ZERO))
  {
    return false;
  }

  /* Nothing to do if we get no data. */
  if(sample_length)
  {

    /*TODO Do we really need to adjust the buffer for mono*/

    /* While reloading the PDC we do not need any active interrupt*/
    //if((global_interrupt_enabled = cpu_irq_is_enabled()))
    //  cpu_irq_disable();

    /*FIXME This assumes a stereo 16-bit sample size */
    // one sample here consists of 2x16-bit (16-bit stereo)
    pdca_reload_channel(TPA6130_ABDAC_PDCA_CHANNEL,
      sample_buffer, sample_length);

    //if(global_interrupt_enabled)
    //  cpu_irq_enable();

    /*TODO enable transfer complete interrupt
     * Is it possible to move this to setup or other places?*/
    if(tpa6130_output_param.callback_opt & AUDIO_DAC_OUT_OF_SAMPLE_CB)
      pdca_enable_interrupt_transfer_complete(TPA6130_ABDAC_PDCA_CHANNEL);
    if (tpa6130_output_param.callback_opt & AUDIO_DAC_RELOAD_CB)
      pdca_enable_interrupt_reload_counter_zero(TPA6130_ABDAC_PDCA_CHANNEL);
  }
  return true;
}

bool tpa6130_dac_is_volume_muted(void)
{
  return false;
}

void tpa6130_dac_mute(bool mute)
{
  // //1st Version Mute Audio for Play/Pause
/*  int volume=tpa6130_get_volume();
  if(mute==true) {
     //Mute volume
     volume= volume|MUTE_L|MUTE_R;
  }
  else {
     //Unmute volume
     volume= volume&(~(MUTE_L|MUTE_R));

  }
 tpa6130_write_data(TPA6130_VOLUME_AND_MUTE,volume);
*/
  //2n Version Stop PDCA >> No lost of audio when pause
/*  if(mute==true) {
    pdca_disable(TPA6130_ABDAC_PDCA_CHANNEL);
  }
  else {
    pdca_enable(TPA6130_ABDAC_PDCA_CHANNEL);
  }
*/

  // 3rd Version wait until the current buffers are empty and disable the interrupts
  int8_t volume = tpa6130_get_volume();
  if (mute)
  {
    uint32_t save_dac_reload_callback_opt;

    // Mute the audio stream
    volume = volume | MUTE_L | MUTE_R;
    tpa6130_write_data(TPA6130_VOLUME_AND_MUTE, volume);
    // Disable the reload channel of the interrupt
    save_dac_reload_callback_opt = tpa6130_output_param.callback_opt;
    tpa6130_output_param.callback_opt = 0;
    // Disable the reload interruption and wait until the transfer is complete
    pdca_disable_interrupt_reload_counter_zero(TPA6130_ABDAC_PDCA_CHANNEL);
    while (!(pdca_get_transfer_status(TPA6130_ABDAC_PDCA_CHANNEL) & PDCA_TRANSFER_COMPLETE));
    // Restore the reload callback function
    tpa6130_output_param.callback_opt = save_dac_reload_callback_opt;
  }
  else
  {
    // Re-enable the interrupts
    pdca_enable_interrupt_reload_counter_zero(TPA6130_ABDAC_PDCA_CHANNEL);
    // Un-mute the audio stream
    volume = volume & (~(MUTE_L | MUTE_R));
    tpa6130_write_data(TPA6130_VOLUME_AND_MUTE, volume);
  }
}

bool tpa6130_dac_is_volume_boosted(void)
{
  return false;
}

/*! \brief Returns the current volume of the DAC.
 *         The volume is in the range 0 - 255
 */
uint8_t tpa6130_dac_get_volume(void)
{
 // return volume is num display step for LCD
 //  volume scale is between 10 and 245
 // 0 is -100db
 // 245 is max volume
  uint16_t raw_volume;
  raw_volume = (tpa6130_get_volume() & (~(MUTE_L | MUTE_R)));
  return (uint8_t) ((raw_volume * 255) / TPA6130_VOL_MAX);
}

/*! \brief Set the volume of the DAC.
 */
void tpa6130_dac_set_volume(uint8_t volume)
{
   tpa6130_set_volume(volume);
}

/*! \brief Increases the output volume of the amplifier by one step.
 * Stops at the maximum volume and thus does not wrap to the
 * lowest volume.
 */
void tpa6130_dac_increase_volume(void)
{
  int8_t volume = tpa6130_get_volume()& (~(MUTE_L | MUTE_R));
  if( volume < TPA6130_VOL_MIN )
    volume = TPA6130_VOL_MIN;
  tpa6130_set_volume(volume+1);
}

/*! \brief Decreases the output volume of the amplifier by one step.
 *
 * Stops at the lowest possible volume.
 */
void tpa6130_dac_decrease_volume(void)
{
  int8_t volume = tpa6130_get_volume()& (~(MUTE_L | MUTE_R));;
  if( volume > TPA6130_VOL_MIN )
    --volume;
  tpa6130_set_volume( volume );
}

/*! \brief Flushes the sample buffer being output to the ABDAC.
 */
void tpa6130_dac_flush(void)
{
  pdca_disable_interrupt_transfer_complete(TPA6130_ABDAC_PDCA_CHANNEL);
  pdca_disable_interrupt_reload_counter_zero(TPA6130_ABDAC_PDCA_CHANNEL);

  /*TODO Do we really want to wait here? Or do we just don't care when
   * the buffer is empty/flushed */
  //while(!pdca_get_transfer_status(TPA6130_ABDAC_PDCA_CHANNEL) &
  //  PDCA_TRANSFER_COMPLETE);
  pdca_disable       (TPA6130_ABDAC_PDCA_CHANNEL       );
  pdca_load_channel  (TPA6130_ABDAC_PDCA_CHANNEL,0x0, 0);
  pdca_reload_channel(TPA6130_ABDAC_PDCA_CHANNEL,0x0, 0);
  pdca_enable        (TPA6130_ABDAC_PDCA_CHANNEL       );
}

/*! \brief Stops the ABDAC and puts the amplifier in low power mode.
 *  Additionally it sets all used pins to the GPIO state.
 *  The counter part of this function is tpa6130_dac_start(...)
 */
void tpa6130_dac_stop(void)
{
  /* Disable amplifier 1st */
  tpa6130_shutdown();

  /* Flush the dac */
  // Don't flush the DAC when stop
  //tpa6130_dac_flush();

  /* Disable ABDAC */
  abdac_disable(TPA6130_ABDAC);

  /* Stop  PDCA */
  pdca_disable(TPA6130_ABDAC_PDCA_CHANNEL);

  /* Set used GPIO pins to GPIO state */
  gpio_enable_gpio(TPA6130_ABDAC_GPIO_MAP,
    sizeof(TPA6130_ABDAC_GPIO_MAP)
    / sizeof(TPA6130_ABDAC_GPIO_MAP[0]));

  tpa6130_output_param.num_channels = 0;
  tpa6130_output_param.callback     = NULL;
  tpa6130_output_param.callback_opt = 0;
}
