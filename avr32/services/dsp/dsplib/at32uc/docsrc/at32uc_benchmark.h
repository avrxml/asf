/*****************************************************************************
 *
 * \file
 *
 * \brief Benchmark
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


/*! \defgroup group_at32uc_bench Benchmarks for the specific target "UC3"
 * \ingroup group_dsplib
 */

/*! \addtogroup group_at32uc_bench
 * Here is a list of benchmarks available.
 */

/*! \addtogroup group_at32uc_bench
 * \section Filtering
 *
 * - \ref FIR\n
 * Computation of a 256-point signal with a 24-TAP filter:\n
 * \b 12\b ,\b 712 \b cycles (423.7us @ 30MHz) for \ref dsp16_filt_fir\n
 * \b 36\b ,\b 432 \b cycles (1.21ms @ 30MHz) for \ref dsp32_filt_fir.
 *
 * - \ref Partial_IIR\n
 * Computation of a 256-point signal with a 7th order filter:\n
 * \b 15\b ,\b 006 \b cycles (500.2us @ 30MHz) for \ref dsp16_filt_iirpart\n
 * \b 33\b ,\b 333 \b cycles (1.11ms @ 30MHz) for \ref dsp32_filt_iirpart.
 *
 * - \ref LMS\n
 * Computation of a 256-point signal:\n
 * \b 3\b ,\b 124 \b cycles (104.1us @ 30MHz) for \ref dsp16_filt_lms\n
 * \b 5\b ,\b 317 \b cycles (177.2us @ 30MHz) for \ref dsp32_filt_lms.
 *
 * - \ref NLMS\n
 * Computation of a 256-point signal:\n
 * \b 3\b ,\b 706 \b cycles (123.5us @ 30MHz) for \ref dsp16_filt_nlms\n
 * \b 6\b ,\b 820 \b cycles (227.3us @ 30MHz) for \ref dsp32_filt_nlms.
 *
 */

/*! \addtogroup group_at32uc_bench
 * \section Operators
 *
 * - \ref Sine / \ref Cosine\n
 * \b 48 \b cycles (1.6us @ 30MHz) for \ref dsp16_op_sin\n
 * \b 196 \b cycles (6.5us @ 30MHz) for \ref dsp32_op_sin.
 *
 * - \ref Arc_sine / \ref Arc_cosine\n
 * \b 70 to \b 153 \b cycles (2.3/5.1us @ 30MHz) for \ref dsp16_op_acos\n
 * \b 161 to \b 570 \b cycles (5.4/19us @ 30MHz) for \ref dsp32_op_acos.
 *
 * - \ref Absolute\n
 * \b 1 \b cycle (33ns @ 30MHz) for \ref dsp16_op_abs\n
 * \b 1 \b cycle (33ns @ 30MHz) for \ref dsp32_op_abs.
 *
 * - \ref Square_root\n
 * \b 61 \b cycles (2.0us @ 30MHz) for \ref dsp16_op_sqrt\n
 * \b 450 \b cycles (15us @ 30MHz) for \ref dsp32_op_sqrt.
 *
 * - \ref Natural_logarithm / \ref Binary_logarithm / \ref Common_logarithm\n
 * \b 90 to \b 102 \b cycles (3.0/3.4us @ 30MHz) for \ref dsp16_op_ln, \ref dsp16_op_log2, \ref dsp16_op_log10\n
 * \b 555 to \b 570 \b cycles (18.5/19us @ 30MHz) for \ref dsp32_op_ln, \ref dsp32_op_log2, \ref dsp32_op_log10.
 *
 * - \ref Exponential\n
 * \b 70 \b cycles (2.3us @ 30MHz) for \ref dsp16_op_exp\n
 * \b 175 \b cycles (5.8us @ 30MHz) for \ref dsp32_op_exp.
 *
 * - \ref Power\n
 * \b 166 to \b 200 \b cycles (5.5/6.7us @ 30MHz) for \ref dsp16_op_pow\n
 * \b 660 to \b 790 \b cycles (22.0/26.3us @ 30MHz) for \ref dsp32_op_pow.
 *
 * - \ref Random\n
 * \b 20 \b cycles (0.7us @ 30MHz) for \ref dsp16_op_rand\n
 * \b 18 \b cycles (0.6us @ 30MHz) for \ref dsp32_op_rand.
 *
 */

/*! \addtogroup group_at32uc_bench
 * \section sgen Signal Generation
 *
 * - \ref Cosinusoidal / \ref Sinusoidal\n
 * Generation of a 100-points signal:\n
 * \b 5\b ,\b 752 \b cycles (191.7us @ 30MHz) for \ref dsp16_gen_cos, \ref dsp16_gen_sin\n
 * \b 19\b ,\b 229 \b cycles (641.0us @ 30MHz) for \ref dsp32_gen_cos, \ref dsp32_gen_sin.
 *
 * - \ref Noise\n
 * Generation of a 100-points signal:\n
 * \b 2\b ,\b 335 \b cycles (77.8us @ 30MHz) for \ref dsp16_gen_noise\n
 * \b 2\b ,\b 240 \b cycles (74.7us @ 30MHz) for \ref dsp32_gen_noise.
 *
 */

 /*! \addtogroup group_at32uc_bench
 * \section Transforms
 *
 * - \ref Real_Complex_FFT\n
 * Computation of a 256-points real to complex FFT:\n
 * \b 13\b ,\b 661 \b cycles (455.4us @ 30MHz) for \ref dsp16_trans_realcomplexfft\n
 * \b 74\b ,\b 297 \b cycles (2.48ms @ 30MHz) for \ref dsp32_trans_realcomplexfft.
 *
 */

/*! \addtogroup group_at32uc_bench
 * \section Vectors
 *
 * - \ref Addition\n
 * Addition of two 256-point vectors:\n
 * \b 1\b ,\b 252 \b cycles (41.7us @ 30MHz) for \ref dsp16_vect_add\n
 * \b 1\b ,\b 638 \b cycles (54.6us @ 30MHz) for \ref dsp32_vect_add.
 *
 * - \ref Subtraction\n
 * Subtraction of two 256-point vectors:\n
 * \b 1\b ,\b 252 \b cycles (41.7us @ 30MHz) for \ref dsp16_vect_sub\n
 * \b 1\b ,\b 638 \b cycles (54.6us @ 30MHz) for \ref dsp32_vect_sub.
 *
 * - \ref Multiplication_with_a_real\n
 * Multiplication of one 256-point vectors with a real number:\n
 * \b 1\b ,\b 452 \b cycles (48.4us @ 30MHz) for \ref dsp16_vect_realmul\n
 * \b 1\b ,\b 909 \b cycles (63.6us @ 30MHz) for \ref dsp32_vect_realmul.
 *
 * - \ref Division_with_a_real\n
 * Division of one 256-point vectors with a real number:\n
 * \b 10\b ,\b 338 \b cycles (344.6us @ 30MHz) for \ref dsp16_vect_realdiv\n
 * \b 47\b ,\b 291 \b cycles (1.58ms @ 30MHz) for \ref dsp32_vect_realdiv.
 *
 * - \ref Dot_multiplication\n
 * Dot multiplication of two 256-point vectors:\n
 * \b 1\b ,\b 561 \b cycles (52.0us @ 30MHz) for \ref dsp16_vect_dotmul\n
 * \b 2\b ,\b 341 \b cycles (78.0us @ 30MHz) for \ref dsp32_vect_dotmul.
 *
 * - \ref Dot_division\n
 * Dot division of two 256-point vectors:\n
 * \b 10\b ,\b 700 \b cycles (356.7us @ 30MHz) for \ref dsp16_vect_dotdiv\n
 * \b 57\b ,\b 400 \b cycles (1.91ms @ 30MHz) for \ref dsp32_vect_dotdiv.
 *
 * - \ref Power_\n
 * Raised to the power of one 256-point vectors with a real number:\n
 * \b 51\b ,\b 927 \b cycles (1.73ms @ 30MHz) for \ref dsp16_vect_pow\n
 * \b 167\b ,\b 276 \b cycles (5.58ms @ 30MHz) for \ref dsp32_vect_pow.
 *
 * - \ref Minimum\n
 * Minimum of a 256-point vector:\n
 * \b 1\b ,\b 023 \b cycles (34.1us @ 30MHz) for \ref dsp16_vect_min\n
 * \b 1\b ,\b 065 \b cycles (35.5us @ 30MHz) for \ref dsp32_vect_min.
 *
 * - \ref Maximum\n
 * Maximum of a 256-point vector:\n
 * \b 1\b ,\b 023 \b cycles (34.1us @ 30MHz) for \ref dsp16_vect_max\n
 * \b 1\b ,\b 065 \b cycles (35.5us @ 30MHz) for \ref dsp32_vect_max.
 *
 * - \ref Negate\n
 * Negate of a 256-point vector:\n
 * \b 1\b ,\b 189 \b cycles (39.6us @ 30MHz) for \ref dsp16_vect_neg\n
 * \b 1\b ,\b 389 \b cycles (46.3us @ 30MHz) for \ref dsp32_vect_neg.
 *
 * - \ref Convolution\n
 * Computation of a 64-points signal with another 64-point signal:\n
 * \b 19\b ,\b 087 \b cycles (636.2us @ 30MHz) for \ref dsp16_vect_conv\n
 * \b 50\b ,\b 501 \b cycles (1.68ms @ 30MHz) for \ref dsp32_vect_conv.
 *
 * - \ref Partial_Convolution\n
 * Computation of a 256-points signal with a 24-points signal:\n
 * \b 12\b ,\b 712 \b cycles (423.7us @ 30MHz) for \ref dsp16_vect_convpart\n
 * \b 36\b ,\b 432 \b cycles (1.21ms @ 30MHz) for \ref dsp32_vect_convpart.
 *
 */

/*! \addtogroup group_at32uc_bench
 * \section ADPCM
 *
 * - Encoding a 100-element vector:\n
 * \b 5\b ,\b 819 \b cycles (194.0us @ 30MHz) using \ref dsp_adpcm_ima_encode.
 *
 * - Decoding a 100-element vector:\n
 * \b 4\b ,\b 740 \b cycles (158.0us @ 30MHz) using \ref dsp_adpcm_ima_decode.
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
