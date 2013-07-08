/**
 * \file
 *
 * \brief SAM D20 I2C Master Driver
 *
 * Copyright (C) 2012-2013 Atmel Corporation. All rights reserved.
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

#ifndef I2C_MASTER_H_INCLUDED
#define I2C_MASTER_H_INCLUDED

#include "i2c_common.h"
#include <sercom.h>
#include <pinmux.h>

#if I2C_MASTER_CALLBACK_MODE == true
#  include <sercom_interrupt.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PINMUX_DEFAULT
#  define PINMUX_DEFAULT 0
#endif

/**
 * \addtogroup asfdoc_samd20_sercom_i2c_group
 *
 * @{
 */

#if !defined(__DOXYGEN__)
/**
 * \internal Setting direction bit in address
 */
enum _i2c_transfer_direction {
	_I2C_TRANSFER_WRITE = 0,
	_I2C_TRANSFER_READ = 1,
};
#endif

/** \brief Interrupt flags
 *
 * Flags used when reading or setting interrupt flags.
 */
enum i2c_master_interrupt_flag {
	/** Interrupt flag used for write */
	I2C_MASTER_INTERRUPT_WRITE = 0,
	/** Interrupt flag used for read */
	I2C_MASTER_INTERRUPT_READ  = 1,
};

/**
 * \brief Values for hold time after start bit.
 *
 * Values for the possible I<SUP>2</SUP>C master mode SDA internal hold times after start
 * bit has been sent.
 */
enum i2c_master_start_hold_time {
	/** Internal SDA hold time disabled */
	I2C_MASTER_START_HOLD_TIME_DISABLED = SERCOM_I2CM_CTRLA_SDAHOLD(0),
	/** Internal SDA hold time 50ns-100ns */
	I2C_MASTER_START_HOLD_TIME_50NS_100NS = SERCOM_I2CM_CTRLA_SDAHOLD(1),
	/** Internal SDA hold time 300ns-600ns */
	I2C_MASTER_START_HOLD_TIME_300NS_600NS = SERCOM_I2CM_CTRLA_SDAHOLD(2),
	/** Internal SDA hold time 400ns-800ns */
	I2C_MASTER_START_HOLD_TIME_400NS_800NS = SERCOM_I2CM_CTRLA_SDAHOLD(3),
};

/**
 * \brief I<SUP>2</SUP>C frequencies
 *
 * Values for standard I<SUP>2</SUP>C speeds supported by the module. The driver
 * will also support setting any value between 10 and 100kHz, in which case set
 * the value in the \ref i2c_master_config at desired value divided by 1000.
 *
 * Example: If 10kHz operation is required, give baud_rate in the configuration
 * structure the value 10.
 *
 * \note Max speed is given by GCLK-frequency divided by 10, and lowest is
 *       given by GCLK-frequency divided by 510.
 */
enum i2c_master_baud_rate {
	/** Baud rate at 100kHz */
	I2C_MASTER_BAUD_RATE_100KHZ = 100,
	/** Baud rate at 400kHz */
	I2C_MASTER_BAUD_RATE_400KHZ = 400,
};

#if I2C_MASTER_CALLBACK_MODE == true
/**
 * \brief Callback types
 *
 * The available callback types for the I<SUP>2</SUP>C master module.
 */
enum i2c_master_callback {
	/** Callback for packet write complete */
	I2C_MASTER_CALLBACK_WRITE_COMPLETE = 0,
	/** Callback for packet read complete */
	I2C_MASTER_CALLBACK_READ_COMPLETE  = 1,
	/** Callback for error */
	I2C_MASTER_CALLBACK_ERROR          = 2,
#  if !defined(__DOXYGEN__)
	/** Total number of callbacks */
	_I2C_MASTER_CALLBACK_N             = 3,
#  endif
};

#  if !defined(__DOXYGEN__)
/* Prototype for software module. */
struct i2c_master_module;

typedef void (*i2c_master_callback_t)(
		struct i2c_master_module *const module);
#  endif
#endif

/**
 * \brief SERCOM I<SUP>2</SUP>C Master driver software device instance structure.
 *
 * SERCOM I<SUP>2</SUP>C Master driver software instance structure, used to
 * retain software state information of an associated hardware module instance.
 *
 * \note The fields of this structure should not be altered by the user
 *       application; they are reserved for module-internal use only.
 */
struct i2c_master_module {
#if !defined(__DOXYGEN__)
	/** Hardware instance initialized for the struct */
	Sercom *hw;
	/** Unknown bus state timeout */
	uint16_t unknown_bus_state_timeout;
	/** Buffer write timeout value */
	uint16_t buffer_timeout;
	/** If true, stop condition will be sent after a read/write */
	bool send_stop;
#  if I2C_MASTER_CALLBACK_MODE == true
	/** Pointers to callback functions */
	volatile i2c_master_callback_t callbacks[_I2C_MASTER_CALLBACK_N];
	/** Mask for registered callbacks */
	volatile uint8_t registered_callback;
	/** Mask for enabled callbacks */
	volatile uint8_t enabled_callback;
	/** The total number of bytes to transfer */
	volatile uint16_t buffer_length;
	/**
	 * Counter used for bytes left to send in write and to count number of
	 * obtained bytes in read
	 */
	volatile uint16_t buffer_remaining;
	/** Data buffer for packet write and read */
	volatile uint8_t *buffer;
	/** Save direction of async request. 1 = read, 0 = write */
	volatile uint8_t transfer_direction;
	/** Status for status read back in error callback */
	volatile enum status_code status;
#  endif
#endif
};

/**
 * \brief Configuration structure for the I<SUP>2</SUP>C Master device
 *
 * This is the configuration structure for the I<SUP>2</SUP>C Master device. It
 * is used as an argument for \ref i2c_master_init to provide the desired
 * configurations for the module. The structure should be initialized using the
 * \ref i2c_master_get_config_defaults .
 */
struct i2c_master_config {
	/** Baud rate for I<SUP>2</SUP>C operations */
	enum i2c_master_baud_rate baud_rate;
	/** GCLK generator to use as clock source */
	enum gclk_generator generator_source;
	/** Bus hold time after start signal on data line */
	enum i2c_master_start_hold_time start_hold_time;
	/** Unknown bus state \ref asfdoc_samd20_sercom_i2c_unknown_bus_timeout "timeout" */
	uint16_t unknown_bus_state_timeout;
	/** Timeout for packet write to wait for slave */
	uint16_t buffer_timeout;
	/** Set to keep module active in sleep modes */
	bool run_in_standby;
	/** PAD0 (SDA) pinmux */
	uint32_t pinmux_pad0;
	/** PAD1 (SCL) pinmux */
	uint32_t pinmux_pad1;
};

/**
 * \name Configuration and Initialization
 * @{
 */

#if !defined(__DOXYGEN__)
/**
 * \internal
 * Wait for hardware module to sync
 *
 * \param[in]  module  Pointer to software module structure
 */
static void _i2c_master_wait_for_sync(
		const struct i2c_master_module *const module)
{
	/* Sanity check. */
	Assert(module);
	Assert(module->hw);

	SercomI2cm *const i2c_module = &(module->hw->I2CM);

	while (i2c_module->STATUS.reg & SERCOM_I2CM_STATUS_SYNCBUSY) {
		/* Wait for I2C module to sync. */
	}
}
#endif

/**
 * \brief Returns the synchronization status of the module
 *
 * Returns the synchronization status of the module.
 *
 * \param[in]  module  Pointer to software module structure
 *
 * \return Status of the synchronization.
 * \retval true   Module is busy synchronizing
 * \retval false  Module is not synchronizing
 */
static inline bool i2c_master_is_syncing (
		const struct i2c_master_module *const module)
{
	/* Sanity check. */
	Assert(module);
	Assert(module->hw);

	SercomI2cs *const i2c_hw = &(module->hw->I2CS);

	return (i2c_hw->STATUS.reg & SERCOM_I2CS_STATUS_SYNCBUSY);
}

/**
 * \brief Gets the I<SUP>2</SUP>C master default configurations
 *
 * Use to initialize the configuration structure to known default values.
 *
 * The default configuration is as follows:
 * - Baudrate 100kHz
 * - GCLK generator 0
 * - Do not run in standby
 * - Start bit hold time 300ns-600ns
 * - Buffer timeout = 65535
 * - Unknown bus status timeout = 65535
 * - Do not run in standby
 * - PINMUX_DEFAULT for SERCOM pads
 *
 * \param[out] config  Pointer to configuration structure to be initiated
 */
static inline void i2c_master_get_config_defaults(
		struct i2c_master_config *const config)
{
	/*Sanity check argument. */
	Assert(config);
	config->baud_rate        = I2C_MASTER_BAUD_RATE_100KHZ;
	config->generator_source = GCLK_GENERATOR_0;
	config->run_in_standby   = false;
	config->start_hold_time  = I2C_MASTER_START_HOLD_TIME_300NS_600NS;
	config->buffer_timeout   = 65535;
	config->unknown_bus_state_timeout = 65535;
	config->pinmux_pad0      = PINMUX_DEFAULT;
	config->pinmux_pad1      = PINMUX_DEFAULT;
}

enum status_code i2c_master_init(
		struct i2c_master_module *const module,
		Sercom *const hw,
		const struct i2c_master_config *const config);

/**
 * \brief Enables the I<SUP>2</SUP>C module
 *
 * Enables the requested I<SUP>2</SUP>C module and set the bus state to IDLE
 * after the specified \ref asfdoc_samd20_sercom_i2c_timeout "timeout" period if no
 * stop bit is detected.
 *
 * \param[in]  module  Pointer to the software module struct
 */
static inline void i2c_master_enable(
		const struct i2c_master_module *const module)
{
	/* Sanity check of arguments. */
	Assert(module);
	Assert(module->hw);

	SercomI2cm *const i2c_module = &(module->hw->I2CM);

	/* Timeout counter used to force bus state. */
	uint32_t timeout_counter = 0;

	/* Wait for module to sync. */
	_i2c_master_wait_for_sync(module);

	/* Enable module. */
	i2c_module->CTRLA.reg |= SERCOM_I2CM_CTRLA_ENABLE;

#if I2C_MASTER_CALLBACK_MODE == true
	/* Enable module interrupts */
	system_interrupt_enable(_sercom_get_interrupt_vector(module->hw));
#endif
	/* Start timeout if bus state is unknown. */
	while (!(i2c_module->STATUS.reg & SERCOM_I2CM_STATUS_BUSSTATE(1))) {
		timeout_counter++;
		if(timeout_counter >= (module->unknown_bus_state_timeout)) {
			/* Timeout, force bus state to idle. */
			i2c_module->STATUS.reg = SERCOM_I2CM_STATUS_BUSSTATE(1);
			/* Workaround #1 */
			return;
		}
	}
}

/**
 * \brief Disable the I<SUP>2</SUP>C module
 *
 * Disables the requested I<SUP>2</SUP>C module.
 *
 * \param[in]  module  Pointer to the software module struct
 */
static inline void i2c_master_disable(
		const struct i2c_master_module *const module)
{
	/* Sanity check of arguments. */
	Assert(module);
	Assert(module->hw);

	SercomI2cm *const i2c_module = &(module->hw->I2CM);

	/* Wait for module to sync. */
	_i2c_master_wait_for_sync(module);

	/* Disable module. */
	i2c_module->CTRLA.reg &= ~SERCOM_I2CM_CTRLA_ENABLE;

#if I2C_MASTER_CALLBACK_MODE == true
	/* Disable module interrupts */
	system_interrupt_disable(_sercom_get_interrupt_vector(module->hw));
#endif
}

void i2c_master_reset(struct i2c_master_module *const module);

/** @} */

/**
* \name Read and Write
* @{
*/

enum status_code i2c_master_read_packet_wait(
		struct i2c_master_module *const module,
		struct i2c_packet *const packet);

enum status_code i2c_master_read_packet_wait_no_stop(
		struct i2c_master_module *const module,
		struct i2c_packet *const packet);

enum status_code i2c_master_write_packet_wait(
		struct i2c_master_module *const module,
		struct i2c_packet *const packet);

enum status_code i2c_master_write_packet_wait_no_stop(
		struct i2c_master_module *const module,
		struct i2c_packet *const packet);

void i2c_master_send_stop(struct i2c_master_module *const module);

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* I2C_MASTER_H_INCLUDED */
