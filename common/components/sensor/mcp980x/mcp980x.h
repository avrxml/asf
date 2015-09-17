/**
 * \file
 *
 * \brief MCP980X driver
 *
 * Copyright (c) 2012-2015 Atmel Corporation. All rights reserved.
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
 
#ifndef MCP980X_H_INCLUDED
#define MCP980X_H_INCLUDED

#include "twi_master.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

//! Checking board configuration of the MCP980X Temperature Sensor
#if !defined(BOARD_MCP980X_TWI_INSTANCE)
# warning The MCP980X TWI instance has not been defined. Using default settings.
# define BOARD_MCP980X_TWI_INSTANCE 0 /* TWI instance (TWI0) */
#endif
#if !defined(BOARD_MCP980X_ADDR)
# warning The MCP980X TWI address has not been defined. Using default settings.
# define BOARD_MCP980X_ADDR 0 /* TWI sensor address (0x48u) */
#endif

//! \brief MCP980X Configuration Register Setting
#define MCP980X_CONFIG_ONE_SHOT_ENABLE            (1 << 7)

#define MCP980X_CONFIG_RESOLUTION_9_BIT           (0 << 5)
#define MCP980X_CONFIG_RESOLUTION_10_BIT          (1 << 5)
#define MCP980X_CONFIG_RESOLUTION_11_BIT          (2 << 5)
#define MCP980X_CONFIG_RESOLUTION_12_BIT          (3 << 5)

#define MCP980X_CONFIG_FAULT_QUEUE_1              (0 << 3)
#define MCP980X_CONFIG_FAULT_QUEUE_2              (1 << 3)
#define MCP980X_CONFIG_FAULT_QUEUE_4              (2 << 3)
#define MCP980X_CONFIG_FAULT_QUEUE_6              (3 << 3)

#define MCP980X_CONFIG_ALERT_POLARITY_ACTIVE_HIGH (1 << 2)
#define MCP980X_CONFIG_ALERT_POLARITY_ACTIVE_LOW  (0 << 2)

#define MCP980X_CONFIG_INTERRUPT_MODE             (1 << 1)
#define MCP980X_CONFIG_COMPARATOR_MODE            (0 << 1)

#define MCP980X_CONFIG_SHUTDOWN_ENABLE            (1 << 0)

uint32_t mcp980x_get_temperature(int8_t *p_integer, 
		uint32_t *p_decimal);
uint32_t mcp980x_set_temperature_limit(int8_t c_integer, uint32_t ul_decimal);
uint32_t mcp980x_get_temperature_limit(int8_t *p_integer, uint32_t *p_decimal);
uint32_t mcp980x_set_temperature_hysteresis(int8_t c_integer, 
		uint32_t ul_decimal);
uint32_t mcp980x_get_temperature_hysteresis(int8_t *p_integer,
		uint32_t *p_decimal);
uint32_t mcp980x_set_configuration(uint8_t uc_config);
uint32_t mcp980x_get_configuration(uint8_t *p_config);
uint32_t mcp980x_init(void);
uint32_t mcp980x_enable(void);
uint32_t mcp980x_disable(void);
uint32_t mcp980x_one_shot_mode(void);

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond

/**
 * \page common_component_sensors_mcp980x_quickstart Quickstart guide for MCP980X module
 *
 * This is the quickstart guide for the \ref common_component_sensors_mcp980x_group "MCP980X module",
 * with step-by-step instructions on how to configure and use the drivers in a
 * selection of use cases.
 *
 * The use cases contain several code fragments. The code fragments in the
 * steps for setup can be copied into a custom initialization function, while
 * the steps for usage can be copied into, e.g., the main application function.
 *
 * \section mcp980x_basic_use_case Basic use case
 * In this basic use case, the MCP980X module is configured to:
 * - Disable ONE-SHOT mode
 * - 12-bit temperature sensor accuracy
 * - 2 fault queue cycles
 * - Active-high alert polarity
 * - Alert output in interrupt mode
 * - Disable shutdown mode
 *
 * \section mcp980x_basic_use_case_setup Setup steps
 *
 * \subsection mcp980x_basic_use_case_setup_prereq Prerequisites
 *  - \ref twi_group "Two-Wire Interface service(TWI)"
 *
 * \subsection mcp980x_basic_use_case_setup_code Example code
 * Add this MCP980X initialization code at the beginning of the main function:
 * \code
	    mcp980x_init();

	    mcp980x_set_configuration(
				MCP980X_CONFIG_RESOLUTION_12_BIT          |
				MCP980X_CONFIG_FAULT_QUEUE_2              |
				MCP980X_CONFIG_ALERT_POLARITY_ACTIVE_HIGH |
				MCP980X_CONFIG_INTERRUPT_MODE);

	    mcp980x_set_temperature_limit(TEMP_LIMIT_MAX, TEMP_LIMIT_DEC);

	    mcp980x_set_temperature_hysteresis(TEMP_LIMIT_MIN, TEMP_LIMIT_DEC);

\endcode
 *
 * \subsection pwm_basic_use_case_setup_flow Workflow
 * -# Initialize MCP980X driver:
 *   - \code mcp980x_init(); \endcode
 * -# Set MCP980X configuration:
 *   - \code mcp980x_set_configuration(
	MCP980X_CONFIG_RESOLUTION_12_BIT          |
	MCP980X_CONFIG_FAULT_QUEUE_2              |
	MCP980X_CONFIG_ALERT_POLARITY_ACTIVE_HIGH |
	MCP980X_CONFIG_INTERRUPT_MODE); \endcode
 * -# Set temperature limit:
 *   - \code mcp980x_set_temperature_limit(TEMP_LIMIT_MAX, TEMP_LIMIT_DEC); \endcode
 * -# Set temperature hysteresis:
 *   - \code 
	mcp980x_set_temperature_hysteresis(TEMP_LIMIT_MIN, TEMP_LIMIT_DEC);
\endcode
 *
 * \section mcp980x_basic_use_case_usage Usage steps
 *
 * \subsection mcp980x_basic_use_case_usage_code Example code
 * Add to, e.g., main loop in application C-file:
 * \code
	mcp980x_one_shot_mode();
	mcp980x_get_temperature(&temperature_int, &temperature_dec);
\endcode
 *
 * \subsection mcp980x_basic_use_case_usage_flow Workflow
 * -# Enable One-Shot mode to perform a single temperature measurement:
 *   - \code mcp980x_one_shot_mode(); \endcode
 * -# Retrieve ambient temperature:
 *   - \code mcp980x_get_temperature(&temperature_int, &temperature_dec); \endcode
 *
 */
#endif /* MCP980X_H_INCLUDED */
