/**
 * \file
 *
 * \brief Unit test configuration
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
#ifndef CONF_TEST_H
#define CONF_TEST_H

#include <ebi.h>
#include <ebi_port.h>
#include <string.h>
#include <sysclk.h>

//! \note USART0 on PORTD (RX on PD2, TX on PD3)
#define CONF_TEST_USART      &USARTD0
//! \note 38.4 kbaud
#define CONF_TEST_BAUDRATE   38400
//! \note 8-bit character length
#define CONF_TEST_CHARLENGTH USART_CHSIZE_8BIT_gc
//! \note No parity check
#define CONF_TEST_PARITY     USART_PMODE_DISABLED_gc
//! \note No extra stopbit, i.e., use 1
#define CONF_TEST_STOPBITS   false

//! \note The EBI is set up so that external RAM starts at 0x800000.
#define CONF_TEST_RAM_BASE   0x800000UL
//! \note The external SRAM device is a CY7C1019DV-3310VXI of size 128 kB.
#define CONF_TEST_RAM_SIZE   0x20000UL

static void ebi_sram_init(void)
{
	static struct ebi_cs_config cs_config;

	memset(&cs_config, 0, sizeof(cs_config));

	/*
	 * Configure the EBI port with 17 address lines, enable both address
	 * latches, no low pin count mode, and set it in SRAM mode with 3-port
	 * EBI port.
	 */
	ebi_setup_port(17, 2, 0, EBI_PORT_SRAM | EBI_PORT_3PORT
			| EBI_PORT_CS0);

	/*
	 * Configure the EBI chip select for an 128 kB SRAM located at
	 * \ref BOARD_EBI_SDRAM_BASE.
	 */
	ebi_cs_set_mode(&cs_config, EBI_CS_MODE_SRAM_gc);
	ebi_cs_set_address_size(&cs_config, EBI_CS_ASPACE_128KB_gc);
	ebi_cs_set_base_address(&cs_config, CONF_TEST_RAM_BASE);

	/*
	 * Further, setup the SRAM timing. The SRAM device has a 10 ns delay
	 * from address to data ready. The EBI is configured to run at maximum
	 * speed, 64 MHz, which gives a minimum wait state of 16 ns per clock
	 * cycle. 1 clock cycle as wait state gives enough headroom.
	 */
	ebi_cs_set_sram_wait_states(&cs_config, EBI_CS_SRWS_1CLK_gc);

	/* Write the chip select configuration into the EBI registers. */
	ebi_cs_write_config(0, &cs_config);

	ebi_enable_cs(0, &cs_config);
}

#endif
