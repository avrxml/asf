/**
 * \file
 *
 * \brief External bus interface (EBI) functions
 *
 * Copyright (c) 2010-2015 Atmel Corporation. All rights reserved.
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
#ifndef DRIVERS_EBI_EBI_H
#define DRIVERS_EBI_EBI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "compiler.h"

#include "sleepmgr.h"

// Header patch to support old headers versions from IAR and GCC compilers.
#ifndef EBI_CS_ASPACE_gm
# define EBI_CS_ASPACE_gm       EBI_CS_ASIZE_gm
# define EBI_CS_ASPACE_t        EBI_CS_ASIZE_t
# define EBI_CS_ASPACE_256B_gc  EBI_CS_ASIZE_256B_gc
# define EBI_CS_ASPACE_512B_gc  EBI_CS_ASIZE_512B_gc
# define EBI_CS_ASPACE_1KB_gc   EBI_CS_ASIZE_1KB_gc
# define EBI_CS_ASPACE_2KB_gc   EBI_CS_ASIZE_2KB_gc
# define EBI_CS_ASPACE_4KB_gc   EBI_CS_ASIZE_4KB_gc
# define EBI_CS_ASPACE_8KB_gc   EBI_CS_ASIZE_8KB_gc
# define EBI_CS_ASPACE_16KB_gc  EBI_CS_ASIZE_16KB_gc
# define EBI_CS_ASPACE_32KB_gc  EBI_CS_ASIZE_32KB_gc
# define EBI_CS_ASPACE_64KB_gc  EBI_CS_ASIZE_64KB_gc
# define EBI_CS_ASPACE_128KB_gc EBI_CS_ASIZE_128KB_gc
# define EBI_CS_ASPACE_256KB_gc EBI_CS_ASIZE_256KB_gc
# define EBI_CS_ASPACE_512KB_gc EBI_CS_ASIZE_512KB_gc
# define EBI_CS_ASPACE_1MB_gc   EBI_CS_ASIZE_1MB_gc
# define EBI_CS_ASPACE_2MB_gc   EBI_CS_ASIZE_2MB_gc
# define EBI_CS_ASPACE_4MB_gc   EBI_CS_ASIZE_4MB_gc
# define EBI_CS_ASPACE_8MB_gc   EBI_CS_ASIZE_8MB_gc
# define EBI_CS_ASPACE_16M_gc   EBI_CS_ASIZE_16M_gc
#endif


/**
 * \defgroup ebi_group External Bus Interface (EBI)
 *
 * See \ref xmega_ebi_quickstart
 *
 * \brief This is a driver for configuring, enabling, disabling and use of
 * the on-chip external bus interface (EBI).
 *
 * \section dependencies Dependencies
 *
 * The external bus interface depends on the following modules:
 *  - \ref sysclk_group for peripheral clock control and getting bus speeds.
 *  - \ref sleepmgr_group for setting available sleep mode.
 *
 *
 * \section xmega_ebi_quickstart_section Quick Start Guide
 * See \ref xmega_ebi_quickstart
 * @{
 */

//! Chip select on the EBI that can be configured for SDRAM operation
#define EBI_SDRAM_CS    3

//! External bus interface chip select configuration
struct ebi_cs_config {
	//! EBI chip select control A register
	uint8_t         ctrla;
	//! EBI chip select control B register
	uint8_t         ctrlb;
	//! EBI chip select base address register
	uint16_t        base_address;
};

//! External bus interface SDRAM configuration
struct ebi_sdram_config {
	//! EBI SDRAM control A register
	uint8_t         sdramctrla;
	//! EBI SDRAM control B register
	uint8_t         sdramctrlb;
	//! EBI SDRAM control C register
	uint8_t         sdramctrlc;
	//! EBI SDRAM refresh period register
	uint16_t        refresh_period;
	//! EBI SDRAM initialization delay register
	uint16_t        init_delay;
};

//! \name External Bus Interface management
//@{
void ebi_cs_write_config(const uint8_t cs, const struct ebi_cs_config *config);
void ebi_cs_read_config(const uint8_t cs, struct ebi_cs_config *config);

void ebi_sdram_write_config(const struct ebi_sdram_config *config);
void ebi_sdram_read_config(struct ebi_sdram_config *config);

void ebi_enable_cs(const uint8_t cs, const struct ebi_cs_config *config);
void ebi_disable_cs(const uint8_t cs);

/**
 * \brief Check if SDRAM is ready for access
 *
 * This function must be checked after the SDRAM chip select has been enabled.
 * It will return true when the SDRAM is initialized.
 *
 * \retval true when SDRAM is ready, false otherwise
 */
static inline bool ebi_sdram_is_ready(void)
{
	return (EBI.CS3.CTRLB & EBI_CS_SDINITDONE_bm);
}

//@}

//! \name External Bus Interface Chip Select configuration helper functions
//@{

/**
 * \brief Set chip select mode
 *
 * This function helps the caller setting the chip select mode. The different
 * modes are provided by an EBI_CS_MODE_t type from the toolchain header files.
 *
 * \param config Pointer to an \ref ebi_cs_config variable
 * \param mode Chip select mode given by an EBI_CS_MODE_t type
 */
static inline void ebi_cs_set_mode(struct ebi_cs_config *config,
		EBI_CS_MODE_t mode)
{
	config->ctrla &= ~EBI_CS_MODE_gm;
	config->ctrla |= mode;
}

/**
 * \brief Set chip select address size
 *
 * This function helps the caller setting the chip select address size. The
 * different sizes are provided by an EBI_CS_ASPACE_t type from the toolchain
 * header files.
 *
 * \param config Pointer to an \ref ebi_cs_config variable
 * \param size Chip select address size given by an EBI_CS_ASPACE_t type
 */
static inline void ebi_cs_set_address_size(struct ebi_cs_config *config,
		EBI_CS_ASPACE_t size)
{
	config->ctrla &= ~EBI_CS_ASPACE_gm;
	config->ctrla |= size;
}

/**
 * \brief Set chip select SRAM wait states
 *
 * This function helps the caller setting the chip select SRAM wait states. The
 * different SRAM wait states (cycles) are provided by an EBI_CS_SRWS_t type
 * from the toolchain header files.
 *
 * \param config Pointer to an \ref ebi_cs_config variable
 * \param cycles Chip select SRAM wait states given by an EBI_CS_SRWS_t type
 */
static inline void ebi_cs_set_sram_wait_states(struct ebi_cs_config *config,
		EBI_CS_SRWS_t cycles)
{
	config->ctrlb &= ~EBI_CS_SRWS_gm;
	config->ctrlb |= cycles;
}

/**
 * \brief Set chip select base address
 *
 * This function helps the caller setting the chip select base address, e.g.,
 * the lowest address space enabled by the chip select.
 *
 * \param config Pointer to an \ref ebi_cs_config variable
 * \param address The chip select base address
 */
static inline void ebi_cs_set_base_address(struct ebi_cs_config *config,
		uint32_t address)
{
	config->base_address = (address >> 8) & 0xfff0;
}

//@}

//! \name External Bus Interface SDRAM configuration helper functions
//@{

/**
 * \brief Set SDRAM mode
 *
 * This function helps the caller setting the SDRAM mode. The different modes
 * are provided by an EBI_CS_SDMODE_t type from the toolchain header files.
 *
 * \note This function needs a pointer to an \ref ebi_cs_config struct, not an
 * \ref ebi_sdram_config struct as the other SDRAM configuration helper
 * function does.
 *
 * \param config Pointer to an \ref ebi_cs_config variable
 * \param mode SDRAM mode given by an EBI_CS_SDMODE_t type
 */
static inline void ebi_sdram_set_mode(struct ebi_cs_config *config,
		EBI_CS_SDMODE_t mode)
{
	config->ctrlb &= ~EBI_CS_SDMODE_gm;
	config->ctrlb |= mode;
}

/**
 * \brief Set SDRAM CAS latency
 *
 * This function helps the caller setting the SDRAM CAS latency.
 *
 * \param config Pointer to an \ref ebi_sdram_config variable
 * \param cas SDRAM CAS latency, 2 or 3
 */
static inline void ebi_sdram_set_cas_latency(struct ebi_sdram_config *config,
		const uint8_t cas)
{
	Assert(cas == 2 || cas == 3);

	if (cas == 3) {
		config->sdramctrla |= EBI_SDCAS_bm;
	} else {
		config->sdramctrla &= ~EBI_SDCAS_bm;
	}
}

/**
 * \brief Set number of SDRAM row bits
 *
 * This function helps the caller setting the number of SDRAM row bits.
 *
 * \param config Pointer to an \ref ebi_sdram_config variable
 * \param bits Number of row bits, 11 or 12
 */
static inline void ebi_sdram_set_row_bits(struct ebi_sdram_config *config,
		const uint8_t bits)
{
	Assert(bits == 11 || bits == 12);

	if (bits == 12) {
		config->sdramctrla |= EBI_SDROW_bm;
	} else {
		config->sdramctrla &= ~EBI_SDROW_bm;
	}
}

/**
 * \brief Set number of SDRAM column bits
 *
 * This function helps the caller setting the number of SDRAM column bits.
 *
 * \param config Pointer to an \ref ebi_sdram_config variable
 * \param bits Number of column bits, 8 to 11
 */
static inline void ebi_sdram_set_col_bits(struct ebi_sdram_config *config,
		const uint8_t bits)
{
	Assert(bits >= 8 && bits <= 11);

	config->sdramctrla &= ~EBI_SDCOL_gm;
	config->sdramctrla |= (bits - 8) << EBI_SDCOL_gp;
}

/**
 * \brief Set SDRAM refresh period
 *
 * This function helps the caller setting the SDRAM refresh period as a number
 * of peripheral 2x clock cycles (CLKper2).
 *
 * \param config Pointer to an \ref ebi_sdram_config variable
 * \param period SDREAM refresh period, number of CLKper2 cycles
 */
static inline void ebi_sdram_set_refresh_period(
		struct ebi_sdram_config *config, const uint16_t period)
{
	config->refresh_period = period;
}

/**
 * \brief Set SDRAM initialization delay
 *
 * This function helps the caller setting the SDRAM initialization delay as a
 * number of peripheral 2x clock cycles (CLKper2).
 *
 * \param config Pointer to an \ref ebi_sdram_config variable
 * \param delay SDREAM initialization delay, number of CLKper2 cycles
 */
static inline void ebi_sdram_set_initialization_delay(
		struct ebi_sdram_config *config, const uint16_t delay)
{
	config->init_delay = delay;
}

/**
 * \brief Set SDRAM mode delay
 *
 * This function helps the caller setting the SDRAM mode delay as a number of
 * peripheral 2x clock cycles (CLKper2). The different modes are provided by an
 * EBI_MRDLY_t type from the toolchain header files.
 *
 * \param config Pointer to an \ref ebi_sdram_config variable
 * \param delay SDRAM mode delay given by an EBI_MRDLY_t type
 */
static inline void ebi_sdram_set_mode_delay(struct ebi_sdram_config *config,
		const EBI_MRDLY_t delay)
{
	config->sdramctrlb &= ~EBI_MRDLY_gm;
	config->sdramctrlb |= delay;
}

/**
 * \brief Set SDRAM row cycle delay
 *
 * This function helps the caller setting the SDRAM row cycle delay as a number
 * of peripheral clock 2x cycles (CLKper2). The different delays are provided
 * by an EBI_ROWCYCDLY_t type from the toolchain header files.
 *
 * \param config Pointer to an \ref ebi_sdram_config variable
 * \param delay SDRAM row cycle delay given by an EBI_ROWCYCDLY_t type
 */
static inline void ebi_sdram_set_row_cycle_delay(
		struct ebi_sdram_config *config, const EBI_ROWCYCDLY_t delay)
{
	config->sdramctrlb &= ~EBI_ROWCYCDLY_gm;
	config->sdramctrlb |= delay;
}

/**
 * \brief Set SDRAM row to pre-charge delay
 *
 * This function helps the caller setting the SDRAM row to pre-charge delay as
 * a number of peripheral clock 2x cycles (CLKper2). The different delays are
 * provided by an EBI_RPDLY_t type from the toolchain header files.
 *
 * \param config Pointer to an \ref ebi_sdram_config variable
 * \param delay SDRAM row to pre-charge delay given by an EBI_ROWCYCDLY_t type
 */
static inline void ebi_sdram_set_row_to_precharge_delay(
		struct ebi_sdram_config *config, const EBI_RPDLY_t delay)
{
	config->sdramctrlb &= ~EBI_RPDLY_gm;
	config->sdramctrlb |= delay;
}

/**
 * \brief Set SDRAM write recovery delay
 *
 * This function helps the caller setting the SDRAM write recovery delay as a
 * number of peripheral clock 2x cycles (CLKper2). The different delays are
 * provided by an EBI_WRDLY_t type from the toolchain header files.
 *
 * \param config Pointer to an \ref ebi_sdram_config variable
 * \param delay SDRAM write recovery delay given by an EBI_WRDLY_t type
 */
static inline void ebi_sdram_set_write_recovery_delay(
		struct ebi_sdram_config *config, const EBI_WRDLY_t delay)
{
	config->sdramctrlc &= ~EBI_WRDLY_gm;
	config->sdramctrlc |= delay;
}

/**
 * \brief Set SDRAM exit self refresh to active delay
 *
 * This function helps the caller setting the SDRAM exit self refresh to active
 * delay as a number of peripheral clock 2x cycles (CLKper2). The different
 * delays are provided by an EBI_ESRDLY_t type from the toolchain header files.
 *
 * \param config Pointer to an \ref ebi_sdram_config variable
 * \param delay SDRAM exit self refresh to active delay given by an
 *              EBI_ESRDLY_t type
 */
static inline void ebi_sdram_set_self_refresh_to_active_delay(
		struct ebi_sdram_config *config, const EBI_ESRDLY_t delay)
{
	config->sdramctrlc &= ~EBI_ESRDLY_gm;
	config->sdramctrlc |= delay;
}

/**
 * \brief Set SDRAM row to column delay
 *
 * This function helps the caller setting the SDRAM row to column delay as a
 * number of peripheral clock 2x cycles (CLKper2). The different delays are
 * provided by an EBI_ROWCOLDLY_t type from the toolchain header files.
 *
 * \param config Pointer to an \ref ebi_sdram_config variable
 * \param delay SDRAM row to column delay given by an EBI_ROWCOLDLY_t type
 */
static inline void ebi_sdram_set_row_to_col_delay(
		struct ebi_sdram_config *config, const EBI_ROWCOLDLY_t delay)
{
	config->sdramctrlc &= ~EBI_ROWCOLDLY_gm;
	config->sdramctrlc |= delay;
}

//@}

//! \name External Bus Interface SDRAM power management
//@{

/**
 * \brief Enable SDRAM self refresh mode
 *
 * This function makes the EBI hardware send a command to the SDRAM to enter
 * self refresh mode.
 */
static inline void ebi_sdram_enable_self_refresh(void)
{
	irqflags_t      iflags = cpu_irq_save();

	EBI.CS3.CTRLB |= EBI_CS_SDSREN_bm;
	sleepmgr_unlock_mode(SLEEPMGR_IDLE);

	cpu_irq_restore(iflags);
}

/**
 * \brief Disable SDRAM self refresh mode
 *
 * This function makes the EBI hardware send a command to the SDRAM to leave
 * self refresh mode.
 */
static inline void ebi_sdram_disable_self_refresh(void)
{
	irqflags_t      iflags = cpu_irq_save();

	sleepmgr_lock_mode(SLEEPMGR_IDLE);
	EBI.CS3.CTRLB &= ~EBI_CS_SDSREN_bm;

	cpu_irq_restore(iflags);
}

//@}

//! @}

/**
 * \page xmega_ebi_quickstart Quick Start Guide for the XMEGA EBI Driver
 *
 * This is the quick start guide for the \ref ebi_group "EBI Driver", with
 * step-by-step instructions on how to configure and use the driver for
 * specific use cases.
 *
 * The section described below can be compiled into e.g. the main application
 * loop or any other function that might use the EBI functionality.
 *
 * \section xmega_ebi_quickstart_basic Basic usage of the EBI driver
 *
 * Basic usage of the EBI driver would be to interface external SRAM/SDRAM.
 * Our specific use case will cover usage of SDRAM.
 *
 * \section xmega_ebi_quickstart_sdram_case Interfacing SDRAM
 *
 * A basic use case for the EBI driver would be to interface SDRAM, and in
 * this specific case, interfacing the on-board 8 MB SDRAM chip of the XMEGA A1
 * Xplained board. The module is connected in three-port mode, utilizing 12
 * address lines. The timing information in the configuration of the device
 * is specific to this chip. You may directly use the source code if you are
 * using the same chip (or board), otherwise you should refer to the SDRAM
 * chip's data sheet for specific timing information.
 *
 * \subsection xmega_ebi_quickstart_sdram_prereq Prerequisites
 * To be able to interface the external SDRAM module, the \ref hugemem_group
 * (hugemem) module must be included and enabled. This is done by selecting
 * the hugemem module from ASF, and including the line
 *
 * \code
	#define CONFIG_HAVE_HUGEMEM
\endcode
 *
 * in your source code. We also need some information about our chip, such as
 * address start, size of usable memory, refresh rate and initialization delay.
 *
 * \code
	#define BOARD_EBI_SDRAM_BASE 0x800000UL
	#define BOARD_EBI_SDRAM_SIZE 0x800000UL
	#define BOARD_EBI_SDRAM_REFRESH (16 * 2 * sysclk_get_per2_hz() / 1000000)
	#define BOARD_EBI_SDRAM_INITDLY (100 * 2 * sysclk_get_per2_hz() / 1000000)
\endcode
 *
 * Include string.h for memset() and strlen() functions:
 * \code
	#include <string.h>
\endcode
 *
 * \section xmega_ebi_quickstart_sdram_case_setup_steps Setup steps
 *
 * \subsection xmega_ebi_quickstart_sdram_case_setup_steps_code Code
 * \code
	 void ebi_init() {
	     struct ebi_cs_config cs_config;
	     struct ebi_sdram_config sdram_config;

	     memset(&cs_config, 0, sizeof(struct ebi_cs_config));
	     memset(&sdram_config, 0, sizeof(struct ebi_sdram_config));

	     ebi_setup_port(12, 0, 0, EBI_PORT_3PORT | EBI_PORT_SDRAM);

	     ebi_cs_set_mode(&cs_config, EBI_CS_MODE_SDRAM_gc);
	     ebi_cs_set_address_size(&cs_config, EBI_CS_ASPACE_8MB_gc);
	     ebi_cs_set_base_address(&cs_config, BOARD_EBI_SDRAM_BASE);

	     ebi_sdram_set_mode(&cs_config, EBI_CS_SDMODE_NORMAL_gc);

	     ebi_sdram_set_row_bits(&sdram_config, 12);
	     ebi_sdram_set_col_bits(&sdram_config, 10);

	     ebi_sdram_set_cas_latency(&sdram_config, 3);
	     ebi_sdram_set_mode_delay(&sdram_config, EBI_MRDLY_2CLK_gc);
	     ebi_sdram_set_row_cycle_delay(&sdram_config, EBI_ROWCYCDLY_7CLK_gc);
	     ebi_sdram_set_row_to_precharge_delay(&sdram_config, EBI_RPDLY_7CLK_gc);
	     ebi_sdram_set_write_recovery_delay(&sdram_config, EBI_WRDLY_1CLK_gc);
	     ebi_sdram_set_self_refresh_to_active_delay(&sdram_config,
	             EBI_ESRDLY_7CLK_gc);
	     ebi_sdram_set_row_to_col_delay(&sdram_config, EBI_ROWCOLDLY_7CLK_gc);
	     ebi_sdram_set_refresh_period(&sdram_config, BOARD_EBI_SDRAM_REFRESH);
	     ebi_sdram_set_initialization_delay(&sdram_config,
	             BOARD_EBI_SDRAM_INITDLY);

	     ebi_sdram_write_config(&sdram_config);
	     ebi_cs_write_config(EBI_SDRAM_CS, &cs_config);

	     ebi_enable_cs(EBI_SDRAM_CS, &cs_config);

	     while (!ebi_sdram_is_ready()) {
	         // Wait
	     }
	 }
\endcode
 *
 * \subsection xmega_ebi_quickstart_sdram_case_setup_workflow Workflow
 *
 * After the board has been initialized, we need to initialize the EBI driver.
 * We need two configuration structures to configure the driver:
 *
 * \code
	 struct ebi_cs_config cs_config;
	 struct ebi_sdram_config sdram_config;

	 memset(&cs_config, 0, sizeof(struct ebi_cs_config));
	 memset(&sdram_config, 0, sizeof(struct ebi_sdram_config));
\endcode
 * \note It is important to zero out the config struct using memset, so that
 *       we do not produce any unwanted behavior.
 *
 * We set up the EBI driver for three-port, 12 address line SDRAM operation:
 * \code
	ebi_setup_port(12, 0, 0, EBI_PORT_3PORT | EBI_PORT_SDRAM);
\endcode
 *
 * We set the chip select mode to SDRAM, address space size to 8 MB, and the
 * base address:
 * \code
	ebi_cs_set_mode(&cs_config, EBI_CS_MODE_SDRAM_gc);
	ebi_cs_set_address_size(&cs_config, EBI_CS_ASPACE_8MB_gc);
	ebi_cs_set_base_address(&cs_config, BOARD_EBI_SDRAM_BASE);
\endcode
 *
 * We want normal SDRAM operation:
 * \code
	ebi_sdram_set_mode(&cs_config, EBI_CS_SDMODE_NORMAL_gc);
\endcode
 *
 * SDRAM is organized in rows and columns, and we want 12 rows and 10 columns:
 * \code
	ebi_sdram_set_row_bits(&sdram_config, 12);
	ebi_sdram_set_col_bits(&sdram_config, 10);
\endcode
 *
 * The timing information for our device is:
 * - CAS Latency - 3 clock cycles
 * - Mode delay - 2 clock cycles
 * - Row cycle delay - 7 clock cycles
 * - Row pre-charge delay - 7 clock cycles
 * - Write recovery delay - 1 clock cycle
 * - Self refresh to active delay - 7 clock cycles
 * - Row to column delay - 7 clock cycles
 * - Refresh period - Dependent on frequency, see
 *   \ref xmega_ebi_quickstart_sdram_prereq.
 * - Init delay - Dependent on frequency, see
 *   \ref xmega_ebi_quickstart_sdram_prereq.
 *
 * Set all timing information:
 * \code
	ebi_sdram_set_cas_latency(&sdram_config, 3);
	ebi_sdram_set_mode_delay(&sdram_config, EBI_MRDLY_2CLK_gc);
	ebi_sdram_set_row_cycle_delay(&sdram_config, EBI_ROWCYCDLY_7CLK_gc);
	ebi_sdram_set_row_to_precharge_delay(&sdram_config, EBI_RPDLY_7CLK_gc);
	ebi_sdram_set_write_recovery_delay(&sdram_config, EBI_WRDLY_1CLK_gc);
	ebi_sdram_set_self_refresh_to_active_delay(&sdram_config,
	        EBI_ESRDLY_7CLK_gc);
	ebi_sdram_set_row_to_col_delay(&sdram_config, EBI_ROWCOLDLY_7CLK_gc);
	ebi_sdram_set_refresh_period(&sdram_config, BOARD_EBI_SDRAM_REFRESH);
	ebi_sdram_set_initialization_delay(&sdram_config,
	        BOARD_EBI_SDRAM_INITDLY);
\endcode
 *
 * Our configuration is complete, now we write it to the device:
 * \code
	ebi_sdram_write_config(&sdram_config);
	ebi_cs_write_config(EBI_SDRAM_CS, &cs_config);
\endcode
 *
 * And enable the chip select for the device:
 * \code
	ebi_enable_cs(EBI_SDRAM_CS, &cs_config);
\endcode
 *
 * Now we wait for the EBI driver to initialize and be ready for use:
 * \code
	while (!ebi_sdram_is_ready()) {
	    // Wait
	}
\endcode
 * \note Not waiting for the EBI to become ready will result in undefined
 *       behavior.
 * \section xmega_ebi_quickstart_sdram_case_example_code Example usage code
 *
 * The EBI is now configured and running. We will write a string to memory,
 * and read it back again.
 * \subsection xmega_ebi_quickstart_sdram_usage_example_code Code
 *
 * \code
	 #define DATA_SIZE 32

	 hugemem_ptr_t ptr;
	 uint8_t data[DATA_SIZE] = "This data written to SDRAM";
	 uint8_t read_data[DATA_SIZE];
	 uint8_t i;

	 ptr = BOARD_EBI_SDRAM_BASE;

	 memset(read_data, 0, DATA_SIZE);

	 for(i = 0; i < strlen(data); i++) {
	     hugemem_write8(ptr, data[i]);
	     ptr = ptr + sizeof(uint8_t);
	 }

	 ptr = BOARD_EBI_SDRAM_BASE;

	 for(i = 0; i < strlen(data); i++) {
	     read_data[i] = (uint8_t)hugemem_read8(ptr);
	     ptr = ptr + sizeof(uint8_t);
	 }
\endcode
 *
 * \subsection xmega_ebi_quickstart_sdram_case_workflow Workflow
 *
 * We declare the size of our data chucks, a hugemem-pointer which we will use
 * to point to SDRAM, a chuck of data, and a chuck to read into.
 * \code
	 #define DATA_SIZE 32

	 hugemem_ptr_t ptr;
	 uint8_t data[DATA_SIZE] = "This data written to SDRAM";
	 uint8_t read_data[DATA_SIZE];
	 uint8_t i;
\endcode
 *
 * We point the pointer to the beginning of SDRAM.
 * \code
	ptr = BOARD_EBI_SDRAM_BASE;
\endcode
 *
 * We zero out the read_data variable to make sure its empty.
 * \code
	memset(read_data, 0, DATA_SIZE);
\endcode
 *
 * We now write the data to SDRAM.
 * \code
	for(i = 0; i < strlen(data); i++) {
	    hugemem_write8(ptr, data[i]);
	    ptr = ptr + sizeof(uint8_t);
	}
\endcode
 *
 * The ptr variable now points to the end of the string, so reset it.
 * \code
	ptr = BOARD_EBI_SDRAM_BASE;
\endcode
 *
 * We now read the data back into the read_data chuck:
 * \code
	for(i = 0; i < strlen(data); i++) {
	    read_data[i] = (uint8_t)hugemem_read8(ptr);
	    ptr = ptr + sizeof(uint8_t);
	}
\endcode
 *
 * The read_data variable now contains the same as the data variable.
 */
#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_EBI_EBI_H */
