/**
 * \file
 *
 * \brief PHY Layer Configuration for ATPL230
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
