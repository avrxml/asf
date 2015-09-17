/**
 * \file
 *
 * \brief MT48LC16M16A2 configuration.
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

#ifndef MT48LC16M16A2_H_INCLUDED
#define MT48LC16M16A2_H_INCLUDED

#include "compiler.h"
#include "sdramc.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

/**
 * \defgroup group_sam_components_memory_sdram_mt48lc16m16a2 SDRAM MT48LC16M16A2
 *
 * This is a configuration for the MT48LC16M16A2 SDRAM from Micron.
 * This configuration will be used by the SDRAM driver to set up the
 * MT48LC16M16A2 device.
 *
 * \{
 */

/* SDRAM MT48LC16M16A2 configuration */
const sdramc_memory_dev_t SDRAM_MICRON_MT48LC16M16A2 = {
	24, /* Block1 is at the bit 24, 1+9+13+1. */
	0,  /*
	     * This configures the SDRAM with the following parameters in the
	     *mode register:
	     * - bits 0 to 2: burst length: 1 (000b);
	     * - bit 3: burst type: sequential (0b);
	     * - bits 4 to 6: CAS latency;
	     * - bits 7 to 8: operating mode: standard operation (00b);
	     * - bit 9: write burst mode: programmed burst length (0b);
	     * - all other bits: reserved: 0b.
	     */
	{
		SDRAMC_CR_NC_COL9      | /* 9 column bits (512 bytes). */
		SDRAMC_CR_NR_ROW13     | /* 13 row bits    (8K). */
		SDRAMC_CR_NB_BANK4     | /* SDRAM 4 bank. */
		SDRAMC_CR_CAS_LATENCY2 | /* CAS Latency 2. */
		SDRAMC_CR_DBW          | /* Data bus width 16 bits. */
		SDRAMC_CR_TWR(2)       | /* Write Recovery Delay. */
		SDRAMC_CR_TRC_TRFC(9)  | /* Row Cycle Delay and Row Refresh
		                            Cycle. */
		SDRAMC_CR_TRP(3)       | /* Row Precharge Delay. */
		SDRAMC_CR_TRCD(3)      | /* Row to Column Delay. */
		SDRAMC_CR_TRAS(6)      | /* Active to Precharge Delay. */
		SDRAMC_CR_TXSR(10)       /* Exit from Self Refresh to Active
		                            Delay. */
	},
};

/**
 * \}
 */

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond

#endif /* MT48LC16M16A2_H_INCLUDED */
