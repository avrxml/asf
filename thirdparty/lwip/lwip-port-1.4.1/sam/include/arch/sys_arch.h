/**
 *
 * \file
 *
 * \brief lwIP abstraction layer for SAM.
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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

#ifndef __ARCH_SYS_ARCH_H__
#define __ARCH_SYS_ARCH_H__

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


typedef xSemaphoreHandle sys_sem_t;
typedef xQueueHandle sys_mutex_t;
typedef xQueueHandle sys_mbox_t;
typedef xTaskHandle sys_thread_t;

#define SYS_MBOX_NULL (xQueueHandle)0
#define SYS_SEM_NULL  (xSemaphoreHandle)0

typedef unsigned long int sys_prot_t;

/* This optional function does a "fast" critical region protection and returns
the previous protection level. This function is only called during very short
critical regions. An embedded system which supports ISR-based drivers might want
to implement this function by disabling interrupts. Task-based systems might want
to implement this by using a mutex or disabling tasking. This function should
support recursive calls from the same task or interrupt. In other words,
sys_arch_protect() could be called while already protected. In that case the
return value indicates that it is already protected. */
extern sys_prot_t sys_arch_protect(void);

/* This optional function does a "fast" set of critical region protection to the
value specified by pval. See the documentation for sys_arch_protect() for more
information. This function is only required if your port is supporting an
operating system. */
extern void sys_arch_unprotect(sys_prot_t pval);

#endif /* __ARCH_SYS_ARCH_H__ */

