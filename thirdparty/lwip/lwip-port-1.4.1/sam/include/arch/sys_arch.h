/**
 *
 * \file
 *
 * \brief lwIP abstraction layer for SAM.
 *
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
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

