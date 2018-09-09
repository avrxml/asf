/*****************************************************************************
 *
 * \file
 *
 * \brief NEWLIB_ADDONS miscellaneous macros include file for AVR32.
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
 ******************************************************************************/


#ifndef __AVR32_NEWLIB_ADDONS_IO_H__
#define __AVR32_NEWLIB_ADDONS_IO_H__

#include <_ansi.h>

_BEGIN_STD_C

typedef char u8;
typedef unsigned int u32;

#define __raw_writeb(v,a)       (*(volatile unsigned char  *)(a) = (v))
#define __raw_writew(v,a)       (*(volatile unsigned short *)(a) = (v))
#define __raw_writel(v,a)       (*(volatile unsigned int   *)(a) = (v))

#define __raw_readb(a)          (*(volatile unsigned char  *)(a))
#define __raw_readw(a)          (*(volatile unsigned short *)(a))
#define __raw_readl(a)          (*(volatile unsigned int   *)(a))

/* As long as I/O is only performed in P4 (or possibly P3), we're safe */
#define writeb(v,a)		__raw_writeb(v,a)
#define writew(v,a)		__raw_writew(v,a)
#define writel(v,a)		__raw_writel(v,a)

#define readb(a)		__raw_readb(a)
#define readw(a)		__raw_readw(a)
#define readl(a)		__raw_readl(a)

/* Memory segments when segmentation is enabled */
#define P0SEG		0x00000000
#define P1SEG		0x80000000
#define P2SEG		0xa0000000
#define P3SEG		0xc0000000
#define P4SEG		0xe0000000

/* Returns the privileged segment base of a given address */
#define PXSEG(a)	(((unsigned long)(a)) & 0xe0000000)

/* Returns the physical address of a PnSEG (n=1,2) address */
#define PHYSADDR(a)	(((unsigned long)(a)) & 0x1fffffff)

/*
 * Map an address to a certain privileged segment
 */
#define P1SEGADDR(a) ((__typeof__(a))(((unsigned long)(a) & 0x1fffffff) | P1SEG))
#define P2SEGADDR(a) ((__typeof__(a))(((unsigned long)(a) & 0x1fffffff) | P2SEG))
#define P3SEGADDR(a) ((__typeof__(a))(((unsigned long)(a) & 0x1fffffff) | P3SEG))
#define P4SEGADDR(a) ((__typeof__(a))(((unsigned long)(a) & 0x1fffffff) | P4SEG))


#define cached(addr) P1SEGADDR(addr)
#define uncached(addr) P2SEGADDR(addr)
#define physaddr(addr) PHYSADDR(addr)

#define BF(field, value) \
  ({ union { \
      struct { \
       unsigned           : 32 - field ##  _OFFSET -  field ##  _SIZE ; \
       unsigned long __val: field ##  _SIZE ; \
      }; \
      unsigned long __ul; \
     } __tmp; \
     __tmp.__ul = 0; \
     __tmp.__val = value; \
     __tmp.__ul;})

#define BF_D(field, value) \
  ({ union { \
      struct { \
       unsigned long long : 64 - field ##  _OFFSET -  field ##  _SIZE ; \
       unsigned long long __val: field ##  _SIZE ; \
      }; \
      unsigned long long __ul; \
     } __tmp; \
     __tmp.__ul = 0; \
     __tmp.__val = value; \
     __tmp.__ul;})

#define BFINS(var, field, value) \
  { union {\
      struct { \
       unsigned           : 32 - field ##  _OFFSET -  field ##  _SIZE ; \
       unsigned long __val: field ##  _SIZE ; \
      }; \
      unsigned long __ul; \
     } __tmp; \
     __tmp.__ul = var; \
     __tmp.__val = value; \
     var = __tmp.__ul;}

#define BFEXT(var, field) \
  ({ union {\
      struct { \
       unsigned           : 32 - field ##  _OFFSET -  field ##  _SIZE ; \
       unsigned long __val: field ##  _SIZE ; \
      }; \
      unsigned long __ul; \
     } __tmp; \
     __tmp.__ul = var; \
     __tmp.__val; })

#define BFINS_D(var, field, value) \
  { union {\
      struct { \
       unsigned long long : 64 - field ##  _OFFSET -  field ##  _SIZE ; \
       unsigned long long __val: field ##  _SIZE ; \
      }; \
      unsigned long long __ul; \
     } __tmp; \
     __tmp.__ul = var; \
     __tmp.__val = value; \
     var = __tmp.__ul;}

#define BFEXT_D(var, field) \
  ({ union {\
      struct { \
       unsigned long long : 64 - field ##  _OFFSET -  field ##  _SIZE ; \
       unsigned long long __val: field ##  _SIZE ; \
      }; \
      unsigned long long __ul; \
     } __tmp; \
     __tmp.__ul = var; \
     __tmp.__val; })


_END_STD_C

#endif
