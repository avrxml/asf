/*****************************************************************************
 *
 * \file
 *
 * \brief Fixed Point DSP library
 *
 * This file declares the functions supported by the DSP library
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

#ifndef __DSP_H__
#define __DSP_H__

/*! \defgroup group_dsplib DSP Library
 *  This group is the entry point for DSP Library users.
 */
/*! \defgroup group_functions Basic
 *  \ingroup group_dsplib
 *  This group regroups all the basic functions you can use in your project.
 */
/*! \defgroup group_advanced Advanced
 *  \ingroup group_dsplib
 *  This group regroups all the functions available in the advanced library.
 */
/*! \defgroup group_config Configuration
 *  \ingroup group_dsplib
 *  This group regroups all the defines you can change to optimize the DSP Library for your needs.
 */
/*! \defgroup group_cstdata Constant tables
 *  \ingroup group_config
 * These defines are used to configure data table storage methods
 */
/*! \defgroup group_optimization Optimization options
 *  \ingroup group_config
 * Those parameters are used for optimizing the algorithms according to your needs.\n
 * By default algorithms are defined to run the fastest as they can.
 */
/*! \defgroup group_qformat Q Format
 * \ingroup group_config
 * This module regroups the options used to configure the signed fixed point Q format.\n
 */
/*! \defgroup group_constants Constants
 *  \ingroup group_dsplib
 *  This group regroups all the constants you can use in your project.
 */
/*! \defgroup group_macros Macros
 *  \ingroup group_dsplib
 *  This group regroups all the defined macros you can use in your project.
 */
/*! \defgroup group_types Types
 *  \ingroup group_dsplib
 *  This group regroups all the types you can use in your project.
 */

/*! \name Algorithm's optimization options
 * \warning WARNING! Do not modify those defines. There are here to be used with the \ref DSP_OPTIMIZATION definition.
 */
//! \{
/*! \ingroup group_optimization
 * \brief To accelerate the algorithms
*/
#define DSP_OPTI_SPEED  0
/*! \ingroup group_optimization
 * \brief To be the more accurate as the algorithm can be
 */
#define DSP_OPTI_ACCURACY   1
/*! \ingroup group_optimization
 * \brief To minimize the size of the data needed to compute the algorithms
 */
#define DSP_OPTI_SIZE  2
/*! \ingroup group_optimization
 * \brief To be the more accurate and smallest as the algorithm can be
 */
#define DSP_OPTI_ACC_AND_SIZE  (DSP_OPTI_ACCURACY | DSP_OPTI_SIZE)
//! \}

/*! \name Algorithm's optimization
 * \ingroup group_optimization
 * \brief This is the define you have to modify in order to optimize the algorithms according to your needs.
 * i.e.: if you want to make the algorithms as accurate as they can be and the smallest possible, use this definition:
 * \code #define DSP_OPTIMIZATION  DSP_OPTI_ACC_AND_SIZE \endcode
 * \note You can directly modify this value by defining at the compilation this define with the desired value.
 */
//! \{
#ifndef DSP_OPTIMIZATION
# if (defined __GNUC__) && (defined __OPTIMIZE_SIZE__)
#   define DSP_OPTIMIZATION  (DSP_OPTI_SIZE)
# else
#   define DSP_OPTIMIZATION  (DSP_OPTI_SPEED)
# endif
#endif
//! \}

// Type definitions
#include "dsp_types.h"

// Target optimizations
#include "target_optim.h"

// ******* Function definitions *******

// Basic modules
#if defined(DSP_VECTORS) && DSP_VECTORS == true
#  include "dsp_vectors.h"
#endif
#if defined(DSP_FILTERS) && DSP_FILTERS == true
#  include "dsp_filters.h"
#endif
#if defined(DSP_OPERATORS) && DSP_OPERATORS == true
#  include "dsp_operators.h"
#endif
#if defined(DSP_SIGNAL_GENERATION) && DSP_SIGNAL_GENERATION == true
#  include "dsp_signal_generation.h"
#endif
#if defined(DSP_TRANSFORMS) && DSP_TRANSFORMS == true
#  include "dsp_transforms.h"
#endif
#if defined(DSP_WINDOWING) && DSP_WINDOWING == true
#  include "dsp_windowing.h"
#endif

// Advanced modules
#if (defined(DSP_ADPCM) && DSP_ADPCM == true) || \
  (defined(DSP_RESAMPLING) && DSP_RESAMPLING == true)

#  if defined(DSP_ADPCM) && DSP_ADPCM == true
#    include "dsp_adpcm.h"
#  endif
#  if defined(DSP_RESAMPLING) && DSP_RESAMPLING == true
#    include "dsp_resampling.h"
#  endif

#endif

#endif //__DSP_H__
