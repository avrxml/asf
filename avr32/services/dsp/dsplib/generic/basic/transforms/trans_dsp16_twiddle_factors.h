/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit twiddle factors table
 *
 * This file define the twiddle factors table
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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
