/**
 * \file
 *
 * \brief ACIFA driver example configuration
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#ifndef CONF_EXAMPLE_H
#define CONF_EXAMPLE_H

/* Connection of the Analog Comparators */
/* ACMP0 : Comparator + (PA6) */
#  define EXAMPLE_AC_ACMP0_INPUT        1
#  define EXAMPLE_AC_ACMP0_PIN          AVR32_AC1AP1_PIN
#  define EXAMPLE_AC_ACMP0_FUNCTION     AVR32_AC1AP1_FUNCTION

/* ACMPN0 : Comparator - (PA7) */
#  define EXAMPLE_AC_ACMPN0_INPUT       1
#  define EXAMPLE_AC_ACMPN0_PIN         AVR32_AC1AN1_PIN
#  define EXAMPLE_AC_ACMPN0_FUNCTION    AVR32_AC1AN1_FUNCTION

/* ACMP1 : Comparator + (PA20) */
#  define EXAMPLE_AC_ACMP1_INPUT        0
#  define EXAMPLE_AC_ACMP1_PIN          AVR32_AC0AP0_PIN
#  define EXAMPLE_AC_ACMP1_FUNCTION     AVR32_AC0AP0_FUNCTION

/* ACMPN0 : Comparator - (PA22) */
#  define EXAMPLE_AC_ACMPN1_INPUT       0
#  define EXAMPLE_AC_ACMPN1_PIN         AVR32_AC0AN0_PIN
#  define EXAMPLE_AC_ACMPN1_FUNCTION    AVR32_AC0AN0_FUNCTION

/* ACMP2 : Comparator + (PA23) */
#  define EXAMPLE_AC_ACMP2_INPUT        2
#  define EXAMPLE_AC_ACMP2_PIN          AVR32_AC0BP0_PIN
#  define EXAMPLE_AC_ACMP2_FUNCTION     AVR32_AC0BP0_FUNCTION

/* ACMPN2 : Comparator - (PA21) */
#  define EXAMPLE_AC_ACMPN2_INPUT       0
#  define EXAMPLE_AC_ACMPN2_PIN         AVR32_AC0BN0_PIN
#  define EXAMPLE_AC_ACMPN2_FUNCTION    AVR32_AC0BN0_FUNCTION

#endif /* CONF_EXAMPLE_H */
