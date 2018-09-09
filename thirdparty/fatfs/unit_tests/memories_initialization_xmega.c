/**
 * \file
 *
 * \brief Initialization of memories (XMEGA)
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
#include "asf.h"
#include "memories_initialization.h"

//! SDRAM initialization delay in number of CLKper2 cycles (100 us)
#define BOARD_EBI_SDRAM_INITDLY         \
	(100 * 2 * sysclk_get_per2_hz() / 1000000)

//! SDRAM refresh interval in number of CLKper2 cycles (16 us)
#define BOARD_EBI_SDRAM_REFRESH         \
	(16 * 2 * sysclk_get_per2_hz() / 1000000)

void memories_initialization(void)
{
	struct ebi_cs_config    cs_config;
	struct ebi_sdram_config sdram_config;

	memset(&cs_config, 0, sizeof(struct ebi_cs_config));
	memset(&sdram_config, 0, sizeof(struct ebi_sdram_config));

	/*
	 * Configure the EBI port with 12 address lines, no address latches or
	 * low pin count, and set it in SDRAM mode with 3-port EBI port.
	 */
	ebi_setup_port(12, 0, 0, EBI_PORT_3PORT | EBI_PORT_SDRAM);

	/*
	 * Configure the EBI chip select for an 8 MB SDRAM located at
	 * \ref BOARD_EBI_SDRAM_BASE.
	 */
	ebi_cs_set_mode(&cs_config, EBI_CS_MODE_SDRAM_gc);
	ebi_cs_set_address_size(&cs_config, EBI_CS_ASPACE_8MB_gc);
	ebi_cs_set_base_address(&cs_config, BOARD_EBI_SDRAM_BASE);

	/* Configure the EBI chip select to be in SDRAM mode. */
	ebi_sdram_set_mode(&cs_config, EBI_CS_SDMODE_NORMAL_gc);

	/* Setup the number of SDRAM rows and columns. */
	ebi_sdram_set_row_bits(&sdram_config, 12);
	ebi_sdram_set_col_bits(&sdram_config, 10);

	/* Further, setup the SDRAM timing. */
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

	/* Write SDRAM configuration into the EBI registers. */
	ebi_sdram_write_config(&sdram_config);
	/* Write the chip select configuration into the EBI registers. */
	ebi_cs_write_config(EBI_SDRAM_CS, &cs_config);

	ebi_enable_cs(EBI_SDRAM_CS, &cs_config);

	do {
		/* Wait for SDRAM to initialize. */
	} while (!ebi_sdram_is_ready());
}

