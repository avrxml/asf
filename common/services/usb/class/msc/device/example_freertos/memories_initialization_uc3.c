/**
 * \file
 *
 * \brief Initialization of memories
 * Specific AVR UC3 functions to initialize memories.
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

#include <asf.h>
#include "conf_access.h"
#include "main.h"

#if (defined AT45DBX_MEM) && (AT45DBX_MEM == ENABLE)
# include "conf_at45dbx.h"
# include "at45dbx.h"
#endif

void memories_initialization(void)
{
	//-- Hmatrix bus configuration
	// This improve speed performance
#ifdef AVR32_HMATRIXB
	union {
		unsigned long scfg;
		avr32_hmatrixb_scfg_t SCFG;
	} u_avr32_hmatrixb_scfg;

	sysclk_enable_pbb_module(SYSCLK_HMATRIX);

	// For the internal-flash HMATRIX slave, use last master as default.
	u_avr32_hmatrixb_scfg.scfg =
			AVR32_HMATRIXB.scfg[AVR32_HMATRIXB_SLAVE_FLASH];
	u_avr32_hmatrixb_scfg.SCFG.defmstr_type =
			AVR32_HMATRIXB_DEFMSTR_TYPE_LAST_DEFAULT;
	AVR32_HMATRIXB.scfg[AVR32_HMATRIXB_SLAVE_FLASH] =
			u_avr32_hmatrixb_scfg.scfg;
	// For the internal-SRAM HMATRIX slave, use last master as default.
	u_avr32_hmatrixb_scfg.scfg =
			AVR32_HMATRIXB.scfg[AVR32_HMATRIXB_SLAVE_SRAM];
	u_avr32_hmatrixb_scfg.SCFG.defmstr_type =
			AVR32_HMATRIXB_DEFMSTR_TYPE_LAST_DEFAULT;
	AVR32_HMATRIXB.scfg[AVR32_HMATRIXB_SLAVE_SRAM] =
			u_avr32_hmatrixb_scfg.scfg;
# ifdef AVR32_HMATRIXB_SLAVE_EBI
	// For the EBI HMATRIX slave, use last master as default.
	u_avr32_hmatrixb_scfg.scfg =
			AVR32_HMATRIXB.scfg[AVR32_HMATRIXB_SLAVE_EBI];
	u_avr32_hmatrixb_scfg.SCFG.defmstr_type =
			AVR32_HMATRIXB_DEFMSTR_TYPE_LAST_DEFAULT;
	AVR32_HMATRIXB.scfg[AVR32_HMATRIXB_SLAVE_EBI] =
			u_avr32_hmatrixb_scfg.scfg;
# endif
#endif

#ifdef AVR32_HMATRIX
	union {
		unsigned long scfg;
		avr32_hmatrix_scfg_t SCFG;
	} u_avr32_hmatrix_scfg;

	sysclk_enable_pbb_module(SYSCLK_HMATRIX);

	// For the internal-flash HMATRIX slave, use last master as default.
	u_avr32_hmatrix_scfg.scfg =
			AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_FLASH];
	u_avr32_hmatrix_scfg.SCFG.defmstr_type =
			AVR32_HMATRIX_DEFMSTR_TYPE_LAST_DEFAULT;
	AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_FLASH] =
			u_avr32_hmatrix_scfg.scfg;
	// For the internal-SRAM HMATRIX slave, use last master as default.
	u_avr32_hmatrix_scfg.scfg =
			AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_SRAM];
	u_avr32_hmatrix_scfg.SCFG.defmstr_type =
			AVR32_HMATRIX_DEFMSTR_TYPE_LAST_DEFAULT;
	AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_SRAM] =
			u_avr32_hmatrix_scfg.scfg;
# ifdef AVR32_HMATRIX_SLAVE_EBI
	// For the EBI HMATRIX slave, use last master as default.
	u_avr32_hmatrix_scfg.scfg =
			AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_EBI];
	u_avr32_hmatrix_scfg.SCFG.defmstr_type =
			AVR32_HMATRIX_DEFMSTR_TYPE_LAST_DEFAULT;
	AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_EBI] =
			u_avr32_hmatrix_scfg.scfg;
# endif
#endif

#ifdef AVR32_HMATRIX_MASTER_USBB_DMA
	union {
		unsigned long                 mcfg;
		avr32_hmatrix_mcfg_t          MCFG;
	} u_avr32_hmatrix_mcfg;
 
	// For the USBB DMA HMATRIX master, use infinite length burst.
	u_avr32_hmatrix_mcfg.mcfg =
			AVR32_HMATRIX.mcfg[AVR32_HMATRIX_MASTER_USBB_DMA];
	u_avr32_hmatrix_mcfg.MCFG.ulbt =
			AVR32_HMATRIX_ULBT_INFINITE;
	AVR32_HMATRIX.mcfg[AVR32_HMATRIX_MASTER_USBB_DMA] =
			u_avr32_hmatrix_mcfg.mcfg;
	
	// For the USBB DPRAM HMATRIX slave, use the USBB DMA as fixed default master.
	u_avr32_hmatrix_scfg.scfg =
			AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_USBB_DPRAM];
	u_avr32_hmatrix_scfg.SCFG.fixed_defmstr =
			AVR32_HMATRIX_MASTER_USBB_DMA;
	u_avr32_hmatrix_scfg.SCFG.defmstr_type =
			AVR32_HMATRIX_DEFMSTR_TYPE_FIXED_DEFAULT;
	AVR32_HMATRIX.scfg[AVR32_HMATRIX_SLAVE_USBB_DPRAM] =
			u_avr32_hmatrix_scfg.scfg;
#endif

#if (defined AT45DBX_MEM) && (AT45DBX_MEM == ENABLE)
	at45dbx_init();
#endif

#if ((defined SD_MMC_0_MEM) && (SD_MMC_0_MEM == ENABLE)) \
	|| ((defined SD_MMC_1_MEM) && (SD_MMC_1_MEM == ENABLE))
	sd_mmc_init();
#endif
}

