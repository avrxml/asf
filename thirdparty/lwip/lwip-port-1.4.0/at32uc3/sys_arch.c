/*****************************************************************************
 *
 * \file
 *
 * \brief lwIP abstraction layer for AVR32 UC3.
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


// Structure associating a thread to a struct sys_timeouts
struct TimeoutlistPerThread
{
  sys_thread_t        pid;       // The thread id.
};

// Thread & struct sys_timeouts association statically allocated per thread.
// Note: SYS_THREAD_MAX is the max number of thread created by sys_thread_new()
// that can run simultaneously; it is defined in conf_lwip_threads.h.
static struct TimeoutlistPerThread Threads_TimeoutsList[SYS_THREAD_MAX];

// Number of active threads.
static u16_t NbActiveThreads = 0;

//----------- INIT -------------------------------------------------------------

// Initialize the sys_arch layer.
void sys_init(void)
{
  int i;


  // Initialize the the per-thread sys_timeouts structures
  // make sure there are no valid pids in the list
  for(i = 0; i < SYS_THREAD_MAX; i++)
  {
    Threads_TimeoutsList[i].pid = 0;
  }

  // keep track of how many threads have been created
  NbActiveThreads = 0;
}



//----------- SEMAPHORES -------------------------------------------------------

// Creates and returns a new semaphore. The "count" argument specifies the
// initial state of the semaphore.
err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
  err_t err_sem = ERR_MEM;

  //sanity check
  if (sem != NULL)
  {
    portENTER_CRITICAL();

    vSemaphoreCreateBinary( *sem );
    if( *sem != SYS_SEM_NULL )
    {
  #if SYS_STATS
      lwip_stats.sys.sem.used++;
      if (lwip_stats.sys.sem.used > lwip_stats.sys.sem.max) {
        lwip_stats.sys.sem.max = lwip_stats.sys.sem.used;
      }
  #endif /* SYS_STATS */

      if( 0 == count )  // Means we want the sem to be unavailable at init state.
      {
        xSemaphoreTake( *sem, 1);
      }

      err_sem = ERR_OK;
    }

    portEXIT_CRITICAL();
  }

  return err_sem;
}


// Frees a semaphore created by sys_sem_new.
void sys_sem_free(sys_sem_t *sem)
{
  //sanity check
  if (sem != NULL)
  {
    if( SYS_SEM_NULL != *sem )
    {
  #ifdef SYS_STATS
      lwip_stats.sys.sem.used--;
  #endif /* SYS_STATS */
      vQueueDelete( *sem );
    }
  }
}


// Signals (or releases) a semaphore.
void sys_sem_signal(sys_sem_t *sem)
{
  //sanity check
  if (sem != NULL)
  {
    xSemaphoreGive( *sem );
  }
}

// Blocks the thread while waiting for the semaphore to be signaled. The timeout
// parameter specifies how many milliseconds the function should block before
// returning; if the function times out, it should return SYS_ARCH_TIMEOUT.
// If timeout=0, then the function should block indefinitely.
// If the function acquires the semaphore, it should return how many milliseconds
// expired while waiting for the semaphore. The function may return 0 if the
// semaphore was immediately available.
//
// Note that there is another function sys_sem_wait in sys.c, but it is a wrapper
// for the sys_arch_sem_wait function. Please note that it is important for the
// semaphores to return an accurate count of elapsed milliseconds, since they are
// used to schedule timers in lwIP.
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
  portTickType TickStart;
  portTickType TickStop;
  portTickType TickElapsed = (portTickType)(timeout/portTICK_RATE_MS); // Express the timeout in OS tick.

  //sanity check
  if (sem != NULL)
  {

    if(timeout && !TickElapsed){
      TickElapsed = 1;            // Wait at least one tick
    }

    if( 0 == TickElapsed )
    {
      TickStart = xTaskGetTickCount();
      // If timeout=0, then the function should block indefinitely.
      while( pdFALSE == xSemaphoreTake( *sem, SYS_ARCH_BLOCKING_TICKTIMEOUT ) );
    }
    else
    {
      TickStart = xTaskGetTickCount();
      if( pdFALSE == xSemaphoreTake( *sem, TickElapsed ) )
        // if the function times out, it should return SYS_ARCH_TIMEOUT.
        return( SYS_ARCH_TIMEOUT );
    }
    // If the function acquires the semaphore, it should return how many milliseconds
    // expired while waiting for the semaphore.
    TickStop = xTaskGetTickCount();
    // Take care of wrap-around.
    if( TickStop >= TickStart )
      TickElapsed = TickStop - TickStart;
    else
      TickElapsed = portMAX_DELAY - TickStart + TickStop ;

    return( TickElapsed*portTICK_RATE_MS );
  }
  else
    return ERR_MEM;
}

#ifndef sys_sem_valid
/** Check if a sempahore is valid/allocated: return 1 for valid, 0 for invalid */
int sys_sem_valid(sys_sem_t *sem)
{
  return ((int)( *sem ));
}
#endif

#ifndef sys_sem_set_invalid
/** Set a semaphore invalid so that sys_sem_valid returns 0 */
void sys_sem_set_invalid(sys_sem_t *sem)
{
  *sem = NULL;
}
#endif



//----------- MAILBOXES --------------------------------------------------------

// Creates an empty mailbox for maximum "size" elements. Elements stored in
// mailboxes are pointers.
// Returns ERR_OK if successfull or ERR_MEM on error.
err_t sys_mbox_new(sys_mbox_t *mBoxNew, int size )
{
  err_t err_mbox = ERR_MEM;

  //sanity check
  if (mBoxNew != NULL)
  {
    *mBoxNew = xQueueCreate( size, sizeof( void * ) );
  #if SYS_STATS
    if( SYS_MBOX_NULL != *mBoxNew )
    {
      lwip_stats.sys.mbox.used++;
      if (lwip_stats.sys.mbox.used > lwip_stats.sys.mbox.max)
      {
        lwip_stats.sys.mbox.max = lwip_stats.sys.mbox.used;
      }
    }
  #endif /* SYS_STATS */
    err_mbox = ERR_OK;
  }
  return( err_mbox );
}


// Deallocates a mailbox.
// If there are messages still present in the mailbox when the mailbox is
// deallocated, it is an indication of a programming error in lwIP and the
// developer should be notified.
void sys_mbox_free(sys_mbox_t *mbox)
{
  //sanity check
  if (mbox != NULL)
  {
    if( SYS_MBOX_NULL != *mbox )
    {
  #ifdef SYS_STATS
      lwip_stats.sys.mbox.used--;
  #endif /* SYS_STATS */
      vQueueDelete( *mbox );
    }
  }
}


// Posts the "msg" to the mailbox. This function have to block until the "msg"
// is really posted.
void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
  //sanit check
  if (mbox != NULL)
  {
    while( pdTRUE != xQueueSend( *mbox, &msg, SYS_ARCH_BLOCKING_TICKTIMEOUT ) );
  }
}


// Try to post the "msg" to the mailbox.
// Returns ERR_MEM if the mailbox is full,
// else, ERR_OK if the "msg" is posted.
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
  err_t err_mbox = ERR_MEM;

  //sanity check
  if (mbox != NULL)
  {
    if( errQUEUE_FULL != xQueueSend( *mbox, &msg, 0 ) )
      err_mbox = ERR_OK;
  }

  return ( err_mbox );
}


// Blocks the thread until a message arrives in the mailbox, but does not block
// the thread longer than "timeout" milliseconds (similar to the sys_arch_sem_wait()
// function).
// If "timeout" is 0, the thread should be blocked until a message arrives.
// The "msg" argument is a result parameter that is set by the function (i.e., by
// doing "*msg = ptr"). The "msg" parameter maybe NULL to indicate that the message
// should be dropped.
//
// The return values are the same as for the sys_arch_sem_wait() function:
// Number of milliseconds spent waiting or SYS_ARCH_TIMEOUT if there was a timeout.
// or ERR_MEM if invalid pointer to message box
//
// Note that a function with a similar name, sys_mbox_fetch(), is implemented by lwIP.
u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
  portTickType TickStart;
  portTickType TickStop;
  void         *tempoptr;
  portTickType TickElapsed = (portTickType)(timeout/portTICK_RATE_MS); // Express the timeout in OS tick.

  //sanity check
  if (mbox != NULL)
  {
    if(timeout && !TickElapsed){
      TickElapsed = 1;            // Wait at least one tick
    }

    if(msg == NULL)
    {
	  msg = &tempoptr;
    }

    // NOTE: INCLUDE_xTaskGetSchedulerState must be set to 1 in FreeRTOSConfig.h
    // for xTaskGetTickCount() to be available.
    if( 0 == TickElapsed )
    {
      TickStart = xTaskGetTickCount();
      // If "timeout" is 0, the thread should be blocked until a message arrives.
      while( pdFALSE == xQueueReceive( *mbox, &(*msg), SYS_ARCH_BLOCKING_TICKTIMEOUT ) );
    }
    else
    {
      TickStart = xTaskGetTickCount();
      if( pdFALSE == xQueueReceive( *mbox, &(*msg), TickElapsed ) )
      {
        *msg = NULL;
        // if the function times out, it should return SYS_ARCH_TIMEOUT.
        return( SYS_ARCH_TIMEOUT );
      }
    }
    // If the function gets a msg, it should return the number of ms spent waiting.
    TickStop = xTaskGetTickCount();
    // Take care of wrap-around.
    if( TickStop >= TickStart )
      TickElapsed = TickStop - TickStart;
    else
      TickElapsed = portMAX_DELAY - TickStart + TickStop ;

    return( TickElapsed*portTICK_RATE_MS);
  }
  else
    return ERR_MEM;
}


// This is similar to sys_arch_mbox_fetch, however if a message is not present
// in the mailbox, it immediately returns with the code SYS_MBOX_EMPTY.
// On success 0 is returned.
//
u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
  void  *tempoptr;

  //sanity check
  if (mbox != NULL)
  {
    if(msg == NULL)
    {
      msg = &tempoptr;
    }
    if( pdFALSE == xQueueReceive( *mbox, &(*msg), 0 ) )
      // if a message is not present in the mailbox, it immediately returns with
      // the code SYS_MBOX_EMPTY.
      return( SYS_MBOX_EMPTY );
    // On success 0 is returned.
    return( 0 );
  }
  else
    return( SYS_MBOX_EMPTY );
}

#ifndef sys_mbox_valid
/** Check if an mbox is valid/allocated: return 1 for valid, 0 for invalid */
int sys_mbox_valid(sys_mbox_t *mbox)
{
  return ((int)( *mbox ));
}
#endif

#ifndef sys_mbox_set_invalid
/** Set an mbox invalid so that sys_mbox_valid returns 0 */
void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
	*mbox = NULL;
}
#endif

// Used to instantiate a thread for lwIP.
// Starts a new thread named "name" with priority "prio" that will begin its
// execution in the function "thread()". The "arg" argument will be passed as an
// argument to the thread() function. The stack size to used for this thread is
// the "stacksize" parameter. The id of the new thread is returned. Both the id
// and the priority are system dependent.

sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
  sys_thread_t    newthread;
  portBASE_TYPE   result;
  SYS_ARCH_DECL_PROTECT(protectionLevel);

  result = xTaskCreate( thread, (signed portCHAR *)name, stacksize, arg, prio, &newthread );

  // Need to protect this -- preemption here could be a problem!
  SYS_ARCH_PROTECT(protectionLevel);
  if( pdPASS == result )
  {
    // For each task created, store the task handle (pid) in the timers array.
    // This scheme doesn't allow for threads to be deleted
    Threads_TimeoutsList[NbActiveThreads++].pid = newthread;
  }
  else
  {
    newthread = NULL;
  }
  SYS_ARCH_UNPROTECT(protectionLevel);

  return( newthread );
}

/* Mutex functions: */

/** Define LWIP_COMPAT_MUTEX if the port has no mutexes and binary semaphores
    should be used instead */
#if !LWIP_COMPAT_MUTEX

/** Create a new mutex
 * @param mutex pointer to the mutex to create
 * @return a new mutex */
err_t sys_mutex_new(sys_mutex_t *mutex)
{
}

/** Lock a mutex
 * @param mutex the mutex to lock */
void sys_mutex_lock(sys_mutex_t *mutex)
{}

/** Unlock a mutex
 * @param mutex the mutex to unlock */
void sys_mutex_unlock(sys_mutex_t *mutex)
{
}

/** Delete a semaphore
 * @param mutex the mutex to delete */
void sys_mutex_free(sys_mutex_t *mutex)
{
}

#ifndef sys_mutex_valid
/** Check if a mutex is valid/allocated: return 1 for valid, 0 for invalid */
int sys_mutex_valid(sys_mutex_t *mutex)
{
  return ((int)( *mutex ));
}
#endif

#ifndef sys_mutex_set_invalid
/** Set a mutex invalid so that sys_mutex_valid returns 0 */
void sys_mutex_set_invalid(sys_mutex_t *mutex)
{
  *mutex = NULL;
}
#endif
#endif

//----------- PREEMPTION PROTECTION --------------------------------------------

// This optional function does a "fast" critical region protection and returns
// the previous protection level. This function is only called during very short
// critical regions. An embedded system which supports ISR-based drivers might
// want to implement this function by disabling interrupts. Task-based systems
// might want to implement this by using a mutex or disabling tasking. This
// function should support recursive calls from the same task or interrupt. In
// other words, sys_arch_protect() could be called while already protected. In
// that case the return value indicates that it is already protected.
extern volatile unsigned portLONG ulCriticalNesting;
sys_prot_t sys_arch_protect(void)
{
  vPortEnterCritical();
  return 1; // Not used
}


// This optional function does a "fast" set of critical region protection to the
// value specified by pval.
void sys_arch_unprotect(sys_prot_t pval)
{
  vPortExitCritical();
}
