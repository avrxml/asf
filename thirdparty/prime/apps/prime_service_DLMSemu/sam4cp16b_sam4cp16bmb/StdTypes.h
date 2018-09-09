/**
 * \file
 *
 * \brief Hardware Abstraction Layer. PRIME stack standard types
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
