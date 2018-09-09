/*****************************************************************************
 *
 * \file
 *
 * \brief AVR32UC C runtime startup file.
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
 ******************************************************************************/


#include <avr32/io.h>


//! @{
//! \verbatim


  // This must be linked @ 0x80000000 if it is to be run upon reset.
  .section  .reset, "ax", @progbits


  .global _start
  .type _start, @function
_start:
  // Jump to the C runtime startup routine.
  lda.w   pc, _stext


  // _stext is placed outside the .reset section so that the program entry point
  // can be changed without affecting the C runtime startup.
  .section  .text._stext, "ax", @progbits


  .global _stext
  .type _stext, @function
_stext:
  // Set initial stack pointer.
  lda.w   sp, _estack

  // Set up EVBA so interrupts can be enabled.
  lda.w   r0, _evba
  mtsr    AVR32_EVBA, r0

  // Enable the exception processing.
  csrf    AVR32_SR_EM_OFFSET

  // Load initialized data having a global lifetime from the data LMA.
  lda.w   r0, _data
  lda.w   r1, _edata
  cp      r0, r1
  brhs    idata_load_loop_end
  lda.w   r2, _data_lma
idata_load_loop:
  ld.d    r4, r2++
  st.d    r0++, r4
  cp      r0, r1
  brlo    idata_load_loop
idata_load_loop_end:

  // Clear uninitialized data having a global lifetime in the blank static storage section.
  lda.w   r0, __bss_start
  lda.w   r1, _end
  cp      r0, r1
  brhs    udata_clear_loop_end
  mov     r2, 0
  mov     r3, 0
udata_clear_loop:
  st.d    r0++, r2
  cp      r0, r1
  brlo    udata_clear_loop
udata_clear_loop_end:

#ifdef CONFIG_FRAME_POINTER
  // Safety: Set the default "return" @ to the exit routine address.
  lda.w   lr, exit
#endif

  // Start the show.
  lda.w   pc, main


//! \endverbatim
//! @}
