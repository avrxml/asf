/*****************************************************************************
 *
 * \file
 *
 * \brief TLV320AIC23B stereo audio CODEC driver for AVR32.
 *
 *                       can be used.
 *
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#ifndef _TLV320AIC23B_H_
#define _TLV320AIC23B_H_

/**
 * \defgroup group_avr32_components_audio_codec_tlv320aic23b AUDIO - TLV320AIC23B Audio Codec
 *
 * Software Driver for the TLV320AIC23B Audio Codec
 *
 * \{
 */

#include <limits.h>
#include <stddef.h>

#include "compiler.h"


/*! \name Control Interface
 */
//! @{

#define AIC23B_CTRL_INTERFACE_TWI   1       //!< Control interface: Two-Wire Interface.
#define AIC23B_CTRL_INTERFACE_SPI   2       //!< Control interface: Serial Peripheral Interface.

//! @}

/*! \name Mode Supported
 */
//! @{

#define AIC23B_MODE_DAC             0       //!< Mode supported: DAC
#define AIC23B_MODE_ADC             1       //!< Mode supported: ADC
#define AIC23B_MODE_CODEC           3       //!< Mode supported: CODEC

//! @}

/*! \name Input Selected
 */
//! @{

#define AIC23B_INPUT_LINE           0       //!< Input Selected: LINE IN
#define AIC23B_INPUT_MIC            1       //!< Input Selected: MIC

//! @}

#include "conf_tlv320aic23b.h"


/*! \name Control Word
 */
//! @{

#define AIC23B_CTRL_SIZE            (AIC23B_CTRL_ADDR_SIZE + AIC23B_CTRL_DATA_SIZE)
#define AIC23B_CTRL_ADDR_OFFSET     9       //!< Control address bit-field offset.
#define AIC23B_CTRL_ADDR_SIZE       7       //!< Control address bit-field size.
#define AIC23B_CTRL_DATA_OFFSET     0       //!< Control data bit-field offset.
#define AIC23B_CTRL_DATA_SIZE       9       //!< Control data bit-field size.

//! Register structure.
typedef struct
{
  const uint16_t addr    : AIC23B_CTRL_ADDR_SIZE;    //!< Control address.
        uint16_t data    : AIC23B_CTRL_DATA_SIZE;    //!< Control data.
} aic23b_ctrl_t;

//! @}


/*! \name Left Line Input Channel Volume Control
 */
//! @{

#define AIC23B_LLICVC               0x00    //!< Register index.
#define AIC23B_LLICVC_ADDRESS       0x00    //!< Register address.
#define AIC23B_LLICVC_DEFAULT       0x0097  //!< Register default value.
#define AIC23B_LLICVC_LRS_OFFSET    8       //!< Left/right line simultaneous volume/mute update bit-field offset.
#define AIC23B_LLICVC_LRS_SIZE      1       //!< Left/right line simultaneous volume/mute update bit-field size.
#define AIC23B_LLICVC_LIM_OFFSET    7       //!< Left line input mute bit-field offset.
#define AIC23B_LLICVC_LIM_SIZE      1       //!< Left line input mute bit-field size.
#define AIC23B_LLICVC_LIV_OFFSET    0       //!< Left line input volume control bit-field offset.
#define AIC23B_LLICVC_LIV_SIZE      5       //!< Left line input volume control bit-field size.

//! Register structure.
typedef union
{
  struct
  {
    const uint16_t addr    : AIC23B_CTRL_ADDR_SIZE;    //!< Control address.
          uint16_t data    : AIC23B_CTRL_DATA_SIZE;    //!< Control data.
  };
  struct
  {
    const uint16_t         : AIC23B_CTRL_ADDR_SIZE;
          uint16_t lrs     : AIC23B_LLICVC_LRS_SIZE;   //!< Left/right line simultaneous volume/mute update.
          uint16_t lim     : AIC23B_LLICVC_LIM_SIZE;   //!< Left line input mute.
          uint16_t         : 2;
          uint16_t liv     : AIC23B_LLICVC_LIV_SIZE;   //!< Left line input volume.
  };
} aic23b_llicvc_t;

//! @}


/*! \name Right Line Input Channel Volume Control
 */
//! @{

#define AIC23B_RLICVC               0x01    //!< Register index.
#define AIC23B_RLICVC_ADDRESS       0x01    //!< Register address.
#define AIC23B_RLICVC_DEFAULT       0x0097  //!< Register default value.
#define AIC23B_RLICVC_RLS_OFFSET    8       //!< Right/left line simultaneous volume/mute update bit-field offset.
#define AIC23B_RLICVC_RLS_SIZE      1       //!< Right/left line simultaneous volume/mute update bit-field size.
#define AIC23B_RLICVC_RIM_OFFSET    7       //!< Right line input mute bit-field offset.
#define AIC23B_RLICVC_RIM_SIZE      1       //!< Right line input mute bit-field size.
#define AIC23B_RLICVC_RIV_OFFSET    0       //!< Right line input volume control bit-field offset.
#define AIC23B_RLICVC_RIV_SIZE      5       //!< Right line input volume control bit-field size.

//! Register structure.
typedef union
{
  struct
  {
    const uint16_t addr    : AIC23B_CTRL_ADDR_SIZE;    //!< Control address.
          uint16_t data    : AIC23B_CTRL_DATA_SIZE;    //!< Control data.
  };
  struct
  {
    const uint16_t         : AIC23B_CTRL_ADDR_SIZE;
          uint16_t rls     : AIC23B_RLICVC_RLS_SIZE;   //!< Right/left line simultaneous volume/mute update.
          uint16_t rim     : AIC23B_RLICVC_RIM_SIZE;   //!< Right line input mute.
          uint16_t         : 2;
          uint16_t riv     : AIC23B_RLICVC_RIV_SIZE;   //!< Right line input volume.
  };
} aic23b_rlicvc_t;

//! @}


/*! \name Left Channel Headphone Volume Control
 */
//! @{

#define AIC23B_LCHVC                0x02    //!< Register index.
#define AIC23B_LCHVC_ADDRESS        0x02    //!< Register address.
#define AIC23B_LCHVC_DEFAULT        0x00F9  //!< Register default value.
#define AIC23B_LCHVC_LRS_OFFSET     8       //!< Left/right headphone channel simultaneous volume/mute update bit-field offset.
#define AIC23B_LCHVC_LRS_SIZE       1       //!< Left/right headphone channel simultaneous volume/mute update bit-field size.
#define AIC23B_LCHVC_LZC_OFFSET     7       //!< Left-channel zero-cross detect bit-field offset.
#define AIC23B_LCHVC_LZC_SIZE       1       //!< Left-channel zero-cross detect bit-field size.
#define AIC23B_LCHVC_LHV_OFFSET     0       //!< Left headphone volume control bit-field offset.
#define AIC23B_LCHVC_LHV_SIZE       7       //!< Left headphone volume control bit-field size.

//! Register structure.
typedef union
{
  struct
  {
    const uint16_t addr    : AIC23B_CTRL_ADDR_SIZE;    //!< Control address.
          uint16_t data    : AIC23B_CTRL_DATA_SIZE;    //!< Control data.
  };
  struct
  {
    const uint16_t         : AIC23B_CTRL_ADDR_SIZE;
          uint16_t lrs     : AIC23B_LCHVC_LRS_SIZE;    //!< Left/right headphone channel simultaneous volume/mute update.
          uint16_t lzc     : AIC23B_LCHVC_LZC_SIZE;    //!< Left-channel zero-cross detect.
          uint16_t lhv     : AIC23B_LCHVC_LHV_SIZE;    //!< Left headphone volume control.
  };
} aic23b_lchvc_t;

//! @}


/*! \name Right Channel Headphone Volume Control
 */
//! @{

#define AIC23B_RCHVC                0x03    //!< Register index.
#define AIC23B_RCHVC_ADDRESS        0x03    //!< Register address.
#define AIC23B_RCHVC_DEFAULT        0x00F9  //!< Register default value.
#define AIC23B_RCHVC_RLS_OFFSET     8       //!< Right/left headphone channel simultaneous volume/mute update bit-field offset.
#define AIC23B_RCHVC_RLS_SIZE       1       //!< Right/left headphone channel simultaneous volume/mute update bit-field size.
#define AIC23B_RCHVC_RZC_OFFSET     7       //!< Right-channel zero-cross detect bit-field offset.
#define AIC23B_RCHVC_RZC_SIZE       1       //!< Right-channel zero-cross detect bit-field size.
#define AIC23B_RCHVC_RHV_OFFSET     0       //!< Right headphone volume control bit-field offset.
#define AIC23B_RCHVC_RHV_SIZE       7       //!< Right headphone volume control bit-field size.

//! Register structure.
typedef union
{
  struct
  {
    const uint16_t addr    : AIC23B_CTRL_ADDR_SIZE;    //!< Control address.
          uint16_t data    : AIC23B_CTRL_DATA_SIZE;    //!< Control data.
  };
  struct
  {
    const uint16_t         : AIC23B_CTRL_ADDR_SIZE;
          uint16_t rls     : AIC23B_RCHVC_RLS_SIZE;    //!< Right/left headphone channel simultaneous volume/mute update.
          uint16_t rzc     : AIC23B_RCHVC_RZC_SIZE;    //!< Right-channel zero-cross detect.
          uint16_t rhv     : AIC23B_RCHVC_RHV_SIZE;    //!< Right headphone volume control.
  };
} aic23b_rchvc_t;

//! @}


/*! \name Analog Audio Path Control
 */
//! @{

#define AIC23B_AAPC                 0x04    //!< Register index.
#define AIC23B_AAPC_ADDRESS         0x04    //!< Register address.
#define AIC23B_AAPC_DEFAULT         0x000A  //!< Register default value.
#define AIC23B_AAPC_STA_OFFSET      6       //!< Sidetone addition bit-field offset.
#define AIC23B_AAPC_STA_SIZE        3       //!< Sidetone addition bit-field size.
#define AIC23B_AAPC_STA_M18DB       0x0003  //!< Added sidetone: -18 dB.
#define AIC23B_AAPC_STA_M12DB       0x0002  //!< Added sidetone: -12 dB.
#define AIC23B_AAPC_STA_M9DB        0x0001  //!< Added sidetone: -9 dB.
#define AIC23B_AAPC_STA_M6DB        0x0000  //!< Added sidetone: -6 dB.
#define AIC23B_AAPC_STA_0DB         0x0004  //!< Added sidetone: 0 dB.
#define AIC23B_AAPC_STE_OFFSET      5       //!< Sidetone enable bit-field offset.
#define AIC23B_AAPC_STE_SIZE        1       //!< Sidetone enable bit-field size.
#define AIC23B_AAPC_DAC_OFFSET      4       //!< DAC select bit-field offset.
#define AIC23B_AAPC_DAC_SIZE        1       //!< DAC select bit-field size.
#define AIC23B_AAPC_BYP_OFFSET      3       //!< Bypass bit-field offset.
#define AIC23B_AAPC_BYP_SIZE        1       //!< Bypass bit-field size.
#define AIC23B_AAPC_INSEL_OFFSET    2       //!< Input select for ADC bit-field offset.
#define AIC23B_AAPC_INSEL_SIZE      1       //!< Input select for ADC bit-field size.
#define AIC23B_AAPC_INSEL_LINE      0x0000  //!< Input selected for ADC: line.
#define AIC23B_AAPC_INSEL_MIC       0x0001  //!< Input selected for ADC: microphone.
#define AIC23B_AAPC_MICM_OFFSET     1       //!< Microphone mute bit-field offset.
#define AIC23B_AAPC_MICM_SIZE       1       //!< Microphone mute bit-field size.
#define AIC23B_AAPC_MICB_OFFSET     0       //!< Microphone boost (+20 dB) bit-field offset.
#define AIC23B_AAPC_MICB_SIZE       1       //!< Microphone boost (+20 dB) bit-field size.

//! Register structure.
typedef union
{
  struct
  {
    const uint16_t addr    : AIC23B_CTRL_ADDR_SIZE;    //!< Control address.
          uint16_t data    : AIC23B_CTRL_DATA_SIZE;    //!< Control data.
  };
  struct
  {
    const uint16_t         : AIC23B_CTRL_ADDR_SIZE;
          uint16_t sta     : AIC23B_AAPC_STA_SIZE;     //!< Sidetone addition.
          uint16_t ste     : AIC23B_AAPC_STE_SIZE;     //!< Sidetone enable.
          uint16_t dac     : AIC23B_AAPC_DAC_SIZE;     //!< DAC select.
          uint16_t byp     : AIC23B_AAPC_BYP_SIZE;     //!< Bypass.
          uint16_t insel   : AIC23B_AAPC_INSEL_SIZE;   //!< Input select for ADC.
          uint16_t micm    : AIC23B_AAPC_MICM_SIZE;    //!< Microphone mute.
          uint16_t micb    : AIC23B_AAPC_MICB_SIZE;    //!< Microphone boost.
  };
} aic23b_aapc_t;

//! @}


/*! \name Digital Audio Path Control
 */
//! @{

#define AIC23B_DAPC                 0x05    //!< Register index.
#define AIC23B_DAPC_ADDRESS         0x05    //!< Register address.
#define AIC23B_DAPC_DEFAULT         0x0008  //!< Register default value.
#define AIC23B_DAPC_DACM_OFFSET     3       //!< DAC soft mute bit-field offset.
#define AIC23B_DAPC_DACM_SIZE       1       //!< DAC soft mute bit-field size.
#define AIC23B_DAPC_DEEMP_OFFSET    1       //!< De-emphasis control bit-field offset.
#define AIC23B_DAPC_DEEMP_SIZE      2       //!< De-emphasis control bit-field size.
#define AIC23B_DAPC_DEEMP_NONE      0x0000  //!< De-emphasis: none.
#define AIC23B_DAPC_DEEMP_32KHZ     0x0001  //!< De-emphasis: 32 kHz.
#define AIC23B_DAPC_DEEMP_44_1KHZ   0x0002  //!< De-emphasis: 44.1 kHz.
#define AIC23B_DAPC_DEEMP_48KHZ     0x0003  //!< De-emphasis: 48 kHz.
#define AIC23B_DAPC_ADCHP_OFFSET    0       //!< ADC high-pass filter bit-field offset.
#define AIC23B_DAPC_ADCHP_SIZE      1       //!< ADC high-pass filter bit-field size.

//! Register structure.
typedef union
{
  struct
  {
    const uint16_t addr    : AIC23B_CTRL_ADDR_SIZE;    //!< Control address.
          uint16_t data    : AIC23B_CTRL_DATA_SIZE;    //!< Control data.
  };
  struct
  {
    const uint16_t         : AIC23B_CTRL_ADDR_SIZE;
          uint16_t         : 5;
          uint16_t dacm    : AIC23B_DAPC_DACM_SIZE;    //!< DAC soft mute.
          uint16_t deemp   : AIC23B_DAPC_DEEMP_SIZE;   //!< De-emphasis control.
          uint16_t adchp   : AIC23B_DAPC_ADCHP_SIZE;   //!< ADC high-pass filter.
  };
} aic23b_dapc_t;

//! @}


/*! \name Power Down Control
 */
//! @{

#define AIC23B_PDC                  0x06    //!< Register index.
#define AIC23B_PDC_ADDRESS          0x06    //!< Register address.
#define AIC23B_PDC_DEFAULT          0x0007  //!< Register default value.
#define AIC23B_PDC_OFF_OFFSET       7       //!< Device power bit-field offset.
#define AIC23B_PDC_OFF_SIZE         1       //!< Device power bit-field size.
#define AIC23B_PDC_CLK_OFFSET       6       //!< Clock bit-field offset.
#define AIC23B_PDC_CLK_SIZE         1       //!< Clock bit-field size.
#define AIC23B_PDC_OSC_OFFSET       5       //!< Oscillator bit-field offset.
#define AIC23B_PDC_OSC_SIZE         1       //!< Oscillator bit-field size.
#define AIC23B_PDC_OUT_OFFSET       4       //!< Outputs bit-field offset.
#define AIC23B_PDC_OUT_SIZE         1       //!< Outputs bit-field size.
#define AIC23B_PDC_DAC_OFFSET       3       //!< DAC bit-field offset.
#define AIC23B_PDC_DAC_SIZE         1       //!< DAC bit-field size.
#define AIC23B_PDC_ADC_OFFSET       2       //!< ADC bit-field offset.
#define AIC23B_PDC_ADC_SIZE         1       //!< ADC bit-field size.
#define AIC23B_PDC_MIC_OFFSET       1       //!< Microphone input bit-field offset.
#define AIC23B_PDC_MIC_SIZE         1       //!< Microphone input bit-field size.
#define AIC23B_PDC_LINE_OFFSET      0       //!< Line input bit-field offset.
#define AIC23B_PDC_LINE_SIZE        1       //!< Line input bit-field size.

//! Register structure.
typedef union
{
  struct
  {
    const uint16_t addr    : AIC23B_CTRL_ADDR_SIZE;    //!< Control address.
          uint16_t data    : AIC23B_CTRL_DATA_SIZE;    //!< Control data.
  };
  struct
  {
    const uint16_t         : AIC23B_CTRL_ADDR_SIZE;
          uint16_t         : 1;
          uint16_t off     : AIC23B_PDC_OFF_SIZE;      //!< Device power.
          uint16_t clk     : AIC23B_PDC_CLK_SIZE;      //!< Clock.
          uint16_t osc     : AIC23B_PDC_OSC_SIZE;      //!< Oscillator.
          uint16_t out     : AIC23B_PDC_OUT_SIZE;      //!< Outputs.
          uint16_t dac     : AIC23B_PDC_DAC_SIZE;      //!< DAC.
          uint16_t adc     : AIC23B_PDC_ADC_SIZE;      //!< ADC.
          uint16_t mic     : AIC23B_PDC_MIC_SIZE;      //!< Microphone input.
          uint16_t line    : AIC23B_PDC_LINE_SIZE;     //!< Line input.
  };
} aic23b_pdc_t;

//! @}


/*! \name Digital Audio Interface Format
 */
//! @{

#define AIC23B_DAIF                 0x07    //!< Register index.
#define AIC23B_DAIF_ADDRESS         0x07    //!< Register address.
#define AIC23B_DAIF_DEFAULT         0x0001  //!< Register default value.
#define AIC23B_DAIF_MS_OFFSET       6       //!< Master/slave mode bit-field offset.
#define AIC23B_DAIF_MS_SIZE         1       //!< Master/slave mode bit-field size.
#define AIC23B_DAIF_MS_SLAVE        0x0000  //!< Slave mode.
#define AIC23B_DAIF_MS_MASTER       0x0001  //!< Master mode.
#define AIC23B_DAIF_LRSWAP_OFFSET   5       //!< DAC left/right swap bit-field offset.
#define AIC23B_DAIF_LRSWAP_SIZE     1       //!< DAC left/right swap bit-field size.
#define AIC23B_DAIF_LRP_OFFSET      4       //!< DAC left/right phase bit-field offset.
#define AIC23B_DAIF_LRP_SIZE        1       //!< DAC left/right phase bit-field size.
#define AIC23B_DAIF_IWL_OFFSET      2       //!< Input bit length bit-field offset.
#define AIC23B_DAIF_IWL_SIZE        2       //!< Input bit length bit-field size.
#define AIC23B_DAIF_IWL_16          0x0000  //!< Input bit length: 16.
#define AIC23B_DAIF_IWL_20          0x0001  //!< Input bit length: 20.
#define AIC23B_DAIF_IWL_24          0x0002  //!< Input bit length: 24.
#define AIC23B_DAIF_IWL_32          0x0003  //!< Input bit length: 32.
#define AIC23B_DAIF_FMT_OFFSET      0       //!< Data format bit-field offset.
#define AIC23B_DAIF_FMT_SIZE        2       //!< Data format bit-field size.
#define AIC23B_DAIF_FMT_RA          0x0000  //!< Data format: MSB first, right aligned.
#define AIC23B_DAIF_FMT_LA          0x0001  //!< Data format: MSB first, left aligned.
#define AIC23B_DAIF_FMT_I2S         0x0002  //!< Data format: I2S format, MSB first, left - 1 aligned.
#define AIC23B_DAIF_FMT_DSP         0x0003  //!< Data format: DSP format, frame sync followed by two data words.

//! Register structure.
typedef union
{
  struct
  {
    const uint16_t addr    : AIC23B_CTRL_ADDR_SIZE;    //!< Control address.
          uint16_t data    : AIC23B_CTRL_DATA_SIZE;    //!< Control data.
  };
  struct
  {
    const uint16_t         : AIC23B_CTRL_ADDR_SIZE;
          uint16_t         : 2;
          uint16_t ms      : AIC23B_DAIF_MS_SIZE;      //!< Master/slave mode.
          uint16_t lrswap  : AIC23B_DAIF_LRSWAP_SIZE;  //!< DAC left/right swap.
          uint16_t lrp     : AIC23B_DAIF_LRP_SIZE;     //!< DAC left/right phase.
          uint16_t iwl     : AIC23B_DAIF_IWL_SIZE;     //!< Input bit length.
          uint16_t fmt     : AIC23B_DAIF_FMT_SIZE;     //!< Data format.
  };
} aic23b_daif_t;

//! @}


/*! \name Sample Rate Control
 */
//! @{

#define AIC23B_SRC                  0x08    //!< Register index.
#define AIC23B_SRC_ADDRESS          0x08    //!< Register address.
#define AIC23B_SRC_DEFAULT          0x0020  //!< Register default value.
#define AIC23B_SRC_CLKOUT_OFFSET    7       //!< Clock output divider bit-field offset.
#define AIC23B_SRC_CLKOUT_SIZE      1       //!< Clock output divider bit-field size.
#define AIC23B_SRC_CLKIN_OFFSET     6       //!< Clock input divider bit-field offset.
#define AIC23B_SRC_CLKIN_SIZE       1       //!< Clock input divider bit-field size.
#define AIC23B_SRC_SR_OFFSET        2       //!< Sampling rate control bit-field offset.
#define AIC23B_SRC_SR_SIZE          4       //!< Sampling rate control bit-field size.
#define AIC23B_SRC_BOSR_OFFSET      1       //!< Base oversampling rate bit-field offset.
#define AIC23B_SRC_BOSR_SIZE        1       //!< Base oversampling rate bit-field size.
#define AIC23B_SRC_USB_OFFSET       0       //!< USB clock mode select bit-field offset.
#define AIC23B_SRC_USB_SIZE         1       //!< USB clock mode select bit-field size.

//! Register structure.
typedef union
{
  struct
  {
    const uint16_t addr    : AIC23B_CTRL_ADDR_SIZE;    //!< Control address.
          uint16_t data    : AIC23B_CTRL_DATA_SIZE;    //!< Control data.
  };
  struct
  {
    const uint16_t         : AIC23B_CTRL_ADDR_SIZE;
          uint16_t         : 1;
          uint16_t clkout  : AIC23B_SRC_CLKOUT_SIZE;   //!< Clock output divider.
          uint16_t clkin   : AIC23B_SRC_CLKIN_SIZE;    //!< Clock input divider.
          uint16_t sr      : AIC23B_SRC_SR_SIZE;       //!< Sampling rate control.
          uint16_t bosr    : AIC23B_SRC_BOSR_SIZE;     //!< Base oversampling rate.
          uint16_t usb     : AIC23B_SRC_USB_SIZE;      //!< USB clock mode select.
  };
} aic23b_src_t;

//! @}


/*! \name Digital Interface Activation
 */
//! @{

#define AIC23B_DIA                  0x09    //!< Register index.
#define AIC23B_DIA_ADDRESS          0x09    //!< Register address.
#define AIC23B_DIA_DEFAULT          0x0000  //!< Register default value.
#define AIC23B_DIA_ACT_OFFSET       0       //!< Activate interface bit-field offset.
#define AIC23B_DIA_ACT_SIZE         1       //!< Activate interface bit-field size.

//! Register structure.
typedef union
{
  struct
  {
    const uint16_t addr    : AIC23B_CTRL_ADDR_SIZE;    //!< Control address.
          uint16_t data    : AIC23B_CTRL_DATA_SIZE;    //!< Control data.
  };
  struct
  {
    const uint16_t         : AIC23B_CTRL_ADDR_SIZE;
          uint16_t         : 8;
          uint16_t act     : AIC23B_DIA_ACT_SIZE;      //!< Activate interface.
  };
} aic23b_dia_t;

//! @}


/*! \name Reset Register
 */
//! @{

#define AIC23B_RR                   0x0A    //!< Register index.
#define AIC23B_RR_ADDRESS           0x0F    //!< Register address.
#define AIC23B_RR_DEFAULT           0x0000  //!< Register default value.
#define AIC23B_RR_RES_OFFSET        0       //!< Reset bit-field offset.
#define AIC23B_RR_RES_SIZE          9       //!< Reset bit-field size.

//! Register structure.
typedef union
{
  struct
  {
    const uint16_t addr    : AIC23B_CTRL_ADDR_SIZE;    //!< Control address.
          uint16_t data    : AIC23B_CTRL_DATA_SIZE;    //!< Control data.
  };
  struct
  {
    const uint16_t         : AIC23B_CTRL_ADDR_SIZE;
          uint16_t res     : AIC23B_RR_RES_SIZE;       //!< Reset.
  };
} aic23b_rr_t;

//! @}


/*! \name Register Structure
 */
//! @{

//! Returns the address of the \a REG register (e.g. \c AIC23B_LLICVC).
#define AIC23B_ADDRESS(REG)                                                    \
  REG##_ADDRESS

//! Returns the default value of the \a REG register (e.g. \c AIC23B_LLICVC).
#define AIC23B_DEFAULT(REG)                                                    \
  REG##_DEFAULT

//! Returns the offset of the \a BITFIELD bit-field (e.g. LRS) in its \a REG
//! register (e.g. \c AIC23B_LLICVC).
#define AIC23B_OFFSET(REG, BITFIELD)                                           \
  REG##_##BITFIELD##_OFFSET

//! Returns the size of the \a BITFIELD bit-field (e.g. LRS) of the \a REG
//! register (e.g. \c AIC23B_LLICVC).
#define AIC23B_SIZE(REG, BITFIELD)                                             \
  REG##_##BITFIELD##_SIZE

//! Returns the mask of the \a BITFIELD bit-field (e.g. LRS) in its \a REG
//! register (e.g. \c AIC23B_LLICVC).
#define AIC23B_MASK(REG, BITFIELD)                                             \
  (((1 << REG##_##BITFIELD##_SIZE) - 1) << REG##_##BITFIELD##_OFFSET)

//! @}


/*! \name Audio Channels
 */
//! @{

#define AIC23B_LEFT_CHANNEL     0x01
#define AIC23B_RIGHT_CHANNEL    0x02

//! @}


/*! \name Volume Control
 */
//! @{

#define AIC23B_MUTED            SCHAR_MIN
#define AIC23B_LIN_VOL_OFFSET   0x17
#define AIC23B_LIN_VOL_MIN      ((int8_t)(-34.5 * 2 / 3))
#define AIC23B_LIN_VOL_MAX      ((int8_t)(+12   * 2 / 3))
#define AIC23B_HP_VOL_OFFSET    0x79
#define AIC23B_HP_VOL_MIN       ((int8_t)(-72)) // -72dB
#define AIC23B_HP_VOL_MAX       ((int8_t)(+ 6)) // +6dB

//! @}


/*! \name Low-Level Interface
 */
//! @{

/*! \brief Reads the \a reg register.
 */
extern uint16_t aic23b_read_reg(uint8_t reg);

/*! \brief Writes \a val to the \a reg register.
 */
extern void aic23b_write_reg(uint8_t reg, uint16_t val);

//! @}


/*! \name Mid-Level Interface
 */
//! @{

/*! \brief Gets the \a ch line input channel volume.
 */
extern int8_t aic23b_get_line_in_volume(uint8_t ch);

/*! \brief Sets the \a ch_mask line input channels volume to \a vol.
 */
extern void aic23b_set_line_in_volume(uint8_t ch_mask, int8_t vol);

/*! \brief Gets the average headphone volume.
 */
extern uint8_t aic23b_dac_get_average_headphone_volume(void);

/*! \brief Sets the average headphone volume.
 */
extern void aic23b_dac_set_average_headphone_volume(uint8_t volume);

/*! \brief Returns the state of the mute mode of the DAC.
 */
extern bool aic23b_dac_is_headphone_volume_muted(void);

/*! \brief Returns the state of the boost mode of the DAC.
 */
extern bool aic23b_dac_is_headphone_volume_boosted(void);

/*! \brief Gets the \a ch channel headphone volume.
 */
extern int8_t aic23b_get_headphone_volume(uint8_t ch);

/*! \brief Sets the \a ch_mask channels headphone volume to \a vol, with
 *         zero-cross detect if \a z_cross is \c true.
 */
extern void aic23b_set_headphone_volume(uint8_t ch_mask, int8_t vol, bool z_cross);

/*! \brief Gets the analog audio path.
 */
extern aic23b_aapc_t aic23b_get_analog_audio_path(void);

/*! \brief Sets the analog audio path to \a aapc.
 */
extern void aic23b_set_analog_audio_path(aic23b_aapc_t aapc);

/*! \brief Gets the digital audio path.
 */
extern aic23b_dapc_t aic23b_get_digital_audio_path(void);

/*! \brief Sets the digital audio path to \a dapc.
 */
extern void aic23b_set_digital_audio_path(aic23b_dapc_t dapc);

/*! \brief Gets the power down state.
 */
extern aic23b_pdc_t aic23b_get_power_down_state(void);

/*! \brief Sets the power down state to \a pdc.
 */
extern void aic23b_set_power_down_state(aic23b_pdc_t pdc);

/*! \brief Tells whether the digital audio interface is activated.
 */
extern bool aic23b_is_dig_audio_activated(void);

/*! \brief Activates or deactivates the digital audio interface according to
 *         \a act.
 */
extern void aic23b_activate_dig_audio(bool act);

/*! \brief Resets the CODEC.
 */
extern void aic23b_reset(void);

/*! \brief Used to configure the codec frequency
 */
extern void aic23b_configure_freq(int master_clock_hz, int sample_rate_hz);

//! @}


/*! \name High-Level Interface
 */
//! @{

/*! \brief Starts the DAC.
 */
extern void aic23b_dac_start(uint32_t sample_rate_hz,
                             uint8_t num_channels,
                             uint8_t bits_per_sample,
                             bool swap_channels,
                             void (*callback)(uint32_t arg),
                             uint32_t callback_opt,
                             uint32_t pba_hz);

/*! \brief Sets the DAC up with new settings.
 *
 * \note The DAC must have been started beforehand.
 */
extern void aic23b_dac_setup(uint32_t sample_rate_hz,
                             uint8_t num_channels,
                             uint8_t bits_per_sample,
                             bool swap_channels,
                             void (*callback)(uint32_t arg),
                             uint32_t callback_opt,
                             uint32_t pba_hz);

/*! \brief Outputs a sample buffer to the DAC.
 *
 * \note The DAC must have been started beforehand.
 */
extern bool aic23b_dac_output(void *sample_buffer, size_t sample_length);

/*! \brief Increases the output volume of the DAC.
 *
 * \note The DAC must have been started beforehand.
 */
extern void aic23b_dac_increase_volume(void);

/*! \brief Decreases the output volume of the DAC.
 *
 * \note The DAC must have been started beforehand.
 */
extern void aic23b_dac_decrease_volume(void);

/*! \brief Flushes the sample buffer being output to the DAC.
 */
extern void aic23b_dac_flush(void);

/*! \brief Stops the DAC.
 */
extern void aic23b_dac_stop(void);


/*! \brief Starts the ADC.
 */
extern void aic23b_adc_start(uint32_t sample_rate_hz,
                             uint8_t num_channels,
                             uint8_t bits_per_sample,
                             bool swap_channels,
                             void (*callback)(uint32_t arg),
                             uint32_t callback_opt,
                             uint32_t pba_hz);

/*! \brief Sets the ADC up with new settings.
 *
 * \note The ADC must have been started beforehand.
 */
extern void aic23b_adc_setup(uint32_t sample_rate_hz,
                             uint8_t num_channels,
                             uint8_t bits_per_sample,
                             bool swap_channels,
                             void (*callback)(uint32_t arg),
                             uint32_t callback_opt,
                             uint32_t pba_hz);

/*! \brief Input a sample buffer from the ADC.
 *
 * \note The ADC must have been started beforehand.
 */
extern bool aic23b_adc_input(void *sample_buffer, size_t sample_length);

/*! \brief Flushes the sample buffer being input from the ADC.
 */
extern void aic23b_adc_flush(void);

/*! \brief Stops the ADC.
 */
extern void aic23b_adc_stop(void);

/*! \brief Starts the CODEC.
 */
extern void aic23b_codec_start(uint32_t sample_rate_hz,
                             uint8_t num_channels,
                             uint8_t bits_per_sample,
                             bool swap_channels,
                             void (*callback)(uint32_t arg),
                             uint32_t callback_opt,
                             uint32_t pba_hz);

/*! \brief Sets the CODEC up with new settings.
 *
 * \note The CODEC must have been started beforehand.
 */
extern void aic23b_codec_setup(uint32_t sample_rate_hz,
                             uint8_t num_channels,
                             uint8_t bits_per_sample,
                             bool swap_channels,
                             void (*callback)(uint32_t arg),
                             uint32_t callback_opt,
                             uint32_t pba_hz);

/*! \brief Flushes the sample buffers being input from the ADC and output to the DAC.
 */
extern void aic23b_codec_flush(void);

/*! \brief Stops the CODEC.
 */
extern void aic23b_codec_stop(void);

/*! \brief Mute audio if mute is set to true.
 */
extern void  aic23b_dac_mute(bool mute);

//! @}

/**
 * \}
 */

#endif  // _TLV320AIC23B_H_
