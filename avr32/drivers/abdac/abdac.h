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
 *  Microchip Technology Inc: http://www.microchip.com \n
 *
 * $Revision: 506 $
 * $Date: 2007-05-21 20:00:05 +0200 (Mon, 21 May 2007) $ \n
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
