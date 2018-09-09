/**
 * \file
 *
 * \brief ACIFA driver example configuration
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
