/*****************************************************************************
 *
 * \file
 *
 * \brief ECC Hamming Reed-Solomon driver.
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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


#ifndef _ECCHRS_H_
#define _ECCHRS_H_

/**
 * \defgroup group_avr32_drivers_ecchrs MEMORY - ECCHRS - Error Corrected Code Controller
 *
 * Hardware Error Corrected Code Generation (Hamming and Reed-Solomon).
 *
 * \{
 */

#include "compiler.h"


/*! \name Type Correct mode
 */
//! @{
#define ECCHRS_TYPECORRECT_1_BIT_MODE_0   AVR32_ECCHRS_MD_TYPECORREC_ECC1_PAGE  //!< 1 bit correction for a page size of 512/1024/2048/4096 bytes.
#define ECCHRS_TYPECORRECT_1_BIT_MODE_1   AVR32_ECCHRS_MD_TYPECORREC_ECC1_256B  //!< 1 bit correction for 256 bytes of data for a page size of 512/2048/4096 bytes.
#define ECCHRS_TYPECORRECT_1_BIT_MODE_2   AVR32_ECCHRS_MD_TYPECORREC_ECC1_512B  //!< 1 bit correction for 512 bytes of data for a page size of 512/2048/4096 bytes.
#define ECCHRS_TYPECORRECT_4_BIT          AVR32_ECCHRS_MD_TYPECORREC_ECC4_512B  //!< 4 correction for 512 bytes of data for a page size of 512 to 4096 bytes (by 512 bytes).
//! @}

/*! \name Nand Flash Page Size when using ECCHRS_TYPECORRECT_1_BIT_MODE_x
 */
//! @{
#define ECCHRS_PAGESIZE_1_BIT_528_W       0 //!< Page of 528 words.
#define ECCHRS_PAGESIZE_1_BIT_1056_W      1 //!< Page of 1056 words.
#define ECCHRS_PAGESIZE_1_BIT_2112_W      2 //!< Page of 2112 words.
#define ECCHRS_PAGESIZE_1_BIT_4224_W      3 //!< Page of 4224 words.
//! @}

/*! \name Nand Flash Page Size when using ECCHRS_TYPECORRECT_4_BIT
 */
//! @{
#define ECCHRS_PAGESIZE_4_BIT_528_W       0 //!< Page of 528 words.
#define ECCHRS_PAGESIZE_4_BIT_1056_W      1 //!< Page of 1056 words.
#define ECCHRS_PAGESIZE_4_BIT_1584_W      2 //!< Page of 1584 words.
#define ECCHRS_PAGESIZE_4_BIT_2112_W      3 //!< Page of 2112 words.
#define ECCHRS_PAGESIZE_4_BIT_2640_W      4 //!< Page of 2640 words.
#define ECCHRS_PAGESIZE_4_BIT_3168_W      5 //!< Page of 3168 words.
#define ECCHRS_PAGESIZE_4_BIT_3696_W      6 //!< Page of 3696 words.
#define ECCHRS_PAGESIZE_4_BIT_4224_W      7 //!< Page of 4224 words.
#define ECCHRS_PAGESIZE_4_BIT_NR_W(x)     ((x)/528 -1)  //!< Page of (x) words.
#define ECCHRS_PAGESIZE_4_BIT_NR_S(x)     ((x) -1)      //!< Page of (x) sectors.
//! @}

//! Input parameters when initializing ECCHRS.
typedef struct
{
  //! Number of bits of the correction: \ref ECCHRS_TYPECORRECT_1_BIT_MODE_0,
  //! \ref ECCHRS_TYPECORRECT_1_BIT_MODE_1, \ref ECCHRS_TYPECORRECT_1_BIT_MODE_2 or
  //! \ref ECCHRS_TYPECORRECT_4_BIT.
  unsigned char typecorrect;

  //! Set page size of the Nand Flash.
  unsigned char pagesize;

} ecchrs_options_t;

/*!
 * \brief Freeze the ECCHRS controller.
 *
 * \param ecchrs   Base address of the ECCHRS instance.
 */
__always_inline static void ecchrs_freeze( volatile avr32_ecchrs_t* ecchrs )
{
  ecchrs->md |= AVR32_ECCHRS_MD_FREEZE_MASK;
}


/*!
 * \brief Unfreeze the ECCHRS controller.
 *
 * \param ecchrs   Base address of the ECCHRS instance.
 */
__always_inline static void ecchrs_unfreeze( volatile avr32_ecchrs_t* ecchrs )
{
  ecchrs->md &= ~AVR32_ECCHRS_MD_FREEZE_MASK;
}


/*!
 * \brief Reset the ECCHRS controller.
 *
 * \param ecchrs   Base address of the ECCHRS instance.
 */
__always_inline static void ecchrs_reset( volatile avr32_ecchrs_t* ecchrs )
{
  ecchrs->ctrl = AVR32_ECCHRS_CTRL_RST_MASK;
}


/*!
 * \brief Launch the 4-bit Reed Solomon correction.
 *
 * \param ecchrs   Base address of the ECCHRS instance.
 */
__always_inline static void ecchrs_correct_rs( volatile avr32_ecchrs_t* ecchrs )
{
  ecchrs->md |= AVR32_ECCHRS_MD_CORRS4_MASK;
}


/*! \brief Initialize the ECCHRS.
 *
 * \param ecchrs  Base address of the ECCHRS instance.
 * \param opt     Options needed to set up the ECCHRS (see \ref ecchrs_options_t).
 */
__always_inline static void ecchrs_init(volatile avr32_ecchrs_t* ecchrs, const ecchrs_options_t *opt)
{
  ecchrs->md =
    opt->pagesize    << AVR32_ECCHRS_MD_PAGESIZE_OFFSET
  | opt->typecorrect << AVR32_ECCHRS_MD_TYPECORREC_OFFSET
  | 0                << AVR32_ECCHRS_MD_CORRS4_OFFSET;
}


/*! \brief Get the ECCHRS codewords.
 *
 * \param ecchrs  Base address of the ECCHRS instance.
 * \param id      Codewords id to return.
 */
__always_inline static U32 ecchrs_get_cw(volatile avr32_ecchrs_t* ecchrs, U32 id)
{
  volatile U32* p_hecc_cw = (volatile U32* )&ecchrs->cwps00;
  return *(p_hecc_cw+id);
}


/*! \brief Check (in the 4-bit corrector mode) if an error is detected after reading
 *  one or more sectors from the Nand Flash.
 *
 * \param ecchrs  Base address of the ECCHRS instance.
 *
 * \return \c bitfield of the corrupted sector (bit 0: sector 0; bit 1: sector1, ../..).
 */
__always_inline static U32 ecchrs_4bit_check_error(volatile avr32_ecchrs_t* ecchrs)
{
  return ecchrs->sr1;
}

/**
 * \}
 */

#endif // _ECCHRS_H_
