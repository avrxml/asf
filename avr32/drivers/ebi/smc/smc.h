/*****************************************************************************
 *
 * \file
 *
 * \brief SMC on EBI driver for AVR32 UC3.
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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


#ifndef _SMC_H_
#define _SMC_H_

/**
 * \defgroup group_avr32_drivers_ebi_smc MEMORY - EBI Static Memory Controller
 *
 * EBI (External Bus Interface) SMC (Static Memory Controller) allows to connect a Static Memory to the microcontroller.
 *
 * \{
 */

#include <avr32/io.h>

#include "compiler.h"
#include "conf_ebi.h"

#ifndef SMC_COMPONENT_CS0
#ifndef SMC_COMPONENT_CS1
#ifndef SMC_COMPONENT_CS2
#ifndef SMC_COMPONENT_CS3
#ifndef SMC_COMPONENT_CS4
#ifndef SMC_COMPONENT_CS5

#warning SMC default configuration is missing.
//! SMC Peripheral Memory Size in log2(Bytes)
#define EXT_SM_SIZE            17        // (65536 * 2) = 2^17 bytes

//! SMC Data Bus Width
#define SMC_DBW                16

//! Whether 8-bit SM chips are connected on the SMC
#define SMC_8_BIT_CHIPS        false

// NWE setup length = (128* NWE_SETUP[5] + NWE_SETUP[4:0])
//! Unit: ns.
#define NWE_SETUP               0

// NCS setup length = (128* NCS_WR_SETUP[5] + NCS_WR_SETUP[4:0])
//! Unit: ns.
#define NCS_WR_SETUP            0

// NRD setup length = (128* NRD_SETUP[5] + NRD_SETUP[4:0])
//! Unit: ns.
#define NRD_SETUP               0

// NCS setup length = (128* NCS_RD_SETUP[5] + NCS_RD_SETUP[4:0])
//! Unit: ns.
#define NCS_RD_SETUP            0

// NCS pulse length = (256* NCS_WR_PULSE[6] + NCS_WR_PULSE[5:0])
//! Unit: ns.
#define NCS_WR_PULSE            82

// NWE pulse length = (256* NWE_PULSE[6] + NWE_PULSE[5:0])
//! Unit: ns.
#define NWE_PULSE               82

// NCS pulse length = (256* NCS_RD_PULSE[6] + NCS_RD_PULSE[5:0])
//! Unit: ns.
#define NCS_RD_PULSE            82

// NRD pulse length = (256* NRD_PULSE[6] + NRD_PULSE[5:0])
//! Unit: ns.
#define NRD_PULSE               82


// Write cycle length = (NWE_CYCLE[8:7]*256 + NWE_CYCLE[6:0])
//! Unit: ns.
#define NCS_WR_HOLD             82
#define NWE_HOLD                82
#define NWE_CYCLE               Max((NCS_WR_SETUP + NCS_WR_PULSE + NCS_WR_HOLD),(NWE_SETUP + NWE_PULSE + NWE_HOLD))

// Read cycle length = (NRD_CYCLE[8:7]*256 + NRD_CYCLE[6:0])
//! Unit: ns.
#define NCS_RD_HOLD             0
#define NRD_HOLD                0
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
#endif
#endif
#endif
#endif
#endif
#endif

/*! \brief Initializes the AVR32 SMC module and the connected SRAM(s).
 * \param hsb_hz HSB frequency in Hz (the HSB frequency is applied to the SMC).
 * \note Each access to the SMC address space validates the mode of the SMC
 *       and generates an operation corresponding to this mode.
 */
extern void smc_init(unsigned long hsb_hz);

/*! \brief Return the size of the peripheral connected  .
 *  \param cs The chip select value
 */
extern unsigned char smc_get_cs_size(unsigned char cs);

/**
 * \}
 */

#endif  // _SMC_H_
