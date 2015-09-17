/**
 * \file
 *
 * \brief Hardware Abstraction Layer. PRIME stack standard types
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

#ifndef STDTYPES_H_INCLUDED
#define STDTYPES_H_INCLUDED

#include "compiler.h"

/* / @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/* / @endcond */

#if defined(_lint) || defined (_cvision)
#define far
#define near
#endif

#define EXTERN          extern
#define KC51_REENT
#define KC51_XDATA
#define KC51_CODE       const

#define OK                      0
#define OFF                     0
#define ON                      1

#ifdef ERROR
#undef ERROR
#endif
#define ERROR           (-1)

#ifndef ERR
#define ERR                     (-1)
#endif

#ifndef true
#define true            1
#endif

#ifndef false
#define false           0
#endif

#ifndef TRUE
#define TRUE            true
#endif

#ifndef FALSE
#define FALSE           false
#endif

#ifndef NULL
#define NULL            ((void *)0L)
#endif

typedef signed char Int8;
typedef unsigned char Uint8;
typedef signed short Int16;
typedef unsigned short Uint16;
typedef signed long Int32;
typedef unsigned long Uint32;
typedef volatile unsigned char Vuint8;
/* typedef unsigned char				CriticalStatus; */

#ifndef KC51_PTYPE
  #define KC51_PTYPE                            Uint32
#endif

/* / @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/* / @endcond */

#endif /* STDTYPES_H_INCLUDED */
