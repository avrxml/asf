/**
 * \file
 *
 * \brief Example configuration header file.
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

/**
 * \defgroup twim_pdca_example_pin_defs
 *   - <b> Board  --  EEPROM </b>
 *   - TWIMS1 TWD(J4 PIN1)  --  SDA
 *   - TWIMS1 TWCK(J4 PIN2)  --  SCL
 *   - VCC  --  VCC
 *   - GND  --  GND
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef CONF_EXAMPLE_H_INCLUDED
#define CONF_EXAMPLE_H_INCLUDED

//! TWIM Interrupt Number
#define EXAMPLE_TWIM_IRQn    TWIM1_IRQn
//! TWIM Module Used
#define EXAMPLE_TWIM         TWIM1
//! Target's TWI address
#define TARGET_ADDRESS       0x50
//! Internal Address length
#define TARGET_ADDR_LGT      2
//! Internal Address
#define VIRTUALMEM_ADDR      0x50
//! Speed of TWI
#define TWIM_MASTER_SPEED    TWI_STD_MODE_SPEED
//! TWIM Interrupt Handler
#define EXAMPLE_TWIM_Handler TWIM1_Handler

#endif /* CONF_EXAMPLE_H_INCLUDED */
