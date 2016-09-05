/**
 * \file
 *
 * \brief BNO055 extension board example for SAME70.
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

/**
 * \mainpage BNO055 nine Axis sensor Example
 *
 * \section Purpose
 *
 * This example demonstrates how to configure BNO055 nine Axis sensor
 * and check the values of nine axes by it.
 *
 * \section Requirements
 *
 * This package can be used with SAME70 evaluation kits.
 *
 * \section Description
 *
 * This example first set BNO055 working mode and enable interrupt, then initialize it,
 * and last get the sensor values of nine axes if the sensor position is changed.
 *
 * \section Usage
 *
 * -# Build the program and download it into the evaluation board.
 * -# On the computer, open and configure a terminal application
 *     (e.g., HyperTerminal on Microsoft Windows) with these settings:
 *    - 115200 bauds
 *    - 8 bits of data
 *    - No parity
 *    - 1 stop bit
 *    - No flow control
 *  -# In the terminal window, the
 *     following text should appear (values depend on the board and the chip used):
 *     \code
	-- BNO055 Example xxx --
	-- xxxxxx-xx
	-- Compiled: xxx xx xxxx xx:xx:xx --
 *
 * Cailbration
 * Compass, M4G & NDOF_FMC_OFF:
 * Make some random movements (for example: writing the number "8" on air) 
 * until the CALIB_STAT register indicates fully calibrated.
 * 
 * It takes more calibration movements to get the magnetometer calibrated than in the NDOF mode.
 * Step 1:The same random movements have to be made to calibrate the sensor as in the FMC_OFF mode, 
 * but here it takes relatively less calibration movements (and slightly higher current consumption) 
 * to get the magnetometer calibrated.
 * Step 2:The register CALIB_STAT can be read to see the calibration status of the magnetometer.
\endcode
 */

#include "asf.h"
#include "conf_board.h"
#include "conf_bno055.h"
#include "bno055_port.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

#define STRING_EOL    "\r"
#define STRING_HEADER "-- BNO055  Example --\r\n" \
		"-- "BOARD_NAME" --\r\n" \
		"-- Compiled: "__DATE__" "__TIME__" --"STRING_EOL

/*! Holds sleep state of the system */
uint8_t sleep_state;

/*! the flag is set by the external interrupt callback function */
bool read_sensor_data = false;
bool sensor_data_changed = false;
uint32_t systick_count = 0;

/************************************************************************/
/**\name Function Prototype Declaration                                 */
/************************************************************************/

static void sensor_init(void);
static void sensors_data_print(void);
static void bno055_interrupt_handler_no_motion(void);
static void bno055_interrupt_handler_any_motion(void);
static void bno055_interrupt_handler(void);


/* 
* This function calls initialization function of BNO055, sets sensor power mode
* to LOWPOWER, sets sensor operation mode to NDOF and enables the required
* interrupts (Accelerometer Any Motion, Accelerometer No Motion, Gyroscope Any Motion).
*
*/
static void sensor_init(void)
{
	bno055_initialize();
	bno055_set_power_mode(POWER_MODE_LOWPOWER);
	bno055_set_intr_rst(ENABLED);
	
	/* Configure sensor interrupts */
	bno055_set_gyro_any_motion_axis_enable(0, ENABLED);
	bno055_set_gyro_any_motion_axis_enable(1, ENABLED);
	bno055_set_gyro_any_motion_axis_enable(2, ENABLED);
	
	bno055_set_intr_gyro_any_motion(ENABLED);
	bno055_set_intr_mask_gyro_any_motion(ENABLED);
	
	bno055_set_accel_any_motion_no_motion_axis_enable(0, ENABLED);
	bno055_set_accel_any_motion_no_motion_axis_enable(1, ENABLED);
	bno055_set_accel_any_motion_no_motion_axis_enable(2, ENABLED);
	
	bno055_set_accel_any_motion_thres(5);
	bno055_set_accel_any_motion_durn(1);
	bno055_set_intr_accel_any_motion(ENABLED);
	bno055_set_intr_mask_accel_any_motion(ENABLED);
	
	bno055_set_accel_slow_no_motion_thres(5);
	bno055_set_accel_slow_no_motion_durn(1);
	bno055_set_intr_accel_no_motion(ENABLED);
	bno055_set_intr_mask_accel_no_motion(ENABLED);
	
	bno055_set_operation_mode(OPERATION_MODE_NDOF);
}

/*!
* @brief	Reads output data of the sensor and sends them data via USART.
*
* This function reads output data of BNO055 (Three Euler angles) using
* sensor API functions and sends sensor data in ASCII Format via USART.
*
* @param[in]	NULL
*
* @param[out]	NULL
*
* @return		NULL
*
*/
static void sensors_data_print (void)
{
	struct bno055_euler_t		   bno055_euler_data;
	struct bno055_linear_accel_t   bno055_linear_accel_data;
	struct bno055_gravity_t        bno055_gravity_data;
	struct bno055_accel_t          bno055_accel_data;
	struct bno055_gyro_t           bno055_gyro_data;
	struct bno055_mag_t            bno055_mag_data;

	bno055_read_euler_hrp(&bno055_euler_data);
	printf("Euler angles\t\t");
	printf("H= %4d\t\tR= %4d\t\tP= %4d\r\n", bno055_euler_data.h, bno055_euler_data.p, bno055_euler_data.r);
	
	bno055_read_linear_accel_xyz(&bno055_linear_accel_data);
	printf("Linear acceleration\t");
	printf("X= %4d\t\tY= %4d\t\tZ= %4d\r\n", bno055_linear_accel_data.x, bno055_linear_accel_data.y, bno055_linear_accel_data.z);
	
	bno055_read_gravity_xyz(&bno055_gravity_data);
	printf("Gravity\t\t\t");
	printf("X= %4d\t\tY= %4d\t\tZ= %4d\r\n", bno055_gravity_data.x, bno055_gravity_data.y, bno055_gravity_data.z);
	
	bno055_read_accel_xyz(&bno055_accel_data);
	printf("Acceleration\t\t");
	printf("X= %4d\t\tY= %4d\t\tZ= %4d\r\n", bno055_accel_data.x, bno055_accel_data.y, bno055_accel_data.z);
	
	bno055_read_gyro_xyz(&bno055_gyro_data);
	printf("Gyro\t\t\t");
	printf("X= %4d\t\tY= %4d\t\tZ= %4d\r\n", bno055_gyro_data.x, bno055_gyro_data.y, bno055_gyro_data.z);
	
	bno055_read_mag_xyz(&bno055_mag_data);
	printf("Mag\t\t\t");
	printf("X= %4d\t\tY= %4d\t\tZ= %4d\r\n", bno055_mag_data.x, bno055_mag_data.y, bno055_mag_data.z);
}

/*!
* @brief	No motion interrupt handler.
*
* This function is called when a no-motion interrupt is triggered
* by the accelerometer in BNO055,	turns the LED color to green,
* stops data stream and sends a message to the terminal window.
*/
static void bno055_interrupt_handler_no_motion(void)
{
	const uint8_t str_sleeping_message[] = "\r\nShake me to wake me!\r\n\r\n";
	
	ioport_set_pin_level(RGB_LED_G,  RGB_LED_G_ON);
	printf("%s", str_sleeping_message);
}

/*!
* @brief	Any motion interrupt handler.
*
* This function is called when an any-motion interrupt is triggered
* by the accelerometer or gyroscope in BNO055, turns off the green light,
* sends a message to the terminal window and starts data stream.
*/
static void bno055_interrupt_handler_any_motion(void)
{
	const uint8_t str_wakingup_message[] = "\r\nWell done!\r\n\r\n";
	
	printf("%s", str_wakingup_message);
	ioport_set_pin_level(RGB_LED_G,  RGB_LED_G_OFF);
}

/*!
* @brief	Sensor general interrupt handler, calls specific handlers.
*
* This function is called when an external interrupt is triggered by the sensor,
* checks interrupt registers of BNO055 to determine the source and type of interrupt
* and calls the specific interrupt handler accordingly.
*
* @param[in]	NULL
*
* @param[out]	NULL
*
* @return		NULL
*
*/
static void bno055_interrupt_handler(void)
{
	uint8_t accel_no_motion_status = 0;
	uint8_t accel_any_motion_status = 0;
	uint8_t gyro_any_motion_status = 0;
	
	bno055_get_intr_stat_accel_no_motion(&accel_no_motion_status);
	bno055_get_intr_stat_accel_any_motion(&accel_any_motion_status);
	bno055_get_intr_stat_gyro_any_motion(&gyro_any_motion_status);
	
	switch (sleep_state) {
	case SLEEP_STATE_AWAKE:
		if (accel_no_motion_status) {
			sleep_state = SLEEP_STATE_SLEEP;
			bno055_interrupt_handler_no_motion();
			sensor_data_changed = false;
		}
		break;
	case SLEEP_STATE_SLEEP:
		if (accel_any_motion_status || gyro_any_motion_status) {
			sleep_state = SLEEP_STATE_AWAKE;
			bno055_interrupt_handler_any_motion();
			sensor_data_changed = true;
		}
		break;
	default:
		break;
	}
	
	bno055_set_intr_rst(ENABLED);
}

/**
 *  \brief Handler for System Tick interrupt.
 */
void SysTick_Handler(void)
{
	systick_count ++;
	if(systick_count == 500) {
		read_sensor_data = true;
		systick_count = 0;
	}
}

/**
 *  \brief Configure UART console.
 */
static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
#ifdef CONF_UART_CHAR_LENGTH
		.charlength = CONF_UART_CHAR_LENGTH,
#endif
		.paritytype = CONF_UART_PARITY,
#ifdef CONF_UART_STOP_BITS
		.stopbits = CONF_UART_STOP_BITS,
#endif
	};

	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

/************************************************************************/
/* Main Function Definition                                             */
/************************************************************************/
/*!
* @brief	Initializes the whole system and runs the desired application
*
* This is the main function of the project. It calls initialization functions
* of the main board and the sensor. It initializes the host microcontroller unit
* and all its required modules such as clock sources, I2C, TC, USART, PINMUX
* and interrupt controllers. It also initializes the global variables.
*/

int main(void)
{
	/* Initialize the system */
	sysclk_init();
	board_init();

	/* Initialize the console uart */
	configure_console();

	puts(STRING_HEADER);
	
	/* Systick configuration. */
	if (SysTick_Config(sysclk_get_cpu_hz() / 1000)) {
		puts("-F- Systick configuration error\r");
	}
	
	bno055_i2c_bus_init();
	/* (BNO055_RESET, BNO055_BOOT_LOAD, BNO055_I2C_ADDR, RGB_LEG_Green) */
	bno055_gpio_config();
	
	/* (BNO055 external interrupt) */
	extint_initialize(&bno055_interrupt_handler);
	
	/* Initialize BNO055 smart sensor */
	sensor_init();
	
	/* Assigns initial values to the global variable sleep_state */
	sleep_state = SLEEP_STATE_AWAKE;
	
	/* RGB LED Green = OFF */
	ioport_set_pin_level(RGB_LED_G,  RGB_LED_G_OFF);
	
	/* Infinite loop */
	while (true) {
		if(read_sensor_data & sensor_data_changed) {
			/* Read Euler angles from BNO055 and send it via USART */
			sensors_data_print();
			/* Reset the timer's interrupt flag */
			read_sensor_data = false;
		}
		
		/* Check sensor reset button (SW0 button on the main board) */
		if (ioport_get_pin_level(BUTTON_0_PIN) == BUTTON_0_ACTIVE) {
			/* turn LED0 on */
			ioport_set_pin_level(LED_0_PIN, LED_0_ACTIVE);
			/* Reset BNO055 */
			sensor_init();
		} else {
			/* turn LED off if the button is not pressed */
			ioport_set_pin_level(LED_0_PIN, !LED_0_ACTIVE);
		}
	}
}


/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond