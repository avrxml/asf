#include "conf_dlmalloc.h"

/* If a custom place for the standard heap is needed
 * (like moving it to SDRAM) following defines are needed.
 *
 * CUSTOM_HEAP
 * CUSTOM_HEAP_START: start address of the heap, must be on a word boundary
 * CUSTOM_HEAP_END: last possible address for the heap before an overflow
 * occurs
 *
 * Default behaviour is to obtain the start and end address of the heap from
 * the linker.
 */
#ifndef CUSTOM_HEAP
extern void __heap_start__, __heap_end__;
#define HEAP_START &__heap_start__
#define HEAP_END &__heap_end__
#else
#define HEAP_START CUSTOM_HEAP_START
#define HEAP_END CUSTOM_HEAP_END
#endif

/* this variable stores the current default heap position
 * Because it is used by the mmap and the sbrk function
 * it is global for the module*/
static void *cur_heap_pos = 0;

void * dlmalloc_sbrk(int increment)
{
	//Initialize cur_heap_pos
	if ( cur_heap_pos == 0 )
	 	cur_heap_pos = (void *) (HEAP_START);

	if ( ((void *)((char *)cur_heap_pos + increment)) <= (void *) HEAP_END ){
		void *heap_pos = cur_heap_pos;
		cur_heap_pos = (void *)((char *)cur_heap_pos +increment);
		return heap_pos;
	} else {
#ifdef DEBUG_MSG_ON
		fprintf(stderr, "Warning: Heap is running full trying to allocate %i bytes!!!!\n", increment);
		fprintf(stderr, "\tHeap start address\t= 0x%x\n", HEAP_START);
		fprintf(stderr, "\tHeap end address\t= 0x%x\n", HEAP_END);
		fprintf(stderr, "\tCurrent heap address\t= 0x%x\n", cur_heap_pos);
		fflush(stderr);
#endif
		//errno = ENOMEM;
		return (void *)-1;
	}
}
/*
 *
 * This function is called by the dlmalloc.c module with
 * following parameters:
 * start = 0
 * prot = PROT_READ | PROT_WRITE
 * flags = MMAP_ANONYMOUS | MMAP_PRIVATE
 * fd = -1
 * offset = 0
 *
 * length is the only relevant (variable) parameter and it
 * specifies the requested memory size.
 */
void *mmap(void *start, int length,
	int prot, int flags, int fd, int offset)
{
#ifndef ONLY_MSPACES
	if(length == 0)
	{
		cur_heap_pos = (void *) HEAP_START;
	}

	if ( (cur_heap_pos + length) <= (void *) HEAP_END )
	{
		void *heap_pos = cur_heap_pos;
		cur_heap_pos += length;
		return heap_pos;
	} else {
		//errno = ENOMEM;
		return (void *)-1;
	}
	return (void *) -1;
#else
	return MORECORE(length);
#endif
}

int munmap(void *start, int length)
{
#ifndef ONLY_MSPACES
	if( (cur_heap_pos - length) < HEAP_START)
	{
		return -1;
	}
	else
	{
		cur_heap_pos -= length;
		return 0;
	}
#else
	if (MORECORE(-length) == ((void *)-1) )
		return -1;
	else
		return 0;
#endif
}

