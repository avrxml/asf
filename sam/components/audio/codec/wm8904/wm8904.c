/**
 * \file
 *
 * \brief WM8904 driver for SAM
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
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

#include "wm8904.h"
#include "pio.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

/*
 * \brief Write data to WM8904 Register.
 *
 * \param uc_register_address Register address to write
 * \param us_data Data to write.
 */
void wm8904_write_register(uint8_t uc_register_address, uint16_t us_data)
{
	uint8_t uc_temp_data[2];
	twihs_packet_t packet;

	uc_temp_data[0] = (us_data & 0xff00) >> 8;
	uc_temp_data[1] = us_data & 0xff;

	packet.addr[0]     = uc_register_address;
	packet.addr_length = 0x1;
	packet.buffer      = uc_temp_data;
	packet.length      = 0x2;
	packet.chip        = WM8904_SLAVE_ADDRESS;

	twihs_master_write(WM8904_TWIHS, &packet);
}

/*
 * \brief Read data from WM8904 Register.
 *
 * \param uc_register_address Register address to write
 * \retun Register value.
 */
uint16_t wm8904_read_register(uint8_t uc_register_address)
{
	uint8_t uc_temp_data[2];
	twihs_packet_t packet;
	uint16_t us_data;

	packet.addr[0]     = uc_register_address;
	packet.addr_length = 0x1;
	packet.buffer      = uc_temp_data;
	packet.length      = 0x2;
	packet.chip        = WM8904_SLAVE_ADDRESS;

	twihs_master_read(WM8904_TWIHS, &packet);
	us_data = (((uint16_t)uc_temp_data[0] << 8) & 0xff00) | uc_temp_data[1];

	return us_data;
}

/*
 * \brief Initialize TWIHS connect to WM8904.
 *
 * \retun TWIHS_SUCCESS if initialization is complete, error code otherwise.
 */
uint32_t wm8904_twi_init(void)
{
	pio_configure_pin(WM8904_TK_PIO, WM8904_TK_FLAGS);
	pio_configure_pin(WM8904_TF_PIO, WM8904_TF_FLAGS);
	pio_configure_pin(WM8904_TD_PIO, WM8904_TD_FLAGS);
	pio_configure_pin(WM8904_RK_PIO, WM8904_RK_FLAGS);
	pio_configure_pin(WM8904_RF_PIO, WM8904_RF_FLAGS);
	pio_configure_pin(WM8904_RD_PIO, WM8904_RD_FLAGS);
	pio_configure_pin(WM8904_PCK2_PIO, WM8904_PCK2_FLAGS);

	twihs_options_t opt;
	opt.master_clk = sysclk_get_cpu_hz();
	opt.speed = WM8904_TWIHS_CLK;
	return twihs_master_setup(WM8904_TWIHS, &opt);
}

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond

