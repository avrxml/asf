/*****************************************************************************
 *
 * \file
 *
 * \brief 16-bit and 32-bit pseudo-random functions for the AVR32 UC3
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

static int seed = 123456789;

// Get the reference's value to compute a pseudo-random number
void dsp_op_srand(int new_seed)
{
  if (new_seed == 0)
    new_seed = 1;
  seed = new_seed;
}

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_OP16_RAND) || \
    !defined(TARGET_SPECIFIC_OP16_RAND)

// Generate a 16-bit pseudo-random number from a reference's value
dsp16_t dsp16_op_rand(void)
{
  // Park and Miller Minimal Standard
  seed = 16807*seed;
  return (dsp16_t) (seed >> (DSP16_QA-1)) & (((U16) -1) >> (DSP16_QA-1));
}

#endif

#if defined(FORCE_ALL_GENERICS) || \
    defined(FORCE_GENERIC_OP32_RAND) || \
    !defined(TARGET_SPECIFIC_OP32_RAND)

// Generate a 32-bit pseudo-random number from a reference's value
dsp32_t dsp32_op_rand(void)
{
  // Park and Miller Minimal Standard
  seed = 16807*seed;
  return (dsp32_t) (seed >> (DSP32_QA-1)) & (((U32) -1) >> (DSP16_QA-1));
}

#endif
