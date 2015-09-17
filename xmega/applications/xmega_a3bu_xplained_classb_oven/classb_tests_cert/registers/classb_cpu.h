/**
 * \file
 *
 * \brief Settings and definitions for the CPU register test.
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

#ifndef CLASSB_CPU_H_
#define CLASSB_CPU_H_

#include "error_handler.h"

#ifndef ASSEMBLY
#  if defined(__ICCAVR__)
#    define ASSEMBLY(x) __asm(x)
#  elif defined(__GNUC__)
#    define ASSEMBLY __asm__ __volatile__
#  endif
#endif

/**
 * \name Internal string manipulation macros
 * @{
 */

#define STRINGIZE_AUX(X) #X
#define STRINGIZE(X) STRINGIZE_AUX(X)
#define CONCAT3(X,Y,Z) X##Y##Z
#define LABEL(X,Y,Z) CONCAT3(X,Y,Z)

/** @} */

/**
 * \defgroup classb_registers CPU Register test
 *
 * \brief This is the self-test module for the registers in the CPU.
 *
 * The CPU registers are tested for stuck bits and some coupling faults.
 * The test is executed by calling \ref classb_register_test(). The test
 * procedure consists of five steps per register:
 *  -# Write 0x55 to register
 *  -# Write 0xAA to register
 *  -# Verify content (0xAA) of register
 *  -# Write 0x55 to register
 *  -# Verify content (0x55) of register
 *
 * Non-destructive testing is done on the R-registers that are stated to
 * need preservation in the compiler documentation and on all IO registers.
 * Further, this test returns its result using a register. Therefore, the
 * register used to return the value, the stack pointer registers or those
 * used within the test (e.g. to save values) are critical in the sense
 * that the test cannot be executed correctly unless these registers are
 * working.
 *
 * The error handler for the test is \ref CLASSB_ERROR_HANDLER_REGISTERS().
 * It is possible to configure independently the behavior of the test with
 * respect to failure in critical and non-critical registers (see
 * \ref CLASSB_ERROR_CRIT and \ref CLASSB_ERROR_NON_CRIT).
 *
 * Given that GCC and IAR make use of the CPU registers in a different manner,
 * we have included a compiler-specific function. This reduces execution time
 * and simplifies the code.
 *
 * \note Interrupts must be disabled during this test.
 *
 * @{
 */

/**
 * \defgroup settings_cpu Settings related to error handling
 *
 * \brief Symbols to define behavior upon detected register faults.
 *
 * In principle, any error in critical registers would mean that the system
 * cannot verify whether the registers are working correctly. Since registers
 * are vital to most applications, the microcontroller should probably hang.
 *
 * However, this behavior can be modified so that failure in critical registers
 * is handled by \ref CLASSB_ERROR_HANDLER_REGISTERS(). The behavior for failure
 * in non-critical registers can also be configured: the microcontroller can
 * call the error handler or hang.
 *
 * @{
 */

/**
 * \brief Error handling in critical registers.
 *
 * If this constant is defined as \c CLASSB_LABEL1, critical register faults
 * will hang the device. However, if this was defined as \c CLASSB_LABEL2,
 * errors in critical registers would be processed by the error handler.
 */
#define CLASSB_ERROR_CRIT CLASSB_LABEL1

/**
 * \brief Error handling in non-critical registers.
 *
 * If this constant is defined as \c CLASSB_LABEL2, errors in non-critical
 * registers will be processed by the error handler. However, it was defined as
 * \c CLASSB_LABEL1, non-critical register faults would hang the device.
 */
#define CLASSB_ERROR_NON_CRIT CLASSB_LABEL2

/** @} */

/**
 * \defgroup reg_check Symbols for presence of special registers.
 *
 * \brief Internal symbols that indicate presence of RAMPx and EIND registers.
 *
 * RAMPx and EIND registers are present if the memory sizes are large enough.
 * The register constants are defined in the header file even if the registers
 * are not present in the device. Therefore, it is necessary to check it
 * indirectly via the memory size.
 *
 * @{
 */

#ifdef __DOXYGEN__
/** Automatically defined for devices with data memory larger than 64KB. */
#  define CLASSB_HAS_BIGMEM
/** Automatically defined for devices with program memory larger than 64KB. */
#  define CLASSB_HAS_BIGFLASH
#else
#  if (DATAMEM_SIZE > 65536)
#    define CLASSB_HAS_BIGMEM
#  endif
#  if (PROGMEM_SIZE > 65536)
#    define CLASSB_HAS_BIGFLASH
#  endif
#endif

/** @} */

/**
 * \defgroup reg_macros Macros for reading, writing and testing registers
 *
 * \brief In order to simplify the code, the assembly code is interfaced through
 * macros.
 *
 * @{
 */

/**
 * \brief Save R register content to R31.
 *
 * \param reg R-register to store in R31.
 */
#define CLASSB_RegStore_R(reg) \
	ASSEMBLY( \
    "mov r31, " #reg " \n" \
	)

/**
 * \brief Load R register content from R31.
 *
 * \param reg R-register to restore from R31.
 */
#define CLASSB_RegRestore_R(reg) \
	ASSEMBLY( \
    "mov " #reg ", r31 \n" \
	)

/**
 * \brief Set high R-register to specified value.
 *
 * \param reg R-register to set (R16-R31).
 * \param value Value to set R-register to.
 */
#define CLASSB_RegSet_R_HI(reg,value) \
    ASSEMBLY( \
    "ldi " #reg ", " #value " \n" \
    )

/**
 * \brief Set low R-register to specified value.
 *
 * \param reg R-register to set (R0-R15).
 * \param value Value to set R-register to.
 *
 * \note The value is copied into the specified register via R30.
 */
#define CLASSB_RegSet_R_LO(reg,value) \
    ASSEMBLY( \
    "ldi r30, " #value " \n" \
    "mov " #reg ", r30 \n" \
    )

/**
 * \brief Set IO-register to specified value.
 *
 * \param reg IO-address of register to set.
 * \param value Value to set register to.
 *
 * \note The value is copied into the specified register via R30.
 */
#define CLASSB_RegSet_IO(reg,value) \
    ASSEMBLY( \
    "ldi r30, " #value " \n" \
    "out " #reg ", r30 \n" \
    )

/**
 * \brief Clear R register.
 *
 * \param reg R-register to clear.
 *
 * \note This macro works for any R-register (R0-R31).
 */
#define CLASSB_RegClear_R(reg) \
	ASSEMBLY( \
    "clr " #reg " \n" \
	)

/**
 * \brief Clear IO-register.
 *
 * \param reg IO-address of register to clear.
 *
 * \note The IO-register is cleared via R30.
 */
#define CLASSB_RegClear_IO(reg) \
	ASSEMBLY( \
    "clr r30 \n" \
    "out " #reg ", r30 \n" \
	)

/**
 * \brief Store IO-register content.
 *
 * \param reg IO-address of register to store.
 *
 * \note The value of the IO-register is stored in R31.
 */
#define CLASSB_RegStore_IO(reg) \
	ASSEMBLY( \
    "in r31, " #reg " \n" \
	)

/**
 * \brief Restore IO-register content.
 *
 * \param reg IO-address of register to restore.
 *
 * \note The value of the IO-register is restored from R31.
 */
#define CLASSB_RegRestore_IO(reg) \
	ASSEMBLY( \
    "out " #reg ", r31 \n" \
	)

/**
 * \brief Test R16-31 with specified value, jump to specified label on fault
 *
 * \param reg Register to test (R16..R31).
 * \param value Value to test register with.
 * \param label Label to jump to if test fails.
 */
#define CLASSB_RegTest_R_HI(reg,value,label) \
	ASSEMBLY( \
    "ldi " #reg ", " #value " \n" \
    "cpi " #reg ", " #value " \n" \
	"breq A_" STRINGIZE(LABEL(reg,value,__LINE__)) " \n" \
    "jmp " STRINGIZE(label) " \n" \
    "A_" STRINGIZE(LABEL(reg,value,__LINE__))  ": \n" \
	)

/**
 * \brief Test R0-15 with specified value, jump to specified label on fault
 *
 * \note R30 is used in the copying values to the register.
 *
 * \param reg Register to test (R0..R15).
 * \param value Value to test register with.
 * \param label Label to jump to if test fails.
 */
#define CLASSB_RegTest_R_LO(reg,value,label) \
	ASSEMBLY( \
    "ldi r30, " #value " \n" \
    "mov " #reg ", r30 \n" \
    "mov r30, " #reg " \n" \
    "cpi r30, " #value " \n" \
    "breq A_" STRINGIZE(LABEL(reg,value,__LINE__)) " \n" \
    "jmp " STRINGIZE(label) " \n" \
    "A_" STRINGIZE(LABEL(reg,value,__LINE__)) ": \n" \
	)

/**
 * \brief Test IO register with specified value, jump to specified label on
 * fault
 *
 * \note R30 is used in the copying of values to the register.
 *
 * \param reg Address of IO register to test.
 * \param value Value to test register with.
 * \param label Label to jump to if test fails.
 */
#define CLASSB_RegTest_IO(reg,value,label) \
	ASSEMBLY( \
    "ldi r30, " #value " \n" \
    "out " #reg ", r30 \n" \
    "in r30, " #reg " \n" \
    "cpi r30, " #value " \n" \
    "breq A_" STRINGIZE(LABEL(reg,value,__LINE__)) " \n" \
    "jmp " STRINGIZE(label) " \n" \
    "A_" STRINGIZE(LABEL(reg,value,__LINE__)) ": \n" \
	)

/** @} */

/**
 * \defgroup reg_func Functions
 *
 * \brief This is the self-test for CPU registers.
 *
 * @{
 */

bool classb_register_test(void);

/** @} */

/** @} */

#endif /* CLASSB_CPU_H_ */
