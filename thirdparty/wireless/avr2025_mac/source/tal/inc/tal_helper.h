/**
 * @file tal_helper.h
 *
 * @brief This file contains TAL helper function declarations
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

/**
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel
 * Support</a>
 */
#ifndef TAL_HELPER_H
#define TAL_HELPER_H

/* === INCLUDES ============================================================ */
/* === EXTERNALS =========================================================== */

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

#define ANT_CTRL_0              (0)
#define ANT_CTRL_1              (1)
#define ANT_CTRL_2              (2)
#define ANT_CTRL_3              (3)

#define ANT_DIVERSITY_ENABLE          (1)
#define ANT_DIVERSITY_DISABLE         (0)
#define ANT_EXTERNAL_SW_ENABLE        (1)
#define ANT_EXTERNAL_SW_DISABLE       (0)
#define ANT_AUTO_SEL                  (0)

#define MAX_PDT_LEVEL           (0x0f)

#define REGISTER_VALUE          (0x01)
#define DBM_VALUE               (0x00)

#define AACK_PROM_ENABLE        (0x01)
#define AACK_PROM_DISABLE       (0x00)

#if ((TAL_TYPE == AT86RF233) || (TAL_TYPE == ATMEGARFR2))
#define BASE_ISM_FREQUENCY_MHZ              (2306)
#define MIN_ISM_FREQUENCY_MHZ               (2322)
#define MAX_ISM_FREQUENCY_MHZ               (2527)
#define MID_ISM_FREQUENCY_MHZ               (2434)

#define CC_BAND_0                           (0x00)
#define CC_BAND_8                           (0x08)
#define CC_BAND_9                           (0x09)
#define CC_NUMBER_0                         (0x00)

#define MIN_CC_BAND_8_OFFSET                (0x20)
#define MIN_CC_BAND_9_OFFSET                (0xBA)
#endif /*End of (TAL_TYPE == AT86RF233) || (TAL_TYPE == ATMEGARFR2)*/

#if ((TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B))
#define CC_1_START_FREQUENCY   (769.0)
#define CC_1_END_FREQUENCY     (794.5)
#define CC_2_START_FREQUENCY   (857.0)
#define CC_2_END_FREQUENCY     (882.5)
#define CC_3_START_FREQUENCY   (903.0)
#define CC_3_END_FREQUENCY     (928.5)
#define CC_4_START_FREQUENCY   (769.0)
#define CC_4_END_FREQUENCY     (863.0)
#define CC_5_START_FREQUENCY   (833.0)
#define CC_5_END_FREQUENCY     (935.0)
#define CC_6_START_FREQUENCY   (902.0)
#define CC_6_END_FREQUENCY     (927.5)

#define CC_BAND_0               (0x00)
#define CC_BAND_1               (0x01)
#define CC_BAND_2               (0x02)
#define CC_BAND_3               (0x03)
#define CC_BAND_4               (0x04)
#define CC_BAND_5               (0x05)
#define CC_BAND_6               (0x06)

#define MIN_CC_BAND_4_OFFSET    (0x5E)
#define MIN_CC_BAND_5_OFFSET    (0x66)

#if (TAL_TYPE == AT86RF212)
#define MAX_CC_BAND              (0x05)
#elif (TAL_TYPE == AT86RF212B)
#define MAX_CC_BAND              (0x06)
#endif /*End of (TAL_TYPE == AT86RF212) */

#endif /*End of(TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B)*/

#if (TAL_TYPE == ATMEGARFA1) || (TAL_TYPE == ATMEGARFR2)
#define MAX_REG_ADDR_VALUE      (0x1ff)
#else
#define MAX_REG_ADDR_VALUE      (0x3f)
#endif

/* parameter types in transceiver */
typedef enum param_tag {
#if ((TAL_TYPE != AT86RF230B) && (TAL_TYPE != AT86RF212))
	ANT_DIVERSITY     = 0x00,
	ANT_SELECT        = 0x01,
#endif
#if (TAL_TYPE != AT86RF230B)
	ANT_CTRL          = 0x02,
	AACK_PROMSCS_MODE    = 0x03,
#endif
#if ((TAL_TYPE == AT86RF233 || TAL_TYPE == ATMEGARFR2 \
	|| TAL_TYPE == AT86RF212 || TAL_TYPE == AT86RF212B))
	CC_BAND           = 0x04,
	CC_NUMBER         = 0x05,
#endif
	TX_PWR            = 0x06
} SHORTENUM param_type;

#ifdef EXT_RF_FRONT_END_CTRL /*For External PA for 231FEM-EK*/

/**
 * RSSI BASE VAL based on External LNA Gain.
 */
#define RSSI_BASE_VAL_EXT      (RSSI_BASE_VAL_DBM - EXT_LNA_HIGH_GAIN)

/*
 * Default tx power for Ch26 to meet FCC compliance
 */
#define DEFAULT_TX_POWER_CH26             (0x80 | 0x0d)

#endif
/* === PROTOTYPES ========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Enable/Disable the external RF front end control
 *
 * \param pa_ext_sw_ctrl true  if external rf front end control has to be
 *                              enabled
 *
 * \return MAC_SUCCESS  if PA_EXT_EN bit is configured correctly
 *         FAILURE      otherwise
 */
#if (TAL_TYPE != AT86RF230B)
retval_t  tal_ext_pa_ctrl(bool pa_ext_sw_ctrl);

#endif /* End of (TAL_TYPE != AT86RF230B) */

#if (TAL_TYPE != AT86RF212 && TAL_TYPE != AT86RF212B)

/**
 * \brief Enable/Disable the external RF front end control
 *
 * \param pa_ext_sw_ctrl true  if external rf front end control has to be
 *                              enabled
 *
 * \return MAC_SUCCESS  if PA_EXT_EN bit is configured correctly
 *         FAILURE      otherwise
 */
retval_t tal_set_tx_pwr(bool type, int8_t pwr_value);

/*
 * \brief Converts a register value to a dBm value
 *
 * \param reg_value TransmitPower register value,
 *
 * \param *dbm_value pointer to dbm_value
 *
 * \return MAC_SUCCESS or FAILURE based on conversion is done or not
 */

retval_t tal_convert_reg_value_to_dBm(uint8_t reg_value, int8_t *dbm_value);

#endif /* End of (TAL_TYPE != AT86RF212)*/

#if ((TAL_TYPE != AT86RF230B) && (TAL_TYPE != AT86RF212))

/*
 * \brief Configures antenna diversity and selects antenna
 *
 * \param div_ctrl  true/false to enable/disable antenna diversity
 * \param ant_ctrl  0 or 3 when antenna diversity is enabled
 *                  1 or 2 to select antenna 1 or antenna 2
 * \return The value set in the TX_PWR bits
 */
retval_t  tal_ant_div_config(bool div_ctrl, uint8_t ant_ctrl);

#endif /* End of ((TAL_TYPE != AT86RF230B) && (TAL_TYPE != AT86RF212))*/

#if ((TAL_TYPE == AT86RF233) || (TAL_TYPE == ATMEGARFR2) || \
	(TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B))

/*
 * \brief Configures the frequency to be set in transceiver
 *
 * \param frequency  frequency value to be set
 * \return MAC_SUCCESS if frequency is configured correctly
 *                 MAC_INVALID_PARAMETER if out of range or incorrect values are
 * given
 *                 FAILURE if frequency registers are not configured properly
 */
retval_t tal_set_frequency(float frequency);

/**
 * \brief to set the frequency based on CC_BAND and CC_NUMBER Registers
 *
 * \param cc_band band to be selected in cc_band register bits
 * \param cc_number offset frequency to be selected in cc_number register bits
 * \return MAC_SUCCESS if frequency is configured correctly
 *                 MAC_INVALID_PARAMETER if out of range or incorrect values are
 * given
 *                 FAILURE if frequency registers are not configured properly
 */

retval_t tal_set_frequency_regs(uint8_t cc_band, uint8_t cc_number);

/*
 * \brief Calculate the frequency based on CC_BAND and CC_NUMBER Registers
 *
 * \param CC_BAND and CC_NUMBER register values to calculate the frequency
 * \param *freq pointer where the calculated frequency value should be stored
 *
 * \return  MAC_SUCCESS if frequency is configured correctly
 *          MAC_INVALID_PARAMETER if out of range or incorrect values are given
 *          FAILURE if frequency registers are not configured properly
 */
retval_t tal_calculate_frequency(uint8_t cc_band, uint8_t cc_number,
		float *freq);

#endif /* End of ((TAL_TYPE == AT86RF233) || (TAL_TYPE == ATMEGARFR2) ||
        *        (TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B))*/
#if (TAL_TYPE != AT86RF230B)

/*
 * \brief Configures receiver sensitivity level
 *
 * \param pdt_level  0 to 15 levels of rx sensitivity
 * \param MAC_SUCCESS if sensitivity level is configured correctly
 *        MAC_INVALID_PARAMETER pdt_level is out of range
 *        FAILURE otheriwse
 */
retval_t tal_set_rx_sensitivity_level(uint8_t pdt_level);

/*
 * \brief Configures promiscous mode in rx_aack_on mode
 *
 * \param prom_ctrl  true/false to enable/disable prom mode
 *
 * \param MAC_SUCCESS if rxaack_prom_mode is configured correctly
 *        FAILURE otheriwse
 */
retval_t tal_rxaack_prom_mode_ctrl(bool prom_ctrl);

#endif /* End of TAL_TYPE != AT86RF230B */

/*
 * \brief to get the current status of the transceiver
 *
 * \return status of the transceiver
 */
tal_trx_status_t tal_get_trx_status(void);

/*
 * \brief to read a particular transceiver register
 * \param reg_addr address of the transveiver register to be read
 * \param *data pointer to the location where the register value need to be
 *              stored
 * \return MAC_SUCCESS if the register is read correctly
 *         MAC_INVALID_PARAMETER if the reg_addr is out of range
 */
retval_t tal_trx_reg_read(uint16_t reg_addr, uint8_t *data);

/*
 * \brief to write a particular transceiver register
 *
 * \param reg_addr address of the transceiver register to be written
 * \param value value to be written in the register
 *
 * \return MAC_SUCCESS if the register is written correctly
 *         MAC_INVALID_PARAMETER if the reg_addr is out of range
 */
retval_t  tal_trx_reg_write(uint16_t reg_addr, uint8_t value);

/*
 * \brief to read a current setting particular transceiver parameter
 * \param parameter type of the parameter to be read
 * \param *param_value pointer to the location where the current parameter value
 * need to be
 *              stored
 * \return MAC_INVALID_PARAMETER if the parameter is invalid
 *         MAC_SUCCESS otherwise
 */

retval_t tal_get_curr_trx_config(param_type parameter, uint8_t *param_value);

/*
 * \brief to read a particular range of transceiver registers
 *
 * \param reg_addr address of the transceiver register to be written
 * \param value value to be written in the register
 *
 * \return MAC_SUCCESS if the register is written correctly
 *         MAC_INVALID_PARAMETER if the reg_addr is out of range
 */

retval_t tal_dump_registers(uint16_t start_addr, uint16_t end_addr,
		uint8_t *value);

/*
 * \brief to configure the reduced power consumption mode
 *
 * \param rpc_mode_sel value to be written in the TRX_RPC bits
 *
 * \return MAC_SUCCESS if the register is written correctly
 *         FAILURE otherwise
 */
#if ((TAL_TYPE == AT86RF233) || (TAL_TYPE == ATMEGARFR2))
retval_t tal_rpc_mode_config(uint8_t rpc_mode_sel);

#endif /* End of ((TAL_TYPE == AT86RF233) || (TAL_TYPE == ATMEGARFR2))*/

/*
 * \brief This function is called to get the base RSSI value for repective
 * radios
 *
 * \return value of the base RSSI value
 */
int8_t tal_get_rssi_base_val(void);

/**
 * \brief Enable/Disable the rx safe mode
 *
 * \param safe_mode_ctrl true  if rx safe mode has to be enabled
 *
 * \return MAC_SUCCESS  if rx_safe_mode bit is configured correctly
 *         FAILURE      otherwise
 */
retval_t tal_rxsafe_mode_ctrl(bool safe_mode_ctrl);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TAL_HELPER_H */
/* EOF */
