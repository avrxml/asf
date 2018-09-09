/**
 * \file
 *
 * \brief  Configuration File for SAM4L-EK Board.
 *
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
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

#ifndef CONF_BOARD_H_INCLUDED
#define CONF_BOARD_H_INCLUDED

/** Enable Com Port. */
#define CONF_BOARD_COM_PORT
#define CONF_BOARD_AT86RFX

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
