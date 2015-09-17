/*****************************************************************************
 *
 * \file
 *
 * \brief Documentation of the Fixed point DSP Library
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

/*! \addtogroup group_dsplib
 * \section intro Introduction
 * The Atmel AVR DSPLib is a library which provides a useful set of digital signal processing functions.
 * All the source code is available and fully compilable with GCC and IAR.
 * This library provides optimized functions for the 32-bit AVRs but also target-independants functions.\n
 * This document is a listing of the functions currently implemented in the DSP Library
 *
 *
 * \section contents What can I find in this document ?
 * Following are the main sections available in this document.
 * - Basic APIs: \ref group_filters, \ref group_operators, \ref group_signal_generation, \ref group_transforms, \ref group_vectors and \ref group_windowing.
 * - Advanced APIs: \ref group_advanced.
 * - Data: \ref group_types, \ref group_macros and \ref group_constants.
 * - Configuration options: \ref group_optimization, \ref group_qformat and \ref group_cstdata.
 *
 * \section architecture Architecture
 * The DSPLib is divided in 2 parts :
 * - The \b basic library : containing the basic functions which are themselves sorted in sub-groups :
 * filters, windows, vector management functions, operators, transforms and signal generation functions.
 * - The \b advanced library : built on the first one and which contains more complex functions such as ADCPM encoder/decoder.
 *
 * Here is the directory architecture of the DSPLib (See /services/dsp/dsplib in the Atmel Software Framework) :
 *<table style="border: 0px solid white;"><tr><td style="border: 0px solid white;">
 *  \image html arch_root.png "Directory Architecture of the DSPLib"
 *</td><td style="border: 0px solid white;">
 * - The \b /at32uc directory includes all the source code of the optimized functions available for the AT32UC targets and provided by the DSPLib (mainly assembly coded).
 * - The \b /examples directory regroups a lot of examples showing a way to use the functions provided by both advanced and basic libraries.
 * - The \b /generic directory regroups all the target-independant source code of the DSPLib.
 * - The \b /include directory includes all the common include files for the optimized functions and the generic functions.
 * - The \b /utils directory regroups useful tools and scripts for the DSP library.
 *</td></tr></table>
 *
 * \section fixpformat Fixed-point format
 * All DSP types in the DSPLib includes the notion of 16-bit and 32-bit fixed-point formats. It is important to understand this format in order to fastest and/or debug an application.
 * See \ref group_qformat for more details on this format.
 *
 *
 * \section namingconv Naming Convention
 * Each function in the basic library of the DSPLib follows this naming convention rule : dspX_Y_Z(...)
 * Where:
 * - X is the type used (16 or 32)
 * - Y is category of the library (op, vect, trans, ...)
 * - Z is the short name of the function (cos, mul, ...)
 *
 * Example :
 * \code dsp16_vect_mul is a function part of the DSP library. It works with 16-bit values and is used to multiply vectors. \endcode
 *
 *
 * \section compilopt Compilation Options
 * The DSPLIB is made to fit the best the user needs.
 * Therefore, many configuration constants can be set at the compilation of the library.
 * Here is the list :
 * - \b DSP_OPTIMIZATION : used to optimize the algorithmic of the functions.
 * This optimization can be for speed, size, accuracy or size and accuracy.
 * It can be set in addition to the one provided by the compiler.
 * - \b FORCE_ALL_GENERICS : to use only the generic (target-independant) functions when you compile the library.
 * - \b FORCE_GENERIC_DSP16_Z : to use the 16-bit generic version of a function.
 * The specified function is defined by replacing the Z letter with the short name of the function.
 * See \ref namingconv for more details on the short name of a function.
 * - \b FORCE_GENERIC_DSP32_Z : to use the 32-bit generic version of a function.
 * The specified function is defined by replacing the Z letter with the short name of the function.
 * See \ref namingconv for more details on the short name of a function.
 * - \b DSP16_FORMAT : to specify the precision of the 16-bit Q format number used.
 * (ie: to use Q3.13 numbers, define DSP16_FORMAT with the value 13).
 * - \b DSP32_FORMAT : to specify the precision of the 32-bit Q format number used.
 * (ie: to use Q10.22 numbers, define DSP32_FORMAT with the value 22).
 *
 * All of these defines can be passed to the preprocessor at the compilation.
 * Example:
 * \code use -D  DSP16_FORMAT=12 in command line with GCC to use formatted Q4.12 numbers. \endcode
 *
 *
 * \section deviceinfo Device Information
 * All 32-bit AVR devices can be used.
 */

/*! \defgroup group_dsplib_tools Tools
 * \ingroup group_dsplib
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

// For documentation purpose only
#define DSP_OPTIMIZATION

