/**
 * \file
 *
 * \brief SRAM test settings.
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

#ifndef CLASSB_SRAM_H
#define CLASSB_SRAM_H

/**
 * \defgroup classb_sram Internal SRAM Test
 *
 * \brief Self-diagnostic test for coupling faults in internal SRAM memory
 *
 * The test \ref classb_sram_test() divides the internal SRAM into
 * \ref CLASSB_NSECS sections that are tested in turns with a March algorithm
 * (see \ref marchx). The simplest behavior of the test is when there is no
 * overlap between memory sections. In this case all sections have the same
 * size, except possibly the last one (see \ref CLASSB_NSECS). The first memory
 * section (referred to as the buffer) is reserved: it is used by the test to
 * store the content of the other sections while they are being tested (for more
 * details see \ref classb_buffer).
 *
 * If there is overlap (see \ref CLASSB_OVERLAP), every time a memory section is
 * tested, a part of the previous section is tested as well. Note that this does
 * not apply to the buffer, since it is the first section. The size of the
 * buffer is then expanded with respect to the previous case. Further, the size
 * of the second section is decreased correspondingly.
 *
 * If there should be an error in internal SRAM the error handler
 * \ref CLASSB_ERROR_HANDLER_SRAM() would be called.
 *
 * \attention Interrupts must be disabled during this test.
 *
 * \section marchx March X
 *
 * The chosen algorithm is <em>March X</em>. This consists on the following
 * steps:
 * \f[
       \Updownarrow ( w_\mathbf{D} ) ;
       \Uparrow ( r_\mathbf{D} , w_{\bar{\mathbf{D}}} );
       \Downarrow ( r_{\bar{\mathbf{D}}} , w_\mathbf{D} );
       \Updownarrow ( r_\mathbf{D} ) \; ,
 * \f]
 * where \f$w\f$ denotes a write operation, \f$r\f$ denotes a read operation,
 * \f$\mathbf{D}\f$ is any data background, \f$\bar{\mathbf{D}}\f$ is the
 * complement of \f$\mathbf{D}\f$ and the arrows refer to the addressing order.
 * In our implementation we have chosen \f$\mathbf{D} = \textnormal{0x00}\f$.
 *
 * Under the restricted coupling faults (CFs) model, the interleaved
 * organization of the memory in XMEGA avoids any kind of intra-word CFs.
 * However, in order to detect some intra-word CFs that are not considered by
 * this fault model, we have included the following optional march element:
 * \f[
       \Updownarrow ( w_\mathbf{D_0} , r_\mathbf{D_0} ,
               \ldots , w_\mathbf{D_d} , r_\mathbf{D_d}) \; ,
 * \f]
 * where the background sequence is \f$ \textnormal{0x00, 0xFF, 0x55, 0xAA,
 * 0x33, 0xCC, 0x0F, 0xF0} \f$. This intra-word test is only executed if
 * \ref CLASSB_SRAM_INTRAWORD_TEST is defined. The elements that correspond to
 * the first two data backgrounds are redundant because the first part of the
 * test includes them. This optional test will detect all intra-word state CFs
 * considered by the unrestricted CF model.
 *
 * @{
 */

/**
 * \name Configuration settings
 * @{
 */

/**
 * \brief Number of sections to divide the SRAM into for testing.
 *
 * It is advisable that \c INTERNAL_SRAM_SIZE is divisible by the number of
 * sections and, therefore, recommended values are 2, 4, 8, 16, etc. Otherwise
 * an extra section will be added with the remainder of the division as size.
 *
 * Note that the higher the number of sections, the smaller the size of
 * \ref classb_buffer, i.e. the section of memory that is reserved for the test)
 * and the faster each partial test is completed.
 */
#define CLASSB_NSECS 16

/**
 * \brief Overlap between memory sections (in % of \c CLASSB_SEC_SIZE).
 *
 * \attention This should be an unsigned long, i.e. \<val\>UL.
 */
#define CLASSB_OVERLAP 25UL

#ifdef __DOXYGEN__
/**
 * \brief Configuration to enable intra-word test
 *
 * Defining this symbol will add intra-word testing after the inter-word test.
 *
 * Given the layout of the memory, the probability of intra-word coupling faults
 * is greatly diminished. However, for extra safety, the test can be expanded to
 * check some intra-word coupling faults.
 */
 #define CLASSB_SRAM_INTRAWORD_TEST
#else
/** #define CLASSB_SRAM_INTRAWORD_TEST */
#endif

/** @} */

/**
 * \name Constants for internal use
 *
 * \attention These should not be modified by user!
 * @{
 */

/** Size of each segment, in bytes */
#define CLASSB_SEC_SIZE (INTERNAL_SRAM_SIZE / CLASSB_NSECS)

/**
 * \brief Size of the last test segment, in bytes
 *
 * This indicates the remaining bytes to test in the case when
 * \c INTERNAL_SRAM_SIZE is not divisible by \ref CLASSB_NSECS.
 */
#define CLASSB_SEC_REM (INTERNAL_SRAM_SIZE % CLASSB_NSECS)

/**
 * \brief Size of overlap in bytes.
 *
 * When testing a memory section, the algorithm starts \c CLASSB_OVERLAP_SIZE
 * bytes before the start address of the section.
 */
#define CLASSB_OVERLAP_SIZE ((CLASSB_SEC_SIZE * CLASSB_OVERLAP) / 100)

/** Total number of segments including remainder, if present */
#if (CLASSB_SEC_REM == 0)
#  define CLASSB_NSEC_TOTAL CLASSB_NSECS
#else
#  define CLASSB_NSEC_TOTAL (CLASSB_NSECS + 1)
#endif

/** @} */

/**
 * \name Class B test
 *
 * @{
 */

void classb_sram_test(void);

/** @} */

void classb_march_x(register volatile uint8_t *p_sram,
		register volatile uint8_t *p_buffer, register uint16_t size);

/** @} */

#endif
