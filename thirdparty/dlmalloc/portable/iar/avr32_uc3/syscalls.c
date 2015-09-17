/*****************************************************************************
 *
 * \file
 *
 * \brief NEWLIB_ADDONS system calls overloading file for AVR32.
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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <intrinsics.h>

#define	ENOMEM 12	/* Not enough core */

extern void __heap_start__, __heap_end__;

int unimplemented_syscall(char *name){
#ifdef DEBUG_MSG_ON
  fprintf(stderr, "Error: Unimplemented syscall: %\n", name);
#endif
  return -1;
}

void * _sbrk(int increment)
{
  #pragma segment = "HEAP"

  static void *cur_heap_pos = 0;

  //Initialize cur_heap_pos
  if ( cur_heap_pos == 0 )
    cur_heap_pos = __segment_begin( "HEAP" );

  if ( (void*)((char *)cur_heap_pos + increment) <= __segment_end( "HEAP" ) ){
    void *heap_pos = cur_heap_pos;
    cur_heap_pos = (void *)((char *)cur_heap_pos +increment);
    return heap_pos;
  } else {
#ifdef DEBUG_MSG_ON
    fprintf(stderr, "Warning: Heap is running full trying to allocate %i bytes!!!!\n", increment);
    fprintf(stderr, "\tHeap start address\t= 0x%x\n", &__segment_begin( "HEAP" ));
    fprintf(stderr, "\tHeap end address\t= 0x%x\n", &__segment_end( "HEAP" ));
    fprintf(stderr, "\tCurrent heap address\t= 0x%x\n", cur_heap_pos);
    fflush(stderr);
#endif
    errno = ENOMEM;
    return (void *)-1;
  }
}

int open (char *pathname, int flags, int mode)
{
  return unimplemented_syscall("_open");
}
