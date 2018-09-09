/**
 * \file main.c
 *
 * \brief  Serial Bridge Application
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
 */

/**
 * \page license License
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
