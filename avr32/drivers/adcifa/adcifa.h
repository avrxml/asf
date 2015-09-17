/*****************************************************************************
*
* \file
*
* \brief ADCIFA header for AVR UC3.
*
* This file defines a useful set of functions for ADC on AVR UC3 devices.
*
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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
*****************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef _ADCIFA_H_
#define _ADCIFA_H_

/**
 * \defgroup group_avr32_drivers_adcifa ADCIFA - ADC Interface A
 *
 * Driver for the ADCIFA (Analog-to-Digital Converter Interface A).
 * Provides functions for configuration of conversion parameters
 * (up to 12-bit signed at 1.5 Msps), channel sequencing (max. 16 channels,
 * w/ 16 different inputs and up to 64x gain), window monitoring,
 * interrupt and conversion triggering.
 *
 * @{
 */

#include <avr32/io.h>
#include "compiler.h"

#if UC3C
#  define AVR32_FLASHC_FACTORY_PAGE_ADDRESS    0x80800200
#  define AVR32_FLASHC_FROW_OCAL_WORD          4
#  define AVR32_FLASHC_FROW_OCAL_MASK          0x3F000000
#  define AVR32_FLASHC_FROW_OCAL_OFFSET        24
#  define AVR32_FLASHC_FROW_GCAL_WORD          4
#  define AVR32_FLASHC_FROW_GCAL_MASK          0x00007FFF
#  define AVR32_FLASHC_FROW_GCAL_OFFSET        0
#  define AVR32_FLASHC_FROW_GAIN0_WORD         8
#  define AVR32_FLASHC_FROW_GAIN0_MASK         0x000003FF
#  define AVR32_FLASHC_FROW_GAIN0_OFFSET       0
#  define AVR32_FLASHC_FROW_GAIN1_WORD         8
#  define AVR32_FLASHC_FROW_GAIN1_MASK         0x03FF0000
#  define AVR32_FLASHC_FROW_GAIN1_OFFSET       16
#endif

/** \name Positive Inputs used by the ADC
 * @{
 */
#define AVR32_ADCIFA_INP_ADCIN0              0
#define AVR32_ADCIFA_INP_ADCIN1              1
#define AVR32_ADCIFA_INP_ADCIN2              2
#define AVR32_ADCIFA_INP_ADCIN3              3
#define AVR32_ADCIFA_INP_ADCIN4              4
#define AVR32_ADCIFA_INP_ADCIN5              5
#define AVR32_ADCIFA_INP_ADCIN6              6
#define AVR32_ADCIFA_INP_ADCIN7              7
#define AVR32_ADCIFA_INP_DAC0_INT            8
#define AVR32_ADCIFA_INP_TSENSE              9
#define AVR32_ADCIFA_INP_GNDANA              10
/** @} */

/** \name Negative Inputs used by the ADC
 * @{
 */
#define AVR32_ADCIFA_INN_ADCIN8              0
#define AVR32_ADCIFA_INN_ADCIN9              1
#define AVR32_ADCIFA_INN_ADCIN10             2
#define AVR32_ADCIFA_INN_ADCIN11             3
#define AVR32_ADCIFA_INN_ADCIN12             4
#define AVR32_ADCIFA_INN_ADCIN13             5
#define AVR32_ADCIFA_INN_ADCIN14             6
#define AVR32_ADCIFA_INN_ADCIN15             7
#define AVR32_ADCIFA_INN_DAC1_INT            8
#define AVR32_ADCIFA_INN_GNDANA              9
/** @} */

/** \name References used by the ADC
 * @{
 */
#define ADCIFA_REF1V              0x0  /** Internal 1V reference */
#define ADCIFA_REF06VDD           0x1  /** Internal 0.6 x VDDANA reference */
#define ADCIFA_ADCREF0            0x2  /** External 0 reference ADCREF0 */
#define ADCIFA_ADCREF1            0x3  /** External 1 reference ADCREF1 */
#define ADCIFA_ADCREF             0x10 /** External reference ADCREFP/ADCREFN */
/** @} */

/** \name Triggering Source used by the ADC
 * @{
 */
#define ADCIFA_TRGSEL_SOFT        0x0     /** Trigger Source Software */
#define ADCIFA_TRGSEL_ITIMER      0x1     /** Trigger Source Timer */
#define ADCIFA_TRGSEL_EVT         0x2     /** Trigger Source Event */
#if !(defined AVR32_ADCIFA_100_H_INCLUDED)
#  define ADCIFA_TRGSEL_CONTINUOUS  0x3   /** Trigger Source Continuous */
#endif
/** @} */

/** \name Resolution selected by the ADC
 * @{
 */
#define ADCIFA_SRES_8B            0x2     /** Resolution 8-Bits */
#define ADCIFA_SRES_10B           0x1     /** Resolution 10-Bits */
#define ADCIFA_SRES_12B           0x0     /** Resolution 12-Bits */
/** @} */

/** \name Gain selected on a Conversion by the ADC
 * @{
 */
#define ADCIFA_SHG_1              0x0     /** Gain Conversion = 1 */
#define ADCIFA_SHG_2              0x1     /** Gain Conversion = 2 */
#define ADCIFA_SHG_4              0x2     /** Gain Conversion = 4 */
#define ADCIFA_SHG_8              0x3     /** Gain Conversion = 8 */
#define ADCIFA_SHG_16             0x4     /** Gain Conversion = 16 */
#define ADCIFA_SHG_32             0x5     /** Gain Conversion = 32 */
#define ADCIFA_SHG_64             0x6     /** Gain Conversion = 64 */
/** @} */

/** \name Conversion Management of the Sequence
 * @{
 */
#define ADCIFA_SOCB_ALLSEQ        0x0     /** A complete sequence is performed
                                           *  on a SOC event */
#define ADCIFA_SOCB_SINGLECONV    0x1     /** A single conversion is performed
                                           *  on a SOC event */
/** @} */

#if (defined AVR32_ADCIFA_100_H_INCLUDED)

/** \name Consecutive Sampling Wait State (CSWS)
 * @{
 */
#  define ADCIFA_CSWS_WSTATE      0x1     /** Enable the CSWS mode */
#  define ADCIFA_CSWS_NOWSTATE    0x0     /** Disable the CSWS mode */
/** @} */
#else

/** \name Sampling Mode (OVSX2 & SHDYN)
 * @{
 */
#  define ADCIFA_SH_MODE_STANDARD 0x0     /** No dynamic over sampling */
#  define ADCIFA_SH_MODE_OVERSAMP 0x1     /** Over sampling: OVSX2 = 1 */
#  define ADCIFA_SH_MODE_DYNAMIC  0x2     /** Dynamic: SHDYN = 1 */
/** @} */
#endif

/** \name Half Word Left Adjustment (HWLA)
 * @{
 */
#define ADCIFA_HWLA_NOADJ         0x0     /** Disable the HWLA mode */
#define ADCIFA_HWLA_LEFTADJ       0x1     /** Enable the HWLA mode */
/** @} */

/** \name Software Acknowledge (SA)
 * @{
 */
#define ADCIFA_SA_EOS_SOFTACK     0x0     /** Disable the SA mode */
#define ADCIFA_SA_NO_EOS_SOFTACK  0x1     /** Enable the SA mode */
/** @} */

/** \name Sequence numbers
 * @{
 */
#define ADCIFA_SEQ0               0x0
#define ADCIFA_SEQ1               0x1
#define ADCIFA_SEQ0_SEQ1          0x3
/** @} */

/** \name Window monitor
 * @{
 */
#define ADCIFA_WINDOW_MODE_NONE     0     /** 	No Window Mode : Default */
#define ADCIFA_WINDOW_MODE_BELOW    1     /**   Active : Result < High Threshold */
#define ADCIFA_WINDOW_MODE_ABOVE    2     /**   Active : Result > Low Threshold */
#define ADCIFA_WINDOW_MODE_INSIDE   3     /**   Active : Low Threshold < Result  < High Threshold */
#define ADCIFA_WINDOW_MODE_OUTSIDE  4     /**   Active : Result >= Low Threshold or Result >= High Threshold */
/** @} */

/** ADCIFA Start-Up time (us) */
#define ADCIFA_START_UP_TIME      1000

/**  Set Offset Calibration */
#define ADCIFA_set_offset_calibration(ocal) \
	{ \
		AVR32_ADCIFA.adccal =  \
			((AVR32_ADCIFA.adccal & ~AVR32_ADCIFA_ADCCAL_OCAL_MASK)|\
			(((ocal) << AVR32_ADCIFA_ADCCAL_OCAL) & \
				AVR32_ADCIFA_ADCCAL_OCAL_MASK)); \
	}

/**  Set Gain Calibration */
#define ADCIFA_set_gain_calibration(gcal) \
	{ \
		AVR32_ADCIFA.adccal = \
		((AVR32_ADCIFA.adccal & ~AVR32_ADCIFA_ADCCAL_GCAL_MASK) | \
		(((gcal)<<AVR32_ADCIFA_ADCCAL_GCAL) & AVR32_ADCIFA_ADCCAL_GCAL_MASK)); \
	}

/**  Set Sample & Hold Gain Calibration for Seq 0 */
#define ADCIFA_set_sh0_gain_calibration(scal) \
	{ \
		AVR32_ADCIFA.shcal = \
		((AVR32_ADCIFA.shcal & ~AVR32_ADCIFA_SHCAL_GAIN0_MASK) | \
		(((scal)<<AVR32_ADCIFA_SHCAL_GAIN0) & AVR32_ADCIFA_SHCAL_GAIN0_MASK)); \
	}

/**  Set Sample & Hold Gain Calibration for Seq 0 */
#define ADCIFA_set_sh1_gain_calibration(scal) \
	{ \
		AVR32_ADCIFA.shcal = \
		((AVR32_ADCIFA.shcal & ~AVR32_ADCIFA_SHCAL_GAIN1_MASK) | \
		(((scal)<<AVR32_ADCIFA_SHCAL_GAIN1) & AVR32_ADCIFA_SHCAL_GAIN1_MASK)); \
	}

/**  Check Startup Time flag */
#define ADCIFA_is_startup_time() \
	(((AVR32_ADCIFA.sr) & (1 << AVR32_ADCIFA_SR_SUTD)) == \
	(1 << AVR32_ADCIFA_SR_SUTD))

/**  Enable the ADCIFA */
#define ADCIFA_enable()	\
	{ \
		AVR32_ADCIFA.cfg \
			|= (1 << AVR32_ADCIFA_CFG_ADCEN); \
	}

/**  Enable the ADCIFA */
#define ADCIFA_disable() \
	{ \
		AVR32_ADCIFA.cfg  &= ~(1 << AVR32_ADCIFA_CFG_ADCEN); \
	}

#define ADCIFA_softsoc_sequencer(seq) \
	{ \
		AVR32_ADCIFA.cr = (seq); \
	}

/** Configuration of Sequencer 0 */
/** cnvb : Number of conversion */
/** sres : ADCIFA Resolution */
/** trgsel : Trigger Selection */
/** socb : Start of Conversion Selection */
/** csws : CSWS Mode Selection */
/** hwla : HWLA Mode Selection */
/** sa   : SA Mode Selection */
#if (defined AVR32_ADCIFA_100_H_INCLUDED)
#  define ADCIFA_configure_sequencer_0(cnvnb, sres, trgsel, socb, csws,	\
			hwla, sa) \
	{ \
		AVR32_ADCIFA.seqcfg0 \
			= ((cnvnb << AVR32_ADCIFA_SEQCFG0_CNVNB) | \
				(sres << AVR32_ADCIFA_SEQCFG0_SRES) | \
				(trgsel << AVR32_ADCIFA_SEQCFG0_TRGSEL) | \
				(socb << AVR32_ADCIFA_SEQCFG0_SOCB) | \
				(csws << AVR32_ADCIFA_SEQCFG0_CSWS) | \
				(hwla << AVR32_ADCIFA_SEQCFG0_HWLA) | \
				(sa << AVR32_ADCIFA_SEQCFG0_SA)); \
	}
#else
#  define ADCIFA_configure_sequencer_0(cnvnb, sres, trgsel, socb, sh_mode, \
			hwla, sa) \
	{ \
		AVR32_ADCIFA.seqcfg0 \
			= ((cnvnb << AVR32_ADCIFA_SEQCFG0_CNVNB) | \
				(sres << AVR32_ADCIFA_SEQCFG0_SRES) | \
				(trgsel << AVR32_ADCIFA_SEQCFG0_TRGSEL) | \
				(socb << AVR32_ADCIFA_SEQCFG0_SOCB) | \
				(sh_mode << AVR32_ADCIFA_SEQCFG0_OVSX2) | \
				(hwla << AVR32_ADCIFA_SEQCFG0_HWLA) | \
				(sa << AVR32_ADCIFA_SEQCFG0_SA)); \
	}
#endif

/**  Sequencer 0 : Software Start of Conversion */
#define ADCIFA_softsoc_sequencer_0() \
	{ AVR32_ADCIFA.cr = AVR32_ADCIFA_CR_SOC0_MASK; }

/**  Sequencer 0 : Check end of Conversion */
#define ADCIFA_is_eoc_sequencer_0() \
	(((AVR32_ADCIFA.sr) & (1 << AVR32_ADCIFA_SR_SEOC0)) == \
	(1 << AVR32_ADCIFA_SR_SEOC0))

/**  Sequencer 0 : Check end of Sequence */
#define ADCIFA_is_eos_sequencer_0() \
	(((AVR32_ADCIFA.sr) & (1 << AVR32_ADCIFA_SR_SEOS0)) == \
	(1 << AVR32_ADCIFA_SR_SEOS0))

/**  Sequencer 0 : Ack end of Conversion */
#define ADCIFA_clear_eoc_sequencer_0() \
	((AVR32_ADCIFA.scr) |= (1 << AVR32_ADCIFA_SCR_SEOC0))

/**  Sequencer 0 : Ack end of Sequence */
#define ADCIFA_clear_eos_sequencer_0() \
	((AVR32_ADCIFA.scr) |= (1 << AVR32_ADCIFA_SCR_SEOS0))

/** Configuration of Mux Positive for Sequencer 0 */
/** m(x) : ADC Channel for element x of Sequencer 0 */
#define ADCIFA_configure_muxsel0p(m7, m6, m5, m4, m3, m2, m1, m0) \
	{ \
		AVR32_ADCIFA.inpsel10 \
			= ((m7) << AVR32_ADCIFA_INNSEL10_CNV7_OFFSET) |	\
				((m6) << AVR32_ADCIFA_INNSEL10_CNV6_OFFSET) | \
				((m5) << AVR32_ADCIFA_INNSEL10_CNV5_OFFSET) | \
				((m4) << AVR32_ADCIFA_INNSEL10_CNV4_OFFSET); \
		AVR32_ADCIFA.inpsel00 \
			= ((m3) << AVR32_ADCIFA_INNSEL00_CNV3_OFFSET) |	\
				((m2) << AVR32_ADCIFA_INNSEL00_CNV2_OFFSET) | \
				((m1) << AVR32_ADCIFA_INNSEL00_CNV1_OFFSET) | \
				((m0) << AVR32_ADCIFA_INNSEL00_CNV0_OFFSET); \
	}

/** Configuration of Mux Negative for Sequencer 0 */
/** m(x) : ADC Channel for element x of Sequencer 0 */
#define ADCIFA_configure_muxsel0n(m7, m6, m5, m4, m3, m2, m1, m0)         { \
		AVR32_ADCIFA.innsel10 \
			= ((m7) << AVR32_ADCIFA_INNSEL10_CNV7_OFFSET) |	\
				((m6) << AVR32_ADCIFA_INNSEL10_CNV6_OFFSET) | \
				((m5) << AVR32_ADCIFA_INNSEL10_CNV5_OFFSET) | \
				((m4) << AVR32_ADCIFA_INNSEL10_CNV4_OFFSET); \
		AVR32_ADCIFA.innsel00 \
			= ((m3) << AVR32_ADCIFA_INNSEL00_CNV3_OFFSET) |	\
				((m2) << AVR32_ADCIFA_INNSEL00_CNV2_OFFSET) | \
				((m1) << AVR32_ADCIFA_INNSEL00_CNV1_OFFSET) | \
				((m0) << AVR32_ADCIFA_INNSEL00_CNV0_OFFSET); \
}

/** Configuration of Gain for Sequencer 0 */
/** g(x) : Gain for element x of Sequencer 0 */
#define ADCIFA_configure_sh0gain(g7, g6, g5, g4, g3, g2, g1, g0) \
	{ \
		AVR32_ADCIFA.shg0 \
			= ((g7) << AVR32_ADCIFA_GCNV7_OFFSET) |	\
				((g6) << AVR32_ADCIFA_GCNV6_OFFSET)  | \
				((g5) <<  AVR32_ADCIFA_GCNV5_OFFSET) | \
				((g4) << AVR32_ADCIFA_GCNV4_OFFSET) | \
				((g3) << AVR32_ADCIFA_GCNV3_OFFSET) | \
				((g2) <<  AVR32_ADCIFA_GCNV2_OFFSET) | \
				((g1) << AVR32_ADCIFA_GCNV1_OFFSET) | \
				((g0) << AVR32_ADCIFA_GCNV0_OFFSET); \
	}

/** Return result for conversion for Sequencer 0 */
/** ind : Index on element of Sequencer 0 */
#define ADCIFA_read_resx_sequencer_0(ind) ((int32_t)AVR32_ADCIFA.resx[(ind)])

/** Configuration of Sequencer 1 */
/** cnvb : Number of conversion */
/** sres : ADCIFA Resolution */
/** trgsel : Trigger Selection */
/** socb : Start of Conversion Selection */
/** csws : CSWS Mode Selection */
/** hwla : HWLA Mode Selection */
/** sa   : SA Mode Selection */
#ifdef AVR32_ADCIFA_100_H_INCLUDED
#  define ADCIFA_configure_sequencer_1(cnvnb, sres, trgsel, socb, csws,	\
			hwla, sa) \
	{ \
		AVR32_ADCIFA.seqcfg1 \
			= ((cnvnb) << AVR32_ADCIFA_SEQCFG1_CNVNB)     |	\
				((sres) << AVR32_ADCIFA_SEQCFG1_SRES)     | \
				((trgsel) << AVR32_ADCIFA_SEQCFG1_TRGSEL) | \
				((socb) << AVR32_ADCIFA_SEQCFG1_SOCB)     | \
				((csws) << AVR32_ADCIFA_SEQCFG1_CSWS)     | \
				((hwla) << AVR32_ADCIFA_SEQCFG1_HWLA)     | \
				((sa) << AVR32_ADCIFA_SEQCFG1_SA); \
	}
#else
#  define ADCIFA_configure_sequencer_1(cnvnb, sres, trgsel, socb, csws,	\
			hwla, sa) \
	{ \
		AVR32_ADCIFA.seqcfg1 \
			= ((cnvnb << AVR32_ADCIFA_SEQCFG1_CNVNB)    | \
				(sres << AVR32_ADCIFA_SEQCFG1_SRES)     | \
				(trgsel << AVR32_ADCIFA_SEQCFG1_TRGSEL) | \
				(socb << AVR32_ADCIFA_SEQCFG1_SOCB)     | \
				(csws << AVR32_ADCIFA_SEQCFG1_OVSX2)    | \
				(hwla << AVR32_ADCIFA_SEQCFG1_HWLA)     | \
				(1 << AVR32_ADCIFA_SEQCFG1_SHDYN)       | \
				(sa << AVR32_ADCIFA_SEQCFG1_SA)); \
	}
#endif

/**  Sequencer 1 : Software Start of Conversion */
#define ADCIFA_softsoc_sequencer_1() \
	{ AVR32_ADCIFA.cr = AVR32_ADCIFA_CR_SOC1_MASK; }

/**  Sequencer 1 : Check end of Conversion */
#define ADCIFA_is_eoc_sequencer_1() \
	(((AVR32_ADCIFA.sr) & (1 << AVR32_ADCIFA_SR_SEOC1)) == \
	(1 << AVR32_ADCIFA_SR_SEOC1))

/**  Sequencer 1 : Check end of Sequence */
#define ADCIFA_is_eos_sequencer_1() \
	(((AVR32_ADCIFA.sr) & (1 << AVR32_ADCIFA_SR_SEOS1)) == \
	(1 << AVR32_ADCIFA_SR_SEOS1))

/**  Sequencer 1 : Ack end of Conversion */
#define ADCIFA_clear_eoc_sequencer_1() \
	((AVR32_ADCIFA.scr) |= (1 << AVR32_ADCIFA_SCR_SEOC1)

/**  Sequencer 1 : Ack end of Sequence */
#define ADCIFA_clear_eos_sequencer_1() \
	((AVR32_ADCIFA.scr) |= (1 << AVR32_ADCIFA_SCR_SEOS1))

/** Configuration of Mux Positive for Sequencer 1 */
/** m(x) : ADC Channel for element x of Sequencer 1 */
#define ADCIFA_configure_muxsel1p(m7, m6, m5, m4, m3, m2, m1, m0) \
	{ \
		AVR32_ADCIFA.inpsel11 \
			= ((m7) << \
				AVR32_ADCIFA_INNSEL10_CNV7_OFFSET) | \
				((m6) << \
				AVR32_ADCIFA_INNSEL10_CNV6_OFFSET) | \
				((m5) << \
				AVR32_ADCIFA_INNSEL10_CNV5_OFFSET) | \
				((m4) << AVR32_ADCIFA_INNSEL10_CNV4_OFFSET); \
		AVR32_ADCIFA.inpsel01 \
			= ((m3) << \
				AVR32_ADCIFA_INNSEL00_CNV3_OFFSET) | \
				((m2) << \
				AVR32_ADCIFA_INNSEL00_CNV2_OFFSET) | \
				((m1) << \
				AVR32_ADCIFA_INNSEL00_CNV1_OFFSET) | \
				((m0) << AVR32_ADCIFA_INNSEL00_CNV0_OFFSET); \
	}

/** Configuration of Mux Negative for Sequencer 1 */
/** m(x) : ADC Channel for element x of Sequencer 1 */
#define ADCIFA_configure_muxsel1n(m7, m6, m5, m4, m3, m2, m1, m0) \
	{ \
		AVR32_ADCIFA.innsel11 \
			= ((m7) << \
				AVR32_ADCIFA_INNSEL10_CNV7_OFFSET) | \
				((m6) << \
				AVR32_ADCIFA_INNSEL10_CNV6_OFFSET) | \
				((m5) << \
				AVR32_ADCIFA_INNSEL10_CNV5_OFFSET) | \
				((m4) << AVR32_ADCIFA_INNSEL10_CNV4_OFFSET); \
		AVR32_ADCIFA.innsel01 \
			= ((m3) << \
				AVR32_ADCIFA_INNSEL00_CNV3_OFFSET) | \
				((m2) << \
				AVR32_ADCIFA_INNSEL00_CNV2_OFFSET) | \
				((m1) << \
				AVR32_ADCIFA_INNSEL00_CNV1_OFFSET) | \
				((m0) << AVR32_ADCIFA_INNSEL00_CNV0_OFFSET); \
	}

/** Configuration of Gain for Sequencer 1 */
/** g(x) : Gain for element x of Sequencer 1 */
#define ADCIFA_configure_sh1gain(g7, g6, g5, g4, g3, g2, g1, g0) \
	{   AVR32_ADCIFA.shg1 \
		    = ((g7) << \
			    AVR32_ADCIFA_GCNV7_OFFSET) \
			    | \
			    ((g6) << \
			    AVR32_ADCIFA_GCNV6_OFFSET) \
			    | \
			    ((g5) << \
			    AVR32_ADCIFA_GCNV5_OFFSET) \
			    | \
			    ((g4) << \
			    AVR32_ADCIFA_GCNV4_OFFSET) \
			    | \
			    ((g3) << \
			    AVR32_ADCIFA_GCNV3_OFFSET) \
			    | \
			    ((g2) << \
			    AVR32_ADCIFA_GCNV2_OFFSET) \
			    | \
			    ((g1) << \
			    AVR32_ADCIFA_GCNV1_OFFSET) \
			    | \
			    ((g0) << \
			    AVR32_ADCIFA_GCNV0_OFFSET);	\
	}

/** Return result for conversion for Sequencer 1 */
#define ADCIFA_read_resx_sequencer_1(ind) ((int)AVR32_ADCIFA.resx[(ind) + 8])

/**  Window Monitor 0 : Check the Window Monitor 0 Status Bit */
#define ADCIFA_is_window_0_set() \
	(((AVR32_ADCIFA.sr) & (1 << AVR32_ADCIFA_SR_WM0)) == \
	(1 << AVR32_ADCIFA_SR_WM0))

/**  Window Monitor 0 : Ack the Window Monitor 0 Status Bit */
#define ADCIFA_clear_window_0() \
	((AVR32_ADCIFA.scr) |= (1 << AVR32_ADCIFA_SCR_WM0))

/**  Window Monitor 1 : Check the Window Monitor 1 Status Bit */
#define ADCIFA_is_window_1_set() \
	(((AVR32_ADCIFA.sr) & (1 << AVR32_ADCIFA_SR_WM1)) == \
	(1 << AVR32_ADCIFA_SR_WM1))

/**  Window Monitor 1 : Ack the Window Monitor 1 Status Bit */
#define ADCIFA_clear_window_1() \
	((AVR32_ADCIFA.scr) |= (1 << AVR32_ADCIFA_SCR_WM1))

/** Parameter */
#define ADCIFA_NONE             0xFF

/** This constant is used as return value for \ref adcifa_configure and
 * \ref adcifa_configure_sequencer functions. */
#define ADCIFA_CONFIGURATION_REFUSED                0x0

/** This constant is used as return value for \ref adcifa_configure and
 * \ref adcifa_configure_sequencer functions. */
#define ADCIFA_CONFIGURATION_ACCEPTED               0x1

/** This constant is used as return value for the
 *  \ref adcifa_get_values_from_sequencer function. */
#define ADCIFA_STATUS_COMPLETED                     0x2

/** This constant is used as return value for the
 *  \ref adcifa_get_values_from_sequencer function. */
#define ADCIFA_STATUS_NOT_COMPLETED                 0x3

/** Parameters for the ADCIFA peripheral. */
typedef struct {
	uint32_t frequency; /**< Internal ADC Frequency in Hz */
	uint8_t reference_source; /**< Reference Source */
	bool sample_and_hold_disable; /**< Sample and Hold Selection */
	bool single_sequencer_mode; /**< Single Sequencer Mode */
	bool free_running_mode_enable; /**< Enable Free Running Mode */
	bool sleep_mode_enable; /**< Sleep Mode Selection */
	bool mux_settle_more_time; /** Multiplexer Settle Time */
	int16_t gain_calibration_value; /**< Gain Calibration Value */
	int16_t offset_calibration_value; /**< Offset Calibration Value */
	int16_t sh0_calibration_value; /**< S/H Gain Calibration Value for Seq0 */
	int16_t sh1_calibration_value; /**< S/H Gain Calibration Value for Seq1 */
} adcifa_opt_t;

/** Parameters for the configuration of the Sequencer. */
typedef struct {
	uint8_t convnb; /**< Number of conversion. */
	uint8_t resolution; /**< Sequencer resolution. */
	uint8_t trigger_selection; /**< Trigger selection. */
	uint8_t start_of_conversion; /**< Start of conversion. */
#if (defined AVR32_ADCIFA_100_H_INCLUDED)
	uint8_t oversampling; /**< Over sampling. */
#else
	uint8_t sh_mode; /**< Sample & Hold mode. */
#endif
	uint8_t half_word_adjustment; /**< Half word adjustment. */
	uint8_t software_acknowledge; /**< Software Acknowledge. */
} adcifa_sequencer_opt_t;

/** Parameters for an conversion in the Sequencer. */
typedef struct {
	uint8_t channel_p; /**< Positive channel. */
	uint8_t channel_n; /**< Negative channel. */
	uint8_t gain; /**< Gain selection */
} adcifa_sequencer_conversion_opt_t;

/** Parameters for the windows monitor mode */
typedef struct {
	uint8_t mode; /**< Internal ADC Frequency in Hz */
	uint8_t source_index; /**< Index of the result register */
	uint16_t low_threshold; /**< Low threshold value */
	uint16_t high_threshold; /**< High threshold value */
} adcifa_window_monitor_opt_t;

void adcifa_calibrate_offset(volatile avr32_adcifa_t *adcifa,
	adcifa_opt_t *p_adcifa_opt, uint32_t pb_hz);
void adcifa_get_calibration_data(volatile avr32_adcifa_t *adcifa,
		adcifa_opt_t *p_adcifa_opt);
uint8_t adcifa_configure(volatile avr32_adcifa_t *adcifa,
		adcifa_opt_t *p_adcifa_opt,
		uint32_t pb_hz);
extern uint8_t adcifa_configure_sequencer(volatile avr32_adcifa_t *adcifa,
		uint8_t sequencer,
		adcifa_sequencer_opt_t *p_adcifa_sequencer_opt,
		adcifa_sequencer_conversion_opt_t *p_adcifa_sequencer_conversion_opt);
void adcifa_configure_window_monitor(volatile avr32_adcifa_t *adcifa,
		uint8_t sequencer,
		adcifa_window_monitor_opt_t *adc_window_monitor_opt);
void adcifa_start_sequencer(volatile avr32_adcifa_t *adcifa,
		uint8_t sequencer);
bool adcifa_check_eoc(volatile avr32_adcifa_t *adcifa, uint8_t sequencer);
bool adcifa_check_eos(volatile avr32_adcifa_t *adcifa, uint8_t sequencer);
uint8_t adcifa_get_values_from_sequencer(volatile avr32_adcifa_t *adcifa,
		uint8_t sequencer,
		adcifa_sequencer_opt_t *p_adcifa_sequencer_opt,
		int16_t *adcifa_values);
void adcifa_start_itimer(volatile avr32_adcifa_t *adcifa,
		uint32_t timer_count);
void adcifa_stop_itimer(volatile avr32_adcifa_t *adcifa);
void adcifa_enable_interrupt(volatile avr32_adcifa_t *adcifa,
		uint32_t interrupt_flags);
void adcifa_disable_interrupt(volatile avr32_adcifa_t *adcifa,
		uint32_t interrupt_flags);
void adcifa_clear_interrupt(volatile avr32_adcifa_t *adcifa,
		uint32_t interrupt_flags);

/**
 * @}
 */

#endif  /* _ADCIFA_H_ */
