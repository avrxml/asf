/**
 * \file
 *
 * \brief BLE Utils.
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
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

#ifndef BLE_UTILS_H_INCLUDED
#define BLE_UTILS_H_INCLUDED

#include <asf.h>

#define BLE_CENTRAL					(0x01)
#define BLE_PERIPHERAL				(0x02)
#define BLE_CENTRAL_AND_PERIPHERAL	(0x03)
#define BLE_OBSERVER				(0x04)
#define BLE_BROADCASTER				(0x05)

#define DBG_LOG_CONT	printf

#define DBG_LOG		    printf("\r\n");\
						printf
						
#define UNUSED1(x) (void)(x)
#define UNUSED2(x,y) (void)(x),(void)(y)
#define UNUSED3(x,y,z) (void)(x),(void)(y),(void)(z)
#define UNUSED4(a,x,y,z) (void)(a),(void)(x),(void)(y),(void)(z)
#define UNUSED5(a,b,x,y,z) (void)(a),(void)(b),(void)(x),(void)(y),(void)(z)
#define UNUSED6(a,b,c,x,y,z) (void)(a),(void)(b),(void)(c),(void)(x),(void)(y),(void)(z)
#define UNUSED7(a,b,c,d,x,y,z) (void)(a),(void)(b),(void)(c),(void)(d),(void)(x),(void)(y),(void)(z)
#define UNUSED8(a,b,c,d,e,x,y,z) (void)(a),(void)(b),(void)(c),(void)(d),(void)(e),(void)(x),(void)(y),(void)(z)

#define VA_NUM_ARGS_IMPL(_1,_2,_3,_4,_5,_6,_7,_8, N,...) N
#define VA_NUM_ARGS(...) VA_NUM_ARGS_IMPL(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1)

#define ALL_UNUSED_IMPL_(nargs) UNUSED ## nargs
#define ALL_UNUSED_IMPL(nargs) ALL_UNUSED_IMPL_(nargs)
#define ALL_UNUSED(...) ALL_UNUSED_IMPL( VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__ )

#define DBG_LOG_DEV		ALL_UNUSED

						
#define IEEE11073_EXPONENT						(0xFF000000)

#define IEEE754_MANTISA(val)					((uint32_t)(val * 10))

#define IEEE754_TO_IEEE11073_FLOAT(f_val)		(IEEE11073_EXPONENT | \
												IEEE754_MANTISA(f_val))

static inline uint32_t convert_ieee754_ieee11073_float(float f_val)
{
	uint32_t ieee11073_float;
	ieee11073_float = IEEE754_TO_IEEE11073_FLOAT(f_val);
	return (ieee11073_float);
}

#endif /* BLE_UTILS_H_INCLUDED */
