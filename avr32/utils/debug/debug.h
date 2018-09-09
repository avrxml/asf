/*****************************************************************************
 *
 * \file
 *
 * \brief Macros and functions dedicated to debug purposes.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


#ifndef _DEBUG_H_
#define _DEBUG_H_

/**
 * \defgroup group_avr32_utils_debug Debug macros and functions
 *
 * Debug macros and functions for tracing memory usage for the malloc() and free() functions. Can also be used to get
 * information about both size and placement of the heap memory.
 *
 * \{
 */

#include "stringz.h"

/*! \brief These macros are used to add traces memory.
 *
 * First, initialize the trace with Uc3_trace_init(pointer), giving the start address
 * of the memory location where will be stored the trace.
 * Use Uc3_trace(something) to store "something" into the memory. The end of the trace
 * is signalled by the "0xdeadbeef" pattern.
 */
#define Uc3_trace_init(debug_addr)   \
      *(U32*)(debug_addr)=debug_addr+4

#define Uc3_trace(debug_addr, x)   \
      *(U32*)(*(U32*)(debug_addr)  ) = (U32)(x)   ;\
      *(U32*)(*(U32*)(debug_addr)+4) = 0xdeadbeef ;\
      *(U32*)(debug_addr  ) = *(U32*)(debug_addr)+4

/*! \brief This macro is used to insert labels into assembly output.
 *
 */
#define Insert_label(name)         \
    __asm__ __volatile__ (STRINGZ(name)":");

#if (defined __GNUC__)
/*! \brief Returns the number of total of used bytes allocated from the HEAP.
 *
 * \retval total number of used bytes.
 */
U32 get_heap_total_used_size( void );

/*! \brief Returns the number of bytes currently used from the HEAP.
 *
 * \retval total number of used bytes.
 */
U32 get_heap_curr_used_size( void );
#endif

/*! \brief Returns the number of free bytes in the HEAP.
 *
 * This function tries to allocate the maximum number of bytes by dichotomical method.
 *
 * \retval number of free bytes.
 */
extern U32 get_heap_free_size( void );

/*! \name Traces function using a round buffer
 */
//! @{

/*! \brief Initialize the trace using a round buffer.
 *
 * \param buf   Base address of the buffer used for the trace.
 * \param size  Size of the round buffer. Must be a power of 2.
 */
void uc3_round_trace_init(void* buf, U32 size);

/*! \brief Trace a data in the round buffer.
 *
 * The end of the trace is signalled by the "0xdeadbeef" pattern.
 * \param val   Data to trace;
 */
void uc3_round_trace(U32 val);

//! @}

/**
 * \}
 */

#endif  // _DEBUG_H_
