/**
 * \file config.h
 *
 * \brief WSNDemo application and stack configuration
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
 *
 */

/*
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */
#ifndef _CONFIG_H_
#define _CONFIG_H_

/*****************************************************************************
*****************************************************************************/
#define APP_ADDR                0x0000
#define APP_PANID               0x1239
#define APP_SENDING_INTERVAL    10000
#define APP_ENDPOINT            1
#define APP_SECURITY_KEY        "TestSecurityKey0"

#if (defined (PHY_AT86RF212B) || defined (PHY_AT86RF212))
  #define APP_CHANNEL           0x01
  #define APP_BAND              0x00
  #define APP_MODULATION        0x24
#else
  #define APP_CHANNEL           0x0f
#endif

/* #define PHY_ENABLE_RANDOM_NUMBER_GENERATOR */

#define SYS_SECURITY_MODE                   1

#define NWK_BUFFERS_AMOUNT                  10
#define NWK_DUPLICATE_REJECTION_TABLE_SIZE  50
#define NWK_DUPLICATE_REJECTION_TTL         2000 /* ms */
#define NWK_ROUTE_TABLE_SIZE                100
#define NWK_ROUTE_DEFAULT_SCORE             3
#define NWK_ACK_WAIT_TIME                   1000 /* ms */
#define NWK_GROUPS_AMOUNT                   3
#define NWK_ROUTE_DISCOVERY_TABLE_SIZE      5
#define NWK_ROUTE_DISCOVERY_TIMEOUT         1000 /* ms */
#define APP_RX_BUF_SIZE                     5
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
