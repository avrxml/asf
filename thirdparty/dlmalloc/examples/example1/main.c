/**
 * \file
 *
 * \mainpage
 * \section title Memory manager example.
 *
 * \section file File(s)
 * - \ref main.c
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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
 ***************************************************************************/


#include "compiler.h"
#include "board.h"
#include "pm.h"
#include "power_clocks_lib.h"
#include "flashc.h"
#include "gpio.h"
#include "sdramc.h"
#include "dlmalloc.h"


#define FHSB_HZ FOSC0
#define FPBA_HZ FOSC0

#define SDRAM_START_ADDRESS 0xD0000000
#define MEM_SPACE_SIZE 0x2000000

/*! \brief Initializes MCU exceptions.
 */
static void init_exceptions(void)
{
#if __GNUC__ && __AVR32__
  // Import the Exception Vector Base Address.
  extern void _evba;

  // Load the Exception Vector Base Address in the corresponding system
  // register.
  Set_system_register(AVR32_EVBA, (int)&_evba);
#endif

  // Enable exceptions globally.
  Enable_global_exception();
}


/*! \brief Initializes the HSB bus matrix.
 */
static void init_hmatrix(void)
{
  // For the internal-flash HMATRIX slave, use last master as default.
  union
  {
    unsigned long                 scfg;
    avr32_hmatrix_scfg_t          SCFG;
  } u_avr32_hmatrix_scfg = {AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_FLASH]};
  u_avr32_hmatrix_scfg.SCFG.defmstr_type = AVR32_HMATRIX_DEFMSTR_TYPE_LAST_DEFAULT;
  AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_FLASH] = u_avr32_hmatrix_scfg.scfg;
}



/*! \brief Initializes the heap.
 */
static void init_heap(void)
{
#ifdef _DEBUG
  #if __GNUC__ && __AVR32__

  // Import the location of the heap.
  extern void __heap_start__;
  extern void __heap_end__;

  U32 *p_heap_word;

  // Initialize the heap.
  for (p_heap_word = &__heap_start__; (void *)p_heap_word < &__heap_end__; p_heap_word++)
    *p_heap_word = DEFAULT_HEAP_INIT_WORD;

  #elif __ICCAVR32__

  // Import the location of the heap.
  #pragma segment = "HEAP"

  U32 *p_heap_word;

  // Initialize the heap.
  for (p_heap_word = __segment_begin("HEAP"); (void *)p_heap_word < __segment_end("HEAP"); p_heap_word++)
    *p_heap_word = DEFAULT_HEAP_INIT_WORD;

  #endif
#endif
}


/*! \brief Low-level initialization routine called during startup, before the
 *         \ref main function.
 */
#if __GNUC__ && __AVR32__
int _init_startup(void)
#elif __ICCAVR32__
int __low_level_init(void)
#endif
{
  init_exceptions();
  init_hmatrix();
  init_heap();

  // EWAVR32: Request initialization of data segments.
  // GCC: Don't-care value.
  return 1;
}

/*! \brief Main function of the example memory manager.
 *
 *  This example shows how memory can be allocated from different
 *  memory spaces.
 *  The default allocation will get memory from the internal SRAM.
 *  By using the "memory space" functionality of the memory manager
 *  it is possible to use other memory spaces as resources like an
 *  attached SDRAM.
 */
int main(void)
{
	void *some_space;
	void *some_more_space;
	void *some_space_in_sdram;
	mspace sdram_msp;

 	// Switch to external oscillator 0.
	pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);

	sdramc_init(FHSB_HZ);

	// default allocation from internal SRAM
  	some_space = dlmalloc(512);
	some_more_space = dlmalloc(64);

	// Create a new memory space the covers the SDRAM
	sdram_msp = create_mspace_with_base((void*) SDRAM_START_ADDRESS, MEM_SPACE_SIZE, 0);

	// allocate memory from the created memroy space
	some_space_in_sdram = mspace_malloc(sdram_msp, 512);

	while (true)
	{
	}
}
