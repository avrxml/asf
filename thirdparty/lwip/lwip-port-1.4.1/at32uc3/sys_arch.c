/*****************************************************************************
 *
 * \file
 *
 * \brief lwIP abstraction layer for AVR32 UC3.
 *
 * Copyright (c) 2013-2016 Atmel Corporation. All rights reserved.
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

/* 
 * This optional function returns the current time in milliseconds (don't care
 * for wraparound, this is only used for time diffs).  Not implementing this function
 * means you cannot use some modules (e.g. TCP  timestamps, internal timeouts 
 * for NO_SYS==1).

	u32_t sys_now(void)
	{
		return (sys_get_ms());
	}

 * 
 * If FreeRTOS is used, the sys_get_ms() function can be realized by invoking 
 * xTaskGetTickCount() function, like this: 

	uint32_t sys_get_ms(void)
	{
		return xTaskGetTickCount();
	}

 *
 * If there is no operating system, here is a solution to realizing u32_t sys_now()
 * function, which is based on timer counter. To do it, a file can be created.

	#include "sysclk.h"
	#include "board.h"
	#include "tc.h"
	#include "lwip/init.h"
	#include "lwip/sys.h"
	#if defined (__GNUC__)
	#  include "intc.h"
	#endif

	//Clock tick count.
	static volatile uint32_t gs_ul_clk_tick;

	//TC interrupt.
	//The ISR handles RC compare interrupt and sets the update_timer flag to update the 
	//timer value.

	#if defined (__GNUC__)
	__attribute__((__interrupt__))
	#elif defined (__ICCAVR32__)
	#pragma handler = AVR32_TC_IRQ_GROUP, 1
	__interrupt
	#endif
	
	static void tc_irq(void)
	{
		// Increment the ms seconds counter
		gs_ul_clk_tick++;

		// Clear the interrupt flag. This is a side effect of reading the TC SR.
		tc_read_sr(&AVR32_TC, 0);
	}

	//TC Initialization.
	//Initializes and start the TC module with the following:
	//- Counter in Up mode with automatic reset on RC compare match.
	//- fPBA/8 is used as clock source for TC
	//- Enables RC compare match interrupt
	//tc Base address of the TC module
	static void tc_init(volatile avr32_tc_t *tc)
	{
		// Options for waveform generation.
		static const tc_waveform_opt_t waveform_opt = {
			// Channel selection.
			.channel  = 0,
			// Software trigger effect on TIOB.
			.bswtrg   = TC_EVT_EFFECT_NOOP,
			// External event effect on TIOB.
			.beevt    = TC_EVT_EFFECT_NOOP,
			// RC compare effect on TIOB.
			.bcpc     = TC_EVT_EFFECT_NOOP,
			// RB compare effect on TIOB.
			.bcpb     = TC_EVT_EFFECT_NOOP,
			// Software trigger effect on TIOA.
			.aswtrg   = TC_EVT_EFFECT_NOOP,
			// External event effect on TIOA.
			.aeevt    = TC_EVT_EFFECT_NOOP,
			// RC compare effect on TIOA.
			.acpc     = TC_EVT_EFFECT_NOOP,

			// RA compare effect on TIOA.
			// (other possibilities are none, set and clear).

				.acpa     = TC_EVT_EFFECT_NOOP,
				
				// Waveform selection: Up mode with automatic trigger(reset)
				// on RC compare.
			
			.wavsel   = TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER,
			// External event trigger enable.
			.enetrg   = false,
			// External event selection.
			.eevt     = 0,
			// External event edge selection.
			.eevtedg  = TC_SEL_NO_EDGE,
			// Counter disable when RC compare.
			.cpcdis   = false,
			// Counter clock stopped with RC compare.
			.cpcstop  = false,
			// Burst signal selection.
			.burst    = false,
			// Clock inversion.
			.clki     = false,
			// Internal source clock 3, connected to fPBA / 8.
			.tcclks   = TC_CLOCK_SOURCE_TC3
		};

		// Options for enabling TC interrupts
		static const tc_interrupt_t tc_interrupt = {
			.etrgs = 0,
			.ldrbs = 0,
			.ldras = 0,
			.cpcs  = 1, // Enable interrupt on RC compare alone
			.cpbs  = 0,
			.cpas  = 0,
			.lovrs = 0,
			.covfs = 0
		};
		// Initialize the timer/counter.
		tc_init_waveform(tc, &waveform_opt);

		// Set the compare triggers.
		// We configure it to count every 1 milliseconds.
		// We want: (1 / (fPBA / 8)) * RC = 1 ms, hence RC = (fPBA / 8) / 1000
		// to get an interrupt every 10 ms.

		tc_write_rc(tc, 0, (sysclk_get_pba_hz() / 8 / 1000));
		// configure the timer interrupt
		tc_configure_interrupts(tc, 0, &tc_interrupt);
		// Start the timer/counter.
		tc_start(tc, 0);
	}

	//Initialize the timer counter (AVR32_TC).
	void sys_init_timing(void)
	{
		volatile avr32_tc_t *tc = &AVR32_TC;
		
		// Clear tick value.
		gs_ul_clk_tick = 0;
		
		// Enable the clock to the selected example Timer/counter peripheral module.
		sysclk_enable_peripheral_clock(&AVR32_TC);
		
		// Disable the interrupts
		cpu_irq_disable();

		#if defined (__GNUC__)
		// Initialize interrupt vectors.
		INTC_init_interrupts();
		// Register the RTC interrupt handler to the interrupt controller.
		INTC_register_interrupt(&tc_irq, AVR32_TC_IRQ0, AVR32_INTC_INT0);
		#endif
		// Enable the interrupts
		cpu_irq_enable();
		// Initialize the timer module
		tc_init(tc);	
	}

	//Return the number of timer ticks (ms).
	uint32_t sys_get_ms(void)
	{
		return gs_ul_clk_tick;
	}

	#if ((LWIP_VERSION) != ((1U << 24) | (3U << 16) | (2U << 8) | (LWIP_VERSION_RC)))
	u32_t sys_now(void)
	{
		return (sys_get_ms());
	}
	#endif
*
*/

