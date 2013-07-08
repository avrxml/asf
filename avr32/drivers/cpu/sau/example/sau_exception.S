/**
 * \file
 *
 * \brief Exception and interrupt vectors
 *
 * Copyright (c) 2012 Atmel Corporation. All rights reserved.
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

#include <avr32/io.h>

	.section  .exception, "ax", @progbits


	/* 
	 *Start of Exception Vector Table.
	 * EVBA must be aligned with a power of two strictly greater than the 
	 * EVBA-relative offset of the last vector.
	 */
	.balign 0x200

	.global _evba
	.type _evba, @function
_evba:
	.org  0x000
	// For all exceptions except the SCALL exception (offset 100h), go to exm
	.rept   0x100 / 4
	pushm   r0-r12, lr          /* Save registers on the stack. */
	rjmp    exm
	.endr

	// Specific for supervisor call - mapped in (_EVBA+100h) 
	pushm   r0-r12, lr          // Save registers on the stack. 
	/*
	 * Set the parameters given to the handle_exception() function. 
	 * R10 <- Exception cause == (EVBA offset of the exception)>>2
	 * R11 <- Address of the offending instruction
	 * R12 <- Ptr to sp before the pushm we just did
	 * Handle the exception now
	 * Restore registers.
	 * Return from SCALL.
	 */
	mov     r10, 0x100 / 4      
	lddsp   r11, sp[14*4+4]     
	sub     r12, sp, -14*4      
	call    handle_exception    
	popm    r0-r12, lr         
	rets                        
	
	exm:                     // All other exceptions 
	mfsr    r10, AVR32_ECR   // R10 <- AVR32_ECR (exception cause register) 
	lddsp   r11, sp[14*4+4]  // R11 <- Address of the offending instruction 
	sub     r12, sp, -14*4   // R12 <- Ptr to sp before the pushm we did 
	call    handle_exception    // Handle the exception now 
	popm    r0-r12, lr          // Restore registers. 
	rete                        // Return from exception. 


/*
 * Interrupt support.
 * The interrupt controller must provide the offset address relative to EVBA.
 * Important note:
 * All interrupts call a C function named _get_interrupt_handler.
 * This function will read group and interrupt line number to then return in
 *R12 a pointer to a user-provided interrupt handler.
 */

.balign 4

.irp    priority, 0, 1, 2, 3
.global _int\priority
.type   _int\priority, @function
_int\priority:
#if __AVR32_UC__
	/*
	 * R8-R12, LR, PC and SR are automatically pushed onto the system stack
	 * by the CPU upon interrupt entry. No other register is saved by 
	 * hardware.
	 */
#elif __AVR32_AP__
	/*
	 * PC and SR are automatically saved in respectively RAR_INTx and 
	 * RSR_INTx by the CPU upon interrupt entry. No other register is saved
	 * by hardware.
	 */
	pushm   r8-r12, lr
#endif
	// Pass the int_level parameter to the _get_interrupt_handler function.
	mov     r12, \priority
	call    _get_interrupt_handler
	// Get the pointer to the interrupt handler returned by the function.
	cp.w    r12, 0
#if __AVR32_UC__
	/* 
	 * If this was not a spurious interrupt (R12 != NULL), jump to the 
	 * handler.
	 */
	movne   pc, r12
#elif __AVR32_AP__
	// If this was a spurious interrupt (R12 == NULL), branch.
	breq    spint\priority
	/* 
	 * Push the pointer to the interrupt handler onto the system stack since
	 * no register may be altered.
	 */
	st.w    --sp, r12
	popm    r8-r12, lr, pc  // Restore registers and jump to the handler.
spint\priority:
	popm    r8-r12, lr
#endif
	/*
	 * If this was a spurious interrupt (R12 == NULL), return from event 
	 * handler.
	 */
	rete
.endr

//! \endverbatim
//! @}
