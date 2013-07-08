/*****************************************************************************
 *
 * \file
 *
 * \brief SAU driver.
 *
 * SAU (Security Access Unit) driver module for AVR32 devices.
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
 *****************************************************************************/

#ifndef _SAU_H_
#define _SAU_H_

#include <avr32/io.h>
#include "compiler.h"

/*!
 * \defgroup group_avr32_driver_cpu_sau SAU Driver
 * \{
 */

//! \todo Remove workaround for bug in header files.
#if defined (__ICCAVR32__)
#define AVR32_SAU_HSB_ADDRESS     0x90000000
#endif

//! A type definition of channels and HSB memories to be mapped.
typedef struct
{
	uint32_t HSB_mem;                          //!< Module HSB memory.
	unsigned char channel_num;         	//!< Module channel number.
} sau_map_t[];

/*!
 * \brief Enable/Disable the SAU module
 *
 */
#define  sau_enable()           (Set_bits(AVR32_SAU.cr, AVR32_SAU_CR_EN_MASK))
#define  sau_disable()          (Set_bits(AVR32_SAU.cr, AVR32_SAU_CR_DIS_MASK))

/*!
 * \brief SAU setup mode management 
 *
 */
#define  sau_enter_setup()      (Set_bits(AVR32_SAU.cr, AVR32_SAU_CR_SEN_MASK))
#define  sau_exit_setup()       (Set_bits(AVR32_SAU.cr, AVR32_SAU_CR_SDIS_MASK))

/*!
 * \brief SAU open mode management
 *
 */
#define  sau_enable_open_mode()         (Set_bits(AVR32_SAU.config,      \
						AVR32_SAU_CONFIG_OPEN_MASK))
#define  sau_disable_open_mode()        (Clr_bits(AVR32_SAU.config,      \
						AVR32_SAU_CONFIG_OPEN_MASK))

/*!
 * \brief SAU Bus Error Exception response management
 *
 */
#define  sau_enable_buserr_resp()       (Set_bits(AVR32_SAU.cr,         \
						AVR32_SAU_CR_BERREN_MASK))
#define  sau_disable_buserr_resp()      (Set_bits(AVR32_SAU.cr,	        \
						AVR32_SAU_CR_BERRDIS_MASK))
/*!
 * \brief Enable the SAU interrupt
 *
 * \param int_source: The events that will generate interrupt request
 */
#define sau_enable_interrupt(int_source) (AVR32_SAU.ier = int_source)

/*!
 * \brief Disable the SAU interrupt
 *
 * \param interrupt_source: The events that will not generate interrupt request
 */
#define sau_disable_interrupt(int_source) (AVR32_SAU.idr = int_source)

/*!
 * \brief Clear the error bits flags in SR
 *
 * \param error_flag: The error bits to be cleared
 */
#define sau_clear_error_flags(error_flag) (AVR32_SAU.icr = error_flag)

/*!
 * \brief Check the error bits flags in SR
 *
 */
#define sau_status_check()       (AVR32_SAU.sr & (0xff ^ AVR32_SAU_SR_CAS_MASK)) 

extern void sau_enable_channel(unsigned char channel_num);
extern void sau_disable_channel(unsigned char channel_num);
extern bool sau_configure_channels(const sau_map_t saumap, uint32_t size);
extern void sau_lock_channels(U8 unlock_HSB_cycles, U8 unlock_key);
extern bool sau_unlock_channel(U8 Unlock_channel_number, U8 Unlock_key);
extern uint32_t sau_read_channel(unsigned char channel_num);
extern void sau_write_channel(uint32_t data, unsigned char channel_num);

/*!
 * \}
 */

#endif // _SAU_H_
