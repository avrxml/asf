/*****************************************************************************
*
* \file
*
* \brief lwIP system architecture layer for SAM.
*
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
*****************************************************************************/

#include "lwip/debug.h"
#include "lwip/sys.h"
#include "lwip/opt.h"
#include "lwip/stats.h"

#define SYS_ARCH_BLOCKING_TICKTIMEOUT    ((portTickType)10000)

/* Structure associating a thread to a struct sys_timeouts */
struct TimeoutlistPerThread {
	struct sys_timeouts timeouts; /* holds a pointer to a linked list of
	                               * timeouts */
	sys_thread_t pid;        /* The thread id. */
};

/* Thread & struct sys_timeouts association statically allocated per thread.
 * Note: SYS_THREAD_MAX is the max number of thread created by sys_thread_new()
 * that can run simultaneously; it is defined in conf_lwip_threads.h. */
static struct TimeoutlistPerThread Threads_TimeoutsList[SYS_THREAD_MAX];

/* Number of active threads. */
static u16_t NbActiveThreads = 0;

/**
 * \brief Initialize the sys_arch layer.
 */
void sys_init(void)
{
	int i;

	/* Initialize the the per-thread sys_timeouts structures */
	/* make sure there are no valid pids in the list */
	for (i = 0; i < SYS_THREAD_MAX; i++) {
		Threads_TimeoutsList[i].pid = 0;
		Threads_TimeoutsList[i].timeouts.next = NULL;
	}

	/* Keep track of how many threads have been created */
	NbActiveThreads = 0;
}

/**
 * \brief Creates and returns a new semaphore.
 *
 * \param count Initial state of the semaphore.
 *
 * \return ERR_OK for OK, other value indicates error.
 */
sys_sem_t sys_sem_new(u8_t count)
{
	sys_sem_t xSemaphore = SYS_SEM_NULL;

	portENTER_CRITICAL();

	vSemaphoreCreateBinary( xSemaphore );
	if (xSemaphore != SYS_SEM_NULL) {
#if SYS_STATS
		lwip_stats.sys.sem.used++;
		if (lwip_stats.sys.sem.used > lwip_stats.sys.sem.max) {
			lwip_stats.sys.sem.max = lwip_stats.sys.sem.used;
		}
#endif /* SYS_STATS */
		/* Means we want the sem to be unavailable at init state. */
		if (0 == count) {
			xSemaphoreTake( xSemaphore, 1);
		}
	}

	portEXIT_CRITICAL();

	return xSemaphore;
}

/**
 * \brief Frees a semaphore created by sys_sem_new.
 *
 * \param sem Semaphore number.
 */
void sys_sem_free(sys_sem_t sem)
{
	if (SYS_SEM_NULL != sem) {
#ifdef SYS_STATS
		lwip_stats.sys.sem.used--;
#endif /* SYS_STATS */
		vQueueDelete( sem );
	}
}

/**
 * \brief Signals (or releases) a semaphore.
 *
 * \param sem Semphore number.
 */
void sys_sem_signal(sys_sem_t sem)
{
	xSemaphoreGive( sem );
}

/**
 * \brief Blocks the thread while waiting for the semaphore to be signaled.
 * Note that there is another function sys_sem_wait in sys.c, but it is a wrapper
 * for the sys_arch_sem_wait function. Please note that it is important for the
 * semaphores to return an accurate count of elapsed milliseconds, since they are
 * used to schedule timers in lwIP.
 *
 * \param sem Semphore number.
 * \param timeout The timeout parameter specifies how many milliseconds the
 * function should block before returning; if the function times out, it should
 * return SYS_ARCH_TIMEOUT. If timeout=0, then the function should block
 * indefinitely. If the function acquires the semaphore, it should return how
 * many milliseconds expired while waiting for the semaphore.
 *
 * \return SYS_ARCH_TIMEOUT if times out, otherwise return the milliseconds
 * expired while waiting for the semaphore.
 */
u32_t sys_arch_sem_wait(sys_sem_t sem, u32_t timeout)
{
	portTickType TickStart;
	portTickType TickStop;
	/* Express the timeout in OS tick. */
	portTickType TickElapsed = (portTickType)(timeout / portTICK_RATE_MS);

	if (timeout && !TickElapsed) {
		TickElapsed = 1; /* Wait at least one tick */
	}

	/* NOTE: we assume sem != SYS_SEM_NULL; iow, we assume the calling
	 * function */
	/* takes care of checking the sem validity before calling this function. */

	if (0 == TickElapsed) {
		TickStart = xTaskGetTickCount();
		/* If timeout=0, then the function should block indefinitely. */
		while (pdFALSE ==
				xSemaphoreTake( sem,
				SYS_ARCH_BLOCKING_TICKTIMEOUT )) {
		}
	} else {
		TickStart = xTaskGetTickCount();
		if (pdFALSE == xSemaphoreTake( sem, TickElapsed )) {
			/* if the function times out, it should return
			 * SYS_ARCH_TIMEOUT. */
			return(SYS_ARCH_TIMEOUT);
		}
	}

	/* If the function acquires the semaphore, it should return how many
	 * milliseconds */
	/* expired while waiting for the semaphore. */
	TickStop = xTaskGetTickCount();
	/* Take care of wrap-around. */
	if (TickStop >= TickStart) {
		TickElapsed = TickStop - TickStart;
	} else {
		TickElapsed = portMAX_DELAY - TickStart + TickStop;
	}

	return(TickElapsed * portTICK_RATE_MS);
}

/* ----------- MAILBOXES
 * -------------------------------------------------------- */

/**
 * \brief Creates an empty mailbox for maximum "size" elements. Elements stored
 * in mailboxes are pointers.
 *
 * \param size Maximum "size" elements.
 *
 * \return Mailbox created.
 */
sys_mbox_t sys_mbox_new( int size )
{
	sys_mbox_t mBoxNew;

	mBoxNew = xQueueCreate( size, sizeof(void *));
#if SYS_STATS
	if (SYS_MBOX_NULL != mBoxNew) {
		lwip_stats.sys.mbox.used++;
		if (lwip_stats.sys.mbox.used > lwip_stats.sys.mbox.max) {
			lwip_stats.sys.mbox.max = lwip_stats.sys.mbox.used;
		}
	}
#endif /* SYS_STATS */
	return(mBoxNew);
}

/**
 * \brief Deallocates a mailbox.
 * If there are messages still present in the mailbox when the mailbox is
 * deallocated, it is an indication of a programming error in lwIP and the
 * developer should be notified.
 *
 * \param mbox Mailbox to be used.
 */
void sys_mbox_free(sys_mbox_t mbox)
{
	if (SYS_MBOX_NULL != mbox) {
#ifdef SYS_STATS
		lwip_stats.sys.mbox.used--;
#endif /* SYS_STATS */
		vQueueDelete( mbox );
	}
}

/**
 * \brief Posts the "msg" to the mailbox. This function have to block until the
 * "msg" is really posted.
 *
 * \param mbox Mailbox used.
 * \param msg Pointer to the message to be post.
 */
void sys_mbox_post(sys_mbox_t mbox, void *msg)
{
	/* NOTE: we assume mbox != SYS_MBOX_NULL; iow, we assume the calling
	 * function */

	/* takes care of checking the mbox validity before calling this
	 * function. */
	while (pdTRUE !=
			xQueueSend( mbox, &msg,
			SYS_ARCH_BLOCKING_TICKTIMEOUT )) {
	}
}

/**
 * \brief Try to posts the "msg" to the mailbox.
 *
 * \param mbox Mailbox used.
 * \param msg Pointer to the message to be post.
 *
 * \return ERR_MEM if the mailbox is full otherwise ERR_OK if the "msg" is
 * posted.
 */
err_t sys_mbox_trypost(sys_mbox_t mbox, void *msg)
{
	if (errQUEUE_FULL == xQueueSend( mbox, &msg, 0 )) {
		return(ERR_MEM);
	} else {
		return(ERR_OK);
	}
}

/**
 * \brief Blocks the thread until a message arrives in the mailbox, but does not
 * block the thread longer than "timeout" milliseconds (similar to the
 * sys_arch_sem_wait() function).
 *
 * \param mbox Mailbox used.
 * \param msg A result parameter that is set by the function (i.e., by doing
 * "*msg = ptr"). The "msg" parameter maybe NULL to indicate that the message
 * should be dropped.
 * \timeout 0 indicates the thread should be blocked until a message arrives.
 *
 * \return Number of milliseconds spent waiting or SYS_ARCH_TIMEOUT if there was
 * a timeout. Or ERR_MEM if invalid pointer to message box.
 */
u32_t sys_arch_mbox_fetch(sys_mbox_t mbox, void **msg, u32_t timeout)
{
	portTickType TickStart;
	portTickType TickStop;
	void *tempoptr;
	/* Express the timeout in OS tick. */
	portTickType TickElapsed = (portTickType)(timeout / portTICK_RATE_MS);

	if (timeout && !TickElapsed) {
		TickElapsed = 1; /* Wait at least one tick */
	}

	/* NOTE: we assume mbox != SYS_MBOX_NULL; iow, we assume the calling
	 * function */

	/* takes care of checking the mbox validity before calling this
	 * function. */

	if (msg == NULL) {
		msg = &tempoptr;
	}

	/* NOTE: INCLUDE_xTaskGetSchedulerState must be set to 1 in
	 * FreeRTOSConfig.h */
	/* for xTaskGetTickCount() to be available. */
	if (0 == TickElapsed) {
		TickStart = xTaskGetTickCount();

		/* If "timeout" is 0, the thread should be blocked until a
		 * message arrives. */
		while (pdFALSE ==
				xQueueReceive( mbox, &(*msg),
				SYS_ARCH_BLOCKING_TICKTIMEOUT )) {
		}
	} else {
		TickStart = xTaskGetTickCount();
		if (pdFALSE == xQueueReceive( mbox, &(*msg), TickElapsed )) {
			*msg = NULL;

			/* if the function times out, it should return
			 * SYS_ARCH_TIMEOUT. */
			return(SYS_ARCH_TIMEOUT);
		}
	}

	/* If the function gets a msg, it should return the number of ms spent
	 * waiting. */
	TickStop = xTaskGetTickCount();
	/* Take care of wrap-around. */
	if (TickStop >= TickStart) {
		TickElapsed = TickStop - TickStart;
	} else {
		TickElapsed = portMAX_DELAY - TickStart + TickStop;
	}

	return(TickElapsed * portTICK_RATE_MS);
}

/**
 * \brief This is similar to sys_arch_mbox_fetch, however if a message is not
 * present in the mailbox, it immediately returns with the code SYS_MBOX_EMPTY.
 * On success 0 is returned.
 *
 * \param mbox Mailbox used.
 * \param msg A result parameter that is set by the function (i.e., by doing
 * "*msg = ptr"). The "msg" parameter maybe NULL to indicate that the message
 * should be dropped.
 *
 * \return Number of milliseconds spent waiting or SYS_ARCH_TIMEOUT if there was
 * a timeout. Or ERR_MEM if invalid pointer to message box.
 */
u32_t sys_arch_mbox_tryfetch(sys_mbox_t mbox, void **msg)
{
	void *tempoptr;

	if (msg == NULL) {
		msg = &tempoptr;
	}

	if (pdFALSE == xQueueReceive( mbox, &(*msg), 0 )) {
		/* if a message is not present in the mailbox, it immediately
		 * returns with */
		/* the code SYS_MBOX_EMPTY. */
		return(SYS_MBOX_EMPTY);
	}

	/* On success 0 is returned. */
	return(0);
}

/**
 * \brief Get a pointer to the per-thread sys_timeouts structure.
 * In lwIP, each thread has a list of timeouts which is repressented as a linked
 * list of sys_timeout structures. The sys_timeouts structure holds a pointer to
 * a linked list of timeouts. This function is called by the lwIP timeout
 * scheduler and must not return a NULL value.
 *
 * \return Pointer to the per-thread sys_timeouts structure.
 */
struct sys_timeouts *sys_arch_timeouts(void)
{
	int i;
	sys_thread_t pid;
	struct TimeoutlistPerThread *tl;

	/* Get the current thread id. */

	/* Note: INCLUDE_xTaskGetCurrentTaskHandle must be set to 1 in
	 * FreeRTOSConfig.h */
	/* for xTaskGetCurrentTaskHandle() to be available. */
	pid = xTaskGetCurrentTaskHandle();

	for (i = 0; i < NbActiveThreads; i++) {
		tl = &(Threads_TimeoutsList[i]);
		if (tl->pid == pid) {
			return &(tl->timeouts);
		}
	}

	/* If we're here, this means the scheduler gave the focus to the task as
	 * it was */

	/* being created(because of a higher priority). Since
	 * Threads_TimeoutsList[] */

	/* update is done just after the task creation, the array is not
	 * up-to-date. */
	/* => the last array entry must be the one of the current task. */
	return(&(Threads_TimeoutsList[NbActiveThreads].timeouts));
}

/**
 * \brief Instantiate a thread for lwIP. Both the id and the priority are
 * system dependent.
 *
 * \param name Pointer to the thread name.
 * \param thread Pointer to the thread function.
 * \param arg Argument will be passed into the thread().
 * \param stacksize Stack size of the thread.
 * \param prio Thread priority.
 *
 * \return The id of the new thread.
 */

sys_thread_t sys_thread_new(char *name, void (*thread)(
		void *arg), void *arg, int stacksize, int prio)
{
	sys_thread_t newthread;
	portBASE_TYPE result;
	SYS_ARCH_DECL_PROTECT(protectionLevel);

	result = xTaskCreate( thread, (signed portCHAR *)name, stacksize, arg,
			prio, &newthread );

	/* Need to protect this -- preemption here could be a problem! */
	SYS_ARCH_PROTECT(protectionLevel);
	if (pdPASS == result) {
		/* For each task created, store the task handle (pid) in the
		 * timers array. */
		/* This scheme doesn't allow for threads to be deleted */
		Threads_TimeoutsList[NbActiveThreads++].pid = newthread;
	} else {
		newthread = NULL;
	}

	SYS_ARCH_UNPROTECT(protectionLevel);

	return(newthread);
}

/* ----------- PREEMPTION PROTECTION
 * -------------------------------------------- */

/* This optional function does a "fast" critical region protection and returns
 * the previous protection level. This function is only called during very short
 * critical regions. An embedded system which supports ISR-based drivers might
 * want to implement this function by disabling interrupts. Task-based systems
 * might want to implement this by using a mutex or disabling tasking. This
 * function should support recursive calls from the same task or interrupt. In
 * other words, sys_arch_protect() could be called while already protected. In
 * that case the return value indicates that it is already protected.*/
extern volatile unsigned portLONG ulCriticalNesting;

/**
 * \brief Protect the system.
 *
 * \return 1 on success.
 */
sys_prot_t sys_arch_protect(void)
{
	vPortEnterCritical();
	return 1; /* Not used */
}

/**
 * \brief Unprotect the system.
 *
 * \param pval Protect value.
 */
void sys_arch_unprotect(sys_prot_t pval)
{
	vPortExitCritical();
}
