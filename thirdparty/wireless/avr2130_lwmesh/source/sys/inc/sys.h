/**
 * \file sys.c
 *
 * \brief Main system routines interface
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

#ifndef _SYS_H_
#define _SYS_H_

/*- Includes ---------------------------------------------------------------*/
#include "sysConfig.h"
#include "phy.h"
#include "nwk.h"

/**
 * \defgroup group_lwmesh Light Weight Mesh  v2.0
 *      Lightweight Mesh software stack is an easy to use proprietary low power
 * wireless mesh network protocol.
 *
 */

/**
 * \ingroup group_lwmesh
 * \defgroup group_phy PHY
 * Radio physical layer (PHY) provides functions for radio transceiver access.
 * Some of them are accessible only by the network layer (request to send data,
 * data indication);
 * some of them can be used from the application (channel selection, random
 * number generation, energy detection, etc.)
 *
 *
 */

/**
 * \ingroup group_lwmesh
 * \defgroup group_lwmesh_nwk Network layer(NWK)
 *  Network layer (NWK) provides core stack functionality and provides two
 * routing algorithms,1.Native routing and 2.AODV routing.
 *
 */

/**
 * \ingroup group_lwmesh
 * \defgroup group_lwmesh_sys LWMesh System Services
 * System services provide common functions for all layers, which are necessary
 * for normal stack operation.
 * System services include basic types and definitions, software timers, default
 * configuration parameters, encryption module access, etc.
 * @{
 */

/*- Prototypes -------------------------------------------------------------*/

void SYS_Init(void);
void SYS_TaskHandler(void);

/** @} */
#endif /* _SYS_H_ */
