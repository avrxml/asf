/**
 *
 * \file
 *
 * \brief WiFi Settings.
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
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

#ifndef CONF_WILC_H_INCLUDED
#define CONF_WILC_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"
#include "osprintf.h"
#include <FreeRTOSConfig.h>

/*
   ---------------------------------
   --------- Chip settings ---------
   ---------------------------------
*/

/** Select WILC firmware to use. */
#define CONF_WILC_USE_1000_REV_B

/*
   ---------------------------------
   ---------- PIN settings ---------
   ---------------------------------
*/

#define CONF_WILC_PIN_RESET				IOPORT_CREATE_PIN(PIOB, 3)
#define CONF_WILC_PIN_CHIP_ENABLE		IOPORT_CREATE_PIN(PIOA, 20)
#define CONF_WILC_PIN_WAKE				IOPORT_CREATE_PIN(PIOB, 9)

/*
   ---------------------------------
   ---------- SPI settings ---------
   ---------------------------------
*/

#define CONF_WILC_USE_SPI				(1)

/** SPI pin and instance settings. */
#define CONF_WILC_SPI					SPI
#define CONF_WILC_SPI_ID				ID_SPI
#define CONF_WILC_SPI_MISO_GPIO			SPI_MISO_GPIO
#define CONF_WILC_SPI_MISO_FLAGS		SPI_MISO_FLAGS
#define CONF_WILC_SPI_MOSI_GPIO			SPI_MOSI_GPIO
#define CONF_WILC_SPI_MOSI_FLAGS		SPI_MOSI_FLAGS
#define CONF_WILC_SPI_CLK_GPIO			SPI_SPCK_GPIO
#define CONF_WILC_SPI_CLK_FLAGS			SPI_SPCK_FLAGS
#define CONF_WILC_SPI_CS_GPIO			SPI_NPCS0_GPIO
#define CONF_WILC_SPI_CS_FLAGS			PIO_OUTPUT_1
#define CONF_WILC_SPI_NPCS				(0)

/** SPI delay before SPCK and between consecutive transfer. */
#define CONF_WILC_SPI_DLYBS				(0)
#define CONF_WILC_SPI_DLYBCT 			(0)

/** SPI interrupt pin. */
#define CONF_WILC_SPI_INT_PIN			IOPORT_CREATE_PIN(PIOA, 24)
#define CONF_WILC_SPI_INT_PIO			PIOA
#define CONF_WILC_SPI_INT_PIO_ID		ID_PIOA
#define CONF_WILC_SPI_INT_MASK			(1 << 24)
/** Highest INT priority that can call FreeRTOS FromISR() functions. */
#define CONF_WILC_SPI_INT_PRIORITY		(configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY)

/** Clock polarity & phase. */
#define CONF_WILC_SPI_POL				(0)
#define CONF_WILC_SPI_PHA				(1)

/** SPI clock. */
#define CONF_WILC_SPI_CLOCK				(48000000)

/*
   ---------------------------------
   ----------- OS options ----------
   ---------------------------------
*/

#define CONF_DRIVER_LOCK				1
#if CONF_DRIVER_LOCK
#  include <os/include/os_hook.h>
#  define DRIVER_SIGNAL_ISR				os_hook_isr
#else
#  define DRIVER_SIGNAL_ISR()
#endif

/*
   ---------------------------------
   --------- Debug options ---------
   ---------------------------------
*/

#define CONF_WILC_DEBUG					(0)
#define CONF_WILC_PRINTF				osprintf

#ifdef __cplusplus
}
#endif

#endif /* CONF_WILC_H_INCLUDED */
