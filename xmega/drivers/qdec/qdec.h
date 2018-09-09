/**
 * \file
 *
 * \brief AVR XMEGA Quadrature Decoder driver header file
 *        This file contains the function prototypes and Macros.
 *
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
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

#ifndef QDEC_H
#define QDEC_H

#include "compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup qdec_group XMEGA Quadrature Decoder (QDEC)
 *
 * This is a driver for the AVR XMEGA Quadrature Decoder module (QDEC).
 * It provides functions for enabling, disabling and configuring the QDEC
 * module.
 *
 * See \ref xmega_qdec_quickstart
 * @{
 */

/*! Recognition states of index pin */
enum QDec_index_rec_state {
	QDEC_STATE_NO_PH0_NO_PH90 = (0x00 << 0),
	QDEC_STATE_NO_PH0_PH90    = (0x01 << 0),
	QDEC_STATE_PH0_NO_PH90    = (0x02 << 0),
	QDEC_STATE_PH0_PH90       = (0x03 << 0),
};

/*! Quadrature encoder configuration structure */
typedef struct qdec_config {
	PORT_t *port;
	uint8_t pins_base;
	uint16_t pins_filter_us;
	struct {
		bool pins_invert;
	} phases;
	struct {
		bool enabled;
		bool pin_invert;
		enum QDec_index_rec_state rec_state;
	} index;
	bool rotary;
	uint8_t event_channel;
	volatile void *timer;
	uint16_t revolution;
	struct {
		bool enabled;
		uint8_t event_channel;
		volatile void *timer;
		uint32_t unit;
		uint32_t coef;
		uint16_t last_freq;
	} freq_opt;
} qdec_config_t;

/**
 * \brief This function initializes the configuration structure with
 * the identified as commonly use case.
 *
 * \param config        Handle on a QDec configuration structure
 */
void qdec_get_config_defaults(qdec_config_t *config);

/**
 * \brief This function set up the needed configuration to the port,
 *        used for the quadrature decoding.
 *
 * \param config      Handle on a QDec configuration structure
 * \param port        Pointer to PORT module, which is used for quadrature
 * \param pins_base   First pin number to be used for QDPH0 input from port
 *                    pins_base+1 for QDPH90 and pins_base+2 for QDINDX
 * \param pins_invert True if IO pins should be inverted
 * \param filter_us   Pin stable timing requested to validate level
 */
void qdec_config_phase_pins(qdec_config_t *config, PORT_t *port,
		uint8_t pins_base, bool pins_invert, uint16_t filter_us);

/**
 * \brief Defines index line to enable the automaticaly position reset
 *
 * \param config      Handle on a QDec configuration structure
 * \param pin_invert  True if IO pins should be inverted
 * \param state       Recognition states of index pin
 */
void qdec_config_enable_index_pin(qdec_config_t *config, bool pin_invert,
		enum QDec_index_rec_state state);

/**
 * \brief Disables index line
 *
 * \param config      Handle on a QDec configuration structure
 */
void qdec_config_disable_index_pin(qdec_config_t *config);

#if XMEGA_E

/**
 * \brief Enables rotary feature
 *
 * \param config      Handle on a QDec configuration structure
 */
void qdec_config_enable_rotary(qdec_config_t *config);

/**
 * \brief Disables rotary feature
 *
 * \param config      Handle on a QDec configuration structure
 */
void qdec_config_disable_rotary(qdec_config_t *config);

#endif

/**
 * \brief Selects the event channel to use for quadrature decoder
 *
 * \param config        Handle on a QDec configuration structure
 * \param event_channel Which event channel to use for quadrature decoder
 *                      XMEGA B, E authorizes channel 0.
 *                      XMEGA D authorizes channel 0, 2.
 *                      XMEGA A, C authorizes channel 0, 2, 4.
 */
void qdec_config_event_channel(qdec_config_t *config, uint8_t event_channel);

/**
 * \brief Selects the event channel to use for quadrature decoder
 *
 * \param config        Handle on a QDec configuration structure
 * \param timer         Pointer to timer module used for Quadrature decoding
 *                      XMEGA E authorizes &TC4 or &TC5.
 *                      other XMEGA authorizes &TC0, ...
 */
void qdec_config_tc(qdec_config_t *config, volatile void *timer);

/**
 * \brief Defines frequency option
 *
 * This option allows to compute automaticaly through an event and timer
 * the frequency rotation.
 * The system will be configurated with a resolution at least as high
 * as the one specified.
 * The resolution of a TC is synonymous with its clock frequency.
 *
 * \param config        Handle on a QDec configuration structure
 * \param unit          Desired unit used by qdec_get_frequency() in mHz.
 */
void qdec_config_enable_freq(qdec_config_t *config, uint32_t unit);

/**
 * \brief Selects the event channel to use for frequency quadrature decoder
 *
 * \param config        Handle on a QDec configuration structure
 * \param event_channel Which event channel to use for frequency QDec
 *                      XMEGA B, E authorizes channel 0.
 *                      XMEGA D authorizes channel 0, 2.
 *                      XMEGA A, C authorizes channel 0, 2, 4.
 * Note: If
 */
void qdec_config_freq_event_channel(qdec_config_t *config,
		uint8_t event_channel);

/**
 * \brief Selects the event channel to use for frequency quadrature decoder
 *
 * \param config        Handle on a QDec configuration structure
 * \param timer         Pointer to timer module used for frequency QDec
 *                      XMEGA E authorizes &TC4 or &TC5.
 *                      other XMEGA authorizes &TC0, ...
 */
void qdec_config_freq_tc(qdec_config_t *config, volatile void *timer);

/**
 * \brief Defines the revolution of quadrature decoder
 *
 * \param config        Handle on a QDec configuration structure
 * \param revolution    Number of position for a revolution
 */
void qdec_config_revolution(qdec_config_t *config, uint16_t revolution);

/**
 * \brief Loads and enables QDec system
 *
 * \param config      Handle on a QDec configuration structure
 */
void qdec_enabled(qdec_config_t *config);

/**
 * \brief Returns the position of quadrature encoder
 *
 * \param config      Handle on a QDec configuration structure
 */
void qdec_reset_position(qdec_config_t *config);

/**
 * \brief Returns the position of quadrature encoder
 *
 * \param config      Handle on a QDec configuration structure
 *
 * \return position
 *
 * Note: The origin position is reseted:
 * - at startup
 * - through \ref qdec_reset_position()
 * - when the index line is detect if it is enabled
 */
uint16_t qdec_get_position(qdec_config_t *config);

/**
 * \brief Returns the direction of the quadrature decoding counter
 *
 * \param config      Handle on a QDec configuration structure
 *
 * \return if true, counter increment
 */
bool qdec_get_direction(qdec_config_t *config);

/**
 * \brief Returns the frequency of quadrature encoder
 *
 * \param config      Handle on a QDec configuration structure
 *
 * \return frequency, the unit is defined by qdec_config_enable_freq()
 */
uint16_t qdec_get_frequency(qdec_config_t *config);

/** @} */

#ifdef __cplusplus
}
#endif

/**
 * \page xmega_qdec_quickstart Quick Start Guide for the XMEGA QDEC Driver
 *
 * This is the quick start guide for the \ref qdec_group , with step-by-step
 * instructions on how to configure and use the driver in a selection of use
 * cases.
 *
 * The use cases are described with "setup" and "usage" sections, which each
 * have "example code" and "workflow" subsections. This documentation first
 * presents code fragments and function definitions along with instructions on
 * where they can be placed, e.g., into the application C-file or the main()
 * function, then follows up with explanations for all the lines of code.
 *
 * \section qdec_basic_use_case Basic use case
 *
 * In this basic use case, QDEC is configured for:
 * - a common quadrature encoder with two lines (phase 0 / phase 90)
 * - no index line on this
 *
 * \section qdec_basic_use_case_setup Setup steps
 *
 * \subsection qdec_basic_use_case_setup_code Example code
 *
 * Add to application C-file:
 * \code

	 static void qdec_init(void)
	 {
	     qdec_config_t config;

	     qdec_get_config_defaults(&config);
	     qdec_config_phase_pins(&config, &PORTA, 6, false, 500);
	     qdec_config_revolution(&config, 20);
	     qdec_enabled(&config);
	 }
\endcode
 *
 * Add to \c main():
 * \code
	sysclk_init();
	qdec_init();
\endcode
 *
 * \subsection qdec_basic_use_case_setup_flow Workflow
 *
 * -# Initialize the clock system:
 *      - \code sysclk_init(); \endcode
 *      \note The QDEC driver requires the system clock driver to be
 * initialized in order to compute the correct QDEC filters.
 * -# Create a function \c qdec_init() to intialize the QDEC:
 *     - \code
	static void qdec_init(void)
	{
	    // ...
	}
\endcode
 * -# Allocate configuration structs for the QDEC:
 *     - \code
	qdec_config_t config;
\endcode
 * -# Initialize the configuration structure with the identified as commonly
 * hardware ressources and parameters:
 *     - \code
	qdec_get_config_defaults(&config);
\endcode
 * -# Define the PORT and pins used by QDec. These must be contiguous on PORT
 * and only the first pin phase is request. A filter timing must be given
 * to filter the rebounces:
 *     - \code
	qdec_config_phase_pins(&config, &PORTA, 6, false, 500);
\endcode
 * -# Define the QDec revolution. It is the number of position for one
 * revolution:
 *     - \code
	qdec_config_revolution(&config, 20);
\endcode
 * -# Load and enable the QDec configuration:
 *     - \code
	qdec_enabled(&config);
\endcode
 *
 * \section qdec_basic_use_case_usage Usage steps
 *
 * \subsection qdec_basic_use_case_usage_code Example code
 *
 * Add to, e.g., main-loop in application C-file:
 * \code
	uint16_t qdec_position = qdec_get_position(&config);
	bool qdec_dir = qdec_get_direction(&config);
\endcode
 *
 * \subsection qdec_basic_use_case_usage_flow Workflow
 *
 * -# Read the current QDec position:
 *     - \code qdec_get_position(&config); \endcode
 * -# Read the current QDec direction:
 *     - \code qdec_get_direction(&config); \endcode
 */

#endif /* QDEC_H */
