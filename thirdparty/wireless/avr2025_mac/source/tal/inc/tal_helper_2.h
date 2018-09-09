/**
 * @file tal_helper_2.h
 *
 * @brief This file contains TAL helper function declarations
 *
 * Copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
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
 */

/*
 * Copyright (c) 2015-2018, Microchip Technology Inc All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* Prevent double inclusion */
#ifndef TAL_HELPER_2_H
#define TAL_HELPER_2_H

/* === INCLUDES ============================================================ */
/* === EXTERNALS =========================================================== */

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/** sub-register TRX_STATUS in register TRX_STATUS */
typedef enum tal_trx_status_tag {
	/** Constant P_ON for sub-register @ref SR_TRX_STATUS */
	P_ON                         = (0x00),

	/** Constant TRX_OFF for sub-register @ref SR_TRX_STATUS */
	TRX_OFF                      = (0x08),

	/** Constant PLL_ON for sub-register @ref SR_TRX_STATUS */
	PLL_ON                       = (0x09),

	/** Constant TRX_SLEEP for sub-register @ref SR_TRX_STATUS */
	TRX_SLEEP                    = (0x0F),

	/** Constant RX_AACK_ON for sub-register @ref SR_TRX_STATUS */
	RX_ON                   = (0x16)
} SHORTENUM tal_trx_status_t;

#define MAX_PDT_LEVEL           (0x0f)

#define AACK_PROM_ENABLE        (0x01)
#define AACK_PROM_DISABLE       (0x00)

#define MAX_REG_ADDR_VALUE              (0X3fff)

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

/* === PROTOTYPES ========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

bool crc_check_ok(trx_id_t trx);

/**
 * \brief Enable/Disable the external RF front end control
 *
 * \param pa_ext_sw_ctrl true  if external rf front end control has to be
 *                              enabled
 *
 * \return MAC_SUCCESS  if PA_EXT_EN bit is configured correctly
 *         FAILURE      otherwise
 */
retval_t  tal_ext_pa_ctrl(bool pa_ext_sw_ctrl);

/**
 * \brief Enable/Disable the external RF front end control
 *
 * \param pa_ext_sw_ctrl true  if external rf front end control has to be
 *                              enabled
 *
 * \return MAC_SUCCESS  if PA_EXT_EN bit is configured correctly
 *         FAILURE      otherwise
 */
retval_t tal_set_tx_pwr(trx_id_t trx, bool type, int8_t pwr_value);

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

/*
 * \brief Configures antenna diversity and selects antenna
 *
 * \param div_ctrl  true/false to enable/disable antenna diversity
 * \param ant_ctrl  0 or 3 when antenna diversity is enabled
 *                  1 or 2 to select antenna 1 or antenna 2
 * \return The value set in the TX_PWR bits
 */
retval_t  tal_ant_div_config(bool div_ctrl, uint8_t ant_ctrl);

/*
 * \brief Configures receiver sensitivity level
 *
 * \param pdt_level  0 to 15 levels of rx sensitivity
 * \param MAC_SUCCESS if sensitivity level is configured correctly
 *        MAC_INVALID_PARAMETER pdt_level is out of range
 *        FAILURE otheriwse
 */
retval_t tal_set_rx_sensitivity_level(trx_id_t trx, uint8_t pdt_level);

/*
 * \brief Configures promiscous mode in rx_aack_on mode
 *
 * \param prom_ctrl  true/false to enable/disable prom mode
 *
 * \param MAC_SUCCESS if rxaack_prom_mode is configured correctly
 *        FAILURE otheriwse
 */
retval_t tal_rxaack_prom_mode_ctrl(trx_id_t trx, bool prom_ctrl);

/*
 * \brief to get the current status of the transceiver
 *
 * \return status of the transceiver
 */
tal_trx_status_t tal_get_trx_status(trx_id_t trx);

/*
 * \brief to read a particular transceiver register
 * \param reg_addr address of the transeiver register to be read
 * \param *data pointer to the location where the register value need to be
 *              stored
 * \return MAC_SUCCESS if the register is read correctly
 *         MAC_INVALID_PARAMETER if the reg_addr is out of range
 */
retval_t tal_trx_reg_read(trx_id_t trx, uint16_t reg_addr, uint8_t *data);

/*
 * \brief to write a particular transceiver register
 *
 * \param reg_addr address of the transceiver register to be written
 * \param value value to be written in the register
 *
 * \return MAC_SUCCESS if the register is written correctly
 *         MAC_INVALID_PARAMETER if the reg_addr is out of range
 */
retval_t  tal_trx_reg_write(trx_id_t trx, uint16_t reg_addr, uint8_t value);

/*
 * \brief to read a current setting particular transceiver parameter
 * \param parameter type of the parameter to be read
 * \param *param_value pointer to the location where the current parameter value
 * need to be
 *              stored
 * \return MAC_INVALID_PARAMETER if the parameter is invalid
 *         MAC_SUCCESS otherwise
 */

retval_t tal_get_curr_trx_config(trx_id_t trx, param_type parameter,
		uint8_t *param_value);

/*
 * \brief to read a particular range of transceiver registers
 *
 * \param reg_addr address of the transceiver register to be written
 * \param value value to be written in the register
 *
 * \return MAC_SUCCESS if the register is written correctly
 *         MAC_INVALID_PARAMETER if the reg_addr is out of range
 */

retval_t tal_dump_registers(trx_id_t trx, uint16_t start_addr,
		uint16_t end_addr,
		uint8_t *value);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TAL_HELPER_H */
/* EOF */
