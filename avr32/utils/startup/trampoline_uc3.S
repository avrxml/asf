/*****************************************************************************
 *
 * \file
 *
 * \brief AVR32 UC3 ISP trampoline.
 *
 * In order to be able to program a project with both BatchISP and JTAGICE mkII
 * without having to take the general-purpose fuses into consideration, add this
 * file to the project and change the program entry point to _trampoline.
 *
 * The pre-programmed ISP will be erased if JTAGICE mkII is used.
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
 * *
 * \asf_license_stop
 *
 ******************************************************************************/


#include "trampoline_uc3.h"


//! @{
//! \verbatim


  // This must be linked @ 0x80000000 if it is to be run upon reset.
  .section  .reset, "ax", @progbits


  .global _trampoline
  .type _trampoline, @function
_trampoline:
  // Jump to program start.
  rjmp    program_start

  .org  PROGRAM_START_OFFSET
program_start:
  // Jump to the C runtime startup routine.
  lda.w   pc, _stext


//! \endverbatim
//! @}
