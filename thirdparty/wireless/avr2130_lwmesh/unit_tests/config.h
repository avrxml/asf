/**
 * \file config.h
 *
 * \brief WSNDemo application and stack configuration
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
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
 *
 */

/*
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */
#ifndef _CONFIG_H_
#define _CONFIG_H_

/*****************************************************************************
*****************************************************************************/
#define APP_ADDR                0x2002
#define APP_PANID               0x1234
#define APP_SENDING_INTERVAL    2000
#define APP_ENDPOINT            1
#define APP_SECURITY_KEY        "TestSecurityKey0"

#if (defined (PHY_AT86RF212B) || defined (PHY_AT86RF212))
  #define APP_CHANNEL           0x01
  #define APP_BAND              0x00
  #define APP_MODULATION        0x24
#else
  #define APP_CHANNEL           0x0f
#endif

#define PHY_ENABLE_RANDOM_NUMBER_GENERATOR

#define SYS_SECURITY_MODE                   0

#define NWK_BUFFERS_AMOUNT                  10
#define NWK_DUPLICATE_REJECTION_TABLE_SIZE  50
#define NWK_DUPLICATE_REJECTION_TTL         2000 /* ms */
#define NWK_ROUTE_TABLE_SIZE                100
#define NWK_ROUTE_DEFAULT_SCORE             3
#define NWK_ACK_WAIT_TIME                   1000 /* ms */
#define NWK_GROUPS_AMOUNT                   3
#define NWK_ROUTE_DISCOVERY_TABLE_SIZE      5
#define NWK_ROUTE_DISCOVERY_TIMEOUT         1000 /* ms */
#define APP_RX_BUF_SIZE                     20
#define NWK_ENABLE_ROUTING
#define NWK_ENABLE_SECURITY
#define NWK_ENABLE_ROUTE_DISCOVERY

#if APP_ADDR == 0
#define APP_CAPTION     "Coordinator"
#define APP_NODE_TYPE   0
#define APP_COORDINATOR 1
#define APP_ROUTER      0
#define APP_ENDDEVICE   0
#elif APP_ADDR < 0x8000
#define APP_CAPTION     "Router"
#define APP_NODE_TYPE   1
#define APP_COORDINATOR 0
#define APP_ROUTER      1
#define APP_ENDDEVICE   0
#else
#define APP_CAPTION     "End Device"
#define APP_NODE_TYPE   2
#define APP_COORDINATOR 0
#define APP_ROUTER      0
#define APP_ENDDEVICE   1
#endif

#endif /* _CONFIG_H_ */
