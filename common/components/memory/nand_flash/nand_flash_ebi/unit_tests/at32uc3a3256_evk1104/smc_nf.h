/*****************************************************************************
 *
 * \file
 *
 * \brief Configuration file for NF Driver.
 *
 * \note The values defined in this file are device-specific. See the device
 *       datasheet for further information.
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

#ifndef _SMC_NF_H_
#define _SMC_NF_H_


//! SMC Peripheral Memory Size in Bytes
#define EXT_SM_SIZE            0x200001

//! SMC Data Bus Width
#define SMC_DBW                8

//! Whether 8-bit SM chips are connected on the SMC
#define SMC_8_BIT_CHIPS        true



// NCS setup time. Unit: ns.
#define NCS_WR_SETUP            0

// NCS pulse time. Unit: ns.
#define NCS_WR_PULSE            100

// NCS hold time. Unit: ns.
#define NCS_WR_HOLD             0

// NWE setup time. Unit: ns.
#define NWE_SETUP               20

// NWE pulse time. Unit: ns.
#define NWE_PULSE               40

// NWE hold time. Unit: ns.
#define NWE_HOLD                40

// Write cycle time. Unit: ns.
#define NWE_CYCLE               Max((NCS_WR_SETUP + NCS_WR_PULSE + NCS_WR_HOLD),(NWE_SETUP + NWE_PULSE + NWE_HOLD))

// NCS setup time. Unit: ns.
#define NCS_RD_SETUP            0

// NCS pulse time. Unit: ns.
#define NCS_RD_PULSE            100

// NCS hold time. Unit: ns.
#define NCS_RD_HOLD             0

// NRD setup time. Unit: ns.
#define NRD_SETUP               20

// NRD pulse time. Unit: ns.
#define NRD_PULSE               40

// NRD hold time. Unit: ns.
#define NRD_HOLD                40

// Read cycle time. Unit: ns.
#define NRD_CYCLE               Max((NCS_RD_SETUP + NCS_RD_PULSE + NCS_RD_HOLD),(NRD_SETUP + NRD_PULSE + NRD_HOLD))



// Data float time
#define TDF_CYCLES              0
#define TDF_OPTIM               false

// Page mode
#define PAGE_MODE               false
#define PAGE_SIZE               0

//! Whether read is controlled by NCS or by NRD
#define NCS_CONTROLLED_READ     false

//! Whether write is controlled by NCS or by NWE
#define NCS_CONTROLLED_WRITE    false

//! Whether to use the NWAIT pin
#define NWAIT_MODE              AVR32_SMC_EXNW_MODE_DISABLED

#endif //_SMC_NF_H_
