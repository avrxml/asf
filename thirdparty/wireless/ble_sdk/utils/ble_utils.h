/**
* \file
*
* \brief BLE Utils declarations
*
* Copyright (c) 2017-2018 Microchip Technology Inc. and its subsidiaries.
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

#ifndef __BLE_UTILS_H__
#define __BLE_UTILS_H__

#include <asf.h>

/* Supported BLE Modules */
#define BTLC1000_MR		0x00
#define BTLC1000_ZR		0x01

	/// BluSDK version
#define BLE_SDK_MAJOR_NO(version)	((version >> 28) & 0x0000000F)
#define BLE_SDK_MINOR_NO(version)	((version >> 24) & 0x0000000F)
#define BLE_SDK_BUILD_NO(version)	(version & 0x0000FFFF)
#define BLUSDK_VER_6_0	(0x60000000)
#define BLUSDK_VER_6_1  (0x61000000)

/* BluSDK Minor version Changes (If this value is non-zero then Library and API have No changes.) */
#define BLE_SDK_MINOR_NO_INC	(1)

#define BLE_SDK_VERSION BLUSDK_VER_6_1

    /// Observer role
#define   BLE_ROLE_OBSERVER   0x01
    /// Broadcaster role
#define   BLE_ROLE_BROADCASTER 0x02
    /// Master/Central role
#define   BLE_ROLE_CENTRAL     0x05
    /// Peripheral/Slave role
#define   BLE_ROLE_PERIPHERAL  0x0A
    /// Device has all role, both peripheral and central
#define   BLE_ROLE_ALL         0x0F

#define BLE_ASSERT		(true)

#if (BLE_ASSERT == true)
#define ble_assert(expr) \
{\
	if (!(expr)) while (true);\
}
#else
#define ble_assert(expr) ((void) 0)
#endif
						
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

#if defined DEBUG_LOG_DISABLED
	#define DBG_LOG_CONT	ALL_UNUSED
	#define DBG_LOG		    ALL_UNUSED
	#define DBG_LOG_ADV	    ALL_UNUSED
#else
	#define DBG_LOG_CONT	printf
	#define DBG_LOG		    printf("\r\n");\
							printf
	#define DBG_LOG_ADV	    printf("\r\nBLE-ADV: ");\
							printf
#endif

#ifdef ENABLE_PTS
#define DBG_LOG_PTS			DBG_LOG
#else
#define DBG_LOG_PTS			ALL_UNUSED
#endif

#define DBG_LOG_DEV			ALL_UNUSED
#define DBG_LOG_CONT_DEV	ALL_UNUSED

#define BLU_SDK_API
						
#define IEEE11073_EXPONENT						(0xFF000000)

#define IEEE754_MANTISA(val)					((uint32_t)(val * 10))

#define IEEE754_TO_IEEE11073_FLOAT(f_val)		(IEEE11073_EXPONENT | \
												IEEE754_MANTISA(f_val))
												
#define NIBBLE2ASCII(nibble) (((nibble < 0x0A) ? (nibble + '0') : (nibble + 0x57)))

static inline void dump_hex_buffer(void *buf, uint32_t len)
{
	uint32_t idx;
	uint8_t *buffer = (uint8_t *)buf;
	for (idx = 0; idx < len; ++idx)
	{
		DBG_LOG_CONT("0x%X ", *buffer++);
	}
}

static inline uint32_t convert_ieee754_ieee11073_float(float f_val)
{
	uint32_t ieee11073_float;
	ieee11073_float = IEEE754_TO_IEEE11073_FLOAT(f_val);
	return (ieee11073_float);
}

#ifndef COSOLE_SERIAL_H
extern uint8_t getchar_timeout(uint32_t timeout);
#endif

#endif /*__BLE_UTILS_H__*/
