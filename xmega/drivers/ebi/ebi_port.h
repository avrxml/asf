/**
 * \file
 *
 * \brief External bus interface (EBI) port/pin configuration
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
#ifndef DRIVERS_EBI_EBI_PORT_H
#define DRIVERS_EBI_EBI_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "compiler.h"
#include "sysclk.h"

/**
 * \ingroup ebi_group
 * \defgroup ebi_port_group EBI port/pin configuration
 *
 * This is a driver for configuring the external bus interface port. Setting up
 * the GPIO lines depending on the mode the user has selected.
 *
 * \section dependencies Dependencies
 *
 * The EBI port depends on the following modules:
 *  - \ref sysclk_group for peripheral clock control
 *
 * @{
 */

//! \name EBI Address Latch Configuration Flags
//@{
#define EBI_ALE1                0x01    //!< Enable ALE1 only
#define EBI_ALE12               0x03    //!< Enable both ALE1 and ALE2
//@}

//! \name EBI Port Configuration Flags
//@{
//! 3-port interface (default is 2-port)
#define EBI_PORT_3PORT  (1 <<  0)
#define EBI_PORT_SDRAM  (1 <<  1)       //!< Enable SDRAM interface
#define EBI_PORT_SRAM   (1 <<  2)       //!< Enable SRAM interface
#define EBI_PORT_LPC    (1 <<  3)       //!< Enable LPC interface
#define EBI_PORT_CS0    (1 <<  4)       //!< Enable SRAM Chip Select 0
#define EBI_PORT_CS1    (1 <<  5)       //!< Enable SRAM Chip Select 1
#define EBI_PORT_CS2    (1 <<  6)       //!< Enable SRAM Chip Select 2
#define EBI_PORT_CS3    (1 <<  7)       //!< Enable SRAM Chip Select 3
//@}

#define EBI_PORT_CS_ALL (EBI_PORT_CS0 | EBI_PORT_CS1            \
				| EBI_PORT_CS2 | EBI_PORT_CS3)

/**
 * \brief Set up the I/O ports for use by the EBI.
 *
 * This function is meant to be called with constant parameters, in
 * which case it will generate a minimal amount of machine code.
 *
 * \note In SDRAM mode the \a sram_ale and \a lpc_ale parameters are ignored by
 *       the hardware.
 *
 * \param addr_bits Number of address lines to be used for SRAM/LPC.
 * \param sram_ale Number of address latches to enable for SRAM mode, 0 to 2.
 * \param lpc_ale Number of address latches to enable for LPC mode, 0 to 2
 * \param flags A bitwise combination of the EBI Port Configuration Flags.
 */
static inline void ebi_setup_port(uint8_t addr_bits, uint8_t sram_ale,
		uint8_t lpc_ale, uint8_t flags)
{
	uint8_t         mode;
	bool            disable_sysclk = true;

	if (sysclk_module_is_enabled(SYSCLK_PORT_GEN, SYSCLK_EBI)) {
		disable_sysclk = false;
	} else {
		sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_EBI);
	}

	/*
	 * Set address and control lines as outputs, and
	 * active-low control lines initially high.
	 */
	if (flags & EBI_PORT_SDRAM) {
		Assert(flags & EBI_PORT_3PORT);
		Assert(!(flags & EBI_PORT_CS_ALL));
		Assert(!(flags & (EBI_PORT_SRAM | EBI_PORT_LPC)));

		/* With SDRAM, the configuration is fairly fixed. */
		PORTH.OUT = 0x0f;
		PORTH.DIR = 0xff;
		PORTJ.DIR = 0xf0;
		PORTK.DIR = 0xff;
	} else {
		uint8_t port_mask;
		uint8_t ale_mask = sram_ale | lpc_ale;

		Assert(flags & (EBI_PORT_SRAM | EBI_PORT_LPC));
		Assert((flags & EBI_PORT_3PORT) || !(flags & EBI_PORT_SRAM));
		Assert(flags & EBI_PORT_CS_ALL);

		/*
		 * Set PORTH initial state, set WE and CAS/RE high by default.
		 * Set chip selects high by default if enabled.
		 */
		port_mask = 0x03;
		if (flags & EBI_PORT_CS0) {
			port_mask |= 0x10;
		}
		if (flags & EBI_PORT_CS1) {
			port_mask |= 0x20;
		}
		if (flags & EBI_PORT_CS2) {
			port_mask |= 0x40;
		}
		if (flags & EBI_PORT_CS3) {
			port_mask |= 0x80;
		}
		PORTH.OUT = port_mask;

		/*
		 * Set PORTH direction, enable WE, CAS/RE and RAS/ALE1 to
		 * output by default. Set chip select direction if enabled.
		 */
		port_mask = 0x07;

		/* If two latches are in use, enable the ALE2 pin as well. */
		if (ale_mask & 0x02) {
			port_mask |= 0x08;
		}
		if (flags & EBI_PORT_CS0 || addr_bits > 16) {
			port_mask |= 0x10;
		}
		if (flags & EBI_PORT_CS1 || addr_bits > 17) {
			port_mask |= 0x20;
		}
		if (flags & EBI_PORT_CS2 || addr_bits > 18) {
			port_mask |= 0x40;
		}
		if (flags & EBI_PORT_CS3 || addr_bits > 19) {
			port_mask |= 0x80;
		}
		PORTH.DIR = port_mask;

		/*
		 * PORTJ is always used for data, direction and value is
		 * controlled by the EBI module.
		 */

		/* PORTK is only used in 3-port mode */
		if (flags & EBI_PORT_3PORT) {
			port_mask = 0x00;

			if (flags & EBI_PORT_SRAM) {
				/*
				 * Bits 0..7 go here, so if we have 8
				 * lines or more, enable all lines.
				 * Otherwise, enable as many as we need.
				 */
				if (addr_bits < 8) {
					port_mask = (1 << addr_bits) - 1;
				} else {
					port_mask = 0xff;
				}
			} else {
				/*
				 * Bits 8..15 go here, so if we have
				 * less than 16 address lines, disable
				 * the ones that we don't need. If we
				 * have 8 lines or less, disable all
				 * address lines on this port.
				 */
				if (addr_bits <= 8) {
					port_mask = 0x00;
				}
				else if (addr_bits < 16) {
					port_mask = (1 << (addr_bits - 8)) - 1;
				} else {
					port_mask = 0xff;
				}
			}

			PORTK.DIR = port_mask;
		}
	}

	if (flags & EBI_PORT_3PORT) {
		mode = EBI_IFMODE_3PORT_gc;
	} else {
		mode = EBI_IFMODE_2PORT_gc;
	}

	if (sram_ale == 1) {
		mode |= EBI_SRMODE_ALE1_gc;
	}
	else if (sram_ale == 2) {
		mode |= EBI_SRMODE_ALE12_gc;
	} else {
		mode |= EBI_SRMODE_NOALE_gc;
	}

	if (lpc_ale > 0) {
		mode |= ((lpc_ale - 1) << EBI_LPCMODE_gp);
	}

	/* 8-bit SDRAM requires 4-port EBI, which we don't have. */
	EBI.CTRL = EBI_SDDATAW_4BIT_gc | mode;

	if (disable_sysclk) {
		sysclk_disable_module(SYSCLK_PORT_GEN, SYSCLK_EBI);
	}
}

//! @}

#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_EBI_EBI_PORT_H */
