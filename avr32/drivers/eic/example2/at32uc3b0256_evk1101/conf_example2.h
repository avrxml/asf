/**
 * \file
 *
 * \brief AVR UC3 External Interrupt Controller(EIC) driver
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef _CONF_BOARD_H_
#define _CONF_BOARD_H_


/*! \name External Interrupt pin Mappings
 */
//! @{

/*
 * With the datasheet, in section "Peripheral Multiplexing on I/O lines" in the
 * table "GPIO Controller Function Multiplexing", for UC3B part, linked to
 * EVK1101 board, we see that PB2 and PB3 are multiplexed with EXTINT lines 6
 * and 7.
 * Note: PB2, PB3 are available with buttons PB0 and PB1.
 */

#define EXT_INT_EXAMPLE_PIN_LINE1               AVR32_EIC_EXTINT_6_PIN
#define EXT_INT_EXAMPLE_FUNCTION_LINE1          AVR32_EIC_EXTINT_6_FUNCTION
#define EXT_INT_EXAMPLE_LINE1                   EXT_INT6
#define EXT_INT_EXAMPLE_IRQ_LINE1               AVR32_EIC_IRQ_6
#define EXT_INT_EXAMPLE_PIN_LINE2               AVR32_EIC_EXTINT_7_PIN
#define EXT_INT_EXAMPLE_FUNCTION_LINE2          AVR32_EIC_EXTINT_7_FUNCTION
#define EXT_INT_EXAMPLE_LINE2                   EXT_INT7
#define EXT_INT_EXAMPLE_IRQ_LINE2               AVR32_EIC_IRQ_7
#define EXT_INT_EXAMPLE_NB_LINES                2

//! @}

#endif // _CONF_BOARD_H_

