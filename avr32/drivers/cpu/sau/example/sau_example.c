/**
 * \file
 *
 * \brief Example of usage of the Software Framework SAU driver in AVR UC3
 *
 * Copyright (c) 2012-2015 Atmel Corporation. All rights reserved.
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
 */

/**
 * \mainpage
 * \section intro Introduction
 * This is the documentation for the data structures, functions, variables,
 * defines, enums, and typedefs for the SAU driver.\n It also gives an example
 * of the usage of the SAU module.
 *
 * The SAU allows the user to remap registers in memory regions protected by
 * the MPU to regions not protected by the MPU. The MPU is set up to protect
 * regions of memory, while the SAU is set up to provide a secure channel into
 * specific memory locations that are protected by the MPU.
 *
 * <b>Example operating mode: </b>
 * - The example starts by remapping the SAU HSB address to the USART registers.
 * - In open mode, SAU can be mapped and accessed directly
 * - The interruption will be then triggered by :
 *   - Attempting to read Unlock Register
 * - In lock mode, the interruption will be then triggered by
 *   - Attempting to write the Unlock register with an invalid key
 * - The exception will be then triggered by : 
 *   - Attempting to read Unlock Register
 *   - Attempting to access the locked channel without unlocking first
 * - The memory areas are then set up by MPU in the following manner:
 * <table>
 * <CAPTION><em>Access permissions of used memory areas</em></CAPTION>
 * <tr>
 *  <td>  </td>
 *  <td> Access Permissions </td>
 * </tr>
 * <tr>
 *  <td> Flash [0x80000000 - 0x80010000], Region 0, all subregions </td>
 *  <td> Read / Write </td>
 * </tr>
 * <tr>
 *  <td> Stack [&stack - &stack + stack size], Region 1, all subregions </td>
 *  <td> Read / Write </td>
 * </tr>
 * <tr>
 *  <td> SAU HSB [0x90000000 - 0x9001000], Region 2, all subregions </td>
 *  <td> Read / None </td>
 * </tr>
 * <tr>
 *  <td> PBA memory [0xFFFF0000 - 0xFFFFF800], Region 3, all subregions </td>
 *  <td> Read / None </td>
 * </tr>
 * <tr>
 *  <td> PBA memory [0xFFFE0000 - 0xFFFF0000], Region 4, subregions 4/5 </td>
 *  <td> Read  / None </td>
 * </tr>
 * </table>
 * - The application then performs a write access to SAU slave address, which
 * is mapped to USART operation and caused the output from USART.
 * - The application then performs a directly write to the USART. since is was
 * protected by MPU, it triggers a DTLB write protection exception => LED3
 * lights up.
 * - End of example: LED0 & LED1 alternatively blink forever.
 *
 * \section files Main Files
 * - sau.c: SAU driver;
 * - sau.h: SAU driver header file;
 * - sau_example.c: SAU example .
 * - sau_exception.S: asm file for exception support (GCC)
 * - sau_exception.s82: asm file for exception support and declaration of
 * function specially located (IAR)
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR UC3 and IAR Systems
 * compiler for AVR UC3. Other compilers may or may not work.
 *
 * \section Configuration Information
 * This example has been tested with the following configuration:
 * - UC3C_EK, UC3L_EK;
 * - USART2 (on UC3C_EK) or USART3 (on UC3L_EK)  abstracted
 *   with a USB CDC connection to a PC; 
 * - PC terminal settings:
 *   - 57600 bps,
 *   - 8 data bits,
 *   - no parity bit,
 *   - 1 stop bit,
 *   - no flow control.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR UC3</A>.\n
 * Support and FAQ: http://www.atmel.com/design-support/
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <avr32/io.h>
#include "compiler.h"
#include "print_funcs.h"
#include "gpio.h"
#include "mpu.h"
#include "sau.h"
#include "cycle_counter.h"
#include "usart.h"
#include "intc.h"
#include "power_clocks_lib.h"
#include "sysclk.h"


// Number of regions considered in this example.
#define NB_CONFIG_MPU_ENTRIES       5

#define REGION_0_FLASH_INDEX        0
#define REGION_1_STACK_INDEX        1
#define REGION_2_PERIPHERALS_INDEX  2
#define REGION_3_SAU_HSB_INDEX      3 
#define REGION_4_SAU_INDEX          4

//SAU RTR channels' assignment
#define USART_CSR_CHANNEL           0
#define USART_THR_CHANNEL           1
#define USART_RHR_CHANNEL           2

//Print character for test
#define PRINT_EXAMPLE_CHAR          0x41

//The start address for PBA and PBB
#define HSB_PBA_START_ADDR          0XFFFF0000
#define HSB_PBB_START_ADDR          0XFFFE0000

//Numbers used for the Lock mode in this example
#define EXAMPLE_UNLOCK_CLOCK_CYCLES 200
#define EXAMPLE_UNLOCK_KEY          0x55
#define EXAMPLE_WRONG_UNLOCK_KEY    0x66

//PBA clock target frequency and SAU slave registers in this example.
#if BOARD == UC3L_EK
	#define EXAMPLE_TARGET_PBACLK_FREQ_HZ 15000000
	#define EXAMPLE_SAU_SLAVE_USART       AVR32_USART3_ADDRESS
#elif BOARD == UC3C_EK
	#define EXAMPLE_TARGET_PBACLK_FREQ_HZ FOSC0
	#define EXAMPLE_SAU_SLAVE_USART       AVR32_USART2_ADDRESS
#endif

// Allocation for NB_CONFIG_MPU_ENTRIES DMPU entries
mpu_entry_t dmpu_entries[NB_CONFIG_MPU_ENTRIES];

/*! \brief The SAU interrupt handler.
 *
 * \note The `__attribute__((__interrupt__))' (under GNU GCC for AVR32) and
 *       `__interrupt' (under IAR Embedded Workbench for Atmel AVR32) C function
 *       attributes are used to manage the `rete' instruction.
 */
#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined(__ICCAVR32__)
__interrupt
#endif
static void sau_int_handler(void)
{
	U8 int_src;
	int_src  = sau_status_check();
	sau_clear_error_flags(0xff);
	//Check the cause of interrupt
	if((int_src & (1 << AVR32_SAU_SR_EXP)) != 0) {
		print_dbg("\r\n   Interrupt generated by EXP!     ");
	}
	if((int_src & (1 << AVR32_SAU_SR_CAU)) != 0) {
		print_dbg("\r\n   Interrupt generated by CAU!     ");
	}
	if((int_src & (1 << AVR32_SAU_SR_URREAD)) != 0) {
		print_dbg("\r\n   Interrupt generated by URREAD!  ");
	}
	if((int_src & (1 << AVR32_SAU_SR_URKEY)) != 0) {
		print_dbg("\r\n   Interrupt generated by URKEY!   ");
	}
	if((int_src & (1 << AVR32_SAU_SR_URES)) != 0) {
		print_dbg("\r\n   Interrupt generated by URES!    ");
	}
	if((int_src & (1 << AVR32_SAU_SR_MBERROR)) != 0) {
		print_dbg("\r\n   Interrupt generated by MBEEROR! ");
	}
	if((int_src & (1 << AVR32_SAU_SR_RTRADR)) != 0) {
		print_dbg("\r\n   Interrupt generated by RTRADR!  ");
	}
}

/*! \brief Handle exception; called by the _evba assembly function.
 *
 * AVR32_EVBA_OFFSET_ITLB_PROT  AVR32_EVBA_OFFSET_DTLB_PROT_x
 *
 * \param *sp               pointer to stack before the exception (provided
 *                          through R12). At this point, sp[0]=SR, sp[1]=PC.
 * \param exception_address address of the offending instruction
 * \param exception_cause   exception cause(==(EVBA offset of the exception)>>2)
 */
void handle_exception(unsigned int *sp, unsigned long exception_address,
			unsigned int exception_cause)
{
	/* 
	 * Test exception cause register AVR32_ECR. AVR32_ECR is updated with a
	 * value equal to the 9 lowest bits of the EVBA offset of the exception
	 * shifted 2 bits to the right.
	 */
	
	switch (exception_cause) {
	case AVR32_EVBA_OFFSET_TLB_MULTIPLE / 4:
		/*
		 * The TLB Multiple Hit Exception is generated when an access
		 * hits in multiple MPU regions. This is usually caused by
		 * programming error.
		 * Turn LED0 on.
		 */
		((avr32_gpio_port_t *)&AVR32_GPIO.port[LED0_GPIO >> 5])->ovrc \
			= 1 << (LED0_GPIO & 0x1F);
		// Disable the MPU to avoid to trigger the exception again!
		disable_mpu();
		break;
	
	case AVR32_EVBA_OFFSET_ITLB_MISS / 4:
		/*
		* The ITLB Miss exception is generated when the MPU is enabled
		* and the instruction memory access does not hit in any regions.
		* Turn LED1 on.
		*/
		((avr32_gpio_port_t *)&AVR32_GPIO.port[LED1_GPIO >> 5])->ovrc \
			= 1 << (LED1_GPIO & 0x1F);
		// Disable the MPU to avoid to trigger the exception again!
		disable_mpu();
		break;
	
	case AVR32_EVBA_OFFSET_ITLB_PROT / 4:
		/*
		 * The ITLB Protection exception is generated when the
		 * instruction memory access violates the access rights
		 * specified by the protection region in which the address lies.
		 * Turn LED2 on.
		 */
		((avr32_gpio_port_t *)&AVR32_GPIO.port[LED2_GPIO >> 5])->ovrc \
			= 1 << (LED2_GPIO & 0x1F);
		// Disable the MPU to avoid to trigger the exception again!
		disable_mpu();
		break;
	
	case AVR32_EVBA_OFFSET_DTLB_MISS_R / 4:
		/*
		 * The DTLB Read Miss exception is generated when the MPU is
		 * enabled and the data memory read access does not hit in any
		 * regions.
		 */
		// Disable the MPU to avoid to trigger the exception again!
		disable_mpu();
		break;
	
	case AVR32_EVBA_OFFSET_DTLB_MISS_W / 4:
		/*
		 * The DTLB Write Miss exception is generated when the MPU is
		 * enabled and the data memory write access does not hit in any
		 * regions.
		 */
		// Disable the MPU to avoid to trigger the exception again!
		disable_mpu();
		break;
	
	case AVR32_EVBA_OFFSET_DTLB_PROT_R / 4:
		/*
		 * The DTLB Protection exception is generated when the data
		 * memory read violates the access rights specified by the
		 * protection region in which the address lies.
		 */
		// Disable the MPU to avoid to trigger the exception again!
		disable_mpu();
		break;
	
	case AVR32_EVBA_OFFSET_DTLB_PROT_W / 4:
		/*
		 * The DTLB Protection exception is generated when the data
		 * memory write violates the access rights specified by the
		 * protection region in which the address lies.
		 * Turn LED3 on.
		 */
		((avr32_gpio_port_t *)&AVR32_GPIO.port[LED3_GPIO >> 5])->ovrc \
			= 1 << (LED3_GPIO & 0x1F);
		/*
		 * Update the return from exception PC address to the next
		 * instruction, to avoid triggering the exception again. Check
		 * if the instruction that generated the exception is extended
		 * (4 bytes) or compact (2 bytes) to find out the address of
		 * the next instruction which we should return to. All extended
		 * instructions have the 3 MSb always set, while this pattern is
		 * forbidden for compact instructions.
	 	 */
		if ((*((unsigned char *)exception_address) & 0xE0 ) == 0xE0) {
			// Extended instruction.
			sp[1] = exception_address + 4;
		} else {
			// Compact instruction
			sp[1] = exception_address + 2;
		}					
		break;
		// Unexpected
	default: 
		break;
	}
}

/*! \brief Toggle the LEDs infinitely 
 *
 *  \note This is used to show errors in case of any MPU configuration.
 */
static void leds_toggle_infinitely(void)
{
	volatile unsigned long i;
	
	LED_On( LED0 | LED1 | LED2 | LED3 );
	while (1) {
		i = 100000;
		while (i--);
		LED_Toggle( LED0 | LED1 | LED2 | LED3 );
	}
}

/*! \brief Toggle the LED0 and LED1 infinitely
 *
 *  \note This is used to show the end of this demo.
 */
static void led_display_chaser_infinitely(void)
{
	volatile unsigned long i;
	U8  u8LedMap = 0x01;
	
	while (1) {
		// Turn the current LED on only and move to next LED.
		LED_Display_Field(LED0 | LED1, u8LedMap);
		i = 100000;
		while (i--);
		u8LedMap = max((U8)(u8LedMap << 1) & 0x0F, 0x01);
	}
}

/*! \brief SAU configuration for USART registers
 * 
 */
void sau_config_to_usart(void)
{
	static const sau_map_t SAU_USART_MAP = {
		{(EXAMPLE_SAU_SLAVE_USART + AVR32_USART_CSR),
			USART_CSR_CHANNEL},
		{(EXAMPLE_SAU_SLAVE_USART + AVR32_USART_THR),
			USART_THR_CHANNEL},
		{(EXAMPLE_SAU_SLAVE_USART + AVR32_USART_RHR), USART_RHR_CHANNEL}
	};

	sau_enter_setup();
	sau_enable();
	sau_configure_channels(SAU_USART_MAP, 	
		sizeof(SAU_USART_MAP) / sizeof(SAU_USART_MAP[0]));
}

/*! \brief MPU configuration in this example
 * 
 */
void mpu_memory_protect_config(void)
{

#if defined (__GNUC__)
	extern void _stack;         // Start address of the stack.
	extern void __stack_size__; // Size of the stack.
#elif defined (__ICCAVR32__)
	#pragma segment = "SSTACK"
#endif
	eRegionSize RegionSize;     // Region size
	uint32_t u32kBSizeValue;         // A region size expressed in kB

	print_dbg("\r\n## Setting up all MPU entries.     ##\r\n");
	/*
	 * Configuring Region 0: the Flash memory area occupied by this
	 * application. Since this program is linked into flash, we want to
	 * define the Flash memory protection scheme as being Readable and
	 * Executable (RX). The flash memory starts at address 0x80000000 (this
	 * info is found in the part specific header file found in the compiler
	 * installation tree; e.g. in uc3a0512.h for an AT32UC3A0512 part).
	 */
	dmpu_entries[REGION_0_FLASH_INDEX].addr = AVR32_FLASH_ADDRESS;
	/*
	 * We estimate the size of this application to be less than 64kB. So we
	 * define region 0 size as being 64kB.
	 */
	dmpu_entries[REGION_0_FLASH_INDEX].size = MPU_REGION_SIZE_64KB;
	// Deem this protection scheme as valid.
	dmpu_entries[REGION_0_FLASH_INDEX].valid = 1;
	if(set_mpu_entry(&dmpu_entries[REGION_0_FLASH_INDEX],
		REGION_0_FLASH_INDEX))
		leds_toggle_infinitely();  // Error
	
	/*
	 * Set Access Permission A to Read/Execute access in both privileged
	 * and unprivileged mode.
	 */
	set_access_permissions(REGION_0_FLASH_INDEX, MPU_APRA_ID,
			MPU_PRIVRX_UNPRIVRX);
	select_subregion(REGION_0_FLASH_INDEX, 0x0000);
 
	/*
	 * Configuring Region 1: the memory used by the stack.
	 * According to the linker script and to the linker step, the stack is
	 * placed in INTRAM (internal RAM memory) after the RAM area explicitly
	 * used by the application.
	 * The stack is used upon exceptions (some registers are saved on the
	 * stack; cf doc32000 "AVR32 Architecture Manual Complete" section 7.1
	 * Event Handling in AVR32A); the stack may be used when doing function
	 * calls also. Since this application intends to generate exceptions,
	 * we must make sure that the memory area used by the stack is
	 * Read/Write!
	*/
#if defined (__GNUC__)
	dmpu_entries[REGION_1_STACK_INDEX].addr = (unsigned int)&_stack;
#elif defined (__ICCAVR32__)
	dmpu_entries[REGION_1_STACK_INDEX].addr =
		(unsigned int)__segment_begin( "SSTACK" );
#endif
	/*
	 * WARNING NOTE: there are limitations concerning the region size; see
	 * doc32002 "AVR32UC Technical Reference Manual Complete" Table 6-1.
	 * "Protection region sizes implied by the Size field". The
	 * mpu_convert_kbsize_to_eregionsize() makes sure this part of the spec
	 * is respected.
	 */
#if defined (__GNUC__)
	u32kBSizeValue = (uint32_t)&__stack_size__ >> 10;
#elif defined (__ICCAVR32__)
	u32kBSizeValue = ((uint32_t)__segment_end( "SSTACK" )        \
		- (uint32_t)__segment_begin( "SSTACK" )) >> 10;
#endif

	if (false == mpu_convert_kbsize_to_eregionsize(&RegionSize,
		u32kBSizeValue)) {
		leds_toggle_infinitely();  // Error
	 }
	dmpu_entries[REGION_1_STACK_INDEX].size = RegionSize;
	// Deem this protection scheme as valid.
	dmpu_entries[REGION_1_STACK_INDEX].valid = 1;
	if (set_mpu_entry(&dmpu_entries[REGION_1_STACK_INDEX],
		REGION_1_STACK_INDEX)) {
		leds_toggle_infinitely();  // Error
	}

	/*
	 * Set Access Permission B to Read/Write access in privileged mode and 
	 * to None in unprivileged mode.
	 */
	set_access_permissions(REGION_1_STACK_INDEX, MPU_APRB_ID,
		MPU_PRIVRW_UNPRIVNONE);
	// Set all subregions of the stack to Read/Write access .
	select_subregion(REGION_1_STACK_INDEX, 0xFFFF);
	
	/*
	 * Configuring Region 2: the peripherals memory mapping.
	 * In this example, we're using the DEBUG module to output traces to
	 * USART2, and then all the registers in USART2 will be protected by MPU
	 * from user mode access. While SAU has been configured to remap the
	 * USART2 buffers into SAU HSB slave space.
	 */
	dmpu_entries[REGION_2_PERIPHERALS_INDEX].addr = HSB_PBA_START_ADDR;
	// Set the size of the PBA memory map as 32kB. and 2K each subregions.
	dmpu_entries[REGION_2_PERIPHERALS_INDEX].size = MPU_REGION_SIZE_32KB;
	// Deem this protection scheme as valid.
	dmpu_entries[REGION_2_PERIPHERALS_INDEX].valid = 1;
	if (set_mpu_entry(&dmpu_entries[REGION_2_PERIPHERALS_INDEX],
		REGION_2_PERIPHERALS_INDEX)) {
		leds_toggle_infinitely();  // Error
	}
	/*
	 * Set Access Permission A to Read/Write access in privileged mode and
	 * to None in unprivileged mode.
	 */
	set_access_permissions(REGION_2_PERIPHERALS_INDEX, MPU_APRA_ID,
		MPU_PRIVRW_UNPRIVNONE);
	set_access_permissions(REGION_2_PERIPHERALS_INDEX, MPU_APRB_ID,
		MPU_PRIVR_UNPRIVNONE);
	//for UC3C, GPIO is the 4th subregion
#if BOARD == UC3C_EK
	select_subregion(REGION_2_PERIPHERALS_INDEX, 0xffef);
#endif
#if BOARD == UC3L_EK
	//for UC3L, GPIO is the 5th subregion
	select_subregion(REGION_2_PERIPHERALS_INDEX, 0xffdf);
#endif

	/*
	 * Configuring Region 3: the SAU Channel Register memory mapping.
	 * We're using the SAU HSB slave space to access the TX buffer of USART
	 * in this example, and this space should not be protected by MPU.
	 */
	dmpu_entries[REGION_3_SAU_HSB_INDEX].addr = AVR32_SAU_SLAVE_ADDRESS;
	// The size of the PBA memory map is 64kB.
	dmpu_entries[REGION_3_SAU_HSB_INDEX].size = MPU_REGION_SIZE_64KB;
	// Deem this protection scheme as valid.
	dmpu_entries[REGION_3_SAU_HSB_INDEX].valid = 1;
	if (set_mpu_entry(&dmpu_entries[REGION_3_SAU_HSB_INDEX],
		REGION_3_SAU_HSB_INDEX)) {
		leds_toggle_infinitely();  // Error
	}
	/*
	 * Set Access Permission A to Read/Write access in privileged mode and 
	 * to None in unprivileged mode.
	 */
	set_access_permissions(REGION_3_SAU_HSB_INDEX, MPU_APRA_ID,
		MPU_PRIVRW_UNPRIVNONE);
	// Set all 16 subregions of the PBA memory map to Read/Write access.
	select_subregion(REGION_3_SAU_HSB_INDEX, 0x0000);

	/*
	 * Configuring Region 4: the SAU Configuration register memory mapping.
	 * In order to prevent the SAU configuration registers to be changed by
	 * malicious or runaway code, they should be protected by the MPU as
	 * soon as they have been configured. Since we need read the status of
	 * SAU, we configure this space as read only.
	 */
	dmpu_entries[REGION_4_SAU_INDEX].addr = HSB_PBB_START_ADDR;
	// The size of the PBA memory map is 64kB.
	dmpu_entries[REGION_4_SAU_INDEX].size = MPU_REGION_SIZE_64KB;
	// Deem this protection scheme as valid.
	dmpu_entries[REGION_4_SAU_INDEX].valid = 1;
	if (set_mpu_entry(&dmpu_entries[REGION_4_SAU_INDEX],
		REGION_4_SAU_INDEX)) {
		leds_toggle_infinitely();  // Error
	}
	/*
	 * Set Access Permission A to Read/Write access in privileged mode and
	 * to None in unprivileged mode.
	 */
	set_access_permissions(REGION_4_SAU_INDEX, MPU_APRA_ID,
		MPU_PRIVR_UNPRIVNONE);
	// Set all 16 subregions of the PBA memory map to Read/Write access.
	select_subregion(REGION_4_SAU_INDEX, 0x0000);

	enable_mpu(); // Enable the MPU address checking.
}

/* \brief Attempt to read UR register
 *
 */
void attempt_read_UR(void)
{
	volatile uint32_t data_ur;
	data_ur = AVR32_SAU_HSB.ur;
}

/* \brief Test if the USART is ready by SAU
 *
 */
bool sau_usart_tx_ready()
{
	if ((sau_read_channel(USART_CSR_CHANNEL) & AVR32_USART_CSR_TXRDY_MASK) 
		== 0) {
		return false;
	} else {
		return true; 
	}
}

/* \brief Main entry point
 * This is an example of how to setup the AVR32 UC3 SAU with MPU protected 
 * regions.
 */
void main(void)
{
	Enable_global_exception();
	Enable_global_interrupt();
	// Clear the LEDs.
	gpio_set_gpio_pin(LED0_GPIO); gpio_set_gpio_pin(LED1_GPIO);
	gpio_set_gpio_pin(LED2_GPIO); gpio_set_gpio_pin(LED3_GPIO);
	
	sysclk_init();
	init_dbg_rs232(EXAMPLE_TARGET_PBACLK_FREQ_HZ); 

	print_dbg("\r\n## This is the AT32UC3 SAU example ##\r\n");
	//Configure SAU to map USART registers
	sau_config_to_usart();
	//Configure SAU access in open mode
	sau_enable_open_mode();
	//Test SAU access in open mode
	print_dbg("\r\n # SAU access in open mode...       \r\n");
	while (sau_usart_tx_ready() == false);
	sau_write_channel(PRINT_EXAMPLE_CHAR, USART_THR_CHANNEL);

	//Interrupt test start
	print_dbg("\r\n # SAU Interrupt test...          ");
	 //And then interrupt test
	INTC_register_interrupt(&sau_int_handler, AVR32_SAU_IRQ,
		AVR32_INTC_INT0);
	// Enable SAU interrupt.
	sau_enable_interrupt(AVR32_SAU_IER_URREAD_MASK
		|AVR32_SAU_IDR_URKEY_MASK);
	//Use URREAD bit to generate interrupt test
	attempt_read_UR();

	//Configure SAU access in lock mode
	print_dbg("\r\n # SAU interrupt in lock mode test...       ");
	sau_lock_channels(EXAMPLE_UNLOCK_CLOCK_CYCLES, EXAMPLE_UNLOCK_KEY);
	//Use wrong unlock key to generate interrupt in lock mode 
	sau_unlock_channel(USART_THR_CHANNEL, EXAMPLE_WRONG_UNLOCK_KEY);
	//End of interrupt example test
	print_dbg("\r\n # SAU interrupt test done         ##\r\n");
	sau_disable_interrupt(AVR32_SAU_IER_URREAD_MASK
		|AVR32_SAU_IDR_URKEY_MASK);

	//Exception test  
	print_dbg("\r\n # SAU Bus Error response test... ");
	//Bus error response enable
	sau_enable_buserr_resp();
	//Use URREAD bit to generate interrupt test
	print_dbg("\r\n # SAU Bus Error response 1---URREAD  ##");
	attempt_read_UR();
	sau_clear_error_flags(AVR32_SAU_ICR_URREAD_MASK);
	//Access the channel without unlocking
	print_dbg("\r\n # SAU Bus Error response 2---");
	print_dbg("access unlocked channel ##\r\n");
	sau_write_channel(PRINT_EXAMPLE_CHAR, USART_THR_CHANNEL);
	// Clear error bits in SR
	sau_clear_error_flags(AVR32_SAU_ICR_CAU_MASK);
	do {
		sau_clear_error_flags(AVR32_SAU_ICR_CAU_MASK);
	} while (sau_status_check() != 0);

	//Disable bus error exception
	sau_disable_buserr_resp();

	//SAU works with MPU test
	mpu_memory_protect_config();

	//Access USART by accessing SAU
	do {
	    sau_unlock_channel(USART_CSR_CHANNEL, EXAMPLE_UNLOCK_KEY);
	} while (sau_usart_tx_ready() == false);
	sau_unlock_channel(USART_THR_CHANNEL, EXAMPLE_UNLOCK_KEY);
	sau_write_channel(PRINT_EXAMPLE_CHAR, USART_THR_CHANNEL);

	/*
	 * There will be exception if we access USART registers directly, since
	 * they are protected by MPU.
	 */
	print_dbg("T");
	// LED0 & LED1 blink alternatively forever.
	led_display_chaser_infinitely();
}
