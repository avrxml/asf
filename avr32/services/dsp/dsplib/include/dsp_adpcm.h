/*****************************************************************************
 *
 * \file
 *
 * \brief This module provides a useful set of IMA/DVI ADPCM coding and decoding functions.
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
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
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
