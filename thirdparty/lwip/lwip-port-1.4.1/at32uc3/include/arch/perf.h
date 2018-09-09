/*****************************************************************************
 *
 * \file
 *
 * \brief lwIP abstraction layer for AVR32 UC3.
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


#ifndef __ARCH_PERF_H__
#define __ARCH_PERF_H__


#ifdef PERF
/*
#include <sys/times.h>
#define PERF_START do { \
                     struct tms __perf_start, __perf_end; \
                     times(&__perf_start)
#define PERF_STOP(x) times(&__perf_end); \
                     perf_print_times(&__perf_start, &__perf_end, x);\
                     } while(0)

void perf_print(unsigned long c1l, unsigned long c1h,
		unsigned long c2l, unsigned long c2h,
		char *key);

void perf_print_times(struct tms *start, struct tms *end, char *key);

void perf_init(char *fname);
*/
#else /* PERF */
#define PERF_START    /* null definition */
#define PERF_STOP(x)  /* null definition */
#endif /* PERF */



#endif /* __ARCH_PERF_H__ */
