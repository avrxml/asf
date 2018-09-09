/**
 * \file
 *
 * \brief ATmega_RF family defines
 *
 * Copyright (c) 2012-2018 Microchip Technology Inc. and its subsidiaries.
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

#ifndef _ADC_MEGARF
#define _ADC_MEGARF

#ifdef __cplusplus
extern "C" {
#endif

#define ADC_ADTS_REG    ADCSRB
#define ADSUT_gm                (1 << ADSUT4 | 1 << ADSUT3 | 1 << ADSUT2 | 1 <<	\
	ADSUT1 | 1 << ADSUT0)
#define ADTHT_gm                (1 << ADTHT1 | 1 << ADTHT0)
#define BGCAL_FINE_gm   (1 << BGCAL_FINE3 | 1 << BGCAL_FINE2 | 1 << \
	BGCAL_FINE1 | 1 << BGCAL_FINE0)
#define BGCAL_COARSE_gm (1 << BGCAL2 | 1 << BGCAL1 | BGCAL0)
#define ADC_VREF_AVCC   ADC_VREF_1V6

enum adc_mux_setting {
	/*!  ADC0, single ended */
	ADC_MUX_ADC0 = 0x0,
	/*!  ADC1, single ended */
	ADC_MUX_ADC1 = 0x1,
	/*!  ADC2, single ended */
	ADC_MUX_ADC2 = 0x2,
	/*!  ADC3, single ended */
	ADC_MUX_ADC3 = 0x3,
	/*!  ADC4, single ended */
	ADC_MUX_ADC4 = 0x4,
	/*!  ADC5, single ended */
	ADC_MUX_ADC5 = 0x5,
	/*!  ADC6, single ended */
	ADC_MUX_ADC6 = 0x6,
	/*!  ADC7, single ended */
	ADC_MUX_ADC7 = 0x7,
	/*!  1.1V internal bandgap voltage */
	ADC_MUX_1V2 = 0x1E,
	/*!  GND, single ended */
	ADC_MUX_GND = 0x1F,
	/*!  Positive: ADC0 Negative: ADC0 Gain: 10x */
	ADC_MUX_ADC0_ADC0_10X = 0x8,
	/*!  Positive: ADC1 Negative: ADC0 Gain: 10x */
	ADC_MUX_ADC1_ADC0_10X = 0x9,
	/*!  Positive: ADC0 Negative: ADC0 Gain: 200x */
	ADC_MUX_ADC0_ADC0_200X = 0xA,
	/*!  Positive: ADC1 Negative: ADC0 Gain: 200x */
	ADC_MUX_ADC1_ADC0_200X = 0xB,
	/*!  Positive: ADC2 Negative: ADC2 Gain: 10x */
	ADC_MUX_ADC2_ADC2_10X = 0xC,
	/*!  Positive: ADC3 Negative: ADC2 Gain: 10x */
	ADC_MUX_ADC3_ADC2_10X = 0xD,
	/*!  Positive: ADC2 Negative: ADC2 Gain: 200x */
	ADC_MUX_ADC2_ADC2_200X = 0xE,
	/*!  Positive: ADC3 Negative: ADC2 Gain: 200x */
	ADC_MUX_ADC3_ADC2_200X = 0xF,
	/*!  Positive: ADC0 Negative: ADC1 */
	ADC_MUX_ADC0_ADC1 = 0x10,
	/*!  Positive: ADC1 Negative: ADC1 */
	ADC_MUX_ADC1_ADC1 = 0x11,
	/*!  Positive: ADC2 Negative: ADC1 */
	ADC_MUX_ADC2_ADC1 = 0x12,
	/*!  Positive: ADC3 Negative: ADC1 */
	ADC_MUX_ADC3_ADC1 = 0x13,
	/*!  Positive: ADC4 Negative: ADC1 */
	ADC_MUX_ADC4_ADC1 = 0x14,
	/*!  Positive: ADC5 Negative: ADC1 */
	ADC_MUX_ADC5_ADC1 = 0x15,
	/*!  Positive: ADC6 Negative: ADC1 */
	ADC_MUX_ADC6_ADC1 = 0x16,
	/*!  Positive: ADC7 Negative: ADC1 */
	ADC_MUX_ADC7_ADC1 = 0x17,
	/*!  Positive: ADC0 Negative: ADC2 */
	ADC_MUX_ADC0_ADC2 = 0x18,
	/*!  Positive: ADC1 Negative: ADC2 */
	ADC_MUX_ADC1_ADC2 = 0x19,
	/*!  Positive: ADC2 Negative: ADC2 */
	ADC_MUX_ADC2_ADC2 = 0x1A,
	/*!  Positive: ADC3 Negative: ADC2 */
	ADC_MUX_ADC3_ADC2 = 0x1B,
	/*!  Positive: ADC4 Negative: ADC2 */
	ADC_MUX_ADC4_ADC2 = 0x1C,
	/*!  Positive: ADC5 Negative: ADC2 */
	ADC_MUX_ADC5_ADC2 = 0x1D,
	/*!  Temperature Sensor */
	ADC_MUX_TEMP_SENSOR     = 0x29,
	/*!  SRAM Back-bias Voltage VDRTBBP */
	ADC_MUX_VDRTBBP = 0x34,
	/*!  SRAM Back-bias Voltage VDRTBBN */
	ADC_MUX_VDRTBBN = 0x3D,
};

enum adc_voltage_reference {
	/*!  AREF, internal Vref turned off */
	ADC_VREF_AREF = (0x0 << REFS0),
	/*!  AVDD, 1.8V Regulated analog supply voltage */
	ADC_VREF_AVDD_1V8 = (0x1  << REFS0),
	/*!  Internal 1.5V Voltage Reference */
	ADC_VREF_1V5 = (0x2  << REFS0),
	/*!  Internal 1.6V Voltage Reference */
	ADC_VREF_1V6 = (0x3  << REFS0),
};

enum adc_reference_voltage_fine_calibration {
	/*!  Reference voltage fine calibration voltage center value */
	REF_VOLTAGE_FINE_CENTER = (0x0 << BGCAL_FINE0),
	/*!  Reference voltage fine calibration voltage step up */
	REF_VOLTAGE_FINE_STEP_UP = (0x1 << BGCAL_FINE0),
	/*!  Reference voltage fine calibration voltage step down */
	REF_VOLTAGE_FINE_STEP_DOWN = (0x8 << BGCAL_FINE0),
	/*!  Reference voltage fine calibration voltage highest voltage */
	REF_VOLTAGE_FINE_HIGHEST_VOLTGAE = (0x7 << BGCAL_FINE0),
	/*!  Reference voltage fine calibration voltage lowest voltage */
	REF_VOLTAGE_FINE_LOWEST_VOLTGAE = (0xF << BGCAL_FINE0),
};

enum adc_reference_voltage_coarse_calibration {
	/*!  Reference voltage coarse calibration voltage center value */
	REF_VOLTAGE_COARSE_CENTER = (0x4 << BGCAL0),
	/*!  Reference voltage coarse calibration voltage step up */
	REF_VOLTAGE_COARSE_STEP_UP = (0x3 << BGCAL0),
	/*!  Reference voltage coarse calibration voltage step down */
	REF_VOLTAGE_COARSE_STEP_DOWN = (0x5 << BGCAL0),
	/*!  Reference voltage coarse calibration voltage highest voltage */
	REF_VOLTAGE_COARSE_HIGHEST_VOLTGAE = (0x0 << BGCAL0),
	/*!  Reference voltage coarse calibration voltage lowest voltage */
	REF_VOLTAGE_COARSE_LOWEST_VOLTGAE = (0x7 << BGCAL0),
};

/**
 * \brief check the status of AVDD domain
 *
 * \return true if AVDD is powered-up
 */
static inline bool adc_avdd_supply_check(void)
{
	return ADCSRB & (1 << AVDDOK);
}

/**
 * \brief check the status of internal generated reference voltage
 *
 * \return true if internal generated reference voltage is powered-up
 */
static inline bool adc_ref_voltage_check(void)
{
	return ADCSRB & (1 << REFOK);
}

/**
 * \brief wait until analog voltage regulator is powered on
 *
 */
static inline void adc_wait_avddok(void)
{
	while (!(ADCSRB & (1 << AVDDOK)));
}

/**
 * \brief set the number of ADC clock cycles required for the start-up time
 * of the analog blocks.
 *
 * \param adsut_val     ADC Startup time
 *
 * \note ADSUT = ((startup_time/4) - 1), minimum 20uSec & max of 128uSec
 */
static inline void adc_start_up_time(uint8_t adsut_val)
{
	adsut_val &= ADSUT_gm;
	ADCSRC = (ADCSRC & ~ADSUT_gm) | adsut_val;    
}

/**
 * \brief set the number of ADC clock cycles required for sampling time
 * of the analog input voltage
 *
 * \param adtht_val		Gain Amplifier Initialization Time
 *
 * \note ADTHT = ((gain_time/2) - 2)
 */
static inline void adc_track_and_hold_time(uint8_t adtht_val)
{
	adtht_val &= ADTHT_gm;
	ADCSRC = (ADCSRC & ~ADTHT_gm) | adtht_val;
}

/**
 * \brief calibrates reference voltage of the ADC with resolution of 2mV
 *
 * \param val		reference_voltage_fine_calibration
 *
 */
static inline void adc_ref_voltage_fine_set(
		enum adc_reference_voltage_fine_calibration val)
{
	BGCR &= ~BGCAL_FINE_gm;
	BGCR |= (uint8_t)val;
}

/**
 * \brief calibrates reference voltage of the ADC with resolution of 10mV
 *
 * \param val		reference_voltage_coarse_calibration
 *
 */
static inline void adc_ref_voltage_coarse_set(
		enum adc_reference_voltage_coarse_calibration val)
{
	BGCR &= ~BGCAL_COARSE_gm;
	BGCR |= (uint8_t)val;
}

#ifdef __cplusplus
}
#endif

#endif  /* _ADC_MEGARF */
