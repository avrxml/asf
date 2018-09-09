/*****************************************************************************
 *
 * \file
 *
 * \brief Macros and functions dedicated to debug purposes.
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
