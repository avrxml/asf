/**
 * \file sys.c
 *
 * \brief Main system routines interface
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
