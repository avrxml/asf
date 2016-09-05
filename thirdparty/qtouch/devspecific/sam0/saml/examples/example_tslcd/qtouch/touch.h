/* This source file is part of the ATMEL QTouch Library 5.0.6 */

/*****************************************************************************
 *
 * \file
 *
 * \brief  This file contains the SAMD QTouch Library pin, register and
 * sensors configuration options for Capacitive Touch acquisition using
 * the PTC module.
 *
 * - User-guide:          QTouch Library Peripheral Touch Controller User Guide.
 * - Support email:      www.atmel.com/design-support/
 *
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
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
 *    Atmel micro-controller product.
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
 ******************************************************************************/

#ifndef TOUCH_CONFIG_SAMD_H
#define TOUCH_CONFIG_SAMD_H

/*----------------------------------------------------------------------------
 *                   Mutual Cap method enable/disable.
 *
 *
 *----------------------------------------------------------------------------*/

/**
 * Enable/Disable Self/Mutual Capacitance method.
 */
#define DEF_TOUCH_MUTLCAP               (1u)
#define DEF_TOUCH_SELFCAP               (0u)

/*----------------------------------------------------------------------------
 *                   PTC module clock and interrupt level configuration.
 *
 *
 *----------------------------------------------------------------------------*/

/**
 * PTC Module clock configuration.
 * Before using the QTouch library API, the PTC module clock generator source
 * should be configured appropriately.  The PTC module clock can be generated
 * using any of the eight generic clock generators (GCLK0-GCLK7).  The
 *associated
 * generic clock multiplexer should be configured such that the PTC module clock
 * is set to 4MHz.  Refer touch_configure_ptc_clock API in touch.c for more
 * information.
 */

/**
 * PTC Module interrupt level.
 * The Nested Vectored Interrupt Controller (NVIC) in the SAMD supports
 * four different priority levels.  The priority level of the PTC end of
 * conversion ISR, used within QTouch library can be chosen based on application
 * requirements in order to accommodate time critical operations.
 * Range: 0u (Highest priority) to 3u (Lowest priority)
 * For more details refer to the Cortex-M0 Technical Reference Manual.
 */
#define   DEF_TOUCH_PTC_ISR_LVL       (3u)

/*----------------------------------------------------------------------------
 *                   Mutual Cap method pin configuration.
 *
 *
 *----------------------------------------------------------------------------*/

/**
 * Mutual Cap method touch channel nodes (GPIO pins) selected.
 * Mutual capacitance method use a pair of sensing electrodes for each touch
 * channel.  These are denoted as X and Y lines.  Touch channel numbering
 * follows the order in which X-Y nodes are specified.  Capacitance measurement
 * is done sequentially in the order in which touch channel nodes are specified.
 */
#define DEF_MUTLCAP_NODES  X(20), Y(21), X(12), Y(21), X(12), Y(11), X(20), Y(11), X(13), Y(11)

/*----------------------------------------------------------------------------
 *                   Mutual Cap method channel and sensor configuration.
 *
 *
 *----------------------------------------------------------------------------*/

/**
 * Mutual Cap number of channels.
 * Specify the number of Mutual Cap touch channels to be used by the Touch
 * Library. A key is formed used one touch channel.  A rotor or slider can be
 * formed using 3 to 8 touch channels.
 * Range: 1u to 256u.
 */
#define DEF_MUTLCAP_NUM_CHANNELS   (5) /* Total number of channels */

/**
 * Mutual Cap number of Sensors.
 * Specify the number of Mutual Cap touch sensors to be used by the Touch
 * Library. A sensor is either a key, rotor or slider.
 * Example configuration: If the configuration has 6 keys (a key is formed
 * using one X-Y node), one rotor (a Mutual Cap rotor is formed using 3 X-Y
 * nodes) and one slider (a Mutual Cap slider is formed using 3 X-Y nodes),
 * then the number of sensors is 6 key + 1 rotor + 1 slider = 8 sensors.
 * Range: 1u to 256u.
 */
#define DEF_MUTLCAP_NUM_SENSORS  (5)  /* Total number of sensors */

/**
 * Mutual Cap number of Rotors and Sliders.
 * Specify the total number of Mutual Cap Rotors and Sliders to be used by
 * the Touch Library.  The number of Rotors and Sliders mentioned here is part
 * of the Total number of sensors specified in the QT_NUM_SENSORS macro.  When
 * no rotors or slider are required, specify a value of 0u.
 * Range: 0u to 64u.
 */
#define DEF_MUTLCAP_NUM_ROTORS_SLIDERS  (0)  /* Number of rotor sliders */

/*----------------------------------------------------------------------------
 *                   Mutual Cap method acquisition parameters.
 *
 *
 *----------------------------------------------------------------------------*/

/**
 * Mutual Cap filter level setting.
 * The filter level setting controls the number of samples taken
 * to resolve each acquisition. A higher filter level setting provides
 * improved signal to noise ratio under noisy conditions, while
 * increasing the total time for measurement resulting in increased
 * power consumption and response time.  Refer filter_level_t in
 *touch_api_ptc.h
 * Range: FILTER_LEVEL_1 (one sample) to FILTER_LEVEL_64 ( 64 samples).
 */
#define DEF_MUTLCAP_FILTER_LEVEL  FILTER_LEVEL_64        /* Filter level */

/**
 * Mutual Cap auto oversample setting.
 * Auto oversample controls the automatic oversampling of sensor channels when
 * unstable signals are detected with the default setting of ‘Filter level?
 * Enabling Auto oversample results in 'Filter level' x 'Auto Oversample' number
 * of samples taken on the corresponding sensor channel when an unstable signal
 * is observed.  In a case where ‘Filter level?is set to FILTER_LEVEL_4 and
 * ‘Auto Over-sample?is set to AUTO_OS_4, 4 over-samples are taken with stable
 * signal values and 16 over-samples are taken when unstable signal is detected.
 * Refer auto_os_t in touch_api_ptc.h
 * Range: 0 to 7
 	0 --> AUTO_OS_DISABLE, 	1 --> AUTO_OS_2,
	2 --> AUTO_OS_4,		3 --> AUTO_OS_8,
	4 --> AUTO_OS_16,		5 --> AUTO_OS_32,
	6 --> AUTO_OS_64,		7 --> AUTO_OS_128
 */
#define  DEF_MUTLCAP_AUTO_OS 0               // Automatic OverSampling

/**
 * Stability limit is the variance in sensor signal value under noisy environment.  
 * The signal stability limit level is set to auto trigger over-samples on noise presence.
 * It is recommended to keep this setting close to the lowest sensor detect threshold of 
 * the system and tune it further based on the noise.
 * Range: 1 to 1000
 */
#define DEF_MUTLCAP_AUTO_OS_SIGNAL_STABILITY_LIMIT 10

/**
 * Mutual Cap gain per touch channel.
 * Gain is applied on a per-channel basis to allow a scaling-up of the touch
 * sensitivity on contact.
 * Note: delta on touch contact, not the resting signal which is measured on
 * each sensor.
 * Refer gain_t in touch_api_ptc.h
 * Range:GAIN_1 (no scaling) to GAIN_32 (scale-up by 32)
 */
#define DEF_MUTLCAP_GAIN_PER_NODE  GAIN_1, GAIN_1, GAIN_1, GAIN_1, GAIN_1

/*----------------------------------------------------------------------------
 *   Tuning for Noise performance, touch response time and Power consumption.
 *
 *
 *----------------------------------------------------------------------------*/

/**
 * Refer QTouch Library Peripheral Touch Controller User Guide for detailed
 * information on tuning for Noise performance, touch response time and  Power
 * consumption
 */

/**
 * For best noise performance, set -
 * - DEF_MUTLCAP_FREQ_MODE to FREQ_MODE_HOP
 * - DEF_MUTLCAP_SENSE_RESISTOR to RSEL_VAL_100
 * - use AUTO_TUNE_PRSC input to touch_xxcap_sensors_calibrate() API in touch.c
 * Based on the type of noise, FREQ_MODE_SPREAD or FREQ_MODE_SPREAD_MEDIAN can
 * also be used.
 *
 * For best power consumption, set -
 * - DEF_MUTLCAP_FREQ_MODE to FREQ_MODE_NONE
 * - DEF_MUTLCAP_CLK_PRESCALE to PRSC_DIV_SEL_1
 * - use AUTO_TUNE_RSEL input to touch_xxcap_sensors_calibrate() API in touch.c
 */

/**
 * Mutual Cap acquisition frequency mode.
 *
 * FREQ_MODE_HOP:
 * When frequency mode hopping option is selected, the PTC runs a
 * frequency hopping cycle with subsequent measurements done using
 * the three PTC acquisition frequency delay settings as specified in
 * DEF_MUTLCAP_HOP_FREQS.
 *
 * FREQ_MODE_SPREAD:
 * When frequency mode spread spectrum option is selected, the PTC
 * runs with spread spectrum enabled for jitter delay based
 * acquisition.
 *
 * FREQ_MODE_SPREAD_MEDIAN:
 * When frequency mode spread spectrum median option is selected,
 * the PTC runs with spread spectrum enabled.  In this case, an
 * additional software median filter is applied to the measured
 * signal values.
 *
 * FREQ_MODE_NONE:
 * When frequency mode none option is selected, the PTC runs at
 * constant speed.  This mode is suited for best power consumption.
 */
#define DEF_MUTLCAP_FREQ_MODE FREQ_MODE_HOP

/**
 * PTC acquisition frequency delay setting.
 *
 * Specify three frequency hop delay settings.
 *
 * The PTC acquisition frequency is dependent on the Generic clock
 * input to PTC and PTC clock pre-scaler setting.  This delay setting
 * inserts "n" PTC clock cycles between consecutive measurements on
 * a given sensor, thereby changing the PTC acquisition frequency.
 * FREQ_HOP_SEL_1 setting inserts 0 PTC clock cycle between consecutive
 * measurements.  FREQ_HOP_SEL_16 setting inserts 15 PTC clock cycles.
 * Hence, higher delay setting will increase the total time taken for
 * capacitance measurement on a given sensor as compared to a lower
 * delay setting.
 *
 * A desired setting can be used to avoid noise around the same frequency
 * as the acquisition frequency.
 * Range: FREQ_HOP_SEL_1 to FREQ_HOP_SEL_16
 */
#define DEF_MUTLCAP_HOP_FREQS  FREQ_HOP_SEL_1, FREQ_HOP_SEL_2, FREQ_HOP_SEL_3

/**
 * Mutual cap PTC clock pre-scalar.
 * PTC clock prescale setting.   Refer touch_configure_ptc_clock() API in
 *touch.c
 * Example: if Generic clock input to PTC = 4MHz, then:
 * PRSC_DIV_SEL_1   sets PTC Clock to  4MHz
 * PRSC_DIV_SEL_2   sets PTC Clock to  2MHz
 * PRSC_DIV_SEL_4   sets PTC Clock to 1MHz
 * PRSC_DIV_SEL_8   sets PTC Clock to 500KHz
 */
#define DEF_MUTLCAP_CLK_PRESCALE_PER_NODE PRSC_DIV_SEL_1

/**
 * PTC series resistor setting.  For Mutual cap mode, this series
 * resistor is switched internally on the Y-pin.  For Self cap mode,
 * the series resistor is switched internally on the Sensor pin.
 *
 * Example:
 * RSEL_VAL_0   sets internal series resistor to 0ohms.
 * RSEL_VAL_20  sets internal series resistor to 20Kohms.
 * RSEL_VAL_50  sets internal series resistor to 50Kohms.
 * RSEL_VAL_75  sets internal series resistor to 75Kohms.
 * RSEL_VAL_100  sets internal series resistor to 100Kohms.
 * RSEL_VAL_200  sets internal series resistor to 200Kohms.
 */
#define DEF_MUTLCAP_SENSE_RESISTOR_PER_NODE RSEL_VAL_200

/**
	This is the default auto tune mode selection.
	This is used in the AUTO calibration routine,
	The allowed values are 0 to 2
	0 --> DEF_AUTO_TUNE_NONE if Auto tune is not required
	1 --> DEF_AUTO_TUNE_PRSC if the Pre-scaler needs to be tuned
	2 --> DEF_AUTO_TUNE_RSEL if the Series resistor needs to be tuned 
	3 --> DEF_AUTO_TUNE_CSD  this is available only in C2x devices,
DEF_AUTO_TUNE_CSD tunes internal Charge sharing delay for proper charge transfer
*/
#define DEF_MUTL_AUTO_TUNE_VALUE		0
/*----------------------------------------------------------------------------
 *                   Mutual Cap method sensor global parameters.
 *
 *
 *----------------------------------------------------------------------------*/

/*! \name Mutual Cap Global acquisition parameters.
 * Refer the Touch Library User guide for more information on these parameters.
 */
/* ! @{ */

/**
 * Mutual Cap Sensor measurement interval.
 * Specify period in milliseconds.  Example, DEF_TOUCH_MEASUREMENT_PERIOD_MS
 *50u
 * will perform measurement on touch sensors every 50msec.
 */
#define DEF_TOUCH_MEASUREMENT_PERIOD_MS 100u


/**
 * Mutual Cap Sensor detect integration (DI) limit.
 * Range: 0u to 255u.
 */
#define   DEF_MUTLCAP_DI         0u


/**
 * Mutual Cap Sensor towards touch drift rate.
 * Units: 200ms
 * Default value: 20 = 4 seconds.
 * Range: 1u to 127u.
 */
#define  DEF_MUTLCAP_TCH_DRIFT_RATE        20u


/**
 * Mutual Cap Sensor away from touch drift rate.
 * Units: 200ms
 * Default value: 5u = 1 second.
 * Range: 1u to 127u.
 */
#define  DEF_MUTLCAP_ATCH_DRIFT_RATE         5u


/**
 * Mutual Cap Sensor maximum ON time duration.
 * Units: 200ms (Example: a value 5u indicated Max ON duration of 1 second.)
 * Default value: 0 (No maximum ON time limit).
 * Range: 0u to 255u.
 */
#define   DEF_MUTLCAP_MAX_ON_DURATION       0u


/**
 * Mutual Cap Sensor drift hold time.
 * Units: 200ms
 * Default value: 20 (hold off drifting for 4 seconds after leaving detect).
 * Range: 1u to 255u.
 */
#define   DEF_MUTLCAP_DRIFT_HOLD_TIME        20u


/**
 * Mutual Cap Sensor away from touch recalibration delay.
 * Default value: 10.
 * Range: 0u to 255u.
 */
#define   DEF_MUTLCAP_ATCH_RECAL_DELAY       10u


/** Mutual Cap Sensor away from touch recalibration threshold.
 * Default: RECAL_50 (recalibration threshold = 50% of detection threshold).
 * Range: refer recal_threshold_t enum in touch_api_ptc.h.
 */
#define   DEF_MUTLCAP_ATCH_RECAL_THRESHOLD      0u


/** Mutual Cap Sensor post-processing mode.
 * Default: TOUCH_LIBRARY_DRIVEN.
 * Range: TOUCH_LIBRARY_DRIVEN or TOUCH_APPLN_DRIVEN refer in touch_api_ptc.h.
 */
#define   DEF_MUTLCAP_TOUCH_POSTPROCESS_MODE       TOUCH_LIBRARY_DRIVEN

/* ! @} */

/*----------------------------------------------------------------------------
*      Mutual Cap method moisture detection feature parameters.
*
*
*----------------------------------------------------------------------------*/
/*! \name Mutual Cap method moisture feature parameters.
* Refer the Touch Library User guide for more information on these parameters.
*/
/* ! @{ */

/**
 * Enable or disable moisture detection feature.
 */
#define   DEF_MUTLCAP_MOIS_TOLERANCE_ENABLE       (0u)

/**
 * Enable or disable quick re-burst feature within a given moisture group.
 * When enabled, if within a given moisture group, when any sensor is touched, 
 * repeated measurements are done only that sensor to resolve detect integration or de-bounce.
 * When disabled, if within a given moisture group, when any sensor is touched, repeated measurements are done 
 * on all sensors within the moisture group to resolve detect integration or de-bounce.
 * It is recommended to enable this feature for best touch response time.
 */
#define   DEF_MUTLCAP_MOIS_QUICK_REBURST_ENABLE       (1u)

/**                          
  * Mutual cap number of moisture groups  
  */  
#define   DEF_MUTLCAP_NUM_MOIS_GROUPS       (0u)

/* ! @} */

/*
 * AKS grouping function can be enabled/disabled
 * 1u = AKS grouping functionality is enabled 
 * 0u = AKS grouping functionality is disabled   
 */
#define   DEF_MUTLCAP_AKS_ENABLE       (0u)
/*
 * Charge share delay indicates the number of additional charge cycles that are inserted within a capacitance measurement cycle 
 * to ensure full charging of the touch sensor.
 * The CSD value is dependent on the sensor capacitance along with the series resistor on the Y line.  
 * When manual tuning is done, the CSD value for the sensor with largest combination of capacitance along with series resistance 
 * should be considered.
 * allowed Range 0-255                                                                     
 */
#define DEF_MUTL_CAP_CSD_VALUE 20

/*----------------------------------------------------------------------------
 *                   Mutual Cap method noise measurement & lockout.
 *
 *
 *----------------------------------------------------------------------------*/

/*! \name Self Cap Global acquisition parameters.
 * Refer the Touch Library User guide for more information on these parameters.
 */
/* ! @{ */

/**
 * Noise measurement enable/disable
 * If configured as 1, noise measurement will be enabled
 * If configured as 0, noise measurement will be disabled
 */
#define   DEF_MUTLCAP_NOISE_MEAS_ENABLE       (0u)

/**
 * Stability limit is the variance in sensor signal value under noisy environment.
 * Any noise level over and above the noise signal stability limit contributes to the Noise limit.
 * It is recommended to keep this setting close to the lowest sensor detect threshold of 
 * the system and tune it further based on the noise.  
 * Range: 1 to 1000
 */
#define   DEF_MUTLCAP_NOISE_MEAS_SIGNAL_STABILITY_LIMIT       20u

/**
 * The noise limit specifies the limit to the total noise accumulated over the noise buffer count.
 * If the accumulated noise exceeds the noise limit, then lockout is triggered.
 * Range: 1 to 255
 */
#define   DEF_MUTLCAP_NOISE_LIMIT       20u

/**
 * Noise buffer count
 * Selection of buffer count for noise calculation.
 * Defines the buffer limit for internal noise measurement
 * Range: 3 to 10   (select value N + 1, here N nmber of  samples)
 * if N = 4 then set  DEF_NM_BUFFER_CNT  5u ->> (N + 1).
 * Default : 5u
 */
#define   DEF_MUTLCAP_NOISE_MEAS_BUFFER_CNT       (5u)

/**
 * Mutual cap method : noisy Sensor lockout settings.
 * 0u: single sensor lockout.
 * 1u: Global sensor lockout.
 * 2u : No lockout
 * Range : 0 to 2
 */
#define   DEF_MUTLCAP_LOCKOUT_SEL       2

/**
 * Mutual cap Lockout count down
 * If the sensor signal is moves from noisy to a good condition and stays there
 * for a count value higher than the max count defined by user, sensor is
 *declared as stable
 * Range: 1 to 255
 */
#define   DEF_MUTLCAP_LOCKOUT_CNTDOWN       10

/* ! @} */

/*----------------------------------------------------------------------------
 *                   Mutual Cap method Frequency auto tune.
 *
 *
 *----------------------------------------------------------------------------*/

/*! \name Self Cap Global acquisition parameters.
 * Refer the Touch Library User guide for more information on these parameters.
 */
/* ! @{ */

/**
 * Freq auto tune enable/disable ( applicable only to freq_hop mode)
 * If configured as 1, Freq auto tune will be enabled
 * If configured as 0, Freq auto tune will be disabled
 */
#define   DEF_MUTLCAP_FREQ_AUTO_TUNE_ENABLE       (1u)

/**
 * Stability limit is the variance in sensor signal value under noisy environment. 
 * A signal stability limit level is set to auto tune acquisition frequency on noise presence.
 * It is recommended to keep this setting close to the lowest sensor detect threshold of 
 * the system and tune it further based on the noise.
 * Range: 1 to 1000
 * Note : this applies only for FREQ_MODE_HOP
 */
#define   DEF_MUTLCAP_FREQ_AUTO_TUNE_SIGNAL_STABILITY_LIMIT       20u

/**
 * Frequency Auto tune-in count
 * If the channel signal is noisy for a count value higher than the max count
 *defined by user,
 * system will trigger auto tune
 * Range: 1 to 255
 * Note : this applies only for FREQ_MODE_HOP
 */
#define   DEF_MUTLCAP_FREQ_AUTO_TUNE_IN_CNT       12

/* ! @} */

/*----------------------------------------------------------------------------
 *                   Mutual Cap method callback functions.
 *
 *
 *----------------------------------------------------------------------------*/

/*! \name Mutual Cap Callback functions.
 */
/* ! @{ */

/**
 * Mutual Cap Filter callback function.
 * A filter callback (when not NULL) is called by the Touch Library each time
 * a new set of Signal values are available.
 * An Example filter callback function prototype.
 * void touch_filter_callback( touch_filter_data_t *p_filter_data );
 */
#define DEF_MUTLCAP_FILTER_CALLBACK              (NULL)

/* ! @} */

/*----------------------------------------------------------------------------
 *                   QDebug debug communication parameters.
 *
 *
 *----------------------------------------------------------------------------*/

/*! \name QDebug debug communication parameters.
 */
/* ! @{ */

#define DEF_TOUCH_QDEBUG_ENABLE_MUTLCAP

#define DEF_TOUCH_QDEBUG_ENABLE 0u

#if DEF_TOUCH_QDEBUG_ENABLE == 1
#define _DEBUG_INTERFACE_
#endif

/* ! @} */

#endif /* TOUCH_CONFIG_SAMD_H */
