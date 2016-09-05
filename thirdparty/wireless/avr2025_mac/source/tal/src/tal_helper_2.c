/**
 * @file tal_helper_2.c
 *
 * @brief This file implements helper functions to configure transceiver
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
 */

/*
 * Copyright (c) 2015, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* === INCLUDES ============================================================ */

/* #if (TAL_TYPE != AT86RF215) */
#include "pal.h"
#include "return_val.h"
#include "tal.h"
#include "tal_internal.h"
#include "ieee_const.h"
#include "tal_helper_2.h"

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/* === GLOBALS ============================================================= */

FLASH_EXTERN(int8_t tx_pwr_table[16]);

/* === PROTOTYPES ========================================================== */
extern uint8_t convert_phyTransmitPower_to_reg_value(
		uint8_t phyTransmitPower_value);

/* === IMPLEMENTATION ====================================================== */

/**
 * \brief Enable/Disable the external RF front end control
 *
 * \param pa_ext_sw_ctrl true  if external rf front end control has to be
 *                              enabled
 *
 * \return MAC_SUCCESS  if PA_EXT_EN bit is configured correctly
 *         FAILURE      otherwise
 */
retval_t tal_set_tx_pwr(trx_id_t trx, bool type, int8_t pwr_value)
{
	return MAC_SUCCESS;

	int8_t temp_var;
	int8_t tx_pwr_dbm = 0;
	/* modify the register for tx_pwr and set the tal_pib accordingly */
	if (true == type) {
		if (MAC_SUCCESS ==
				tal_convert_reg_value_to_dBm(pwr_value,
				&tx_pwr_dbm)) {
			temp_var = CONV_DBM_TO_phyTransmitPower(tx_pwr_dbm);
			tal_pib_set(trx, phyTransmitPower,
					(pib_value_t *)&temp_var);

			/* To make sure that TX_PWR register is updated with the
			 * value whatever user povided.Otherwise lowest dBm
			 * power
			 * (highest reg value will be taken)
			 */
			return MAC_SUCCESS;
		} else {
			/* return invalid parameter if out of range */
			return MAC_INVALID_PARAMETER;
		}
	} else {
		temp_var = CONV_DBM_TO_phyTransmitPower(pwr_value);
		tal_pib_set(trx, phyTransmitPower, (pib_value_t *)&temp_var);
	}

	uint8_t reg_value = convert_phyTransmitPower_to_reg_value(
			tal_pib[trx].TransmitPower);
	/* check the value written in the transceiver register */
	uint8_t temp = 0X00;
	if (temp == reg_value) {
		return MAC_SUCCESS;
	} else {
		return FAILURE;
	}
}

/*
 * \brief Configures receiver sensitivity level
 *
 * \param pdt_level  0 to 15 levels of rx sensitivity
 * \param MAC_SUCCESS if sensitivity level is configured correctly
 *        MAC_INVALID_PARAMETER pdt_level is out of range
 *        FAILURE otheriwse
 */
retval_t tal_set_rx_sensitivity_level(trx_id_t trx, uint8_t pdt_level)
{
	uint8_t temp;
	/* return invalid parameter if sensitivity level is out of range*/
	if (pdt_level > MAX_PDT_LEVEL) {
		return MAC_INVALID_PARAMETER;
	}

	/* configure sensitivity level*/
	trx_bit_write(SR_BBC1_OQPSKC1_PDT0, pdt_level);
#if (TAL_TYPE == ATMEGARFA1)
	CONF_REG_WRITE();
#endif /* TAL_TYPE == ATMEGA128RFA1 */
	temp = trx_bit_read(SR_BBC1_OQPSKC1_PDT0);
	if (temp == pdt_level) {
		return MAC_SUCCESS;
	} else {
		return FAILURE;
	}
}

/**
 * \brief Calculates CRC manually and compares with the received
 * and returns true if both are same,false otherwise.
 */
bool crc_check_ok(trx_id_t trx)
{
	uint16_t rf_reg_offset = RF_BASE_ADDR_OFFSET * trx;
	return (trx_bit_read(rf_reg_offset + SR_BBC0_PC_FCSOK));
}

/*
 * \brief Configures promiscous mode in rx_aack_on mode
 *
 * \param prom_ctrl  true/false to enable/disable prom mode
 *
 * \param MAC_SUCCESS if rxaack_prom_mode is configured correctly
 *        FAILURE otheriwse
 */

retval_t tal_rxaack_prom_mode_ctrl(trx_id_t trx, bool prom_ctrl)
{
	uint16_t rf_reg_offset = RF_BASE_ADDR_OFFSET * trx;
	bool temp;
	if (prom_ctrl) {
		/* configure promiscuous mode */
		trx_bit_write(rf_reg_offset + SR_BBC0_AFC0_PM, prom_ctrl);
		trx_bit_write(rf_reg_offset + SR_BBC0_PC_FCSFE, 0X00); /*
		                                                       * Disable
		                                                       * FCSFE
		                                                       **/
	} else {
		/* configure promiscuous mode */
		trx_bit_write(rf_reg_offset + SR_BBC0_AFC0_PM, prom_ctrl);
		trx_bit_write(rf_reg_offset + SR_BBC0_PC_FCSFE, 0X01); /* Enable
		                                                        * FCSFE
		                                                        **/
	}

	temp = trx_bit_read(SR_BBC0_AFC0_PM);
	if (temp == prom_ctrl) {
		return MAC_SUCCESS;
	} else {
		return FAILURE;
	}
}

/*
 * \brief to get the current status of the transceiver
 *
 * \return status of the transceiver
 */
tal_trx_status_t tal_get_trx_status(trx_id_t trx)
{
	uint16_t rf_reg_offset = RF_BASE_ADDR_OFFSET * trx;
	rf_cmd_status_t trx_status;
	if (tal_state[trx] == TAL_SLEEP) {
		return TRX_SLEEP;
	}

	/* Read the status from trx_status bits */
	trx_status = trx_bit_read(rf_reg_offset + SR_RF09_STATE_STATE);
	switch (trx_status) {
	case STATUS_RF_RESET:
	{
		return P_ON;
	}
	break;

	case STATUS_RF_TRXOFF:
	{
		return TRX_OFF;
	}
	break;

	case STATUS_RF_TXPREP:
	{
		return PLL_ON;
	}
	break;

	case STATUS_RF_RX:
	{
		return RX_ON;
	}
	break;

	default:
		return trx_status;
	}
}

/*
 * \brief to read a particular transceiver register
 * \param reg_addr address of the transveiver register to be read
 * \param *data pointer to the location where the register value need to be
 *              stored
 * \return MAC_SUCCESS if the register is read correctly
 *         MAC_INVALID_PARAMETER if the reg_addr is out of range
 */

retval_t tal_trx_reg_read(trx_id_t trx, uint16_t reg_addr, uint8_t *data)
{
	/*check if register address is out of range*/
#if ((TAL_TYPE == ATMEGARFA1) || (TAL_TYPE == ATMEGARFR2))
	if (reg_addr > MAX_REG_ADDR_VALUE) {
		return MAC_INVALID_PARAMETER;
	}

#else
	if (reg_addr > MAX_REG_ADDR_VALUE) {
		return MAC_INVALID_PARAMETER;
	}
#endif /* End of (TAL_TYPE == ATMEGARFA1) || (TAL_TYPE == ATMEGA256RFR2)*/

	/* Read the corresponding transceiver register and return through data
	 * pointer */
	*data = trx_reg_read(reg_addr);

	return MAC_SUCCESS;
}

/*
 * \brief to write a particular transceiver register
 *
 * \param reg_addr address of the transceiver register to be written
 * \param value value to be written in the register
 *
 * \return MAC_SUCCESS if the register is written correctly
 *         MAC_INVALID_PARAMETER if the reg_addr is out of range
 */
retval_t  tal_trx_reg_write(trx_id_t trx, uint16_t reg_addr, uint8_t value)
{
#if ((TAL_TYPE == ATMEGARFA1) || (TAL_TYPE == ATMEGARFR2))
	if (reg_addr > MAX_REG_ADDR_VALUE) {
		return MAC_INVALID_PARAMETER;
	}

#else
	if (reg_addr > MAX_REG_ADDR_VALUE) {
		return MAC_INVALID_PARAMETER;
	}
#endif /* End of (TAL_TYPE == ATMEGARFA1) || (TAL_TYPE == ATMEGA256RFR2)*/
	trx_reg_write(reg_addr, value);

#if (TAL_TYPE == ATMEGARFA1)
	CONF_REG_WRITE();
#endif /* TAL_TYPE == ATMEGA128RFA1 */
	/* check the value written in the transceiver register address*/
	if (value != trx_reg_read(reg_addr)) {
		return MAC_INVALID_PARAMETER;
	}

	return MAC_SUCCESS;
}

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
		uint8_t *param_value)
{
	uint16_t rf_reg_offset = RF_BASE_ADDR_OFFSET * trx;

	switch (parameter) {
	case AACK_PROMSCS_MODE:
		*param_value = trx_bit_read(rf_reg_offset + SR_BBC0_AFC0_PM);
		break;

	case TX_PWR:
		*param_value = trx_bit_read(rf_reg_offset + SR_RF09_PAC_TXPWR);
		break;

	default:
		return MAC_INVALID_PARAMETER;
	}
	return MAC_SUCCESS;
}

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
		uint8_t *value)
{
	uint16_t i;
	int16_t length;

	/*check start and end address, return invalid parameter if out of range
	**/
#if ((TAL_TYPE == ATMEGARFA1) || (TAL_TYPE == ATMEGARFR2))
	if (start_addr > MAX_REG_ADDR_VALUE || end_addr > MAX_REG_ADDR_VALUE) {
		return MAC_INVALID_PARAMETER;
	}

#else
	if (start_addr > MAX_REG_ADDR_VALUE || end_addr > MAX_REG_ADDR_VALUE) {
		return MAC_INVALID_PARAMETER;
	}
#endif /* End of (TAL_TYPE == ATMEGARFA1) || (TAL_TYPE == ATMEGA256RFR2)*/
	length = end_addr - start_addr;
	if (length < 0) {
		/* return invalid parameter if start and end addresses are not
		 * in order*/
		return MAC_INVALID_PARAMETER;
	} else {
		/* Read and store the values in input address*/
		for (i = start_addr; i <= end_addr; i++) {
			*value = trx_reg_read(i);
			value++;
		}
		return MAC_SUCCESS;
	}
}

/*
 * \brief Converts a register value to a dBm value
 *
 * \param reg_value TransmitPower register value,
 *
 * \param *dbm_value pointer to dbm_value
 *
 * \return MAC_SUCCESS or FAILURE based on conversion is done or not
 */
retval_t tal_convert_reg_value_to_dBm(uint8_t reg_value, int8_t *dbm_value)
{
	*dbm_value = reg_value - 17;
	return MAC_SUCCESS;
}
