/*****************************************************************************
 *
 * \file
 *
 * \brief Basic header that describes the compiler and AVR32 UC3 to lwIP.
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
