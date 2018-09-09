/**
 * \file
 *
 * \brief Program memory access
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

#ifndef UTILS_PROGMEM_H
#define UTILS_PROGMEM_H

/**
 * \defgroup group_xmega_utils_progmem Program memory
 *
 * \ingroup group_xmega_utils
 *
 * \{
 */

/*! \name Program memory
 *
 * Macros for locating and accessing data in program memory.
 *
 * @{
 */
#if defined(__GNUC__) || defined(__DOXYGEN__)
# include <avr/pgmspace.h>
# define PROGMEM_LOCATION(type, name, loc) \
		type name __attribute__((section (#loc)))
# define PROGMEM_DECLARE(type, name) const type name __attribute__((__progmem__))
# define PROGMEM_STRING(x) PSTR(x)
# define PROGMEM_STRING_T  PGM_P
# define PROGMEM_T const
# define PROGMEM_PTR_T const *
# define PROGMEM_BYTE_ARRAY_T uint8_t*
# define PROGMEM_WORD_ARRAY_T uint16_t*
# define PROGMEM_READ_BYTE(x) pgm_read_byte(x)
# define PROGMEM_READ_WORD(x) pgm_read_word(x)

#elif defined(__ICCAVR__)
# include <pgmspace.h>
# ifndef __HAS_ELPM__
#  define _MEMATTR_ASF  __flash
# else /* __HAS_ELPM__ */
#  define _MEMATTR_ASF  __hugeflash
# endif /* __HAS_ELPM__ */
# define PROGMEM_LOCATION(type, name, loc) const _MEMATTR_ASF type name @ loc
# define PROGMEM_DECLARE(type, name) _MEMATTR_ASF type name
# define PROGMEM_STRING(x) ((_MEMATTR_ASF const char *)(x))
# define PROGMEM_STRING_T  char const _MEMATTR_ASF *
# define PROGMEM_T const _MEMATTR_ASF
# define PROGMEM_PTR_T const _MEMATTR_ASF *
# define PROGMEM_BYTE_ARRAY_T uint8_t const _MEMATTR_ASF *
# define PROGMEM_WORD_ARRAY_T uint16_t const _MEMATTR_ASF *
# define PROGMEM_READ_BYTE(x) *(x)
# define PROGMEM_READ_WORD(x) *(x)
#endif
//! @}

/**
 * \}
 */

#endif /* UTILS_PROGMEM_H */
