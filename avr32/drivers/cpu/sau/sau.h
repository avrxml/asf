/*****************************************************************************
 *
 * \file
 *
 * \brief SAU driver.
 *
 * SAU (Security Access Unit) driver module for AVR32 devices.
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
 *****************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

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
