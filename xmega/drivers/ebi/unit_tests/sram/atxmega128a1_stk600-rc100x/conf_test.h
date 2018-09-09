/**
 * \file
 *
 * \brief Unit test configuration
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
