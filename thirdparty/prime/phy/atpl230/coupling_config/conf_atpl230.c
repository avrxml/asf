/**
 * \file
 *
 * \brief PHY Layer Configuration for ATPL230.
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

#include "conf_atpl230.h"
#include "conf_phy.h"
#include "atpl230.h"

/* ASF includes */
#include "asf.h"

extern atpl230ChnCfg_t atpl230ChnCfg;

/**
 * \brief This function initiates PRIME PHY layer configuration and
 * it must be implemented externally by the user
 *
 * \note Init PHY layer configuration with the rest of PHY layer functions. This
 * function will be invoked by function phy_init () and it allows user to
 * configure PHY layer initialization. User can adapt the PHY layer to every
 * hardware platform.
 * This is an example of usage, configuring analog front end and different
 * threshold for load detection.
 * ATMEL strongly recommend to consult with technical support to change any
 * parameter in this file.
 */
void phy_init_cfg(void)
{
	atpl230Cfg.impedance = HI_STATE;
	atpl230Cfg.autoDetectBranch = true;
	atpl230Cfg.timeBeforeTxHighZ = 10;
	atpl230Cfg.timeBeforeTxLowZ = 10;
	atpl230Cfg.timeAfterTxHighZ = 10;
	atpl230Cfg.timeAfterTxLowZ = 10;
	atpl230Cfg.agc0KrssiOffset = 0x0B;
	atpl230Cfg.agc1KrssiOffset = 0x16;
	atpl230Cfg.agc2KrssiOffset = 0x23;
	atpl230Cfg.agc3KrssiOffset = 0x29;
	atpl230Cfg.rxCorrThreshold = 0x200;

	/* Coupling Board configuration. Select configuration in conf_atpl230.h
	**/
#ifdef BOARD_COUPLING
	atpl230Cfg.atplCoupling = BOARD_COUPLING;
#else
#  error No known Atmel coupling board defined
#endif
}

/**
 * \brief This function initiates PRIME Coupling Board configuration and
 * it must be implemented externally by the user
 *
 * ATMEL strongly recommend to consult with technical support to change any
 * parameter in this file.
 */
void phy_init_coupling_cfg(void)
{
	switch (atpl230Cfg.atplCoupling) {
	case ATPLCOUP000_v1:
	case ATPLCOUP000_v2:
		atpl230Cfg.driver1Mode = EXTERNAL_DRV_MODE;
		atpl230Cfg.driver2Mode = EXTERNAL_DRV_MODE;
		atpl230Cfg.txrx1Polarity = DRV_POL_TX_0_RX_1;
		atpl230Cfg.txrx2Polarity = DRV_POL_TX_0_RX_1;
		atpl230Cfg.high_Z_driver = DRIVER_2;
		atpl230Cfg.low_Z_driver = DRIVER_1;
		atpl230Cfg.vlow_Z_driver = DRIVER_1;
		atpl230Cfg.n1Delay = 0x21;
		atpl230Cfg.p1Delay = 0x20;
		atpl230Cfg.n2Delay = 0x21;
		atpl230Cfg.p2Delay = 0x20;
		atpl230Cfg.emit1Active = 0x77;
		atpl230Cfg.emit2Active = 0x77;
		atpl230Cfg.emit3Active = 0x77;
		atpl230Cfg.emit4Active = 0x77;
		atpl230Cfg.txrxChannel = 1;
		atpl230Cfg.enable_vlow_pk = true;
		break;

	case ATPLCOUP001_v1:
		atpl230Cfg.driver1Mode = EXTERNAL_DRV_MODE;
		atpl230Cfg.txrx1Polarity = DRV_POL_TX_0_RX_1;
		atpl230Cfg.high_Z_driver = DRIVER_1;
		atpl230Cfg.low_Z_driver = DRIVER_1;
		atpl230Cfg.vlow_Z_driver = DRIVER_1;
		atpl230Cfg.n1Delay = 0x21;
		atpl230Cfg.p1Delay = 0x20;
		atpl230Cfg.emit1Active = 0x77;
		atpl230Cfg.emit2Active = 0x77;
		atpl230Cfg.txrxChannel = 1;
		atpl230Cfg.enable_vlow_pk = false;
		break;

	case ATPLCOUP002_v1:
		atpl230Cfg.driver1Mode = EXTERNAL_DRV_MODE;
		atpl230Cfg.driver2Mode = INTERNAL_DRV_MODE;
		atpl230Cfg.txrx1Polarity = DRV_POL_TX_0_RX_1;
		atpl230Cfg.high_Z_driver = DRIVER_2;
		atpl230Cfg.low_Z_driver = DRIVER_1;
		atpl230Cfg.vlow_Z_driver = DRIVER_1;
		atpl230Cfg.n1Delay = 0x21;
		atpl230Cfg.p1Delay = 0x20;
		atpl230Cfg.emit1Active = 0x77;
		atpl230Cfg.emit2Active = 0x77;
		atpl230Cfg.emit3Active = 0x33;
		atpl230Cfg.emit4Active = 0x33;
		atpl230Cfg.txrxChannel = 4;
		atpl230Cfg.enable_vlow_pk = false;
		break;

	case ATPLCOUP002_v2:
		atpl230Cfg.driver1Mode = EXTERNAL_DRV_MODE;
		atpl230Cfg.driver2Mode = INTERNAL_DRV_MODE;
		atpl230Cfg.txrx1Polarity = DRV_POL_TX_0_RX_1;
		atpl230Cfg.txrx2Polarity = DRV_POL_TX_0_RX_1;
		atpl230Cfg.high_Z_driver = DRIVER_2;
		atpl230Cfg.low_Z_driver = DRIVER_1;
		atpl230Cfg.vlow_Z_driver = DRIVER_1;
		atpl230Cfg.n1Delay = 0x21;
		atpl230Cfg.p1Delay = 0x20;
		atpl230Cfg.n2Delay = 0x21;
		atpl230Cfg.p2Delay = 0x20;
		atpl230Cfg.emit1Active = 0x77;
		atpl230Cfg.emit2Active = 0x77;
		atpl230Cfg.emit3Active = 0x77;
		atpl230Cfg.emit4Active = 0x77;
		atpl230Cfg.txrxChannel = 4;
		atpl230Cfg.enable_vlow_pk = false;
		break;

	case ATPLCOUP003_v1:
		atpl230Cfg.driver1Mode = EXTERNAL_DRV_MODE;
		atpl230Cfg.driver2Mode = EXTERNAL_DRV_MODE;
		atpl230Cfg.txrx1Polarity = DRV_POL_TX_0_RX_1;
		atpl230Cfg.txrx2Polarity = DRV_POL_TX_0_RX_1;
		atpl230Cfg.high_Z_driver = DRIVER_2;
		atpl230Cfg.low_Z_driver = DRIVER_1;
		atpl230Cfg.vlow_Z_driver = DRIVER_1;
		atpl230Cfg.n1Delay = 0x21;
		atpl230Cfg.p1Delay = 0x20;
		atpl230Cfg.n2Delay = 0x21;
		atpl230Cfg.p2Delay = 0x20;
		atpl230Cfg.emit1Active = 0x77;
		atpl230Cfg.emit2Active = 0x77;
		atpl230Cfg.emit3Active = 0x77;
		atpl230Cfg.emit4Active = 0x77;
		atpl230Cfg.txrxChannel = 1;
		atpl230Cfg.enable_vlow_pk = false;
		break;

	case ATPLCOUP004_v1:
		atpl230Cfg.driver1Mode = EXTERNAL_DRV_MODE;
		atpl230Cfg.driver2Mode = EXTERNAL_DRV_MODE;
		atpl230Cfg.txrx1Polarity = DRV_POL_TX_0_RX_1;
		atpl230Cfg.txrx2Polarity = DRV_POL_TX_0_RX_1;
		atpl230Cfg.high_Z_driver = DRIVER_1;
		atpl230Cfg.low_Z_driver = DRIVER_1;
		atpl230Cfg.vlow_Z_driver = DRIVER_1;
		atpl230Cfg.n1Delay = 0x21;
		atpl230Cfg.p1Delay = 0x20;
		atpl230Cfg.n2Delay = 0x21;
		atpl230Cfg.p2Delay = 0x20;
		atpl230Cfg.emit1Active = 0x77;
		atpl230Cfg.emit2Active = 0x77;
		atpl230Cfg.emit3Active = 0x77;
		atpl230Cfg.emit4Active = 0x77;
		atpl230Cfg.txrxChannel = 1;
		atpl230Cfg.enable_vlow_pk = false;
		break;

	case ATPLCOUP005_v1:
		atpl230Cfg.driver1Mode = EXTERNAL_DRV_MODE;
		atpl230Cfg.driver2Mode = EXTERNAL_DRV_MODE;
		atpl230Cfg.txrx1Polarity = DRV_POL_TX_0_RX_1;
		atpl230Cfg.txrx2Polarity = DRV_POL_TX_0_RX_1;
		atpl230Cfg.high_Z_driver = DRIVER_1;
		atpl230Cfg.low_Z_driver = DRIVER_1;
		atpl230Cfg.vlow_Z_driver = DRIVER_1;
		atpl230Cfg.n1Delay = 0x21;
		atpl230Cfg.p1Delay = 0x20;
		atpl230Cfg.n2Delay = 0x21;
		atpl230Cfg.p2Delay = 0x20;
		atpl230Cfg.emit1Active = 0x77;
		atpl230Cfg.emit2Active = 0x77;
		atpl230Cfg.emit3Active = 0x77;
		atpl230Cfg.emit4Active = 0x77;
		atpl230Cfg.txrxChannel = 2;
		atpl230Cfg.enable_vlow_pk = false;
		break;

	case ATPLCOUP006_v1:
		atpl230Cfg.driver1Mode = EXTERNAL_DRV_MODE;
		atpl230Cfg.driver2Mode = EXTERNAL_DRV_MODE;
		atpl230Cfg.txrx1Polarity = DRV_POL_TX_0_RX_1;
		atpl230Cfg.txrx2Polarity = DRV_POL_TX_0_RX_1;
		atpl230Cfg.high_Z_driver = DRIVER_2;
		atpl230Cfg.low_Z_driver = DRIVER_1;
		atpl230Cfg.vlow_Z_driver = DRIVER_1;
		atpl230Cfg.n1Delay = 0x21;
		atpl230Cfg.p1Delay = 0x20;
		atpl230Cfg.n2Delay = 0x10;
		atpl230Cfg.p2Delay = 0x10;
		atpl230Cfg.emit1Active = 0x77;
		atpl230Cfg.emit2Active = 0x77;
		atpl230Cfg.emit3Active = 0x77;
		atpl230Cfg.emit4Active = 0x77;
		atpl230Cfg.txrxChannel = 3;
		atpl230Cfg.enable_vlow_pk = false;
		break;
	}
}

/**
 * \brief This function initiates PRIME PHY layer attenuation and threshold
 * parameters depending on channel configuration
 *
 */
void phy_init_channel_cfg(void)
{
	atpl230ChnCfg.txAttGlobal = 0xFF;

	switch (atpl230Cfg.atplCoupling) {
	case ATPLCOUP000_v1:
		atpl230ChnCfg.tx1AttChirpHighZ = 0xC0;
		atpl230ChnCfg.tx1AttSignalHighZ = 0xE0;
		atpl230ChnCfg.tx1AttChirpLowZ = 0xA0;
		atpl230ChnCfg.tx1AttSignalLowZ = 0xA0;
		atpl230ChnCfg.tx1AttChirpVLowZ = 0xFF;
		atpl230ChnCfg.tx1AttSignalVLowZ = 0xFF;
		atpl230ChnCfg.tx1LoadThreshold1 = 12000;
		atpl230ChnCfg.tx1LoadThreshold2 = 12000;
		atpl230ChnCfg.tx1LoadThreshold3 = 20500;
		atpl230ChnCfg.tx1LoadThreshold4 = 13000;
		break;

	case ATPLCOUP000_v2:
		atpl230ChnCfg.tx1AttChirpHighZ = 0xC0;
		atpl230ChnCfg.tx1AttSignalHighZ = 0xE0;
		atpl230ChnCfg.tx1AttChirpLowZ = 0xA0;
		atpl230ChnCfg.tx1AttSignalLowZ = 0xA0;
		atpl230ChnCfg.tx1AttChirpVLowZ = 0xFF;
		atpl230ChnCfg.tx1AttSignalVLowZ = 0xFF;
		atpl230ChnCfg.tx1LoadThreshold1 = 12000;
		atpl230ChnCfg.tx1LoadThreshold2 = 12000;
		atpl230ChnCfg.tx1LoadThreshold3 = 20500;
		atpl230ChnCfg.tx1LoadThreshold4 = 13000;
		break;

	case ATPLCOUP001_v1:
		atpl230ChnCfg.tx1AttChirpHighZ = 0x5A;
		atpl230ChnCfg.tx1AttSignalHighZ = 0x78;
		atpl230ChnCfg.tx1AttChirpLowZ = 0xFF;
		atpl230ChnCfg.tx1AttSignalLowZ = 0xFF;
		atpl230ChnCfg.tx1AttChirpVLowZ = 0xFF;
		atpl230ChnCfg.tx1AttSignalVLowZ = 0xFF;
		atpl230ChnCfg.tx1LoadThreshold1 = 8000;
		atpl230ChnCfg.tx1LoadThreshold2 = 0;
		atpl230ChnCfg.tx1LoadThreshold3 = 0;
		atpl230ChnCfg.tx1LoadThreshold4 = 0;
		break;

	case ATPLCOUP002_v1:
		atpl230ChnCfg.tx4AttChirpHighZ = 0x38;
		atpl230ChnCfg.tx4AttSignalHighZ = 0x38;
		atpl230ChnCfg.tx4AttChirpLowZ = 0x60;
		atpl230ChnCfg.tx4AttSignalLowZ = 0xA0;
		atpl230ChnCfg.tx4AttChirpVLowZ = 0xFF;
		atpl230ChnCfg.tx4AttSignalVLowZ = 0xFF;
		atpl230ChnCfg.tx4LoadThreshold1 = 1477;
		atpl230ChnCfg.tx4LoadThreshold2 = 448;
		atpl230ChnCfg.tx4LoadThreshold3 = 7000;
		atpl230ChnCfg.tx4LoadThreshold4 = 3900;

		atpl230ChnCfg.tx5AttChirpHighZ = 0x38;
		atpl230ChnCfg.tx5AttSignalHighZ = 0x38;
		atpl230ChnCfg.tx5AttChirpLowZ = 0x60;
		atpl230ChnCfg.tx5AttSignalLowZ = 0xA0;
		atpl230ChnCfg.tx5AttChirpVLowZ = 0xFF;
		atpl230ChnCfg.tx5AttSignalVLowZ = 0xFF;
		atpl230ChnCfg.tx5LoadThreshold1 = 1417;
		atpl230ChnCfg.tx5LoadThreshold2 = 448;
		atpl230ChnCfg.tx5LoadThreshold3 = 5250;
		atpl230ChnCfg.tx5LoadThreshold4 = 3700;

		atpl230ChnCfg.tx6AttChirpHighZ = 0x38;
		atpl230ChnCfg.tx6AttSignalHighZ = 0x38;
		atpl230ChnCfg.tx6AttChirpLowZ = 0x60;
		atpl230ChnCfg.tx6AttSignalLowZ = 0xA0;
		atpl230ChnCfg.tx6AttChirpVLowZ = 0xFF;
		atpl230ChnCfg.tx6AttSignalVLowZ = 0xFF;
		atpl230ChnCfg.tx6LoadThreshold1 = 1417;
		atpl230ChnCfg.tx6LoadThreshold2 = 448;
		atpl230ChnCfg.tx6LoadThreshold3 = 4900;
		atpl230ChnCfg.tx6LoadThreshold4 = 3700;

		atpl230ChnCfg.tx7AttChirpHighZ = 0x38;
		atpl230ChnCfg.tx7AttSignalHighZ = 0x38;
		atpl230ChnCfg.tx7AttChirpLowZ = 0x60;
		atpl230ChnCfg.tx7AttSignalLowZ = 0xA0;
		atpl230ChnCfg.tx7AttChirpVLowZ = 0xFF;
		atpl230ChnCfg.tx7AttSignalVLowZ = 0xFF;
		atpl230ChnCfg.tx7LoadThreshold1 = 1417;
		atpl230ChnCfg.tx7LoadThreshold2 = 448;
		atpl230ChnCfg.tx7LoadThreshold3 = 5100;
		atpl230ChnCfg.tx7LoadThreshold4 = 3700;
		break;

	case ATPLCOUP002_v2:
		atpl230ChnCfg.tx4AttChirpHighZ = 0x38;
		atpl230ChnCfg.tx4AttSignalHighZ = 0x38;
		atpl230ChnCfg.tx4AttChirpLowZ = 0x60;
		atpl230ChnCfg.tx4AttSignalLowZ = 0xA0;
		atpl230ChnCfg.tx4AttChirpVLowZ = 0xFF;
		atpl230ChnCfg.tx4AttSignalVLowZ = 0xFF;
		atpl230ChnCfg.tx4LoadThreshold1 = 1477;
		atpl230ChnCfg.tx4LoadThreshold2 = 448;
		atpl230ChnCfg.tx4LoadThreshold3 = 7000;
		atpl230ChnCfg.tx4LoadThreshold4 = 3900;

		atpl230ChnCfg.tx5AttChirpHighZ = 0x38;
		atpl230ChnCfg.tx5AttSignalHighZ = 0x38;
		atpl230ChnCfg.tx5AttChirpLowZ = 0x60;
		atpl230ChnCfg.tx5AttSignalLowZ = 0xA0;
		atpl230ChnCfg.tx5AttChirpVLowZ = 0xFF;
		atpl230ChnCfg.tx5AttSignalVLowZ = 0x00;
		atpl230ChnCfg.tx5LoadThreshold1 = 1417;
		atpl230ChnCfg.tx5LoadThreshold2 = 448;
		atpl230ChnCfg.tx5LoadThreshold3 = 5250;
		atpl230ChnCfg.tx5LoadThreshold4 = 3700;

		atpl230ChnCfg.tx6AttChirpHighZ = 0x38;
		atpl230ChnCfg.tx6AttSignalHighZ = 0x38;
		atpl230ChnCfg.tx6AttChirpLowZ = 0x60;
		atpl230ChnCfg.tx6AttSignalLowZ = 0xA0;
		atpl230ChnCfg.tx6AttChirpVLowZ = 0xFF;
		atpl230ChnCfg.tx6AttSignalVLowZ = 0x00;
		atpl230ChnCfg.tx6LoadThreshold1 = 1417;
		atpl230ChnCfg.tx6LoadThreshold2 = 448;
		atpl230ChnCfg.tx6LoadThreshold3 = 4900;
		atpl230ChnCfg.tx6LoadThreshold4 = 3700;

		atpl230ChnCfg.tx7AttChirpHighZ = 0x38;
		atpl230ChnCfg.tx7AttSignalHighZ = 0x38;
		atpl230ChnCfg.tx7AttChirpLowZ = 0x60;
		atpl230ChnCfg.tx7AttSignalLowZ = 0xA0;
		atpl230ChnCfg.tx7AttChirpVLowZ = 0x90;
		atpl230ChnCfg.tx7AttSignalVLowZ = 0x90;
		atpl230ChnCfg.tx7LoadThreshold1 = 1300;
		atpl230ChnCfg.tx7LoadThreshold2 = 448;
		atpl230ChnCfg.tx7LoadThreshold3 = 5100;
		atpl230ChnCfg.tx7LoadThreshold4 = 3700;
		break;

	case ATPLCOUP003_v1:
		atpl230ChnCfg.tx1AttChirpHighZ = 0xC0;
		atpl230ChnCfg.tx1AttSignalHighZ = 0xE0;
		atpl230ChnCfg.tx1AttChirpLowZ = 0xFF;
		atpl230ChnCfg.tx1AttSignalLowZ = 0xFF;
		atpl230ChnCfg.tx1AttChirpVLowZ = 0xFF;
		atpl230ChnCfg.tx1AttSignalVLowZ = 0xFF;
		atpl230ChnCfg.tx1LoadThreshold1 = 12000;
		atpl230ChnCfg.tx1LoadThreshold2 = 0;
		atpl230ChnCfg.tx1LoadThreshold3 = 0;
		atpl230ChnCfg.tx1LoadThreshold4 = 0;
		break;

	case ATPLCOUP004_v1:
		atpl230ChnCfg.tx1AttChirpHighZ = 0x5A;
		atpl230ChnCfg.tx1AttSignalHighZ = 0x78;
		atpl230ChnCfg.tx1AttChirpLowZ = 0xFF;
		atpl230ChnCfg.tx1AttSignalLowZ = 0xFF;
		atpl230ChnCfg.tx1AttChirpVLowZ = 0xFF;
		atpl230ChnCfg.tx1AttSignalVLowZ = 0xFF;
		atpl230ChnCfg.tx1LoadThreshold1 = 8000;
		atpl230ChnCfg.tx1LoadThreshold2 = 0;
		atpl230ChnCfg.tx1LoadThreshold3 = 0;
		atpl230ChnCfg.tx1LoadThreshold4 = 0;
		break;

	case ATPLCOUP005_v1:
		atpl230ChnCfg.tx2AttChirpHighZ = 0xFF;
		atpl230ChnCfg.tx2AttSignalHighZ = 0xFF;
		atpl230ChnCfg.tx2AttChirpLowZ = 0xFF;
		atpl230ChnCfg.tx2AttSignalLowZ = 0xFF;
		atpl230ChnCfg.tx2AttChirpVLowZ = 0xFF;
		atpl230ChnCfg.tx2AttSignalVLowZ = 0xFF;
		atpl230ChnCfg.tx2LoadThreshold1 = 600;
		atpl230ChnCfg.tx2LoadThreshold2 = 270;
		atpl230ChnCfg.tx2LoadThreshold3 = 1475;
		atpl230ChnCfg.tx2LoadThreshold4 = 1100;
		break;

	case ATPLCOUP006_v1:
		atpl230ChnCfg.tx3AttChirpHighZ = 0x70;
		atpl230ChnCfg.tx3AttSignalHighZ = 0x70;
		atpl230ChnCfg.tx3AttChirpLowZ = 0xFF;
		atpl230ChnCfg.tx3AttSignalLowZ = 0xFF;
		atpl230ChnCfg.tx3AttChirpVLowZ = 0xFF;
		atpl230ChnCfg.tx3AttSignalVLowZ = 0xFF;
		atpl230ChnCfg.tx3LoadThreshold1 = 600;
		atpl230ChnCfg.tx3LoadThreshold2 = 270;
		atpl230ChnCfg.tx3LoadThreshold3 = 1475;
		atpl230ChnCfg.tx3LoadThreshold4 = 1100;

		atpl230ChnCfg.tx4AttChirpHighZ = 0x70;
		atpl230ChnCfg.tx4AttSignalHighZ = 0x70;
		atpl230ChnCfg.tx4AttChirpLowZ = 0xFF;
		atpl230ChnCfg.tx4AttSignalLowZ = 0xFF;
		atpl230ChnCfg.tx4AttChirpVLowZ = 0xFF;
		atpl230ChnCfg.tx4AttSignalVLowZ = 0xFF;
		atpl230ChnCfg.tx4LoadThreshold1 = 1477;
		atpl230ChnCfg.tx4LoadThreshold2 = 448;
		atpl230ChnCfg.tx4LoadThreshold3 = 7000;
		atpl230ChnCfg.tx4LoadThreshold4 = 3900;

		atpl230ChnCfg.tx5AttChirpHighZ = 0x70;
		atpl230ChnCfg.tx5AttSignalHighZ = 0x70;
		atpl230ChnCfg.tx5AttChirpLowZ = 0xFF;
		atpl230ChnCfg.tx5AttSignalLowZ = 0xFF;
		atpl230ChnCfg.tx5AttChirpVLowZ = 0xFF;
		atpl230ChnCfg.tx5AttSignalVLowZ = 0x00;
		atpl230ChnCfg.tx5LoadThreshold1 = 1417;
		atpl230ChnCfg.tx5LoadThreshold2 = 448;
		atpl230ChnCfg.tx5LoadThreshold3 = 5250;
		atpl230ChnCfg.tx5LoadThreshold4 = 3700;

		atpl230ChnCfg.tx6AttChirpHighZ = 0x70;
		atpl230ChnCfg.tx6AttSignalHighZ = 0x70;
		atpl230ChnCfg.tx6AttChirpLowZ = 0xFF;
		atpl230ChnCfg.tx6AttSignalLowZ = 0xFF;
		atpl230ChnCfg.tx6AttChirpVLowZ = 0xFF;
		atpl230ChnCfg.tx6AttSignalVLowZ = 0xFF;
		atpl230ChnCfg.tx6LoadThreshold1 = 1417;
		atpl230ChnCfg.tx6LoadThreshold2 = 448;
		atpl230ChnCfg.tx6LoadThreshold3 = 4900;
		atpl230ChnCfg.tx6LoadThreshold4 = 3700;

		atpl230ChnCfg.tx7AttChirpHighZ = 0x70;
		atpl230ChnCfg.tx7AttSignalHighZ = 0x70;
		atpl230ChnCfg.tx7AttChirpLowZ = 0xFF;
		atpl230ChnCfg.tx7AttSignalLowZ = 0xFF;
		atpl230ChnCfg.tx7AttChirpVLowZ = 0xFF;
		atpl230ChnCfg.tx7AttSignalVLowZ = 0xFF;
		atpl230ChnCfg.tx7LoadThreshold1 = 1300;
		atpl230ChnCfg.tx7LoadThreshold2 = 448;
		atpl230ChnCfg.tx7LoadThreshold3 = 5100;
		atpl230ChnCfg.tx7LoadThreshold4 = 3700;

		atpl230ChnCfg.tx8AttChirpHighZ = 0x70;
		atpl230ChnCfg.tx8AttSignalHighZ = 0x70;
		atpl230ChnCfg.tx8AttChirpLowZ = 0xFF;
		atpl230ChnCfg.tx8AttSignalLowZ = 0xFF;
		atpl230ChnCfg.tx8AttChirpVLowZ = 0xFF;
		atpl230ChnCfg.tx8AttSignalVLowZ = 0xFF;
		atpl230ChnCfg.tx8LoadThreshold1 = 600;
		atpl230ChnCfg.tx8LoadThreshold2 = 270;
		atpl230ChnCfg.tx8LoadThreshold3 = 1475;
		atpl230ChnCfg.tx8LoadThreshold4 = 1100;
		break;
	}
}
