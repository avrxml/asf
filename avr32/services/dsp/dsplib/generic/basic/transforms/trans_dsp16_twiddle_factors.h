/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit twiddle factors table
 *
 * This file define the twiddle factors table
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

#ifndef __TRANS_DSP16_TWIDDLE_FACTORS_H__
#define __TRANS_DSP16_TWIDDLE_FACTORS_H__

#include "dsp.h"

#ifdef N_TWIDDLE_FACTORS
#  define DSP16_N_TWIDDLE_FACTORS  N_TWIDDLE_FACTORS
#endif

/*! \name Twiddle factors table
 */
//! \{
#ifndef DSP16_N_TWIDDLE_FACTORS
/*! \ingroup group_cstdata
 * \brief This constant defines the size of the twiddle factors table we want to store in memory.\n
 * i.e.: if we want to compute a 1024-point (\b or \b less) complex FFT, we will define this constant as follow:
 * \code #define DSP16_N_TWIDDLE_FACTORS  1024 \endcode
 * It will store a 1024 twiddle factors elements table into memory that will allow you to compute either 4, 16, 256 or 1024 complex FFTs.
 * \warning This is not the size in byte of the twiddle factors table but it defines the number of items in the twiddle factors table.
 * Its actual size can differ with the use of the algorithm's optimization option \ref DSP_OPTI_SIZE.
 */
# define DSP16_N_TWIDDLE_FACTORS  1024
#endif

#ifndef TWIDDLE_FACTORS_PREFIX_TAB
/*! \ingroup group_cstdata
 * \brief This constant permits you to add attributes to the twiddle factors table.\n
 * It is useful if you want to store the table into another memory for example.\n
 * i.e.: \code #define TWIDDLE_FACTORS_PREFIX_TAB  const \endcode
 *This will store the twiddle factors table in FLASH for the avr32-uc30512.
 */
# define TWIDDLE_FACTORS_PREFIX_TAB
#endif
//! \}

extern TWIDDLE_FACTORS_PREFIX_TAB dsp16_t dsp16_twiddle_factors[];
extern TWIDDLE_FACTORS_PREFIX_TAB dsp16_t dsp16_twiddle_factors2[];

#endif //__TRANS_DSP16_TWIDDLE_FACTORS_H__
