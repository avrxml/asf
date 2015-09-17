/*****************************************************************************
 *
 * \file
 *
 * \brief CAN Configuration File for AVR32 UC3.
 *
 * This file contains basic functions for the AVR32 CAN, with support for all
 * modes, settings and clock speeds.
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef _CONF_CAN_H
#define _CONF_CAN_H

//! Define the number of MOB per channel. It is a fixed value: 16.
#define NB_MOB_CHANNEL     16
//! Setup the usage of CAN Under interruption
#define CAN_LIB_UNDER_INTERRUPT

#define CAN_BAUDRATE_1MHZ_CANCLOCK_16MHz
#define CAN_BAUDRATE_1MHZ_CANCLOCK_16MHz_SJW         1
#define CAN_BAUDRATE_1MHZ_CANCLOCK_16MHz_PRES        1
#define CAN_BAUDRATE_1MHZ_CANCLOCK_16MHz_PRS         2
#define CAN_BAUDRATE_1MHZ_CANCLOCK_16MHz_PHS1        1
#define CAN_BAUDRATE_1MHZ_CANCLOCK_16MHz_PHS2        1

#define BAUDRATE        CAN_BAUDRATE_1MHZ_CANCLOCK_16MHz
#define BAUDRATE_HZ     1000000
#define BAUDRATE_SJW    CAN_BAUDRATE_1MHZ_CANCLOCK_16MHz_SJW
#define BAUDRATE_PRES   CAN_BAUDRATE_1MHZ_CANCLOCK_16MHz_PRES
#define BAUDRATE_PRS    CAN_BAUDRATE_1MHZ_CANCLOCK_16MHz_PRS
#define BAUDRATE_PHS1   CAN_BAUDRATE_1MHZ_CANCLOCK_16MHz_PHS1
#define BAUDRATE_PHS2   CAN_BAUDRATE_1MHZ_CANCLOCK_16MHz_PHS2

#define CAN0_INT_TX_LEVEL           AVR32_INTC_INT0
#define CAN0_INT_RX_LEVEL           AVR32_INTC_INT0
#define CAN0_INT_ERR_LEVEL          AVR32_INTC_INT0
#define CAN0_INT_BOFF_LEVEL         AVR32_INTC_INT0
#define CAN0_INT_WAKE_UP_LEVEL      AVR32_INTC_INT0

#define CAN1_INT_TX_LEVEL           AVR32_INTC_INT0
#define CAN1_INT_RX_LEVEL           AVR32_INTC_INT0
#define CAN1_INT_ERR_LEVEL          AVR32_INTC_INT0
#define CAN1_INT_BOFF_LEVEL         AVR32_INTC_INT0
#define CAN1_INT_WAKE_UP_LEVEL      AVR32_INTC_INT0

#endif // _CONF_CAN_H
