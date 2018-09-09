/*****************************************************************************
 *
 * \file
 *
 * \brief NEWLIB_ADDONS exceptions include file for AVR32.
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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


#ifndef __AVR32_NEWLIB_ADDONS_EXCEPTIONS_H__
#define __AVR32_NEWLIB_ADDONS_EXCEPTIONS_H__

#include <_ansi.h>

_BEGIN_STD_C

/*
 Exception vector offsets
*/
#define EVBA_UNRECOVERABLE    0x000
#define EVBA_TLB_MULTIPLE     0x004
#define EVBA_BUS_ERROR_DATA   0x008
#define EVBA_BUS_ERROR_INSTR  0x00C
#define EVBA_NMI              0x010
#define EVBA_INSTR_ADDR       0x014
#define EVBA_ITLB_MISS        0x050
#define EVBA_ITLB_PROT        0x018
#define EVBA_BREAKPOINT       0x01C
#define EVBA_ILLEGAL_OPCODE   0x020
#define EVBA_UNIMPLEMENTED    0x024
#define EVBA_PRIVILEGE_VIOL   0x028
#define EVBA_FLOATING_POINT   0x02C
#define EVBA_COP_ABSENT       0x030
#define EVBA_SCALL            0x100
#define EVBA_DATA_ADDR_R      0x034
#define EVBA_DATA_ADDR_W      0x038
#define EVBA_DTLB_MISS_R      0x060
#define EVBA_DTLB_MISS_W      0x070
#define EVBA_DTLB_PROT_R      0x03C
#define EVBA_DTLB_PROT_W      0x040
#define EVBA_DTLB_MODIFIED    0x044


/*
   Define the form of the function used when registering exceptions.
   The function should return the address which the exception should
   return to after the exception processing.
*/

typedef unsigned int (*__exception_handler)(int /*evba_offset*/, int /*return address*/);

/*
   Define the form of the function used when registering a scall handler.
*/

typedef void (*__scall_handler)(int /*code*/, int /*p1*/, int /*p2*/
                                         , int /*p3*/, int /*p4*/);

/*
   Function for registering an exception handler for the exception with
   offset given by evba_offset.
*/
void _register_exception_handler(__exception_handler handler, int evba_offset);

/*
   Function for registering a scall handler which can be a arbirary
   function which uses r8-r12 for parameters.
*/
void _register_scall_handler(__scall_handler handler);

/*
   Initialize exceptions. Must be called before registering exception handlers
   and needed to enable exceptions. 'evba' is the pointer to the exception
   vector. 'handler_table' is a pointer to an array where the pointers to
   the exception handlers are stored. This array must be at least 0x104 bytes
   and word aligned.
*/
void init_exceptions(void *evba, void *handler_table);

_END_STD_C

#endif
