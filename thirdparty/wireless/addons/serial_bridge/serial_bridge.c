/**
 * \file main.c
 *
 * \brief  Serial Bridge Application
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
 */

/**
 * \page license License
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/**
 * \mainpage Serial Bridge Application
 */

/* === INCLUDES ============================================================ */

#include <stdlib.h>
#include "serial_bridge.h"
#include "sio2host.h"
#include "sio2ncp.h"
#include "asf.h"

/* === GLOBALS ============================================================== */

uint8_t length_received_host, length_received_ncp;
uint8_t temp[SIO_RX_BUF_SIZE];

/* === IMPLEMENTATION ====================================================== */

void serial_bridge_init()
{
	sio2host_init();

	sio2ncp_init();
}

void serial_bridge_handler()
{
	length_received_host = sio2host_rx(temp, SIO_RX_BUF_SIZE);
	if (length_received_host != 0) {
		sio2ncp_tx(temp, length_received_host);
		length_received_host = 0;
	}

	length_received_ncp = sio2ncp_rx(temp, SIO_RX_BUF_SIZE);
	if (length_received_ncp != 0) {
		sio2host_tx(temp, length_received_ncp);
		length_received_ncp = 0;
	}
}

/* EOF */
