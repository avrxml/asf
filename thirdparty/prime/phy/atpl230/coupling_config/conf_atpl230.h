/**
 * \file
 *
 * \brief PHY Layer Configuration for ATPL230
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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

#ifndef CONF_ATPL230_H_INCLUDE
#define CONF_ATPL230_H_INCLUDE

#include "compiler.h"

/* Declaration of Phy parameters structure */
typedef struct {
	uint8_t driver1Mode;         /*  Indicate if driver 1 is extern driver
	                              * (1) or intern driver (0) */
	uint8_t driver2Mode;         /*  Indicate if driver 2 is extern driver
	                              * (1) or intern driver (0) */
	uint8_t txrx1Polarity;       /*  Indicate if txrx1 polarity is high or
	                              * low active */
	uint8_t txrx2Polarity;       /*  Indicate if txrx2 polarity is high or
	                              * low active */
	uint8_t autoDetectBranch;    /*  Flag to enable branch auto detection
	                              **/
	uint8_t impedance;           /*  When branch auto detection disabled,
	                              * indicate impedance to use */
	uint8_t high_Z_driver;       /*  Select driver for high impedance */
	uint8_t low_Z_driver;        /*  Select driver for low impedance */
	uint8_t vlow_Z_driver;       /*  Select driver for very low impedance
	                              **/
	uint8_t agc0KrssiOffset;     /*  Offset for received signal strength
	                              * (rssi) according to AGC 0 configuration
	                              **/
	uint8_t agc1KrssiOffset;     /*  Offset for received signal strength
	                              * (rssi) according to AGC 1 configuration
	                              **/
	uint8_t agc2KrssiOffset;     /*  Offset for received signal strength
	                              * (rssi) according to AGC 2 configuration
	                              **/
	uint8_t agc3KrssiOffset;     /*  Offset for received signal strength
	                              * (rssi) according to AGC 3 configuration
	                              **/
	uint8_t n1Delay;             /*  N1 Delay */
	uint8_t p1Delay;             /*  P1 Delay */
	uint8_t n2Delay;             /*  N2 delay */
	uint8_t p2Delay;             /*  P2 delay */
	uint8_t emit1Active;         /*  Emit 1 Active */
	uint8_t emit2Active;         /*  Emit 2 Active */
	uint8_t emit3Active;         /*  Emit 3 Active */
	uint8_t emit4Active;         /*  Emit 4 Active */
	uint8_t txrxChannel;         /*  Transmission/Reception Channel */
	uint8_t atplCoupling;        /*  Coupling board in use */
	uint8_t primeMode;           /*  PRIME mode (see mode values in
	                              * atpl230.h) */
	uint16_t timeBeforeTxHighZ;  /*  Time in 10's of us for HIMP pin before
	                              * transmission with high impedance */
	uint16_t timeBeforeTxLowZ;   /*  Time in 10's of us for TXRX pin before
	                              * transmission with low impedance */
	uint16_t timeAfterTxHighZ;   /*  Time (unit depends on platform) for
	                              * HIMP pin after transmission with high
	                              * impedance */
	uint16_t timeAfterTxLowZ;    /*  Time (unit depends on platform) for
	                              * TXRX pin after transmission with low
	                              * impedance */
	uint16_t rxCorrThreshold;    /*  Threshold for autocorrelation filter
	                              **/
	uint8_t enable_vlow_pk;      /*  Flag to enable Peak Cut On in case of
	                              * low impedance */
} atpl230Cfg_t;

extern atpl230Cfg_t atpl230Cfg;
void phy_init_cfg(void);
void phy_init_coupling_cfg(void);
void phy_init_channel_cfg(void);
void phy_init_adc_cfg(void);

#endif  /* CONF_ATPL230_H_INCLUDE */
