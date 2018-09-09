/**
 * \file
 *
 * \brief Sensor Tag reference design for SAMB11
 *
 * Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries.
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
#include <asf.h>
#include "platform.h"
#include "at_ble_api.h"
#include "console_serial.h"
#include "timer_hw.h"
#include "ble_manager.h"
#include "ble_utils.h"
#include "bhi160.h"
#include "bhi160/bhy_uc_driver.h"
#include "sensor_tag.h"
#include "BHIfw.h"
#include "bme280/bme280_support.h"

struct uart_module uart_instance;
struct uart_config config_uart;

void configure_i2c_master(void);
void platform_resume_cb(void);

#undef BLE_EVENT_TIMEOUT
#define BLE_EVENT_TIMEOUT 0xFFFFFFFF

//! [initialize_i2c]
void configure_i2c_master(void)
{
	/* Initialize config structure and software module. */
	//! [init_conf]
	struct i2c_master_config config_i2c_master;
	i2c_master_get_config_defaults(&config_i2c_master);
	config_i2c_master.pin_number_pad0 = SENSOR_I2C_PIN_PAD0;
	config_i2c_master.pin_number_pad1 = SENSOR_I2C_PIN_PAD1;
	config_i2c_master.pinmux_sel_pad0 = SENSOR_I2C_MUX_PAD0;
	config_i2c_master.pinmux_sel_pad1 = SENSOR_I2C_MUX_PAD1;
	//! [init_conf]
	/* Initialize and enable device with config, and enable i2c. */
	//! [init_module]
	i2c_master_init(&i2c_master_instance, SENSOR_I2C_MODULE, &config_i2c_master);
	//! [init_module]
	
	//! [enable_module]
	i2c_enable(i2c_master_instance.hw);
	//! [enable_module]
}
void platform_resume_cb(void)
{
	configure_i2c_master();
#ifndef DEBUG_LOG_DISABLED
	serial_console_init();
#endif	
}

int main(void)
{
	platform_driver_init();
	acquire_sleep_lock();
	system_board_init();
	
#if defined DEBUG_LED
    LED_On(LED_0_PIN);
#endif
#ifndef DEBUG_LOG_DISABLED
	/* Initialize serial console */
	serial_console_init();
#endif
	//! [config]
	configure_i2c_master();
	//! [config]
	bhy_driver_init(_bhi_fw, _bhi_fw_len);
	DBG_LOG("\r\nBHI driver Init Done!");
	bhy_delay_msec(300);
	//Initialize bme280
	bme280_sensor_init();
	//Initialize veml6080
	veml6080_init();		
	DBG_LOG("Initializing BLE Application");
 	/* initialize the BLE chip  and Set the Device Address */
 	ble_device_init(NULL);
 	/* Register Primary/Included service in case of GATT Server */
 	wbe_profile_init();
    /* Register callback */
	register_resume_callback(platform_resume_cb);
	ble_set_ulp_mode(BLE_ULP_MODE_CLEAR);
	
	while (1)
	{
	   	ble_event_task(BLE_EVENT_TIMEOUT);
	}
}
