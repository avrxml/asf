/*****************************************************************************
 *
 * \file
 *
 * \brief SDRAMC on EBI driver for AVR32 UC3.
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
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#ifndef _SDRAMC_H_
#define _SDRAMC_H_

/**
 * \defgroup group_avr32_drivers_ebi_sdramc MEMORY - EBI SDRAM Controller
 *
 * EBI (External Bus Interface) SDRAM Controller allows to connect a SDRAM to the microcontroller.
 *
 * \{
 */

#include <avr32/io.h>
#include "board.h"

#ifdef SDRAM_PART_HDR
  #include SDRAM_PART_HDR
#else
# warning Timing setups configuration to use in the driver is missing. Default configuration is used.
//! The number of bank bits for this SDRAM (1 or 2).
#define SDRAM_BANK_BITS                 2

//! The number of row bits for this SDRAM (11 to 13).
#define SDRAM_ROW_BITS                  13

//! The number of column bits for this SDRAM (8 to 11).
#define SDRAM_COL_BITS                  9

//! The minimal column address select (READ) latency for this SDRAM (1 to 3 SDRAM cycles).
//! Unit: tCK (SDRAM cycle period).
#define SDRAM_CAS                       2

//! The minimal write recovery time for this SDRAM (0 to 15 SDRAM cycles).
//! Unit: ns.
#define SDRAM_TWR                       14

//! The minimal row cycle time for this SDRAM (0 to 15 SDRAM cycles).
//! ACTIVE-to-ACTIVE command delay.
//! Unit: ns.
#define SDRAM_TRC                       60

//! The minimal row precharge time for this SDRAM (0 to 15 SDRAM cycles).
//! PRECHARGE command period.
//! Unit: ns.
#define SDRAM_TRP                       15

//! The minimal row to column delay time for this SDRAM (0 to 15 SDRAM cycles).
//! ACTIVE-to-READ/WRITE command delay.
//! Unit: ns.
#define SDRAM_TRCD                      15

//! The minimal row address select time for this SDRAM (0 to 15 SDRAM cycles).
//! ACTIVE-to-PRECHARGE command delay.
//! Unit: ns.
#define SDRAM_TRAS                      37

//! The minimal exit self refresh time for this SDRAM (0 to 15 SDRAM cycles).
//! Exit SELF REFRESH to ACTIVE command delay.
//! Unit: ns.
#define SDRAM_TXSR                      67

//! The maximal refresh time for this SDRAM (0 to 4095 SDRAM cycles).
//! Refresh period.
//! Unit: ns.
#define SDRAM_TR                        7812

//! The minimal refresh cycle time for this SDRAM.
//! AUTO REFRESH command period.
//! Unit: ns.
#define SDRAM_TRFC                      66

//! The minimal mode register delay time for this SDRAM.
//! LOAD MODE REGISTER command to ACTIVE or REFRESH command delay.
//! Unit: tCK (SDRAM cycle period).
#define SDRAM_TMRD                      2

//! The minimal stable-clock initialization delay for this SDRAM.
//! Unit: us.
#define SDRAM_STABLE_CLOCK_INIT_DELAY   100

//! The minimal number of AUTO REFRESH commands required during initialization for this SDRAM.
#define SDRAM_INIT_AUTO_REFRESH_COUNT   2

#endif

//! Pointer to SDRAM.
#if UC3C
#define SDRAM           ((void *)AVR32_EBI_CS1_0_ADDRESS)
#else
#define SDRAM           ((void *)AVR32_EBI_CS1_ADDRESS)
#endif

//! SDRAM size.
#define SDRAM_SIZE      (1 << (SDRAM_BANK_BITS + \
                               SDRAM_ROW_BITS  + \
                               SDRAM_COL_BITS  + \
                               (SDRAM_DBW >> 4)))


/*! \brief Initializes the AVR32 SDRAM Controller and the connected SDRAM(s).
 *
 * \param hsb_hz HSB frequency in Hz (the HSB frequency is applied to the SDRAMC
 *               and to the SDRAM).
 *
 * \note HMATRIX and SDRAMC registers are always read with a dummy load
 *       operation after having been written to, in order to force write-back
 *       before executing the following accesses, which depend on the values set
 *       in these registers.
 *
 * \note Each access to the SDRAM address space validates the mode of the SDRAMC
 *       and generates an operation corresponding to this mode.
 */
extern void sdramc_init(unsigned long hsb_hz);

/*! \brief Set the SDRAM in self refresh mode. The SELF REFRESH command can be used to retain
 * data in the SDRAM, even if the rest of the system is
 * powered down. When in the self refresh mode, the
 * SDRAM retains data without external clocking.
 *
 * \note Once the SELF REFRESH command is registered, all
 * the inputs to the SDRAM become "Don't Care" with
 * the exception of CKE, which must remain LOW.
 * Once self refresh mode is engaged, the SDRAM provides its own internal
 * clocking, causing it to perform its
 * own AUTO REFRESH cycles. The SDRAM must remain
 * in self refresh mode for a minimum period equal to
 * tRAS and may remain in self refresh mode for an indefinite
 * period beyond that.
 *
 * \note An example of entering/exiting CPU sleep mode while keeping SDRAM content is :
 * sdram_enter_self_refresh(); SLEEP(AVR32_PM_SMODE_STATIC);  sdram_exit_self_refresh();
 *
 */
void sdram_enter_self_refresh(void);

/*! \brief Exit from the SDRAM self refresh mode, inhibits self refresh mode
 */
void sdram_exit_self_refresh(void);

/**
 * \}
 */

#endif  // _SDRAMC_H_
