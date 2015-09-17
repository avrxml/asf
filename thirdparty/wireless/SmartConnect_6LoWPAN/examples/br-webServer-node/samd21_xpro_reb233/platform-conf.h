/*
 * Copyright (c) 2013, Thingsquare, http://www.thingsquare.com/.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef PLATFORM_CONF_H
#define PLATFORM_CONF_H

#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include "thsq-conf-const.h"
/* 
 * This is the platform configuration file for Atmel SAM D21 platform.
 */
#if RDC==CONTIKIMAC
#define F_CPU                           8000000ul
#else
#define F_CPU                           48000000ul
#endif

#define CLOCK_CONF_SECOND               128
#define RTIMER_ARCH_SECOND              31250
#define RTIMER_CLOCK_LT(a,b)            ((signed short)((a)-(b)) < 0)
typedef unsigned long                   clock_time_t;
typedef uint32_t                        rtimer_clock_t;
typedef unsigned short                  uip_stats_t;

#define CC_CONF_REGISTER_ARGS           0
#define CC_CONF_FUNCTION_POINTER_ARGS   1
#define CC_CONF_FASTCALL
#define CC_CONF_NO_VA_ARGS   0
#define CC_NO_VA_ARGS   0
#define CC_CONF_DOUBLE_HASH   0
#define CC_CONF_VA_ARGS                 1
#define CC_CONF_UNSIGNED_CHAR_BUGS	0
#define CC_CONF_INLINE                  inline
#define CCIF
#define CLIF

/**
 * Adjust the system current clock time.
 * \param dt   How many ticks to add
 *
 * Typically used to add ticks after an MCU sleep
 * clock_seconds will increment if necessary to reflect the tick addition.
  * Leap ticks or seconds can (rarely) be introduced if the ISR is not blocked.
 */
void
clock_adjust_ticks(clock_time_t howmany);


#include "rf233.h"
#define MULTICHAN_CONF_SET_CHANNEL(x)     rf_set_channel(x)
#define MULTICHAN_CONF_READ_RSSI(x)       0

/*---------------------------------------------------------------------------*/
#define EXT_HEADER_1   1
#define EXT_HEADER_2   2
#define EXT_HEADER_3   3
#define SAMR21_SIP     4

/* 
 * Define the pin extension header on the SAM R21 board that the
 * expansion boards are connected to. On the SAM R21 the radio is
 * connected inside the package, so no extension headers are used.
 */
/* Radio board */
#undef REB233XPRO_HEADER
#define REB233XPRO_HEADER     SAMR21_SIP
/*---------------------------------------------------------------------------*/
#endif /* PLATFORM_CONF_H */
