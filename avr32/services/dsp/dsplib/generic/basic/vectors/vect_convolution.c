/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit and 32-bit convolution functions for the AVR32 UC3
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


#include "dsp.h"

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_VECT16_CONV) || \
    !defined(TARGET_SPECIFIC_VECT16_CONV)

void dsp16_vect_conv(dsp16_t *vect1, dsp16_t *vect2, int vect2_size, dsp16_t *vect3, int vect3_size)
{
  if (vect2_size >= vect3_size)
    {
        // Zero-pads the beginning of the vector
      dsp16_vect_zeropad(vect1, vect3_size-1, vect3_size-1);
        // Copy the rest of the vector
      dsp16_vect_copy(&vect1[vect3_size-1], vect2, vect2_size);
        // Zero-pads the tail of the vector
      dsp16_vect_zeropad(&vect1[vect2_size+vect3_size-1], vect3_size-1, vect3_size-1);

      dsp16_vect_convpart(vect1, vect1, vect2_size + 2*(vect3_size - 1), vect3, vect3_size);
    }
  else
    {
        // Zero-pads the beginning of the vector
      dsp16_vect_zeropad(vect1, vect2_size-1, vect2_size-1);
        // Copy the rest of the vector
      dsp16_vect_copy(&vect1[vect2_size-1], vect3, vect3_size);
        // Zero-pads the tail of the vector
      dsp16_vect_zeropad(&vect1[vect3_size+vect2_size-1], vect2_size-1, vect2_size-1);

      dsp16_vect_convpart(vect1, vect1, vect3_size + 2*(vect2_size - 1), vect2, vect2_size);
    }
}

#endif

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_VECT32_CONV) || \
    !defined(TARGET_SPECIFIC_VECT32_CONV)

void dsp32_vect_conv(dsp32_t *vect1, dsp32_t *vect2, int vect2_size, dsp32_t *vect3, int vect3_size)
{
  if (vect2_size >= vect3_size)
    {
        // Zero-pads the beginning of the vector
      dsp32_vect_zeropad(vect1, vect3_size-1, vect3_size-1);
        // Copy the rest of the vector
      dsp32_vect_copy(&vect1[vect3_size-1], vect2, vect2_size);
        // Zero-pads the tail of the vector
      dsp32_vect_zeropad(&vect1[vect2_size+vect3_size-1], vect3_size-1, vect3_size-1);

      dsp32_vect_convpart(vect1, vect1, vect2_size + 2*(vect3_size - 1), vect3, vect3_size);
    }
  else
    {
        // Zero-pads the beginning of the vector
      dsp32_vect_zeropad(vect1, vect2_size-1, vect2_size-1);
        // Copy the rest of the vector
      dsp32_vect_copy(&vect1[vect2_size-1], vect3, vect3_size);
        // Zero-pads the tail of the vector
      dsp32_vect_zeropad(&vect1[vect3_size+vect2_size-1], vect2_size-1, vect2_size-1);

      dsp32_vect_convpart(vect1, vect1, vect3_size + 2*(vect2_size - 1), vect2, vect2_size);
    }
}

#endif
