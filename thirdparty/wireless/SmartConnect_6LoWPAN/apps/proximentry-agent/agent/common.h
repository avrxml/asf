/*******************************************************************************
 *
 * Copyright (c) 2014 Proximetry Inc. All rights reserved.
 * Authors: Proximetry
 * All Rights Reserved
 *
 ******************************************************************************/
/**
 * @file common.h
 * @brief
 * */


#ifndef COMMON_H
#define COMMON_H

#include "conf_logging.h"

/**
 * @brief get number of elements of an array
 */
#ifndef ARR_SIZE
	#define ARR_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#endif
#ifndef ARRAY_ELEMS
	#define ARRAY_ELEMS(arr) ARR_SIZE(arr)
#endif

/*******************************************************************************
 * GCC Optimization
 *******************************************************************************/

#if (!defined(__GNUC__) || (__GNUC__ == 2 && __GNUC_MINOR__ < 96)) && !defined(__builtin_expect)
	#define __builtin_expect(_x2, expected_value) (_x2)
#endif

#ifndef likely
	#define likely(_x1)   __builtin_expect((_x1), 1)
#endif
#ifndef unlikely
	#define unlikely(_x1) __builtin_expect((_x1), 0)
#endif

#ifndef ATTR
	#if defined(__GNUC__) || defined(__clang__)
		#define ATTR(...)  __attribute__((__VA_ARGS__))
	#else
		#define ATTR(...)
	#endif
#endif


#ifndef _PURE_
	#define _PURE_        ATTR(pure)
#endif
#ifndef _CONST_
	#define _CONST_       ATTR(const)
#endif
#ifndef _NO_RETURN_
	#define _NO_RETURN_   ATTR(noreturn)
#endif
#ifndef _UNUSED_
	#define _UNUSED_      ATTR(unused)
#endif
#ifndef _FORMAT_
	#define _FORMAT_(archetype, string_index, first_to_check) ATTR(format(archetype, string_index, first_to_check))
#endif

#ifndef TO_STRING
	#define TO_STRING(str)    #str
#endif


#ifndef ASSERT
	#if defined(NDEBUG)
		#if defined(__GNUC__) || defined(__clang__)
			#define ASSERT(expr)                 \
			    do                               \
			    {                                \
			        if (!(expr))                 \
			            __builtin_unreachable(); \
			    } while(0)
	#else
	#define ASSERT(expr)
	#endif
	#else
		#include <assert.h>
		#define ASSERT(expr) assert((expr))
	#endif
#endif

#ifndef STATIC_ASSERT
	#define STATIC_ASSERT(x) { int static_assert_error[0 == (x) ? -1 : 1]; static_assert_error[0] = 1; UNUSED(static_assert_error); }
#endif
#ifndef STATIC_ASSERT_GLOBAL
	#define STATIC_ASSERT_GLOBAL(x) int static_assert_error[0 == (x) ? -1 : 1];
#endif

#ifndef MIN
	#define MIN(a, b) ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
	#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif


#define PRINT_MAC_FORMAT      "%02X:%02X:%02X:%02X:%02X:%02X"
#define PRINT_MAC_FORMAT_2    "%02X-%02X-%02X-%02X-%02X-%02X"
#define PRINT_MAC_FORMAT_3    "%02X%02X%02X%02X%02X%02X"
#define PRINT_MAC_VALUES(mac)  (mac)[0], (mac)[1], (mac)[2], (mac)[3], (mac)[4], (mac)[5]
#define PRINT_EUI64_FORMAT_3    "%02X%02X%02X%02X%02X%02X%02X%02X"
#define PRINT_EUI64_VALUES(eui64)  (eui64)[0], (eui64)[1], (eui64)[2], (eui64)[3], (eui64)[4], (eui64)[5] , (eui64)[6], (eui64)[7] 

#define SCANF_MAC_FORMAT      "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx"
#define SCANF_MAC_FORMAT_2    "%02hhx-%02hhx-%02hhx-%02hhx-%02hhx-%02hhx"
#define SCANF_MAC_VALUES(mac)  (mac), (mac) + 1, (mac) + 2, (mac) + 3, (mac) + 4, (mac) + 5
#define SCANF_EUI64_VALUES(eui64)  (eui64), (eui64) + 1, (eui64) + 2, (eui64) + 3, (eui64) + 4, (eui64) + 5  , (eui64) + 6, (eui64) + 7

#define PRINT_IP_FORMAT       "%d.%d.%d.%d"
#define PRINT_IP_VALUES(ip)    (ip)[0], (ip)[1], (ip)[2], (ip)[3]

#define SCANF_IP_FORMAT       "%hhd.%hhd.%hhd.%hhd"
#define SCANF_IP_VALUES(ip)    (ip), (ip) + 1, (ip) + 2, (ip) + 3


#endif /* COMMON_H */
