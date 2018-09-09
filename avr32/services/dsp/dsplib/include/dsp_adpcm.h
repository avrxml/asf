/*****************************************************************************
 *
 * \file
 *
 * \brief This module provides a useful set of IMA/DVI ADPCM coding and decoding functions.
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 ******************************************************************************/

/*! \defgroup group_adpcm DSPLIB - IMA/DVI ADPCM
 *  \ingroup group_advanced
 *  All the IMA/DVI ADPCM functions implemented in the DSP advanced library.
 */
/*! \addtogroup group_adpcm
 *
 * \htmlonly
 * <hr />
 * \endhtmlonly
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef __ADPCM_H__
#define __ADPCM_H__

#ifdef __AVR32_ABI_COMPILER__

/*! \brief IMA/DVI ADPCM sample encoder
 * \ingroup group_adpcm
 * \param nibble The sample to encode.
 * \param step_index A pointer on a 16 bits data which contain the current
 * step index of the ADPCM algorithm.
 * \param predicted_value A pointer on a 16 bits data which contain the current
 * predicted value of the ADPCM algorithm.
 * \return A 4-bit data that corresponds to the sample encoded.
 */
S8 dsp_adpcm_ima_encode_nibble(S16 nibble, S16 *step_index, S16 *predicted_value);

/*! \brief IMA/DVI ADPCM sample decoder
 * \ingroup group_adpcm
 * \param nibble The sample to decode. It must be a 4-bit data.
 * \param step_index A pointer on a 16 bits data which contain the previous
 * step index of the ADPCM algorithm.
 * \param predicted_value A pointer on a 16 bits data which contain the previous
 * predicted value of the ADPCM algorithm.
 * \return A 16-bit data that corresponds to the sample decoded.
 */
S16 dsp_adpcm_ima_decode_nibble(S8 nibble, S16 *step_index, S16 *predicted_value);

/*! \brief IMA/DVI ADPCM encoder
 * \ingroup group_adpcm
 * \param out A 4-bit data vector that will contain the encoded data.
 * \param in A 16-bit data vector that contains the data to encode.
 * \param size The number of data to encode.
 * \param step_index A pointer on a 16 bits data which contain the current
 * step index of the ADPCM algorithm.
 * \param predicted_value A pointer on a 16 bits data which contain the current
 * predicted value of the ADPCM algorithm.
 * \note Can be performed "in-place".
 */
void dsp_adpcm_ima_encode(void *out, S16 *in, int size, S16 *step_index, S16 *predicted_value);

/*! \brief IMA/DVI ADPCM decoder
 * \ingroup group_adpcm
 * \param out A 16-bit data vector that will contain the decoded data.
 * \param in A 4-bit data vector that contains the IMA/DVI ADPCM encoded data to decode.
 * \param size The number of data to decode.
 * \param step_index A pointer on a 16 bits data which contain the current
 * step index of the ADPCM algorithm.
 * \param predicted_value A pointer on a 16 bits data which contain the current
 * predicted value of the ADPCM algorithm.
 */
void dsp_adpcm_ima_decode(S16 *out, void *in, int size, S16 *step_index, S16 *predicted_value);

#endif // __AVR32_ABI_COMPILER__
#endif //__ADPCM_H__
