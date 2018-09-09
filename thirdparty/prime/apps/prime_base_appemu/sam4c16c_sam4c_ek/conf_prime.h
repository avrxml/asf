/**
 * \file
 *
 * \brief PRIME configuration file.
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

#ifndef CONF_PRIME_H_INCLUDED
#define CONF_PRIME_H_INCLUDED

#include "compiler.h"
#include "conf_usi.h"

/* PRIME MODE */
#define PRIME_BASE

/* Firmware Information	*/
#define PRIME_FW_VENDOR                         "ATMEL"
#define PRIME_FW_MODEL                          "SAM4CEK/000"
#define PRIME_FW_VERSION                        "01.03.08.01"

/* Prime PIB firmware information */
#define PRIME_PIB_VENDOR                        0x0000
#define PRIME_PIB_MODEL                         0x3430
#define PRIME_PIB_VERSION_LEN       8
#define PRIME_PIB_VERSION                       "01030801"

/* Not Minimum SNR value */
/* #define NOT_USE_MINIMUM_SNR_VALID */

/* MAC Capabilities */
/* #define NODE_CAPABLE_CFP */
#define NODE_CAPABLE_PRM
#define NODE_CAPABLE_ARQ
#define NODE_CAPABLE_DIR_SW

/* Firmware Upgrade Protocol */
#define CFG_BASEMNG             BASEMNG_DISABLE

#ifdef USE_MNGP_PRIME_PORT
  #define MNGLAYER_USE_SERIAL_PROFILE
#endif

#ifdef USE_PROTOCOL_MAC_PRIME_PORT
  #define MAC_USI_SERIALIZATION
#endif

#ifdef USE_PROTOCOL_MLME_PRIME_PORT
  #define MLME_USI_SERIALIZATION
#endif

#ifdef USE_PROTOCOL_PLME_PRIME_PORT
  #define PLME_USI_SERIALIZATION
#endif

#ifdef USE_PROTOCOL_432_PRIME_PORT
  #define SSCS432_USI_SERIALIZATION
#endif

#ifdef USE_PROTOCOL_SNIF_PRIME_PORT
  #define SNIFFER_USI_SERIALIZATION
#endif

#endif /* CONF_PRIME_H_INCLUDED */
