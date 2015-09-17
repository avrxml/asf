/**
 * @file tfa.h
 *
 * @brief This file is the interface for Transceiver Feature Access (TFA)
 * functionality.
 *
 * $Id: tfa.h 24320 2010-11-26 08:59:36Z sschneid $
 *
 * @author    Atmel Corporation: http://www.atmel.com
 * @author    Support: http://www.atmel.com/design-support/
 */
/*
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
 *
 * 
 */

/* Prevent double inclusion */
#ifndef TFA_H
#define TFA_H

#if (defined ENABLE_TFA) || (defined TFA_BAT_MON) || (defined DOXYGEN)

/* === INCLUDES ============================================================ */

#include "return_val.h"
#include "ieee_const.h"

/* === TYPES =============================================================== */

/** Transceiver commands */
typedef enum tfa_pib_t
{
    TFA_PIB_RX_SENS         = 0
} SHORTENUM tfa_pib_t;

/** Continuous Transmission modes */
typedef enum continuous_tx_mode_tag
{
    CW_MODE = 0,
    PRBS_MODE = 1
} SHORTENUM continuous_tx_mode_t;

/* === MACROS ============================================================== */

/**
 * Default value of TFA PIB attribute to reduce the Rx sensitivity.
 */
#define TFA_PIB_RX_SENS_DEF             (0)

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

#if (defined ENABLE_TFA) || (defined DOXYGEN)
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

#if (defined ENABLE_TFA) || (defined DOXYGEN)
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

#if (defined ENABLE_TFA) || (defined DOXYGEN)
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

#if (defined ENABLE_TFA) || (defined DOXYGEN)
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

#if (defined ENABLE_TFA) || (defined DOXYGEN)
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
#endif

#if (defined ENABLE_TFA) || (defined DOXYGEN)
/**
 * @brief Perform a single ED measurement
 *
 * @return ed_value Result of the measurement (transceiver's register value)
 *
 * @ingroup apiTfaApi
 */
uint8_t tfa_ed_sample(void);
#endif

#if (defined ENABLE_TFA) || (defined TFA_BAT_MON) || (defined DOXYGEN)
/**
 * @brief Get the transceiver's supply voltage
 *
 * @return mv Milli Volt; 0 if below threshold, 0xFFFF if above threshold
 *
 * @ingroup apiTfaApi
 */
uint16_t tfa_get_batmon_voltage(void);
#endif

#if (defined ENABLE_TFA) || (PAL_GENERIC_TYPE == MEGA_RF_) || (defined DOXYGEN)
/**
 * @brief Get the temperature value from the integrated sensor
 *
 * @return temperature value in degree Celsius
 *
 * @ingroup apiTfaApi
 */
double tfa_get_temperature(void);
#endif

#if ((defined ENABLE_TFA) || (TAL_TYPE != AT86RF230B) || \
     ((TAL_TYPE == AT86RF230B) && (defined CW_SUPPORTED))  || defined(DOXYGEN))
/**
 * @brief Starts continuous transmission on current channel
 *
 * @param tx_mode Transmission mode
 */
void tfa_continuous_tx_start(continuous_tx_mode_t tx_mode);
#endif

#if ((defined ENABLE_TFA) || (TAL_TYPE != AT86RF230B) || \
     ((TAL_TYPE == AT86RF230B) && (defined CW_SUPPORTED))  || defined(DOXYGEN))
/**
 * @brief Stops CW transmission
 */
void tfa_continuous_tx_stop(void);
#endif


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* #ifdef ENABLE_TFA */

#endif /* TFA_H */
/* EOF */
