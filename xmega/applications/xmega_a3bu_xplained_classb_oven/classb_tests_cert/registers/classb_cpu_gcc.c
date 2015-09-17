/**
 * \file
 *
 * \brief CPU register test that is compatible with AVR GCC.
 *
 * \par Application note:
 *      AVR1610: Guide to IEC60730 Class B compliance with XMEGA
 *
 * Copyright (C) 2011-2015 Atmel Corporation. All rights reserved.
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include "classb_cpu.h"

/**
 * \brief CPU register self-diagnostic routine for AVR GCC.
 *
 * This function tests the CPU registers, preserving those registers specified
 * in the AVR GCC compiler documentation. Note that if there should be a fault
 * in a register that needs preservation, its value would not be restored.
 *
 * Registers are tested in this order:
 * -# Critical registers
 *    -# Return value register: R24
 *    -# stack pointer: SPL/SPH
 *    -# register %file: R31 -- R30
 * -# Non-critical registers
 *    -# register %file: R29 -- R25, R23 -- R0
 *    -# extended addressing registers: RAMPD/X/Y/Z, EIND
 *    -# status register: SREG (except interrupt flag)
 *
 * \retval false No register fault detected.
 * \retval true Register fault detected.
 *
 * \note Interrupts must be disabled during this test.
 */
bool classb_register_test(void)
{
	/* The error flag used locally */
	register bool error asm("r24") = true;

	/* Test R24 first because it is the return register! */
	CLASSB_RegSet_R_HI(R24,0x55);
	CLASSB_RegTest_R_HI(R24,0xAA,CLASSB_ERROR_CRIT);
	CLASSB_RegTest_R_HI(R24,0x55,CLASSB_ERROR_CRIT);

	/* Then test R31 and R30 because they are used in the rest of the tests. */
	CLASSB_RegSet_R_HI(R31,0x55);
	CLASSB_RegTest_R_HI(R31,0xAA,CLASSB_ERROR_CRIT);
	CLASSB_RegTest_R_HI(R31,0x55,CLASSB_ERROR_CRIT);

	CLASSB_RegSet_R_HI(R30,0x55);
	CLASSB_RegTest_R_HI(R30,0xAA,CLASSB_ERROR_CRIT);
	CLASSB_RegTest_R_HI(R30,0x55,CLASSB_ERROR_CRIT);

	/* SPH */
	CLASSB_RegStore_IO(0x3D);
	CLASSB_RegSet_IO(0x3D,0x55);
	CLASSB_RegTest_IO(0x3D,0xAA,CLASSB_ERROR_CRIT);
	CLASSB_RegTest_IO(0x3D,0x55,CLASSB_ERROR_CRIT);
	CLASSB_RegRestore_IO(0x3D);

	/* SPL */
	CLASSB_RegStore_IO(0x3E);
	CLASSB_RegSet_IO(0x3E,0x55);
	CLASSB_RegTest_IO(0x3E,0xAA,CLASSB_ERROR_CRIT);
	CLASSB_RegTest_IO(0x3E,0x55,CLASSB_ERROR_CRIT);
	CLASSB_RegRestore_IO(0x3E);

	/* Test R29-R28 non-destructively. */
	CLASSB_RegStore_R(R29);
	CLASSB_RegSet_R_HI(R29,0x55);
	CLASSB_RegTest_R_HI(R29,0xAA,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegTest_R_HI(R29,0x55,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegRestore_R(R29);

	CLASSB_RegStore_R(R28);
	CLASSB_RegSet_R_HI(R28,0x55);
	CLASSB_RegTest_R_HI(R28,0xAA,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegTest_R_HI(R28,0x55,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegRestore_R(R28);

	/* Test R27-R25, R23-R18 destructively. */
	CLASSB_RegSet_R_HI(R27,0x55);
	CLASSB_RegTest_R_HI(R27,0xAA,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegTest_R_HI(R27,0x55,CLASSB_ERROR_NON_CRIT);

	CLASSB_RegSet_R_HI(R26,0x55);
	CLASSB_RegTest_R_HI(R26,0xAA,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegTest_R_HI(R26,0x55,CLASSB_ERROR_NON_CRIT);

	CLASSB_RegSet_R_HI(R25,0x55);
	CLASSB_RegTest_R_HI(R25,0xAA,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegTest_R_HI(R25,0x55,CLASSB_ERROR_NON_CRIT);

	CLASSB_RegSet_R_HI(R23,0x55);
	CLASSB_RegTest_R_HI(R23,0xAA,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegTest_R_HI(R23,0x55,CLASSB_ERROR_NON_CRIT);

	CLASSB_RegSet_R_HI(R22,0x55);
	CLASSB_RegTest_R_HI(R22,0xAA,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegTest_R_HI(R22,0x55,CLASSB_ERROR_NON_CRIT);

	CLASSB_RegSet_R_HI(R21,0x55);
	CLASSB_RegTest_R_HI(R21,0xAA,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegTest_R_HI(R21,0x55,CLASSB_ERROR_NON_CRIT);

	CLASSB_RegSet_R_HI(R20,0x55);
	CLASSB_RegTest_R_HI(R20,0xAA,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegTest_R_HI(R20,0x55,CLASSB_ERROR_NON_CRIT);

	CLASSB_RegSet_R_HI(R19,0x55);
	CLASSB_RegTest_R_HI(R19,0xAA,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegTest_R_HI(R19,0x55,CLASSB_ERROR_NON_CRIT);

	CLASSB_RegSet_R_HI(R18,0x55);
	CLASSB_RegTest_R_HI(R18,0xAA,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegTest_R_HI(R18,0x55,CLASSB_ERROR_NON_CRIT);

	/* Test R17-R0 non-destructively. */
	CLASSB_RegStore_R(R17);
	CLASSB_RegSet_R_HI(R17,0x55);
	CLASSB_RegTest_R_HI(R17,0xAA,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegTest_R_HI(R17,0x55,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegRestore_R(R17);

	CLASSB_RegStore_R(R16);
	CLASSB_RegSet_R_HI(R16,0x55);
	CLASSB_RegTest_R_HI(R16,0xAA,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegTest_R_HI(R16,0x55,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegRestore_R(R16);

	CLASSB_RegStore_R(R15);
	CLASSB_RegSet_R_LO(R15,0x55);
	CLASSB_RegTest_R_LO(R15,0xAA,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegTest_R_LO(R15,0x55,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegRestore_R(R15);

	CLASSB_RegStore_R(R14);
	CLASSB_RegSet_R_LO(R14,0x55);
	CLASSB_RegTest_R_LO(R14,0xAA,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegTest_R_LO(R14,0x55,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegRestore_R(R14);

	CLASSB_RegStore_R(R13);
	CLASSB_RegSet_R_LO(R13,0x55);
	CLASSB_RegTest_R_LO(R13,0xAA,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegTest_R_LO(R13,0x55,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegRestore_R(R13);

	CLASSB_RegStore_R(R12);
	CLASSB_RegSet_R_LO(R12,0x55);
	CLASSB_RegTest_R_LO(R12,0xAA,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegTest_R_LO(R12,0x55,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegRestore_R(R12);

	CLASSB_RegStore_R(R11);
	CLASSB_RegSet_R_LO(R11,0x55);
	CLASSB_RegTest_R_LO(R11,0xAA,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegTest_R_LO(R11,0x55,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegRestore_R(R11);

	CLASSB_RegStore_R(R10);
	CLASSB_RegSet_R_LO(R10,0x55);
	CLASSB_RegTest_R_LO(R10,0xAA,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegTest_R_LO(R10,0x55,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegRestore_R(R10);

	CLASSB_RegStore_R(R9);
	CLASSB_RegSet_R_LO(R9,0x55);
	CLASSB_RegTest_R_LO(R9,0xAA,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegTest_R_LO(R9,0x55,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegRestore_R(R9);

	CLASSB_RegStore_R(R8);
	CLASSB_RegSet_R_LO(R8,0x55);
	CLASSB_RegTest_R_LO(R8,0xAA,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegTest_R_LO(R8,0x55,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegRestore_R(R8);

	CLASSB_RegStore_R(R7);
	CLASSB_RegSet_R_LO(R7,0x55);
	CLASSB_RegTest_R_LO(R7,0xAA,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegTest_R_LO(R7,0x55,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegRestore_R(R7);

	CLASSB_RegStore_R(R6);
	CLASSB_RegSet_R_LO(R6,0x55);
	CLASSB_RegTest_R_LO(R6,0xAA,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegTest_R_LO(R6,0x55,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegRestore_R(R6);

	CLASSB_RegStore_R(R5);
	CLASSB_RegSet_R_LO(R5,0x55);
	CLASSB_RegTest_R_LO(R5,0xAA,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegTest_R_LO(R5,0x55,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegRestore_R(R5);

	CLASSB_RegStore_R(R4);
	CLASSB_RegSet_R_LO(R4,0x55);
	CLASSB_RegTest_R_LO(R4,0xAA,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegTest_R_LO(R4,0x55,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegRestore_R(R4);

	CLASSB_RegStore_R(R3);
	CLASSB_RegSet_R_LO(R3,0x55);
	CLASSB_RegTest_R_LO(R3,0xAA,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegTest_R_LO(R3,0x55,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegRestore_R(R3);

	CLASSB_RegStore_R(R2);
	CLASSB_RegSet_R_LO(R2,0x55);
	CLASSB_RegTest_R_LO(R2,0xAA,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegTest_R_LO(R2,0x55,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegRestore_R(R2);

	CLASSB_RegStore_R(R1);
	CLASSB_RegSet_R_LO(R1,0x55);
	CLASSB_RegTest_R_LO(R1,0xAA,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegTest_R_LO(R1,0x55,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegRestore_R(R1);

	CLASSB_RegStore_R(R0);
	CLASSB_RegSet_R_LO(R0,0x55);
	CLASSB_RegTest_R_LO(R0,0xAA,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegTest_R_LO(R0,0x55,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegRestore_R(R0);

	/* Test IO registers non-destructively. */
	/* RAMPD */
#ifdef CLASSB_HAS_BIGMEM
	CLASSB_RegStore_IO(0x38);
	CLASSB_RegSet_IO(0x38,0x55);
	CLASSB_RegTest_IO(0x38,0xAA,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegTest_IO(0x38,0x55,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegRestore_IO(0x38);
#endif

	/* RAMPX */
#ifdef CLASSB_HAS_BIGMEM
	CLASSB_RegStore_IO(0x39);
	CLASSB_RegSet_IO(0x39,0x55);
	CLASSB_RegTest_IO(0x39,0xAA,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegTest_IO(0x39,0x55,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegRestore_IO(0x39);
#endif

	/* RAMPY */
#ifdef CLASSB_HAS_BIGMEM
	CLASSB_RegStore_IO(0x3A);
	CLASSB_RegSet_IO(0x3A,0x55);
	CLASSB_RegTest_IO(0x3A,0xAA,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegTest_IO(0x3A,0x55,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegRestore_IO(0x3A);
#endif

	/* RAMPZ */
#if defined(CLASSB_HAS_BIGMEM) || defined(CLASSB_HAS_BIGFLASH)
	CLASSB_RegStore_IO(0x3B);
	CLASSB_RegSet_IO(0x3B,0x55);
	CLASSB_RegTest_IO(0x3B,0xAA,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegTest_IO(0x3B,0x55,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegRestore_IO(0x3B);
#endif

	/* EIND */
#ifdef CLASSB_HAS_BIGFLASH
	CLASSB_RegStore_IO(0x3C);
	CLASSB_RegSet_IO(0x3C,0x55);
	CLASSB_RegTest_IO(0x3C,0xAA,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegTest_IO(0x3C,0x55,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegRestore_IO(0x3C);
#endif

	/* SREG (Not interrupt flag!) */
	CLASSB_RegStore_IO(0x3F);
	CLASSB_RegSet_IO(0x3F,0x55);
	CLASSB_RegTest_IO(0x3F,0x2A,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegTest_IO(0x3F,0x55,CLASSB_ERROR_NON_CRIT);
	CLASSB_RegRestore_IO(0x3F);

	ASSEMBLY(
		/* Continue here if test was successful: Clear return register and return. */
		"clr %0 \n\t"
		"jmp END \n\t"
		"CLASSB_LABEL1: jmp CLASSB_LABEL1 \n\t"
		/* If a fault was detected, set return register. */
		"CLASSB_LABEL2: ldi %0, 0x01 \n\t"
		"END: \n\t"
		/* Output value */
		:"=r" (error)
		/* No inputs */
		:
	);

	/* Handle global error. This will not be reached if there was a fault in
	 * a) critical registers and CLASSB_ERROR_CRIT was defined as
	 * CLASSB_LABEL1.
	 * b) non-critical registers and CLASSB_ERROR_NON_CRIT was defined as
	 * CLASSB_LABEL1.
	 */
	if (error == true) {
		CLASSB_ERROR_HANDLER_REGISTERS();
	}

	return (error);
}
