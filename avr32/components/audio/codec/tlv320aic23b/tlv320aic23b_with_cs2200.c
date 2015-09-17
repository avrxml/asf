/*****************************************************************************
 *
 * \file
 *
 * \brief TLV320AIC23B stereo audio CODEC driver for AVR32.
 *
 *                       can be used.
 *
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
#include <string.h>

#include "compiler.h"
#include "audio.h"
#include "tlv320aic23b.h"

#include <avr32/io.h>
#include "intc.h"
#include "gpio.h"
#if AIC23B_CTRL_INTERFACE == AIC23B_CTRL_INTERFACE_TWI
  #if AIC23B_TWI_ADDRESS != 0x1A && AIC23B_TWI_ADDRESS != 0x1B
    #error TLV320AIC23B: Invalid TWI address selected!
  #endif
  #include "twi.h"
#elif AIC23B_CTRL_INTERFACE == AIC23B_CTRL_INTERFACE_SPI
  #include "spi.h"
#else
  #error TLV320AIC23B: Invalid control interface selected!
#endif
#include "ssc_i2s.h"
#include "pdca.h"
#include "cs2200.h"
#include "conf_audio_player.h"


//! Number of registers.
#define AIC23B_REG_COUNT  (sizeof(aic23b_ctrl) / sizeof(aic23b_ctrl[0]))

//! Default local image of the registers.
#define AIC23B_CTRL_RESET                                                      \
{                                                                              \
  {                                                                            \
    .addr = AIC23B_ADDRESS(AIC23B_LLICVC),                                     \
    .data = AIC23B_DEFAULT(AIC23B_LLICVC)                                      \
  },                                                                           \
  {                                                                            \
    .addr = AIC23B_ADDRESS(AIC23B_RLICVC),                                     \
    .data = AIC23B_DEFAULT(AIC23B_RLICVC)                                      \
  },                                                                           \
  {                                                                            \
    .addr = AIC23B_ADDRESS(AIC23B_LCHVC),                                      \
    .data = AIC23B_DEFAULT(AIC23B_LCHVC)                                       \
  },                                                                           \
  {                                                                            \
    .addr = AIC23B_ADDRESS(AIC23B_RCHVC),                                      \
    .data = AIC23B_DEFAULT(AIC23B_RCHVC)                                       \
  },                                                                           \
  {                                                                            \
    .addr = AIC23B_ADDRESS(AIC23B_AAPC),                                       \
    .data = AIC23B_DEFAULT(AIC23B_AAPC)                                        \
  },                                                                           \
  {                                                                            \
    .addr = AIC23B_ADDRESS(AIC23B_DAPC),                                       \
    .data = AIC23B_DEFAULT(AIC23B_DAPC)                                        \
  },                                                                           \
  {                                                                            \
    .addr = AIC23B_ADDRESS(AIC23B_PDC),                                        \
    .data = AIC23B_DEFAULT(AIC23B_PDC)                                         \
  },                                                                           \
  {                                                                            \
    .addr = AIC23B_ADDRESS(AIC23B_DAIF),                                       \
    .data = AIC23B_DEFAULT(AIC23B_DAIF)                                        \
  },                                                                           \
  {                                                                            \
    .addr = AIC23B_ADDRESS(AIC23B_SRC),                                        \
    .data = AIC23B_DEFAULT(AIC23B_SRC)                                         \
  },                                                                           \
  {                                                                            \
    .addr = AIC23B_ADDRESS(AIC23B_DIA),                                        \
    .data = AIC23B_DEFAULT(AIC23B_DIA)                                         \
  },                                                                           \
  {                                                                            \
    .addr = AIC23B_ADDRESS(AIC23B_RR),                                         \
    .data = AIC23B_DEFAULT(AIC23B_RR)                                          \
  }                                                                            \
}


//! Map of the SSC pins used by the TLV320AIC23B.
#if (AIC23B_MODE==AIC23B_MODE_CODEC)
static const gpio_map_t AIC23B_SSC_CODEC_GPIO_MAP =
{
  {AIC23B_SSC_TX_CLOCK_PIN,       AIC23B_SSC_TX_CLOCK_FUNCTION      },
  {AIC23B_SSC_TX_DATA_PIN,        AIC23B_SSC_TX_DATA_FUNCTION       },
  {AIC23B_SSC_TX_FRAME_SYNC_PIN,  AIC23B_SSC_TX_FRAME_SYNC_FUNCTION },
  {AIC23B_SSC_RX_DATA_PIN,        AIC23B_SSC_RX_DATA_FUNCTION       },
  {AIC23B_SSC_RX_FRAME_SYNC_PIN,  AIC23B_SSC_RX_FRAME_SYNC_FUNCTION }
};
#elif (AIC23B_MODE==AIC23B_MODE_ADC)
static const gpio_map_t AIC23B_SSC_ADC_GPIO_MAP =
{
  {AIC23B_SSC_TX_CLOCK_PIN,       AIC23B_SSC_TX_CLOCK_FUNCTION      },
  {AIC23B_SSC_RX_DATA_PIN,        AIC23B_SSC_RX_DATA_FUNCTION       },
  {AIC23B_SSC_RX_FRAME_SYNC_PIN,  AIC23B_SSC_RX_FRAME_SYNC_FUNCTION }
};
#elif (AIC23B_MODE==AIC23B_MODE_DAC)
static const gpio_map_t AIC23B_SSC_DAC_GPIO_MAP =
{
#if defined(AIC23B_DAC_USE_RX_CLOCK) && AIC23B_DAC_USE_RX_CLOCK == true
  {AVR32_SSC_RX_CLOCK_0_PIN,      AVR32_SSC_RX_CLOCK_0_FUNCTION     },
#endif
  {AIC23B_SSC_TX_CLOCK_PIN,       AIC23B_SSC_TX_CLOCK_FUNCTION      },
  {AIC23B_SSC_TX_DATA_PIN,        AIC23B_SSC_TX_DATA_FUNCTION       },
  {AIC23B_SSC_TX_FRAME_SYNC_PIN,  AIC23B_SSC_TX_FRAME_SYNC_FUNCTION }
};
#else
  #error  No Mode defined in file 'conf_tlv320aic23b.h'
#endif

//! Default local image of the registers.
static const aic23b_ctrl_t aic23b_ctrl_reset[] = AIC23B_CTRL_RESET;

//! Local image of the registers.
static aic23b_ctrl_t aic23b_ctrl[] = AIC23B_CTRL_RESET;

//! CS2200 out frequency
extern uint32_t usb_stream_resync_frequency;

//! Output parameters.
static struct
{
  uint8_t num_channels;
  void (*callback)(uint32_t arg);
  uint32_t callback_opt;
} aic23b_output_params =
{
  .num_channels = 0,
  .callback     = NULL,
  .callback_opt = 0
};


/*! \brief SSC TX PDCA interrupt handler managing the out-of-samples condition.
 */
#if (AIC23B_MODE==AIC23B_MODE_CODEC)||(AIC23B_MODE==AIC23B_MODE_DAC)
#if (defined __GNUC__) && (defined __AVR32__)
__attribute__((__interrupt__))
#elif (defined __ICCAVR32__)
__interrupt
#endif
static void aic23b_ssc_tx_pdca_int_handler(void)
{
  if (pdca_get_transfer_status(AIC23B_SSC_TX_PDCA_CHANNEL) & PDCA_TRANSFER_COMPLETE)
  {
    pdca_disable_interrupt_transfer_complete(AIC23B_SSC_TX_PDCA_CHANNEL);
    if (aic23b_output_params.callback_opt & AUDIO_DAC_OUT_OF_SAMPLE_CB)
      aic23b_output_params.callback(AUDIO_DAC_OUT_OF_SAMPLE_CB);
  }

  if (pdca_get_transfer_status(AIC23B_SSC_TX_PDCA_CHANNEL) & PDCA_TRANSFER_COUNTER_RELOAD_IS_ZERO)
  {
    pdca_disable_interrupt_reload_counter_zero(AIC23B_SSC_TX_PDCA_CHANNEL);
    if (aic23b_output_params.callback_opt & AUDIO_DAC_RELOAD_CB)
      aic23b_output_params.callback(AUDIO_DAC_RELOAD_CB);
  }
}
#endif


/*! \brief SSC RX PDCA interrupt handler managing the out-of-samples condition.
 */
#if (AIC23B_MODE==AIC23B_MODE_CODEC)||(AIC23B_MODE==AIC23B_MODE_ADC)
#if (defined __GNUC__) && (defined __AVR32__)
__attribute__((__interrupt__))
#elif (defined __ICCAVR32__)
__interrupt
#endif
static void aic23b_ssc_rx_pdca_int_handler(void)
{
  if (pdca_get_transfer_status(AIC23B_SSC_RX_PDCA_CHANNEL) & PDCA_TRANSFER_COMPLETE)
  {
    pdca_disable_interrupt_transfer_complete(AIC23B_SSC_RX_PDCA_CHANNEL);
    if (aic23b_output_params.callback_opt & AUDIO_ADC_OUT_OF_SAMPLE_CB)
      aic23b_output_params.callback(AUDIO_ADC_OUT_OF_SAMPLE_CB);
  }

  if (pdca_get_transfer_status(AIC23B_SSC_RX_PDCA_CHANNEL) & PDCA_TRANSFER_COUNTER_RELOAD_IS_ZERO)
  {
    pdca_disable_interrupt_reload_counter_zero(AIC23B_SSC_RX_PDCA_CHANNEL);
    if (aic23b_output_params.callback_opt & AUDIO_ADC_RELOAD_CB)
      aic23b_output_params.callback(AUDIO_ADC_RELOAD_CB);
  }
}
#endif

/*! \brief Writes the \a ctrl control word.
 */
static void aic23b_write_control_word(aic23b_ctrl_t ctrl)
{
#if AIC23B_CTRL_INTERFACE == AIC23B_CTRL_INTERFACE_TWI
  int twi_status;
  aic23b_ctrl_t my_ctrl = ctrl;
  twi_package_t twi_package =
  {
    .chip         = AIC23B_TWI_ADDRESS,
    .addr_length  = 0,
    .buffer       = &my_ctrl,
    .length       = sizeof(my_ctrl)
  };
  do
  {
     twi_status=twi_master_write(AIC23B_TWI, &twi_package);
  }
  while( twi_status != TWI_SUCCESS );
#elif AIC23B_CTRL_INTERFACE == AIC23B_CTRL_INTERFACE_SPI
  spi_selectChip(AIC23B_SPI, AIC23B_SPI_NPCS);
  spi_write(AIC23B_SPI, *(uint16_t *)&ctrl);
  spi_unselectChip(AIC23B_SPI, AIC23B_SPI_NPCS);
#endif
}


/*! \name Low-Level Interface
 */
//! @{

void aic23b_configure_freq(int master_clock_hz, int sample_rate_hz)
{
  aic23b_src_t src;

  src.data    = AIC23B_DEFAULT(AIC23B_SRC);
  src.clkout  = 0;
  src.clkin   = 0;

  switch (master_clock_hz)
  {
   case 12000000:
    src.usb     = 1;
    if (sample_rate_hz < (8000 + 8021) / 2)
    { // 8000 Hz
      src.sr    = 0x3;
      src.bosr  = 0;
    }
    else if (sample_rate_hz < (8021 + 32000) / 2)
    { // 8021 Hz
      src.sr    = 0xB;
      src.bosr  = 1;
    }
    else if (sample_rate_hz < (32000 + 44100) / 2)
    { // 32000 Hz
      src.sr    = 0x6;
      src.bosr  = 0;
    }
    else if (sample_rate_hz < (44100 + 48000) / 2)
    { // 44100 Hz
      src.sr    = 0x8;
      src.bosr  = 1;
    }
    else if (sample_rate_hz < (48000 + 88200) / 2)
    { // 48000 Hz
      src.sr    = 0x0;
      src.bosr  = 0;
    }
    else if (sample_rate_hz < (88200 + 96000) / 2)
    { // 88200 Hz
      src.sr    = 0xF;
      src.bosr  = 1;
    }
    else
    { // 96000 Hz
      src.sr    = 0x7;
      src.bosr  = 0;
    }
    break;
  case 11289600:
    src.usb     = 0;
    if (sample_rate_hz < (8021 + 22050) / 2)
    { // 8021 Hz
      src.sr    = 0xB;
      src.bosr  = 0;
    }
    else if (sample_rate_hz < (22050 + 88200) / 2)
    { // 22050, 44100 and 48000 Hz
      src.sr    = 0x8;
      src.bosr  = 0;
    }
    else
    { // 88200 Hz
      src.sr    = 0xF;
      src.bosr  = 0;
    }
    break;
  case 18432000:
  case 8192000:
    src.usb   = 0;
    src.sr    = 0;
    src.bosr  = 1;
    break;
  case 12288000:
    src.usb   = 0;
    src.sr    = 6;
    src.bosr  = 0;
    break;
  default:
    //Not supported
    return;
  }

  aic23b_write_reg(AIC23B_SRC, src.data);
}



uint16_t aic23b_read_reg(uint8_t reg)
{
  if (reg >= AIC23B_REG_COUNT) return 0x0000;
  return aic23b_ctrl[reg].data;
}


void aic23b_write_reg(uint8_t reg, uint16_t val)
{
  if (reg >= AIC23B_REG_COUNT) return;
  aic23b_ctrl[reg].data = val;
  if (reg == AIC23B_RR && aic23b_ctrl[reg].data == AIC23B_DEFAULT(AIC23B_RR))
  {
    memcpy(aic23b_ctrl, aic23b_ctrl_reset, sizeof(aic23b_ctrl));
  }
  aic23b_write_control_word(aic23b_ctrl[reg]);
}


//! @}


/*! \name Mid-Level Interface
 */
//! @{


int8_t aic23b_get_line_in_volume(uint8_t ch)
{
  switch (ch)
  {
  case AIC23B_LEFT_CHANNEL:
    {
      aic23b_llicvc_t llicvc;
      llicvc.data = aic23b_read_reg(AIC23B_LLICVC);
      return (llicvc.lim) ?
             AIC23B_MUTED :
             llicvc.liv - AIC23B_LIN_VOL_OFFSET;
    }
  case AIC23B_RIGHT_CHANNEL:
    {
      aic23b_rlicvc_t rlicvc;
      rlicvc.data = aic23b_read_reg(AIC23B_RLICVC);
      return (rlicvc.rim) ?
             AIC23B_MUTED :
             rlicvc.riv - AIC23B_LIN_VOL_OFFSET;
    }
  default:
    return AIC23B_MUTED;
  }
}


void aic23b_set_line_in_volume(uint8_t ch_mask, int8_t vol)
{
  if (ch_mask & AIC23B_LEFT_CHANNEL)
  {
    aic23b_llicvc_t llicvc;
    llicvc.data = aic23b_read_reg(AIC23B_LLICVC);
    llicvc.lrs = ((ch_mask & AIC23B_RIGHT_CHANNEL) != 0);
    if (vol < AIC23B_LIN_VOL_MIN) llicvc.lim = 1;
    else
    {
      llicvc.lim = 0;
      llicvc.liv = min(vol, AIC23B_LIN_VOL_MAX) + AIC23B_LIN_VOL_OFFSET;
    }
    aic23b_write_reg(AIC23B_LLICVC, llicvc.data);
  }
  else if (ch_mask & AIC23B_RIGHT_CHANNEL)
  {
    aic23b_rlicvc_t rlicvc;
    rlicvc.data = aic23b_read_reg(AIC23B_RLICVC);
    rlicvc.rls = 0;
    if (vol < AIC23B_LIN_VOL_MIN) rlicvc.rim = 1;
    else
    {
      rlicvc.rim = 0;
      rlicvc.riv = min(vol, AIC23B_LIN_VOL_MAX) + AIC23B_LIN_VOL_OFFSET;
    }
    aic23b_write_reg(AIC23B_RLICVC, rlicvc.data);
  }
}

uint8_t aic23b_dac_get_average_headphone_volume(void)
{
  int32_t volume, volume_avg = 0;
  aic23b_lchvc_t lchvc;
  aic23b_rchvc_t rchvc;

  lchvc.data = aic23b_read_reg(AIC23B_LCHVC);
  rchvc.data = aic23b_read_reg(AIC23B_RCHVC);
  volume_avg = (lchvc.lhv < AIC23B_HP_VOL_MIN + AIC23B_HP_VOL_OFFSET) ?
               0 : lchvc.lhv - AIC23B_HP_VOL_OFFSET - AIC23B_HP_VOL_MIN;
  volume = (rchvc.rhv < AIC23B_HP_VOL_MIN + AIC23B_HP_VOL_OFFSET) ?
           0 : rchvc.rhv - AIC23B_HP_VOL_OFFSET - AIC23B_HP_VOL_MIN;
  // Get the volume average
  volume_avg = (volume_avg + volume) / 2;
  // Recenter the volume
  volume_avg = (volume_avg * 255) / (AIC23B_HP_VOL_MAX - AIC23B_HP_VOL_MIN);

  return (uint8_t) volume_avg;
}

void aic23b_dac_set_average_headphone_volume(uint8_t volume)
{
  aic23b_set_headphone_volume(AIC23B_LEFT_CHANNEL | AIC23B_RIGHT_CHANNEL,volume,true);
}

bool aic23b_dac_is_headphone_volume_muted(void)
{
  return false;
}

bool aic23b_dac_is_headphone_volume_boosted(void)
{
  return false;
}

int8_t aic23b_get_headphone_volume(uint8_t ch)
{
  switch (ch)
  {
  case AIC23B_LEFT_CHANNEL:
    {
      aic23b_lchvc_t lchvc;
      lchvc.data = aic23b_read_reg(AIC23B_LCHVC);
      return (lchvc.lhv < AIC23B_HP_VOL_MIN + AIC23B_HP_VOL_OFFSET) ?
             AIC23B_MUTED :
             lchvc.lhv - AIC23B_HP_VOL_OFFSET;
    }
  case AIC23B_RIGHT_CHANNEL:
    {
      aic23b_rchvc_t rchvc;
      rchvc.data = aic23b_read_reg(AIC23B_RCHVC);
      return (rchvc.rhv < AIC23B_HP_VOL_MIN + AIC23B_HP_VOL_OFFSET) ?
             AIC23B_MUTED :
             rchvc.rhv - AIC23B_HP_VOL_OFFSET;
    }
  default:
    return AIC23B_MUTED;
  }
}

void aic23b_set_headphone_volume(uint8_t ch_mask, int8_t vol, bool z_cross)
{
  if (ch_mask & AIC23B_LEFT_CHANNEL)
  {
    aic23b_lchvc_t lchvc;
    lchvc.data = aic23b_read_reg(AIC23B_LCHVC);
    lchvc.lrs = ((ch_mask & AIC23B_RIGHT_CHANNEL) != 0);
    lchvc.lzc = z_cross;
    lchvc.lhv = (vol < AIC23B_HP_VOL_MIN) ?
                0 :
                min(vol, AIC23B_HP_VOL_MAX) + AIC23B_HP_VOL_OFFSET;
    aic23b_write_reg(AIC23B_LCHVC, lchvc.data);
  }
  if (ch_mask & AIC23B_RIGHT_CHANNEL)
  {
    aic23b_rchvc_t rchvc;
    rchvc.data = aic23b_read_reg(AIC23B_RCHVC);
    rchvc.rls = 0;
    rchvc.rzc = z_cross;
    rchvc.rhv = (vol < AIC23B_HP_VOL_MIN) ?
                0 :
                min(vol, AIC23B_HP_VOL_MAX) + AIC23B_HP_VOL_OFFSET;
    aic23b_write_reg(AIC23B_RCHVC, rchvc.data);
  }
}


aic23b_aapc_t aic23b_get_analog_audio_path(void)
{
  aic23b_aapc_t aapc;
  aapc.data = aic23b_read_reg(AIC23B_AAPC);
  return aapc;
}


void aic23b_set_analog_audio_path(aic23b_aapc_t aapc)
{
  aic23b_write_reg(AIC23B_AAPC, aapc.data);
}


aic23b_dapc_t aic23b_get_digital_audio_path(void)
{
  aic23b_dapc_t dapc;
  dapc.data = aic23b_read_reg(AIC23B_DAPC);
  return dapc;
}


void aic23b_set_digital_audio_path(aic23b_dapc_t dapc)
{
  aic23b_write_reg(AIC23B_DAPC, dapc.data);
}


aic23b_pdc_t aic23b_get_power_down_state(void)
{
  aic23b_pdc_t pdc;
  pdc.data = aic23b_read_reg(AIC23B_PDC);
  return pdc;
}


void aic23b_set_power_down_state(aic23b_pdc_t pdc)
{
  aic23b_write_reg(AIC23B_PDC, pdc.data);
}


bool aic23b_is_dig_audio_activated(void)
{
  aic23b_dia_t dia;
  dia.data = aic23b_read_reg(AIC23B_DIA);
  return dia.act;
}


void aic23b_activate_dig_audio(bool act)
{
  aic23b_dia_t dia;
  dia.data = aic23b_read_reg(AIC23B_DIA);
  dia.act = act;
  aic23b_write_reg(AIC23B_DIA, dia.data);
}


void aic23b_reset(void)
{
  aic23b_rr_t rr;
  rr.data = aic23b_read_reg(AIC23B_RR);
  rr.res = AIC23B_DEFAULT(AIC23B_RR) >> AIC23B_OFFSET(AIC23B_RR, RES);
  aic23b_write_reg(AIC23B_RR, rr.data);
}


//! @}


/*! \name High-Level Interface
 */
//! @{
#if (AIC23B_MODE==AIC23B_MODE_DAC)
void aic23b_dac_start(uint32_t sample_rate_hz,
                      uint8_t num_channels,
                      uint8_t bits_per_sample,
                      bool swap_channels,
                      void (*callback)(uint32_t arg),
                      uint32_t callback_opt,
                      uint32_t pba_hz)
{
#if AIC23B_CTRL_INTERFACE == AIC23B_CTRL_INTERFACE_SPI
  static const spi_options_t AIC23B_SPI_OPTIONS =
  {
    .reg          = AIC23B_SPI_NPCS,
    .baudrate     = AIC23B_SPI_MASTER_SPEED,
    .bits         = AIC23B_CTRL_SIZE,
    .spck_delay   = 0,
    .trans_delay  = 0,
    .stay_act     = 0,
    .spi_mode     = 3,
    .modfdis      = 1
  };
  spi_setupChipReg(AIC23B_SPI, &AIC23B_SPI_OPTIONS, pba_hz);
#endif

  aic23b_dac_stop();

  gpio_enable_module(AIC23B_SSC_DAC_GPIO_MAP,
                     sizeof(AIC23B_SSC_DAC_GPIO_MAP) / sizeof(AIC23B_SSC_DAC_GPIO_MAP[0]));

  aic23b_pdc_t pdc;
  pdc.data  = AIC23B_DEFAULT(AIC23B_PDC);
  pdc.off   = 0;
  pdc.clk   = 0;
  pdc.osc   = 0;
  pdc.out   = 0;
  pdc.dac   = 0;
  pdc.adc   = 1;
  pdc.mic   = 1;
  pdc.line  = 1;
  aic23b_set_power_down_state(pdc);

  aic23b_dac_setup(sample_rate_hz,
                   num_channels,
                   bits_per_sample,
                   swap_channels,
                   callback,
                   callback_opt,
                   pba_hz);

  aic23b_aapc_t aapc;
  aapc.data = AIC23B_DEFAULT(AIC23B_AAPC);
  aapc.ste  = 0;
  aapc.dac  = 1;
  aapc.byp  = 0;
  aapc.micm = 1;
  aapc.micb = 0;
  aic23b_set_analog_audio_path(aapc);

  aic23b_dapc_t dapc;
  dapc.data   = AIC23B_DEFAULT(AIC23B_DAPC);
  dapc.dacm   = 0;
  dapc.deemp  = AIC23B_DAPC_DEEMP_NONE;
  dapc.adchp  = 1;
  aic23b_set_digital_audio_path(dapc);

  // set an acceptable start volume
  aic23b_set_headphone_volume(AIC23B_LEFT_CHANNEL | AIC23B_RIGHT_CHANNEL,
                              -30,
                              true);

  aic23b_activate_dig_audio(true);

  INTC_register_interrupt(&aic23b_ssc_tx_pdca_int_handler,
                          AIC23B_SSC_TX_PDCA_IRQ,
                          AIC23B_SSC_TX_PDCA_INT_LEVEL);
}


void aic23b_dac_setup(uint32_t sample_rate_hz,
                      uint8_t num_channels,
                      uint8_t bits_per_sample,
                      bool swap_channels,
                      void (*callback)(uint32_t arg),
                      uint32_t callback_opt,
                      uint32_t pba_hz)
{
  uint32_t master_clock = AIC23B_MCLK_HZ; // default configuration

  // Change the CPU frequency
  //
  //Disable_global_interrupt();

  // Switch to OSC0 during OSC1 transition
  //pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);

  // Switch to PLL0 as the master clock
  //pm_switch_to_clock(&AVR32_PM, AVR32_PM_MCCTRL_MCSEL_PLL0);

  if (sample_rate_hz < (8000 + 8021) / 2)
  { // 8000 Hz
  }
  else if (sample_rate_hz < (8021 + 32000) / 2)
  { // 8021 Hz
  }
  else if (sample_rate_hz < (32000 + 44100) / 2)
  { // 32000 Hz
    master_clock = usb_stream_resync_frequency = 8192000;
    cs2200_freq_clk_out(_32_BITS_RATIO(usb_stream_resync_frequency, FOSC0));
    pba_hz = FCPU_HZ = FHSB_HZ = FPBA_HZ = FPBB_HZ = FMCK_HZ(8192000);
  }
  else if (sample_rate_hz < (44100 + 48000) / 2)
  { // 44100 Hz
    master_clock = usb_stream_resync_frequency = 11289600;
    cs2200_freq_clk_out(_32_BITS_RATIO(usb_stream_resync_frequency, FOSC0));
    pba_hz = FCPU_HZ = FHSB_HZ = FPBA_HZ = FPBB_HZ = FMCK_HZ(11289600);
  }
  else if (sample_rate_hz < (48000 + 88200) / 2)
  { // 48000 Hz
    master_clock = usb_stream_resync_frequency = 12288000;
    cs2200_freq_clk_out(_32_BITS_RATIO(usb_stream_resync_frequency, FOSC0));
    pba_hz = FCPU_HZ = FHSB_HZ = FPBA_HZ = FPBB_HZ = FMCK_HZ(12288000);
  }
  else if (sample_rate_hz < (88200 + 96000) / 2)
  { // 88200 Hz
  }
  else
  { // 96000 Hz
  }

  //Enable_global_interrupt();

#if defined(AIC23B_DAC_USE_RX_CLOCK) && AIC23B_DAC_USE_RX_CLOCK == true
  #if defined(AIC23B_DAC_RX_CLOCK_SET_CALLBACK)
    AIC23B_DAC_RX_CLOCK_SET_CALLBACK(2 * sample_rate_hz *
                                     ((bits_per_sample <= 16) ? 16 :
                                     (bits_per_sample <= 20) ? 20 :
                                     (bits_per_sample <= 24) ? 24 :
                                     32));
  #endif
  ssc_i2s_init(AIC23B_SSC,
               sample_rate_hz,
               bits_per_sample,
               (bits_per_sample <= 16) ? 16 :
               (bits_per_sample <= 20) ? 20 :
               (bits_per_sample <= 24) ? 24 :
                                         32,
               SSC_I2S_MODE_STEREO_OUT_EXT_CLK,
               pba_hz);
#else
  ssc_i2s_init(AIC23B_SSC,
               sample_rate_hz,
               bits_per_sample,
               (bits_per_sample <= 16) ? 16 :
               (bits_per_sample <= 20) ? 20 :
               (bits_per_sample <= 24) ? 24 :
                                         32,
               SSC_I2S_MODE_STEREO_OUT,
               pba_hz);
#endif

  pdca_channel_options_t aic23b_ssc_pdca_options =
  {
    .addr           = NULL,
    .size           = 0,
    .r_addr         = NULL,
    .r_size         = 0,
    .pid            = AIC23B_SSC_TX_PDCA_PID,
    .transfer_size  = (bits_per_sample <=  8) ? PDCA_TRANSFER_SIZE_BYTE      :
                      (bits_per_sample <= 16) ? PDCA_TRANSFER_SIZE_HALF_WORD :
                                                PDCA_TRANSFER_SIZE_WORD
  };
  pdca_init_channel(AIC23B_SSC_TX_PDCA_CHANNEL, &aic23b_ssc_pdca_options);
  pdca_enable(AIC23B_SSC_TX_PDCA_CHANNEL);

#if !defined(AIC23B_DAC_USE_RX_CLOCK) || AIC23B_DAC_USE_RX_CLOCK == false || \
    !defined(AIC23B_DAC_RX_CLOCK_SET_CALLBACK)
  // Set DAC frequency
  aic23b_configure_freq(master_clock, sample_rate_hz);
#endif

  aic23b_daif_t daif;
  daif.data   = AIC23B_DEFAULT(AIC23B_DAIF);
  daif.ms     = AIC23B_DAIF_MS_SLAVE;
  daif.lrswap = swap_channels;
  daif.lrp    = 0;
  daif.iwl    = (bits_per_sample <= 16) ? AIC23B_DAIF_IWL_16 :
                (bits_per_sample <= 20) ? AIC23B_DAIF_IWL_20 :
                (bits_per_sample <= 24) ? AIC23B_DAIF_IWL_24 :
                                          AIC23B_DAIF_IWL_32;
  daif.fmt    = AIC23B_DAIF_FMT_I2S;
  aic23b_write_reg(AIC23B_DAIF, daif.data);

  aic23b_output_params.num_channels             = num_channels;
  aic23b_output_params.callback                 = callback;
  aic23b_output_params.callback_opt             = callback_opt;
}
#endif

bool aic23b_dac_output(void *sample_buffer, size_t sample_length)
{
  bool global_interrupt_enabled;

  if (!(pdca_get_transfer_status(AIC23B_SSC_TX_PDCA_CHANNEL) &
        PDCA_TRANSFER_COUNTER_RELOAD_IS_ZERO))
    return false;

  if (sample_length)
  {
    if (aic23b_output_params.num_channels == 1)
    {
      int16_t *s16_sample_buffer = sample_buffer;
      int i;

      for (i = sample_length - 1; i >= 0; i--)
      {
        s16_sample_buffer[2 * i + 1] =
        s16_sample_buffer[2 * i]     = s16_sample_buffer[i];
      }
    }

    // The PDCA is not able to synchronize its start of transfer with the SSC
    // start of period, so this has to be done by polling the TF pin.
    // Not doing so may result in channels being swapped randomly.
    if ((global_interrupt_enabled = Is_global_interrupt_enabled()))
      Disable_global_interrupt();
    if (pdca_get_transfer_status(AIC23B_SSC_TX_PDCA_CHANNEL) &
        PDCA_TRANSFER_COMPLETE)
    {
      while (gpio_get_pin_value(AIC23B_SSC_TX_FRAME_SYNC_PIN));
      while (!gpio_get_pin_value(AIC23B_SSC_TX_FRAME_SYNC_PIN));
    }
    pdca_reload_channel(AIC23B_SSC_TX_PDCA_CHANNEL, sample_buffer, sample_length * 2);
    pdca_get_reload_size(AIC23B_SSC_TX_PDCA_CHANNEL);
    if (global_interrupt_enabled)
      Enable_global_interrupt();

    if (aic23b_output_params.callback_opt & AUDIO_DAC_OUT_OF_SAMPLE_CB)
      pdca_enable_interrupt_transfer_complete(AIC23B_SSC_TX_PDCA_CHANNEL);
    if (aic23b_output_params.callback_opt & AUDIO_DAC_RELOAD_CB)
      pdca_enable_interrupt_reload_counter_zero(AIC23B_SSC_TX_PDCA_CHANNEL);
  }
  return true;
}


void aic23b_dac_increase_volume(void)
{
  int8_t volume = aic23b_get_headphone_volume(AIC23B_LEFT_CHANNEL);
  if( volume < AIC23B_HP_VOL_MIN )
    volume = AIC23B_HP_VOL_MIN;
  aic23b_set_headphone_volume(AIC23B_LEFT_CHANNEL | AIC23B_RIGHT_CHANNEL,
                              volume + 1,
                              true);
}


void aic23b_dac_decrease_volume(void)
{
  int8_t volume = aic23b_get_headphone_volume(AIC23B_LEFT_CHANNEL);
  if( volume != AIC23B_MUTED )
    volume--;
  aic23b_set_headphone_volume(AIC23B_LEFT_CHANNEL | AIC23B_RIGHT_CHANNEL,
                              volume,
                              true);
}

void  aic23b_dac_mute(bool mute)
{
#if (AIC23B_MODE==AIC23B_MODE_DAC)
/*  if(mute==true) {
    pdca_disable(AIC23B_SSC_TX_PDCA_CHANNEL);
  }
  else {
       pdca_enable(AIC23B_SSC_TX_PDCA_CHANNEL);
  }
*/
  if (mute)
  {
    uint32_t save_dac_reload_callback_opt;

    // Disable the reload callback function
    save_dac_reload_callback_opt = aic23b_output_params.callback_opt;
    aic23b_output_params.callback_opt = 0;
    // Disable the transfer complete interruption and wait until the transfer is complete
    pdca_disable_interrupt_reload_counter_zero(AIC23B_SSC_TX_PDCA_CHANNEL);
    while (!(pdca_get_transfer_status(AIC23B_SSC_TX_PDCA_CHANNEL) & PDCA_TRANSFER_COMPLETE));
    // Re-enable the reload callback function
    aic23b_output_params.callback_opt = save_dac_reload_callback_opt;
  }
  else
  {
    // Re-enable the reload interrupt
    pdca_enable_interrupt_reload_counter_zero(AIC23B_SSC_TX_PDCA_CHANNEL);
  }

#endif
}

#if (AIC23B_MODE==AIC23B_MODE_DAC)
void aic23b_dac_flush(void)
{
  pdca_disable_interrupt_transfer_complete(AIC23B_SSC_TX_PDCA_CHANNEL);
  pdca_disable_interrupt_reload_counter_zero(AIC23B_SSC_TX_PDCA_CHANNEL);

  while (!(pdca_get_transfer_status(AIC23B_SSC_TX_PDCA_CHANNEL) &
           PDCA_TRANSFER_COMPLETE));
}


void aic23b_dac_stop(void)
{
  aic23b_dac_flush();

  // Disable AIC23B
  aic23b_reset();

  aic23b_pdc_t pdc;
  pdc.data  = AIC23B_DEFAULT(AIC23B_PDC);
  pdc.off   = 1;
  pdc.clk   = 1;
  pdc.osc   = 1;
  pdc.out   = 1;
  pdc.dac   = 1;
  pdc.adc   = 1;
  pdc.mic   = 1;
  pdc.line  = 1;
  aic23b_set_power_down_state(pdc);

  // Stop  PDCA
  pdca_disable(AIC23B_SSC_TX_PDCA_CHANNEL);

  ssc_i2s_reset(AIC23B_SSC);

  // Set used GPIO pins to GPIO state
  gpio_enable_gpio(AIC23B_SSC_DAC_GPIO_MAP,
                   sizeof(AIC23B_SSC_DAC_GPIO_MAP) / sizeof(AIC23B_SSC_DAC_GPIO_MAP[0]));

  aic23b_output_params.num_channels             = 0;
  aic23b_output_params.callback                 = NULL;
  aic23b_output_params.callback_opt             = 0;
}
#endif

#if (AIC23B_MODE==AIC23B_MODE_ADC)
void aic23b_adc_start(uint32_t sample_rate_hz,
                      uint8_t num_channels,
                      uint8_t bits_per_sample,
                      bool swap_channels,
                      void (*callback)(uint32_t arg),
                      uint32_t callback_opt,
                      uint32_t pba_hz)
{
#if AIC23B_CTRL_INTERFACE == AIC23B_CTRL_INTERFACE_SPI
  static const spi_options_t AIC23B_SPI_OPTIONS =
  {
    .reg          = AIC23B_SPI_NPCS,
    .baudrate     = AIC23B_SPI_MASTER_SPEED,
    .bits         = AIC23B_CTRL_SIZE,
    .spck_delay   = 0,
    .trans_delay  = 0,
    .stay_act     = 0,
    .spi_mode     = 3,
    .modfdis      = 1
  };
  spi_setupChipReg(AIC23B_SPI, &AIC23B_SPI_OPTIONS, pba_hz);
#endif

  aic23b_adc_stop();

  gpio_enable_module(AIC23B_SSC_ADC_GPIO_MAP,
                     sizeof(AIC23B_SSC_ADC_GPIO_MAP) / sizeof(AIC23B_SSC_ADC_GPIO_MAP[0]));

  aic23b_pdc_t pdc;
  pdc.data  = AIC23B_DEFAULT(AIC23B_PDC);
  pdc.off   = 0;
  pdc.clk   = 0;
  pdc.osc   = 0;
  pdc.out   = 0;
  pdc.dac   = 1;
  pdc.adc   = 0;
#if (AIC23B_INPUT==AIC23B_INPUT_LINE)
  pdc.mic   = 1;
  pdc.line  = 0;
#elif (AIC23B_INPUT==AIC23B_INPUT_MIC)
  pdc.mic   = 0;
  pdc.line  = 1;
#else
  #error No Input defined in file 'conf_tlv320aic23b.h'
#endif
  aic23b_set_power_down_state(pdc);

  aic23b_adc_setup(sample_rate_hz,
                   num_channels,
                   bits_per_sample,
                   swap_channels,
                   callback,
                   callback_opt,
                   pba_hz);

  aic23b_aapc_t aapc;
  aapc.data  = AIC23B_DEFAULT(AIC23B_AAPC);
#if (AIC23B_INPUT==AIC23B_INPUT_LINE)
  aapc.ste   = 0;
  aapc.dac   = 0;
  aapc.byp   = 0;
  aapc.insel = 0;
  aapc.micm  = 0;
  aapc.micb  = 0;
#elif (AIC23B_INPUT==AIC23B_INPUT_MIC)
  aapc.ste   = 0;
  aapc.dac   = 0;
  aapc.byp   = 0;
  aapc.insel = 1;
  aapc.micm  = 0;
  aapc.micb  = 0;
#else
  #error No Input defined in file 'conf_tlv320aic23b.h'
#endif
  aic23b_set_analog_audio_path(aapc);

  aic23b_dapc_t dapc;
  dapc.data   = AIC23B_DEFAULT(AIC23B_DAPC);
  dapc.dacm   = 0;
  dapc.deemp  = AIC23B_DAPC_DEEMP_NONE;
  dapc.adchp  = 0;
  aic23b_set_digital_audio_path(dapc);


  aic23b_llicvc_t llivc;
  llivc.data  = AIC23B_DEFAULT(AIC23B_LLICVC);
  llivc.liv   = 20;
  llivc.lim   = 0;
  llivc.lrs   = 1;
  aic23b_write_reg(AIC23B_LLICVC, llivc.data);

  aic23b_rlicvc_t rlivc;
  rlivc.data  = AIC23B_DEFAULT(AIC23B_RLICVC);
  rlivc.riv   = 20;
  rlivc.rim   = 0;
  rlivc.rls   = 1;
  aic23b_write_reg(AIC23B_RLICVC, rlivc.data);

  INTC_register_interrupt(&aic23b_ssc_rx_pdca_int_handler,
                          AIC23B_SSC_RX_PDCA_IRQ,
                          AIC23B_SSC_RX_PDCA_INT_LEVEL);

  aic23b_activate_dig_audio(true);

}

void aic23b_adc_setup(uint32_t sample_rate_hz,
                      uint8_t num_channels,
                      uint8_t bits_per_sample,
                      bool swap_channels,
                      void (*callback)(uint32_t arg),
                      callback_opt,
                      uint32_t pba_hz)
{
  ssc_i2s_init(AIC23B_SSC,
               sample_rate_hz,
               bits_per_sample,
               (bits_per_sample <= 16) ? 16 :
               (bits_per_sample <= 20) ? 20 :
               (bits_per_sample <= 24) ? 24 :
                                         32,
               SSC_I2S_MODE_STEREO_IN,
               pba_hz);

  pdca_channel_options_t aic23b_ssc_pdca_options =
  {
    .addr           = NULL,
    .size           = 0,
    .r_addr         = NULL,
    .r_size         = 0,
    .pid            = AIC23B_SSC_RX_PDCA_PID,
    .transfer_size  = (bits_per_sample <=  8) ? PDCA_TRANSFER_SIZE_BYTE      :
                      (bits_per_sample <= 16) ? PDCA_TRANSFER_SIZE_HALF_WORD :
                                                PDCA_TRANSFER_SIZE_WORD
  };
  pdca_init_channel(AIC23B_SSC_RX_PDCA_CHANNEL, &aic23b_ssc_pdca_options);
  pdca_enable(AIC23B_SSC_RX_PDCA_CHANNEL);

  // Set ADC frequency
  aic23b_configure_freq(AIC23B_MCLK_HZ, sample_rate_hz);

  aic23b_daif_t daif;
  daif.data   = AIC23B_DEFAULT(AIC23B_DAIF);
  daif.ms     = AIC23B_DAIF_MS_SLAVE;
  daif.lrswap = swap_channels;
  daif.lrp    = 0;
  daif.iwl    = (bits_per_sample <= 16) ? AIC23B_DAIF_IWL_16 :
                (bits_per_sample <= 20) ? AIC23B_DAIF_IWL_20 :
                (bits_per_sample <= 24) ? AIC23B_DAIF_IWL_24 :
                                          AIC23B_DAIF_IWL_32;
  daif.fmt    = AIC23B_DAIF_FMT_I2S;
  aic23b_write_reg(AIC23B_DAIF, daif.data);

  aic23b_output_params.num_channels              = num_channels;
  aic23b_output_params.callback                  = callback;
  aic23b_output_params.callback_opt              = callback_opt;
}

void aic23b_adc_flush(void)
{
  pdca_disable_interrupt_transfer_complete(AIC23B_SSC_RX_PDCA_CHANNEL);

  while (!(pdca_get_transfer_status(AIC23B_SSC_RX_PDCA_CHANNEL) &
           PDCA_TRANSFER_COMPLETE));
}


void aic23b_adc_stop(void)
{
  aic23b_adc_flush();

  aic23b_reset();

  aic23b_pdc_t pdc;
  pdc.data  = AIC23B_DEFAULT(AIC23B_PDC);
  pdc.off   = 1;
  pdc.clk   = 1;
  pdc.osc   = 1;
  pdc.out   = 1;
  pdc.dac   = 1;
  pdc.adc   = 1;
  pdc.mic   = 1;
  pdc.line  = 1;
  aic23b_set_power_down_state(pdc);

  pdca_disable(AIC23B_SSC_RX_PDCA_CHANNEL);
  ssc_i2s_reset(AIC23B_SSC);

  gpio_enable_gpio(AIC23B_SSC_ADC_GPIO_MAP,
                   sizeof(AIC23B_SSC_ADC_GPIO_MAP) / sizeof(AIC23B_SSC_ADC_GPIO_MAP[0]));

  aic23b_output_params.num_channels             = 0;
  aic23b_output_params.callback                 = NULL;
  aic23b_output_params.callback_opt             = 0;
}
#endif

#if (AIC23B_MODE==AIC23B_MODE_ADC)||(AIC23B_MODE==AIC23B_MODE_CODEC)
bool aic23b_adc_input(void *sample_buffer, size_t sample_length)
{
  if (!(pdca_get_transfer_status(AIC23B_SSC_RX_PDCA_CHANNEL) &
        PDCA_TRANSFER_COUNTER_RELOAD_IS_ZERO))
    return false;

  if (sample_length)
  {
    pdca_reload_channel(AIC23B_SSC_RX_PDCA_CHANNEL, sample_buffer, sample_length * 2);
    pdca_get_reload_size(AIC23B_SSC_RX_PDCA_CHANNEL);

    if (aic23b_output_params.callback_opt & AUDIO_ADC_OUT_OF_SAMPLE_CB)
      pdca_enable_interrupt_transfer_complete(AIC23B_SSC_RX_PDCA_CHANNEL);
    if (aic23b_output_params.callback_opt & AUDIO_ADC_RELOAD_CB)
      pdca_enable_interrupt_reload_counter_zero(AIC23B_SSC_RX_PDCA_CHANNEL);
  }

  return true;
}
#endif

#if (AIC23B_MODE==AIC23B_MODE_CODEC)
void aic23b_codec_start(uint32_t sample_rate_hz,
                      uint8_t num_channels,
                      uint8_t bits_per_sample,
                      bool swap_channels,
                      void (*callback)(uint32_t arg),
                      uint32_t callback_opt,
                      uint32_t pba_hz)
{
#if AIC23B_CTRL_INTERFACE == AIC23B_CTRL_INTERFACE_SPI
  static const spi_options_t AIC23B_SPI_OPTIONS =
  {
    .reg          = AIC23B_SPI_NPCS,
    .baudrate     = AIC23B_SPI_MASTER_SPEED,
    .bits         = AIC23B_CTRL_SIZE,
    .spck_delay   = 0,
    .trans_delay  = 0,
    .stay_act     = 0,
    .spi_mode     = 3,
    .modfdis      = 1
  };
  spi_setupChipReg(AIC23B_SPI, &AIC23B_SPI_OPTIONS, pba_hz);
#endif

  aic23b_codec_stop();

  gpio_enable_module(AIC23B_SSC_CODEC_GPIO_MAP,
                     sizeof(AIC23B_SSC_CODEC_GPIO_MAP) / sizeof(AIC23B_SSC_CODEC_GPIO_MAP[0]));

  aic23b_pdc_t pdc;
  pdc.data  = AIC23B_DEFAULT(AIC23B_PDC);
  pdc.off   = 0;
  pdc.clk   = 0;
  pdc.osc   = 0;
  pdc.out   = 0;
  pdc.dac   = 0;
  pdc.adc   = 0;
#if (AIC23B_INPUT==AIC23B_INPUT_LINE)
  pdc.mic   = 1;
  pdc.line  = 0;
#elif (AIC23B_INPUT==AIC23B_INPUT_MIC)
  pdc.mic   = 0;
  pdc.line  = 1;
#else
  #error No Input defined in file 'conf_tlv320aic23b.h'
#endif
  aic23b_set_power_down_state(pdc);

  aic23b_codec_setup(sample_rate_hz,
                   num_channels,
                   bits_per_sample,
                   swap_channels,
                   callback,
                   callback_opt,
                   pba_hz);

  aic23b_aapc_t aapc;
  aapc.data  = AIC23B_DEFAULT(AIC23B_AAPC);
#if (AIC23B_INPUT==AIC23B_INPUT_LINE)
  aapc.ste   = 0;
  aapc.dac   = 1;
  aapc.byp   = 0;
  aapc.insel = 0;
  aapc.micm  = 0;
  aapc.micb  = 1;
#elif (AIC23B_INPUT==AIC23B_INPUT_MIC)
  aapc.ste   = 0;
  aapc.dac   = 1;
  aapc.sta   = 4;
  aapc.byp   = 0;
  aapc.insel = 1;
  aapc.micm  = 0;
  aapc.micb  = 1;
#else
  #error No Input defined in file 'conf_tlv320aic23b.h'
#endif
  aic23b_set_analog_audio_path(aapc);

  aic23b_dapc_t dapc;
  dapc.data   = AIC23B_DEFAULT(AIC23B_DAPC);
  dapc.dacm   = 0;
  dapc.deemp  = AIC23B_DAPC_DEEMP_NONE;
  dapc.adchp  = 0;
  aic23b_set_digital_audio_path(dapc);


  aic23b_llicvc_t llivc;
  llivc.data  = AIC23B_DEFAULT(AIC23B_LLICVC);
  llivc.liv   = 20;
  llivc.lim   = 0;
  llivc.lrs   = 1;
  aic23b_write_reg(AIC23B_LLICVC, llivc.data);

  aic23b_rlicvc_t rlivc;
  rlivc.data  = AIC23B_DEFAULT(AIC23B_RLICVC);
  rlivc.riv   = 20;
  rlivc.rim   = 0;
  rlivc.rls   = 1;
  aic23b_write_reg(AIC23B_RLICVC, rlivc.data);

  INTC_register_interrupt(&aic23b_ssc_rx_pdca_int_handler,
                          AIC23B_SSC_RX_PDCA_IRQ,
                          AIC23B_SSC_RX_PDCA_INT_LEVEL);

  // set an acceptable start volume
  aic23b_set_headphone_volume(AIC23B_LEFT_CHANNEL | AIC23B_RIGHT_CHANNEL,
                              -30,
                              true);

  aic23b_activate_dig_audio(true);

  INTC_register_interrupt(&aic23b_ssc_tx_pdca_int_handler,
                          AIC23B_SSC_TX_PDCA_IRQ,
                          AIC23B_SSC_TX_PDCA_INT_LEVEL);
}

void aic23b_codec_setup(uint32_t sample_rate_hz,
                      uint8_t num_channels,
                      uint8_t bits_per_sample,
                      bool swap_channels,
                      void (*callback)(uint32_t opt),
                      uint32_t callback_opt,
                      uint32_t pba_hz)
{
  uint32_t master_clock = AIC23B_MCLK_HZ; // default configuration

  // Change the CPU frequency
  //
  //Disable_global_interrupt();

  // Switch to OSC0 during OSC1 transition
  //pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);

  // Switch to PLL0 as the master clock
  //pm_switch_to_clock(&AVR32_PM, AVR32_PM_MCCTRL_MCSEL_PLL0);

  if (sample_rate_hz < (8000 + 8021) / 2)
  { // 8000 Hz
  }
  else if (sample_rate_hz < (8021 + 32000) / 2)
  { // 8021 Hz
  }
  else if (sample_rate_hz < (32000 + 44100) / 2)
  { // 32000 Hz
    master_clock = usb_stream_resync_frequency = 8192000;
    cs2200_freq_clk_out(_32_BITS_RATIO(usb_stream_resync_frequency, FOSC0));
    pba_hz = FCPU_HZ = FHSB_HZ = FPBA_HZ = FPBB_HZ = FMCK_HZ(8192000);
  }
  else if (sample_rate_hz < (44100 + 48000) / 2)
  { // 44100 Hz
    master_clock = usb_stream_resync_frequency = 11289600;
    cs2200_freq_clk_out(_32_BITS_RATIO(usb_stream_resync_frequency, FOSC0));
    pba_hz = FCPU_HZ = FHSB_HZ = FPBA_HZ = FPBB_HZ = FMCK_HZ(11289600);
  }
  else if (sample_rate_hz < (48000 + 88200) / 2)
  { // 48000 Hz
    master_clock = usb_stream_resync_frequency = 12288000;
    cs2200_freq_clk_out(_32_BITS_RATIO(usb_stream_resync_frequency, FOSC0));
    pba_hz = FCPU_HZ = FHSB_HZ = FPBA_HZ = FPBB_HZ = FMCK_HZ(12288000);
  }
  else if (sample_rate_hz < (88200 + 96000) / 2)
  { // 88200 Hz
  }
  else
  { // 96000 Hz
  }

  //Enable_global_interrupt();

  ssc_i2s_init(AIC23B_SSC,
               sample_rate_hz,
               bits_per_sample,
               (bits_per_sample <= 16) ? 16 :
               (bits_per_sample <= 20) ? 20 :
               (bits_per_sample <= 24) ? 24 :
                                         32,
               SSC_I2S_MODE_STEREO_OUT_STEREO_IN,
               pba_hz);

  pdca_channel_options_t aic23b_ssc_pdca_options_rx =
  {
    .addr           = NULL,
    .size           = 0,
    .r_addr         = NULL,
    .r_size         = 0,
    .pid            = AIC23B_SSC_RX_PDCA_PID,
    .transfer_size  = (bits_per_sample <=  8) ? PDCA_TRANSFER_SIZE_BYTE      :
                      (bits_per_sample <= 16) ? PDCA_TRANSFER_SIZE_HALF_WORD :
                                                PDCA_TRANSFER_SIZE_WORD

  };
  pdca_init_channel(AIC23B_SSC_RX_PDCA_CHANNEL, &aic23b_ssc_pdca_options_rx);
  pdca_enable(AIC23B_SSC_RX_PDCA_CHANNEL);

  pdca_channel_options_t aic23b_ssc_pdca_options_tx =
  {
    .addr           = NULL,
    .size           = 0,
    .r_addr         = NULL,
    .r_size         = 0,
    .pid            = AIC23B_SSC_TX_PDCA_PID,
    .transfer_size  = (bits_per_sample <=  8) ? PDCA_TRANSFER_SIZE_BYTE      :
                      (bits_per_sample <= 16) ? PDCA_TRANSFER_SIZE_HALF_WORD :
                                                PDCA_TRANSFER_SIZE_WORD
  };
  pdca_init_channel(AIC23B_SSC_TX_PDCA_CHANNEL, &aic23b_ssc_pdca_options_tx);
  pdca_enable(AIC23B_SSC_TX_PDCA_CHANNEL);

  // Set codec frequency
  aic23b_configure_freq(master_clock, sample_rate_hz);

  aic23b_daif_t daif;
  daif.data   = AIC23B_DEFAULT(AIC23B_DAIF);
  daif.ms     = AIC23B_DAIF_MS_SLAVE;
  daif.lrswap = swap_channels;
  daif.lrp    = 0;
  daif.iwl    = (bits_per_sample <= 16) ? AIC23B_DAIF_IWL_16 :
                (bits_per_sample <= 20) ? AIC23B_DAIF_IWL_20 :
                (bits_per_sample <= 24) ? AIC23B_DAIF_IWL_24 :
                                          AIC23B_DAIF_IWL_32;
  daif.fmt    = AIC23B_DAIF_FMT_I2S;
  aic23b_write_reg(AIC23B_DAIF, daif.data);

  aic23b_output_params.num_channels              = num_channels;
  aic23b_output_params.callback                  = callback;
  aic23b_output_params.callback_opt              = callback_opt;
}

void aic23b_codec_flush(void)
{
  pdca_disable_interrupt_transfer_complete(AIC23B_SSC_RX_PDCA_CHANNEL);
  while (!(pdca_get_transfer_status(AIC23B_SSC_RX_PDCA_CHANNEL) &
           PDCA_TRANSFER_COMPLETE));
  pdca_disable_interrupt_transfer_complete(AIC23B_SSC_TX_PDCA_CHANNEL);
  while (!(pdca_get_transfer_status(AIC23B_SSC_TX_PDCA_CHANNEL) &
           PDCA_TRANSFER_COMPLETE));
}

void aic23b_codec_stop(void)
{
  aic23b_codec_flush();

  aic23b_reset();

  aic23b_pdc_t pdc;
  pdc.data  = AIC23B_DEFAULT(AIC23B_PDC);
  pdc.off   = 1;
  pdc.clk   = 1;
  pdc.osc   = 1;
  pdc.out   = 1;
  pdc.dac   = 1;
  pdc.adc   = 1;
  pdc.mic   = 1;
  pdc.line  = 1;
  aic23b_set_power_down_state(pdc);

  pdca_disable(AIC23B_SSC_RX_PDCA_CHANNEL);
  pdca_disable(AIC23B_SSC_TX_PDCA_CHANNEL);

  ssc_i2s_reset(AIC23B_SSC);

  gpio_enable_gpio(AIC23B_SSC_CODEC_GPIO_MAP,
                   sizeof(AIC23B_SSC_CODEC_GPIO_MAP) / sizeof(AIC23B_SSC_CODEC_GPIO_MAP[0]));

  aic23b_output_params.num_channels             = 0;
  aic23b_output_params.callback                 = NULL;
  aic23b_output_params.callback_opt             = 0;
}
#endif

//! @}
