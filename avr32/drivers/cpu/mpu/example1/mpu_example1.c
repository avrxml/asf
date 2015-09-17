/**
 * \file
 *
 * \brief Example of usage of the Software Framework MPU driver in AVR UC3
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
 */

/**
 * \mainpage
 * \section intro Introduction
 * This is the documentation for the data structures, functions, variables,
 * defines, enums, and typedefs for the MPU driver.\n It also gives an example
 * of the usage of the MPU module.
 *
 * The MPU allows the user to divide the memory space into different protection
 * regions. These regions have a user-defined size and start at a user-defined
 * address. The different regions can have different access privileges
 * attributes.
 *
 * You can also read the Memory Protection Unit section in the AVR32 UC
 * technical reference guide.
 *
 * \section description Example Description
 * - The memory areas are set up in the following manner:
 * <table>
 * <CAPTION><em>Access permissions of used memory areas</em></CAPTION>
 * <tr>
 *  <td><em> Memory Range </em></td>
 *  <td><em> Access Permissions </em></td>
 * </tr>
 * <tr>
 *  <td> RAM   [0x00000000 - 0x00000F00[, Region 1, subregions 0 to 14 </td>
 *  <td> Read / Write </td>
 * </tr>
 * <tr>
 *  <td> RAM   [0x00000F00 - 0x00001000[, Region 1, subregion 15 </td>
 *  <td> Read </td>
 * </tr>
 * <tr>
 *  <td> Stack [&stack - &stack + stack size[, Region 2, all subregions </td>
 *  <td> Read / Write </td>
 * </tr>
 * <tr>
 *  <td> Flash [0x80000000 - 0x8000C000[, Region 0, subregions 0 to 11</td>
 *  <td> Read / eXecute </td>
 * </tr>
 * <tr>
 *  <td> Flash [0x8000C000 - 0x80010000[, Region 0, subregions 12 to 15 </td>
 *  <td> Read </td>
 * </tr>
 * <tr>
 *  <td> PBA memory map[0xFFFF0000 - 0xFFFFFFFF], Region 3, all subregions</td>
 *  <td> Read / Write </td>
 * </tr>
 * </table>
 * - The application then performs a write access to Region 1/subregion 15.
 * That triggers a DTLB write protection exception => MPU_STATE changes to
 * DTLB_WRITE_EXCEPTION and LED3 lights up.
 * - The application then calls a function in Region 0/Subregion 12.
 * That triggers an ITLB protection exception => MPU_STATE changes to
 * ITLB_PROTECTION_EXCEPTION and LED2 lights up.
 * - End of example: LED0 & LED1 alternatively blink forever.
 *
 * \section files Main Files
 * - mpu.c: MPU driver
 * - mpu.h: MPU driver header file
 * - mpu_example1.c: MPU example
 * - mpu_exception.S: asm file for exception support (GCC)
 * - mpu_exception.s82: asm file for exception support and declaration of
 *                      function specially located (IAR)
 *
 * \section compinfo Compilation Info
 * This software was written for the GCC for AVR32 and IAR Embedded System
 * Workbench for AVR32.
 *
 * \section Configuration Information
 * This example has been tested with the following configuration:
 * - EVK1100, EVK1101, EVK1104, EVK1105, UC3L_EK, UC3C_EK, UC3-A3 XPLAINED,
 *   UC3-L0 XPLAINED boards.
 * - CPU clock: 16MHz in UC3C_EK and 12 MHz in all other boards.
 * - PC terminal settings:
 *   - 57600 bps,
 *   - 8 data bits,
 *   - no parity bit,
 *   - 1 stop bit,
 *   - no flow control.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/uc3">Atmel AVR UC3</A>.\n
*/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <stdint.h>

#include "board.h"
#include "compiler.h"
#include "print_funcs.h"
#include "sysclk.h"
#include "gpio.h"
#include "mpu.h"
#include "cycle_counter.h"

/**
 * \name Regions
 */
//! @{

// Number of regions considered in this example.
#define NB_CONFIG_MPU_ENTRIES       4
// Region Index
#define REGION_0_FLASH_INDEX        0
#define REGION_1_RAM_INDEX          1
#define REGION_2_STACK_INDEX        2
#define REGION_3_PERIPHERALS_INDEX  3

//! @}

//! \brief Exception State
enum exception_status {
	/**
	 * No exception has occurred.
	 */
	NO_EXCEPTION = 0,
	/**
	 * The TLB Multiple Hit Exception is generated when an access hits in
	 * multiple MPU regions. This is usually caused by programming error.
	 */
	TLB_MULTIPLE_HIT_EXCEPTION = 1,
	/**
	 * The ITLB Miss exception is generated when the MPU is enabled and the
	 * instruction memory access does not hit in any regions.
	 */
	ITLB_MISS_EXCEPTION = 2,
	/**
	 * The ITLB Protection exception is generated when the instruction memory
	 * access violates the access rights specified by the protection region in
	 * which the address lies.
	 */
	ITLB_PROTECTION_EXCEPTION = 3,
	/**
	 * The DTLB Protection exception is generated when the data memory write
	 * violates the access rights specified by the protection region in which
	 * the address lies.
	 */
	DTLB_WRITE_PROTECTION_EXCEPTION = 4,
	/**
	 * The DTLB Write Miss exception is generated when the MPU is enabled and
	 * the data memory write access does not hit in any regions.
	 */
	DTLB_WRITE_MISS_EXCEPTION = 5,
	/**
	 * The DTLB Protection exception is generated when the data memory read
	 * violates the access rights specified by the protection region in which
	 * the address lies.
	 */
	DTLB_READ_PROTECTION_EXCEPTION = 6,
	/**
	 * The DTLB Read Miss exception is generated when the MPU is enabled and
	 * the data memory read access does not hit in any regions.
	 */
	DTLB_READ_MISS_EXCEPTION = 7,
	/**
	 * Any other unexpected exception
	 */
	UNEXPECTED_EXCEPTION = -1
};

// Function to handle exception
void handle_exception(uint32_t *sp, uint32_t exception_address,
		uint32_t exception_cause);

// Allocation for NB_CONFIG_MPU_ENTRIES DMPU entries
mpu_entry_t dmpu_entries[NB_CONFIG_MPU_ENTRIES];
// Exception state
enum exception_status exception_state = NO_EXCEPTION;

#if defined (__GNUC__)
/**
 * \brief Function with restricted access
 *
 * Forbidden_access_func() is placed in the section .ForbiddenText. This region
 * is configured as non-executable in configure_memory_area().
 * .ForbiddenText is a section created for this example, placed in a well-known
 * location in Flash. See the  linker script bundled with the example.
 */
__attribute__ ((__naked__, __section__(".ForbiddenText")))
#elif __ICCAVR32__
#pragma shadow_registers = full
#pragma location="FORBIDDENTEXT"
#endif
static void forbidden_access_func(void) /*@"FORBIDDENTEXT"*/
{
#if defined (__GNUC__)
	__asm__ __volatile__ (
	".balign 0x4\n\t"
	// We'll get an MPU exception here.
	"nop\n\t"
	/*
	 * This is setting the PC to the instruction following the call of
	 * forbidden_access_func.
	 */
	"mov     pc,lr"
	::
	);
#elif defined (__ICCAVR32__)
	__asm__ __volatile__ (
	// We'll get an MPU exception here.
	"nop\n\t"
	/*
	 * This is setting the PC to the instruction following the call of
	 * forbidden_access_func.
	 */
	"mov     pc,lr"
	);
#endif
}

/**
 * \brief Handle exception; called by the _evba assembly function.
 *
 * \param *sp               pointer to stack before the exception
                            (provided through R12.
                            At this point, sp[0]=SR, sp[1]=PC.
 * \param exception_address address of the offending instruction
 * \param exception_cause   exception cause ((EVBA offset of the exception)>>2)
 */
void handle_exception(uint32_t *sp, uint32_t exception_address,
		uint32_t exception_cause)
{
	/**
	 * \note Test exception cause register AVR32_ECR. AVR32_ECR is updated
	 * with a value equal to the 9 lowest bits of the EVBA offset of the
	 * exception shifted 2 bits to the right.
	 */
	switch (exception_cause << 2) {
	case AVR32_EVBA_OFFSET_TLB_MULTIPLE:
		/*
		 * The TLB Multiple Hit Exception is generated when an access hits in
		 * multiple MPU regions. This is usually caused by programming error.
		 */
		exception_state = TLB_MULTIPLE_HIT_EXCEPTION;
		LED_On(LED0);
		// Disable the MPU to avoid to trigger the exception again!
		disable_mpu();
		break;

	case AVR32_EVBA_OFFSET_ITLB_MISS:
		/*
		 * The ITLB Miss exception is generated when the MPU is enabled and the
		 * instruction memory access does not hit in any regions.
		 */
		exception_state = ITLB_MISS_EXCEPTION;
		LED_On(LED1);
		// Disable the MPU to avoid to trigger the exception again!
		disable_mpu();
		break;

	case AVR32_EVBA_OFFSET_ITLB_PROT:
		/*
		 * The ITLB Protection exception is generated when the instruction
		 * memory access violates the access rights specified by the
		 * protection region in which the address lies.
		 */
		exception_state = ITLB_PROTECTION_EXCEPTION;
		LED_On(LED2);
		// Disable the MPU to avoid to trigger the exception again!
		disable_mpu();
		break;

	case AVR32_EVBA_OFFSET_DTLB_PROT_W:
		/*
		 * The DTLB Protection exception is generated when the data memory
		 * write violates the access rights specified by the protection region
		 * in which the address lies.
		 */
		exception_state = DTLB_WRITE_PROTECTION_EXCEPTION;

		/*
		 * Update the return from exception PC address to the next instruction,
		 * to avoid triggering the exception again.
		 * Check if the instruction that generated the exception is extended
		 * (4 bytes) or compact (2 bytes) to find out the address of the next
		 * instruction which we should return to. All extended instructions have
		 * the 3 MSb always set, while this pattern is forbidden for compact
		 * instructions.
		 */
		if ((*((uint8_t*)exception_address) & 0xE0) == 0xE0) {
			// Extended instruction.
			sp[1] = exception_address + 4;
		} else {
			// Compact instruction
			sp[1] = exception_address + 2;
		}
		LED_On(LED3);
		break;

	case AVR32_EVBA_OFFSET_DTLB_MISS_W:
		/*
		 * The DTLB Write Miss exception is generated when the MPU is enabled
		 * and the data memory write access does not hit in any regions.
		 */
		exception_state = DTLB_WRITE_MISS_EXCEPTION;
#if defined(LED4)
		LED_On(LED4);
#endif
		// Disable the MPU to avoid to trigger the exception again!
		disable_mpu();
		break;

	case AVR32_EVBA_OFFSET_DTLB_PROT_R:
		/*
		 * The DTLB Protection exception is generated when the data memory read
		 * violates the access rights specified by the protection region in
		 * which the address lies.
		 */
		exception_state = DTLB_READ_PROTECTION_EXCEPTION;
#if defined(LED5)
		LED_On(LED5);
#endif
		// Disable the MPU to avoid to trigger the exception again!
		disable_mpu();
		break;

	case AVR32_EVBA_OFFSET_DTLB_MISS_R:
/*
		 * The DTLB Read Miss exception is generated when the MPU is enabled
		 * and the data memory read access does not hit in any regions.
		 */
		exception_state = DTLB_READ_MISS_EXCEPTION;
#if defined(LED6)
		LED_On(LED6);
#endif
		// Disable the MPU to avoid to trigger the exception again!
		disable_mpu();
		break;

    default:
		// Unexpected exception
		exception_state = UNEXPECTED_EXCEPTION;
#if defined(LED7)
		LED_On(LED7);
#endif
		break;
  }
}

/**
 * \brief Error occurred during MPU configuration
 *
 * This function is called when an error occurs in configuring MPU. LEDs are
 * toggled infinitely in a loop.
 */
static void mpu_configuration_error(void)
{
	volatile uint32_t i;
	LED_On(LED0 | LED1 | LED2 | LED3);
	while (true) {
		i = 100000;
		while (i--);
		LED_Toggle(LED0 | LED1 | LED2 | LED3);
	}
}

/**
 * \brief LED chaser function
 *
 * This function is called when the example has completed successfully.
 * LED0 & LED1 blink alternatively forever after the example completion.
 */
static void infinite_led_chaser(void)
{
	volatile uint32_t i;
	uint8_t led_map = 0x01;
	while (true) {
		// Turn the current LED on only and move to next LED.
		LED_Display_Field(LED0 | LED1, led_map);
		i = 100000;
		while (i--);
		led_map = max((led_map << 1) & 0x0F, 0x01);
	}
}

/**
 * \brief Configure the access permissions for different memory regions
 *
 * Assign different access permissions to the memory regions
 * - Flash Region 0, subregions 0 to 11      - Read / eXecute
 * - Flash Region 0, subregions 12 to 15     - Read
 * - RAM Region 1, subregions 0 to 14        - Read / Write
 * - RAM Region 1, subregion 15              - Read
 * - Stack Region 2, all subregions          - Read / Write
 * - PBA memory map Region 3, all subregions - Read / Write
 */
static void configure_memory_area(void)
{
	avr32_config1_t config1;    // configuration register
	uint8_t mpu_entries;        // Number of MPU entries.
	eRegionSize region_size;    // Region size
	uint32_t region_size_value; // A region size expressed in kB
#if defined (__GNUC__)
	extern void _stack;         // Start address of the stack.
	extern void __stack_size__; // Size of the stack.
#elif defined (__ICCAVR32__)
	#pragma segment = "SSTACK"
#endif

	// Read the CONFIG1 register to check the MPU configuration.
	*(uint32_t *)&config1 = Get_system_register(AVR32_CONFIG1);
	mpu_entries = config1.dmmusz;
	print_dbg("\r\nNumber of MPU entries: 0x");
	// mpu_entries should be 8 on UC3.
	print_dbg_hex(mpu_entries);

	print_dbg("\r\nSetting up all MPU entries:");
	/*
	 * Configuring Region 0: the Flash memory area occupied by this
	 * application. Since this program is linked into flash, we want to define
	 * the Flash memory protection scheme as being Readable and Executable(RX).
	 * The flash memory starts at address 0x80000000 (this info is found in the
	 * part specific header file found in the compiler installation tree;
	 * e.g. in uc3a0512.h for an AT32UC3A0512 part).
	 */
	dmpu_entries[REGION_0_FLASH_INDEX].addr = AVR32_FLASH_ADDRESS;

	/*
	 * We estimate the size of this application to be less than 64kB. So we
	 * define region 0 size as being 64kB.
	 */
	dmpu_entries[REGION_0_FLASH_INDEX].size = MPU_REGION_SIZE_64KB;

	// Deem this protection scheme as valid.
	dmpu_entries[REGION_0_FLASH_INDEX].valid = true;
	// set the mpu configuration for Region 0
	if (set_mpu_entry(&dmpu_entries[REGION_0_FLASH_INDEX],
			REGION_0_FLASH_INDEX)) {
		mpu_configuration_error();
	}
	/*
	 * Set Access Permission A to Read/Execute access in both privileged
	 * and unprivileged mode.
	 */
	set_access_permissions(REGION_0_FLASH_INDEX, MPU_APRA_ID,
			MPU_PRIVRX_UNPRIVRX);

	/*
	 * Set Access Permission B to Read access in privileged mode and to
	 * None in unprivileged mode.
	 */
	set_access_permissions(REGION_0_FLASH_INDEX, MPU_APRB_ID,
			MPU_PRIVR_UNPRIVNONE);

	/*
	 * Each region is divided into 16 sub-regions. So in the case of this
	 * region (region 0) we are currently setting up, each sub-region is
	 * 4kB in size.
	 * All subregions that hold our application should be in EXECUTE access;
	 * except for the ForbiddenFunc function which we implicitly put towards
	 * the end of the .text section, for test purpose.
	 * We estimated that our application should fit in much less that 64kB. So
	 * we configure the subregions 0 to 11 to use the permission access
	 * Read/eXecute as defined in the MPUAPRA register and the remaining
	 * subregions (12 to 15) to use the permission access Read. Each subregion
	 * is mapped to a bit: when the bit is set to 0, the subregion permission
	 * access is taken from the MPUAPRA register (previously defined as
	 * MPU_PRIVRX_UNPRIVRX for region 0); if the bit is set to 1; the subregion
	 * permission access is taken from the MPUAPRB register (previously set to
	 * MPU_PRIVR_UNPRIVNONE for region 0).
	 * We purposely placed the ForbiddenFunc function in one of the subregion
	 * 12 to 15; so that when the code jumps to this location, we should get an
	 * ITLB exception.
	 */
	select_subregion(REGION_0_FLASH_INDEX, 0xF000);

	/*
	 * Configuring Region 1: the internal RAM memory used by this application.
	 * The RAM memory starts at address 0x00000000 (this info is found in the
	 * part specific header file found in the compiler installation tree;
	 * e.g. in uc3a0512.h for an AT32UC3A0512 part).
	 */
	dmpu_entries[REGION_1_RAM_INDEX].addr = AVR32_SRAM_ADDRESS;

	/*
	 * We estimate the RAM footprint of this application to be less than 4kB.
	 * So we define region 1 size as being 4kB in size.
	 */
	dmpu_entries[REGION_1_RAM_INDEX].size = MPU_REGION_SIZE_4KB;

	// Deem this protection scheme as valid.
	dmpu_entries[REGION_1_RAM_INDEX].valid = true;
	// set the mpu configuration for Region 1
	if (set_mpu_entry(&dmpu_entries[REGION_1_RAM_INDEX], REGION_1_RAM_INDEX)) {
		mpu_configuration_error();
	}

	/*
	 * Set Access Permission A to Read/Write access in privileged mode and to
	 * None in unprivileged mode.
	 */
	set_access_permissions(REGION_1_RAM_INDEX, MPU_APRA_ID,
			MPU_PRIVRW_UNPRIVNONE);

	/*
	 * Set Access Permission B to Read access in privileged mode and to None in
	 * unprivileged mode.
	 */
	set_access_permissions(REGION_1_RAM_INDEX, MPU_APRB_ID,
			MPU_PRIVR_UNPRIVNONE);

	/*
	 * Each region is divided into 16 sub-regions. So in the case of region 1
	 * we are currently setting up, each sub-region is 256B in size.
	 * All subregions that our application uses should be in Read/Write access;
	 * except for the last subregion (in [0x00000F00, 0x00001000[)
	 * (for test purpose).
	 * We estimated that our application should use much less than 4kB. So we
	 * configure the subregions 0 to 14 to use the permission access Read/Write
	 * as defined in the MPUAPRA register and the remaining subregion (15) to
	 * use the permission access Read.
	 * Each subregion is mapped to a bit: when the bit is set to 0, the
	 * subregion permission access is taken from the MPUAPRA register
	 * (previously defined as MPU_PRIVRW_UNPRIVNONE for region 1); if the  bit
	 * is set to 1; the subregion permission access is taken from the MPUAPRB
	 * register (previously set to MPU_PRIVR_UNPRIVNONE for region 1).
	 * For the sake of this example, the application will try to do a write
	 * access at an address in the range [0x00000F00, 0x00001000[: we should
	 * then get a DTLB exception.
	 */
	select_subregion(REGION_1_RAM_INDEX, 0x8000);

	/*
	 * Configuring Region 2: the memory used by the stack.
	 * According to the linker script and to the linker step, the stack is
	 * placed in INTRAM (internal RAM memory) after the RAM area explicitly
	 * used by the application.
	 * The stack is used upon exceptions (some registers are saved on the
	 * stack; See "AVR32 Architecture Manual Complete" section 7.1 Event
	 * Handling in AVR32A); the stack may be used when doing function calls
	 * also. Since this application intends to generate exceptions, we must
	 * make sure that the memory area used by the stack is Read/Write!
	 */
#if defined (__GNUC__)
	dmpu_entries[REGION_2_STACK_INDEX].addr = (uint32_t)&_stack;
#elif defined (__ICCAVR32__)
	dmpu_entries[REGION_2_STACK_INDEX].addr =
			(uint32_t)__segment_begin("SSTACK");
#endif

	/**
	 * \note WARNING NOTE: there are limitations concerning the region size;
	 * See doc32002 "AVR32UC Technical Reference Manual Complete" Table 6-1.
	 * "Protection region sizes implied by the Size field".
	 * The mpu_convert_kbsize_to_eregionsize() makes sure this part of the
	 * spec is respected.
	 */
#if defined (__GNUC__)
	region_size_value = (uint32_t)&__stack_size__ >> 10;
#elif defined (__ICCAVR32__)
	region_size_value = ((uint32_t)__segment_end("SSTACK") -
			(uint32_t)__segment_begin("SSTACK")) >> 10;
#endif

	if (!mpu_convert_kbsize_to_eregionsize(&region_size, region_size_value)) {
		mpu_configuration_error();
	}

	dmpu_entries[REGION_2_STACK_INDEX].size = region_size;

	// Deem this protection scheme as valid.
	dmpu_entries[REGION_2_STACK_INDEX].valid = true;
	// set the mpu configuration for Region 2
	if (set_mpu_entry(&dmpu_entries[REGION_2_STACK_INDEX],
			REGION_2_STACK_INDEX)) {
		mpu_configuration_error();
	}

	/*
	 * Set Access Permission A to Read access in privileged mode and to None in
	 * unprivileged mode.
	 */
	set_access_permissions(REGION_2_STACK_INDEX, MPU_APRA_ID,
			MPU_PRIVR_UNPRIVNONE);

	/*
	 * Set Access Permission B to Read/Write access in privileged mode and to
	 * None in unprivileged mode.
	 */
	set_access_permissions(REGION_2_STACK_INDEX, MPU_APRB_ID,
			MPU_PRIVRW_UNPRIVNONE);

	/*
	 * Set all subregions of the stack to Read/Write access
	 * (i.e. use the Access Permissions B).
	 */
	select_subregion(REGION_2_STACK_INDEX,0xFFFF);

	/*
	 * Configuring Region 3: the peripherals memory mapping.
	 * We're using the DEBUG module to output traces to USART, we're using the
	 * GPIO module => we have to define access permissions to the region where
	 * the USART & GPIO peripherals registers are mapped. We'll set this region
	 * as Read/Write because we don't want to generate MPU exceptions when
	 * using a peripheral.
	 * The USART & GPIO peripherals are on the same Peripheral Bus (PBA). We
	 * configure all peripherals on PBA with the same access rights.
	 * The PBA physical memory map starts at address 0xFFFF0000 (this info is
	 * found in the datasheet in the table "AT32UC3x Physical Memory Map".
	 * The PDCA module happens to be the module that is mapped at the start of
	 * the PBA memory map in all devices except AT32UC3C series(INTC module).
	 */
#if UC3C
	dmpu_entries[REGION_3_PERIPHERALS_INDEX].addr = AVR32_INTC_ADDRESS;
#else
	dmpu_entries[REGION_3_PERIPHERALS_INDEX].addr = AVR32_PDCA_ADDRESS;
#endif

	// The size of the PBA memory map is 64kB.
	dmpu_entries[REGION_3_PERIPHERALS_INDEX].size = MPU_REGION_SIZE_64KB;

	// Deem this protection scheme as valid.
	dmpu_entries[REGION_3_PERIPHERALS_INDEX].valid = true;

	// set the mpu configuration for Region 3
	if (set_mpu_entry(&dmpu_entries[REGION_3_PERIPHERALS_INDEX],
			REGION_3_PERIPHERALS_INDEX)) {
		mpu_configuration_error();
	}

	/*
	 * Set Access Permission A to Read/Write access in privileged mode and to
	 * None in unprivileged mode.
	 */
	set_access_permissions(REGION_3_PERIPHERALS_INDEX, MPU_APRA_ID,
			MPU_PRIVRW_UNPRIVNONE);

	/*
	 * We don't set the Access Permission B because we won't use it.
	 * Set all 16 subregions of the PBA memory map to Read/Write access.
	 */
	select_subregion(REGION_3_PERIPHERALS_INDEX, 0x0000);

	// Enable the MPU address checking.
	enable_mpu();
}


/**
 * \brief Main function
 *
 * This example describes how to setup the AVR32 UC3 MPU with protected
 * regions.\n
 * The following exceptions arises in the main function when trying to access
 * the protected regions.
 * - AVR32_EVBA_OFFSET_ITLB_PROT -> On trying to execute from a Read only
 *                                  (eXecute disabled)Flash region
 * - AVR32_EVBA_OFFSET_DTLB_PROT_W -> On writing to Read only region in RAM
 */
int main(void)
{
	/*
	 * \note the call to sysclk_init() will disable all non-vital
	 * peripheral clocks, except for the peripheral clocks explicitly
	 * enabled in conf_clock.h.
	 */
	sysclk_init();
	// Enable exceptions
	Enable_global_exception();
	// Enable interrupts
	cpu_irq_enable();
	// Initialize the USART module to print trace messages.
	init_dbg_rs232(sysclk_get_pba_hz());
	print_dbg("\r\n## This is the AT32UC3 MPU example ##");

	/*
	 * Set up and enable the MPU to protect the following memory areas:
	 *                                             --------------------
	 *                                            | Access permissions |
	 *  ------------------------------------------|--------------------|
	 * | Flash [0x80000000 - 0x8000C000[          | Read / eXecute     |
	 * | Region 0, subregions 0 to 11             |                    |
	 * |------------------------------------------|--------------------|
	 * | Flash [0x8000C000 - 0x80010000[          | Read               |
	 * | Region 0, subregions 12 to 15            |                    |
	 * |------------------------------------------|--------------------|
	 * | RAM   [0x00000000 - 0x00000F00[          | Read / Write       |
	 * | Region 1, subregions 0 to 14             |                    |
	 * |------------------------------------------|--------------------|
	 * | RAM   [0x00000F00 - 0x00001000[          | Read               |
	 * | Region 1, subregion 15                   |                    |
	 * |------------------------------------------|--------------------|
	 * | Stack [&stack - &stack + stack size[     | Read / Write       |
	 * | Region 2, all subregions                 |                    |
	 *  ---------------------------------------------------------------|
	 * | PBA memory map [0xFFFF0000 - 0xFFFFFFFF] | Read / Write       |
	 * | Region 3, all subregions                 |                    |
	 *  ---------------------------------------------------------------
	 */
	configure_memory_area();

	/*
	 * The address 0x00000F00 is mpu-configured as Read only. We perform a
	 * Write access there, so that should trigger a DTLB write protection
	 * exception => exception_state changes to DTLB_WRITE_PROTECTION_EXCEPTION
	 * and LED3 lights up.
	 */
	print_dbg("\r\nRAM - Subregion[0x00000F00 - 0x00001000]");
	print_dbg("\r\nTesting write protection ");
	*((volatile uint32_t*)0x00000F00) = 0x55AA55AA;
	if(exception_state == DTLB_WRITE_PROTECTION_EXCEPTION) {
		print_dbg("\r\nDTLB write protection exception triggered on writing.");
	}

	/*
	 * Wait two seconds before doing the next test, to clearly see that LED3
	 * lights up first then LED2 does.
	 */
	cpu_delay_ms(2000, FOSC0);

	/*
	 * The memory area [0x8000C000 - 0x80010000] is mpu-configured as Read
	 * only. We forced the forbidden_access_func() function to be mapped in
	 * this area. Calling this function should thus trigger an ITLB protection
	 * exception => exception_state changes to ITLB_PROTECTION_EXCEPTION and
	 * LED2 lights up.
	 */
	print_dbg("\r\nFLASH - Subregion [0x8000C000 - 0x80010000]");
	print_dbg("\r\nTesting the eXecute protection in FLASH");
	forbidden_access_func();
	if(exception_state == ITLB_PROTECTION_EXCEPTION) {
		print_dbg("\r\n ITLB protection exception triggered on execution.");
	}

	// LED0 & LED1 blink alternatively forever after the example completion.
	infinite_led_chaser();

	return 0;
}
