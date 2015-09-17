/**
 * \file
 *
 * \brief SAM D21 serial driver configuration.
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

#ifndef CONF_SERIALDRV_H_INCLUDED
#define CONF_SERIALDRV_H_INCLUDED

void serial_rx_callback(void);
void serial_tx_callback(void);

#define SERIAL_DRV_RX_CB serial_rx_callback
#define SERIAL_DRV_TX_CB serial_tx_callback
#define SERIAL_DRV_TX_CB_ENABLE  true
#define SERIAL_DRV_RX_CB_ENABLE  true


static inline void ble_enable_pin_init(void)
{
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);

	/* Configure LEDs as outputs, turn them off */
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(PIN_PB07, &pin_conf);
	port_pin_set_output_level(PIN_PB07, true);
	port_pin_set_config(PIN_PB05, &pin_conf);
	port_pin_set_output_level(PIN_PB05, true);
}

static inline void ble_enable_pin_set_low(void)
{
	//port_pin_set_output_level(PIN_PB07, false);
}

static inline void ble_enable_pin_set_high(void)
{
	//port_pin_set_output_level(PIN_PB07, true);
}


#endif /* CONF_SERIALDRV_H_INCLUDED */
