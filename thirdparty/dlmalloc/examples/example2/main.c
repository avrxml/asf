/**
 * \file
 *
 * \mainpage
 * \section title Memory manager example.
 *
 * \section file File(s)
 * - \ref main.c
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

	// default allocation with C-library malloc from internal SRAM
  	some_space = malloc(512);
	some_more_space = malloc(64);

	// Create a new memory space the covers the SDRAM
	sdram_msp = create_mspace_with_base((void*) SDRAM_START_ADDRESS, MEM_SPACE_SIZE, 0);

	// allocate memory from the created memroy space
	some_space_in_sdram = mspace_malloc(sdram_msp, 512);

	while (true)
	{
	}
}
