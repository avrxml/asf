/**
 * \file
 *
 * \brief SRAM test based on the March X algorithm.
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
#include "classb_sram.h"
#include "error_handler.h"

/**
 * \ingroup classb_sram
 * @{
 */

#if defined(__DOXYGEN__)
/**
 * \brief Buffer for temporary storage of SRAM sections during test.
 *
 * This buffer is placed in the beginning of SRAM, and its size in Bytes should
 * be (\c INTERNAL_SRAM_SIZE / \c CLASSB_NSECS) + \c CLASSB_OVERLAP_SIZE.
 *
 * As an example, let us suppose the device has \c INTERNAL_SRAM_SIZE = 4096
 * Bytes of SRAM, \ref CLASSB_NSECS = 8 and \ref CLASSB_OVERLAP = 25.
 * Since the number of sections is a divisor of \c INTERNAL_SRAM_SIZE, there
 * would be exactly 8 sections, each with a size of 512 Bytes. With 25% overlap,
 * we need to extend the first section (the buffer) by 128 Bytes, and the second
 * section is shortened accordingly. Therefore, we would have to reserve
 * 512 + 128 = 640 Bytes for the buffer.
 *
 * Refer to the compiler's device header file for \c INTERNAL_SRAM_START and
 * \c INTERNAL_SRAM_SIZE.
 *
 * The implementation depends on the compiler:\n
 * For IAR it is easy because variables can be placed in a specific address
 * using the location pragma directive.\n
 * For GCC the following steps should be followed:
 * - First, it is necessary to define in <em>AVR/GNU C linker > Memory settings
 * </em> a section that starts at \c INTERNAL_SRAM_START, for example:\n
 * \code .classb_sram_buffer=0x2000 \endcode
 * - Second, the buffer variable has to be placed in that memory section. This
 * can be done by appending this to the definition of the variable:\n
 * \code __attribute__ ((section(".classb_sram_buffer"))) \endcode
 * - Third, the <tt>.data</tt> section has to be moved so that there is no
 * overlap, i.e., the start should be \c INTERNAL_SRAM_START + the buffer size.
 * Continuing on the earlier example, we get a buffer size of 640 = 0x280, so
 * we would need to set:\n
 * \code .data=0x2280 \endcode
 */
uint8_t classb_buffer[CLASSB_SEC_SIZE + CLASSB_OVERLAP_SIZE];

#elif defined(__ICCAVR__)
# pragma location=INTERNAL_SRAM_START
__no_init uint8_t classb_buffer[CLASSB_SEC_SIZE + CLASSB_OVERLAP_SIZE];

#elif defined(__GNUC__)
static uint8_t classb_buffer[CLASSB_SEC_SIZE + CLASSB_OVERLAP_SIZE]
		__attribute__ ((section (".classb_sram_buffer")));
#endif


/**
 * \brief Execute March X test on one memory section at a time.
 *
 * This function keeps track of which section to test, and tests them in order.
 *
 * The test behaves as follows for a general section:
 * - The size of the section is \c CLASSB_SEC_SIZE.
 * - In order to have overlap between sections, the total number of bytes to be
 * tested must be \c CLASSB_SEC_SIZE + \c CLASSB_OVERLAP_SIZE.
 * - The algorithm is set to start \c CLASSB_OVERLAP_SIZE bytes before the start
 * of the current section, thus overlapping \c CLASSB_OVERLAP_SIZE bytes with
 * the previous section.
 *
 * Special cases of memory sections are:
 * - The first section to test is the buffer segment, which starts at
 * \c INTERNAL_SRAM_START and is of size \c CLASSB_SEC_SIZE +
 * \c CLASSB_OVERLAP_SIZE. In this case there is no previous segment, so no
 * overlap is necessary. The total number of bytes to test is then the size of
 * the buffer.
 * - The second section to test is the first "data" memory section, i.e., the
 * one after the buffer). The size of this section is \c CLASSB_SEC_SIZE -
 * \c CLASSB_OVERLAP_SIZE: the size of this section was reduced by
 * \c CLASSB_OVERLAP_SIZE bytes in order to expand the buffer by the same number
 * of bytes. The test starts \c CLASSB_OVERLAP_SIZE bytes before the beginning
 * of the section, and * the total number of bytes tested is then
 * \c CLASSB_SEC_SIZE.
 * - The "remainder" segment, if any. This section has a size of
 * \c CLASSB_SEC_REM, so the total number of bytes to test will be
 * \c CLASSB_SEC_REM + \c CLASSB_OVERLAP_SIZE.
 */
void classb_sram_test(void)
{
	/* This variable keeps track of the section to test. */
	static uint8_t current_section = 0;

	switch (current_section) {
	case 0:
		/* Test the buffer, which starts at INTERNAL_SRAM_START and ends
		 * at  CLASSB_SEC_SIZE + CLASSB_OVERLAP_SIZE.
		 * There is no overlap with previous segments.
		 */
		classb_march_x((uint8_t *)INTERNAL_SRAM_START, classb_buffer,
				CLASSB_SEC_SIZE + CLASSB_OVERLAP_SIZE);
		break;

	case 1:
		/* Test the first section, which size shrunk from below by the
		 * buffer when there is overlap. In order to overlap with the
		 * buffer, we simply start at INTERNAL_SRAM_START +
		 * CLASSB_SEC_SIZE.
		 */
		classb_march_x((uint8_t *)INTERNAL_SRAM_START + CLASSB_SEC_SIZE,
				classb_buffer, CLASSB_SEC_SIZE);
		break;

	case CLASSB_NSECS:
		/* Test the last section of size SRAM_SIZE % CLASSB_NSECS. Size
		 * is given by the amount of memory remaining when dividing
		 * SRAM_SIZE with CLASSB_NSECS.
		 */
		classb_march_x((uint8_t *)INTERNAL_SRAM_START
				+ (CLASSB_NSECS * CLASSB_SEC_SIZE)
				- CLASSB_OVERLAP_SIZE, classb_buffer,
				CLASSB_SEC_REM + CLASSB_OVERLAP_SIZE);
		break;

	default:
		/* Sections in the middle. Start CLASSB_OVERLAP_SIZE bytes
		 * before the segment and test CLASSB_SEC_SIZE
		 * + CLASSB_OVERLAP_SIZE bytes
		 */
		classb_march_x((uint8_t *)INTERNAL_SRAM_START
				+ (current_section * CLASSB_SEC_SIZE)
				- CLASSB_OVERLAP_SIZE, classb_buffer,
				CLASSB_SEC_SIZE + CLASSB_OVERLAP_SIZE);
		break;
	}

	/* Increase section count for next iteration, or reset if all memory is
	 * tested.
	 */
	current_section++;
	if (current_section > (CLASSB_NSEC_TOTAL - 1)) {
		current_section = 0;
	}
}


/**
 * \brief Run March X algorithm on specified memory range.
 *
 * The following steps are followed:
 * -# The content of the area to be tested is copied to the buffer. This does
 * not apply when the buffer is tested.
 * -# The March X algorithm is applied to the section to test for coupling
 * faults.
 * -# Memory content is copied back from the buffer. This does not apply when
 * the buffer is tested.
 *
 * This function requires that all variables are placed in registers by the
 * compiler. This is achieved by using the \c register specifier (probably not
 * necessary if the right level of optimization is chosen).
 *
 * If there should be an error in SRAM a local variable (implemented in a
 * register) would be flagged. This would lead to the error handler \ref
 * CLASSB_ERROR_HANDLER_SRAM() being called.
 *
 * \param p_sram Pointer to first byte in memory area to be tested
 * \param p_buffer Pointer to first byte in the buffer
 * \param size Size of area to be tested in bytes.
 */
void classb_march_x(register volatile uint8_t *p_sram,
		register volatile uint8_t *p_buffer, register uint16_t size)
{
	register uint16_t i = 0;
	register uint8_t error = 0;

	/* Save content of the section: copy to buffer unless testing the buffer
	 */
	if (p_buffer != p_sram) {
		for (uint16_t i = 0; i < size; i++) {
			*(p_buffer + i) = *(p_sram + i);
		}
	}

	/* Test phase 1: write 0 to all bit locations. */
	for (i = 0; i < size; i++) {
		*(p_sram + i) = 0x00;
	}

	/* Test phase 2: read 0, write FF. */
	for (i = 0; i < size; i++) {
		if (*(p_sram + i) != 0x00) {
			error = 1;
		} else {
			*(p_sram + i) = 0xFF;
		}
	}

	/* Test phase 3: read FF, write 0 (reverse order). */
	for (i = size ; i > 0; i--) {
		if (*(p_sram + i - 1) != 0xFF) {
			error = 1;
		} else {
			*(p_sram + i - 1) = 0x00;
		}
	}

	/* Test phase 4: read 0. */
	for (i = 0; i < size; i++) {
		if (*(p_sram + i) != 0x00) {
			error = 1;
		}
	}

#ifdef CLASSB_SRAM_INTRAWORD_TEST
	/* Intra-word march test. */
	for (i = 0; i < size; i++) {
		*(p_sram + i) = 0x55;
		if (*(p_sram + i) != 0x55) {
			error = 1;
		}

		*(p_sram + i) = 0xAA;
		if (*(p_sram + i) != 0xAA) {
			error = 1;
		}

		*(p_sram + i) = 0x33;
		if (*(p_sram + i) != 0x33) {
			error = 1;
		}

		*(p_sram + i) = 0xCC;
		if (*(p_sram + i) != 0xCC) {
			error = 1;
		}

		*(p_sram + i) = 0xF0;
		if (*(p_sram + i) != 0xF0) {
			error = 1;
		}

		*(p_sram + i) = 0x0F;
		if (*(p_sram + i) != 0x0F) {
			error = 1;
		}
	}
#endif

	/* Restore content of the section: copy from buffer, unless buffer is
	 * tested.
	 */
	if (p_buffer != p_sram)
		for (i = 0; i < size; i++)
			*(p_sram + i) = *(p_buffer + i);

	/* Call the error handler if there was an error. */
	if (error) {
		CLASSB_ERROR_HANDLER_SRAM();
	}
}

/** @} */
