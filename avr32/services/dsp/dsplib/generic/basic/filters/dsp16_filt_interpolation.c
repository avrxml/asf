/*****************************************************************************
 *
 * \file
 *
 * \brief Generic 16-bit interpolation filter function.
 *
 * This file contains the code of the interpolation filter.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


#include "dsp.h"

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_FILT16_INTERPOLATION) || \
    !defined(TARGET_SPECIFIC_FILT16_INTERPOLATION)

// Must be a multiple of 2 to ensure compiler optimizations
#define LOOP_UNROLL             6
#define LOOP_UNROLL_PLUS_ONE    7

#define DSP16_INTREPOLATION_FILTER_FUNCTION_NAME(x_num, data) \
  TPASTE2(dsp16_filt_interpolation_kernel_x, x_num),

#define DSP16_INTREPOLATION_NO_LOOP_FILTER_FUNCTION_NAME(x_num, data) \
  TPASTE2(dsp16_filt_interpolation_no_loop_kernel_x, x_num),

#define DSP16_INTREPOLATION_FILTER(x_num, data) \
  sum += ph[(x_num+data)] * pvect2[-(x_num+data)];

#if LOOP_UNROLL > 4

#define DSP16_INTERPOLATION_KERNEL_X_FCT(x_num, data)                           \
static void TPASTE2(dsp16_filt_interpolation_kernel_x, x_num)(dsp16_t *vect1, dsp16_t *vect2, int vect2_size, dsp16_t *h, int h_size, int interpolation_ratio) \
{                                                                               \
  S32 sum = 0;                                                                  \
  int i, k, n;                                                                  \
  int n_tap;                                                                    \
  dsp16_t *ph;                                                                  \
  dsp16_t *pvect1;                                                              \
  dsp16_t *pvect2;                                                              \
                                                                                \
  pvect1 = vect1;                                                               \
  n_tap = h_size / interpolation_ratio;                                         \
                                                                                \
  for(n=0; n<vect2_size; n++)                                                   \
  {                                                                             \
    for(k=0; k<interpolation_ratio; k++)                                        \
    {                                                                           \
      sum = 0;                                                                  \
      ph = &h[k*n_tap];                                                         \
      pvect2 = &vect2[n + n_tap - 1];                                           \
      for(i=0; i<n_tap - LOOP_UNROLL + 1; i += LOOP_UNROLL)                     \
      {                                                                         \
        MREPEAT(LOOP_UNROLL, DSP16_INTREPOLATION_FILTER, 0)                     \
        ph += LOOP_UNROLL;                                                      \
        pvect2 -= LOOP_UNROLL;                                                  \
      }                                                                         \
      MREPEAT(x_num, DSP16_INTREPOLATION_FILTER, 0);                            \
      *pvect1++ = sum >> DSP16_QB;                                              \
    }                                                                           \
  }                                                                             \
}

#else // LOOP_UNROLL <= 4

#define DSP16_INTERPOLATION_KERNEL_X_FCT(x_num, data)                           \
static void TPASTE2(dsp16_filt_interpolation_kernel_x, x_num)(dsp16_t *vect1, dsp16_t *vect2, int vect2_size, dsp16_t *h, int h_size, int interpolation_ratio) \
{                                                                               \
  S32 sum = 0;                                                                  \
  int i, k, n;                                                                  \
  int n_tap;                                                                    \
  dsp16_t *ph;                                                                  \
  dsp16_t *pvect1;                                                              \
  dsp16_t *pvect2;                                                              \
                                                                                \
  pvect1 = vect1;                                                               \
  n_tap = h_size / interpolation_ratio;                                         \
                                                                                \
  for(n=0; n<vect2_size; n++)                                                   \
  {                                                                             \
    pvect2 = &vect2[n + n_tap - 1];                                             \
    for(k=0; k<interpolation_ratio; k++)                                        \
    {                                                                           \
      sum = 0;                                                                  \
      ph = &h[k*n_tap];                                                         \
      for(i=0; i<n_tap - LOOP_UNROLL + 1; i += LOOP_UNROLL)                     \
      {                                                                         \
        MREPEAT(LOOP_UNROLL, DSP16_INTREPOLATION_FILTER, i)                     \
      }                                                                         \
      MREPEAT(x_num, DSP16_INTREPOLATION_FILTER, i);                            \
      *pvect1++ = sum >> DSP16_QB;                                              \
    }                                                                           \
  }                                                                             \
}

#endif // LOOP_UNROLL > 4

#define DSP16_INTERPOLATION_NO_LOOP_KERNEL_X_FCT(x_num, data)                   \
static void TPASTE2(dsp16_filt_interpolation_no_loop_kernel_x, x_num)(dsp16_t *vect1, dsp16_t *vect2, int vect2_size, dsp16_t *h, int h_size, int interpolation_ratio) \
{                                                                               \
  S32 sum = 0;                                                                  \
  int k, n;                                                                     \
  int n_tap;                                                                    \
  dsp16_t *ph;                                                                  \
  dsp16_t *pvect1;                                                              \
  dsp16_t *pvect2;                                                              \
                                                                                \
  pvect1 = vect1;                                                               \
  n_tap = h_size / interpolation_ratio;                                         \
                                                                                \
  for(n=0; n<vect2_size; n++)                                                   \
  {                                                                             \
    pvect2 = &vect2[n + n_tap - 1];                                             \
    for(k=0; k<interpolation_ratio; k++)                                        \
    {                                                                           \
      sum = 0;                                                                  \
      ph = &h[k*n_tap];                                                         \
      MREPEAT(x_num, DSP16_INTREPOLATION_FILTER, 0)                             \
      *pvect1++ = sum >> DSP16_QB;                                              \
    }                                                                           \
  }                                                                             \
}

static void dsp16_filt_interpolation_no_loop_kernel_x0(dsp16_t *vect1, dsp16_t *vect2, int vect2_size, dsp16_t *h, int h_size, int interpolation_ratio)
{
  int k, n;
  dsp16_t *pvect1;
  dsp16_t *pvect2;

  pvect1 = vect1;

  for(n=0; n<vect2_size; n++)
  {
    pvect2 = &vect2[n + 1];
    for(k=0; k<interpolation_ratio; k++)
      *pvect1++ = *pvect2;
  }
}
DSP16_INTERPOLATION_NO_LOOP_KERNEL_X_FCT(1, )
DSP16_INTERPOLATION_NO_LOOP_KERNEL_X_FCT(2, )
DSP16_INTERPOLATION_NO_LOOP_KERNEL_X_FCT(3, )
DSP16_INTERPOLATION_NO_LOOP_KERNEL_X_FCT(4, )
DSP16_INTERPOLATION_NO_LOOP_KERNEL_X_FCT(5, )
DSP16_INTERPOLATION_NO_LOOP_KERNEL_X_FCT(6, )

DSP16_INTERPOLATION_KERNEL_X_FCT(0, )
DSP16_INTERPOLATION_KERNEL_X_FCT(1, )
DSP16_INTERPOLATION_KERNEL_X_FCT(2, )
DSP16_INTERPOLATION_KERNEL_X_FCT(3, )
DSP16_INTERPOLATION_KERNEL_X_FCT(4, )
DSP16_INTERPOLATION_KERNEL_X_FCT(5, )

void dsp16_filt_interpolation(dsp16_t *vect1, dsp16_t *vect2, int vect2_size, dsp16_t *h, int h_size, int interpolation_ratio)
{
  int n_tap;
  typedef void (*interpolation_kernel_opti_t)(dsp16_t *, dsp16_t *, int, dsp16_t *, int, int);
  static const interpolation_kernel_opti_t interpolation_end_kernel_opti[] = {
    MREPEAT(LOOP_UNROLL, DSP16_INTREPOLATION_FILTER_FUNCTION_NAME, )
  };
  static const interpolation_kernel_opti_t interpolation_no_loop_end_kernel_opti[] = {
    MREPEAT(LOOP_UNROLL_PLUS_ONE, DSP16_INTREPOLATION_NO_LOOP_FILTER_FUNCTION_NAME, )
  };

  n_tap = h_size / interpolation_ratio;

  if (n_tap <= LOOP_UNROLL)
    interpolation_no_loop_end_kernel_opti[n_tap](vect1, vect2, vect2_size, h, h_size, interpolation_ratio);
  else
    interpolation_end_kernel_opti[n_tap%LOOP_UNROLL](vect1, vect2, vect2_size, h, h_size, interpolation_ratio);
}

#endif
