/**
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
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#ifndef _CONF_RTOUCH_H_
#define _CONF_RTOUCH_H_

#include <board.h>

/** Sample count per signal line. */
#define RTOUCH_OVERSAMPLING          32

/** Number of right shifts required to scale down to 10 bits after oversampling. */
#define RTOUCH_SAMPLESCALE           5

/** Deepest sleep mode allowed when waiting for touch detect. */
#define TOUCH_DETECT_SLEEP_MODE      SLEEPMGR_SAVE

/** Deepest sleep mode allowed when sampling. */
#define TOUCH_SAMPLE_SLEEP_MODE      SLEEPMGR_IDLE

/** Set to 0 if the timer module should be used for delayed resampling. */
#define TOUCH_USE_IMMEDIATE_RESAMPLE 1

/** Interrupt priority for the resistive touch driver. */
#define RTOUCH_ADC_INT_LEVEL         AVR32_INTC_INTLEVEL_INT0

#if !defined(RTOUCH_ADC_IRQ)         || \
    !defined(RTOUCH_XH_PIN)          || \
    !defined(RTOUCH_XH_PIN_FUNCTION) || \
    !defined(RTOUCH_ADC_XH_CHANNEL)  || \
    !defined(RTOUCH_YH_PIN)          || \
    !defined(RTOUCH_YH_PIN_FUNCTION) || \
    !defined(RTOUCH_ADC_YH_CHANNEL)  || \
    !defined(RTOUCH_XL_PIN)          || \
    !defined(RTOUCH_XL_PIN_FUNCTION) || \
    !defined(RTOUCH_ADC_XL_CHANNEL)  || \
    !defined(RTOUCH_YL_PIN)          || \
    !defined(RTOUCH_YL_PIN_FUNCTION) || \
    !defined(RTOUCH_ADC_YL_CHANNEL)  || \
    !defined(RTOUCH_ADC)

# define RTOUCH_ADC_IRQ              AVR32_ADCIFA_SEQUENCER0_IRQ

/** IO pin used for XL line. */
# define RTOUCH_XH_PIN               AVR32_ADCIN1_PIN /* PA5 */
# define RTOUCH_XH_PIN_FUNCTION      AVR32_ADCIN1_FUNCTION
# define RTOUCH_ADC_XH_CHANNEL       AVR32_ADCIFA_INP_ADCIN1

/** IO pin used for XH line. */
# define RTOUCH_XL_PIN               AVR32_ADCIN15_PIN/* PA13 */
# define RTOUCH_XL_PIN_FUNCTION      AVR32_ADCIN15_FUNCTION
# define RTOUCH_ADC_XL_CHANNEL       AVR32_ADCIFA_INN_ADCIN15

/** IO pin used for YL line. */
# define RTOUCH_YL_PIN               AVR32_ADCIN13_PIN /* PA24 */
# define RTOUCH_YL_PIN_FUNCTION      AVR32_ADCIN13_FUNCTION
# define RTOUCH_ADC_YL_CHANNEL       AVR32_ADCIFA_INN_ADCIN13

/** IO pin used for YH line. */
# define RTOUCH_YH_PIN               AVR32_ADCIN0_PIN /* PA4 */
# define RTOUCH_YH_PIN_FUNCTION      AVR32_ADCIN0_FUNCTION
# define RTOUCH_ADC_YH_CHANNEL       AVR32_ADCIFA_INP_ADCIN0

/** AVR32 ADC module used for touch panel sampling. */
# define RTOUCH_ADC                  AVR32_ADCIFA

# warning "Using a default resistive touch define value. Edit the conf_rtouch.h file to modify that define value according to the current board."
#endif

#endif
