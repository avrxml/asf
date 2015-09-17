/**
 * \file
 *
 * \brief ATPL230 Physical layer - Registers
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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

#ifndef _ATPL230REG_INSTANCE_
#define _ATPL230REG_INSTANCE_

/**
 * \weakgroup phy_plc_group
 * @{
 */

/* ========== Register definition for ATPL230 peripheral ========== */
//! \name Mapped address for a phy special control register
//@{
#define REG_ATPL230_PHY_SFR                                   (0xFE2A)
//@}
//! \name System configuration
//@{
#define REG_ATPL230_SYS_CONFIG                                (0xFE2C)
//@}
//! \name Mapped address to peak_cut and emit_gain in transmission
//@{
#define REG_ATPL230_TXRXBUF_PEAK_CUT_GAIN_TX0                 (0xFE38)
#define REG_ATPL230_TXRXBUF_PEAK_CUT_GAIN_TX1                 (0xFE39)
#define REG_ATPL230_TXRXBUF_PEAK_CUT_GAIN_TX2                 (0xFE3A)
#define REG_ATPL230_TXRXBUF_PEAK_CUT_GAIN_TX3                 (0xFE3B)
#define REG_ATPL230_TXRXBUF_EMIT_GAIN_TX0                     (0xFE3C)
#define REG_ATPL230_TXRXBUF_EMIT_GAIN_TX1                     (0xFE3D)
#define REG_ATPL230_TXRXBUF_EMIT_GAIN_TX2                     (0xFE3E)
#define REG_ATPL230_TXRXBUF_EMIT_GAIN_TX3                     (0xFE3F)
//@}
//! \name Mapped addresses to keep the time reference from the last received beacon
//@{
#define REG_ATPL230_VHIGH_TIMER_BEACON_REF                    (0xFE47)
#define REG_ATPL230_HIGH_TIMER_BEACON_REF                     (0xFE48)
#define REG_ATPL230_MED_TIMER_BEACON_REF                      (0xFE49)
#define REG_ATPL230_LOW_TIMER_BEACON_REF                      (0xFE4A)
//@}
//! \name Filter Configuration
//@{
#define REG_ATPL230_FILTER_MIN_HIGH						(0xFE53)
#define REG_ATPL230_FILTER_MIN_MED						(0xFE54)
#define REG_ATPL230_FILTER_MIN_LOW						(0xFE55)
#define REG_ATPL230_TX_DELAY_SAMPLES					(0xFE57)
//@}
//! \name RSSI correction factors for AGC 1 and 2
//@{
#define REG_ATPL230_AGC0_KRSSI							(0xFE5C)
#define REG_ATPL230_AGC1_KRSSI							(0xFE5D)
//@}
//! \name RSSI correction factors for AGC 3 and 4
//@{
#define REG_ATPL230_AGC2_KRSSI							(0xFE5F)
#define REG_ATPL230_AGC3_KRSSI							(0xFE60)
//@}
//! \name Mapped addresses for the SNA
//@{
#define REG_ATPL230_SNA0								(0xFE62)
#define REG_ATPL230_SNA1								(0xFE63)
#define REG_ATPL230_SNA2								(0xFE64)
#define REG_ATPL230_SNA3								(0xFE65)
#define REG_ATPL230_SNA4								(0xFE66)
#define REG_ATPL230_SNA5								(0xFE67)
//@}
//! \name Mapped address for the physicial layer configuration
//@{
#define REG_ATPL230_PHY_CONFIG							(0xFE68)
//@}
//! \name Mapped addresses for emitter
//@{
#define REG_ATPL230_EMIT_CONFIG							(0xFE8F)
#define REG_ATPL230_AFE_CTL								(0xFE90)
//@}
//! \name Mapped addresses to configure the emitter
//@{
#define REG_ATPL230_N1_DELAY							(0xFE9F)
#define REG_ATPL230_P1_DELAY							(0xFEA0)
#define REG_ATPL230_N2_DELAY							(0xFEA1)
#define REG_ATPL230_P2_DELAY							(0xFEA2)
#define REG_ATPL230_EMIT1_ACTIVE						(0xFEA3)
#define REG_ATPL230_EMIT2_ACTIVE						(0xFEA4)
#define REG_ATPL230_EMIT3_ACTIVE						(0xFEA5)
#define REG_ATPL230_EMIT4_ACTIVE						(0xFEA6)
//@}
//! \name Mapped address for direct input buffer to FFT
//@{
#define REG_ATPL230_FFT_MODE							(0xFEB0)
//@}
//! \name Mapped addresses to configure automatic gain control
//@{
#define REG_ATPL230_AGC_THRESHOLD_HIGH					(0xFEB2)
#define REG_ATPL230_AGC_THRESHOLD_LOW					(0xFEB3)
#define REG_ATPL230_RX_TIME_OFFSET						(0xFEB4)
#define REG_ATPL230_ROBO_RX_TIME_OFFSET					(0xFEB7)
#define REG_ATPL230_CRC32_MAC_HIGH						(0xFEBA)
#define REG_ATPL230_CRC32_MAC_LOW						(0xFEBB)
#define REG_ATPL230_CRC8_MAC_HIGH						(0xFEBC)
#define REG_ATPL230_CRC8_MAC_LOW						(0xFEBD)
#define REG_ATPL230_CRC8_MAC_HD_HIGH					(0xFEC0)
#define REG_ATPL230_CRC8_MAC_HD_LOW						(0xFEC1)
#define REG_ATPL230_CRC8_PHY_HIGH						(0xFEC2)
#define REG_ATPL230_CRC8_PHY_LOW						(0xFEC3)
#define REG_ATPL230_POSITIVE_FAIL_CONFIG				(0xFEC4)
#define REG_ATPL230_POSITIVE_FAIL_HIGH					(0xFEC5)
#define REG_ATPL230_POSITIVE_FAIL_LOW					(0xFEC6)
#define REG_ATPL230_MAX_LEN_DBPSK						(0xFEC8)
#define REG_ATPL230_MAX_LEN_DBPSK_VTB					(0xFEC9)
#define REG_ATPL230_MAX_LEN_DQPSK						(0xFECA)
#define REG_ATPL230_MAX_LEN_DQPSK_VTB					(0xFECB)
#define REG_ATPL230_MAX_LEN_D8PSK						(0xFECC)
#define REG_ATPL230_MAX_LEN_D8PSK_VTB					(0xFECD)
#define REG_ATPL230_SOFT_STOP_TIMEH						(0xFECE)
#define REG_ATPL230_SOFT_STOP_TIMEL						(0xFECF)
#define REG_ATPL230_IIR_CONFIG							(0xFED3)
#define REG_ATPL230_EMITTER_FREC						(0xFEDB)
#define REG_ATPL230_MODE_CONJ							(0xFEE0)
#define REG_ATPL230_PASO_FASE_CORDIC_TX_VH				(0xFEFA)
#define REG_ATPL230_PASO_FASE_CORDIC_TX_H				(0xFEFB)
#define REG_ATPL230_PASO_FASE_CORDIC_TX_M				(0xFEFC)
#define REG_ATPL230_PASO_FASE_CORDIC_TX_L				(0xFEFD)
#define REG_ATPL230_AFE_1023							(0xFEFE)
//@}
//! \name Mapped address for hardware version
//@{
#define REG_ATPL230_VERSION								(0xFEFF)
//@}
//! \name HEADER PRIME13, PRIME PLUS compatible
//@{
#define REG_ATPL230_VCRC_POLY_VH						(0xFF0E)
#define REG_ATPL230_VCRC_POLY_H							(0xFF0F)
#define REG_ATPL230_VCRC_POLY_M							(0xFF10)
#define REG_ATPL230_VCRC_POLY_L							(0xFF11)
#define REG_ATPL230_VCRC_RST_VH							(0xFF12)
#define REG_ATPL230_VCRC_RST_H							(0xFF13)
#define REG_ATPL230_VCRC_RST_M							(0xFF14)
#define REG_ATPL230_VCRC_RST_L							(0xFF15)
#define REG_ATPL230_VCRC_CONF							(0xFF16)
#define REG_ATPL230_VCRC_INPUT							(0xFF17)
#define REG_ATPL230_VCRC_CTL							(0xFF18)
#define REG_ATPL230_VCRC_CRC_VH							(0xFF19)
#define REG_ATPL230_VCRC_CRC_H							(0xFF1A)
#define REG_ATPL230_VCRC_CRC_M							(0xFF1B)
#define REG_ATPL230_VCRC_CRC_L							(0xFF1C)
#define REG_ATPL230_SOFT_TIME_X1_H						(0xFF33)
#define REG_ATPL230_SOFT_TIME_X1_L						(0xFF34)
#define REG_ATPL230_SOFT_TIME_X2_H						(0xFF35)
#define REG_ATPL230_SOFT_TIME_X2_L						(0xFF36)
#define REG_ATPL230_SOFT_TIME_Y1						(0xFF37)
#define REG_ATPL230_SOFT_TIME_Y2						(0xFF38)
#define REG_ATPL230_FACTOR_COMP_DOWN_ROBO				(0xFF3A)
#define REG_ATPL230_FACTOR_COMP_UP_ROBO					(0xFF3B)
#define REG_ATPL230_FT_TIME_DOWN_ROBO					(0xFF3C)
#define REG_ATPL230_FT_TIME_UP_ROBO						(0xFF3D)
#define REG_ATPL230_FT_STEP_UP_ROBO						(0xFF3E)
#define REG_ATPL230_FT_STEP_UP_FD_ROBO					(0xFF3F)
#define REG_ATPL230_FT_STEP_DOWN_ROBO					(0xFF40)
#define REG_ATPL230_FT_MIN_FACTOR_TH_ROBO				(0xFF41)
#define REG_ATPL230_FT_MAX_FACTOR_TH_ROBO				(0xFF42)
#define REG_ATPL230_FT_CORR_MIN_ROBO					(0xFF43)
#define REG_ATPL230_RSSI_OFFSET							(0xFF4C)
#define REG_ATPL230_FACTOR_THRESHOLD_4_AND_1 			(0xFF51)
#define REG_ATPL230_INI_CHIRP1							(0xFF52)
#define REG_ATPL230_INI_CHIRP2							(0xFF53)
#define REG_ATPL230_FACTOR_ROBUST						(0xFF54)
#define REG_ATPL230_FACTOR_THRESHOLD_3_AND_2 			(0xFF55)
#define REG_ATPL230_FACTOR_THRESHOLD_2_3_AND_1_3 		(0xFF56)
#define REG_ATPL230_FACTOR_ROBUST_3_2					(0xFF58)
#define REG_ATPL230_EQUALIZE_H							(0xFF5E)
#define REG_ATPL230_EQUALIZE_L							(0xFF5F)
#define REG_ATPL230_AGC_CTL_AUX							(0xFF75)
#define REG_ATPL230_AGC_CTL								(0xFF76)
#define REG_ATPL230_LOAD_ADDRH							(0xFF78)
#define REG_ATPL230_LOAD_ADDRL							(0xFF79)
#define REG_ATPL230_LOAD_CTL							(0xFF80)
#define REG_ATPL230_N_MOD								(0xFF81)
#define REG_ATPL230_N_RAMPH								(0xFF82)
#define REG_ATPL230_N_RAMPL								(0xFF83)
#define REG_ATPL230_N_CHIRP								(0xFF84)
#define REG_ATPL230_STEP_M_UP							(0xFF85)
#define REG_ATPL230_STEP_M_DOWN							(0xFF86)
#define REG_ATPL230_MIN_M_TH_HIGH						(0xFF87)
#define REG_ATPL230_MIN_M_TH_LOW						(0xFF88)
#define REG_ATPL230_STEP_R_UP							(0xFF89)
#define REG_ATPL230_STEP_R_DOWN							(0xFF8A)
#define REG_ATPL230_MIN_R_TH							(0xFF8B)
#define REG_ATPL230_R_MARGIN							(0xFF8C)
#define REG_ATPL230_CD_CONTROL							(0xFF8D)
#define REG_ATPL230_FT_SUMADOR_CONF						(0xFF93)
#define REG_ATPL230_FACTOR_COMP_DOWN					(0xFF94)
#define REG_ATPL230_FACTOR_THR_CTL						(0xFF95)
#define REG_ATPL230_FACTOR_COMP_UP						(0xFF97)
#define REG_ATPL230_FT_TIME_DOWN						(0xFF98)
#define REG_ATPL230_FT_TIME_UP							(0xFF99)
#define REG_ATPL230_FT_STEP_UP							(0xFF9A)
#define REG_ATPL230_FT_STEP_UP_FD						(0xFF9B)
#define REG_ATPL230_FT_STEP_DOWN						(0xFF9C)
#define REG_ATPL230_FT_MIN_FACTOR_TH					(0xFF9D)
#define REG_ATPL230_FT_MAX_FACTOR_TH					(0xFF9E)
#define REG_ATPL230_FT_CORR_MIN							(0xFF9F)
//@}
//! \name Mapped addresses for emission time
//@{
#define REG_ATPL230_TXRXBUF_EMITIME1_TX0 				(0xFD00)
#define REG_ATPL230_TXRXBUF_EMITIME2_TX0 				(0xFD01)
#define REG_ATPL230_TXRXBUF_EMITIME3_TX0 				(0xFD02)
#define REG_ATPL230_TXRXBUF_EMITIME4_TX0 				(0xFD03)
#define REG_ATPL230_TXRXBUF_EMITIME1_TX1 				(0xFD04)
#define REG_ATPL230_TXRXBUF_EMITIME2_TX1 				(0xFD05)
#define REG_ATPL230_TXRXBUF_EMITIME3_TX1 				(0xFD06)
#define REG_ATPL230_TXRXBUF_EMITIME4_TX1 				(0xFD07)
#define REG_ATPL230_TXRXBUF_EMITIME1_TX2 				(0xFD08)
#define REG_ATPL230_TXRXBUF_EMITIME2_TX2 				(0xFD09)
#define REG_ATPL230_TXRXBUF_EMITIME3_TX2 				(0xFD0A)
#define REG_ATPL230_TXRXBUF_EMITIME4_TX2 				(0xFD0B)
#define REG_ATPL230_TXRXBUF_EMITIME1_TX3 				(0xFD0C)
#define REG_ATPL230_TXRXBUF_EMITIME2_TX3 				(0xFD0D)
#define REG_ATPL230_TXRXBUF_EMITIME3_TX3 				(0xFD0E)
#define REG_ATPL230_TXRXBUF_EMITIME4_TX3 				(0xFD0F)
//@}
//! \name Mapped addresses for time to commute HIMP after transmission
//@{
#define REG_ATPL230_TXRXBUF_TXRX_TA1_TX0 				(0xFD10)
#define REG_ATPL230_TXRXBUF_TXRX_TA2_TX0 				(0xFD11)
#define REG_ATPL230_TXRXBUF_TXRX_TA1_TX1 				(0xFD12)
#define REG_ATPL230_TXRXBUF_TXRX_TA2_TX1 				(0xFD13)
#define REG_ATPL230_TXRXBUF_TXRX_TA1_TX2 				(0xFD14)
#define REG_ATPL230_TXRXBUF_TXRX_TA2_TX2 				(0xFD15)
#define REG_ATPL230_TXRXBUF_TXRX_TA1_TX3 				(0xFD16)
#define REG_ATPL230_TXRXBUF_TXRX_TA2_TX3 				(0xFD17)
//@}
//! \name Mapped addresses for time to commute HIMP before transmission
//@{
#define REG_ATPL230_TXRXBUF_TXRX_TB1_TX0 				(0xFD18)
#define REG_ATPL230_TXRXBUF_TXRX_TB2_TX0 				(0xFD19)
#define REG_ATPL230_TXRXBUF_TXRX_TB1_TX1 				(0xFD1A)
#define REG_ATPL230_TXRXBUF_TXRX_TB2_TX1 				(0xFD1B)
#define REG_ATPL230_TXRXBUF_TXRX_TB1_TX2 				(0xFD1C)
#define REG_ATPL230_TXRXBUF_TXRX_TB2_TX2 				(0xFD1D)
#define REG_ATPL230_TXRXBUF_TXRX_TB1_TX3 				(0xFD1E)
#define REG_ATPL230_TXRXBUF_TXRX_TB2_TX3 				(0xFD1F)
//@}
//! \name Mapped addresses for global amplitude
//@{
#define REG_ATPL230_TXRXBUF_GLOBAL_AMP_TX0 				(0xFD20)
#define REG_ATPL230_TXRXBUF_GLOBAL_AMP_TX1 				(0xFD21)
#define REG_ATPL230_TXRXBUF_GLOBAL_AMP_TX2 				(0xFD22)
#define REG_ATPL230_TXRXBUF_GLOBAL_AMP_TX3 				(0xFD23)
//@}
//! \name Mapped addresses for signal amplitude
//@{
#define REG_ATPL230_TXRXBUF_SIGNAL_AMP_TX0 				(0xFD24)
#define REG_ATPL230_TXRXBUF_SIGNAL_AMP_TX1 				(0xFD25)
#define REG_ATPL230_TXRXBUF_SIGNAL_AMP_TX2 				(0xFD26)
#define REG_ATPL230_TXRXBUF_SIGNAL_AMP_TX3 				(0xFD27)
//@}
//! \name Mapped addresses for chirp amplitude
//@{
#define REG_ATPL230_TXRXBUF_CHIRP_AMP_TX0 				(0xFD28)
#define REG_ATPL230_TXRXBUF_CHIRP_AMP_TX1 				(0xFD29)
#define REG_ATPL230_TXRXBUF_CHIRP_AMP_TX2 				(0xFD2A)
#define REG_ATPL230_TXRXBUF_CHIRP_AMP_TX3 				(0xFD2B)
//@}
//! \name Mapped addresses for transmission timeout value
//@{
#define REG_ATPL230_TXRXBUF_TIMEOUT1_TX0 				(0xFD2C)
#define REG_ATPL230_TXRXBUF_TIMEOUT2_TX0 				(0xFD2D)
#define REG_ATPL230_TXRXBUF_TIMEOUT3_TX0 				(0xFD2E)
#define REG_ATPL230_TXRXBUF_TIMEOUT4_TX0 				(0xFD2F)
#define REG_ATPL230_TXRXBUF_TIMEOUT1_TX1 				(0xFD30)
#define REG_ATPL230_TXRXBUF_TIMEOUT2_TX1 				(0xFD31)
#define REG_ATPL230_TXRXBUF_TIMEOUT3_TX1 				(0xFD32)
#define REG_ATPL230_TXRXBUF_TIMEOUT4_TX1 				(0xFD33)
#define REG_ATPL230_TXRXBUF_TIMEOUT1_TX2 				(0xFD34)
#define REG_ATPL230_TXRXBUF_TIMEOUT2_TX2 				(0xFD35)
#define REG_ATPL230_TXRXBUF_TIMEOUT3_TX2 				(0xFD36)
#define REG_ATPL230_TXRXBUF_TIMEOUT4_TX2 				(0xFD37)
#define REG_ATPL230_TXRXBUF_TIMEOUT1_TX3 				(0xFD38)
#define REG_ATPL230_TXRXBUF_TIMEOUT2_TX3 				(0xFD39)
#define REG_ATPL230_TXRXBUF_TIMEOUT3_TX3 				(0xFD3A)
#define REG_ATPL230_TXRXBUF_TIMEOUT4_TX3 				(0xFD3B)
//@}
//! \name Mapped addresses for transmission configuration
//@{
#define REG_ATPL230_TXRXBUF_TXCONF_TX0 					(0xFD3C)
#define REG_ATPL230_TXRXBUF_TXCONF_TX1 					(0xFD3D)
#define REG_ATPL230_TXRXBUF_TXCONF_TX2 					(0xFD3E)
#define REG_ATPL230_TXRXBUF_TXCONF_TX3 					(0xFD3F)
//@}
//! \name Mapped addresses for initial address for every transmission buffer
//@{
#define REG_ATPL230_TXRXBUF_INITAD1_TX0 				(0xFD40)
#define REG_ATPL230_TXRXBUF_INITAD2_TX0 				(0xFD41)
#define REG_ATPL230_TXRXBUF_INITAD1_TX1 				(0xFD42)
#define REG_ATPL230_TXRXBUF_INITAD2_TX1 				(0xFD43)
#define REG_ATPL230_TXRXBUF_INITAD1_TX2 				(0xFD44)
#define REG_ATPL230_TXRXBUF_INITAD2_TX2 				(0xFD45)
#define REG_ATPL230_TXRXBUF_INITAD1_TX3 				(0xFD46)
#define REG_ATPL230_TXRXBUF_INITAD2_TX3 				(0xFD47)
//@}
//! \name Mapped addresses for RMS calculation after transmission
//@{
#define REG_ATPL230_TXRXBUF_RMSCALC1_TX0 				(0xFD48)
#define REG_ATPL230_TXRXBUF_RMSCALC2_TX0 				(0xFD49)
#define REG_ATPL230_TXRXBUF_RMSCALC1_TX1 				(0xFD4A)
#define REG_ATPL230_TXRXBUF_RMSCALC2_TX1 				(0xFD4B)
#define REG_ATPL230_TXRXBUF_RMSCALC1_TX2 				(0xFD4C)
#define REG_ATPL230_TXRXBUF_RMSCALC2_TX2 				(0xFD4D)
#define REG_ATPL230_TXRXBUF_RMSCALC1_TX3 				(0xFD4E)
#define REG_ATPL230_TXRXBUF_RMSCALC2_TX3 				(0xFD4F)
//@}
//! \name Mapped addresses for transmission result
//@{
#define REG_ATPL230_TXRXBUF_RESULT_TX10 				(0xFD50)
#define REG_ATPL230_TXRXBUF_RESULT_TX32 				(0xFD51)
//@}
//! \name Mapped addresses for transmission interrupt flags to read and to erase
//@{
#define REG_ATPL230_TXRXBUF_TX_INT 					(0xFD52)
//@}
//! \name Mapped addresses for Viterbi soft Bit Error Rate value
//@{
#define REG_ATPL230_TXRXBUF_BERSOFT_RX0 				(0xFD53)
#define REG_ATPL230_TXRXBUF_BERSOFT_RX1 				(0xFD54)
#define REG_ATPL230_TXRXBUF_BERSOFT_RX2 				(0xFD55)
#define REG_ATPL230_TXRXBUF_BERSOFT_RX3 				(0xFD56)
//@}
//! \name Mapped addresses for Viterbi soft Bit Error Rate average value
//@{
#define REG_ATPL230_TXRXBUF_BERSOFT_AVG_RX0 			(0xFD57)
#define REG_ATPL230_TXRXBUF_BERSOFT_AVG_RX1 			(0xFD58)
#define REG_ATPL230_TXRXBUF_BERSOFT_AVG_RX2 			(0xFD59)
#define REG_ATPL230_TXRXBUF_BERSOFT_AVG_RX3 			(0xFD5A)
//@}
//! \name Mapped addresses for Viterbi soft Bit Error Rate maximum value
//@{
#define REG_ATPL230_TXRXBUF_BERSOFT_MAX_RX0 			(0xFD5B)
#define REG_ATPL230_TXRXBUF_BERSOFT_MAX_RX1 			(0xFD5C)
#define REG_ATPL230_TXRXBUF_BERSOFT_MAX_RX2 			(0xFD5D)
#define REG_ATPL230_TXRXBUF_BERSOFT_MAX_RX3 			(0xFD5E)
//@}
//! Mapped addresses for Viterbi hard Bit Error Rate value
//@{
#define REG_ATPL230_TXRXBUF_BERHARD_RX0 				(0xFD5F)
#define REG_ATPL230_TXRXBUF_BERHARD_RX1 				(0xFD60)
#define REG_ATPL230_TXRXBUF_BERHARD_RX2 				(0xFD61)
#define REG_ATPL230_TXRXBUF_BERHARD_RX3 				(0xFD62)
//@}
//! \name Mapped addresses for Viterbi hard Bit Error Rate average value
//@
#define REG_ATPL230_TXRXBUF_BERHARD_AVG_RX0 			(0xFD63)
#define REG_ATPL230_TXRXBUF_BERHARD_AVG_RX1 			(0xFD64)
#define REG_ATPL230_TXRXBUF_BERHARD_AVG_RX2 			(0xFD65)
#define REG_ATPL230_TXRXBUF_BERHARD_AVG_RX3 			(0xFD66)
//@}
//! \name Mapped addresses for Viterbi hard Bit Error Rate maximum value
//@{
#define REG_ATPL230_TXRXBUF_BERHARD_MAX_RX0 			(0xFD67)
#define REG_ATPL230_TXRXBUF_BERHARD_MAX_RX1 			(0xFD68)
#define REG_ATPL230_TXRXBUF_BERHARD_MAX_RX2 			(0xFD69)
#define REG_ATPL230_TXRXBUF_BERHARD_MAX_RX3 			(0xFD6A)
//@}
//! \name Mapped addresses for RSSI minimum value
//@{
#define REG_ATPL230_TXRXBUF_RSSIMIN_RX0 				(0xFD6B)
#define REG_ATPL230_TXRXBUF_RSSIMIN_RX1 				(0xFD6C)
#define REG_ATPL230_TXRXBUF_RSSIMIN_RX2 				(0xFD6D)
#define REG_ATPL230_TXRXBUF_RSSIMIN_RX3 				(0xFD6E)
//@}
//! \name Mapped addresses for RSSI average value
//@{
#define REG_ATPL230_TXRXBUF_RSSIAVG_RX0 				(0xFD6F)
#define REG_ATPL230_TXRXBUF_RSSIAVG_RX1 				(0xFD70)
#define REG_ATPL230_TXRXBUF_RSSIAVG_RX2 				(0xFD71)
#define REG_ATPL230_TXRXBUF_RSSIAVG_RX3 				(0xFD72)
//@}
//! \name Mapped addresses for RSSI maximum value
//@{
#define REG_ATPL230_TXRXBUF_RSSIMAX_RX0 				(0xFD73)
#define REG_ATPL230_TXRXBUF_RSSIMAX_RX1 				(0xFD74)
#define REG_ATPL230_TXRXBUF_RSSIMAX_RX2 				(0xFD75)
#define REG_ATPL230_TXRXBUF_RSSIMAX_RX3 				(0xFD76)
//@}
//! \name Mapped addresses for CINR minimum value
//@{
#define REG_ATPL230_TXRXBUF_CINRMIN_RX0 				(0xFD77)
#define REG_ATPL230_TXRXBUF_CINRMIN_RX1 				(0xFD78)
#define REG_ATPL230_TXRXBUF_CINRMIN_RX2 				(0xFD79)
#define REG_ATPL230_TXRXBUF_CINRMIN_RX3 				(0xFD7A)
//@}
//! \name Mapped addresses for CINR average value
//@{
#define REG_ATPL230_TXRXBUF_CINRAVG_RX0 				(0xFD7B)
#define REG_ATPL230_TXRXBUF_CINRAVG_RX1 				(0xFD7C)
#define REG_ATPL230_TXRXBUF_CINRAVG_RX2 				(0xFD7D)
#define REG_ATPL230_TXRXBUF_CINRAVG_RX3 				(0xFD7E)
//@}
//! \name Mapped addresses for CINR maximum value
//@{
#define REG_ATPL230_TXRXBUF_CINRMAX_RX0 				(0xFD7F)
#define REG_ATPL230_TXRXBUF_CINRMAX_RX1 				(0xFD80)
#define REG_ATPL230_TXRXBUF_CINRMAX_RX2 				(0xFD81)
#define REG_ATPL230_TXRXBUF_CINRMAX_RX3 				(0xFD82)
//@}
//! \name Mapped addresses for reception time for every buffer
//@{
#define REG_ATPL230_TXRXBUF_RECTIME1_RX0 				(0xFD83)
#define REG_ATPL230_TXRXBUF_RECTIME2_RX0 				(0xFD84)
#define REG_ATPL230_TXRXBUF_RECTIME3_RX0 				(0xFD85)
#define REG_ATPL230_TXRXBUF_RECTIME4_RX0 				(0xFD86)
#define REG_ATPL230_TXRXBUF_RECTIME1_RX1 				(0xFD87)
#define REG_ATPL230_TXRXBUF_RECTIME2_RX1 				(0xFD88)
#define REG_ATPL230_TXRXBUF_RECTIME3_RX1 				(0xFD89)
#define REG_ATPL230_TXRXBUF_RECTIME4_RX1 				(0xFD8A)
#define REG_ATPL230_TXRXBUF_RECTIME1_RX2 				(0xFD8B)
#define REG_ATPL230_TXRXBUF_RECTIME2_RX2 				(0xFD8C)
#define REG_ATPL230_TXRXBUF_RECTIME3_RX2 				(0xFD8D)
#define REG_ATPL230_TXRXBUF_RECTIME4_RX2 				(0xFD8E)
#define REG_ATPL230_TXRXBUF_RECTIME1_RX3 				(0xFD8F)
#define REG_ATPL230_TXRXBUF_RECTIME2_RX3 				(0xFD90)
#define REG_ATPL230_TXRXBUF_RECTIME3_RX3 				(0xFD91)
#define REG_ATPL230_TXRXBUF_RECTIME4_RX3 				(0xFD92)
//@}
//! \name Mapped addresses for last zero cross time
//@{
#define REG_ATPL230_TXRXBUF_ZCT1_RX0 					(0xFD93)
#define REG_ATPL230_TXRXBUF_ZCT2_RX0 					(0xFD94)
#define REG_ATPL230_TXRXBUF_ZCT3_RX0 					(0xFD95)
#define REG_ATPL230_TXRXBUF_ZCT4_RX0 					(0xFD96)
#define REG_ATPL230_TXRXBUF_ZCT1_RX1 					(0xFD97)
#define REG_ATPL230_TXRXBUF_ZCT2_RX1 					(0xFD98)
#define REG_ATPL230_TXRXBUF_ZCT3_RX1 					(0xFD99)
#define REG_ATPL230_TXRXBUF_ZCT4_RX1 					(0xFD9A)
#define REG_ATPL230_TXRXBUF_ZCT1_RX2 					(0xFD9B)
#define REG_ATPL230_TXRXBUF_ZCT2_RX2 					(0xFD9C)
#define REG_ATPL230_TXRXBUF_ZCT3_RX2 					(0xFD9D)
#define REG_ATPL230_TXRXBUF_ZCT4_RX2 					(0xFD9E)
#define REG_ATPL230_TXRXBUF_ZCT1_RX3 					(0xFD9F)
#define REG_ATPL230_TXRXBUF_ZCT2_RX3 					(0xFDA0)
#define REG_ATPL230_TXRXBUF_ZCT3_RX3 					(0xFDA1)
#define REG_ATPL230_TXRXBUF_ZCT4_RX3 					(0xFDA2)
//@}
//! \name Mapped addresses for Error Vector Magnitude from header
//@{
#define REG_ATPL230_TXRXBUF_EVM_HEADER1_RX0 			(0xFDA3)
#define REG_ATPL230_TXRXBUF_EVM_HEADER2_RX0 			(0xFDA4)
#define REG_ATPL230_TXRXBUF_EVM_HEADER1_RX1 			(0xFDA5)
#define REG_ATPL230_TXRXBUF_EVM_HEADER2_RX1 			(0xFDA6)
#define REG_ATPL230_TXRXBUF_EVM_HEADER1_RX2 			(0xFDA7)
#define REG_ATPL230_TXRXBUF_EVM_HEADER2_RX2 			(0xFDA8)
#define REG_ATPL230_TXRXBUF_EVM_HEADER1_RX3 			(0xFDA9)
#define REG_ATPL230_TXRXBUF_EVM_HEADER2_RX3 			(0xFDAA)
//@}
//! \name Mapped addresses for Error Vector Magnitude from payload
//@{
#define REG_ATPL230_TXRXBUF_EVM_PAYLOAD1_RX0			(0xFDAB)
#define REG_ATPL230_TXRXBUF_EVM_PAYLOAD2_RX0			(0xFDAC)
#define REG_ATPL230_TXRXBUF_EVM_PAYLOAD1_RX1			(0xFDAD)
#define REG_ATPL230_TXRXBUF_EVM_PAYLOAD2_RX1			(0xFDAE)
#define REG_ATPL230_TXRXBUF_EVM_PAYLOAD1_RX2			(0xFDAF)
#define REG_ATPL230_TXRXBUF_EVM_PAYLOAD2_RX2			(0xFDB0)
#define REG_ATPL230_TXRXBUF_EVM_PAYLOAD1_RX3			(0xFDB1)
#define REG_ATPL230_TXRXBUF_EVM_PAYLOAD2_RX3			(0xFDB2)
//@}
//! \name Mapped addresses for accumulated Error Vector Magnitude from header
//@{
#define REG_ATPL230_TXRXBUF_EVM_HEADACUM1_RX0 			(0xFDB3)
#define REG_ATPL230_TXRXBUF_EVM_HEADACUM2_RX0 			(0xFDB4)
#define REG_ATPL230_TXRXBUF_EVM_HEADACUM3_RX0 			(0xFDB5)
#define REG_ATPL230_TXRXBUF_EVM_HEADACUM4_RX0 			(0xFDB6)
#define REG_ATPL230_TXRXBUF_EVM_HEADACUM1_RX1 			(0xFDB7)
#define REG_ATPL230_TXRXBUF_EVM_HEADACUM2_RX1 			(0xFDB8)
#define REG_ATPL230_TXRXBUF_EVM_HEADACUM3_RX1 			(0xFDB9)
#define REG_ATPL230_TXRXBUF_EVM_HEADACUM4_RX1 			(0xFDBA)
#define REG_ATPL230_TXRXBUF_EVM_HEADACUM1_RX2 			(0xFDBB)
#define REG_ATPL230_TXRXBUF_EVM_HEADACUM2_RX2 			(0xFDBC)
#define REG_ATPL230_TXRXBUF_EVM_HEADACUM3_RX2 			(0xFDBD)
#define REG_ATPL230_TXRXBUF_EVM_HEADACUM4_RX2 			(0xFDBE)
#define REG_ATPL230_TXRXBUF_EVM_HEADACUM1_RX3 			(0xFDBF)
#define REG_ATPL230_TXRXBUF_EVM_HEADACUM2_RX3 			(0xFDC0)
#define REG_ATPL230_TXRXBUF_EVM_HEADACUM3_RX3 			(0xFDC1)
#define REG_ATPL230_TXRXBUF_EVM_HEADACUM4_RX3 			(0xFDC2)
//@}
//! \name Mapped addresses for accumulated Error Vector Magnitude from payload
//@{
#define REG_ATPL230_TXRXBUF_EVM_PAYLACUM1_RX0 			(0xFDC3)
#define REG_ATPL230_TXRXBUF_EVM_PAYLACUM2_RX0 			(0xFDC4)
#define REG_ATPL230_TXRXBUF_EVM_PAYLACUM3_RX0 			(0xFDC5)
#define REG_ATPL230_TXRXBUF_EVM_PAYLACUM4_RX0 			(0xFDC6)
#define REG_ATPL230_TXRXBUF_EVM_PAYLACUM1_RX1 			(0xFDC7)
#define REG_ATPL230_TXRXBUF_EVM_PAYLACUM2_RX1 			(0xFDC8)
#define REG_ATPL230_TXRXBUF_EVM_PAYLACUM3_RX1 			(0xFDC9)
#define REG_ATPL230_TXRXBUF_EVM_PAYLACUM4_RX1 			(0xFDCA)
#define REG_ATPL230_TXRXBUF_EVM_PAYLACUM1_RX2 			(0xFDCB)
#define REG_ATPL230_TXRXBUF_EVM_PAYLACUM2_RX2 			(0xFDCC)
#define REG_ATPL230_TXRXBUF_EVM_PAYLACUM3_RX2 			(0xFDCD)
#define REG_ATPL230_TXRXBUF_EVM_PAYLACUM4_RX2 			(0xFDCE)
#define REG_ATPL230_TXRXBUF_EVM_PAYLACUM1_RX3 			(0xFDCF)
#define REG_ATPL230_TXRXBUF_EVM_PAYLACUM2_RX3 			(0xFDD0)
#define REG_ATPL230_TXRXBUF_EVM_PAYLACUM3_RX3 			(0xFDD1)
#define REG_ATPL230_TXRXBUF_EVM_PAYLACUM4_RX3 			(0xFDD2)
//@}
//! \name Mapped addresses for reception buffer selection
//@{
#define REG_ATPL230_TXRXBUF_SELECT_BUFF_RX 				(0xFDD3)
//@}
//! \name Mapped addresses for reception interrupt flags to read
//@{
#define REG_ATPL230_TXRXBUF_RX_INT 					(0xFDD4)
//@}
//! \name Mapped addresses for reception configuration
//@{
#define REG_ATPL230_TXRXBUF_RXCONF 					(0xFDD5)
//@}
//! \name Mapped addresses for initial address for every reception buffer
//@{
#define REG_ATPL230_TXRXBUF_INITAD1_RX0 				(0xFDD6)
#define REG_ATPL230_TXRXBUF_INITAD2_RX0 				(0xFDD7)
#define REG_ATPL230_TXRXBUF_INITAD1_RX1 				(0xFDD8)
#define REG_ATPL230_TXRXBUF_INITAD2_RX1 				(0xFDD9)
#define REG_ATPL230_TXRXBUF_INITAD1_RX2 				(0xFDDA)
#define REG_ATPL230_TXRXBUF_INITAD2_RX2 				(0xFDDB)
#define REG_ATPL230_TXRXBUF_INITAD1_RX3 				(0xFDDC)
#define REG_ATPL230_TXRXBUF_INITAD2_RX3 				(0xFDDD)
//@}
//! \name Mapped addresses for busy channel statistics
//@{
#define REG_ATPL230_TXRXBUF_CDONSTA1 					(0xFDDE)
#define REG_ATPL230_TXRXBUF_CDONSTA2 					(0xFDDF)
#define REG_ATPL230_TXRXBUF_CDONSTA3 					(0xFDE0)
#define REG_ATPL230_TXRXBUF_CDONSTA4 					(0xFDE1)
//@}
//! \name Mapped addresses for free channel statistics
//@{
#define REG_ATPL230_TXRXBUF_CDOFFSTA1 					(0xFDE2)
#define REG_ATPL230_TXRXBUF_CDOFFSTA2 					(0xFDE3)
#define REG_ATPL230_TXRXBUF_CDOFFSTA3 					(0xFDE4)
#define REG_ATPL230_TXRXBUF_CDOFFSTA4 					(0xFDE5)
//@}
//! \name Mapped addresses for number of AGCs activated in reception
//@{
#define REG_ATPL230_NUM_AGCS_RX0						(0xFDE6)
#define REG_ATPL230_NUM_AGCS_RX1						(0xFDE7)
#define REG_ATPL230_NUM_AGCS_RX2						(0xFDE8)
#define REG_ATPL230_NUM_AGCS_RX3						(0xFDE9)
//@}
//! \name Mapped addresses for robo mode tx/rx
//@{
#define REG_ATPL230_TXRXBUF_TXCONF_ROBO_CTL				(0xFDF2)
#define REG_ATPL230_TXRXBUF_RXCONF_INFO_ROBO_MODE		(0xFDF3)
//@}
//! \name Mapped addresses for noise mode
//@{
#define REG_ATPL230_TXRXBUF_RECTIME_NOISE1				(0xFDF4)
#define REG_ATPL230_TXRXBUF_RECTIME_NOISE2				(0xFDF5)
#define REG_ATPL230_TXRXBUF_RECTIME_NOISE3				(0xFDF6)
#define REG_ATPL230_TXRXBUF_RECTIME_NOISE4				(0xFDF7)
#define REG_ATPL230_TXRXBUF_INITREG_ATPL230_NOISE1		(0xFDF8)
#define REG_ATPL230_TXRXBUF_INITREG_ATPL230_NOISE2		(0xFDF9)
#define REG_ATPL230_TXRXBUF_NOISECONF					(0xFDFA)
//@}
//! \name Mapped addresses for selection transmision branch
//@{
#define REG_ATPL230_TXRXBUF_TXCONF_SELBRANCH			(0xFDFB)
//@}
//! \name Mapped addresses for RSSI in noise reception
//@{
#define REG_ATPL230_TXRXBUF_RSSIMIN_NOISE 				(0xFDFC)
#define REG_ATPL230_TXRXBUF_RSSIAVG_NOISE 				(0xFDFD)
#define REG_ATPL230_TXRXBUF_RSSIMAX_NOISE 				(0xFDFE)
//@}
//! \name Mapped addresses for number of AGCs activated in noise reception
//@
#define REG_ATPL230_NUM_AGCS_NOISE	 					(0xFDFF)
//@}

//! \name ATPL230 MAC CRC processing
//@{
#define ATPL230_MAC_EN		(0x1u)
#define ATPL230_MAC_EN_Pos	0
#define ATPL230_MAC_EN_Msk	(ATPL230_MAC_EN << ATPL230_MAC_EN_Pos)
//@}

//! \name System Configuration Register
//@{
#define ATPL230_SYS_CONFIG_RST					(0x1u)
#define ATPL230_SYS_CONFIG_RST_Pos				0
#define ATPL230_SYS_CONFIG_RST_Msk				(ATPL230_SYS_CONFIG_RST << ATPL230_SYS_CONFIG_RST_Pos)		/**< \brief (ATPL230) Physical Layer Reset  */
#define ATPL230_SYS_CONFIG_ERR					(0x1u)
#define ATPL230_SYS_CONFIG_ERR_Pos				1
#define ATPL230_SYS_CONFIG_ERR_Msk				(ATPL230_SYS_CONFIG_ERR << ATPL230_SYS_CONFIG_ERR_Pos)		/**< \brief (ATPL230) Physical Layer Error Flag  */
#define ATPL230_SYS_CONFIG_WDG_EN				(0x1u)
#define ATPL230_SYS_CONFIG_WDG_EN_Pos			2
#define ATPL230_SYS_CONFIG_WDG_EN_Msk			(ATPL230_SYS_CONFIG_WDG_EN << ATPL230_SYS_CONFIG_WDG_EN_Pos)		/**< \brief (ATPL230) Physical Layer Watchdog enable  */
#define ATPL230_SYS_CONFIG_PD					(0x1u)
#define ATPL230_SYS_CONFIG_PD_Pos				3
#define ATPL230_SYS_CONFIG_PD_Msk				(ATPL230_SYS_CONFIG_PD << ATPL230_SYS_CONFIG_PD_Pos)			/**< \brief (ATPL230) Converter Power Down  */
#define ATPL230_SYS_CONFIG_STOP_GCLK1X			(0x1u)
#define ATPL230_SYS_CONFIG_STOP_GCLK1X_Pos		4
#define ATPL230_SYS_CONFIG_STOP_GCLK1X_Msk		(ATPL230_SYS_CONFIG_STOP_GCLK1X << ATPL230_SYS_CONFIG_STOP_GCLK1X_Pos)		/**< \brief (ATPL230) Stop 20MHz clock */
#define ATPL230_SYS_CONFIG_STOP_GCLK2X			(0x1u)
#define ATPL230_SYS_CONFIG_STOP_GCLK2X_Pos		5
#define ATPL230_SYS_CONFIG_STOP_GCLK2X_Msk		(ATPL230_SYS_CONFIG_STOP_GCLK2X << ATPL230_SYS_CONFIG_STOP_GCLK2X_Pos)		/**< \brief (ATPL230) Stop 40MHz clock */
#define ATPL230_SYS_CONFIG_STOP_GCLKNX			(0x1u)
#define ATPL230_SYS_CONFIG_STOP_GCLKNX_Pos		6
#define ATPL230_SYS_CONFIG_STOP_GCLKNX_Msk		(ATPL230_SYS_CONFIG_STOP_GCLKNX << ATPL230_SYS_CONFIG_STOP_GCLKNX_Pos)		/**< \brief (ATPL230) Stop 200MHz clock */
#define ATPL230_SYS_CONFIG_STOP_CLKOUT			(0x1u)
#define ATPL230_SYS_CONFIG_STOP_CLKOUT_Pos		7
#define ATPL230_SYS_CONFIG_STOP_CLKOUT_Msk		(ATPL230_SYS_CONFIG_STOP_CLKOUT << ATPL230_SYS_CONFIG_STOP_CLKOUT_Pos)		/**< \brief (ATPL230) Stop output clock */
#define ATPL230_SYS_CONFIG_STOP_ENRAM			(0x1u)
#define ATPL230_SYS_CONFIG_STOP_ENRAM_Pos		7
#define ATPL230_SYS_CONFIG_STOP_ENRAM_Msk		(ATPL230_SYS_CONFIG_STOP_ENRAM << ATPL230_SYS_CONFIG_STOP_ENRAM_Pos)		/**< \brief (ATPL230) RAMs Output Enable */
//@}

//! \name Special Function Register
//@{
#define ATPL230_SFR_PHY_INT              (0x1u)
#define ATPL230_SFR_PHY_INT_Pos          0
#define ATPL230_SFR_PHY_INT_Msk          (ATPL230_SFR_PHY_INT << ATPL230_SFR_PHY_INT_Pos)		/**< \brief (ATPL230) Physical Layer interruption */
#define ATPL230_SFR_UMD                  (0x1u)
#define ATPL230_SFR_UMD_Pos              5
#define ATPL230_SFR_UMD_Msk              (ATPL230_SFR_UMD << ATPL230_SFR_UMD_Pos)		/**< \brief (ATPL230) Unsupported Modulation Scheme flag */
#define ATPL230_SFR_ERR_PYL              (0x1u)
#define ATPL230_SFR_ERR_PYL_Pos          4
#define ATPL230_SFR_ERR_PYL_Msk          (ATPL230_SFR_ERR_PYL << ATPL230_SFR_ERR_PYL_Pos)	/**< \brief (ATPL230) Unsupported Modulation Scheme flag */
#define ATPL230_SFR_CD                   (0x1u)
#define ATPL230_SFR_CD_Pos               6
#define ATPL230_SFR_CD_Msk               (ATPL230_SFR_CD << ATPL230_SFR_CD_Pos)			/**< \brief (ATPL230) Carrier Detect flag */
#define ATPL230_SFR_BCH_ERR              (0x1u)
#define ATPL230_SFR_BCH_ERR_Pos          7
#define ATPL230_SFR_BCH_ERR_Msk          (ATPL230_SFR_BCH_ERR << ATPL230_SFR_BCH_ERR_Pos)		/**< \brief (ATPL230) Busy Channel Error Flag */
//@}

//! \name TX Configuration Registers
//@{
#define ATPL230_TXRXBUF_TXCONF_DR				(0x1u)
#define ATPL230_TXRXBUF_TXCONF_DR_Pos			0
#define ATPL230_TXRXBUF_TXCONF_DR_Msk			(ATPL230_TXRXBUF_TXCONF_DR << ATPL230_TXRXBUF_TXCONF_DR_Pos)		/**< \brief (ATPL230) Reception enabled/disabled for emission  */
#define ATPL230_TXRXBUF_TXCONF_DC				(0x1u)
#define ATPL230_TXRXBUF_TXCONF_DC_Pos			1
#define ATPL230_TXRXBUF_TXCONF_DC_Msk			(ATPL230_TXRXBUF_TXCONF_DC << ATPL230_TXRXBUF_TXCONF_DC_Pos)		/**< \brief (ATPL230) Carrier Detect enabled/disabled for emission  */
#define ATPL230_TXRXBUF_TXCONF_EB				(0x1u)
#define ATPL230_TXRXBUF_TXCONF_EB_Pos			2
#define ATPL230_TXRXBUF_TXCONF_EB_Msk			(ATPL230_TXRXBUF_TXCONF_EB << ATPL230_TXRXBUF_TXCONF_EB_Pos)		/**< \brief (ATPL230) Buffer enabled/disabled  */
#define ATPL230_TXRXBUF_TXCONF_FE				(0x1u)
#define ATPL230_TXRXBUF_TXCONF_FE_Pos			3
#define ATPL230_TXRXBUF_TXCONF_FE_Msk			(ATPL230_TXRXBUF_TXCONF_FE << ATPL230_TXRXBUF_TXCONF_FE_Pos)		/**< \brief (ATPL230) Emission forced/unforced  */
#define ATPL230_TXRXBUF_TXCONF_BF				(0x1u)
#define ATPL230_TXRXBUF_TXCONF_BF_Pos			4
#define ATPL230_TXRXBUF_TXCONF_BF_Msk			(ATPL230_TXRXBUF_TXCONF_BF << ATPL230_TXRXBUF_TXCONF_BF_Pos)		/**< \brief (ATPL230) Bit Flipping mode  */
#define ATPL230_TXRXBUF_TXCONF_ATR				(0x1u)
#define ATPL230_TXRXBUF_TXCONF_ATR_Pos			5
#define ATPL230_TXRXBUF_TXCONF_ATR_Msk			(ATPL230_TXRXBUF_TXCONF_ATR << ATPL230_TXRXBUF_TXCONF_ATR_Pos)	/**< \brief (ATPL230) TxRx control mode  */
#define ATPL230_TXRXBUF_TXCONF_TRS				(0x1u)
#define ATPL230_TXRXBUF_TXCONF_TRS_Pos			6
#define ATPL230_TXRXBUF_TXCONF_TRS_Msk			(ATPL230_TXRXBUF_TXCONF_TRS << ATPL230_TXRXBUF_TXCONF_TRS_Pos)	/**< \brief (ATPL230) TxRx established by Software  */
#define ATPL230_TXRXBUF_TXCONF_PCO				(0x1u)
#define ATPL230_TXRXBUF_TXCONF_PCO_Pos			7
#define ATPL230_TXRXBUF_TXCONF_PCO_Msk			(ATPL230_TXRXBUF_TXCONF_PCO << ATPL230_TXRXBUF_TXCONF_PCO_Pos)	/**< \brief (ATPL230) Peak Cut mode  */
#define ATPL230_TXRXBUF_TXCONF_BR1				(0x1u)										/**< \brief (ATPL230) Branch 1 enable/disable for emission  */
#define ATPL230_TXRXBUF_TXCONF_BR2				(0x2u)										/**< \brief (ATPL230) Branch 2 enable/disable for emission  */
#define ATPL230_TXRXBUF_TXCONF_BR_Msk			(0x3u)										/**< \brief (ATPL230) Branch select mask  */
//@}

//! \name FFT Test Mode Register
//@{
#define ATPL230_FFT_MODE_EN					(0x1u)
#define ATPL230_FFT_MODE_EN_Pos					0
#define ATPL230_FFT_MODE_EN_Msk					(ATPL230_FFT_MODE_EN << ATPL230_FFT_MODE_EN_Pos)		/**< \brief (ATPL230) Enabled/disabled fft test mode  */
#define ATPL230_FFT_MODE_CONT					(0x1u)
#define ATPL230_FFT_MODE_CONT_Pos				1
#define ATPL230_FFT_MODE_CONT_Msk				(ATPL230_FFT_MODE_CONT << ATPL230_FFT_MODE_CONT_Pos)		/**< \brief (ATPL230) Enabled/disabled fft test mode  */
//@}

//! \name TXRXBUF_TX_INT : TX Interrupts Register
//@{
#define ATPL230_TXRXBUF_TX_INT_TX0_Msk			(1u)										/**< \brief (ATPL230) Notice Interrupt Transmission Buffer 0  */
#define ATPL230_TXRXBUF_TX_INT_TX1_Msk			(2u)										/**< \brief (ATPL230) Notice Interrupt Transmission Buffer 1  */
#define ATPL230_TXRXBUF_TX_INT_TX2_Msk			(4u)										/**< \brief (ATPL230) Notice Interrupt Transmission Buffer 2  */
#define ATPL230_TXRXBUF_TX_INT_TX3_Msk			(8u)										/**< \brief (ATPL230) Notice Interrupt Transmission Buffer 3  */
#define ATPL230_TXRXBUF_TX_INT_N_Msk			(16u)										/**< \brief (ATPL230) Notice Interrupt Noise Buffer  */
//@}

//! \name TXRXBUF_RX_INT : RX Interrupts Register
//@{
#define ATPL230_TXRXBUF_RX_INT_HRX0_Msk			(1u)										/**< \brief (ATPL230) Notice Header Interrupt Reception Buffer 0  */
#define ATPL230_TXRXBUF_RX_INT_HRX1_Msk			(2u)										/**< \brief (ATPL230) Notice Header Interrupt Reception Buffer 1  */
#define ATPL230_TXRXBUF_RX_INT_HRX2_Msk			(4u)										/**< \brief (ATPL230) Notice Header Interrupt Reception Buffer 2  */
#define ATPL230_TXRXBUF_RX_INT_HRX3_Msk			(8u)										/**< \brief (ATPL230) Notice Header Interrupt Reception Buffer 3  */
#define ATPL230_TXRXBUF_RX_INT_PRX0_Msk			(16u)										/**< \brief (ATPL230) Notice Payload  Interrupt Reception Buffer 0  */
#define ATPL230_TXRXBUF_RX_INT_PRX1_Msk			(32u)										/**< \brief (ATPL230) Notice Payload  Interrupt Reception Buffer 1  */
#define ATPL230_TXRXBUF_RX_INT_PRX2_Msk			(64u)										/**< \brief (ATPL230) Notice Payload  Interrupt Reception Buffer 2  */
#define ATPL230_TXRXBUF_RX_INT_PRX3_Msk			(128u)									/**< \brief (ATPL230) Notice Payload  Interrupt Reception Buffer 3  */
//@}

//! \name TXRXBUF_RESULT_TX : TX Result Register
//@{
#define ATPL230_TXRXBUF_RESULT_TX0				(0x7u)
#define ATPL230_TXRXBUF_RESULT_TX0_Pos			8
#define ATPL230_TXRXBUF_RESULT_TX0_Msk			(ATPL230_TXRXBUF_RESULT_TX0 << ATPL230_TXRXBUF_RESULT_TX0_Pos)		/**< \brief (ATPL230) Transmission result in buffer 0  */
#define ATPL230_TXRXBUF_RESULT_TX1				(0x7u)
#define ATPL230_TXRXBUF_RESULT_TX1_Pos			12
#define ATPL230_TXRXBUF_RESULT_TX1_Msk			(ATPL230_TXRXBUF_RESULT_TX1 << ATPL230_TXRXBUF_RESULT_TX1_Pos)		/**< \brief (ATPL230) Transmission result in buffer 1  */
#define ATPL230_TXRXBUF_RESULT_TX2				(0x7u)
#define ATPL230_TXRXBUF_RESULT_TX2_Pos			0
#define ATPL230_TXRXBUF_RESULT_TX2_Msk			(ATPL230_TXRXBUF_RESULT_TX2 << ATPL230_TXRXBUF_RESULT_TX2_Pos)		/**< \brief (ATPL230) Transmission result in buffer 2  */
#define ATPL230_TXRXBUF_RESULT_TX3				(0x7u)
#define ATPL230_TXRXBUF_RESULT_TX3_Pos			4
#define ATPL230_TXRXBUF_RESULT_TX3_Msk			(ATPL230_TXRXBUF_RESULT_TX3 << ATPL230_TXRXBUF_RESULT_TX3_Pos)		/**< \brief (ATPL230) Transmission result in buffer 3  */
//@}

//! \name Values of TXRXBUF_RESULT_TX register
//@{
#define ATPL230_TXRXBUF_RESULT_INPROCESS			0
#define ATPL230_TXRXBUF_RESULT_SUCCESSFUL			1
#define ATPL230_TXRXBUF_RESULT_WRONG_LEN			2
#define ATPL230_TXRXBUF_RESULT_BUSY_CHANNEL			3
#define ATPL230_TXRXBUF_RESULT_PREV_TX_INPROCESS	4
#define ATPL230_TXRXBUF_RESULT_RX_INPROCESS			5
#define ATPL230_TXRXBUF_RESULT_INVALID_SCHEME		6
#define ATPL230_TXRXBUF_RESULT_TIMEOUT				7
//@}

//! \name TXRXBUF_RXCONF_ROBO_MODE : Robust RX Mode Register
//@{
#define ATPL230_TXRXBUF_RXCONF_ROBO_MODE_RX0		(0x3u)
#define ATPL230_TXRXBUF_RXCONF_ROBO_MODE_RX0_Pos	0
#define ATPL230_TXRXBUF_RXCONF_ROBO_MODE_RX0_Msk	(ATPL230_TXRXBUF_RXCONF_ROBO_MODE_RX0 << ATPL230_TXRXBUF_RXCONF_ROBO_MODE_RX0_Pos)		/**< \brief (ATPL230) Robust Mode Reading in Reception Buffer 0  */
#define ATPL230_TXRXBUF_RXCONF_ROBO_MODE_RX1		(0x3u)
#define ATPL230_TXRXBUF_RXCONF_ROBO_MODE_RX1_Pos	2
#define ATPL230_TXRXBUF_RXCONF_ROBO_MODE_RX1_Msk	(ATPL230_TXRXBUF_RXCONF_ROBO_MODE_RX1 << ATPL230_TXRXBUF_RXCONF_ROBO_MODE_RX1_Pos)		/**< \brief (ATPL230) Robust Mode Reading in Reception Buffer 1  */
#define ATPL230_TXRXBUF_RXCONF_ROBO_MODE_RX2		(0x3u)
#define ATPL230_TXRXBUF_RXCONF_ROBO_MODE_RX2_Pos	4
#define ATPL230_TXRXBUF_RXCONF_ROBO_MODE_RX2_Msk	(ATPL230_TXRXBUF_RXCONF_ROBO_MODE_RX2 << ATPL230_TXRXBUF_RXCONF_ROBO_MODE_RX2_Pos)		/**< \brief (ATPL230) Robust Mode Reading in Reception Buffer 2  */
#define ATPL230_TXRXBUF_RXCONF_ROBO_MODE_RX3		(0x3u)
#define ATPL230_TXRXBUF_RXCONF_ROBO_MODE_RX3_Pos	6
#define ATPL230_TXRXBUF_RXCONF_ROBO_MODE_RX3_Msk	(ATPL230_TXRXBUF_RXCONF_ROBO_MODE_RX3 << ATPL230_TXRXBUF_RXCONF_ROBO_MODE_RX3_Pos)		/**< \brief (ATPL230) Robust Mode Reading in Reception Buffer 3  */
//@}

//! \name TXRXBUF_NOISECONF : Noise Configuration Register
//@{
#define ATPL230_TXRXBUF_NOISECONF_ETN				(0x7u)
#define ATPL230_TXRXBUF_NOISECONF_ETN_Pos			5
#define ATPL230_TXRXBUF_NOISECONF_ETN_Msk			(ATPL230_TXRXBUF_NOISECONF_ETN << ATPL230_TXRXBUF_NOISECONF_ETN_Pos)		/**< \brief (ATPL230) Noise Error Type  */
#define ATPL230_TXRXBUF_NOISECONF_EBN				(0x1u)
#define ATPL230_TXRXBUF_NOISECONF_EBN_Pos			4
#define ATPL230_TXRXBUF_NOISECONF_EBN_Msk			(ATPL230_TXRXBUF_NOISECONF_EBN << ATPL230_TXRXBUF_NOISECONF_EBN_Pos)		/**< \brief (ATPL230) Enable Noise Buffer  */
#define ATPL230_TXRXBUF_NOISECONF_FTN				(0x1u)
#define ATPL230_TXRXBUF_NOISECONF_FTN_Pos			3
#define ATPL230_TXRXBUF_NOISECONF_FTN_Msk			(ATPL230_TXRXBUF_NOISECONF_FTN << ATPL230_TXRXBUF_NOISECONF_FTN_Pos)		/**< \brief (ATPL230) Capture Noise Forced  */
#define ATPL230_TXRXBUF_NOISECONF_NS				(0x7u)
#define ATPL230_TXRXBUF_NOISECONF_NS_Pos			0
#define ATPL230_TXRXBUF_NOISECONF_NS_Msk			(ATPL230_TXRXBUF_NOISECONF_NS << ATPL230_TXRXBUF_NOISECONF_NS_Pos)			/**< \brief (ATPL230) Number of symbols wanted  */
//@}

//! \name Values of TXRXBUF_NOISECONF ETN
//@{
#define ATPL230_NOISE_CAPTURE_IN_PROCESS			0
#define ATPL230_NOISE_CAPTURE_SUCCESS				1
#define ATPL230_NOISE_START_TX						2
#define ATPL230_NOISE_START_RX						3
#define ATPL230_NOISE_BCHN_PREV_TX					4
#define ATPL230_NOISE_BCHN_PREV_RX					5
#define ATPL230_NOISE_AGCS_VARIATION				6
//@}

//! @}

#endif /* _ATPL230REG_INSTANCE_ */

