/**
 * @file tal_ed.c
 *
 * @brief This file implements helper functions to configure transceiver
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

/* === INCLUDES ============================================================ */

/**
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel
 * Support</a>
 */

#include "pal.h"
#include "return_val.h"
#include "tal.h"
#include "tal_internal.h"
#include "ieee_const.h"
#include "tal_helper.h"

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/* === GLOBALS ============================================================= */
#if (TAL_TYPE != AT86RF212 && TAL_TYPE != AT86RF212B)
FLASH_EXTERN(int8_t tx_pwr_table[16]);
#endif

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
#if ((TAL_TYPE != AT86RF230B) && (TAL_TYPE != AT86RF232))

retval_t  tal_ext_pa_ctrl(bool pa_ext_sw_ctrl)
{
	bool temp;
	trx_bit_write(SR_PA_EXT_EN, pa_ext_sw_ctrl);
#if (TAL_TYPE == ATMEGARFA1)
	CONF_REG_WRITE();
#endif /* TAL_TYPE == ATMEGA128RFA1 */
	/* Read the PA_EXT_EN bit to check the configuration */
	temp = /*(bool)*/ trx_bit_read(SR_PA_EXT_EN);
	if (pa_ext_sw_ctrl == temp) {
		/* return success if the configuration is done correctly */
		return MAC_SUCCESS;
	} else {
		/* return success if the configuration is not done correctly */
		return FAILURE;
	}
}

#endif /* End of (TAL_TYPE != AT86RF230B) */

/**
 * \brief Enable/Disable the external RF front end control
 *
 * \param pa_ext_sw_ctrl true  if external rf front end control has to be
 *                              enabled
 *
 * \return MAC_SUCCESS  if PA_EXT_EN bit is configured correctly
 *         FAILURE      otherwise
 */

#if (TAL_TYPE != AT86RF212 && TAL_TYPE != AT86RF212B)
retval_t tal_set_tx_pwr(bool type, int8_t pwr_value)
{
	uint64_t temp_var;
	int8_t tx_pwr_dbm = 0;
	/* modify the register for tx_pwr and set the tal_pib accordingly */
	if (true == type) {
		if (MAC_SUCCESS ==
				tal_convert_reg_value_to_dBm(pwr_value,
				&tx_pwr_dbm)) {
			temp_var = CONV_DBM_TO_phyTransmitPower(tx_pwr_dbm);
			tal_pib_set(phyTransmitPower, (pib_value_t *)&temp_var);

			/* To make sure that TX_PWR register is updated with the
			 * value whatever user povided.Otherwise lowest dBm
			 * power
			 * (highest reg value will be taken)
			 */
			trx_bit_write(SR_TX_PWR, pwr_value);
#if (TAL_TYPE == ATMEGARFA1)
			CONF_REG_WRITE();
#endif /* TAL_TYPE == ATMEGA128RFA1 */
			return MAC_SUCCESS;
		} else {
			/* return invalid parameter if out of range */
			return MAC_INVALID_PARAMETER;
		}
	} else {
		temp_var = CONV_DBM_TO_phyTransmitPower(pwr_value);
		tal_pib_set(phyTransmitPower, (pib_value_t *)&temp_var);
	}

	uint8_t reg_value = convert_phyTransmitPower_to_reg_value(
			tal_pib.TransmitPower);
	/* check the value written in the transceiver register */
	uint8_t temp = trx_bit_read(SR_TX_PWR);
	if (temp == reg_value) {
		return MAC_SUCCESS;
	} else {
		return FAILURE;
	}
}

#endif /* End of (TAL_TYPE != AT86RF212) */

/*
 * \brief Configures antenna diversity and selects antenna
 *
 * \param div_ctrl  true/false to enable/disable antenna diversity
 * \param ant_ctrl  0 or 3 when antenna diversity is enabled
 *                  1 or 2 to select antenna 1 or antenna 2
 * \return The value set in the TX_PWR bits
 */
#if ((TAL_TYPE != AT86RF230B) && (TAL_TYPE != AT86RF212))
retval_t  tal_ant_div_config(bool div_ctrl, uint8_t ant_ctrl)
{
	retval_t return_var = FAILURE;
	if (true == div_ctrl) {
		/* do the configurations if diversity has to be enabled */
		trx_bit_write(SR_ANT_CTRL, ANT_CTRL_0);
		trx_bit_write(SR_ANT_DIV_EN, ANT_DIV_ENABLE);

#if ((TAL_TYPE != AT86RF212) && (TAL_TYPE != AT86RF212B))
		trx_bit_write(SR_PDT_THRES, THRES_ANT_DIV_ENABLE);
#endif /* End of ((TAL_TYPE != AT86RF212) && (TAL_TYPE!= AT86RF212B)) */
		trx_bit_write(SR_ANT_EXT_SW_EN, ANT_EXT_SW_ENABLE);

#if (TAL_TYPE == ATMEGARFA1)
		CONF_REG_WRITE();
#endif /* TAL_TYPE == ATMEGA128RFA1 */

		/* check the values written in transceiver registers */
		if ((trx_bit_read(SR_ANT_CTRL) == ANT_CTRL_0) &&
				(trx_bit_read(SR_ANT_DIV_EN) ==
				ANT_DIV_ENABLE) && \
				(trx_bit_read(SR_ANT_EXT_SW_EN) ==
				ANT_EXT_SW_ENABLE)) {
#if ((TAL_TYPE != AT86RF212) && (TAL_TYPE != AT86RF212B))
			if ((trx_bit_read(SR_PDT_THRES) ==
					THRES_ANT_DIV_ENABLE))
#endif
			return_var = MAC_SUCCESS;
		} else {
			return_var = FAILURE;
		}
	} else {
		/* do the configurations if diversity has to be disabled */
		trx_bit_write(SR_ANT_DIV_EN, ANT_DIV_DISABLE);
#if ((TAL_TYPE != AT86RF212) && (TAL_TYPE != AT86RF212B))
		trx_bit_write(SR_PDT_THRES, THRES_ANT_DIV_DISABLE);
#endif

		trx_bit_write(SR_ANT_EXT_SW_EN, ANT_EXT_SW_ENABLE);
		if (ant_ctrl == ANT_CTRL_1) {
			/* Enable A1/X2 */
			trx_bit_write(SR_ANT_CTRL, ANT_CTRL_1);
		} else if (ant_ctrl == ANT_CTRL_2) {
			/* Enable A2/X3 */
			trx_bit_write(SR_ANT_CTRL, ANT_CTRL_2);
		} else if (ant_ctrl == ANT_CTRL_0 || ant_ctrl == ANT_CTRL_3) {
			trx_bit_write(SR_ANT_CTRL, ANT_CTRL_0);
			ant_ctrl = 0;
		} else {
			return_var = MAC_INVALID_PARAMETER;
		}

#if (TAL_TYPE == ATMEGARFA1)
		CONF_REG_WRITE();
#endif /* TAL_TYPE == ATMEGA128RFA1 */
		/* check the values written in transceiver registers */
		if ((trx_bit_read(SR_ANT_CTRL) == ant_ctrl) &&
				(trx_bit_read(SR_ANT_DIV_EN) ==
				ANT_DIV_DISABLE) && \
				(trx_bit_read(SR_ANT_EXT_SW_EN) ==
				ANT_EXT_SW_DISABLE)) {
#if ((TAL_TYPE != AT86RF212) && (TAL_TYPE != AT86RF212B))
			if ((trx_bit_read(SR_PDT_THRES) ==
					THRES_ANT_DIV_ENABLE))
#endif
			return_var = MAC_SUCCESS;
		} else {
			return_var = FAILURE;
		}
	}

	return return_var;
}

#endif /* End of ((TAL_TYPE != AT86RF230B) && (TAL_TYPE != AT86RF212))*/

/*
 * \brief Configures the frequency to be set in transceiver
 *
 * \param frequency  frequency value to be set
 * \return MAC_SUCCESS if frequency is configured correctly
 *                 MAC_INVALID_PARAMETER if out of range or incorrect values are
 * given
 *                 FAILURE if frequency registers are not configured properly
 */
#if ((TAL_TYPE == AT86RF233) || (TAL_TYPE == ATMEGARFR2) || \
	(TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B))

retval_t tal_set_frequency(float frequency)
{
	float epsilon = 0.000000001;
	double dummy = 0.0;
	uint8_t cc_number = 0;
	uint8_t cc_band = 0;
	tal_trx_status_t previous_trx_status = TRX_OFF;
	/* frequency has to selected by CHANNEL register bits in PHY_CC_CCA*/
	if (abs(frequency - dummy) < epsilon) {
		cc_band = 0;
		cc_number = 0;
	}

#if ((TAL_TYPE == AT86RF233) || (TAL_TYPE == ATMEGARFR2))
	/* return invalid parameter if input frequency is out of range */
	else if ((frequency < MIN_ISM_FREQUENCY_MHZ) ||
			(frequency > MAX_ISM_FREQUENCY_MHZ)) {
		return MAC_INVALID_PARAMETER;
	}
	/* Choose CC_BAND & CC_NUMBER reg values for the input frequency */
	else if (frequency < MID_ISM_FREQUENCY_MHZ) {
		/* required frequency maps to band 8 */
		cc_band = CC_BAND_8;
		cc_number = (uint8_t)((frequency - BASE_ISM_FREQUENCY_MHZ) * 2);
	} else {
		/* required frequency maps to band 9 */
		cc_band = CC_BAND_9;
		cc_number = (uint8_t)((frequency - MID_ISM_FREQUENCY_MHZ) * 2);
	}
#elif ((TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B))
	/* Choose CC_BAND & CC_NUMBER reg values for the input frequency */
	else if ((frequency >= CC_1_START_FREQUENCY && frequency <=
			CC_1_END_FREQUENCY)) {
		cc_band = CC_BAND_1;
		cc_number = (uint8_t)((frequency - CC_1_START_FREQUENCY) * 10);
	} else if (frequency >= CC_2_START_FREQUENCY && frequency <=
			CC_2_END_FREQUENCY) {
		cc_band = CC_BAND_2;
		cc_number = (uint8_t)((frequency - CC_2_START_FREQUENCY) * 10);
	} else if (frequency >= CC_3_START_FREQUENCY && frequency <=
			CC_3_END_FREQUENCY) {
		cc_band = CC_BAND_3;
		cc_number = (uint8_t)((frequency - CC_3_START_FREQUENCY) * 10);
	}
#if (TAL_TYPE == AT86RF212B)
	else if ((frequency >= CC_6_START_FREQUENCY) &&
			(frequency <= CC_6_END_FREQUENCY)) {
		cc_band = CC_BAND_6;
		cc_number = (uint8_t)((frequency - CC_6_START_FREQUENCY) * 10);
	}
#endif /* End of (TAL_TYPE == AT86RF212B) */
	else if (frequency >= CC_4_START_FREQUENCY && frequency <=
			CC_4_END_FREQUENCY) {
		cc_band = CC_BAND_4;
	} else if (frequency >= CC_5_START_FREQUENCY && frequency <=
			CC_5_END_FREQUENCY) {
		cc_band = CC_BAND_5;
	} else {
		/* input frequency did not fit in any of the bands */
		return MAC_INVALID_PARAMETER;
	}
#endif /* End of ((TAL_TYPE == AT86RF233) || (TAL_TYPE == ATMEGARFR2)) */
	if (tal_trx_status != TRX_OFF) {
		previous_trx_status = RX_AACK_ON; /* any other than TRX_OFF
		                                   * state */
		do {
			/* set TRX_OFF until it could be set;
			 * trx might be busy */
		} while (set_trx_state(CMD_TRX_OFF) != TRX_OFF);
	}

	trx_bit_write(SR_CC_BAND, cc_band);
	trx_reg_write(RG_CC_CTRL_0, cc_number);

#if (TAL_TYPE == ATMEGARFA1)
	CONF_REG_WRITE();
#endif /* TAL_TYPE == ATMEGA128RFA1 */

	/* Re-store previous trx state */
	if (previous_trx_status != TRX_OFF) {
		/* Set to default state */
		set_trx_state(CMD_RX_AACK_ON);
	}

	/* check the values written in transceiver registers */
	if (trx_bit_read(SR_CC_BAND) != cc_band || \
			trx_reg_read(RG_CC_CTRL_0) != cc_number) {
		return FAILURE;
	} else {
		return MAC_SUCCESS;
	}
}

#endif /* End of ((TAL_TYPE == AT86RF233) || (TAL_TYPE == ATMEGARFR2) ||
        *        (TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B)) */

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
#if ((TAL_TYPE == AT86RF233) || (TAL_TYPE == ATMEGARFR2) || \
	(TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B))

retval_t tal_set_frequency_regs(uint8_t cc_band, uint8_t cc_number)
{
	tal_trx_status_t previous_trx_status = TRX_OFF;

#if (TAL_TYPE == AT86RF233) || (TAL_TYPE == ATMEGARFR2)
	/* check cc band and cc number fit in the range*/
	if ((cc_band != CC_BAND_8) && (cc_band != CC_BAND_9)) {
		return MAC_INVALID_PARAMETER;
	} else if ((cc_band == CC_BAND_8) &&
			(cc_number < MIN_CC_BAND_8_OFFSET)) {
		return MAC_INVALID_PARAMETER;
	} else if ((cc_band == CC_BAND_9) &&
			(cc_number > MIN_CC_BAND_9_OFFSET)) {
		return MAC_INVALID_PARAMETER;
	}

#elif ((TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B))
	/* check cc band and cc number fit in the range*/
	if (cc_band > MAX_CC_BAND) {
		return MAC_INVALID_PARAMETER;
	} else if (cc_band == CC_BAND_4 && cc_number > MIN_CC_BAND_4_OFFSET) {
		return MAC_INVALID_PARAMETER;
	} else if (cc_band == CC_BAND_5 && cc_number > MIN_CC_BAND_5_OFFSET) {
		return MAC_INVALID_PARAMETER;
	}
#endif /* End of (TAL_TYPE == AT86RF233) || (TAL_TYPE == ATMEGARFR2) */

	/*
	 * Set trx to trx_off to avoid interruption in ongoing
	 * transaction
	 */
	if (tal_trx_status != TRX_OFF) {
		previous_trx_status = RX_AACK_ON; /* any other than TRX_OFF
		                                   * state */
		do {
			/* set TRX_OFF until it could be set;
			 * trx might be busy */
		} while (set_trx_state(CMD_TRX_OFF) != TRX_OFF);
	}

	trx_bit_write(SR_CC_BAND, cc_band);
	trx_reg_write(RG_CC_CTRL_0, cc_number);

#if (TAL_TYPE == ATMEGARFA1)
	CONF_REG_WRITE();
#endif /* TAL_TYPE == ATMEGA128RFA1 */

	/* Re-store previous trx state */
	if (previous_trx_status != TRX_OFF) {
		/* Set to default state */
		set_trx_state(CMD_RX_AACK_ON);
	}

	/* check the values written in transceiver registers */
	if (trx_bit_read(SR_CC_BAND) != cc_band || \
			trx_reg_read(RG_CC_CTRL_0) != cc_number) {
		return FAILURE;
	} else {
		return MAC_SUCCESS;
	}
}

#endif /* End of ((TAL_TYPE == AT86RF233) || (TAL_TYPE == ATMEGARFR2) ||
        *        (TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B)) */

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
#if ((TAL_TYPE == AT86RF233) || (TAL_TYPE == ATMEGARFR2) || \
	(TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B))
retval_t tal_calculate_frequency(uint8_t cc_band, uint8_t cc_number,
		float *freq)
{
#if ((TAL_TYPE == AT86RF233) || (TAL_TYPE == ATMEGARFR2))
	/* check cc band and cc number fit in the range*/
	if ((cc_band != CC_BAND_8) && (cc_band != CC_BAND_9)) {
		return MAC_INVALID_PARAMETER;
	} else if ((cc_band == CC_BAND_8) &&
			(cc_number < MIN_CC_BAND_8_OFFSET)) {
		return MAC_INVALID_PARAMETER;
	} else if ((cc_band == CC_BAND_9) &&
			(cc_number > MIN_CC_BAND_9_OFFSET)) {
		return MAC_INVALID_PARAMETER;
	}

	*freq = (float)((cc_band == CC_BAND_8) ?
			(BASE_ISM_FREQUENCY_MHZ + 0.5 * cc_number) :
			(MID_ISM_FREQUENCY_MHZ + 0.5 * cc_number));

	return MAC_SUCCESS;

#elif ((TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B))
	/* check cc band and cc number fit in the range*/
	if (cc_band > MAX_CC_BAND) {
		return MAC_INVALID_PARAMETER;
	}

	/* calculate frequency based on cc band and cc number*/
	switch (cc_band) {
	case 0:
		break;

	case 1:
	{
		*freq = CC_1_START_FREQUENCY + (0.1 * cc_number);
	}
	break;

	case 2:
	{
		*freq = CC_2_START_FREQUENCY + (0.1 * cc_number);
	}
	break;

	case 3:
	{
		*freq = CC_3_START_FREQUENCY + (0.1 * cc_number);
	}
	break;

	case 4:
	{
		if (cc_number > MIN_CC_BAND_4_OFFSET) {
			return MAC_INVALID_PARAMETER;
		}

		*freq = CC_4_START_FREQUENCY +  cc_number;
	}
	break;

	case 5:
	{
		if (cc_number > MIN_CC_BAND_5_OFFSET) {
			return MAC_INVALID_PARAMETER;
		}

		*freq = CC_5_START_FREQUENCY + cc_number;
	}
	break;

#if (TAL_TYPE == AT86RF212)
	case 6:
		break;

#elif (TAL_TYPE == AT86RF212B)
	case 6:
		*freq = CC_6_START_FREQUENCY + (0.1 * cc_number);
		break;
#endif /* End of TAL_TYPE == AT86RF212 */
	default:
		return MAC_INVALID_PARAMETER;
	}

	return MAC_SUCCESS;
#endif /* End of ((TAL_TYPE == AT86RF233) || (TAL_TYPE == ATMEGARFR2))*/
}

#endif /* End of ((TAL_TYPE == AT86RF233) || (TAL_TYPE == ATMEGARFR2) ||
        *        (TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B))*/

/*
 * \brief Configures receiver sensitivity level
 *
 * \param pdt_level  0 to 15 levels of rx sensitivity
 * \param MAC_SUCCESS if sensitivity level is configured correctly
 *        MAC_INVALID_PARAMETER pdt_level is out of range
 *        FAILURE otherwise
 */
#if (TAL_TYPE != AT86RF230B)
retval_t tal_set_rx_sensitivity_level(uint8_t pdt_level)
{
	uint8_t temp;
	/* return invalid parameter if sensitivity level is out of range*/
	if (pdt_level > MAX_PDT_LEVEL) {
		return MAC_INVALID_PARAMETER;
	}

	/* configure sensitivity level*/
	trx_bit_write(SR_RX_PDT_LEVEL, pdt_level);
#if (TAL_TYPE == ATMEGARFA1)
	CONF_REG_WRITE();
#endif /* TAL_TYPE == ATMEGA128RFA1 */
	temp = trx_bit_read(SR_RX_PDT_LEVEL);
	if (temp == pdt_level) {
		return MAC_SUCCESS;
	} else {
		return FAILURE;
	}
}

#endif /* End of TAL_TYPE != AT86RF230B */

/*
 * \brief Configures promiscous mode in rx_aack_on mode
 *
 * \param prom_ctrl  true/false to enable/disable prom mode
 *
 * \param MAC_SUCCESS if rxaack_prom_mode is configured correctly
 *        FAILURE otherwise
 */
#if (TAL_TYPE != AT86RF230B)
retval_t tal_rxaack_prom_mode_ctrl(bool prom_ctrl)
{
	bool temp;
	/* configure promiscuous mode */
	trx_bit_write(SR_AACK_PROM_MODE, prom_ctrl);
#if (TAL_TYPE == ATMEGARFA1)
	CONF_REG_WRITE();
#endif /* TAL_TYPE == ATMEGA128RFA1 */
	temp = trx_bit_read(SR_AACK_PROM_MODE);
	if (temp == prom_ctrl) {
		return MAC_SUCCESS;
	} else {
		return FAILURE;
	}
}

#endif /* End of TAL_TYPE != AT86RF230B */

/*
 * \brief to get the current status of the transceiver
 *
 * \return status of the transceiver
 */
tal_trx_status_t tal_get_trx_status(void)
{
	tal_trx_status_t trx_status;
	/* Read the status from trx_status bits */
	trx_status = /* (tal_trx_status_t) */ trx_bit_read(SR_TRX_STATUS);
	return trx_status;
}

/*
 * \brief to read a particular transceiver register
 * \param reg_addr address of the transceiver register to be read
 * \param *data pointer to the location where the register value need to be
 *              stored
 * \return MAC_SUCCESS if the register is read correctly
 *         MAC_INVALID_PARAMETER if the reg_addr is out of range
 */

retval_t tal_trx_reg_read(uint16_t reg_addr, uint8_t *data)
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
retval_t  tal_trx_reg_write(uint16_t reg_addr, uint8_t value)
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

retval_t tal_get_curr_trx_config(param_type parameter, uint8_t *param_value)
{
	switch (parameter) {
#if ((TAL_TYPE != AT86RF230B) && (TAL_TYPE != AT86RF212))
	case ANT_DIVERSITY:
		*param_value = trx_bit_read(SR_ANT_DIV_EN);
		break;

	case ANT_SELECT:
		*param_value = trx_bit_read(SR_ANT_SEL);
		break;
#endif
#if (TAL_TYPE != AT86RF230B)
	case ANT_CTRL:
		*param_value = trx_bit_read(SR_ANT_CTRL);
		break;

	case AACK_PROMSCS_MODE:
		*param_value = trx_bit_read(SR_AACK_PROM_MODE);
		break;
#endif

#if ((TAL_TYPE == AT86RF233 || TAL_TYPE == ATMEGARFR2 \
		|| TAL_TYPE == AT86RF212 || TAL_TYPE == AT86RF212B))
	case CC_BAND:
		*param_value = trx_bit_read(SR_CC_BAND);
		break;

	case CC_NUMBER:
		*param_value = trx_reg_read(RG_CC_CTRL_0);
		break;
#endif
	case TX_PWR:
		*param_value = trx_bit_read(SR_TX_PWR);
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

retval_t tal_dump_registers(uint16_t start_addr, uint16_t end_addr,
		uint8_t *value)
{
	uint16_t i;
	int8_t length;

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
 * \brief to configure the reduced power consumption mode
 *
 * \param rpc_mode_sel value to be written in the TRX_RPC bits
 *
 * \return MAC_SUCCESS if the register is written correctly
 *         FAILURE otherwise
 */
#if ((TAL_TYPE == AT86RF233) || (TAL_TYPE == ATMEGARFR2))
retval_t tal_rpc_mode_config(uint8_t rpc_mode_sel)
{
	/*configure the rpc modes*/
	trx_reg_write(RG_TRX_RPC, rpc_mode_sel);
	/*check whether the configuration is done properly*/
	if (trx_reg_read(RG_TRX_RPC) == rpc_mode_sel) {
		return MAC_SUCCESS;
	} else {
		return FAILURE;
	}
}

#endif /* End of #if ((TAL_TYPE == AT86RF233) || (TAL_TYPE == ATMEGARFR2)) */

/*
 * \brief Converts a register value to a dBm value
 *
 * \param reg_value TransmitPower register value,
 *
 * \param *dbm_value pointer to dbm_value
 *
 * \return MAC_SUCCESS or FAILURE based on conversion is done or not
 */
#if ((TAL_TYPE != AT86RF212) && (TAL_TYPE != AT86RF212B))
retval_t tal_convert_reg_value_to_dBm(uint8_t reg_value, int8_t *dbm_value)
{
	if (reg_value < sizeof(tx_pwr_table)) {
		*dbm_value = (int8_t)PGM_READ_BYTE(&tx_pwr_table[reg_value]);
		return MAC_SUCCESS;
	} else {
		return FAILURE;
	}
}

#endif /* End of ((TAL_TYPE != AT86RF212) && (TAL_TYPE != AT86RF212B)) */

/*
 * \brief This function is called to get the base RSSI value for respective
 * radios
 *
 * \return value of the base RSSI value
 */
int8_t tal_get_rssi_base_val(void)
{
#if (TAL_TYPE == AT86RF212B)
	switch (tal_pib.CurrentPage) {
	case 0: /* BPSK */
		if (tal_pib.CurrentChannel == 0) {
			return(RSSI_BASE_VAL_BPSK_300_DBM);
		} else {
			return(RSSI_BASE_VAL_BPSK_300_DBM);
		}

	case 2: /* O-QPSK */
		if (tal_pib.CurrentChannel == 0) {
			return(RSSI_BASE_VAL_OQPSK_400_SIN_RC_DBM);
		} else {
			return(RSSI_BASE_VAL_OQPSK_400_SIN_RC_DBM);
		}

	case 5: /* Chinese band */
	default:    /* High data rate modes */
		return(RSSI_BASE_VAL_OQPSK_400_RC_DBM);
	}
#elif (TAL_TYPE == AT86RF212)
	switch (tal_pib.CurrentPage) {
	case 0: /* BPSK */
		if (tal_pib.CurrentChannel == 0) {
			return(RSSI_BASE_VAL_BPSK_20_DBM);
		} else {
			return(RSSI_BASE_VAL_BPSK_40_DBM);
		}

	case 2: /* O-QPSK */
		if (tal_pib.CurrentChannel == 0) {
			return(RSSI_BASE_VAL_OQPSK_100_DBM);
		} else {
			return(RSSI_BASE_VAL_OQPSK_SIN_250_DBM);
		}

	case 5: /* Chinese band */
	default:    /* High data rate modes */
		return(RSSI_BASE_VAL_OQPSK_RC_250_DBM);
	}
#elif ((TAL_TYPE == AT86RF230B) || (TAL_TYPE == AT86RF231) || \
	(TAL_TYPE == ATMEGARFA1) || (TAL_TYPE == AT86RF232) || (TAL_TYPE == \
	AT86RF233) || \
	(TAL_TYPE == ATMEGARFR2))
	return (RSSI_BASE_VAL_DBM);

#else
#error "Missing RSSI_BASE_VAL define"
#endif
}

/**
 * \brief Enable/Disable the rx safe mode
 *
 * \param safe_mode_ctrl true  if rx safe mode has to be enabled
 *
 * \return MAC_SUCCESS  if rx_safe_mode bit is configured correctly
 *         FAILURE      otherwise
 */
#if (TAL_TYPE != AT86RF230B)
retval_t tal_rxsafe_mode_ctrl(bool safe_mode_ctrl)
{
	/*configure rx_safe_mode */
	trx_bit_write(SR_RX_SAFE_MODE, safe_mode_ctrl); /* configure buffer
	                                                 * protection mode */
#if (TAL_TYPE == ATMEGARFA1)
	CONF_REG_WRITE();
#endif /* TAL_TYPE == ATMEGA128RFA1 */
	/*check the configuration */
	if (trx_bit_read(SR_RX_SAFE_MODE) == (uint8_t)safe_mode_ctrl) {
		return MAC_SUCCESS;
	} else {
		return FAILURE;
	}
}

#endif /* End of (TAL_TYPE != AT86RF230B) */
/* EOF */
