/**
 * \file
 *
 * \brief PARC driver for SAM.
 *
 * Copyright (C) 2013-2014 Atmel Corporation. All rights reserved.
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

#ifndef PARC_H_INCLUDED
#define PARC_H_INCLUDED

#include "compiler.h"
#include "status_codes.h"
/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond


/**
 * \defgroup group_sam_drivers_parc PARC - Parallel Capture
 *
 * This driver provides a unified interface for the configuration and
 * management of PARC (Parallel Capture) module.
 *
 * The Parallel Capture peripheral samples an external 8-bit bus with
 * an external input clock. It can be connected to a CMOS digital
 * image sensor, an ADC, a DSP synchronous port etc..
 *
 * \{
 */

#if PARC_CALLBACK_MODE == true
extern struct parc_module *parc_module_instance;
struct parc_module;
typedef void (*parc_callback_t)(const struct parc_module *const module_inst);
#endif

/**
 * \brief Sampling Edge
 *
 *  Enum for possible sampling edges for PARC.
 *
 */
enum parc_sampling_edge {
	/** Data capture occurs on rising edge of PCCK */
	PARC_RISING_EDGE = 0,
	/** Data capture occurs on falling edge of PCCK */
	PARC_FALLING_EDGE = 1,
};


/**
 * \brief Capture mode
 *
 * Enum for possible captured byte selection.
 *
 */
enum parc_capture_mode {
	/* Bytes are captured when data captured every cycle*/
	PARC_BOTH_CAPTURE = 0,
	/** Even bytes are captured when data captured every two cycles */
	PARC_EVEN_CAPTURE,
	/** Odd bytes are captured when data captured every two cycles */
	PARC_ODD_CAPTURE,
};

/**
 * \brief Sample mode
 *
 * Enum for possible SMODE (required EN pins) for PARC
 *
 */
enum parc_smode {
	/** Data capture occurs with PCEN1 high */
	PARC_SMODE_PCEN1_H = 0,
	/** Data capture occurs with PCEN1 and PCEN2 high */
	PARC_SMODE_PCEN1_AND_PCEN2_H,
	/** Data capture occurs with PCEN1 or PCEN2 high */
	PARC_SMODE_PCEN1_OR_PCEN2_H,
	/** Data capture always occurs */
	PARC_SMODE_ALWAYS,
};

/**
 * \brief Data Size
 *
 * Enum for the possible transferred data size for PARC
 *
 */
enum parc_data_size {
	/** Data size is 8bits */
	PARC_DSIZE_BYTE = 0,
	/** Data size is 16bits */
	PARC_DSIZE_HALFWORD = 1,
	/** Data size is 32bits */
	PARC_DSIZE_WORD = 2,
};

/**
 * \brief Status enum
 *
 * Enum for status bit for PARC.
 *
 */
enum parc_status {
	PARC_STATUS_EN = PARC_SR_EN,
	PARC_STATUS_CS = PARC_SR_CS,
	PARC_STATUS_DRDY = PARC_SR_DRDY,
	PARC_STATUS_OVR = PARC_SR_OVR,
};

#if PARC_CALLBACK_MODE == true
/**
 * \brief interrupt type enum
 *
 * Enum for interrupt source for PARC.
 *
 */
enum parc_interrupt_source {
	PARC_INTERRUPT_DRDY = PARC_SR_DRDY,
	PARC_INTERRUPT_OVR = PARC_SR_OVR,
};

/**
 * \brief callback type enum
 */
enum parc_callback_type {
	/* Data ready */
	PARC_CALLBACK_DATA_READY = 0,
	/* Data overrun */
	PARC_CALLBACK_OVERRUN,
	/* Callback count*/
	PARC_CALLBACK_N,
};
#endif

/**
 * \brief PARC configuration structure
 *
 * Configuration structure for an PARC instance. This structure should be
 * initialized by the \ref parc_get_config_defaults()
 * function before being modified by the user application.
 */
struct parc_config {
	/*
	 * Capture mode: every byte, or odd/even indexd bytes are captured.
	 */
	enum parc_capture_mode capture_mode;

	/*
	 * Sampling edge select: capture occurs on rising edge or falling
	 * edge of PCCK.
	 */
	enum parc_sampling_edge sampling_edge;

	/*
	 * Sampling mode: data capture can occur under 4 types of sampling
	 * modes:
	 * PCEN1 is high, PCEN1 and PCEN2 are high, PCEN1 or PCEN2 is high,
	 * always capture data.
	 */
	enum parc_smode smode;

	/*
	 * Data size: effective data size in RHR register. Captured data can be
	 * concatenated to make 16-bit or 32-bit in RHR
	 */
	enum parc_data_size dsize;
};

/**
 * \brief PARC driver software instance structure.
 *
 * Device instance structure for a PARC driver instance. This
 * structure should be initialized by the \ref parc_init() function to
 * associate the instance with a particular hardware module of the device.
 */
struct parc_module {
	/** Base address of the PARC module. */
	Parc *hw;
	/** Pointer to PARC configuration structure. */
	struct parc_config *parc_cfg;

#if PARC_CALLBACK_MODE == true
	/** Array to store callback functions */
	parc_callback_t callback[PARC_CALLBACK_N];
	/** Bit mask for callbacks registered */
	uint8_t registered_callback_mask;
	/** Bit mask for callbacks enabled */
	uint8_t enabled_callback_mask;
#endif
};

enum status_code parc_init(
	struct parc_module *const module_inst,
	Parc *const hw,
	struct parc_config *const config);

enum status_code parc_set_config(
	struct parc_module *const module_inst,
	struct parc_config *config);

/**
 * \brief Initializes an PARC configuration structure to defaults
 *
 * Initializes a given PARC configuration struct to a set of known default
 * values. This function should be called on any new instance of the
 * configuration struct before being modified by the user application.
 *
 * The default configurations are:
 * - Capture every btye
 * - Sample the data bus on the rising edge of the PCCK input clock
 * - Sample data regardless of the levels of PCEN1 and PCEN2
 * - 8-bit data width in the Rececive Holding Register
 *
 * \param[in] config  Pointer to configuration struct to initialize to
 *                     default values
 */
static inline void parc_get_config_defaults(struct parc_config *const config)
{
	config->capture_mode = PARC_BOTH_CAPTURE;
	config->sampling_edge = PARC_RISING_EDGE;
	config->smode = PARC_SMODE_ALWAYS;
	config->dsize = PARC_DSIZE_BYTE;
}

/**
 * \brief Enables the PARC module
 *
 * Enables an PARC module that has previously been configured.
 *
 * \param[in] module_inst  Pointer to the PARC software instance struct
 *
 * \return Status of the enable procedure
 * \retval STATUS_OK       The enable procedure was successful
 */
static inline enum status_code parc_enable(
		struct parc_module *const module_inst)
{
	Assert(module_inst);
	Assert(module_inst->hw);

	Parc *const parc_module_hw = module_inst->hw;
	parc_module_hw->PARC_CR = PARC_CR_EN;

#if PARC_CALLBACK_MODE == true
	/* enable interrupt*/
	NVIC_EnableIRQ(PARC_IRQn);
#endif
	return STATUS_OK;
}

/**
 * \brief Disables the PARC module
 *
 * Disables an PARC module that has previously been configured.
 *
 * \param[in] module_inst  Pointer to the PARC software instance struct
 *
 * \return Status of the enable procedure
 * \retval STATUS_OK       The disable procedure was successful
 */
static inline enum status_code parc_disable(
		struct parc_module *const module_inst)
{
	Assert(module_inst);
	Assert(module_inst->hw);

	Parc *const parc_module_hw = module_inst->hw;
	parc_module_hw->PARC_CR = PARC_CR_DIS;

#if PARC_CALLBACK_MODE == true
	/* disable interrupt*/
	NVIC_DisableIRQ(PARC_IRQn);
#endif
	return STATUS_OK;
}

/**
 * \brief Start an PARC conversion
 *
 * Start PARC capture.
 *
 * \param[in] module_inst  Pointer to the PARC software instance struct
 */
static inline void parc_start_capture(
		struct parc_module *const module_inst)
{
	Assert(module_inst);
	Assert(module_inst->hw);

	Parc *const parc_module_hw = module_inst->hw;

	parc_module_hw->PARC_CR = PARC_CR_START;
}

/**
 * \brief Stop an PARC conversion
 *
 * Stop PARC capture.
 *
 * \param[in] module_inst  Pointer to the PARC software instance struct
 */
static inline void parc_stop_capture(
		struct parc_module *const module_inst)
{
	Assert(module_inst);
	Assert(module_inst->hw);

	Parc *const parc_module_hw = module_inst->hw;

	parc_module_hw->PARC_CR = PARC_CR_STOP;
}

/**
 * \brief Retrieves the current module status.
 *
 * Retrieves the status of the module, giving overall state information.
 *
 * \param[in] module_inst  Pointer to the PARC software instance struct
 *
 * \return Bitmask of PARC_STATUS_* flags in parc_status enum
 *
 * \retval PARC_STATUS_EN           PARC is enabled
 * \retval PARC_STATUS_CS           PARC capture status
 * \retval PARC_STATUS_DRDY         PARC data ready
 * \retval PARC_STATUS_OVR          PARC overrun
 */
static inline uint32_t parc_get_status(struct parc_module *const module_inst)
{
	Parc *const parc_module_hw = module_inst->hw;
	return parc_module_hw->PARC_SR;
}

/**
 * \brief Check the data ready status of PARC
 *
 * \param[in] module_inst Pointer to the PARC software instance struct
 *
 * \return 1 if captured data is ready, else it will return 0.
 */
static inline bool parc_is_data_ready(struct parc_module *const module_inst)
{
	Parc *const parc_module_hw = module_inst->hw;
	return ((parc_module_hw->PARC_SR & PARC_SR_DRDY) != 0);
}

/**
 * \brief Clear status flag
 *
 * Clear the given interrupt-related status flag from the PARC module.
 *
 * \param[in] module_inst Pointer to the PARC software instance struct
 * \param[in] status      Enum to interrupt-related PARC status
 */
static inline enum status_code parc_clear_status(
	struct parc_module *const module_inst, const uint32_t status)
{
	/* Sanity check arguments*/
	Assert(module_inst);
	Assert(module_inst->hw);

	Parc *const parc_module_hw = module_inst->hw;

	uint32_t int_flags = 0;

	/*check for PARC data ready*/
	if(status & PARC_STATUS_DRDY){
		int_flags |= PARC_ICR_DRDY;
	}
	/*check for PARC overrun*/
	if(status & PARC_STATUS_OVR){
		int_flags |= PARC_ICR_OVR;
	}

	/*clear interrupt flags*/
	parc_module_hw->PARC_ICR = int_flags;

	return STATUS_OK;
}

/**
 * \brief Reads the PARC result
 *
 * Reads the result from an PARC conversion that was previously started.
 *
 * \param[in]  module_inst  Pointer to the PARC software instance struct
 * \param[out] data         Pointer to store the captured data in. Captured
 *                          data size is configured through parc_config.
 *
 * \return Status of the PARC read request.
 * \retval STATUS_OK    The result was retrieved successfully
 * \retval STATUS_ERR_BUSY  A captured data was not ready
 */
static inline enum status_code parc_read(struct parc_module *const module_inst,
		uint32_t *data)
{
	Assert(module_inst);
	Assert(module_inst->hw);
	Assert(data);

	if(!(parc_get_status(module_inst) & PARC_SR_DRDY)){
		/* Captured data not ready*/
		return STATUS_ERR_BUSY;
	}

	Parc *const parc_module_hw = module_inst->hw;
	*data = parc_module_hw->PARC_RHR;

	parc_clear_status(module_inst, PARC_STATUS_DRDY);
	return STATUS_OK;
}

#if PARC_CALLBACK_MODE == true
/**
 * \brief Enable interrupt
 *
 * Enable the given interrupt request from the PARC module.
 *
 * \param[in] module_inst Pointer to the PARC software instance struct
 * \param[in] source Interrupt source to enable
 */
static inline void parc_enable_interrupts(struct parc_module *const module_inst,
	enum parc_interrupt_source source)
{
	/* Sanity check arguments*/
	Assert(module_inst);
	Assert(module_inst->hw);

	Parc *const parc_module_hw = module_inst->hw;

	parc_module_hw->PARC_IER = source;
}

/**
 * \brief Disable interrupt
 *
 * Disable the given interrupt request from the PARC module.
 *
 * \param[in] module_inst Pointer to the PARC software instance struct
 * \param[in] source Interrupt source to disable
 */
static inline void parc_disable_interrupts(struct parc_module *const module_inst,
	enum parc_interrupt_source source)
{
	/* Sanity check arguments*/
	Assert(module_inst);
	Assert(module_inst->hw);

	Parc *const parc_module_hw = module_inst->hw;

	parc_module_hw->PARC_IDR = source;
}

/**
 * \brief This function return the PARC interrupts mask value.
 *
 * \param module_inst      Pointer to the PARC software instance struct
 *
 * \return Interrupt mask value
 */
static inline uint32_t parc_read_interrupt_mask(
	struct parc_module *const module_inst)
{
	Parc *const parc_module_hw = module_inst->hw;
	return parc_module_hw->PARC_IMR;
}
#endif

/**
 * \brief Enables the PARC events mode
 *
 * Enables event mode on the PARC which has previously been configured.
 *
 * \param[in] module_inst  Pointer to the PARC software instance struct
 *
 * \return Status of the enable procedure
 * \retval STATUS_OK       The enable procedure was successful
 */
static inline enum status_code parc_enable_events(
	struct parc_module *const module_inst)
{
	Parc *const parc_module_hw = module_inst->hw;
	parc_module_hw->PARC_CFG |= PARC_CFG_EMODE;

	return STATUS_OK;
}

/**
 * \brief Disables the PARC events mode
 *
 * Disables event mode on the PARC which has previously been configured.
 *
 * \param[in] module_inst  Pointer to the PARC software instance struct
 *
 * \return Status of the enable procedure
 * \retval STATUS_OK       The enable procedure was successful
 */
static inline enum status_code parc_disable_events(
	struct parc_module *const module_inst)
{
	Parc *const parc_module_hw = module_inst->hw;
	parc_module_hw->PARC_CFG &= ~(PARC_CFG_EMODE);

	return STATUS_OK;
}

/**
 * \brief This function return the PARC module version.
 *
 * \param module_inst        Pointer to the PARC software instance struct
 *
 * \return PARC module version
 */
static inline uint32_t parc_get_version(struct parc_module *const module_inst)
{
	Parc *const parc_module_hw = module_inst->hw;
	return parc_module_hw->PARC_VERSION;
}

/**
 * \}
 */

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond

/**
 * \page sam_parc_quick_start Quick Start Guide for the PARC driver
 *
 * This is the quick start guide for the \ref group_sam_drivers_parc, with
 * step-by-step instructions on how to configure and use the driver for
 * a specific use case.
 *
 * The use cases contain several code fragments. The code fragments in the
 * steps for setup can be copied into a custom initialization function, while
 * the steps for usage can be copied into, e.g., the main application function.
 *
 * \section parc_qs_use_cases Use Cases
 * - \ref parc_basic_use_case
 *
 * \section parc_basic_use_case PARC Basic Use Case
 *
 * This use case will demonstrate how to use the PARC module
 * on SAM4L. In this use case, PARC is configured as:
 * - Capture every byte
 * - Sample the data bus on the rising edge of the PCCK input clock
 * - Sample data regardless of the levels of PCEN1 and PCEN2
 * - 8-bit data width in the Rececive Holding Register
 *
 * \section parc_basic_setup Setup Steps
 *
 * \subsection parc_basic_prereq Prerequisites
 *
 * This module requires the following service
 * - \ref clk_group
 *
 * \subsection parc_basic_setup_code Setup Code Example
 *
 * Add this to the main loop or a setup function:
 * \code
	   struct parc_module   module_inst;
	   struct parc_config   config;

	   // Get default configuration
	   parc_get_config_defaults(&config);
	   // Initialize PARC
	   parc_init(&module_inst, PARC, &config);

	   // Enable the PARC
	   parc_enable(&module_inst);
	   // Start capture
	   parc_start_capture(&module_inst);

\endcode
 *
 * \subsection parc_basic_setup_workflow Basic Setup Workflow
 *
 * -# Initialize and configure PARC,
 *  see sam/drivers/parc/example for detail.
 *  \code
	parc_get_config_defaults(&config);
	- \note the config can be modified here from the default parameters.
	parc_init(&module_inst, PARC, &config);
\endcode
 * -# Enable PARC and start capture
 *  \code
	parc_enable(&module_inst);
	parc_start_capture(&module_inst);
\endcode
 *
 * \section parc_basic_usage PARC Basic Usage
 *
 * \subsection parc_basic_usage_code
 *
 * We can poll the data status then read it once data capture finishes.
 * \code
	uint32_t captured_data;
	while(parc_is_data_ready(&module_inst) == false);
	parc_read(&module_inst, &captured_data);
\endcode
 *
 * We can enable interrupt of data ready and link callback function to perform
 * customer function.
 * \code
	parc_register_callback(&module_inst,
	(parc_callback_t)parc_complete_callback, PARC_CALLBACK_DATA_READY);
	parc_enable_interrupts(&module_inst, PARC_INTERRUPT_DRDY);
	parc_enable_callback(&module_inst,PARC_CALLBACK_DATA_READY);
	parc_start_capture(&module_inst);
	//the callback function example.
	static void parc_complete_callback(struct parc_module *const module)
	{
	  callback_data_ready = true;
	  parc_read(module, &captured_data);
	}
\endcode
 */
#endif  /* PARC_H_INCLUDED */

