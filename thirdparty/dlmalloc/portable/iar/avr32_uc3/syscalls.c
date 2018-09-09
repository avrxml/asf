/*****************************************************************************
 *
 * \file
 *
 * \brief NEWLIB_ADDONS system calls overloading file for AVR32.
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
