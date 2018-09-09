/*****************************************************************************
 *
 * \file
 *
 * \brief NEWLIB_ADDONS interrupts include file for AVR32.
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


#ifndef __AVR32_NEWLIB_ADDONS_INTERRUPTS_H__
#define __AVR32_NEWLIB_ADDONS_INTERRUPTS_H__

#include <_ansi.h>

_BEGIN_STD_C

#define INT_GRPS          64
#define INT_LINES         32
#define INTPR_BASE        (__intc_base__ + 0x0000)
#define INTREQ_BASE       (__intc_base__ + 64*4)
#define INTCAUSE_BASE     (__intc_base__ + 2*64*4)

//Register offsets
#define INTLEVEL          30
#define AUTOVECTOR        0
#define AUTOVECTOR_BITS   14

//Priorities
#define INT0              0
#define INT1              1
#define INT2              2
#define INT3              3


typedef void (*__newlib_int_handler)(int /* int_grp*/, void */*user_handle*/);

__newlib_int_handler register_interrupt(__newlib_int_handler handler, int int_grp, int line, int priority,
                                 .../* void *user_handle*/);
void init_interrupts();
void set_interrupts_base(void *base);

_END_STD_C

#endif
