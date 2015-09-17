/**
 * \file
 *
 * \brief  Configuration File for SAM4L-EK Board.
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
 *
 */

#ifndef CONF_BOARD_H_INCLUDED
#define CONF_BOARD_H_INCLUDED

/** Enable Com Port. */
#define CONF_BOARD_COM_PORT
#define CONF_BOARD_AT86RFX

/*Board Has an External PA Connected to the Radio ,Following are the constatnts
 * related to the Ext PA */

#ifdef EXT_RF_FRONT_END_CTRL /*For External PA for 233FEM*/

#define EXT_PA_SE2431L

/*
 * Value of an external LNA gain.
 * If no external LNA is available, the value is 0.
 */
#define EXT_LNA_HIGH_GAIN    (14)

#endif

#ifdef CUSTOM_DEFAULT_TX_PWR /*For External PA for 233FEM*/

/*
 * Default value of transmit power of transceiver: Preset
 *    - definition acct. IEEE802.15.4 PHY PIB attribute phyTransmitPower
 *    - TX Pout init value based on validation
 */
#define TAL_TRANSMIT_POWER_DEFAULT      (TX_PWR_TOLERANCE | 0x14)
#endif

#endif  /* CONF_BOARD_H_INCLUDED */
