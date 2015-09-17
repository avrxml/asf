/*****************************************************************************
 *
 * \file
 *
 * \brief AVR32 ABDAC example application.
 *
 * \par Application note:
 *  AVR32120 - Using the AVR32 audio bitstream DAC ABDAC
 *
 * \par Documentation
 *  For comprehensive code documentation, supported compilers, compiler
 *  settings and supported devices see readme.html.
 *
 *  Atmel Corporation: http://www.atmel.com \n
 *
 * $Revision: 506 $
 * $Date: 2007-05-21 20:00:05 +0200 (Mon, 21 May 2007) $ \n
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef _ABDAC_H_
#define _ABDAC_H_

/**
 * \defgroup group_avr32_drivers_abdac AUDIO - ABDAC - Audio Bitstream DAC
 *
 * Audio Bitstream DAC (Digital to Analog Converter) provides functions to convert a 16-bit sample value to a digital bitstream.
 * For UC3 B, this module is only available in AT32UC3B0512 and AT32UC3B1512.
 *
 * \{
 */

#include "compiler.h"
#include <avr32/io.h>

/*! \brief Number of tries before the driver reports timeout. */
#define ABDAC_TIMEOUT 5000
/*! \brief GCLK used by the ABDAC to generate bitstream. */
#define ABDAC_GCLK  AVR32_PM_GCLK_ABDAC


enum {
  EINVAL = 22,
  ETIMEOUT,
};

void abdac_enable(volatile avr32_abdac_t *abdac);
void abdac_disable(volatile avr32_abdac_t *abdac);
void abdac_swap_channels(volatile avr32_abdac_t *abdac);
unsigned long abdac_set_dac_hz(volatile avr32_abdac_t *abdac,
    const unsigned long bus_hz, const unsigned long dac_hz);
unsigned long abdac_get_dac_hz(volatile avr32_abdac_t *abdac,
    const unsigned long bus_hz);

int abdac_sink(volatile avr32_abdac_t *abdac,
    const unsigned short ch0, const unsigned short ch1);
bool abdac_set_dac_sample_rate(const unsigned long dac_hz);

/*! \brief Bit macro for ABDAC. */
#define ABDAC_BIT(name)           \
  (1 << AVR32_ABDAC_##name##_OFFSET)
/*! \brief Bit-field macro for GCLK. */
#define ABDAC_BF(name, value)         \
  (((value) & ((1 << AVR32_ABDAC_##name##_SIZE) - 1)) \
   << AVR32_ABDAC_##name##_OFFSET)
/*! \brief Bit macro for ABDAC GCLK. */
#define GCLK_BIT(name)            \
  (1 << AVR32_PM_GCCTRL_##name##_OFFSET)
/*! \brief Bit-field macro for ABDAC GCLK. */
#define GCLK_BF(name, value)          \
  (((value) & ((1 << AVR32_PM_GCCTRL_##name##_SIZE) - 1))\
   << AVR32_PM_GCCTRL_##name##_OFFSET)
/*! \brief Bit-field extraction macro for ABDAC GCLK. */
#define GCLK_BFEXT(name, value)         \
  (((value) >> AVR32_PM_GCCTRL_##name##_OFFSET)    \
   & ((1 << AVR32_PM_GCCTRL_##name##_SIZE) - 1))

/**
 * \}
 */

#endif /* _DAC_H_ */
