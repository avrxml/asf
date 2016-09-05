/**
 * @file tfa.h
 *
 * @brief This file is the interface for Transceiver Feature Access (TFA)
 * functionality.
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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
 */

/*
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* Prevent double inclusion */
#ifndef TFA_H
#define TFA_H

#if (defined ENABLE_TFA) || (defined TFA_BAT_MON) || (defined __DOXYGEN__) || \
	(defined TFA_BAT_MON_READ) || (defined TFA_BAT_MON_IRQ) || \
	(defined TFA_CW) || (defined TFA_CCA)

/**
 * \defgroup group_tfa           Transceiver Feature Access
 * The Atmel transceivers provide a variety of additional hardware features
 * features that are not reflected in the IEEE 802.15.4 standard.
 * These features are for eg .Reading battery voltage, Continuous wave
 * transmission etc .
 * @{
 */

/* === INCLUDES ============================================================ */

#include "return_val.h"
#include "ieee_const.h"
#include "tal_types.h"

/* === TYPES =============================================================== */

/** Transceiver commands */
typedef enum tfa_pib_t {
	TFA_PIB_RX_SENS         = 0
} SHORTENUM tfa_pib_t;

/** Continuous Transmission modes */
/* RF212B has a 2nd CW mode, described as "Additional CW Mode" in the datasheet
 **/
typedef enum continuous_tx_mode_tag {
	CW_MODE = 0,
	PRBS_MODE = 1
#if (TAL_TYPE == AT86RF212B)
	,
	CW_MODE_2 = 2,
	CW_ALL_ZEROS_MODE = 3
#endif /* (TAL_TYPE == AT86RF212B) */
} SHORTENUM continuous_tx_mode_t;

/* === MACROS ============================================================== */

/**
 * Default value of TFA PIB attribute to reduce the Rx sensitivity.
 * By default, reduced sensitivity is disabled.
 */
#if (TAL_TYPE == AT86RF212)
#define TFA_PIB_RX_SENS_DEF                 (RSSI_BASE_VAL_BPSK_40_DBM)
#elif (TAL_TYPE == AT86RF212B)
#define TFA_PIB_RX_SENS_DEF                 (RSSI_BASE_VAL_BPSK_600_DBM)
#else
#define TFA_PIB_RX_SENS_DEF                 (RSSI_BASE_VAL_DBM)
#endif

/**
 * Supply voltage above upper limit.
 */
#define SUPPLY_VOLTAGE_ABOVE_UPPER_LIMIT    (0xFFFF)

/**
 * Supply voltage below lower limit.
 */
#define SUPPLY_VOLTAGE_BELOW_LOWER_LIMIT    (0)

/* === GLOBALS ============================================================= */

/* === PROTOTYPES ========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#if (defined ENABLE_TFA) || (defined __DOXYGEN__)

/**
 * @brief Initializes the TFA
 *
 * This function is called to initialize the TFA.
 *
 * @return MAC_SUCCESS if everything went correct;
 *         FAILURE otherwise
 *
 * @ingroup apiTfaApi
 */
retval_t tfa_init(void);

#endif

#if (defined ENABLE_TFA) || (defined __DOXYGEN__)

/**
 * @brief Reset the TFA
 *
 * This function is called to reset the TFA.
 *
 * @param set_default_pib Defines whether PIB values need to be set
 *                        to its default values
 *
 * @ingroup apiTfaApi
 */
void tfa_reset(bool set_default_pib);

#endif

#if (defined ENABLE_TFA) || (defined __DOXYGEN__)

/**
 * @brief Gets a TFA PIB attribute
 *
 * This function is called to retrieve the transceiver information base
 * attributes.
 *
 * @param[in] tfa_pib_attribute TAL infobase attribute ID
 * @param[out] value TFA infobase attribute value
 *
 * @return MAC_UNSUPPORTED_ATTRIBUTE if the TFA infobase attribute is not found
 *         MAC_SUCCESS otherwise
 *
 * @ingroup apiTfaApi
 */
retval_t tfa_pib_get(tfa_pib_t tfa_pib_attribute, void *value);

#endif

#if (defined ENABLE_TFA) || (defined __DOXYGEN__)

/**
 * @brief Sets a TFA PIB attribute
 *
 * This function is called to set the transceiver information base
 * attributes.
 *
 * @param[in] tfa_pib_attribute TFA infobase attribute ID
 * @param[in] value TFA infobase attribute value to be set
 *
 * @return MAC_UNSUPPORTED_ATTRIBUTE if the TFA info base attribute is not found
 *         TAL_BUSY if the TAL is not in TAL_IDLE state.
 *         MAC_SUCCESS if the attempt to set the PIB attribute was successful
 *
 * @ingroup apiTfaApi
 */
retval_t tfa_pib_set(tfa_pib_t tfa_pib_attribute, void *value);

#endif

#if (defined ENABLE_TFA) || (defined TFA_CCA) || (defined __DOXYGEN__)
#ifdef MULTI_TRX_SUPPORT

/**
 * @brief Perform a CCA
 *
 * This function performs a CCA request.
 *
 * @param trx_id Transceiver identifier
 *
 * @return phy_enum_t PHY_IDLE or PHY_BUSY
 *
 * @ingroup apiTfaApi
 */
phy_enum_t tfa_cca_perform(trx_id_t trx_id);

#else

/**
 * @brief Perform a CCA
 *
 * This function performs a CCA request.
 *
 * @return phy_enum_t PHY_IDLE or PHY_BUSY
 *
 * @ingroup apiTfaApi
 */
phy_enum_t tfa_cca_perform(void);

#endif /* #ifdef MULTI_TRX_SUPPORT */
#endif

#if (defined ENABLE_TFA) || (defined TFA_CCA) || (defined __DOXYGEN__)
#ifdef MULTI_TRX_SUPPORT

/**
 * @brief Perform a single ED measurement
 *
 * @param trx_id Identifier of the transceiver
 *
 * @return ed_value Result of the measurement (transceiver's register value)
 *         If the build switch TRX_REG_RAW_VALUE is defined, the transceiver's
 *         register value is returned.
 *
 * @ingroup apiTfaApi
 */
uint8_t tfa_ed_sample(trx_id_t trx_id);

#else

/**
 * @brief Perform a single ED measurement
 *
 * @return ed_value Result of the measurement (transceiver's register value)
 *         If the build switch TRX_REG_RAW_VALUE is defined, the transceiver's
 *         register value is returned.
 *
 * @ingroup apiTfaApi
 */
uint8_t tfa_ed_sample(void);

#endif /* #ifdef MULTI_TRX_SUPPORT */
#endif

#if (defined ENABLE_TFA) || (defined TFA_BAT_MON) || (defined __DOXYGEN__) || \
	(defined TFA_BAT_MON_READ)

/**
 * @brief Gets the transceiver's supply voltage
 *
 * @return mv Milli Volt; 0 if below threshold, 0xFFFF if above threshold
 *
 * @ingroup apiTfaApi
 */
uint16_t tfa_get_batmon_voltage(void);

#endif

#if (defined ENABLE_TFA) || (defined TFA_BAT_MON) || (defined __DOXYGEN__) || \
	(defined TFA_BAT_MON_IRQ)

/**
 * @brief Setups the battery monitor interrupt
 *
 * @param   batmon_irq_cb Callback function for the batmon interrupt
 * @param   vth Threshold value in millivolt
 *
 * @return MAC_SUCCESS if provided voltage is within range, else
 *         MAC_INVALID_PARAMETER
 *
 * @ingroup apiTfaApi
 */
retval_t tfa_batmon_irq_init(FUNC_PTR(batmon_irq_cb), uint16_t vth);
#endif

#if (defined ENABLE_TFA) || (PAL_GENERIC_TYPE == MEGA_RF) || \
	(defined __DOXYGEN__)

/**
 * @brief Get the temperature value from the integrated sensor
 *
 * @return temperature value in degree Celsius
 *
 * @ingroup apiTfaApi
 */
double tfa_get_temperature(void);

#endif

#if ((defined ENABLE_TFA) || (defined TFA_CW) || defined(__DOXYGEN__))
#ifdef MULTI_TRX_SUPPORT

/**
 * @brief Starts continuous transmission on current channel
 *
 * @param trx_if Transceiver identifier
 * @param tx_mode Transmission mode
 * @param random_content Use random content if true
 */
void tfa_continuous_tx_start(trx_id_t trx_id, continuous_tx_mode_t tx_mode);

#else

/**
 * @brief Starts continuous transmission on current channel
 *
 * @param trx_id Identifier of the transceiver
 * @param tx_mode Transmission mode
 * @param random_content Use random content if true
 */
void tfa_continuous_tx_start(continuous_tx_mode_t tx_mode, bool random_content);

#endif /* #ifdef MULTI_TRX_SUPPORT */
#endif

#if ((defined ENABLE_TFA) || (defined TFA_CW) || defined(__DOXYGEN__))
#ifdef MULTI_TRX_SUPPORT

/**
 * @brief Stops CW transmission
 *
 * @param trx_if Transceiver identifier
 */
void tfa_continuous_tx_stop(trx_id_t trx_id);

#else

/**
 * @brief Stops CW transmission
 */
void tfa_continuous_tx_stop(void);

#endif /* #ifdef MULTI_TRX_SUPPORT */
#endif

#if ((defined SUPPORT_TFA) || (defined TFA_PLL_BIST) || defined(DOXYGEN))
void tfa_pll_bist_init(void);
void tfa_pll_bist_run(trx_id_t trx_id);

#endif  /* #if ((defined SUPPORT_TFA) || (defined TFA_PLL_BIST) ||
         *defined(DOXYGEN)) */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* #ifdef ENABLE_TFA */

#endif /* TFA_H */
/* EOF */
