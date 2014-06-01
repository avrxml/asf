/**
 * \file
 *
 * \brief SAM SERCOM I2C Master Quick Start Guide with Callbacks
 *
 * Copyright (C) 2012-2014 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
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

#include <asf.h>
#include <system_interrupt.h>

void i2c_write_complete_callback(
		struct i2c_master_module *const module);
void configure_i2c(void);
void configure_i2c_callbacks(void);

//! [packet_data]
#define DATA_LENGTH 8

static uint8_t buffer[DATA_LENGTH] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
};

static uint8_t buffer_reversed[DATA_LENGTH] = {
	0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00
};
//! [packet_data]

//! [address]
#define SLAVE_ADDRESS 0x12
//! [address]

//! [packet_glob]
struct i2c_master_packet packet;
//! [packet_glob]

/* Init software module instance. */
//! [dev_inst]
struct i2c_master_module i2c_master_instance;
//! [dev_inst]

//! [callback_func]
void i2c_write_complete_callback(
		struct i2c_master_module *const module)
{
	/* Send every other packet with reversed data */
	//! [revert_order]
	if (packet.data[0] == 0x00) {
		packet.data = &buffer_reversed[0];
	} else {
		packet.data = &buffer[0];
	}
	//! [revert_order]

	/* Initiate new packet write */
	//! [write_next]
	i2c_master_write_packet_job(module, &packet);
	//! [write_next]
}
//! [callback_func]

//! [initialize_i2c]
void configure_i2c(void)
{
	/* Initialize config structure and software module */
	//! [init_conf]
	struct i2c_master_config config_i2c_master;
	i2c_master_get_config_defaults(&config_i2c_master);
	//! [init_conf]

	/* Change buffer timeout to something longer */
	//! [conf_change]
	config_i2c_master.buffer_timeout = 65535;
	//! [conf_change]

	/* Initialize and enable device with config */
	//! [init_module]
	while(i2c_master_init(&i2c_master_instance, SERCOM2, &config_i2c_master) != STATUS_OK);
	//! [init_module]

	//! [enable_module]
	i2c_master_enable(&i2c_master_instance);
	//! [enable_module]
}
//! [initialize_i2c]

//! [setup_callback]
void configure_i2c_callbacks(void)
{
	/* Register callback function. */
	//! [callback_reg]
	i2c_master_register_callback(&i2c_master_instance, i2c_write_complete_callback,
			I2C_MASTER_CALLBACK_WRITE_COMPLETE);
	//! [callback_reg]
	//! [callback_en]
	i2c_master_enable_callback(&i2c_master_instance,
			I2C_MASTER_CALLBACK_WRITE_COMPLETE);
	//! [callback_en]
}
//! [setup_callback]

int main(void)
{
	system_init();

	//! [run_initialize_i2c]
	/* Configure device and enable. */
	//! [config]
	configure_i2c();
	//! [config]
	/* Configure callbacks and enable. */
	//! [config_callback]
	configure_i2c_callbacks();
	//! [config_callback]
	//! [run_initialize_i2c]

	/* Init i2c packet. */
	//! [packet]
	packet.address     = SLAVE_ADDRESS;
	packet.data_length = DATA_LENGTH;
	packet.data        = buffer;
	//! [packet]

	/* Initiate first packet to be sent to slave. */
	//! [write_packet]
	i2c_master_write_packet_job(&i2c_master_instance, &packet);
	//! [write_packet]

	//! [while]
	while (true) {
		/* Infinite loop */
	}
	//! [while]
}
