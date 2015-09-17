/*****************************************************************************
 *
 * \file
 *
 * \brief Macros and functions dedicated to debug purposes.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#include "compiler.h"
#include "debug.h"


#if (defined __GNUC__)
#   include "malloc.h"

U32 get_heap_curr_used_size( void )
{
  struct mallinfo my_info=mallinfo();
  return my_info.uordblks;
}

U32 get_heap_total_used_size( void )
{
  struct mallinfo my_info=mallinfo();
  return my_info.arena;
}
#elif (defined __ICCAVR32__)

/*
 * Include the following in the include directory path: "$TOOLKIT_DIR$\src\lib\dlib\"
 * Make sure the followings point on the malloc and free definitions.
 */
#define INCLUDE_MALLOC_PATH "..\src\lib\dlib\xxmemxmalloc.c"
#define INCLUDE_FREE_PATH "..\src\lib\dlib\xxmemxfree.c"

static size_t malloc_mem_monitor = 0;
static size_t malloc_mem_monitor_max = 0;

U32 get_heap_curr_used_size( void )
{
  return malloc_mem_monitor;
}

U32 get_heap_total_used_size( void )
{
  return malloc_mem_monitor_max;
}

// Hooks malloc function
void *malloc(size_t size)
{
  extern void *__malloc(size_t);
  void *ptr;

  ptr = __malloc(size);
  if (ptr)
  {
    malloc_mem_monitor += size;
    if (malloc_mem_monitor > malloc_mem_monitor_max)
      malloc_mem_monitor_max = malloc_mem_monitor;
  }
  return ptr;
}

// Hooks free function
void free(void *ptr)
{
  extern void __free(void *);
  static size_t get_memory_size(void *ptr);
  size_t size;

  size = get_memory_size(ptr);
  malloc_mem_monitor -= size;
  __free(ptr);
}

#define free __free
#include INCLUDE_FREE_PATH
#define malloc __malloc
#include INCLUDE_MALLOC_PATH

#define GET_PTR_ALLOC_SIZE(M, ptr, size)                        \
{                                                               \
  M##_Cell M * q;                                               \
  if (ptr == 0)                                                 \
    return 0;                                                   \
  q = (M##_Cell M *)((char M *)ptr - CELL_OFF(M));              \
  if (q->_Size < SIZE_CELL(M) || (q->_Size & M_MASK) != 0)      \
    return 0;                                                   \
  size = q->_Size;                                              \
}

static size_t get_memory_size(void *ptr)
{
  size_t size;
  GET_PTR_ALLOC_SIZE(__data32, ptr, size);
  return size;
}

#endif

U32 get_heap_free_size( void )
{
  U32 high_mark= AVR32_SRAM_SIZE;
  U32 low_mark = 0;
  U32 size ;
  void* p_mem;

  size = (high_mark + low_mark)/2;

  do
  {
    p_mem = malloc(size);
    if( p_mem != NULL)
    { // Can allocate memory
      free(p_mem);
      low_mark = size;
    }
    else
    { // Can not allocate memory
      high_mark = size;
    }

    size = (high_mark + low_mark)/2;
  }
  while( (high_mark-low_mark) >1 );

  return size;
}

static void* round_trace_pbuf;
static U32   round_trace_size;

void uc3_round_trace_init(void* buf, U32 size)
{
  round_trace_pbuf = buf;
  (*(U32*)round_trace_pbuf)=(U32)buf+4;
  round_trace_size = size;
}

void uc3_round_trace(U32 val)
{
  //Disable_global_interrupt();

  U32* p_wr = (U32*)(*(U32*)round_trace_pbuf);
  *p_wr = val;
  p_wr++;
  if( ((U32)p_wr % round_trace_size) ==0 )
    p_wr= (U32*)round_trace_pbuf+1;
  *p_wr = 0xdeadbeef;
  *(U32*)round_trace_pbuf = (U32)p_wr;

  //Enable_global_interrupt();
}
