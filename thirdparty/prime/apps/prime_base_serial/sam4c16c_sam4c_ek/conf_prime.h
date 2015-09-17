/**
 * \file
 *
 * \brief PRIME configuration file.
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
