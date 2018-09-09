/**
 * \file sysConfig.h
 *
 * \brief Main system configyration file
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

#ifndef _SYS_CONFIG_H_
#define _SYS_CONFIG_H_

/*- Includes ---------------------------------------------------------------*/
#include "config.h"

/*- Definitions ------------------------------------------------------------*/
#ifndef NWK_BUFFERS_AMOUNT
#define NWK_BUFFERS_AMOUNT                       5
#endif

#ifndef NWK_DUPLICATE_REJECTION_TABLE_SIZE
#define NWK_DUPLICATE_REJECTION_TABLE_SIZE       10
#endif

#ifndef NWK_DUPLICATE_REJECTION_TTL
#define NWK_DUPLICATE_REJECTION_TTL              1000 /* ms */
#endif

#ifndef NWK_ROUTE_TABLE_SIZE
#define NWK_ROUTE_TABLE_SIZE                     10
#endif

#ifndef NWK_ROUTE_DEFAULT_SCORE
#define NWK_ROUTE_DEFAULT_SCORE                  3
#endif

#ifndef NWK_ACK_WAIT_TIME
#define NWK_ACK_WAIT_TIME                        1000 /* ms */
#endif

#ifndef NWK_GROUPS_AMOUNT
#define NWK_GROUPS_AMOUNT                        10
#endif

#ifndef NWK_ROUTE_DISCOVERY_TABLE_SIZE
#define NWK_ROUTE_DISCOVERY_TABLE_SIZE           5
#endif

#ifndef NWK_ROUTE_DISCOVERY_TIMEOUT
#define NWK_ROUTE_DISCOVERY_TIMEOUT              1000 /* ms */
#endif

/* #define NWK_ENABLE_ROUTING */
/* #define NWK_ENABLE_SECURITY */
/* #define NWK_ENABLE_MULTICAST */
/* #define NWK_ENABLE_ROUTE_DISCOVERY */
/* #define NWK_ENABLE_SECURE_COMMANDS */

#ifndef SYS_SECURITY_MODE
#define SYS_SECURITY_MODE                        1
#endif

/*- Sanity checks ----------------------------------------------------------*/

#endif /* _SYS_CONFIG_H_ */
