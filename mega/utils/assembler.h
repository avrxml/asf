/**
 * \file
 *
 * \brief Assembler abstraction layer and utilities
 *
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
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
#ifndef ASSEMBLER_H_INCLUDED
#define ASSEMBLER_H_INCLUDED

#if !defined(__ASSEMBLER__) && !defined(__IAR_SYSTEMS_ASM__) \
		&& !defined(__DOXYGEN__)
# error This file may only be included from assembly files
#endif

#if defined(__ASSEMBLER__)
# include "assembler/gas.h"
# include <avr/io.h>
#elif defined(__IAR_SYSTEMS_ASM__)
# include "assembler/iar.h"
# include <ioavr.h>
#endif

/**
 * \ingroup group_mega_utils
 * \defgroup assembler_group Assembler Support
 *
 * This group provides a good handful of macros intended to smooth out
 * the differences between various assemblers, similar to what compiler.h does
 * for compilers, except that assemblers tend to be much less standardized than
 * compilers.
 *
 * @{
 */

//! \name Control Statements
//@{
/**
 * \def REPEAT(count)
 * \brief Repeat the following statements \a count times
 */
/**
 * \def END_REPEAT()
 * \brief Mark the end of the statements to be repeated
 */
/**
 * \def SET_LOC(offset)
 * \brief Set the location counter to \a offset
 */
/**
 * \def END_FILE()
 * \brief Mark the end of the file
 */
//@}

//! \name Data Objects
//@{
/**
 * \def FILL_BYTES(count)
 * \brief Allocate space for \a count bytes
 */
//@}

//! \name Symbol Definition
//@{
/**
 * \def L(name)
 * \brief Turn \a name into a local symbol, if possible
 */
/**
 * \def EXTERN_SYMBOL(name)
 * \brief Declare \a name as an external symbol referenced by this file
 */
/**
 * \def FUNCTION(name)
 * \brief Define a file-local function called \a name
 */
/**
 * \def PUBLIC_FUNCTION(name)
 * \brief Define a globally visible function called \a name
 */
/**
 * \def WEAK_FUNCTION(name)
 * \brief Define a weak function called \a name
 *
 * Weak functions are only referenced if no strong definitions are found
 */
/**
 * \def WEAK_FUNCTION_ALIAS(name, strong_name)
 * \brief Define \a name as a weak alias for the function \a strong_name
 * \sa WEAK_FUNCTION
 */
/**
 * \def END_FUNC(name)
 * \brief Mark the end of the function called \a name
 */
//@}

//! \name Section Definition
//@{
/**
 * \def TEXT_SECTION(name)
 * \brief Start a new section containing executable code
 */
/**
 * \def RODATA_SECTION(name)
 * \brief Start a new section containing read-only data
 */
/**
 * \def DATA_SECTION(name)
 * \brief Start a new section containing writeable initialized data
 */
/**
 * \def BSS_SECTION(name)
 * \brief Start a new section containing writeable zero-initialized data
 */
//@}

//! @}

#endif /* ASSEMBLER_H_INCLUDED */
