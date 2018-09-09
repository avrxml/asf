/**
 * \file
 *
 * \brief External bus interface (EBI) functions
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
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
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
