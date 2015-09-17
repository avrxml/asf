/*****************************************************************************
 *
 * \file
 *
 * \brief Basic header that describes the compiler and AVR32 UC3 to lwIP.
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


#ifndef __ARCH_CC_H__
#define __ARCH_CC_H__

/* Include some files for defining library routines */
/*#include <string.h>
#include <sys/time.h>*/

/* Define platform endianness */
#ifndef BYTE_ORDER
#define BYTE_ORDER BIG_ENDIAN
#endif /* BYTE_ORDER */

/* Define generic types used in lwIP */
typedef unsigned   char    u8_t;
typedef signed     char    s8_t;
typedef unsigned   short   u16_t;
typedef signed     short   s16_t;
typedef unsigned   long    u32_t;
typedef signed     long    s32_t;

typedef u32_t mem_ptr_t;

/* Define (sn)printf formatters for these lwIP types */
#define U16_F "hu"
#define S16_F "hd"
#define X16_F "hx"
#define U32_F "lu"
#define S32_F "ld"
#define X32_F "lx"

/* Compiler hints for packing structures */
#define PACK_STRUCT_FIELD(x) x

#if defined(__GNUC__)
#define PACK_STRUCT_STRUCT __attribute__ ((__packed__))
#elif defined(__ICCAVR32__)
#define PACK_STRUCT_STRUCT
#endif

#if defined(__GNUC__)
#define PACK_STRUCT_BEGIN
#elif defined(__ICCAVR32__)
#define PACK_STRUCT_BEGIN _Pragma("pack(1)")
#endif

#if defined(__GNUC__)
#define PACK_STRUCT_END
#elif defined(__ICCAVR32__)
#define PACK_STRUCT_END _Pragma("pack()")
#endif

/* define LWIP_COMPAT_MUTEX
    to let sys.h use binary semaphores instead of mutexes - as before in 1.3.2
    Refer CHANGELOG
*/
#define  LWIP_COMPAT_MUTEX  1

/* Plaform specific diagnostic output */

/* Include some files for defining library routines */

/*#include <stdio.h>
#include "print_funcs.h"
#define LWIP_PLATFORM_DIAG(x) { char tmpbuf[256];sprintf(tmpbuf,(char *)x);print_dbg(tmpbuf);}*/

#define LWIP_PLATFORM_DIAG(x)

#define LWIP_PLATFORM_ASSERT(x) do {/*printf("Assertion \"%s\" failed at line %d in %s\n", x, __LINE__, __FILE__); \
                                      fflush(NULL); abort();*/} while(0)

#define LWIP_PROVIDE_ERRNO // Make lwip/arch.h define the codes which are used throughout.
#endif /* __ARCH_CC_H__ */
