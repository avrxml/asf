/**
 * \file
 *
 * \brief External bus interface (EBI) functions
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
#include <stdint.h>

#include "compiler.h"

#include "ebi.h"
#include "sysclk.h"
#include "sleepmgr.h"
#include "interrupt.h"

//! \internal Local storage of enabled chip selects
static uint8_t ebi_enabled_chip_selects;

/**
 * \internal
 * \brief Get chip select register address from chip select number
 *
 * This functions converts the chip select number into an EBI_CS_t memory
 * pointer, which points to the base of the chip select register.
 *
 * \param cs Chip select number to get the chip select register address for
 *
 * \return Pointer to a chip select register address
 */
static inline EBI_CS_t *ebi_get_cs_addr_from_cs_num(const uint8_t cs)
{
	Assert(cs <= 3);

	switch (cs) {
	case 0:
		return &EBI.CS0;

	case 1:
		return &EBI.CS1;

	case 2:
		return &EBI.CS2;

	default:
		/*
		 * Fall-through, return highest available chip select address
		 * if parameter is out of range.
		 */
	case 3:
		return &EBI.CS3;
	}
}

/**
 * \brief Write EBI chip select configuration to hardware
 *
 * This function will write an \ref ebi_cs_config struct to the selected chip
 * select hardware.
 *
 * \param cs Chip select number to write configuration for
 * \param config Pointer to an \ref ebi_cs_config variable
 */
void ebi_cs_write_config(const uint8_t cs, const struct ebi_cs_config *config)
{
	irqflags_t      iflags;
	EBI_CS_t        *cs_p = ebi_get_cs_addr_from_cs_num(cs);
	bool            disable_sysclk = true;

	iflags = cpu_irq_save();

	if (sysclk_module_is_enabled(SYSCLK_PORT_GEN, SYSCLK_EBI)) {
		disable_sysclk = false;
	} else {
		sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_EBI);
	}

	cs_p->CTRLA     = config->ctrla & ~EBI_CS_MODE_gm;
	cs_p->CTRLB     = config->ctrlb;
	cs_p->BASEADDR  = config->base_address;

	if (disable_sysclk) {
		sysclk_disable_module(SYSCLK_PORT_GEN, SYSCLK_EBI);
	}

	cpu_irq_restore(iflags);
}

/**
 * \brief Read EBI chip select configuration from hardware
 *
 * This function will read the EBI chip select hardware configuration and put
 * it into the \ref ebi_cs_config argument.
 *
 * \param cs Chip select number to read configuration from
 * \param config Pointer to an \ref ebi_cs_config variable
 */
void ebi_cs_read_config(const uint8_t cs, struct ebi_cs_config *config)
{
	irqflags_t      iflags;
	EBI_CS_t        *cs_p = ebi_get_cs_addr_from_cs_num(cs);
	bool            disable_sysclk = true;

	iflags = cpu_irq_save();

	if (sysclk_module_is_enabled(SYSCLK_PORT_GEN, SYSCLK_EBI)) {
		disable_sysclk = false;
	} else {
		sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_EBI);
	}

	config->ctrla           = cs_p->CTRLA;
	config->ctrlb           = cs_p->CTRLB;
	config->base_address    = cs_p->BASEADDR;

	if (disable_sysclk) {
		sysclk_disable_module(SYSCLK_PORT_GEN, SYSCLK_EBI);
	}

	cpu_irq_restore(iflags);
}

/**
 * \brief Write EBI SDRAM configuration to hardware
 *
 * This function will write an \ref ebi_sdram_config struct to the SDRAM part
 * of the EBI hardware.
 *
 * \note SDRAM is by design limited to chip select 3.
 *
 * \param config Pointer to an \ref ebi_sdram_config variable
 */
void ebi_sdram_write_config(const struct ebi_sdram_config *config)
{
	irqflags_t      iflags;
	bool            disable_sysclk = true;

	iflags = cpu_irq_save();

	if (sysclk_module_is_enabled(SYSCLK_PORT_GEN, SYSCLK_EBI)) {
		disable_sysclk = false;
	} else {
		sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_EBI);
	}

	EBI.SDRAMCTRLA  = config->sdramctrla;
	EBI.SDRAMCTRLB  = config->sdramctrlb;
	EBI.SDRAMCTRLC  = config->sdramctrlc;
	EBI.REFRESH     = config->refresh_period;
	EBI.INITDLY     = config->init_delay;

	if (disable_sysclk) {
		sysclk_disable_module(SYSCLK_PORT_GEN, SYSCLK_EBI);
	}

	cpu_irq_restore(iflags);
}

/**
 * \brief Read EBI SDRAM configuration from hardware
 *
 * This function will read the SDRAM EBI hardware configuration and put it into
 * the \ref ebi_cs_config argument.
 *
 * \note SDRAM is by design limited to chip select 3.
 *
 * \param config Pointer to an \ref ebi_sdram_config variable
 */
void ebi_sdram_read_config(struct ebi_sdram_config *config)
{
	irqflags_t      iflags;
	bool            disable_sysclk = true;

	iflags = cpu_irq_save();

	if (sysclk_module_is_enabled(SYSCLK_PORT_GEN, SYSCLK_EBI)) {
		disable_sysclk = false;
	} else {
		sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_EBI);
	}

	config->sdramctrla      = EBI.SDRAMCTRLA;
	config->sdramctrlb      = EBI.SDRAMCTRLB;
	config->sdramctrlc      = EBI.SDRAMCTRLC;
	config->refresh_period  = EBI.REFRESH;
	config->init_delay      = EBI.INITDLY;

	if (disable_sysclk) {
		sysclk_disable_module(SYSCLK_PORT_GEN, SYSCLK_EBI);
	}

	cpu_irq_restore(iflags);
}

/**
 * \brief Enable an EBI chip select
 *
 * \param cs Chip select number to enable
 * \param config Pointer to an \ref ebi_cs_config variable
 */
void ebi_enable_cs(const uint8_t cs, const struct ebi_cs_config *config)
{
	EBI_CS_t        *cs_p = ebi_get_cs_addr_from_cs_num(cs);
	irqflags_t      iflags = cpu_irq_save();

	sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_EBI);
	sleepmgr_lock_mode(SLEEPMGR_IDLE);

	cs_p->CTRLA = config->ctrla;

	ebi_enabled_chip_selects |= (1 << cs);

	cpu_irq_restore(iflags);
}

/**
 * \brief Disable an EBI chip select
 *
 * \param cs Chip select number to disable
 */
void ebi_disable_cs(const uint8_t cs)
{
	EBI_CS_t        *cs_p = ebi_get_cs_addr_from_cs_num(cs);
	irqflags_t      iflags = cpu_irq_save();

	ebi_enabled_chip_selects &= ~(1 << cs);

	cs_p->CTRLA &= ~EBI_CS_MODE_gm;

	if (!ebi_enabled_chip_selects) {
		sysclk_disable_module(SYSCLK_PORT_GEN, SYSCLK_EBI);
	}

	sleepmgr_unlock_mode(SLEEPMGR_IDLE);

	cpu_irq_restore(iflags);
}
